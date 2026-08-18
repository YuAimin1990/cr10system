ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

applications_sub_dir = libs \
				utils \
				tests \
				mpp_sample \
				hal_sample \
				ipcam \
				ipcam/ipctool \
				web_service \
				uvc_service2 \
				uac_service \
				ar_wdt_service \
				peripheral_sample

ifneq ($(CONFIG_BIT), 32bits)
applications_sub_dir += mpp_infrared_sample
endif

applications_sub_dir_filter := $(foreach sub_dir, $(applications_sub_dir), $(shell if [ -f $(sub_dir)/Makefile ]; then echo $(sub_dir); fi))

all::

define APPLICATIONS_SUBDIR_MAKE_template
app-y += $(1)
$(1): force
	make -C $(1) all
endef

$(foreach subdir,$(applications_sub_dir_filter),$(eval $(call APPLICATIONS_SUBDIR_MAKE_template,$(subdir))))

all:: prepare_dir $(app-y)
	@echo -e '\033[32;1m ==============building $@: $^ \033[0m'

prepare_dir:
	@mkdir -p $(APPLICATION_INSTALL_DIR)

clean:
	@$(foreach applications, $(app-y), make -C $(applications) clean;)

force:


