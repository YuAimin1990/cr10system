#Define target, only one type can be choosed: program, static library or dynamic library.
#------------------------------------------
ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

PROGRAM   = raw_simu

# Define files
#------------------------------------------

SRCDIRS   = ./ipc/src ./mid/cfg/src ./mid/mediabuffer/src ./mid/recmng/src ./mid/utility/src ./platform/src 

# Define flags
#------------------------------------------
LDFLAGS += -L$(APPLICATION_INSTALL_DIR)/lib -lar_md5 -lar_mov -lar_avi -lar_minirtsp -lrt -ldl -lm -losal -lutils -lpthread -lstdc++ -lhal_i2c -lhal_clk -lhal_proc -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lopencv_highgui -lsns_isp_in_0 -lar_g711 

CFLAGS  += -I./ipc/inc -I./mid/cfg/inc  -I./mid/mediabuffer/inc -I./mid/recmng/inc -I./mid/utility/inc -I./platform/inc -I../../libs/sensor/ -I$(APPLICATION_INSTALL_DIR)/inc -Wno-unused-function -O3

LOCAL_LIB = -lcjson

include ../Makefile.param
# Include make rule file
#------------------------------------------
include ../../usr_lib_rules.mk

