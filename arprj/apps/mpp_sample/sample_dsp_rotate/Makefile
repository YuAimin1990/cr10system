ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

PROGRAM   = test_ceva_rotate

# Define files
#------------------------------------------

SRCDIRS   = ./src

CFLAGS += -I. -ggdb \
                -DUSR_INCLUDE \
                -I$(APPLICATION_INSTALL_DIR)/inc/ \
                -I./inc \

CXXFLAGS += -I. -ggdb \
                -DUSR_INCLUDE \
                -I$(APPLICATION_INSTALL_DIR)/inc/ \
                -I./inc \


# Define flags
#------------------------------------------
LDFLAGS   += -lpthread -lrt -ldl -lm
LDFLAGS   += -lmpi_dsp

LOCAL_LIB =

include ../Makefile.param
# Include make rule file
#------------------------------------------
include ../../usr_lib_rules.mk
