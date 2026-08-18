# Define target, only one type can be choosed: program, static library or dynamic library.
#------------------------------------------
ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

PROGRAM   = uac_service

# Define files
#------------------------------------------

SRCDIRS   = ./

# Define flags
#------------------------------------------



LDFLAGS += -L$(APPLICATION_INSTALL_DIR)/lib -lrt -ldl -lm -lpthread

LDFLAGS += -losal -lutils -lhal_dbglog -lhal_vb -lhal_sys -lhal_aio -lbinder_ipc -lhal_adec -lhal_aenc -lhal_vdec -lhal_venc -lhal_scaler -lhal_gdc -lhal_region -lhal_ge2d -lhal_proc -lhal_npu -lhal_dsp -lhal_i2c -lhal_clk -lhal_vin

LDFLAGS += -lmpi_vb -lmpi_sys -lmpi_ai -lmpi_ao -lmpi_adec -lmpi_aenc -lmpi_acodec -lmpi_vdec -lmpi_venc -lmpi_vpss -lmpi_ifc -lmpi_region -lmpi_scaler -lmpi_gdc -lmpi_vin

LDFLAGS += -L$(OUTPUT_APPLICATION_LIB_DIR) -lcjson_utils -lacodec_rt5633

ifeq ($(CONFIG_FEAT_EIS),y)
LDFLAGS += -lhal_eis -lmpi_eis
endif

CFLAGS  +=	-O3 -I$(OUTPUT_APPLICATION_HEADER_DIR) \
		-I$(KERNEL_INSTALL_DIR)/khdr/include

ifeq ($(CONFIG_BIT), 32bits)
CFLAGS  += -I./alsa-lib-32bit/include
CFLAGS	+= -mfpu=neon-vfpv4 -mfloat-abi=hard
LDFLAGS += -L./alsa-lib-32bit/lib/ -lasound -latopology
LOCAL_LIB += ./alsa-lib-32bit/lib/libasound.a
else
CFLAGS  += -I./alsa-lib/include
LDFLAGS += -L./alsa-lib/lib/ -lasound -latopology
LOCAL_LIB += ./alsa-lib/lib/libasound.a
endif

LOCAL_LIB += -lfuse3 -lrpc_fs -lrpc_proxy

LDFLAGS += -lhal_analyze_time -lmpp_service -lswresample -lavutil -lhal_gpio

#include Makefile.param
# Include make rule file
#------------------------------------------
include ../usr_lib_rules.mk

