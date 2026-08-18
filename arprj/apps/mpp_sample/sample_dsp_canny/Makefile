ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

PROGRAM   = sample_ceva_canny

# Define files
#------------------------------------------

SRCDIRS   = ./

# Define flags
#------------------------------------------
LDFLAGS   += -lpthread -lrt -ldl -lm
LDFLAGS   += -lmpi_dsp

LOCAL_LIB =

include ../Makefile.param
# Include make rule file
#------------------------------------------
include ../../usr_lib_rules.mk
