#include "cfg_all.h"
#include "util_common.h"
#include "util_api.h"
#include "recmng_rec.h"
#include "ipc_od.h"
#include "cfg_od.h"
#include "pf_od.h"
#include "ar_ive.h"
#include "mpi_vpss.h"
#include "hal_sys.h"
#include "mpi_scaler_api.h"

pthread_t pOdId;
static AR_U32 isOdThreadStop;
static AR_S32 g_s32OdThreadSuspend = 0;
static AR_S32 IPC_OD_Acquire_Stream(AR_S32 VpssGrp, AR_S32 VpssChn, OD_ATTR_S *pstOdAttr)
{
	VIDEO_FRAME_INFO_S VFrameInfo = {0};
	AR_S32 ret=AR_MPI_VPSS_GetChnFrame(VpssGrp, VpssChn, &VFrameInfo, -1);
	if(ret<0)
	{
		PRINT_ERR("AR_MPI_VPSS_GetChnFrame ERR ret=%x\r\n", ret);
		return AR_FAILURE;
	}

	pstOdAttr->u32Width = RESIZED_WIDTH;
	pstOdAttr->u32Height = RESIZED_HEIGHT;

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

static void *IPC_OD_ThreadFunc(void *arg)
{
    usleep(5000000);
	
    IPC_CFG_OD_S * pOdCfg = IPC_CFG_OD_GetParam();
	
	AR_U32 VpssGrp = pOdCfg->u32GrpId;
	AR_U32 VpssChn = pOdCfg->u32ChnId;
	VIDEO_FRAME_INFO_S VFrameInfo = {0};
	AR_S32 ret=AR_MPI_VPSS_GetChnFrame(VpssGrp, VpssChn, &VFrameInfo, -1);
	if(ret<0)
	{
		PRINT_ERR("AR_MPI_VPSS_GetChnFrame ERR ret=%x\r\n", ret);
		return NULL;
	}

	AR_U32 u32StreamSize = VFrameInfo.stVFrame.u32Width*VFrameInfo.stVFrame.u32Height;

	ret = AR_MPI_VPSS_ReleaseChnFrame(VpssGrp, VpssChn, &VFrameInfo);
	if(ret < 0)
	{
		PRINT_ERR("AR_MPI_VPSS_ReleaseChnFrame ERR ret=%x.\n",ret);
		return NULL;
	}


	OD_ATTR_S * pstOdAttr = (OD_ATTR_S *)malloc(sizeof(OD_ATTR_S));
	if(!pstOdAttr)
	{
	   PRINT_ERR("malloc pstOdAttr err \r\n");
	   return NULL;
	}

	pstOdAttr->u32AlarmBlockNumThred =	  pOdCfg->u32AlarmBlockNumThred;

    char tmp[30] ={0};
	sprintf(tmp, "ODStream_%d_%d", VpssGrp,VpssChn);
	ret = ar_hal_sys_mmz_alloc(&pstOdAttr->u64StreamPa, &pstOdAttr->pStreamVa, tmp, NULL, u32StreamSize);
	if(ret)
	{
		PRINT_ERR("Malloc Stream error.\n");
		free(pstOdAttr);
		return NULL;
	}


	struct timeval tm;
	AR_S32 OdChn = VpssGrp*IPC_MAX_STREAM_PER_PIPE + VpssChn;
	ret = AR_IVE_OD_CreateChn(OdChn,pstOdAttr);
	if(ret<0)
	{
		PRINT_ERR("AR_IVE_OD_CreateChn err... \r\n");
		ar_hal_sys_mmz_free(pstOdAttr->u64StreamPa,pstOdAttr->pStreamVa);
		free(pstOdAttr);
		return NULL;
	}

	AR_U32 u32DetectCnt =0;
	AR_U32 u32DetectCntThread = 5;
	while(!isOdThreadStop)
	{
        if(g_s32OdThreadSuspend)
        {
            usleep(500 * 1000);
            continue;
        }
	
		gettimeofday(&tm, NULL);
		AR_U64 u64Tic = tm.tv_sec * 1000000 + tm.tv_usec;

		pOdCfg = IPC_CFG_OD_GetParam();
        if(!pOdCfg->bEnable)
        {
            usleep(500 * 1000);
            continue;
        }

		ret= IPC_OD_Acquire_Stream(VpssGrp,VpssChn,pstOdAttr);
		if(ret<0)
		{
			PRINT_ERR("[%d][%d]acquire live stream err... \r\n",VpssGrp,VpssChn);
			continue;
		}

		ret = AR_IVE_OD_Process(OdChn,pstOdAttr);
		if(ret>0)
		{
			u32DetectCnt++;
			if(u32DetectCnt>=u32DetectCntThread)
			{
				gettimeofday(&tm, NULL);
				AR_U64 u64Toc = tm.tv_sec * 1000000 + tm.tv_usec;
				PRINT_INFO("[%d][%d]Occlusion Detected, Taskes %f ms...\r\n",VpssGrp,VpssChn,(u64Toc-u64Tic)*1.0/1000);
				u32DetectCnt = 0;
			}
		}

	}

	AR_IVE_OD_DestroyChn(OdChn);
	ar_hal_sys_mmz_free(pstOdAttr->u64StreamPa,pstOdAttr->pStreamVa);
	free(pstOdAttr);

	return NULL;
}

AR_S32 IPC_OD_Start()
{
    ProfileStart(__FUNCTION__);
	AR_S32 s32Ret = 0;
	
	isOdThreadStop=0;
	s32Ret = IPC_MID_UTIL_CreateDetachThread(IPC_OD_ThreadFunc, NULL, &pOdId),
	//pthread_create(&pOdId, NULL, IPC_OD_ThreadFunc, NULL);

    ProfileEnd(__FUNCTION__);

    return s32Ret;
}

void IPC_OD_Stop()
{
    //pthread_cancel(pOdId);
    //pthread_join(pOdId, NULL);
	isOdThreadStop = 1;
}

void IPC_OD_Thread_Suspend()
{
	g_s32OdThreadSuspend = 1;
	printf("IPC_OD_Suspend.\n");
}

void IPC_OD_Thread_Resume()
{
	g_s32OdThreadSuspend = 0;
	printf("IPC_OD_Resume.\n");
}

