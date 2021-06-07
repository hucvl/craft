# https://github.com/rosinality/mac-network-pytorch/blob/564ca5bbb39bb2bff45e9fa84c898fadd8145d94/model.py

from math import sqrt

from intphys.data import SimulationInput
import torch
from torch.autograd import Variable
from torch import nn
from torch.nn.init import kaiming_uniform_, xavier_uniform_, normal
import torch.nn.functional as F

from intphys.submodule import resnet18, r3d_18


def linear(in_dim, out_dim, bias=True):
    lin = nn.Linear(in_dim, out_dim, bias=bias)
    xavier_uniform_(lin.weight)
    if bias:
        lin.bias.data.zero_()

    return lin


class ControlUnit(nn.Module):
    def __init__(self, dim, max_step):
        super().__init__()

        self.position_aware = nn.ModuleList()
        for i in range(max_step):
            self.position_aware.append(linear(dim * 2, dim))

        self.control_question = linear(dim * 2, dim)
        self.attn = linear(dim, 1)

        self.dim = dim

    def forward(self, step, context, question, control):
        position_aware = self.position_aware[step](question)

        control_question = torch.cat([control, position_aware], 1)
        control_question = self.control_question(control_question)
        control_question = control_question.unsqueeze(1)

        context_prod = control_question * context
        attn_weight = self.attn(context_prod)

        attn = F.softmax(attn_weight, 1)

        next_control = (attn * context).sum(1)

        return next_control


class ReadUnit(nn.Module):
    def __init__(self, dim):
        super().__init__()

        self.mem = linear(dim, dim)
        self.concat = linear(dim * 2, dim)
        self.attn = linear(dim, 1)

    def forward(self, memory, know, control):
        scale = 1 / sqrt(know.shape[1])
        mem = self.mem(memory[-1]).unsqueeze(2)
        know = scale * know
        input = torch.cat([mem * know, know], 1).permute(0, 2, 1)
        concat = self.concat(input)
        attn = concat * control[-1].unsqueeze(1)
        attn = self.attn(attn).squeeze(2)
        attn = F.softmax(attn, 1).unsqueeze(1)
        read = (attn * know).sum(2)
        return read


class WriteUnit(nn.Module):
    def __init__(self, dim, self_attention=False, memory_gate=False):
        super().__init__()

        self.concat = linear(dim * 2, dim)

        if self_attention:
            self.attn = linear(dim, 1)
            self.mem = linear(dim, dim)

        if memory_gate:
            self.control = linear(dim, 1)

        self.self_attention = self_attention
        self.memory_gate = memory_gate

    def forward(self, memories, retrieved, controls):
        prev_mem = memories[-1]
        concat = self.concat(torch.cat([retrieved, prev_mem], 1))
        next_mem = concat

        if self.self_attention:
            controls_cat = torch.stack(controls[:-1], 2)
            attn = controls[-1].unsqueeze(2) * controls_cat
            attn = self.attn(attn.permute(0, 2, 1))
            attn = F.softmax(attn, 1).permute(0, 2, 1)

            memories_cat = torch.stack(memories, 2)
            attn_mem = (attn * memories_cat).sum(2)
            next_mem = self.mem(attn_mem) + concat

        if self.memory_gate:
            control = self.control(controls[-1])
            gate = F.sigmoid(control)
            next_mem = gate * prev_mem + (1 - gate) * next_mem

        return next_mem


class MACUnit(nn.Module):
    def __init__(self, dim, max_step=12,
                self_attention=False, memory_gate=False,
                dropout=0.15):
        super().__init__()

        self.control = ControlUnit(dim, max_step)
        self.read = ReadUnit(dim)
        self.write = WriteUnit(dim, self_attention, memory_gate)

        self.mem_0 = nn.Parameter(torch.zeros(1, dim))
        self.control_0 = nn.Parameter(torch.zeros(1, dim))

        self.dim = dim
        self.max_step = max_step
        self.dropout = dropout

    def get_mask(self, x, dropout):
        mask = torch.empty_like(x).bernoulli_(1 - dropout)
        mask = mask / (1 - dropout)

        return mask

    def forward(self, context, question, knowledge):
        b_size = question.size(0)

        control = self.control_0.expand(b_size, self.dim)
        memory = self.mem_0.expand(b_size, self.dim)

        if self.training:
            control_mask = self.get_mask(control, self.dropout)
            memory_mask = self.get_mask(memory, self.dropout)
            control = control * control_mask
            memory = memory * memory_mask

        controls = [control]
        memories = [memory]

        for i in range(self.max_step):
            control = self.control(i, context, question, control)
            if self.training:
                control = control * control_mask
            controls.append(control)

            read = self.read(memories, knowledge, controls)
            memory = self.write(memories, read, controls)
            if self.training:
                memory = memory * memory_mask
            memories.append(memory)

        return memory


class MACNetwork(nn.Module):
    def __init__(self, n_vocab, dim, embed_hidden=300,
                max_step=12, self_attention=False, memory_gate=False,
                classes=28, visual_features_dim=1024, dropout=0.15,
                sim_input=None):
        super().__init__()

        if sim_input != SimulationInput.VIDEO:
            self.conv = nn.Sequential(
                nn.Conv2d(visual_features_dim, dim, 3, padding=1),
                nn.BatchNorm2d(dim),
                nn.ELU(),
                nn.Conv2d(dim, dim, 3, padding=1),
                nn.BatchNorm2d(dim),
                nn.ELU(),
            )
        else:
            self.conv = nn.Sequential(
                nn.Conv3d(visual_features_dim, dim, 3, padding=1),
                nn.BatchNorm3d(dim),
                nn.ELU(),
                nn.Conv3d(dim, dim, 3, padding=1),
                nn.BatchNorm3d(dim),
                nn.ELU(),
            ) 

        self.embed = nn.Embedding(n_vocab, embed_hidden)
        self.lstm = nn.LSTM(embed_hidden, dim,
                        batch_first=True, bidirectional=True)
        self.lstm_proj = nn.Linear(dim * 2, dim)

        self.mac = MACUnit(dim, max_step,
                        self_attention, memory_gate, dropout)


        self.classifier = nn.Sequential(linear(dim * 3, dim),
                                        nn.ELU(),
                                        linear(dim, classes))

        self.max_step = max_step
        self.dim = dim

        self.reset()

    def reset(self):
        self.embed.weight.data.uniform_(0, 1)

        kaiming_uniform_(self.conv[0].weight)
        self.conv[0].bias.data.zero_()
        kaiming_uniform_(self.conv[3].weight)
        self.conv[3].bias.data.zero_()

        kaiming_uniform_(self.classifier[0].weight)

    def forward(self, image, question, question_len):
        b_size = question.size(0)

        img = self.conv(image)
        img = img.view(b_size, self.dim, -1)

        embed = self.embed(question)
        embed = nn.utils.rnn.pack_padded_sequence(embed, question_len,
                                                batch_first=True)
                                               
        lstm_out, (h, _) = self.lstm(embed)
        lstm_out, _ = nn.utils.rnn.pad_packed_sequence(lstm_out,
                                                    batch_first=True)
        lstm_out = self.lstm_proj(lstm_out)
        h = h.permute(1, 0, 2).contiguous().view(b_size, -1)

        memory = self.mac(lstm_out, h, img)
        out = torch.cat([memory, h], 1)
        out = self.classifier(out)
        return out


class MACBaseline(nn.Module):
    SIMULATION_INPUT = SimulationInput.NO_FRAMES
    NUM_VIDEO_FRAMES = 0
    
    def __init__(self, config):
        super().__init__()
        config["mac"]["n_vocab"] = config["input_size"]
        config["mac"]["dropout"] = config["dropout"]
        config["mac"]["classes"] = config["output_size"]
        config["frame_encoder"]["depth_size"] = config["depth_size"]
        config["frame_encoder"]["input_width"] = config["input_width"]
        config["frame_encoder"]["input_height"] = config["input_height"] 
        self.config = config

        self.frame_encoder = self.create_submodule("frame_encoder")
        config["mac"]["visual_features_dim"] = self.frame_encoder.out_channels
        self.mac = MACNetwork(**config["mac"], sim_input=self.SIMULATION_INPUT)

    def create_submodule(self, submodule):
        config = self.config[submodule]
        submodule = eval(config["architecture"])(config)
        return submodule
    
    def process_simulation(self, simulations, **kwargs):
        y = self.frame_encoder(simulations) 
        return y

    def forward(self, simulations, questions, lengths, **kwargs):
        vis = self.process_simulation(simulations, **kwargs)
        return self.mac(vis, questions, lengths)


class MACBaselineFF(MACBaseline):
    SIMULATION_INPUT = SimulationInput.FIRST_FRAME
    NUM_VIDEO_FRAMES = 1


class MACBaselineLF(MACBaseline):
    SIMULATION_INPUT = SimulationInput.LAST_FRAME
    NUM_VIDEO_FRAMES = 1


class MACVideoBaseline(MACBaseline):
    SIMULATION_INPUT = SimulationInput.VIDEO
    NUM_VIDEO_FRAMES = 1 # see parent class constructor
