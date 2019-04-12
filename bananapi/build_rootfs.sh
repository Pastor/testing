#!/bin/sh

CWD=`pwd`
ROOTFS_SRC=~/repo/buildroot
ROOTFS_TARGET=~/bananapi/build/rootfs

cd ${ROOTFS_SRC}
make O=${ROOTFS_TARGET} mrproper
cp $CWD/BR2_config ${ROOTFS_TARGET}/.config
make O=${ROOTFS_TARGET} menuconfig
make O=${ROOTFS_TARGET}
cd ${CWD}

