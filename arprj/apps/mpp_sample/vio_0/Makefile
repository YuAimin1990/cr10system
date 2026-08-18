# Define target, only one type can be choosed: program, static library or dynamic library.
#------------------------------------------
ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

PROGRAM   = test_mpp_vio

# Define files
#------------------------------------------

SRCDIRS   = $(CURDIR)/ \
			$(CURDIR)/../common \
			$(CURDIR)/smp

# Define flags
#------------------------------------------
LDFLAGS   += -lpthread -lrt -ldl -lm
CFLAGS    += -I../common  -I../../libs/sensor/ 
CFLAGS    += -Wno-unused-function

ifeq ($(CONFIG_FEAT_EIS),y)
CFLAGS += -I$(KERNEL_INSTALL_DIR)/khdr/include/linux/iio
endif

LOCAL_LIB =

include ../Makefile.param
# Include make rule file
#------------------------------------------
include ../../usr_lib_rules.mk

