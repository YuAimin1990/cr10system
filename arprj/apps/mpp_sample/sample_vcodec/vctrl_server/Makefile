# Define target, only one type can be choosed: program, static library or dynamic library.
#------------------------------------------
ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

PROGRAM   = vctrl_server

# Define files
#------------------------------------------

SRCDIRS   = ./ \

# Define flags
#------------------------------------------
#LDFLAGS   += -lmpi_vdec -lmpi_venc -lmpi_vb -lmpi_sys -lmpi_vpss -lmpi_region -lmpi_scaler -lmpi_gdc -lmpi_vin -lmpi_ifc
#LDFLAGS   += -losal -lutils -lhal_venc -lhal_vdec -lhal_vb -lhal_sys -lhal_dbglog -lhal_aio -lhal_region -lhal_scaler -lhal_gdc -lhal_vin -lhal_npu
LDFLAGS   += -lpthread -lrt -ldl -lm -ltools
#CFLAGS    += -I../../common
CFLAGS    += -I../include

LOCAL_LIB =

include ../../Makefile.param
# Include make rule file
#------------------------------------------
include ../../../usr_lib_rules.mk

