# Define target, only one type can be choosed: program, static library or dynamic library.
#------------------------------------------
ifeq ($(PARAM_FILE), )
	PARAM_FILE:=../../../../../arsdk/Makefile.param
	include $(PARAM_FILE)
endif

PROGRAM = test_mpp_vpss_mpu

# Define files
#------------------------------------------

SRCDIRS = ./ \
		../../common

# Define flags
#------------------------------------------
CFLAGS += -I./ \
		-I../../common \
		-I../../../libs/sensor \
		-I../sample_vpss_fileio \
		-I$(OUTPUT_DIR)/prebuilds/apps/inc/ \
		-L$(OUTPUT_DIR)/prebuilds/apps/lib
CFLAGS += -I$(KERNEL_INSTALL_DIR)/khdr/include/linux/iio

CFLAGS += -Wno-implicit-function-declaration

LDFLAGS += -lpthread -lrt -lm

LOCAL_LIB =

include ../../Makefile.param
# Include make rule file
#------------------------------------------
include ../../../usr_lib_rules.mk
