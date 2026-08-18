#Check the python version, Python2.x required.
ifeq (, $(shell which python2 ))
  $(error "python2 not found in $(PATH)")
endif

PARAM_FILE:=../arsdk/Makefile.param
include $(PARAM_FILE)

#Color define
BLACK  = "\e[30;1m"
RED    = "\e[31;1m"
GREEN  = "\e[32;1m"
YELLOW = "\e[33;1m"
BLUE   = "\e[34;1m"
PURPLE = "\e[35;1m"
CYAN   = "\e[36;1m"
WHITE  = "\e[37;1m"
DONE   = "\033[0m"

export BASE_BOARD ?= fam30
export BUILD_TYPE ?= debug
export BOOT_MEDIA ?= nand
export APP        ?= ipcam
export CHIP       ?= ar9341
export SEC_BOOT   ?= n
NEED_STRIP = TRUE

MPP_SERVICE_str = "MPP_SERVICE=$(MPP_SERVICE)"

default: all

help:
	@echo -e $(YELLOW)"-------------------------------------------------------------------------------------------------------------------" $(DONE)
	@echo -e $(YELLOW)"help: 	 Please specify <BASE_BOARD> <BOOT_MEDIA> <BUILD_TYPE> <APP>!"    $(DONE)
	@echo -e $(YELLOW)"help: 	 The params will be saved to .current_params file. If you want change params, please make clean!"    $(DONE)
	@echo -e $(BLUE)"e.g.:    BOOT_MEDIA=(nor/nand/emmc) BASE_BOARD=(evb/fpga/..) BUILD_TYPE=(release/debug/debug1) APP=(ipcam/usbcam)"    $(DONE)
	@echo -e $(BLUE)"e.g.:    make BOOT_MEDIA=nor BASE_BOARD=evb BUILD_TYPE=release APP=ipcam all"    $(DONE)
	@echo -e $(BLUE)"e.g.:    make BOOT_MEDIA=nand BASE_BOARD=fpga BUILD_TYPE=debug APP=usbcam all"    $(DONE)
	@echo -e $(BLUE)"default: make BOOT_MEDIA=${BOOT_MEDIA} BASE_BOARD=$(BASE_BOARD) BUILD_TYPE=${BUILD_TYPE} APP=ipcam CHIP=$(CHIP) SEC_BOOT=$(SEC_BOOT) $(MPP_SERVICE_str) all"    $(DONE)
	@echo -e $(BLUE)"*Meaning of BUILD_TYPE:" $(DONE)
	@echo -e $(BLUE)    "release - No debug info in image" $(DONE)
	@echo -e $(BLUE)    "debug   - Default level of debug. Enable symbols in libraries, and support GDB on board" $(DONE)
	@echo -e $(BLUE)    "debug1  - Higher level of debug. Support asan on board, based on 'debug' level"    $(DONE)
	@echo -e $(YELLOW)"-------------------------------------------------------------------------------------------------------------------" $(DONE)

display_param:
	@echo -e $(GREEN)"-------------------------------------------------------------------------------------------------------------------"  $(DONE)
	@echo -e $(GREEN)"BASE_BOARD=$(BASE_BOARD)"  $(DONE)
	@echo -e $(GREEN)"BOOT_MEDIA=$(BOOT_MEDIA)"  $(DONE)
	@echo -e $(GREEN)"BUILD_TYPE=$(BUILD_TYPE)"  $(DONE)
	@echo -e $(GREEN)"APP=$(APP)"  $(DONE)
	@echo -e $(GREEN)"CHIP=$(CHIP)"  $(DONE)
	@echo -e $(GREEN)"SEC_BOOT=$(SEC_BOOT)"  $(DONE)
	@echo -e $(GREEN)$(MPP_SERVICE_str)  $(DONE)
	@echo -e $(GREEN)"-------------------------------------------------------------------------------------------------------------------"  $(DONE)

include $(MAKERY_DIR)/make_dtb_rules.mk
include $(MAKERY_DIR)/uboot_env.mk
include $(MAKERY_DIR)/fs_img.mk
include $(MAKERY_DIR)/image.mk
all: display_param
	make product_config
	make security_config
	make rom_code
	make uboot

ifneq ($(findstring $(CHIP), ar9311 ars31),)
	make bl31
endif

ifeq (x$(SEC_BOOT),xy)
	make troot
endif
	make kernel
	make dtb
	make rootfs
	make kmodule
	make middlewares
	make single_app
	make strip
	make fs_img
	make uboot_env
	make image

#### 1.make product_config ####
include $(MAKERY_DIR)/make_config.inc
product_config:
	@if [ $(BUILD_TYPE) != "release" ] && [ $(BUILD_TYPE) != "debug" ] && [ $(BUILD_TYPE) != "debug1" ]; then \
		echo "***ERROR: Invalid BUILD_TYPE. Please use 'make help' to get more info.***"; \
		exit 1; \
	fi
	@if [ ! -d ./configs/$(CHIP)/$(BASE_BOARD)_config ]; then \
		echo "***ERROR: Invalid CHIP or BASE_BOARD. Please use 'make help' to get more info.***"; \
		exit 1; \
	fi;
	make config-$(CHIP)-$(BASE_BOARD)_config
	@if [ ! -f .current_params ]; then \
		echo "BOOT_MEDIA=$(BOOT_MEDIA)" >> .current_params; \
		echo "BASE_BOARD=$(BASE_BOARD)" >> .current_params; \
		echo "BUILD_TYPE=$(BUILD_TYPE)" >> .current_params; \
		echo "APP=$(APP)" >> .current_params; \
		echo "CHIP=$(CHIP)" >> .current_params; \
		echo "SEC_BOOT=$(SEC_BOOT)" >> .current_params; \
		echo $(MPP_SERVICE_str) >> .current_params; \
		echo -e '\033[32;1m Save current params sucessfully!>>>>> \033[0m'; \
	fi;

security_config:
	@sed -n '/MAKEFILE/,$$p' $(SECURITY_CFG_FILE) > $(SECURITY_CFG_MK)
	@sed -i '/MAKEFILE/d' $(SECURITY_CFG_MK)
	$(shell sed -i "s/^PKf.*/PKf\ \ \ \ \ =\ `cat $(SECURITY_CFG_FILE) | awk -F'=' -v section="[TROOT]" -v k="PKf" '$$0==section {flag=1;next} /\[/ {flag=0;next} flag && $$1==k {print $$2}'`/g" $(SDKDIR)/platforms/security/troot/firmware/firmware.aic)

#### 2.build uboot ####
uboot:
	make -C	$(SDKDIR)/platforms/uboot BOOT_MEDIA=$(BOOT_MEDIA) all

uboot_clean:
	make -C	$(SDKDIR)/platforms/uboot clean

bl31:
	make CROSS_COMPILE=$(CROSS_COMPILE) CFLAGS= DEBUG=1 PLAT_XLAT_TABLES_DYNAMIC=0 RESET_TO_BL31=1 -C $(ARM_TRUSTED_FIRMWARE_DIR) bl31 PLAT=ar_9311 SPD=opteed V=0

bl31_clean:
	make -C $(ARM_TRUSTED_FIRMWARE_DIR) distclean

uboot_clobber:
	make -C	$(SDKDIR)/platforms/uboot clobber

troot:
	make -C	$(SDKDIR)/platforms/security/troot all

troot_clean:
	make -C	$(SDKDIR)/platforms/security/troot clean

a53_rtos:
	make -C	$(SDKDIR)/platforms/a53_rtos all

a53_rtos_clean:
	make -C	$(SDKDIR)/platforms/a53_rtos clean

#### 3.build kernel ####
kernel:
	make -C	$(SDKDIR)/platforms/kernel BOOT_MEDIA=$(BOOT_MEDIA) all

kernel_clean:
	make -C	$(SDKDIR)/platforms/kernel clean

kernel_clobber:
	make -C	$(SDKDIR)/platforms/kernel clobber

#### 4.build rootfs ####
rootfs:
	make -C	$(SDKDIR)/platforms/rootfs all

rootfs_clean:
	make -C	$(SDKDIR)/platforms/rootfs clean

rootfs_clobber:
	make -C	$(SDKDIR)/platforms/rootfs clobber

initramfs:
	make -C $(SDKDIR)/platforms/rootfs initramfs

#### 6.build kmodule ####
kmodule:
	@if [ -f $(SDKDIR)/platforms/kmodule/Makefile ] ; then \
		make -C	$(SDKDIR)/platforms/kmodule all; \
	fi

kmodule_clean:
	@if [ -f $(SDKDIR)/platforms/kmodule/Makefile ] ; then \
		make -C	$(SDKDIR)/platforms/kmodule clean; \
	fi

#### 7.build middlewares ####
middlewares:
ifeq (x$(BUILD_TYPE), xdebug1)
	cp -fL $(gTOOLCHAIN_OPT_PATH)/../aarch64-linux-gnu/lib64/libasan.so $(OUTPUT_MIDDLEWARES_DIR)/lib
endif
	@if [ -f $(SDKDIR)/middlewares/Makefile ] ; then \
		make -C	$(SDKDIR)/middlewares all; \
	fi

middlewares_clean:
	@if [ -f $(SDKDIR)/middlewares/Makefile ] ; then \
		make -C	$(SDKDIR)/middlewares clean; \
	fi
	rm -f $(OUTPUT_MIDDLEWARES_DIR)/lib/libasan.so

#### 8.build app from $(APP) ####
single_app:
	@if [ -f $(PRJDIR)/apps/libs/Makefile ] ; then \
		make -C $(PRJDIR)/apps/libs all; \
	fi
	@if [ -f $(PRJDIR)/apps/$(APP)/Makefile ] ; then \
		make -C $(PRJDIR)/apps/$(APP) all; \
	fi
	@if [ -f $(PRJDIR)/apps/ipcam/ipctool/Makefile ] ; then \
		make -C $(PRJDIR)/apps/ipcam/ipctool all; \
	fi
	@if [ -f $(PRJDIR)/apps/utils/Makefile ] ; then \
		make -C $(PRJDIR)/apps/utils; \
	fi
	@if [ -f $(PRJDIR)/apps/hal_sample/Makefile ] ; then \
		make -C $(PRJDIR)/apps/hal_sample all; \
	fi
	@if [ -f $(PRJDIR)/apps/mpp_sample/Makefile ] ; then \
		make -C $(PRJDIR)/apps/mpp_sample all; \
	fi
	@if [ -f $(PRJDIR)/apps/web_service/Makefile ] ; then \
		make -C $(PRJDIR)/apps/web_service; \
	fi
ifneq ($(CONFIG_BIT), 32bits)
	@if [ -f $(PRJDIR)/apps/mpp_infrared_sample/Makefile ] ; then \
		make -C $(PRJDIR)/apps/mpp_infrared_sample; \
	fi
endif
	@if [ -f $(PRJDIR)/apps/uvc_service2/Makefile ] ; then \
		make -C $(PRJDIR)/apps/uvc_service2; \
	fi
	@if [ -f $(PRJDIR)/apps/uac_service/Makefile ] ; then \
		make -C $(PRJDIR)/apps/uac_service; \
	fi
	@if [ -f $(PRJDIR)/apps/peripheral_sample/Makefile ] ; then \
                make -C $(PRJDIR)/apps/peripheral_sample; \
	fi
	@if [ -f $(PRJDIR)/apps/ar_wdt_service/Makefile ] ; then \
                make -C $(PRJDIR)/apps/ar_wdt_service; \
	fi

single_app_clean:
	make all_apps_clean;

rom_code:
	@if [ -f $(SDKDIR)/platforms/rom_code/Makefile ]; then \
		make -C $(SDKDIR)/platforms/rom_code all; \
	fi

#### 8.1 build all apps ####
all_apps:
	@if [ -f $(PRJDIR)/apps/Makefile ] ; then \
		make -C $(PRJDIR)/apps all; \
	fi
	@if [ -f $(SDKDIR)/middlewares/hal/unitests/Makefile ] ; then \
		make -C $(SDKDIR)/middlewares/hal/unitests all; \
	fi

all_apps_clean:
	@if [ -f $(PRJDIR)/apps/Makefile ] ; then \
		make -C $(PRJDIR)/apps clean; \
	fi
	@if [ -f $(SDKDIR)/middlewares/hal/unitests/Makefile ] ; then \
		make -C $(SDKDIR)/middlewares/hal/unitests clean; \
	fi

fs_clean:
	@if [ -d $(FS_INSTALL_DIR) ] ; then \
		rm -rf $(FS_INSTALL_DIR); \
	fi

ifneq ($(gBOARD), )
clean:
	make uboot_clean
ifneq ($(findstring $(CHIP), ar9311 ars31),)
	make bl31_clean
endif
	make kernel_clean
	make rootfs_clean
	make kmodule_clean
	make middlewares_clean
	make all_apps_clean
	make fs_clean
	rm -rf $(IMAGE_INSTALL_DIR)
	rm -rf .current_params
	rm -rf .product_profile
else
clean:
	@echo -e $(RED) "clean: gBOARD is not set; Need Make config first;" $(DONE)
	@echo -e $(RED) "clean: exit" $(DONE)

endif

#9. strip for non-debug image
strip:
ifneq ($(findstring $(BUILD_TYPE), debug1 debug),)
ifneq ($(BASE_BOARD),ipcnand) #For ipcnand board, always need to strip
NEED_STRIP =
endif
endif

ifneq (x$(NEED_STRIP), x)
	@$(foreach usr_lib, $(OUTPUT_APPLICATION_LIB_DIR)/lib*, $(STRIP) $(usr_lib);)
	@$(foreach usr_bin, $(OUTPUT_APPLICATION_BIN_DIR)/*, $(STRIP) $(usr_bin);)
	@$(foreach hal_lib, $(OUTPUT_HAL_DIR)/lib/lib*, $(STRIP) $(hal_lib);)
	@$(foreach hal_bin, $(OUTPUT_HAL_DIR)/bin/*.app, $(STRIP) $(hal_bin);)
	@$(foreach mpp_lib, $(OUTPUT_MPP_DIR)/lib/lib*, $(STRIP) $(mpp_lib);)
	@$(foreach mid_lib, $(OUTPUT_MIDDLEWARES_DIR)/lib/lib*, $(STRIP) $(mid_lib);)
	@$(foreach ko, $(KMODULE_INSTALL_DIR)/*.ko, $(STRIP) --strip-debug $(ko);)
	@echo -e $(YELLOW) "strip debug info from objs" $(DONE)
endif


force:
# vim: syntax=make
