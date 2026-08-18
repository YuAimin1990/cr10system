ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

mpp_vo_test_sub_dirs := sample_vo \
                          sample_vo_config

all::

define TEST_SUBDIR_MAKE_template
test-y += $(1)
$(1): force
	make -C $(1) all j=$(N)
endef

$(foreach subdir,$(mpp_vo_test_sub_dirs),$(eval $(call TEST_SUBDIR_MAKE_template,$(subdir))))

all:: prepare_dir $(test-y)
	@echo -e '\033[32;1m building $@: $^ \033[0m'

prepare_dir:
	@mkdir -p $(OUTPUT_APPLICATION_BIN_DIR)

clean:
	@$(foreach test_app, $(test-y), make -C $(test_app) clean;)

force:
