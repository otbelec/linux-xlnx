#!/bin/bash

export PATH="$PATH:/opt/Xilinx/SDK/2019.1/gnu/aarch32/lin/gcc-arm-linux-gnueabi/bin/"
export INSTALL_MOD_PATH="arch/arm/boot/"

export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabihf-

make otbelec_protoboard_defconfig
make UIMAGE_LOADADDR=0x8000 uImage
make otbelec-protoboard.dtb

make modules_prepare
make modules
make modules_install

