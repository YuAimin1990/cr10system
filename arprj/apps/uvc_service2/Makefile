# Define target, only one type can be choosed: program, static library or dynamic library.
#------------------------------------------
ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

PROGRAM   = uvc_service2

# Define files
#------------------------------------------

SRCDIRS   = ./

# Define flags
#------------------------------------------



LDFLAGS += -L$(APPLICATION_INSTALL_DIR)/lib -lar_md5 -lar_mov -lar_avi -lar_g711 -lar_minirtsp -lrt -ldl -lm -lbinder_ipc -losal -lutils -lpthread \
           -lsns_imx290 -lsns_sc132gs -lbinder_ipc

LDFLAGS += -losal -lutils -lhal_dbglog -lhal_ge2d -lhal_vb -lhal_sys -lhal_vdec -lhal_venc -lhal_vin -lhal_vo -lhal_aio -lhal_gpio -lhal_i2c \
           -lhal_region -lhal_scaler -lhal_gdc -lhal_npu -lhal_dsp -lhal_vgs -lhal_proc -lhal_clk -lhal_adec -lhal_aenc -lswresample -lavutil

LDFLAGS += -lmpi_vin -lmpi_vo -lmpi_vb -lmpi_vdec -lmpi_venc -lmpi_vpss -lmpi_sys -lmpi_hdmi -lmpi_region -lmpi_scaler -lmpi_gdc \
           -lmpi_ifc -lmpi_ai -lmpi_ao -lmpi_vgs -lmpi_adec -lmpi_aenc -lmpi_acodec

#LDFLAGS += -L$(OUTPUT_APPLICATION_LIB_DIR) -lsns_isp_in_0 -lcjson_utils -lsns_os04a10

LOCAL_LIB += -lfuse3 -lrpc_fs -lrpc_proxy

ifeq ($(CONFIG_FEAT_EIS),y)
LDFLAGS += -lhal_eis -lmpi_eis
endif


LDFLAGS += -lhal_analyze_time -lmpp_service

CFLAGS  +=	-O3 -I$(APPLICATION_INSTALL_DIR)/inc \
		-I$(KERNEL_INSTALL_DIR)/khdr/include \
		-I. \
        -I../libs/sensor/ \

ifeq ($(CONFIG_BIT), 32bits)
LOCAL_LIB +=  ./lib32/libyuv.a
else
LOCAL_LIB +=  ./libyuv.a
endif

#include Makefile.param
# Include make rule file
#------------------------------------------
include ../usr_lib_rules.mk

