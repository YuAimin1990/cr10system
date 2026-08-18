# Define target, only one type can be choosed: program, static library or dynamic library.
#------------------------------------------
ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

PROGRAM   = test_timo256_raw

# Define files
#------------------------------------------

SRCDIRS   = ./ \
			./smp \
			../common \
			../util \
			./guide

# Define flags
#------------------------------------------
LDFLAGS   += -lpthread -lrt -ldl -lm
CFLAGS	  += -O3
CFLAGS    += -I../common \
			 -I../../libs/sensor/ \
			 -I. \
			 -I../util \
			 -I$(APPLICATION_INSTALL_DIR)/inc

CFLAGS       += -I./guide -I./3rdparty/libITA/include -I../ar_inf_alg/include

LIBS 		 += -L./3rdparty/libITA/lib -lITA \
				-L../ar_inf_alg/lib -lARInfAlg

LDFLAGS   += -L$(APPLICATION_INSTALL_DIR)/lib -lar_g711 -lar_minirtsp

LOCAL_LIB =

include ../Makefile.param
# Include make rule file
#------------------------------------------
include ../../usr_lib_rules.mk

