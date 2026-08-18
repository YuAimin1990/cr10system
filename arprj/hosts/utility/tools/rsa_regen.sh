#!/bin/bash

public_cfile_path=../../../../arsdk/platforms/uboot/mods/u-boot-2020.10/security/keys
upgrade_pem_path=../../../../arsdk/platforms/rootfs/skel/nodes/etc/artosyn_upgrade_public_rsa2048.pem

usage(){
    echo "Usage: $0 keyname"
    echo "example: $0 vendor"
    echo "example: $0 bl31"
    echo "example: $0 uboot"
    echo "example: $0 env"
    echo "example: $0 kernel"
    echo "example: $0 upgrade"
}
if [ $# != 1 ];then
    usage
    exit 1
fi

case $1 in
    vendor|bl31|uboot|env|kernel|upgrade)
    echo "Regenerating $keyname RSA key"
    ;;
    *)
    echo "Unknown RSA key name $1"
    usage
    exit 1
esac

keyname=$1
pem_path=../data/rsakey_$keyname

../bin/rsagenkey 2048
#rm useless file
rm rsakey2048.private.bin
#rename public pem
mv rsakey2048.public.pem ${pem_path}/rsakey2048_${keyname}_public.pem
#rename private pem
mv rsakey2048.private.pem ${pem_path}/rsakey2048_${keyname}_private.pem
#rename public .c file
mv rsakey2048.public.c ${pem_path}/rsakey2048_${keyname}_public.c
#modify array name in rsa public .c file
sed -i "s/external/$keyname/g" ${pem_path}/rsakey2048_${keyname}_public.c

case $1 in
    vendor|bl31|uboot|env|kernel|upgrade)
    cp ${pem_path}/rsakey2048_${keyname}_public.c $public_cfile_path
    ;;&
    upgrade)
    cp ${pem_path}/rsakey2048_${keyname}_public.pem $upgrade_pem_path
    ;;
esac

exit 0
