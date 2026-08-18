# Define target, only one type can be choosed: program, static library or dynamic library.
#------------------------------------------
ifeq ($(PARAM_FILE), )
	PARAM_FILE:=../../../../arsdk/Makefile.param
	include $(PARAM_FILE)
endif

PROGRAM   = camera

LINK_SCRIPT = ./camera.lds
# Define files
#------------------------------------------

SRCDIRS   = $(CURDIR)/

# Define flags
#------------------------------------------
LDFLAGS   += -L$(APPLICATION_INSTALL_DIR)/lib -lar_g711 -lar_minirtsp -lhal_dbglog -lhal_gpio -losal -lutils -lrt -lpthread -lm -ldl -lbinder_ipc

CFLAGS    += -I./ \
             -I$(APPLICATION_INSTALL_DIR)/inc \

ifeq ($(BASE_BOARD), evb)
	CFLAGS += -D USE_EVB
else ifeq ($(BASE_BOARD), evbext)
	CFLAGS += -D USE_EVB_EXT
endif
CFLAGS    += -Wno-unused-function

LOCAL_LIB = -lhal_vin -lhal_vo -lhal_dbglog -losal -lutils -lrt -lhal_vb  -lhal_i2c -lhal_sys -lhal_venc -lhal_analyze_time

LOCAL_LIB += -lfuse3 -lrpc_fs -lrpc_proxy -lhal_proc

LOCAL_LIB += -lmpp_service -lhal_npu -lswresample -lavutil -lhal_gpio -lhal_dsp -lhal_scaler

# Include make rule file
#------------------------------------------
include ../../usr_lib_rules.mk

