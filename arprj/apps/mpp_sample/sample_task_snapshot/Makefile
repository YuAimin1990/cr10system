ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

PROGRAM   = sample_task_snapshot

# Define files
#------------------------------------------

SRCDIRS   = ./ ../common

# Define flags
#------------------------------------------
CXXFLAGS += -I. -ggdb \
                -DUSR_INCLUDE \
                -I$(APPLICATION_INSTALL_DIR)/inc/ \
                -I./inc \

CFLAGS    += -I../common  -I../../libs/sensor/
CFLAGS    += -Wno-unused-function

LDFLAGS += -L$(APPLICATION_INSTALL_DIR)/lib/ \
	  -lhal_sys -losal -lutils -lpthread -lrt -lhal_npu -lcnn_operator -lhal_dsp -lmpi_npu -lmpi_dsp -lhal_scaler -lmpi_scaler -lopencv_core -lopencv_imgproc -lopencv_imgcodecs


# Include make rule file
#------------------------------------------
include ../../usr_lib_rules.mk
include ../Makefile.param
