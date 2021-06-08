# CRAFT Baseline Models

This part of the repository contains official baseline implementations for [CRAFT: A Benchmark for Causal Reasoning About Forces and inTeractions](https://sites.google.com/view/craft-benchmark). We used existing implementations for [TVQA](https://github.com/jayleicn/TVQA), [TVQA+](https://github.com/jayleicn/TVQAplus/), [MAC](https://github.com/rosinality/mac-network-pytorch), [G-SWM](https://github.com/zhixuan-lin/G-SWM).

## Requirements

To install requirements:

``bash
conda env create -f environment.yml
conda activate intphys
pip install numpy==1.20.3 click==8.0.1 hydra-core==1.0.6
pip install torch==1.8.1+cu101 torchvision==0.9.1+cu101 torchtext==0.9.1 \ 
    -f https://download.pytorch.org/whl/torch_stable.html
    pip install pytorch-lightning=1.3.3
    pip install -e .
    ``
    
## Dataset Setup
    
    First, download and extract dataset. Then, prepare dataset for baseline models:
    
    ``bash
    ./bin/json2tsv ./path/to/data
    ./bin/extract-frames --datadir ./path/to/data
    DATASET_DIR=./path/to/data ./bin/downsample-videos
    ``
    
## Training
    
    To train the models for easy setting, run the following commands,
    
    ``bash
    ./bin/train model.architecture=LSTMBaseline # LSTM 
    ./bin/train model.architecture=LSTMCNNBaselineFF # LSTM-CNN-F
    ./bin/train model.architecture=LSTMCNNBaselineLF # LSTM-CNN-L
    ./bin/train model=MAC model.architecture=MACBaselineFF # MAC-F
    ./bin/train model=MAC model.architecture=MACBaselineLF # MAC-L
    ./bin/train model=VideoBaseline trainer.max_epochs=30 loader.batch_size=32 # LSTM-CNN-V
    ./bin/train model=MACV trainer.max_epochs=30 loader.batch_size=32 # MAC-V
    ./bin/train model=TVQA trainer.max_epochs=30 loader.batch_size=32 # TVQA
    ./bin/train model=TVQAPlus trainer.max_epochs=30 loader.batch_size=32 trainer.precision=32 # TVQA+
    ``
    
    To train models on hard setting insert `dataset.params.split_info="hard"` option. Please type `./bin/train help` or take a look at `config/train.yaml` file for more training configuration.


## Evaluation

To evaluate a baseline, generate answers first, and then evaluate:

``bash
./bin/generate checkpoint="./path/to/best/model.ckpt" output="./path/to/output/file.tsv"
./bin/accuracy ./path/to/data/questions-random.tsv ./path/to/output/file.tsv
``

To evaluate models on hard setting, insert `dataset.params.split_info="hard"` option for `./bin/generate` command and use `questions-hard.tsv` as first input argument of `./bin/accuracy` command.

