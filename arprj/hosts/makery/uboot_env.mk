ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

sinclude $(SECURITY_CFG_MK)

ifeq ($(dm_verity), y)
	UBOOT_ENV_PATH = $(gUBOOT_ENV:.txt=.dmverity.txt)
	ifeq ($(wildcard $(UBOOT_ENV_PATH)),)
		UBOOT_ENV_PATH = $(gUBOOT_ENV)
	endif
else
	UBOOT_ENV_PATH = $(gUBOOT_ENV)
endif
#### build env image: according to env_*.txt ####
u-boot-env = $(UBOOT_INSTALL_DIR)/u-boot-env
# uboot env size is fixed 32KB
#u-boot-env-partsize=$(shell $(MAKERY_DIR)/get_part_size.sh $(gIMAGE_INI_FILE) env env | grep env | cut -d "=" -f2)
uboot_env:
	@echo -e '\033[32;1m ==============generate u-boot-env: $(notdir $@) ============== \033[0m'
	@mkdir -p $(UBOOT_INSTALL_DIR)
	$(UTILITY_DIR)/bin/mkenvimage -s 0x8000 -p 0x00 -o $(u-boot-env) $(UBOOT_ENV_PATH)

# vim: syntax=make
