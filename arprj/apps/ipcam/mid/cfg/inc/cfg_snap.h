#ifndef _CFG_SNAP_H__
#define _CFG_SNAP_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "cfg_common.h"

/***********************************/
/***         snap                ***/
/***********************************/
typedef struct {
    AR_S32     enable;
    CFG_SCHEDTIME scheduleTime[7][4];
    AR_S32     interval;
    AR_S32     nums;
    AR_S32     pictureQuality;
    AR_S32     imageSize;
    AR_S32     snapShotImageType;
    AR_S32     storagerMode;
    AR_S32     channelID;
} TIMER_SNAP;

typedef struct {
	AR_S32     enable;
	AR_S32     interval;
	AR_S32     nums;
	AR_S32     pictureQuality;
	AR_S32     imageSize;
	AR_S32     snapShotImageType;
	AR_S32     storagerMode;
	AR_S32     channelID;
}EVENT_SNAP;

//If we need to snap multi channels, we should expand the struct to array.
typedef struct {
	TIMER_SNAP timer_snap;
	EVENT_SNAP event_snap;
} IPC_CFG_SNAP_S;

extern AR_S32 IPC_CFG_SNAP_Save();
extern AR_S32 IPC_CFG_SNAP_Load(CFG_RUN_MODE_E enMode);
extern AR_S32 IPC_CFG_SNAP_LoadDefault();
extern void IPC_CFG_SNAP_Print();
extern IPC_CFG_SNAP_S * IPC_CFG_SNAP_GetParam();

#define SNAP_CFG_FILE "cfg_snap.json"

#ifdef __cplusplus
}
#endif
#endif

