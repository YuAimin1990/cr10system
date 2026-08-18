#!/bin/sh
echo "start..."

if [ -e "/etc/init.d/start_common.sh" ]; then
	. /etc/init.d/start_common.sh
fi

insmod /mod/ar_mpp_drv.ko; 
insmod /mod/ar_osal.ko anony=1 mmz_allocator=hisi mmz=anonymous,0,0x40000000,0x20000000; 
insmod /mod/ar_vb.ko;
insmod /mod/ar_sys.ko
insmod /mod/mpp_proc.ko
insmod /mod/ar_cipher.ko

if [ -e "/etc/init.d/start_ssh.sh" ]; then
	. /etc/init.d/start_ssh.sh
fi

#if [ -e "/etc/init.d/start_npu.sh" ]; then
#	. /etc/init.d/start_npu.sh
#fi

#if [ -e "/etc/init.d/start_network.sh" ]; then
#	. /etc/init.d/start_network.sh
#fi

if [ -e "/etc/init.d/start_app.sh" ]; then
	. /etc/init.d/start_app.sh
fi

if [ -e "/usrdata/run.sh" ]; then
	. /usrdata/run.sh &
fi



