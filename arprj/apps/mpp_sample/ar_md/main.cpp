
/***********************************************************************
 * canny edge detect/vibe motion detect demo
 * 2019-11-11
 ***********************************************************************/
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/prctl.h>
#include <errno.h>
#include <poll.h>
#include <pthread.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "ar_ivs.h"
#include "mpi_vpss.h"
#include "hal_sys.h"
#include "mpi_scaler_api.h"

using namespace std;
using namespace cv;


#define SAMPLE_PRT(fmt...)   \
    do {\
        printf("[%s]-%d: ", __FUNCTION__, __LINE__);\
        printf(fmt);\
    }while(0)

#define SAMPLE_PAUSE()\
    do {\
        printf("---------------press Enter key to exit!---------------\n");\
        (void)getchar();\
    } while (0)
#define SAMPLE_CHECK_EXPR_RET(expr, ret, fmt...)\
do\
{\
    if(expr)\
    {\
        SAMPLE_PRT(fmt);\
        return (ret);\
    }\
}while(0)
#define SAMPLE_CHECK_EXPR_GOTO(expr, label, fmt...)\
do\
{\
    if(expr)\
    {\
        SAMPLE_PRT(fmt);\
        goto label;\
    }\
}while(0)


typedef struct
{
    POINT_S astPoint[4];
} SAMPLE_IVE_RECT_S;

typedef struct
{
    AR_U16 u16Num;
    SAMPLE_IVE_RECT_S astRect[16];
} SAMPLE_RECT_ARRAY_S;

typedef struct
{
	AR_U32 u32VpssGrp;
	AR_U32 u32VpssChn;
	AR_U32 u32PostMethod;
	AR_U32 u32AreaPercentage;
	AR_IMG_S stSrc;
	MD_ATTR_S stMdAttr;
	SAMPLE_RECT_ARRAY_S stUsrDefRegion;

}SAMPLE_IVE_MD_S;



static AR_BOOL s_bStopSignal = AR_FALSE;
static pthread_t s_MdThread = 0;
static SAMPLE_IVE_MD_S s_stMd ={0};
static struct timeval tm;
static AR_U64 u64Tic = 0;
static AR_U64 u64TocSegMap = 0;
static AR_U64 u64TocBlob = 0;


AR_S32 SAMPLE_IVE_MD_DmaImage(VIDEO_FRAME_INFO_S *pstFrameInfo,AR_U32 u32DstPhyAddr)
{
    AR_S32 s32Ret;

	AR_IMG_S pstSrcImg;
	pstSrcImg.enFormat = AR_IMG_GRAY;
	pstSrcImg.u32Width = pstFrameInfo->stVFrame.u32Width;
	pstSrcImg.u32Height = pstFrameInfo->stVFrame.u32Height;
	pstSrcImg.u32ChannelNum = 1;
	pstSrcImg.astChannels[0].u32Stride = pstFrameInfo->stVFrame.u32Stride[0];
	pstSrcImg.astChannels[0].u32AddrPhy = (AR_U32)pstFrameInfo->stVFrame.u64PhyAddr[0];

	AR_IMG_S pstDstImg;
	pstDstImg.enFormat = AR_IMG_GRAY;
	pstDstImg.u32Width = pstFrameInfo->stVFrame.u32Width;
	pstDstImg.u32Height = pstFrameInfo->stVFrame.u32Height;
	pstDstImg.u32ChannelNum = 1;
	pstDstImg.astChannels[0].u32Stride = pstFrameInfo->stVFrame.u32Width;
	pstDstImg.astChannels[0].u32AddrPhy = u32DstPhyAddr;


	AR_HAL_SCALER_CROP_S stCrop;
	stCrop.u32X = 0;
	stCrop.u32Y = 0;
	stCrop.u32W = pstSrcImg.u32Width;
	stCrop.u32H = pstSrcImg.u32Height;
	s32Ret = AR_MPI_SCALER_CropResize(&pstSrcImg, &stCrop, &pstDstImg, 1, 1);
    SAMPLE_CHECK_EXPR_RET(AR_SUCCESS != s32Ret,s32Ret,"Error(%#x),AR_MPI_SCALER_CropResize failed!\n",s32Ret);

    return AR_SUCCESS;
}

static AR_S32 SAMPLE_IVE_Md_GetStreamInfo(AR_U32 u32VpssGrp, AR_U32 u32VpssChn,AR_U32*pPicSize,SIZE_S *pstSize)
{
    AR_S32 s32Ret = AR_SUCCESS;

	VIDEO_FRAME_INFO_S VFrameInfo = {0};
	s32Ret=AR_MPI_VPSS_GetChnFrame(u32VpssGrp, u32VpssChn, &VFrameInfo, -1);
    SAMPLE_CHECK_EXPR_RET(AR_SUCCESS != s32Ret,s32Ret,"Error(%#x),AR_MPI_VPSS_GetChnFrame failed!\n",s32Ret);

	pstSize->u32Width = VFrameInfo.stVFrame.u32Width;
	pstSize->u32Height = VFrameInfo.stVFrame.u32Height;

	*pPicSize = VFrameInfo.stVFrame.u32Stride[0]*VFrameInfo.stVFrame.u32Height*sizeof(AR_UCHAR);


	s32Ret = AR_MPI_VPSS_ReleaseChnFrame(u32VpssGrp, u32VpssChn, &VFrameInfo);
    SAMPLE_CHECK_EXPR_RET(AR_SUCCESS != s32Ret,s32Ret,"Error(%#x),AR_MPI_VPSS_ReleaseChnFrame failed!\n",s32Ret);


    return s32Ret;

}

static AR_S32 SAMPLE_IVE_Md_Init(SAMPLE_IVE_MD_S *pstMd,AR_U32 u32PicSize,AR_U32 u32Width,AR_U32 u32Height)
{
    AR_S32 s32Ret = AR_SUCCESS;

	pstMd->stSrc.u32Width = u32Width;
	pstMd->stSrc.u32Height = u32Height;

    //malloc buffer for dma,cause live stream should release immediately
	s32Ret = ar_hal_sys_mmz_alloc((AR_U64*)&pstMd->stSrc.astChannels[0].u32AddrPhy,
	(AR_VOID**)&pstMd->stSrc.astChannels[0].uptrAddrVirt, "MDStream", NULL, u32PicSize);
    SAMPLE_CHECK_EXPR_RET(AR_SUCCESS != s32Ret,s32Ret,"Error(%#x),ar_hal_sys_mmz_alloc failed!\n",s32Ret);


    //Set attr info
    pstMd->stMdAttr.u16Width = (AR_U16)u32Width;
    pstMd->stMdAttr.u16Height = (AR_U16)u32Height;

    s32Ret = AR_IVS_MD_Init();
    SAMPLE_CHECK_EXPR_GOTO(AR_SUCCESS != s32Ret, MD_INIT_FAIL,
        "Error(%#x),AR_IVS_MD_Init failed!\n", s32Ret);

	return s32Ret;

MD_INIT_FAIL:

	ar_hal_sys_mmz_free((AR_U64)pstMd->stSrc.astChannels[0].u32AddrPhy,
		(AR_VOID*)pstMd->stSrc.astChannels[0].uptrAddrVirt);

    return s32Ret;

}


static AR_VOID SAMPLE_IVE_Md_Uninit(SAMPLE_IVE_MD_S *pstMd)
{
    AR_S32 s32Ret = AR_SUCCESS;

	ar_hal_sys_mmz_free((AR_U64)pstMd->stSrc.astChannels[0].u32AddrPhy,
		(AR_VOID*)pstMd->stSrc.astChannels[0].uptrAddrVirt);

    s32Ret = AR_IVS_MD_Exit();
    if(s32Ret != AR_SUCCESS)
    {
       SAMPLE_PRT("AR_IVS_MD_Exit fail,Error(%#x)\n",s32Ret);
       return ;
    }
}


static AR_VOID SAMPLE_IVE_MD_SegmapPost(AR_U32 MdChn,SAMPLE_IVE_MD_S *pstMd,AR_IMG_S stSegmapOut)
{
	Rect rect;

    AR_U32 u32W =pstMd->stSrc.u32Width;
	AR_U32 u32H =pstMd->stSrc.u32Height;
	Mat Image(Size(u32W,u32H),CV_8UC1);
	memcpy(Image.data,(AR_UCHAR*)stSegmapOut.astChannels[0].uptrAddrVirt, u32W*u32H );

    AR_U32 u32RegionNum = pstMd->stUsrDefRegion.u16Num;
    for (AR_U32 rgnId = 0; rgnId < u32RegionNum; rgnId++)
    {
		u32W = pstMd->stUsrDefRegion.astRect[rgnId].astPoint[1].s32X - pstMd->stUsrDefRegion.astRect[rgnId].astPoint[0].s32X;
		u32H = pstMd->stUsrDefRegion.astRect[rgnId].astPoint[2].s32Y - pstMd->stUsrDefRegion.astRect[rgnId].astPoint[0].s32Y;
		rect.x = pstMd->stUsrDefRegion.astRect[rgnId].astPoint[0].s32X;
		rect.y = pstMd->stUsrDefRegion.astRect[rgnId].astPoint[0].s32Y;
		rect.width = u32W;
		rect.height = u32H;
		Mat subImage = Mat(Image, rect);

		vector<vector <Point> > contours;
		vector<Vec4i> hierarchy;
		findContours(subImage,contours,hierarchy,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);
		AR_DOUBLE darea=0;
		for(int i=0; i<contours.size(); i++)
		{
			if( contourArea(contours[i]) > 100 )
			{
				darea += contourArea(contours[i]);
			}
		}

		if(fabs(darea)>= ((AR_DOUBLE)pstMd->u32AreaPercentage * (u32W *u32H))/100)
		{
			gettimeofday(&tm, NULL);
			u64TocSegMap= tm.tv_sec * 1000000 + tm.tv_usec;
			printf("[%d][%d] SegMapMethod usrRegion-%d Moving Detected.Taskes %f ms.\r\n",pstMd->u32VpssGrp,pstMd->u32VpssChn,rgnId,(u64TocSegMap-u64Tic)*1.0/1000);
		}
	}

}

static AR_VOID SAMPLE_IVE_MD_BlobPost(AR_U32 MdChn,SAMPLE_IVE_MD_S *pstMd,MD_BLOB_S stBlob)
{
	Rect usrRect,blobRect,iouRect;

    AR_U32 u32W =pstMd->stSrc.u32Width;
	AR_U32 u32H =pstMd->stSrc.u32Height;

	AR_U32 u32RegionNum = pstMd->stUsrDefRegion.u16Num;
	for (AR_U32 usrRgnId = 0; usrRgnId < u32RegionNum; usrRgnId++)
	{
		u32W = pstMd->stUsrDefRegion.astRect[usrRgnId].astPoint[1].s32X - pstMd->stUsrDefRegion.astRect[usrRgnId].astPoint[0].s32X;
		u32H = pstMd->stUsrDefRegion.astRect[usrRgnId].astPoint[2].s32Y - pstMd->stUsrDefRegion.astRect[usrRgnId].astPoint[0].s32Y;
		usrRect.x = pstMd->stUsrDefRegion.astRect[usrRgnId].astPoint[0].s32X;
		usrRect.y = pstMd->stUsrDefRegion.astRect[usrRgnId].astPoint[0].s32Y;
		usrRect.width = u32W;
		usrRect.height = u32H;

		AR_U32 u32area=0;
		for (AR_U32 blobRgnId = 0; blobRgnId < stBlob.u8RegionNum; blobRgnId++)
		{
			AR_U32 W = stBlob.astRegion[blobRgnId].u16Right - stBlob.astRegion[blobRgnId].u16Left;
			AR_U32 H = stBlob.astRegion[blobRgnId].u16Bottom- stBlob.astRegion[blobRgnId].u16Top;
			blobRect.x = stBlob.astRegion[blobRgnId].u16Left;
			blobRect.y = stBlob.astRegion[blobRgnId].u16Top;
			blobRect.width = W;
			blobRect.height = H;

			iouRect = usrRect & blobRect;
			u32area  = u32area + iouRect.area();
		}

		if(u32area >= (AR_U32)((AR_DOUBLE)pstMd->u32AreaPercentage/100 *(u32W *u32H)))
		{
			gettimeofday(&tm, NULL);
			u64TocBlob= tm.tv_sec * 1000000 + tm.tv_usec;
			printf("[%d][%d] BlobMethod usrRegion-%d Moving Detected.Taskes %f ms.\r\n",pstMd->u32VpssGrp,pstMd->u32VpssChn,usrRgnId,(u64TocBlob-u64Tic)*1.0/1000);
		}
	}

}

static AR_VOID * SAMPLE_IVE_MdProc(AR_VOID * pArgs)
{
    AR_S32 s32Ret;
    SAMPLE_IVE_MD_S *pstMd;
    pstMd = (SAMPLE_IVE_MD_S *)(pArgs);
    AR_S32 s32VpssGrp = pstMd->u32VpssGrp;
    AR_S32 s32VpssChn = pstMd->u32VpssChn;
	MD_CHN MdChn = 0;
	AR_S32 s32MilliSec = -1;
	VIDEO_FRAME_INFO_S VFrameInfo = {0};

    //Create chn
    s32Ret = AR_IVS_MD_CreateChn(MdChn,&(pstMd->stMdAttr));
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_IVS_MD_CreateChn fail,Error(%#x)\n",s32Ret);
        return NULL;
    }

	MD_BLOB_S stBlob ={0};
	stBlob.u16CurAreaThr = 10;

	AR_IMG_S stSegmapOut={0};

	AR_U32 u32PicSize = pstMd->stSrc.u32Width*pstMd->stSrc.u32Width*sizeof(AR_UCHAR);
	s32Ret = ar_hal_sys_mmz_alloc((AR_U64*)&stSegmapOut.astChannels[0].u32AddrPhy,
		(AR_VOID**)&stSegmapOut.astChannels[0].uptrAddrVirt, "MDSegMap", NULL, u32PicSize);
    SAMPLE_CHECK_EXPR_GOTO(AR_SUCCESS != s32Ret, PROC_FAIL,
        "Error(%#x),ar_hal_sys_mmz_alloc failed!\n", s32Ret);

    while (AR_FALSE == s_bStopSignal)
    {
		gettimeofday(&tm, NULL);
		u64Tic = tm.tv_sec * 1000000 + tm.tv_usec;

        memset((AR_UCHAR*)stSegmapOut.astChannels[0].uptrAddrVirt,0,u32PicSize);
		memset(&stBlob,0,sizeof(MD_BLOB_S));
		memset(&VFrameInfo,0,sizeof(VIDEO_FRAME_INFO_S));

        s32Ret = AR_MPI_VPSS_GetChnFrame(s32VpssGrp, s32VpssChn, &VFrameInfo, s32MilliSec);
        if(AR_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("Error(%#x),AR_MPI_VPSS_GetChnFrame failed, VPSS_GRP(%d), VPSS_CHN(%d)!\n",
                s32Ret,s32VpssGrp, s32VpssChn);
            continue;
        }

        s32Ret = SAMPLE_IVE_MD_DmaImage(&VFrameInfo,pstMd->stSrc.astChannels[0].u32AddrPhy);
        if(AR_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("Error(%#x),SAMPLE_IVE_MD_DmaImage failed, VPSS_GRP(%d), VPSS_CHN(%d)!\n",
                s32Ret,s32VpssGrp, s32VpssChn);
            continue;
        }

		s32Ret = AR_MPI_VPSS_ReleaseChnFrame(s32VpssGrp, s32VpssChn, &VFrameInfo);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("Error(%#x),AR_MPI_VPSS_ReleaseChnFrame failed,Grp(%d) chn(%d)!\n",
				s32Ret,s32VpssGrp,s32VpssChn);
			continue;
		}

		s32Ret = AR_IVS_MD_Process(MdChn,&pstMd->stSrc,&stSegmapOut,&stBlob);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("Error(%#x),AR_IVS_MD_Process failed,Grp(%d) chn(%d)!\n",
				s32Ret,s32VpssGrp,s32VpssChn);
			continue;
		}

		if(pstMd->u32PostMethod == 0)
		{
			SAMPLE_IVE_MD_SegmapPost(MdChn,pstMd,stSegmapOut);
		}
		else
		{
			SAMPLE_IVE_MD_BlobPost(MdChn,pstMd,stBlob);
		}

     }

	 ar_hal_sys_mmz_free((AR_U64)stSegmapOut.astChannels[0].u32AddrPhy,
			 (AR_VOID*)stSegmapOut.astChannels[0].uptrAddrVirt);

PROC_FAIL:

     //destroy chn
     s32Ret = AR_IVS_MD_DestroyChn(MdChn);
     if (AR_SUCCESS != s32Ret)
     {
         SAMPLE_PRT("AR_IVS_MD_DestroyChn fail,Error(%#x)\n",s32Ret);
     }

     return AR_NULL;
}

AR_VOID SAMPLE_IVE_Md(AR_VOID)
{
	AR_S32 s32Ret = AR_SUCCESS;
	AR_CHAR acThreadName[16] = {0};

    /******************************************
     step 1: Get frame info
     ******************************************/
	s_stMd.stUsrDefRegion.u16Num = 3;
	s_stMd.stUsrDefRegion.astRect[0].astPoint[0].s32X =0;
	s_stMd.stUsrDefRegion.astRect[0].astPoint[0].s32Y =0;
	s_stMd.stUsrDefRegion.astRect[0].astPoint[1].s32X =100;
	s_stMd.stUsrDefRegion.astRect[0].astPoint[1].s32Y =0;
	s_stMd.stUsrDefRegion.astRect[0].astPoint[2].s32X =0;
	s_stMd.stUsrDefRegion.astRect[0].astPoint[2].s32Y =100;
	s_stMd.stUsrDefRegion.astRect[0].astPoint[3].s32X =100;
	s_stMd.stUsrDefRegion.astRect[0].astPoint[3].s32Y =100;

	s_stMd.stUsrDefRegion.astRect[1].astPoint[0].s32X =110;
	s_stMd.stUsrDefRegion.astRect[1].astPoint[0].s32Y =110;
	s_stMd.stUsrDefRegion.astRect[1].astPoint[1].s32X =410;
	s_stMd.stUsrDefRegion.astRect[1].astPoint[1].s32Y =110;
	s_stMd.stUsrDefRegion.astRect[1].astPoint[2].s32X =110;
	s_stMd.stUsrDefRegion.astRect[1].astPoint[2].s32Y =310;
	s_stMd.stUsrDefRegion.astRect[1].astPoint[3].s32X =410;
	s_stMd.stUsrDefRegion.astRect[1].astPoint[3].s32Y =310;

	s_stMd.stUsrDefRegion.astRect[2].astPoint[0].s32X =420;
	s_stMd.stUsrDefRegion.astRect[2].astPoint[0].s32Y =320;
	s_stMd.stUsrDefRegion.astRect[2].astPoint[1].s32X =600;
	s_stMd.stUsrDefRegion.astRect[2].astPoint[1].s32Y =320;
	s_stMd.stUsrDefRegion.astRect[2].astPoint[2].s32X =420;
	s_stMd.stUsrDefRegion.astRect[2].astPoint[2].s32Y =450;
	s_stMd.stUsrDefRegion.astRect[2].astPoint[3].s32X =600;
	s_stMd.stUsrDefRegion.astRect[2].astPoint[3].s32Y =450;

    SIZE_S stSize;
    AR_U32 u32PicSize;
    s32Ret = SAMPLE_IVE_Md_GetStreamInfo(s_stMd.u32VpssGrp,s_stMd.u32VpssChn,&u32PicSize,&stSize);
    SAMPLE_CHECK_EXPR_GOTO(AR_SUCCESS != s32Ret, END_MD_1,
        "Error(%#x),SAMPLE_IVE_Md_GetStreamInfo failed!\n", s32Ret);

    /******************************************
     step 2: Init Md
     ******************************************/
    s32Ret = SAMPLE_IVE_Md_Init(&s_stMd,u32PicSize,stSize.u32Width,stSize.u32Height);
    SAMPLE_CHECK_EXPR_GOTO(AR_SUCCESS != s32Ret, END_MD_0,
        " Error(%#x),SAMPLE_IVE_Md_Init failed!\n", s32Ret);


    /******************************************
      step 3: Create work thread
     ******************************************/
    s_bStopSignal = AR_FALSE;
    snprintf(acThreadName, 16, "IVE_MdProc");
    prctl(PR_SET_NAME, (unsigned long)acThreadName, 0,0,0);
    pthread_create(&s_MdThread, 0, SAMPLE_IVE_MdProc, (AR_VOID *)&s_stMd);

    SAMPLE_PAUSE();
    s_bStopSignal = AR_TRUE;
    pthread_join(s_MdThread, AR_NULL);
    s_MdThread = 0;


END_MD_0:

	SAMPLE_IVE_Md_Uninit(&s_stMd);
	memset(&s_stMd,0,sizeof(s_stMd));

END_MD_1:

    return ;
}

AR_S32 main(AR_S32 argc, AR_CHAR* argv[])
{
	if(argc<3)
	{
		 printf("ar_md	usage:\n");
		 printf("ar_md vibe usage:\n ar_md [VpssGrp] [VpssChn] [MatchingThreshold] [AreaPercentage] [PostMethod] \n");
		 printf("VpssGrp:\t\t Vpss Group ID\n");
		 printf("VpssChn:\t\t Vpss Channel ID\n");
		 printf("MatchingThreshold:\t\t ref:20 ,0~255\n");
		 printf("AreaPercentage:\t\t ref:20 ,range:0~100\n");
		 printf("PostMethod:\t\t 0:SegMap 1:Blob, range:0~1\n");
		 return -1;
	}

    memset(&s_stMd,0,sizeof(s_stMd));
	s_stMd.u32VpssGrp = atoi(argv[1]);
	s_stMd.u32VpssChn = atoi(argv[2]);
	s_stMd.stMdAttr.u16MatchingThr = atoi(argv[3]);
	s_stMd.u32AreaPercentage = atoi(argv[4]);
	s_stMd.u32PostMethod = atoi(argv[5]);

	SAMPLE_IVE_Md();

	return 0;
}


























