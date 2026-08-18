ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

PROGRAM   = ardeeplab

# Define files
#------------------------------------------

SRCDIRS   = ./src ../common

# Define flags
#------------------------------------------
CXXFLAGS += -I. -ggdb \
                -DUSR_INCLUDE \
                -I$(APPLICATION_INSTALL_DIR)/inc/ \
                -I./inc \
                -I../common \
                -I../../libs/sensor \
#CFLAGS    += -I../common -I../../libs/sensor/

LDFLAGS += -L$(APPLICATION_INSTALL_DIR)/lib/ \
	  -lhal_sys -losal -lutils -lpthread -lrt -lhal_npu -lcnn_operator -lhal_dsp -lhal_scaler -lmpi_scaler -lmpi_npu -lmpi_dsp -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lopencv_highgui

LOCAL_LIB =  -lcjson

# Include make rule file
#------------------------------------------
include ../../usr_lib_rules.mk
