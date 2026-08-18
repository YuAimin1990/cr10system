#!/bin/sh

if [ ! -n "$1" ]; then
	echo invalid argument
	exit
fi

cpu_pll_loopdiv_addr=0xa0000000
cpu_pll_loopdiv_val=0

echo Set CPU frequence to $1MHz
if [ $1 -gt 2550 ] || [ $1 -lt 40 ]; then
	echo Not support frequence $1
else
	cpu_pll_loopdiv_val=$(($1 / 10))
	echo DIV: $cpu_pll_loopdiv_val
fi

reg_val=`devmem $cpu_pll_loopdiv_addr`
echo Reg value before change $reg_val

reg_val=$((reg_val & 0xffffff00))
reg_val=$((reg_val | cpu_pll_loopdiv_val))

devmem $cpu_pll_loopdiv_addr 32 $reg_val

reg_val=`devmem $cpu_pll_loopdiv_addr`
echo Reg value after change $reg_val
