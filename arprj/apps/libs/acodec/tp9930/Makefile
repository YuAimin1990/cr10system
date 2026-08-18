ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif
STATIC_LIBRARY = libacodec_tp9930.a
SHARED_LIBRARY = libacodec_tp9930.so

# Define files
#------------------------------------------

SRCDIRS   = $(CURDIR)/

INSTALL_HEADERS := ./tp9930_audio.h 

# Define flags
#------------------------------------------

CFLAGS    += -I./   -I../

#LOCAL_LIB =
# Include make rule file
#------------------------------------------
include ../../../usr_lib_rules.mk


