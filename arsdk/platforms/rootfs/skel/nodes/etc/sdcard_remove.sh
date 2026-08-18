#!/bin/sh

umount /mnt/sdcard
if [ $? -eq 0 ]; then
    rm -rf /mnt/sdcard
else
    echo "umount /mnt/sdcard failed!"
fi
