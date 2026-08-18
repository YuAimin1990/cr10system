ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

PROGRAM   = sample_rpc

# Define files
#------------------------------------------

SRCDIRS   = ./

# Define flags
#------------------------------------------
LDFLAGS += -lpthread -lrt -ldl -lm
CFLAGS  += -I../common -I../../libs/sensor/

include ../Makefile.param
# Include make rule file
#------------------------------------------
include ../../usr_lib_rules.mk
