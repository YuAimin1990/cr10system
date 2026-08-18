#!/bin/bash


#para 1: ini file name
#para 2: full path name
function get_part_size_by_path()
{
	echo $1 $2 $# 1>/dev/null 2 >& 1
	if [ ! -f $1 ] || [ $# -ne 3 ];then
		echo "args error for get_part_size_by_path"
		return 0
	fi

	filename=$2

	while read line
	do
		#delete space in line
		line=`echo $line | sed 's/[[:space:]]//g'` 1>/dev/null 2>&1
		echo $line | grep "\[*\]" | grep "partition" 1>/dev/null 2>&1
		if [ $? = 0 ]; then
			partsize=0
			found=0
			ret=1
			continue
		fi

		#get field path=, or size=,
		field=`echo $line | cut -d = -f 1`
		if [ "$field" = "path" ] || [ "$field" = "path0" ] || [ "$field" = "path1" ]; then
			echo line=$line 1>/dev/null 2>&1
			echo filename=$filename 1>/dev/null 2>&1

			#if $filename in $line, return 0
			echo $line | grep $filename 1>/dev/null 2>&1
			if [ $? = 0 ]; then
				found=1
				if [ "$partsize" != 0 ]; then
					ret=0
					echo get partsize=$partsize 1>/dev/null 2>&1
					break;
				fi
			fi
		fi

		if [ "$field" = "size" ]; then
			partsize=`echo $line | cut -d = -f 2`
			if [ "$found" = 1 ]; then
				echo get partsize=$partsize 1>/dev/null 2>&1
				ret=0
				break
			fi
		fi
	done < $1

	echo "partition size of $3"=$partsize

	return $ret
}

get_part_size_by_path $1 $2 $3


