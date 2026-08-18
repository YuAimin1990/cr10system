# Define target, only one type can be choosed: program, static library or dynamic library.
#------------------------------------------
ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif
STATIC_LIBRARY = libar_g711.a
SHARED_LIBRARY = libar_g711.so

# Define files
#------------------------------------------

SRCDIRS   = ./src/

INSTALL_HEADERS := ./inc/ar_g711.h

# Define flags
#------------------------------------------

CFLAGS    += -I./inc/ -lar_plat -lpthread -lrt -Werror

# Include make rule file
#------------------------------------------
include ../../usr_lib_rules.mk

