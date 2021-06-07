import glob
import json
import os
import time
from abc import ABC, abstractmethod
from collections import defaultdict
from pathlib import Path
from random import Random
from typing import List

from loguru import logger

from framework.balance import DatasetInspector, DatasetUnderSampler
from framework.dataset import DatasetGenerationConfig, DatasetGenerator, CRAFTDataset, DatasetStatistics, DatasetUtils
from framework.utils import FileIO


class Stage(ABC):

    def __init__(self, name=None):
        self._owner = None
        self.name = name

    @abstractmethod
    def process(self, obj: object):
        pass

    def cleanup(self):
        pass

    @abstractmethod
    def get_output(self):
        pass

    def set_owner(self, owner):
        self._owner = owner


class Pipeline:

    def __init__(self, stages: List[Stage] = None):
        self.stages: List[Stage] = [] if stages is None else stages

    def add_stage(self, stage: Stage):
        stage.set_owner(self)
        self.stages.append(stage)

    def execute_all(self):
        stage_index = 1
        next_input = self.__initial_data
        for stage in self.stages:
            start_time = time.time()

            stage.process(next_input)
            stage.cleanup()

            next_input = stage.get_output()

            logger.info(f"{stage.name if stage.name is not None else f'Pipeline Stage {stage_index}'} "
                        f"took {round((time.time() - start_time) / 60, 2)} minutes")
            stage_index += 1

    def feed_first_stage(self, initial_data: object):
        self.__initial_data: object = initial_data


class DatasetGenerationStage(Stage):

    def __init__(self):
        """
        Generates a CRAFT dataset according to the supplied configuration object.

        Depending on the dataset size, this can take a long time and large disk space.
        For instance, generating a dataset that consists of 10000 videos could take approximately 2 days and
        ~170GB of disk space. Much of the disk space is occupied by the "intermediates" folder
        in the root dataset folder (but it is not a part of the dataset itself),
        which includes data about each simulation instance and their corresponding
        variation & perturbation data along with the debug data.
        """
        super().__init__(name="Dataset Generation Stage")
        self.__dataset = None

    def process(self, config: DatasetGenerationConfig):
        logger.info("Initiating dataset generation process...")
        dataset_generator = DatasetGenerator(config)
        dataset_generator.execute()
        dataset_folder_path = dataset_generator.config.output_folder_path
        self.__dataset = CRAFTDataset(dataset_folder_path, FileIO.read_json(config.dataset_metadata_file_path))

    def get_output(self):
        return self.__dataset


class DatasetStatisticsGenerationStage(Stage):

    def __init__(self):
        super().__init__()
        self.__dataset_statistics = None

    def process(self, dataset: CRAFTDataset):
        self.__dataset_statistics = DatasetStatistics(dataset)
        self.__dataset_statistics.generate_all_stats()

    def get_output(self):
        return self.__dataset_statistics


class InspectionStage(Stage):

    def __init__(self):
        super().__init__()
        self.__needed_answers = None
        self.__inspector = None

    def process(self, stats: DatasetStatistics):
        self.__inspector = DatasetInspector(stats)
        self.__needed_answers: dict = self.__inspector.compute_answers_needed_for_tid_and_sid_versus_answer_balance()

    def cleanup(self):
        pass

    def get_output(self):
        return {"needed_answers": self.__needed_answers}


class PreBalancingPostProcessStage(Stage):

    def __init__(self):
        super().__init__(name="Pre-Balancing Post-Process Stage")
        self.__dataset_obj: CRAFTDataset = None

    def process(self, dataset_obj: CRAFTDataset):
        logger.info("Initiating post process stage before balancing...")

        self.__dataset_obj = dataset_obj

        for i, instance_id in enumerate(sorted(dataset_obj.video_index_to_questions_map.keys())):
            question_list = dataset_obj.video_index_to_questions_map[instance_id]
            sid = int(question_list[0]["simulation_id"])

            annotations = FileIO.read_json(dataset_obj.get_simulation_with_variations_output_path(sid, instance_id))
            objects_in_scene = annotations["original_video_output"]["scene_states"][0]["scene"]["objects"]
            dynamic_objects = [object for object in objects_in_scene if object["bodyType"] == 2]

            new_questions_list = []
            for question in question_list:
                # Postprocess Before Balancing 1: Do not ask shape if only one shape is present in the scene.
                answer_type = dataset_obj.get_answer_type_for_answer(question["answer"])
                if answer_type == "Shape":
                    if len(set([f"{object['shape']}" for object in dynamic_objects])) <= 1:
                        # Remove the question that asks shape even though there's only one shape present
                        logger.info(f"Question asks shape even though there's only 1 "
                                    f"shape present in the scene. Removing {question['video_index']}/{question['question_index']}")
                        continue

                if "hexagon" in question["question"]:
                    logger.info(f"Question asks about hexagons, which are not present in any of the videos. "
                                f"Removing {question['video_index']}/{question['question_index']}")
                    continue

                # Postprocess Before Balancing 2: Remove questions regarding collisions with the basket
                # to avoid ambiguity. Note that these are not yet removed from the question template
                # files in svqa/SVQA_1.0_templates. Following can be removed from post processing once
                # they are removed from the question template files and if the dataset is generated
                # according to the updated question templates.
                if question["template_id"] in [
                    "cause_2",
                    "cause_5",
                    "counterfactual_2",
                    "counterfactual_5",
                    "counterfactual_8",
                    "descriptive_12",
                    "descriptive_13",
                    "descriptive_14",
                    "descriptive_15",
                    "descriptive_20",
                    "descriptive_21",
                    "descriptive_30",
                    "descriptive_31",
                    "descriptive_36",
                    "descriptive_37",
                    "enable_2",
                    "enable_5",
                    "prevent_2",
                    "prevent_5",
                ]:
                    continue

                # Postprocess Before Balancing 3: Correct typos in the question templates.
                # These are also corrected in the question template files in svqa/SVQA_1.0_templates,
                # so the following can be deleted.
                if question["template_id"] == "counterfactual_2":
                    question_text: str = question["question"]
                    if question_text.startswith("Will"):
                        question_text = question_text.replace("the basket the", "the basket if the")
                        question_text = question_text.replace("the container the", "the container if the")
                        question_text = question_text.replace("the bucket the", "the bucket if the")
                        question["question"] = question_text

                if question["template_id"] in ["prevent_0", "prevent_1", "prevent_2"]:
                    question_text: str = question["question"]
                    if question_text.startswith("Is"):
                        question_text = question_text.replace("is prevented by", "prevented by")
                        question_text = question_text.replace("is kept by", "kept by")
                        question_text = question_text.replace("is held by", "held by")
                        question_text = question_text.replace("is blocked by", "blocked by")
                        question["question"] = question_text

                new_questions_list.append(question)

            question_list[:] = new_questions_list

            logger.info(f"Processed: {i}/{len(dataset_obj.video_index_to_questions_map.keys())}")

        # Continue postprocessing before balancing here

        self.__rewrite_dataset()

    def __rewrite_dataset(self):
        with open(f"{self.__dataset_obj.dataset_folder_path}/dataset_minimal.json", "w") as minimal_dataset_file:
            minimal_dataset_file.write("[")

            logger.info(f"Rewriting preprocessed minimal dataset...")
            video_indices = sorted(self.__dataset_obj.video_index_to_questions_map.keys())
            for i, instance_id in enumerate(video_indices):
                question_list = self.__dataset_obj.video_index_to_questions_map[instance_id]

                for j, question in enumerate(question_list):
                    minimal_dataset_file.write(json.dumps(question))
                    if i == len(video_indices) - 1 and j == len(question_list) - 1:
                        pass
                    else:
                        minimal_dataset_file.write(",")

            minimal_dataset_file.write("]")

            logger.info(f"Successfully rewritten to: {self.__dataset_obj.dataset_folder_path}")

    def cleanup(self):
        logger.info(f"Re-reading preprocessed minimal dataset...")
        self.__dataset_obj = CRAFTDataset(self.__dataset_obj.dataset_folder_path, self.__dataset_obj.metadata)

    def get_output(self):
        return self.__dataset_obj


class BalancingStage(Stage):

    def __init__(self, purge_single_answers=False):
        """
        In this stage, questions in a tuple (template_id, simulation_id) are balanced according to the answers.
        :param purge_single_answers: Removes all questions if there are only one answer for that pair.
        """
        super().__init__(name="Balancing Stage")
        self.purge = purge_single_answers
        self.balanced_dataset = None

    def process(self, dataset_obj: CRAFTDataset):
        logger.info("Initiating dataset balancing stage...")

        dataset_obj.generate_statistics(output_folder=f"{dataset_obj.dataset_folder_path}/stats/imbalanced")

        logger.info(f"Performing various under-sampling operations on dataset...")
        balanced_dataset_output_path = f"{dataset_obj.dataset_folder_path}/balanced_dataset.json"
        DatasetUnderSampler(dataset_obj, balanced_dataset_output_path) \
            .balance_answers_within_each_template_and_simulation_ids(self.purge) \
            .dump()
        logger.info(f"Copying imbalanced dataset to its file")
        FileIO.copy(f"{dataset_obj.dataset_folder_path}/dataset_minimal.json",
                    f"{dataset_obj.dataset_folder_path}/imbalanced_dataset.json")
        logger.info(f"Copying balanced dataset to original file")
        FileIO.copy(f"{dataset_obj.dataset_folder_path}/balanced_dataset.json",
                    f"{dataset_obj.dataset_folder_path}/dataset_minimal.json")

        self.balanced_dataset = CRAFTDataset(dataset_obj.dataset_folder_path, dataset_obj.metadata)

        self.balanced_dataset.generate_statistics(output_folder=f"{dataset_obj.dataset_folder_path}/stats/balanced")

    def cleanup(self):
        pass

    def get_output(self):
        return self.balanced_dataset


class ExportDatasetStatistics(Stage):
    def __init__(self, output_folder_path):
        super().__init__(name="Export Dataset Statistics Stage")
        self.dataset_obj = None
        self.output_folder_path = output_folder_path

    def process(self, dataset_obj: CRAFTDataset):
        self.dataset_obj = dataset_obj

        self.dataset_obj.generate_statistics(f"{dataset_obj.dataset_folder_path}/{self.output_folder_path}")

    def get_output(self):
        return self.dataset_obj


class DescriptiveBalanceStage(Stage):

    def __init__(self, percentage):
        super().__init__(name="Descriptive Balance Stage")
        self.balanced_dataset = None
        self.percentage = percentage

    def __compute_dist(self, qtype_q):
        N = len([q for qs in qtype_q.values() for q in qs])

        qtype_dist = {}
        qtype_len = {}

        for qtype in qtype_q:
            qtype_len[qtype] = len(qtype_q[qtype])
            qtype_dist[qtype] = len(qtype_q[qtype]) / N

        logger.info(f"Question type distribution: {str(qtype_dist)}")

        logger.info(f"Number of questions by question type: {str(qtype_len)}")

        return qtype_len, qtype_dist

    def process(self, dataset_obj: CRAFTDataset):
        logger.info("Balancing descriptive questions...")
        rnd = Random(42)

        qtype_q = dataset_obj.build_question_type_question_map()

        qtype_len, qtype_dist = self.__compute_dist(qtype_q)

        N_to_discard = round(qtype_len["Descriptive"] * self.percentage)

        logger.info(f"Discarding {N_to_discard} descriptive questions")

        rnd.shuffle(qtype_q["Descriptive"])

        while N_to_discard > 0:
            q = qtype_q["Descriptive"].pop(0)
            if len(dataset_obj.video_index_to_questions_map[q["video_index"]]) <= 1:
                qtype_q["Descriptive"].append(q)
                continue
            N_to_discard -= 1

        logger.info(f"{qtype_len['Descriptive'] - len(qtype_q['Descriptive'])} descriptive questions are discarded.")

        balanced_questions = []
        for qs in qtype_q.values():
            balanced_questions.extend(qs)

        logger.info(f"Sorting...")
        balanced_questions.sort(key=lambda q: (q["video_index"], q["question_index"]))

        dataset_obj.questions = balanced_questions

        self.__compute_dist(dataset_obj.build_question_type_question_map())

        self.balanced_dataset = dataset_obj
        self.balanced_dataset.prepare_auxiliaries()

        self.__write_dataset()

    def __write_dataset(self):
        with open(f"{self.balanced_dataset.dataset_folder_path}/dataset_minimal_descriptive_reduced.json",
                  "w") as minimal_dataset_file:
            minimal_dataset_file.write("[")

            logger.info(f"Writing descriptive-reduced minimal dataset...")
            video_indices = sorted(self.balanced_dataset.video_index_to_questions_map.keys())
            for i, instance_id in enumerate(video_indices):
                question_list = self.balanced_dataset.video_index_to_questions_map[instance_id]

                for j, question in enumerate(question_list):
                    minimal_dataset_file.write(json.dumps(question))
                    if i == len(video_indices) - 1 and j == len(question_list) - 1:
                        pass
                    else:
                        minimal_dataset_file.write(",")

            minimal_dataset_file.write("]")

            logger.info(f"Successfully written to: {self.balanced_dataset.dataset_folder_path}")

    def cleanup(self):
        self.balanced_dataset.generate_statistics(
            output_folder=f"{self.balanced_dataset.dataset_folder_path}/stats/descriptive_reduced")

    def get_output(self):
        return self.balanced_dataset


class DatasetSplitStage(Stage):

    def __init__(self, config, seed=10435):
        super().__init__(name="Dataset Split Stage", )
        self.__dataset_obj: CRAFTDataset = None
        self.config = config
        self.seed = seed

    def process(self, dataset_obj: CRAFTDataset):
        logger.info("Initiating dataset splitting stage...")
        rnd = Random(self.seed)

        self.__dataset_obj = dataset_obj

        splits = defaultdict(list)
        vi_qi_to_split = {}

        if self.config == "hard":
            split_sizes = {"train": 12, "validation": 4, "test": 4}

            # Similar scene types
            counterparts = {1: 18, 3: 16, 4: 17}

            sids = list(range(1, 21))

            chosen = {"test": [], "validation": [], "train": []}

            # Bogo method. The best. I've spend a lot of time until I reached this ultimate conclusion.
            while True:
                rnd.shuffle(sids)
                chosen["train"] = sids[:split_sizes["train"]]
                chosen["validation"] = sids[split_sizes["train"]:split_sizes["train"] + split_sizes["validation"]]
                chosen["test"] = sids[split_sizes["train"] + split_sizes["validation"]:sum(split_sizes.values())]

                ok = True
                for split, ss in chosen.items():
                    for s in ss:
                        if s in counterparts and counterparts[s] in ss:
                            ok = False
                            break
                        if not ok:
                            break
                if ok:
                    break

            counts = defaultdict(int)
            for question in dataset_obj.questions:
                if int(question["simulation_id"]) in chosen["train"]:
                    counts["train"] += 1
                if int(question["simulation_id"]) in chosen["validation"]:
                    counts["validation"] += 1
                if int(question["simulation_id"]) in chosen["test"]:
                    counts["test"] += 1

            logger.info(f"Splits: {json.dumps(chosen)}")

            logger.info(f"Number of questions for each split: {json.dumps(dict(counts))}")

            sid_to_split = {}
            for split, sids in chosen.items():
                for sid in sids:
                    sid_to_split[sid] = split

            for question in dataset_obj.questions:
                sid = int(question["simulation_id"])
                splits[sid_to_split[sid]].append({
                    "video_index": question["video_index"],
                    "question_index": question["question_index"]
                })
                vi_qi_to_split[(question["video_index"], question["question_index"])] = sid_to_split[sid]

        elif self.config == "random":

            video_indices = sorted(self.__dataset_obj.video_index_to_questions_map.keys())
            N = len(video_indices)
            test_count = int(N * 0.2)
            val_count = int(N * 0.2)
            train_count = int(N * 0.6)
            train_count += N - test_count - val_count - train_count
            rnd.shuffle(video_indices)

            train = video_indices[:train_count]
            val = video_indices[train_count:train_count + val_count]
            test = video_indices[train_count + val_count:N]
            assert len(train) + len(val) + len(test) == N

            for video_index in train:
                questions = self.__dataset_obj.video_index_to_questions_map[video_index]
                for question in questions:
                    splits["train"].append({
                        "video_index": question["video_index"],
                        "question_index": question["question_index"]
                    })
            for video_index in val:
                questions = self.__dataset_obj.video_index_to_questions_map[video_index]
                for question in questions:
                    splits["validation"].append({
                        "video_index": question["video_index"],
                        "question_index": question["question_index"]
                    })
            for video_index in test:
                questions = self.__dataset_obj.video_index_to_questions_map[video_index]
                for question in questions:
                    splits["test"].append({
                        "video_index": question["video_index"],
                        "question_index": question["question_index"]
                    })

            split_to_vid = defaultdict(set)

            for s in splits:
                for pair in splits[s]:
                    split_to_vid[s].add(pair["video_index"])

            print(f"Stats for {self.config}:")
            for s in split_to_vid:
                print(s, len(split_to_vid[s]))

            logger.info(f"Number of questions for each split: {json.dumps({k: len(v) for k, v in splits.items()})}")

        FileIO.write_json(dict(splits), f"{dataset_obj.dataset_folder_path}/split_info_{self.config}.json")

    def get_output(self):
        return self.__dataset_obj


class FullDatasetWriteStage(Stage):

    def __init__(self, output_file_name):
        super().__init__(name="Full Dataset Write Stage", )
        self.__dataset_obj: CRAFTDataset = None
        self.output_file_name = output_file_name

    def process(self, dataset_obj: CRAFTDataset):
        logger.info("Write full dataset...")

        self.__dataset_obj = dataset_obj

        with open(f"{self.__dataset_obj.dataset_folder_path}/{self.output_file_name}", "w") as full_dataset_file:
            full_dataset_file.write("[")
            instance_ids = sorted(list(dataset_obj.video_index_to_questions_map.keys()))
            i = 0
            for instance_id in instance_ids:
                question_list = dataset_obj.video_index_to_questions_map[instance_id]
                sid = int(question_list[0]["simulation_id"])

                questions_file_path = self.__dataset_obj.get_questions_output_path(sid, instance_id)

                try:
                    # Add them into dataset.json
                    with open(questions_file_path, "r") as questions_file:
                        qa_json = json.load(questions_file)

                        filtered_questions = []
                        for q in qa_json["questions"]:
                            ok_questions = self.__dataset_obj.get_questions_for_video(q["video_index"])
                            for ok_q in ok_questions:
                                if ok_q["question_index"] == q["question_index"]:
                                    q["question"] = ok_q["question"]  # If post-processed.
                                    filtered_questions.append(q)
                                    break

                        qa_json["questions"][:] = filtered_questions

                        simulation_instance = json.loads(f"""{{
                                    "simulation_id": "{sid}",
                                    "video_path": "{self.__dataset_obj.get_video_output_path(sid, instance_id)}",
                                    "questions": {json.dumps(qa_json)}
                                 }}""")
                        if instance_id % 10 == 0:
                            logger.info(f"Writing: {instance_id}/{len(instance_ids)}")
                        simulation_instance = \
                            DatasetUtils.relativize_paths([simulation_instance],
                                                          self.__dataset_obj.dataset_folder_path)[0]
                        full_dataset_file.write(json.dumps(simulation_instance))
                        if i != len(instance_ids) - 1:
                            full_dataset_file.write(",")
                        else:
                            full_dataset_file.write("]")
                except FileNotFoundError:
                    logger.warning(f"{instance_id:06d}: Questions file cannot be found")
                i += 1

    def get_output(self):
        return self.__dataset_obj


class AnnotationsFileCollector(Stage):
    def __init__(self, output_file_name):
        super().__init__(name="Annotations File Collector Stage")
        self.__dataset_obj: CRAFTDataset = None
        self.output_file_name = output_file_name

    def process(self, dataset_obj: CRAFTDataset):
        logger.info("Collecting annotations...")
        self.__dataset_obj = dataset_obj
        with open(f"{dataset_obj.dataset_folder_path}/{self.output_file_name}", "w") as annotations_file:
            annotations_file.write("{")
            instance_ids = sorted(list(dataset_obj.video_index_to_questions_map.keys()))
            for i, instance_id in enumerate(instance_ids):
                sid = int(dataset_obj.video_index_to_questions_map[instance_id][0]["simulation_id"])
                annotations_file_path = dataset_obj.get_simulation_with_variations_output_path(sid, instance_id)
                with open(annotations_file_path) as this_annotations_file:
                    annotations = json.dumps(json.load(this_annotations_file))
                    # Relativize paths
                    annotations = annotations.replace(Path(dataset_obj.dataset_folder_path).resolve().as_posix(), ".")
                    annotations_file.write(f"""
                            "{instance_id:06d}": {annotations}
                    """)
                    if i != len(instance_ids) - 1:
                        annotations_file.write(",")
                    if i % 10 == 0:
                        logger.info(f"Collecting annotations: {i}/{len(instance_ids)}")
            annotations_file.write("}")

    def get_output(self):
        return self.__dataset_obj


class CleanupStage(Stage):
    def __init__(self):
        super().__init__(name="Cleanup Stage")
        self.__dataset_obj: CRAFTDataset = None

    def process(self, dataset_obj: CRAFTDataset):
        self.__dataset_obj = dataset_obj
        videos_with_no_questions = []
        ground = list(range(0, 10000))
        for idx in ground:
            if idx not in dataset_obj.video_index_to_questions_map:
                videos_with_no_questions.append(idx)

        with open(f"{dataset_obj.dataset_folder_path}/videos_with_no_questions.json", "w") as vwnq_file:
            json.dump(videos_with_no_questions, vwnq_file)

        os.makedirs(f"{dataset_obj.dataset_folder_path}/videos_with_no_questions", exist_ok=True)

        # Move videos without questions to a separate folder.
        for idx in videos_with_no_questions:
            vid_path = glob.glob(f"{dataset_obj.dataset_folder_path}/videos/**/{idx:06d}.mpg")[0]
            dest = vid_path.replace("videos", "videos_with_no_questions")
            os.makedirs(Path(dest).parent.as_posix(), exist_ok=True)
            FileIO.move(vid_path, dest)

    def get_output(self):
        return self.__dataset_obj


class PostProcessStage(Stage):

    def __init__(self):
        super().__init__(name="Post-Process Stage")
        self.__dataset_obj: CRAFTDataset = None

    def process(self, dataset_obj: CRAFTDataset):
        logger.info("Initiating post process stage...")

        self.__dataset_obj = dataset_obj

        for instance_id in dataset_obj.video_index_to_questions_map.keys():
            question_list = dataset_obj.video_index_to_questions_map[instance_id]

            for question in question_list:
                # Correct typos in the question templates. These are also corrected in the question template
                # files in svqa/SVQA_1.0_templates, so the following can be deleted.
                if question["template_id"] == "counterfactual_2":
                    question_text: str = question["question"]
                    if question_text.startswith("Will"):
                        question_text = question_text.replace("the basket the", "the basket if the")
                        question_text = question_text.replace("the container the", "the container if the")
                        question_text = question_text.replace("the bucket the", "the bucket if the")
                        question["question"] = question_text
                if question["template_id"] in ["prevent_0", "prevent_1", "prevent_2"]:
                    question_text: str = question["question"]
                    if question_text.startswith("Is"):
                        question_text = question_text.replace("is prevented by", "prevented by")
                        question_text = question_text.replace("is kept by", "kept by")
                        question_text = question_text.replace("is held by", "held by")
                        question_text = question_text.replace("is blocked by", "blocked by")
                        question["question"] = question_text

            logger.info(f"Processed: {instance_id}/{len(dataset_obj.video_index_to_questions_map.keys())}")

        self.__rewrite_dataset()

    def __rewrite_dataset(self):
        with open(f"{self.__dataset_obj.dataset_folder_path}/dataset_minimal.json", "w") as minimal_dataset_file:
            minimal_dataset_file.write("[")

            logger.info(f"Rewriting preprocessed minimal dataset...")
            video_indices = sorted(self.__dataset_obj.video_index_to_questions_map.keys())
            for i, instance_id in enumerate(video_indices):
                question_list = self.__dataset_obj.video_index_to_questions_map[instance_id]

                for j, question in enumerate(question_list):
                    minimal_dataset_file.write(json.dumps(question))
                    if i == len(video_indices) - 1 and j == len(question_list) - 1:
                        pass
                    else:
                        minimal_dataset_file.write(",")

            minimal_dataset_file.write("]")

            logger.info(f"Successfully rewritten to: {self.__dataset_obj.dataset_folder_path}")

    def cleanup(self):
        logger.info(f"Re-reading post-processed minimal dataset...")
        self.__dataset_obj = CRAFTDataset(self.__dataset_obj.dataset_folder_path, self.__dataset_obj.metadata)

    def get_output(self):
        return self.__dataset_obj


class CollectUnperturbedDataset(Stage):

    def __init__(self, output_folder_path: str):
        super().__init__(name="Collect Unperturbed Dataset Stage")
        self.__dataset_obj: CRAFTDataset = None
        self.output_folder_path = output_folder_path

    def process(self, dataset_obj: CRAFTDataset):

        self.__dataset_obj = dataset_obj

        original_questions = []

        instance_ids = sorted(list(dataset_obj.video_index_to_questions_map.keys()))
        for i, instance_id in enumerate(instance_ids):
            sid = int(dataset_obj.video_index_to_questions_map[instance_id][0]["simulation_id"])
            original_qa_json = FileIO.read_json(self.__dataset_obj.get_original_questions_path(sid, instance_id))
            for qa in original_qa_json["questions"]:
                original_questions.append(dataset_obj.get_question_from_question_obj(qa, sid))

            logger.info(f"Processed: {instance_id}/{len(instance_ids)}")

        os.makedirs(self.output_folder_path, exist_ok=True)

        FileIO.write_json(original_questions, f"{self.output_folder_path}/dataset_minimal.json")

        self.__dataset_obj = CRAFTDataset(self.output_folder_path, self.__dataset_obj.metadata)

    def get_output(self):
        return self.__dataset_obj
