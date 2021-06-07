# -*- coding: utf-8 -*-

import os
import os.path as osp
from itertools import repeat

import torch
import torch.nn as nn
from torch.nn import functional as F
import pytorch_lightning as pl
from .model import *


__all__ = (
    'Experiment',
)


class Experiment(pl.LightningModule):
    def __init__(self, config):
        super().__init__()
        self.config = config
        self.model = eval(config["model"]["architecture"])(config["model"])
        self.criterion = nn.CrossEntropyLoss()
        self.save_hyperparameters(self.config)
        self._generate_flag = False

    def forward(self, simulation, question, lengths, **kwargs):
        return self.model(simulation, question, lengths, **kwargs)

    def training_step(self, batch, batch_index):
        (simulations, questions, lengths, additional), (answers,) = batch
        output = self(simulations, questions, lengths, **additional["kwargs"])
        loss = self.criterion(output, answers)
        _, predictions = torch.max(output, 1)
        acc = (predictions == answers).float().mean()
        logs = {"train_loss": loss, "train_accuracy": acc}
        return {"loss": loss, "accuracy": acc}

    def training_epoch_end(self, outputs):
        loss = torch.stack([x["loss"] for x in outputs]).mean()
        acc = torch.stack([x["accuracy"] for x in outputs]).mean()
        self.log("train_loss", loss, prog_bar=True)
        self.log("train_accuracy", acc, prog_bar=True)

    def configure_optimizers(self):
        optimizer = eval("torch.optim.{}".format(
            self.config["optimizer"]["method"]))
        optimizers = [optimizer(
            self.model.parameters(),
            **self.config["optimizer"]["params"])]
        if self.config.get("scheduler") is None: return optimizers, []
        
        scheduler = eval("torch.optim.lr_scheduler.{}".format(
            self.config["scheduler"]["method"]))
        schedulers = [{
            "scheduler": scheduler(
                optimizers[0], **self.config["scheduler"]["params"]),
            "monitor": "val_loss",
            "interval": "epoch",
            "frequency": 1,
            "strict": True,
        }]
        return optimizers, schedulers    

    def validation_step(self, batch, batch_index):
        return self.batch_accuracy(batch, batch_index)

    def validation_epoch_end(self, outputs):
        dataset = self.get_dataset(self.val_dataloader())
        split = "val" if dataset.split.startswith("val") else dataset.split
        self.calculate_accuracy(outputs, split=split)

    def test_step(self, batch, batch_index):
        return self.batch_accuracy(batch, batch_index, testing=True)

    def test_epoch_end(self, outputs):
        split = self.test_dataloader().dataset.split
        self.calculate_accuracy(outputs, split=split)

    def batch_accuracy(self, batch, batch_index, testing=False):
        (simulations, questions, lengths, additional), (answers,) = batch
        output = self(simulations, questions, lengths, **additional["kwargs"])
        _, predictions = torch.max(output, 1)
        correct = (answers == predictions).sum()
        num_instances = torch.tensor(answers.size(), device=self.device).float()
        loss = self.criterion(output, answers)

        if testing and self.generate_flag:
            self.write_generations(
                additional["video_indexes"],
                additional["question_indexes"],
                predictions)

        return {"correct": correct,
                "num_instances": num_instances.squeeze(),
                "loss": loss}

    def calculate_accuracy(self, outputs, split):
        correct = torch.stack([x["correct"] for x in outputs]).sum()
        loss = torch.stack([x["loss"] for x in outputs]).mean()
        num_instances = sum([x["num_instances"] for x in outputs])
        acc = correct.float() / num_instances
        self.log(f"{split}_loss", loss, prog_bar=True)
        self.log(f"{split}_accuracy", acc, prog_bar=True)

    @property
    def generate_flag(self):
        return self._generate_flag

    @generate_flag.setter
    def generate_flag(self, flag):
        self._generate_flag = flag

    def write_generations(self, vids, qids, predictions):
        dataset = self.get_dataset(self.test_dataloader())
        split, vocab = dataset.split, dataset.answer_vocab
        vocab = dataset.answer_vocab
        zipped = zip(vids, qids, repeat(split), vocab[predictions.tolist()])
        lines = ["\t".join([str(xi) for xi in x]) + "\n" for x in zipped]
        filepath = self.config["output"]
        with open(filepath, "a") as f:
            f.writelines(lines)

    def get_dataset(self, data_loader):
        return data_loader.dataset
