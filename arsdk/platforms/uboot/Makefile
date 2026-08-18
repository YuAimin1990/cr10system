PARAM_FILE:=../../Makefile.param
include $(PARAM_FILE)

export SEC_BOOT

BOOT_MEDIA ?= emmc
IS_KEEP_TAR ?= false

UBOOT_CONFIG   :=$(CHIP)_$(BOOT_MEDIA)_defconfig

gUBOOT_VER     := u-boot-2020.10
gUBOOT_RT_DIR  := $(gBINARIES_DOWNLOAD_URL)/u-boot
gUBOOT_TAR     := $(gUBOOT_VER).tar.bz2

all: prepare uboot-config uboot install

signtool           ?= $(UTILITY_DIR)/bin/sign
genpackbootloader  ?= $(UTILITY_DIR)/bin/gen_packed_bootloader.py
genvendorimage     ?= $(UTILITY_DIR)/bin/gen_vendor_image.py
genvendorimage_sec ?= $(UTILITY_DIR)/bin/gen_vendor_image_sec.py
mkimagetool        ?= $(UTILITY_DIR)/bin/mkimage

###### download and patch######
prepare:
	@echo -e '\033[32;1m uboot $@ >>>>> \033[0m'
	@if [ -d mods/$(gUBOOT_VER) ]; then \
		cp -u $(signtool) $(genpackbootloader) $(genvendorimage) $(genvendorimage_sec) $(mkimagetool) mods/$(gUBOOT_VER); \
	fi
	python2 $(MAKERY_DIR)/download.py \
				--dl_type=$(gDOWNLOAD_TYPE) \
				--dl_dir=./ \
				--filename=$(gUBOOT_TAR) \
				--rt_dir=$(gUBOOT_RT_DIR) \
				--extract_dir=./ \
				--version=$(gUBOOT_VER) \
				--keep_tar_file=$(IS_KEEP_TAR);

###### config, need $(BOOT_MEDIA) ######
default_config=$(shell echo $(gUBOOT_CONFIG) | sed s/".$(BOOT_MEDIA)"//)
uboot-config:
	@echo -e '\033[32;1m uboot config....>>>>> \033[0m'
	cp -rfL $(gBOARD_BASE)/uboot/*.dtsi $(gUBOOT_VER)/arch/arm/dts/
	@if [ -f $(gUBOOT_CONFIG) ] && [ -n "$(gUBOOT_CONFIG)" ]; then \
		echo -e '\033[32;1m" >>>>> "use $(gUBOOT_CONFIG)" >>>>> \033[0m'; \
		cp -u $(gUBOOT_CONFIG) $(gUBOOT_VER)/.config; \
	elif [ -f $(default_config) ] && [ -n "$(default_config)" ]; then \
		cp -u $(default_config) $(gUBOOT_VER)/.config; \
		echo -e '\033[32;1m use default_config \033[0m'; \
	else \
		echo -e '\033[32;1m" >>>>> "make $(UBOOT_CONFIG)" >>>>> \033[0m'; \
		make -C $(gUBOOT_VER) CROSS_COMPILE=$(gCROSS_COMPILE) $(UBOOT_CONFIG); \
	fi
	@echo -e '\033[32;1m prepare $@ end >>>>> \033[0m'

uboot:
	@echo -e '\033[32;1m $@ start>>>>> \033[0m'
	@make -j8 -C $(gUBOOT_VER) CROSS_COMPILE=$(gCROSS_COMPILE) PLATFORM_LIBS=$(SDKDIR)/platforms/uboot/$(gUBOOT_VER)/libefuse_$(CHIP).a CHIP=$(CHIP) V=1

install:
	@echo -e '\033[32;1m =============== uboot install =============== \033[0m'
	mkdir -p $(UBOOT_INSTALL_DIR)
ifeq (x$(SEC_BOOT),xn)
	cp -u $(gUBOOT_VER)/troot_spl.nonsec.img $(UBOOT_INSTALL_DIR)
	cp -u $(gUBOOT_VER)/uboot.nonsec.img $(UBOOT_INSTALL_DIR)
	cp -u $(gUBOOT_VER)/vendor.nonsec.img $(UBOOT_INSTALL_DIR)
endif
	cp -u $(gUBOOT_VER)/u-boot.bin $(UBOOT_INSTALL_DIR)
ifeq (x$(SEC_BOOT),xy)
	cp -u $(gUBOOT_VER)/vendor.common.img $(UBOOT_INSTALL_DIR)
	cp -u $(gUBOOT_VER)/u-boot $(UBOOT_INSTALL_DIR)
	cp -u $(gUBOOT_VER)/spl/u-boot-spl $(UBOOT_INSTALL_DIR)
	cp -u $(gUBOOT_VER)/spl/u-boot-spl.bin $(UBOOT_INSTALL_DIR)
	cp -u $(gUBOOT_VER)/spl/u-boot-spl-nodtb.bin $(UBOOT_INSTALL_DIR)
endif

clean:
	@if [ -d $(gUBOOT_VER) ]; then make -C $(gUBOOT_VER) clean; fi
	@rm -f $(gUBOOT_VER)/.config
	@rm -rf $(UBOOT_INSTALL_DIR)

clobber:
	@if [ -d $(gUBOOT_VER) ]; then \
		rm -rf $(gUBOOT_VER); \
	fi
	@make clean

# vim: syntax=make
