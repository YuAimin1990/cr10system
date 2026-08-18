#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "sample_comm.h"
#include "hal_vo.h"
#include "hal_vin_log.h"
#include "hal_region.h"
#include "mpi_vb.h"
#include "osal.h"
#include "hal_sys.h"

#include "common_utils.h"
#include "ar_inf_alg.h"
#include "simple_rtsp_srv.h"
#include "ispin_vi_buffer_loop.h"

#define DUAL_ISPIN 1               //dual ispin vi, 1 enable or 0 disable 

static int IRAW_W = 1280;
static int IRAW_H = 512;
static int IYUV_W = 640;
static int IYUV_H = 512;

extern SAMPLE_SNS_TYPE_E g_enSnsType[];


AR_VOID SAMPLE_AR_MPI_VIN_OpenDev(AR_S32 mode)
{
   VI_DEV_PROP_S Prop={0};
   if(mode==0){
      Prop.cam_mode=VIN_CAMERA_NORMAL;
   }else if(mode==1)
   {
      Prop.cam_mode=VIN_CAMERA_OFFLINE;
   }else if(mode==2)
   {
      Prop.cam_mode=VIN_CMAERA_MULTI_MODE;
   }else
   {
      Prop.cam_mode=VIN_CAMERA_OFFLINE;
   }
   //cfg the fre
   Prop.hdr_fre_mod=1;
   Prop.hdr_fre_hz=200000000;
   Prop.vif_fre_mod=1;
   Prop.vif_fre_hz=400000000;
   Prop.isp_fre_mod=1;
   Prop.isp_fre_hz=200000000;
   Prop.mipi_fre_mod=1;
   Prop.mipi_fre_hz=200000000;

   AR_MPI_VIN_OpenDev(&Prop);
   return;
}

AR_VOID SAMPLE_AR_MPI_VIN_CloseDev()
{
	return;
}


AR_VOID SAMPLE_VIO_MsgInit(AR_VOID)
{
}

AR_VOID SAMPLE_VIO_MsgExit(AR_VOID)
{
}

void SAMPLE_VIO_HandleSig(AR_S32 signo)
{
    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);

    if (SIGINT == signo || SIGTERM == signo)
    {
        SAMPLE_COMM_VENC_StopGetStream();
        SAMPLE_COMM_All_ISP_Stop();
        SAMPLE_COMM_VO_HdmiStop();
        SAMPLE_COMM_SYS_Exit();
        printf("\033[0;31mprogram termination abnormally!\033[0;39m\n");
    }
    exit(-1);
}

typedef struct
{
    int ViPipe_isp;
    int ViChn_isp;
    int ViPipe_isp1;
    int ViChn_isp1;
}STRU_IR_RAW_PROCESS_PARAM_T;

typedef struct
{
    int ViPipe_isp;
    int ViChn_isp;
    int VpssGrp_inf;
}STRU_YUV_PROCESS_PARAM_T;


static STRU_IR_RAW_PROCESS_PARAM_T g_RawProcessParam = {-1};
static INF_ALGO_OBJECT_S*           g_pstInfAlgObject = NULL;

static int save_y16 = 0, save_tp = 0, save_raw = 0;
static int isDoLms = 1;
static int isPrintTimeLog  = 0;

static void *debug_update_thread(void* arg)
{
	printf("debug_update_thread start \n");

    while(1)
    {
        sleep(3);
        if(access("/tmp/param",F_OK)==0){
            printf("update parameter from file...\n");
            int para_fd = 0;
            char line[128];
            float para[32] = {0};
            int aaa = 0;
            para_fd = open("/tmp/param", O_RDONLY);
            if(para_fd < 0)
            {
                printf("open file failed\n");
            }
            else
            {
                read(para_fd, line, sizeof(line));
                char *pch = strtok(line, " ");
                while (pch != NULL)
                {
                    //printf("cpch = %s, ipch = %f\n", pch, atof(pch));
                    para[aaa++] = atof(pch);
                    pch = strtok(NULL, " ");
                }
                isDoLms = para[0];
                isPrintTimeLog = para[1];
                close(para_fd);
                doSystem("mv /tmp/param /tmp/param_pre");
            }
        }

        if((access("/tmp/dump",F_OK))!=-1)
        {
            remove("/tmp/dump");
            printf("will dump..\n");
            save_y16 = save_tp = save_raw = 1;
        }
        /*if((access("/tmp/dumpy16.txt",F_OK))!=-1)
        {
            remove("/tmp/dumpy16.txt");
            printf("will dumpy16..\n");
            save_y16_data(100);
        }*/
    }

    return 0;
}

int on_inf_alg_status_event(ENUM_ALG_STATUS_CODE enStatusCode,void *data)
{
    static int gain_pre = 0;
    AR_S32 s32Ret = 0;
    ISP_AEC_MANU_TIDY_ATTR_S stAecManuTidyAttr = {0};
    switch (enStatusCode)
    {
    case ST_GAIN_UPDATE:
    {
        //s32Ret = AR_MPI_ISP_GetAecManuTidyAttr(4, &stAecManuTidyAttr);
        int *pos = (int*)data;
        int gain_cur = pos[1] - pos[0];
        int gain_diff = fabs(gain_cur - gain_pre);
        if(gain_diff>20)
        {
            gain_pre = gain_cur;
            stAecManuTidyAttr.stAecManu.aec_mode = 5;
            stAecManuTidyAttr.stAecManu.gain = (pos[1] - pos[0]);

            s32Ret = AR_MPI_ISP_SetAecManuTidyAttr(g_RawProcessParam.ViPipe_isp, &stAecManuTidyAttr);
            if(s32Ret!=AR_SUCCESS) printf("set ace manu tidy attr failed\n");

            if(g_RawProcessParam.ViPipe_isp1>=0)
                s32Ret = AR_MPI_ISP_SetAecManuTidyAttr(g_RawProcessParam.ViPipe_isp1, &stAecManuTidyAttr);
        }
    }
        break;
    default:
        break;
    }

    return 0;
}

int init_inf_alg_obj(int set_width, int set_height)
{
    int ret;

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
        .width = set_width,
        .height = set_height,
        .lms_out_bitwidth = 14,
        .gtm_out_bitwidth = 10,
    };
    ret = inf_algo_create(&g_pstInfAlgObject,"/usrdata/guide_demo/params.json",&stInfAlgParams,&stPriConfig,NULL);
    if(ret==0)
    {
        g_pstInfAlgObject->set_status_callbak(g_pstInfAlgObject, on_inf_alg_status_event);
    }
    return ret;
}

static void *colormap_process_thread(void* arg)
{
	printf("colormap_process_thread start \n");
    
    AR_S32 s32Ret;
    STRU_YUV_PROCESS_PARAM_T* pstYuvProcessParam = (STRU_YUV_PROCESS_PARAM_T*)arg;

    AR_S32 ViPipe_isp = pstYuvProcessParam->ViPipe_isp;
    AR_S32 ViChn_isp = pstYuvProcessParam->ViChn_isp;
    AR_S32 VpssGrp_inf = pstYuvProcessParam->VpssGrp_inf;

    COMMON_MMZ_BUFFER_T tmp_yuv_buf = {0};
    
    sleep(2);

    VIDEO_FRAME_INFO_S irFrame = {0};
    ar_memset(&irFrame, sizeof(VIDEO_FRAME_INFO_S), 0, sizeof(VIDEO_FRAME_INFO_S));
	while(1)
	{
	    s32Ret = AR_MPI_VI_GetChnFrame(ViPipe_isp,ViChn_isp,&irFrame,-1);
        if(s32Ret!=0)
        {
            printf("AR_MPI_VI_GetChnFrame pipe=%d chn=%d failed\n",ViPipe_isp,ViChn_isp);
            sleep(1);
            //break;
        }
        else{
            int y_offset = irFrame.stVFrame.u64PhyAddr[1] - irFrame.stVFrame.u64PhyAddr[0];
            int u_offset = irFrame.stVFrame.u64PhyAddr[2] - irFrame.stVFrame.u64PhyAddr[1];
            if(!tmp_yuv_buf.pViraddr)
            {
                tmp_yuv_buf.u32Size = y_offset + u_offset*2; 
                s32Ret = init_common_mmz_buffer(&tmp_yuv_buf);
                if(s32Ret<0)
                {
                    printf("init tmp yuv mmz buffer failed\n");
                    AR_MPI_VI_ReleaseChnFrame(ViPipe_isp,ViChn_isp,&irFrame);
                    sleep(1);
                    continue;
                }
            }

            ar_hal_sys_memcpy_pa(tmp_yuv_buf.u64PhyAddr, irFrame.stVFrame.u64PhyAddr[0], tmp_yuv_buf.u32Size);

            int64_t tbegin = getTickCount();
            g_pstInfAlgObject->mapping_color_process(g_pstInfAlgObject,
                                tmp_yuv_buf.pViraddr, 
                                tmp_yuv_buf.pViraddr + y_offset, 
                                tmp_yuv_buf.pViraddr + y_offset + u_offset,
                                irFrame.stVFrame.u32Width, irFrame.stVFrame.u32Height,
                                irFrame.stVFrame.u32Stride[0], irFrame.stVFrame.u32Stride[1], irFrame.stVFrame.u32Stride[2]);
            if(isPrintTimeLog)  printf("colormap time: %.2f ms.\n", getTickInterval(tbegin)/1000000.0);

            flush_common_mmz_buffer(&tmp_yuv_buf);
            ar_hal_sys_memcpy_pa(irFrame.stVFrame.u64PhyAddr[0], tmp_yuv_buf.u64PhyAddr, tmp_yuv_buf.u32Size);

            s32Ret = AR_MPI_VPSS_SendFrame(VpssGrp_inf,0,&irFrame,0);
            if(s32Ret!=AR_SUCCESS){
                printf("AR_MPI_VPSS_SendFrame grp=%d failed, ret=%d\n",VpssGrp_inf,s32Ret);
            }

            AR_MPI_VI_ReleaseChnFrame(ViPipe_isp,ViChn_isp,&irFrame);
        }
	}

    if(tmp_yuv_buf.pViraddr)
    {
        release_common_mmz_buffer(&tmp_yuv_buf);
    }

	return 0;
}

int onGetGdSdkY16Data(short *data,int len)
{
	//printf("get y16 data %p %d \n",data,len);
	int64_t tbegin;
	AR_S32 s32Ret;

    VIDEO_FRAME_INFO_S VideoFrame = {0};
	s32Ret = ISPIN_VI_Buf_Loop_Dq_RawFrame(&VideoFrame);
	if(s32Ret!=0){
	    return 0;
	}

    void *p_vaddr_align=(void*)VideoFrame.stVFrame.u64VirAddr[0];
    //AR_U64 phy_addr_align=VideoFrame.stVFrame.u64PhyAddr[0];
    
    if(isDoLms){
        tbegin = getTickCount();
        g_pstInfAlgObject->lms_process(g_pstInfAlgObject,data,data,-1);
        if(isPrintTimeLog) printf("lms time: %.2f ms.\n", getTickInterval(tbegin)/1000000.0);
    }

    tbegin = getTickCount();
    g_pstInfAlgObject->gtm_process(g_pstInfAlgObject,data,p_vaddr_align,-1);
    if(isPrintTimeLog) printf("gtm time: %.2f ms.\n", getTickInterval(tbegin)/1000000.0);

    if(save_y16){
		save_y16 = 0;
		dump_to_file("/tmp/y16.raw",data,len*2);
	}

	if(save_tp){
		save_tp = 0;
		dump_to_file("/tmp/tp.raw",p_vaddr_align,len*2);
	}

    //VIDEO_FRAME_INFO_S VideoFrame;
	VIDEO_FRAME_INFO_S *pstVideoFrame=&VideoFrame;
	pstVideoFrame->stVFrame.u32Stride[0]=IYUV_W*2;
	pstVideoFrame->stVFrame.u32Stride[1]=0;
	pstVideoFrame->stVFrame.u32Stride[2]=0;
	pstVideoFrame->stVFrame.u32Width=IYUV_W;
	pstVideoFrame->stVFrame.u32Height=IYUV_H;
	pstVideoFrame->stVFrame.u64PTS=0;

    tbegin = getTickCount();
	s32Ret = ISPIN_VI_Buf_Loop_Q_RawFrame(&VideoFrame);
	//s32Ret=AR_MPI_VI_SendPipeRaw(g_RawProcessParam->ViPipe_isp,pstVideoFrame,-1);
    if(isPrintTimeLog) printf("AR_MPI_VI_SendPipeRaw time: %.2f ms.\n", getTickInterval(tbegin)/1000000.0);
	if(s32Ret<0)
	{
		SAMPLE_PRT("AR_MPI_VI_SendPipeRaw failed exit the thread\n");
	}

	return 0;
}

AR_S32 SAMPLE_plug617_test(AR_S32 outMode)
{
    AR_S32             s32Ret;

    AR_S32			   s32ViCnt 	  = 2;
    AR_S32             s32WorkSnsId;

    SAMPLE_VI_CONFIG_S stViConfig;

    SIZE_S             stSize;
    VB_CONFIG_S        stVbConf;
    //PIC_SIZE_E         enPicSize;
    AR_U32             u32BlkSize;

    VO_CHN             VoChn          = 0;
    SAMPLE_VO_CONFIG_S stVoConfig;

    //SAMPLE_VI_INFO_S *pstViInfo = NULL;
    
    //int i;
    AR_S32              s32OutMode = outMode;

    if(DUAL_ISPIN) s32ViCnt = 3;

    /*config vi*/
    SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

    stViConfig.s32WorkingViNum    = s32ViCnt;
    stViConfig.as32WorkingViId[0] = 0;
	stViConfig.as32WorkingViId[1] = 1;
	stViConfig.as32WorkingViId[2] = 2;

    s32WorkSnsId = 0;
    VI_DEV			   ViDev_raw		  = 0;
    VI_PIPE 		   ViPipe_raw		  = ViDev_raw;
    VI_CHN			   ViChn_raw    	  = 2;
    
    {
        DYNAMIC_RANGE_E    enDynamicRange = DYNAMIC_RANGE_SDR8;
        PIXEL_FORMAT_E     enPixFormat    = PIXEL_FORMAT_RGB_BAYER_8BPP;
        VIDEO_FORMAT_E     enVideoFormat  = VIDEO_FORMAT_LINEAR;
        COMPRESS_MODE_E    enCompressMode = COMPRESS_MODE_NONE;

        g_enSnsType[s32WorkSnsId] = PLUG617_DVP_640x512_IR;
        
        stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = s32WorkSnsId;
    	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 2;
    	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev  = 2;
    	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = PLUG617_DVP_640x512_IR;

    	stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev			 = ViDev_raw;
    	stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode		 = WDR_MODE_NONE;
    	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
    	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0]		 = ViPipe_raw;
    	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1]		 = -1;
    	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2]		 = -1;
    	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3]		 = -1;
    	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn			 = ViChn_raw;
    	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat	 = enPixFormat;
    	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange  = enDynamicRange;
    	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat	 = enVideoFormat;
	    stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode  = enCompressMode;
    }

    SAMPLE_PRT("0---------------enSnsType=%d \n",stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType);

    /*config two ispin vi*/
    s32WorkSnsId = 1;
    VI_DEV			   ViDev_isp		  = 1;
    VI_PIPE 		   ViPipe_isp		  = ViDev_isp;
    VI_CHN			   ViChn_isp    	  = 0;
    {
        DYNAMIC_RANGE_E    enDynamicRange = DYNAMIC_RANGE_SDR8;
        PIXEL_FORMAT_E     enPixFormat    = PIXEL_FORMAT_YVU_PLANAR_420;
        VIDEO_FORMAT_E     enVideoFormat  = VIDEO_FORMAT_LINEAR;
        COMPRESS_MODE_E    enCompressMode = COMPRESS_MODE_NONE;

        g_enSnsType[s32WorkSnsId] = ISP_VIN_COMMON_INF_640x512;
        
        stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = s32WorkSnsId;
    	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 0;
    	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev  = 0;
    	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = ISP_VIN_COMMON_INF_640x512;

    	stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev			 = ViDev_isp;
    	stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode		 = WDR_MODE_NONE;
    	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
    	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0]		 = ViPipe_isp;
    	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1]		 = -1;
    	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2]		 = -1;
    	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3]		 = -1;
    	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn			 = ViChn_isp;
    	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat	 = enPixFormat;
    	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange  = enDynamicRange;
    	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat	 = enVideoFormat;
	    stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode  = enCompressMode;
    }

    s32WorkSnsId = 2;
    VI_DEV			   ViDev_isp1		  = 2;
    VI_PIPE 		   ViPipe_isp1		  = ViDev_isp1;
    VI_CHN			   ViChn_isp1    	  = 0;
    {
        DYNAMIC_RANGE_E    enDynamicRange = DYNAMIC_RANGE_SDR8;
        PIXEL_FORMAT_E     enPixFormat    = PIXEL_FORMAT_YVU_PLANAR_420;
        VIDEO_FORMAT_E     enVideoFormat  = VIDEO_FORMAT_LINEAR;
        COMPRESS_MODE_E    enCompressMode = COMPRESS_MODE_NONE;

        g_enSnsType[s32WorkSnsId] = ISP_VIN_COMMON_INF1_640x512;
        
        stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = s32WorkSnsId;
    	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 0;
    	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev  = 0;
    	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = ISP_VIN_COMMON_INF1_640x512;

    	stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev			 = ViDev_isp1;
    	stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode		 = WDR_MODE_NONE;
    	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
    	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0]		 = ViPipe_isp1;
    	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1]		 = -1;
    	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2]		 = -1;
    	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3]		 = -1;
    	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn			 = ViChn_isp1;
    	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat	 = enPixFormat;
    	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange  = enDynamicRange;
    	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat	 = enVideoFormat;
	    stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode  = enCompressMode;
    }
    SAMPLE_PRT("2---------------enSnsType=%d \n",stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType);

    g_RawProcessParam.ViPipe_isp = ViPipe_isp;
    g_RawProcessParam.ViChn_isp = ViChn_isp;
    if(DUAL_ISPIN){
        g_RawProcessParam.ViPipe_isp1 = ViPipe_isp1;
        g_RawProcessParam.ViChn_isp1 = ViChn_isp1;
    }

    /*config vb*/
    ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
    stVbConf.u32MaxPoolCnt = DUAL_ISPIN?4:2;

    stSize.u32Width = IRAW_W;
    stSize.u32Height = IRAW_H;
    u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_8BPP, COMPRESS_MODE_NONE, 256);
    stVbConf.astCommPool[0].u64BlkSize = u32BlkSize+8192;
    stVbConf.astCommPool[0].u32BlkCnt  = 10;

    stSize.u32Width = IYUV_W;
    stSize.u32Height = IYUV_H;
    u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height,
        PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
    stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
    stVbConf.astCommPool[1].u32BlkCnt  = 10;

    if(DUAL_ISPIN){
        /* for ispin vi buffer loop */
        stSize.u32Width = IYUV_W;
        stSize.u32Height = IYUV_H;
        u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_8BPP, COMPRESS_MODE_NONE, 512);
        stVbConf.astCommPool[2].u64BlkSize = u32BlkSize;
        stVbConf.astCommPool[2].u32BlkCnt  = 3;

        stSize.u32Width = IYUV_W*2;
        stSize.u32Height = IYUV_H;
        u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_8BPP, COMPRESS_MODE_NONE, 256);
        stVbConf.astCommPool[3].u64BlkSize = u32BlkSize;
        stVbConf.astCommPool[3].u32BlkCnt  = 3;
    }

    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (AR_SUCCESS != s32Ret) {
        SAMPLE_PRT("system init failed with %d!\n", s32Ret);
        return s32Ret;
    }

    AR_S32 cam_mode = DUAL_ISPIN?2:1;
    SAMPLE_AR_MPI_VIN_OpenDev(cam_mode);

    /*start vi*/
    
    s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
    if (AR_SUCCESS != s32Ret) {
        SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
        goto EXIT;
    }

    //vpss ******************************************
    VPSS_GRP VpssGrp_inf = 0;
    VPSS_CHN VpssChn_inf0 = 0;
    {
        //***********infrared group*****************
        VPSS_GRP            VpssGrp = VpssGrp_inf;
        DYNAMIC_RANGE_E     enDynamicRange = DYNAMIC_RANGE_SDR8;
        PIXEL_FORMAT_E      enPixFormat    = PIXEL_FORMAT_YVU_PLANAR_420;
        VIDEO_FORMAT_E      enVideoFormat  = VIDEO_FORMAT_LINEAR;
        COMPRESS_MODE_E     enCompressMode = COMPRESS_MODE_NONE;

        VPSS_GRP_ATTR_S     stVpssGrpAttr = {0};
        VPSS_CHN_ATTR_S     astVpssChnAttr[VPSS_MAX_PHY_CHN_NUM] = {0};
        AR_BOOL             abChnEnable[VPSS_MAX_CHN_NUM] = {0};

        SIZE_S GrpOutSize = {IYUV_W,IYUV_H};
        SIZE_S ChnOutSize = {IYUV_W,IYUV_H};

        /*config vpss*/
        stVpssGrpAttr.stFrameRate.s32SrcFrameRate    = -1;
        stVpssGrpAttr.stFrameRate.s32DstFrameRate    = -1;
        stVpssGrpAttr.u32Width                       = GrpOutSize.u32Width;
        stVpssGrpAttr.u32Height                      = GrpOutSize.u32Height;

        stVpssGrpAttr.enCompressMode                 = enCompressMode;
        stVpssGrpAttr.enDynamicRange                 = enDynamicRange;
        stVpssGrpAttr.enVideoFormat                  = enVideoFormat;
        stVpssGrpAttr.enPixelFormat                  = enPixFormat;

        // enable channel 0
        for(int i = 0; i < VPSS_MAX_PHY_CHN_NUM && i < 1; i++){
            VPSS_CHN VpssPhyChn = i;

            astVpssChnAttr[VpssPhyChn].u32Width                     = ChnOutSize.u32Width;//stSize.u32Width;
            astVpssChnAttr[VpssPhyChn].u32Height                    = ChnOutSize.u32Height;//stSize.u32Height;
            astVpssChnAttr[VpssPhyChn].enChnMode                    = VPSS_CHN_MODE_USER;
            astVpssChnAttr[VpssPhyChn].enCompressMode               = enCompressMode;
            astVpssChnAttr[VpssPhyChn].enDynamicRange               = enDynamicRange;
            astVpssChnAttr[VpssPhyChn].enVideoFormat                = enVideoFormat;
            astVpssChnAttr[VpssPhyChn].enPixelFormat                = enPixFormat;
            astVpssChnAttr[VpssPhyChn].stFrameRate.s32SrcFrameRate  = 25;
            astVpssChnAttr[VpssPhyChn].stFrameRate.s32DstFrameRate  = 25;
            astVpssChnAttr[VpssPhyChn].u32Depth                     = 1;
            
            astVpssChnAttr[VpssPhyChn].stAspectRatio.enMode = ASPECT_RATIO_NONE;

            abChnEnable[i] = AR_TRUE;
        }
        
        s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
        if (AR_SUCCESS != s32Ret)
        {
            printf("Start vpss group failed. s32Ret: 0x%x !\n", s32Ret);
            goto EXIT2;
        }
    }
    //vpss end***************************************

    //use yuv color map thread instead of bind
    STRU_YUV_PROCESS_PARAM_T stYuvProcessParam;
    pthread_t yuv_process_thread;
    stYuvProcessParam.ViPipe_isp = DUAL_ISPIN?ViPipe_isp1:ViPipe_isp;
    stYuvProcessParam.ViChn_isp = DUAL_ISPIN?ViChn_isp1:ViChn_isp;
    stYuvProcessParam.VpssGrp_inf = VpssGrp_inf;
    s32Ret = pthread_create(&yuv_process_thread, NULL, colormap_process_thread, &stYuvProcessParam);
    //s32Ret = SAMPLE_COMM_VI_Bind_VPSS(stYuvProcessParam.ViPipe_isp, stYuvProcessParam.ViChn_isp, VpssGrp_inf);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("vi bind vpss failed with %d!\n", s32Ret);
        goto EXIT2;
    }

    if(s32OutMode == 0 || s32OutMode == 2)
    {
        /************************************************
    	 start V0
    	*************************************************/
    	SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
    	stVoConfig.VoDev									= SAMPLE_VO_DEV_DHD0;
    	stVoConfig.enVoIntfType 							= VO_INTF_HDMI;
    	stVoConfig.enIntfSync								= VO_OUTPUT_1080P60;
        //stVoConfig.enPicSize                                = enPicSize;
    	stVoConfig.u32DisBufLen 							= 3;
    	stVoConfig.enDstDynamicRange						= DYNAMIC_RANGE_SDR8;
    	stVoConfig.enVoMode 								= VO_MODE_1MUX;

    	s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);

    	if (AR_SUCCESS != s32Ret)
    	{
    		SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %d!\n", s32Ret);
    		goto EXIT2;
    	}
    	/************************************************
    	step 8:  Vi bind VO
    	*************************************************/

    	SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO \n");
        VO_LAYER VoLayer = 0;
        s32Ret = SAMPLE_COMM_VPSS_Bind_VO(VpssGrp_inf,VpssChn_inf0,VoLayer,VoChn);
    	//s32Ret = SAMPLE_COMM_VI_Bind_VO(ViPipe_isp, ViChn_isp, 0, VoChn);
    	if (AR_SUCCESS != s32Ret)
    	{
    		SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO failed with %d!\n", s32Ret);
    		goto EXIT2;
    	}
    }
    
    if(s32OutMode == 1 || s32OutMode == 2)
    {
        /************************************************
         start VENC
        *************************************************/
        VENC_CHN VencChn_id = 0;
        AR_U32 u32Width = IYUV_W;
        AR_U32 u32Height = IYUV_H;
        AR_U32 u32VencBitrate = 2048;
		AR_U32 u32VencBufSize =  u32VencBitrate*4*1024/8;
        AR_U32 u32EncoderCnt=1;
        AR_U32 u32VencDoneQueueSize = u32VencBufSize*25*(u32VencBitrate<<7);

        //set venc mod param
        VENC_PARAM_MOD_S stParam = {0};
        stParam.enVencModType = MODTYPE_H265E;
        s32Ret = AR_MPI_VENC_GetModParam(&stParam);
        if(s32Ret){
            SAMPLE_PRT("Get venc mod param error! ret = %x\n", s32Ret);
            goto EXIT2;
        }
        stParam.stEventModParam.u32VencIrqQueueSize   = 64;
        stParam.stEventModParam.u32VencTaskQueueSize  = u32EncoderCnt * 10;
        stParam.stEventModParam.u32VencDoneQueueSize  = u32VencDoneQueueSize;
        s32Ret = AR_MPI_VENC_SetModParam(&stParam);
        if(s32Ret){
            SAMPLE_PRT("Set venc mod param error! ret = %x\n", s32Ret);
            goto EXIT2;
        }

        //start venc
        VENC_CHN_ATTR_S stChnAttr;
        memset(&stChnAttr, 0, sizeof(stChnAttr));
        stChnAttr.stVencAttr.enType = PT_H265;
        stChnAttr.stVencAttr.u32PicWidth = u32Width;
        stChnAttr.stVencAttr.u32PicHeight = u32Height;
        stChnAttr.stVencAttr.u32MaxPicWidth = u32Width;
        stChnAttr.stVencAttr.u32MaxPicHeight = u32Height;
        stChnAttr.stVencAttr.u32BufSize = u32VencBufSize;
        stChnAttr.stVencAttr.bByFrame = AR_TRUE;
        stChnAttr.stVencAttr.enPixelFormat = PIXEL_FORMAT_YVU_PLANAR_420;
        stChnAttr.stVencAttr.stAttrH265e.u32CmdQueueDepth = 2;
        stChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265CBR;
        stChnAttr.stRcAttr.stH265Cbr.u32SrcFrameRate = 25;
        stChnAttr.stRcAttr.stH265Cbr.u32BitRate = u32VencBitrate;
        stChnAttr.stRcAttr.stH265Cbr.u32Gop = 50;
        stChnAttr.stRcAttr.stH265Cbr.u32StatTime = 30;          //must set
        stChnAttr.stGopAttr.enGopMode = VENC_GOPMODE_NORMALP;
        stChnAttr.stGopAttr.stNormalP.s32IPQpDelta = -2;

        s32Ret = AR_MPI_VENC_CreateChn(VencChn_id, &stChnAttr);
    	if (s32Ret) {
    		printf("create channel failed\n");
    		goto EXIT2;
    	}

        s32Ret = SAMPLE_COMM_VPSS_Bind_VENC(VpssGrp_inf,VpssChn_inf0,VencChn_id);
        if (s32Ret) {
            printf("bind venc failed\n");
            goto EXIT2;
        }
        
        VENC_RECV_PIC_PARAM_S param;
    	param.s32RecvPicNum = -1;
        s32Ret = AR_MPI_VENC_StartRecvFrame(VencChn_id, &param);
    	if (s32Ret != 0) {
    		printf("venc dev id:%d start failed! ret=%d\n", VencChn_id, s32Ret);
    		goto EXIT2;
    	}

        smp_rtsp_enable_venc_chn(0, VencChn_id, "/ch0/stream0");
        smp_rtsp_start();
    }

    COMMON_MMZ_BUFFER_T tmp_buf = {0};
    tmp_buf.u32Size = IRAW_W*IRAW_H;
    s32Ret = init_common_mmz_buffer(&tmp_buf);
    if(s32Ret!=0){
        printf("init_common_mmz_buffer failed\n");
        goto EXIT2;
    }

    STRU_ISPIN_VI_LOOP_CONFIG_T stIspinViConfig = {0};
    stIspinViConfig.ViPipe_isp0 = ViPipe_isp;
    stIspinViConfig.ViChn_isp0 = ViChn_isp;
    if(DUAL_ISPIN){
        stIspinViConfig.ViPipe_isp1 = ViPipe_isp1;
        stIspinViConfig.ViChn_isp1 = ViChn_isp1;
        stIspinViConfig.u32DualIspinEnable = 1;
    }
    stIspinViConfig.stSize.u32Width = IYUV_W;
    stIspinViConfig.stSize.u32Height = IYUV_H;
    stIspinViConfig.u32RawAlign = 256;
    stIspinViConfig.u32YuvAlign = 512;
    s32Ret = ISPIN_VI_Buf_Loop_Start(&stIspinViConfig);
    if(s32Ret!=0){
        printf("ISPIN_VI_Buf_Loop_Start failed\n");
        goto EXIT2;
    }

    s32Ret = init_inf_alg_obj(IYUV_W,IYUV_H);
    if(s32Ret!=0){
        printf("inf_algo_create failed.\n");
        goto EXIT2;
    }

    if(s32OutMode==1)
    {//if only rtsp out, change csc
        sleep(1);
        ISP_CSC_TIDY_ATTR_S stISPCscTidyAttr = {0};
        s32Ret = AR_MPI_ISP_GetCscTidyAttr(ViPipe_isp, &stISPCscTidyAttr);
        printf("get csc attr type=%d range=%d, ret=%d\n",
            stISPCscTidyAttr.stCsc.type, stISPCscTidyAttr.stCsc.full_range, s32Ret);
        if(AR_SUCCESS==s32Ret){
            stISPCscTidyAttr.stCsc.type = CAM_BT601_TYPE;
            stISPCscTidyAttr.stCsc.full_range = stISPCscTidyAttr.stCsc.full_range==0?1:0;
            s32Ret = AR_MPI_ISP_SetCscTidyAttr(ViPipe_isp, &stISPCscTidyAttr);
            printf("set csc attr type=%d range=%d, ret=%d\n",
                stISPCscTidyAttr.stCsc.type,stISPCscTidyAttr.stCsc.full_range,s32Ret);
            }
    }

    pthread_t debug_thread;
    s32Ret = pthread_create(&debug_thread, NULL, debug_update_thread, NULL);

    while(1)
	{
        //get a raw from raw ch
        VIDEO_FRAME_INFO_S *pstFrameInfo=malloc(sizeof(VIDEO_FRAME_INFO_S));
		AR_MPI_VI_GetChnFrame(ViPipe_raw,ViChn_raw,pstFrameInfo,500000);
        //printf("get chn frame %u %u %u\n",pstFrameInfo->stVFrame.u32Width, pstFrameInfo->stVFrame.u32Height,pstFrameInfo->stVFrame.u32Stride[0]);
        
        if(save_raw){
		    save_raw --;
            printf("will save raw, info: %u %u %u %u %u %p %p %p\n",
                pstFrameInfo->stVFrame.u32Width, pstFrameInfo->stVFrame.u32Height,
                pstFrameInfo->stVFrame.u32Stride[0],pstFrameInfo->stVFrame.u32Stride[1],pstFrameInfo->stVFrame.u32Stride[2],
                (void*)pstFrameInfo->stVFrame.u64VirAddr[0],(void*)pstFrameInfo->stVFrame.u64VirAddr[1],(void*)pstFrameInfo->stVFrame.u64VirAddr[2]);

            char file_name[128] = {0};
            sprintf(file_name,"/tmp/ori_%d.raw",save_raw);
		    dump_to_file(file_name,(const void*)pstFrameInfo->stVFrame.u64VirAddr[0],pstFrameInfo->stVFrame.u32Stride[0]*pstFrameInfo->stVFrame.u32Height);
	    }

        int64_t tbegin = getTickCount();

        ar_hal_sys_memcpy_pa(tmp_buf.u64PhyAddr, pstFrameInfo->stVFrame.u64PhyAddr[0], tmp_buf.u32Size);
        flush_common_mmz_buffer(&tmp_buf);

        onGetGdSdkY16Data((short *)tmp_buf.pViraddr/*pstFrameInfo->stVFrame.u64VirAddr[0]*/, 640*512);

        if(isPrintTimeLog) printf("all time: %.2f ms.\n", getTickInterval(tbegin)/1000000.0);

        AR_MPI_VI_ReleaseChnFrame(ViPipe_raw,ViChn_raw,pstFrameInfo);
		free(pstFrameInfo);
	}

    release_common_mmz_buffer(&tmp_buf);
EXIT2:
    SAMPLE_COMM_VI_StopVi(&stViConfig);
EXIT:
    SAMPLE_COMM_SYS_Exit();
    return s32Ret;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
