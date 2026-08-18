# Define target, only one type can be choosed: program, static library or dynamic library.
#------------------------------------------
ifeq ($(PARAM_FILE), )
	PARAM_FILE:=../../../../arsdk/Makefile.param
	include $(PARAM_FILE)
endif

PROGRAM   = ar_iqtool_service

# Define files
#------------------------------------------

SRCDIRS   = $(CURDIR)/

# Define flags
#------------------------------------------
LDFLAGS   += -L$(APPLICATION_INSTALL_DIR)/lib -lhal_dbglog -losal -lutils -lrt -lpthread -lm -lcjson -lbinder_ipc

CFLAGS    += -I./ \
			 -I$(APPLICATION_INSTALL_DIR)/inc \

LOCAL_LIB = -lhal_vin -lhal_dbglog -losal -lrt -lhal_vb -lhal_sys -lhal_venc

LOCAL_LIB += -lfuse3 -lrpc_fs -lrpc_proxy -lhal_proc -lhal_analyze_time

LDFLAGS   += -ldl

LOCAL_LIB += -lmpp_service -lhal_npu -lswresample -lavutil -lhal_gpio -lhal_dsp -lhal_scaler

# Include make rule file
#------------------------------------------
include ../../usr_lib_rules.mk

