#kernel module definitions

TARGET_MODULE := ar_mpp_proc_ctrl
EXTRA_CFLAGS += $(AR_DFLAGS)
EXTRA_CFLAGS  += -I$(kernel_module_dir)/$(TARGET_MODULE)

$(TARGET_MODULE)-objs := ar_mpp_proc.o
obj-m := $(TARGET_MODULE).o
