#######fix voltage#######
#1.00v
let voltage=100
let voltage_tmp=voltage*10
let voltage=voltage-30
voltage=`printf "%X\n" $voltage`

echo voltage :0x$voltage:$voltage_tmp mV

i2cset -f -y 0 0x58 0x0a3 0x$voltage
i2cset -f -y 0 0x58 0x0a4 0x$voltage

#######run cpu test and memory test#######
chmod -R 777 /etc/*
/etc/cpuburn-a53 &
/etc/memtester 100M > /dev/null &
#/etc/memtester 100M > /dev/null &
#/etc/memtester 100M > /dev/null &
#/etc/memtester 100M > /dev/null &

#######open temperature#######
devmem 0x01072000 32 0x1
let tmp=`devmem 0x01072038`
#<6:4> = 111
let mask=0x70
let val=$(($tmp|$mask))
devmem 0x01072038 32 $val

let times=1
#cpu sign off 900Mhz clock
let cpu_freq=0x5A

while true
do
	let temp0=`devmem 0x010720cc`
	let temp1=`devmem 0x010720d0`
	let mask=0x01
	let temp1=$(($temp1&$mask))
	let temp=temp1*256+temp0
	let val=$(($temp*5320-1378400))
	
	echo `date` raw:$temp temp:$val
	
	val=$(( $times % 10 )) 
	if [ $val == 0 ] ; then  
		let cpu_freq=cpu_freq+1
		echo set cpu_freq $(($cpu_freq*10)) Mhz 
		let cpu_freq_hex=cpu_freq*256+0x2000000
		cpu_freq_hex=`printf "%X\n" $cpu_freq_hex`
		echo cpu_freq 0x$cpu_freq_hex
		devmem 0x010e0000 32 0x$cpu_freq_hex
	fi
	let times=times+1

	sleep 1
done
