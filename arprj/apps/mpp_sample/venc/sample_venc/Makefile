# Define target, only one type can be choosed: program, static library or dynamic library.
#------------------------------------------
ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

PROGRAM   = test_mpp_venc

# Define files
#------------------------------------------

SRCDIRS   = ./

# Define flags
#------------------------------------------
LDFLAGS   += -lpthread -lrt -ldl -lm -ltools
CFLAGS    += -I../../common

CFLAGS    += -I$(OUTPUT_APPLICATION_HEADER_DIR)
LDFLAGS   += -Wl,-rpath-link=$(OUTPUT_APPLICATION_LIB_DIR) -lswresample -lavutil -lavcodec -lavformat 

LOCAL_LIB =

include ../../Makefile.param
# Include make rule file
#------------------------------------------
include ../../../usr_lib_rules.mk

