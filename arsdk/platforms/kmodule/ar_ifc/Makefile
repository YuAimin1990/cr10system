TARGET_MODULE:=ar_ifc
EXTRA_CFLAGS += $(AR_DFLAGS)
EXTRA_CFLAGS += -I$(kernel_module_dir)/$(TARGET_MODULE)
$(TARGET_MODULE)-objs :=ar_ifc_main.o ar_ifc_hal.o
obj-m := $(TARGET_MODULE).o

