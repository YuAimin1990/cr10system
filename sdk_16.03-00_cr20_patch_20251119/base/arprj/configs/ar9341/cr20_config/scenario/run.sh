#!/bin/sh
echo "run ..."

# start wifi
/var/as_start.sh &
#as_ap.sh

camera --start_vin_driver -mpi -af_stats 1

servicemanager &

# test display
test_mpp_vo 12 &
