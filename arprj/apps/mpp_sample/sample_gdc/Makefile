# Define target, only one type can be choosed: program, static library or dynamic library.
#------------------------------------------
ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

PROGRAM   = test_mpp_gdc

# Define files
#------------------------------------------

SRCDIRS   = ./

# Define flags
#------------------------------------------
LDFLAGS   += -L$(OUTPUT_APPLICATION_LIB_DIR) -lpthread -lrt -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lopencv_highgui -lar_ivs -lmpi_sysctl
CXXFLAGS    += -I$(OUTPUT_APPLICATION_HEADER_DIR)

ifeq ($(CONFIG_FEAT_EIS),y)
LDFLAGS += -lhal_eis -lmpi_eis
endif

LOCAL_LIB = -lhal_sys -lcjson

include ../Makefile.param
# Include make rule file
#------------------------------------------
include ../../usr_lib_rules.mk
