export KBUILD_EXTRA_SYMBOLS
TARGET_MODULE := cryptodev
EXTRA_CFLAGS  += $(AR_DFLAGS) -DENABLE_ASYNC
EXTRA_CFLAGS  += -I$(kernel_module_dir)/$(TARGET_MODULE)
$(TARGET_MODULE)-objs := ioctl.o main.o cryptlib.o authenc.o zc.o util.o
obj-m := $(TARGET_MODULE).o
