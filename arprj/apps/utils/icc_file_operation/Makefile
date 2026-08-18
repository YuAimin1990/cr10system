ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

PROGRAM   = icc_file_operation

# Define files
#------------------------------------------

SRCDIRS   = ./

# Define flags
#------------------------------------------
CFLAGS += -I$(OUTPUT_APPLICATION_HEADER_DIR) \

CFLAGS    += -I./ \
 
LDFLAGS += -L$(OUTPUT_APPLICATION_LIB_DIR) \
		 -losal -lutils -lpthread -lrt -lhal_icc 

LOCAL_LIB =

# Include make rule file
#------------------------------------------
include ../../usr_lib_rules.mk
