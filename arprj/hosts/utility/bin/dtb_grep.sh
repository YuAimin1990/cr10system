# $(1) Project output img dir 
# $(2) Global output dir
# $(3) Path menu.config file
# $(4) Path to store ddr dtb

echo =====dtb_grep.sh $1 $2 $3 $4 ======

mkdir -p $1;
cp $2/image/u-boot-spl.dtb $1/u-boot-spl-raw.dtb;
cp $2/image/u-boot-spl-nodtb.bin $1/;

type=`cat $3 | grep "=y" | grep CONFIG_DDR_TYPE | cut -d ':' -f 2 | cut -d '=' -f 1 | cut -d '_' -f 4 | tr '[A-Z]' '[a-z]'`;
freq=`cat $3 | grep "=y" | grep CONFIG_DDR_FREQ | cut -d ':' -f 2 | cut -d '=' -f 1 | cut -d '_' -f 4`;
width=`cat $3 | grep "=y" | grep CONFIG_DDR_WIDTH | cut -d ':' -f 2 | cut -d '=' -f 1 | cut -d '_' -f 4`;
qos=`cat $3 | grep "=y" | grep CONFIG_DDR_QOS | cut -d ':' -f 2 | cut -d '=' -f 1 | cut -d '_' -f 4 | tr '[A-Z]' '[a-z]'`;
density=`cat $3 | grep "=y" | grep CONFIG_DDR_DENSITY | cut -d ':' -f 2 | cut -d '=' -f 1 | cut -d '_' -f 4`;
version=`cat $3 | grep "=y" | grep CONFIG_DDR_VERSION | cut -d ':' -f 2 | cut -d '=' -f 1 | cut -d '_' -f 4 | tr '[A-Z]' '[a-z]'`;

cd $1;
$2/utility/bin/fdtgrep -O dtb -o u-boot-spl-grep.dtb \
	-b $type,$freq,$width,$version -b ddr,qos,$type,$qos -b ddr,gconfig,$density -b root u-boot-spl-raw.dtb;
spl_dtb_size_str=`ls -l u-boot-spl-grep.dtb | awk '{print $5}' | bc`;
dd if=/dev/zero of=u-boot-spl-dtb-pad.bin bs=1 count=$[(4-$spl_dtb_size_str%4)%4] 2>/dev/null;
echo "config ddr to $type $freq $width $qos $density u-boot-spl-grep.dtb size $spl_dtb_size_str bytes";
cp u-boot-spl-grep.dtb $4/$type-$freq-$width-$qos-$density-$version.dtb

cat u-boot-spl-grep.dtb u-boot-spl-dtb-pad.bin > u-boot-spl-align.dtb;
cat u-boot-spl-nodtb.bin u-boot-spl-align.dtb > u-boot-spl-final.bin;
python2 $2/utility/bin/gen_packed_bootloader.py \
	--out_file=troot_spl.nonsec.img \
	--spl_file=u-boot-spl-final.bin \
	--spl_nodtb_file=u-boot-spl-nodtb.bin \
	--spl_addr=0x30010000;


rm u-boot-spl-raw.dtb;
rm u-boot-spl-nodtb.bin;
rm u-boot-spl-align.dtb;
rm u-boot-spl-grep.dtb;
rm u-boot-spl-dtb-pad.bin;
