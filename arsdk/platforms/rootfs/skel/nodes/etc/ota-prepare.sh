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

echo "ota prepare..."

kill_apps "ar_dbg_service"
kill_apps "mpp_service.app"

#umount partitions
for i in "/factory" "/usrdata"
do
	mountpoint -q $i
	#mountpoint is exist
	if [ $? -eq 0 ]; then
		umount $i 	1>/dev/null 2>&1
		if [ $? -ne 0 ]; then
			echo "umount $i failed"
			exit 1
		fi
	fi
done

mount -o remount -o ro /
echo "ota prepare complete"
