# This is just a redirector file.
#    The make rules for this directory are found in makefile.mk
#    The generic rules are found in topmake.mk which includes this directory's makefile.mk

export kernel_module_dir:=$(shell pwd)

PARAM_FILE:=../../Makefile.param
include $(PARAM_FILE)

ifeq ($(gFPGA_SIMULATION),y)
export AR_DFLAGS  += -DAR_FPGA
endif

ifeq ($(CONFIG_BIT), 32bits)
AR_DFLAGS += -DKERNEL_BIT_64 -DUSER_BIT_32
endif

default: all

module_sub_dirs = ar_osal \
    ar_framebuffer \
    ar_mpp_irq \
    ar_mpp_proc \
    ar_vb \
    ar_sys \
    ar_panic \
    ar_npu \
    ar_scaler \
    ar_ifc \
    ar_gdc \
	ar_cipher \
    8189es \
    8189fs \
	cryptodev \
    ar_pwm \
	ar_sysctl \
	GT1X \
	ssv6x5x

ifeq ($(CHIP), ar9341)
	module_sub_dirs += ar_dsp
endif

gKERNEL_DIR ?= $(SDKDIR)/platforms/kernel/linux-linaro-stable-lsk-v4.9-17.07

#build the selectd libs
define MODULE_SUBDIR_MAKE_template

module-y += $(1)
# chech if the module release with c source, build if source avail
$(1): force
ifneq ("$(strip $(shell find ./$(1) -name "*.c" -o -name "*.o"))","")
	make -j$(N) -C $(gKERNEL_DIR) ARCH=arm64 CROSS_COMPILE=$(gCROSS_COMPILE) M=$(kernel_module_dir)/$(1) modules V=1 || exit 1;
else
	@echo "no source file, use prebuild"
endif

endef


$(foreach subdir,$(module_sub_dirs),$(eval $(call MODULE_SUBDIR_MAKE_template,$(subdir))))

all: prepare $(module-y) install
	@echo -e '\033[32;1m ==============building $@: $^ \033[0m'
	@echo -e '\033[32;1m ==============module-y: $(module-y) \033[0m'

prepare: prepare_dir prepare_header

prepare_dir:
	mkdir -p $(OUTPUT_KMODULE_DIR)/inc
	mkdir -p $(KMODULE_INSTALL_DIR)

prepare_header:
	@if [ -f ar_framebuffer/v4ar_dev.h ]; then \
		cp ar_framebuffer/v4ar_dev.h $(OUTPUT_KMODULE_DIR)/inc -u; \
	fi
	@if [ -f ar_framebuffer/rpc_fs_private_buffer.h ]; then \
		cp ar_framebuffer/rpc_fs_private_buffer.h $(OUTPUT_KMODULE_DIR)/inc -u; \
	fi
	@if [ -f ar_framebuffer/ar_framebuffer_def.h ]; then \
    	cp ar_framebuffer/ar_framebuffer_def.h $(OUTPUT_KMODULE_DIR)/inc -u; \
	fi
	@if [ -d ar_cam ]; then \
		cp ar_cam/cam_api.h $(OUTPUT_KMODULE_DIR)/inc -u; \
	fi
	@if [ -d ar_mpp_irq ]; then \
		cp ar_mpp_irq/*.h $(OUTPUT_KMODULE_DIR)/inc -u; \
	fi
	@if [ -d ar_osal ]; then \
		cp ar_osal/*.h $(OUTPUT_KMODULE_DIR)/inc -u; \
		cp ar_osal/include/*.h $(OUTPUT_KMODULE_DIR)/inc -u; \
	fi
	@if [ -d ar_vb ]; then \
		cp ar_vb/*.h $(OUTPUT_KMODULE_DIR)/inc -u; \
	fi
	@if [ -d ar_sys ]; then \
		cp ar_sys/*.h $(OUTPUT_KMODULE_DIR)/inc -u; \
	fi
	@if [ -d ar_sysctl ]; then \
		cp ar_sysctl/*.h $(OUTPUT_KMODULE_DIR)/inc -u; \
	fi
	@if [ -d ar_dsp ]; then \
                cp ar_dsp/*.h $(OUTPUT_KMODULE_DIR)/inc -u; \
        fi
	@if [ -d ar_npu ]; then \
                cp ar_npu/ar_npu.h $(OUTPUT_KMODULE_DIR)/inc -u; \
        fi
	@if [ -d ar_scaler ]; then \
                cp ar_scaler/ar_scaler.h $(OUTPUT_KMODULE_DIR)/inc -u; \
        fi
	@if [ -d ar_ifc ]; then \
                cp ar_ifc/ar_ifc.h $(OUTPUT_KMODULE_DIR)/inc -u; \
        fi
	@if [ -d ar_gdc ]; then \
                cp ar_gdc/*.h $(OUTPUT_KMODULE_DIR)/inc -u; \
        fi
	@if [ -d ar_mpp_proc ]; then \
		cp ar_mpp_proc/*.h $(OUTPUT_KMODULE_DIR)/inc -u; \
	fi
	@if [ -d ar_cipher/ ]; then \
		cp ar_cipher/*.h $(OUTPUT_KMODULE_DIR)/inc -u; \
	fi

install:
	@echo "install..."
	@$(foreach kmod, $(module-y), cp $(kmod)/*.ko $(KMODULE_INSTALL_DIR);)
	@find $(gKERNEL_DIR)/ -name *.ko | xargs -i cp {} $(KMODULE_INSTALL_DIR)

clean:
	@echo "make-clean: sub_kmod_list:$(module-y)"
	@if [ -d $(gKERNEL_DIR) ]; then \
		echo "">/dev/null; \
		$(foreach kmod, $(module_sub_dirs), if [ -f $(kmod)/modules.order ]; then make -C $(gKERNEL_DIR) $(gKERNEL_MAKE_OPTS) M=$(kernel_module_dir)/$(kmod) clean || exit 1; fi;) \
	fi

force:
