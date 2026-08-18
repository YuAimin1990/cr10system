ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

SHARED_LIBRARY = libar_fbg.so

CFLAGS    += -Isrc -Werror

SRCDIRS   = src src/lodepng src/nanojpeg custom_backend/fbdev
INSTALL_HEADERS := src/fbgraphics.h custom_backend/fbdev/fbg_fbdev.h

include ../../../usr_lib_rules.mk

