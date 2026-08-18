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

#rtcmd run hdmi_tx_init
servicemanager &
ipcam &

ifconfig lo up
chmod 666 /dev/null

#ifconfig eth0
ifconfig eth0 hw ether "92:05:00:f0:0a:1a"
ifconfig eth0 up
ifconfig eth0 192.168.162.216

insmod /mod/gt911.ko width=1280 height=720 swapxy=1 mirrorx2y=1
insmod /mod/ar_framebuffer.ko width=720 height=1280 format=0

