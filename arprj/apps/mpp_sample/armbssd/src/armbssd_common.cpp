//Common APIs
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <ctype.h>
#include "mpi_npu_api.h"
#include "hal_sys.h"

#include "armbssd_common.h"

AR_S32 GetImageFileType(AR_CHAR * pchFileName)
{
    AR_CHAR * pchExt = NULL;
    AR_CHAR * pchTmp = NULL;
    AR_S32 s32Ret = 0;

    if(!pchFileName)
    {
        s32Ret = INVALID_IMG_FILE;
        return s32Ret;
    }

    //ignore "." ".." and any non-image files
    if(strcmp(pchFileName, ".") == 0 || strcmp(pchFileName, "..") == 0)
    {
        s32Ret = INVALID_IMG_FILE;
        return s32Ret;
    }

    pchExt = strrchr(pchFileName, '.');
    if(!pchExt || pchExt == pchFileName)
    {
        s32Ret = INVALID_IMG_FILE;
        return s32Ret;
    }

    //convert to lowercase
    pchTmp = pchExt;
    for(; *pchTmp; ++pchTmp) *pchTmp = tolower(*pchTmp);

    if(strcmp(pchExt, ".rgb") == 0)
    {
        s32Ret = RGB_RAW_DATA_FILE;
    }
    else if(strcmp(pchExt, ".bgr") == 0)
    {
        s32Ret = BGR_RAW_DATA_FILE;
    }
    else if(strcmp(pchExt, ".rgbd") == 0)
    {
        s32Ret = RGBD_RAW_DATA_FILE;
    }
    else if(strcmp(pchExt, ".jpg") == 0 || strcmp(pchExt, ".bmp") == 0 || strcmp(pchExt, ".png") == 0)
    {
        s32Ret = JPG_BMP_PNG_IMG_FILE;
    }
    else if(strcmp(pchExt, ".fmat") == 0)
    {
	s32Ret = FLOAT_RAW_DATA_FILE;
    }
    else if(strcmp(pchExt, ".smat") == 0)
    {
	s32Ret = FIX_RAW_DATA_FILE;
    }
	else if(strcmp(pchExt, ".y") == 0)
    {
        s32Ret = GRAY_RAW_DATA_FILE;
    }
    else
    {
        s32Ret = INVALID_IMG_FILE;
    }

    //Need to add RGBD or RGB interleave format later..

    return s32Ret;
}

AR_S32 GetMMZMemory(AR_U64* u64PhyAddr, void ** pvVirtAddr, AR_CHAR* pstr_mmb, AR_U32 u32Size)
{
	AR_S32 s32Ret;
	s32Ret = ar_hal_sys_mmz_alloc(u64PhyAddr, pvVirtAddr, pstr_mmb, NULL, u32Size);
    	return s32Ret;
}

void ReleaseMMZMemory(AR_U64 u64PhyAddr, void* pvVirtAddr)
{
    	ar_hal_sys_mmz_free(u64PhyAddr, pvVirtAddr);
	u64PhyAddr = 0;
	pvVirtAddr = NULL;
}

