#Define target, only one type can be choosed: program, static library or dynamic library.
#------------------------------------------
ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

PROGRAM   = ipcam

# Define files
#------------------------------------------

SRCDIRS   = ./ipc/src ./mid/cfg/src ./mid/eventalarm/src ./mid/mediabuffer/src ./mid/recmng/src ./mid/ssd/src ./mid/uart/src ./mid/utility/src ./mid/wireless/src ./platform/src ./thermal/common/src ./thermal/sensor

# Define flags
#------------------------------------------
LDFLAGS += -L$(APPLICATION_INSTALL_DIR)/lib -lpn_tp2803 -lcnn_operator -lacodec_tp9930 -lar_md5 -lar_ivs -lar_ive -lar_mov -lar_avi -lar_g711 -lar_minirtsp -lhal_vgs -lmpi_vgs -lrt -ldl -lm -lbinder_ipc -losal -lutils -lpthread -lstdc++\
           -lsns_imx290 -lsns_imx290_1 -lsns_imx290_2 -lsns_imx290_3 -lsns_imx415 -lsns_tp9930 -lsns_imx464 -lsns_sc910gs -lsns_sc530ai -lsns_os04a10 -lsns_imx347 -lsns_os05a20 -lhal_i2c -lbinder_ipc -lhal_clk -lhal_proc -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lopencv_highgui -lsns_sc230ai -lhal_aenc -ltools

CFLAGS  += -I./ipc/inc -I./mid/cfg/inc -I./mid/eventalarm/inc -I./mid/mediabuffer/inc -I./mid/recmng/inc -I./mid/ssd/inc -I./mid/uart/inc -I./mid/utility/inc -I./mid/wireless/inc \
           -I./platform/inc -I./thermal/common/inc \
		   -I../libs/sensor/ \
		   -I$(APPLICATION_INSTALL_DIR)/inc -Wno-unused-function -O3

LOCAL_LIB = -lcjson

LDFLAGS += -lsns_gst417w -lsns_isp_in_inf_thermal -lsns_isp_in_inf -lsns_isp_in1_inf -lsns_gst212w4 -lsns_h3812c1sg

#THERMAL_TYPE = nk1221a
ifneq ($(THERMAL_TYPE), )
ifneq ($(THERMAL_TYPE), nk1221a)
YUV_PROCESS_ENABLE = enable
endif
endif
ifeq ($(YUV_PROCESS_ENABLE), enable)
        LOCAL_LIB += -lARInfAlg 
        CFLAGS  += -I./thermal/ar_inf_alg/include -DYUV_PROCESS_ENABLE -g -O0
        LDFLAGS += -L./thermal/ar_inf_alg/lib -lARInfAlg
endif
ifeq ($(THERMAL_TYPE), timo256)
        SRCDIRS += ./thermal/common/src ./thermal/3rdparty/gd
        LOCAL_LIB += -lModuleSdk
        CFLAGS  += -I./thermal/3rdparty/gd -I./thermal/3rdparty/gd/libModuleSDK_AR9341/include -I./thermal/common/inc -DTHERMAL_ENABLE -DTHERMAL_TYPE_TIMO256 -g -O0
        LDFLAGS += -L./thermal/3rdparty/gd/libModuleSDK_AR9341/lib/release -lModuleSdk -lsns_timo256 -lsns_elf1 -lsns_isp_in_inf -lsns_nk1221a
endif
ifeq ($(THERMAL_TYPE), elf1)
        SRCDIRS += ./thermal/common/src ./thermal/3rdparty/iray
        LOCAL_LIB += -lirayrtd
        CFLAGS  += -I./thermal/common/inc -I./thermal/3rdparty/iray -I./thermal/3rdparty/iray/libirayrtd/include -DTHERMAL_ENABLE -DTHERMAL_TYPE_ELF1 -g -o0
        LDFLAGS += -L./thermal/3rdparty/iray/libirayrtd/lib -lirayrtd -lsns_timo256 -lsns_elf1 -lsns_isp_in_inf -lsns_nk1221a
endif

ifeq ($(THERMAL_TYPE), nk1221a)
        SRCDIRS += ./thermal/common/src
#        LOCAL_LIB += -lModuleSdk
        CFLAGS  += -I./thermal/common/inc -DTHERMAL_ENABLE -DTHERMAL_TYPE_NK1221A -g -O0
        LDFLAGS += -lsns_timo256 -lsns_elf1 -lsns_isp_in_inf -lsns_nk1221a
endif

#for ars31 infrared fusion
ifeq ($(THERMAL_TYPE), )
CFLAGS  += -I./thermal/ar_inf_alg_ars31/include
LIBS += -L./thermal/ar_inf_alg_ars31/lib -lARInfAlg
endif

include Makefile.param
# Include make rule file
#------------------------------------------
include ../usr_lib_rules.mk

