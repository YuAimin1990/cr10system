#ifndef __GUIDE_MODULE_MANAGER__
#define __GUIDE_MODULE_MANAGER__

#ifdef __cplusplus
extern "C" {
#endif

#include "guide_hal_api.h"
#include "ITA.h"
#include "ITADTD.h"

extern HANDLE_T g_GuideModuleHandle;

typedef int (*YuvImageCallBack)(unsigned char *yuvData,int yuvLen,int w,int h);
typedef int (*Y16DataCallBack)(short *data,int len);

int guide_init();
void guide_release();
ITA_RESULT guide_push_raw_data(unsigned char *pData, int dataLen, ITAISPResult *pResult);
void guide_set_yuv_callback(YuvImageCallBack func);
void guide_set_y16_callback(Y16DataCallBack func);
void save_y16_data(int count);
int guide_MeasureTempPoint(short y16, float *surfaceTemp);

#ifdef __cplusplus
};
#endif

#endif