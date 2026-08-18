ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

PROGRAM   = armtcnn

# Define files
#------------------------------------------

SRCDIRS   = ./src

# Define flags
#------------------------------------------
CXXFLAGS += -I. -ggdb \
                -DUSR_INCLUDE \
                -I$(APPLICATION_INSTALL_DIR)/inc/ \
                -I./inc \

ifeq ($(CONFIG_BIT), 32bits)
CXXFLAGS    += -D_FILE_OFFSET_BITS=64
CXXFLAGS += -std=c++11
endif

LDFLAGS += -L$(APPLICATION_INSTALL_DIR)/lib/ \
	  -lpthread -lrt -lcnn_operator -lmpi_npu -lmpi_dsp -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lopencv_highgui

LOCAL_LIB = -lcjson

include ../Makefile.param
# Include make rule file
#------------------------------------------
include ../../usr_lib_rules.mk
