# Define target, only one type can be choosed: program, static library or dynamic library.
#------------------------------------------
ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

PROGRAM   = artosyn_upgrade

# Define files
#------------------------------------------
SRCDIRS   = ./src \
			./src/ubiformat \
			./src/libfdt

# Define flags
#------------------------------------------
CFLAGS    = -I./inc/ \
			-I./inc/tomcrypt/ \
			-I./inc/ubiformat \
			-I./inc/libfdt \
			-I$(OUTPUT_DIR)/prebuilds/kernel/khdr/include \
			-DLTM_DESC -DUSE_LTC -DLTC_SOURCE -DLTC_EASY

ifeq ($(CONFIG_BIT), 32bits)
LOCAL_LIB += -L./lib32
else
LOCAL_LIB += -L./lib
endif

LOCAL_LIB += -ltomcrypt -ltommath -luuid

# Include make rule file
#------------------------------------------
include ../../usr_lib_rules.mk
