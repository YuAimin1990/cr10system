#!/bin/sh
sleep 1
dir_reg=0x08400028
dir_bit=0

out_reg=0x08400024
out_bit=0

#set dir to out
old=`devmem $dir_reg`
echo $old
new=$(($old|0x01<<$dir_bit));
echo "set dir reg to $new"
devmem $dir_reg 32 $new

#set wdt to low
old=`devmem $out_reg`
echo $old
new=$(($old&&$((~$((0x01<<$out_bit))))));
echo "set out reg to $new"
devmem $dir_reg 32 $new
