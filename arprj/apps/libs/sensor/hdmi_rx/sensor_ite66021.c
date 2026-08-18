#if !defined(__ITE_66021__)
#define __ITE_66021__

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "ar_comm_sns.h"
#include "ar_comm_video.h"
#include "ar_comm_vi.h"
#include "ar_sns_ctrl.h"
#include "mpi_isp.h"
#include "mpi_ae.h"
#include "mpi_awb.h"
#include "mpi_vi.h"
#include "hal_gpio.h"
#include "hal_i2c.h"
#include "ar_ite_hdmi_rx_app.h"
#include "osal.h"
#include "hal_sys.h"
#include "hal_vin_log.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


typedef struct{
    ar_signal_t signal;	
    ar_signal_t signal_top;
    STRU_AR_FIFO_T *queue;
	int hdmi_connected;
	int start_deteced;
	int stop_req;
}sensor_hdmi_it66021_t;


static sensor_hdmi_it66021_t g_sesnor_obj={0}; 
static pthread_mutex_t g_lock=PTHREAD_MUTEX_INITIALIZER;


static AR_S32 ITE66021_init_ctx(VI_PIPE ViPipe)
{
   return 0;
}

static void ITE66021_deinit_ctx(VI_PIPE ViPipe)
{
	return 0;
}

static AR_S32 ITE66021_power_on(VI_PIPE ViPipe, ar_dev_power_attr_t *p_dev_power_attr)
{
    
	return 0;
}

static AR_S32 ITE66021_power_off(VI_PIPE ViPipe, ar_dev_power_attr_t *p_dev_power_attr)
{
	return 0;

}

AR_S32 ITE66021_set_image_mode(VI_PIPE ViPipe, ISP_CMOS_SENSOR_IMAGE_MODE_S *pstSensorImageMode)
{
    ISP_TRACE(AR_DBG_INFO, "pipe[%d] w[%u] h[%u] fps[%f] sns_mode[%u]\n",
        ViPipe, pstSensorImageMode->u16Width, pstSensorImageMode->u16Height,
        pstSensorImageMode->f32Fps, (AR_U32)pstSensorImageMode->u8SnsMode);

    return 0;
}

void ITE66021_init(VI_PIPE ViPipe)
{

}

void ITE66021_exit(VI_PIPE ViPipe)
{

}

static ISP_CMOS_DNG_COLORPARAM_S g_stITE66021DngColorParam = {
    .stWbGain1 = {256, 256, 256},
    .stWbGain2 = {256, 256, 256},
};

static AR_S32 ITE66021_get_isp_default(VI_PIPE ViPipe, ISP_CMOS_DEFAULT_S *pstDef)
{
    if (ViPipe < 0 || ViPipe >= ISP_MAX_PIPE_NUM) {
        ISP_TRACE(AR_DBG_ERR, "the pipe id [%d] is invalid\n", ViPipe);
        return AR_ERR_VI_INVALID_PIPEID;
    }
    memset(pstDef, 0, sizeof(ISP_CMOS_DEFAULT_S));
    pstDef->stSensorMode.u32SensorID = 0;
    pstDef->stSensorMode.u8SensorMode = 0;
    memcpy(&pstDef->stDngColorParam, &g_stITE66021DngColorParam,
        sizeof(ISP_CMOS_DNG_COLORPARAM_S));
    pstDef->bDvpDownSampleEn = AR_FALSE;

    return AR_SUCCESS;
}



static int proxy_notify_proc_connect(int camera_id,notify_t notify_code,void * pra,int size,void* data)
{
      STRU_AR_HDMI_PLUG_STATUS_T hdmi_status = {0};
    	//ar_hdmi_format_change_t hdmi_change;
      int status=0;
      STRU_NOTIFY_EVENT_DATA_SRC_PRA_T  *notify=(STRU_NOTIFY_EVENT_DATA_SRC_PRA_T *)pra;

	  ar_always("NOTIFY_HDMI_CONNECT %d %d %d %d %d",g_sesnor_obj.hdmi_connected,notify->e_data_src,notify->video_format.format,\
		notify->video_format.width,notify->video_format.height);
	  if(!g_sesnor_obj.hdmi_connected)
	  {
		g_sesnor_obj.hdmi_connected=1;
		hdmi_status.hdmi_plug_status=1;
		hdmi_status.src_type=notify->e_data_src;
	  }
	  hdmi_status.format.video_format.format=notify->video_format.format;
	  hdmi_status.format.video_format.width=notify->video_format.width;
	  hdmi_status.format.video_format.height=notify->video_format.height;
	  hdmi_status.format.video_format.fps_num=notify->video_format.fps_num;
	  hdmi_status.format.video_format.fps_den=notify->video_format.fps_den;
	  hdmi_status.format.video_format.luma_stride=notify->video_format.luma_stride;
	  hdmi_status.format.video_format.chroma_stride=notify->video_format.chroma_stride;
	  hdmi_status.format.video_format.is_interlace=notify->video_format.is_interlace;
	  ar_always("color_mode=%d color_imetry=%d, extended_color_imetry=%d, rgb_quantization_range=%d, ycc_quantization_range=%d",
			  notify->video_format.color_mode,
			  notify->video_format.color_imetry,
			  notify->video_format.extended_color_imetry,
			  notify->video_format.rgb_quantization_range,
			  notify->video_format.ycc_quantization_range);
	   hdmi_status.format.video_format.origin_color_fmt = notify->video_format.color_mode;

	   if(notify->video_format.rgb_quantization_range==2)
	   {
		   switch(notify->video_format.color_imetry) {
				case 0:
					hdmi_status.format.video_format.matrix = AR_VIDEO_MATRIX_NODATA_FULL;
					break;
				case 1:
					hdmi_status.format.video_format.matrix = AR_VIDEO_MATRIX_BT601_FULL;
					break;
				case 2:
					hdmi_status.format.video_format.matrix = AR_VIDEO_MATRIX_BT709_FULL;
					break;
				case 3:
					hdmi_status.format.video_format.matrix = AR_VIDEO_MATRIX_BT709_FULL;
					break;
				default:
					hdmi_status.format.video_format.matrix = AR_VIDEO_MATRIX_BT709_FULL;
					break;
		   }
	   }else
	   {
			   switch(notify->video_format.color_imetry) {
					case 0:
						hdmi_status.format.video_format.matrix = AR_VIDEO_MATRIX_NODATA_LIMIT;
						break;
					case 1:
						hdmi_status.format.video_format.matrix = AR_VIDEO_MATRIX_BT601_LIMIT;
						break;
					case 2:
						hdmi_status.format.video_format.matrix = AR_VIDEO_MATRIX_BT709_LIMIT;
						break;
					case 3:
						hdmi_status.format.video_format.matrix = AR_VIDEO_MATRIX_BT709_LIMIT;
						break;
					default:
						hdmi_status.format.video_format.matrix = AR_VIDEO_MATRIX_BT709_LIMIT;
						break;
			   }
	  }
	  ar_always("hdmi_input_format=%d hdmi_input_color_matric=%d",
			  hdmi_status.format.video_format.origin_color_fmt,hdmi_status.format.video_format.matrix);

	  ar_always("********notify dvp in, report 422 8bit********");
	  hdmi_status.format.video_format.format=PIXEL_FORMAT_YVU_PLANAR_422;

	  memcpy(&hdmi_status.format.audio_format, &(notify->audio_format), sizeof(STRU_AUDIO_FORAMT_NOTITY_PRA_T));

	  
	  ISP_SNS_DETECT_INFO_S *info=malloc(sizeof(ISP_SNS_DETECT_INFO_S));
	  if(info){
	  	  info->bIsChanged=0;
		  info->bIsConnected=1;
	      info->bIsInterlace=hdmi_status.format.video_format.is_interlace;
		  info->enFormat=PIXEL_FORMAT_YVU_PLANAR_422;
		  info->f32Fps=(float)hdmi_status.format.video_format.fps_num/(float)hdmi_status.format.video_format.fps_den;
		  info->u16Width=hdmi_status.format.video_format.width;
		  info->u16Height=hdmi_status.format.video_format.height;
		  info->stExtInfo.hdmi_info.plug_info=hdmi_status;
	      if(g_sesnor_obj.queue->queue_insert(g_sesnor_obj.queue,info))
	      {
	          free(info);
	      }
		  ar_signal(g_sesnor_obj.signal);
	  }
	  return 0;
}
 static int proxy_notify_process_change(int camera_id,notify_t notify_code,void * pra,int size,void* data)
 {
     ar_always("NOTIFY_HDMI_FORAMAT_CHANGE");
     //ar_hdmi_plug_status_t hdmi_status;
     STRU_AR_HDMI_FORMAT_CHANGE_T hdmi_change;
     int status=0;
     STRU_NOTIFY_EVENT_DATA_SRC_PRA_T *notify=(STRU_NOTIFY_EVENT_DATA_SRC_PRA_T *)pra;

	 ar_always("NOTIFY_HDMI_CONNECT %d %d %d %d %d",g_sesnor_obj.hdmi_connected,notify->e_data_src,notify->video_format.format,\
	   notify->video_format.width,notify->video_format.height);
	 
     hdmi_change.format.video_format.format=notify->video_format.format;
     hdmi_change.format.video_format.width=notify->video_format.width;
     hdmi_change.format.video_format.height=notify->video_format.height;
     hdmi_change.format.video_format.fps_num=notify->video_format.fps_num;
     hdmi_change.format.video_format.fps_den=notify->video_format.fps_den;
     hdmi_change.format.video_format.luma_stride=notify->video_format.luma_stride;
     hdmi_change.format.video_format.chroma_stride=notify->video_format.chroma_stride;
     hdmi_change.format.video_format.is_interlace=notify->video_format.is_interlace;
     hdmi_change.format.video_format.origin_color_fmt = notify->video_format.color_mode;
     if(notify->video_format.rgb_quantization_range==2)
     {
           switch(notify->video_format.color_imetry) {
                case 0:
                    hdmi_change.format.video_format.matrix = AR_VIDEO_MATRIX_NODATA_FULL;
                    break;
                case 1:
                    hdmi_change.format.video_format.matrix = AR_VIDEO_MATRIX_BT601_FULL;
                    break;
                case 2:
                    hdmi_change.format.video_format.matrix = AR_VIDEO_MATRIX_BT709_FULL;
                    break;
                case 3:
                    hdmi_change.format.video_format.matrix = AR_VIDEO_MATRIX_BT709_FULL;
                    break;
                default:
                    hdmi_change.format.video_format.matrix = AR_VIDEO_MATRIX_BT709_FULL;
                    break;
           }
       }else
       {
               switch(notify->video_format.color_imetry) {
                    case 0:
                        hdmi_change.format.video_format.matrix = AR_VIDEO_MATRIX_NODATA_LIMIT;
                        break;
                    case 1:
                        hdmi_change.format.video_format.matrix = AR_VIDEO_MATRIX_BT601_LIMIT;
                        break;
                    case 2:
                        hdmi_change.format.video_format.matrix = AR_VIDEO_MATRIX_BT709_LIMIT;
                        break;
                    case 3:
                        hdmi_change.format.video_format.matrix = AR_VIDEO_MATRIX_BT709_LIMIT;
                        break;
                    default:
                        hdmi_change.format.video_format.matrix = AR_VIDEO_MATRIX_BT709_LIMIT;
                        break;
               }
      }
     ar_always("hdmi_input_format=%d hdmi_input_color_matric=%d",\
               hdmi_change.format.video_format.origin_color_fmt,hdmi_change.format.video_format.matrix);
     hdmi_change.format.audio_format.format = notify->audio_format.format;
     hdmi_change.format.audio_format.rate = notify->audio_format.rate;
     hdmi_change.format.audio_format.channels = notify->audio_format.channels;
     if (notify->e_data_src == DATA_HDMI_SRC_AUDIO_VIDEO) {
        hdmi_change.video_change = true;
        hdmi_change.audio_change = true;
     } else if (notify->e_data_src == DATA_HDMI_SRC_AUDIO) {
        hdmi_change.video_change = false;
        hdmi_change.audio_change = true;
     } else {
        hdmi_change.video_change = true;
        hdmi_change.audio_change = false;
     }	 
	 hdmi_change.src_type=notify->e_data_src;

	 ISP_SNS_DETECT_INFO_S *info=malloc(sizeof(ISP_SNS_DETECT_INFO_S));
	 if(info){
		 info->bIsChanged=1;
		 info->bIsConnected=1;
		 info->bIsInterlace=hdmi_change.format.video_format.is_interlace;
		 info->enFormat=PIXEL_FORMAT_YVU_PLANAR_422;
		 info->f32Fps=(float)hdmi_change.format.video_format.fps_num/(float)hdmi_change.format.video_format.fps_den;
		 info->u16Width=hdmi_change.format.video_format.width;
		 info->u16Height=hdmi_change.format.video_format.height;
		 info->stExtInfo.hdmi_info.change_info=hdmi_change;
		 if(g_sesnor_obj.queue->queue_insert(g_sesnor_obj.queue,info))
		 {
			 free(info);
		 }		 
	     ar_signal(g_sesnor_obj.signal);
	 }

	 
     return 0;
}

static int notify_hdmi_call_back(int camera_id,notify_t notify_code,void * pra,int size,void* data)
{
   int status=0;
   ar_func_enter();   
   pthread_mutex_lock(&g_lock);

   if(!g_sesnor_obj.start_deteced) 	   
   {   
	  pthread_mutex_unlock(&g_lock);
      return 0;
   }
   
   switch(notify_code)
   {

     case NOTIFY_EVENT_CONNECT:
     proxy_notify_proc_connect(camera_id,notify_code,pra,size,data);
     break;
     case NOTIFY_EVENT_DISCONNECT:
     {
         ar_always("NOTIFY_HDMI_DISCONNECT %d",g_sesnor_obj.hdmi_connected);
         if(g_sesnor_obj.hdmi_connected){
           g_sesnor_obj.hdmi_connected=0;
         }

    	 ISP_SNS_DETECT_INFO_S *info=malloc(sizeof(ISP_SNS_DETECT_INFO_S));
    	 if(info){
    		 info->bIsChanged=0;
    		 info->bIsConnected=0;
    		 if(g_sesnor_obj.queue->queue_insert(g_sesnor_obj.queue,info))
    		 {
    			 free(info);
    		 }			 
	         ar_signal(g_sesnor_obj.signal);
    	 }		 
     }
     break;
     case NOTIFY_EVENT_FORAMAT_CHANGE:
     {
         if(!g_sesnor_obj.hdmi_connected)
         {
             proxy_notify_proc_connect(camera_id,notify_code,pra,size,data);
         }else{
             proxy_notify_process_change(camera_id,notify_code,pra,size,data);
         }
     }
     break;
	 default:
		 ar_err("unsupported inform 0x%x from cam-%d", notify_code, camera_id);
	 break;
   }
   ar_func_exit();   
   pthread_mutex_unlock(&g_lock);
   return status;
}


static AR_S32 ITE66021_start_detect(VI_PIPE ViPipe)
{
	pthread_mutex_lock(&g_lock);

    if(!g_sesnor_obj.start_deteced)		
	{   
	    g_sesnor_obj.start_deteced=1;
		g_sesnor_obj.stop_req=0;
        g_sesnor_obj.queue=ar_hal_sys_creat_queue(16, "ite66021");
    	g_sesnor_obj.signal=ar_create_signal();
    	ar_ite_hdmi_rx_init(NULL);
    	register_hdmi_notify_dvp_in(NULL, notify_hdmi_call_back);
    }	
	pthread_mutex_unlock(&g_lock);
    return 0;
}

static AR_S32 ITE66021_stop_detect(VI_PIPE ViPipe)
{
	pthread_mutex_lock(&g_lock);
	g_sesnor_obj.start_deteced=0;
	g_sesnor_obj.stop_req=1;
	ar_signal(g_sesnor_obj.signal);	
	pthread_mutex_unlock(&g_lock);

	
	ar_signal_wait(g_sesnor_obj.signal_top);				

	
	pthread_mutex_lock(&g_lock);
    if(g_sesnor_obj.queue){
	   ISP_SNS_DETECT_INFO_S *info=NULL;
       do
       {
           g_sesnor_obj.queue->queue_pop(g_sesnor_obj.queue,&info);
		   if(info)
    	   {
    	      free(info);
    	   }else
    	   {
    	      break;
    	   }
       }while(1);
	
       ar_hal_sys_delete_queue(&g_sesnor_obj.queue);
    }
	if(g_sesnor_obj.signal){
	   ar_delete_signal(g_sesnor_obj.signal);
	}
	pthread_mutex_unlock(&g_lock);
    return 0;
}

static AR_S32 ITE66021_get_detect_info(VI_PIPE ViPipe, ISP_SNS_DETECT_INFO_S *pstInfo)
{   
    int status=0;
    ISP_SNS_DETECT_INFO_S *info=NULL;
	pthread_mutex_lock(&g_lock);
	while(g_sesnor_obj.start_deteced)
	{
	     g_sesnor_obj.queue->queue_pop(g_sesnor_obj.queue,&info);
		 if(info)
		 {
		     *pstInfo=*info;
			  free(info);
			  break;
		 }else
		 {		   		     
			 pthread_mutex_unlock(&g_lock);
		     ar_signal_wait(g_sesnor_obj.signal);				 
		     pthread_mutex_lock(&g_lock);
		 }		 
	}
	if(g_sesnor_obj.stop_req==1)
	{
	    g_sesnor_obj.stop_req=0;
		ar_signal(g_sesnor_obj.signal_top);
		status=-1;
	}	
	pthread_mutex_unlock(&g_lock);

    if(!g_sesnor_obj.start_deteced)
    {
       usleep(1000);
	   status=-1;
    }

    return status;
}

static AR_S32 ITE66021_sensor_ctl(VI_PIPE ViPipe, ISP_CMOS_SENSOR_CTL *pSensorCtl)
{
    AR_S32 ret = 0;

    if (ViPipe < 0 || ViPipe >= ISP_MAX_PIPE_NUM) {
        ISP_TRACE(AR_DBG_ERR, "the pipe id [%d] is invalid\n", ViPipe);
        return AR_ERR_VI_INVALID_PIPEID;
    }

    if (pSensorCtl == NULL) {
         ISP_TRACE(AR_DBG_ERR, "null pointer of pSensorCtl\n");
         return AR_ERR_VI_INVALID_NULL_PTR;
    }

    switch (pSensorCtl->u8CtlCode) {
    case SENSOR_CTL_START_DETECT:
        ret = ITE66021_start_detect(ViPipe);
        break;

    case SENSOR_CTL_STOP_DETECT:
        ret = ITE66021_stop_detect(ViPipe);
        break;

    case SENSOR_CTL_GET_DETECT_INFO:
        ret = ITE66021_get_detect_info(ViPipe, (ISP_SNS_DETECT_INFO_S *)(pSensorCtl->pCtlData));
        break;

    default:
        ISP_TRACE(AR_DBG_ERR, "error ctl code\n");
        ret = AR_FAILURE;
        break;
    }

    return ret;
}

static AR_S32 ITE66021_init_sensor_exp_function( ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
{
    CMOS_CHECK_POINTER(pstSensorExpFunc);
    memset(pstSensorExpFunc, 0, sizeof(ISP_SENSOR_EXP_FUNC_S));

    pstSensorExpFunc->pfn_cmos_sensor_init = ITE66021_init;
    pstSensorExpFunc->pfn_cmos_sensor_exit = ITE66021_exit;
    pstSensorExpFunc->pfn_cmos_sensor_global_init = AR_NULL;
    pstSensorExpFunc->pfn_cmos_set_image_mode = ITE66021_set_image_mode;
    pstSensorExpFunc->pfn_cmos_set_wdr_mode = AR_NULL;
    pstSensorExpFunc->pfn_cmos_get_isp_default = ITE66021_get_isp_default;
    pstSensorExpFunc->pfn_cmos_set_pixel_detect = AR_NULL;
    pstSensorExpFunc->pfn_cmos_get_sns_reg_info = AR_NULL;
    pstSensorExpFunc->pfn_cmos_sns_power_on = ITE66021_power_on;
    pstSensorExpFunc->pfn_cmos_sns_power_off = ITE66021_power_off;
    pstSensorExpFunc->pfn_cmos_sns_ctl = ITE66021_sensor_ctl;

    return AR_SUCCESS;
}

static AR_S32 ITE66021_init_ae_exp_function(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
    CMOS_CHECK_POINTER(pstExpFuncs);
    memset(pstExpFuncs, 0, sizeof(AE_SENSOR_EXP_FUNC_S));
    return AR_SUCCESS;
}

static AR_S32 ITE66021_init_awb_exp_function(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
    CMOS_CHECK_POINTER(pstExpFuncs);
    memset(pstExpFuncs, 0, sizeof(AWB_SENSOR_EXP_FUNC_S));
    return AR_SUCCESS;
}


static AR_S32 ITE66021_set_bus_info(VI_PIPE ViPipe, ISP_SNS_COMMBUS_U unSNSBusInfo)
{
    return AR_SUCCESS;
}

static AR_S32 ITE66021_register_callback(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ALG_LIB_S *pstAwbLib)
{
    AR_S32 s32Ret;
    ISP_SENSOR_REGISTER_S stIspRegister;
    AE_SENSOR_REGISTER_S  stAeRegister;
    AWB_SENSOR_REGISTER_S stAwbRegister;
    ISP_SNS_ATTR_INFO_S   stSnsAttrInfo;

    CMOS_CHECK_POINTER(pstAeLib);
    CMOS_CHECK_POINTER(pstAwbLib);

    s32Ret = ITE66021_init_ctx(ViPipe);
    if (s32Ret != AR_SUCCESS) {
        ISP_TRACE(AR_DBG_ERR, "init ctx failed, pipe[%d] ret[%d]\n", ViPipe, s32Ret);
        return s32Ret;
    }
    s32Ret  = ITE66021_init_sensor_exp_function(&stIspRegister.stSnsExp);
    s32Ret |= AR_MPI_ISP_SensorRegCallBack(ViPipe, &stSnsAttrInfo, &stIspRegister);
    if (AR_SUCCESS != s32Ret) {
        ISP_TRACE(AR_DBG_ERR, "sensor register callback function failed!\n");
        return s32Ret;
    }

    s32Ret  = ITE66021_init_ae_exp_function(&stAeRegister.stSnsExp);
    s32Ret |= AR_MPI_AE_SensorRegCallBack(ViPipe, pstAeLib, &stSnsAttrInfo, &stAeRegister);
    if (AR_SUCCESS != s32Ret) {
        ISP_TRACE(AR_DBG_ERR, "sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }

    s32Ret  = ITE66021_init_awb_exp_function(&stAwbRegister.stSnsExp);
    s32Ret |= AR_MPI_AWB_SensorRegCallBack(ViPipe, pstAwbLib, &stSnsAttrInfo, &stAwbRegister);
    if (AR_SUCCESS != s32Ret) {
        ISP_TRACE(AR_DBG_ERR, "sensor register callback function to awb lib failed!\n");
        return s32Ret;
    }

    return AR_SUCCESS;
}

static AR_S32 ITE66021_unregister_callback(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ALG_LIB_S *pstAwbLib)
{
    AR_S32 s32Ret;

    CMOS_CHECK_POINTER(pstAeLib);
    CMOS_CHECK_POINTER(pstAwbLib);

    s32Ret = AR_MPI_ISP_SensorUnRegCallBack(ViPipe, 0);
    if (AR_SUCCESS != s32Ret) {
        ISP_TRACE(AR_DBG_ERR, "sensor unregister callback function failed!\n");
        return s32Ret;
    }

    s32Ret = AR_MPI_AE_SensorUnRegCallBack(ViPipe, pstAeLib, 0);
    if (AR_SUCCESS != s32Ret) {
        ISP_TRACE(AR_DBG_ERR, "sensor unregister callback function to ae lib failed!\n");
        return s32Ret;
    }

    s32Ret = AR_MPI_AWB_SensorUnRegCallBack(ViPipe, pstAwbLib, 0);
    if (AR_SUCCESS != s32Ret) {
        ISP_TRACE(AR_DBG_ERR, "sensor unregister callback function to awb lib failed!\n");
        return s32Ret;
    }

    ITE66021_deinit_ctx(ViPipe);
    return AR_SUCCESS;
}
// ITE66021 default vi attributes are based on "bt656 2-vc 1080p 30fps"
static const STRU_COMBO_DEV_ATTR_T ITE66021_def_combo_dev_attr = {
    .devno = 0,
    .input_mode = INPUT_MODE_BT656,
    .img_rect = {0, 0, 1920, 1080},
};

static const VI_DEV_ATTR_S ITE66021_def_dev_attr = {
    .enIntfMode = VI_MODE_BT601,
    .enWorkMode = VI_WORK_MODE_1Multiplex,
    .au32ComponentMask = {0xFF000000, 0x0},
    .enScanMode = VI_SCAN_PROGRESSIVE,
    .as32AdChnId = {-1, -1, -1, -1},
    .enDataSeq = VI_DATA_SEQ_YUYV,
    .enInputDataType = VI_DATA_TYPE_YUV,
    .bDataReverse = AR_FALSE,
    .stSize = {1920, 1080},
    .stBasAttr = {
        .stSacleAttr = {
            .stBasSize = {1920 , 1080},
        },
        .stRephaseAttr = {
            .enHRephaseMode = VI_REPHASE_MODE_NONE,
            .enVRephaseMode = VI_REPHASE_MODE_NONE,
        },
    },
    .stWDRAttr = {
        .enWDRMode = WDR_MODE_NONE,
        .u32CacheLine = 1080,
    },
    .enDataRate = DATA_RATE_X2,
    .bDvpHighLowByteSwap = AR_FALSE,
};

static const VI_PIPE_ATTR_S ITE66021_def_pipe_attr = {
    .enPipeBypassMode = VI_PIPE_BYPASS_NONE,
    .bYuvSkip = AR_FALSE,
    .bIspBypass = AR_FALSE,
    .u32MaxW = 1920,
    .u32MaxH = 1080,
    .enPixFmt = PIXEL_FORMAT_YVU_PLANAR_422,
    .enCompressMode = COMPRESS_MODE_NONE,
    .enBitWidth = DATA_BITWIDTH_8,
    .bNrEn = AR_FALSE,
    .stNrAttr = {
        .enPixFmt = PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        .enBitWidth = DATA_BITWIDTH_8,
        .enNrRefSource = VI_NR_REF_FROM_RFR,
        .enCompressMode = COMPRESS_MODE_NONE,
    },
    .bSharpenEn = AR_FALSE,
    .stFrameRate = {
        .s32SrcFrameRate = -1,
        .s32DstFrameRate = -1,
    },
    .bDiscardProPic = AR_FALSE,
};

static const ISP_PUB_ATTR_S ITE66021_def_isp_pub_attr = {
    .stWndRect = {0, 0, 1920, 1080},
    .stSnsSize = {1920, 1080},
    .f32FrameRate = 30,
    .enBayer = AR_BAYER_RGGB,
    .enWDRMode = WDR_MODE_NONE,
    .u8SnsMode = 0,
};

static const VI_CHN_ATTR_S ITE66021_def_chn_attr = {
    .stSize = {1920, 1080},
    .enPixelFormat = PIXEL_FORMAT_YVU_PLANAR_420,
    .enDynamicRange = DYNAMIC_RANGE_SDR8,
    .enVideoFormat = VIDEO_FORMAT_LINEAR,
    .enCompressMode = COMPRESS_MODE_NONE,
    .bMirror = 0,
    .bFlip = 0,
    .u32Depth = 0,
    .stFrameRate = {
        .s32SrcFrameRate = -1,
        .s32DstFrameRate = -1,
    },
};

static AR_S32 ITE66021_get_default_attr(AR_U8 u8SnsMode, SRTU_SENSOR_DEFAULT_ATTR_T *pstDefaultAttr)
{
    CMOS_CHECK_POINTER(pstDefaultAttr);

    pstDefaultAttr->stComboAttr = ITE66021_def_combo_dev_attr;
    pstDefaultAttr->stDevAttr = ITE66021_def_dev_attr;
    pstDefaultAttr->stPipeAttr = ITE66021_def_pipe_attr;
    pstDefaultAttr->stPubAttr = ITE66021_def_isp_pub_attr;
    pstDefaultAttr->stChnAttr = ITE66021_def_chn_attr;

    return AR_SUCCESS;
}


ISP_SNS_OBJ_S stSnsITE66021Obj = {
    .pfnRegisterCallback    = ITE66021_register_callback,
    .pfnUnRegisterCallback  = ITE66021_unregister_callback,
    .pfnStandby             = AR_NULL,
    .pfnRestart             = AR_NULL,
    .pfnMirrorFlip          = AR_NULL,
    .pfnWriteReg            = AR_NULL,
    .pfnReadReg             = AR_NULL,
    .pfnSetBusInfo          = ITE66021_set_bus_info,
    .pfnSetInit             = AR_NULL,
    .pfnGetDefaultAttr      = ITE66021_get_default_attr,
};


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __ITE66021_CMOS_H_ */


