import copy
import json
import os
import time
import traceback

import pandas as pd
import numpy as np
from deepdiff import DeepDiff
from loguru import logger

from framework.dataset import DatasetStatistics, CRAFTDataset, DatasetUtils, DatasetGenerationConfig
from framework.simulation import SimulationInstance, SimulationRunner
from framework.utils import Funnel, FileIO


class DatasetUnderSampler:
    def __init__(self, dataset: CRAFTDataset, output_file_path):
        self.__dataset_copy = copy.deepcopy(dataset)
        self.questions = self.__dataset_copy.questions
        self.output_file_path = output_file_path

    def get_unique_values(self, column: str) -> set:
        return set(pd.DataFrame(self.questions)[column].to_list())

    @staticmethod
    def answer_discard_strategy(class_name: str, val):
        if class_name == "answer":
            if val.isnumeric():
                return int(val) in range(3, 11)
        return False

    def balance_answers_within_answer_types(self):
        questions = []

        answer_types = self.get_unique_values("answer_type")
        for answer_type in answer_types:
            questions_with_this_answer_type = Funnel(self.questions) \
                .filter(lambda x: x["answer_type"] == answer_type) \
                .get_result()
            questions.extend(DatasetUtils.imblearn_random_undersampling(questions_with_this_answer_type, "answer",
                                                                        discard_strategy_fn=DatasetUnderSampler.answer_discard_strategy))

        self.questions = questions
        return self

    def balance_answers_within_each_simulation_id(self):
        questions = []
        answer_types = self.get_unique_values("answer_type")
        sim_ids = self.get_unique_values("simulation_id")
        for answer_type in answer_types:
            questions_with_this_answer_type = Funnel(self.questions) \
                .filter(lambda x: x["answer_type"] == answer_type) \
                .get_result()
            for sid in sim_ids:
                questions_with_this_simulation_id = Funnel(questions_with_this_answer_type) \
                    .filter(lambda x: x["simulation_id"] == sid) \
                    .get_result()
                questions.extend(DatasetUtils.imblearn_random_undersampling(questions_with_this_simulation_id, "answer",
                                                                            discard_strategy_fn=DatasetUnderSampler.answer_discard_strategy))
        self.questions = questions
        return self

    def balance_answers_within_each_template_and_simulation_ids(self, purge_single_answers):
        questions = []
        sim_ids = self.get_unique_values("simulation_id")
        template_ids = self.get_unique_values("template_id")
        for sid in sim_ids:
            for template_id in template_ids:
                questions_with_this_template_id = Funnel(self.questions) \
                    .filter(lambda x: x["template_id"] == template_id and x["simulation_id"] == sid) \
                    .get_result()
                undersampled = DatasetUtils.imblearn_random_undersampling(questions_with_this_template_id, "answer",
                                                                          discard_strategy_fn=DatasetUnderSampler.answer_discard_strategy,
                                                                          purge_single_answers=purge_single_answers)
                questions.extend(undersampled)

        self.questions = questions
        return self

    def balance_answers_within_each_template_id(self):
        questions = []

        template_ids = self.get_unique_values("template_id")
        for template_id in template_ids:
            questions_with_this_template_id = Funnel(self.questions) \
                .filter(lambda x: x["template_id"] == template_id) \
                .get_result()
            questions.extend(DatasetUtils.imblearn_random_undersampling(questions_with_this_template_id, "answer",
                                                                        discard_strategy_fn=DatasetUnderSampler.answer_discard_strategy))

        self.questions = questions
        return self

    def balance_template_ids_within_each_simulation_id(self):
        questions = []

        simulation_ids = self.get_unique_values("simulation_id")
        for sid in simulation_ids:
            questions_with_this_sid = Funnel(self.questions) \
                .filter(lambda x: x["simulation_id"] == sid) \
                .get_result()
            questions.extend(DatasetUtils.imblearn_random_undersampling(questions_with_this_sid, "template_id"))

        self.questions = questions
        return self

    def get_result(self) -> list:
        return self.questions

    def dump(self):
        with open(self.output_file_path, "w") as minimal_balanced_dataset_file:
            minimal_balanced_dataset_file.write("[")
            N = len(self.questions)
            for i, question in enumerate(self.questions):
                minimal_balanced_dataset_file.write(json.dumps(question))
                if i != N - 1:
                    minimal_balanced_dataset_file.write(",")

            minimal_balanced_dataset_file.write("]")

            logger.info(f"Balanced version successfully written to: {self.output_file_path}")
        return


class DatasetInspector:
    """
    Deprecated
    """

    def __init__(self, stats: DatasetStatistics):
        self.stats = stats
        self.dataset = stats.dataset

    def compute_answers_needed_for_tid_and_sid_versus_answer_balance(self) -> dict:
        unique_answers = self.dataset.get_unique_values("answer")
        # TODO: Include answers with 0 frequency...
        unique_answers = [answer for answer in unique_answers if answer not in [str(i) for i in range(3, 50)]]
        dict_of_needed_answers = {}

        for row in self.stats.answer_freq_per_tid_and_sid:
            simulation_id = row["simulation_id"]
            template_id = row["template_id"]
            answer_type = row["answer_type"]
            if simulation_id not in dict_of_needed_answers:
                dict_of_needed_answers[simulation_id] = {}
            if template_id not in dict_of_needed_answers[simulation_id]:
                dict_of_needed_answers[simulation_id][template_id] = {}
            if answer_type not in dict_of_needed_answers:
                dict_of_needed_answers[simulation_id][template_id][answer_type] = {}

            answers = Funnel(list(self.stats.answer_freq_per_tid_and_sid)) \
                .filter(lambda x: x["template_id"] == template_id) \
                .filter(lambda x: x["simulation_id"] == simulation_id) \
                .filter(lambda x: x["answer_type"] == answer_type) \
                .get_result()

            answer_with_max_count = max(answers, key=lambda x: x["count"])

            for answer_obj in answers:
                answer = answer_obj["answer"]
                if answer_with_max_count["answer"] != answer:
                    dict_of_needed_answers[simulation_id][template_id][answer_type][answer] \
                        = answer_with_max_count["count"] - answer_obj["count"]

            for answer in unique_answers:
                if self.dataset.get_answer_type_for_answer(answer) == answer_type:
                    if answer not in dict_of_needed_answers[simulation_id][template_id][answer_type].keys():
                        if answer != answer_with_max_count["answer"]:
                            dict_of_needed_answers[simulation_id][template_id][answer_type][answer] = \
                                answer_with_max_count["count"]

        return dict_of_needed_answers


class DatasetBalancer:
    """
    Balances the dataset by generating additional videos and questions.
    Deprecated
    """

    def __init__(self, config: DatasetGenerationConfig, answers_needed: dict, video_index: int):
        self.config = config
        os.makedirs(f"{config.output_folder_path}/temp", exist_ok=True)
        os.makedirs(f"{config.output_folder_path}/temp/to_exclude/", exist_ok=True)
        self.__answers_needed = answers_needed
        self.__sid_to_simulation_configs = {simulation_config["id"]: simulation_config for simulation_config in
                                            config.simulation_configs}
        self.__runner = SimulationRunner(self.config.executable_path)
        self.__video_index = video_index

        self.__start_time = None
        self.__times = np.array([])

    def get_temp_controller_path(self, sid: int, instance_id: int):
        return f"{self.config.output_folder_path}/temp/controller_{instance_id:06d}.json"

    def get_temp_questions_output_path(self, sid: int, instance_id: int):
        return f"{self.config.output_folder_path}/temp/sid_{sid}_qa_{instance_id:06d}.json"

    def get_temp_simulation_with_variations_output_path(self, sid: int, instance_id: int):
        return f"{self.config.output_folder_path}/temp/{instance_id:06d}.json"

    def get_temp_video_output_path(self, sid: int, instance_id: int):
        return f"{self.config.output_folder_path}/temp/{sid}_{instance_id:06d}.mpg"

    def get_temp_bare_simulation_output_path(self, sid: int, instance_id: int):
        return f"{self.config.output_folder_path}/temp/bare_{instance_id:06d}.json"

    def get_temp_debug_output_path(self, sid: int, instance_id: int):
        return f"{self.config.output_folder_path}/temp/cl_debug_{instance_id:06d}.txt"

    def get_left_out_needed_answers_path(self, sid: int):
        return f"{self.config.output_folder_path}/temp/to_exclude/left_out_answers_for_sid_{sid}.json"

    def get_temp_varations_debug_output_path(self, sid: int, instance_id: int):
        return f"{self.config.output_folder_path}/temp/cl_debug_var_{instance_id:06d}.txt"

    def get_balancing_state_file_path(self):
        return f"{self.config.output_folder_path}/temp/balancing_state_file.json"

    def dump_controller_file(self, instance_id: int, simulation_config: dict) -> str:
        sid = simulation_config["id"]

        controller_file_path = self.get_temp_controller_path(sid, instance_id)

        with open(controller_file_path, 'w') as controller_file:
            json.dump(
                json.loads(
                    f"""{{
                            "simulationID": {sid},
                            "offline": {str(self.config.offline).lower()},
                            "outputVideoPath": "{self.get_temp_video_output_path(sid, instance_id)}",
                            "outputJSONPath": "{self.get_temp_bare_simulation_output_path(sid, instance_id)}",
                            "width":  {simulation_config['width']},
                            "height": {simulation_config['height']},
                            "inputScenePath":  "",
                            "stepCount": {simulation_config['step_count']}
                        }}"""),
                controller_file,
                indent=2
            )

        return controller_file_path

    def generate_new_sample(self, sid: int, instance_id: int) -> str:
        simulation_config = self.__sid_to_simulation_configs[sid]

        # Create controller file for current simulation instance.
        controller_file_path = self.dump_controller_file(instance_id, simulation_config)

        variations_output_path = self.get_temp_simulation_with_variations_output_path(sid, instance_id)

        questions_file_path = self.get_temp_questions_output_path(sid, instance_id)

        simulation = SimulationInstance(instance_id,
                                        controller_file_path,
                                        variations_output_path,
                                        questions_file_path,
                                        self.__runner)

        # Run simulation.
        simulation.run_simulation(self.get_temp_debug_output_path(sid, instance_id))

        # Run its variations.
        simulation.run_variations()

        # Generate questions.
        try:
            simulation.generate_questions(simulation_config)
        except Exception as e:
            traceback.print_exception(type(e), e, e.__traceback__)
            logger.error(e)

        return questions_file_path

    def filter_answers_according_to_needs(self, qa_json: dict, simulation_id: str,
                                          answers_needed: dict) -> dict:
        acceptable_answers = list(range(3, 50))

        new_qa_json = {"info": qa_json["info"], "questions": []}
        for question in qa_json["questions"]:
            tid = f"{question['template_filename'].split('.')[0]}_{question['question_family_index']}"
            if tid in answers_needed[simulation_id]:
                answers = list(answers_needed[simulation_id][tid].values())[0]
                if str(question["answer"]) in answers.keys():
                    if answers[str(question["answer"])] > 0:
                        answers[str(question["answer"])] -= 1
                        new_qa_json["questions"].append(question)
                elif question["answer"] in acceptable_answers:
                    new_qa_json["questions"].append(question)

        return {"new_needed_answers": answers_needed, "filtered_questions": new_qa_json}

    @staticmethod
    def number_of_qa_needed_for_sid(sid: str, answers_needed: dict) -> int:
        count = 0
        for tid in answers_needed[sid]:
            answers = list(answers_needed[sid][tid].values())[0]
            count += sum(list(answers.values()))
        return count

    @staticmethod
    def list_of_tids_needed(sid: str, answers_needed: dict) -> list:
        ret = []
        for tid in answers_needed[sid]:
            answers = list(answers_needed[sid][tid].values())[0]
            if sum(list(answers.values())) > 0:
                ret.append(tid)
        return ret

    @staticmethod
    def total_number_of_qas_needed(answers_needed: dict) -> int:
        count = 0
        for sid in answers_needed:
            count += DatasetBalancer.number_of_qa_needed_for_sid(sid, answers_needed)
        return count

    def get_state(self):
        if os.path.exists(self.get_balancing_state_file_path()):
            return FileIO.read_json(self.get_balancing_state_file_path())
        else:
            return None

    def __save_state(self, state_obj: dict):
        FileIO.write_json(state_obj, self.get_balancing_state_file_path())

    def start_balancing(self, video_generation_max_try: int = 30):
        logger.info("Starting to balance the dataset by appending new items...")
        self.__start_time = time.time()

        answers_needed = self.__answers_needed
        state_json = self.get_state()
        if state_json is not None:
            logger.info(f"Previous unfinished balancing state found. Continuing...")
            answers_needed = state_json["answers_needed"]
            self.__video_index = state_json["current_video_index"]

        answers_needed = json.loads(json.dumps(answers_needed, sort_keys=True))

        total_qas_needed = DatasetBalancer.total_number_of_qas_needed(answers_needed)
        total_number_of_qas_generated = 0
        logger.info(f"There are {total_qas_needed} questions with specific answers needed for total balance")

        prev_answers_needed = copy.deepcopy(answers_needed)

        for sid in answers_needed:
            number_of_video_tries = 0
            sid = int(sid)

            while True:
                t1 = time.time()  # To measure remaining time.

                logger.info(f"For the scene type with SID {sid}, there are currently "
                            f"{DatasetBalancer.number_of_qa_needed_for_sid(str(sid), prev_answers_needed)} Q-As"
                            f" with TIDs {DatasetBalancer.list_of_tids_needed(str(sid), answers_needed)} needed")
                instance_id: int = self.__video_index
                simulation_config = self.__sid_to_simulation_configs[sid]

                # Create controller file for current simulation instance.
                controller_file_path = self.dump_controller_file(instance_id, simulation_config)

                variations_output_path = self.get_temp_simulation_with_variations_output_path(sid, instance_id)

                questions_file_path = self.get_temp_questions_output_path(sid, instance_id)

                simulation = SimulationInstance(instance_id,
                                                controller_file_path,
                                                variations_output_path,
                                                questions_file_path,
                                                self.__runner)

                # Run simulation.
                logger.info(f"Running simulation with SID: {sid}, index: {instance_id}")
                simulation.run_simulation(self.get_temp_debug_output_path(sid, instance_id))

                # Run its variations.
                logger.info(f"Running its variations")
                simulation.run_variations(self.get_temp_varations_debug_output_path(sid, instance_id))

                # Generate questions.
                logger.info(f"Generating questions")
                simulation.generate_questions(simulation_config)

                with open(questions_file_path) as qa_json_file:
                    qa_json = json.load(qa_json_file)
                logger.info(f"{len(qa_json['questions'])} number of Q-As generated")

                logger.info(f"Filtering questions according to needs...")
                output = self.filter_answers_according_to_needs(qa_json,
                                                                str(sid),
                                                                copy.deepcopy(prev_answers_needed))

                new_answers_needed = output["new_needed_answers"]
                filtered_questions = output["filtered_questions"]

                FileIO.write_json(filtered_questions, questions_file_path)

                self.__save_state({"answers_needed": new_answers_needed, "current_video_index": self.__video_index})

                prev_number_of_answers_needed = DatasetBalancer.number_of_qa_needed_for_sid(str(sid),
                                                                                            prev_answers_needed)
                curr_number_of_answers_needed = DatasetBalancer.number_of_qa_needed_for_sid(str(sid),
                                                                                            new_answers_needed)
                obtained_number_of_questions = prev_number_of_answers_needed - curr_number_of_answers_needed
                logger.info(f"{obtained_number_of_questions} "
                            f"questions with needed answers have been obtained")

                total_number_of_qas_generated += obtained_number_of_questions
                progress = total_number_of_qas_generated / total_qas_needed
                logger.info(
                    f"Current progress: {total_number_of_qas_generated}/{total_qas_needed} ({round(progress * 100, 3)})")

                if obtained_number_of_questions != 0:
                    self.__update_clock(t1 / obtained_number_of_questions, total_qas_needed,
                                        total_number_of_qas_generated)

                diff: DeepDiff = DeepDiff(new_answers_needed, prev_answers_needed, ignore_order=True)

                if prev_answers_needed == new_answers_needed:
                    number_of_video_tries += 1
                    logger.info(
                        f"Cannot reduce number of Q-As needed, retrying {video_generation_max_try - number_of_video_tries} times before giving up...")
                else:
                    logger.info(f"Was able to reduce the answer imbalance for SID: {sid}")
                    self.__increment_video_index()
                    number_of_video_tries = 0
                    if curr_number_of_answers_needed == 0:
                        logger.info(
                            f"We have got total balance for scene type with SID {sid}! It's a miracle, no, science!")
                        break

                prev_answers_needed = copy.deepcopy(new_answers_needed)

                if number_of_video_tries >= video_generation_max_try:
                    logger.info(f"Maximum number of video generation attempts reached for this SID: {sid}")
                    logger.info(
                        f"There are still {DatasetBalancer.number_of_qa_needed_for_sid(str(sid), new_answers_needed)} "
                        f"Q-As with TIDs {DatasetBalancer.list_of_tids_needed(str(sid), new_answers_needed)} needed for SID: {sid}")
                    FileIO.write_json(new_answers_needed, self.get_left_out_needed_answers_path(sid))
                    break

        logger.info(
            f"Dataset balancing process is complete. Process took {round((time.time() - self.__start_time) / 60, 2)} minutes.")

    def __increment_video_index(self):
        self.__video_index += 1
        logger.info(f"Increment last video index to {self.__video_index}")

    def __update_clock(self, t1, total_runs: int, current: int):
        diff = time.time() - t1
        times = np.append(self.__times, diff)
        logger.info(f"Approximately {round((np.mean(times) * (total_runs - current - 1)) / 60, 2)} "
                    "minutes remaining if we keep up with the current pace...".ljust(75, " "))
