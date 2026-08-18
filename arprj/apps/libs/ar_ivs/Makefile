# Define target, only one type can be choosed: program, static library or dynamic library.
#------------------------------------------
ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif
STATIC_LIBRARY = libar_ivs.a
SHARED_LIBRARY = libar_ivs.so

# Define files
#------------------------------------------

SRCDIRS   = ./


INSTALL_HEADERS := vibe-background-sequential.h \
                   ar_ivs.h \

# Define flags
#------------------------------------------

CFLAGS    +=  -I$(OUTPUT_APPLICATION_HEADER_DIR) -I./ -lpthread -lrt -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lopencv_highgui  -lhal_npu -O3 -ffast-math -ftree-vectorize 

LDFLAGS   += -L$(OUTPUT_APPLICATION_LIB_DIR)
# Include make rule file
#------------------------------------------
include ../../usr_lib_rules.mk

