#!/bin/sh
echo "iray demo ..."

export PATH=/usrdata/iray_demo/:$PATH

ar_iqtool_service &

cp /usrdata/local/factory/tunning/cam_common_inf/infrared_elf1_tuning_preview.bin /usrdata/local/factory/tunning/cam_common_inf/common_infrared_tuning_preview.bin
sync

test_elf1_raw -o 1&
