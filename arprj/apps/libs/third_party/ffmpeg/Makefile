ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif


prebuild:install
	
install:
	cp -ru prebuild/include/* $(OUTPUT_APPLICATION_HEADER_DIR)
ifeq ($(CONFIG_BIT), 32bits)
	cp -ru prebuild/lib32/* $(OUTPUT_APPLICATION_LIB_DIR)
else
	cp -ru prebuild/lib/* $(OUTPUT_APPLICATION_LIB_DIR)
endif

clean:
	rm -rf $(OUTPUT_APPLICATION_HEADER_DIR)/libavcodec
	rm -rf $(OUTPUT_APPLICATION_HEADER_DIR)/libavdevice
	rm -rf $(OUTPUT_APPLICATION_HEADER_DIR)/libavfilter
	rm -rf $(OUTPUT_APPLICATION_HEADER_DIR)/libavformat
	rm -rf $(OUTPUT_APPLICATION_HEADER_DIR)/libavutil
	rm -rf $(OUTPUT_APPLICATION_HEADER_DIR)/libswresample
	rm -rf $(OUTPUT_APPLICATION_HEADER_DIR)/libswscale

	rm -rf $(OUTPUT_APPLICATION_LIB_DIR)/libavcodec.*
	rm -rf $(OUTPUT_APPLICATION_LIB_DIR)/libavdevice.*
	rm -rf $(OUTPUT_APPLICATION_LIB_DIR)/libavfilter.*
	rm -rf $(OUTPUT_APPLICATION_LIB_DIR)/libavformat.*
	rm -rf $(OUTPUT_APPLICATION_LIB_DIR)/libavutil.*
	rm -rf $(OUTPUT_APPLICATION_LIB_DIR)/libswresample.*
	rm -rf $(OUTPUT_APPLICATION_LIB_DIR)/libswscale.*
