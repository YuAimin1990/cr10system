# Define target, only one type can be choosed: program, static library or dynamic library.
#------------------------------------------
ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

PROGRAM   = test_mpp_stream

# Define files
#------------------------------------------

SRCDIRS   = $(CURDIR)/ \
			$(CURDIR)/../common

# Define flags
#------------------------------------------
LDFLAGS   +=  -lmpi_aaccodec -laac_codec -lpthread -lrt -ldl -lm -lstdc++
CFLAGS    += -I../common -I../../libs/sensor -I./ffmpeg/include
CFLAGS    += -Wno-unused-function

ifeq ($(CONFIG_BIT), 32bits)
LOCAL_LIB = ./ffmpeg/lib32/libavcodec.a ./ffmpeg/lib32/libavformat.a ./ffmpeg/lib32/libavutil.a
else
LOCAL_LIB = ./ffmpeg/lib/libavcodec.a ./ffmpeg/lib/libavformat.a ./ffmpeg/lib/libavutil.a
endif

include ../Makefile.param
# Include make rule file
#------------------------------------------
include ../../usr_lib_rules.mk

