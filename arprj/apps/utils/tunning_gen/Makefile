# Define target, only one type can be choosed: program, static library or dynamic library.
#------------------------------------------
#ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
#endif

gCUSTOM_TUNING ?= ./tuning_ext.c
#

.phony: tuning
tuning:
	chmod 777 ./convert.sh
	./convert.sh $(OUTPUT_HAL_DIR)/inc $(OUTPUT_KMODULE_DIR)/inc ./inc $(gCUSTOM_TUNING)
	chmod 644 ./convert.sh

# Define files
#------------------------------------------
SRCDIRS   = $(CURDIR)/

# Define flags
#------------------------------------------
LDFLAGS   += -lpthread 

LDFLAGS   += -L$(OUTPUT_CORE_LIB_DIR) 
LOCAL_LIB =

CFLAGS    += -I./inc


# Include make rule file
#------------------------------------------
include ../../usr_lib_rules.mk
