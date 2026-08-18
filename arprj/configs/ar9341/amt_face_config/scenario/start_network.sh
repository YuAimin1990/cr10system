#!/bin/sh
# configure network: set mac addr, dhcp

sysctl -p /etc/init.d/sysctl.conf &

if [[ ! -z $val_nw_itf_name ]]; then
	ifconfig $val_nw_itf_name up && \
	echo 1 >/proc/sys/net/ipv6/conf/$val_nw_itf_name/disable_ipv6

	if [[ ! -z $val_nw_itf_mac ]]; then
		ifconfig $val_nw_itf_name hw ether $val_nw_itf_mac
	fi

	if [[ ! -z $val_nw_itf_ip ]]; then
        ifconfig $val_nw_itf_name $val_nw_itf_ip
    else
        /sbin/udhcpc -b -i $val_nw_itf_name -s /etc/udhcpc.script -x lease:300
    fi
fi
