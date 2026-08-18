ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif
STATIC_LIBRARY = libsns_sc530ai.a
SHARED_LIBRARY = libsns_sc530ai.so

# Define files
#------------------------------------------

SRCDIRS   = ./

INSTALL_HEADERS := 

# Define flags
#------------------------------------------

CFLAGS    += -I./  -I../

#LOCAL_LIB =
# Include make rule file
#------------------------------------------
include ../../../usr_lib_rules.mk


