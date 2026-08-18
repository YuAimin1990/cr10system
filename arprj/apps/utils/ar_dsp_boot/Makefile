ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

PROGRAM   = ar_dsp_boot

# Define files
#------------------------------------------

SRCDIRS   = ./src

# Define flags
#------------------------------------------
CFLAGS += -I$(OUTPUT_APPLICATION_HEADER_DIR) \

CFLAGS    += -I./inc \
 
LDFLAGS += -L$(OUTPUT_APPLICATION_LIB_DIR) \
		 -losal -lutils -lpthread -lrt -lbinder_ipc -lhal_vb -lhal_sys -lhal_dsp -lmpi_dsp

LOCAL_LIB += -lfuse3 -lrpc_fs -lrpc_proxy -lhal_proc

LDFLAGS += -ldl

LOCAL_LIB += -lhal_analyze_time -lmpp_service -lhal_npu -lhal_vb -lswresample -lavutil -lhal_gpio -lhal_dsp -lhal_dbglog -lhal_scaler

# Include make rule file
#------------------------------------------
include ../../usr_lib_rules.mk
