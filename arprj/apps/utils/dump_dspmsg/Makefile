ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

PROGRAM   = dump_dspmsg

# Define files
#------------------------------------------

SRCDIRS   = ./

# Define flags
#------------------------------------------
CFLAGS += -I$(OUTPUT_APPLICATION_HEADER_DIR) \

CFLAGS    += -I./ \
		-I../../../../output/component_out/apps/inc \
 
LDFLAGS += -L$(OUTPUT_APPLICATION_LIB_DIR) \
		  -losal -lutils -lpthread -lrt

#LOCAL_LIB = -lplat

# Include make rule file
#------------------------------------------
include ../../usr_lib_rules.mk
