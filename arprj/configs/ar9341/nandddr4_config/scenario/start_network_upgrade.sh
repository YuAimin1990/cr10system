#!/bin/sh

USR_DIR=/usrdata/local/web_service/
rtfs_dir=/tmp

if [ -e "$USR_DIR/lib/libar_flash.so" ]; then
cp $USR_DIR/lib/libar_flash.so $rtfs_dir/usr/lib
fi

mkdir -p $rtfs_dir/sbin
mkdir -p $rtfs_dir/factory

if [ -e "/factory/web_boot.conf" ]; then
cp /factory/web_boot.conf   $rtfs_dir/factory
elif [ -e "$USR_DIR/resources/default_web_boot.conf" ]; then
cp $USR_DIR/resources/default_web_boot.conf $rtfs_dir/factory/web_boot.conf
fi

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

# prepare for ssh
if [ -e "/etc/init.d/start_common.sh" ]; then
        . /etc/init.d/start_common.sh
fi

if [ -e "/etc/init.d/start_ssh.sh" ]; then
        . /etc/init.d/start_ssh.sh
fi

ifconfig eth0 down
if [ -f "/factory/mac.cfg" ];then
        ifconfig eth0 hw ether $(cat /factory/mac.cfg)
else
        echo "$(hexdump -n3 -e'/3 "00:16:28" 3/1 ":%02X"' /dev/urandom)" > /factory/mac.cfg
        ifconfig eth0 hw ether $(cat /factory/mac.cfg)
fi
ifconfig eth0 up

ifconfig lo up

chroot /tmp ./run_upgrade.sh

if [ -f $flagfile ]; then
	rm $flagfile
fi
#reboot
$rtfs_dir/shutdown.sh