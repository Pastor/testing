#!/bin/sh

export CROSS_COMPILE=arm-none-eabi-
export ARCH=arm

CWD=`pwd`
KERNEL_SRC=~/repo/linux
KERNEL_CONFIG_NAME=SPI64_Banana_M2M_V2
KERNEL_TARGET=~/bananapi/build/kernel

rm -rf ${KERNEL_TARGET} && mkdir -p ${KERNEL_TARGET}
cd ${KERNEL_SRC}
make O=${KERNEL_TARGET} sunxi_defconfig && make O=${KERNEL_TARGET} menuconfig
make O=${KERNEL_TARGET} all -j6
cd ${CWD}

