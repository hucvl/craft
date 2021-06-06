import copy
import json
import os
import subprocess
import sys
from pathlib import Path

from loguru import logger

from framework.utils import FileIO
from svqa.causal_graph import CausalGraph
import svqa.generate_questions as QuestionGeneratorScript


class SimulationRunner(object):

    def __init__(self, exec_path: str, working_directory: str = None):
        self.exec_path = exec_path
        self.working_directory = working_directory if working_directory is not None \
            else Path(exec_path).parents[4].joinpath("Testbed").absolute().as_posix()

    def run_simulation(self, controller_json_path: str, debug_output_path=None):
        subprocess.call(f"{self.exec_path} {controller_json_path}",
                        shell=True,
                        universal_newlines=True,
                        cwd=self.working_directory,
                        stdout=open(os.devnull, 'wb') if debug_output_path is None else open(debug_output_path, "w"))

    def run_variations(self, controller_json_path: str, variations_output_path: str, debug_output_path=None):
        variation_runner = VariationRunner(self)
        variation_runner.run_variations(controller_json_path, variations_output_path, debug_output_path)


class SimulationInstance:

    def __init__(self, instance_id: int,
                 controller_json_path: str,
                 variations_output_path: str,
                 questions_file_path: str,
                 runner: SimulationRunner):
        self.__runner = runner
        self.__controller_json_path = controller_json_path
        self.__variations_output_path = variations_output_path
        self.__questions_file_path = questions_file_path
        self.instance_id = instance_id

    def run_simulation(self, debug_output_path=None):
        self.__runner.run_simulation(self.__controller_json_path, debug_output_path)

    def run_variations(self, debug_output_path=None):
        self.__runner.run_variations(self.__controller_json_path, self.__variations_output_path, debug_output_path)

    def generate_questions(self,
                           simulation_config,
                           output_file_path=None,
                           instances_per_template=1,
                           metadata_file_path: str = '../svqa/metadata.json',
                           synonyms_file_path: str = '../svqa/synonyms.json',
                           templates_dir: str = '../svqa/SVQA_1.0_templates'):
        question_generator = QuestionGenerator(self.__variations_output_path,
                                               self.__questions_file_path if output_file_path is None else output_file_path,
                                               simulation_config,
                                               metadata_file_path=metadata_file_path,
                                               synonyms_file_path=synonyms_file_path,
                                               templates_dir=templates_dir,
                                               instances_per_template=instances_per_template)
        question_generator.execute()


class VariationRunner(object):

    def __init__(self, runner: SimulationRunner):
        self.__runner = runner

    def __new_output_json(self, output: json, i: int):
        ret = copy.deepcopy(output)
        del ret["scene_states"][0]["scene"]["objects"][i]
        del ret["causal_graph"]
        for i in range(len(ret["scene_states"])):
            if ret["scene_states"][i]["step"] != 0:
                del ret["scene_states"][i]
        return ret

    def __create_variations(self, path: str, controller: json, output: json) -> list:
        start_scene_state = output["scene_states"][0]  # best to check step count
        objects = start_scene_state["scene"]["objects"]
        variations = [(objects[i]["uniqueID"], self.__new_output_json(output, i)) for i in range(len(objects)) if
                      objects[i]["bodyType"] != 0]  # 0 for static objects
        controller_paths = []
        for i in range(len(variations)):
            output = variations[i]
            name = f"{os.path.splitext(path)[0]}_var_{output[0]}"
            with open(f"{name}.json", "w") as f:
                json.dump(output[1], f)
            controller_paths.append((output[0], self.__create_controller_variations(controller, name)))

        return controller_paths

    def __create_controller_variations(self, controller: json, name: str) -> str:
        controller = copy.deepcopy(controller)
        controller["outputVideoPath"] = f"{name}_out.mpg"
        controller["outputJSONPath"] = f"{name}_out.json"
        controller["inputScenePath"] = f"{name}.json"

        name = f"{name}_controller.json"
        with open(name, "w") as f:
            json.dump(controller, f)
        return name

    def __get_variation_output(self, controller: str):
        with open(controller) as controller_json_file:
            controller_data = json.load(controller_json_file)
            with open(controller_data["outputJSONPath"]) as output_json_file:
                output_data = json.load(output_json_file)

        return output_data

    def __is_equal_without_step(self, event1, event2):
        return set(event1["objects"]) == set(event2["objects"]) and event1["type"] == event2["type"]

    def __get_different_event_list(self, causal_graph_src: CausalGraph, causal_graph_compare: CausalGraph,
                                   object_props: dict,
                                   discarded_object_id: int):
        src_events = causal_graph_src.events
        compare_events = causal_graph_compare.events

        discarded_shapes = ['platform']
        objects_ids_discarded = [object['uniqueID'] for object in object_props if
                                 object['shape'] in discarded_shapes]

        res = []
        for src_event in src_events:
            objects_of_event = src_event['objects']
            # discard events including object to be discarded
            if discarded_object_id in objects_of_event:
                continue
            found_discarded_shape = False
            for object_of_event in objects_of_event:
                if object_of_event in objects_ids_discarded:
                    found_discarded_shape = True
                    break
            if found_discarded_shape:
                continue

            found_equal = False
            for compare_event in compare_events:
                if self.__is_equal_without_step(src_event, compare_event):
                    found_equal = True
                    break
            if not found_equal:
                res.append(src_event["id"])

        return res

    def __write_enables_prevents(self, output_dict: dict):
        original_causal_graph = CausalGraph(output_dict["original_video_output"]["causal_graph"])
        variation_outputs = output_dict["variations_outputs"]

        output_dict_enables = []
        output_dict_prevents = []
        for removed_object_key in variation_outputs:
            removed_object_id = int(removed_object_key)
            variation_causal_graph = CausalGraph(variation_outputs[removed_object_key]["causal_graph"])
            enables = self.__get_different_event_list(original_causal_graph, variation_causal_graph,
                                                      output_dict['original_video_output']['scene_states'][0]['scene'][
                                                          'objects'],
                                                      removed_object_id)
            prevents = self.__get_different_event_list(variation_causal_graph, original_causal_graph,
                                                       output_dict['original_video_output']['scene_states'][0]['scene'][
                                                           'objects'],
                                                       removed_object_id)

            output_dict_enables.extend([{removed_object_key: enabled_event_id} for enabled_event_id in enables])
            output_dict_prevents.extend([{removed_object_key: prevent_event_id} for prevent_event_id in prevents])

        output_dict["enables"] = output_dict_enables
        output_dict["prevents"] = output_dict_prevents

    def run_variations(self, controller_json_path: str, variations_output_path: str, debug_output_path: str):
        final_output_json = {}

        controller_json = FileIO.read_json(controller_json_path)
        original_output_path: str = controller_json["outputJSONPath"]
        original_output: dict = FileIO.read_json(original_output_path)
        final_output_json["original_video_output"] = original_output
        variation_outputs = {}

        controller_paths = self.__create_variations(original_output_path,
                                                    controller_json,
                                                    original_output)
        for c in controller_paths:
            self.__runner.run_simulation(c[1], debug_output_path)
            variation_outputs[str(c[0])] = self.__get_variation_output(c[1])
        final_output_json["variations_outputs"] = variation_outputs

        self.__write_enables_prevents(final_output_json)

        with open(variations_output_path, "w") as f:
            json.dump(final_output_json, f)


class Perturbator:
    @staticmethod
    def regenerate_answers(original_variations_output_file_path,
                           perturbed_variations_output_path,
                           original_questions_path,
                           new_perturbed_qa_file_path,
                           metadata_path):
        variations_output = FileIO.read_json(perturbed_variations_output_path)
        metadata = FileIO.read_json(metadata_path)

        original_questions = FileIO.read_json(original_questions_path)

        original_variations_output = FileIO.read_json(original_variations_output_file_path)

        new_answers = {"info": original_questions["info"], "questions": []}

        for qa in original_questions["questions"]:
            program = qa["program"]

            scene_structs = original_variations_output["original_video_output"]["scene_states"]
            causal_graph = CausalGraph(original_variations_output["original_video_output"]["causal_graph"])
            start_scene_struct = [scene['scene'] for scene in scene_structs if scene['step'] == 0][0]
            end_scene_struct = [scene['scene'] for scene in scene_structs if scene['step'] != 0][0]
            scene_structs_array = [start_scene_struct, end_scene_struct]

            answer = None
            try:
                answer = QuestionGeneratorScript.answer_question_offline(variations_output,
                                                                         scene_structs_array,
                                                                         causal_graph,
                                                                         program, metadata)
            except Exception as e:
                logger.error(f"Answer could not be generated: {str(e)}")

            new_qa = copy.deepcopy(qa)

            new_qa["answer"] = answer

            new_answers["questions"].append(new_qa)

        # Because of parallelization, we need to write to file, to not make things more complex with process-safety
        FileIO.write_json(new_answers, new_perturbed_qa_file_path)

    @staticmethod
    def measure_similarity(questions_original, questions_perturbed):
        correct = 0
        found_count = 0

        wrong_answers = []
        correct_answers = []
        not_found = []

        for original in questions_original:
            perturbed = None
            for question in questions_perturbed:
                if (original["question"] == question["question"]) and (
                        str(original["video_index"]) == str(question["video_index"])):
                    perturbed = question

            if perturbed is None:
                not_found.append(original)
                continue
            else:
                found_count += 1
                if str(original["answer"]) == str(perturbed["answer"]):
                    correct += 1
                    correct_answers.append(original)
                else:
                    wrong_answers.append({"original": original, "perturbed": perturbed})

        data = {"correct": correct_answers, "wrong": wrong_answers, "not_found_in_perturbed_questions": not_found}
        return data, len(questions_original), found_count, correct / found_count if found_count != 0 else 0


class QuestionGenerator:

    def __init__(self,
                 input_scene_file_path: str,
                 output_file_path: str,
                 simulation_config: dict,
                 metadata_file_path: str = '../svqa/metadata.json',
                 synonyms_file_path: str = '../svqa/synonyms.json',
                 templates_dir: str = '../svqa/SVQA_1.0_templates',
                 instances_per_template=1):
        self.__args = QuestionGeneratorScript.parser.parse_args(['--input-scene-file', input_scene_file_path,
                                                                 '--output-questions-file', output_file_path,
                                                                 '--metadata-file', metadata_file_path,
                                                                 '--synonyms-json', synonyms_file_path,
                                                                 '--template-dir', templates_dir,
                                                                 '--instances-per-template',
                                                                 str(instances_per_template),
                                                                 '--restrict-template-count-per-video', False,
                                                                 '--print-stats', False,
                                                                 '--excluded-task-ids',
                                                                 simulation_config[
                                                                     "excluded_task_ids"] if simulation_config is not None else []])

    def execute(self):
        QuestionGeneratorScript.main(self.__args)
