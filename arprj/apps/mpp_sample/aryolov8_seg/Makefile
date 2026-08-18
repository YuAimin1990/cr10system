ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

PROGRAM   = aryolov8_seg

# Define files
#------------------------------------------

SRCDIRS   = ./src ../common

# Define flags
#------------------------------------------
CFLAGS 	 += -I. -I../common \
                -I../../libs/sensor/
CXXFLAGS += -I. -ggdb \
                -DUSR_INCLUDE \
                -I$(APPLICATION_INSTALL_DIR)/inc/ \
                -I./inc \
                $(CFLAGS)

CXXFLAGS += -O3

LDFLAGS += -L$(APPLICATION_INSTALL_DIR)/lib/ \
	  -lhal_sys -losal -lutils -lpthread -lrt -lhal_npu -lhal_scaler -lmpi_scaler -lmpi_npu -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lopencv_highgui

LOCAL_LIB =  -lcjson

include ../Makefile.param
# Include make rule file
#------------------------------------------
include ../../usr_lib_rules.mk
