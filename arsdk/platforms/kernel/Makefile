PARAM_FILE:=../../Makefile.param
include $(PARAM_FILE)

export SEC_BOOT

BOOT_MEDIA  ?= emmc
IS_KEEP_TAR ?= false

KERNEL_CONFIG:=$(CHIP)_$(BOOT_MEDIA)_defconfig

gKERNEL_VER      := linux-linaro-stable-lsk-v4.9-17.07
gKERNEL_RT_DIR   := $(gBINARIES_DOWNLOAD_URL)/kernel
gKERNEL_TAR      := $(gKERNEL_VER).tar.gz

#ifeq ("$(gFPGA_SIMULATION)", "y")
#GLOBAL_CFLAG = -DFPGA_SIMULATION
#endif

ifeq ($(CONFIG_BIT), 32bits)
GLOBAL_CFLAG += -DBINDER_IPC_32BIT
endif

all:prepare kernel-config kernel install

###### download and patch ######
prepare:
	@echo -e '\033[32;1m kernel $@ >>>>> \033[0m'
	@if [ -d mods/$(gKERNEL_VER) ]; then \
		cp -u $(UTILITY_DIR)/bin/sign $(UTILITY_DIR)/bin/mkimage mods/$(gKERNEL_VER); \
	fi
	python2 $(MAKERY_DIR)/download.py \
				--dl_type=$(gDOWNLOAD_TYPE) \
				--dl_dir=./ \
				--filename=$(gKERNEL_TAR) \
				--rt_dir=$(gKERNEL_RT_DIR) \
				--extract_dir=./ \
				--version=$(gKERNEL_VER) \
				--keep_tar_file=$(IS_KEEP_TAR);

###### config, need $(BOOT_MEDIA)######
default_config=$(shell echo $(gKERNEL_CONFIG) | sed s/".$(BOOT_MEDIA)"//)
kernel-config:
	@echo -e '\033[32;1m kernel config....>>>>> \033[0m'
	@if [ -f $(gKERNEL_CONFIG) ]  && [ -n "$(gKERNEL_CONFIG)" ]; then \
		cp -u $(gKERNEL_CONFIG) $(gKERNEL_VER)/.config; \
	elif [ -f $(default_config) ] && [ -n "$(default_config)" ]; then \
		cp -u $(default_config) $(gKERNEL_VER)/.config; \
		echo -e '\033[32;1m use default_config \033[0m'; \
	else \
		echo -e '\033[32;1m" >>>>> "make $(KERNEL_CONFIG)" >>>>> \033[0m'; \
		make -C $(gKERNEL_VER) ARCH=arm64 CROSS_COMPILE=$(gCROSS_COMPILE) $(KERNEL_CONFIG); \
	fi
	@echo -e '\033[32;1m prepare $@ end >>>>> \033[0m'

kernel:
	@make -C $(gKERNEL_VER) ARCH=arm64 -j16 CROSS_COMPILE=$(gCROSS_COMPILE) KCFLAGS=$(GLOBAL_CFLAG) V=0

install:
	@echo -e '\033[32;1m =============== kernel install =============== \033[0m'
	@mkdir -p $(KERNEL_INSTALL_DIR)/khdr
	@cp -u $(gKERNEL_VER)/drivers/usb/gadget/function/uvc.h $(gKERNEL_VER)/include/uapi/linux/usb/ -rf
	@cp -u $(gKERNEL_VER)/drivers/iio/imu/inv_mpu6509/inv_mpu_ioctl.h $(gKERNEL_VER)/include/uapi/linux/iio/
	cp $(gKERNEL_VER)/kernel.common.img $(KERNEL_INSTALL_DIR)
ifeq (x$(SEC_BOOT),xn)
	cp $(gKERNEL_VER)/kernel.nonsec.img $(KERNEL_INSTALL_DIR)
endif
ifeq ($(CONFIG_BIT), 32bits)
	@make -C $(gKERNEL_VER) headers_install ARCH=arm CROSS_COMPILE=$(gCROSS_COMPILE) V=0 INSTALL_HDR_PATH=$(KERNEL_INSTALL_DIR)/khdr
else
	@make -C $(gKERNEL_VER) headers_install ARCH=arm64 CROSS_COMPILE=$(gCROSS_COMPILE) V=0 INSTALL_HDR_PATH=$(KERNEL_INSTALL_DIR)/khdr
endif
clean:
	@if [ -d $(gKERNEL_VER) ] ; then make -C $(gKERNEL_VER) clean; fi
	@rm -f $(gKERNEL_VER)/.config
	@rm -rf $(KERNEL_INSTALL_DIR)

clobber:
	@if [ -d $(gKERNEL_VER) ]; then \
		rm -rf $(gKERNEL_VER); \
	fi
	@make clean

# vim: syntax=make
