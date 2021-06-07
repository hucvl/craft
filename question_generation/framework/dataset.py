import copy
import json
import os
import time
import traceback
from collections import defaultdict
from pathlib import Path
from typing import List, Dict

import numpy as np
import pandas as pd
import plotly.express as px
import plotly.graph_objects as go
from colour import Color
from imblearn.under_sampling import RandomUnderSampler
from loguru import logger

from framework.simulation import SimulationRunner, SimulationInstance, Perturbator
from framework.utils import FileIO, Funnel, MultithreadedProcessor


class CRAFTDataset:

    def __init__(self, dataset_folder_path: str, metadata: dict, load_immediately=True):
        # Only load minimal version to help memory usage for scripts
        self.minimal_dataset_file_path = f"{dataset_folder_path}/dataset_minimal.json" \
            if os.path.isdir(dataset_folder_path) \
            else dataset_folder_path

        self.dataset_folder_path = dataset_folder_path \
            if os.path.isdir(dataset_folder_path) \
            else Path(dataset_folder_path).parent.as_posix()

        self.metadata = metadata if isinstance(metadata, dict) else FileIO.read_json(metadata)

        self.dataset_minimal_json = None
        if load_immediately:
            self.load()

        self.questions = self.dataset_minimal_json
        self.questions_dataframe = None
        self.max_video_index = None
        self.video_index_to_questions_map = None
        self.sid_vi_q_map = None
        self.vi_sid_map = None
        self.question_type_question_map = None
        if load_immediately:
            self.prepare_auxiliaries()

    def load(self):
        self.dataset_minimal_json = FileIO.read_json(self.minimal_dataset_file_path)

    def prepare_auxiliaries(self):
        # self.questions = self.get_all_questions_as_list()
        self.questions_dataframe = pd.DataFrame(self.questions)
        self.max_video_index = None
        self.video_index_to_questions_map = defaultdict(list)
        self.vi_sid_map = {}
        for question in self.questions:
            self.video_index_to_questions_map[question["video_index"]].append(question)
            self.vi_sid_map[question["video_index"]] = int(question["simulation_id"])


    def build_sid_vi_q_map(self):
        self.sid_vi_q_map = {}
        for question in self.questions:
            sid = int(question["simulation_id"])
            vi = question["video_index"]
            if sid not in self.sid_vi_q_map:
                self.sid_vi_q_map[sid] = {}
            if vi not in self.sid_vi_q_map[sid]:
                self.sid_vi_q_map[sid][vi] = []
            self.sid_vi_q_map[sid][vi].append(question)

    def build_question_type_question_map(self):
        self.question_type_question_map = {}
        for question in self.questions:
            if "question_type" not in question:
                question["question_type"] = question["template_id"].split("_")[0].capitalize()
            qtype = question["question_type"]
            if qtype not in self.question_type_question_map:
                self.question_type_question_map[qtype] = []
            self.question_type_question_map[qtype].append(question)
        return self.question_type_question_map

    def get_questions_for_video(self, video_index: int) -> List[Dict]:
        return self.video_index_to_questions_map[video_index]

    def get_last_video_index(self):
        if self.max_video_index is None:
            max_index = 0
            for question in self.questions:
                if max_index < question["video_index"]:
                    max_index = question["video_index"]
            self.max_video_index = max_index
        return self.max_video_index

    def get_questions_output_path(self, sid: int, instance_id: int):
        return f"{self.dataset_folder_path}/intermediates/sid_{sid}/questions/qa_{instance_id:06d}.json"

    def get_original_questions_path(self, sid: int, instance_id: int):
        return f"{self.dataset_folder_path}/intermediates/sid_{sid}/questions/qa_orig_{instance_id:06d}.json"

    def get_simulation_with_variations_output_path(self, sid: int, instance_id: int):
        return f"{self.dataset_folder_path}/intermediates/sid_{sid}/{instance_id:06d}.json"

    def get_video_output_path(self, sid: int, instance_id: int):
        return f"{self.dataset_folder_path}/videos/sid_{sid}/{instance_id:06d}.mpg"

    def get_bare_simulation_output_path(self, sid: int, instance_id: int):
        return f"{self.dataset_folder_path}/intermediates/sid_{sid}/simulations/{instance_id:06d}.json"

    def get_answer_type_for_answer(self, answer: str) -> str:
        return ("Boolean" if answer in ["False", "True"]
                else "Shape" if answer in self.metadata["types"]["Shape"]
                else "Color" if answer in self.metadata["types"]["Color"]
                else "Size" if answer in self.metadata["types"]["Size"]
                else "Count")

    def get_unique_values(self, column: str) -> set:
        return set(self.questions_dataframe[column].to_list())

    def get_question_from_question_obj(self, question_obj, simulation_id):
        template_filename = question_obj["template_filename"]
        answer = str(question_obj["answer"])
        question = question_obj["question"]
        video_file_path = question_obj["video_filename"]
        video_index = question_obj["video_index"]
        question_index = question_obj["question_index"]
        question_family_index = question_obj["question_family_index"]
        split = question_obj["split"] if "split" in question_obj else None

        return {"question": question,
                "answer": answer,
                "answer_type": self.get_answer_type_for_answer(answer),
                "template_filename": template_filename,
                "video_file_path": video_file_path,
                "video_index": video_index,
                "question_index": question_index,
                "question_family_index": question_family_index,
                "template_id": f"{os.path.splitext(template_filename)[0]}_{question_family_index}",
                "simulation_id": simulation_id,
                "split": split}

    def get_question_list_from_qa_json(self, qa_json):
        questions = []
        question_list = qa_json["questions"]
        simulation_id = qa_json["simulation_id"]
        for question_obj in question_list:
            questions.append(self.get_question_from_question_obj(question_obj, simulation_id))

        return questions

    @staticmethod
    def convert_to_original_dataset_json(dataset_json, questions: list) -> dict:
        video_index_question_indices = defaultdict(list)
        for question in questions:
            video_index_question_indices[question["video_index"]].append(question["question_index"])

        for video_index, question_indices in video_index_question_indices.items():
            DatasetUtils.retain_questions(dataset_json, video_index, question_indices)

        return dataset_json

    def generate_statistics(self, output_folder):
        stats = DatasetStatistics(self)
        logger.info(f"Generating all statistics of the dataset")
        stats.generate_all_stats()

        os.makedirs(output_folder, exist_ok=True)

        exporter = DatasetStatisticsExporter(stats, export_png=True, output_folder=output_folder)

        logger.info(f"Generating charts: Answer frequencies per template ID")
        exporter.generate_chart__answer_per_template()
        logger.info(f"Generating charts: Template ID frequencies per simulation ID")
        exporter.generate_chart__template_per_sim_id()
        logger.info(f"Generating charts: Answer frequencies in the dataset")
        exporter.generate_chart__answer_frequencies()
        logger.info(f"Generating charts: Answer frequencies per simulation ID")
        exporter.generate_chart__answer_frequencies_per_sim_id()
        logger.info(f"Generating charts: Answer frequencies per TID and SID")
        exporter.generate_chart__answer_per_template_and_simulation()
        logger.info(f"Generating charts: Sunburst charts for each SID")
        exporter.generate_sunburst_charts_for_each_scene_type()
        logger.info(f"Generating charts: Sunburst chart for the dataset")
        exporter.generate_sunburst_chart_question_type_to_answer_type_to_answer_for_whole_dataset()
        logger.info(f"Statistics folder: {output_folder}")


class DatasetUtils:
    @staticmethod
    def convert_to_list(array):
        dataset = []
        for i, elem in enumerate(array):
            obj = array[i][0]
            dataset.append(obj)
        return dataset

    @staticmethod
    def convert_to_ndarray(dataset: list):
        m = np.array([obj for obj in dataset])
        return m

    @staticmethod
    def imblearn_random_undersampling(dataset: list, class_name, discard_strategy_fn=None, purge_single_answers=False):
        freq = defaultdict(int)
        for q in dataset:
            freq[q[class_name]] += 1

        if purge_single_answers and len(freq.keys()) <= 1:
            logger.info("Cannot balance set. Purging...")
            return []

        if len(freq.keys()) <= 1:
            return dataset

        outliers = set()
        if discard_strategy_fn is not None:
            for val in freq.keys():
                if discard_strategy_fn(class_name, val):
                    outliers.add(val)

        to_be_resampled = []
        passed = []
        for q in dataset:
            if q[class_name] not in outliers:
                to_be_resampled.append(q)
            else:
                passed.append(q)

        data = DatasetUtils.convert_to_ndarray(to_be_resampled)

        labels = np.array([])
        for item in data:
            labels = np.append(labels, f"{item[class_name]}")

        if len(set(labels)) <= 1:
            return dataset

        reshaped = data.reshape((-1, 1))

        rus = RandomUnderSampler()
        X_rus, y_rus = rus.fit_resample(reshaped, labels)

        undersampled_dataset = DatasetUtils.convert_to_list(X_rus)

        undersampled_dataset.extend(passed)

        return undersampled_dataset

    @staticmethod
    def retain_questions(dataset_json, video_index, question_indices: list):
        dataset_json[video_index]["questions"]["questions"] = [
            question for question in dataset_json[video_index]["questions"]["questions"]
            if question["question_index"] in question_indices
        ]

    @staticmethod
    def relativize_paths(dataset_json, dataset_folder_path) -> dict:
        return json.loads(json.dumps(dataset_json).replace(dataset_folder_path, "."))

    @staticmethod
    def minimized_dataset(dataset_json) -> dict:
        video_to_qa = {}
        for qa_json in dataset_json:
            video_to_qa[Path(qa_json["questions"]["info"]["video_filename"]).name] = \
                [
                    {
                        "question": question_obj["question"],
                        "answer": question_obj["answer"],
                        "template_filename": question_obj["template_filename"]
                    }
                    for question_obj in qa_json["questions"]["questions"]
                ]
        return video_to_qa

    @staticmethod
    def dataset_as_list(dataset_json, metadata) -> list:
        questions = []
        for qa_json in dataset_json:
            questions.extend(DatasetUtils.convert_to_minimal_version(qa_json, metadata))

        return questions

    @staticmethod
    def convert_to_minimal_version(qa_json, metadata):
        questions = []
        question_list = qa_json["questions"]["questions"]
        simulation_id = qa_json["simulation_id"]
        for question_obj in question_list:
            template_filename = question_obj["template_filename"]
            answer = str(question_obj["answer"])
            question = question_obj["question"]
            video_file_path = question_obj["video_filename"]
            video_index = question_obj["video_index"]
            question_index = question_obj["question_index"]
            question_family_index = question_obj["question_family_index"]

            answer_type = ("Boolean" if answer in ["False", "True"]
                           else "Shape" if answer in metadata["types"]["Shape"]
            else "Color" if answer in metadata["types"]["Color"]
            else "Size" if answer in metadata["types"]["Size"]
            else "Count")

            questions.append({"question": question,
                              "answer": answer,
                              "answer_type": answer_type,
                              "template_filename": template_filename,
                              "video_file_path": video_file_path,
                              "video_index": video_index,
                              "question_index": question_index,
                              "question_family_index": question_family_index,
                              "template_id": f"{os.path.splitext(template_filename)[0]}_{question_family_index}",
                              "simulation_id": simulation_id})
        return questions


class DatasetStatistics:

    def __init__(self, dataset: CRAFTDataset):
        self.dataset = dataset
        self.answer_freq_per_tid_and_sid = None
        self.answer_freq_per_sid = None
        self.answer_freq_per_tid = None
        self.answer_freq_total = None
        self.template_id_freq_per_sid = None
        self.map_of_sid_tid_pairs_to_answer_freqs = {}
        self.map_of_sid_to_answer_freqs = {}
        self.map_of_tid_to_answer_freqs = {}
        self.map_of_sid_to_tid_freqs = {}

    @staticmethod
    def counts_from_question_list(question_list: list, column: str) -> dict:
        counts = defaultdict(int)

        for question in question_list:
            counts[str(question[column])] += 1

        return counts

    def generate_all_stats(self):
        return self.generate_stat__answer_per_tid_and_sid() \
            .generate_stat__answer_frequencies_per_sid() \
            .generate_stat__answer_per_template() \
            .generate_stat__answer_frequencies() \
            .generate_stat__template_per_sid()

    def generate_stat__answer_per_template(self):
        answer_freq_v_template_id = []
        template_ids = self.dataset.get_unique_values("template_id")
        for tid in template_ids:
            questions = Funnel(self.dataset.questions) \
                .filter(lambda q: q["template_id"] == tid) \
                .get_result()

            answer_counts = DatasetStatistics.counts_from_question_list(questions, "answer")
            self.map_of_tid_to_answer_freqs[tid] = answer_counts
            for answer, count in answer_counts.items():
                answer_freq_v_template_id.append({"template_id": tid,
                                                  "answer": answer,
                                                  "answer_type": self.dataset.get_answer_type_for_answer(answer),
                                                  "count": count})
        self.answer_freq_per_tid = answer_freq_v_template_id
        return self

    def generate_stat__answer_per_tid_and_sid(self):
        answer_freq_per_tid_and_sid = []
        template_ids = self.dataset.get_unique_values("template_id")
        simulation_ids = self.dataset.get_unique_values("simulation_id")
        for sid in simulation_ids:
            for tid in template_ids:
                questions = Funnel(self.dataset.questions) \
                    .filter(lambda q: q["template_id"] == tid and q["simulation_id"] == sid) \
                    .get_result()

                answer_counts = DatasetStatistics.counts_from_question_list(questions, "answer")
                self.map_of_sid_tid_pairs_to_answer_freqs[(sid, tid)] = answer_counts
                for answer, count in answer_counts.items():
                    answer_freq_per_tid_and_sid.append({"template_id": tid,
                                                        "simulation_id": sid,
                                                        "answer": answer,
                                                        "answer_type": self.dataset.get_answer_type_for_answer(answer),
                                                        "count": count})
        self.answer_freq_per_tid_and_sid = answer_freq_per_tid_and_sid
        return self

    def generate_stat__answer_frequencies_per_sid(self):
        sim_id_v_answer_freq = []
        sim_ids = self.dataset.get_unique_values("simulation_id")
        for sid in sim_ids:
            questions = Funnel(self.dataset.questions) \
                .filter(lambda q: q["simulation_id"] == sid) \
                .get_result()

            answer_id_counts = DatasetStatistics.counts_from_question_list(questions, "answer")
            self.map_of_sid_to_answer_freqs[sid] = answer_id_counts
            for answer, count in answer_id_counts.items():
                sim_id_v_answer_freq.append({"simulation_id": sid,
                                             "answer": answer,
                                             "answer_type": self.dataset.get_answer_type_for_answer(answer),
                                             "count": count})
        self.answer_freq_per_sid = sim_id_v_answer_freq
        return self

    def generate_stat__template_per_sid(self):
        sim_id_v_template_freq = []
        sim_ids = self.dataset.get_unique_values("simulation_id")
        for sid in sim_ids:
            questions = Funnel(self.dataset.questions) \
                .filter(lambda q: q["simulation_id"] == sid) \
                .get_result()

            template_id_counts = DatasetStatistics.counts_from_question_list(questions, "template_id")
            self.map_of_sid_to_tid_freqs[sid] = template_id_counts
            for tid, count in template_id_counts.items():
                sim_id_v_template_freq.append({"simulation_id": sid, "template_id": tid, "count": count})
        self.template_id_freq_per_sid = sim_id_v_template_freq
        return self

    def generate_stat__answer_frequencies(self):
        answer_counts = DatasetStatistics.counts_from_question_list(self.dataset.questions, "answer")
        self.answer_freq_total = answer_counts
        return self


class DatasetStatisticsExporter:
    def __init__(self, stats: DatasetStatistics, output_folder="statistics", export_png=True):
        self.stats = stats
        self.export_png = export_png
        self.output_folder = output_folder
        os.makedirs(output_folder, exist_ok=True)

    def generate_sunburst_charts_for_each_scene_type(self):
        unique_sids = self.stats.dataset.get_unique_values("simulation_id")
        for sid in unique_sids:
            self.generate_sunburst_chart_question_type_to_answer_type_to_answer_in_a_scene_type(
                f"Answer distribution of each question type for SID={sid}", sid, self.stats.dataset.questions
            )

    def generate_sunburst_chart_question_type_to_answer_type_to_answer_in_a_scene_type(self, title, sid: str,
                                                                                       questions):
        filtered = Funnel(questions).filter(lambda question: question["simulation_id"] == sid).get_result()

        for i in range(len(filtered)):
            filtered[i].update({"question_type": filtered[i]["template_id"].split("_")[0].capitalize(),
                                "answer": filtered[i]["answer"].capitalize()})

        df = pd.DataFrame(filtered)
        fig = px.sunburst(df, path=['question_type', 'answer_type', 'answer'],
                          color='answer_type',
                          color_discrete_map={'Boolean': '#8dd3c7', 'Count': '#ffffb3', 'Color': '#bebada',
                                              'Shape': '#fb8072', 'Size': '#80b1d3'}
                          )

        fig.update_traces(marker=dict(line=dict(color='#000000', width=0.5)))
        fig.update_layout(
            autosize=False,
            width=640,
            height=640,
            font=dict(
                size=16,
                family="Times New Roman",
            ),
        )

        if self.export_png:
            if not os.path.exists(self.output_folder):
                os.makedirs(self.output_folder)
            fig.write_image(self.output_folder + os.path.sep + title + ".png")
            fig.write_image(self.output_folder + os.path.sep + title + ".pdf")
        else:
            fig.show()

    def generate_sunburst_chart_question_type_to_answer_type_to_answer_for_whole_dataset(self):
        title = "Answer distribution of the dataset"

        filtered = list(self.stats.dataset.questions)

        for i in range(len(filtered)):
            filtered[i].update({"question_type": filtered[i]["template_id"].split("_")[0].capitalize(),
                                "answer": filtered[i]["answer"].capitalize()})

        df = pd.DataFrame(filtered)
        fig = px.sunburst(df, path=['question_type', 'answer_type', 'answer'],
                          color='answer_type',
                          color_discrete_map={'Boolean': '#8dd3c7', 'Count': '#ffffb3', 'Color': '#bebada',
                                              'Shape': '#fb8072', 'Size': '#80b1d3'}
                          )

        fig.update_traces(marker=dict(line=dict(color='#000000', width=0.5)))
        fig.update_layout(
            autosize=False,
            width=640,
            height=640,
            font=dict(
                size=16,
                family="Times New Roman",
            ),
        )

        if self.export_png:
            if not os.path.exists(self.output_folder):
                os.makedirs(self.output_folder)
            fig.write_image(self.output_folder + os.path.sep + title + ".png")
            fig.write_image(self.output_folder + os.path.sep + title + ".pdf")
        else:
            fig.show()

    def generate_pie_chart(self, title, counts, labels, colors, explodes):
        fig = go.Figure(data=[go.Pie(labels=labels, values=counts, textinfo='label+percent+value',
                                     texttemplate="%{label}: %{percent} (%{value})",
                                     insidetextorientation='radial',
                                     titleposition="top center",
                                     title=title,
                                     sort=False,
                                     marker=dict(colors=[str(color.hex) for color in colors])
                                     )])

        fig.update_layout(
            autosize=False,
            width=640,
            height=640,
            # legend=dict(
            #     y=1.3,
            #     x=-0.30,
            #     bgcolor="rgba(0,0,0,0)",
            # ),
            font=dict(
                size=16,
            ),
            legend_title_text='Answers'
        )

        if self.export_png:
            if not os.path.exists(self.output_folder):
                os.makedirs(self.output_folder)
            fig.write_image(self.output_folder + os.path.sep + title + ".png")
        else:
            fig.show()

        """
        fig1, ax1 = plt.subplots(figsize=(12, 12))
        ax1.pie(counts,
                labels=labels,
                colors=colors,
                autopct='%1.1f%%',
                startangle=90,
                radius=4,
                explode=explodes)

        '''ax1.legend(wedges, answers_sorted,
                  title="Answers",
                  loc="best")'''

        ax1.axis('equal')  # Equal aspect ratio ensures that pie is drawn as a circle.
        ax1.set_title(title)
        plt.tight_layout()

        if self.export_png:
            if not os.path.exists(self.output_folder):
                os.makedirs(self.output_folder)
            plt.savefig(self.output_folder + os.path.sep + title + ".png")
        else:
            plt.show()
        """

    @staticmethod
    def answer_counts_from_question_list(question_list: list) -> dict:
        answer_counts = defaultdict(int)

        for question in question_list:
            answer_counts[str(question["answer"])] += 1

        return answer_counts

    def generate_stat__answer_counts(self, answer_counts, title):
        # TODO: Beautify the charts.
        # Pie chart, where the slices will be ordered and plotted counter-clockwise:
        answers = list(answer_counts.keys())

        colors = []
        answers_sorted = []
        counting_answers = sorted([answer for answer in answers if
                                   self.stats.dataset.get_answer_type_for_answer(answer) == "Count"], reverse=True,
                                  key=lambda x: answer_counts[x])
        shape_answers = sorted([answer for answer in answers if
                                self.stats.dataset.get_answer_type_for_answer(answer) == "Shape"], reverse=True,
                               key=lambda x: answer_counts[x])
        color_answers = sorted([answer for answer in answers if
                                self.stats.dataset.get_answer_type_for_answer(answer) == "Color"], reverse=True,
                               key=lambda x: answer_counts[x])
        boolean_answers = sorted([answer for answer in answers if
                                  self.stats.dataset.get_answer_type_for_answer(answer) == "Boolean"], reverse=True,
                                 key=lambda x: answer_counts[x])

        answers_sorted.extend(counting_answers)
        if len(counting_answers) > 0:
            colors.extend(Color("#8a3059").range_to(Color("#f4a3a8"), len(counting_answers)))
        answers_sorted.extend(shape_answers)
        if len(shape_answers) > 0:
            colors.extend(Color("#266a6e").range_to(Color("#96d2a4"), len(shape_answers)))
        answers_sorted.extend(color_answers)
        if len(color_answers) > 0:
            colors.extend(Color("#3b738f").range_to(Color("#a8dbd9"), len(color_answers)))
        answers_sorted.extend(boolean_answers)
        if len(boolean_answers) > 0:
            colors.extend(Color("#555555").range_to(Color("#aaaaaa"), len(boolean_answers)))

        counts = [answer_counts[answer] for answer in answers_sorted]

        # answers_sorted = [f"{answer} ({answer_counts[answer]})" for answer in answers_sorted]
        # explodes_s = [0.1] * len(answers_sorted)
        proportions = {c: c / sum(counts) for c in counts}
        explodes = [pow(1 - proportions[c] / max(proportions.values()), 2) for c in counts]

        self.generate_pie_chart(title, counts, labels=answers_sorted, colors=list(colors), explodes=explodes)

    def generate_chart__answer_per_template(self):
        df = pd.DataFrame(self.stats.answer_freq_per_tid)
        FileIO.write_to_file(f"{self.output_folder}/Answer frequencies per each template ID.csv",
                             df.to_csv())
        for tid in self.stats.map_of_tid_to_answer_freqs:
            self.generate_stat__answer_counts(self.stats.map_of_tid_to_answer_freqs[tid], f'Template ID={tid}')

    def generate_chart__answer_per_template_and_simulation(self):
        df = pd.DataFrame(self.stats.answer_freq_per_tid_and_sid)
        FileIO.write_to_file(
            f"{self.output_folder}/Answer frequencies per each template ID and sim ID.csv",
            df.to_csv())
        for key in self.stats.map_of_sid_tid_pairs_to_answer_freqs:
            tid = key[1]
            sid = key[0]
            self.generate_stat__answer_counts(self.stats.map_of_sid_tid_pairs_to_answer_freqs[key],
                                              f'SID={sid}-TID={tid}')

    def generate_chart__answer_frequencies_per_sim_id(self):
        df = pd.DataFrame(self.stats.answer_freq_per_sid)
        FileIO.write_to_file(f"{self.output_folder}/Answer frequencies for each simulation ID.csv",
                             df.to_csv())
        for sid in self.stats.map_of_sid_to_answer_freqs:
            self.generate_stat__answer_counts(self.stats.map_of_sid_to_answer_freqs[sid],
                                              f'Answer frequencies for Simulation ID={sid}')

    def generate_chart__template_per_sim_id(self):
        df = pd.DataFrame(self.stats.generate_stat__template_per_sid().template_id_freq_per_sid)
        FileIO.write_to_file(
            f"{self.output_folder}/Template ID frequencies for each simulation type.csv",
            df.to_csv())
        for sid in self.stats.map_of_sid_to_tid_freqs:
            self.generate_stat__answer_counts(self.stats.map_of_sid_to_tid_freqs[sid],
                                              f'Template ID frequencies for Simulation ID={sid}')

    def generate_chart__answer_frequencies(self):
        answer_counts = self.stats.answer_freq_total
        self.generate_stat__answer_counts(answer_counts, f'Answer frequencies')


class DatasetGenerationConfig:
    def __init__(self, config_dict):
        self.dataset_size = config_dict['dataset_size']
        self.dataset_metadata_file_path = config_dict['metadata_file_path']
        self.executable_path = str(Path(config_dict['executable_path']).resolve().as_posix())
        self.executable_working_directory = str(Path(config_dict['executable_working_directory']).resolve().as_posix())
        self.output_folder_path = str(Path(config_dict['output_folder_path']).resolve().as_posix())

        self.split_ratios = config_dict['split_ratios'] if 'split_ratios' in config_dict else None
        self.split_dataset = self.split_ratios is not None
        if self.split_dataset:  # Deprecated
            self.test_set_ratio = self.split_ratios['test']
            self.validation_set_ratio = self.split_ratios['validation']
            self.train_set_ratio = self.split_ratios['train']

        self.simulation_configs = config_dict['simulation_configs']
        if self.split_dataset:  # Deprecated
            self.sim_ids_for_each_split = config_dict['sim_ids_for_each_split']
            self.train_set_scene_ids = self.sim_ids_for_each_split["train"] if self.sim_ids_for_each_split[
                                                                                   "train"] is not None else [
                config["id"] for config in self.simulation_configs]
            self.validation_set_scene_ids = self.sim_ids_for_each_split["validation"] if self.sim_ids_for_each_split[
                                                                                             "validation"] is not None else [
                config["id"] for config in self.simulation_configs]
            self.test_set_scene_ids = self.sim_ids_for_each_split["test"] if self.sim_ids_for_each_split[
                                                                                 "test"] is not None else [config["id"]
                                                                                                           for config in
                                                                                                           self.simulation_configs]

        self.offline = config_dict['offline']

        self.concurrent_process_count = 16
        if 'concurrent_process_count' in config_dict:
            # Override default value
            self.concurrent_process_count = config_dict['concurrent_process_count']

        self.should_generate_questions: bool = True
        if 'do_not_generate_questions' in config_dict:
            # Override default value
            self.should_generate_questions = not config_dict['do_not_generate_questions']

        self.enable_perturbation: bool = False
        self.perturbation_config = None
        if 'perturbation_config' in config_dict:
            self.enable_perturbation = True
            self.perturbation_config = config_dict['perturbation_config']
            if 'main_seed' not in self.perturbation_config:
                self.perturbation_config['main_seed'] = None
            if 'perturbations_per_simulation' not in self.perturbation_config:
                self.perturbation_config['perturbations_per_simulation'] = 5
            if 'amount' not in self.perturbation_config:
                self.perturbation_config['amount'] = 0.0


class DatasetGenerator:
    """
    Generates a dataset that contains simulation outputs with variations and their videos.

    Single data in the dataset is generated as follows:
    - Run a simulation
    - Run its variations
    - Run its perturbations
    - Generate question-answer pairs based on the simulation and its variations & perturbations
    - Merge the questions into a single file along with video paths.

    - Dataset folder
      - /intermediates          (A folder for intermediate outputs, may be used for debugging purposes.)
        - /sid_1
          - 000000.json         (One simulation output with variations.)
          - 000001.json
            ...
          - /debug              (A folder for command line outputs from simulations)
          - /controllers        (A folder for controller files for simulations)
          - /perturbations      (Contains perturbation outputs for each simulation instance)
          - /variations         (Contains variation outputs for each simulation instance)
          - /questions          (Contains both original questions generated from a simulation and its variations,
                                    and questions that are sieved with perturbations)
        - /sid_2
          ...
        ...
      - /videos
        - /sid_1
          - 000000.mpg
          - 000001.mpg
            ...
      - dataset_minimal.json    (Flat JSON array of questions in the dataset without properties like "program" for
                                    each question.)
      - dataset.json            (This json file is collected in a later stage, "FullDatasetWriteStage". It is organized
                                    per-simulation, and includes "program" property for each question.)
    """

    def __init__(self, config: DatasetGenerationConfig):
        self.config = config
        self.__state_file_path = f"{config.output_folder_path}/dataset_generation_state"
        self.__runner = SimulationRunner(self.config.executable_path, self.config.executable_working_directory)
        # To measure remaining and elapsed_time.
        self.__start_time = None
        self.__times = np.array([])

    @staticmethod
    def generate_video_and_questions_in_parallel(obj, instance_id: int, simulation_config: dict):
        obj.generate_video_and_questions(instance_id, simulation_config)

    def get_state(self) -> int:
        last_state = None
        if os.path.exists(self.__state_file_path):
            with open(self.__state_file_path, "r") as state_file:
                last_state = int(state_file.read())
        return last_state

    def __save_state(self, index: int):
        with(open(self.__state_file_path, "w")) as state_file:
            state_file.write(f"{index}")
            state_file.close()

    def __remove_state_file(self):
        os.remove(self.__state_file_path)

    def dump_controller_file(self,
                             instance_id: int,
                             simulation_config: dict,
                             controller_file_path: str):
        sid = simulation_config["id"]

        with open(controller_file_path, 'w') as controller_file:
            json.dump(
                json.loads(
                    f"""{{
                            "simulationID": {sid},
                            "offline": {str(self.config.offline).lower()},
                            "outputVideoPath": "{self.get_video_output_path(sid, instance_id)}",
                            "outputJSONPath": "{self.get_bare_simulation_output_path(sid, instance_id)}",
                            "width": {simulation_config['width']},
                            "height": {simulation_config['height']},
                            "inputScenePath":  "",
                            "stepCount": {simulation_config['step_count']}
                        }}"""),
                controller_file
            )

    def dump_perturbation_controller_file(self,
                                          pid: int,
                                          instance_id: int,
                                          simulation_config: dict,
                                          perturbation_config: dict,
                                          controller_file_path: str):
        sid = simulation_config["id"]
        seed = -1  # TODO.
        with open(controller_file_path, 'w') as controller_file:
            json.dump(
                json.loads(
                    f"""{{
                            "simulationID": {sid},
                            "offline": {str(self.config.offline).lower()},
                            "outputVideoPath": "{self.get_perturbation_video_output_path(sid, instance_id, pid)}",
                            "outputJSONPath": "{self.get_perturbation_bare_simulation_output_path(sid, instance_id, pid)}",
                            "width": {simulation_config['width']},
                            "height": {simulation_config['height']},
                            "inputScenePath": "{self.get_bare_simulation_output_path(sid, instance_id)}",
                            "stepCount": {simulation_config['step_count']},
                            "perturbationSeed": {seed},
                            "noiseAmount": {perturbation_config['amount']}
                        }}"""),
                controller_file
            )

    def __update_clock(self, diff, total_runs: int, current: int):
        times = np.append(self.__times, diff)
        logger.info(f"Approximately {round((np.mean(times) * (total_runs - current - 1)) / 60, 2)} "
                    "minutes remaining".ljust(75, " "))

    def __generate_configs_to_run(self) -> List[Dict]:
        if self.config.split_dataset:  # Deprecated
            configs_to_run = []
            # Train
            for sid in self.config.train_set_scene_ids:
                config_for_sid = next(config for config in self.config.simulation_configs if config["id"] == sid)
                train_configs = [copy.deepcopy(config_for_sid)] * (
                        int(self.config.dataset_size * self.config.train_set_ratio)
                        // len(self.config.train_set_scene_ids))
                for conf in train_configs:
                    conf["split"] = "train"
                configs_to_run.extend(train_configs)
            # Validation
            for sid in self.config.validation_set_scene_ids:
                config_for_sid = next(config for config in self.config.simulation_configs if config["id"] == sid)
                validation_configs = [copy.deepcopy(config_for_sid)] * (
                        int(self.config.dataset_size * self.config.validation_set_ratio)
                        // len(self.config.validation_set_scene_ids))
                for conf in validation_configs:
                    conf["split"] = "validation"
                configs_to_run.extend(validation_configs)
            # Test
            for sid in self.config.test_set_scene_ids:
                config_for_sid = next(config for config in self.config.simulation_configs if config["id"] == sid)
                test_configs = [copy.deepcopy(config_for_sid)] * (
                        int(self.config.dataset_size * self.config.test_set_ratio)
                        // len(self.config.test_set_scene_ids))
                for conf in test_configs:
                    conf["split"] = "test"
                configs_to_run.extend(test_configs)

            # Collate, sid: 1,2,3,4,5,1,2,3,4,5
            sid_to_config = defaultdict(list)
            for config in configs_to_run:
                sid_to_config[config["id"]].append(config)

            sids = sorted(list(set(sid_to_config.keys())))
            collated = []
            N = len(configs_to_run)
            M = len(sids)
            for i in range(N):
                collated.append(sid_to_config[sids[i % M]].pop(0))

            return collated
        else:
            configs_to_run = []
            for simulation_config in self.config.simulation_configs:
                configs_to_run.extend(
                    [simulation_config] * (self.config.dataset_size // len(self.config.simulation_configs)))

            return configs_to_run

    def generate_video_and_questions(self, instance_id: int, simulation_config: dict):
        sid = simulation_config["id"]

        logger.info(f"Running simulation with SID: {sid}, instance_id: {instance_id:06d}")

        # Create controller file for current simulation instance.
        controller_file_path = self.get_controller_path(sid, instance_id)
        logger.info(f"{instance_id:06d}: Creating controller file")
        self.dump_controller_file(instance_id, simulation_config, controller_file_path)

        variations_output_path = self.get_simulation_with_variations_output_path(sid, instance_id)

        questions_file_path = self.get_questions_output_path(sid, instance_id)

        simulation = SimulationInstance(instance_id,
                                        controller_file_path,
                                        variations_output_path,
                                        questions_file_path,
                                        self.__runner)

        # Run simulation.
        logger.info(f"{instance_id:06d}: Running base simulation")
        simulation.run_simulation(self.get_debug_output_path(sid, instance_id))

        # Run its variations.
        logger.info(f"{instance_id:06d}: Running variations of the base simulation")
        simulation.run_variations()

        # Generate questions.
        if self.config.should_generate_questions:
            try:
                logger.info(f"{instance_id:06d}: Generating questions and answers for the base simulation")
                simulation.generate_questions(simulation_config)

                # Perturbations
                if self.config.enable_perturbation:

                    original_questions_backup_file_path = self.get_original_questions_output_path(sid, instance_id)
                    orig_questions = FileIO.read_json(questions_file_path)
                    FileIO.write_json(orig_questions, original_questions_backup_file_path)

                    perturbation_config = self.config.perturbation_config
                    for pid in range(perturbation_config["perturbations_per_simulation"]):
                        perturbation_controller_file_path = self.get_perturbation_controller_path(sid, instance_id, pid)

                        self.dump_perturbation_controller_file(pid, instance_id, simulation_config,
                                                               perturbation_config, perturbation_controller_file_path)

                        perturbed_with_variations_output_path = self.get_perturbation_with_variations_output_path(sid,
                                                                                                                  instance_id,
                                                                                                                  pid)

                        perturbed_questions_file_path = self.get_perturbation_questions_output_path(sid,
                                                                                                    instance_id,
                                                                                                    pid)

                        perturbation_instance = SimulationInstance(instance_id,
                                                                   perturbation_controller_file_path,
                                                                   perturbed_with_variations_output_path,
                                                                   perturbed_questions_file_path,
                                                                   self.__runner)

                        logger.info(f"{instance_id:06d} perturbation {pid}: Running a perturbation of base simulation")
                        perturbation_instance.run_simulation(
                            self.get_perturbation_debug_output_path(sid, instance_id, pid))
                        logger.info(
                            f"{instance_id:06d} perturbation {pid}: Running variations of the perturbation of base simulation")
                        perturbation_instance.run_variations()
                        logger.info(
                            f"{instance_id:06d} perturbation {pid}: Regenerating questions and answers for the perturbed simulation")
                        perturbation_instance.generate_questions(simulation_config)

                        Perturbator.regenerate_answers(
                            original_variations_output_file_path=variations_output_path,
                            perturbed_variations_output_path=perturbed_with_variations_output_path,
                            original_questions_path=questions_file_path,
                            new_perturbed_qa_file_path=perturbed_questions_file_path,
                            metadata_path=self.config.dataset_metadata_file_path
                        )

                        questions_original = FileIO.read_json(questions_file_path)
                        questions_perturbed = FileIO.read_json(perturbed_questions_file_path)

                        data, orig_size, found, ratio = Perturbator.measure_similarity(questions_original["questions"],
                                                                                       questions_perturbed["questions"])
                        logger.info(f"{instance_id:06d} perturbation {pid} match ratio: {found / orig_size}")
                        logger.info(f"{instance_id:06d} perturbation {pid} correctness: {ratio}")

                        correct_questions = FileIO.read_json(questions_file_path)
                        correct_questions["questions"] = data["correct"]

                        logger.info(f"{instance_id:06d}: Overriding previous questions")
                        FileIO.write_json(correct_questions, questions_file_path)
            except Exception as e:
                traceback.print_exception(type(e), e, e.__traceback__)
                logger.error(f"{instance_id:06d}: Error while generating questions")
        else:
            logger.info(f"{instance_id:06d}: Bypassing question generation")

    def execute(self):
        logger.info("Dataset generation process has started.")

        # To measure remaining time.
        self.__start_time = time.time()

        concurrent_process_count = self.config.concurrent_process_count
        logger.info(f"Set concurrent process count to {concurrent_process_count}")

        self.make_directories()

        configs_to_run = self.__generate_configs_to_run()

        start = 0
        if self.get_state() is not None:
            start = self.get_state()
            logger.info(f"Dataset generation state file found at output folder path, continuing from {start}...")

        for i in range(start, len(configs_to_run), concurrent_process_count):
            t1 = time.time()  # To measure remaining time.

            self.__save_state(i)

            jobs = []
            args = []
            for instance_id in range(i, i + concurrent_process_count):
                if len(configs_to_run) > instance_id:
                    simulation_config = configs_to_run[instance_id]
                    jobs.append(DatasetGenerator.generate_video_and_questions_in_parallel)
                    args.append([self, instance_id, simulation_config])

            concurrent_processes = MultithreadedProcessor(jobs, args)
            logger.info(f"Forking simulation processes into threads")
            concurrent_processes.fork_processes()
            logger.info(f"Starting simulations from {i} to {i + concurrent_process_count}")
            concurrent_processes.start_all()
            logger.info(f"Waiting for simulations to finish")
            concurrent_processes.join_all()
            logger.info(f"Joined all threads into main thread")

            self.__update_clock((time.time() - t1) / concurrent_process_count, len(configs_to_run),
                                i + concurrent_process_count)

        logger.info(
            f"Dataset generation is complete. Process took {round((time.time() - self.__start_time) / 60, 2)} minutes.")

        if self.config.should_generate_questions:
            logger.info(f"Dumping dataset...")
            self.__dump_dataset()
        else:
            logger.info(f"Not dumping the dataset, since 'do_not_generate_questions' flag is set to true")

        # TODO: Verify the integrity of dataset

    def __dump_dataset(self):
        metadata = FileIO.read_json(self.config.dataset_metadata_file_path)
        # with open(f"{self.config.output_folder_path}/dataset.json", "w") as dataset_file:
        with open(f"{self.config.output_folder_path}/dataset_minimal.json", "w") as minimal_dataset_file:
            minimal_dataset_file.write("[")

            configs_to_run = self.__generate_configs_to_run()

            logger.info(f"Incrementally writing to minimal dataset JSON file...")
            logger.info(f"Converting absolute paths to relative paths "
                        f"along the way, based on current working directory...")
            for instance_id in range(len(configs_to_run)):
                simulation_config = configs_to_run[instance_id]
                sid = simulation_config["id"]

                questions_file_path = self.get_questions_output_path(sid, instance_id)

                try:
                    # Add them into dataset.json
                    with open(questions_file_path, "r") as questions_file:
                        simulation_instance = json.loads(f"""{{
                                    "simulation_id": "{sid}",
                                    "video_path": "{self.get_video_output_path(sid, instance_id)}",
                                    "questions": {json.dumps(json.load(questions_file))}
                                 }}""")

                        if instance_id % 10 == 0:
                            logger.info(f"Writing: {instance_id}/{len(configs_to_run)}")

                        simulation_instance = \
                            DatasetUtils.relativize_paths([simulation_instance], self.config.output_folder_path)[0]
                        minimal_qa = DatasetUtils.convert_to_minimal_version(simulation_instance, metadata)

                        for i, question_obj in enumerate(minimal_qa):
                            minimal_dataset_file.write(json.dumps(question_obj))
                            if instance_id == len(configs_to_run) - 1 and i == len(minimal_qa) - 1:
                                pass
                            else:
                                minimal_dataset_file.write(",")
                        if instance_id != len(configs_to_run) - 1:
                            pass
                        else:
                            minimal_dataset_file.write("]")
                except FileNotFoundError:
                    logger.warning(f"{instance_id:06d}: Questions file cannot be found")
                    continue

        logger.info(f"Successfully written to: {self.config.output_folder_path}")

    def make_directories(self):
        os.makedirs(self.config.output_folder_path, exist_ok=True)

        dataset = json.loads("[]")

        with open(f"{self.config.output_folder_path}/dataset.json", "w") as f:
            json.dump(dataset, f, indent=4)

        for sim in self.config.simulation_configs:
            os.makedirs(f"{self.config.output_folder_path}/intermediates/sid_{sim['id']}/controllers", exist_ok=True)
            os.makedirs(f"{self.config.output_folder_path}/intermediates/sid_{sim['id']}/simulations", exist_ok=True)
            os.makedirs(f"{self.config.output_folder_path}/intermediates/sid_{sim['id']}/perturbations", exist_ok=True)
            os.makedirs(f"{self.config.output_folder_path}/intermediates/sid_{sim['id']}/questions", exist_ok=True)
            os.makedirs(f"{self.config.output_folder_path}/intermediates/sid_{sim['id']}/debug", exist_ok=True)
            os.makedirs(f"{self.config.output_folder_path}/videos/sid_{sim['id']}", exist_ok=True)

    def get_controller_path(self, sid: int, instance_id: int):
        return f"{self.config.output_folder_path}/intermediates/sid_{sid}/controllers/controller_{instance_id:06d}.json"

    def get_questions_output_path(self, sid: int, instance_id: int):
        return f"{self.config.output_folder_path}/intermediates/sid_{sid}/questions/qa_{instance_id:06d}.json"

    def get_original_questions_output_path(self, sid: int, instance_id: int):
        return f"{self.config.output_folder_path}/intermediates/sid_{sid}/questions/qa_orig_{instance_id:06d}.json"

    def get_simulation_with_variations_output_path(self, sid: int, instance_id: int):
        return f"{self.config.output_folder_path}/intermediates/sid_{sid}/{instance_id:06d}.json"

    def get_video_output_path(self, sid: int, instance_id: int):
        return f"{self.config.output_folder_path}/videos/sid_{sid}/{instance_id:06d}.mpg"

    def get_bare_simulation_output_path(self, sid: int, instance_id: int):
        return f"{self.config.output_folder_path}/intermediates/sid_{sid}/simulations/{instance_id:06d}.json"

    def get_debug_output_path(self, sid: int, instance_id: int):
        return f"{self.config.output_folder_path}/intermediates/sid_{sid}/debug/cl_debug_{instance_id:06d}.txt"

    def get_perturbation_debug_output_path(self, sid: int, instance_id: int, pid: int):
        return f"{self.config.output_folder_path}/intermediates/sid_{sid}/debug/cl_perturbation_debug_{instance_id:06d}_{pid}.txt"

    def get_perturbation_controller_path(self, sid: int, instance_id: int, pid: int):
        return f"{self.config.output_folder_path}/intermediates/sid_{sid}/perturbations/p_controller_{instance_id:06d}_{pid}.json"

    def get_perturbation_questions_output_path(self, sid: int, instance_id: int, pid: int):
        return f"{self.config.output_folder_path}/intermediates/sid_{sid}/perturbations/p_qa_{instance_id:06d}_{pid}.json"

    def get_perturbation_video_output_path(self, sid: int, instance_id: int, pid: int):
        return f"{self.config.output_folder_path}/intermediates/sid_{sid}/perturbations/p_{instance_id:06d}_{pid}.mpg"

    def get_perturbation_with_variations_output_path(self, sid: int, instance_id: int, pid: int):
        return f"{self.config.output_folder_path}/intermediates/sid_{sid}/perturbations/p_{instance_id:06d}_{pid}_full.json"

    def get_perturbation_bare_simulation_output_path(self, sid: int, instance_id: int, pid: int):
        return f"{self.config.output_folder_path}/intermediates/sid_{sid}/perturbations/p_{instance_id:06d}_{pid}.json"

