#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */
#include "../sample_vio.h"
#include "sample_lcd.h"


extern ISP_SNS_OBJ_S stSnsImx290Obj;
extern STRU_DISPLAY_OBJ_T g_vo_obj;
extern volatile AR_S8 g_exit;

const char *elog_port_get_time(void) {
    static char cur_system_time[10] = { 0 };
	/*
    time_t timep;
    struct tm *p;

    time(&timep);
    p = localtime(&timep);
    if (p == NULL) {
        return "";
    }
    snprintf(cur_system_time, 18, "%02d-%02d %02d:%02d:%02d", p->tm_mon + 1, p->tm_mday,
            p->tm_hour, p->tm_min, p->tm_sec);*/
    struct timeval tv;
    gettimeofday(&tv, NULL);

	unsigned long time = tv.tv_sec*1000 + tv.tv_usec/1000;
	unsigned int t = (unsigned int)time;

	snprintf(cur_system_time, 10, "%u", t);

    return cur_system_time;
}

AR_S32 SAMPLE_VIO_sof_lowdelay(SAMPLE_VIO_PRA *ppra)
{
	AR_S32			   s32Ret, s32pra_index = 0;

    ISP_SNS_OBJ_S *p_obj=&stSnsImx290Obj;
	SRTU_SENSOR_DEFAULT_ATTR_T default_attr;
	SIZE_S			   stSize;
	VB_CONFIG_S 	   stVbConf;
	AR_U32			   u32BlkSize;

	AR_S32			   s32ViCnt 	  = 1;
	VI_DEV			   ViDev		  = 0;
	VI_PIPE 		   ViPipe		  = ViDev;
	VI_CHN			   ViChn		  = 0;
	AR_S32             mipi_index      =0;
	AR_S8              s8I2cDev        =1;

	VO_CHN			   VoChn		  = 0;
	SAMPLE_VO_CONFIG_S stVoConfig;


	if(p_obj->pfnGetDefaultAttr)
	{
	   p_obj->pfnGetDefaultAttr(0,&default_attr);
	}else
	{
	  ar_err("pfnGetDefaultAttr is null, exit the test");
	  return -1;
	}

	int fps_sensor=25;
    s32pra_index = sample_find_pra_by_name("-fps", ppra->argc, ppra->argv);
	if(s32pra_index > 0)
   	{
    	fps_sensor=atoi(ppra->argv[s32pra_index+1]);
   	}
	int check=25;
    s32pra_index = sample_find_pra_by_name("-check", ppra->argc, ppra->argv);
	if(s32pra_index > 0)
   	{
    	check=atoi(ppra->argv[s32pra_index+1]);
   	}


    stSize=default_attr.stPubAttr.stSnsSize;
	//s8I2cDev = ppra->s32I2cDev;
    /*config vb*/
    ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
    stVbConf.u32MaxPoolCnt              = 2;

    u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
    stVbConf.astCommPool[0].u64BlkSize  = u32BlkSize;
    stVbConf.astCommPool[0].u32BlkCnt   = 10;

    u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
    stVbConf.astCommPool[1].u64BlkSize  = u32BlkSize;
    stVbConf.astCommPool[1].u32BlkCnt   = 10;

    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("system init failed with %d!\n", s32Ret);
        return s32Ret;
    }


	if(ppra->isp_fre==0)
	{
	     ppra->isp_fre=300000000;
    }

	if(ppra->vif_fre==0)
	{
	   ppra->vif_fre=300000000;
	}

	if(ppra->pcs_fre==0)
	{
	   ppra->pcs_fre=100000000;
	}

    if(ppra->cam_mode>2){
	    SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre,ppra->vif_fre,ppra->isp_fre,ppra->pcs_fre);
    }else{
	    SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre,ppra->vif_fre,ppra->isp_fre,ppra->pcs_fre);
	}

    if(ppra->settle>0)
	{
       default_attr.stComboAttr.mipi_attr.settle_count=ppra->settle;
    }

	if(ppra->mipi_fre > 0)
	{
	    default_attr.stComboAttr.mipi_attr.mipi_pix_clk=ppra->mipi_fre/1000/1000;
	}
	if(ppra->dpcm==1)
	{
	   default_attr.stPipeAttr.enCompressMode=COMPRESS_MODE_DPCM_6BITS;
	   default_attr.stPipeAttr.stNrAttr.enCompressMode=COMPRESS_MODE_DPCM_6BITS;
	}else if(ppra->dpcm==2)
	{
	    default_attr.stPipeAttr.enCompressMode=COMPRESS_MODE_NONE;
		default_attr.stPipeAttr.stNrAttr.enCompressMode=COMPRESS_MODE_NONE;
	}

	if(default_attr.stPipeAttr.enCompressMode==COMPRESS_MODE_NONE)
	{
	   default_attr.stPipeAttr.enEnableNoCmp=VIN_PIPE_NOCPM_ENABLE;
	}

	if(ppra->feature_mask){
	   default_attr.stPipeAttr.u32FeatureMask=ppra->feature_mask;
	}

	/*start vi*/
    AR_MPI_VI_SetMipiBindDev(ViDev, mipi_index);
	default_attr.stComboAttr.devno=mipi_index;
	AR_MPI_VI_SetComboDevAttr(&default_attr.stComboAttr);
    AR_MPI_VI_SetDevAttr(ViDev, &default_attr.stDevAttr);
	AR_MPI_VI_EnableDev(ViDev);
	VI_DEV_BIND_PIPE_S stDevBindPipe;
	stDevBindPipe.u32Num=1;
	stDevBindPipe.PipeId[0]=ViPipe;
	AR_MPI_VI_SetDevBindPipe(ViDev, &stDevBindPipe);
	AR_MPI_VI_CreatePipe(ViPipe, &default_attr.stPipeAttr);
	AR_MPI_VI_StartPipe(ViPipe);
	default_attr.stChnAttr.u32DepthClient[0]=-1;
	default_attr.stChnAttr.u32DepthClient[1]=3;
	AR_MPI_VI_SetChnAttr(ViPipe,ViChn,&default_attr.stChnAttr);

	VI_CHN_EXT_ATTR_S stChnEXtAttr={0};
    stChnEXtAttr.enLowDelayMode=VI_CH_LOW_DELAY_DDR;
	stChnEXtAttr.s32DelayLine=default_attr.stChnAttr.stSize.u32Height/4;
	stChnEXtAttr.stSoftLowdelayAttr.s32SofLowdeayEn=1;
	stChnEXtAttr.stSoftLowdelayAttr.u32LowdeayPattern=0x12345678;
	AR_MPI_VI_SetChnExtAttr(ViPipe,ViChn,&stChnEXtAttr);

	
	AR_MPI_VI_EnableChn(ViPipe,ViChn);


	ALG_LIB_S stAeLib;
	ALG_LIB_S stAwbLib;
	ALG_LIB_S stAFLib;
    stAeLib.s32Id = ViPipe;
    stAwbLib.s32Id = ViPipe;
	stAFLib.s32Id=ViPipe;

    strncpy(stAeLib.acLibName, "artosyn", sizeof("artosyn"));
    strncpy(stAwbLib.acLibName, "artosyn", sizeof("artosyn"));
    strncpy(stAFLib.acLibName, "artosyn", sizeof("artosyn"));

	if (p_obj->pfnRegisterCallback != AR_NULL)
	{
		  s32Ret = p_obj->pfnRegisterCallback(ViPipe, &stAeLib, &stAwbLib);

		  if (s32Ret != AR_SUCCESS)
		  {
			  ar_err("sensor_register_callback failed with %#x!\n", s32Ret);
			  goto EXIT;
		  }
	}
	else
	{
		  ar_err("sensor_register_callback failed with AR_NULL!\n");
		  goto EXIT;
	}

    if (AR_NULL != p_obj->pfnSetBusInfo)
    {
        ISP_SNS_COMMBUS_U uSnsBusInfo;
		uSnsBusInfo.s8I2cDev=s8I2cDev;
        s32Ret = p_obj->pfnSetBusInfo(ViPipe, uSnsBusInfo);

        if (s32Ret != AR_SUCCESS)
        {
             ar_err("set sensor bus info failed with %#x!\n", s32Ret);
             goto EXIT;
        }
    }
    else
    {
        ar_err("not support set sensor bus info!\n");
        goto EXIT;
    }
	#if 0
    //register vsync and aec update callback to pipe
    ISP_NOTIFY_REGISTER_S NotifyRegister={0};
	NotifyRegister.stNotify.aec_update=aec_update;
	NotifyRegister.stNotify.vsync_update=vsync_update;
    AR_MPI_ISP_RegisterNotifyCallBack(ViPipe,&NotifyRegister);
	#endif

	AR_MPI_ISP_MemInit(ViPipe);
	default_attr.stPubAttr.f32FrameRate = fps_sensor;
	AR_MPI_ISP_SetPubAttr(ViPipe, &default_attr.stPubAttr);


	VI_PIPE_EXT_ATTR_S stPipeAttr;
	AR_MPI_VI_GetPipeExtAttr(ViPipe, &stPipeAttr);
	stPipeAttr.bFoucs=0;
	AR_MPI_VI_SetPipeExtAttr(ViPipe, &stPipeAttr);

	AR_MPI_ISP_Init(ViPipe);


    //here to creat a thread to get 3a info every 1s

	pthread_t	ch_thread;

	pthread_attr_t* pstAttr = NULL;
	STRU_VIO_SAMPLE_FRAME_RUN_T pipe0_ch0;
	pipe0_ch0.pipe_id=0;
	pipe0_ch0.ch_id=0;
	pipe0_ch0.vo_ch_id=0;
	pipe0_ch0.nframes = ppra->s32nframes;

    //four layout
    AR_S32 mesh_w=1920;
    AR_S32 mesh_h=1080;

    //init display
    if (ppra->u32votype)
    {
    	ar_vo_dev_init_for_mipi(&g_vo_obj,1920,1080,-1,-1, 25, ppra->u32votype, 0);
    }
	else
	{
		ar_vo_dev_init(&g_vo_obj,1920,1080,-1,-1);
	}

	STRU_AR_HAL_VO_RECT chan_pos={0};
	chan_pos.x=0;
	chan_pos.y=0;
	chan_pos.w=mesh_w;
	chan_pos.h=mesh_h;
	ar_vo_surface_init(&g_vo_obj,0,&chan_pos);

	AR_MPI_ISP_Run(ViPipe);

    //get frame buffer
    //usleep(5000000);
    VIDEO_FRAME_INFO_S FrameInfo;
	AR_S32 ref=0;
	AR_S32 s32cnt = 0;
	AR_S32 s32nframes = ppra->s32nframes;
	AR_U64 last_pts=0;
	while(1){
		 int status=AR_MPI_VI_GetChnFrame(ViPipe,ViChn,&FrameInfo,5000);

		 if(status){
    		 if (g_exit)
    		 {
    			 break;
    		 }
			continue;
		 }

         //SAMPLE_PRT("pipe_id=%d %ld deta=%ld\n",ViPipe,FrameInfo.stVFrame.u64PTS,FrameInfo.stVFrame.u64PTS-last_pts);
         last_pts=FrameInfo.stVFrame.u64PTS;
		 ref++;

		 if(stChnEXtAttr.stSoftLowdelayAttr.s32SofLowdeayEn&&check)
		 {
			char *p_y=(char *)(FrameInfo.stVFrame.u64VirAddr[0]+(default_attr.stChnAttr.stSize.u32Width-4));
			for(int kk=stChnEXtAttr.s32DelayLine-1;kk<default_attr.stChnAttr.stSize.u32Height;)
			{
			    while(1)
				{
    				if(*((volatile uint32_t*)(p_y+kk*FrameInfo.stVFrame.u32Stride[0]))!=stChnEXtAttr.stSoftLowdelayAttr.u32LowdeayPattern)
    				{
    				   ar_printf("[%s]line %d is ok\n",elog_port_get_time(),kk);
					   break;
    				}
				    if (g_exit)
            	 	{
            			 break;
            	 	}
					ar_delay(1);
			    }
				kk+=stChnEXtAttr.s32DelayLine;
			}
			 while(1)
    		 {    		 
				 int kk=default_attr.stChnAttr.stSize.u32Height-1;					 
    			 if(*((volatile uint32_t*)(p_y+kk*FrameInfo.stVFrame.u32Stride[0]))!=stChnEXtAttr.stSoftLowdelayAttr.u32LowdeayPattern)
    			 {
    				ar_printf("[%s]line  %d last is ok, we can send to display \n",elog_port_get_time(),kk);
    				break;
    			 }
				 if (g_exit)
            	 {
            		 break;
            	 }
				 ar_delay(1);
    		 }
							
		 }


         //push to display
		 send_to_fw_display(&g_vo_obj,ViChn,&FrameInfo);

		 AR_MPI_VI_ReleaseChnFrame(ViPipe,ViChn,&FrameInfo);
		 if (s32nframes >= 0)
		 {
		 	s32cnt = ppra->s32nframes;
			s32nframes = -1;
		 }

		 if (s32cnt > 0)
	 	 {
			if (--s32cnt == 0)
			{
				s32cnt = 0;
				break;
			}
		 }

		 if (g_exit)
	 	 {
			 break;
	 	 }
	}

	close_display_surface(&g_vo_obj,0);
	close_display(&g_vo_obj);

	AR_MPI_ISP_Exit(ViPipe);
    if (p_obj->pfnUnRegisterCallback != AR_NULL)
	{
		  s32Ret = p_obj->pfnUnRegisterCallback(ViPipe, &stAeLib, &stAwbLib);
		  if (s32Ret != AR_SUCCESS)
		  {
			  ar_err("sensor_register_callback failed with %#x!\n", s32Ret);
		  }
	}
	AR_MPI_VI_DisableChn(ViPipe,ViChn);
	AR_MPI_VI_StopPipe(ViPipe);
	AR_MPI_VI_DestroyPipe(ViPipe);
	AR_MPI_VI_DisableDev(ViDev);
    SAMPLE_AR_MPI_VIN_CloseDev();

EXIT:
    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
}
/**
@brief:SAMPLE_VIO_Only_use_default_attr_case 测试vin 工作在软降lowdelay 模式下
ISP输出的图像能通过HDMI正确显示出来
	测试条件:sony imx307 sensor, artosyn evb板
	测试参数:
	测试结果:；log 确认每个阶段

*/

SAMPLE_VIO_TEST_CASE SAMPLE_VIO_sof_lowdelay_case =
{
	.p_name="SAMPLE_VIO_sof_lowdelay_case",
	.vio_test_case=SAMPLE_VIO_sof_lowdelay,
	.p_help_string=
	{
	   "-fps [25/30/60]",
	   "-check [1/0]",
	   NULL,
	}

};



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

