################for ar9341
########run single uvc
/etc/usb_gadget_configfs.sh uvc_bulk_15_hdr 0 dwc2_0 0x1d6b 0x0110

uvc_service2 -b --name imx307 --power_gpio C1_0 --reset_gpio C1_1 --i2c 1 --fps 25 &

########run dual uvc
/etc/usb_gadget_configfs.sh uvc_bulk_15_hdr+uvc_bulk_15_hdr 0 dwc2_0 0x1d6b 0x0110

uvc_service2 -b --multi --name imx307 --power_gpio C1_0 --reset_gpio C1_1 --i2c 1 --fps 25 &
uvc_service2 -b --multi --cam_index 1 --dev_index 1 --name imx307 --power_gpio C1_0 --reset_gpio C1_3 --i2c 2 --fps 25 &


################for ar9311
########run single uvc
/etc/usb_gadget_configfs.sh uvc_bulk_15_hdr 0 dwc2_9311 0x1d6b 0x0110

########for evb
uvc_service2 -b --name imx307 --power_gpio G0_3 --reset_gpio G0_15 --i2c 1 --fps 25  --hdr_fre 100000000 --vif_fre 100000000 --isp_fre 100000000 --mipi_fre 100000000 --h26x_c_fre 100000000 --h26x_b_fre 100000000 --mjpeg_fre 100000000 &

