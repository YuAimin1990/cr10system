# Define target, only one type can be choosed: program, static library or dynamic library.
#------------------------------------------
ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

SHARED_LIBRARY = libar_mmap.so

# Define files
#------------------------------------------

SRCDIRS   = ./

INSTALL_HEADERS := ar_mmap.h

# Define flags
#------------------------------------------

CFLAGS    += -I. -Werror

# Include make rule file
#------------------------------------------
include ../../usr_lib_rules.mk

