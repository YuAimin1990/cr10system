ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

PROGRAM   = bdcmd

# Define files
#------------------------------------------

SRCDIRS   = ./

# Define flags
#------------------------------------------
LDFLAGS   += -lutils -losal -lpthread -lrt -lbinder_ipc 

CFLAGS    += -I./ \
            -DUSR_INCLUDE

# Include make rule file
#------------------------------------------

include ../../usr_lib_rules.mk
