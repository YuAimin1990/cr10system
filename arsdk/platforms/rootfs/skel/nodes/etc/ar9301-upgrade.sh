#!/bin/sh

function get_flashtype() {

	# diff 9341 and 9311
	grep "proxima-9311" /sys/firmware/devicetree/base/compatible
	if [ $? == 0 ]; then
		bootconfig=`devmem 0x0a10602c`
	else
		bootconfig=`devmem 0x01073120`
	fi

	mask=0x7

	let bootmode=$(($bootconfig & $mask))

	case $bootmode in
	0)
		flashtype=nor
		;;
	1)
		flashtype=nand
		;;
	4)
		#if bootmod is emmc, wait and check emmc partition
		echo "waiting emmc or sd device done"
		flashtype=mmc0
		;;
	*)
		echo "bootmode is not support now, bootmode = $bootmode"
		;;
	esac
}

if [ $# != 1 ]; then
	echo "usage: $0 image"
	echo "    if the flash is emmc, upgrade by chroot, otherwise, upgrade directly"
	echo "    if the flash is emmc, the image is the path in chroot enviroment,"
	echo "    eg: the file is /tmp/mnt/artosyn.img, and /tmp is chroot path,"
	echo "    so the image should be /mnt/artosyn.img"
	exit
fi

#call function to get flashtype
get_flashtype

if [ $flashtype == "mmc0" ]; then
	chroot /tmp artosyn_upgrade $1
else
	artosyn_upgrade $1
fi

