#!/bin/sh

export CROSS_COMPILE=arm-none-eabi-
export ARCH=arm

CWD=`pwd`
UBOOT_SRC=~/bananapi/u-boot
UBOOT_CONFIG_NAME=SPI64_Banana_M2M_V2
UBOOT_CONF=~/bananapi/config/${UBOOT_CONFIG_NAME}
UBOOT_TARGET=~/bananapi/build/u-boot

cp -f ${UBOOT_CONF} ${UBOOT_SRC}/configs/${UBOOT_CONFIG_NAME}_defconfig

rm -rf ${UBOOT_TARGET} && mkdir -p ${UBOOT_TARGET}
cd ${UBOOT_SRC}
make O=${UBOOT_TARGET} mrproper && make O=${UBOOT_TARGET} ${UBOOT_CONFIG_NAME}_defconfig
make O=${UBOOT_TARGET} all -j6
cd ${CWD}

