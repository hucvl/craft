## Configuration files for CRAFT dataset generation

This directory contains sample configuration files for generating the dataset.

Below is the list of fields in the configuration JSON file and their descriptions.

| Field  | Description |
|---|---|
| `dataset_size`  | Specifies the number of videos to simulate. Note that after balancing, some videos may be left without questions, so final dataset size might be lower than this number.  |
| `executable_path`  | Path to the simulator executable. |
| `executable_working_directory`  | Path to the working directory of the simulator executable. The directory contains texture data under Data/Textures folder. |
| `metadata_file_path`  | Specifies the path of the metadata file that contains object, size, and shape information along with event and input information. |
| `output_folder_path`  | Specifies the output path: Dataset files, statistics, and intermediates. |
| `do_not_generate_questions`  | If true, only videos are generated. |
| `offline`  | If true, simulator works silently in the background. |
| `perturbation_config`  | If `null` or unspecified, no perturbation is performed on the simulations. `amount` specifies the percentage of deviation of the dynamic objects' positions and velocities from the original simulation. `perturbations_per_simulation` specifies the number of random perturbations to be performed on each simulation instance. |
| `simulation_configs` | List of objects that contain scene type to be run and their configurations. `id` specifies the scene type to be run,`step_count` specifies the number of steps of the simulation (for instance, 600 would mean a 10 second video), `width` and `height` specify the size of the generated video, `excluded_task_ids` specifies the questions not to be asked when running this scene type (for instance, "descriptive_2").  | 