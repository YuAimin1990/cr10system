ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif
STATIC_LIBRARY = libsns_sc910gs.a
SHARED_LIBRARY = libsns_sc910gs.so

# Define files
#------------------------------------------

SRCDIRS   = $(CURDIR)/

INSTALL_HEADERS := 

# Define flags
#------------------------------------------

CFLAGS    += -I./  -I../

#LOCAL_LIB =
# Include make rule file
#------------------------------------------
include ../../../usr_lib_rules.mk


