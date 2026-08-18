#ifndef __ARCNN_COMMON_H__
#define __ARCNN_COMMON_H__

#define RGB_RAW_DATA_FILE     1
#define BGR_RAW_DATA_FILE     2
#define JPG_BMP_PNG_IMG_FILE  3
#define RGBD_RAW_DATA_FILE    4
#define YUV420P_FILE    8

#define INVALID_IMG_FILE      (-1)

//
#if 0
#define AR_GET_ION_MEMORY(VA, PA, SIZE, cachable) \
do{ \
    VA = (AR_CHAR *)ar_ion_malloc_ex(SIZE,cachable); \
    if(!VA) \
    { \
        printf("%s %d: Malloc memory failed, exit!\r\n", __FUNCTION__, __LINE__); \
    } \
    else \
    {\
        PA = (AR_CHAR *)ar_ion_virt_to_phy((AR_UCHAR *)VA); \
        memset(VA, 0, SIZE); \
    }\
}while(0)

#define AR_RELEASE_ION_MEMORY(VA) ar_ion_free((AR_UCHAR *)VA)
#endif

AR_S32 GetMMZMemory(AR_U64* u64PhyAddr, void ** pvVirtAddr, AR_CHAR* pstr_mmb, AR_U32 u32Size);
void ReleaseMMZMemory(AR_U64 u64PhyAddr, void* pvVirtAddr);
AR_S32 GetImageFileType(AR_CHAR * pchFileName);

#endif //ifndef
