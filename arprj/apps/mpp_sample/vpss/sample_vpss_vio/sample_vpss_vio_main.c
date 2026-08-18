#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <signal.h>
#include "sample_comm.h"
#include "sample_vpss_vio_osd.h"
#include "sample_vpss_vio_vifile.h"
#include "ar_mipi_tx.h"
#include "hal_sys.h"
#include "sample_vpss_vio_config.h"


typedef enum arSAMPLE_VPSS_TEST_MASK_E
{
	VPSS_TEST_CROP = 1 << 0,
	VPSS_TEST_ROTATION	= 1 << 1,
	VPSS_TEST_MIRROR  = 1 << 2,
	VPSS_TEST_FLIP	= 1 << 3,
	VPSS_TEST_EXTCHAN = 1 << 4,
	VPSS_TEST_LDC = 1 << 5,
	VPSS_TEST_REGION_LUMA = 1 << 6,
	VPSS_TEST_THREAD_GET_FRAME = 1 << 7
} SAMPLE_VPSS_TEST_MASK_E;



#define TC1_ENABLE_VPSS   AR_TRUE


static AR_BOOL quit = AR_FALSE;

static const WDR_MODE_E enWDRMode = WDR_MODE_NONE;
static const DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
static const PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
static const VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
static const COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;


AR_S32 g_groupNum = 0;
AR_S32 g_channelNum = 0;
VPSS_GRP_ATTR_S g_astVpssGrpAttr[VPSS_MAX_GRP_NUM] = {0};
VPSS_CHN_ATTR_S g_aastVpssChnAttr[VPSS_MAX_GRP_NUM][VPSS_MAX_PHY_CHN_NUM] = {0};
VPSS_EXT_CHN_ATTR_S g_aastVpssExtChnAttr[VPSS_MAX_GRP_NUM][VPSS_MAX_CHN_NUM] = {0};     // only [VPSS_MAX_PHY_CHN_NUM, VPSS_MAX_CHN_NUM)



static AR_VOID SAMPLE_AR_MPI_VIN_OpenDev(AR_S32 mode)
{
	VI_DEV_PROP_S Prop={0};
	if(mode==0)
	{
		Prop.cam_mode=VIN_CAMERA_NORMAL;
	}
	else if(mode==1)
	{
		Prop.cam_mode=VIN_CAMERA_OFFLINE;
	}
	else if(mode==2)
	{
		Prop.cam_mode=VIN_CMAERA_MULTI_MODE;
	}
	else
	{
		Prop.cam_mode=VIN_CAMERA_OFFLINE;
	}

	//cfg the fre
	Prop.hdr_fre_mod=1;
	Prop.hdr_fre_hz=100000000;
	Prop.vif_fre_mod=1;
	Prop.vif_fre_hz=100000000;
	Prop.isp_fre_mod=1;
	Prop.isp_fre_hz=100000000;
	Prop.mipi_fre_mod=1;
	Prop.mipi_fre_hz=100000000;

	AR_MPI_VIN_OpenDev(&Prop);
	return;
}

static AR_VOID SAMPLE_Config_Vi(SAMPLE_VI_CONFIG_S *stViConfig, VI_PIPE ViPipe, VI_CHN ViChn, AR_S32 s32WorkSnsId)
{
	AR_S32			   s32ViCnt 	  = 1;
	VI_DEV			   ViDev		  = 0;

	SAMPLE_COMM_VI_GetSensorInfo(stViConfig);

	stViConfig->s32WorkingViNum 								  = s32ViCnt;
	stViConfig->as32WorkingViId[0]								  = 0;
	stViConfig->astViInfo[s32WorkSnsId].stSnsInfo.MipiDev		  = ViDev;
	stViConfig->astViInfo[s32WorkSnsId].stDevInfo.ViDev 		  = ViDev;
	stViConfig->astViInfo[s32WorkSnsId].stDevInfo.enWDRMode 	  = enWDRMode;
	stViConfig->astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
	stViConfig->astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] 	  = ViPipe;
	stViConfig->astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] 	  = -1;
	stViConfig->astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] 	  = -1;
	stViConfig->astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] 	  = -1;
	stViConfig->astViInfo[s32WorkSnsId].stChnInfo.ViChn 		  = ViChn;
	stViConfig->astViInfo[s32WorkSnsId].stChnInfo.enPixFormat	  = enPixFormat;
	stViConfig->astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange  = enDynamicRange;
	stViConfig->astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat   = enVideoFormat;
	stViConfig->astViInfo[s32WorkSnsId].stChnInfo.enCompressMode  = enCompressMode;
}

static AR_S32 SAMPLE_Config_Vb(SAMPLE_VI_CONFIG_S *stViConfig, SIZE_S *stSize)
{
	AR_S32			   s32Ret = AR_SUCCESS;

	VB_CONFIG_S 	   stVbConf;
	AR_U32			   u32BlkSize;

	ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
	stVbConf.u32MaxPoolCnt				= 2;

	u32BlkSize = COMMON_GetPicBufferSize(stSize->u32Width, stSize->u32Height, enPixFormat, DATA_BITWIDTH_8, enCompressMode, 256);
	stVbConf.astCommPool[0].u64BlkSize	= u32BlkSize;
	stVbConf.astCommPool[0].u32BlkCnt	= 10;

	u32BlkSize = VI_GetRawBufferSize(stSize->u32Width, stSize->u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, enCompressMode, 256);
	stVbConf.astCommPool[1].u64BlkSize	= u32BlkSize;
	stVbConf.astCommPool[1].u32BlkCnt	= 10;

	s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
	if (AR_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("system init failed with %d!\n", s32Ret);
		return s32Ret;
	}

	return s32Ret;
}

static AR_S32 SAMPLE_Config_VPSS_Tc0(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_CHN VpssExtChn, AR_BOOL *abChnEnable, AR_U32 mask)
{
	AR_S32			   s32Ret = AR_SUCCESS;
	VPSS_GRP_ATTR_S    stVpssGrpAttr = {0};
	VPSS_CHN_ATTR_S    astVpssChnAttr[VPSS_MAX_PHY_CHN_NUM] = {0};

	VPSS_EXT_CHN_ATTR_S stVpssExtChnAttr = {0};

	/*config vpss*/
	stVpssGrpAttr.stFrameRate.s32SrcFrameRate	 = -1;
	stVpssGrpAttr.stFrameRate.s32DstFrameRate	 = -1;
	stVpssGrpAttr.u32Width						 = 1280;
	stVpssGrpAttr.u32Height 					 = 720;

	stVpssGrpAttr.enCompressMode				 = enCompressMode;
	stVpssGrpAttr.enDynamicRange				 = enDynamicRange;
	stVpssGrpAttr.enVideoFormat 				 = enVideoFormat;
	stVpssGrpAttr.enPixelFormat 				 = enPixFormat;

	astVpssChnAttr[VpssChn].u32Width					= 1280;//stSize.u32Width;
	astVpssChnAttr[VpssChn].u32Height					= 720;//stSize.u32Height;
	astVpssChnAttr[VpssChn].enChnMode					= VPSS_CHN_MODE_USER;
	astVpssChnAttr[VpssChn].enCompressMode				= enCompressMode;
	astVpssChnAttr[VpssChn].enDynamicRange				= enDynamicRange;
	astVpssChnAttr[VpssChn].enVideoFormat				= enVideoFormat;
	astVpssChnAttr[VpssChn].enPixelFormat				= enPixFormat;
	astVpssChnAttr[VpssChn].stFrameRate.s32SrcFrameRate = 30;
	astVpssChnAttr[VpssChn].stFrameRate.s32DstFrameRate = 30;
	astVpssChnAttr[VpssChn].u32Depth					= 1;
	// add mirror test functioni
	if(mask & VPSS_TEST_MIRROR)
	{
		astVpssChnAttr[VpssChn].bMirror 					= AR_TRUE;
	}
	// add flip test function
	if(mask & VPSS_TEST_FLIP)
	{
		astVpssChnAttr[VpssChn].bFlip						= AR_TRUE;
	}
	astVpssChnAttr[VpssChn].stAspectRatio.enMode		= ASPECT_RATIO_NONE;
	abChnEnable[VpssChn] = AR_TRUE;

	// set ldc
	if(mask & VPSS_TEST_LDC)
	{
		astVpssChnAttr[VpssChn].stLdcAttr.bEnable = AR_TRUE;
		astVpssChnAttr[VpssChn].stLdcAttr.stAttr.ldc_k0 = -0.4501;
		astVpssChnAttr[VpssChn].stLdcAttr.stAttr.ldc_k1 = 0.2584;
		astVpssChnAttr[VpssChn].stLdcAttr.stAttr.ldc_k2 = -0.0927;
		astVpssChnAttr[VpssChn].stLdcAttr.stAttr.k[0]=1367.9;
		astVpssChnAttr[VpssChn].stLdcAttr.stAttr.k[1]=0;
		astVpssChnAttr[VpssChn].stLdcAttr.stAttr.k[2]=962.29;
		astVpssChnAttr[VpssChn].stLdcAttr.stAttr.k[3]=0;
		astVpssChnAttr[VpssChn].stLdcAttr.stAttr.k[4]=1367.7;
		astVpssChnAttr[VpssChn].stLdcAttr.stAttr.k[5]=513.1752;
		astVpssChnAttr[VpssChn].stLdcAttr.stAttr.k[6]=0;
		astVpssChnAttr[VpssChn].stLdcAttr.stAttr.k[7]=0;
		astVpssChnAttr[VpssChn].stLdcAttr.stAttr.k[8]=1;
	}

	// set scale mode
	if(mask & VPSS_TEST_CROP)
	{
		astVpssChnAttr[VpssChn].u32ScaleMode				= 0;
		SAMPLE_PRT("set scale mode %d === !\n", astVpssChnAttr[VpssChn].u32ScaleMode);
	}

	s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (AR_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("start vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	//add ext channel
	if(mask & VPSS_TEST_EXTCHAN)
	{
		stVpssExtChnAttr.s32BindChn = VpssChn;
		memcpy(&stVpssExtChnAttr.enChnMode, (const VPSS_CHN_ATTR_S *)&astVpssChnAttr[VpssChn].enChnMode, sizeof(VPSS_CHN_ATTR_S));
		s32Ret = AR_MPI_VPSS_SetExtChnAttr(VpssGrp, VpssExtChn, &stVpssExtChnAttr);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("AR_MPI_VPSS_SetExtChnAttr failed. s32Ret: 0x%x !\n", s32Ret);
			return s32Ret;
		}

		s32Ret = AR_MPI_VPSS_EnableChn(VpssGrp, VpssExtChn);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("AR_MPI_VPSS_EnableChn failed with %#x\n", s32Ret);
			return AR_FAILURE;
		}
	}

	//add crop test function
	if(mask & VPSS_TEST_CROP)
	{
		VPSS_CROP_INFO_S stCropInfo = {
			.bEnable	= AR_TRUE,
			.enCropCoordinate = VPSS_CROP_ABS_COOR,
			.stCropRect = {
				.s32X		= 0,
				.s32Y		= 0,
				.u32Width	= 640,
				.u32Height	= 480,
			},
		};

		s32Ret = AR_MPI_VPSS_SetChnCrop(VpssGrp, VpssChn, &stCropInfo);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("AR_MPI_VPSS_SetChnCrop failed. s32Ret: 0x%x !\n", s32Ret);
			return s32Ret;
		}

		SAMPLE_PRT("AR_MPI_VPSS_SetChnCrop pass.=== !\n");
	}

	// add rotation test function
	if(mask & VPSS_TEST_ROTATION)
	{
		ROTATION_E rotation = ROTATION_90;

		s32Ret = AR_MPI_VPSS_SetChnRotation(VpssGrp, VpssChn, rotation);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("AR_MPI_VPSS_SetChnRotation failed. s32Ret: 0x%x !\n", s32Ret);
			return s32Ret;
		}

		SAMPLE_PRT("AR_MPI_VPSS_SetChnRotation %d pass.=== !\n", rotation);
	}

	return s32Ret;
}

static AR_VOID SAMPLE_Config_Vo(SAMPLE_VO_CONFIG_S *stVoConfig, AR_U32 u32VoIntfType, PIC_SIZE_E enPicSize)
{
	RECT_S			   stDefDispRect  = {0, 0, 1280, 720};
	SIZE_S			   stDefImageSize = {1280, 720};

	SAMPLE_COMM_VO_GetDefConfig(stVoConfig);

	stVoConfig->VoDev									 = SAMPLE_VO_DEV_DHD0;
	stVoConfig->enIntfSync								 = VO_OUTPUT_1080P60;
	stVoConfig->enPicSize								 = enPicSize;
	stVoConfig->u32DisBufLen							 = 3;
	stVoConfig->enDstDynamicRange						 = enDynamicRange;
	stVoConfig->enVoMode								 = VO_MODE_1MUX;
	stVoConfig->stDispRect								 = stDefDispRect;
	stVoConfig->stImageSize 							 = stDefImageSize;

	if (1 == u32VoIntfType)
	{
		stVoConfig->enVoIntfType = VO_INTF_BT1120;
	}
	else
	{
		stVoConfig->enVoIntfType = VO_INTF_HDMI;
	}
}

static AR_S32 SAMPLE_VPSS_Run_Rgnluma(VPSS_GRP VpssGrp, VPSS_CHN VpssChn)
{
	AR_S32			   s32Ret;
	VIDEO_REGION_INFO_S stRegionInfo = {0};
	AR_U64* 		   pu64LumaData = NULL;

	stRegionInfo.u32RegionNum = 2;
	stRegionInfo.pstRegion = malloc(stRegionInfo.u32RegionNum * sizeof(*stRegionInfo.pstRegion));
	if(stRegionInfo.pstRegion == NULL)
	{
		SAMPLE_PRT("ar_malloc failed!\n");
		return AR_FAILURE;
	}

	stRegionInfo.pstRegion[0].s32X = 32;
	stRegionInfo.pstRegion[0].s32Y = 32;
	stRegionInfo.pstRegion[0].u32Width = 128;
	stRegionInfo.pstRegion[0].u32Height = 128;

	stRegionInfo.pstRegion[1].s32X = 320;
	stRegionInfo.pstRegion[1].s32Y = 320;
	stRegionInfo.pstRegion[1].u32Width = 160;
	stRegionInfo.pstRegion[1].u32Height = 160;

	pu64LumaData = malloc(stRegionInfo.u32RegionNum * sizeof(AR_U64));
	if(pu64LumaData == NULL)
	{
		SAMPLE_PRT("ar_malloc failed!\n");
		if(stRegionInfo.pstRegion)
		{
			free(stRegionInfo.pstRegion);
		}
		return AR_FAILURE;
	}

	memset(pu64LumaData, 0, stRegionInfo.u32RegionNum * sizeof(*pu64LumaData));
	s32Ret = AR_MPI_VPSS_GetRegionLuma(VpssGrp, VpssChn, &stRegionInfo, pu64LumaData, 1000);
	if (AR_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("AR_MPI_VPSS_GetRegionLuma failed. s32Ret: 0x%x !\n", s32Ret);
	}
	free(pu64LumaData);
	free(stRegionInfo.pstRegion);

	return s32Ret;
}

static int AR_SAMPLE_VPSS_ipc_callback(uint32_t cmd, ar_ipc_data_t* data, unsigned len, void* reply)
{
    AR_S32 s32Ret = AR_FAILURE;
    SAMPLE_VPSS_REPLY_S replyRet;

    if(data == AR_NULL){
        printf("AR_SAMPLE_VPSS_ipc_callback data is NULL\n");
        goto OUT;
    }

    SAMPLE_VPSS_CONFIG_S *pstParam = (SAMPLE_VPSS_CONFIG_S *)data->blob;
    VPSS_GRP grpIndex = pstParam->grpId;
    VPSS_CHN chnIndex = pstParam->chnId;
    if(grpIndex < 0 || grpIndex >= g_groupNum){
        printf("AR_SAMPLE_VPSS_ipc_callback group index %d doesn't exist, range [0, %d]\n", grpIndex, g_groupNum - 1);
        goto OUT;
    }
    if(chnIndex >= g_channelNum){
        printf("AR_SAMPLE_VPSS_ipc_callback channel index %d doesn't exist, range [0, %d]\n", chnIndex, g_channelNum - 1);
        goto OUT;
    }

	printf("grp,chn=(%d,%d)\n",grpIndex,chnIndex);

	if(chnIndex < 0)
	{
		s32Ret = AR_MPI_VPSS_GetGrpAttr(grpIndex, &g_astVpssGrpAttr[grpIndex]);
		if (s32Ret != AR_SUCCESS)
		{
			printf("AR_MPI_VPSS_GetGrpAttr failed with %#x\n", s32Ret);
			goto OUT;
		}
	}
	else
	{
	    s32Ret = AR_MPI_VPSS_GetChnAttr(grpIndex, chnIndex, &g_aastVpssChnAttr[grpIndex][chnIndex]);
	    if (s32Ret != AR_SUCCESS)
	    {
	    	printf("AR_MPI_VPSS_GetChnAttr failed with %#x\n", s32Ret);
	    	goto OUT;
	    }
    }

	s32Ret = AR_FAILURE;
    switch (cmd)
    {
        case CMD_VPSS_CONFIG_SCALE:
            printf("+++++++++ CMD_VPSS_CONFIG_SCALE\n");
            if(chnIndex < 0) {
                g_astVpssGrpAttr[grpIndex].u32Width = pstParam->grpAttr.u32Width;
                g_astVpssGrpAttr[grpIndex].u32Height = pstParam->grpAttr.u32Height;
            }
            else if(chnIndex < VPSS_MAX_PHY_CHN_NUM){
                g_aastVpssChnAttr[grpIndex][chnIndex].u32Width = pstParam->chnAttr.u32Width;
                g_aastVpssChnAttr[grpIndex][chnIndex].u32Height = pstParam->chnAttr.u32Height;
            } else if(chnIndex < VPSS_MAX_CHN_NUM){
                g_aastVpssExtChnAttr[grpIndex][chnIndex].s32BindChn = pstParam->extChnAttr.s32BindChn;
                g_aastVpssExtChnAttr[grpIndex][chnIndex].u32Width = pstParam->extChnAttr.u32Width;
                g_aastVpssExtChnAttr[grpIndex][chnIndex].u32Height = pstParam->extChnAttr.u32Height;
            } else {
                printf("chnIndex %d must be an existing channel.\n", chnIndex);
				goto OUT;
            }
            break;
        case CMD_VPSS_CONFIG_MIRROR:
            printf("+++++++++ CMD_VPSS_CONFIG_MIRROR %d:%d\n",pstParam->grpAttr.bMirror,pstParam->chnAttr.bMirror);
            if(chnIndex < 0) {
                g_astVpssGrpAttr[grpIndex].bMirror = pstParam->grpAttr.bMirror;
            }
            else if(chnIndex < VPSS_MAX_PHY_CHN_NUM){
                g_aastVpssChnAttr[grpIndex][chnIndex].bMirror = pstParam->chnAttr.bMirror;
            } else if(chnIndex < VPSS_MAX_CHN_NUM){
                g_aastVpssExtChnAttr[grpIndex][chnIndex].s32BindChn = pstParam->extChnAttr.s32BindChn;
                g_aastVpssExtChnAttr[grpIndex][chnIndex].bMirror = pstParam->extChnAttr.bMirror;
            } else {
                printf("chnIndex %d must be an existing channel.\n", chnIndex);
				goto OUT;
            }
            break;
        case CMD_VPSS_CONFIG_FLIP:
            printf("+++++++++ CMD_VPSS_CONFIG_FLIP\n");
            if(chnIndex < 0) {
                g_astVpssGrpAttr[grpIndex].bFlip = pstParam->grpAttr.bFlip;
            }
            else if(chnIndex < VPSS_MAX_PHY_CHN_NUM){
                g_aastVpssChnAttr[grpIndex][chnIndex].bFlip = pstParam->chnAttr.bFlip;
            } else if(chnIndex < VPSS_MAX_CHN_NUM){
                g_aastVpssExtChnAttr[grpIndex][chnIndex].s32BindChn = pstParam->extChnAttr.s32BindChn;
                g_aastVpssExtChnAttr[grpIndex][chnIndex].bFlip = pstParam->extChnAttr.bFlip;
            } else {
                printf("chnIndex %d must be an existing channel.\n", chnIndex);
				goto OUT;
            }
            break;
        case CMD_VPSS_CONFIG_ROTATE:
            printf("+++++++++ CMD_VPSS_CONFIG_ROTATE\n");
            if(chnIndex < 0) {
                g_astVpssGrpAttr[grpIndex].enRotation = pstParam->grpAttr.enRotation;
            }
            else if(chnIndex >= 0 && chnIndex < VPSS_MAX_PHY_CHN_NUM){
                g_aastVpssChnAttr[grpIndex][chnIndex].enRotation = pstParam->rotateAngle;
            } else if(chnIndex < VPSS_MAX_CHN_NUM){
                g_aastVpssExtChnAttr[grpIndex][chnIndex].enRotation = pstParam->rotateAngle;
            } else {
                printf("chnIndex %d must be an existing channel.\n", chnIndex);
                goto OUT;
            }
            break;
        case CMD_VPSS_CONFIG_PHY_CHN_LDC:
            printf("+++++++++ CMD_VPSS_CONFIG_PHY_CHN_LDC\n");
            if(chnIndex < 0) {
                g_astVpssGrpAttr[grpIndex].stLdcAttr.bEnable = pstParam->ldcAttr.bEnable;
            }
            else if(chnIndex >= 0 && chnIndex < VPSS_MAX_PHY_CHN_NUM){
                g_aastVpssChnAttr[grpIndex][chnIndex].stLdcAttr.bEnable = pstParam->ldcAttr.bEnable;
            } else {
                printf("chnIndex %d must be an existing physical channel.\n", chnIndex);
                goto OUT;
            }
            break;
        case CMD_VPSS_CONFIG_PHY_CHN_SPREAD:
            printf("+++++++++ CMD_VPSS_CONFIG_PHY_CHN_SPREAD\n");
            if(chnIndex < 0) {
                g_astVpssGrpAttr[grpIndex].stSpreadAttr.bEnable = pstParam->spreadAttr.bEnable;
            }
            else if(chnIndex >= 0 && chnIndex < VPSS_MAX_PHY_CHN_NUM){
                g_aastVpssChnAttr[grpIndex][chnIndex].stSpreadAttr.bEnable = pstParam->spreadAttr.bEnable;
            } else {
                SAMPLE_PRT("chnIndex %d must be an existing physical channel.\n", chnIndex);
                goto OUT;
            }
            break;
        default:
            printf("Unknown command %d\n", cmd);
            goto OUT;
    }
	s32Ret = AR_SUCCESS;

    switch (cmd)
    {
        case CMD_VPSS_CONFIG_SCALE:
        case CMD_VPSS_CONFIG_MIRROR:
        case CMD_VPSS_CONFIG_FLIP:
        case CMD_VPSS_CONFIG_CROP:
        case CMD_VPSS_CONFIG_ROTATE:        
			if(chnIndex < 0)
			{
				AR_MPI_VPSS_SetGrpAttr(grpIndex, &g_astVpssGrpAttr[grpIndex]);
			}
            else if(chnIndex < VPSS_MAX_PHY_CHN_NUM)
            {
                AR_MPI_VPSS_SetChnAttr(grpIndex, chnIndex, &g_aastVpssChnAttr[grpIndex][chnIndex]);
            }
            else
            {
                AR_MPI_VPSS_SetExtChnAttr(grpIndex, chnIndex, &g_aastVpssExtChnAttr[grpIndex][chnIndex]);
            }
            break;
    }

OUT:
    replyRet.s32Ret = s32Ret;
    if(s32Ret == AR_SUCCESS){
        strcpy(replyRet.result, "SUCCESS");
    } else {
        strcpy(replyRet.result, "FAIL");
    }
    ar_ipc_return_buffer(reply, s32Ret, &replyRet, sizeof(SAMPLE_VPSS_REPLY_S));

    return 0;
}


static ar_ipc_state_t* AR_SAMPLE_create_ipc_server()
{
    int ret;
    ar_ipc_state_t* ipc = ar_ipc_create();
    if (!ipc) {
        SAMPLE_PRT("failed to create server ipc");
        return NULL;
    }

    ret = ar_ipc_add_service(ipc, "vpss_control", AR_SAMPLE_VPSS_ipc_callback);
    if (ret) {
        SAMPLE_PRT("failed to publish service vpss_control\n");
        return NULL;
    }

    ar_ipc_start_thread_pool(ipc);

    return ipc;
}

static AR_S32 SAMPLE_VIO_ViOfflineVpssOfflineRoute(AR_U32 u32VoIntfType, AR_BOOL u32UseVpss)
{
	AR_S32			   s32Ret;

	VI_PIPE 		   ViPipe		  = 0;
	VI_CHN			   ViChn		  = 0;
	AR_S32			   s32WorkSnsId   = 0;
	SAMPLE_VI_CONFIG_S stViConfig;

	SIZE_S			   stSize;
	PIC_SIZE_E		   enPicSize;

	VO_CHN			   VoChn		  = 0;
	SAMPLE_VO_CONFIG_S stVoConfig;

	VPSS_GRP		   VpssGrp		  = 0;
	VPSS_CHN		   VpssChn		  = VPSS_CHN0;
	AR_BOOL 		   abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {0};
	AR_U32			   VpssTestMask   =  0;  //VPSS_TEST_CROP | VPSS_TEST_ROTATION | VPSS_TEST_MIRROR | VPSS_TEST_FLIP | VPSS_TEST_EXTCHAN | VPSS_TEST_LDC;
	VPSS_CHN		   VpssExtChn = 5;

    g_groupNum = VpssGrp +1;
    g_channelNum = VpssChn +1;

	/*config vi*/
	SAMPLE_Config_Vi(&stViConfig, ViPipe, ViChn, s32WorkSnsId);

	/*get picture size*/
	s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
	if (AR_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("get picture size by sensor failed!\n");
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
	if (AR_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("get picture size failed!\n");
		return s32Ret;
	}
	printf("%s %d size=(%d, %d)\n", __func__, __LINE__, stSize.u32Width, stSize.u32Height);

	/*config vb*/
	s32Ret = SAMPLE_Config_Vb(&stViConfig, &stSize);
	if (AR_SUCCESS != s32Ret)
	{
		return s32Ret;
	}

    ar_ipc_state_t* ipc = NULL;
    ipc = AR_SAMPLE_create_ipc_server();
    if (ipc) {
        SAMPLE_PRT("AR_SAMPLE_VPSS_create_ipc_server Success.\n");
    } else {
        SAMPLE_PRT("AR_SAMPLE_VPSS_create_ipc_server Failed!\n");
    }

	/*start vi*/
	SAMPLE_AR_MPI_VIN_OpenDev(0);
	s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
	if (AR_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("start vi failed. s32Ret:0x%x !\n", s32Ret);
		goto TC0_EXIT;
	}

	if(u32UseVpss)
	{
		/* config & start vpss */
		s32Ret = SAMPLE_Config_VPSS_Tc0(VpssGrp, VpssChn, VpssExtChn, abChnEnable, VpssTestMask);
		if (AR_SUCCESS != s32Ret)
		{
			goto TC0_EXIT1;
		}

		s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe, ViChn, VpssGrp);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("vi bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
			goto TC0_EXIT2;
		}
	}

	/*config vo*/
	SAMPLE_Config_Vo(&stVoConfig, u32VoIntfType, enPicSize);

	/*start vo*/
	s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);
	if (AR_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("start vo failed. s32Ret: 0x%x !\n", s32Ret);
		goto TC0_EXIT5;
	}

	if(u32UseVpss)
	{
		/*vpss bind vo*/
		if(VpssTestMask & VPSS_TEST_EXTCHAN)
		{
			s32Ret = SAMPLE_COMM_VPSS_Bind_VO(VpssGrp, VpssExtChn, stVoConfig.VoDev, VoChn);
		}
		else
		{
			s32Ret = SAMPLE_COMM_VPSS_Bind_VO(VpssGrp, VpssChn, stVoConfig.VoDev, VoChn);
		}
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("vo bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
			goto TC0_EXIT6;
		}

		if(VpssTestMask & VPSS_TEST_REGION_LUMA)
		{
			SAMPLE_VPSS_Run_Rgnluma(VpssGrp, VpssChn);
		}
	}
	else
	{
		s32Ret = SAMPLE_COMM_VI_Bind_VO(ViPipe, ViChn, stVoConfig.VoDev, VoChn);
	}

	while(!quit)
	{
		usleep(5000000);
	}

TC0_EXIT7:
	if(u32UseVpss)
	{
		if(VpssTestMask & VPSS_TEST_EXTCHAN)
		{
			SAMPLE_COMM_VPSS_UnBind_VO(VpssGrp, VpssExtChn, stVoConfig.VoDev, VoChn);
		}
		else
		{
			SAMPLE_COMM_VPSS_UnBind_VO(VpssGrp, VpssChn, stVoConfig.VoDev, VoChn);
		}
	}
	else
	{
		SAMPLE_COMM_VI_UnBind_VO(ViPipe, ViChn, stVoConfig.VoDev, VoChn);
	}
TC0_EXIT6:
	SAMPLE_COMM_VO_StopVO(&stVoConfig);
TC0_EXIT5:
	if(u32UseVpss)
	{
		SAMPLE_COMM_VI_UnBind_VPSS(ViPipe, ViChn, VpssGrp);
	}
TC0_EXIT2:
	if(u32UseVpss)
	{
		SAMPLE_COMM_VPSS_Stop(VpssGrp, abChnEnable);
	}
TC0_EXIT1:
	SAMPLE_COMM_VI_StopVi(&stViConfig);
TC0_EXIT:
    ar_ipc_stop_thread_pool(ipc);
    ar_ipc_close(ipc);
	SAMPLE_COMM_SYS_Exit();
	return s32Ret;
}

static AR_S32 SAMPLE_Config_VPSS_Tc1(VPSS_GRP VpssGrp, VPSS_CHN VpssBindChn, VPSS_CHN VpssChnToVo, VPSS_CHN VpssExtChn, AR_BOOL *abChnEnable)
{
	AR_S32			   s32Ret = AR_SUCCESS;
	VPSS_GRP_ATTR_S    stVpssGrpAttr = {0};
	VPSS_CHN_ATTR_S    astVpssChnAttr[VPSS_MAX_PHY_CHN_NUM] = {0};
	VPSS_CHN		   VpssChn	   = 0;
	VPSS_EXT_CHN_ATTR_S stVpssExtChnAttr = {0};

	s32Ret = SAMPLE_VPSS_Init_Osd(VpssGrp, VpssChnToVo);
	if(AR_SUCCESS != s32Ret)
	{
		return AR_FAILURE;
	}

	/*config vpss*/
	stVpssGrpAttr.stFrameRate.s32SrcFrameRate	 = 30;
	stVpssGrpAttr.stFrameRate.s32DstFrameRate	 = 30;
	stVpssGrpAttr.u32Width						 = 1280;
	stVpssGrpAttr.u32Height 					 = 720;

	stVpssGrpAttr.enCompressMode				 = enCompressMode;
	stVpssGrpAttr.enDynamicRange				 = enDynamicRange;
	stVpssGrpAttr.enVideoFormat 				 = enVideoFormat;
	stVpssGrpAttr.enPixelFormat 				 = enPixFormat;

	for(VpssChn=0; VpssChn<VPSS_MAX_PHY_CHN_NUM; VpssChn++)
	{
		if(abChnEnable[VpssChn])
		{
			astVpssChnAttr[VpssChn].u32Width					= 1280;
			astVpssChnAttr[VpssChn].u32Height					= 720;
			astVpssChnAttr[VpssChn].enChnMode					= VPSS_CHN_MODE_USER;
			astVpssChnAttr[VpssChn].enCompressMode				= enCompressMode;
			astVpssChnAttr[VpssChn].enDynamicRange				= enDynamicRange;
			astVpssChnAttr[VpssChn].enVideoFormat				= enVideoFormat;
			astVpssChnAttr[VpssChn].enPixelFormat				= enPixFormat;
			astVpssChnAttr[VpssChn].stFrameRate.s32SrcFrameRate = 30;
			astVpssChnAttr[VpssChn].stFrameRate.s32DstFrameRate = 30;
			astVpssChnAttr[VpssChn].u32Depth					= 1;
			astVpssChnAttr[VpssChn].stAspectRatio.enMode		= ASPECT_RATIO_NONE;
			SAMPLE_VPSS_Add_Rgn(VpssGrp, VpssChn);
		}
	}

	s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (AR_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("SAMPLE_COMM_VPSS_Start failed. s32Ret: 0x%x\n", s32Ret);
		return s32Ret;
	}

	if(abChnEnable[VpssBindChn] && (VpssExtChn>=VPSS_MAX_PHY_CHN_NUM && VpssExtChn<VPSS_MAX_CHN_NUM))
	{
		stVpssExtChnAttr.s32BindChn = VpssBindChn;
		memcpy(&stVpssExtChnAttr.enChnMode, (const VPSS_CHN_ATTR_S *)&astVpssChnAttr[VpssBindChn].enChnMode, sizeof(VPSS_CHN_ATTR_S));
		s32Ret = AR_MPI_VPSS_SetExtChnAttr(VpssGrp, VpssExtChn, &stVpssExtChnAttr);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("AR_MPI_VPSS_SetExtChnAttr failed. s32Ret: 0x%x !\n", s32Ret);
			return s32Ret;
		}

		s32Ret = AR_MPI_VPSS_EnableChn(VpssGrp, VpssExtChn);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("AR_MPI_VPSS_EnableChn failed with 0x%x\n", s32Ret);
			return AR_FAILURE;
		}
	}
	else
	{
		SAMPLE_PRT("ext chn not enabled, phy %d ext %d\n", VpssBindChn, VpssExtChn);
	}

	return s32Ret;
}

static AR_VOID SAMPLE_VPSS_Stop_Tc1(VPSS_GRP VpssGrp, AR_BOOL* pabChnEnable)
{
	VPSS_CHN		   VpssChn	   = 0;

	for(VpssChn=0; VpssChn<VPSS_MAX_PHY_CHN_NUM; VpssChn++)
	{
		if(pabChnEnable[VpssChn])
		{
			SAMPLE_VPSS_Remove_Rgn(VpssGrp, VpssChn);
		}
	}

	SAMPLE_COMM_VPSS_Stop(VpssGrp, pabChnEnable);
	SAMPLE_VPSS_UnInit_Osd();
}

/*
 * vin - vpss - vo
 * vpss:
 *	  group0 - ch0 - ch4
 *			 - ch1
 */
static AR_S32 SAMPLE_VPSS_OSD_Test(AR_VOID)
{
	AR_S32 s32Ret = AR_SUCCESS;

	VI_PIPE 		   ViPipe		  = 0;
	VI_CHN			   ViChn		  = 0;
	AR_S32			   s32WorkSnsId   = 0;
	SAMPLE_VI_CONFIG_S stViConfig;

	SIZE_S			   stSize;
	PIC_SIZE_E		   enPicSize;

	VO_CHN			   VoChn		  = 0;
	SAMPLE_VO_CONFIG_S stVoConfig;

	VPSS_GRP		   VpssGrp		  = 0;
	VPSS_CHN		   VpssBindChn	  = VPSS_CHN0;
	VPSS_CHN		   VpssExtChn	  = VPSS_INVALID_CHN;
	AR_BOOL 		   abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {
		[VPSS_CHN0] = AR_TRUE,
		[VPSS_CHN1] = AR_TRUE,
	};
	VPSS_CHN		   VpssChnToVo	  = VPSS_CHN0;
//	VPSS_CHN		   VpssChnToVo	  = VpssExtChn;

	/*config vi*/
	SAMPLE_Config_Vi(&stViConfig, ViPipe, ViChn, s32WorkSnsId);

	/*get picture size*/
	s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
	if (AR_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("get picture size by sensor failed!\n");
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
	if (AR_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("get picture size failed!\n");
		return s32Ret;
	}

	/*config vb*/
	s32Ret = SAMPLE_Config_Vb(&stViConfig, &stSize);
	if (AR_SUCCESS != s32Ret)
	{
		return s32Ret;
	}

	/*start vi*/
	SAMPLE_AR_MPI_VIN_OpenDev(0);
	s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
	if (AR_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("start vi failed. s32Ret:0x%x !\n", s32Ret);
		goto TC1_EXIT;
	}

	/* config & start vpss */
	s32Ret = SAMPLE_Config_VPSS_Tc1(VpssGrp, VpssBindChn, VpssChnToVo, VpssExtChn, abChnEnable);
	if (AR_SUCCESS != s32Ret)
	{
		goto TC1_EXIT1;
	}

	s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe, ViChn, VpssGrp);
	if (AR_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("vi bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
		goto TC1_EXIT2;
	}

	/*config vo*/
	SAMPLE_Config_Vo(&stVoConfig, 0, enPicSize);

	/*start vo*/
	s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);
	if (AR_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("start vo failed. s32Ret: 0x%x !\n", s32Ret);
		goto TC1_EXIT5;
	}

	/*vpss bind vo*/
	s32Ret = SAMPLE_COMM_VPSS_Bind_VO(VpssGrp, VpssChnToVo, stVoConfig.VoDev, VoChn);
	if (AR_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("vo bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
		goto TC1_EXIT6;
	}

	while(!quit)
	{
		usleep(5000000);
	}

	SAMPLE_COMM_VPSS_UnBind_VO(VpssGrp, VpssChnToVo, stVoConfig.VoDev, VoChn);
TC1_EXIT6:
	SAMPLE_COMM_VO_StopVO(&stVoConfig);
TC1_EXIT5:
	SAMPLE_COMM_VI_UnBind_VPSS(ViPipe, ViChn, VpssGrp);
TC1_EXIT2:
	SAMPLE_VPSS_Stop_Tc1(VpssGrp, abChnEnable);
TC1_EXIT1:
	SAMPLE_COMM_VI_StopVi(&stViConfig);
TC1_EXIT:
	SAMPLE_COMM_SYS_Exit();
	return s32Ret;
}

static AR_S8 yuvFile[32] = "/mnt/test.yuv";

static AR_S32 SAMPLE_VIO_ViFileVpssOfflineRoute(AR_U32 u32VoIntfType, AR_BOOL u32UseVpss)
{
	AR_S32			   s32Ret;

	VI_PIPE 		   ViPipe		  = 0;
	VI_CHN			   ViChn		  = 0;
	FILE*              ViInFd          = NULL;
	SAMPLE_VI_CONFIG_S stViConfig;

	VO_CHN			   VoChn		  = 0;
	SAMPLE_VO_CONFIG_S stVoConfig;

	VPSS_GRP		   VpssGrp		  = 0;
	VPSS_CHN		   VpssChn		  = VPSS_CHN0;
	AR_BOOL 		   abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {0};
	AR_U32			   VpssTestMask   = 0;
	VPSS_CHN		   VpssExtChn = 5;

	AR_S32			   inWidth		  = 1920;
	AR_S32             inHeight       = 1080;
	AR_S32             frameRate      = 25;

	SIZE_S			   stSize = { .u32Width = inWidth, .u32Height = inHeight };

    g_groupNum = VpssGrp +1;
    g_channelNum = VpssChn +1;


    ar_ipc_state_t* ipc = NULL;
    ipc = AR_SAMPLE_create_ipc_server();
    if (ipc) {
        SAMPLE_PRT("AR_SAMPLE_VPSS_create_ipc_server Success.\n");
    } else {
        SAMPLE_PRT("AR_SAMPLE_VPSS_create_ipc_server Failed!\n");
    }

	/*config vb*/
	s32Ret = SAMPLE_Config_Vb(&stViConfig, &stSize);
	if (AR_SUCCESS != s32Ret)
	{
		return s32Ret;
	}

	/*start vi*/
//	SAMPLE_AR_MPI_VIN_OpenDev(0);
    ViInFd = fopen(yuvFile, "rb");
    if (!ViInFd)
	{
		SAMPLE_PRT("open yuv file %s failed!\n", yuvFile);
		goto TC2_EXIT;
    }
	SAMPLE_PRT("open yuv file %s success!\n", yuvFile);

	config_vin_data(ViInFd, inWidth, inHeight, frameRate);

	if(u32UseVpss)
	{
		/* config & start vpss */
		s32Ret = SAMPLE_Config_VPSS_Tc0(VpssGrp, VpssChn, VpssExtChn, abChnEnable, VpssTestMask);
		if (AR_SUCCESS != s32Ret)
		{
			goto TC2_EXIT2;
		}

		s32Ret = SAMPLE_VPSS_VIFILE_VI_Bind_VPSS(ViPipe, ViChn, VpssGrp, VpssChn);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("vi bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
			goto TC2_EXIT2;
		}
	}

	/*config vo*/
	SAMPLE_Config_Vo(&stVoConfig, u32VoIntfType, PIC_720P);

	/*start vo*/
	s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);
	if (AR_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("start vo failed. s32Ret: 0x%x !\n", s32Ret);
		goto TC2_EXIT5;
	}

	if(u32UseVpss)
	{
		/*vpss bind vo*/
		if(VpssTestMask & VPSS_TEST_EXTCHAN)
		{
			s32Ret = SAMPLE_COMM_VPSS_Bind_VO(VpssGrp, VpssExtChn, stVoConfig.VoDev, VoChn);
		}
		else
		{
			s32Ret = SAMPLE_COMM_VPSS_Bind_VO(VpssGrp, VpssChn, stVoConfig.VoDev, VoChn);
		}
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("vo bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
			goto TC2_EXIT6;
		}

		if(VpssTestMask & VPSS_TEST_REGION_LUMA)
		{
			s32Ret = SAMPLE_VPSS_Run_Rgnluma(VpssGrp, VpssChn);
			if (AR_SUCCESS != s32Ret)
			{
				goto TC2_EXIT7;
			}
		}
	}
	else
	{
		s32Ret = SAMPLE_VPSS_VIFILE_VI_Bind_VO(ViPipe, ViChn, stVoConfig.VoDev, VoChn);
	}

	while(!quit && !feof(ViInFd))
	{
		usleep(5000000);
	}

TC2_EXIT7:
	if(u32UseVpss)
	{
		if(VpssTestMask & VPSS_TEST_EXTCHAN)
		{
			SAMPLE_COMM_VPSS_UnBind_VO(VpssGrp, VpssExtChn, stVoConfig.VoDev, VoChn);
		}
		else
		{
			SAMPLE_COMM_VPSS_UnBind_VO(VpssGrp, VpssChn, stVoConfig.VoDev, VoChn);
		}
	}
	else
	{
		SAMPLE_VPSS_VIFILE_VI_Unbind_VO(ViPipe, ViChn, stVoConfig.VoDev, VoChn);
	}
TC2_EXIT6:
	SAMPLE_COMM_VO_StopVO(&stVoConfig);
TC2_EXIT5:
	if(u32UseVpss)
	{
		SAMPLE_VPSS_VIFILE_VI_Unbind_VPSS(ViPipe, ViChn, VpssGrp, VpssChn);
	}
TC2_EXIT2:
	if(u32UseVpss)
	{
		SAMPLE_COMM_VPSS_Stop(VpssGrp, abChnEnable);
	}
TC2_EXIT1:
	if (ViInFd)
	{
		fclose(ViInFd);
		ViInFd = NULL;
	}
TC2_EXIT:
	ar_ipc_stop_thread_pool(ipc);
	ar_ipc_close(ipc);
	SAMPLE_COMM_SYS_Exit();
	return s32Ret;
}

typedef struct
{
	VPSS_GRP VpssGrp;
	VPSS_CHN VpssChn;
} sample_vpss_chn_st;

static const AR_BOOL auto_repeat = AR_TRUE;
static const AR_BOOL join_get_frame_thread_at_repeat = AR_FALSE;

static AR_BOOL stop_tc3 = AR_FALSE;

static void* SAMPLE_VPSS_GetChnFrame(void* args)
{
	sample_vpss_chn_st *chn_st = (sample_vpss_chn_st*)args;
	VIDEO_FRAME_INFO_S video_frame;
	AR_S32 s32Ret = 0;

	while(!quit && !stop_tc3)
	{
		s32Ret = AR_MPI_VPSS_GetChnFrame(chn_st->VpssGrp, chn_st->VpssChn, &video_frame, -1);
		if(s32Ret)
		{
			SAMPLE_PRT("get chn frame failed 0x%x\n", s32Ret);
			break;
		}
		SAMPLE_PRT("get chn frame %llx\n", video_frame.stVFrame.u64PhyAddr[0]);
		s32Ret = AR_MPI_VPSS_ReleaseChnFrame(chn_st->VpssGrp, chn_st->VpssChn, &video_frame);
		if(s32Ret)
		{
			SAMPLE_PRT("release chn frame failed 0x%x\n", s32Ret);
			break;
		}
	}

	if(quit || stop_tc3)
	{
		ar_thread_exit();
	}

	return (void*)s32Ret;
}

static ar_thread_id_t SAMPLE_VPSS_Create_Get_Frame_Thread(VPSS_GRP VpssGrp, VPSS_CHN VpssChn)
{
	sample_vpss_chn_st chn_st =
	{
		.VpssGrp = VpssGrp,
		.VpssChn = VpssChn
	};
	return ar_thread_new(SAMPLE_VPSS_GetChnFrame, &chn_st, sizeof(chn_st), NULL);
}

static void SAMPLE_VPSS_Join_Get_Frame_Thread(ar_thread_id_t tid)
{
	ar_thread_terminate(tid);
}

static AR_S32 SAMPLE_VPSS_Mt_Test(AR_U32 u32VoIntfType, AR_BOOL u32UseVpss, int repeat_cnt_max)
{
	AR_S32			   s32Ret;

	VI_PIPE 		   ViPipe		  = 0;
	VI_CHN			   ViChn		  = 0;
	AR_S32			   s32WorkSnsId   = 0;
	SAMPLE_VI_CONFIG_S stViConfig;

	SIZE_S			   stSize;
	PIC_SIZE_E		   enPicSize;

	VO_CHN			   VoChn		  = 0;
	SAMPLE_VO_CONFIG_S stVoConfig;

	VPSS_GRP		   VpssGrp		  = 0;
	VPSS_CHN		   VpssChn		  = VPSS_CHN0;
	AR_BOOL 		   abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {0};
	AR_U32			   VpssTestMask   = VPSS_TEST_THREAD_GET_FRAME;
	VPSS_CHN		   VpssExtChn = 5;

	ar_thread_id_t     tid = NULL;

	AR_U32             repeat_cnt = 0;

    g_groupNum = VpssGrp +1;
    g_channelNum = VpssChn +1;

	/*config vi*/
	SAMPLE_Config_Vi(&stViConfig, ViPipe, ViChn, s32WorkSnsId);

	/*get picture size*/
	s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
	if (AR_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("get picture size by sensor failed!\n");
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
	if (AR_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("get picture size failed!\n");
		return s32Ret;
	}

	/*config vb*/
	s32Ret = SAMPLE_Config_Vb(&stViConfig, &stSize);
	if (AR_SUCCESS != s32Ret)
	{
		return s32Ret;
	}

    ar_ipc_state_t* ipc = NULL;
    ipc = AR_SAMPLE_create_ipc_server();
    if (ipc) {
        SAMPLE_PRT("AR_SAMPLE_VPSS_create_ipc_server Success.\n");
    } else {
        SAMPLE_PRT("AR_SAMPLE_VPSS_create_ipc_server Failed!\n");
    }

	SAMPLE_AR_MPI_VIN_OpenDev(0);

TC3_REPEART:
	SAMPLE_PRT("\n----------%d----------\n", repeat_cnt);

	/*start vi*/
	s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
	if (AR_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("start vi failed. s32Ret:0x%x !\n", s32Ret);
		goto TC3_EXIT;
	}

	if(u32UseVpss)
	{
		/* config & start vpss */
		s32Ret = SAMPLE_Config_VPSS_Tc0(VpssGrp, VpssChn, VpssExtChn, abChnEnable, VpssTestMask);
		if (AR_SUCCESS != s32Ret)
		{
			goto TC3_EXIT1;
		}

		s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe, ViChn, VpssGrp);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("vi bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
			goto TC3_EXIT2;
		}
	}

	/*config vo*/
	SAMPLE_Config_Vo(&stVoConfig, u32VoIntfType, enPicSize);

	/*start vo*/
	s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);
	if (AR_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("start vo failed. s32Ret: 0x%x !\n", s32Ret);
		goto TC3_EXIT5;
	}

	if(u32UseVpss)
	{
		/*vpss bind vo*/
		if(VpssTestMask & VPSS_TEST_EXTCHAN)
		{
			s32Ret = SAMPLE_COMM_VPSS_Bind_VO(VpssGrp, VpssExtChn, stVoConfig.VoDev, VoChn);
		}
		else
		{
			s32Ret = SAMPLE_COMM_VPSS_Bind_VO(VpssGrp, VpssChn, stVoConfig.VoDev, VoChn);
		}
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("vo bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
			goto TC3_EXIT6;
		}

		if(VpssTestMask & VPSS_TEST_REGION_LUMA)
		{
			SAMPLE_VPSS_Run_Rgnluma(VpssGrp, VpssChn);
		}

		if(repeat_cnt == 0)
		{
			if(VpssTestMask & VPSS_TEST_THREAD_GET_FRAME)
			{
				tid = SAMPLE_VPSS_Create_Get_Frame_Thread(VpssGrp, VpssChn);
			}
		}
	}
	else
	{
		s32Ret = SAMPLE_COMM_VI_Bind_VO(ViPipe, ViChn, stVoConfig.VoDev, VoChn);
	}

	while(!quit)
	{
		if(auto_repeat)
		{
			if(join_get_frame_thread_at_repeat || repeat_cnt==repeat_cnt_max)
			{
				stop_tc3 = AR_TRUE;
			}

			usleep(5000000);
			break;
		}
		else
		{
			if(getchar() == 'r')
			{
				stop_tc3 = AR_TRUE;
				break;
			}
			usleep(1000000);
		}
	}

TC3_EXIT7:
	if(u32UseVpss)
	{
		if(quit || stop_tc3)
		{
			if(VpssTestMask & VPSS_TEST_THREAD_GET_FRAME)
			{
				SAMPLE_VPSS_Join_Get_Frame_Thread(tid);
			}
		}

		if(VpssTestMask & VPSS_TEST_EXTCHAN)
		{
			SAMPLE_COMM_VPSS_UnBind_VO(VpssGrp, VpssExtChn, stVoConfig.VoDev, VoChn);
		}
		else
		{
			SAMPLE_COMM_VPSS_UnBind_VO(VpssGrp, VpssChn, stVoConfig.VoDev, VoChn);
		}
	}
	else
	{
		SAMPLE_COMM_VI_UnBind_VO(ViPipe, ViChn, stVoConfig.VoDev, VoChn);
	}
TC3_EXIT6:
	SAMPLE_COMM_VO_StopVO(&stVoConfig);
TC3_EXIT5:
	if(u32UseVpss)
	{
		SAMPLE_COMM_VI_UnBind_VPSS(ViPipe, ViChn, VpssGrp);
	}
TC3_EXIT2:
	if(u32UseVpss)
	{
		SAMPLE_COMM_VPSS_Stop(VpssGrp, abChnEnable);
	}
TC3_EXIT1:
	SAMPLE_COMM_VI_StopVi(&stViConfig);

	if(!quit && repeat_cnt < repeat_cnt_max)
	{
		repeat_cnt++;
		goto TC3_REPEART;
	}
TC3_EXIT:
	ar_ipc_stop_thread_pool(ipc);
	ar_ipc_close(ipc);
	SAMPLE_COMM_SYS_Exit();
	return s32Ret;
}


static AR_VOID sigint_handler(int signum)
{
	SAMPLE_PRT("recive signal %d\n", signum);
	quit = AR_TRUE;
}

static void usage(char* argv0)
{
	SAMPLE_PRT("Usage:\n");
	SAMPLE_PRT("%s\n", argv0);
	SAMPLE_PRT("Options:\n");
	SAMPLE_PRT("	-i	<int>\n");
	SAMPLE_PRT("			testcase index, default 0\n");
	SAMPLE_PRT("	-h	<No arg>\n");
	SAMPLE_PRT("			Show help message\n");
}

int main(int argc, char *argv[])
{
	int ret = 0;
	int opt = -1;
	int tc_id = 0;
	int repeat_cnt_max;
	AR_BOOL enable_vpss = TC1_ENABLE_VPSS;

	while((opt=getopt(argc, argv, "i:e:r:h")) != -1)
	{
		switch (opt)
		{
			case 'i':
				tc_id = atoi(optarg);
				break;
			case 'e':
				enable_vpss = (atoi(optarg) == 0 ? AR_FALSE : AR_TRUE);
				break;
			case 'r':
				repeat_cnt_max = atoi(optarg);
				if(repeat_cnt_max <= 0)
					repeat_cnt_max = 100;
				break;
			case 'h':
			default:
				usage(argv[0]);
				exit(1);
		}
	}

	signal(SIGINT, sigint_handler);

	switch(tc_id)
	{
		case 0:
			ret = SAMPLE_VIO_ViOfflineVpssOfflineRoute(0, enable_vpss);
			break;
		case 1:
			ret = SAMPLE_VPSS_OSD_Test();
			break;
		case 2:
			ret = SAMPLE_VIO_ViFileVpssOfflineRoute(0, enable_vpss);
			break;
		case 3:
			ret = SAMPLE_VPSS_Mt_Test(0, enable_vpss, repeat_cnt_max);
			break;
		default:
			break;
	}
	return ret;
}
