# Define target, only one type can be choosed: program, static library or dynamic library.
#------------------------------------------
ifeq ($(PARAM_FILE), )
	PARAM_FILE:=../../../../arsdk/Makefile.param
	include $(PARAM_FILE)
endif

PROGRAM   = test_adc

# Define files
#------------------------------------------

SRCDIRS   = ./

# Define flags
#------------------------------------------
LDFLAGS   += -L$(APPLICATION_INSTALL_DIR)/lib -lhal_adc -lhal_sys -losal -lutils -lrt -lbinder_ipc

CFLAGS    += -I./\
             -I$(APPLICATION_INSTALL_DIR)/inc \

LOCAL_LIB += -lfuse3 -lrpc_fs -lrpc_proxy -lhal_proc -lhal_analyze_time

LDFLAGS   += -ldl

LDFLAGS   += -lpthread
LOCAL_LIB += -lmpp_service -lhal_npu -lhal_vb -lswresample -lavutil -lhal_gpio -lhal_dsp -lhal_dbglog -lhal_scaler

# Include make rule file
#------------------------------------------
include ../../usr_lib_rules.mk
