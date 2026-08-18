#include <assert.h>
#include <time.h>
#include <stdio.h>
#include "hal_npu_types.h"
#include "ar_ivs.h"
#include <opencv2/core/core_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui_c.h>

static MD_ATTR_S gMdAttr[MD_CHN_MAX];
static MD_ATTR_EX_S gMdAttrEx[MD_CHN_MAX];
static vibeModel_Sequential_t *gModel[MD_CHN_MAX];
static AR_U32 u32MdInit[MD_CHN_MAX] ={0};

AR_S32 AR_IVS_MD_Init()
{
    for(AR_U32 i=0;i<MD_CHN_MAX;i++)
    {
       memset(&gMdAttr[i],0,sizeof(MD_ATTR_S));
	   memset(&gMdAttrEx[i],0,sizeof(MD_ATTR_EX_S));
	   gModel[i] =NULL;
	   u32MdInit[i] =0;
	}
	return 0;
}

AR_S32 AR_IVS_MD_Exit()
{
    for(AR_U32 i=0;i<MD_CHN_MAX;i++)
    {
       memset(&gMdAttr[i],0,sizeof(MD_ATTR_S));
	   memset(&gMdAttrEx[i],0,sizeof(MD_ATTR_EX_S));
	   gModel[i] =NULL;
	   u32MdInit[i] =0;
	}
	return 0;
}

AR_S32 AR_IVS_MD_CreateChn(AR_U32 u32MdChn, MD_ATTR_S *pstMdAttr)
{
	if(!pstMdAttr)
	{
	    printf("pstMdAttr is null \r\n");
	    return -1;
	}

	if(!CHECK_MDCHN_IS_VALID(u32MdChn))
	{
	    printf("[%d] MD CHN error\r\n",u32MdChn);
		return -1;
	}

	if(!CHECK_WIDTH_IS_VALID(pstMdAttr->u16Width))
	{
	    printf("[%d] MD WIDTH error\r\n",u32MdChn);
		return -1;
	}

	if(!CHECK_HEIGHT_IS_VALID(pstMdAttr->u16Height))
	{
	    printf("[%d] MD HEIGHT error\r\n",u32MdChn);
		return -1;
	}

	if(!CHECK_MATCHTHR_IS_VALID(pstMdAttr->u16MatchingThr))
	{
	    printf("[%d] MD MatchingThr error\r\n",u32MdChn);
		return -1;
	}

	if(!CHECK_CCLMODE_IS_VALID(pstMdAttr->stCclCtrl.enMode))
	{
	    printf("[%d] MD CCLMODE error\r\n",u32MdChn);
		return -1;
	}

	if(gModel[u32MdChn])
	{
        printf("[%d] MD CHN has Created \r\n",u32MdChn);
		return -1;
	}

    // set necessary attr value
	gMdAttr[u32MdChn].u16Width                 = pstMdAttr->u16Width;
	gMdAttr[u32MdChn].u16Height                = pstMdAttr->u16Height;
	gMdAttr[u32MdChn].u16MatchingThr           = pstMdAttr->u16MatchingThr;
	gMdAttr[u32MdChn].stCclCtrl.enMode         = pstMdAttr->stCclCtrl.enMode;

    // set attr ex : default value
	gMdAttrEx[u32MdChn].u32NumberOfSamples = NUM_SAMPLES;
	gMdAttrEx[u32MdChn].u32MatchingNumber  = MATCHING_NUMBER;
	gMdAttrEx[u32MdChn].u32UpdateFactor    = UPDATE_FACTOR;

	gModel[u32MdChn] = (vibeModel_Sequential_t*)calloc(1, sizeof(*gModel[u32MdChn]));
	if(gModel[u32MdChn] == NULL)
	{
        printf("[%d] MD calloc gModel error\r\n",u32MdChn);
		return -1;
	}

    return 0;

}

AR_S32 AR_IVS_MD_DestroyChn(AR_U32 u32MdChn)
{
	if(!CHECK_MDCHN_IS_VALID(u32MdChn))
	{
	    printf("[%d] MD CHN error\r\n",u32MdChn);
		return -1;
	}

	AR_S32 ret =0;
	ret = libvibeModel_Sequential_Free(gModel[u32MdChn]);

	memset(&gMdAttr[u32MdChn],0,sizeof(MD_ATTR_S));
	memset(&gMdAttrEx[u32MdChn],0,sizeof(MD_ATTR_EX_S));
	gModel[u32MdChn]=NULL;
	u32MdInit[u32MdChn] =0;

	return ret;
}

AR_S32 AR_IVS_MD_SetChnAttr(AR_U32 u32MdChn, MD_ATTR_S *pstMdAttr)
{
	if(!pstMdAttr)
	{
		printf("pstMdAttr is null \r\n");
		return -1;
	}

	if(!CHECK_MDCHN_IS_VALID(u32MdChn))
	{
		printf("[%d] MD CHN error\r\n",u32MdChn);
		return -1;
	}

	if(!CHECK_WIDTH_IS_VALID(pstMdAttr->u16Width))
	{
		printf("[%d] MD WIDTH error\r\n",u32MdChn);
		return -1;
	}

	if(!CHECK_HEIGHT_IS_VALID(pstMdAttr->u16Height))
	{
	    printf("[%d] MD HEIGHT error\r\n",u32MdChn);
		return -1;
	}

	if(!CHECK_MATCHTHR_IS_VALID(pstMdAttr->u16MatchingThr))
	{
		printf("[%d] MD MatchingThr error\r\n",u32MdChn);
		return -1;
	}

	if(!CHECK_CCLMODE_IS_VALID(pstMdAttr->stCclCtrl.enMode))
	{
		printf("[%d] MD CCLMODE error\r\n",u32MdChn);
		return -1;
	}

	// set necessary attr value
	gMdAttr[u32MdChn].u16Width				   = pstMdAttr->u16Width;
	gMdAttr[u32MdChn].u16Height 			   = pstMdAttr->u16Height;
	gMdAttr[u32MdChn].u16MatchingThr		   = pstMdAttr->u16MatchingThr;
	gMdAttr[u32MdChn].stCclCtrl.enMode		   = pstMdAttr->stCclCtrl.enMode;

	return 0;

}
AR_S32 AR_IVS_MD_GetChnAttr(AR_U32 u32MdChn, MD_ATTR_S *pstMdAttr)
{
	if(!pstMdAttr)
	{
		printf("pstMdAttr is null \r\n");
		return -1;
	}

	if(!CHECK_MDCHN_IS_VALID(u32MdChn))
	{
		printf("[%d] MD CHN error\r\n",u32MdChn);
		return -1;
	}

	memcpy(pstMdAttr,&gMdAttr[u32MdChn],sizeof(MD_ATTR_S));

	return 0;

}
AR_S32 AR_IVS_MD_SetChnAttrEx(AR_U32 u32MdChn, MD_ATTR_EX_S *pstMdAttrEx)
{
	if(!pstMdAttrEx)
	{
		printf("pstMdAttrEx is null \r\n");
		return -1;
	}

	if(!CHECK_MDCHN_IS_VALID(u32MdChn))
	{
		printf("[%d] MD CHN error\r\n",u32MdChn);
		return -1;
	}

	if(!CHECK_SAMPLENUM_IS_VALID(pstMdAttrEx->u32NumberOfSamples))
	{
		printf("[%d] MD NumberOfSamples error\r\n",u32MdChn);
		return -1;
	}

	if(!CHECK_MATCHNUM_IS_VALID(pstMdAttrEx->u32MatchingNumber))
	{
		printf("[%d] MD MatchingNumber error\r\n",u32MdChn);
		return -1;
	}

	if(!CHECK_UPDATEFACTOR_IS_VALID(pstMdAttrEx->u32UpdateFactor))
	{
		printf("[%d] MD UpdateFactor error\r\n",u32MdChn);
		return -1;
	}

	gMdAttrEx[u32MdChn].u32NumberOfSamples		= pstMdAttrEx->u32NumberOfSamples;
	gMdAttrEx[u32MdChn].u32MatchingNumber 		= pstMdAttrEx->u32MatchingNumber;
	gMdAttrEx[u32MdChn].u32UpdateFactor		    = pstMdAttrEx->u32UpdateFactor;

    return 0;
}
AR_S32 AR_IVS_MD_GetChnAttrEx(AR_U32 u32MdChn, MD_ATTR_EX_S *pstMdAttrEx)
{
	if(!pstMdAttrEx)
	{
		printf("pstMdAttrEx is null \r\n");
		return -1;
	}

	if(!CHECK_MDCHN_IS_VALID(u32MdChn))
	{
		printf("[%d] MD CHN error\r\n",u32MdChn);
		return -1;
	}

	memcpy(pstMdAttrEx,&gMdAttrEx[u32MdChn],sizeof(MD_ATTR_EX_S));

	return 0;

}

static AR_S32 AR_IVS_MD_InitModel(AR_U32 u32MdChn, AR_IMG_S *pSrc)
{
	if(!CHECK_MDCHN_IS_VALID(u32MdChn))
	{
	    printf("[%d] MD CHN error\r\n",u32MdChn);
		return -1;
	}

	void *pStreamVa = (void*)pSrc->astChannels[0].uptrAddrVirt;
	if(!pStreamVa)
	{
		printf("[%d] pStreamVa is NULL!\n",u32MdChn);
		return -1;
	}

	AR_U32 u32Width = pSrc->u32Width;
	AR_U32 u32Height = pSrc->u32Height;
	gMdAttr[u32MdChn].u16Width = pSrc->u32Width;
	gMdAttr[u32MdChn].u16Height = pSrc->u32Height;

	/* Get a model data structure. */
	//model = (vibeModel_Sequential_t *)libvibeModel_Sequential_New(param);

	/* Default parameters values. */
	gModel[u32MdChn]->numberOfSamples 	   = gMdAttrEx[u32MdChn].u32NumberOfSamples;
	gModel[u32MdChn]->matchingThreshold	   = (uint32_t)gMdAttr[u32MdChn].u16MatchingThr;
	gModel[u32MdChn]->matchingNumber	   = gMdAttrEx[u32MdChn].u32MatchingNumber;
	gModel[u32MdChn]->updateFactor		   = gMdAttrEx[u32MdChn].u32UpdateFactor;

	/* Storage for the history. */
	gModel[u32MdChn]->historyImage		   = NULL;
	gModel[u32MdChn]->historyBuffer		   = NULL;
	gModel[u32MdChn]->lastHistoryImageSwapped = 0;

	/* Buffers with random values. */
	gModel[u32MdChn]->jump				   = NULL;
	gModel[u32MdChn]->neighbor			   = NULL;
	gModel[u32MdChn]->position			   = NULL;

	libvibeModel_Sequential_AllocInit_8u_C1R(gModel[u32MdChn], (uint8_t *)pStreamVa, u32Width, u32Height);

    return 0;

}

static AR_S32 AR_IVS_MD_GetRegions(AR_U32 u32MdChn,AR_IMG_S *pstSegmapOut,MD_BLOB_S*pstBlob)
{
    CvSize size;
    size.width = gMdAttr[u32MdChn].u16Width;
    size.height = gMdAttr[u32MdChn].u16Height;
	IplImage* image = cvCreateImage(size,8,1);
	if(!image)
	{
       printf("cvCreateImage error \r\n");
	   return -1;
	}
	memcpy(image->imageData,(void*)pstSegmapOut->astChannels[0].uptrAddrVirt, size.width * size.height);

    CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* pContour = NULL;
	AR_U32 header_size = sizeof(CvContour);
	AR_U32 mode = CV_RETR_EXTERNAL;
	AR_U32 method = CV_CHAIN_APPROX_SIMPLE;
	CvPoint offset = cvPoint(0,0);

	cvFindContours(image,storage,&pContour,header_size,mode,method,offset);

	AR_U32 num =0;
	AR_DOUBLE dArea = 0.0;
    for (; pContour != NULL; pContour = pContour->h_next)
    {
        dArea = fabs(cvContourArea(pContour, CV_WHOLE_SEQ, 0));
		if( dArea >= pstBlob->u16CurAreaThr*1.0)
		{
             CvRect rect = cvBoundingRect(pContour, 1 );
			 pstBlob->astRegion[pstBlob->u8RegionNum].u16Left = rect.x;
			 pstBlob->astRegion[pstBlob->u8RegionNum].u16Top= rect.y;
			 pstBlob->astRegion[pstBlob->u8RegionNum].u16Right= rect.x + rect.width;
			 pstBlob->astRegion[pstBlob->u8RegionNum].u16Bottom= rect.y + rect.height;
			 pstBlob->astRegion[pstBlob->u8RegionNum].u32Area = rect.width*rect.height;
			 pstBlob->u8RegionNum++;
			 if(pstBlob->u8RegionNum>=MD_MAX_REGION_NUM)
			 {
			 	break;
			 }
		}
    }
	if(pstBlob->u8RegionNum ==0)
	{
		pstBlob->s8LabelStatus =-1;
	}

    cvReleaseMemStorage(&storage);
	cvReleaseImage(&image);

	return 0;
}

AR_S32 AR_IVS_MD_Process(AR_U32 u32MdChn, AR_IMG_S *pSrc,AR_IMG_S *pstSegmapOut,MD_BLOB_S*pstBlob)
{
	AR_S32 s32Ret;

	if(!CHECK_MDCHN_IS_VALID(u32MdChn))
	{
		printf("[%d] MD CHN error\r\n",u32MdChn);
		return -1;
	}

	if((!pSrc)||(!pstSegmapOut)||(!pstBlob))
	{
		printf("[%d] pSrc|pstSegmapOut|pstBlob is NULL!\n",u32MdChn);
		return -1;
	}

	void *pStreamVa = (void*)pSrc->astChannels[0].uptrAddrVirt;
	if(!pStreamVa)
	{
		printf("[%d] pStreamVa is NULL!\n",u32MdChn);
		return -1;
	}

	void *pSegmentMapVa = (void*)pstSegmapOut->astChannels[0].uptrAddrVirt;
	if(!pSegmentMapVa)
	{
		printf("[%d] pSegmentMapVa is NULL!\n",u32MdChn);
		return -1;
	}

	if(!gModel[u32MdChn])
	{
		printf("[%d] MD CHN has not Created \r\n",u32MdChn);
		return -1;
	}

	if(u32MdInit[u32MdChn] == 0)
	{
	    s32Ret = AR_IVS_MD_InitModel(u32MdChn,pSrc);
		if(s32Ret <0)
		{
			printf("[%d] AR_IVS_MD_InitModel err!\n",u32MdChn);
			return -1;
		}
		u32MdInit[u32MdChn]=1;
	}

	/* Segmentation step: produces the output mask. */
	libvibeModel_Sequential_Segmentation_8u_C1R(gModel[u32MdChn], (uint8_t *)pStreamVa, (uint8_t *)pSegmentMapVa);
	/* Next, we update the model. This step is optional. */
	libvibeModel_Sequential_Update_8u_C1R(gModel[u32MdChn], (uint8_t *)pStreamVa, (uint8_t *)pSegmentMapVa);
	/* segmentation_map is the binary output map that you would like to display, save or
	   use in your own application. Put your own code hereafter. */
	AR_S32 ret = AR_IVS_MD_GetRegions(u32MdChn,pstSegmapOut,pstBlob);
	return ret;

}





