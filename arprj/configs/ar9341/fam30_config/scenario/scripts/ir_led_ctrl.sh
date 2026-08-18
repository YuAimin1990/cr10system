#!/bin/sh

I2C_BUS=1
ADDR=0x63

# current value, 200mA:0x19, 300mA:0x25, 400mA:0x32, 500mA:0x3F, 600mA:0x4C
CURRENT=0x32

if [ "x""$1" == "x""on" ]; then
    i2cset -f -y $I2C_BUS $ADDR 0x02 0x00
    usleep 5000
    i2cset -f -y $I2C_BUS $ADDR 0x03 $CURRENT
    usleep 5000
    i2cset -f -y $I2C_BUS $ADDR 0x04 $CURRENT
    usleep 5000
    i2cset -f -y $I2C_BUS $ADDR 0x05 0x00
    usleep 5000
    i2cset -f -y $I2C_BUS $ADDR 0x06 0x00
    usleep 5000
    i2cset -f -y $I2C_BUS $ADDR 0x07 0x0B
    usleep 5000
    i2cset -f -y $I2C_BUS $ADDR 0x08 0x00
    usleep 5000
    i2cset -f -y $I2C_BUS $ADDR 0x01 0x27
elif [ "x""$1" == "x""off" ]; then
    i2cset -f -y $I2C_BUS $ADDR 0x01 0x24
elif [ "x""$1" == "x""geti" ]; then
    echo -n "get current reg: "
    i2cget -f -y $I2C_BUS $ADDR 0x03
    i2cget -f -y $I2C_BUS $ADDR 0x04
elif [ "x""$1" == "x""seti" ]; then
    [ $# != 2 ] && {
        echo "Usage: $0 seti 0x99"
        exit
    }
    echo "set current reg: $2"
    i2cset -f -y $I2C_BUS $ADDR 0x03 $2
    i2cset -f -y $I2C_BUS $ADDR 0x04 $2
elif [ "x""$1" == "x""dump" ]; then
    i2cget -f -y $I2C_BUS $ADDR 0x02
    usleep 5000
    i2cget -f -y $I2C_BUS $ADDR 0x03
    usleep 5000
    i2cget -f -y $I2C_BUS $ADDR 0x04
    usleep 5000
    i2cget -f -y $I2C_BUS $ADDR 0x05
    usleep 5000
    i2cget -f -y $I2C_BUS $ADDR 0x06
    usleep 5000
    i2cget -f -y $I2C_BUS $ADDR 0x07
    usleep 5000
    i2cget -f -y $I2C_BUS $ADDR 0x08
    usleep 5000
    i2cget -f -y $I2C_BUS $ADDR 0x01
else
    echo "Usage: $0 on/off/geti/seti/dump"
fi
