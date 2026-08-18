#!/bin/sh

#set -x
if [ ! -f $1 ] || [ $# -ne 2 ];then
	echo "args error for $0"
	exit 1
fi

if [ -f $2 ]; then
	rm $2
fi

flash_part=0
upgrade=n
filesize=0
partsize=0
dualpart=n
end=-1
bootloader=n
blocksize=0

while read line
do
	#delete space in line
	line=`echo $line | sed 's/[[:space:]]//g'` 1>/dev/null 2>&1
	echo $line | grep "\[*\]" | grep "partition" 1>/dev/null 2>&1
	if [ $? = 0 ]; then
		if [ $upgrade = y -a $path = "" ]; then
			echo "error: upgrade = y, but no path field!!!"
			exit 1
		fi
		if [ $upgrade = n ]; then
			filesize=0
		fi
		let start=end+1
		let end=start+partsize/blocksize-1
		let size=filesize/blocksize
		let mod=filesize%blocksize
		if [ $mod != 0 ]; then
			let size=size+1
		fi
		echo $start,$end,$size,0xffffffff >> $2

		if [ $dualpart = y ]; then
			let start=end+1
			let end=start+partsize/blocksize-1
			let size=filesize/blocksize
			let mod=filesize%blocksize
			if [ $mod != 0 ]; then
				let size=size+1
			fi
			echo $start,$end,$size,0xffffffff >> $2
		fi
		# spl and gpt
		if [ $bootloader = y ]; then
			bootloader=n
			for i in `seq 3`
			do
				let start=end+1
				let end=start+partsize/blocksize-1
				echo $start,$end,$size,0xffffffff >> $2
			done
			# gpt
			let start=end+1
			let end=start+3
			let size=1
			echo $start,$end,$size,0xffffffff >> $2
		fi
		flash_part=0
		upgrade=n
		filesize=0
		partsize=0
		dualpart=n
		continue
	fi

	echo $line | grep "\[*\]" | grep "bootloader" 1>/dev/null 2>&1
	if [ $? = 0 ]; then
		bootloader=y
	fi

	#get field path=, size=, upgrade=, or flash_part=,
	field=`echo $line | cut -d = -f 1`

	if [ "$field" = "upgrade" ]; then
		upgrade=`echo $line | cut -d = -f 2`
	fi

	if [ "$field" = "path" ]; then
		path=`echo $line | cut -d = -f 2`
		filename=$(basename $path)
		ubifilename=`find ./release/  -name "$filename.ubi"`
		if [ $ubifilename ]; then
			filesize=`du -b $ubifilename | awk '{print $1}'`
		else
			filesize=`du -b $path | awk '{print $1}'`
		fi
	fi

	if [ "$field" = "size" ]; then
		partsize=`echo $line | cut -d = -f 2`
	fi

	if [ "$field" = "dual_part" ]; then
		dualpart=`echo $line | cut -d = -f 2`
	fi

	if [ $bootloader = y ] && [ $partsize = "0x40000" ]; then
		blocksize=0x20000
	elif [ $bootloader = y ] && [ $partsize = "0x80000" ]; then
		blocksize=0x40000
	fi
done < $1

# the last partition
if [ $upgrade = y -a $path = "" ]; then
	echo "error: upgrade = y, but no path field!!!"
	exit 1
fi
if [ $upgrade = n ]; then
	filesize=0
fi
let start=end+1
let end=start+partsize/0x20000-1
let size=filesize/0x20000
let mod=filesize%0x20000
if [ $mod != 0 ]; then
	let size=size+1
fi
echo $start,$end,$size,0xffffffff >> $2

if [ $dualpart = y ]; then
	let start=end+1
	let end=start+partsize/0x20000-1
	let size=filesize/0x20000
	let mod=filesize%0x20000
	if [ $mod != 0 ]; then
		let size=size+1
	fi
	echo $start,$end,$size,0xffffffff >> $2
fi
