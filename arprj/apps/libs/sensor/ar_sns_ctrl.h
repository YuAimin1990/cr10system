#ifndef __AR_SNS_CTRL_H__
#define __AR_SNS_CTRL_H__

#include "hal_type.h"
#include "ar_comm_3a.h"
#include "ar_comm_vi.h"
#include "hal_vin_log.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


typedef struct arISP_SNS_STATE_S
{
    AR_BOOL     bInit;                  /* AR_TRUE: Sensor init */
    AR_BOOL     bSyncInit;              /* AR_TRUE: Sync Reg init */
    AR_U8       u8ImgMode;
    AR_U8       u8Hdr;               /* AR_TRUE: HDR enbale */
    WDR_MODE_E  enWDRMode;

    ISP_SNS_REGS_INFO_S astRegsInfo[2]; /* [0]: Sensor reg info of cur-frame; [1]: Sensor reg info of pre-frame ; */

    AR_U32      au32FL[2];              /* [0]: FullLines of cur-frame; [1]: Pre FullLines of pre-frame */
    AR_U32      u32FLStd;               /* FullLines std */
    AR_U32      u32FLMid;
    AR_U32      u32FLShort;
    AR_U32      au32WDRIntTime[4];	
	AR_S32   focus;
} ISP_SNS_STATE_S;

typedef enum arISP_SNS_MIRRORFLIP_TYPE_E
{
    ISP_SNS_NORMAL      = 0,
    ISP_SNS_MIRROR      = 1,
    ISP_SNS_FLIP        = 2,
    ISP_SNS_MIRROR_FLIP = 3,
    ISP_SNS_BUTT
}ISP_SNS_MIRRORFLIP_TYPE_E;

/**< 定义sensor的默认属性，用户可以直接从驱动获取这些属性*/
typedef struct
{
   STRU_COMBO_DEV_ATTR_T stComboAttr;
   VI_DEV_ATTR_S stDevAttr;
   VI_PIPE_ATTR_S stPipeAttr;
   ISP_PUB_ATTR_S stPubAttr;
   VI_CHN_ATTR_S  stChnAttr;
}SRTU_SENSOR_DEFAULT_ATTR_T;

typedef struct arISP_SNS_OBJ_S
{
    AR_S32  (*pfnRegisterCallback)(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ALG_LIB_S *pstAwbLib);
    AR_S32  (*pfnUnRegisterCallback)(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ALG_LIB_S *pstAwbLib);
    AR_S32  (*pfnSetBusInfo)(VI_PIPE ViPipe, ISP_SNS_COMMBUS_U unSNSBusInfo);
    AR_VOID (*pfnStandby)(VI_PIPE ViPipe);
    AR_VOID (*pfnRestart)(VI_PIPE ViPipe);
    AR_VOID (*pfnMirrorFlip)(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip);
    AR_S32  (*pfnWriteReg)(VI_PIPE ViPipe, AR_S32 s32Addr, AR_S32 s32Data);
    AR_S32  (*pfnReadReg)(VI_PIPE ViPipe, AR_S32 s32Addr);
    AR_S32  (*pfnSetInit)(VI_PIPE ViPipe, ISP_INIT_ATTR_S *pstInitAttr);	
    AR_S32  (*pfnSetFocus)(VI_PIPE ViPipe, AR_S32 focus);  //0 disable foucus, 1:enable foucus
    AR_S32  (*pfnGetDefaultAttr)(AR_U8 u8SnsMode,SRTU_SENSOR_DEFAULT_ATTR_T *pstDefaultAttr); /**<通过sensor 模式获取到默认的属性参数，
    u8SnsMode 的值和ISP_PUB_ATTR_S  中的u8SnsMode一一对应，驱动实现的时候注意配合，获取到的属性用户针对需求进行修改，然后设置*/
} ISP_SNS_OBJ_S;

#define CMOS_CHECK_POINTER(ptr)\
    do {\
        if (AR_NULL == ptr)\
        {\
            ISP_TRACE(AR_DBG_ERR, "Null Pointer!\n");\
            return AR_ERR_ISP_NULL_PTR;\
        }\
    }while(0)

#define CMOS_CHECK_POINTER_VOID(ptr)\
    do {\
        if (AR_NULL == ptr)\
        {\
            ISP_TRACE(AR_DBG_ERR, "Null Pointer!\n");\
            return;\
        }\
    }while(0)

#define SENSOR_FREE(ptr)\
    do{\
        if (AR_NULL != ptr)\
        {\
            free(ptr);\
            ptr = AR_NULL;\
        }\
    } while (0)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __AR_SNS_CTRL_H__ */
