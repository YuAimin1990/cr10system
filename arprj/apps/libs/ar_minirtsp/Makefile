# Define target, only one type can be choosed: program, static library or dynamic library.
#------------------------------------------
ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif
SHARED_LIBRARY = libar_minirtsp.so

# Define files
#------------------------------------------

SRCDIRS   = ./src/

INSTALL_HEADERS := ./inc/ar_rtsp_server.h ./inc/rtp.h 

# Define flags
#------------------------------------------

CFLAGS    = -I./inc/ -I../ar_g711/inc -I$(APPLICATION_INSTALL_DIR)/inc

# Include make rule file
#------------------------------------------
include ../../usr_lib_rules.mk

