#!/bin/sh
echo "reset elfe1..."

gpio_config -e B0_2
gpio_config -s B0_2 -d output -l low
usleep 10000
gpio_config -s B0_2 -d output -l high