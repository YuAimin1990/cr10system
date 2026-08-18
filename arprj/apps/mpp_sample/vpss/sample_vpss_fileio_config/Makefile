# Define target, only one type can be choosed: program, static library or dynamic library.
#------------------------------------------
ifeq ($(PARAM_FILE), )
	PARAM_FILE:=../../../../../arsdk/Makefile.param
	include $(PARAM_FILE)
endif

PROGRAM = test_mpp_vpss_fileio_config

# Define files
#------------------------------------------

SRCDIRS = ./ \
		../../common

# Define flags
#------------------------------------------
CFLAGS += -I./ \
		-I../../common \
		-I../../../libs/sensor/ \
		-I../sample_vpss_fileio \
		-I$(OUTPUT_DIR)/prebuilds/apps/inc/ \
		-L$(OUTPUT_DIR)/prebuilds/apps/lib

LDFLAGS += -lpthread -lrt -lm -lbinder_ipc -ldl

LOCAL_LIB =

include ../../Makefile.param
# Include make rule file
#------------------------------------------
include ../../../usr_lib_rules.mk
