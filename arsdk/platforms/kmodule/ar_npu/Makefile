#kernel module definitions
KBUILD_EXTRA_SYMBOLS += $(kernel_module_dir)/ar_osal/Module.symvers
export KBUILD_EXTRA_SYMBOLS

TARGET_MODULE := ar_npu
EXTRA_CFLAGS += $(AR_DFLAGS)
EXTRA_CFLAGS  += -I$(kernel_module_dir)/$(TARGET_MODULE)
EXTRA_CFLAGS  += -I$(kernel_module_dir)/ar_osal/include 
$(TARGET_MODULE)-objs := ar_npu_main.o ar_npu_hal.o ar_npu_warmup_scu.o ar_npu_warmup_param.o
obj-m := $(TARGET_MODULE).o

