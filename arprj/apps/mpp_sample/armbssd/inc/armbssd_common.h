#ifndef __ARCNN_COMMON_H__
#define __ARCNN_COMMON_H__

#define RGB_RAW_DATA_FILE     1
#define BGR_RAW_DATA_FILE     2
#define JPG_BMP_PNG_IMG_FILE  3
#define RGBD_RAW_DATA_FILE    4
#define FLOAT_RAW_DATA_FILE    5
#define FIX_RAW_DATA_FILE    6
#define GRAY_RAW_DATA_FILE    7

#define INVALID_IMG_FILE      (-1)

AR_S32 GetMMZMemory(AR_U64* u64PhyAddr, void ** pvVirtAddr, AR_CHAR* pstr_mmb, AR_U32 u32Size);
void ReleaseMMZMemory(AR_U64 u64PhyAddr, void* pvVirtAddr);
AR_S32 GetImageFileType(AR_CHAR * pchFileName);

#endif //ifndef
