import torch
import torch.nn as nn
import torch.nn.functional as F

from intphys.extra.tvqa.tvqa_abc import ABC
from intphys.extra.tvqa_plus.stage import STAGE
from intphys.data import SimulationInput
from intphys.submodule import *


class TVQA(nn.Module):
    SIMULATION_INPUT = SimulationInput.VIDEO
    
    def __init__(self, config):
        super().__init__()
        config["tvqa"]["vocab_size"] = config["input_size"]
        config["tvqa"]["dropout"] = config["dropout"]
        config["tvqa"]["output_size"] = config["output_size"]
        config["frame_encoder"]["depth_size"] = config["depth_size"]
        config["frame_encoder"]["input_width"] = config["input_width"]
        config["frame_encoder"]["input_height"] = config["input_height"] 
        self.config = config

        self.frame_encoder = self.create_submodule("frame_encoder")
        self.adaptive_pool = nn.AdaptiveAvgPool2d(config["pool_size"])
        self.flatten = nn.Flatten()
        config["tvqa"]["vid_feat_size"] = config["pool_size"]**2 * self.frame_encoder.out_channels
        self.tvqa = ABC(config["tvqa"])
    
    def create_submodule(self, submodule):
        config = self.config[submodule]
        submodule = eval(config["architecture"])(config)
        return submodule

    def process_simulation(self, simulations, **kwargs):
        B, C, T, X1, X2 = simulations.shape
        y = simulations.permute(0, 2, 1, 3, 4)
        y = y.reshape(B*T, C, X1, X2)
        y = self.frame_encoder(y)
        y = self.adaptive_pool(y)
        y = self.flatten(y)
        y = y.reshape(B, T, -1)
        return y 

    def forward(self, simulations, questions, lengths, **kwargs):
        visual = self.process_simulation(simulations, **kwargs)
        B, T = visual.shape[:2]
        visual_lengths = torch.tensor([T for i in range(B)])
        return self.tvqa(questions, torch.tensor(lengths), visual, visual_lengths)


class TVQAPlus(nn.Module):
    SIMULATION_INPUT = SimulationInput.VIDEO
    
    def __init__(self, config):
        super().__init__()
        config["stage"]["embed_size"] = config["question_encoder"]["hidden_size"]
        config["stage"]["dropout"] = config["dropout"]
        config["stage"]["output_size"] = config["output_size"]
        config["question_encoder"]["vocab_size"] = config["input_size"]
        config["frame_encoder"]["depth_size"] = config["depth_size"]
        config["frame_encoder"]["input_width"] = config["input_width"]
        config["frame_encoder"]["input_height"] = config["input_height"] 
        self.config = config

        self.frame_encoder = self.create_submodule("frame_encoder")
        self.question_encoder = self.create_submodule("question_encoder")
        self.adaptive_pool = nn.AdaptiveAvgPool2d(config["pool_size"])
        self.flatten = nn.Flatten()
        config["stage"]["vfeat_size"] = self.frame_encoder.out_channels
        self.stage = STAGE(config["stage"])
    
    def create_submodule(self, submodule):
        config = self.config[submodule]
        submodule = eval(config["architecture"])(config)
        return submodule

    def process_simulation(self, simulations, **kwargs):
        B, C, T, X1, X2 = simulations.shape
        y = simulations.permute(0, 2, 1, 3, 4)
        y = y.reshape(B*T, C, X1, X2)
        y = self.frame_encoder(y)
        y = self.adaptive_pool(y)
        K, X1, X2 = y.shape[-3:]
        y = y.view(B, T, K, X1 * X2)
        y = y.permute(0, 1, 3, 2)
        return y 

    def process_question(self, questions, lengths, **kwargs):
        output, (hiddens, _) = self.question_encoder(questions, lengths)
        return nn.utils.rnn.pad_packed_sequence(output, batch_first=True)[0]

    def forward(self, simulations, questions, lengths, **kwargs):
        visual = self.process_simulation(simulations, **kwargs)
        textual = self.process_question(questions, lengths, **kwargs)
        B, T, HW = visual.shape[:3]
        device = visual.device
        visual_lengths = torch.empty(B, T, HW, dtype=visual.dtype).fill_(1)
        textual_lengths = torch.zeros(B, 1, max(lengths), dtype=visual.dtype)
        for (i, length) in enumerate(lengths):
            textual_lengths[i, 0, :length] = 1.0
        batch = {
            "qas_bert": textual,
            "qas_mask": textual_lengths.to(device),
            "vid": visual,
            "vid_mask": visual_lengths.to(device),
        }
        return self.stage(batch)
