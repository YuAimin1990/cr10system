#include "ipc_md.h"
#include "cfg_all.h"
#include "util_common.h"
#include "util_api.h"
#include "util_profile.h"
#include "recmng_rec.h"
#include "pf_md.h"
#include "ar_ivs.h"
#include "mpi_vpss.h"
#include "hal_sys.h"
#include "mpi_scaler_api.h"
#include <opencv2/core/core_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui_c.h>


pthread_t pMdId;
static AR_U32 isMdThreadStop;
static AR_S32 g_s32MdThreadSuspend = 0;

static AR_S32 IPC_MD_SegmapPost(AR_U32 MdChn,IPC_CFG_MD_S *pstMdCfg,AR_IMG_S stSrc,AR_IMG_S stSegmapOut)
{
	CvSize size;
	size.width = stSrc.u32Width;
	size.height = stSrc.u32Height;
	IplImage* image = cvCreateImage(size,8,1);
	if(!image)
	{
	  printf("cvCreateImage error \r\n");
	  return -1;
	}
	memcpy(image->imageData,(AR_UCHAR*)stSegmapOut.astChannels[0].uptrAddrVirt, size.width * size.height);

	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* pContour = NULL;
	AR_U32 header_size = sizeof(CvContour);
	AR_U32 mode = CV_RETR_EXTERNAL;
	AR_U32 method = CV_CHAIN_APPROX_SIMPLE;
	CvPoint offset = cvPoint(0,0);

	cvFindContours(image,storage,&pContour,header_size,mode,method,offset);

	//AR_U32 num =0;
	AR_DOUBLE dArea = 0.0;
	for (; pContour != NULL; pContour = pContour->h_next)
	{
	   dArea = dArea + fabs(cvContourArea(pContour, CV_WHOLE_SEQ, 0));
	}

	cvReleaseMemStorage(&storage);
	cvReleaseImage(&image);

	if(dArea >= ((AR_DOUBLE)pstMdCfg->u32AreaPercentage/100 *(stSrc.u32Width *stSrc.u32Height)))
	{
		return 1;
	}
	else
	{
        return 0;
	}
}

static AR_S32 IPC_MD_BlobPost(AR_U32 MdChn,IPC_CFG_MD_S *pstMdCfg,AR_IMG_S stSrc,MD_BLOB_S stBlob)
{
	CvRect usrRect;
	CvRect blobRect;
	CvRect iouRect;

	AR_U32 u32W =stSrc.u32Width;
	AR_U32 u32H =stSrc.u32Height;

	AR_U32 u32RegionNum = 1;
	for (AR_U32 usrRgnId = 0; usrRgnId < u32RegionNum; usrRgnId++)
	{

		usrRect.x = 0;
		usrRect.y = 0;
		usrRect.width = stSrc.u32Width;
		usrRect.height = stSrc.u32Height;

		AR_U32 u32area=0;
		for (AR_U32 blobRgnId = 0; blobRgnId < stBlob.u8RegionNum; blobRgnId++)
		{
			AR_U32 W = stBlob.astRegion[blobRgnId].u16Right - stBlob.astRegion[blobRgnId].u16Left;
			AR_U32 H = stBlob.astRegion[blobRgnId].u16Bottom- stBlob.astRegion[blobRgnId].u16Top;
			blobRect.x = stBlob.astRegion[blobRgnId].u16Left;
			blobRect.y = stBlob.astRegion[blobRgnId].u16Top;
			blobRect.width = W;
			blobRect.height = H;

			iouRect =  blobRect;
			u32area  = u32area + iouRect.width*iouRect.height;
		}

		if(u32area >= (AR_U32)((AR_DOUBLE)pstMdCfg->u32AreaPercentage/100 *(u32W *u32H)))
		{
			return 1;
		}
		else
		{
			return 0;
		}

	}
    return 0;
}

static AR_S32 IPC_MD_Acquire_Stream(AR_S32 VpssGrp, AR_S32 VpssChn, AR_IMG_S *pstSrc)
{
	VIDEO_FRAME_INFO_S VFrameInfo = {0};
	AR_S32 ret=AR_MPI_VPSS_GetChnFrame(VpssGrp, VpssChn, &VFrameInfo, -1);
	if(ret<0)
	{
		PRINT_ERR("AR_MPI_VPSS_GetChnFrame ERR ret=%x\r\n", ret);
		return AR_FAILURE;
	}

	AR_IMG_S pstSrcImg;
	pstSrcImg.enFormat = AR_IMG_GRAY;
	pstSrcImg.u32Width = VFrameInfo.stVFrame.u32Width;
	pstSrcImg.u32Height = VFrameInfo.stVFrame.u32Height;
	pstSrcImg.u32ChannelNum = 1;
	pstSrcImg.astChannels[0].u32Stride = VFrameInfo.stVFrame.u32Stride[0];
	pstSrcImg.astChannels[0].u32AddrPhy = (AR_U32)VFrameInfo.stVFrame.u64PhyAddr[0];

	AR_IMG_S pstDstImg;
	pstDstImg.enFormat = AR_IMG_GRAY;
	pstDstImg.u32Width = VFrameInfo.stVFrame.u32Width;
	pstDstImg.u32Height = VFrameInfo.stVFrame.u32Height;
	pstDstImg.u32ChannelNum = 1;
	pstDstImg.astChannels[0].u32Stride = VFrameInfo.stVFrame.u32Width;
	pstDstImg.astChannels[0].u32AddrPhy = pstSrc->astChannels[0].u32AddrPhy;


	AR_HAL_SCALER_CROP_S stCrop;
	stCrop.u32X = 0;
	stCrop.u32Y = 0;
	stCrop.u32W = pstSrcImg.u32Width;
	stCrop.u32H = pstSrcImg.u32Height;
	AR_S32 s32Ret = AR_MPI_SCALER_CropResize(&pstSrcImg, &stCrop, &pstDstImg, 1, 1);
	if(s32Ret < 0)
	{
		AR_MPI_VPSS_ReleaseChnFrame(VpssGrp, VpssChn, &VFrameInfo);
		PRINT_ERR("Do crop/resize error.\n");
		return AR_FAILURE;
	}

	ret = AR_MPI_VPSS_ReleaseChnFrame(VpssGrp, VpssChn, &VFrameInfo);
	if(ret < 0)
	{
		PRINT_ERR("AR_MPI_VPSS_ReleaseChnFrame ERR ret=%x.\n",ret);
		return AR_FAILURE;
	}

	return 0;
}

static void *IPC_MD_ThreadFunc(void *arg)
{
	usleep(5000000);

    //1. get stream info
	IPC_CFG_MD_S * pMdCfg = IPC_CFG_MD_GetParam();
	AR_U32 VpssGrp = pMdCfg->u32GrpId;
	AR_U32 VpssChn = pMdCfg->u32ChnId;
	VIDEO_FRAME_INFO_S VFrameInfo = {0};
	AR_S32 ret=AR_MPI_VPSS_GetChnFrame(VpssGrp, VpssChn, &VFrameInfo, -1);
	if(ret<0)
	{
		PRINT_ERR("AR_MPI_VPSS_GetChnFrame ERR ret=%x\r\n", ret);
		return NULL;
	}

	AR_U32 u32Width = VFrameInfo.stVFrame.u32Width;
	AR_U32 u32Height = VFrameInfo.stVFrame.u32Height;
	AR_U32 u32StreamSize = u32Width*u32Height;

	ret = AR_MPI_VPSS_ReleaseChnFrame(VpssGrp, VpssChn, &VFrameInfo);
	if(ret < 0)
	{
		PRINT_ERR("AR_MPI_VPSS_ReleaseChnFrame ERR ret=%x.\n",ret);
		return NULL;
	}

	//2. malloc mmz buffer for process
	AR_IMG_S stSrc={0};
	AR_IMG_S stSegmapOut={0};
	MD_BLOB_S stBlob={0};
	stBlob.u16CurAreaThr = 100;
	stSrc.u32Width = u32Width;
	stSrc.u32Height= u32Height;

    char tmp[30] ={0};
	sprintf(tmp, "Stream_%d_%d", VpssGrp,VpssChn);
	ret = ar_hal_sys_mmz_alloc((AR_U64*)&stSrc.astChannels[0].u32AddrPhy,
		(AR_VOID**)&stSrc.astChannels[0].uptrAddrVirt, tmp, NULL, u32StreamSize);
	if(ret)
	{
		PRINT_ERR("Malloc Stream error.\n");
		return NULL;
	}

	sprintf(tmp, "SegMap_%d_%d", VpssGrp,VpssChn);
	ret = ar_hal_sys_mmz_alloc((AR_U64*)&stSegmapOut.astChannels[0].u32AddrPhy,
		(AR_VOID**)&stSegmapOut.astChannels[0].uptrAddrVirt, tmp, NULL, u32StreamSize);if(ret)
	{
		PRINT_ERR("Malloc SegMap error.\n");
		goto PROC_FAIL_2;
	}

    //3. create chn
	MD_ATTR_S * pstMdAttr = (MD_ATTR_S *)malloc(sizeof(MD_ATTR_S));
	if(!pstMdAttr)
	{
	   PRINT_ERR("malloc pstMdAttr err \r\n");
	   goto PROC_FAIL_1;
	}
	memset(pstMdAttr, 0, sizeof(MD_ATTR_S));
	pstMdAttr->u16Width = u32Width;
	pstMdAttr->u16Height= u32Height;
	pstMdAttr->u16MatchingThr = pMdCfg->u16MatchingThr;
	AR_S32 MdChn = VpssGrp*IPC_MAX_STREAM_PER_PIPE + VpssChn;
	ret = AR_IVS_MD_CreateChn(MdChn,pstMdAttr);
	if(ret<0)
	{
		PRINT_ERR("AR_IVS_MD_CreateChn err... \r\n");
		goto PROC_FAIL_0;
	}

    //4. process thread
	struct timeval tm;
	while(!isMdThreadStop)
	{
        if(g_s32MdThreadSuspend)
        {
            usleep(500 * 1000);
            continue;
        }
	
		gettimeofday(&tm, NULL);
		AR_U64 u64Tic = tm.tv_sec * 1000000 + tm.tv_usec;

		memset((AR_UCHAR*)stSegmapOut.astChannels[0].uptrAddrVirt,0,u32StreamSize);
		memset(&stBlob,0,sizeof(MD_BLOB_S));

		pMdCfg = IPC_CFG_MD_GetParam();
        if(!pMdCfg->bEnable)
        {
            usleep(500 * 1000);
            continue;
        }

		ret= IPC_MD_Acquire_Stream(VpssGrp,VpssChn,&stSrc);
		if(ret<0)
		{
			PRINT_ERR("[%d][%d]acquire live stream err... \r\n",VpssGrp,VpssChn);
			continue;
		}

		ret = AR_IVS_MD_Process(MdChn,&stSrc,&stSegmapOut,&stBlob);
		if(ret > 0)
		{
			PRINT_INFO("[%d][%d]Target is Moving... \r\n",VpssGrp,VpssChn);
		}

        if(pMdCfg->u32PostMethod == 0)
		{
			ret = IPC_MD_SegmapPost(MdChn,pMdCfg,stSrc,stSegmapOut);
			if(ret)
			{
				gettimeofday(&tm, NULL);
				AR_U64 u64Toc = tm.tv_sec * 1000000 + tm.tv_usec;
				PRINT_INFO("[%d][%d] SegMapMethod Motion Detect Taskes %f ms \r\n",VpssGrp,VpssChn,(u64Toc-u64Tic)*1.0/1000);
			}
		}
		else
		{
			ret = IPC_MD_BlobPost(MdChn,pMdCfg,stSrc,stBlob);
			if(ret)
			{
				gettimeofday(&tm, NULL);
				AR_U64 u64Toc = tm.tv_sec * 1000000 + tm.tv_usec;
				PRINT_INFO("[%d][%d] BlobMethod Motion Detect Taskes %f ms \r\n",VpssGrp,VpssChn,(u64Toc-u64Tic)*1.0/1000);
			}
		}

	}

	//5.deinit
PROC_FAIL:
	AR_IVS_MD_DestroyChn(MdChn);

PROC_FAIL_0:
	free(pstMdAttr);

PROC_FAIL_1:
	ar_hal_sys_mmz_free((AR_U64)stSegmapOut.astChannels[0].u32AddrPhy,
		(AR_VOID*)stSegmapOut.astChannels[0].uptrAddrVirt);

PROC_FAIL_2:
	ar_hal_sys_mmz_free((AR_U64)stSrc.astChannels[0].u32AddrPhy,
		(AR_VOID*)stSrc.astChannels[0].uptrAddrVirt);

	return NULL;
}


AR_S32 IPC_MD_Start()
{
    ProfileStart(__FUNCTION__);
	AR_S32 s32Ret = 0;

	isMdThreadStop=0;
	s32Ret = IPC_MID_UTIL_CreateDetachThread(IPC_MD_ThreadFunc, NULL, &pMdId),
	//pthread_create(&pMdId, NULL, IPC_MD_ThreadFunc, NULL);

    ProfileEnd(__FUNCTION__);

    return s32Ret;

}
void IPC_MD_Stop()
{
    //pthread_cancel(pMdId);
    //pthread_join(pMdId, NULL);
    isMdThreadStop =1;
}

void IPC_MD_Thread_Suspend()
{
	g_s32MdThreadSuspend = 1;
	printf("IPC_MD_Suspend \n");
}

void IPC_MD_Thread_Resume()
{
	g_s32MdThreadSuspend = 0;
	printf("IPC_MD_Resume \n");
}

