#ifndef _UVC_CONFIGFS_H__
#define _UVC_CONFIGFS_H__

#include <stdio.h>
#include <stdlib.h>

#include "uvc_service2.h"

#define PATH_OF_CONTROL_INTERFACE   "/sys/kernel/config/usb_gadget/g1/functions/uvc.usb%d/control/bInterfaceNumber"
#define PATH_OF_STREAM_INTERFACE    "/sys/kernel/config/usb_gadget/g1/functions/uvc.usb%d/streaming/bInterfaceNumber"
#define PATH_OF_STREAM_MAXPACKET    "/sys/kernel/config/usb_gadget/g1/functions/uvc.usb%d/streaming_maxpacket"
#define PATH_OF_STREAM_MAXBURST     "/sys/kernel/config/usb_gadget/g1/functions/uvc.usb%d/streaming_maxburst"
#define PATH_OF_STREAM_INTERVAL     "/sys/kernel/config/usb_gadget/g1/functions/uvc.usb%d/streaming_interval"
#define PATH_OF_STREAM_MAXPAYLOAD   "/sys/kernel/config/usb_gadget/g1/functions/uvc.usb%d/streaming_maxpayload"
#define PATH_OF_USBGADGET_UDC       "/sys/kernel/config/usb_gadget/g1/UDC"
#define PATH_OF_USBGADGET_BCDUSB    "/sys/kernel/config/usb_gadget/g1/bcdUSB"
#define PATH_OF_STREAM_FORMAT_H265  "/sys/kernel/config/usb_gadget/g1/functions/uvc.usb%d/streaming/header/h/h265"
#define PATH_OF_STREAM_FORMAT_H264  "/sys/kernel/config/usb_gadget/g1/functions/uvc.usb%d/streaming/header/h/h264"
#define PATH_OF_STREAM_FORMAT_MJPEG "/sys/kernel/config/usb_gadget/g1/functions/uvc.usb%d/streaming/header/h/m"
#define PATH_OF_STREAM_FORMAT_NV12  "/sys/kernel/config/usb_gadget/g1/functions/uvc.usb%d/streaming/header/h/nv12"
#define PATH_OF_STREAM_FORMAT_YUY2  "/sys/kernel/config/usb_gadget/g1/functions/uvc.usb%d/streaming/header/h/yuy2"
#define PATH_OF_STREAM_FORMAT_Z16   "/sys/kernel/config/usb_gadget/g1/functions/uvc.usb%d/streaming/header/h/z16"

int getConfigfsUDCName(char * buf, int buf_size);
int getCtrlInterfaceNum(int index);
int getStreamInterfaceNum(int index);
int getMaxPayloadTransferSize(int index);
int getMaxPayloadTransferSizeBulk(int index);
int getTransferMaxburst(int index);
int getConfigfsFormat(struct uvc_format_info ** uvc_formats, int index);
void freeConfigfsFormat(struct uvc_format_info ** uvc_formats, int format_num);
int getConfigfsbcdUSB(void);

#endif
