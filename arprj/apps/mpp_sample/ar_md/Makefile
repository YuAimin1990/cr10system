# Define target, only one type can be choosed: program, static library or dynamic library.
#------------------------------------------
ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

PROGRAM   = ar_md

# Define files
#------------------------------------------

SRCDIRS   = ./

# Define flags
#------------------------------------------
LDFLAGS   += -L$(OUTPUT_APPLICATION_LIB_DIR) -losal -lutils -lpthread -lrt -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lopencv_highgui -lar_ivs -lhal_gdc -lmpi_gdc -lhal_npu -lhal_scaler -lhal_dsp -lmpi_scaler -lmpi_npu -lmpi_ifc -lmpi_vpss -lmpi_region -lhal_ge2d -lhal_region -lhal_vb -lhal_proc -lhal_sys
CFLAGS    += -I$(OUTPUT_APPLICATION_HEADER_DIR)
CXXFLAGS    += -I$(OUTPUT_APPLICATION_HEADER_DIR)

ifeq ($(CONFIG_FEAT_EIS),y)
LDFLAGS += -lhal_eis -lmpi_eis
endif

LOCAL_LIB = -lhal_sys -lcjson

# Include make rule file
#------------------------------------------
include ../../usr_lib_rules.mk

