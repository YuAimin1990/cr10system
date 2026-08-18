ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

PROGRAM   = ar_logcat

# Define files
#------------------------------------------

SRCDIRS   = ./

# Define flags
#------------------------------------------
LDFLAGS   += -lutils -losal -lpthread -lrt

CFLAGS    += -I./ \
            -DUSR_INCLUDE

# Include make rule file
#------------------------------------------

include ../../usr_lib_rules.mk
