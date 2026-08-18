#!/bin/sh

#set -x

#$1: the application name, $2: signal if exsit, for example, kill_apps "app" -15
function kill_apps()
{
	pidof $1 	1>/dev/null 2>&1
	#process is exist
	if [ $? -eq 0 ]; then
		for i in `seq 1 3`
		do
			killall -q $2 $1
			sleep 2
			pidof $1 	1>/dev/null 2>&1
			#process is not exist
			if [ $? -ne 0 ]; then
				return;
			fi
		done

		echo "kill $1 failed"
		exit 1
	fi
}

# login by ssh cannot get the exported enviroment varibale flagfile
flagfile=`grep -ow "flagfile=.*sirius-clean-system-flag" /proc/cmdline | cut -d = -f 2`

if [ ! -e $flagfile ]; then
	touch $flagfile
	if [ $? != 0 ]; then
		echo "create $flagfile failed, cannot enter clean system"
		exit 1
	fi
	if [ $# == 1 ]; then
		# eg: web:/usrdata/xx.sh
		echo "$1" > $flagfile
	fi
fi

sync

# reboot use da9061 watchdog
i2cset -y -f 0 0x58 0x11 1 &
sleep 5
echo "reboot by da9062 failed, reboot by yourself"

# ignore now, waiting for soc watchdog works
if [ -e "/usrdata/bin/ar_wdt_service" ]; then
	kill_apps "ar_wdt_service"
	/usrdata/bin/ar_wdt_service -t 1 1>/dev/null 2>&1 &
	if [ $? != 0 ]; then
		echo "reboot system failed, you should reboot by yourself"
		exit 1
	fi
	sleep 1
	# kill ar_wdt_service reboot
	kill_apps ar_wdt_service
else
	echo "ar_wdt_service is not exsit, can not reboot the system,"
	echo "you should reboot by yourself"
	exit 1
fi

exit 0
