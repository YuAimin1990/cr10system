#kernel module definitions
KBUILD_EXTRA_SYMBOLS += $(kernel_module_dir)/ar_osal/Module.symvers
export KBUILD_EXTRA_SYMBOLS

TARGET_MODULE:=ar_scaler
#src = $(kernel_module_dir)/$(TARGET_MODULE)
EXTRA_CFLAGS += $(AR_DFLAGS)
EXTRA_CFLAGS += -I$(kernel_module_dir)/$(TARGET_MODULE)
EXTRA_CFLAGS += -I$(kernel_module_dir)/ar_osal/include
$(TARGET_MODULE)-objs := ar_scaler_hal.o ar_scaler_main.o
obj-m := $(TARGET_MODULE).o

