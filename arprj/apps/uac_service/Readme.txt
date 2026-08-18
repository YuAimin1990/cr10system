# alsa lib compile sample
# CC=/home/kwang/Workspace/UVC_UAC/siriussw/arsdk/toolchain/bin/arm-linux-gnueabihf-gcc LD=/home/kwang/Workspace/UVC_UAC/siriussw/arsdk/toolchain/bin/arm-linux-gnueabihf-ld CCX=/home/kwang/Workspace/UVC_UAC/siriussw/arsdk/toolchain/bin/arm-linux-gnueabihf-g++ ./configure --host=arm-linux-gnueabihf --enable-static=yes --enable-shared=no --prefix=/home/kwang/Workspace/UVC_UAC/siriussw/arsdk/app/artosyn/usr_app/uac_service/alsa-lib  --with-configdir=/home/kwang/Workspace/UVC_UAC/siriussw/arsdk/app/artosyn/usr_app/uac_service/alsa-lib
# make && make install

# run uac_service demo
# alsa.conf in fold alsa-lib, you must copy it to /etc
# /etc/usb_gadget_configfs.sh uac1 0 dwc2_0 0x1d6b 0x0110
# or /etc/usb_gadget_configfs.sh uac1_in 0 dwc2_0 0x1d6b 0x0110
# or /etc/usb_gadget_configfs.sh uac1_out 0 dwc2_0 0x1d6b 0x0110
# export ALSA_CONFIG_PATH=/etc/alsa.conf
# uac_service &
