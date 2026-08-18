#!/bin/sh

WORK_HOME=/tmp
USR_DIR=/usrdata/usr
#export LD_LIBRARY_PATH=$WORK_HOME/lib:$LD_LIBRARY_PATH
########################################################################
#==============this is a demo, do whatever you want to=================#
########################################################################

# add some ota-related applications here, such an ota server application.
# attentions: copy the applications to /tmp before running it.
# example:
# cp /local/usr/bin/foo /tmp
# /tmp/foo

# flagfile, can restore any information you want, such as image filename,
# so you can use artosyn_upgrade upgrade automatically

#flagfile=$1

# reboot need it
#if [ -e "/local/usr/bin/ar_wdt_service" ]; then
#       cp /local/usr/bin/ar_wdt_service /tmp
#fi

#cp /usrdata/usr/data/arstack/araccess/wifi/start_usbwifi.sh /tmp
#chmod 777 /tmp/start_usbwifi.sh
#sed -i "s/local/tmp/g" /tmp/start_usbwifi.sh
#mkdir -p /tmp/usr/bin
#/tmp/start_usbwifi.sh &

# tmp rootfs dir
rtfs_dir="/tmp"

# cp /dev
cp -r /dev $rtfs_dir

# cp /etc
cp -r /etc $rtfs_dir/

# cp /bin
cp -r /bin $rtfs_dir

# cp /lib
mkdir -p $rtfs_dir/lib
cp /lib/ld-linux-aarch64.so.1 $rtfs_dir/lib
cp /lib/libpthread.so.0 $rtfs_dir/lib
cp /lib/libc.so.6 $rtfs_dir/lib

# cp /usr/bin
#mkdir -p $rtfs_dir/usr/bin
#cp -r /usr/bin/artosyn_upgrade $rtfs_dir/usr/bin
#cp -r /usr/bin/ar_wdt_service $rtfs_dir/usr/bin

# cp /usr/lib
# cp /usr/lib
mkdir -p $rtfs_dir/usr/lib
if [ -e "/usrdata/lib/libar_flash.so" ]; then
cp /usrdata/lib/libar_flash.so $rtfs_dir/usr/lib
fi


# cp /proc
mkdir -p $rtfs_dir/proc/sys/vm/
cp /proc/meminfo $rtfs_dir/proc
cp /proc/cmdline $rtfs_dir/proc
cp /proc/sys/vm/drop_caches $rtfs_dir/proc/sys/vm

# cp /sys
echo 0 > /sys/block/mmcblk0boot0/force_ro
echo 0 > /sys/block/mmcblk0boot1/force_ro
mkdir -p $rtfs_dir/sys/block/mmcblk0
mkdir -p $rtfs_dir/sys/block/mmcblk0boot0
mkdir -p $rtfs_dir/sys/block/mmcblk0boot1
cp /sys/block/mmcblk0/size $rtfs_dir/sys/block/mmcblk0
cp /sys/block/mmcblk0boot0/size $rtfs_dir/sys/block/mmcblk0boot0
cp /sys/block/mmcblk0boot1/size $rtfs_dir/sys/block/mmcblk0boot1
cp /sys/block/mmcblk0boot0/force_ro $rtfs_dir/sys/block/mmcblk0boot0
cp /sys/block/mmcblk0boot1/force_ro $rtfs_dir/sys/block/mmcblk0boot1

#cp  upgrade_service
mkdir -p $rtfs_dir/sbin
mkdir -p $rtfs_dir/factory
cp /factory/web_boot.conf   /tmp/factory
mkdir -p $rtfs_dir/tmp
cp -r $USR_DIR/web $rtfs_dir/tmp
mkdir -p $rtfs_dir/tmp/web/lighttpd/upload
cp -r $USR_DIR/scripts/shutdown.sh $rtfs_dir

cp /sbin/udhcpc /tmp/sbin
cp /sbin/*    /tmp/bin
cp $USR_DIR/scripts/run_upgrade.sh /tmp
cp $USR_DIR/bin/upgrade_service /tmp
cp $USR_DIR/lib/libjsoncpp.so   /tmp/lib
cp $USR_DIR/lib/libminini.so    /tmp/lib
cp $USR_DIR/lib/libuuid.so.1    /tmp/lib
cp $USR_DIR/lib/libpcre.so* /tmp/lib
cp $USR_DIR/lib/libcrypto.so* /tmp/lib
cp /lib/libm.so.6        /tmp/lib
cp /lib/libstdc++.so.6   /tmp/lib
cp /lib/libdl.so.2       /tmp/lib
cp /lib/libgcc_s.so.1    /tmp/lib

ifconfig eth0 down
if [ -f "/factory/mac.cfg" ];then
        ifconfig eth0 hw ether $(cat /factory/mac.cfg)
else
        echo "$(hexdump -n3 -e'/3 "00:16:28" 3/1 ":%02X"' /dev/urandom)" > /factory/mac.cfg
        ifconfig eth0 hw ether $(cat /factory/mac.cfg)
fi
ifconfig eth0 up
ifconfig lo up

#/sbin/udhcpc -b -i eth0 -s /etc/udhcpc.script -x lease:300 &
#telnetd &

chroot /tmp ./run_upgrade.sh

chroot /tmp exit

#reboot
$rtfs_dir/shutdown.sh

#echo "start upgrade service"
#$WORK_HOME/bin/upgrade_service&
#$WORK_HOME/upgrade_service&

# crond need it
#mkdir -p /var/spool/cron/crontabs/
# reboot after 10min, if you have a different reboot command,
# replace "/tmp/ar_wdt_service -t 0" with your command
#echo "*/10 * * * * /tmp/ar_wdt_service -t 0" > /var/spool/cron/crontabs/root
# run crond
#crond -L /tmp/cron.log
