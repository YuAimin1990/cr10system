# the input parameters
ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

bootload-sec-img        ?= $(UBOOT_INSTALL_DIR)/troot_spl.sec.img
spl-raw                 ?= $(UBOOT_INSTALL_DIR)/u-boot-spl.bin
spl-elf                 ?= $(UBOOT_INSTALL_DIR)/u-boot-spl
spl-nodtb-raw           ?= $(UBOOT_INSTALL_DIR)/u-boot-spl-nodtb.bin
troot-firmware-raw      ?= $(TROOT_INSTALL_DIR)/firmware.le
troot-sign-raw          ?= $(TROOT_INSTALL_DIR)/M1_hdr.bin

uboot-raw               ?= $(UBOOT_INSTALL_DIR)/u-boot.bin
uboot-elf               ?= $(UBOOT_INSTALL_DIR)/u-boot
uboot-sec-img           ?= $(UBOOT_INSTALL_DIR)/uboot.sec.img
uboot-encrypt-key       ?= $(UTILITY_DIR)/data/aeskey/sign.txt

uboot-env-sec-img       ?= $(UBOOT_INSTALL_DIR)/u-boot-env.sec.img

vendor-sec-img          ?= $(UBOOT_INSTALL_DIR)/vendor.sec.img

kernel-sec-img          ?= $(KERNEL_INSTALL_DIR)/kernel.sec.img

rootfs-sec-img          ?= $(FS_INSTALL_DIR)/all_rootfs.sec.ext4
bl31-sec-img            ?= bl31.sec.img
bl31-nonsec-img         ?= bl31.nonsec.img

genpackbootloader       ?= $(UTILITY_DIR)/bin/gen_packed_bootloader.py
genimagetool-program    ?= $(UTILITY_DIR)/bin/genimage_program
genimagetool-upgrade    ?= $(UTILITY_DIR)/bin/genimage_upgrade
generate-ubi            ?= $(UTILITY_DIR)/bin/generate_ubi.sh
generate-nand-configs   ?= $(UTILITY_DIR)/bin/generate_nand_configs_sofi.sh
signtool                ?= $(UTILITY_DIR)/bin/sign

include $(PRJDIR)/hosts/versions/sdk.version
sinclude $(SECURITY_CFG_MK)

CROSS_COMPILE=/opt/toolchain/7.5.0/gcc-linaro-7.5.0-2019.12-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-

# create the final image
image:
	@echo -e '\033[32;1m =============== Generating program image =============== \033[0m'
	@mkdir -p $(IMAGE_INSTALL_DIR)
ifeq (x$(SEC_BOOT),xy)
	make $(bootload-sec-img)
	make $(uboot-sec-img)
	make $(uboot-env-sec-img)
	make $(vendor-sec-img)
	make $(kernel-sec-img)
    ifneq ($(findstring $(CHIP), ar9311 ars31),)
	make $(bl31-sec-img)
    endif
	cp -u $(bootload-sec-img) $(UBOOT_INSTALL_DIR)/troot_spl.img
	cp -u $(uboot-env-sec-img) $(UBOOT_INSTALL_DIR)/u-boot-env
	cp -u $(uboot-sec-img) $(UBOOT_INSTALL_DIR)/uboot.img
	cp -u $(vendor-sec-img) $(UBOOT_INSTALL_DIR)/vendor.img
	cp -u $(kernel-sec-img) $(KERNEL_INSTALL_DIR)/kernel.img
    ifneq ($(findstring $(CHIP), ar9311 ars31),)
	cp -u $(bl31-sec-img) $(UBOOT_INSTALL_DIR)/bl31.img
    endif
else
    ifneq ($(findstring $(CHIP), ar9311 ars31),)
	make $(bl31-nonsec-img)
    endif
	cp -u $(UBOOT_INSTALL_DIR)/troot_spl.nonsec.img $(UBOOT_INSTALL_DIR)/troot_spl.img
	cp -u $(UBOOT_INSTALL_DIR)/uboot.nonsec.img $(UBOOT_INSTALL_DIR)/uboot.img
	cp -u $(UBOOT_INSTALL_DIR)/vendor.nonsec.img $(UBOOT_INSTALL_DIR)/vendor.img
	cp -u $(KERNEL_INSTALL_DIR)/kernel.nonsec.img $(KERNEL_INSTALL_DIR)/kernel.img
    ifneq ($(findstring $(CHIP), ar9311 ars31),)
	cp -u $(ARM_TRUSTED_FIRMWARE_DIR)/$(bl31-nonsec-img) $(UBOOT_INSTALL_DIR)/bl31.img
    endif
endif
	# append 16KB for ddr restore registers
	@truncate -s +16K $(UBOOT_INSTALL_DIR)/vendor.img

	@$(genimagetool-program) $(gIMAGE_INI_FILE) $(generate-ubi) $(gOBJ_VERSION)
	@if [ -f "$(BOOT_MEDIA).img" ] ; then \
		echo "$(BOOT_MEDIA).img create successfully!"; \
		mv $(BOOT_MEDIA).img $(IMAGE_INSTALL_DIR) ; \
		if [ $(BOOT_MEDIA) == "nand" ] ; then \
			$(generate-nand-configs) $(gIMAGE_INI_FILE) nand.csv; \
			if [ $$? -eq 0 ]; then \
				echo "nand.csv generate successfully!" ; \
				mv nand.csv $(IMAGE_INSTALL_DIR) ; \
			else \
				echo "mcp-nand.csv generate failed!!!" ; \
			fi; \
		fi; \
	else \
		echo "$(BOOT_MEDIA).img create failed!"; \
	fi

	@echo -e '\033[32;1m =============== Generating upgrade image ===============  \033[0m'
	$(genimagetool-upgrade) $(gIMAGE_INI_FILE) $(upgrade-image-sign-key) $(generate-ubi) $(gSDK_VERSION) $(gDEV_PHASE) $(gOBJ_VERSION) $(gDEP_VERSION)
	@mv artosyn-upgrade* $(IMAGE_INSTALL_DIR)/artosyn-upgrade-$(CHIP).img

$(bootload-sec-img): force
	@echo -e '\033[32;1m  ============== bootload-sec-img: $(notdir $@) =============="  \033[0m'
	v_entry_addr=$(shell $(READELF) -h $(spl-elf) | grep "Entry" | cut -d : -f2 | sed 's/[[:space:]]//g') ; \
	python2 $(genpackbootloader) \
            --out_file=$@ \
            --spl_file=$(spl-raw) \
            --spl_nodtb_file=$(spl-nodtb-raw) \
            --spl_addr=$$v_entry_addr \
            --fw_file=$(troot-firmware-raw) \
            --fw_addr=0x17d0000 \
            --sig_file=$(troot-sign-raw) \
            --sig_addr=0x17e9800 \
            --chip=$(CHIP)

$(uboot-env-sec-img): force
	@echo -e '\033[32;1m ==============generate uboot-env sec img: $(notdir $@) ============== \033[0m'
	cp -u $(UBOOT_INSTALL_DIR)/u-boot-env $(UBOOT_INSTALL_DIR)/u-boot-env.orig
	#reserve 512 bytes for ar_header 64bytes digest 32bytes signature 256bytes
	@truncate -s 32256 $(UBOOT_INSTALL_DIR)/u-boot-env
	$(signtool) -i $(UBOOT_INSTALL_DIR)/u-boot-env -o $@ -s $(uboot-env-sign-key)

$(uboot-sec-img): force
	@echo -e '\033[32;1m ==============generate uboot sec img: $(notdir $@) ============== \033[0m'
	v_entry_addr=$(shell $(READELF) -h $(uboot-elf) | grep "Entry" | cut -d : -f2 | sed 's/[[:space:]]//g') ; \
	$(UTILITY_DIR)/bin/mkimage -A arm64 -C none -d $(uboot-raw) -a $$v_entry_addr -e $$v_entry_addr -O linux -T firmware -n U-boot $(UBOOT_INSTALL_DIR)/uboot.common.img; \
	$(signtool) -i $(UBOOT_INSTALL_DIR)/uboot.common.img -o $@ -s $(spl-uboot-sign-key) -t uboot -a $$v_entry_addr

$(vendor-sec-img): force
	@echo -e '\033[32;1m ==============generate vendor sec img: $(notdir $@) ============== \033[0m'
	$(signtool) -i $(UBOOT_INSTALL_DIR)/vendor.common.img -o $@ -s $(spl-vendor-sign-key) -t uboot

$(kernel-sec-img): force
	@echo -e '\033[32;1m ==============generate kernel sec img: $(notdir $@) ============== \033[0m'
	$(signtool) -i $(KERNEL_INSTALL_DIR)/kernel.common.img -o $@ -s $(uboot-kernel-sign-key)

$(bl31-sec-img): force
	@echo -e '\033[32;1m ==============generate bl31 sec img: $(notdir $@) ============== \033[0m'
	v_entry_addr=$(shell $(CROSS_COMPILE)readelf -h $(ARM_TRUSTED_FIRMWARE_DIR)/build/ar_9311/debug/bl31/bl31.elf | grep "Entry" | cut -d : -f2 | sed 's/[[:space:]]//g' ) ; \
	$(UTILITY_DIR)/bin/mkimage -A arm64 -T firmware -C none -O arm-trusted-firmware -a $$v_entry_addr -e $$v_entry_addr  -n "arm trusted firmware" -d $(ARM_TRUSTED_FIRMWARE_DIR)/build/ar_9311/debug/bl31.bin $(ARM_TRUSTED_FIRMWARE_DIR)/bl31.common.img; \
	$(signtool) -i $(ARM_TRUSTED_FIRMWARE_DIR)/bl31.common.img -o $(ARM_TRUSTED_FIRMWARE_DIR)/bl31.sec.img -t uboot -a $$v_entry_addr

$(bl31-nonsec-img): force
	@echo -e '\033[32;1m ==============generate bl31 nonsec img: $(notdir $@) ============== \033[0m'
	v_entry_addr=$(shell $(CROSS_COMPILE)readelf -h $(ARM_TRUSTED_FIRMWARE_DIR)/build/ar_9311/debug/bl31/bl31.elf | grep "Entry" | cut -d : -f2 | sed 's/[[:space:]]//g' ) ; \
	$(UTILITY_DIR)/bin/mkimage -A arm64 -T firmware -C none -O arm-trusted-firmware -a $$v_entry_addr -e $$v_entry_addr  -n "arm trusted firmware" -d $(ARM_TRUSTED_FIRMWARE_DIR)/build/ar_9311/debug/bl31.bin $(ARM_TRUSTED_FIRMWARE_DIR)/bl31.common.img; \
	$(signtool) -i $(ARM_TRUSTED_FIRMWARE_DIR)/bl31.common.img -o $(ARM_TRUSTED_FIRMWARE_DIR)/bl31.nonsec.img -t uboot -a $$v_entry_addr

force:
# vim: syntax=make
