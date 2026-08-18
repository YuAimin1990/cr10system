ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

sensor_lib_sub_dirs = sony_imx290 \
					  sc_sc132gs \
					  isp_in_0 \
					  isp_in_l \
					  isp_in_r \
					  sony_imx290_1 \
					  sony_imx290_2 \
					  sony_imx290_3 \
					  tp9930 \
                      sony_imx415 \
                      sony_imx464 \
                      sc_sc910gs \
		      isp_in_inf \
		      isp_in_inf1 \
			  isp_in_inf_thermal \
		      guide_timo120 \
		      guide_timo256 \
		      guide_timo256af \
		      guide_nk1221a \
		      iray_elf1 \
              ov_04a10 \
		sc_sc530ai \
			  gc_gc2093 \
              iray_microiii \
                      sony_imx347 \
                      sc_sc2210 \
              ov_05a20 \
	      sc_sc230ai \
			  obc_pleco_tof \
			  gc_gc2053 \
			  dvp_rx_color_bar \
			  hdmi_rx \



sensor_lib_sub_dirs += gst417w \
                       gst412c \
                       gst612c \
					   gst212w4 \
                       h3812c1sg \
                       rtd6122c \
                       rtd3172c \
                       gn2

all::

#build the selectd libs
define USR_LIB_SUBDIR_MAKE_template
#usr_lib-$(CONFIG_USR_LIB_$(1)) += $(1)
sub_lib-y += $(1)
$(1): force
	$(Q)$(MAKE) -C $(1) all
endef

$(foreach subdir,$(sensor_lib_sub_dirs),$(eval $(call USR_LIB_SUBDIR_MAKE_template,$(subdir))))

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


