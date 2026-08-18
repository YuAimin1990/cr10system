#!/bin/sh

if [[ ! -z $val_kern_log_level ]]; then
	echo $(($val_kern_log_level)) > /proc/sys/kernel/printk
fi

if [ -e /etc/dropbear/dropbear ]; then
	chmod +x /etc/dropbear/dropbear
	cp /etc/dropbear/dropbear /tmp/
	/tmp/dropbear &
fi

if [ -e /etc/dropbear/scp ]; then
	chmod +x /etc/dropbear/scp
	cp /etc/dropbear/scp /usr/bin/
fi