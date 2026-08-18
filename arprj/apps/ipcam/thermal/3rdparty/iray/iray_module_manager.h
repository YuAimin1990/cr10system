#ifndef __IRAY_MODULE_MANAGER__
#define __IRAY_MODULE_MANAGER__

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*YuvImageCallBack)(unsigned char *yuvData,int yuvLen,int w,int h);
typedef int (*NucDataCallBack)(unsigned short *data,int len);

int iray_init();
void iray_release();
int iray_push_raw_data(unsigned char * pData, int dataLen);
void iray_set_yuv_callback(YuvImageCallBack func);
void iray_set_nuc_callback(NucDataCallBack func);
void save_nuc_data(int count);

#ifdef __cplusplus
};
#endif

#endif