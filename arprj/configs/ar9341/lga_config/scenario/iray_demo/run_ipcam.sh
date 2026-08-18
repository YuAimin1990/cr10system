#!/bin/sh
echo "iray demo ..."

if [ ! -e "/dev/ar_efuse" ]; then
  insmod /mod/ar_efuse.ko
fi

export PATH=/usrdata/iray_demo/:$PATH

USE_DUAL_ISP=N

if [ $USE_DUAL_ISP != Y ]; then
cp /usrdata/local/factory/tunning/cam_common_inf/infrared_elf1_tuning_preview.bin /usrdata/local/factory/tunning/cam_common_inf/common_infrared_tuning_preview.bin
else
cp /usrdata/local/factory/tunning/cam_common_inf/infrared_elf1_tuning_preview_isp0.bin /usrdata/local/factory/tunning/cam_common_inf/common_infrared_tuning_preview.bin
cp /usrdata/local/factory/tunning/cam_common_inf/infrared_elf1_tuning_preview_isp1.bin /usrdata/local/factory/tunning/cam_common_inf/common_infrared_tuning_preview1.bin
fi
sync

ipcam &
