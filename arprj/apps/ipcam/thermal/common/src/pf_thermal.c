#include "pf_thermal.h"
#include "util_api.h"

#include "hal_vo.h"
#include "hal_vin_log.h"
#include "hal_region.h"
#include "mpi_vb.h"
#include "mpi_vi.h"
#include "osal.h"
#include "ar_comm_isp.h"
#include "mpi_vpss.h"
#include "mpi_venc.h"
#include "mpi_gdc_api.h"
#include "mpi_region.h"
#include "hal_scaler_api.h"
#include "cfg_vio.h"
#include "cfg_channel.h"
#include "ar_buffer.h"

#ifndef THERMAL_TYPE //for ars31
#include "ar_inf_alg.h"
#endif

#ifdef THERMAL_TYPE //this is the code for ar9341 series

#ifdef YUV_PROCESS_ENABLE
#include "ar_inf_alg.h"
#endif
#if THERMAL_TYPE == THERMAL_TYPE_GD_TIMO256
#include "guide_module_manager.h"
#elif THERMAL_TYPE == THERMAL_TYPE_IRAY_ELF
#include "iray_module_manager.h"
#endif

//#define DUMP_DATA 1

static STRU_IR_RAW_PROCESS_PARAM_T g_RawProcessParam;
#ifdef DUMP_DATA
static int save_y16 = 0, save_tp = 0, save_raw = 1;
static int debug_count = 2000;
#endif
#ifdef YUV_PROCESS_ENABLE
static INF_ALGO_OBJECT_S* g_pstInfAlgObject = NULL;
#endif
static STRU_YUV_PROCESS_PARAM_T stYuvProcessParam = {-1};

int IPC_PF_IsThermalSensor(IPC_SNS_TYPE_E sns){
    switch(sns){
        case TIMO256_DVP_256_IR:
        case ELF1_DVP_256_IR:
        case NK1221A_DVP_384_IR:
            return 1;

        default:
            return 0;
    }
}

void setRawProcessParam(IPC_SNS_TYPE_E sns){
    ar_memset(&g_RawProcessParam, sizeof(STRU_IR_RAW_PROCESS_PARAM_T), 0, sizeof(STRU_IR_RAW_PROCESS_PARAM_T));
    g_RawProcessParam.ViPipe_isp = -1;
    g_RawProcessParam.ViChn_isp = -1;
    if(sns == TIMO256_DVP_256_IR){
        g_RawProcessParam.RawWidth = 512;
        g_RawProcessParam.RawHeight = 200;
        g_RawProcessParam.ImgWidth = 256;
        g_RawProcessParam.ImgHeight = 192;

    }else if(sns == ELF1_DVP_256_IR){
        g_RawProcessParam.RawWidth = 520;
        g_RawProcessParam.RawHeight = 204;
        g_RawProcessParam.ImgWidth = 256;
        g_RawProcessParam.ImgHeight = 192;

    }else if(sns == NK1221A_DVP_384_IR){
        g_RawProcessParam.RawWidth = 768;
        g_RawProcessParam.RawHeight = 292;
        g_RawProcessParam.ImgWidth = 768;
        g_RawProcessParam.ImgHeight = 292;

    }
}

void setRawProcessParamISP(int pipe_isp, int ch_isp){
    g_RawProcessParam.ViPipe_isp = pipe_isp;
    g_RawProcessParam.ViChn_isp = ch_isp;
    stYuvProcessParam.ViPipe_isp = pipe_isp;
    stYuvProcessParam.ViChn_isp = ch_isp;
    stYuvProcessParam.VpssGrp_inf = pipe_isp;
    stYuvProcessParam.VpssChn_inf =  ch_isp;
}
void setRawProcessParamRaw(int pipe_raw, int ch_raw){
    g_RawProcessParam.ViPipe_raw = pipe_raw;
    g_RawProcessParam.ViChn_raw = ch_raw;
}

void setYuvProcessParamVis(int pipe_vis, int ch_vis, int grp_vis, int grpch_vis){
    stYuvProcessParam.ViPipe_vis = pipe_vis;
    stYuvProcessParam.ViChn_vis = ch_vis;
    stYuvProcessParam.VpssGrp_vis = grp_vis;
    stYuvProcessParam.VpssChn_vis =  grpch_vis;
}

int InitVbBuffer(STRU_VB_BUFFER_T* vb_buff)
{
    if(vb_buff->size<=0){
        return -1;
    }
    vb_buff->vb_blk=AR_MPI_VB_GetBlock(VB_INVALID_POOLID,vb_buff->size,NULL);
    AR_MPI_VB_MmapPool(AR_MPI_VB_Handle2PoolId(vb_buff->vb_blk));
    vb_buff->phy_addr=AR_MPI_VB_Handle2PhysAddr(vb_buff->vb_blk);
    vb_buff->p_vaddr=NULL;
    AR_MPI_VB_GetBlockVirAddr(AR_MPI_VB_Handle2PoolId(vb_buff->vb_blk),vb_buff->phy_addr,&vb_buff->p_vaddr);

    printf("vb_blk=%d phy_addr=%p  p_vaddr=%p raw_size=%lu\n",vb_buff->vb_blk,(void *)vb_buff->phy_addr,vb_buff->p_vaddr,vb_buff->size);

    if(!vb_buff->phy_addr || !vb_buff->p_vaddr)
    {
       ar_err("vb too small");
       return -1;
    }

    vb_buff->p_vaddr_align= (void*)CAM_ALIGNE_TO((AR_U64)vb_buff->p_vaddr,256);
    vb_buff->phy_addr_align=CAM_ALIGNE_TO(vb_buff->phy_addr,256);

    return 0;
}

static void dump_frame_info(VIDEO_FRAME_INFO_S* pstFrame,const char* name)
{
    printf("[%s] -> w=%u h=%u pformat=%d stride=[%u %u %u] addr=[%p %p %p] TimeRef=%u pts=%llu phy_addr = %llu\n",
        name,
        pstFrame->stVFrame.u32Width,pstFrame->stVFrame.u32Height,
        pstFrame->stVFrame.enPixelFormat,
        pstFrame->stVFrame.u32Stride[0],pstFrame->stVFrame.u32Stride[1],pstFrame->stVFrame.u32Stride[2],
        (AR_U64*)pstFrame->stVFrame.u64PhyAddr[0],(AR_U64*)pstFrame->stVFrame.u64PhyAddr[1],(AR_U64*)pstFrame->stVFrame.u64PhyAddr[2],
        pstFrame->stVFrame.u32TimeRef,pstFrame->stVFrame.u64PTS, pstFrame->stVFrame.u64PhyAddr[0]);
}
#ifdef YUV_PROCESS_ENABLE
int init_inf_alg_obj()
{
    int ret;
#if (THERMAL_TYPE == THERMAL_TYPE_IRAY_ELF)
    INF_ALGO_ALL_PARAM_S stInfAlgParams = {
        .stGtmParam = {
            .upThrowPointProportion = 0.05,
            .downThrowPointProportion = 0.05,
            .target_mean_value = 320,
            .contrastBaseUp = 2,
            .contrastBaseDown = 1,
            .compensate1 = 100,
            .compensate2 = 100,
        },
        .stLmsParam = {
            .enable_horizontal = 0,
            .prob = 0.8,
            .RaduisVL = 8,
            .RaduisHL = 8,
            .uStepVL = 0.35,
            .uStepHL = 0.35,
            .skipFrames = 0
        },
        .stColorPaletteParam = {
            .ColorPaletteFilePath = "/usrdata/iray_demo/iraycfg/ColorMapV2_3.dat",
            .palette_index = 5,
        },
        .stFusionParam = {
            .src_alpha = 50,
            .fusion_mode = FUSION_MODE_0,
        },
    };
    INF_ALGO_PRI_CONFIG_S stPriConfig = {
        .width = 256,
        .height = 192,
        .lms_out_bitwidth = 14,
        .gtm_out_bitwidth = 10,
    };
    ret = inf_algo_create(&g_pstInfAlgObject, "/usrdata/iray_demo/params.json",&stInfAlgParams,&stPriConfig,NULL);
#elif (THERMAL_TYPE == THERMAL_TYPE_GD_TIMO256)
    INF_ALGO_ALL_PARAM_S stInfAlgParams = {
        .stGtmParam = {
            .upThrowPointProportion = 0.01,
            .downThrowPointProportion = 0.01,
            .target_mean_value = 390,
            .contrastBaseUp = 2,
            .contrastBaseDown = 1,
            .compensate1 = 100,
            .compensate2 = 300,
        },
        .stLmsParam = {
            .enable_horizontal = 0,
            .prob = 0.8,
            .RaduisVL = 8,
            .RaduisHL = 8,
            .uStepVL = 0.35,
            .uStepHL = 0.35,
            .skipFrames = 0
        },
        .stColorPaletteParam = {
            .ColorPaletteFilePath = "/usrdata/guide_demo/ColorMapV2_3.dat",
            .palette_index = 5,
        },
        .stFusionParam = {
            .src_alpha = 50,
            .fusion_mode = FUSION_MODE_0,
        },

    };
    INF_ALGO_PRI_CONFIG_S stPriConfig = {
        .width = 256,
        .height = 192,
        .lms_out_bitwidth = 14,
        .gtm_out_bitwidth = 10,
    };
    ret = inf_algo_create(&g_pstInfAlgObject, "/usrdata/guide_demo/params.json",&stInfAlgParams,&stPriConfig,NULL);
#endif

    return ret;
}

void *stream_yuv_process_thread(void* arg)
{
    printf("yuv_process_thread start \n");
    AR_S32 s32Ret;
    STRU_YUV_PROCESS_PARAM_T* pstYuvProcessParam = (STRU_YUV_PROCESS_PARAM_T*)arg;
    VPSS_GRP VpssGrp_vis = pstYuvProcessParam->VpssGrp_vis;
    VPSS_CHN VpssChn_vis = pstYuvProcessParam->VpssChn_vis;
    VPSS_GRP VpssGrp_inf = pstYuvProcessParam->VpssGrp_inf;
    VPSS_CHN VpssChn_inf = pstYuvProcessParam->VpssChn_inf;

    sleep(5);
    int frame_count = 0;
    while(1)
    {
        frame_count ++;
        VIDEO_FRAME_INFO_S irFrame = {0};
        //printf("yuv_process_thread frame count %d \n", frame_count);
        //s32Ret = AR_MPI_VI_GetChnFrame(pstYuvProcessParam->ViPipe_isp,pstYuvProcessParam->ViChn_isp,&irFrame,-1);
        s32Ret = AR_MPI_VPSS_GetChnFrame(VpssGrp_inf, VpssChn_inf, &irFrame, -1);
        if(s32Ret!=0)
        {
            //printf("AR_MPI_VI_GetChnFrame pipe=%d chn=%d failed\n",pstYuvProcessParam->ViPipe_isp,pstYuvProcessParam->ViChn_isp);
            printf("get infrared frame failed\n");
            //break;
            sleep(1);
            continue;
        }
        //printf("yuv_process_thread frame get suc count %d \n", frame_count);
#ifdef DUMP_DATA
        if(frame_count == debug_count){
            dump_frame_info(&irFrame, "abc");
            char dump_file[128];
            sprintf(dump_file, "/usrdata/vpss_yuv_%d.i420", frame_count);
            dump_yuv_to_file(dump_file, irFrame.stVFrame.u64VirAddr[0], irFrame.stVFrame.u64VirAddr[1], irFrame.stVFrame.u64VirAddr[2],
                irFrame.stVFrame.u32Width, irFrame.stVFrame.u32Height,
                irFrame.stVFrame.u32Stride[0], irFrame.stVFrame.u32Stride[1], irFrame.stVFrame.u32Stride[2]);
        }
#endif

        g_pstInfAlgObject->mapping_color_process(g_pstInfAlgObject,
                (void*)irFrame.stVFrame.u64VirAddr[0], (void*)irFrame.stVFrame.u64VirAddr[1], (void*)irFrame.stVFrame.u64VirAddr[2],
                irFrame.stVFrame.u32Width, irFrame.stVFrame.u32Height,
                irFrame.stVFrame.u32Stride[0], irFrame.stVFrame.u32Stride[1], irFrame.stVFrame.u32Stride[2]);

        AR_MPI_VENC_SendFrame(g_RawProcessParam.ViPipe_raw, &irFrame, 0);

#if 1
        if(VpssGrp_vis > -1){
            static AR_BOOL isFusionInit = AR_FALSE;
            if(!isFusionInit){
                s32Ret =  g_pstInfAlgObject->fusion_init(g_pstInfAlgObject,
                        irFrame.stVFrame.u32Width,irFrame.stVFrame.u32Height,
                        irFrame.stVFrame.u32Stride[0],irFrame.stVFrame.u32Stride[1]);
                if(s32Ret!=0){
                    printf("fusion_init failed\n");
                    AR_MPI_VPSS_ReleaseChnFrame(VpssGrp_inf, VpssChn_inf, &irFrame);
                    sleep(1);
                    continue;
                }
                else{
                    isFusionInit = AR_TRUE;
                }
            }

            VIDEO_FRAME_INFO_S visFrame = {0};
            s32Ret = AR_MPI_VPSS_GetChnFrame(VpssGrp_vis, VpssChn_vis, &visFrame, -1);
            if(s32Ret!=0){
                printf("get visible frmae failed\n");
                AR_MPI_VPSS_ReleaseChnFrame(VpssGrp_inf, VpssChn_inf, &irFrame);
                sleep(1);
                continue;
            }

            size_t uv_len = visFrame.stVFrame.u32Height*visFrame.stVFrame.u32Stride[1];
            ar_memset((void*)visFrame.stVFrame.u64VirAddr[1], uv_len, 128, uv_len);
            ar_memset((void*)visFrame.stVFrame.u64VirAddr[2], uv_len, 128, uv_len);

            VIDEO_FRAME_INFO_S fusionFrame = {0};
            s32Ret = g_pstInfAlgObject->fusion_process(g_pstInfAlgObject,&irFrame,&visFrame,&fusionFrame);
            if(s32Ret==0){
                AR_MPI_VENC_SendFrame(g_RawProcessParam.ViPipe_isp + 1, &fusionFrame, 0);
            } else {
                printf("fusion frame failed !\n");
            }

            s32Ret = AR_MPI_VPSS_ReleaseChnFrame(VpssGrp_vis, VpssChn_vis, &visFrame);
        }
#endif
        s32Ret = AR_MPI_VPSS_ReleaseChnFrame(VpssGrp_inf, VpssChn_inf, &irFrame);

    }
    return 0;
}
#endif

static void *stream_buf_dq_thread(void* arg)
{
	printf("dq_loop_thread start");
    STRU_IR_RAW_PROCESS_PARAM_T *p_raw_obj=(STRU_IR_RAW_PROCESS_PARAM_T *)arg;
	while(1)
	{
	    AR_S32 ret;
        VIDEO_FRAME_INFO_S VideoFrame;
        ret = AR_MPI_VI_DqPipeRaw(p_raw_obj->ViPipe_isp,&VideoFrame,-1);
        if(ret!=0)
        {
            ar_err("AR_MPI_VI_DqPipeRaw failed\n");
            break;
        }
        STRU_VB_BUFFER_T* p_buff = (STRU_VB_BUFFER_T*)VideoFrame.stVFrame.u64PrivateData;
		p_raw_obj->stream_buffer_pool->queue_insert(p_raw_obj->stream_buffer_pool,p_buff);
	}
	return 0;
}
#if (THERMAL_TYPE == THERMAL_TYPE_IRAY_ELF)
int onGetNucData(unsigned short *data,int len)
{
	//printf("get nuc data %p %d \n",data,len);
    AR_S32 ret;
    int64_t tbegin;

    STRU_VB_BUFFER_T *p_buff=NULL;
	g_RawProcessParam.stream_buffer_pool->queue_pop(g_RawProcessParam.stream_buffer_pool,(void**)&p_buff);
    if(!p_buff)
    {
        printf("stream_buffer_pool no buffer\n");
        return 0;
    }

    //printf(" onGetNucData ------------------------------------\n");

    static int      isDoLms = 1;
    static int      isPrintTimeLog  = 0;

    if(isDoLms){
        tbegin = getTickCount();
        g_pstInfAlgObject->lms_process(g_pstInfAlgObject,data,data);
        if(isPrintTimeLog) printf("lms time: %.2f ms.\n", getTickInterval(tbegin)/1000000.0);
    }

    static uint64_t ii = 0;
    ii++;
    tbegin = getTickCount();

	g_pstInfAlgObject->gtm_process(g_pstInfAlgObject,data,p_buff->p_vaddr_align);
    if(isPrintTimeLog) printf("global_tone_mapping time: %.2f ms.\n", getTickInterval(tbegin)/1000000.0);
#ifdef DUMP_DATA
    if(save_y16 && ii == debug_count){
		dump_to_file("/tmp/y16.raw",data,len*2);
	}

	if(save_tp && ii == debug_count){
		dump_to_file("/tmp/tp.raw",p_buff->p_vaddr_align,len*2);
	}
#endif
    VIDEO_FRAME_INFO_S VideoFrame;
	VIDEO_FRAME_INFO_S *pstVideoFrame=&VideoFrame;
	pstVideoFrame->stVFrame.u64PhyAddr[0]=(AR_U64)p_buff->phy_addr_align;
	pstVideoFrame->stVFrame.u64PhyAddr[1]=0;
	pstVideoFrame->stVFrame.u64PhyAddr[2]=0;
	pstVideoFrame->stVFrame.u64VirAddr[0]=(AR_U64)p_buff->p_vaddr_align;
	pstVideoFrame->stVFrame.u64VirAddr[1]=0;
	pstVideoFrame->stVFrame.u64VirAddr[2]=0;
	pstVideoFrame->stVFrame.u32Stride[0]=256*2;
	pstVideoFrame->stVFrame.u32Stride[1]=0;
	pstVideoFrame->stVFrame.u32Stride[2]=0;

	pstVideoFrame->stVFrame.u32Width=256;
	pstVideoFrame->stVFrame.u32Height=192;
	pstVideoFrame->stVFrame.u64PTS=0;
	pstVideoFrame->stVFrame.u64PrivateData=0;
	pstVideoFrame->u32PoolId=VB_INVALID_POOLID;

    pstVideoFrame->stVFrame.u64PrivateData = (AR_U64)p_buff;

    ret = AR_MPI_VI_QPipeRaw(g_RawProcessParam.ViPipe_isp,pstVideoFrame);

    if(ret<0)

	{
		printf("AR_MPI_VI_SendPipeRaw failed exit the thread\n");
	}

	return 0;
}
#endif
#if (THERMAL_TYPE == THERMAL_TYPE_GD_TIMO256)
int onGetGdSdkY16Data(short *data,int len)
{
	//printf("get nuc data %p %d \n",data,len);
    AR_S32 ret;
    int64_t tbegin;

    STRU_VB_BUFFER_T *p_buff=NULL;
	g_RawProcessParam.stream_buffer_pool->queue_pop(g_RawProcessParam.stream_buffer_pool, (void**)&p_buff);
    if(!p_buff)
    {
        printf("stream_buffer_pool no buffer\n");
        return 0;
    }

	int i;
    for(i=0;i<len;i++){
        data[i]+=5000;
    }


    static int      isDoLms = 1;
    static int      isPrintTimeLog  = 0;

    //printf("trace : %s %d : get y16 data \n", __FUNCTION__, __LINE__);
    if(isDoLms){
        tbegin = getTickCount();
        g_pstInfAlgObject->lms_process(g_pstInfAlgObject,data,data);
        if(isPrintTimeLog) printf("lms time: %.2f ms.\n", getTickInterval(tbegin)/1000000.0);
    }

    static uint64_t ii = 0;
    ii++;

    tbegin = getTickCount();
    g_pstInfAlgObject->gtm_process(g_pstInfAlgObject,data,p_buff->p_vaddr_align);
    if(isPrintTimeLog) printf("global_tone_mapping time: %.2f ms.\n", getTickInterval(tbegin)/1000000.0);
#ifdef DUMP_DATA
    if(save_y16){
		save_y16 = 0;
		dump_to_file("/tmp/tp.raw",p_buff->p_vaddr_align,len*2);
	}

	if(save_tp){
		save_tp = 0;
		dump_to_file("/tmp/tp.raw",p_buff->p_vaddr_align,len*2);
	}
#endif
    //printf("trace : %s %d : get y16 data \n", __FUNCTION__, __LINE__);
#ifdef RAW_DATA_FROM_FILE
    unsigned char* tmp_buf = NULL;
    tmp_buf = (unsigned char*)malloc(512*200);
    read_from_file("/usrdata/y16.raw", tmp_buf, 512*192);
    memcpy(p_buff->p_vaddr_align, tmp_buf, 512*192);
    free(tmp_buf);
#endif

    VIDEO_FRAME_INFO_S VideoFrame;
	VIDEO_FRAME_INFO_S *pstVideoFrame=&VideoFrame;
	pstVideoFrame->stVFrame.u64PhyAddr[0]=(AR_U64)p_buff->phy_addr_align;
	pstVideoFrame->stVFrame.u64PhyAddr[1]=0;
	pstVideoFrame->stVFrame.u64PhyAddr[2]=0;
	pstVideoFrame->stVFrame.u64VirAddr[0]=(AR_U64)p_buff->p_vaddr_align;
	pstVideoFrame->stVFrame.u64VirAddr[1]=0;
	pstVideoFrame->stVFrame.u64VirAddr[2]=0;
	pstVideoFrame->stVFrame.u32Stride[0]=256*2;
	pstVideoFrame->stVFrame.u32Stride[1]=0;
	pstVideoFrame->stVFrame.u32Stride[2]=0;

	pstVideoFrame->stVFrame.u32Width=256;
	pstVideoFrame->stVFrame.u32Height=192;
	pstVideoFrame->stVFrame.u64PTS=0;
	pstVideoFrame->stVFrame.u64PrivateData=0;
	pstVideoFrame->u32PoolId=VB_INVALID_POOLID;

    pstVideoFrame->stVFrame.u64PrivateData = (AR_U64)p_buff;

    ret = AR_MPI_VI_QPipeRaw(g_RawProcessParam.ViPipe_isp,pstVideoFrame);

	if(ret<0)
	{
		printf("AR_MPI_VI_SendPipeRaw failed exit the thread\n");
	}

	return 0;
}
#endif

void thermal_init(){
    if(g_RawProcessParam.ViPipe_isp != -1){
        g_RawProcessParam.stream_buffer_pool = ar_hal_sys_creat_queue(5, "isp_raw_stream_buf_pool");
        for(int i=0;i<3;i++){
            g_RawProcessParam.astIspRawBuffer[i].size = g_RawProcessParam.RawWidth * g_RawProcessParam.RawHeight*2;
            if(InitVbBuffer(&g_RawProcessParam.astIspRawBuffer[i])<0)
            {
                printf("InitVbBuffer failed\n");
                return;
            }
            g_RawProcessParam.stream_buffer_pool->queue_insert(g_RawProcessParam.stream_buffer_pool,
                                                                &g_RawProcessParam.astIspRawBuffer[i]);
        }

        pthread_t raw_dq_thread;
        pthread_create(&raw_dq_thread, NULL, stream_buf_dq_thread, &g_RawProcessParam);
    }
#if (THERMAL_TYPE == THERMAL_TYPE_IRAY_ELF)
    int s32Ret = iray_init();
    if(s32Ret<0){
        printf("iray init fail.\n");
        return;
    }
    iray_set_nuc_callback(onGetNucData);
#elif (THERMAL_TYPE == THERMAL_TYPE_GD_TIMO256)
    printf("guide_init \n");
    int s32Ret = guide_init();
    if(s32Ret<0){
        printf("guide_init fail.\n");
        return;
    }
    guide_set_y16_callback(onGetGdSdkY16Data);
#endif

#ifdef YUV_PROCESS_ENABLE
    //start yuv process
    pthread_t yuv_process_thread;
    //if(s32OutMode==1)
    {
        s32Ret = pthread_create(&yuv_process_thread, NULL, stream_yuv_process_thread, &stYuvProcessParam);
    }

    int ret = init_inf_alg_obj();
    if(ret!=0){
        printf("inf_algo_create fail.\n");
        return;
    }
#endif
}

//#define RAW_DATA_FROM_FILE 1

void * thermal_process(void *args){
    AR_U32 u32RawIndex = 0;
    unsigned char* tmp_buf = NULL;
    int s32Ret = -1;

#if (THERMAL_TYPE == THERMAL_TYPE_IRAY_ELF)
    tmp_buf = (unsigned char*)malloc(512*204);
#elif (THERMAL_TYPE == THERMAL_TYPE_GD_TIMO256)
    tmp_buf = (unsigned char*)malloc(512*200);
#else
    tmp_buf = (unsigned char*)malloc(384*292*2);
#endif

    while(1)
    {

        u32RawIndex++;
#ifdef RAW_DATA_FROM_FILE
        read_from_file("/usrdata/y16_512x200.raw",tmp_buf,512*200);
        guide_push_raw_data(tmp_buf, 512*200);
        usleep(40*1000);
#else
//        printf("get frame ----- %d \n", u32RawIndex);
        VIDEO_FRAME_INFO_S *pstFrameInfo=malloc(sizeof(VIDEO_FRAME_INFO_S));
		s32Ret = AR_MPI_VI_GetChnFrame(g_RawProcessParam.ViPipe_raw,g_RawProcessParam.ViChn_raw,pstFrameInfo,500000);
        if(s32Ret!=0)
        {
            ar_err("AR_MPI_VI_GetChnFrame failed. pipe=%d chn=%d ret=%d", g_RawProcessParam.ViPipe_raw,g_RawProcessParam.ViChn_raw,s32Ret);
            continue;
        }
//        printf("get frame sus----- %d \n", u32RawIndex);

#if (THERMAL_TYPE == THERMAL_TYPE_IRAY_ELF)
        int h;
        unsigned char* p_data_src = (unsigned char*)(pstFrameInfo->stVFrame.u64VirAddr[0]+256);
        for(h=0;h<203;h++){
            unsigned char* p_dest = tmp_buf+h*512;
            unsigned char* p_src1 = p_data_src+h*pstFrameInfo->stVFrame.u32Stride[0]+2;
            memcpy(p_dest,p_src1,512);
        }
#ifdef DUMP_DATA
        if(save_raw && u32RawIndex == debug_count){
            //save_raw = 0;
            printf("will save raw, size: %u %u stride: %u %u %u valid: 256*204\n",
                 pstFrameInfo->stVFrame.u32Width, pstFrameInfo->stVFrame.u32Height,
                 pstFrameInfo->stVFrame.u32Stride[0],pstFrameInfo->stVFrame.u32Stride[1],pstFrameInfo->stVFrame.u32Stride[2]);
            char ori_name[128];
            sprintf(ori_name, "/tmp/ori_%d.raw", u32RawIndex);
            dump_to_file(ori_name,pstFrameInfo->stVFrame.u64VirAddr[0],pstFrameInfo->stVFrame.u32Stride[0]*pstFrameInfo->stVFrame.u32Height);
            sprintf(ori_name, "/tmp/ori-valid_%d.raw", u32RawIndex);
            dump_to_file(ori_name ,tmp_buf,512*204);
        }

        if((access("/tmp/dump.txt",F_OK))!=-1)
        {
            remove("/tmp/dump.txt");
            printf("will dump..\n");
            save_y16 = save_tp = save_raw = 1;
        }
#endif
        iray_push_raw_data(tmp_buf, 512*204);
#elif (THERMAL_TYPE == THERMAL_TYPE_GD_TIMO256)
#ifdef DUMP_DATA
        if(save_raw){
            save_raw = 0;
            printf("will save raw, info: %u %u %u %u %u %p %p %p\n",
                pstFrameInfo->stVFrame.u32Width, pstFrameInfo->stVFrame.u32Height,
                pstFrameInfo->stVFrame.u32Stride[0],pstFrameInfo->stVFrame.u32Stride[1],pstFrameInfo->stVFrame.u32Stride[2],
                pstFrameInfo->stVFrame.u64VirAddr[0],pstFrameInfo->stVFrame.u64VirAddr[1],pstFrameInfo->stVFrame.u64VirAddr[2]);

            dump_to_file("/tmp/ori.raw",pstFrameInfo->stVFrame.u64VirAddr[0],pstFrameInfo->stVFrame.u32Stride[0]*pstFrameInfo->stVFrame.u32Height);
        }
        if(u32RawIndex%50==0){
            if((access("/tmp/dump.txt",F_OK))!=-1)
            {
                remove("/tmp/dump.txt");
                printf("will dump..\n");
                save_y16 = save_tp = save_raw = 1;
            }
            if((access("/tmp/dumpy16.txt",F_OK))!=-1)
            {
                remove("/tmp/dumpy16.txt");
                printf("will dumpy16..\n");
                save_y16_data(100);
            }
        }
#endif
        unsigned short *ori = (unsigned short *)pstFrameInfo->stVFrame.u64VirAddr[0];
        unsigned short *dst = (unsigned short *)tmp_buf;

        for (int i=0;i<256*200;i++) {
            dst[i] = (ori[i] << 8) | (ori[i] >> 8);
        }
        //printf("trace : %s %d : push data \n", __FUNCTION__, __LINE__);
        //int64_t tbegin = getTickCount();
        guide_push_raw_data(tmp_buf, 512*200);
        //printf("IRDataFunc time: %.2f ms.\n", getTickInterval(tbegin)/1000000.0);
#else
#ifdef DUMP_DATA
        if((u32RawIndex - 500 < 10) && (u32RawIndex - 500 > 0)){
            printf("will save raw, info: %u %u %u %u %u %p %p %p\n",
                pstFrameInfo->stVFrame.u32Width, pstFrameInfo->stVFrame.u32Height,
                pstFrameInfo->stVFrame.u32Stride[0],pstFrameInfo->stVFrame.u32Stride[1],pstFrameInfo->stVFrame.u32Stride[2],
                pstFrameInfo->stVFrame.u64VirAddr[0],pstFrameInfo->stVFrame.u64VirAddr[1],pstFrameInfo->stVFrame.u64VirAddr[2]);
            memcpy(tmp_buf, pstFrameInfo->stVFrame.u64VirAddr[0], pstFrameInfo->stVFrame.u32Stride[0]*pstFrameInfo->stVFrame.u32Height);
            char name_str[128];
            sprintf(name_str, "/tmp/ori_%d.raw", u32RawIndex);
            dump_to_file(name_str,tmp_buf,pstFrameInfo->stVFrame.u32Stride[0]*pstFrameInfo->stVFrame.u32Height);
        }
#endif
#endif
        AR_MPI_VI_ReleaseChnFrame(g_RawProcessParam.ViPipe_raw,g_RawProcessParam.ViChn_raw,pstFrameInfo);
        free(pstFrameInfo);
#endif

    }

    free(tmp_buf);
    return NULL;
}


int IPC_PF_THERMAL_Start(void)
{
    thermal_init();
    IPC_MID_UTIL_CreateDetachThread(thermal_process, NULL, NULL);
    return 0;
}
#endif //THERMAL_TYPE

/************************************************************************************************************
 * The following code is for AR9311/ARS31 chips, all process will be wrapped into a object VI group.
 * The basic process is :
 * get raw -> occ -> send to thermal_isp -> get frame -> call gtm/3dnr on A53 -> send to isp -> get frame and bind to others.
 *************************************************************************************************************/

#ifndef THERMAL_TYPE
INF_ALGO_OBJECT_S* g_pstInfAlgObject_ars31 = NULL;
STRU_MMZ_YUV_BUFFER_T gColorMapTmpBuf = {0};
AR_BOOL bFussionRun = AR_FALSE;
static pthread_t g_fussionPid;
#endif

extern VI_IR_GROUP_S Gst417wGrpCfg;
extern VI_IR_GROUP_S Gst212wGrpCfg;
extern VI_IR_GROUP_S H3812c1sgGrpCfg;


extern AR_S32 AR_VI_CreateSingleVi(VI_DEV ViDev, VI_PIPE ViPipe, int mipiDev,
              IPC_SNS_TYPE_E enSensorType, WDR_MODE_E enWDRMode, ENUM_VFE_MODE enVfe,
              AR_CHAR * strPowerGpio, AR_CHAR * strResetGpio, AR_CHAR * strCommGpio, AR_BOOL bRawCompress);
extern AR_S32 AR_VI_CreateSingleIsp(VI_DEV ViDev, VI_PIPE ViPipe,IPC_SNS_TYPE_E enSnsType, AR_U32 u32BusId, WDR_MODE_E enWDRMode, AR_U32 u32Fps);

static VI_IR_GROUP_S * GetViIRGroupBySnsType(IPC_SNS_TYPE_E enSnsType)
{
    switch(enSnsType)
    {
        case GST417W_IR_400X308:
            return &Gst417wGrpCfg;
        case GST212W4_IR_256X200:
            return &Gst212wGrpCfg;
        case H3812C1SG_IR_444X336:
            return &H3812c1sgGrpCfg;
        default:
            break;
    }

    return NULL;
}

static AR_S32 InitViGrpVb(VI_IR_GROUP_S * pstViIRGrp)
{
    AR_U32 u32BlkSize_Raw = 0, u32BlkSize_Thermal = 0, u32BlkSize_Isp0 = 0;
    AR_U32 u32BlkSize_Max = 0;
    VB_POOL vbPoolId = -1;
    VB_POOL_CONFIG_S stVbPoolCfg;

    memset(&stVbPoolCfg, 0, sizeof(VB_POOL_CONFIG_S));

    u32BlkSize_Raw = VI_GetRawBufferSize(pstViIRGrp->stViSns.stOutSize.u32Width,
                 pstViIRGrp->stViSns.stOutSize.u32Height, pstViIRGrp->stViSns.enOutPixFmt, COMPRESS_MODE_NONE, 256);

    u32BlkSize_Isp0 = COMMON_GetPicBufferSize(pstViIRGrp->stViIsp0.stOutSize.u32Width,
                 pstViIRGrp->stViIsp0.stOutSize.u32Height, pstViIRGrp->stViIsp0.enOutPixFmt,DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);

    u32BlkSize_Thermal = VI_GetRawBufferSize(pstViIRGrp->stViThermal.stOutSize.u32Width,
                 pstViIRGrp->stViThermal.stOutSize.u32Height, pstViIRGrp->stViThermal.enOutPixFmt, COMPRESS_MODE_NONE, 256);


    u32BlkSize_Max = u32BlkSize_Raw > u32BlkSize_Thermal ? u32BlkSize_Raw : u32BlkSize_Thermal;
    u32BlkSize_Max = u32BlkSize_Max > u32BlkSize_Isp0 ? u32BlkSize_Max : u32BlkSize_Isp0;

    stVbPoolCfg.u64BlkSize = u32BlkSize_Max + 8192;
    stVbPoolCfg.u32BlkCnt = 5 * 3;
     // create VB Pool
    vbPoolId = AR_MPI_VB_CreatePool(&stVbPoolCfg);
    if(vbPoolId < 0)
    {
        PRINT_ERR("Create vb pool for vi ir group failed.\n");
        return -1;
    }

    return 0;
}

static AR_S32 StartViIRGroup(VI_IR_GROUP_S * pstViIRGrp)
{
    AR_S32 i = 0;
    AR_S32 s32Ret = 0;
    AR_S32 s32ViCnt = 0;
    AR_S32 s32BusId = 0;
    AR_S32 s32Fps = 0;
    VI_DEV ViDev = 0;
    VI_PIPE ViPipe = 0;
    VI_CHN ViChn = 0;
    MIPI_DEV MipiDev = 0;
    IPC_SNS_TYPE_E enSnsType = 0;
    VI_WRAPPER_S * pstViWrapper = NULL;
    IPC_CFG_VIO_S * pstVioCfg = IPC_CFG_VIO_GetParam();
    VI_CHN_ATTR_S stChnAttr = {0};

    s32ViCnt = sizeof(VI_IR_GROUP_S)/sizeof(VI_WRAPPER_S);
    PRINT_INFO("Start IR group...\n");
    pstViWrapper = (VI_WRAPPER_S *)pstViIRGrp;

    for(i = 0; i < s32ViCnt; i++)
    {
        if(pstViWrapper->enSnsType == IPC_SNS_TYPE_BUTT)
        {
            continue;
        }

        ViDev = pstViWrapper->ViDev;
        ViPipe = pstViWrapper->ViPipe;
        ViChn = pstViWrapper->ViChn;
        MipiDev = pstViWrapper->s32MipiDev;

        s32Ret= AR_MPI_VI_SetMipiBindDev(ViDev, MipiDev); //this must refer to the hardware design
        if(s32Ret)
        {
            PRINT_ERR("Mipi bind dev %d/%d error!\n", ViDev, MipiDev);
            return -1;
        }

        s32Fps = pstViWrapper->s32Fps;
        s32BusId = pstViWrapper->s32BusId;
        enSnsType = pstViWrapper->enSnsType;
        PRINT_INFO("Init dev %d pipe %d chn %d mipidev %d sns type: %d\n", ViDev, ViPipe, ViChn, MipiDev, enSnsType);

        s32Ret = AR_VI_CreateSingleVi(ViDev, ViPipe, MipiDev, enSnsType, WDR_MODE_NONE, VFE_MODE_NULL,
                pstViWrapper->achPowerGPIO, pstViWrapper->achResetGPIO, pstViWrapper->achCommGPIO, AR_FALSE);
        if (AR_SUCCESS != s32Ret)
        {
            return s32Ret;
        }

        PRINT_INFO("Init dev %d pipe %d chn %d mipidev %d done \n", ViDev, ViPipe, ViChn, MipiDev);
        pstViWrapper++;
    }

    //start chn
    pstViWrapper = &pstViIRGrp->stViSns;
    for(i = 0; i < s32ViCnt; i++)
    {
        if(pstViWrapper->enSnsType == IPC_SNS_TYPE_BUTT)
        {
            continue;
        }

        ViDev = pstViWrapper->ViDev;
        ViPipe = pstViWrapper->ViPipe;
        ViChn = pstViWrapper->ViChn;
        MipiDev = pstViWrapper->s32MipiDev;
        s32Fps = pstViWrapper->s32Fps;
        s32BusId = pstViWrapper->s32BusId;
        enSnsType = pstViWrapper->enSnsType;

        PRINT_INFO("Enable pipe %d chn %d \n", ViPipe, ViChn);

        //enable chn
        IPC_PF_VI_GetChnAttrBySns(enSnsType, &stChnAttr);
        stChnAttr.u32Depth = 5;
        stChnAttr.u32BufCount = pstVioCfg->vi_cfg.vb_blk_cnt;
        stChnAttr.u32DepthClient[0] = 1; //for bind
        stChnAttr.u32DepthClient[1] = 3; //for getframe
        for(int cn = 2; cn < 8; cn++)
        {
            stChnAttr.u32DepthClient[cn] = 1;
        }

        s32Ret = AR_MPI_VI_SetChnAttr(ViPipe, ViChn, &stChnAttr);
        if (s32Ret != AR_SUCCESS)
        {
            PRINT_ERR("AR_MPI_VI_SetChnAttr pipe %d chn %d failed with %#x!\n", ViPipe, ViChn ,s32Ret);
            return AR_FAILURE;
        }

        //enMastPipeMode = pstViInfo->stPipeInfo.enMastPipeMode;
        s32Ret = AR_MPI_VI_EnableChn(ViPipe, ViChn);
        if (s32Ret != AR_SUCCESS)
        {
            PRINT_ERR("Enable pipe %d chn %d failed !\n", ViPipe, ViChn);
            return s32Ret;
        }

        s32Ret = AR_VI_CreateSingleIsp(ViDev, ViPipe, enSnsType, s32BusId, WDR_MODE_NONE, s32Fps);
        if (AR_SUCCESS != s32Ret)
        {
            return s32Ret;
        }
        pstViWrapper++;
    }

    //we'll open the shutter after kb calib at the beginning
    IrCloseShutter();

    //Set init params: sns, occ, kb
    IrLoadAndApplyAllParam(pstViIRGrp);

    //create thread to process frames from sns -> thermal -> isp0 -> isp1
    pstViWrapper = &pstViIRGrp->stViIsp1;
    for(i = 0; i < s32ViCnt; i++)
    {
        if(pstViWrapper->pfnThreadFunc)
        {
        	pstViWrapper->bIrfnThreadStart = AR_TRUE;
            s32Ret = pthread_create(&pstViWrapper->threadPid, NULL, pstViWrapper->pfnThreadFunc, pstViWrapper);
            if (s32Ret)
            {
                PRINT_ERR("create ir out process thread failed\n");
				pstViWrapper->bIrfnThreadStart = AR_FALSE;
                return s32Ret;
            }
        }

        if(pstViWrapper->pfnThreadDeqFunc)
        {
        	pstViWrapper->bIrDeThreadStart = AR_TRUE;
            s32Ret = pthread_create(&pstViWrapper->threadDeqPid, NULL, pstViWrapper->pfnThreadDeqFunc, pstViWrapper);
            if (s32Ret)
            {
                PRINT_ERR("create ir dequeue thread failed\n");
				pstViWrapper->bIrDeThreadStart = AR_FALSE;
                return s32Ret;
            }
        }

        pstViWrapper--;
    }

    if (pstViIRGrp->stViSns.enSnsType != IPC_SNS_TYPE_BUTT) {
        pstViIRGrp->stViSns.bIrDynamicCalibThreadStart = AR_TRUE;
        s32Ret = pthread_create(&pstViIRGrp->stViSns.irDynamicCalibThread, NULL, pstViIRGrp->stViSns.pfnIrDynamicCalibFunc, &pstViIRGrp->stViSns);
        if (s32Ret) {
            PRINT_ERR("create ir dynamic calib thread failed\n");
            pstViIRGrp->stViSns.bIrDynamicCalibThreadStart = AR_FALSE;
            return s32Ret;
        }
    }

    return 0;
}

static AR_S32 StopViIRGroup(VI_IR_GROUP_S * pstViIRGrp)
{
    AR_S32 i = 0;
    AR_S32 s32Ret = 0;
    AR_S32 s32ViCnt = 0;
    AR_S32 s32BusId = 0;
    AR_S32 s32Fps = 0;
    VI_DEV ViDev = 0;
    VI_PIPE ViPipe = 0;
    VI_CHN ViChn = 0;
    MIPI_DEV MipiDev = 0;
    IPC_SNS_TYPE_E enSnsType = 0;
    VI_WRAPPER_S * pstViWrapper = NULL;
    IPC_CFG_VIO_S * pstVioCfg = IPC_CFG_VIO_GetParam();
    VI_CHN_ATTR_S stChnAttr = {0};

    s32ViCnt = sizeof(VI_IR_GROUP_S)/sizeof(VI_WRAPPER_S);
    
    //stop the threads
    if (pstViIRGrp->stViSns.enSnsType != IPC_SNS_TYPE_BUTT) 
    {
        pstViIRGrp->stViSns.bIrDynamicCalibThreadStart = AR_FALSE;
        pthread_join(pstViIRGrp->stViSns.irDynamicCalibThread, NULL);
    }

    pstViWrapper = &pstViIRGrp->stViSns;
    for(i = 0; i < s32ViCnt; i++)
    {
        if(pstViWrapper->pfnThreadFunc)
        {
            pstViWrapper->bIrfnThreadStart = AR_FALSE;
            pthread_join(pstViWrapper->threadPid, NULL);	
        }
        pstViWrapper++;
    }

    pstViWrapper = &pstViIRGrp->stViSns;
    for(i = 0; i < s32ViCnt; i++)
    {
        if(pstViWrapper->pfnThreadDeqFunc)
        {
            pstViWrapper->bIrDeThreadStart = AR_FALSE;
            pthread_join(pstViWrapper->threadDeqPid, NULL);	
        }
        pstViWrapper++;
    }

    //stop isp
    pstViWrapper = &pstViIRGrp->stViIsp1;
    for(i = 0; i < s32ViCnt; i++)
    {
        if(pstViWrapper->enSnsType == IPC_SNS_TYPE_BUTT)
        {
            pstViWrapper--;
            continue;
        }
        
        ViPipe = pstViWrapper->ViPipe;   
        
        AR_VI_StopSingleIsp(ViPipe, pstViWrapper->enSnsType);
        pstViWrapper--;
    }

    //diable chn and dev
    pstViWrapper = &pstViIRGrp->stViIsp1;
    for(i = 0; i < s32ViCnt; i++)
    {
        if(pstViWrapper->enSnsType == IPC_SNS_TYPE_BUTT)
        {
            pstViWrapper--;
            continue;
        }
    
        ViDev = pstViWrapper->ViDev;
        ViPipe = pstViWrapper->ViPipe;
        ViChn = pstViWrapper->ViChn;
        
        AR_MPI_VI_DisableChn(ViPipe, ViChn);
        AR_VI_StopSingleVi(ViDev, ViPipe);
        pstViWrapper--;
    }
    
    return 0;
}


// start id (vi dev id) should not conflict with other pipeline, we use vi index by default and increase by one for each vi
// after start group, you can bind the last vi output with vpss/vo/venc as usual.
static VI_IR_GROUP_S * g_pstViIRGrp = NULL;
AR_S32 IPC_PF_CreateViIRGroup(IPC_SNS_TYPE_E enSnsType, AR_S32 s32DevStartId)
{
    AR_S32 s32Ret = 0;
    IPC_CFG_VIO_S * pstVioCfg = IPC_CFG_VIO_GetParam();
    VI_IR_GROUP_S * pstViIRGrpCfg = NULL;

    VI_IR_GROUP_S * pstViIRGrp = (VI_IR_GROUP_S *)malloc(sizeof(VI_IR_GROUP_S));
    if(!pstViIRGrp)
    {
        PRINT_ERR("Malloc for vi ir group failed.\n");
        return -1;
    }

    pstViIRGrpCfg = GetViIRGroupBySnsType(enSnsType);
    if(!pstViIRGrpCfg)
    {
        PRINT_ERR("Not find senser cfg for %d\n", enSnsType);
        free(pstViIRGrp);
        return -1;
    }

    PRINT_INFO("Vi IR sns %d start...\n", enSnsType);

    memcpy(pstViIRGrp, pstViIRGrpCfg, sizeof(VI_IR_GROUP_S));

    pstViIRGrp->stViSns.ViDev = s32DevStartId;
    pstViIRGrp->stViSns.ViPipe = pstViIRGrp->stViSns.ViDev; //Suppose the pipe is available
    pstViIRGrp->stViSns.ViChn = 0;
    pstViIRGrp->stViSns.s32MipiDev = 0;
    pstViIRGrp->stViSns.s32BusId = pstVioCfg->vi[s32DevStartId].i2c_bus;
    pstViIRGrp->stViSns.s32Fps = pstVioCfg->vi[s32DevStartId].fps;
    pstViIRGrp->stViSns.pstNextVi = &pstViIRGrp->stViThermal;
    strcpy(pstViIRGrp->stViSns.achPowerGPIO, pstVioCfg->vi[s32DevStartId].power_gpio);
    strcpy(pstViIRGrp->stViSns.achResetGPIO, pstVioCfg->vi[s32DevStartId].reset_gpio);
    strcpy(pstViIRGrp->stViSns.achCommGPIO, pstVioCfg->vi[s32DevStartId].comm_gpio);

    pstViIRGrp->stViThermal.ViDev = s32DevStartId + 1;
    pstViIRGrp->stViThermal.ViPipe = pstViIRGrp->stViThermal.ViDev;
    pstViIRGrp->stViThermal.ViChn = 4;
    pstViIRGrp->stViThermal.s32MipiDev = 0;
    pstViIRGrp->stViThermal.s32Fps = pstVioCfg->vi[s32DevStartId].fps;
    pstViIRGrp->stViThermal.pstNextVi = &pstViIRGrp->stViIsp0;

    pstViIRGrp->stViIsp0.ViDev = s32DevStartId + 2;
    pstViIRGrp->stViIsp0.ViPipe = pstViIRGrp->stViIsp0.ViDev;
    pstViIRGrp->stViIsp0.ViChn = 0;
    pstViIRGrp->stViIsp0.s32MipiDev = 0;
    pstViIRGrp->stViIsp0.s32Fps = pstVioCfg->vi[s32DevStartId].fps;

    if(pstViIRGrp->stViIsp1.enSnsType != IPC_SNS_TYPE_BUTT)
    {
        pstViIRGrp->stViIsp0.pstNextVi = &pstViIRGrp->stViIsp1;

        pstViIRGrp->stViIsp1.ViDev = s32DevStartId + 3;
        pstViIRGrp->stViIsp1.ViPipe = pstViIRGrp->stViIsp1.ViDev;
        pstViIRGrp->stViIsp1.ViChn = 0;
        pstViIRGrp->stViIsp1.s32MipiDev = 0;
        pstViIRGrp->stViIsp1.s32Fps = pstVioCfg->vi[s32DevStartId].fps;
        pstViIRGrp->stViIsp1.pstNextVi = NULL;
    }
    else
    {
        pstViIRGrp->stViIsp0.pfnFrameProcess = NULL;
        pstViIRGrp->stViIsp0.pfnThreadFunc = NULL;
        pstViIRGrp->stViIsp0.pfnThreadDeqFunc = NULL;
        pstViIRGrp->stViIsp0.pstNextVi = NULL;
    }

    IrOpenShutter();
    
    s32Ret = StartViIRGroup(pstViIRGrp);
    if(s32Ret < 0)
    {
        PRINT_ERR("Start VI IR group failed.\n");
        free(pstViIRGrp);
        return -1;
    }

    g_pstViIRGrp = pstViIRGrp;
    return 0;
}

AR_S32 IPC_PF_DestroyViIRGroup()
{
    AR_S32 s32Ret = 0;
    VI_IR_GROUP_S * pstViIRGrp = g_pstViIRGrp;

    StopViIRGroup(pstViIRGrp);

    free(pstViIRGrp);
    return 0;
}

AR_S32 IPC_PF_StartIrFussion()
{
    AR_S32 s32Ret = 0;
	IPC_CFG_CHANNEL_S * pstChnCfg = IPC_CFG_CHANNEL_GetParam();
	
#ifndef THERMAL_TYPE
    if(strcmp(pstChnCfg->stChannel[0].chType, "low_bw") == 0 
      && strcmp(pstChnCfg->stChannel[1].chType, "infrared") == 0)
   	{
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
        s32Ret = inf_algo_create(&g_pstInfAlgObject_ars31,"/usrdata/local/ir_params.json",&stInfAlgParams,NULL);
        if(s32Ret!=0)
        {
            g_pstInfAlgObject_ars31 = NULL;
            PRINT_ERR("inf_algo_create failed, ret=%d", s32Ret);
        }
        
        //fussion thread
        s32Ret = pthread_create(&g_fussionPid, NULL, IrFussionThreadFunc, NULL);
        if (s32Ret) 
        {
            PRINT_ERR("create ir dequeue thread failed\n");
            return s32Ret;
        }
   	}
#endif

    return 0;
}

AR_S32 IPC_PF_StopIrFussion()
{
    AR_S32 s32Ret = 0;
    IPC_CFG_CHANNEL_S * pstChnCfg = IPC_CFG_CHANNEL_GetParam();
    //fusion thread exit
    //algo object release
#ifndef THERMAL_TYPE
    if(strcmp(pstChnCfg->stChannel[0].chType, "low_bw") == 0 
        && strcmp(pstChnCfg->stChannel[1].chType, "infrared") == 0)
    {
        bFussionRun = AR_FALSE;
        if(g_pstInfAlgObject_ars31)
        {
            s32Ret = inf_algo_release(&g_pstInfAlgObject_ars31);
            if(s32Ret!=0)
            {
                g_pstInfAlgObject_ars31 = NULL;
                PRINT_ERR("inf_algo_release failed, ret=%d", s32Ret);
            }
        }	
    }
#endif
	return 0;
}

VI_IR_GROUP_S * IPC_PF_GetViIRGroup()
{
    return g_pstViIRGrp;
}
