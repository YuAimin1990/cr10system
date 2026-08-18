#!/bin/bash

# troot firmware
python bin2hex.py ./firmware/firmware.le fw.hex 32768 128
#./bin_for_ddr.pl fw.hex 0x17d0000
./bin_for_ddr.pl fw.hex 0x31000000
mv ddr4_init_vel_32bit_file0 troot.fw.ddr.0
mv ddr4_init_vel_32bit_file1 troot.fw.ddr.1

#signature 
python bin2hex.py ./tools/sboot_chains/M1_hdr.bin sign.hex 32768 128
#./bin_for_ddr.pl sign.hex 0x17E9800
./bin_for_ddr.pl sign.hex 0x31019800
mv ddr4_init_vel_32bit_file0 sign.ddr.0
mv ddr4_init_vel_32bit_file1 sign.ddr.1

#spl 
python bin2hex.py ./samples/u-boot-spl.bin spl.hex 32768 128
#./bin_for_ddr.pl spl.hex 0x17E9c00
./bin_for_ddr.pl spl.hex 0x31019c00
mv ddr4_init_vel_32bit_file0 spl.ddr.0
mv ddr4_init_vel_32bit_file1 spl.ddr.1
