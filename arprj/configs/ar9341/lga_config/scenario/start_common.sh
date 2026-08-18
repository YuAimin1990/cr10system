#!/bin/sh
# script for common use at startup

START_CONF=/factory/start.conf

function read_param()
{
	conf=''
	[[ -f $START_CONF ]] && conf=$(grep $1 $START_CONF | sed -e "s/$1\s*\t*//g" | tr -d '\r\n')
	echo $conf
}

ulimit -c unlimited

if [[ ! -f $START_CONF ]]; then
	echo
	echo $START_CONF' not exist, please add it with your own config as following:'
	echo 'kern_log_level	1'
	echo 'nw_itf_name		eth0'
	echo 'nw_itf_mac		aa:bb:cc:dd:ee:ff'
	echo
else
	# common
	val_kern_log_level=$(read_param 'kern_log_level')

	# network
	val_nw_itf_name=$(read_param 'nw_itf_name')
	val_nw_itf_mac=$(read_param 'nw_itf_mac')
fi
