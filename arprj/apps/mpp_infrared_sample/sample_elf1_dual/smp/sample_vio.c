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
#include "mpi_gdc_api.h"
#include "mpi_region.h"
#include "hal_scaler_api.h"

#include "common_utils.h"
#include "ar_inf_alg.h"
#include "simple_rtsp_srv.h"
#include "ispin_vi_buffer_loop.h"
#include "iray_module_manager.h"
#include "minIni.h"

#define DUAL_ISPIN 1               //dual ispin vi, 1 enable or 0 disable 
#define DEMO_CFG_PATH ("/usrdata/iray_demo/demo.cfg")

static int IRAW_W = 520;
static int IRAW_H = 204;
static int IYUV_W = 256;
static int IYUV_H = 192;

static int IFUSION_W = 640;
static int IFUSION_H = 480;

static int gOutMode = 0;

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
    int ViPipe_vis;
    int ViChn_vis;
    int VpssGrp_vis;
    int VpssChn_vis;
    int VpssGrp_inf;
    int VpssChn_inf;
}STRU_YUV_PROCESS_PARAM_T;


static STRU_IR_RAW_PROCESS_PARAM_T  g_RawProcessParam = {-1};
//static STRU_YUV_PROCESS_PARAM_T     g_YuvProcessParam = {-1};
static INF_ALGO_OBJECT_S*           g_pstInfAlgObject = NULL;

static int save_y16 = 0, save_tp = 0, save_raw = 0;
static int isDoLms = 1;
static int isPrintTimeLog  = 0;
static int crop_x = 336,crop_y=140,crop_w=1404,crop_h=928;

static int set_demo_config_value()
{
    long n;
    n = ini_putl("fusion", "crop_x", crop_x, DEMO_CFG_PATH);
    if(n!=1) return -1;
    n = ini_putl("fusion", "crop_y", crop_y, DEMO_CFG_PATH);
    if(n!=1) return -1;
    n = ini_putl("fusion", "crop_w", crop_w, DEMO_CFG_PATH);
    if(n!=1) return -1;
    n = ini_putl("fusion", "crop_h", crop_h, DEMO_CFG_PATH);
    if(n!=1) return -1;
    return 0;
}

static int get_demo_config_value()
{
    long n;
    n = ini_hassection("fusion", DEMO_CFG_PATH);
    if(!n){
        printf("save default to demo config\n");
        set_demo_config_value();
        return 0;
    }
    n = ini_getl("fusion", "crop_x", 336, DEMO_CFG_PATH);
    crop_x = n;
    n = ini_getl("fusion", "crop_y", 140, DEMO_CFG_PATH);
    crop_y = n;
    n = ini_getl("fusion", "crop_w", 1404, DEMO_CFG_PATH);
    crop_w = n;
    n = ini_getl("fusion", "crop_h", 928, DEMO_CFG_PATH);
    crop_h = n;
    printf("get demo config end\n");
    return 0;
}

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
                crop_x = para[2];
                crop_y = para[3];
                crop_w = para[4];
                crop_h = para[5];

                close(para_fd);
                doSystem("mv /tmp/param /tmp/param_pre");

                //change crop
                VPSS_CROP_INFO_S stCropInfo = {0};
                stCropInfo.bEnable = true;
                stCropInfo.enCropCoordinate = 1;
                stCropInfo.stCropRect.s32X = crop_x;
                stCropInfo.stCropRect.s32Y = crop_y;
                stCropInfo.stCropRect.u32Width = crop_w;
                stCropInfo.stCropRect.u32Height = crop_h;
                AR_MPI_VPSS_SetChnCrop(0, 1, &stCropInfo);

                set_demo_config_value();
            }
        }

        if((access("/tmp/dump",F_OK))!=-1)
        {
            remove("/tmp/dump");
            printf("will dump..\n");
            save_y16 = save_tp = save_raw = 1;
        }
        if((access("/tmp/dumpy16",F_OK))!=-1)
        {
            remove("/tmp/dumpy16");
            printf("will dumpy16..\n");
            save_nuc_data(100);
        }
    }

    return 0;
}

static void dump_frame_info(VIDEO_FRAME_INFO_S* pstFrame,const char* name)
{
    printf("[%s] -> w=%u h=%u pformat=%d stride=[%u %u %u] addr=[%p %p %p] TimeRef=%u pts=%llu\n",
        name,
        pstFrame->stVFrame.u32Width,pstFrame->stVFrame.u32Height,
        pstFrame->stVFrame.enPixelFormat,
        pstFrame->stVFrame.u32Stride[0],pstFrame->stVFrame.u32Stride[1],pstFrame->stVFrame.u32Stride[2],
        (void*)pstFrame->stVFrame.u64PhyAddr[0],(void*)pstFrame->stVFrame.u64PhyAddr[1],(void*)pstFrame->stVFrame.u64PhyAddr[2],
        pstFrame->stVFrame.u32TimeRef,pstFrame->stVFrame.u64PTS);
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
            .upThrowPointProportion = 0.05,
            .downThrowPointProportion = 0.05,
            .target_mean_value = 280,
            .contrastBaseUp = 2,
            .contrastBaseDown = 0.6,
            .compensate1 = 100,
            .compensate2 = 600,
        },
        .stLmsParam = {
            .enable_horizontal = 0,
            .prob = 0.8,
            .RaduisVL = 8,
            .RaduisHL = 8,
            .uStepVL = 0.25,
            .uStepHL = 0.25,
            .skipFrames = 0
        },
        .stColorPaletteParam = {
            .ColorPaletteFilePath = "/usrdata/iray_demo/iraycfg/ColorMapV2_3.dat",
            .palette_index = 5,
        },
        .stFusionParam = {
            .src_alpha = 50,
            .fusion_mode = FUSION_MODE_2,
        },
    };

    INF_ALGO_PRI_CONFIG_S stPriConfig = {
        .width = set_width,
        .height = set_height,
        .lms_out_bitwidth = 14,
        .gtm_out_bitwidth = 10,
    };
    ret = inf_algo_create(&g_pstInfAlgObject,"/usrdata/iray_demo/params.json",&stInfAlgParams,&stPriConfig,NULL);
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

            g_pstInfAlgObject->mapping_color_process(g_pstInfAlgObject,
                                tmp_yuv_buf.pViraddr, 
                                tmp_yuv_buf.pViraddr + y_offset, 
                                tmp_yuv_buf.pViraddr + y_offset + u_offset,
                                irFrame.stVFrame.u32Width, irFrame.stVFrame.u32Height,
                                irFrame.stVFrame.u32Stride[0], irFrame.stVFrame.u32Stride[1], irFrame.stVFrame.u32Stride[2]);

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


static void *stream_yuv_process_thread(void* arg)
{
	printf("yuv_process_thread start \n");
    AR_S32 s32Ret;
    STRU_YUV_PROCESS_PARAM_T* pstYuvProcessParam = (STRU_YUV_PROCESS_PARAM_T*)arg;
    VPSS_GRP VpssGrp_vis = pstYuvProcessParam->VpssGrp_vis;
    VPSS_CHN VpssChn_vis = pstYuvProcessParam->VpssChn_vis;
    VPSS_GRP VpssGrp_inf = pstYuvProcessParam->VpssGrp_inf;
    VPSS_CHN VpssChn_inf = pstYuvProcessParam->VpssChn_inf;

    sleep(3);
    
	while(1)
	{
	    VIDEO_FRAME_INFO_S irFrame = {0};
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

        VIDEO_FRAME_INFO_S fusionFrame = {0};
        time_statistics_begin(&gTSTmp);
        s32Ret = g_pstInfAlgObject->fusion_process(g_pstInfAlgObject,&irFrame,&visFrame,&fusionFrame);
        time_statistics_end(&gTSTmp, isPrintTimeLog, "fusion process");
        if(s32Ret==0){
            if(gOutMode==1) AR_MPI_VO_SendFrame(0, 3, &fusionFrame, 0);
            if(gOutMode==2) AR_MPI_VENC_SendFrame(2, &fusionFrame, -1);
        }

        s32Ret = AR_MPI_VPSS_ReleaseChnFrame(VpssGrp_vis, VpssChn_vis, &visFrame);
        s32Ret = AR_MPI_VPSS_ReleaseChnFrame(VpssGrp_inf, VpssChn_inf, &irFrame);

	}
	return 0;
}

int onGetNucData(unsigned short *data,int len)
{
	//printf("get nuc data %p %d \n",data,len);
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

	s32Ret = ISPIN_VI_Buf_Loop_Q_RawFrame(&VideoFrame);
	//s32Ret=AR_MPI_VI_SendPipeRaw(g_RawProcessParam->ViPipe_isp,pstVideoFrame,-1);
	if(s32Ret<0)
	{
		SAMPLE_PRT("AR_MPI_VI_SendPipeRaw failed exit the thread\n");
	}

	return 0;
}

AR_S32 SAMPLE_elf1_dual_test(AR_S32 outMode)
{
    AR_S32             s32Ret;

    AR_S32			   s32ViCnt 	  = 3;
    AR_S32             s32WorkSnsId;

    SAMPLE_VI_CONFIG_S stViConfig;

    SIZE_S             stSize;
    VB_CONFIG_S        stVbConf;
    //PIC_SIZE_E         enPicSize;
    AR_U32             u32BlkSize;

    VO_CHN             VoChn          = 0;
    SAMPLE_VO_CONFIG_S stVoConfig;

    //SAMPLE_VI_INFO_S *pstViInfo = NULL;

    unsigned char* tmp_buf = (unsigned char*)malloc(512*204);
    int i;
    AR_S32              s32OutMode = outMode;
    gOutMode = outMode;

    if(DUAL_ISPIN) s32ViCnt = 4;

    VPSS_GRP VpssGrp_vis = 0;
    VPSS_CHN VpssChn_vis0 = 0;
    VPSS_CHN VpssChn_vis1 = 1;
    VPSS_GRP VpssGrp_inf = 1;
    VPSS_CHN VpssChn_inf0 = 0;
    VPSS_CHN VpssChn_inf1 = 1;

    //update demo config
    get_demo_config_value();

    /*config vi*/
    SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

    stViConfig.s32WorkingViNum    = s32ViCnt;
    stViConfig.as32WorkingViId[0] = 0;
	stViConfig.as32WorkingViId[1] = 1;
    stViConfig.as32WorkingViId[2] = 2;
    stViConfig.as32WorkingViId[3] = 3;

    s32WorkSnsId = 0;
    VI_DEV			   ViDev_vis		  = 0;
    VI_PIPE 		   ViPipe_vis		  = ViDev_vis;
    VI_CHN			   ViChn_vis    	  = 0;
    {
        DYNAMIC_RANGE_E    enDynamicRange = DYNAMIC_RANGE_SDR8;
        PIXEL_FORMAT_E     enPixFormat    = PIXEL_FORMAT_YVU_PLANAR_420;
        VIDEO_FORMAT_E     enVideoFormat  = VIDEO_FORMAT_LINEAR;
        COMPRESS_MODE_E    enCompressMode = COMPRESS_MODE_NONE;

        g_enSnsType[s32WorkSnsId] = SENSOR0_TYPE;
        
        stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = s32WorkSnsId;
    	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 1;
    	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev  = 0;
    	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = SENSOR0_TYPE;

    	stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev			 = ViDev_vis;
    	stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode		 = WDR_MODE_NONE;
    	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
    	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0]		 = ViPipe_vis;
    	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1]		 = -1;
    	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2]		 = -1;
    	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3]		 = -1;
    	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn			 = ViChn_vis;
    	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat	 = enPixFormat;
    	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange  = enDynamicRange;
    	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat	 = enVideoFormat;
	    stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode  = enCompressMode;
    }
    SAMPLE_PRT("%d---------------enSnsType=%d \n",s32WorkSnsId,stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType);

    s32WorkSnsId = 1;
    VI_DEV			   ViDev_raw		  = 1;
    VI_PIPE 		   ViPipe_raw		  = ViDev_raw;
    VI_CHN			   ViChn_raw    	  = 2;
    {
        DYNAMIC_RANGE_E    enDynamicRange = DYNAMIC_RANGE_SDR8;
        PIXEL_FORMAT_E     enPixFormat    = PIXEL_FORMAT_RGB_BAYER_8BPP;
        VIDEO_FORMAT_E     enVideoFormat  = VIDEO_FORMAT_LINEAR;
        COMPRESS_MODE_E    enCompressMode = COMPRESS_MODE_NONE;

        g_enSnsType[s32WorkSnsId] = SENSOR2_TYPE;
        
        stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = s32WorkSnsId;
    	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 1;
    	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev  = 1;
    	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = SENSOR2_TYPE;

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
    SAMPLE_PRT("%d---------------enSnsType=%d \n",s32WorkSnsId,stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType);

    s32WorkSnsId = 2;
    VI_DEV			   ViDev_isp		  = 2;
    VI_PIPE 		   ViPipe_isp		  = ViDev_isp;
    VI_CHN			   ViChn_isp    	  = 0;
    {
        DYNAMIC_RANGE_E    enDynamicRange = DYNAMIC_RANGE_SDR8;
        PIXEL_FORMAT_E     enPixFormat    = PIXEL_FORMAT_YVU_PLANAR_420;
        VIDEO_FORMAT_E     enVideoFormat  = VIDEO_FORMAT_LINEAR;
        COMPRESS_MODE_E    enCompressMode = COMPRESS_MODE_NONE;

        g_enSnsType[s32WorkSnsId] = ISP_VIN_COMMON_INF;
        
        stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = s32WorkSnsId;
    	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 0;
    	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev  = 0;
    	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = ISP_VIN_COMMON_INF;

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
    s32WorkSnsId = 3;
    VI_DEV			   ViDev_isp1		  = 3;
    VI_PIPE 		   ViPipe_isp1		  = ViDev_isp1;
    VI_CHN			   ViChn_isp1    	  = 0;
    {
        DYNAMIC_RANGE_E    enDynamicRange = DYNAMIC_RANGE_SDR8;
        PIXEL_FORMAT_E     enPixFormat    = PIXEL_FORMAT_YVU_PLANAR_420;
        VIDEO_FORMAT_E     enVideoFormat  = VIDEO_FORMAT_LINEAR;
        COMPRESS_MODE_E    enCompressMode = COMPRESS_MODE_NONE;

        g_enSnsType[s32WorkSnsId] = ISP_VIN_COMMON_INF1;
        
        stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = s32WorkSnsId;
    	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 0;
    	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev  = 0;
    	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = ISP_VIN_COMMON_INF1;

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
    SAMPLE_PRT("%d---------------enSnsType=%d \n",s32WorkSnsId,stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType);

    g_RawProcessParam.ViPipe_isp = ViPipe_isp;
    g_RawProcessParam.ViChn_isp = ViChn_isp;
    if(DUAL_ISPIN){
        g_RawProcessParam.ViPipe_isp1 = ViPipe_isp1;
        g_RawProcessParam.ViChn_isp1 = ViChn_isp1;
    }

    /*config vb*/
    ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
    stVbConf.u32MaxPoolCnt = DUAL_ISPIN?6:4;

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
    stVbConf.astCommPool[1].u32BlkCnt  = 15;

    stSize.u32Width = 1920;
    stSize.u32Height = 1080;
    u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, 
        PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
    stVbConf.astCommPool[2].u64BlkSize  = u32BlkSize;
    stVbConf.astCommPool[2].u32BlkCnt   = 12;

    stSize.u32Width = IFUSION_W;
    stSize.u32Height = IFUSION_H;
    u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height,
        PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
    stVbConf.astCommPool[3].u64BlkSize = u32BlkSize;
    stVbConf.astCommPool[3].u32BlkCnt  = 10;

    if(DUAL_ISPIN){
        /* for ispin vi buffer loop */
        stSize.u32Width = IYUV_W;
        stSize.u32Height = IYUV_H;
        u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_8BPP, COMPRESS_MODE_NONE, 512);
        stVbConf.astCommPool[4].u64BlkSize = u32BlkSize;
        stVbConf.astCommPool[4].u32BlkCnt  = 3;

        stSize.u32Width = IYUV_W*2;
        stSize.u32Height = IYUV_H;
        u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_8BPP, COMPRESS_MODE_NONE, 256);
        stVbConf.astCommPool[5].u64BlkSize = u32BlkSize;
        stVbConf.astCommPool[5].u32BlkCnt  = 3;
    }

    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (AR_SUCCESS != s32Ret) {
        SAMPLE_PRT("system init failed with %d!\n", s32Ret);
        return s32Ret;
    }

    SAMPLE_AR_MPI_VIN_OpenDev(2);

    /*start vi*/
    
    s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
    if (AR_SUCCESS != s32Ret) {
        SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
        goto EXIT;
    }

    if(s32OutMode == 0)
    {//only vio
        /************************************************
         start VO
        *************************************************/
        SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
        stVoConfig.VoDev                                    = SAMPLE_VO_DEV_DHD0;
        stVoConfig.enVoIntfType                             = VO_INTF_HDMI;
        stVoConfig.enIntfSync                               = VO_OUTPUT_1080P60;
        //stVoConfig.enPicSize                                = enPicSize;
        stVoConfig.u32DisBufLen                             = 3;
        stVoConfig.enDstDynamicRange                        = DYNAMIC_RANGE_SDR8;
        stVoConfig.enVoMode                                 = VO_MODE_4MUX;
    
        s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);
    
        if (AR_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %d!\n", s32Ret);
            goto EXIT2;
        }

        SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO \n");
        VoChn = 1;
        s32Ret = SAMPLE_COMM_VI_Bind_VO(ViPipe_isp, ViChn_isp, 0, VoChn);
    
        if (AR_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO failed with %d!\n", s32Ret);
            goto EXIT2;
        }
        
        VoChn = 0;
        s32Ret = SAMPLE_COMM_VI_Bind_VO(ViPipe_vis, ViChn_vis, 0, 0);
    
        if (AR_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO failed with %d!\n", s32Ret);
            goto EXIT2;
        }
    }
    else if(s32OutMode == 1 || s32OutMode == 2)
    {//dual fusion

        /************************************************
             start VPSS for fusion
        *************************************************/
        //vis group
        VpssGrp_vis = 0;
        VpssChn_vis0 = 0;
        VpssChn_vis1 = 1;
        {
            VPSS_GRP            VpssGrp = VpssGrp_vis;
            DYNAMIC_RANGE_E     enDynamicRange = DYNAMIC_RANGE_SDR8;
            PIXEL_FORMAT_E      enPixFormat    = PIXEL_FORMAT_YVU_PLANAR_420;
            VIDEO_FORMAT_E      enVideoFormat  = VIDEO_FORMAT_LINEAR;
            COMPRESS_MODE_E     enCompressMode = COMPRESS_MODE_NONE;
            VPSS_GRP_ATTR_S     stVpssGrpAttr = {0};

            SIZE_S GrpOutSize = {1920,1080};
            AR_S32 s32fps = 30;
            
            stVpssGrpAttr.stFrameRate.s32SrcFrameRate    = s32fps;
            stVpssGrpAttr.stFrameRate.s32DstFrameRate    = s32fps;
            stVpssGrpAttr.u32Width                       = GrpOutSize.u32Width;
            stVpssGrpAttr.u32Height                      = GrpOutSize.u32Height;
            stVpssGrpAttr.enCompressMode                 = enCompressMode;
            stVpssGrpAttr.enDynamicRange                 = enDynamicRange;
            stVpssGrpAttr.enVideoFormat                  = enVideoFormat;
            stVpssGrpAttr.enPixelFormat                  = enPixFormat;
            //ldc in group
            stVpssGrpAttr.stLdcAttr.bEnable = AR_TRUE;
            stVpssGrpAttr.stLdcAttr.stAttr.ldc_k0 = -0.4501 ;
            stVpssGrpAttr.stLdcAttr.stAttr.ldc_k1 =	0.2584 ;
            stVpssGrpAttr.stLdcAttr.stAttr.ldc_k2 = -0.0927 ;
            stVpssGrpAttr.stLdcAttr.stAttr.k[0]=1367.9;
            stVpssGrpAttr.stLdcAttr.stAttr.k[1]=0;
            stVpssGrpAttr.stLdcAttr.stAttr.k[2]=962.29;
            stVpssGrpAttr.stLdcAttr.stAttr.k[3]=0;
            stVpssGrpAttr.stLdcAttr.stAttr.k[4]=1367.7;
            stVpssGrpAttr.stLdcAttr.stAttr.k[5]=513.1752;
            stVpssGrpAttr.stLdcAttr.stAttr.k[6]=0;
            stVpssGrpAttr.stLdcAttr.stAttr.k[7]=0;
            stVpssGrpAttr.stLdcAttr.stAttr.k[8]=1;

            //start group
            s32Ret = AR_MPI_VPSS_CreateGrp(VpssGrp, &stVpssGrpAttr);
            if (s32Ret != AR_SUCCESS)
            {
                SAMPLE_PRT("AR_MPI_VPSS_CreateGrp(grp:%d) failed with %#x!\n", VpssGrp, s32Ret);
                goto EXIT2;
            }
            s32Ret = AR_MPI_VPSS_StartGrp(VpssGrp);
            if (s32Ret != AR_SUCCESS)
            {
                SAMPLE_PRT("AR_MPI_VPSS_StartGrp failed with %#x\n", s32Ret);
                goto EXIT2;
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
            stVpssChnAttr.stFrameRate.s32SrcFrameRate  = s32fps;
            stVpssChnAttr.stFrameRate.s32DstFrameRate  = s32fps;
            stVpssChnAttr.u32Depth                     = 2;
            {
                //channel 0
                VpssChn = VpssChn_vis0;
                
                s32Ret = AR_MPI_VPSS_SetChnAttr(VpssGrp, VpssChn, &stVpssChnAttr);
                if (s32Ret != AR_SUCCESS)
                {
                    SAMPLE_PRT("AR_MPI_VPSS_SetChnAttr failed with %#x\n", s32Ret);
                    goto EXIT2;
                }

                AR_U32 u32Align = 64;
                s32Ret = AR_MPI_VPSS_SetChnAlign(VpssGrp, VpssChn, u32Align);
                if (s32Ret != AR_SUCCESS)
                {
                    SAMPLE_PRT("AR_MPI_VPSS_SetChnAlign failed with %#x\n", s32Ret);
                    goto EXIT2;
                }

                s32Ret = AR_MPI_VPSS_EnableChn(VpssGrp, VpssChn);
                if (s32Ret != AR_SUCCESS)
                {
                    SAMPLE_PRT("AR_MPI_VPSS_EnableChn failed with %#x\n", s32Ret);
                    goto EXIT2;
                }
            }
            {
                //channel 1
                VpssChn = VpssChn_vis1;
                stVpssChnAttr.u32Width                     = IFUSION_W;
                stVpssChnAttr.u32Height                    = IFUSION_H;
                
                s32Ret = AR_MPI_VPSS_SetChnAttr(VpssGrp, VpssChn, &stVpssChnAttr);
                if (s32Ret != AR_SUCCESS)
                {
                    SAMPLE_PRT("AR_MPI_VPSS_SetChnAttr failed with %#x\n", s32Ret);
                    goto EXIT2;
                }

                VPSS_CROP_INFO_S stCropInfo;
                stCropInfo.bEnable = true;
                stCropInfo.enCropCoordinate = 1;
                stCropInfo.stCropRect.s32X = crop_x;
                stCropInfo.stCropRect.s32Y = crop_y;
                stCropInfo.stCropRect.u32Width = crop_w;
                stCropInfo.stCropRect.u32Height = crop_h;
                s32Ret = AR_MPI_VPSS_SetChnCrop(VpssGrp_vis, VpssChn, &stCropInfo);

                AR_U32 u32Align = 64;
                s32Ret = AR_MPI_VPSS_SetChnAlign(VpssGrp, VpssChn, u32Align);
                if (s32Ret != AR_SUCCESS)
                {
                    SAMPLE_PRT("AR_MPI_VPSS_SetChnAlign failed with %#x\n", s32Ret);
                    goto EXIT2;
                }

                s32Ret = AR_MPI_VPSS_EnableChn(VpssGrp, VpssChn);
                if (s32Ret != AR_SUCCESS)
                {
                    SAMPLE_PRT("AR_MPI_VPSS_EnableChn failed with %#x\n", s32Ret);
                    goto EXIT2;
                }
            }
        }
        //inf group
        VpssGrp_inf = 1;
        VpssChn_inf0 = 0;
        VpssChn_inf1 = 1;
        {
            VPSS_GRP            VpssGrp = VpssGrp_inf;
            DYNAMIC_RANGE_E     enDynamicRange = DYNAMIC_RANGE_SDR8;
            PIXEL_FORMAT_E      enPixFormat    = PIXEL_FORMAT_YVU_PLANAR_420;
            VIDEO_FORMAT_E      enVideoFormat  = VIDEO_FORMAT_LINEAR;
            COMPRESS_MODE_E     enCompressMode = COMPRESS_MODE_NONE;
            VPSS_GRP_ATTR_S     stVpssGrpAttr = {0};

            SIZE_S GrpOutSize = {IYUV_W,IYUV_H};
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
                goto EXIT2;
            }
            s32Ret = AR_MPI_VPSS_StartGrp(VpssGrp);
            if (s32Ret != AR_SUCCESS)
            {
                SAMPLE_PRT("AR_MPI_VPSS_StartGrp failed with %#x\n", s32Ret);
                goto EXIT2;
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
            stVpssChnAttr.stFrameRate.s32SrcFrameRate  = s32fps;
            stVpssChnAttr.stFrameRate.s32DstFrameRate  = s32fps;
            stVpssChnAttr.u32Depth                     = 2;
            //start two same channel
            for(i=0;i<2;i++)
            {
                //channel 0
                VpssChn = i;

                if(i==VpssChn_inf1)
                {
                    stVpssChnAttr.u32Width                     = IFUSION_W;
                    stVpssChnAttr.u32Height                    = IFUSION_H;
                }
                s32Ret = AR_MPI_VPSS_SetChnAttr(VpssGrp, VpssChn, &stVpssChnAttr);
                if (s32Ret != AR_SUCCESS)
                {
                    SAMPLE_PRT("AR_MPI_VPSS_SetChnAttr failed with %#x\n", s32Ret);
                    goto EXIT2;
                }

                AR_U32 u32Align = 64;
                s32Ret = AR_MPI_VPSS_SetChnAlign(VpssGrp, VpssChn, u32Align);
                if (s32Ret != AR_SUCCESS)
                {
                    SAMPLE_PRT("AR_MPI_VPSS_SetChnAlign failed with %#x\n", s32Ret);
                    goto EXIT2;
                }

                s32Ret = AR_MPI_VPSS_EnableChn(VpssGrp, VpssChn);
                if (s32Ret != AR_SUCCESS)
                {
                    SAMPLE_PRT("AR_MPI_VPSS_EnableChn failed with %#x\n", s32Ret);
                    goto EXIT2;
                }
            }
        }

        //vi(vis) bind vpss
        s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe_vis, ViChn_vis, VpssGrp_vis);
        if (AR_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VPSS failed with %d!\n", s32Ret);
            goto EXIT2;
        }

        if(s32OutMode == 1)
        {//hdmi
            /************************************************
             start VO
            *************************************************/
            SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
            stVoConfig.VoDev                                    = SAMPLE_VO_DEV_DHD0;
            stVoConfig.enVoIntfType                             = VO_INTF_HDMI;
            stVoConfig.enIntfSync                               = VO_OUTPUT_1080P60;
            //stVoConfig.enPicSize                                = enPicSize;
            stVoConfig.u32DisBufLen                             = 3;
            stVoConfig.enDstDynamicRange                        = DYNAMIC_RANGE_SDR8;
            stVoConfig.enVoMode                                 = VO_MODE_4MUX;

            VO_LAYER VoLayer = 0;
            
            s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);
            if (AR_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %d!\n", s32Ret);
                goto EXIT2;
            }

            //vpss vind vo
            s32Ret = SAMPLE_COMM_VPSS_Bind_VO(VpssGrp_vis,VpssChn_vis0,VoLayer,0);
            if (AR_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("SAMPLE_COMM_VPSS_Bind_VO failed with %d!\n", s32Ret);
                goto EXIT2;
            }

            s32Ret = SAMPLE_COMM_VPSS_Bind_VO(VpssGrp_inf,VpssChn_inf0,VoLayer,1);
            if (AR_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("SAMPLE_COMM_VPSS_Bind_VO failed with %d!\n", s32Ret);
                goto EXIT2;
            }
        }
        else if(s32OutMode == 2)
        {//rtsp
            /************************************************
             start VENC
            *************************************************/
            VENC_CHN VencChn_vis = 0;
            VENC_CHN VencChn_inf0 = 1;
            VENC_CHN VencChn_inf1 = 2;

            AR_U32 u32VencWidth;
            AR_U32 u32VencHeight;
            AR_U32 u32VencBitrate = 2048;
            AR_U32 u32VencBufSize = 2*1024*1024;
            AR_U32 u32VencFps;

            AR_U32 u32EncoderCnt=3;
            //如果三路的bitrate不同，请分开计算DoneSize并累加，可以参考ipcam
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
            stParam.stEventModParam.u32VencDoneQueueSize  = u32VencDoneQueueSize * u32EncoderCnt;
            s32Ret = AR_MPI_VENC_SetModParam(&stParam);
            if(s32Ret){
                SAMPLE_PRT("Set venc mod param error! ret = %x\n", s32Ret);
                goto EXIT2;
            }

            VENC_CHN_ATTR_S stChnAttr;
            memset(&stChnAttr, 0, sizeof(stChnAttr));

            //create vis channel
            u32VencWidth = 1920;
            u32VencHeight = 1080;
            u32VencBitrate = 2048;
            u32VencBufSize = u32VencBitrate*3*1024/8 + u32VencWidth*u32VencHeight*3/2;
            u32VencFps = 25;
            stChnAttr.stVencAttr.enType = PT_H265;
            stChnAttr.stVencAttr.u32PicWidth = u32VencWidth;
            stChnAttr.stVencAttr.u32PicHeight = u32VencHeight;
            stChnAttr.stVencAttr.u32MaxPicWidth = u32VencWidth;
            stChnAttr.stVencAttr.u32MaxPicHeight = u32VencHeight;
            stChnAttr.stVencAttr.u32BufSize = u32VencBufSize;
            stChnAttr.stVencAttr.bByFrame = AR_TRUE;
            stChnAttr.stVencAttr.enPixelFormat = PIXEL_FORMAT_YVU_PLANAR_420;
            stChnAttr.stVencAttr.stAttrH265e.u32CmdQueueDepth = 2;
            stChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265CBR;
            stChnAttr.stRcAttr.stH265Cbr.u32SrcFrameRate = u32VencFps;
            stChnAttr.stRcAttr.stH265Cbr.u32BitRate = u32VencBitrate;
            stChnAttr.stRcAttr.stH265Cbr.u32Gop = u32VencFps*2;
            stChnAttr.stRcAttr.stH265Cbr.u32StatTime = 30;          //must set
            stChnAttr.stGopAttr.enGopMode = VENC_GOPMODE_NORMALP;
            stChnAttr.stGopAttr.stNormalP.s32IPQpDelta = -2;
            
            s32Ret = AR_MPI_VENC_CreateChn(VencChn_vis, &stChnAttr);
        	if (s32Ret) {
        		printf("create venc channel failed\n");
        		goto EXIT2;
        	}

        	//create inf channel 0
        	u32VencWidth = 256;
            u32VencHeight = 192;
            u32VencBitrate = 2048;
            u32VencBufSize =  u32VencBitrate*3*1024/8 + u32VencWidth*u32VencHeight*3/2;
            u32VencFps = 25;

            stChnAttr.stVencAttr.enType = PT_H265;
            stChnAttr.stVencAttr.u32PicWidth = u32VencWidth;
            stChnAttr.stVencAttr.u32PicHeight = u32VencHeight;
            stChnAttr.stVencAttr.u32MaxPicWidth = u32VencWidth;
            stChnAttr.stVencAttr.u32MaxPicHeight = u32VencHeight;
            stChnAttr.stVencAttr.u32BufSize = u32VencBufSize;
            stChnAttr.stVencAttr.bByFrame = AR_TRUE;
            stChnAttr.stVencAttr.enPixelFormat = PIXEL_FORMAT_YVU_PLANAR_420;
            stChnAttr.stVencAttr.stAttrH265e.u32CmdQueueDepth = 2;
            stChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265CBR;
            stChnAttr.stRcAttr.stH265Cbr.u32SrcFrameRate = u32VencFps;
            stChnAttr.stRcAttr.stH265Cbr.u32BitRate = u32VencBitrate;
            stChnAttr.stRcAttr.stH265Cbr.u32Gop = u32VencFps*2;
            stChnAttr.stRcAttr.stH265Cbr.u32StatTime = 30;          //must set
            stChnAttr.stGopAttr.enGopMode = VENC_GOPMODE_NORMALP;
            stChnAttr.stGopAttr.stNormalP.s32IPQpDelta = -2;
            
            s32Ret = AR_MPI_VENC_CreateChn(VencChn_inf0, &stChnAttr);
        	if (s32Ret) {
        		printf("create venc channel failed\n");
        		goto EXIT2;
        	}

        	//create inf channel 1
        	u32VencWidth = IFUSION_W;
            u32VencHeight = IFUSION_H;
            u32VencBitrate = 2048;
            u32VencBufSize =  u32VencBitrate*3*1024/8 + u32VencWidth*u32VencHeight*3/2;
            u32VencFps = 25;

            stChnAttr.stVencAttr.enType = PT_H265;
            stChnAttr.stVencAttr.u32PicWidth = u32VencWidth;
            stChnAttr.stVencAttr.u32PicHeight = u32VencHeight;
            stChnAttr.stVencAttr.u32MaxPicWidth = u32VencWidth;
            stChnAttr.stVencAttr.u32MaxPicHeight = u32VencHeight;
            stChnAttr.stVencAttr.u32BufSize = u32VencBufSize;
            stChnAttr.stVencAttr.bByFrame = AR_TRUE;
            stChnAttr.stVencAttr.enPixelFormat = PIXEL_FORMAT_YVU_PLANAR_420;
            stChnAttr.stVencAttr.stAttrH265e.u32CmdQueueDepth = 2;
            stChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265CBR;
            stChnAttr.stRcAttr.stH265Cbr.u32SrcFrameRate = u32VencFps;
            stChnAttr.stRcAttr.stH265Cbr.u32BitRate = u32VencBitrate;
            stChnAttr.stRcAttr.stH265Cbr.u32Gop = u32VencFps*2;
            stChnAttr.stRcAttr.stH265Cbr.u32StatTime = 30;          //must set
            stChnAttr.stGopAttr.enGopMode = VENC_GOPMODE_NORMALP;
            stChnAttr.stGopAttr.stNormalP.s32IPQpDelta = -2;
            s32Ret = AR_MPI_VENC_CreateChn(VencChn_inf1, &stChnAttr);
        	if (s32Ret) {
        		printf("create venc channel failed\n");
        		goto EXIT2;
        	}
            
            VENC_RECV_PIC_PARAM_S param;
        	param.s32RecvPicNum = -1;
            
            s32Ret = AR_MPI_VENC_StartRecvFrame(VencChn_inf0, &param);
        	if (s32Ret != 0) {
        		printf("venc dev id:%d start failed! ret=%d\n", VencChn_inf0, s32Ret);
        		goto EXIT2;
        	}
            s32Ret = AR_MPI_VENC_StartRecvFrame(VencChn_inf1, &param);
        	if (s32Ret != 0) {
        		printf("venc dev id:%d start failed! ret=%d\n", VencChn_inf1, s32Ret);
        		goto EXIT2;
        	}
            s32Ret = AR_MPI_VENC_StartRecvFrame(VencChn_vis, &param);
        	if (s32Ret != 0) {
        		printf("venc dev id:%d start failed! ret=%d\n", VencChn_vis, s32Ret);
        		goto EXIT2;
        	}

            //vpss bind venc
            s32Ret = SAMPLE_COMM_VPSS_Bind_VENC(VpssGrp_vis,VpssChn_vis0, VencChn_vis);
            if (AR_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("SAMPLE_COMM_VPSS_Bind_VENC failed with %d!\n", s32Ret);
                goto EXIT2;
            }

        	s32Ret = SAMPLE_COMM_VPSS_Bind_VENC(VpssGrp_inf,VpssChn_inf0, VencChn_inf0);
            if (AR_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("SAMPLE_COMM_VPSS_Bind_VENC failed with %d!\n", s32Ret);
                goto EXIT2;
            }

            smp_rtsp_enable_venc_chn(0, VencChn_vis, "/ch0/stream0");
            smp_rtsp_enable_venc_chn(1, VencChn_inf0, "/ch1/stream0");
            smp_rtsp_enable_venc_chn(2, VencChn_inf1, "/ch1/stream1");
            smp_rtsp_start();
        }
    }

    //use yuv color map thread instead of bind
    STRU_YUV_PROCESS_PARAM_T stColorProcessParam;
    pthread_t color_process_thread;
    stColorProcessParam.ViPipe_isp = DUAL_ISPIN?ViPipe_isp1:ViPipe_isp;
    stColorProcessParam.ViChn_isp = DUAL_ISPIN?ViChn_isp1:ViChn_isp;
    stColorProcessParam.VpssGrp_inf = VpssGrp_inf;
    if(s32OutMode == 1 || s32OutMode == 2){
        s32Ret = pthread_create(&color_process_thread, NULL, colormap_process_thread, &stColorProcessParam);
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

    s32Ret = iray_init();
    if(s32Ret<0){
		printf("iray init fail.\n");
        goto EXIT2;
	}
    iray_set_nuc_callback(onGetNucData);

    
    //start yuv process
    STRU_YUV_PROCESS_PARAM_T stYuvProcessParam;
    stYuvProcessParam.VpssGrp_vis = VpssGrp_vis;
    stYuvProcessParam.VpssChn_vis =  VpssChn_vis1;
    stYuvProcessParam.VpssGrp_inf = VpssGrp_inf;
    stYuvProcessParam.VpssChn_inf =  VpssChn_inf1;
    pthread_t yuv_process_thread;
    if(s32OutMode==1 || s32OutMode == 2){
        s32Ret = pthread_create(&yuv_process_thread, NULL, stream_yuv_process_thread, &stYuvProcessParam);
    }

    s32Ret = init_inf_alg_obj(IYUV_W,IYUV_H);
    if(s32Ret!=0){
        printf("inf_algo_create fail.\n");
        goto EXIT2;
    }

    pthread_t debug_thread;
    s32Ret = pthread_create(&debug_thread, NULL, debug_update_thread, NULL);

    while(1)
	{
        //get a raw from raw ch and send to isp in dev
        VIDEO_FRAME_INFO_S *pstFrameInfo=malloc(sizeof(VIDEO_FRAME_INFO_S));
        
		s32Ret = AR_MPI_VI_GetChnFrame(ViPipe_raw,ViChn_raw,pstFrameInfo,500000);
        if(s32Ret!=0)
        {
            ar_err("AR_MPI_VI_GetChnFrame failed. pipe=%d chn=%d ret=%d", ViPipe_raw,ViChn_raw,s32Ret);
            continue;
        }
        //printf("get chn frame %u %u %u\n",pstFrameInfo->stVFrame.u32Width, pstFrameInfo->stVFrame.u32Height,pstFrameInfo->stVFrame.u32Stride[0]);

        unsigned char* p_data_src = (unsigned char*)pstFrameInfo->stVFrame.u64VirAddr[0]+256;
        for(int h=0;h<203;h++){
            unsigned char* p_dest = tmp_buf+h*512;
            unsigned char* p_src1 = p_data_src+h*pstFrameInfo->stVFrame.u32Stride[0]+2;
            memcpy(p_dest,p_src1,512);
        }

        if(save_raw){
		    save_raw = 0;
            printf("will save raw, size: %u %u stride: %u %u %u valid: 256*204\n",
                pstFrameInfo->stVFrame.u32Width, pstFrameInfo->stVFrame.u32Height,
                pstFrameInfo->stVFrame.u32Stride[0],pstFrameInfo->stVFrame.u32Stride[1],pstFrameInfo->stVFrame.u32Stride[2]);
        
		    dump_to_file("/tmp/ori.raw",(const void*)pstFrameInfo->stVFrame.u64VirAddr[0],pstFrameInfo->stVFrame.u32Stride[0]*pstFrameInfo->stVFrame.u32Height);
            dump_to_file("/tmp/ori-valid.raw",tmp_buf,512*204);
	    }

        iray_push_raw_data(tmp_buf, 512*204);
        AR_MPI_VI_ReleaseChnFrame(ViPipe_raw,ViChn_raw,pstFrameInfo);
		free(pstFrameInfo);
	}

    free(tmp_buf);
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
