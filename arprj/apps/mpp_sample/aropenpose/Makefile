
ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

PROGRAM   = aropenpose

# Define files
#------------------------------------------

SRCDIRS   = ./src

# Define flags
#------------------------------------------

CXXFLAGS += -I. -ggdb -O0 \
                -DUSR_INCLUDE \
                -I$(APPLICATION_INSTALL_DIR)/inc/ \
                -I./inc \

LDFLAGS += -L$(APPLICATION_INSTALL_DIR)/lib/ \
	  -lpthread -lrt -lcnn_operator  -lmpi_npu -lmpi_dsp -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lopencv_highgui -lm

ifeq ($(CONFIG_FEAT_EIS),y)
LDFLAGS += -lhal_eis -lmpi_eis
endif

LOCAL_LIB =  -lcjson

include ../Makefile.param
# Include make rule file
#------------------------------------------
include ../../usr_lib_rules.mk


