#kernel module definitions
KBUILD_EXTRA_SYMBOLS += $(kernel_module_dir)/ar_osal/Module.symvers
export KBUILD_EXTRA_SYMBOLS

TARGET_MODULE:=ar_panic
EXTRA_CFLAGS += $(AR_DFLAGS)
$(TARGET_MODULE)-objs := ar_panic_drv.o
obj-m := $(TARGET_MODULE).o

