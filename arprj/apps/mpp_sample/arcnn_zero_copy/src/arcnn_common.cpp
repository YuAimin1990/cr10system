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
#include "arcnn_common.h"

AR_S32 CmpImageAndTensorName(AR_CHAR * pchFileName, AR_CHAR * pchTensorName)
{
	AR_CHAR * pchExt = NULL;
	AR_CHAR * pchPre = NULL;
	//AR_CHAR * pchTmp = NULL;
	AR_S32 s32Ret = 0;
	AR_CHAR fileExt[32] = {0};

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

	pchPre = strrchr(pchFileName, '/');
	if(!pchPre)
	{
	 	s32Ret = INVALID_IMG_FILE;
	 	return s32Ret;
	}

	strncpy(fileExt, pchPre + 1, strlen(pchPre) - strlen(pchExt) -1);

	if(strcmp(fileExt, pchTensorName))
	{
	    	s32Ret = INVALID_IMG_FILE;
	 	return s32Ret;
	}

	return s32Ret;
}


AR_S32 GetImageFileType(AR_CHAR * pchFileName)
{
    AR_CHAR * pchExt = NULL;
    AR_CHAR * pchTmp = NULL;
    AR_S32 s32Ret = 0;
    AR_CHAR fileExt[8] = {0};

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
    strcpy(fileExt, pchExt);
    pchTmp = fileExt;
    for(; *pchTmp; ++pchTmp) *pchTmp = tolower(*pchTmp);

    if(strcmp(fileExt, ".rgb") == 0)
    {
        s32Ret = RGB_RAW_DATA_FILE;
    }
    else if(strcmp(fileExt, ".bgr") == 0)
    {
        s32Ret = BGR_RAW_DATA_FILE;
    }
    else if(strcmp(fileExt, ".rgbd") == 0)
    {
        s32Ret = RGBD_RAW_DATA_FILE;
    }
    else if(strcmp(fileExt, ".jpg") == 0 || strcmp(fileExt, ".bmp") == 0
        || strcmp(fileExt, ".png") == 0 || strcmp(fileExt, ".jpeg") == 0)
    {
        s32Ret = JPG_BMP_PNG_IMG_FILE;
    }
    else if(strcmp(fileExt, ".fmat") == 0)
    {
        s32Ret = FLOAT_RAW_DATA_FILE;
    }
    else if(strcmp(fileExt, ".smat") == 0)
    {
        s32Ret = FIX_RAW_DATA_FILE;
    }
	else if(strcmp(fileExt, ".y") == 0)
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



