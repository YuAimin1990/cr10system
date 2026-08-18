#!/bin/sh
echo "guide demo ..."

setGPIORegFunc(){
        echo "gpio set func $1 $2 $3"
        reg=$(devmem $1)
        let regv=$reg
        offset=$2
        func=$3
        bitmask=0x7
        let 'regv=regv|(bitmask<<offset)'
        mask=0xffffffff
        let 'func=(~func&0x7)<<offset'
        let 'regv=regv&(mask^func)'
        regvs=$(printf "0x0%X\n" $regv)
        #echo $reg to $regvs
        devmem $1 32 $regvs
}

#set gpio pinshare register
setGPIORegFunc 0x0107307C 0 0
setGPIORegFunc 0x0107307C 10 0
setGPIORegFunc 0x0107307C 20 0
setGPIORegFunc 0x01073080 0 0
setGPIORegFunc 0x01073080 10 0
setGPIORegFunc 0x01073080 20 0

if [ ! -e "/dev/ar_efuse" ]; then
  insmod /mod/ar_efuse.ko
fi

export LD_LIBRARY_PATH=/usrdata/guide_demo/:$LD_LIBRARY_PATH

ar_iqtool_service &

USE_DUAL_ISP=Y

if [ $USE_DUAL_ISP != Y ]; then
cp /usrdata/local/factory/tunning/cam_common_inf/infrared_timo256_tuning_preview.bin /usrdata/local/factory/tunning/cam_common_inf/common_infrared_tuning_preview.bin
else
cp /usrdata/local/factory/tunning/cam_common_inf/infrared_timo256_tuning_preview_isp0.bin /usrdata/local/factory/tunning/cam_common_inf/common_infrared_tuning_preview.bin
cp /usrdata/local/factory/tunning/cam_common_inf/infrared_timo256_tuning_preview_isp1.bin /usrdata/local/factory/tunning/cam_common_inf/common_infrared_tuning_preview1.bin
fi

sync

test_timo256_raw -o 1&
