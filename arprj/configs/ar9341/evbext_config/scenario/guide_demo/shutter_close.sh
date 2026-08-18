#!/bin/sh
echo "shutter close..."

gpio_config -e A3_1
gpio_config -e A3_2
gpio_config -s A3_1 -d output -l low
gpio_config -s A3_2 -d output -l high
usleep 20000
gpio_config -s A3_1 -d output -l low
gpio_config -s A3_2 -d output -l low