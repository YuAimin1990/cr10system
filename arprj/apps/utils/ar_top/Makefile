# Define target, only one type can be choosed: program, static library or dynamic library.
#------------------------------------------
ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

PROGRAM   = ar_top

# Define files
#------------------------------------------

SRCDIRS   = $(CURDIR)/

# Define flags
#------------------------------------------
LDFLAGS   += -L$(APPLICATION_INSTALL_DIR)/lib -lhal_dbglog -losal -lutils -lrt -lpthread -lm -lcjson -lbinder_ipc

CFLAGS    += -I./ \
			 -I$(APPLICATION_INSTALL_DIR)/inc \

# Include make rule file
#------------------------------------------
include ../../usr_lib_rules.mk

