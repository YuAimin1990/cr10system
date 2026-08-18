# Define target, only one type can be choosed: program, static library or dynamic library.
#------------------------------------------
ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

PROGRAM   = test_mpp_acodec

# Define files
#------------------------------------------

SRCDIRS   = $(CURDIR)/ \
            $(CURDIR)/../common

# Define flags
#------------------------------------------
LDFLAGS   += -L$(APPLICATION_INSTALL_DIR)/lib -L$(OUTPUT_APPLICATION_LIB_DIR)
LDFLAGS   += -lmpi_aaccodec -laac_codec -lrt -ldl -lm -lpthread -lacodec_rt5633 -lstdc++
CFLAGS    += -I$(OUTPUT_APPLICATION_HEADER_DIR) -I../common -I../../libs/sensor
CFLAGS    += -Wno-unused-function

LOCAL_LIB =

include ../Makefile.param
# Include make rule file
#------------------------------------------
include ../../usr_lib_rules.mk

