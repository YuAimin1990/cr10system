#include "ar_ive.h"

typedef struct ArOD_STATUS_S
{
	AR_U32 u32GrayScaleStatus;
	AR_U32 u32ClarityStatus;
	AR_U32 u32NormalClarityValue;
	AR_U32 u32SkinColorStatus;
	AR_U32 u32EdgeStatus;
	AR_U32 u32AlarmStatus;
}OD_STATUS_S;

static OD_ATTR_S gOdAttr[OD_CHN_MAX];
static OD_ATTR_PRIV_S gOdAttrPriv[OD_CHN_MAX];

AR_S32 AR_IVE_OD_Init()
{
	memset(gOdAttr,0,sizeof(OD_ATTR_S)*OD_CHN_MAX);
	memset(gOdAttrPriv,0,sizeof(OD_ATTR_PRIV_S)*OD_CHN_MAX);
	return 0;
}

AR_S32 AR_IVE_OD_Exit()
{
    memset(gOdAttr,0,sizeof(OD_ATTR_S)*OD_CHN_MAX);
	memset(gOdAttrPriv,0,sizeof(OD_ATTR_PRIV_S)*OD_CHN_MAX);
	return 0;
}

AR_S32 AR_IVE_OD_CreateChn(AR_U32 u32OdChn, OD_ATTR_S *pstOdAttr)
{
	if(!pstOdAttr)
	{
	    printf("pstOdAttr is null \r\n");
	    return -1;
	}

	if(!CHECK_ODCHN_IS_VALID(u32OdChn))
	{
	    printf("OD CHN error\r\n");
		return -1;
	}

	gOdAttrPriv[u32OdChn].u32HistGrayThred = HIST_GRAY_THRED;
	gOdAttrPriv[u32OdChn].u32HistPixelPercentThred = HIST_PIXEL_PERCENT_THRED;
	gOdAttrPriv[u32OdChn].u32EdgePointThred = CANNY_EDGE_POINT_THRED;


	if(CANNY_CHECK_ALARMNUM_IS_VALID(pstOdAttr->u32AlarmBlockNumThred))
	{
		gOdAttr[u32OdChn].u32AlarmBlockNumThred = pstOdAttr->u32AlarmBlockNumThred;
	}
	else
	{
		gOdAttr[u32OdChn].u32AlarmBlockNumThred = ALARM_BLOCK_NUM_THRED;
	}

    return 0;

}


AR_S32 AR_IVE_OD_SetChnPrivAttr(AR_U32 u32OdChn, OD_ATTR_PRIV_S *pstOdPrivAttr)
{
	if(!pstOdPrivAttr)
	{
	    printf("pstOdPrivAttr is null \r\n");
	    return -1;
	}

	if(!CHECK_ODCHN_IS_VALID(u32OdChn))
	{
	    printf("OD CHN error\r\n");
		return -1;
	}

	if( CANNY_CHECK_GRAY_IS_VALID(pstOdPrivAttr->u32HistGrayThred))
	{
		gOdAttrPriv[u32OdChn].u32HistGrayThred = pstOdPrivAttr->u32HistGrayThred;
	}
	else
	{
		gOdAttrPriv[u32OdChn].u32HistGrayThred = HIST_GRAY_THRED;
	}

	if( CANNY_CHECK_PIXELPERCENYT_IS_VALID(pstOdPrivAttr->u32HistPixelPercentThred) )
	{
		gOdAttrPriv[u32OdChn].u32HistPixelPercentThred = pstOdPrivAttr->u32HistPixelPercentThred;
	}
	else
	{
		gOdAttrPriv[u32OdChn].u32HistPixelPercentThred = HIST_PIXEL_PERCENT_THRED;
	}

	if(CANNY_CHECK_EDGEPOINT_IS_VALID(pstOdPrivAttr->u32EdgePointThred))
	{
		gOdAttrPriv[u32OdChn].u32EdgePointThred = pstOdPrivAttr->u32EdgePointThred;
	}
	else
	{
		gOdAttrPriv[u32OdChn].u32EdgePointThred = CANNY_EDGE_POINT_THRED;
	}

    return 0;

}


AR_S32 AR_IVE_OD_GetChnPrivAttr(AR_U32 u32OdChn, OD_ATTR_PRIV_S *pstOdPrivAttr)
{
	if(!pstOdPrivAttr)
	{
	    printf("pstOdPrivAttr is null \r\n");
	    return -1;
	}

	if(!CHECK_ODCHN_IS_VALID(u32OdChn))
	{
	    printf("OD CHN error\r\n");
		return -1;
	}

	memcpy(pstOdPrivAttr,&gOdAttrPriv[u32OdChn],sizeof(OD_ATTR_PRIV_S));

    return 0;

}


AR_S32 AR_IVE_OD_DestroyChn(AR_U32 u32OdChn)
{
	if(!CHECK_ODCHN_IS_VALID(u32OdChn))
	{
	    printf("OD CHN error\r\n");
		return -1;
	}

	memset(&gOdAttr[u32OdChn],0,sizeof(OD_ATTR_S));
	memset(&gOdAttrPriv[u32OdChn],0,sizeof(OD_ATTR_PRIV_S));

	return 0;
}

static AR_S32  ar_GridCopy(char*pdst, char* psrc, int grid_idx)
{
    if(!psrc || !pdst)
    {
       printf("null pointer \r\n");
	   return -1;
	}

    AR_S32 nRows = RESIZED_WIDTH/GRID_WIDTH;
	AR_S32 nCols = RESIZED_HEIGHT/GRID_HEIGHT;
	AR_S32 iRow = grid_idx/nRows;
	AR_S32 iCol = grid_idx%nCols;

	char* ps=NULL;
	char* pd=NULL;
	ps = psrc + iRow*GRID_HEIGHT*RESIZED_WIDTH + iCol*GRID_WIDTH;
    pd = pdst;
	for(AR_S32 jj=0;jj<GRID_HEIGHT;jj++)
	{
		memcpy(pd, ps, GRID_WIDTH);
		ps = ps + RESIZED_WIDTH;
		pd = pd + GRID_WIDTH;
	}
	return 0;
}



static AR_S32 ar_CalcHist(IplImage* grid, OD_ATTR_PRIV_S* pstOdPrivAttr, OD_STATUS_S* pstOdState)
{
	AR_S32 dims=1;
	AR_S32 size=256;
	AR_FLOAT rang[]={0,255};
	AR_FLOAT* rangs[]={rang};
	CvHistogram* hist;
	hist=cvCreateHist(dims,&size,CV_HIST_ARRAY,rangs,1);
	cvClearHist(hist);

	cvCalcArrHist(&grid,hist,0,0);


	AR_FLOAT bin_val_sum   = 0;
	AR_FLOAT bin_val_total = 0;

    for(AR_S32 i=0;i<size;i++)
    {
        AR_FLOAT bin_val = (float)cvGetReal1D(hist->bins, i);//cvQueryHistValue_1D( hist, i);
		if((i<=pstOdPrivAttr->u32HistGrayThred)&&(i>=0))
		{
			bin_val_sum  += bin_val;
		}
		bin_val_total += bin_val;
    }

	cvReleaseHist(&hist);


	AR_FLOAT percent = (bin_val_sum / bin_val_total )*100;
	//printf("ar_CalcHist bin_val_sum %f bin_val_total %f \r\n",bin_val_sum,bin_val_total);
	if(percent >= (AR_FLOAT)pstOdPrivAttr->u32HistPixelPercentThred)
	{
		pstOdState->u32GrayScaleStatus = 1;
	}
	else
	{
		pstOdState->u32GrayScaleStatus = 0;
	}

	return 0;
}

static AR_S32 ar_Canny(IplImage*  imgBlurred, OD_ATTR_PRIV_S* pstOdPrivAttr, OD_STATUS_S* pstOdState)
{
	IplImage*  imgCanny =NULL;
	imgCanny = cvCreateImage(cvGetSize(imgBlurred),IPL_DEPTH_8U,1);

	cvCanny(imgBlurred,imgCanny,CANNY_LOW_THRED,CANNY_HIGH_THRED,3);

	/* calc canny edge point num */
	unsigned int canny_cnt = 0;
	for(AR_S32 m=0;m<imgCanny->height;m++)
	{
		for(AR_S32 n=0;n<imgCanny->width;n++)
		{
		    uchar* canny_p = (uchar*)imgCanny->imageData;
			canny_p = canny_p + m*imgCanny->width +n;
			uchar  canny_val = *(uchar*)canny_p;
			if(canny_val>0)
			{
				canny_cnt++;
			}
		}
	}
	cvReleaseImage(&imgCanny);

//	printf("canny_cnt %d \n",canny_cnt);
	if((canny_cnt>=0)&&(canny_cnt<pstOdPrivAttr->u32EdgePointThred))
		pstOdState->u32EdgeStatus = 1;
	else
		pstOdState->u32EdgeStatus = 0;

	return 0;
}

AR_S32 AR_IVE_OD_Process(AR_U32 u32OdChn, OD_ATTR_S *pstOdAttr)
{
	CvSize size;
	size.width = RESIZED_WIDTH;
	size.height = RESIZED_HEIGHT;
	IplImage* ImgResized = cvCreateImage(size,8,1);
	if(!ImgResized)
	{
	   printf("cvCreateImage error \r\n");
	   return 0;
	}

	IplImage* ImgOrg = NULL;
	if(( pstOdAttr->u32Width != RESIZED_WIDTH) ||(pstOdAttr->u32Height !=RESIZED_HEIGHT))
	{
	    CvSize sizeorg;
		sizeorg.width = pstOdAttr->u32Width;
		sizeorg.height = pstOdAttr->u32Height;
		ImgOrg = cvCreateImage(sizeorg,8,1);
		if(!ImgOrg)
		{
		   printf("cvCreateImage error \r\n");
		   return -1;
		}
		memcpy(ImgOrg->imageData,(uchar*)pstOdAttr->pStreamVa, sizeorg.width * sizeorg.height);
		cvResize(ImgOrg,ImgResized,CV_INTER_LINEAR);
	}
	else
	{
		memcpy(ImgResized->imageData,(uchar*)pstOdAttr->pStreamVa, size.width * size.height);
	}


	IplImage* ImgBlurred = cvCreateImage(size,8,1);
	if(!ImgBlurred)
	{
	   printf("cvCreateImage error \r\n");
	   cvReleaseImage(&ImgResized);
	   if(!ImgOrg){
	      cvReleaseImage(&ImgOrg);
	   }
	   return -1;
	}

	cvSmooth( ImgResized, ImgBlurred, CV_GAUSSIAN, \
          GAUSSIAN_KSIZE_WIDTH, GAUSSIAN_KSIZE_HEIGHT, GAUSSIAN_SIGMA_X, GAUSSIAN_SIGMA_Y );

	/* crop image to rectangle grid */
	size.width = GRID_WIDTH;
	size.height = GRID_HEIGHT;
	IplImage* grid[GRID_NUM];
	for(int i=0;i<GRID_NUM;i++)
	{
 		grid[i] = cvCreateImage(size,8,1);
		ar_GridCopy((char*)grid[i]->imageData, (char*)ImgBlurred->imageData, i);
	}

   
	OD_STATUS_S pstOdState[GRID_NUM]={0};

	/* imgGrid calcHist & Canny edge detect */
	AR_S32 alarmCnt = 0;
	for(AR_S32 i=0;i<GRID_NUM;i++)
	{
		/* calcHist & calc */
		ar_CalcHist(grid[i],&gOdAttrPriv[u32OdChn],&pstOdState[i]);

		//cout<< "occlusion_state[" << i << "].GrayScaleStatus " <<occlusion_state[i].GrayScaleStatus<<endl;

		/* canny edge detection */
		ar_Canny(grid[i],&gOdAttrPriv[u32OdChn],&pstOdState[i]);

		//cout<< "occlusion_state[" <<i<<"].EdgeStatus " <<occlusion_state[i].EdgeStatus<<endl;

		/* check is alarm state */
		if((pstOdState[i].u32GrayScaleStatus==1)&&(pstOdState[i].u32EdgeStatus==1))
		{
			pstOdState[i].u32AlarmStatus == 1;
			alarmCnt++;
		}
	}

	for(AR_S32 i=0;i<GRID_NUM;i++)
	{
		cvReleaseImage(&grid[i]);
	}

	if(!ImgOrg){
	   cvReleaseImage(&ImgOrg);
	}

	cvReleaseImage(&ImgResized);
	cvReleaseImage(&ImgBlurred);

	 /* check is occlusion */
	if(alarmCnt>=gOdAttr[u32OdChn].u32AlarmBlockNumThred)
	{
	    //printf("alarmCnt %d thread %d\r\n",alarmCnt,gOdAttr[u32OdChn].u32AlarmBlockNumThred);
		return 1;
	}
	else
	{
		return 0;
	}

}


