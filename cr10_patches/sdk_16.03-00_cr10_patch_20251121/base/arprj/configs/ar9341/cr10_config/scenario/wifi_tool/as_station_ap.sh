#!/bin/sh
if [ ! -d "/var/wpa_supplicant/" ]; then
    mkdir -p /var/wpa_supplicant
fi

ifconfig wlan0 up
sleep 1

./run/wpa_supplicant -iwlan0 -Dnl80211 -c./wpa_supplicant.conf &
sleep 1

udhcpc -b -i wlan0 -s /etc/udhcpc.script -x lease:300
 
### scan ap
#./wpa_cli -p /var/wpa_supplicant scan
#./wpa_cli -p /var/wpa_supplicant scan_resultsifconfig -a