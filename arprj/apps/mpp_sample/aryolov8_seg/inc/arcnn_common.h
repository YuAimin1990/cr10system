#ifndef __ARCNN_COMMON_H__
#define __ARCNN_COMMON_H__

#define RGB_RAW_DATA_FILE     1
#define BGR_RAW_DATA_FILE     2
#define JPG_BMP_PNG_IMG_FILE  3
#define RGBD_RAW_DATA_FILE    4
#define FLOAT_RAW_DATA_FILE    5
#define FIX_RAW_DATA_FILE    6
#define GRAY_RAW_DATA_FILE    7
#define NV12_RAW_DATA_FILE    8
#define NV21_RAW_DATA_FILE    9

#define INVALID_IMG_FILE      (-1)


AR_S32 GetImageFileType(AR_CHAR * pchFileName);
AR_S32 CmpImageAndTensorName(AR_CHAR * pchFileName, AR_CHAR * pchTensorName);

#endif //ifndef
