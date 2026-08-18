# Define target, only one type can be choosed: program, static library or dynamic library.
#------------------------------------------
ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

PROGRAM   = test_mpp_vo_config

# Define files
#------------------------------------------

SRCDIRS   = ./ 

# Define flags
#------------------------------------------
LDFLAGS   += -lpthread -lrt -ldl -lm
CFLAGS    += -I../../common -I../../../libs/sensor/ -I../../../libs/panel/

LOCAL_LIB =

include ../../Makefile.param
# Include make rule file
#------------------------------------------
include ../../../usr_lib_rules.mk