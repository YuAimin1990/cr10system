# Define target, only one type can be choosed: program, static library or dynamic library.
#------------------------------------------
ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif
SHARED_LIBRARY = libcnn_operator.so

# Define files
#------------------------------------------

SRCDIRS   = ./src/

INSTALL_HEADERS := ./inc/ar_cnn_op_common.h \
			 ./inc/ar_cnn_operator.h \
			    ./inc/ar_cnn_parser.h \
				./inc/ar_cnn_channelcvt.h

# Define flags
#------------------------------------------

CFLAGS    += -I$(OUTPUT_APPLICATION_HEADER_DIR) -I./inc/ -lhal_npu -lmpi_npu -lhal_sys -lhal_dsp -lpthread -lrt -O3 -ftree-vectorize
LDFLAGS   += -L$(OUTPUT_APPLICATION_LIB_DIR)
# Include make rule file
#------------------------------------------
include ../../usr_lib_rules.mk
