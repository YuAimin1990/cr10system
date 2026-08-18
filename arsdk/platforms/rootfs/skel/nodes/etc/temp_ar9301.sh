#####temp enable#######
devmem 0x01072000 32 0x1

let tmp=`devmem 0x01072038`
#<6:4> = 111
let mask=0x70
let val=$(($tmp|$mask))
devmem 0x01072038 32 $val

while true
do
	let temp0=`devmem 0x010720cc`
	let temp1=`devmem 0x010720d0`
	let mask=0x01
	let temp1=$(($temp1&$mask))
	let temp=temp1*256+temp0
	let val=$(($temp*5320-1378400))
	
	echo `date` raw:$temp temp:$val

	sleep 1

done
