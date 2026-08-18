#!/bin/sh

case $ACTION in
	add)
		mount=0
		for i in `seq 1 10`
		do
			if [ -e "/dev/${MDEV}p${i}" ]; then
				if [ ! -d "/mnt/sdcard_${MDEV}p${i}" ]; then
					mkdir /mnt/sdcard_${MDEV}p${i}
				fi

				mount /dev/${MDEV}p${i} /mnt/sdcard_${MDEV}p${i}
				if [ $? -ne 0 ]; then
					echo "mount /dev/${MDEV}p${i} to /mnt/sdcard_${MDEV}p${i} failed!"
				else
					$mount=1;
				fi
			fi
		done

		if [ $mount -eq 0 ]; then
			if [ ! -d "/mnt/sdcard_${MDEV}" ]; then
				mkdir /mnt/sdcard_${MDEV}
			fi
			mount /dev/${MDEV} /mnt/sdcard_${MDEV}
			if [ $? -ne 0 ]; then
				echo "mount /dev/${MDEV} to /mnt/sdcard_${MDEV} failed!"
			fi
		fi
		;;
	remove)
		umount=0
		for i in `seq 1 10`
		do
			if [ -d "/mnt/sdcard_${MDEV}p${i}" ]; then
				umount /mnt/sdcard_${MDEV}p${i}
				if [ $? -ne 0 ]; then
					echo "umount /mnt/sdcard_${MDEV}p${i} failed!"
				else
				$umount = 1
				fi
			fi
		done

		if [ $umount -eq 0 ]; then
			if [ -d "/mnt/sdcard_${MDEV}" ]; then
				umount /mnt/sdcard_${MDEV}
				if [ $? -ne 0 ]; then
					echo "umount /mnt/sdcard_${MDEV} failed!"
				fi
			fi
		fi
		;;
esac
