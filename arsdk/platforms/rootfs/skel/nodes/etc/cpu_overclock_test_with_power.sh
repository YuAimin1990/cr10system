#######fix voltage#######
#1.00v
let voltage=90
let voltage_tmp=voltage*10
let voltage=voltage-30
voltage=`printf "%X\n" $voltage`

i2cset -f -y 0 0x58 0x0a3 0x$voltage
i2cset -f -y 0 0x58 0x0a4 0x$voltage

#######set idle cpu freq#######
#devmem 0x010e0000 32 0x0100

#######get idle current info#######
let r_shunt=10
let current_lsb=1
let cali=40960/r_shunt/current_lsb

cali_hex=`printf '%x' $cali`
cali_hex=`echo 0x${cali_hex:2:2}${cali_hex:0:2}`
echo cali: $cali cali_hex: $cali_hex
i2cset -f -y 0 0x40 0x05 $cali_hex w

v=`i2cget -f -y 0 0x40 0x04 w`
let current=`echo 0x${v:4:2}${v:2:2}`

r_v=`i2cget -f -y 0 0x40 0x02 w` 
let real_volt=`echo 0x${r_v:4:2}${r_v:2:2}`
let real_volt=real_volt/2

echo voltage :0x$voltage:$voltage_tmp mV, far-end real voltage: $real_volt 
echo current $current mA : raw: `echo 0x${v:4:2}${v:2:2}`
echo idle power $(($current*$voltage_tmp/1000)) mW, far-end idle power $(($current*$real_volt/1000)) mW

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

#$1 sample count
let max_current=0
let avg_current=0
let max_power=0
let avg_power=0
function get_current_info()
{
	let sample_cnt=${1}
	let max_current=0
	let avg_current=0
	let max_power=0
	let avg_power=0
	for i in $(seq 1 $sample_cnt)
	do
		v=`i2cget -f -y 0 0x40 0x04 w`
		p=`i2cget -f -y 0 0x40 0x03 w`
		let current=`echo 0x${v:4:2}${v:2:2}`
		let power=`echo 0x${p:4:2}${p:2:2}`
		let power=power*20
		let avg_current=avg_current+current
		let avg_power=avg_power+power
		if [ $current -gt $max_current ]
		then
			let max_current=current
		fi
		if [ $power -gt $max_power ]
		then
			let max_power=power
		fi
	done
	let avg_current=avg_current/sample_cnt
	let avg_power=avg_power/sample_cnt
}

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

	#######get current info#######
	get_current_info 100
	r_v=`i2cget -f -y 0 0x40 0x02 w` 
	let real_volt=`echo 0x${r_v:4:2}${r_v:2:2}`
	let real_volt=real_volt/2
	echo max current $max_current mA, avg current $avg_current mA,  far-end real volt $real_volt mV
	echo near-end:max power $(($max_current*$voltage_tmp/1000)) mW, avg power $(($avg_current*$voltage_tmp/1000)) mW
	echo far-end :max power $max_power mW, avg power $avg_power mW

	let times=times+1

	sleep 1
done
