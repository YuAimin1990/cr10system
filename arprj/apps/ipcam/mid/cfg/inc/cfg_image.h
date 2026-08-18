#ifndef _CFG_IMAGE_H__
#define _CFG_IMAGE_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "cfg_common.h"

/***********************************/
/***          image              ***/
/***********************************/
typedef struct {
    AR_U8     sceneMode;
    AR_U8     imageStyle;

    AR_U8     infraEnable;
    AR_U8     irCutControlMode; // 0 software  ,  1 hardware
    AR_U8     irCutMode;        // ircut 0 auto, 1 day, 2 night
    AR_U8     strengthDenoise2d;
    AR_U8     strengthDenoise3d;

    AR_U8     lowlightMode; // 0 close, 1 only night, 2 day-night, 3 auto
    AR_U8     exposureMode; /* 0 - auto 1 - bright, 2 - dark */
    AR_U8     dcIrisEnable; /* 1 - enable 0 - disable */
    AR_U8     antiFlickerFreq; /* 50: 60  50HZ 60HZ */
    AR_U8     backLightEnable;
    AR_S32    backLightLevel;

    AR_S32    brightness; /* 0 ~ 100 */
    AR_S32    saturation; /* 0 ~ 100  */
    AR_S32    contrast;   /* 0 ~ 100 */
    AR_S32    sharpness;  /* 0 ~ 100 */
    AR_S32    hue;        /* 0 ~ 100 */
} CFG_IMAGE_S;

typedef struct cfg_image
{
    /* data */
    CFG_IMAGE_S stImage[IPC_MAX_PIPELINE_NUM];
} IPC_CFG_IMAGE_S;


extern AR_S32 IPC_CFG_IMAGE_Save();
extern AR_S32 IPC_CFG_IMAGE_Load(CFG_RUN_MODE_E enMode);
extern AR_S32 IPC_CFG_IMAGE_LoadDefault();
extern void IPC_CFG_IMAGE_Print();
extern IPC_CFG_IMAGE_S * IPC_CFG_IMAGE_GetParam();

#define IMAGE_CFG_FILE "cfg_image.json"

#ifdef __cplusplus
}
#endif
#endif

