ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../../arsdk/Makefile.param
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
	rm -rf $(OUTPUT_APPLICATION_HEADER_DIR)/opencv
	rm -rf $(OUTPUT_APPLICATION_HEADER_DIR)/opencv2
	rm -rf $(OUTPUT_APPLICATION_LIB_DIR)/libopencv*
