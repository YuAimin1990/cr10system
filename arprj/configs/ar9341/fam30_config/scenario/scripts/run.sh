#!/bin/sh
echo "run ..."

# Enable the following scripts if you want to use dsp
#ar_dsp_boot 0 /usrdata/local/factory/tunning/cam_imx307/ceva.AIispbin

# Enable the following scripts if you run IPC demo

USE_MEM_REDUCE_CONFIG=N

if [ $USE_MEM_REDUCE_CONFIG != Y ]; then
camera --start_vin_driver \
-mpi \
-af_stats 1 \

else

camera --start_vin_driver \
-use_stats_pingpong 1 \
-en_invalide_hw_addr 1  \
-hw_burst 16 \
-isp_out_hw_burst 16 \
-share_stats_buffer 1\
-max_comm_sns_num 1 \
-max_wh 3840 2160  \
-en_mem_over_dbg 1 \
-ltm_max_mesh_w 16 \
-ltm_max_mesh_h 16 \
-ltm_lut_pool_count 0 \
-tuning_req_pool_count 0 \
-mpi \
-max_stream_per_dev 2  \
-visual_isp_count 1 \
-hardware_mask 0x1069 \
-filter_mask 0x147 \
-isp_use_loop_mem 1 \


fi

servicemanager &

exit

/etc/usb_gadget_configfs.sh rndis+uvc_30+uvc_30 0 dwc2_0 0x1d6b 0x0110
dropbear
uvc_service2 --multi --cam_index 0 --dev_index 0 --name imx307  --power_gpio C0_0 --reset_gpio C1_1 --i2c 0 --vi_comp &
uvc_service2 --multi --cam_index 1 --dev_index 1 --name sc132gs --power_gpio C0_0 --reset_gpio C1_0 --i2c 1 &
ir_led_ctrl.sh on
