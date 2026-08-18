#if 0
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
#include <errno.h>
#include <poll.h>
#include <pthread.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "ar_ive.h"
#include "mpi_vpss.h"
#include "hal_sys.h"
#include "mpi_scaler_api.h"

using namespace std;
using namespace cv;

#define OD_MEM_SIZE    8294400 //(3840 * 2160)

static AR_S32 AR_OD_Acquire_Image(AR_S32 VpssGrp, AR_S32 VpssChn, OD_ATTR_S *pstOdAttr)
{
	VIDEO_FRAME_INFO_S VFrameInfo = {0};
	AR_S32 ret=AR_MPI_VPSS_GetChnFrame(VpssGrp, VpssChn, &VFrameInfo, -1);
	if(ret<0)
	{
		printf("AR_MPI_VPSS_GetChnFrame ERR ret=%x\r\n", ret);
		return -1;
	}

	pstOdAttr->u32Width = VFrameInfo.stVFrame.u32Width;
	pstOdAttr->u32Height = VFrameInfo.stVFrame.u32Height;

	AR_IMG_S pstSrcImg;
	pstSrcImg.enFormat = AR_IMG_GRAY;
	pstSrcImg.u32Width = VFrameInfo.stVFrame.u32Width;
	pstSrcImg.u32Height = VFrameInfo.stVFrame.u32Height;
	pstSrcImg.u32ChannelNum = 1;
	pstSrcImg.astChannels[0].u32Stride = VFrameInfo.stVFrame.u32Stride[0];
	pstSrcImg.astChannels[0].u32AddrPhy = (AR_U32)VFrameInfo.stVFrame.u64PhyAddr[0];

	AR_IMG_S pstDstImg;
	pstDstImg.enFormat = AR_IMG_GRAY;
	pstDstImg.u32Width = RESIZED_WIDTH;
	pstDstImg.u32Height = RESIZED_HEIGHT;
	pstDstImg.u32ChannelNum = 1;
	pstDstImg.astChannels[0].u32Stride = RESIZED_WIDTH;
	pstDstImg.astChannels[0].u32AddrPhy = (AR_U32)pstOdAttr->u64StreamPa;

	AR_HAL_SCALER_CROP_S stCrop;
	stCrop.u32X = 0;
	stCrop.u32Y = 0;
	stCrop.u32W = pstSrcImg.u32Width;
	stCrop.u32H = pstSrcImg.u32Height;
	AR_S32 s32Ret = AR_MPI_SCALER_CropResize(&pstSrcImg, &stCrop, &pstDstImg, 1, 1);
	if(s32Ret < 0)
	{
		AR_MPI_VPSS_ReleaseChnFrame(VpssGrp, VpssChn, &VFrameInfo);
		printf("Do crop/resize error.\n");
		return -1;
	}

	ret = AR_MPI_VPSS_ReleaseChnFrame(VpssGrp, VpssChn, &VFrameInfo);
	if(ret < 0)
	{
		printf("AR_MPI_VPSS_ReleaseChnFrame ERR ret=%x.\n",ret);
		return -1;
	}

	return 0;
}


AR_S32 main(AR_S32 argc, AR_CHAR* argv[])
{
    if(argc<3)
	{
		printf("ar_od  usage:\n");
		printf("ar_od canny usage:\n ar_od [VpssGrp] [VpssChn] [HistGrayThred] [HistPixelPercentThred] [CannyEdgePointThred] [AlarmBlockNumThred] [LoopCnt]\n");
		printf("VpssGrp:\t\t Vpss Group ID\n");
		printf("VpssChn:\t\t Vpss Channel ID\n");
		printf("HistGrayThred :\t\t0~255,default:150 img gray hist threshold.\n");
		printf("HistPixelPercentThred:\t0~100,default:80 hist piexel percent .\n");
		printf("CannyEdgePointThred:\t0~100,default:50 hist piexel percent .\n");
		printf("AlarmBlockNumThred:\t1~48,default:16 Alarm block num,grid size:40*40,img resize:320*240.\n");
		printf("LoopCnt:\t\t app Loop Cnt\n");
		return -1;
	}

	printf("occlusion detect init start \r\n");

	struct timeval tm;
	gettimeofday(&tm, NULL);
	AR_U64 t1 = tm.tv_sec * 1000000 + tm.tv_usec;

	AR_S32 ret = AR_IVE_OD_Init();
	if(ret<0)
	{
		printf("AR_IVE_OD_Init err... \r\n");
		return -1;
	}

	OD_ATTR_S * pstOdAttr = (OD_ATTR_S *)malloc(sizeof(OD_ATTR_S));
	if(!pstOdAttr)
	{
	   printf("malloc pstOdAttr err \r\n");
	   AR_IVE_OD_Exit();
	   return -1;
	}


	ret = ar_hal_sys_mmz_alloc(&pstOdAttr->u64StreamPa, &pstOdAttr->pStreamVa, "OdStream", NULL, OD_MEM_SIZE);
	if(ret)
	{
		printf("Malloc Stream error.\n");
		free(pstOdAttr);
		AR_IVE_OD_Exit();
		return -1;
	}

	AR_S32 OdChn = 0;
	AR_S32 VpssGrp = atoi(argv[1]);
    AR_S32 VpssChn = atoi(argv[2]);
	pstOdAttr->u32HistGrayThred =           atoi(argv[3]);
	pstOdAttr->u32HistPixelPercentThred =   atoi(argv[4]);
	pstOdAttr->u32EdgePointThred =          atoi(argv[5]);
	pstOdAttr->u32AlarmBlockNumThred =      atoi(argv[6]);

	ret = AR_IVE_OD_CreateChn(OdChn,pstOdAttr);
	if(ret<0)
	{
		printf("AR_IVE_OD_CreateChn err... \r\n");
		ar_hal_sys_mmz_free(pstOdAttr->u64StreamPa,pstOdAttr->pStreamVa);
		free(pstOdAttr);
		AR_IVE_OD_Exit();
		return -1;
	}

	gettimeofday(&tm, NULL);
	AR_U64 t2 = tm.tv_sec * 1000000 + tm.tv_usec;
	printf("occlusion detect init done taskes %f ms\r\n",(t2-t1)*1.0/1000);


	AR_U32 u32Cnt =0;
	AR_U32 u32DetectCnt =0;
	AR_U32 u32TotalCnt = atoi(argv[7]);
	while (u32Cnt < u32TotalCnt)
	{

		gettimeofday(&tm, NULL);
		AR_U64 u64Tic = tm.tv_sec * 1000000 + tm.tv_usec;


		ret= AR_OD_Acquire_Image(VpssGrp,VpssChn,pstOdAttr);
		if(ret<0)
		{
			printf("acquire live stream err... \r\n");
			continue;
		}

		ret = AR_IVE_OD_Process(OdChn,pstOdAttr);
		if(ret>0)
		{
			u32DetectCnt++;
			if(u32DetectCnt>=10)
			{
				printf("occlusion detected occlusion ...\r\n");

				gettimeofday(&tm, NULL);
        		AR_U64 u64Toc = tm.tv_sec * 1000000 + tm.tv_usec;
				printf("[%d] occlusion detect taskes %f ms\r\n",u32Cnt++,(u64Toc-u64Tic)*1.0/1000);
				u32DetectCnt = 0;
			}
		}

	}

exit_handler:

	AR_IVE_OD_DestroyChn(OdChn);
	AR_IVE_OD_Exit();
	ar_hal_sys_mmz_free(pstOdAttr->u64StreamPa,pstOdAttr->pStreamVa);
	free(pstOdAttr);

	return 0;
}


#else

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
#include "ar_ive.h"
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
	AR_U32 u32VpssGrp;
	AR_U32 u32VpssChn;
	OD_ATTR_S stOdAttr;
	AR_U32 u32DetectCntThread;
}SAMPLE_IVE_OD_S;



static AR_BOOL s_bStopSignal = AR_FALSE;
static pthread_t s_OdThread = 0;
static SAMPLE_IVE_OD_S s_stOd ={0};


AR_S32 SAMPLE_IVE_OD_DmaImage(VIDEO_FRAME_INFO_S *pstFrameInfo,OD_ATTR_S *pstOdAttr)
{
    AR_S32 s32Ret;

	pstOdAttr->u32Width = RESIZED_WIDTH;
	pstOdAttr->u32Height = RESIZED_HEIGHT;

	AR_IMG_S pstSrcImg;
	pstSrcImg.enFormat = AR_IMG_GRAY;
	pstSrcImg.u32Width = pstFrameInfo->stVFrame.u32Width;
	pstSrcImg.u32Height = pstFrameInfo->stVFrame.u32Height;
	pstSrcImg.u32ChannelNum = 1;
	pstSrcImg.astChannels[0].u32Stride = pstFrameInfo->stVFrame.u32Stride[0];
	pstSrcImg.astChannels[0].u32AddrPhy = (AR_U32)pstFrameInfo->stVFrame.u64PhyAddr[0];

	AR_IMG_S pstDstImg;
	pstDstImg.enFormat = AR_IMG_GRAY;
	pstDstImg.u32Width = RESIZED_WIDTH;
	pstDstImg.u32Height = RESIZED_HEIGHT;
	pstDstImg.u32ChannelNum = 1;
	pstDstImg.astChannels[0].u32Stride = RESIZED_WIDTH;
	pstDstImg.astChannels[0].u32AddrPhy = (AR_U32)pstOdAttr->u64StreamPa;


	AR_HAL_SCALER_CROP_S stCrop;
	stCrop.u32X = 0;
	stCrop.u32Y = 0;
	stCrop.u32W = pstSrcImg.u32Width;
	stCrop.u32H = pstSrcImg.u32Height;
	s32Ret = AR_MPI_SCALER_CropResize(&pstSrcImg, &stCrop, &pstDstImg, 1, 1);
    SAMPLE_CHECK_EXPR_RET(AR_SUCCESS != s32Ret,s32Ret,"Error(%#x),AR_MPI_SCALER_CropResize failed!\n",s32Ret);

    return AR_SUCCESS;
}

static AR_S32 SAMPLE_IVE_Od_GetStreamInfo(AR_U32 u32VpssGrp, AR_U32 u32VpssChn,AR_U32*pPicSize,SIZE_S *pstSize)
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

static AR_S32 SAMPLE_IVE_Od_Init(SAMPLE_IVE_OD_S *pstOd,AR_U32 u32PicSize,AR_U32 u32Width,AR_U32 u32Height)
{
    AR_S32 s32Ret = AR_SUCCESS;

    //malloc buffer for dma,cause live stream should release immediately
	s32Ret = ar_hal_sys_mmz_alloc((AR_U64*)&pstOd->stOdAttr.u64StreamPa,
	(AR_VOID**)&pstOd->stOdAttr.pStreamVa, "ODStream", NULL, u32PicSize);
    SAMPLE_CHECK_EXPR_RET(AR_SUCCESS != s32Ret,s32Ret,"Error(%#x),ar_hal_sys_mmz_alloc failed!\n",s32Ret);

    //Set attr info
    pstOd->stOdAttr.u32Width = u32Width;
    pstOd->stOdAttr.u32Height = u32Height;

    s32Ret = AR_IVE_OD_Init();
    SAMPLE_CHECK_EXPR_GOTO(AR_SUCCESS != s32Ret, OD_INIT_FAIL,
        "Error(%#x),AR_IVS_OD_Init failed!\n", s32Ret);

	return s32Ret;

OD_INIT_FAIL:

	ar_hal_sys_mmz_free((AR_U64)pstOd->stOdAttr.u64StreamPa,
		(AR_VOID*)pstOd->stOdAttr.pStreamVa);

    return s32Ret;

}


static AR_VOID SAMPLE_IVE_Od_Uninit(SAMPLE_IVE_OD_S *pstOd)
{
    AR_S32 s32Ret = AR_SUCCESS;

	ar_hal_sys_mmz_free((AR_U64)pstOd->stOdAttr.u64StreamPa,
		(AR_VOID*)pstOd->stOdAttr.pStreamVa);

    s32Ret = AR_IVE_OD_Exit();
    if(s32Ret != AR_SUCCESS)
    {
       SAMPLE_PRT("AR_IVE_OD_Exit fail,Error(%#x)\n",s32Ret);
       return ;
    }
}


static AR_VOID * SAMPLE_IVE_OdProc(AR_VOID * pArgs)
{
    AR_S32 s32Ret;
    SAMPLE_IVE_OD_S *pstOd;
    pstOd = (SAMPLE_IVE_OD_S *)(pArgs);
    AR_S32 s32VpssGrp = pstOd->u32VpssGrp;
    AR_S32 s32VpssChn = pstOd->u32VpssChn;
	MD_CHN OdChn = 0;
	AR_U64 u64Tic = 0,u64Toc = 0;
	static struct timeval tm;
	AR_S32 s32MilliSec = -1;
	AR_U32 u32DetectCnt =0;
	VIDEO_FRAME_INFO_S VFrameInfo = {0};

    //Create chn
    s32Ret = AR_IVE_OD_CreateChn(OdChn,&(pstOd->stOdAttr));
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_IVE_OD_CreateChn fail,Error(%#x)\n",s32Ret);
        return NULL;
    }

    while (AR_FALSE == s_bStopSignal)
    {
		gettimeofday(&tm, NULL);
		u64Tic = tm.tv_sec * 1000000 + tm.tv_usec;
		
		memset(&VFrameInfo,0,sizeof(VIDEO_FRAME_INFO_S));

        s32Ret = AR_MPI_VPSS_GetChnFrame(s32VpssGrp, s32VpssChn, &VFrameInfo, s32MilliSec);
        if(AR_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("Error(%#x),AR_MPI_VPSS_GetChnFrame failed, VPSS_GRP(%d), VPSS_CHN(%d)!\n",
                s32Ret,s32VpssGrp, s32VpssChn);
            continue;
        }

        s32Ret = SAMPLE_IVE_OD_DmaImage(&VFrameInfo,&pstOd->stOdAttr);
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

		s32Ret = AR_IVE_OD_Process(OdChn, &pstOd->stOdAttr);
		if(s32Ret > 0)
		{
			u32DetectCnt++;
			if(u32DetectCnt >= pstOd->u32DetectCntThread)
			{
			    u32DetectCnt = 0;

				gettimeofday(&tm, NULL);
			    u64Toc = tm.tv_sec * 1000000 + tm.tv_usec;	
				printf("OD Alg Detected Occlusion, takes %f ms ...\r\n",(u64Toc-u64Tic)*1.0/1000);					
			}
		}

     }

PROC_FAIL:

     //destroy chn
     s32Ret = AR_IVE_OD_DestroyChn(OdChn);
     if (AR_SUCCESS != s32Ret)
     {
         SAMPLE_PRT("AR_IVE_OD_DestroyChn fail,Error(%#x)\n",s32Ret);
     }

     return AR_NULL;
}

AR_VOID SAMPLE_IVE_Od(AR_VOID)
{
	AR_S32 s32Ret = AR_SUCCESS;
	AR_CHAR acThreadName[16] = {0};

    /******************************************
     step 1: Get frame info
     ******************************************/
    SIZE_S stSize;
    AR_U32 u32PicSize;
    s32Ret = SAMPLE_IVE_Od_GetStreamInfo(s_stOd.u32VpssGrp,s_stOd.u32VpssChn,&u32PicSize,&stSize);
    SAMPLE_CHECK_EXPR_GOTO(AR_SUCCESS != s32Ret, END_OD_1,
        "Error(%#x),SAMPLE_IVE_Od_GetStreamInfo failed!\n", s32Ret);

    /******************************************
     step 2: Init Od
     ******************************************/
    s32Ret = SAMPLE_IVE_Od_Init(&s_stOd,u32PicSize,stSize.u32Width,stSize.u32Height);
    SAMPLE_CHECK_EXPR_GOTO(AR_SUCCESS != s32Ret, END_OD_0,
        " Error(%#x),SAMPLE_IVE_Od_Init failed!\n", s32Ret);


    /******************************************
      step 3: Create work thread
     ******************************************/
    s_bStopSignal = AR_FALSE;
    snprintf(acThreadName, 16, "IVE_OdProc");
    prctl(PR_SET_NAME, (unsigned long)acThreadName, 0,0,0);
    pthread_create(&s_OdThread, 0, SAMPLE_IVE_OdProc, (AR_VOID *)&s_stOd);

    SAMPLE_PAUSE();
    s_bStopSignal = AR_TRUE;
    pthread_join(s_OdThread, AR_NULL);
    s_OdThread = 0;


END_OD_0:

	SAMPLE_IVE_Od_Uninit(&s_stOd);
	memset(&s_stOd,0,sizeof(s_stOd));

END_OD_1:

    return ;
}

AR_S32 main(AR_S32 argc, AR_CHAR* argv[])
{
	
    if(argc<3)
	{
		printf("ar_od  usage:\n");
		printf("ar_od canny usage:\n ar_od [VpssGrp] [VpssChn] [HistGrayThred] [HistPixelPercentThred] [CannyEdgePointThred] [AlarmBlockNumThred]\n");
		printf("VpssGrp:\t\t Vpss Group ID\n");
		printf("VpssChn:\t\t Vpss Channel ID\n");
		printf("AlarmBlockNumThred:\t1~48,default:30 Alarm grid num, grid size:40*40,img resize:320*240.\n");
		return -1;
	}

    memset(&s_stOd,0,sizeof(s_stOd));
	s_stOd.u32VpssGrp = atoi(argv[1]);
	s_stOd.u32VpssChn = atoi(argv[2]);
	s_stOd.stOdAttr.u32AlarmBlockNumThred =    atoi(argv[3]);
	s_stOd.u32DetectCntThread = 5;

	SAMPLE_IVE_Od();

	return 0;
}


#endif




