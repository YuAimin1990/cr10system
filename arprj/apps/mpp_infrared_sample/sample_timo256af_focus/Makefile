# Define target, only one type can be choosed: program, static library or dynamic library.
#------------------------------------------
ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

PROGRAM   = test_timo256af_focus

# Define files
#------------------------------------------

SRCDIRS   = ./ \
			#../util \

# Define flags
#------------------------------------------
LDFLAGS   += -lpthread -lrt -ldl -lm
#CFLAGS	  += -O3
CFLAGS    += -I../common \
			 -I. \
			 -I../util \
			 -I$(APPLICATION_INSTALL_DIR)/inc

LDFLAGS   += -L$(APPLICATION_INSTALL_DIR)/lib

LOCAL_LIB =

#include ../Makefile.param
# Include make rule file
#------------------------------------------
include ../../usr_lib_rules.mk

