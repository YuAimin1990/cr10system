ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

SHARED_LIBRARY = libar_ge2d_wrapper.so
SRCDIRS   = ./

INSTALL_HEADERS = ar_ge2d.h

CFLAGS    += -I.

# Include make rule file
#------------------------------------------
include ../../usr_lib_rules.mk
