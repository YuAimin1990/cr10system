#kernel module definitions

TARGET_MODULE := ar_mpp_drv
EXTRA_CFLAGS += $(AR_DFLAGS)
EXTRA_CFLAGS  = -I$(kernel_module_dir)/$(TARGET_MODULE)

$(TARGET_MODULE)-objs := ar_mpp_irq.o
obj-m := $(TARGET_MODULE).o
