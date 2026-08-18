#!/bin/sh
if [ -f "/factory/mac.cfg" ]
then
ifconfig eth0 hw ether `cat /factory/mac.cfg`
fi
if [ -f "/factory/mac1.cfg" ]
then
    ifconfig eth1 hw ether `cat /factory/mac1.cfg`
fi

WORK_HOME=/usrdata/usr/data/arstack

ifconfig eth0 192.168.1.100 up
#cp $WORK_HOME/udhcpc.script /tmp/
#/sbin/udhcpc -b -i eth0 -s /tmp/udhcpc.script -x lease 300 &


WORK_HOME=/usrdata/usr/data/arstack
cp -r $WORK_HOME/web /tmp/
cp $WORK_HOME/librsface.so /tmp/
cp $WORK_HOME/libchipid.so /tmp/
cp $WORK_HOME/arotp.ko    /tmp/
cp $WORK_HOME/shutdown.sh    /tmp/
cp $WORK_HOME/emmc_shutdown    /tmp/

export LD_LIBRARY_PATH=$WORK_HOME:$LD_LIBRARY_PATH
insmod /tmp/arotp.ko
cp -rf $WORK_HOME/web /tmp/
$WORK_HOME/web/upgrade_service &
$WORK_HOME/systemd /factory/web_boot.ini --test&
sleep 5
$WORK_HOME/web/lighttpd/sbin/lighttpd -m $WORK_HOME/web/lighttpd/lib -f $WORK_HOME/web/lighttpd/config/lighttpd_normal.conf
sleep 1
cp /usrdata/usr/data/arstack/ar_wdt_service 	 /tmp
/tmp/ar_wdt_service -t 5 >/dev/null 2>&1 &
sleep 1
exit
