ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

PROGRAM   = arcnn_yolov5

# Define files
#------------------------------------------

SRCDIRS   = ./src

# Define flags
#------------------------------------------
CXXFLAGS += -O3 -I. -ggdb \
                -DUSR_INCLUDE \
                -I$(APPLICATION_INSTALL_DIR)/inc/ \
                -I./inc \

LDFLAGS += -L$(APPLICATION_INSTALL_DIR)/lib/ \
	  -lpthread -lrt -lcnn_operator -lmpi_npu -lmpi_dsp -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lopencv_highgui

LOCAL_LIB =  -lcjson

include ../Makefile.param
# Include make rule file
#------------------------------------------
include ../../usr_lib_rules.mk
