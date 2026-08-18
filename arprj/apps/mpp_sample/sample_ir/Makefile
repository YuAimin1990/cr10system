# Define target, only one type can be choosed: program, static library or dynamic library.
#------------------------------------------
ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

PROGRAM   = ir_sample

# Define files
#------------------------------------------

SRCDIRS   = ./ \
			./smp \
			../common \
			../util

# Define flags
#------------------------------------------
LDFLAGS   += -lpthread -lrt -ldl -lm -lstdc++
CFLAGS	  += -O3
CFLAGS    += -I../common \
			 -I../../libs/sensor/ \
			 -I. \
			 -I../util \
			 -I$(APPLICATION_INSTALL_DIR)/inc
				  
# CFLAGS       += -I./guide -I./3rdparty/libModuleSDK_AR9341/include -I../ar_inf_alg/include

# LIBS 		 += -L./3rdparty/libModuleSDK_AR9341/lib/release -lModuleSdk \
# 				-L../ar_inf_alg/lib -lARInfAlg

CFLAGS       += -I./ar_inf_alg/include

LIBS 		 += -L./ar_inf_alg/lib -lARInfAlg

LDFLAGS   += -L$(APPLICATION_INSTALL_DIR)/lib -lar_g711 -lar_minirtsp -lopencv_core -lopencv_imgproc

LOCAL_LIB =

include ../Makefile.param
# Include make rule file
#------------------------------------------
include ../../usr_lib_rules.mk

