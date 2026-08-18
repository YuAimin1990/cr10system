
ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

app_sub_dirs := dump_mem \
			load_mem \
			ddr_loading \
			rtcmd \
            servicemanager \
			tunning_gen \
			artosyn_upgrade \
			ar_logcat \
			bdcmd \
			icc_file_operation \
			ar_top \


ifeq ($(CHIP), ar9341)
	app_sub_dirs += ar_dsp_boot \
		        dump_dspmsg
endif

ifneq ($(CONFIG_BIT), 32bits)
	app_sub_dirs += artosyn_upgrade
endif

app_sub_dirs_filter := $(foreach sub_dir, $(app_sub_dirs), $(shell if [ -f $(sub_dir)/Makefile ]; then echo $(sub_dir); fi))

all::

define APP_SUBDIR_MAKE_template
#app-$$(CONFIG_APP_$(1)) += $(1)
app-y += $(1)
$(1): force
	make -C $(1) -j$(N)
endef


$(foreach subdir,$(app_sub_dirs_filter),$(eval $(call APP_SUBDIR_MAKE_template,$(subdir))))


all:: prepare_dir $(app-y)
	@echo -e '\033[32;1m building $@: $^ \033[0m'

prepare_dir:
	@mkdir -p $(OUTPUT_APPLICATION_BIN_DIR)

clean:
	@$(foreach usr_app, $(app-y), make -C $(usr_app) clean;)

force:
