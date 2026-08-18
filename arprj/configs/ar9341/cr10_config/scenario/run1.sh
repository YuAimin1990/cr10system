#!/bin/sh
echo "run ..."

camera --start_vin_driver -mpi -af_stats 1

chmod 0666 /dev/null

# 配置loopback, 127.0.0.1
ifconfig lo up

# start wifi
/var/as_start.sh &

ifconfig eth0 hw ether "00:17:61:00:00:04"
ifconfig eth0 192.168.1.251 netmask 255.255.255.0 up
route add default gw 192.168.1.1
echo "nameserver 8.8.8.8" > /etc/resolv.conf
ifconfig eth0:0 10.12.160.251 netmask 255.255.255.0 up

servicemanager &

# touch screen and framebuffer
insmod /mod/GT1X.ko
insmod /mod/ar_framebuffer.ko width=720 height=1440 format=0

export LD_LIBRARY_PATH=/usrdata/app/lib:/usrdata/lib:$LD_LIBRARY_PATH
export PATH=/usrdata/app/bin:$PATH

