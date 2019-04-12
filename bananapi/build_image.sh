#!/bin/sh

CWD=`pwd`
UBOOT_TARGET=~/bananapi/build/u-boot
KERNEL_TARGET=~/bananapi/build/kernel
IMAGE_TARGET=~/bananapi/build/images
ROOTFS_TARGET=~/bananapi/build/rootfs/images/rootfs.cpio.gz
RESULT_TARGET=${IMAGE_TARGET}/W25Q128_spl.bin
IMAGE_MUST_SIZE=16777216


mkdir -p ${IMAGE_TARGET}

UBOOTSIZE=$(stat -c%s ${UBOOT_TARGET}/u-boot-sunxi-with-spl.bin)
KERNELSIZE=$(stat -c%s ${KERNEL_TARGET}/arch/arm/boot/zImage)
cat ${UBOOT_TARGET}/u-boot-sunxi-with-spl.bin ${KERNEL_TARGET}/arch/arm/boot/zImage > ${IMAGE_TARGET}/stage1.img
echo "UBoot  size ${UBOOTSIZE},  offset 0x00000000 - 0x$(printf "%08X" ${UBOOTSIZE})"

ROOTFS_OFFSET=`expr $UBOOTSIZE + $KERNELSIZE`
ROOTFSSIZE=$(stat -c%s ${ROOTFS_TARGET})
echo "zImage size ${KERNELSIZE}, offset 0x$(printf "%08X" ${UBOOTSIZE}) - 0x$(printf "%08X" ${ROOTFS_OFFSET})"
echo "RootFS size ${ROOTFSSIZE}, offset 0x$(printf "%08X" ${ROOTFS_OFFSET}) - na"

cat ${IMAGE_TARGET}/stage1.img ${ROOTFS_TARGET} > ${IMAGE_TARGET}/stage2.img 

FILESIZE=$(stat -c%s ${IMAGE_TARGET}/stage2.img)
PADDING=`expr $IMAGE_MUST_SIZE - $FILESIZE`
dd if=/dev/zero of=${IMAGE_TARGET}/stage3.img bs=${PADDING} count=1
cat ${IMAGE_TARGET}/stage2.img ${IMAGE_TARGET}/stage3.img > ${RESULT_TARGET}
RESULTSIZE=$(stat -c%s ${RESULT_TARGET})
echo "Result ${RESULTSIZE} bytes"
sudo cp ${RESULT_TARGET} /tftpboot/spi.bin
sudo chown nobody: /tftpboot/spi.bin


