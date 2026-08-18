ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

ARSDK_BASE_DTS_BINDINGS    := $(PRJDIR)/configs/common
ARSDK_KERNEL_DTSI          := $(PRJDIR)/configs/$(gBOARD_BASE_name)/tktool/kernel_dts

DTS_CFLAGS += -I$(ARSDK_BASE_DTS_BINDINGS) -I$(ARSDK_KERNEL_COMMON_DTSI)

ifneq ($(BOOT_MEDIA), nand)
DTS_CFLAGS += -DSPI_NOR
endif

cfg_dtb = $(patsubst %.dts,%.dtb, $(gKERNEL_DTS))
.PHONY: dtb
dtb: $(cfg_dtb) force
	cp -u $(cfg_dtb) $(KERNEL_INSTALL_DIR)

%.dtb: %.dts force
	echo -e '\033[32;1m "build dts to dtb: $@:%^ " >>>>> \033[0m'
	$(CC) -E -Wp,-MD,$@.pre.tmp -nostdinc $(DTS_CFLAGS) -undef -D__DTS__ -x assembler-with-cpp -o $@.dts.tmp $<
	$(UTILITY_DIR)/bin/dtc -O dtb -o $@ -b 0 -Wno-unit_address_vs_reg -d $@.dtc.tmp $@.dts.tmp
	rm -rf $@.pre.tmp $@.dts.tmp $@.dtc.tmp $@.dts.tmp

force:
# vim: syntax=make
