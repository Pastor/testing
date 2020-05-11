#!/bin/sh

FIRMWARE=ZTE_ZXHN_H298A_HV1120_FV1120_ROS_T14.bin

#dd if=${FIRMWARE} of=root.squashfs skip=4194304 bs=1
#dd if=${FIRMWARE} of=kernel.bin count=4194304 bs=1
#dd if=${FIRMWARE} of=only_kernel.bin count=4194048 skip=256 bs=1

#unsquashfs root.squashfs
#mksquashfs squashfs-root modify_root.squashfs -b 131072 -comp lzma -e boot

cat kernel.bin modify_root.squashfs > modify_firmware.bin
#cat only_kernel.bin root.squashfs > firmware2.bin
