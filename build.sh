#!/bin/bash

export PATH="$PATH:/opt/Xilinx/SDK/2019.1/gnu/aarch32/lin/gcc-arm-linux-gnueabi/bin/"

export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabihf-

make otbelec_protoboard_defconfig
make UIMAGE_LOADADDR=0x8000 uImage

