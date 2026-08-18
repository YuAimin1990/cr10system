# Define target, only one type can be choosed: program, static library or dynamic library.
#------------------------------------------
ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

PROGRAM   = test_mpp_vdec

# Define files
#------------------------------------------

SRCDIRS   = ./ \
			../../common

# Define flags
#------------------------------------------
LDFLAGS   += -lpthread -lrt -ldl -lm -ltools
CFLAGS    += -I../../common -I../../../libs/sensor/

LOCAL_LIB =

include ../../Makefile.param
# Include make rule file
#------------------------------------------
include ../../../usr_lib_rules.mk

