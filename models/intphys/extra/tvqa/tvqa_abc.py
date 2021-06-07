__author__ = "Jie Lei"

import torch
from torch import nn

from intphys.extra.tvqa.rnn import RNNEncoder, max_along_time
from intphys.extra.tvqa.bidaf import BidafAttn
from intphys.extra.tvqa.mlp import MLP


class ABC(nn.Module):
    def __init__(self, config):
        super(ABC, self).__init__()
        lstm_hidden_size_1 = config["lstm_hidden_size_1"] # 256
        lstm_hidden_size_2 = config["lstm_hidden_size_2"] # 256
        mlp_hidden_size = config["mlp_hidden_size"] # 256
        mlp_num_layers = config["mlp_num_layers"]
        vid_feat_size = config["vid_feat_size"]  # 2048
        embed_size = config["embed_size"] # 300
        vocab_size = config["vocab_size"]
        num_classes = config["output_size"]
        pdrop = config["dropout"]

        self.embedding = nn.Embedding(vocab_size, embed_size)
        self.bidaf = BidafAttn(lstm_hidden_size_1 * 3, method="dot")  # no parameter for dot
        self.lstm_raw = RNNEncoder(embed_size, lstm_hidden_size_1, bidirectional=True, dropout_p=0, n_layers=1, rnn_type="lstm")

        self.video_fc = nn.Sequential(
            nn.Dropout(pdrop),
            nn.Linear(vid_feat_size, embed_size),
            nn.Tanh(),
        )

        self.lstm_mature_vid = RNNEncoder(
            lstm_hidden_size_1 * 2 * 3, lstm_hidden_size_2,
            bidirectional=True, 
            dropout_p=0,
            n_layers=1,
            rnn_type="lstm")

        self.classifier_vid = MLP(
            lstm_hidden_size_2*2,
            num_classes,
            mlp_hidden_size,
            mlp_num_layers,
            pdrop,
        )

        self.config = config

    def load_embedding(self, pretrained_embedding):
        self.embedding.weight.data.copy_(torch.from_numpy(pretrained_embedding))

    def forward(self, q, q_l, vid, vid_l):
        e_q = self.embedding(q)
        raw_out_q, _ = self.lstm_raw(e_q, q_l)
        e_vid = self.video_fc(vid)
        raw_out_vid, _ = self.lstm_raw(e_vid, vid_l)
        vid_out = self.stream_processor(self.lstm_mature_vid, self.classifier_vid, raw_out_vid, vid_l,
                                        raw_out_q, q_l)
        out = vid_out
        return out.squeeze()

    def stream_processor(self, lstm_mature, classifier, ctx_embed, ctx_l, q_embed, q_l):
        u_q, _ = self.bidaf(ctx_embed, ctx_l, q_embed, q_l)
        concat = torch.cat([ctx_embed, u_q, u_q * ctx_embed], dim=-1)
        mature_maxout, _ = lstm_mature(concat, ctx_l)
        mature_maxout = max_along_time(mature_maxout, ctx_l).unsqueeze(1)
        out = classifier(mature_maxout)  # (B, num_classes)
        return out

    @staticmethod
    def get_fake_inputs(device="cuda:0"):
        bsz = 16
        q = torch.ones(bsz, 25).long().to(device)
        q_l = torch.ones(bsz).fill_(25).long().to(device)
        vid = torch.ones(bsz, 100, 2048).to(device)
        vid_l = torch.ones(bsz).fill_(100).long().to(device)
        return q, q_l, vid, vid_l


if __name__ == '__main__':
    config = {
        "lstm_hidden_size_1": 150,
        "lstm_hidden_size_2": 300,
        "mlp_hidden_size": 300,
        "mlp_num_layers": 2,
        "vid_feat_size": 2048,
        "embed_size": 300,
        "input_size": 100,
        "output_size": 21,
        "vocab_size": 100,
        "dropout": 0.2,
        "device": "cuda:0",
    }
    model = ABC(config)
    model.to(config["device"])
    test_in = model.get_fake_inputs(device=config["device"])
    test_out = model(*test_in)
    print(test_out.size())
