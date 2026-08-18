#ifndef __GUIDE_MODULE_MANAGER__
#define __GUIDE_MODULE_MANAGER__

#ifdef __cplusplus
extern "C" {
#endif

#include "guide_hal_api.h"

typedef int (*YuvImageCallBack)(unsigned char *yuvData,int yuvLen,int w,int h);
typedef int (*Y16DataCallBack)(short *data,int len);

int guide_init();
void guide_release();
int guide_push_raw_data(unsigned char * pData, int dataLen);
void guide_set_yuv_callback(YuvImageCallBack func);
void guide_set_y16_callback(Y16DataCallBack func);
void save_y16_data(int count);

#ifdef __cplusplus
};
#endif

#endif