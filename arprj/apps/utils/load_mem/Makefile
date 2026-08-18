# Define target, only one type can be choosed: program, static library or dynamic library.
#------------------------------------------
ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

PROGRAM   = load_mem

# Define files
#------------------------------------------

SRCDIRS   = ./

# Define flags
#------------------------------------------

#LDFLAGS   = -lartosyn
LDFLAGS   +=
CFLAGS    += -I./inc

# Include make rule file
#------------------------------------------
include ../../usr_lib_rules.mk

