ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

MKFS = fakeroot python2 $(MAKERY_DIR)/mkfs.py --mpp_service=$(MPP_SERVICE)

#### build fs image: according to fs.json and flash flash_layout_*.ini ####
fs_img:
	@echo -e '\033[32;1m =============== Generating fs image ===============  \033[0m'
	$(MKFS) \
				--fs_config_file=$(gFS_CFG) \
				--flash_layout_file=$(gIMAGE_INI_FILE) \
				--output_dir=$(FS_INSTALL_DIR) \
				--board_config_dir=$(gBOARD_BASE) \
				--uboot_env_file=$(gUBOOT_ENV) \
				--security_cfg_file=$(SECURITY_CFG_FILE) \
				--build_type=$(BUILD_TYPE)

# vim: syntax=make
