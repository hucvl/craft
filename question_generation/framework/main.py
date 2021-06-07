import os
import sys
from datetime import datetime

from loguru import logger

from framework.dataset import DatasetGenerationConfig, CRAFTDataset
from framework.pipeline import Pipeline, DatasetSplitStage, FullDatasetWriteStage, AnnotationsFileCollector, \
    DatasetGenerationStage, PreBalancingPostProcessStage, BalancingStage, CleanupStage, PostProcessStage, \
    CollectUnperturbedDataset, DescriptiveBalanceStage, ExportDatasetStatistics
from framework.utils import FileIO

if __name__ == '__main__':

    # For logging:
    os.makedirs("./out", exist_ok=True)
    logger.add(f"out/dataset_generation_{datetime.now().strftime('%m%d%Y_%H%M')}.log", enqueue=True)

    craft_dataset_generation_pipeline = Pipeline([
        DatasetGenerationStage(),
        PreBalancingPostProcessStage(),
        BalancingStage(purge_single_answers=True),
        DatasetSplitStage("hard"),
        DatasetSplitStage("random"),
        FullDatasetWriteStage("dataset.json"),  # Full dataset includes "program"s for each question
        AnnotationsFileCollector("annotations.json"),
        CleanupStage(),
    ])
    logger.info("Dataset generation pipeline object initiated")

    dataset_generation_config_file_path = "../config/craft_10000_config.json"
    config = DatasetGenerationConfig(FileIO.read_json(dataset_generation_config_file_path))
    logger.info("Dataset generation configuration file loaded")

    craft_dataset_generation_pipeline.feed_first_stage(config)

    logger.info("Starting execution...")
    craft_dataset_generation_pipeline.execute_all()
