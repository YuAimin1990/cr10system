export KBUILD_EXTRA_SYMBOLS
TARGET_MODULE := ar_dsp
EXTRA_CFLAGS  += $(AR_DFLAGS)
EXTRA_CFLAGS  += -I$(kernel_module_dir)/$(TARGET_MODULE)
$(TARGET_MODULE)-objs := ar_dsp_platform.o dsp_proc_seq.o
obj-m := $(TARGET_MODULE).o

