#!/bin/sh
echo "run ..."

# start wifi
/var/as_start.sh &
#as_ap.sh

camera --start_vin_driver -mpi -af_stats 1

servicemanager &

test_mpp_vio -index 2307 -dpcm 1 &

# tunning mode:
#test_mpp_vio -index 2307 -dpcm 2 &
