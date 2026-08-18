#!/bin/sh
echo "guide demo ..."

export LD_LIBRARY_PATH=/usrdata/guide_demo/:$LD_LIBRARY_PATH

ar_iqtool_service &

cp /usrdata/local/factory/tunning/cam_common_inf/infrared_timo256_tuning_preview.bin /usrdata/local/factory/tunning/cam_common_inf/common_infrared_tuning_preview.bin
sync

test_timo256_raw -o 1&
