#ifndef __UAC_CONFIGFS_H__
#define __UAC_CONFIGFS_H__

#define AR_UAC_CAPTURE_MASK_PATH    "/sys/kernel/config/usb_gadget/g1/functions/uac1.usb%d/c_chmask"        //epout
#define AR_UAC_PLAYBACK_MASK_PATH   "/sys/kernel/config/usb_gadget/g1/functions/uac1.usb%d/p_chmask"        //epin


int getPlayMask(int index);
int getCaptureMask(int index);

#endif
