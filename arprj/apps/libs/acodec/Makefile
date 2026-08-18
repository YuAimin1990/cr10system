ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

acodec_lib_sub_dirs = tp9930 rt5633
				
all::

#build the selectd libs
define USR_LIB_SUBDIR_MAKE_template
#usr_lib-$(CONFIG_USR_LIB_$(1)) += $(1)
sub_lib-y += $(1)
$(1): force
	$(Q)$(MAKE) -C $(1) all
endef

$(foreach subdir,$(acodec_lib_sub_dirs),$(eval $(call USR_LIB_SUBDIR_MAKE_template,$(subdir))))

all:: prepare $(sub_lib-y)
	@echo -e '\033[32;1m building $@: $^ \033[0m'

install_header:prepare

prepare: prepare_dir prepare_header

prepare_dir:
	@mkdir -p $(OUTPUT_APPLICATION_HEADER_DIR)
	@mkdir -p $(OUTPUT_APPLICATION_LIB_DIR)

prepare_header:
	@$(foreach usr_lib, $(sub_lib-y), make -C $(usr_lib) install_header;)

clean:
	@$(foreach usr_lib, $(sub_lib-y), make -C $(usr_lib) clean;)

force:


