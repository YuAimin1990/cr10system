#!/bin/bash

function __readFile() {
  awk -F '=' -v section="[$2]" -v key="$3" '
  $0==section     { flag=1; next }\
  /\[/            { flag=0; next }\
  flag && $1==key { print $2 }' $1
}

if [[ $# -lt 3 ]] || [[ ! -e $1 ]];then
    echo "Info: No configuration file for crypt. Exit."
    exit 0
fi

dm_crypt_file=$1      #./configs/$(CHIP)/common/dm_crypt_cfg.config
flash_layout_file=$2  #./configs/$(CHIP)/evb_config_debug/plat/flash_layout_emmc.ini
rootfs_dir=$3

board_path=${dm_crypt_file#*configs/}
chip=${board_path%%/*}

crypt_key_size=($( __readFile $dm_crypt_file DM_CRYPT_CFG key_size ))
crypt_key_cipher=($( __readFile $dm_crypt_file DM_CRYPT_CFG key_value ))
cipher_len_bit=`expr ${#crypt_key_cipher} \* 4`
vol_name=""
encrypt="n"

for partition in `awk '/^\[.*\]/' ${flash_layout_file}`;
do
    vol_name=($( __readFile ${flash_layout_file} ${partition:1:-1} name ))
    encrypt=($( __readFile ${flash_layout_file} ${partition:1:-1} encrypt ))
    if [ x"$encrypt" == "xy" ];then
        break
    fi
done

#echo "CRYPT_KEY_SIZE:"${crypt_key_size}
#echo "CRYPT_KEY_CIPHER:"$crypt_key_cipher
#echo "vol_name:"$vol_name
#echo "encrypt:"$encrypt

script_on_board=./configs/$chip/common/tools/dm_crypt.sh
flag_on_board=./configs/$chip/common/tools/dm_crypt_init_flag
key_on_board=./configs/$chip/common/tools/dm_crypt_key

if [ x"$encrypt" != "xy" ];then
    # Clean the existing files
    rm -f $script_on_board
    rm -f $flag_on_board
    rm -f $key_on_board

    echo "Info: Erypt is disable on the volume. Exit."
    exit 0
fi

if [[ $crypt_key_size -ne 128 ]] && [[ $crypt_key_size -ne 256 ]];then
    echo "Error: Invalid crypt key size "$crypt_key_size". Valid value is 128 or 256. Exit."
	exit 1
fi

if [ $cipher_len_bit != $crypt_key_size ];then
    echo "Error: Mismatch between key_size "$crypt_key_size" and key_value length "$cipher_len_bit". Exit."
    exit 1
fi

#### Generate crypt scripts, key and init_flag on board ####
echo -n $crypt_key_cipher | xxd -r -p > $key_on_board

echo '#!/bin/sh' > $script_on_board
echo -e "if [ -e \"/dev/mmcblk0p`expr ${partition:10:-1} + 1`\" ]; then" >> $script_on_board
echo -e '    /etc/cryptsetup create '$vol_name' '/dev/mmcblk0p`expr ${partition:10:-1} + 1`' -d dm_crypt_key -s '$crypt_key_size' -c aes-cbc-essiv:sha256 --sector-size=4096' >> $script_on_board

cat >> $script_on_board << EOF
else
    exit 0
fi

if [ -e "/usrdata/tools/dm_crypt_init_flag" ]; then
EOF
echo -e '   /etc/mkfs.ext4 -j /dev/mapper/'$vol_name -b 4k >> $script_on_board
echo -e '   rm -f /usrdata/tools/dm_crypt_init_flag' >> $script_on_board
echo -e 'fi' >> $script_on_board

echo -e 'mount /dev/mapper/'$vol_name' /mnt/'$vol_name'' >> $script_on_board
echo '' > $flag_on_board

chmod 775 $script_on_board

result=$(echo $rootfs_dir | grep -b -o "rootfs")
if [[ "$result" != "" ]];then
    mkdir -p $rootfs_dir/mnt/$vol_name
fi
