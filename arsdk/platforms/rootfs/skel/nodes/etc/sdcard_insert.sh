#!/bin/sh

if [ ! -d "/mnt/sdcard" ]; then
    mkdir /mnt/sdcard
fi

mount -t vfat /dev/$MDEV /mnt/sdcard
if [ $? -ne 0 ]; then
    echo "mount /dev/$MDEV to /mnt/sdcard failed!"
fi
