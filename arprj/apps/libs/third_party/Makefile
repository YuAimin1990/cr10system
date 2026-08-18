ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

3rd_lib_sub_dirs = cjson \
			opencv \
			ffmpeg 

usr_lib_sub_dirs_filter := $(foreach sub_dir, $(3rd_lib_sub_dirs), $(shell if [ -f $(sub_dir)/Makefile ]; then echo $(sub_dir); fi))

all::

#build the selectd libs
define USR_LIB_SUBDIR_MAKE_template
sub_lib-y += $(1)
$(1): force
	$(Q)$(MAKE) -C $(1) prebuild
endef

$(foreach subdir,$(usr_lib_sub_dirs_filter),$(eval $(call USR_LIB_SUBDIR_MAKE_template,$(subdir))))

all:: prepare $(sub_lib-y)
	@echo -e '\033[32;1m prebuild $@: $^ \033[0m'

prepare:
	@mkdir -p $(OUTPUT_APPLICATION_HEADER_DIR)
	@mkdir -p $(OUTPUT_APPLICATION_LIB_DIR)

clean:
	@$(foreach usr_lib, $(sub_lib-y), make -C $(usr_lib) clean;)

force:


