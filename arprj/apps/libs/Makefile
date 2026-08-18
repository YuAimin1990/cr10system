ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

usr_lib_sub_dirs = third_party \
		ar_avi \
		ar_g711 \
		ar_md5 \
		ar_minirtsp \
		ar_mov \
		cnn_operator \
		sensor \
                ar_ivs \
                ar_ive \
                acodec \
                panel

usr_lib_sub_dirs_filter := $(foreach sub_dir, $(usr_lib_sub_dirs), $(shell if [ -f $(sub_dir)/Makefile ]; then echo $(sub_dir); fi))

all::

#build the selectd libs
define USR_LIB_SUBDIR_MAKE_template
#usr_lib-$(CONFIG_USR_LIB_$(1)) += $(1)
sub_lib-y += $(1)
$(1): force
	$(Q)$(MAKE) -C $(1) all
endef

$(foreach subdir,$(usr_lib_sub_dirs_filter),$(eval $(call USR_LIB_SUBDIR_MAKE_template,$(subdir))))

all:: prepare $(sub_lib-y)
	@echo -e '\033[32;1m building $@: $^ \033[0m'

prepare: prepare_dir prepare_header

prepare_dir:
	@mkdir -p $(OUTPUT_APPLICATION_HEADER_DIR)
	@mkdir -p $(OUTPUT_APPLICATION_LIB_DIR)

prepare_header:
	@$(foreach usr_lib, $(sub_lib-y), make -C $(usr_lib) install_header;)

clean:
	@$(foreach usr_lib, $(sub_lib-y), make -C $(usr_lib) clean;)

force:


