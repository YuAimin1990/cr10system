ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif
include Makefile.param

mpp_test_sub_dirs := vdec \
                     venc \
                     sample_vcodec \
                     vo \
                     vpss \
                     vgs \
                     arcnndemo \
		     ardeeplab \
		     aryolov8 \
		     arbytetrack \
		     aryolov8_seg \
                     aropenpose \
                     arcnn_multi_input \
                     arcnn_zero_copy \
                     arcnn_yolov5 \
                     arcnn_c3d \
                     armbssd \
                     armtcnn \
                     sample_gdc \
                     sample_ifc \
                     sample_scaler \
                     ar_md \
                     ar_od \
                     acodec \
                     sample_cipher \
                     sample_stream \
		     sample_task_snapshot \
		     raw_simulation \
                     sample_rpc

ifeq ($(CHIP), ar9341)
mpp_test_sub_dirs += sample_dsp_ddma \
		     sample_dsp_gauss3x3 \
		     sample_dsp_warpaffine \
		     sample_dsp_canny \
		     sample_dsp_rotate \
		     sample_dsp_fast9 
endif

mpp_test_sub_dirs += sample_ir \
                     sample_ir/ir_cmd

all::

define TEST_SUBDIR_MAKE_template
#test-$$(CONFIG_TEST_$(1)) += $(1)
test-y += $(1)
$(1): force
	make -C $(1) all j=$(N)
endef

$(foreach subdir,$(mpp_test_sub_dirs),$(eval $(call TEST_SUBDIR_MAKE_template,$(subdir))))

all:: prepare_dir $(test-y)
	@echo -e '\033[32;1m building $@: $^ \033[0m'

prepare_dir:
	@mkdir -p $(OUTPUT_APPLICATION_BIN_DIR)

clean:
	@$(foreach test_app, $(test-y), make -C $(test_app) clean;)

force:
