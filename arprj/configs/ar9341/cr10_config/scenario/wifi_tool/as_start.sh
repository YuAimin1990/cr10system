#!/bin/sh
echo 32 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio32/direction
echo 1 > /sys/class/gpio/gpio32/value
echo 32 > /sys/class/gpio/unexport
sleep 1
echo 45 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio45/direction
echo 1 > /sys/class/gpio/gpio45/value
echo 45 > /sys/class/gpio/unexport
sleep 1

insmod /mod/ssv6x5x.ko stacfgpath=/var/ssv6x5x-wifi.cfg
sleep 1


