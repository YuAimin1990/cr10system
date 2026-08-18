ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

hal_vo_test_sub_dirs := test_hal_vo_client \
					test_hal_vo_server

all::

define TEST_SUBDIR_MAKE_template
#test-$$(CONFIG_TEST_$(1)) += $(1)
test-y += $(1)
$(1): force
	make -C $(1) all j=$(N)
endef

$(foreach subdir,$(hal_vo_test_sub_dirs),$(eval $(call TEST_SUBDIR_MAKE_template,$(subdir))))

all:: prepare_dir $(test-y)
	@echo -e '\033[32;1m building $@: $^ \033[0m'

prepare_dir:
	@mkdir -p $(OUTPUT_APPLICATION_BIN_DIR)

clean:
	@$(foreach test_app, $(test-y), make -C $(test_app) clean;)

force:
