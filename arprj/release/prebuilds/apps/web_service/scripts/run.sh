#!/bin/sh
echo "run ......."

WORK_HOME=/usrdata
export LD_LIBRARY_PATH=$WORK_HOME/lib:$LD_LIBRARY_PATH

insmod /mod/ar_mpp_drv.ko
insmod /mod/ar_gdc.ko

ifconfig lo up
ifconfig eth0 up
ifconfig eth0 192.168.1.100 netmask 255.255.255.0

mpp_service.app --dts /local/board-cfg.dtb &

sleep 6

if [ ! -e /factory/web_boot.ini ]; then
    cp -rf $WORK_HOME/resources/web_boot.ini /factory/web_boot.ini
fi

function start_web(){
	cp -r $WORK_HOME/web /tmp/
	sleep 5
	/tmp/web/lighttpd/sbin/lighttpd -m /tmp/web/lighttpd/lib -f /tmp/web/lighttpd/config/lighttpd_normal.conf
	return 0
}

#/sbin/udhcpc -b -i eth0 -s /etc/udhcpc.script -x lease:300 &
telnetd &

echo "start ipcam service"
export LD_LIBRARY_PATH
$WORK_HOME/bin/ipcam_service &

live555MediaServer -c /usr/conf/live555_ipcam.config &

echo "start systemd"
#$WORK_HOME/bin/systemd /factory/web_boot.ini&

start_web
exit
