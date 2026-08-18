#!/bin/sh
echo "shutter open..."

gpio_config -e A3_1
gpio_config -e A3_2
gpio_config -s A3_1 -d output -l high
gpio_config -s A3_2 -d output -l low
usleep 20000
gpio_config -s A3_1 -d output -l low
gpio_config -s A3_2 -d output -l low