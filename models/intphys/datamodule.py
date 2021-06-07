from torch.utils.data import DataLoader
from torchvision import transforms as tr
import pytorch_lightning as pl

from intphys.data import *


MAX_IMAGE_SIZE = 640


class DataModule(pl.LightningDataModule):
    def __init__(self, config):
        super().__init__()

        image_dim = config.get("image_size", 112)
        transforms = [tr.Resize(image_dim)]

        frame_encoder = config["model"]["frame_encoder"]["architecture"]
        normalizer = None
        if frame_encoder.startswith("resnet"):
            normalizer = tr.Normalize(
                mean=[0.485, 0.456, 0.406],
                std=[0.229, 0.224, 0.225])
        elif frame_encoder in ("r3d_18",):
            normalizer = tr.Normalize(
                mean=[0.43216, 0.394666, 0.37645],
                std=[0.22803, 0.22145, 0.216989])
        
        if normalizer is not None:
            transforms.append(normalizer)
        
        self.frame_transform = tr.Compose(transforms)
        self.dataset_class = eval(config["dataset"]["name"])
        self.config = config

    @property
    def dataset_name(self):
        return self.config["dataset"]["name"]

    def setup(self, stage=None):
        Dataset = self.dataset_class
        
        if stage == "fit" or stage is None:
            self.train_data = Dataset(
                split="train",
                transform=self.frame_transform,
                **self.config["dataset"]["params"]
            )

            self.val_data = Dataset(
                split="validation",
                transform=self.frame_transform,
                **self.config["dataset"]["params"]
            )

        if stage == "test" or stage is None:
            self.test_data = Dataset(
                split="test",
                transform=self.frame_transform,
                **self.config["dataset"]["params"]
            )

    def train_dataloader(self):
        return DataLoader(
            self.train_data,
            shuffle=True,
            collate_fn=train_collate_fn,
            **self.config["loader"])

    def val_dataloader(self):
        return DataLoader(
            self.val_data,
            shuffle=False,
            collate_fn=train_collate_fn,
            **self.config["loader"])

    def test_dataloader(self):
        return DataLoader(
            self.test_data,
            collate_fn=inference_collate_fn,
            **self.config["loader"],
        )