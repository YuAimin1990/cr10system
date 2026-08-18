#
#------------------------------------------

ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

PROGRAM   = ar_wdt_service

# Define files
#------------------------------------------

SRCDIRS   = ./

# Define flags
#------------------------------------------
LDFLAGS   += -lpthread -lhal_wdt
CFLAGS    += -I./inc \
            -ggdb
# Include make rule file
#------------------------------------------
include ../usr_lib_rules.mk

