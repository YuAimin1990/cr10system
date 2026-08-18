#!/bin/sh
echo "run ..."

ifconfig eth0 up && echo 1 >/proc/sys/net/ipv6/conf/eth0/disable_ipv6 && ifconfig eth0 hw ether B0:88:FE:9c:12:34 && /sbin/udhcpc -b -i eth0 -s /etc/udhcpc.script -x lease:300 &
# Enable the following scripts if you want to use dsp
#ar_dsp_boot 0 /usrdata/local/factory/tunning/cam_imx307/ceva.AIispbin

# Enable the following scripts if you run IPC demo
camera --start_vin_driver -mpi

#single camera 1080p linear mode reduce mem
#camera --start_vin_driver  -hw_burst 16 -isp_out_hw_burst 16 -share_stats_buffer -max_comm_sns_num 1 -max_wh 1920 1080 -en_mem_over_dbg 1 -ltm_max_mesh_w 8 -ltm_max_mesh_h 8  -ltm_lut_pool_count 0 -tuning_req_pool_count 0 -mpi


#rtcmd run hdmi_tx_init
servicemanager &
#ipcam &

#start ipcam web service
#sleep 3
#ipcam -m 9 &
#/usrdata/run_web.sh &

# test_mpp_vio 15 0 &
