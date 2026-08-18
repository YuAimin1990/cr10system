PARAM_FILE:=../../Makefile.param
include $(PARAM_FILE)

gBUSYBOX_RT_DIR    = $(gBINARIES_DOWNLOAD_URL)/app
gBUSYBOX_VER       = busybox-1.25.0
gBUSYBOX_FILENAME  = $(gBUSYBOX_VER).tar.bz2

IS_KEEP_TAR ?= false

FAKEROOT = fakeroot -i fakeroot.save -s fakeroot.save --
FAKEROOT_RESTOREONLY   = fakeroot -i fakeroot.save --
FAKEROOT_SAVEONLY      = fakeroot -s fakeroot.save --
FAKEROOT_RESET         = fakeroot -- rm -f fakeroot.save ; fakeroot -- touch fakeroot.save

################################################################################################
# combine busybox + rootfs_skel + sysroot into roofs.tar.gz
################################################################################################
all:busybox_prepare busybox_config busybox rootfs_skel sysroot
	rm -rf tmp
	mkdir tmp -p
	$(FAKEROOT_SAVEONLY) tar -x -f $(ROOTFS_SKEL_INSTALL_DIR)/rootfs_skel.tar -C tmp
	cp -rf $(BUSYBOX_INSTALL_DIR)/* tmp
	cp -rf $(SYSROOT_INSTALL_DIR)/* tmp
	cd tmp; fakeroot -i ../fakeroot.save -- tar -cz -f $(ROOTFS_INSTALL_DIR)/rootfs.tar.gz *
	rm -rf tmp

initramfs:
	rm -rf tmp_initramfs
	mkdir -p tmp_initramfs
	$(FAKEROOT_SAVEONLY) tar -xz -f $(ROOTFS_INSTALL_DIR)/rootfs.tar.gz -C tmp_initramfs
	cd tmp_initramfs; \
	ln -s bin/busybox init; find . -print | fakeroot -i ../fakeroot.save -- cpio --create -H newc > $(ROOTFS_INSTALL_DIR)/initramfs.cpio
	rm -rf tmp_initramfs

clean:busybox_clean rootfs_skel_clean sysroot_clean

clobber:busybox_clobber
	@make clean

prepare:
	@mkdir $(BUSYBOX_INSTALL_DIR) -p
	@mkdir $(SYSROOT_INSTALL_DIR) -p
	@mkdir $(ROOTFS_INSTALL_DIR) -p
	@mkdir $(ROOTFS_SKEL_INSTALL_DIR) -p

################################################################################################
# build busybox
################################################################################################
busybox_prepare:prepare
	@echo -e '\033[32;1m $@ >>>>> \033[0m'
	python2 $(MAKERY_DIR)/download.py \
				--dl_type=$(gDOWNLOAD_TYPE) \
				--dl_dir=./busybox \
				--filename=$(gBUSYBOX_FILENAME) \
				--rt_dir=$(gBUSYBOX_RT_DIR) \
				--extract_dir=./busybox/ \
				--version=$(gBUSYBOX_VER) \
				--keep_tar_file=$(IS_KEEP_TAR);

busybox_config:
	@echo -e '\033[32;1m busybox config....>>>>> \033[0m'
	@if [ -f $(gBUSYBOX_CONFIG) ]  && [ -n "$(gBUSYBOX_CONFIG)" ]; then \
		cp -u $(gBUSYBOX_CONFIG) busybox/$(gBUSYBOX_VER)/.config; \
	else \
		echo -e '\033[32;1m" >>>>> "make artosyn_defconfig" >>>>> \033[0m'; \
		make -C busybox/$(gBUSYBOX_VER) artosyn_defconfig; \
	fi
	@echo -e '\033[32;1m prepare $@ end >>>>> \033[0m'

busybox:force
	@echo -e '\033[32;1m $@ start>>>>> \033[0m'
	make -C busybox/$(gBUSYBOX_VER) oldconfig V=1 CROSS_COMPILE=$(gCROSS_COMPILE)
	make -C busybox/$(gBUSYBOX_VER) -j16 V=1 CROSS_COMPILE=$(gCROSS_COMPILE)
	fakeroot make -C busybox/$(gBUSYBOX_VER) install CONFIG_PREFIX=$(BUSYBOX_INSTALL_DIR) V=1 CROSS_COMPILE=$(gCROSS_COMPILE)
	@echo -e '\033[32;1m $@ end>>>>> \033[0m'

busybox_clean:
	@if [ -d busybox/$(gBUSYBOX_VER) ]; then \
		make -C busybox/$(gBUSYBOX_VER) clean V=1 CROSS_COMPILE=$(gTOOLCHAIN_DIR)/$(gCROSS_COMPILER_PREFIX)- -j16 ; \
	fi
	rm -rf $(BUSYBOX_INSTALL_DIR)

busybox_clobber:
ifneq ($(gBUSYBOX_VER),)
	@if [ -d busybox/$(gBUSYBOX_VER) ]; then \
		rm -rf busybox/$(gBUSYBOX_VER); \
	fi
endif
	@make busybox_clean

################################################################################################
# make rootfs_skel
################################################################################################
rootfs-dirs=/bin /sbin /usr /proc /sys /dev /etc /mnt /media /mod /var /tmp /local /root /factory /usrdata /database
rootfs_skel:
	@echo -e '\033[32;1m $@ start>>>>> \033[0m'
	rm -rf tmp_skel
	mkdir tmp_skel
	$(FAKEROOT_RESET)
	for i in $(patsubst /%,%,$(rootfs-dirs)) ; do \
		[ -d skel/nodes/$$i ] && cp -rfd skel/nodes/$$i tmp_skel/ ; \
		! [ -d skel/$$i ] && mkdir -p tmp_skel/$$i ; \
		[ -e $(tmp_skel)/$$i/.gitignore ] && rm -f tmp_skel/$$i/.gitignore || /bin/true ; \
	done
	mv tmp_skel/etc/dropbear/dropbear tmp_skel/bin
	mv tmp_skel/etc/dropbear/scp tmp_skel/bin
	$(FAKEROOT) mknod tmp_skel/dev/console c 5 1
	$(FAKEROOT) mknod tmp_skel/dev/null c 1 3
	$(FAKEROOT) mknod tmp_skel/dev/zero c 1 5
	$(FAKEROOT) chmod a+rwx tmp_skel/tmp
	$(FAKEROOT) chmod o+t tmp_skel/tmp
	$(FAKEROOT) mkdir tmp_skel/dev/pts
	$(FAKEROOT) mkdir tmp_skel/dev/shm
	cd tmp_skel; fakeroot -i ../fakeroot.save -- tar -c -f $(ROOTFS_SKEL_INSTALL_DIR)/rootfs_skel.tar *
	rm -rf tmp_skel
	@echo -e '\033[32;1m $@ end>>>>> \033[0m'

rootfs_skel_clean:
	rm -rf $(ROOTFS_SKEL_INSTALL_DIR)

################################################################################################
# make sysroot
################################################################################################
fTOOLCHAIN_LIBSTDCXX_DIR = $(dir $(shell $(gCROSS_COMPILE_BASE)gcc -print-file-name=libstdc++.so))
fTOOLCHAIN_SYSROOT       = $(shell $(gCROSS_COMPILE_BASE)gcc -print-sysroot)
fTOOLCHAIN_LIBGCC_ALL    = $(wildcard $(dir $(shell $(gCROSS_COMPILE_BASE)gcc -print-file-name=libgcc_s.so))/libgcc_s.so*)
fTOOLCHAIN_LIBGCC_ALL   += $(wildcard $(dir $(shell $(gCROSS_COMPILE_BASE)gcc -print-file-name=libgcc.so))/libgcc.so*)
fTOOLCHAIN_LIBGCC_ALL   += $(wildcard $(dir $(shell $(gCROSS_COMPILE_BASE)gcc -print-file-name=libgcc_eh.so))/libgcc_eh.so*)
gPRODUCT_ROOTFS_SKEL_SIMPLE = y
ifeq ($(gPRODUCT_ROOTFS_SKEL_SIMPLE),y)
sysroot:prepare
	@echo -e '\033[32;1m $@ start>>>>> \033[0m'
	mkdir -p $(SYSROOT_INSTALL_DIR)/lib
	mkdir -p $(SYSROOT_INSTALL_DIR)/usr/lib
	@echo -e '\033[32;1m $@ end>>>>> \033[0m'
	@if [ "$(fTOOLCHAIN_LIBSTDCXX_DIR)" != "./" ]; then cp -r $(fTOOLCHAIN_LIBSTDCXX_DIR)/libstdc++.so* $(SYSROOT_INSTALL_DIR)/lib/; fi ;
	@if [ "$(fTOOLCHAIN_SYSROOT)" != "./" ]; then rsync --exclude={*asan*,*gfortran*,*cidn*,*gomp*,*ubsan*} -l $(fTOOLCHAIN_SYSROOT)/lib/*.so* $(SYSROOT_INSTALL_DIR)/lib/; fi ;
	@if [ "$(fTOOLCHAIN_SYSROOT)" != "./" ]; then rsync --exclude={*asan*,*gfortran*,*cidn*,*gomp*,*ubsan*} -l $(fTOOLCHAIN_SYSROOT)/usr/lib/*.so* $(SYSROOT_INSTALL_DIR)/usr/lib/; fi ;
	@find $(SYSROOT_INSTALL_DIR)/lib -type f -exec file {} \; | grep 'ELF .* shared object' | cut -d : -f 1 | xargs --no-run-if-empty $(gCROSS_COMPILE)strip --strip-debug --strip-unneeded --remove-section=.comment --remove-section=.note ;
	@find $(SYSROOT_INSTALL_DIR)/usr/lib -type f -exec file {} \; | grep 'ELF .* shared object' | cut -d : -f 1 | xargs --no-run-if-empty $(gCROSS_COMPILE)strip --strip-debug --strip-unneeded --remove-section=.comment --remove-section=.note ;
else
sysroot:prepare
	@echo -e '\033[32;1m $@ start>>>>> \033[0m'
	mkdir -p $(SYSROOT_INSTALL_DIR)/lib
	mkdir -p $(SYSROOT_INSTALL_DIR)/usr/lib
	@if [ "$(fTOOLCHAIN_LIBSTDCXX_DIR)" != "./" ]; then cp -r $(fTOOLCHAIN_LIBSTDCXX_DIR)/libstdc++.so* $(SYSROOT_INSTALL_DIR)/lib/; fi ;
	@if [ "$(fTOOLCHAIN_SYSROOT)" != "./" ]; then cp -r $(fTOOLCHAIN_SYSROOT)/lib/*.so* $(SYSROOT_INSTALL_DIR)/lib/; fi ;
	@if [ "$(fTOOLCHAIN_SYSROOT)" != "./" ]; then cp -r $(fTOOLCHAIN_SYSROOT)/usr/lib/*.so* $(SYSROOT_INSTALL_DIR)/usr/lib/; fi ;
	#@if [ "$(fTOOLCHAIN_LIBGCC_ALL)" != "./" ]; then cp -d $(fTOOLCHAIN_LIBGCC_ALL) $(component-out-dir)/lib; fi ;
	@find $(SYSROOT_INSTALL_DIR)/lib -type f -exec file {} \; | grep 'ELF .* shared object' | cut -d : -f 1 | xargs --no-run-if-empty $(gCROSS_COMPILE)strip --strip-debug --strip-unneeded --remove-section=.comment --remove-section=.note ;
	@find $(SYSROOT_INSTALL_DIR)/usr/lib -type f -exec file {} \; | grep 'ELF .* shared object' | cut -d : -f 1 | xargs --no-run-if-empty $(gCROSS_COMPILE)strip --strip-debug --strip-unneeded --remove-section=.comment --remove-section=.note ;
	@echo -e '\033[32;1m $@ end>>>>> \033[0m'
endif

sysroot_clean:
	rm -rf $(SYSROOT_INSTALL_DIR)

force:
# vim: syntax=make
