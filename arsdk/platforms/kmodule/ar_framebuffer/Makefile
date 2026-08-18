TARGET_MODULE:=ar_framebuffer
EXTRA_CFLAGS += $(AR_DFLAGS)
EXTRA_CFLAGS += -I$(kernel_module_dir)/$(TARGET_MODULE)
EXTRA_CFLAGS += -I$(kernel_module_dir)/ardis
EXTRA_CFLAGS += -I$(gKERNEL_DIR)/drivers/staging/android
ifndef CONFIG_ION
    EXTRA_CFLAGS += -I$(kernel_module_dir)/ar_osal/include
    KBUILD_EXTRA_SYMBOLS += $(kernel_module_dir)/ar_osal/Module.symvers
endif
ifeq ($(CONFIG_BIT), 32bits)
	EXTRA_CFLAGS += -D_32BIT_SDK
endif
$(TARGET_MODULE)-objs := ar_framebuffer_l.o
obj-m := $(TARGET_MODULE).o
