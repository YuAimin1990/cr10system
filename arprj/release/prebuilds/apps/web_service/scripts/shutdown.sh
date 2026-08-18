#!/bin/sh
sleep 1 && i2cset -y -f 0 0x58 0x11 1 &