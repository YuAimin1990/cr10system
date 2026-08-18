#!/bin/sh
echo "run ..."

camera --start_vin_driver -mpi -af_stats 1

chmod 0666 /dev/null

# 配置loopback, 127.0.0.1
ifconfig lo up

# start wifi
/var/as_start.sh &

# 启动 eth0 默认配置，该配置在 /usrdata/app/network.sh 中被重新配置并失效
ifconfig eth0 hw ether "00:17:61:00:00:04" # 临时MAC地址，以后出厂设置时需要去掉
ifconfig eth0 10.12.160.251 netmask 255.255.255.0 up
echo "nameserver 8.8.8.8" > /etc/resolv.conf

# 配置虚拟网卡 eth0:0，避免常用网段冲突，用于固定IP
# 该配置的IP地址在用户可能忘记固件IP地址、并且固件不能正常工作时的临时使用
ifconfig eth0:0 10.12.161.251 netmask 255.255.255.0 up
ip addr flush dev eth0 label "eth0"

servicemanager &

# wifi driver
cd /var && ./as_start.sh

if [ -e "/usrdata/app/eth0.sh" ]; then
/usrdata/app/eth0.sh
fi
if [ -e "/usrdata/app/wlan0.sh" ]; then
/usrdata/app/wlan0.sh
fi

# touch screen and framebuffer
insmod /mod/GT1X.ko
insmod /mod/ar_framebuffer.ko width=720 height=1440 format=0

sleep 2


export LD_LIBRARY_PATH=/usrdata/app/lib:/usrdata/lib:$LD_LIBRARY_PATH
export PATH=/usrdata/app/bin:$PATH

# 摄像头及其预览
# export SENSOR0_TUNING_FILE=/usrdata/local/factory/tunning/cam_imx307/imx307_tuning_preview_rgb_face_0108.bin
export SENSOR0_TUNING_FILE=/usrdata/local/factory/tunning/cam_imx307/imx307_tuning_preview_rgb_palm_0107.bin
# export SENSOR1_TUNING_FILE=/usrdata/local/factory/tunning/cam_imx307/imx307_tuning_preview_ir_face_0108.bin
export SENSOR1_TUNING_FILE=/usrdata/local/factory/tunning/cam_imx307/imx307_tuning_preview_ir_palm_0107.bin
mpp_vio_service &
sleep 1
mpp_vio_mjpeg_streamer 0 8080 &
mpp_vio_mjpeg_streamer 1 8081 &

# OS time sync
htwrtc_test &

# services
cd /usrdata/app/xserver/ && /usrdata/app/bin/redis-server &
cd /usrdata/app/xserver/ && openresty/nginx/sbin/nginx -p running/

mpp_audio_service &

crond start &
sleep 1

cd /usrdata/app/ && app.sh &
