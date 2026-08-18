#include "sample_comm.h"
#include "osal.h"
#include "hal_sys.h"
#include "sample_vpss_vio_vifile.h"

struct VinPara
{
	FILE* fd;
	AR_S32 width;
	AR_S32 height;
	AR_S32 frameRate;
};

static struct VinPara vinData = {0};

AR_VOID config_vin_data(FILE* fd, AR_S32 width, AR_S32 height, AR_S32 frameRate)
{
	vinData.fd = fd;
	vinData.width = width;
	vinData.height = height;
	vinData.frameRate = frameRate;
}

static AR_S32 VinGetFrame(AR_S32 s32_dev_id, AR_S32 s32_chn_id,  AR_VOID *pv_data, AR_VOID *pv_priv_data)
{
	AR_S32 s32Ret = AR_SUCCESS;
	AR_S32 s32RetTmp = AR_SUCCESS;

	VB_BLK blk = VB_INVALID_HANDLE;
	VB_CAL_CONFIG_S stCalConfig = {0};
	VB_POOL poolId = VB_INVALID_POOLID;
	AR_U8* pDst = NULL;
	AR_U32 u32Row = 0;
	struct VinPara *data = (struct VinPara *)pv_priv_data;
	VIDEO_FRAME_INFO_S *stFrame = (VIDEO_FRAME_INFO_S *)pv_data;

	if(!data->fd || feof(data->fd))
	{
		return AR_FAILURE;
	}

	COMMON_GetPicBufferConfig(data->width, data->height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, 0, 64, &stCalConfig);

	blk = AR_MPI_VB_GetBlock(VB_INVALID_POOLID, stCalConfig.u32VBSize, NULL);
	if (blk == VB_INVALID_HANDLE)
	{
		SAMPLE_PRT("AR_MPI_VB_GetBlock failed %d\n", stCalConfig.u32VBSize);
		return blk;
	}

	poolId = AR_MPI_VB_Handle2PoolId(blk);
	if (poolId < 0)
	{
		SAMPLE_PRT("AR_MPI_VB_Handle2PoolId failed %x\n", poolId);
		goto out;
	}

	s32Ret = AR_MPI_VB_MmapPool(poolId);
	if (s32Ret)
	{
		SAMPLE_PRT("AR_MPI_VB_MmapPool failed, s32Ret = 0x%x\n", s32Ret);
		goto out;
	}

	memset(stFrame, 0, sizeof(*stFrame));
	stFrame->stVFrame.u64PhyAddr[0] = AR_MPI_VB_Handle2PhysAddr(blk);

	s32Ret = AR_MPI_VB_GetBlockVirAddr(poolId, stFrame->stVFrame.u64PhyAddr[0], (AR_VOID**)&stFrame->stVFrame.u64VirAddr[0]);
	if (s32Ret)
	{
		SAMPLE_PRT("AR_MPI_VB_GetBlockVirAddr failed, s32Ret = 0x%x\n", s32Ret);
		goto out;
	}

	stFrame->stVFrame.enCompressMode = COMPRESS_MODE_NONE;
	stFrame->stVFrame.enPixelFormat = PIXEL_FORMAT_YVU_PLANAR_420;
	stFrame->stVFrame.u32Width = data->width;
	stFrame->stVFrame.u32Height = data->height;
	stFrame->stVFrame.u32Stride[0] = stCalConfig.u32MainStride;
	stFrame->stVFrame.u32Stride[1] = stCalConfig.u32MainStride / 2;
	stFrame->stVFrame.u32Stride[2] = stCalConfig.u32MainStride / 2;
	stFrame->stVFrame.u32TimeRef = 0;
	stFrame->stVFrame.u64PTS = 0;
	stFrame->u32PoolId = VB_INVALID_POOLID;
	stFrame->enModId = AR_ID_VI;

	stFrame->stVFrame.u64PhyAddr[1] = stFrame->stVFrame.u64PhyAddr[0] + stCalConfig.u32MainYSize;
	stFrame->stVFrame.u64VirAddr[1] = stFrame->stVFrame.u64VirAddr[0] + stCalConfig.u32MainYSize;

	stFrame->stVFrame.u64PhyAddr[2] = stFrame->stVFrame.u64PhyAddr[1] + stCalConfig.u32MainYSize / 4;
	stFrame->stVFrame.u64VirAddr[2] = stFrame->stVFrame.u64VirAddr[1] + stCalConfig.u32MainYSize / 4;

    pDst = (AR_U8*)(AR_U64)stFrame->stVFrame.u64VirAddr[0];
    for ( u32Row = 0; u32Row < data->height; u32Row++ )
	{
		if (fread(pDst, 1, data->width, data->fd) != data->width)
		{
			SAMPLE_PRT("Read frame Y failed!\n");
			s32Ret = AR_FAILURE;
			goto out;
		}
		pDst += stFrame->stVFrame.u32Stride[0];
    }

    pDst = (AR_U8*)(AR_U64)stFrame->stVFrame.u64VirAddr[1];
    for ( u32Row = 0; u32Row < data->height / 2; u32Row++ )
	{
		if (fread(pDst, 1, data->width / 2, data->fd) != data->width / 2)
		{
			SAMPLE_PRT("Read frame U failed!\n");
			s32Ret = AR_FAILURE;
			goto out;
		}
		pDst += stFrame->stVFrame.u32Stride[1];
    }

    pDst = (AR_U8*)(AR_U64)stFrame->stVFrame.u64VirAddr[2];
    for ( u32Row = 0; u32Row < data->height / 2; u32Row++ )
	{
		if (fread(pDst, 1, data->width / 2, data->fd) != data->width / 2)
		{
			SAMPLE_PRT("Read frame V failed!\n");
			s32Ret = AR_FAILURE;
			goto out;
		}
		pDst += stFrame->stVFrame.u32Stride[2];
    }

	s32Ret = AR_MPI_VB_MunmapPool(poolId);
	if (s32Ret)
	{
		SAMPLE_PRT("AR_MPI_VB_MunmapPool failed, s32Ret = 0x%x\n", s32Ret);
		goto out;
	}

	usleep((1000 / data->frameRate) * 1000);

	return AR_SUCCESS;

out:
	s32RetTmp = AR_MPI_VB_ReleaseBlock(blk);
	if (s32RetTmp)
	{
		SAMPLE_PRT("AR_MPI_VB_ReleaseBlock failed, s32RetTmp = 0x%x\n", s32RetTmp);
	}

	return s32Ret;
}

static AR_S32 VinReleaseFrame(AR_S32 s32_dev_id, AR_S32 s32_chn_id, const AR_VOID *pv_data, AR_VOID *pv_priv_data)
{
	AR_S32 s32Ret = AR_SUCCESS;
	VB_BLK blk = VB_INVALID_HANDLE;
	VIDEO_FRAME_INFO_S *stFrame = (VIDEO_FRAME_INFO_S *)pv_data;

	blk = AR_MPI_VB_PhysAddr2Handle(stFrame->stVFrame.u64PhyAddr[0]);
	if (blk == VB_INVALID_HANDLE)
	{
		SAMPLE_PRT("AR_MPI_VB_PhysAddr2Handle failed\n");
		return blk;
	}

	s32Ret = AR_MPI_VB_ReleaseBlock(blk);
	if (s32Ret)
	{
		SAMPLE_PRT("AR_MPI_VB_ReleaseBlock failed, s32Ret=0x%x\n", s32Ret);
		return s32Ret;
	}

	return AR_SUCCESS;
}

AR_S32 SAMPLE_VPSS_VIFILE_VI_Bind_VPSS(VI_PIPE ViPipe, VI_CHN ViChn, VPSS_GRP VpssGrp, VPSS_CHN VpssChn)
{
	AR_S32 s32Ret = AR_SUCCESS;
	STRU_SYS_BIND_SENDER sender;
	MPP_CHN_S stSrcChn;
	MPP_CHN_S stDstChn;

	sender.e_mod_id = AR_SYS_ID_VI;
	sender.u32_max_dev_cnt = 1;
	sender.u32_max_chn_cnt = 4;
	sender.s32_flag = 0;
	sender.e_data_type = AR_SYS_DATA_VI_FRAME;
	sender.pv_priv_data = (AR_VOID*)&vinData;
	sender.give_bind_call_back = NULL;
	sender.get_frame_call_back = VinGetFrame;
	sender.release_frame_call_back = VinReleaseFrame;

	s32Ret = ar_hal_sys_bind_register_sender(&sender);
	if (s32Ret)
	{
		SAMPLE_PRT("ar_hal_sys_bind_register_sender failed, s32Ret: 0x%x\n", s32Ret);
		return s32Ret;
	}

	stSrcChn.enModId = AR_ID_VI;
	stSrcChn.s32DevId = ViPipe;
	stSrcChn.s32ChnId = ViChn;

	stDstChn.enModId = AR_ID_VPSS;
	stDstChn.s32DevId = VpssGrp;
	stDstChn.s32ChnId = VpssChn;

	s32Ret = AR_MPI_SYS_Bind(&stSrcChn, &stDstChn);
	if (s32Ret)
	{
		SAMPLE_PRT("AR_MPI_SYS_Bind failed, s32Ret: 0x%x\n", s32Ret);
		return s32Ret;
	}

	return AR_SUCCESS;
}

AR_S32 SAMPLE_VPSS_VIFILE_VI_Unbind_VPSS(VI_PIPE ViPipe, VI_CHN ViChn, VPSS_GRP VpssGrp, VPSS_CHN VpssChn)
{
	AR_S32 s32Ret = AR_SUCCESS;
	MPP_CHN_S stSrcChn;
	MPP_CHN_S stDstChn;

	stSrcChn.enModId = AR_ID_VI;
	stSrcChn.s32DevId = ViPipe;
	stSrcChn.s32ChnId = ViChn;

	stDstChn.enModId = AR_ID_VPSS;
	stDstChn.s32DevId = VpssGrp;
	stDstChn.s32ChnId = VpssChn;

	s32Ret = AR_MPI_SYS_UnBind(&stSrcChn, &stDstChn);
	if (s32Ret)
	{
		SAMPLE_PRT("AR_MPI_SYS_UnBind failed, s32Ret: 0x%x\n", s32Ret);
	}

    return s32Ret;
}

AR_S32 SAMPLE_VPSS_VIFILE_VI_Bind_VO(VI_PIPE ViPipe, VI_CHN ViChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
	AR_S32 s32Ret = AR_SUCCESS;
	STRU_SYS_BIND_SENDER sender;
	MPP_CHN_S stSrcChn;
	MPP_CHN_S stDstChn;

	sender.e_mod_id = AR_SYS_ID_VI;
	sender.u32_max_dev_cnt = 1;
	sender.u32_max_chn_cnt = 4;
	sender.s32_flag = 0;
	sender.e_data_type = AR_SYS_DATA_VI_FRAME;
	sender.pv_priv_data = (AR_VOID*)&vinData;
	sender.give_bind_call_back = NULL;
	sender.get_frame_call_back = VinGetFrame;
	sender.release_frame_call_back = VinReleaseFrame;

	s32Ret = ar_hal_sys_bind_register_sender(&sender);
	if (s32Ret)
	{
		SAMPLE_PRT("ar_hal_sys_bind_register_sender failed, s32Ret: 0x%x\n", s32Ret);
		return s32Ret;
	}

	stSrcChn.enModId = AR_ID_VI;
	stSrcChn.s32DevId = ViPipe;
	stSrcChn.s32ChnId = ViChn;

	stDstChn.enModId = AR_ID_VO;
	stDstChn.s32DevId = VoLayer;
	stDstChn.s32ChnId = VoChn;

	s32Ret = AR_MPI_SYS_Bind(&stSrcChn, &stDstChn);
	if (s32Ret)
	{
		SAMPLE_PRT("AR_MPI_SYS_Bind failed, s32Ret: 0x%x\n", s32Ret);
		return s32Ret;
	}

	return AR_SUCCESS;
}

AR_S32 SAMPLE_VPSS_VIFILE_VI_Unbind_VO(VI_PIPE ViPipe, VI_CHN ViChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
	AR_S32 s32Ret = AR_SUCCESS;
	MPP_CHN_S stSrcChn;
	MPP_CHN_S stDstChn;

	stSrcChn.enModId = AR_ID_VI;
	stSrcChn.s32DevId = ViPipe;
	stSrcChn.s32ChnId = ViChn;

	stDstChn.enModId = AR_ID_VO;
	stDstChn.s32DevId = VoLayer;
	stDstChn.s32ChnId = VoChn;

	s32Ret = AR_MPI_SYS_UnBind(&stSrcChn, &stDstChn);
	if (s32Ret)
	{
		SAMPLE_PRT("AR_MPI_SYS_UnBind failed, s32Ret: 0x%x\n", s32Ret);
	}

    return s32Ret;
}

