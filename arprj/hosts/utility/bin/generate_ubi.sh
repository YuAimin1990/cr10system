
#function
getKeyAndValues()
{
        inifile="$1"
        section="$2"
        key="$3"
        if [ $# -ne 3 ] || [ ! -f ${inifile} ]
        then
                echo "wrong parameter or no inifile ${inifile}"
		exit -1
        else
		values=$(sed -n '/'name=$section'/,/^$/p' $inifile | sed 's/ /\n/g' | sed -n '/'$key'/p')
                echo ${values}
        fi
}

getKeysValues()
{
	inifile="$1"
	section="$2"
	key="$3"
	if [ $# -ne 3 ] || [ ! -f ${inifile} ]
	then
		echo "ini file [${inifile}]"
		exit -1
	else
		values=$(sed -n '/'name=$section'/,/^$/p' $inifile | sed 's/ /\n/g' | sed -n '/'$key'/p' | cut -d "=" -f2)
		echo ${values}
	fi
}

writeHeadToIni()
{
	file=$1
	if [ -f $file ];then
		rm $file
	fi
	touch $file
	echo "[ubi-volume]" > $file
}
writeInfoToIni()
{
	file=$1;key=$2;value=$3
	if [ x$value != x"" ];then
		echo $key"="$value >> $file 
	fi
}
writeTailToIni()
{
	file=$1
	echo "vol_flags=autoresize" >> $file
	echo "vol_id=0" >> $file
	echo "vol_type=dynamic" >> $file
}
removeIni()
{
	file=$1
	rm $file
}

#check inifile exist
if [ ! -f $1 ];then
	echo "no ini file"
	exit -1
fi

if [ x`getKeyAndValues $1 $2 name` != x"" ];then
	vol_name=`getKeysValues $1 $2 name`
	echo "vol name = $vol_name"
else
	echo "no vol name"
	exit -1
fi

if [ x`getKeyAndValues $1 $2 size` != x"" ];then
	vol_size=`getKeysValues $1 $2 size`
	echo "vol size = $vol_size"
else
	echo "use auto size"
fi

if [ x`getKeyAndValues $1 $2 mode` != x"" ];then
	mode=`getKeysValues $1 $2 mode`
	echo "vol mode = $mode"
	
else
	mode=ubi
	echo "use default mode ubi"
fi

if [ x`getKeyAndValues $1 $2 path` != x"" ];then
	image=`getKeysValues $1 $2 path`
	if [ $# -ge 3 ]; then \
		image=$image.$3; \
	fi
	echo "input image = $image"
else
	echo "no path"
	exit -1
fi

#define output file
outputfile=$image".ubi"
echo "output file = $outputfile"

#generate config ini file
tmpinifile=tmp_segment.ini
writeHeadToIni $tmpinifile
writeInfoToIni $tmpinifile "mode" $mode
writeInfoToIni $tmpinifile "vol_name" $vol_name
#writeInfoToIni $tmpinifile "vol_size" $vol_size
writeInfoToIni $tmpinifile "image" $image
writeTailToIni $tmpinifile
echo "tmp cfg file : $tmpinifile :"
cat $tmpinifile

#use ubinize to generate ubi
ubinize -o $outputfile -p 128KiB -m 2048 -x 512 -O 2048 -x 1 -v $tmpinifile
echo "ubinize generate $outputfile done"

#remove tmp ubi cfg file
removeIni $tmpinifile
echo "removed tmp cfg file $tmpinifile"
#print outputfile path(from arsdk path)
echo -e "${outputfile}\c"









