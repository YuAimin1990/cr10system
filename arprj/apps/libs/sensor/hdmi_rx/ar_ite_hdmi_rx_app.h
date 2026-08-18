#ifndef _AR_ITE_HDMI_RX_APP_H_
#define _AR_ITE_HDMI_RX_APP_H_

#include "hal_vin_type_def.h"

typedef int (*ptf_notify_call_back)(int camera_id,notify_t notify_code,void * pra,int size,void *usr_data);


int register_hdmi_notify_dvp_in(void *data, ptf_notify_call_back cb);
void ar_ite_hdmi_rx_init(void* arg);

#endif
