#!/bin/sh
echo "guide demo ..."

if [ ! -e "/dev/ar_efuse" ]; then
  insmod /mod/ar_efuse.ko
fi

export LD_LIBRARY_PATH=/usrdata/guide_demo/:$LD_LIBRARY_PATH

ar_iqtool_service &

USE_DUAL_ISP=Y

if [ $USE_DUAL_ISP != Y ]; then
cp /usrdata/local/factory/tunning/cam_common_inf/infrared_timo256_tuning_preview.bin /usrdata/local/factory/tunning/cam_common_inf/common_infrared_tuning_preview.bin
else
cp /usrdata/local/factory/tunning/cam_common_inf/infrared_timo256_tuning_preview_isp0.bin /usrdata/local/factory/tunning/cam_common_inf/common_infrared_tuning_preview.bin
cp /usrdata/local/factory/tunning/cam_common_inf/infrared_timo256_tuning_preview_isp1.bin /usrdata/local/factory/tunning/cam_common_inf/common_infrared_tuning_preview1.bin
fi

sync

test_timo256_raw -o 1&
