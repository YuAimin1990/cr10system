#include "../sample_vio.h"

/* log marco */
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
// Recommended use ar_printf to print log for AR api

#define log_info(format, ...) ar_printf("\033[0m[ INFO]<%s: %s():%d> " format "\033[0m", \
                                        __FILENAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define log_err(format, ...) ar_printf("\033[0;31m[ERROR]<%s: %s():%d> " format "\033[0m", \
                                       __FILENAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__)

// #define __DBG
#ifdef __DBG
#define log_dbg(format, ...) ar_printf("\033[0;32m[DEBUG]<%s: %s():%d> " format "\033[0m", \
                                       __FILENAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define log_dbg(format, ...)
#endif

/* log marco end */

extern volatile AR_S8 g_exit;
extern ISP_SNS_OBJ_S stSnsImx290Obj;

#define SENSOR_NUM 2
typedef struct
{
    AR_BOOL Run;

    AR_S32  DevId;
    AR_S32  ChnId;

    AR_BOOL bSendToVo;
    AR_S32  VoLayerId;
    AR_S32  VoChnId;

    AR_BOOL bSendToVpss;
    AR_S32  VpssGrpId;

    pthread_t tid;
} SAMPLE_FRAME_PROCESS_T;

static VO_DSI_ATTR_S pstDsiCfg =
{
    .u32Lane = {1, 1, 1, 1},
    .u32BitsPerPixel = 24,
    .stSyncInfo = {
        .u32Hsa = 6,
        .u32Hbp = 39,
        .u32Hact = 720,
        .u32Hfp = 39,
        .u32Vsa = 4,
        .u32Vbp = 21,
        .u32Vact = 1440,
        .u32Vfp = 17},
    .fDphyClkMhz = 443.88864,
    .u32PllFreqReg2c0 = 0x240B8A22,
    .u32PllFreqReg38c = 0x2800
};

SAMPLE_FRAME_PROCESS_T ViPipe0Ch0Thr = {0};
SAMPLE_FRAME_PROCESS_T ViPipe1Ch0Thr = {0};
SAMPLE_FRAME_PROCESS_T VpssGrp0ChnThr[2] = {0};
SAMPLE_FRAME_PROCESS_T VpssGrp1ChnThr[2] = {0};

AR_S32 sample_vo_init_st7703_720x1440(VO_DEV VoDev, VO_LAYER VoLayer)
{
    VO_PUB_ATTR_S stPubAttr = {0};
    VO_VIDEO_LAYER_ATTR_S stLayerAttr = {0};
    VO_USER_INTFSYNC_INFO_S stUserInfo = {0};
    AR_U32 u32Framerate = 0;
    SIZE_S stDevSize = {0};

    /* SET VO PUB ATTR OF USER TYPE */
    stPubAttr.u32BgColor = COLOR_RGB_BLUE;
    stPubAttr.stSyncInfo.u16Hmid = 1;
    stPubAttr.stSyncInfo.u16Bvact = 1;
    stPubAttr.stSyncInfo.u16Bvbb = 1;
    stPubAttr.stSyncInfo.u16Bvfb = 1;

    /* USER SET VO FRAME RATE */
    u32Framerate = 60;
    CHECK_RET(AR_MPI_VO_SetDevFrameRate(VoDev, u32Framerate), "AR_MPI_VO_SetDevFrameRate");

    /* USER SET VO DEV SYNC INFO */
    stPubAttr.enIntfSync = VO_OUTPUT_USER;
    stPubAttr.stSyncInfo.bSynm = AR_TRUE;
    stPubAttr.stSyncInfo.bIop = AR_TRUE;
    stPubAttr.stSyncInfo.u8Intfb = 0;
    stPubAttr.stSyncInfo.u16Hpw = 8;
    stPubAttr.stSyncInfo.u16Hbb = 52;
    stPubAttr.stSyncInfo.u16Hact = 720;
    stPubAttr.stSyncInfo.u16Hfb = 52;
    stPubAttr.stSyncInfo.u16Vpw = 4;
    stPubAttr.stSyncInfo.u16Vbb = 21;
    stPubAttr.stSyncInfo.u16Vact = 1440;
    stPubAttr.stSyncInfo.u16Vfb = 17;
    stPubAttr.stSyncInfo.u16Hmid = 0;
    stPubAttr.stSyncInfo.u16Bvact = 0;
    stPubAttr.stSyncInfo.u16Bvbb = 0;
    stPubAttr.stSyncInfo.u16Bvfb = 0;
    stPubAttr.stSyncInfo.bIdv = AR_FALSE;
    stPubAttr.stSyncInfo.bIhs = AR_FALSE;
    stPubAttr.stSyncInfo.bIvs = AR_FALSE;
    stPubAttr.enIntfType = VO_INTF_MIPI;

    CHECK_RET(AR_MPI_VO_SetPubAttr(VoDev, &stPubAttr), "AR_MPI_VO_SetPubAttr");

    /* USER SET VO SYNC INFO OF USER INTF */
    stUserInfo.bClkReverse = AR_TRUE;
    stUserInfo.u32DevDiv = 1;
    stUserInfo.u32PreDiv = 1;
    stUserInfo.stUserIntfSyncAttr.enClkSource = VO_CLK_SOURCE_PLL;
    stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Fbdiv = 73;
    stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Frac = 0x3D75E2;
    stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Refdiv = 2;
    stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Postdiv1 = 3;
    stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Postdiv2 = 2;

    CHECK_RET(AR_MPI_VO_SetUserIntfSyncInfo(VoDev, &stUserInfo), "AR_MPI_VO_SetUserIntfSyncInfo");

    /* ENABLE VO DEV */
    CHECK_RET(AR_MPI_VO_Enable(VoDev), "AR_MPI_VO_Enable");

    /* SET LCD */
    CHECK_RET(AR_MPI_VO_Dsi_SetAttr(VoDev, &pstDsiCfg), "AR_MPI_VO_Dsi_SetAttr");

    CHECK_RET(AR_MPI_VO_Dsi_Enable(VoDev), "AR_MPI_VO_Dsi_Enable");

    /*SET VO LAYER ATTR*/
    stDevSize.u32Width = stPubAttr.stSyncInfo.u16Hact;
    stDevSize.u32Height = stPubAttr.stSyncInfo.u16Vact;

    stLayerAttr.bClusterMode = AR_FALSE;
    stLayerAttr.bDoubleFrame = AR_FALSE;
    stLayerAttr.enDstDynamicRange = DYNAMIC_RANGE_SDR8;
    stLayerAttr.enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;

    stLayerAttr.stDispRect.s32X = 0;
    stLayerAttr.stDispRect.s32Y = 0;
    stLayerAttr.stDispRect.u32Height = stDevSize.u32Height;
    stLayerAttr.stDispRect.u32Width = stDevSize.u32Width;

    stLayerAttr.stImageSize.u32Height = stDevSize.u32Height;
    stLayerAttr.stImageSize.u32Width = stDevSize.u32Width;
    printf("x%d y%d h%d w%d imgH%d imgW%d \n", stLayerAttr.stDispRect.s32X, stLayerAttr.stDispRect.s32Y,
    stLayerAttr.stDispRect.u32Height, stLayerAttr.stDispRect.u32Width,
    stLayerAttr.stImageSize.u32Height, stLayerAttr.stImageSize.u32Width);

    stLayerAttr.u32DispFrmRt = 60;

    CHECK_RET(AR_MPI_VO_SetVideoLayerAttr(VoLayer, &stLayerAttr), "AR_MPI_VO_SetVideoLayerAttr");

    /* ENABLE VO LAYER */
    CHECK_RET(AR_MPI_VO_EnableVideoLayer(VoLayer), "AR_MPI_VO_EnableVideoLayer");
}

AR_S32 sample_vo_chn_enable(VO_LAYER VoLayer, VO_CHN VoChn, STRU_AR_HAL_VO_RECT *VoChnPosition)
{
    VO_CHN_ATTR_S astChnAttr = {0};

    astChnAttr.bDeflicker = AR_FALSE;
    astChnAttr.u32Priority = 0;
    astChnAttr.stRect.s32X = VoChnPosition->x;
    astChnAttr.stRect.s32Y = VoChnPosition->y;
    astChnAttr.stRect.u32Height = VoChnPosition->h;
    astChnAttr.stRect.u32Width = VoChnPosition->w;
    CHECK_RET(AR_MPI_VO_SetChnAttr(VoLayer, VoChn, &astChnAttr), "AR_MPI_VO_SetChnAttr");
    CHECK_RET(AR_MPI_VO_EnableChn(VoLayer, VoChn), "AR_MPI_VO_EnableChn");

    return AR_SUCCESS;
}

/*
    VI: chn0: 1920x1080 --> VPSS Group: 1920x1080 -> VPSS Chn 0: 1920x1080 & VPSS Chn 1: 1280x720
*/
AR_S32 sample_sys_init(SRTU_SENSOR_DEFAULT_ATTR_T astDefSnsAttr[], AR_BOOL abViRawChnEnable[], AR_U32 SensorNum)
{
    AR_S32 s32Ret = 0;
    VB_CONFIG_S stVbConf = {0};
    AR_U32 u32BlkSize = 0;

    /* Config VB for Vi Chn0 and Chn2 */
    for (int i = 0; i < SensorNum; i++)
    {
        SIZE_S stSize = {0};
        stSize.u32Width = astDefSnsAttr[i].stPubAttr.stSnsSize.u32Width;
        stSize.u32Height = astDefSnsAttr[i].stPubAttr.stSnsSize.u32Height;

        // vi ch0 yuv
        u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height,
            PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
        log_dbg("%dx%d blk size %d\n", stSize.u32Width, stSize.u32Height, u32BlkSize);
        stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u64BlkSize = u32BlkSize;
        stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt = 6;
        stVbConf.u32MaxPoolCnt++;

        if (abViRawChnEnable[i])
        {
            // vi raw
            u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height,
                PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
            log_dbg("%dx%d blk size %d\n", stSize.u32Width, stSize.u32Height, u32BlkSize + 8192);
            stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u64BlkSize = u32BlkSize + 8192;
            stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt = 6;
            stVbConf.u32MaxPoolCnt++;
        }
        // VPSS Chn 0
        u32BlkSize = CAM_ALIGNE_TO(stSize.u32Width, 64) * CAM_ALIGNE_TO(stSize.u32Height, 64) * 3 /2;
        log_dbg("%dx%d blk size %d\n", stSize.u32Width, stSize.u32Height, u32BlkSize);
        stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u64BlkSize = u32BlkSize;
        stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt = 6;
        stVbConf.u32MaxPoolCnt++;
    }

    // VPSS Chn 1
    u32BlkSize = CAM_ALIGNE_TO(1280, 64) * CAM_ALIGNE_TO(720, 64) * 3 /2;
    log_dbg("%dx%d blk size %d\n", 1280, 720, u32BlkSize);
    stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u64BlkSize = u32BlkSize;
    stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt = 6*2;
    stVbConf.u32MaxPoolCnt++;

    // VO layer
    u32BlkSize = CAM_ALIGNE_TO(720, 512) * 1440 * 3 /2;
    log_dbg("%dx%d blk size %d\n", 720, 1440, u32BlkSize);
    stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u64BlkSize = u32BlkSize;
    stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt = 3;
    stVbConf.u32MaxPoolCnt++;

    for (int i = 0; i < stVbConf.u32MaxPoolCnt; i++)
    {
        log_info("VB Pool %d: blk size %d, blk count %d, total size %.4f MB\n", i,
            stVbConf.astCommPool[i].u64BlkSize, stVbConf.astCommPool[i].u32BlkCnt,
            (float)(stVbConf.astCommPool[i].u64BlkSize * stVbConf.astCommPool[i].u32BlkCnt) / 1024 / 1024);
    }

    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (AR_SUCCESS != s32Ret)
    {
        log_err("system init failed with %d!\n", s32Ret);
        return s32Ret;
    }

    return s32Ret;
}

AR_VOID sample_vi_opendev(ENUM_VIN_WORK_MODE_T mode, AR_S32 isp_fre, AR_S32 vif_fre, AR_S32 pcs_fre)
{
	VI_DEV_PROP_S Prop = {0};
	Prop.cam_mode = mode;
	Prop.hdr_fre_mod = 1;
	Prop.hdr_fre_hz = isp_fre;
	Prop.vif_fre_mod = 1;
	Prop.vif_fre_hz = vif_fre;
	Prop.isp_fre_mod = 1;
	Prop.isp_fre_hz = isp_fre;
	Prop.mipi_fre_mod = 1;
	Prop.mipi_fre_hz = pcs_fre;
    log_info("AR_MPI_VIN_OpenDev mode: %d, freq: isp %d vif %d pcs %d\n", mode, isp_fre, vif_fre, pcs_fre);
    return AR_MPI_VIN_OpenDev(&Prop);
}

AR_S32 sample_vi_init(ISP_SNS_OBJ_S *pSnsObj, SRTU_SENSOR_DEFAULT_ATTR_T stDefSnsAttr,
    VI_DEV ViDev, VI_PIPE ViPipe, AR_BOOL abChnEn[], SIZE_S astChnSize[], AR_S8 s8I2cDev)
{
    AR_S32 s32Ret = 0;
    ALG_LIB_S stAeLib = {0};
    ALG_LIB_S stAwbLib = {0};

    log_info("vi dev %d pipe %d i2c%d mipi%d\n", ViDev, ViPipe, s8I2cDev, stDefSnsAttr.stComboAttr.devno);

    AR_MPI_VI_SetMipiBindDev(ViDev, stDefSnsAttr.stComboAttr.devno);
    AR_MPI_VI_SetComboDevAttr(&stDefSnsAttr.stComboAttr);
    AR_MPI_VI_SetDevAttr(ViDev, &stDefSnsAttr.stDevAttr);
    AR_MPI_VI_EnableDev(ViDev);
    VI_DEV_BIND_PIPE_S stDevBindPipe;
    stDevBindPipe.u32Num = 1;
    stDevBindPipe.PipeId[0] = ViPipe;
    AR_MPI_VI_SetDevBindPipe(ViDev, &stDevBindPipe);
    AR_MPI_VI_CreatePipe(ViPipe, &stDefSnsAttr.stPipeAttr);
    AR_MPI_VI_StartPipe(ViPipe);

    if (abChnEn[0])
    {
        AR_MPI_VI_SetChnAttr(ViPipe, 0, &stDefSnsAttr.stChnAttr);
        AR_MPI_VI_EnableChn(ViPipe, 0);
    }

    if (abChnEn[1])
    {
        if (astChnSize[1].u32Width > 0 && astChnSize[1].u32Height > 0)
        {
            stDefSnsAttr.stChnAttr.stSize.u32Width  = astChnSize[1].u32Width;
            stDefSnsAttr.stChnAttr.stSize.u32Height = astChnSize[1].u32Height;
        }
        AR_MPI_VI_SetChnAttr(ViPipe, 1, &stDefSnsAttr.stChnAttr);
        AR_MPI_VI_EnableChn(ViPipe, 1);
    }

    if (abChnEn[2])
    {
        log_info("enable raw chn for pipe %d!\n", ViPipe);
        stDefSnsAttr.stChnAttr.enPixelFormat = stDefSnsAttr.stPipeAttr.enPixFmt;
        AR_MPI_VI_SetChnAttr(ViPipe, 2, &stDefSnsAttr.stChnAttr);
        AR_MPI_VI_EnableChn(ViPipe, 2);
    }

    stAeLib.s32Id = ViPipe;
    stAwbLib.s32Id = ViPipe;
    strncpy(stAeLib.acLibName, "artosyn", sizeof("artosyn"));
    strncpy(stAwbLib.acLibName, "artosyn", sizeof("artosyn"));

    if (pSnsObj->pfnRegisterCallback != AR_NULL)
    {
        s32Ret = pSnsObj->pfnRegisterCallback(ViPipe, &stAeLib, &stAwbLib);

        if (s32Ret != AR_SUCCESS)
        {
            log_err("sensor_register_callback failed with %#x!\n", s32Ret);
            return AR_FAILURE;
        }
    }
    else
    {
        log_err("sensor_register_callback failed with AR_NULL!\n");
        return AR_FAILURE;
    }

    if (AR_NULL != pSnsObj->pfnSetBusInfo)
    {
        ISP_SNS_COMMBUS_U uSnsBusInfo;
        uSnsBusInfo.s8I2cDev = s8I2cDev;
        s32Ret = pSnsObj->pfnSetBusInfo(ViPipe, uSnsBusInfo);

        if (s32Ret != AR_SUCCESS)
        {
            log_err("set sensor bus info failed with %#x!\n", s32Ret);
            return AR_FAILURE;
        }
    }
    else
    {
        log_err("not support set sensor bus info!\n");
        return AR_FAILURE;
    }

    AR_MPI_ISP_MemInit(ViPipe);
    AR_MPI_ISP_SetPubAttr(ViPipe, &stDefSnsAttr.stPubAttr);
    VI_PIPE_EXT_ATTR_S stPipeAttr;
    AR_MPI_VI_GetPipeExtAttr(ViPipe, &stPipeAttr);
    stPipeAttr.bFoucs = 0;
    AR_MPI_VI_SetPipeExtAttr(ViPipe, &stPipeAttr);
    AR_MPI_ISP_Init(ViPipe);
    AR_MPI_ISP_Run(ViPipe);

    return 0;
}

AR_S32 sample_vi_deinit(ISP_SNS_OBJ_S *pSnsObj, VI_DEV ViDev, VI_PIPE ViPipe, AR_BOOL abChnEn[])
{
    AR_S32 s32Ret = 0;
    ALG_LIB_S stAeLib = {0};
    ALG_LIB_S stAwbLib = {0};
    stAeLib.s32Id = ViPipe;
    stAwbLib.s32Id = ViPipe;
    strncpy(stAeLib.acLibName, "artosyn", sizeof("artosyn"));
    strncpy(stAwbLib.acLibName, "artosyn", sizeof("artosyn"));

    AR_MPI_ISP_Exit(ViPipe);
    if (pSnsObj->pfnUnRegisterCallback != AR_NULL)
    {
        s32Ret = pSnsObj->pfnUnRegisterCallback(ViPipe, &stAeLib, &stAwbLib);
        if (s32Ret != AR_SUCCESS)
        {
            log_err("sensor_register_callback failed with %#x!\n", s32Ret);
            return AR_FAILURE;
        }
    }

    if (abChnEn[0])
    {
        AR_MPI_VI_DisableChn(ViPipe, 0);
    }

    if (abChnEn[1])
    {
        AR_MPI_VI_DisableChn(ViPipe, 1);
    }

    AR_MPI_VI_StopPipe(ViPipe);
    AR_MPI_VI_DestroyPipe(ViPipe);
    AR_MPI_VI_DisableDev(ViDev);

    return 0;
}

AR_S32 sample_vo_init(VO_DEV VoDev, VO_LAYER VoLayer, AR_BOOL abChnEn[],
    STRU_AR_HAL_VO_RECT aChnPosition[], AR_S32 s32ChnNum)
{
    AR_S32 s32Ret = 0;
    s32Ret = sample_vo_init_st7703_720x1440(VoDev, VoLayer);

    for (int i = 0; i < s32ChnNum; i++)
    {
        if (abChnEn[i])
        {
            s32Ret += sample_vo_chn_enable(VoLayer, i, &aChnPosition[i]);
        }
    }

    return s32Ret;
}

AR_S32 sample_vo_deinit(VO_DEV VoDev, VO_LAYER VoLayer, AR_BOOL abChnEn[], AR_S32 s32ChnNum)
{
    AR_S32 s32Ret = 0;
    for (int i = 0; i < s32ChnNum; i++)
    {
        if(abChnEn[i])
        {
            s32Ret = AR_MPI_VO_DisableChn(VoLayer, i);
            if (s32Ret)
            {
                log_err("AR_MPI_VO_DisableChn layer %d chn %d failed, ret = 0x%x\n", VoLayer, i);
            }
        }
    }

    s32Ret = AR_MPI_VO_DisableVideoLayer(VoLayer);
    if (s32Ret)
    {
        log_err("AR_MPI_VO_DisableVideoLayer layer %d failed, ret = 0x%x\n", VoLayer);
    }

    s32Ret = AR_MPI_VO_Disable(VoDev);
    if (s32Ret)
    {
        log_err("AR_MPI_VO_Disable dev %d failed, ret = 0x%x\n", VoDev);
    }

    return s32Ret;
}

AR_S32 sample_vpss_init(VPSS_GRP VpssGrp, SIZE_S VpssGrpSize, ROTATION_E eGrpRotateAngle,
    AR_BOOL bGrpMirrorEn, AR_BOOL bGrpFlipEn, RECT_S stVpssGrpCrop, AR_S32 VpssChnNum,
    AR_BOOL abVpssChnEn[], SIZE_S astVpssChnSize[],ROTATION_E aeChnRotateAngle[],
    AR_BOOL abChnMirrorEn[], AR_BOOL abChnFlipEn[], RECT_S astVpssChnCrop[])
{
    AR_S32 s32Ret;

    VPSS_GRP_ATTR_S stVpssGrpAttr = {0};
    stVpssGrpAttr.stFrameRate.s32SrcFrameRate = -1;
    stVpssGrpAttr.stFrameRate.s32DstFrameRate = -1;
    stVpssGrpAttr.u32Width = VpssGrpSize.u32Width;
    stVpssGrpAttr.u32Height = VpssGrpSize.u32Height;
    stVpssGrpAttr.enChnMode = VPSS_CHN_MODE_USER;
    stVpssGrpAttr.enCompressMode = COMPRESS_MODE_NONE;
    stVpssGrpAttr.enDynamicRange = DYNAMIC_RANGE_SDR8;
    stVpssGrpAttr.enVideoFormat = VIDEO_FORMAT_LINEAR;
    stVpssGrpAttr.enPixelFormat = PIXEL_FORMAT_YVU_PLANAR_420;
    stVpssGrpAttr.stAspectRatio.enMode = ASPECT_RATIO_NONE;
    stVpssGrpAttr.u32ScaleMode = 0; // 1 bilinear, 0 bicubic
    stVpssGrpAttr.u32Depth = 1;
    stVpssGrpAttr.enRotation = eGrpRotateAngle;
    stVpssGrpAttr.bMirror = bGrpMirrorEn;
    stVpssGrpAttr.bFlip = bGrpFlipEn;

    if (stVpssGrpAttr.u32Width < 2 || stVpssGrpAttr.u32Height < 2)
    {
        log_err("vpss group %d size is invalid!!!!!! width = %d height = %d\n",
            VpssGrp, stVpssGrpAttr.u32Width, stVpssGrpAttr.u32Height);
        return AR_FAILURE;
    }

    log_dbg("vpss grp %d attr: u32Width=%d u32Height=%d enRotation=%d bMirror=%d bFlip=%d\n",
        VpssGrp, stVpssGrpAttr.u32Width, stVpssGrpAttr.u32Height,
        stVpssGrpAttr.enRotation, stVpssGrpAttr.bMirror, stVpssGrpAttr.bFlip);

    s32Ret = AR_MPI_VPSS_CreateGrp(VpssGrp, &stVpssGrpAttr);
    if (s32Ret != AR_SUCCESS)
    {
        logerr("AR_MPI_VPSS_CreateGrp(grp:%d) failed with %#x!\n", VpssGrp, s32Ret);
        return AR_FAILURE;
    }
    log_dbg("AR_MPI_VPSS_CreateGrp grp %d success\n", VpssGrp);

    s32Ret = AR_MPI_VPSS_StartGrp(VpssGrp);
    if (s32Ret != AR_SUCCESS)
    {
        log_err("AR_MPI_VPSS_StartGrp(grp:%d) failed with %#x\n", VpssGrp, s32Ret);
        return AR_FAILURE;
    }
    log_dbg("AR_MPI_VPSS_StartGrp grp %d success\n", VpssGrp);

    if(stVpssGrpCrop.u32Width > 0 && stVpssGrpCrop.u32Height > 0)
    {
        VPSS_CROP_INFO_S stGrpCropInfo = {0};
        stGrpCropInfo.bEnable = 1;
        stGrpCropInfo.enCropCoordinate = VPSS_CROP_ABS_COOR;
        stGrpCropInfo.stCropRect.s32X = stVpssGrpCrop.s32X;
        stGrpCropInfo.stCropRect.s32Y = stVpssGrpCrop.s32Y;
        stGrpCropInfo.stCropRect.u32Width = stVpssGrpCrop.u32Width;
        stGrpCropInfo.stCropRect.u32Height = stVpssGrpCrop.u32Height;
        log_dbg("vpss grp %d: crop rect [%d %d %d %d]\n", VpssGrp,
            stGrpCropInfo.stCropRect.s32X, stGrpCropInfo.stCropRect.s32Y,
            stGrpCropInfo.stCropRect.u32Width, stGrpCropInfo.stCropRect.u32Height);
        s32Ret = AR_MPI_VPSS_SetGrpCrop(VpssGrp, &stGrpCropInfo);
        if (s32Ret != AR_SUCCESS)
        {
            log_err("AR_MPI_VPSS_SetGrpCrop grp %d failed! ret = 0%x\n", VpssGrp, s32Ret);
            return AR_FAILURE;
        }
        log_dbg("AR_MPI_VPSS_SetGrpCrop grp %d success\n", VpssGrp);
    }

    for (int i = 0; i < VpssChnNum; i++)
    {
        if(AR_TRUE == abVpssChnEn[i])
        {
            VPSS_CHN VpssChn = i;
            VPSS_CHN_ATTR_S stVpssChnAttr = {0};
            memcpy(&stVpssChnAttr, &stVpssGrpAttr, sizeof(VPSS_GRP_ATTR_S));
            stVpssChnAttr.u32Width = astVpssChnSize[i].u32Width;
            stVpssChnAttr.u32Height = astVpssChnSize[i].u32Height;
            stVpssChnAttr.enRotation = aeChnRotateAngle[i];
            stVpssChnAttr.bMirror = abChnMirrorEn[i];
            stVpssChnAttr.bFlip = abChnFlipEn[i];

            if (stVpssChnAttr.u32Width < 2 || stVpssChnAttr.u32Height < 2)
            {
                log_err("vpss grp %d ch %d size is invalid!!!!!! width = %d height = %d\n",
                    VpssGrp, VpssChn, stVpssChnAttr.u32Width, stVpssChnAttr.u32Height);
                continue;
            }

            log_dbg("vpss grp %d chn %d attr: u32Width=%d u32Height=%d enRotation=%d bMirror=%d bFlip=%d\n",
                VpssGrp, VpssChn, stVpssGrpAttr.u32Width, stVpssGrpAttr.u32Height,
                stVpssGrpAttr.enRotation, stVpssGrpAttr.bMirror, stVpssGrpAttr.bFlip);

            s32Ret = AR_MPI_VPSS_SetChnAttr(VpssGrp, VpssChn, &stVpssChnAttr);

            if (s32Ret != AR_SUCCESS)
            {
                log_err("AR_MPI_VPSS_SetChnAttr grp %d chn %d failed! ret = 0x%x\n", VpssGrp, VpssChn, s32Ret);
                return AR_FAILURE;
            }
            log_dbg("AR_MPI_VPSS_SetChnAttr grp %d chn %d success\n", VpssGrp, VpssChn);

            s32Ret = AR_MPI_VPSS_SetChnAlign(VpssGrp, VpssChn, 64);
            if (s32Ret != AR_SUCCESS)
            {
                log_err("AR_MPI_VPSS_SetChnAlign grp %d chn %d failed! ret = 0%x\n", VpssGrp, VpssChn, s32Ret);
                return AR_FAILURE;
            }
            log_dbg("AR_MPI_VPSS_SetChnAlign grp %d chn %d success\n", VpssGrp, VpssChn);

            s32Ret = AR_MPI_VPSS_EnableChn(VpssGrp, VpssChn);
            if (s32Ret != AR_SUCCESS)
            {
                log_err("AR_MPI_VPSS_EnableChn grp %d chn %d failed! ret = 0%x\n", VpssGrp, VpssChn, s32Ret);
                return AR_FAILURE;
            }
            log_dbg("AR_MPI_VPSS_EnableChn grp %d chn %d success\n", VpssGrp, VpssChn);

            if(astVpssChnCrop[i].u32Width > 0 && astVpssChnCrop[i].u32Height > 0)
            {
                VPSS_CROP_INFO_S stChnCropInfo = {0};
                stChnCropInfo.bEnable = 1;
                stChnCropInfo.enCropCoordinate = VPSS_CROP_ABS_COOR;
                stChnCropInfo.stCropRect.s32X = astVpssChnCrop[i].s32X;
                stChnCropInfo.stCropRect.s32Y = astVpssChnCrop[i].s32Y;
                stChnCropInfo.stCropRect.u32Width = astVpssChnCrop[i].u32Width;
                stChnCropInfo.stCropRect.u32Height = astVpssChnCrop[i].u32Height;
                log_dbg("vpss grp %d chn %d: crop rect [%d %d %d %d]\n", VpssGrp, VpssChn,
                    stChnCropInfo.stCropRect.s32X, stChnCropInfo.stCropRect.s32Y,
                    stChnCropInfo.stCropRect.u32Width, stChnCropInfo.stCropRect.u32Height);
                s32Ret = AR_MPI_VPSS_SetChnCrop(VpssGrp, VpssChn, &stChnCropInfo);
                if (s32Ret != AR_SUCCESS)
                {
                    log_err("AR_MPI_VPSS_SetChnCrop grp %d chn %d failed! ret = 0%x\n", VpssGrp, VpssChn, s32Ret);
                    return AR_FAILURE;
                }
                log_dbg("AR_MPI_VPSS_SetChnCrop grp %d chn %d success\n", VpssGrp, VpssChn);
            }
        }
    }

    return AR_SUCCESS;
}

AR_S32 sample_vpss_deinit(VPSS_GRP VpssGrp, AR_BOOL abVpssChnEn[], AR_S32 VpssChnNum)
{
    AR_S32 s32Ret = AR_SUCCESS;

    for (int i = 0; i < VpssChnNum; i++)
    {
        if(AR_TRUE == abVpssChnEn[i])
        {
            s32Ret = AR_MPI_VPSS_DisableChn(VpssGrp, i);

            if (s32Ret != AR_SUCCESS)
            {
                log_err("AR_MPI_VPSS_DisableChn Grp %d chn %d failed! ret = 0x%x!\n", VpssGrp, i, s32Ret);
                return AR_FAILURE;
            }
        }
    }

    s32Ret = AR_MPI_VPSS_StopGrp(VpssGrp);

    if (s32Ret != AR_SUCCESS)
    {
        log_err("AR_MPI_VPSS_StopGrp Grp %d failed! ret = 0x%x!\n", VpssGrp, s32Ret);
        return AR_FAILURE;
    }

    s32Ret = AR_MPI_VPSS_DestroyGrp(VpssGrp);

    if (s32Ret != AR_SUCCESS)
    {
        log_err("AR_MPI_VPSS_DestroyGrp Grp %d failed! ret = 0x%x!\n", VpssGrp, s32Ret);
        return AR_FAILURE;
    }

    return AR_SUCCESS;
}

static AR_S32 sample_check_frame_valid(VIDEO_FRAME_INFO_S stFrameInfo)
{
    if (stFrameInfo.stVFrame.u32Width <= 2 || stFrameInfo.stVFrame.u32Height < 2  ||
        stFrameInfo.stVFrame.u32Stride[0] < CAM_ALIGNE_TO(stFrameInfo.stVFrame.u32Width, 16) ||
        stFrameInfo.stVFrame.u32Stride[1] < CAM_ALIGNE_TO(stFrameInfo.stVFrame.u32Width, 16) / 2 ||
        stFrameInfo.stVFrame.u32Stride[2] < CAM_ALIGNE_TO(stFrameInfo.stVFrame.u32Width, 16) / 2 ||
        stFrameInfo.stVFrame.u32Len[0] < stFrameInfo.stVFrame.u32Stride[0] * stFrameInfo.stVFrame.u32Height ||
        stFrameInfo.stVFrame.u32Len[1] < stFrameInfo.stVFrame.u32Stride[1] * stFrameInfo.stVFrame.u32Height / 2 ||
        stFrameInfo.stVFrame.u32Len[2] < stFrameInfo.stVFrame.u32Stride[2] * stFrameInfo.stVFrame.u32Height / 2 ||
        stFrameInfo.stVFrame.u64PhyAddr[0] <= 0x20000000 ||
        stFrameInfo.stVFrame.u64PhyAddr[1] <= 0x20000000 ||
        stFrameInfo.stVFrame.u64PhyAddr[2] <= 0x20000000)
    {
        log_err("frame %d is invalid!!!!!!!!\n", stFrameInfo.stVFrame.u32FrameId);
        log_err("\nframe %d width %d height %d, stride %d %d %d, len %d %d %d\nphyaddr 0x%lx 0x%lx 0x%lx, viraddr 0x%lx 0x%lx 0x%lx\n",
            stFrameInfo.stVFrame.u32FrameId, stFrameInfo.stVFrame.u32Width, stFrameInfo.stVFrame.u32Height,
            stFrameInfo.stVFrame.u32Stride[0], stFrameInfo.stVFrame.u32Stride[1], stFrameInfo.stVFrame.u32Stride[2],
            stFrameInfo.stVFrame.u32Len[0], stFrameInfo.stVFrame.u32Len[1], stFrameInfo.stVFrame.u32Len[2],
            stFrameInfo.stVFrame.u64PhyAddr[0], stFrameInfo.stVFrame.u64PhyAddr[1], stFrameInfo.stVFrame.u64PhyAddr[2],
            stFrameInfo.stVFrame.u64VirAddr[0], stFrameInfo.stVFrame.u64VirAddr[1], stFrameInfo.stVFrame.u64VirAddr[2]);
        return -1;
    }

    return 0;
}

static AR_VOID *sample_vi_frame_process_thr(void *args)
{
    AR_S32 ret = 0;
    AR_U64 cnt = 0; 
    SAMPLE_FRAME_PROCESS_T *pPrams = (SAMPLE_FRAME_PROCESS_T *)args;

    log_info("**** start vi pipe %d chn %d frame process thread ****\n", pPrams->DevId, pPrams->ChnId);

    char t_name[64] = {0};
    sprintf(t_name, "t_vi%dch%d", pPrams->DevId, pPrams->ChnId);
    prctl(PR_SET_NAME, t_name);

    while (pPrams->Run)
    {
        VIDEO_FRAME_INFO_S stFrameInfo = {0};
        ret = AR_MPI_VI_GetChnFrame(pPrams->DevId, pPrams->ChnId, &stFrameInfo, 5000);
        if (ret)
        {
            log_err("AR_MPI_VI_GetChnFrame pipe %d chn %d failed! ret = 0x%x\n", pPrams->DevId, pPrams->ChnId, ret);
            if (g_exit)
            {
                break;
            }
            usleep(30000);
            continue;
        }

        cnt++;

        ret = sample_check_frame_valid(stFrameInfo);
        if (pPrams->bSendToVpss && !ret)
        {
            ret = AR_MPI_VPSS_SendFrame(pPrams->VpssGrpId, 0, &stFrameInfo, 500);
            if (ret)
            {
                log_err("AR_MPI_VPSS_SendFrame grp %d failed! ret = 0x%x\n", pPrams->VpssGrpId, ret);
            }
        }

        AR_MPI_VI_ReleaseChnFrame(pPrams->DevId, pPrams->ChnId, &stFrameInfo);

        if (g_exit)
        {
            break;
        }
    }
    log_info("**** vi pipe %d chn %d frame process thread exit ****\n", pPrams->DevId, pPrams->ChnId);

    return NULL;
}

static AR_VOID *sample_vpss_frame_process_thr(void *args)
{
    AR_S32 ret = 0;
    AR_U64 cnt = 0; 
    SAMPLE_FRAME_PROCESS_T *pPrams = (SAMPLE_FRAME_PROCESS_T *)args;

    log_info("**** start vpss group %d channel %d frame process thread ****\n", pPrams->DevId, pPrams->ChnId);

    char t_name[64] = {0};
    sprintf(t_name, "t_vi%dch%d", pPrams->DevId, pPrams->ChnId);
    prctl(PR_SET_NAME, t_name);

    while (pPrams->Run)
    {
        VIDEO_FRAME_INFO_S stFrameInfo = {0};
        ret = AR_MPI_VPSS_GetChnFrame(pPrams->DevId, pPrams->ChnId, &stFrameInfo, 5000);
        if (ret)
        {
            log_err("AR_MPI_VPSS_GetChnFrame grp %d chn %d failed! ret = 0x%x\n", pPrams->DevId, pPrams->ChnId, ret);
            if (g_exit)
            {
                break;
            }
            usleep(30000);
            continue;
        }

        cnt++;

        ret = sample_check_frame_valid(stFrameInfo);
        if (pPrams->bSendToVo && !ret)
        {
            ret = AR_MPI_VO_SendFrame(pPrams->VoLayerId, pPrams->VoChnId, &stFrameInfo, 500);
            if (ret)
            {
                log_err("AR_MPI_VO_SendFrame layer %d chn %d failed! ret = 0x%x\n", pPrams->VoLayerId, pPrams->VoChnId, ret);
            }
        }

        AR_MPI_VPSS_ReleaseChnFrame(pPrams->DevId, pPrams->ChnId, &stFrameInfo);

        if (g_exit)
        {
            break;
        }
    }
    log_info("**** vpss group %d channel %d frame process thread exit! ****\n", pPrams->DevId, pPrams->ChnId);

    return NULL;
}

// #define SEND_VIDEO_FRAME_MANUALLY

AR_S32 sample_pipeline_dual_imx307(SAMPLE_VIO_PRA *ppra)
{
    AR_S32 s32Ret;

    ISP_SNS_OBJ_S *pSnsObj = &stSnsImx290Obj;
    SRTU_SENSOR_DEFAULT_ATTR_T astDefSnsAttr[SENSOR_NUM];

    VI_DEV  ViDev[SENSOR_NUM]       = {0, 1};
    VI_PIPE ViPipe[SENSOR_NUM]      = {0, 1};
    AR_S32  s32MipiDev[SENSOR_NUM]  = {0, 1};
    AR_S8   s8I2cDev[SENSOR_NUM]    = {0, 1};
    AR_S32  abViChnEn[VI_MAX_CHN_NUM] = {0};
    SIZE_S  astViChnSize[VI_MAX_CHN_NUM] = {0};

    VO_DEV VoDev = 0;
    VO_LAYER VoLayer = 0;
    AR_BOOL abVoChnEn[MAX_VO_CHN_NUM] = {0};
    STRU_AR_HAL_VO_RECT aVoChnPosition[MAX_VO_CHN_NUM] = {0};
    AR_S32 VoChnNum = 2;

    VPSS_GRP VpssGrp[SENSOR_NUM] = {0, 1};
    SIZE_S astVpssGrpSize[SENSOR_NUM] = {0};
    AR_BOOL abVpssChnEn[VPSS_MAX_PHY_CHN_NUM] = {0};
    SIZE_S astVpssChnSize[VPSS_MAX_PHY_CHN_NUM] = {0};

    // mipi_index[0] = 0;
    // s8I2cDev[0] = 0;
    // mipi_index[1] = 1;
    // s8I2cDev[1] = 1;

    // get all vi default attr from sensor driver
    if (pSnsObj->pfnGetDefaultAttr)
    {
        pSnsObj->pfnGetDefaultAttr(0, &astDefSnsAttr[0]);
        pSnsObj->pfnGetDefaultAttr(1, &astDefSnsAttr[1]);
    }
    else
    {
        log_err("pfnGetDefaultAttr is null, exit the test");
        return AR_FAILURE;
    }

    AR_BOOL abViRawChnEnable[SENSOR_NUM] = {0, 0};
    if (ppra->dpcm == 2)
    {
        abViRawChnEnable[0] = AR_TRUE;
        abViRawChnEnable[1] = AR_TRUE;
    }

    // sys init, config vb
    s32Ret = sample_sys_init(astDefSnsAttr, abViRawChnEnable, SENSOR_NUM);
    if (AR_SUCCESS != s32Ret)
    {
        log_err("sample_sys_init\n");
        return AR_FAILURE;
    }

    if (ppra->isp_fre == 0)
    {
        ppra->isp_fre = 200000000;
    }

    if (ppra->vif_fre == 0)
    {
        ppra->vif_fre = 300000000;
    }

    if (ppra->pcs_fre == 0)
    {
        ppra->pcs_fre = 100000000;
    }

    // open VI dev, config frequency
    sample_vi_opendev(VIN_CMAERA_MULTI_MODE, ppra->isp_fre, ppra->vif_fre, ppra->pcs_fre);

    if (ppra->settle > 0)
    {
        astDefSnsAttr[0].stComboAttr.mipi_attr.settle_count = ppra->settle;
        astDefSnsAttr[1].stComboAttr.mipi_attr.settle_count = ppra->settle;
    }

    if (ppra->mipi_fre > 0)
    {
        // sensor 0
        astDefSnsAttr[0].stComboAttr.mipi_attr.mipi_pix_clk = ppra->mipi_fre / 1000 / 1000;
        // sensor 1
        astDefSnsAttr[1].stComboAttr.mipi_attr.mipi_pix_clk = ppra->mipi_fre / 1000 / 1000;
    }

    if (ppra->dpcm == 1)
    {
        // enable raw compress to save band width
        // sensor 0
        astDefSnsAttr[0].stPipeAttr.enEnableNoCmp = VIN_PIPE_NOCPM_DISABLE;
        astDefSnsAttr[0].stPipeAttr.enCompressMode = COMPRESS_MODE_DPCM_6BITS;
        astDefSnsAttr[0].stPipeAttr.stNrAttr.enCompressMode = COMPRESS_MODE_DPCM_6BITS;
        // sensor 1
        astDefSnsAttr[1].stPipeAttr.enEnableNoCmp = VIN_PIPE_NOCPM_DISABLE;
        astDefSnsAttr[1].stPipeAttr.enCompressMode = COMPRESS_MODE_DPCM_6BITS;
        astDefSnsAttr[1].stPipeAttr.stNrAttr.enCompressMode = COMPRESS_MODE_DPCM_6BITS;
    }
    else if (ppra->dpcm == 2)
    {
        // disable raw compress when tunning mode
        // sensor 0
        astDefSnsAttr[0].stPipeAttr.enEnableNoCmp = VIN_PIPE_NOCPM_ENABLE;
        astDefSnsAttr[0].stPipeAttr.enCompressMode = COMPRESS_MODE_NONE;
        astDefSnsAttr[0].stPipeAttr.stNrAttr.enCompressMode = COMPRESS_MODE_NONE;
        // sensor 1
        astDefSnsAttr[1].stPipeAttr.enEnableNoCmp = VIN_PIPE_NOCPM_ENABLE;
        astDefSnsAttr[1].stPipeAttr.enCompressMode = COMPRESS_MODE_NONE;
        astDefSnsAttr[1].stPipeAttr.stNrAttr.enCompressMode = COMPRESS_MODE_NONE;
    }

    if (ppra->feature_mask)
    {
        // sensor 0
        astDefSnsAttr[0].stPipeAttr.u32FeatureMask = ppra->feature_mask;
        // sensor 1
        astDefSnsAttr[1].stPipeAttr.u32FeatureMask = ppra->feature_mask;
    }

    abViChnEn[0] = AR_TRUE; // enable vi chn 0
    if (ppra->dpcm == 2)
    {
        // enable raw chn for tunning mode
        abViChnEn[2] = AR_TRUE;
    }

    // sensor 0
    astDefSnsAttr[0].stComboAttr.devno = s32MipiDev[0];
    sample_vi_init(pSnsObj, astDefSnsAttr[0], ViDev[0], ViPipe[0], abViChnEn, astViChnSize, s8I2cDev[0]);

    // sensor 1
    astDefSnsAttr[0].stComboAttr.devno = s32MipiDev[1];
    sample_vi_init(pSnsObj, astDefSnsAttr[1], ViDev[1], ViPipe[1], abViChnEn, astViChnSize, s8I2cDev[1]);

    /* VO */
    SIZE_S DisplaySize = {720, 1440};
    VoChnNum = 2;
    // the top half of the display
    aVoChnPosition[0].x = 0;
    aVoChnPosition[0].y = 0;
    aVoChnPosition[0].w = DisplaySize.u32Width;
    aVoChnPosition[0].h = DisplaySize.u32Height/2;
    abVoChnEn[0] = AR_TRUE;

    // the bottom half of the display
    aVoChnPosition[1].x = 0;
    aVoChnPosition[1].y = DisplaySize.u32Height/2;
    aVoChnPosition[1].w = DisplaySize.u32Width;
    aVoChnPosition[1].h = DisplaySize.u32Height/2;
    abVoChnEn[1] = AR_TRUE;

    sample_vo_init(VoDev, VoLayer, abVoChnEn, aVoChnPosition, VoChnNum);

    /* VPSS */
    ROTATION_E aeVpssGrpRotateAngle[SENSOR_NUM] = {0};
    AR_BOOL bGrpMirrorEn = AR_FALSE;
    AR_BOOL bGrpFlipEn = AR_FALSE;
    RECT_S stVpssGrpCrop = {0};

    AR_S32 VpssChnNum = 2;
    ROTATION_E aeVpssChnRotateAngle[VPSS_MAX_PHY_CHN_NUM] = {0};
    AR_BOOL abChnMirrorEn[VPSS_MAX_PHY_CHN_NUM] = {0};
    AR_BOOL abChnFlipEn[VPSS_MAX_PHY_CHN_NUM] = {0};
    RECT_S astVpssChnCrop[VPSS_MAX_PHY_CHN_NUM] = {0};

    for (int i = 0; i < SENSOR_NUM; i++)
    {
        // vpss group size is same as the vi chn0 size
        astVpssGrpSize[i].u32Width = astDefSnsAttr[i].stChnAttr.stSize.u32Width;
        astVpssGrpSize[i].u32Height = astDefSnsAttr[i].stChnAttr.stSize.u32Height;
        // vpss chn 0 resolution
        astVpssChnSize[0].u32Width = 1920;
        astVpssChnSize[0].u32Height = 1080;
        abVpssChnEn[0] = AR_TRUE;
        // vpss chn 1 resolution
        astVpssChnSize[1].u32Width = 1280;
        astVpssChnSize[1].u32Height = 720;
        abVpssChnEn[1] = AR_TRUE;
        sample_vpss_init(VpssGrp[i], astVpssGrpSize[i], aeVpssGrpRotateAngle[i], bGrpMirrorEn, bGrpFlipEn, stVpssGrpCrop,
            VpssChnNum, abVpssChnEn, astVpssChnSize, aeVpssChnRotateAngle, abChnMirrorEn, abChnFlipEn, astVpssChnCrop);
    }

#ifdef SEND_VIDEO_FRAME_MANUALLY
    /* send frame to next module */
    // Vi pipe 0 chn 0 --> Vpss group 0
    ViPipe0Ch0Thr.Run = 1;
    ViPipe0Ch0Thr.DevId = ViPipe[0];
    ViPipe0Ch0Thr.ChnId = 0;
    ViPipe0Ch0Thr.bSendToVpss = 1;
    ViPipe0Ch0Thr.VpssGrpId = VpssGrp[0];
    pthread_create(&ViPipe0Ch0Thr.tid, NULL, sample_vi_frame_process_thr, &ViPipe0Ch0Thr);

    // Vi pipe 1 chn 0 --> Vpss group 1
    ViPipe1Ch0Thr.Run = 1;
    ViPipe1Ch0Thr.DevId = ViPipe[1];
    ViPipe1Ch0Thr.ChnId = 0;
    ViPipe1Ch0Thr.bSendToVpss = 1;
    ViPipe1Ch0Thr.VpssGrpId = VpssGrp[1];
    pthread_create(&ViPipe1Ch0Thr.tid, NULL, sample_vi_frame_process_thr, &ViPipe1Ch0Thr);

    // Vpss group 0 chn 0 --> vo layer 0 chn 0
    VpssGrp0ChnThr[0].Run = 1;
    VpssGrp0ChnThr[0].DevId = VpssGrp[0];
    VpssGrp0ChnThr[0].ChnId = 0;
    VpssGrp0ChnThr[0].bSendToVo = 1;
    VpssGrp0ChnThr[0].VoLayerId = VoLayer;
    VpssGrp0ChnThr[0].VoChnId = 0;
    pthread_create(&VpssGrp0ChnThr[0].tid, NULL, sample_vpss_frame_process_thr, &VpssGrp0ChnThr[0]);

    // Vpss group 1 chn 0 --> vo layer 0 chn 1
    VpssGrp1ChnThr[0].Run = 1;
    VpssGrp1ChnThr[0].DevId = VpssGrp[1];
    VpssGrp1ChnThr[0].ChnId = 0;
    VpssGrp1ChnThr[0].bSendToVo = 1;
    VpssGrp1ChnThr[0].VoLayerId = VoLayer;
    VpssGrp1ChnThr[0].VoChnId = 1;
    pthread_create(&VpssGrp1ChnThr[1].tid, NULL, sample_vpss_frame_process_thr, &VpssGrp1ChnThr[0]);
#else
    /* bind modules */
    // Sensor 0 pipeline: VI pipe0 ch 0 -> VPSS group 0
    SAMPLE_COMM_VI_Bind_VPSS(ViPipe[0], 0, VpssGrp[0]);
    // VPSS group 0 chn 0 -> VO chn 0
    SAMPLE_COMM_VPSS_Bind_VO(VpssGrp[0], 0, VoLayer, 0);

    // Sensor 1 pipeline: VI pipe1 ch 0 -> VPSS group 1
    SAMPLE_COMM_VI_Bind_VPSS(ViPipe[1], 0, VpssGrp[1]);
    // VPSS group 1 chn 0 -> VO chn 1
    SAMPLE_COMM_VPSS_Bind_VO(VpssGrp[1], 0, VoLayer, 1);
#endif

    while (!g_exit)
    {
        ar_delay(10);
        continue;
    }

EXIT:
#ifdef SEND_VIDEO_FRAME_MANUALLY
    VpssGrp0ChnThr[0].Run = 0;
    pthread_join(VpssGrp0ChnThr[0].tid, NULL);
    VpssGrp0ChnThr[1].Run = 0;
    pthread_join(VpssGrp0ChnThr[1].tid, NULL);
    ViPipe0Ch0Thr.Run = 0;
    pthread_join(ViPipe0Ch0Thr.tid, NULL);
    ViPipe1Ch0Thr.Run = 0;
    pthread_join(ViPipe1Ch0Thr.tid, NULL);
#else
    /* unbind module */
    SAMPLE_COMM_VI_UnBind_VPSS(ViPipe[0], 0, VpssGrp[0]);
    SAMPLE_COMM_VI_UnBind_VPSS(ViPipe[1], 0, VpssGrp[1]);
    SAMPLE_COMM_VPSS_UnBind_VO(VpssGrp[0], 0, VoLayer, 0);
    SAMPLE_COMM_VPSS_UnBind_VO(VpssGrp[1], 1, VoLayer, 1);
#endif

    for (int i = 0; i < SENSOR_NUM; i++)
    {
        sample_vi_deinit(pSnsObj, ViDev[i], ViPipe[i], abViChnEn);
        sample_vpss_deinit(VpssGrp[i], abVpssChnEn, 2);
    }

    sample_vo_deinit(VoDev, VoLayer, abVoChnEn, VoChnNum);

    SAMPLE_COMM_SYS_Exit();
    return s32Ret;
}
