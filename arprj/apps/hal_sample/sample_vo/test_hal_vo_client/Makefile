# Define target, only one type can be choosed: program, static library or dynamic library.
#------------------------------------------
ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

PROGRAM  = test_hal_vo_client

# Define files
#------------------------------------------

SRCDIRS   = ./

# Define flags
#------------------------------------------
LDFLAGS   += -L$(OUTPUT_APPLICATION_LIB_DIR) -lhal_vo -losal -lutils -lpthread -lrt -lbinder_ipc
CFLAGS    += -I$(OUTPUT_APPLICATION_HEADER_DIR) -I../test_hal_vo_server

LOCAL_LIB = -lhal_sys

LOCAL_LIB += -lfuse3 -lrpc_fs -lrpc_proxy -lhal_proc -lhal_analyze_time

LDFLAGS   += -ldl

LOCAL_LIB += -lmpp_service -lhal_npu -lhal_vb -lswresample -lavutil -lhal_gpio -lhal_dsp -lhal_dbglog -lhal_scaler

# Include make rule file
#------------------------------------------
include ../../../usr_lib_rules.mk

