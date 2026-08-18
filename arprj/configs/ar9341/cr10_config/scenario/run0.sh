#!/bin/sh
echo "run ..."

# start wifi
/var/as_start.sh &
#as_ap.sh

camera --start_vin_driver -mpi -af_stats 1

servicemanager &

# usb rndis network
#/etc/usb_gadget_configfs.sh rndis src dwc2_0 0x1d6b 0x0101

# TUNNING_MODE=Y: isp tunning mode, N: normal mode
TUNNING_MODE=Y

if [ $TUNNING_MODE == "Y" ]; then
    # isp tunning mode
    test_mpp_vio -index 2307 -tunning &
    ar_iqtool_service &
else
    # normal mode
    test_mpp_vio -index 2307 &
fi

# open ir light
echo 56 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio56/direction
echo 1 > /sys/class/gpio/gpio56/value
