#include "ir_fusion.h"
#include "hal_sys.h"

static IR_FUSION_S gIrFusion = {0};

AR_BOOL EnableVisibleLightViChn1(IR_VI_GROUP_CFG_S *pstIrViGrpCfg)
{
    AR_S32 s32Ret;

    if(pstIrViGrpCfg->s32VisibleLightIdx<0 || pstIrViGrpCfg->s32IrIsp1Idx<0 || pstIrViGrpCfg->s32IrIsp2Idx>=0) {
        //fusion only enable in mode 2
        return AR_FALSE;
    }

    VI_PIPE ViPipe = pstIrViGrpCfg->aViPipe[pstIrViGrpCfg->s32VisibleLightIdx];
    VI_CHN  ViChn1 = 1;
    VI_CHN_ATTR_S stChnAttr = {0};
    
    if(pstIrViGrpCfg->aViChn[pstIrViGrpCfg->s32VisibleLightIdx]==1) {
        //visiable 1 already enabled
        return AR_TRUE;
    }

    s32Ret = AR_MPI_VI_GetChnAttr(ViPipe, 0, &stChnAttr);
    if (AR_SUCCESS != s32Ret) {
        ar_err("AR_MPI_VI_GetChnAttr failed. s32Ret: 0x%x !\n", s32Ret);
        goto FAIL;
    }
    stChnAttr.stSize.u32Width = 640;
    stChnAttr.stSize.u32Height = 512;
    stChnAttr.u32Depth = 1;
    //stChnAttr.u32BufCount = 5;
    for(int cn = 0; cn < 8; cn++) {
        stChnAttr.u32DepthClient[cn] = 1;
    }

    s32Ret = AR_MPI_VI_SetChnAttr(ViPipe, ViChn1, &stChnAttr);
    if (AR_SUCCESS != s32Ret) {
        ar_err("AR_MPI_VI_GetChnAttr failed. s32Ret: 0x%x !\n", s32Ret);
        goto FAIL;
    }

    s32Ret = AR_MPI_VI_EnableChn(ViPipe, ViChn1);
    if (AR_SUCCESS != s32Ret) {
        ar_err("AR_MPI_VI_EnableChn failed. s32Ret: 0x%x !\n", s32Ret);
        goto FAIL;
    }

    return AR_TRUE;
FAIL:
    return AR_FALSE;
}

AR_S32 DisableVisibleLightViChn1(IR_VI_GROUP_CFG_S *pstIrViGrpCfg)
{
    if(pstIrViGrpCfg->aViChn[pstIrViGrpCfg->s32VisibleLightIdx]==1)
    {
        return -1;
    }

    VI_PIPE ViPipe = pstIrViGrpCfg->aViPipe[pstIrViGrpCfg->s32VisibleLightIdx];
    VI_CHN  ViChn1 = 1;
    return AR_MPI_VI_DisableChn(ViPipe, ViChn1);
}

AR_S32 StartIrVpss(IR_VI_GROUP_CFG_S *pstIrViGrpCfg)
{
    AR_S32 s32Ret;
    //inf group
    VPSS_GRP VpssGrp_inf = 0;
    VPSS_CHN VpssChn_inf0 = 0;
    {
        VPSS_GRP            VpssGrp = VpssGrp_inf;
        DYNAMIC_RANGE_E     enDynamicRange = DYNAMIC_RANGE_SDR8;
        PIXEL_FORMAT_E      enPixFormat    = PIXEL_FORMAT_YVU_PLANAR_420;
        VIDEO_FORMAT_E      enVideoFormat  = VIDEO_FORMAT_LINEAR;
        COMPRESS_MODE_E     enCompressMode = COMPRESS_MODE_NONE;
        VPSS_GRP_ATTR_S     stVpssGrpAttr = {0};

        SIZE_S GrpOutSize = pstIrViGrpCfg->astOutSize[pstIrViGrpCfg->s32IrIsp1Idx];
        if (GrpOutSize.u32Width == 408) {
            GrpOutSize.u32Width = 400;
        }
        AR_S32 s32fps = 25;
        
        stVpssGrpAttr.stFrameRate.s32SrcFrameRate    = s32fps;
        stVpssGrpAttr.stFrameRate.s32DstFrameRate    = s32fps;
        stVpssGrpAttr.u32Width                       = GrpOutSize.u32Width;
        stVpssGrpAttr.u32Height                      = GrpOutSize.u32Height;
        stVpssGrpAttr.enCompressMode                 = enCompressMode;
        stVpssGrpAttr.enDynamicRange                 = enDynamicRange;
        stVpssGrpAttr.enVideoFormat                  = enVideoFormat;
        stVpssGrpAttr.enPixelFormat                  = enPixFormat;

        //start group
        s32Ret = AR_MPI_VPSS_CreateGrp(VpssGrp, &stVpssGrpAttr);
        if (s32Ret != AR_SUCCESS)
        {
            SAMPLE_PRT("AR_MPI_VPSS_CreateGrp(grp:%d) failed with %#x!\n", VpssGrp, s32Ret);
            goto FAIL;
        }
        s32Ret = AR_MPI_VPSS_StartGrp(VpssGrp);
        if (s32Ret != AR_SUCCESS)
        {
            SAMPLE_PRT("AR_MPI_VPSS_StartGrp failed with %#x\n", s32Ret);
            goto FAIL1;
        }

        VPSS_CHN VpssChn;
        VPSS_CHN_ATTR_S stVpssChnAttr = {0};
        stVpssChnAttr.u32Width                     = GrpOutSize.u32Width;
        stVpssChnAttr.u32Height                    = GrpOutSize.u32Height;
        stVpssChnAttr.enChnMode                    = VPSS_CHN_MODE_USER;
        stVpssChnAttr.enCompressMode               = enCompressMode;
        stVpssChnAttr.enDynamicRange               = enDynamicRange;
        stVpssChnAttr.enVideoFormat                = enVideoFormat;
        stVpssChnAttr.enPixelFormat                = enPixFormat;
        //stVpssChnAttr.stFrameRate.s32SrcFrameRate  = s32fps;
        //stVpssChnAttr.stFrameRate.s32DstFrameRate  = s32fps;
        stVpssChnAttr.u32Depth                     = 2;

        {
            //channel 0
            VpssChn = 0;

            s32Ret = AR_MPI_VPSS_SetChnAttr(VpssGrp, VpssChn, &stVpssChnAttr);
            if (s32Ret != AR_SUCCESS)
            {
                SAMPLE_PRT("AR_MPI_VPSS_SetChnAttr failed with %#x\n", s32Ret);
                goto FAIL2;
            }

            AR_U32 u32Align = 64;
            s32Ret = AR_MPI_VPSS_SetChnAlign(VpssGrp, VpssChn, u32Align);
            if (s32Ret != AR_SUCCESS)
            {
                SAMPLE_PRT("AR_MPI_VPSS_SetChnAlign failed with %#x\n", s32Ret);
                goto FAIL2;
            }

            s32Ret = AR_MPI_VPSS_EnableChn(VpssGrp, VpssChn);
            if (s32Ret != AR_SUCCESS)
            {
                SAMPLE_PRT("AR_MPI_VPSS_EnableChn failed with %#x\n", s32Ret);
                goto FAIL2;
            }
        }
    }

    return 0;

//FAIL3:
    AR_MPI_VPSS_DisableChn(VpssGrp_inf, VpssChn_inf0);
FAIL2:
    AR_MPI_VPSS_StopGrp(VpssGrp_inf);
FAIL1:
    AR_MPI_VPSS_DestroyGrp(VpssGrp_inf);
FAIL:
    return -1;
}

AR_S32 StopIrVpss()
{
    VPSS_GRP VpssGrp_inf = 0;
    VPSS_CHN VpssChn_inf0 = 0;
    AR_MPI_VPSS_DisableChn(VpssGrp_inf, VpssChn_inf0);
    AR_MPI_VPSS_StopGrp(VpssGrp_inf);
    AR_MPI_VPSS_DestroyGrp(VpssGrp_inf);
    return 0;
}

AR_S32 SendIrFrameToVpss(VIDEO_FRAME_INFO_S *pstIrFrame)
{
    return AR_MPI_VPSS_SendFrame(0, 0, pstIrFrame, -1);
}

AR_S32 ColorMapHandle(VIDEO_FRAME_INFO_S *pstIrFrame)
{
    AR_S32 s32Ret;

    if(NULL ==gIrFusion.pstInfAlgObject)
    {
        ar_err("inf alg not init");
        return -1;
    }

#if 1
    if(gIrFusion.u64ColormapTmpBufPhyAddr == 0){
        //malloc cacheable mmz buffer
        int y_offset = pstIrFrame->stVFrame.u64PhyAddr[1] - pstIrFrame->stVFrame.u64PhyAddr[0];
        int u_offset = pstIrFrame->stVFrame.u64PhyAddr[2] - pstIrFrame->stVFrame.u64PhyAddr[1];
        int all_len = y_offset+u_offset*2;

        s32Ret = AR_MPI_SYS_MmzAlloc_Cached(&gIrFusion.u64ColormapTmpBufPhyAddr, &gIrFusion.pColormapTmpBufVirAddr, 
                NULL, NULL, all_len);
        if(s32Ret!=0){
            ar_err("mmz malloc failed");
            gIrFusion.u64ColormapTmpBufPhyAddr = 0;
            return -1;
        }
        gIrFusion.pColormapTmpBufVirAddr_u = gIrFusion.pColormapTmpBufVirAddr+y_offset;
        gIrFusion.pColormapTmpBufVirAddr_v = gIrFusion.pColormapTmpBufVirAddr_u+u_offset;
        gIrFusion.u32ColormapTmpBufLen = all_len;
    }

    ar_hal_sys_memcpy_pa(gIrFusion.u64ColormapTmpBufPhyAddr, pstIrFrame->stVFrame.u64PhyAddr[0], gIrFusion.u32ColormapTmpBufLen);
    void* y_vaddr = gIrFusion.pColormapTmpBufVirAddr;
    void* u_vaddr = gIrFusion.pColormapTmpBufVirAddr_u;
    void* v_vaddr = gIrFusion.pColormapTmpBufVirAddr_v;

    //color map
    s32Ret = gIrFusion.pstInfAlgObject->mapping_color_process(gIrFusion.pstInfAlgObject,
        y_vaddr, u_vaddr, v_vaddr,
        pstIrFrame->stVFrame.u32Width, pstIrFrame->stVFrame.u32Height,
        pstIrFrame->stVFrame.u32Stride[0], pstIrFrame->stVFrame.u32Stride[1], pstIrFrame->stVFrame.u32Stride[2]);
    if(s32Ret!=0){
        ar_err("color map failed");
        return -1;
    }
    
    ar_hal_sys_mmz_flush_cache_pa(gIrFusion.u64ColormapTmpBufPhyAddr, gIrFusion.u32ColormapTmpBufLen);
    ar_hal_sys_memcpy_pa(pstIrFrame->stVFrame.u64PhyAddr[0], gIrFusion.u64ColormapTmpBufPhyAddr, gIrFusion.u32ColormapTmpBufLen);
#else
    void* y_vaddr = (void*)pstIrFrame->stVFrame.u64VirAddr[0];
    void* u_vaddr = (void*)pstIrFrame->stVFrame.u64VirAddr[1];
    void* v_vaddr = (void*)pstIrFrame->stVFrame.u64VirAddr[2];

    //color map
    s32Ret = gIrFusion.pstInfAlgObject->mapping_color_process(gIrFusion.pstInfAlgObject,
        y_vaddr, u_vaddr, v_vaddr,
        pstIrFrame->stVFrame.u32Width, pstIrFrame->stVFrame.u32Height,
        pstIrFrame->stVFrame.u32Stride[0], pstIrFrame->stVFrame.u32Stride[1], pstIrFrame->stVFrame.u32Stride[2]);
    if(s32Ret!=0){
        ar_err("color map failed");
        return -1;
    }
#endif

    return 0;
}

static AR_VOID *FusionHanldeThreadFunc(AR_VOID *arg)
{
    AR_S32 s32Ret;
    VI_PIPE ViPipe_vis = gIrFusion.ViPipe_vis;
    VI_CHN  ViChn_vis = gIrFusion.ViChn_vis;
    VPSS_GRP VpssGrp_ir = gIrFusion.VpssGrp_ir;
    VPSS_CHN VpssChn_ir = gIrFusion.VpssChn_ir;

    while(gIrFusion.bStart){
        VIDEO_FRAME_INFO_S visFrame = {0}, irFrame = {0};
        
        s32Ret = AR_MPI_VI_GetChnFrame(ViPipe_vis, ViChn_vis, &visFrame, 1000);
        if(s32Ret!=0) {
            ar_err("get vis frmae failed %d", s32Ret);
            continue;
        }

        s32Ret = AR_MPI_VPSS_GetChnFrame(VpssGrp_ir, VpssChn_ir, &irFrame, 1000);
        if(s32Ret!=0) {
            AR_MPI_VI_ReleaseChnFrame(ViPipe_vis, ViChn_vis, &visFrame);
            ar_err("get ir frmae failed %d", s32Ret);
            continue;
        }

        static AR_BOOL isFusionInit = AR_FALSE;
        if(!isFusionInit){
            s32Ret =  gIrFusion.pstInfAlgObject->fusion_init(gIrFusion.pstInfAlgObject,
                    visFrame.stVFrame.u32Width,visFrame.stVFrame.u32Height,
                    visFrame.stVFrame.u32Stride[0],visFrame.stVFrame.u32Stride[1]);
            if(s32Ret!=0){
                ar_err("fusion_init failed\n");
                break;
            }
            else{
                isFusionInit = AR_TRUE;
            }
        }

        //VIDEO_FRAME_INFO_S fusionFrame = {0};
        s32Ret = gIrFusion.pstInfAlgObject->fusion_process(gIrFusion.pstInfAlgObject,&visFrame,&irFrame);
        if(s32Ret==0){
            if(gIrFusion.pfnFusionOutHandle){
                gIrFusion.pfnFusionOutHandle(&visFrame, NULL);
            }
        }

        AR_MPI_VI_ReleaseChnFrame(ViPipe_vis, ViChn_vis, &visFrame);
        AR_MPI_VPSS_ReleaseChnFrame(VpssGrp_ir, VpssChn_ir, &irFrame);
    }

    return 0;
}

AR_S32 StartIrFusion(IR_SAMPLE_CTX_S *pstIrSampleCtx, FusionOutHandleFunc outHanle)
{
    AR_S32 s32Ret;
    INF_ALGO_ALL_PARAM_S stInfAlgParams = {
        .stColorPaletteParam = {
            .ColorPaletteFilePath = "/usrdata/local/ColorMapV2_3.dat",
            .palette_index = 5,
        },
        .stFusionParam = {
            .luma_alpha = 180,
            .chroma_alpha = 180,
            .edge_weight = 1,
            .palette_index = -1,
            .calib_enable = 0,
            .fusion_mode = FUSION_MODE_1,
        },
    };
    s32Ret = inf_algo_create(&gIrFusion.pstInfAlgObject,"/usrdata/ir_params.json",&stInfAlgParams,NULL);
    if(s32Ret!=0)
    {
        ar_err("-----------------inf_algo_create %d", s32Ret);
        return -1;
    }

    gIrFusion.ViPipe_vis = pstIrSampleCtx->stIrViGrpCfg.aViPipe[pstIrSampleCtx->stIrViGrpCfg.s32VisibleLightIdx];
    gIrFusion.ViChn_vis = 1;
    gIrFusion.VpssGrp_ir = 0;
    gIrFusion.VpssChn_ir = 0;

    gIrFusion.bStart = AR_TRUE;
    s32Ret = pthread_create(&gIrFusion.fusion_thread, NULL, FusionHanldeThreadFunc, NULL);
    if (s32Ret) {
        ar_err("create fusion thread failed");
        gIrFusion.bStart = AR_FALSE;
        return -1;
    }

    gIrFusion.pfnFusionOutHandle = outHanle;

    return 0;    
}

AR_S32 StopIrFusion()
{
    if (gIrFusion.bStart) {
        gIrFusion.bStart = AR_FALSE;
        pthread_join(gIrFusion.fusion_thread, NULL);
    }

    if(gIrFusion.pstInfAlgObject){
        inf_algo_release(&gIrFusion.pstInfAlgObject);
        gIrFusion.pstInfAlgObject = NULL;
    }
    
    return 0;
}

AR_S32 SetColormapIndex(int index)
{
    INF_ALGO_ALL_PARAM_S stParam = {0};
    gIrFusion.pstInfAlgObject->get_config_params(gIrFusion.pstInfAlgObject, &stParam);
    stParam.stColorPaletteParam.palette_index = index;
    gIrFusion.pstInfAlgObject->set_config_params(gIrFusion.pstInfAlgObject, &stParam);
    return 0;
}

AR_S32 SetFusionMode(int mode)
{
    INF_ALGO_ALL_PARAM_S stParam = {0};
    gIrFusion.pstInfAlgObject->get_config_params(gIrFusion.pstInfAlgObject, &stParam);
    stParam.stFusionParam.fusion_mode = mode;
    gIrFusion.pstInfAlgObject->set_config_params(gIrFusion.pstInfAlgObject, &stParam);
    return 0;
}

AR_S32 SetFusionParam(int alpha1, int alpha2, int weight, int color)
{
    INF_ALGO_ALL_PARAM_S stParam = {0};
    gIrFusion.pstInfAlgObject->get_config_params(gIrFusion.pstInfAlgObject, &stParam);
    stParam.stFusionParam.luma_alpha = alpha1;
    stParam.stFusionParam.chroma_alpha = alpha2;
    stParam.stFusionParam.edge_weight = weight;
    stParam.stFusionParam.palette_index = color;
    gIrFusion.pstInfAlgObject->set_config_params(gIrFusion.pstInfAlgObject, &stParam);
    return 0;
}

INF_ALGO_OBJECT_S* GetInfObj()
{
    return gIrFusion.pstInfAlgObject;
}


AR_S32 SetFusionCalibPoint(int w, int h, POINT_S *pSrcPt, POINT_S *pDstPt)
{
    int ret = gIrFusion.pstInfAlgObject->set_calib_point(gIrFusion.pstInfAlgObject, w, h, pSrcPt, pDstPt);
    if(ret!=0)
    {
        printf("set caib point failed");
    }
    return 0;
}

AR_S32 SetFusionCalibEnable(int enable)
{
    INF_ALGO_ALL_PARAM_S stParam = {0};
    gIrFusion.pstInfAlgObject->get_config_params(gIrFusion.pstInfAlgObject, &stParam);
    stParam.stFusionParam.calib_enable = enable;
    gIrFusion.pstInfAlgObject->set_config_params(gIrFusion.pstInfAlgObject, &stParam);
    return 0;
}


