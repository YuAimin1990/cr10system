TARGET_MODULE:=ar_gdc
EXTRA_CFLAGS += $(AR_DFLAGS)
EXTRA_CFLAGS += -I$(kernel_module_dir)/$(TARGET_MODULE) 
$(TARGET_MODULE)-objs :=ar_gdc_main.o ar_gdc_hal.o
obj-m := $(TARGET_MODULE).o



