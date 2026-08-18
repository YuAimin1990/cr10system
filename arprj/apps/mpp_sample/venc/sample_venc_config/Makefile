# Define target, only one type can be choosed: program, static library or dynamic library.
#------------------------------------------
ifeq ($(PARAM_FILE), )
	PARAM_FILE:=../../../../../arsdk/Makefile.param
	include $(PARAM_FILE)
endif

PROGRAM = test_mpp_venc_config

# Define files
#------------------------------------------

SRCDIRS = ./

# Define flags
#------------------------------------------
CFLAGS += -I./ \
		-I../../../common \
		-I$(OUTPUT_DIR)/prebuilds/apps/inc/ \
		-L$(OUTPUT_DIR)/prebuilds/apps/lib

LDFLAGS += -lpthread -lrt -lm -lbinder_ipc -ldl -ltools

LOCAL_LIB =

include ../../Makefile.param
# Include make rule file
#------------------------------------------
include ../../../usr_lib_rules.mk
