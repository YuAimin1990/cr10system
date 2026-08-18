# Define target, only one type can be choosed: program, static library or dynamic library.
#------------------------------------------
ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

PROGRAM   = ddr_loading

# Define files
#------------------------------------------

SRCDIRS   = ./

# Define flags
#------------------------------------------

#LDFLAGS   = -lartosyn
LDFLAGS   += -lhal_sys -lpthread -losal -lutils -lrt -ldl -lm -lbinder_ipc -lrpc_proxy -lrpc_fs -lfuse3 -lhal_proc
CFLAGS    += -I./inc

# Include make rule file
#------------------------------------------
include ../../usr_lib_rules.mk

