#!/bin/bash

rootfs_img=$1 #release/prebuilds/fs/all_rootfs.squashfs
if [ ! -n "$2" ];then
	echo "No need to verity rootfs. Exit"
	exit 0
fi
env_file=$2 #configs/ar9341/evb_config_debug/uboot/env_emmc.txt

# Check existing of command "veritysetup" on server
veritysetup --version > /dev/null
if [ $? -ne 0 ];then
	echo "Error - Command 'veritysetup' not found on server. Please check."
	exit 1
fi

rootfs_img_verified=${rootfs_img%%.*}.verified.${rootfs_img##*.}

hash_tree_size=4194304 #4MBytes
rootfs_size_orig=`stat --printf="%s" $rootfs_img`
#rootfs_padding_size=1048576 #1MBytes
#rootfs_size_orig=`expr \( $rootfs_size_orig + $rootfs_padding_size \) - \( $rootfs_size_orig % $rootfs_padding_size \)`
rootfs_size_verified=`expr $rootfs_size_orig + $hash_tree_size`

dd if=/dev/zero of=$rootfs_img_verified bs=$rootfs_size_verified count=1 2>/dev/null
dd if=$rootfs_img of=$rootfs_img_verified conv=notrunc 2>/dev/null

data_block_size=4096 #4KBytes
data_block_count=`expr $rootfs_size_orig / $data_block_size`
hash_block_size=$data_block_size
hash_block_start=`expr $data_block_count + 1`

hash_tree_offset_bytes=$rootfs_size_orig

#echo "veritysetup format --data-block-size=$data_block_size --data-blocks=$data_block_count --hash-offset=$hash_tree_offset_bytes $rootfs_img_verified $rootfs_img_verified"
verity_result=`veritysetup format --data-block-size=$data_block_size --data-blocks=$data_block_count --hash-offset=$hash_tree_offset_bytes $rootfs_img_verified $rootfs_img_verified`
rm $rootfs_img
mv $rootfs_img_verified $rootfs_img

salt=`echo $verity_result | awk '{print $26}'`
roothash=`echo $verity_result | awk '{print $29}'`

sector_size=512
sector_count=`expr $hash_tree_offset_bytes / $sector_size`

root_dev=`awk -F'verity' '/^bootargs/ {print $2}' $env_file | awk '{print $2}'`
if [ x$root_dev == 'x' ]
then
    root_dev=`awk -F'root=' '/^bootargs/ {print $2}' $env_file | awk '{print $1}'`
fi

mmcroot="/dev/dm-0 rootwait ro rootfstype=squashfs"
DM_MOD="\"root,,0,ro, 0 $sector_count verity 1 $root_dev $root_dev $data_block_size $hash_block_size $data_block_count $hash_block_start sha256 $roothash $salt\""

rootfs_cmd="root=$mmcroot dm-mod.create=$DM_MOD"

sed "s@\ \+root=.*gpt@\ ${rootfs_cmd}\ gpt@" ${env_file} > ${env_file%.*}.dmverity.${env_file##*.}
