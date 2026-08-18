#!/bin/sh

#AUDIO_AMP_EN 
echo 30 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio30/direction
echo 1 > /sys/class/gpio/gpio30/value

#DISABLE OTG POWER BY DEFAULT
echo 102 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio102/direction
echo 0 > /sys/class/gpio/gpio102/value


mpp_service.app &
