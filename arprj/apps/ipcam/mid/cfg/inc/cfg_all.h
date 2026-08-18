
#ifndef _CFG_ALL_H_
#define _CFG_ALL_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "cfg_gpio.h"
#include "cfg_vio.h"
#include "cfg_venc.h"
#include "cfg_audio.h"
#include "cfg_network.h"

#include "cfg_user.h"
#include "cfg_ptz.h"
#include "cfg_alarm.h"
#include "cfg_osd.h"
#include "cfg_shelter.h"
#include "cfg_image.h"
#include "cfg_md.h"
#include "cfg_record.h"
#include "cfg_snap.h"
#include "cfg_system.h"
#include "cfg_vendor.h"
#include "cfg_svp.h"
#include "cfg_channel.h"
#include "cfg_vpss.h"
#include "cfg_control.h"

extern AR_S32 IPC_CFG_SaveAll();
extern AR_S32 IPC_CFG_LoadAll();
extern AR_S32 IPC_CFG_PrintAll();
extern AR_S32 IPC_CFG_LoadDefaultAll(CFG_RUN_MODE_E enMode);
extern void IPC_CFG_ClearAll();

#ifdef __cplusplus
}
#endif
#endif
