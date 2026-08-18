#ifndef __CFG_SVP_H__
#define __CFG_SVP_H__

#define IPC_MAX_CNN_NUM   8 //temp defined.
#define IPC_MAX_STR_LEN   128

typedef struct
{
    AR_U32   u32Enable;
	AR_CHAR  achNetName[IPC_MAX_STR_LEN];
    AR_CHAR  achNpuBinName[IPC_MAX_STR_LEN];
    AR_U32   u32VpssGrp;
    AR_U32   u32VpssChn;
    AR_U32   u32OsdEnable;
    AR_U32   u32OsdGrp;
    AR_U32   u32OsdChn;
} IPC_CFG_SVP_S;

AR_S32   IPC_CFG_SVP_Save();
AR_S32   IPC_CFG_SVP_Load(CFG_RUN_MODE_E enMode);
AR_S32   IPC_CFG_SVP_LoadDefault();
IPC_CFG_SVP_S * IPC_CFG_SVP_GetParam();

#define CFG_SVP_FILE "cfg_svp.json"

#endif
