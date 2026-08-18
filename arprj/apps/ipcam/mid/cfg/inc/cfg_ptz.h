#ifndef _CFG_PTZ_H__
#define _CFG_PTZ_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "cfg_common.h"


/***********************************/
/***         ptz                 ***/
/***********************************/
typedef struct {
    AR_U32 channel;
    AR_U32 baudRate;
    AR_U8  dataBit;
    AR_U8  stopBit;
    AR_U8  parity;
    AR_U8  flowcontrol;
    AR_S8  decoderType[32];
    AR_S32 workMode;
    AR_U16 decoderAddress;
    AR_U16 speedH;
    AR_U16 speedV;
    AR_U16 watchPos;
    AR_U32 res;
} NET_DECODERCFG;

#define PTZ_PROTOCOL_NUM        200

typedef struct {
    AR_U32 type;
    AR_S8  describe[32];
} PTZ_PROTOCOL;

typedef struct {
    AR_U8  type;
    AR_U8  value;
    AR_U8  reserve[2];
} PTZ_LINK;

typedef struct {
    AR_U32 baudRate;
    AR_U8  dataBit;
    AR_U8  stopBit;
    AR_U8  parity;
    AR_U8  flowcontrol;
    AR_U32 workMode;
} NET_RS232CFG;

typedef struct {
    NET_DECODERCFG decoderCfg;
    AR_S32          ptzNum;
    AR_S8           describe[32];
    PTZ_PROTOCOL   protocol[PTZ_PROTOCOL_NUM];
} NET_PTZ_PROTOCOL_CFG;

typedef struct {
    AR_S32     id;
    AR_S32     serialPortID;
    AR_S32     videoInputID;
    AR_S32     duplexMode;   // 0-half, 1-full
    AR_S32     controlType;  // 0- controlType, 1- external
    AR_S32     protocol; // 0 pelco-d, 1 pelco-p
    AR_S32     address;
} NET_PTZ_CFG;

#define PTZ_MAX_PRESET             255
#define PTZ_MAX_CRUISE_POINT_NUM   32
#define PTZ_MAX_CRUISE_GROUP_NUM   5

typedef struct tagNET_PRESET_INFO
{
    AR_U16   nChannel;
    AR_U16   nPresetNum;
    AR_U32   no[PTZ_MAX_PRESET];
    AR_S8    csName[PTZ_MAX_PRESET][64];
}NET_PRESET_INFO;

typedef struct tagNET_CRUISE_POINT
{
	AR_S8 	byPointIndex;
	AR_S8 	byPresetNo;
	AR_S8 	byRemainTime;
	AR_S8 	bySpeed;
}NET_CRUISE_POINT;

typedef struct tagNET_CRUISE_GROUP
{
	AR_S8 byPointNum;
	AR_S8 byCruiseIndex;
	AR_S8 byRes[2];
	NET_CRUISE_POINT struCruisePoint[PTZ_MAX_CRUISE_POINT_NUM];
}NET_CRUISE_GROUP;

typedef struct tagNET_CRUISE_CFG
{
	AR_S32  nChannel;
	AR_S8   byIsCruising;
	AR_S8   byCruisingIndex;
	AR_S8   byPointIndex;
	AR_S8   byEnableCruise;;
	NET_CRUISE_GROUP struCruise[PTZ_MAX_CRUISE_GROUP_NUM];
}NET_CRUISE_CFG;

int IPC_CFG_PTZ_Save();
int IPC_CFG_PTZ_Load(CFG_RUN_MODE_E enMode);
int IPC_CFG_LoadDefault();
void IPC_CFG_PTZ_Print();
int IPC_CFG_LoadDefault();


#define PTZ_CFG_FILE    "cfg_ptz.json"
#define PTZ_CRUISE_CFG  "/opt/custom/cfg/ptz_cruise.cfg"

#ifdef __cplusplus
}
#endif
#endif

