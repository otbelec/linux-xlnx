#!/bin/bash

export PATH="$PATH:/opt/Xilinx/SDK/2019.1/gnu/aarch32/lin/gcc-arm-linux-gnueabi/bin/"
export INSTALL_MOD_PATH="arch/arm/boot/"

export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabihf-

make otbelec_protoboard_defconfig

# Kernel
make UIMAGE_LOADADDR=0x8000 uImage

# Devicetree
make otbelec-protoboard.dtb

# Modules
rm -rf $INSTALL_MOD_PATH/lib/
make modules_prepare
make modules && make modules_install
tar -cf $INSTALL_MOD_PATH/rootfs_modules.tar -C $INSTALL_MOD_PATH lib/

