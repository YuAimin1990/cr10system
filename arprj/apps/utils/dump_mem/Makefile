# Define target, only one type can be choosed: program, static library or dynamic library.
#------------------------------------------
ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

PROGRAM   = dump_mem

# Define files
#------------------------------------------

SRCDIRS   = ./

# Define flags
#------------------------------------------
LDFLAGS   += -lpthread 

LDFLAGS   += -L$(OUTPUT_CORE_LIB_DIR) 
LOCAL_LIB =

CFLAGS    += -I./inc

# Include make rule file
#------------------------------------------
include ../../usr_lib_rules.mk
