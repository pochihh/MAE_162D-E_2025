#!/bin/bash

conda create -y -n week2_new python=3.12
conda init --all
source /home/pi/miniconda3/etc/profile.d/conda.sh
conda activate week2_new

conda install opencv -y
conda install -c conda-forge libstdcxx-ng -y

sudo apt update
sudo apt install -y libcamera-dev
pip install rpi-libcamera

sudo apt install -y libkms++-dev libfmt-dev libdrm-dev
pip install rpi-kms

sudo apt-get install -y libcap-dev
pip install picamera2

pip install gdown
mkdir ./YOLOv4/weights
cd ./YOLOv4/weights
gdown 18RjNcN-wpUMie2FpkYxdG0MMrF0rP5Ns
