#include "ispin_vi_buffer_loop.h"

#include "hal_sys.h"
#include "hal_vin_log.h"

#include "ar_math.h"
#include "ar_common.h"
#include "ar_comm_video.h"

#include "mpi_vi.h"

typedef struct
{
    VI_PIPE     ViPipe_isp;
    VI_CHN      ViChn_isp;
    COMMON_VB_BUFFER_T astIspRawBuffer[3];
    STRU_AR_QUEUE_T *stream_buffer_pool;
    AR_BOOL     bRunFlag;
    pthread_t raw_dq_thread_t;
}STRU_ISPIN_VI_BUF_OBJ_T;

typedef struct
{
    AR_U32      u32DualIspinEnable;
    STRU_ISPIN_VI_BUF_OBJ_T astViBufObj[2];
    SIZE_S      stSize;
    AR_BOOL     bRunFlag;
    pthread_t yuv2ispin_thread_t;
}STRU_ISPIN_VI_LOOP_OBJ_T;

static STRU_ISPIN_VI_LOOP_OBJ_T g_stIspinViLoopObj = {0};

static void *stream_buf_dq_thread(void* arg)
{
    STRU_ISPIN_VI_BUF_OBJ_T *p_raw_obj=(STRU_ISPIN_VI_BUF_OBJ_T *)arg;
    printf("dq_loop_thread start, pipe=%d chn=%d\n",p_raw_obj->ViPipe_isp,p_raw_obj->ViChn_isp);

    AR_S32 s32Ret;
	while(p_raw_obj->bRunFlag)
	{
	    int qsize = p_raw_obj->stream_buffer_pool->get_queue_size(p_raw_obj->stream_buffer_pool);
	    if(qsize==3){
	        usleep(5000);
	        continue;
	    }
        VIDEO_FRAME_INFO_S VideoFrame = {0};
        s32Ret = AR_MPI_VI_DqPipeRaw(p_raw_obj->ViPipe_isp,&VideoFrame,-1);
        if(s32Ret!=0)
        {
            ar_err("AR_MPI_VI_DqPipeRaw failed, ret=%d pipe=%d\n",s32Ret,p_raw_obj->ViPipe_isp);
	    sleep(1);
	    continue;
            //break;
        }
        COMMON_VB_BUFFER_T* p_buff = (COMMON_VB_BUFFER_T*)VideoFrame.stVFrame.u64PrivateData;
		p_raw_obj->stream_buffer_pool->queue_insert(p_raw_obj->stream_buffer_pool,p_buff);
	}
	return 0;
}

static void *stream_buf_yuv2ispin_thread(void* arg)
{
	AR_S32 s32Ret;
    STRU_ISPIN_VI_LOOP_OBJ_T *pIspinLoopObj=(STRU_ISPIN_VI_LOOP_OBJ_T *)arg;
    STRU_ISPIN_VI_BUF_OBJ_T *pIspinVi0 = &pIspinLoopObj->astViBufObj[0];
    STRU_ISPIN_VI_BUF_OBJ_T *pIspinVi1 = &pIspinLoopObj->astViBufObj[1];

    printf("yuv2ispin_thread start\n");
    
	while(pIspinLoopObj->bRunFlag)
	{
	    VIDEO_FRAME_INFO_S irFrame = {0};
	    s32Ret = AR_MPI_VI_GetChnFrame(pIspinVi0->ViPipe_isp,pIspinVi0->ViChn_isp,&irFrame,-1);
        if(s32Ret!=0)
        {
            printf("AR_MPI_VI_GetChnFrame pipe=%d chn=%d failed\n",pIspinVi0->ViPipe_isp,pIspinVi0->ViChn_isp);
            sleep(1);
            continue;
        }
        COMMON_VB_BUFFER_T *p_buff = NULL;
        pIspinVi1->stream_buffer_pool->queue_pop(pIspinVi1->stream_buffer_pool,(void**)(&p_buff));
        if(!p_buff)
        {
            printf("stream_buffer_pool1 no buffer\n");
            AR_MPI_VI_ReleaseChnFrame(pIspinVi0->ViPipe_isp,pIspinVi0->ViChn_isp,&irFrame);
            sleep(1);
            continue;
        }
        ar_hal_sys_memcpy_pa(p_buff->u64PhyAddrAlign, irFrame.stVFrame.u64PhyAddr[0], 
            irFrame.stVFrame.u32Stride[0]*irFrame.stVFrame.u32Height);

        VIDEO_FRAME_INFO_S VideoFrame;
        VIDEO_FRAME_INFO_S *pstVideoFrame=&VideoFrame;
        pstVideoFrame->stVFrame.u64PhyAddr[0]=p_buff->u64PhyAddrAlign;
        pstVideoFrame->stVFrame.u64PhyAddr[1]=0;
        pstVideoFrame->stVFrame.u64PhyAddr[2]=0;
        pstVideoFrame->stVFrame.u64VirAddr[0]=(AR_U64)p_buff->pViraddrAlign;
        pstVideoFrame->stVFrame.u64VirAddr[1]=0;
        pstVideoFrame->stVFrame.u64VirAddr[2]=0;
        pstVideoFrame->stVFrame.u32Stride[0]=irFrame.stVFrame.u32Stride[0];
        pstVideoFrame->stVFrame.u32Stride[1]=0;
        pstVideoFrame->stVFrame.u32Stride[2]=0;
        
        pstVideoFrame->stVFrame.u32Width=irFrame.stVFrame.u32Width;
        pstVideoFrame->stVFrame.u32Height=irFrame.stVFrame.u32Height;
        pstVideoFrame->stVFrame.u64PTS=0;
        pstVideoFrame->stVFrame.u64PrivateData=(AR_U64)p_buff;
        pstVideoFrame->u32PoolId=VB_INVALID_POOLID;

        s32Ret = AR_MPI_VI_QPipeRaw(pIspinVi1->ViPipe_isp,pstVideoFrame);
        if(s32Ret < 0)
        {
            printf("AR_MPI_VI_QPipeRaw failed exit the thread\n");
        }

        AR_MPI_VI_ReleaseChnFrame(pIspinVi0->ViPipe_isp,pIspinVi0->ViChn_isp,&irFrame);
	}
	return 0;
}

static AR_S32 InitIspinViBufObj(STRU_ISPIN_VI_BUF_OBJ_T *pstViBufObj, AR_U32 buf_size)
{
    pstViBufObj->stream_buffer_pool = ar_hal_sys_creat_queue(5, "ispin_raw_buf_pool0");
    for(int i=0; i<3; i++){
        pstViBufObj->astIspRawBuffer[i].u32Size = buf_size;
        if(init_common_vb_buffer(&pstViBufObj->astIspRawBuffer[i])!=0)
        {
            printf("init_common_vb_buffer failed\n");
            return -1;
        }
        pstViBufObj->stream_buffer_pool->queue_insert(pstViBufObj->stream_buffer_pool,
                                                            &pstViBufObj->astIspRawBuffer[i]);
    }
    return 0;
}

static void RelaseIspinViBufObj(STRU_ISPIN_VI_BUF_OBJ_T *pstViBufObj)
{
    for(int i=0; i<3; i++){
        release_common_vb_buffer(&pstViBufObj->astIspRawBuffer[i]);
    }
    ar_hal_sys_delete_queue(&pstViBufObj->stream_buffer_pool);
}

static void SafeReleaseLoopObj()
{
    if(g_stIspinViLoopObj.bRunFlag)
    {
        printf("stop yuv2ispin_thread_t\n");
        g_stIspinViLoopObj.bRunFlag = 0;
        pthread_join(g_stIspinViLoopObj.yuv2ispin_thread_t, NULL);
    }
    if(g_stIspinViLoopObj.astViBufObj[1].bRunFlag)
    {
        printf("stop raw_dq_thread_t 1\n");
        g_stIspinViLoopObj.astViBufObj[1].bRunFlag = 0;
        pthread_join(g_stIspinViLoopObj.astViBufObj[1].raw_dq_thread_t, NULL);
    }
    if(g_stIspinViLoopObj.astViBufObj[0].bRunFlag)
    {
        printf("stop raw_dq_thread_t 0\n");
        g_stIspinViLoopObj.astViBufObj[0].bRunFlag = 0;
        pthread_join(g_stIspinViLoopObj.astViBufObj[0].raw_dq_thread_t, NULL);
    }

    RelaseIspinViBufObj(&g_stIspinViLoopObj.astViBufObj[0]);
    RelaseIspinViBufObj(&g_stIspinViLoopObj.astViBufObj[1]);
}

AR_S32 ISPIN_VI_Buf_Loop_Start(STRU_ISPIN_VI_LOOP_CONFIG_T *psConfig)
{
    AR_S32 s32Ret;

    ar_memset(&g_stIspinViLoopObj, sizeof(STRU_ISPIN_VI_LOOP_OBJ_T), 0, sizeof(STRU_ISPIN_VI_LOOP_OBJ_T));
    g_stIspinViLoopObj.astViBufObj[0].astIspRawBuffer[0].vb_blk = VB_INVALID_HANDLE;
    g_stIspinViLoopObj.astViBufObj[0].astIspRawBuffer[1].vb_blk = VB_INVALID_HANDLE;
    g_stIspinViLoopObj.astViBufObj[0].astIspRawBuffer[2].vb_blk = VB_INVALID_HANDLE;
    g_stIspinViLoopObj.astViBufObj[1].astIspRawBuffer[0].vb_blk = VB_INVALID_HANDLE;
    g_stIspinViLoopObj.astViBufObj[1].astIspRawBuffer[1].vb_blk = VB_INVALID_HANDLE;
    g_stIspinViLoopObj.astViBufObj[1].astIspRawBuffer[2].vb_blk = VB_INVALID_HANDLE;
    
    g_stIspinViLoopObj.u32DualIspinEnable = psConfig->u32DualIspinEnable;
    g_stIspinViLoopObj.astViBufObj[0].ViPipe_isp = psConfig->ViPipe_isp0;
    g_stIspinViLoopObj.astViBufObj[0].ViChn_isp = psConfig->ViChn_isp0;
    g_stIspinViLoopObj.astViBufObj[1].ViPipe_isp = psConfig->ViPipe_isp1;
    g_stIspinViLoopObj.astViBufObj[1].ViChn_isp = psConfig->ViChn_isp1;
    g_stIspinViLoopObj.stSize = psConfig->stSize;

    AR_U32 w_stride = ALIGN_UP(g_stIspinViLoopObj.stSize.u32Width*2, psConfig->u32RawAlign);
    s32Ret = InitIspinViBufObj(&g_stIspinViLoopObj.astViBufObj[0], 
        w_stride * g_stIspinViLoopObj.stSize.u32Height);
    if(s32Ret != 0){
        goto FAIL;
    }
    g_stIspinViLoopObj.astViBufObj[0].bRunFlag = 1;
    s32Ret = pthread_create(&g_stIspinViLoopObj.astViBufObj[0].raw_dq_thread_t, NULL, stream_buf_dq_thread, &g_stIspinViLoopObj.astViBufObj[0]);

    if(g_stIspinViLoopObj.u32DualIspinEnable)
    {
        AR_U32 y_stride = ALIGN_UP(g_stIspinViLoopObj.stSize.u32Width, psConfig->u32YuvAlign);
        s32Ret = InitIspinViBufObj(&g_stIspinViLoopObj.astViBufObj[1], 
            y_stride * g_stIspinViLoopObj.stSize.u32Height);
        if(s32Ret != 0){
            goto FAIL;
        }
        g_stIspinViLoopObj.astViBufObj[1].bRunFlag = 1;
        s32Ret = pthread_create(&g_stIspinViLoopObj.astViBufObj[1].raw_dq_thread_t, NULL, stream_buf_dq_thread, &g_stIspinViLoopObj.astViBufObj[1]);
        g_stIspinViLoopObj.bRunFlag = 1;
        s32Ret = pthread_create(&g_stIspinViLoopObj.yuv2ispin_thread_t, NULL, stream_buf_yuv2ispin_thread, &g_stIspinViLoopObj);
    }
    return 0;

FAIL:
    SafeReleaseLoopObj();
    return -1;
}

AR_S32 ISPIN_VI_Buf_Loop_Dq_RawFrame(VIDEO_FRAME_INFO_S *pFrame)
{
    COMMON_VB_BUFFER_T *p_buff = NULL;
    g_stIspinViLoopObj.astViBufObj[0].stream_buffer_pool->queue_pop(g_stIspinViLoopObj.astViBufObj[0].stream_buffer_pool,(void**)(&p_buff));
    if(!p_buff)
    {
        printf("stream_buffer_pool0 no buffer\n");
        return -1;
    }
    pFrame->stVFrame.u64PhyAddr[0]=p_buff->u64PhyAddrAlign;
	pFrame->stVFrame.u64PhyAddr[1]=0;
	pFrame->stVFrame.u64PhyAddr[2]=0;
	pFrame->stVFrame.u64VirAddr[0]=(AR_U64)p_buff->pViraddrAlign;
	pFrame->stVFrame.u64VirAddr[1]=0;
	pFrame->stVFrame.u64VirAddr[2]=0;
	pFrame->stVFrame.u64PrivateData=(AR_U64)p_buff;
	pFrame->u32PoolId=VB_INVALID_POOLID;
	return 0;
}

AR_S32 ISPIN_VI_Buf_Loop_Q_RawFrame(VIDEO_FRAME_INFO_S *pFrame)
{
    return AR_MPI_VI_QPipeRaw(g_stIspinViLoopObj.astViBufObj[0].ViPipe_isp,pFrame);
}

AR_S32 ISPIN_VI_Buf_Loop_Stop()
{
    SafeReleaseLoopObj();
    return 0;
}

