import json
import os
from pathlib import Path

from framework.simulation import SimulationInstance, SimulationRunner


def run_simulation_instance(scene_id: int, id: int):
    output_json_path = Path(f"samples/outputs/{id:06d}.json").absolute().as_posix()
    output_video_path = Path(f"samples/outputs/{id:06d}.mpg").absolute().as_posix()
    controller_file_path = Path(f"samples/outputs/controller_{scene_id}_{id:06d}.json").absolute().as_posix()
    variations_file_path = Path(f"samples/outputs/variations_{scene_id}_{id:06d}.json").absolute().as_posix()
    questions_file_path = Path(f"samples/outputs/questions_{scene_id}_{id:06d}.json").absolute().as_posix()
    debug_file_path = Path(f"samples/outputs/debug_{scene_id}_{id:06d}.txt").absolute().as_posix()

    with open(controller_file_path, 'w') as controller_file:
        json.dump(
            json.loads(
                f"""{{
                        "simulationID": {scene_id},
                        "offline": false,
                        "outputVideoPath": "{output_video_path}",
                        "outputJSONPath": "{output_json_path}",
                        "width": 256,
                        "height": 256,
                        "inputScenePath":  "",
                        "stepCount": 600
                    }}"""),
            controller_file,
            indent=2
        )

    exec_path = Path("../../simulation/2d/SVQA-Box2D/Build/bin/x86_64/Release/Testbed").absolute().as_posix()
    working_dir = Path("../../simulation/2d/SVQA-Box2D/Testbed").absolute().as_posix()

    runner = SimulationRunner(exec_path, working_directory=working_dir)

    instance = SimulationInstance(id, controller_file_path, variations_file_path, questions_file_path, runner)

    instance.run_simulation(debug_output_path=debug_file_path)
    instance.run_variations()
    instance.generate_questions(simulation_config=None)


if __name__ == '__main__':
    # CAUTION: Current working directory must be one level from the root directory, preferably "framework".

    os.makedirs("samples/outputs", exist_ok=True)

    run_simulation_instance(6, 1)
