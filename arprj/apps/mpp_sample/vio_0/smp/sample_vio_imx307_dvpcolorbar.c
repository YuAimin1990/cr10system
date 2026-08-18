#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */
#include "../sample_vio.h"
#include "sample_lcd.h"


extern ISP_SNS_OBJ_S stSnsImx290Obj;
extern ISP_SNS_OBJ_S stSnsImx290Obj1;
extern ISP_SNS_OBJ_S stSnscolor_barObj;

extern STRU_DISPLAY_OBJ_T g_vo_obj;
extern volatile AR_S8 g_exit;

static int g_nframes=0;
static int g_vo_init=0;

static int sample_vio_init_common(SAMPLE_VIO_PRA *ppra)
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
    stSize=default_attr.stPubAttr.stSnsSize;		
    /*config vb*/
    ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
    stVbConf.u32MaxPoolCnt              = 2;

    u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
    stVbConf.astCommPool[0].u64BlkSize  = u32BlkSize;
    stVbConf.astCommPool[0].u32BlkCnt   = 30;


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
	
	SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre,ppra->vif_fre,ppra->isp_fre,ppra->pcs_fre);

	
	return 0;
}
static void* frame_run_this(void* param)
{
    AR_S32 ref=0, ret = 0;
    VIDEO_FRAME_INFO_S FrameInfo;
	STRU_VIO_SAMPLE_FRAME_RUN_T *run_inf=(STRU_VIO_SAMPLE_FRAME_RUN_T *)param;
	AR_S32 s32cnt = 0;	
	AR_S32 s32nframes = run_inf->nframes;
	AR_U64 last_pts=0;
	int fd = -1;
	char name[256];
	if (run_inf->vo_ch_id < 0)
	{
		memset(name, 0, sizeof(name));
		sprintf(name, "mipi_vc%d_pipe%d_chn%d.raw", run_inf->pipe_id, run_inf->pipe_id, run_inf->ch_id);
		fd = open(name, O_RDWR | O_CREAT);
	}
	while(1){
	     //SAMPLE_PRT("get frame = %d : pipe_id = %d, ch_id = %d, vo_ch_id = %d\n", ref, run_inf->pipe_id, run_inf->ch_id, run_inf->vo_ch_id);
		 int status=AR_MPI_VI_GetChnFrame(run_inf->pipe_id,run_inf->ch_id,&FrameInfo,50);


		 ISP_FRAME_INFO_S *info=(ISP_FRAME_INFO_S *)FrameInfo.stVFrame.stSupplement.pIspInfoVirAddr;

         //ar_printf("pipe_id %d iso:%d \n",run_inf->pipe_id,info->u32ISO);
	
         if(status){

			if (g_exit) 
			{		   
				break;
				
			}  

		    usleep(30000);
		 	continue;
         }
		 if(ref > 1000){
			//SAMPLE_PRT("get frame %lx \n",FrameInfo.stVFrame.u64PhyAddr[0]);
			//ref=0;
		 }
		 ref++;

         //SAMPLE_PRT("pipe_id=%d %ld deta=%ld\n",run_inf->pipe_id,FrameInfo.stVFrame.u64PTS,FrameInfo.stVFrame.u64PTS-last_pts);
         last_pts=FrameInfo.stVFrame.u64PTS;
		 
		 //push to display
		 if (run_inf->vo_ch_id >=0){
		 	send_to_fw_display(&g_vo_obj,run_inf->vo_ch_id,&FrameInfo);
		 }
		 else
		 {
             if (fd > 0 && ref > 30 && ref <= 30+1)
			{
				ret = write(fd, FrameInfo.stVFrame.u64VirAddr[0], FrameInfo.stVFrame.u32Len[0]);
				if (ret < 0)
					SAMPLE_PRT("write raw to file error, fd = %d", fd);
				SAMPLE_PRT("pipe_id=%d, frame_len = %d, u64VirAddr = 0x%x",run_inf->pipe_id, FrameInfo.stVFrame.u32Len[0], FrameInfo.stVFrame.u64VirAddr[0]);	
			}	
		 }	
		 AR_MPI_VI_ReleaseChnFrame(run_inf->pipe_id,run_inf->ch_id,&FrameInfo);
		 if (s32nframes >= 0)
		 {
		 	s32cnt = run_inf->nframes;
			s32nframes = -1;
		 }
  		 
		 if (s32cnt > 0)
	 	 { 	
			if (--s32cnt == 0)
			{ 				
				break;
			}
		 } 

		 if (g_exit) 
	 	 {	
			 break;
	 	 }	
	}
    ar_printf("frame run pipe_id=%d run_inf=%d exit",run_inf->pipe_id,run_inf->ch_id);
	if (fd > 0)
		close(fd);
    return NULL;
}


static int start_imx307_0(SAMPLE_VIO_PRA *ppra)
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

    //start sensor1
    s32pra_index = sample_find_pra_by_name("-i2c", ppra->argc, ppra->argv);
	if (s32pra_index > 0)
	{
		ppra->s32I2cDev = atoi(ppra->argv[s32pra_index+1]);
		s8I2cDev = ppra->s32I2cDev;
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
	AR_MPI_VI_SetChnAttr(ViPipe,ViChn,&default_attr.stChnAttr);
	AR_MPI_VI_EnableChn(ViPipe,ViChn);

	
	ALG_LIB_S stAeLib;
	ALG_LIB_S stAwbLib;	
    stAeLib.s32Id = ViPipe;
    stAwbLib.s32Id = ViPipe;
    strncpy(stAeLib.acLibName, "artosyn", sizeof("artosyn"));
    strncpy(stAwbLib.acLibName, "artosyn", sizeof("artosyn"));
	
	if (p_obj->pfnRegisterCallback != AR_NULL)
	{
		  s32Ret = p_obj->pfnRegisterCallback(ViPipe, &stAeLib, &stAwbLib);
	
		  if (s32Ret != AR_SUCCESS)
		  {
			  ar_err("sensor_register_callback failed with %#x!\n", s32Ret);
			  goto EXIT1;
		  }
	}
	else
	{
		  ar_err("sensor_register_callback failed with AR_NULL!\n");
		  goto EXIT1;
	}

    if (AR_NULL != p_obj->pfnSetBusInfo)
    {
        ISP_SNS_COMMBUS_U uSnsBusInfo;
		uSnsBusInfo.s8I2cDev=s8I2cDev;
        s32Ret = p_obj->pfnSetBusInfo(ViPipe, uSnsBusInfo);

        if (s32Ret != AR_SUCCESS)
        {
             ar_err("set sensor bus info failed with %#x!\n", s32Ret);
             goto EXIT1;
        }
    }
    else
    {
        ar_err("not support set sensor bus info!\n");
        goto EXIT1;
    }
#if 0
    //register vsync and aec update callback to pipe
    ISP_NOTIFY_REGISTER_S NotifyRegister={0};
	NotifyRegister.stNotify.aec_update=aec_update;
	NotifyRegister.stNotify.vsync_update=vsync_update;
    AR_MPI_ISP_RegisterNotifyCallBack(ViPipe,&NotifyRegister);
#endif

	AR_MPI_ISP_MemInit(ViPipe);
	AR_MPI_ISP_SetPubAttr(ViPipe, &default_attr.stPubAttr);
	VI_PIPE_EXT_ATTR_S stPipeAttr;
	AR_MPI_VI_GetPipeExtAttr(ViPipe, &stPipeAttr);
	stPipeAttr.bFoucs=0;
	AR_MPI_VI_SetPipeExtAttr(ViPipe, &stPipeAttr);

	AR_MPI_ISP_Init(ViPipe);
	AR_MPI_ISP_Run(ViPipe);


    if(g_vo_init==0){
    //here is have power, so we can start display
    //start display--------------------	
    //four layout
    AR_S32 mesh_w=1920/2;
    AR_S32 mesh_h=1080/2;
    //init display
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

	chan_pos.x=mesh_w;
	chan_pos.y=0;
	chan_pos.w=mesh_w;
	chan_pos.h=mesh_h;
	ar_vo_surface_init(&g_vo_obj,1,&chan_pos);
	
	chan_pos.x=0;
	chan_pos.y=mesh_h;
	chan_pos.w=mesh_w;
	chan_pos.h=mesh_h;
	ar_vo_surface_init(&g_vo_obj,2,&chan_pos);

	chan_pos.x=mesh_w;
	chan_pos.y=mesh_h;
	chan_pos.w=mesh_w;
	chan_pos.h=mesh_h;
	ar_vo_surface_init(&g_vo_obj,3,&chan_pos);
    g_vo_init=1;
	}


	//get frame buffer
	pthread_t	ch_thread = {0};

    pthread_attr_t* pstAttr = NULL;
	STRU_VIO_SAMPLE_FRAME_RUN_T pipe0_ch0;
	pipe0_ch0.pipe_id=0;
	pipe0_ch0.ch_id=0;
	pipe0_ch0.vo_ch_id=0;
	pipe0_ch0.nframes = ppra->s32nframes;		
	pthread_create(&ch_thread, pstAttr, frame_run_this, &pipe0_ch0);
    pthread_join(ch_thread,NULL);

	EXIT1:
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
	return 0;
}

static int start_dvp_colorbar(SAMPLE_VIO_PRA *ppra)
{
   
  	AR_S32			   s32Ret, s32pra_index = 0;
	
    ISP_SNS_OBJ_S *p_obj=&stSnscolor_barObj;	
	SRTU_SENSOR_DEFAULT_ATTR_T default_attr;
	SIZE_S			   stSize;
	VB_CONFIG_S 	   stVbConf;
	AR_U32			   u32BlkSize;

	AR_S32			   s32ViCnt 	  = 1;
	VI_DEV			   ViDev		  = 1;
	VI_PIPE 		   ViPipe		  = ViDev;
	VI_CHN			   ViChn		  = 2;
	AR_S32             mipi_index      =0;
	AR_S8              s8I2cDev        =2;	

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

	
    //start sensor1
    s32pra_index = sample_find_pra_by_name("-i2c1", ppra->argc, ppra->argv);
	if (s32pra_index > 0)
	{
		s8I2cDev = atoi(ppra->argv[s32pra_index+1]);
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
	AR_MPI_VI_SetChnAttr(ViPipe,ViChn,&default_attr.stChnAttr);
	AR_MPI_VI_EnableChn(ViPipe,ViChn);

	
	ALG_LIB_S stAeLib;
	ALG_LIB_S stAwbLib;	
    stAeLib.s32Id = ViPipe;
    stAwbLib.s32Id = ViPipe;
    strncpy(stAeLib.acLibName, "artosyn", sizeof("artosyn"));
    strncpy(stAwbLib.acLibName, "artosyn", sizeof("artosyn"));
	
	if (p_obj->pfnRegisterCallback != AR_NULL)
	{
		  s32Ret = p_obj->pfnRegisterCallback(ViPipe, &stAeLib, &stAwbLib);
	
		  if (s32Ret != AR_SUCCESS)
		  {
			  ar_err("sensor_register_callback failed with %#x!\n", s32Ret);
			  goto EXIT1;
		  }
	}
	else
	{
		  ar_err("sensor_register_callback failed with AR_NULL!\n");
		  goto EXIT1;
	}

    if (AR_NULL != p_obj->pfnSetBusInfo)
    {
        ISP_SNS_COMMBUS_U uSnsBusInfo;
		uSnsBusInfo.s8I2cDev=s8I2cDev;
        s32Ret = p_obj->pfnSetBusInfo(ViPipe, uSnsBusInfo);

        if (s32Ret != AR_SUCCESS)
        {
             ar_err("set sensor bus info failed with %#x!\n", s32Ret);
             goto EXIT1;
        }
    }
    else
    {
        ar_err("not support set sensor bus info!\n");
        goto EXIT1;
    }
#if 0
    //register vsync and aec update callback to pipe
    ISP_NOTIFY_REGISTER_S NotifyRegister={0};
	NotifyRegister.stNotify.aec_update=aec_update;
	NotifyRegister.stNotify.vsync_update=vsync_update;
    AR_MPI_ISP_RegisterNotifyCallBack(ViPipe,&NotifyRegister);
#endif

	AR_MPI_ISP_MemInit(ViPipe);
	AR_MPI_ISP_SetPubAttr(ViPipe, &default_attr.stPubAttr);
	VI_PIPE_EXT_ATTR_S stPipeAttr;
	AR_MPI_VI_GetPipeExtAttr(ViPipe, &stPipeAttr);
	stPipeAttr.bFoucs=0;
	AR_MPI_VI_SetPipeExtAttr(ViPipe, &stPipeAttr);

	AR_MPI_ISP_Init(ViPipe);
	AR_MPI_ISP_Run(ViPipe);

    if(g_vo_init==0){
    //here is have power, so we can start display
    //start display--------------------	
    //four layout
    AR_S32 mesh_w=1920/2;
    AR_S32 mesh_h=1080/2;
    //init display
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

	chan_pos.x=mesh_w;
	chan_pos.y=0;
	chan_pos.w=mesh_w;
	chan_pos.h=mesh_h;
	ar_vo_surface_init(&g_vo_obj,1,&chan_pos);
	
	chan_pos.x=0;
	chan_pos.y=mesh_h;
	chan_pos.w=mesh_w;
	chan_pos.h=mesh_h;
	ar_vo_surface_init(&g_vo_obj,2,&chan_pos);

	chan_pos.x=mesh_w;
	chan_pos.y=mesh_h;
	chan_pos.w=mesh_w;
	chan_pos.h=mesh_h;
	ar_vo_surface_init(&g_vo_obj,3,&chan_pos);
    g_vo_init=1;
	}


	//get frame buffer
	pthread_t	ch_thread = {0};

    pthread_attr_t* pstAttr = NULL;
	STRU_VIO_SAMPLE_FRAME_RUN_T pipe0_ch0;
	pipe0_ch0.pipe_id=1;
	pipe0_ch0.ch_id=2;
	pipe0_ch0.vo_ch_id=2;
	pipe0_ch0.nframes = ppra->s32nframes;		
	pthread_create(&ch_thread, pstAttr, frame_run_this, &pipe0_ch0);


    pthread_join(ch_thread,NULL);

	
	EXIT1:
	
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
	return 0;
}

static void * start_imx307_0_run(void* param)
{
     SAMPLE_VIO_PRA *ppra=(SAMPLE_VIO_PRA *)param;
	 start_imx307_0(ppra);
}
static void * start_dvp_colorbar_run(void* param)
{
	SAMPLE_VIO_PRA *ppra=(SAMPLE_VIO_PRA *)param;
	start_dvp_colorbar(ppra);
}

AR_S32 sample_vio_imx307_dvpcolorbar(SAMPLE_VIO_PRA *ppra)
{
    g_nframes=ppra->s32nframes;
    sample_vio_init_common(ppra);

	pthread_t	ch_thread = {0};
	pthread_t	ch1_thread = {0};	

    pthread_attr_t* pstAttr = NULL;

    int dvp_first=0;
    int s32pra_index = sample_find_pra_by_name("-dvp_first", ppra->argc, ppra->argv);
	if (s32pra_index > 0)
	{
		dvp_first = atoi(ppra->argv[s32pra_index+1]);
	}

	if(dvp_first==0)
	{
    	pthread_create(&ch_thread, pstAttr, start_imx307_0_run, ppra);
        sleep(1);	
    	pthread_create(&ch1_thread, pstAttr, start_dvp_colorbar_run, ppra);	
	}else
	{	   
    	pthread_create(&ch_thread, pstAttr, start_dvp_colorbar_run, ppra);
        sleep(1);	
    	pthread_create(&ch1_thread, pstAttr, start_imx307_0_run, ppra);	
	}
	
    pthread_join(ch_thread,NULL);
    pthread_join(ch1_thread,NULL);

	
	close_display_surface(&g_vo_obj,0);
	close_display_surface(&g_vo_obj,1);	
	close_display_surface(&g_vo_obj,2);
	close_display_surface(&g_vo_obj,3);
	close_display(&g_vo_obj);
	g_vo_init=0;

EXIT:
    SAMPLE_COMM_SYS_Exit();
    return 0;
}
/**
@brief sample_vio_2_imx307_4ch_rand_start_stop_case 用来测试ISP工作在多camera 模式下， 随机启动停止任意一路
    基本测试流程是：
       1 通用部分初始化，启动显示
       2 等待两个camera 结束
       3 关闭display
       4 退出
	测试条件:sony imx307 sensor, artosyn evb板,插入mipi 0 口
	测试参数:
	测试结果:ISP正确出一个colorbar和sensor的的图像，图像不见异常, ctrl-c/kill 测试例正常退出
	
*/

#if 0
SAMPLE_VIO_TEST_CASE sample_vio_imx307_dvpcolorbar_case = 
{
	.p_name="sample_vio_imx307_dvpcolorbar",
	.vio_test_case=sample_vio_imx307_dvpcolorbar,
	.p_help_string=
	{
	   "-dvp_first [0/1]",
	   NULL,
	}
};
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

