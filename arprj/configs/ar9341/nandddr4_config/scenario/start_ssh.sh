#!/bin/sh

if [[ ! -z $val_kern_log_level ]]; then
	echo $(($val_kern_log_level)) > /proc/sys/kernel/printk
fi

if [ -e /bin/dropbear ]; then
	dropbear &
fi
