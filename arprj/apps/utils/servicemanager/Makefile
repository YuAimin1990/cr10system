# Define target, only one type can be choosed: program, static library or dynamic library.
#------------------------------------------
ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

PROGRAM  = servicemanager

# Define files
#------------------------------------------

SRCDIRS   = ./

# Define flags
#------------------------------------------

LDFLAGS   +=

ifeq ($(CONFIG_BIT), 32bits)
CFLAGS += -DBINDER_IPC_32BIT
endif

# Include make rule file
#------------------------------------------
include ../../usr_lib_rules.mk

