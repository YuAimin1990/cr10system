#!/bin/sh
echo "run ..."

# Enable the following scripts if you want to use dsp
#ar_dsp_boot 0 /usrdata/local/factory/tunning/cam_imx307/ceva.AIispbin

# Enable the following scripts if you run IPC demo
camera --start_vin_driver  -hw_burst 16 -isp_out_hw_burst 16 -share_stats_buffer -mpi
#camera --start_vin_driver  -hw_burst 16 -isp_out_hw_burst 16 -share_stats_buffer -mpi -max_dev_num 1 -max_wh 1920 1080 -en_mem_over_dbg 1

#rtcmd run hdmi_tx_init
servicemanager &
#ipcam &

# test_mpp_vio 15 0 &
