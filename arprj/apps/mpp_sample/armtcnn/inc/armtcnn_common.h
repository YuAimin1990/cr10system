#ifndef __ARMTCNN_COMMON_H__
#define __ARMTCNN_COMMON_H__

#include <list>

#define RGB_RAW_DATA_FILE     1
#define BGR_RAW_DATA_FILE     2
#define JPG_BMP_PNG_IMG_FILE  3
#define RGBD_RAW_DATA_FILE    4

#define INVALID_IMG_FILE      (-1)

#define ARMTCNN_NMS_MIN    1
#define ARMTCNN_NMS_UNION    2

//has defined in opencv/cvdef.h
//#define MAX(a,b) ((a) > (b) ? (a) : (b))
//#define MIN(a,b) ((a) > (b) ? (b) : (a))

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
//we use a double-list to save the bonding box, from highest score to lowest
typedef struct
{
	AR_U32 u32Valid;
	AR_DOUBLE fX0;
	AR_DOUBLE fY0;
	AR_DOUBLE fX1;
	AR_DOUBLE fY1;
    AR_DOUBLE fDx0;
	AR_DOUBLE fDy0;
	AR_DOUBLE fDx1;
	AR_DOUBLE fDy1;
	AR_DOUBLE dScore;
    AR_DOUBLE fLandmark[10]; //5 landmarks
    AR_U32 u32IndexH;
    AR_U32 u32IndexW;
} BONDING_BOX_S;

typedef std::list<BONDING_BOX_S> BondingBoxList;

AR_S32 GetImageFileType(AR_CHAR * pchFileName);
AR_S32 CalcNMS(BondingBoxList & listBBox, AR_DOUBLE fThresh, AR_U32 u32Mode);
AR_S32 ReRectangle(BondingBoxList::iterator    stBBox);
AR_S32 BBoxRegression(BondingBoxList::iterator stBBox);
void SaveRGBImg(AR_CHAR * pR, AR_CHAR * pG, AR_CHAR * pB, AR_U32 u32W, AR_U32 u32H, AR_CHAR * pName);
AR_DOUBLE getBboxTop(AR_U32 u32Idx, AR_DOUBLE dScale);
AR_DOUBLE getBboxBottom(AR_U32 u32Idx, AR_DOUBLE dScale);

AR_S32 GetMMZMemory(AR_U64* u64PhyAddr, void ** pvVirtAddr, AR_CHAR* pstr_mmb, AR_U32 u32Size);
void ReleaseMMZMemory(AR_U64 u64PhyAddr, void* pvVirtAddr);
AR_S32 GetImageFileType(AR_CHAR * pchFileName);

#endif //ifndef

