#!/bin/sh

# ignore Makefile, makefile.mk, rule.mk

target=""
configfile=""

if [ $# != 1 ]; then
	echo "Usage:"
	echo "  $0 pathname"
	echo ""
	echo "Attention:"
	echo " 	it will ignore the file Makefile makefile.mk rule.mk if exsit"
	echo " 	you must delete config firstly, be careful"
	echo ""
	exit 1
fi

target=$(basename $1)
configfile=$1/$target"_config"

if [ -e "$configfile" ]; then
	echo "$configfile is exsit, if you want regenerate, please delete this file first, be careful!!!"
	exit 1
fi

echo "generate config for directory: $1"

# add menu xxx
echo "menu $target selection" > $configfile
echo "" >> $configfile

ls $1 | awk -v tgt=$target '{ if ($0 == "Makefile" || $0 == "rule.mk" || $0 == "makefile.mk") {next} \
		printf("config %s_%s\n", toupper(tgt), $0); \
		printf("    bool \"CONFIG_%s_%s\"\n", toupper(tgt), $0); \
		printf("    default n\n"); \
		printf("    help\n"); \
		printf("    %s %s application support\n", tgt, $(0)); \
		printf("\n")}' >> $1/$target"_config"

# add endmenu
echo "endmenu" >> $configfile
echo "$configfile generate success"
exit 0

