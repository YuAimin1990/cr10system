#!/bin/sh
echo "start..."

if [ -e "/etc/init.d/start_common.sh" ]; then
	. /etc/init.d/start_common.sh
fi

insmod /mod/ar_mpp_drv.ko; 
insmod /mod/ar_osal.ko anony=1 mmz_allocator=hisi mmz=anonymous,0,0x80000000,0x80000000; 
insmod /mod/ar_vb.ko;
insmod /mod/ar_sys.ko
insmod /mod/ar_mpp_proc_ctrl.ko
insmod /mod/ar_scaler.ko
insmod /mod/ar_ifc.ko
insmod /mod/ar_npu.ko
insmod /mod/ar_gdc.ko
insmod /mod/ar_cipher.ko
#insmod /mod/ar_efuse.ko

# increase scaler freq for 8x display
devmem 0x0107003C 32 0xfff1fbf5

if [ -e "/etc/init.d/start_ssh.sh" ]; then
	. /etc/init.d/start_ssh.sh
fi

#if [ -e "/etc/init.d/start_npu.sh" ]; then
#	. /etc/init.d/start_npu.sh
#fi

if [ -e "/etc/init.d/start_network.sh" ]; then
	. /etc/init.d/start_network.sh
fi

if [ -e "/etc/init.d/start_app.sh" ]; then
	. /etc/init.d/start_app.sh
fi

if [ -e "/usrdata/run.sh" ]; then
	. /usrdata/run.sh &
fi

