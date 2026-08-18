PARAM_FILE:=../../../Makefile.param
include $(PARAM_FILE)

all: prepare troot install

prepare:
	mkdir -p $(TROOT_INSTALL_DIR)

troot:
	./build_fw.sh $(UBOOT_INSTALL_DIR)/u-boot-spl.bin
	./build_ddr.sh

install:
	@cp -f firmware/firmware.le $(TROOT_INSTALL_DIR)
	@cp -f tools/sboot_chains/M1_hdr.bin $(TROOT_INSTALL_DIR)

clean::
	rm -rf firmware/firmware.le
	rm -rf tools/sboot_chains/M1_hdr.bin
	rm -rf $(TROOT_INSTALL_DIR)
