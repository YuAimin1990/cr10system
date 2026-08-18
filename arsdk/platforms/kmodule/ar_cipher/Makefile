TARGET_MODULE:=ar_cipher
src = $(kernel_module_dir)/$(TARGET_MODULE)
EXTRA_CFLAGS += $(AR_DFLAGS)
EXTRA_CFLAGS += -I$(kernel_module_dir)/ar_osal/include
EXTRA_CFLAGS += -I$(kernel_module_dir)/ar_vb
EXTRA_CFLAGS += -I$(src)
EXTRA_CFLAGS += -I$(src)/osal/include
EXTRA_CFLAGS += -I$(src)/include
EXTRA_CFLAGS += -I$(src)/drivers/core
EXTRA_CFLAGS += -I$(src)/drivers/core/include
EXTRA_CFLAGS += -I$(src)/drivers/crypto/include

$(TARGET_MODULE)-objs := cipher_initdevice.o \
						./osal/drv_osal_init_linux.o \
						./drivers/kapi_dispatch.o \
						./drivers/kapi_symc.o \
						./drivers/crypto/cryp_symc.o \
						./drivers/core/drv_symc_artosyn.o \
						./drivers/core/spacc_cipher.o

obj-m := $(TARGET_MODULE).o
