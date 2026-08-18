//this is the base class
#include <iostream>
#include <fstream>
#include <string>
#include <sys/time.h>
#include <stdarg.h>
#include "mpi_vgs.h"
#include "hal_sys.h"
#include "sample_comm.h"
#include <arm_neon.h>

#ifdef CONFIG_BIT_32BITS
#include <errno.h>
#endif
#include "cJSON.h"

#include "mpi_scaler_api.h"
#include "ar_comm_video.h"
#include "hal_vo.h"

#include "arcnn_base.h"
#include "arcnn_common.h"


AR_U32	  u32GSramAddrPhy =0;
AR_U32	  u32GSramSize =0;


AR_VOID SAMPLE_AR_MPI_VIN_OpenDev(AR_S32 mode)
{
   VI_DEV_PROP_S Prop;
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

AR_VOID SAMPLE_AR_MPI_VIN_CloseDev()
{
	AR_MPI_VIN_CloseDev();
	return;
}


CARCNN::CARCNN(AR_CHAR * pchCfgFile)
{
    AR_S32 ret = 0;
    AR_CHAR * pchBuff = NULL;
    AR_U32 u32Size;

    u32Initialized = 0;
    memset(&stCNNDesc, 0, sizeof(stCNNDesc));
    handle = NULL;
    u32IfcPreProcess = 0;
	u32CbEnable = 1;
    u32Mode = 0;
    strLocalDir.clear();

    s32LiveFd = 0;
    pchLocalImage = NULL;
    u32ProfileEnable = 0;
    cbFunc = NULL;
	
	u32TotalPreTime = 0;
	u32TotalFwTime = 0;
	u32TotalCBTime = 0;

	u32FirstPreTime = 0;
	u32FirstFwTime = 0;
	u32FirstCBTime = 0;

	u32FisrtRun = 0;

    memset(astProfile, 0, sizeof(astProfile));

    if(!pchCfgFile)
    {
        Log(ARCNN_LOG_ERR, "Invalid json file name.\n");
        return;
    }

    ifstream cfgFile(pchCfgFile, ios::in|ios::binary|ios::ate);
    if(!cfgFile.is_open())
    {
        Log(ARCNN_LOG_ERR, "Open json file failed.\n");
        return;
    }

    u32Size = cfgFile.tellg();
    cfgFile.seekg(0, ios::beg);
    pchBuff = new char[u32Size];
    if(!pchBuff)
    {
        Log(ARCNN_LOG_ERR, "Get buffer error in constructor.\n");
        return;
    }

    cfgFile.read(pchBuff, u32Size);
    cfgFile.close();

    cJSON * pJson = NULL;
    cJSON * pNet = NULL;
    pJson = cJSON_Parse(pchBuff);
    if(!pJson)
    {
        Log(ARCNN_LOG_ERR, "Parse cfg json file[%s] failed.\n", pchCfgFile);
        delete[] pchBuff;
        return;
    }

    pNet = cJSON_GetObjectItemCaseSensitive(pJson, "network");
    if(!pNet)
    {
        Log(ARCNN_LOG_ERR, "Parse network node failed.\n");
        delete[] pchBuff;
        cJSON_Delete(pJson);
        return;
    }

    cJSON * id = cJSON_GetObjectItemCaseSensitive(pNet, "id");
    if(!id)
    {
        Log(ARCNN_LOG_ERR, "Parse network id failed.\n");
        stCNNDesc.u16NetworkID = 0xFFFF;
    }
    else
    {
        stCNNDesc.u16NetworkID = atoi(id->valuestring);
    }

    cJSON * priority = cJSON_GetObjectItemCaseSensitive(pNet, "priority");
    if(priority && strcmp(priority->valuestring, "high") == 0)
    {
        stCNNDesc.u32Priority = NETWORK_PRIORITY_HIGH;
    }
    else
    {
        stCNNDesc.u32Priority = NETWORK_PRIORITY_NORMAL;
    }


	cJSON * preproc = cJSON_GetObjectItemCaseSensitive(pNet, "ifc-pre-process");
	if(preproc && strcmp(preproc->valuestring, "yes") == 0)
	{
		 u32IfcPreProcess = 1;
	}
	else
	{
		 u32IfcPreProcess = 0;
	}

	cJSON * cben = cJSON_GetObjectItemCaseSensitive(pNet, "callback_enable");
	if(cben && strcmp(cben->valuestring, "no") == 0)
	{
		 u32CbEnable = 0;
	}
	else
	{
		 u32CbEnable = 1;
	}

	cJSON * npufile = cJSON_GetObjectItemCaseSensitive(pNet, "npubin");
	if(!npufile)
	{
		Log(ARCNN_LOG_ERR, "Parse npubin node failed.\n");
		delete[] pchBuff;
		cJSON_Delete(pJson);
		return;
	}
	else
	{
		strcpy(stCNNDesc.au8NpubinFileName, npufile->valuestring);
		Log(ARCNN_LOG_INFO, "au8NpubinFileName=%s\n",stCNNDesc.au8NpubinFileName);
	}

    cJSON * cbfile = cJSON_GetObjectItemCaseSensitive(pNet, "callback_to_arm");
    if(!cbfile || strcmp(cbfile->valuestring, "yes") == 0)
    {
        stCNNDesc.u32CBToArm = 1;
    }
    else
    {
        stCNNDesc.u32CBToArm = 0;
    }

    cJSON * mode = cJSON_GetObjectItemCaseSensitive(pNet, "mode");
    if(!mode)
    {
        Log(ARCNN_LOG_ERR, "Parse mode node failed.\n");
        delete[] pchBuff;
        cJSON_Delete(pJson);
        return;
    }
    if(strcmp(mode->valuestring, "local") == 0)
    {
        printf("local \n");
		u32Mode = ARCNN_INPUT_LOCAL;
        cJSON * path = cJSON_GetObjectItemCaseSensitive(pNet, "local_path");
        if(!path)
        {
            Log(ARCNN_LOG_ERR, "Parse local path failed.\n");
            delete[] pchBuff;
            cJSON_Delete(pJson);
            return;
        }

        strLocalDir = path->valuestring;
		
    }
    else
    {
		printf("live \n");
		u32Mode = ARCNN_INPUT_LIVE;
        cJSON * devname = cJSON_GetObjectItemCaseSensitive(pNet, "devname");
        if(!devname)
        {
            Log(ARCNN_LOG_ERR, "Parse devname failed.\n");
            delete[] pchBuff;
            cJSON_Delete(pJson);
            return;
        }
		else
        {	
        	strDevName = devname->valuestring;
		}
		strDevName = devname->valuestring;
    }

    cJSON * sramaddr = cJSON_GetObjectItemCaseSensitive(pNet, "sram_addr");
    if(!sramaddr)
    {
        stCNNDesc.u32SramAddrPhy = 0;
		ret = AR_MPI_NPU_SetSecurity(0);
		if(ret<0)
		{
			Log(ARCNN_LOG_ERR, "ar_hal_npu_set_security failed.\n");
		}
    }
    else
    {
        u32GSramAddrPhy = strtoul(sramaddr->valuestring, NULL, 16);
        stCNNDesc.u32SramAddrPhy = strtoul(sramaddr->valuestring, NULL, 16);
        cJSON * sramsize = cJSON_GetObjectItemCaseSensitive(pNet, "sram_size");
        if(!sramsize)
        {
            stCNNDesc.u32SramSize = 0;
			ret = AR_MPI_NPU_SetSecurity(0);
			if(ret<0)
			{
				Log(ARCNN_LOG_ERR, "ar_hal_npu_set_security failed.\n");
			}
        }
        else
        {
            u32GSramSize = strtoul(sramsize->valuestring, NULL, 16);
            stCNNDesc.u32SramSize = strtoul(sramsize->valuestring, NULL, 16);
			ret= AR_MPI_NPU_SetSecurity(1);
			if(ret<0)
			{
				Log(ARCNN_LOG_ERR, "ar_hal_npu_set_security failed.\n");
			}
        }
    }

	cJSON * imgwidth = cJSON_GetObjectItemCaseSensitive(pNet, "img_width");
	if(!imgwidth)
    {
	   Log(ARCNN_LOG_ERR, "Parse img_width node failed. SET DEFAULT 0\n");
	   u32ImgWidth = 0;
    }
	else
    {
        u32ImgWidth = atoi(imgwidth->valuestring);
    }

	cJSON * imgheight = cJSON_GetObjectItemCaseSensitive(pNet, "img_height");
	if(!imgheight)
	{
	   Log(ARCNN_LOG_ERR, "Parse img_height node failed.SET DEFAULT 0\n");
       u32ImgHeight =0;
	}
	else
	{
		u32ImgHeight= atoi(imgheight->valuestring);
	}

	cJSON * conf_Thres = cJSON_GetObjectItemCaseSensitive(pNet, "conf");
	if(!conf_Thres)
	{
		Log(ARCNN_LOG_ERR, "Parse conf threshold node failed.SET DEFAULT 0.25\n");
		yolov8Param.conf_thres = 0.25;
	}
	else
	{
		yolov8Param.conf_thres= atof(conf_Thres->valuestring);
	}

	cJSON * iou_Thres = cJSON_GetObjectItemCaseSensitive(pNet, "iou");
	if(!iou_Thres)
	{
		Log(ARCNN_LOG_ERR, "Parse iou threshold node failed.SET DEFAULT 0.70\n");
		yolov8Param.iou_thres = 0.70;
	}
	else
	{
		yolov8Param.iou_thres= atof(iou_Thres->valuestring);
	}

	cJSON * use_neon = cJSON_GetObjectItemCaseSensitive(pNet, "useNeon");
	if(!use_neon)
	{
	   Log(ARCNN_LOG_ERR, "Parse UseNeon node failed.SET DEFAULT 1\n");
       yolov8Param.useNeon = 1;
	}
	else
	{
		yolov8Param.useNeon = atoi(use_neon->valuestring);
	}


    delete[] pchBuff;
    cJSON_Delete(pJson);

    Log(ARCNN_LOG_INFO, "Load Model begin.\n");

    handle = AR_MPI_NPU_LoadModel(&stCNNDesc);
    if(!handle)
    {
        Log(ARCNN_LOG_ERR, "Load Model failed.\n");
        return;
    }

    Log(ARCNN_LOG_INFO, "Load net %d finished.\n", AR_MPI_NPU_GetNetworkId(handle));


	ret = AR_MPI_NPU_GetInputTensorParam(handle, 0, &inputTensor);
	if(ret)
		printf("get input tensor param fail~ \n");

	yolov8Param.net_h = inputTensor.u32Height;
	yolov8Param.net_w = inputTensor.u32Width;

	
	ret = AR_MPI_NPU_GetIFCParamByName(handle, inputTensor.achName, &ifcParam);
	if(ret)
		printf("get ifc param fail~ \n");


	getTensorId(handle, tensorsId);
	
    u32Initialized = 1;
	
	if(u32Mode == ARCNN_INPUT_LOCAL)
	{
		yolov8Param.mask_w 	= 640;
		yolov8Param.mask_h 	= 640;
		inputImgSize = ifcParam.u32YStride * ifcParam.u32YuvHeight * 3;
	}
	else
	{
		yolov8Param.mask_w 	= 640;
		yolov8Param.mask_h 	= 480;
		inputImgSize = ifcParam.u32YStride*ifcParam.u32YuvHeight + ifcParam.u32UStride*ifcParam.u32YuvHeight/2 + ifcParam.u32VStride*ifcParam.u32YuvHeight/2 ;
	}
	
	argbBuff.u64Len = yolov8Param.mask_w * yolov8Param.mask_h * 4;
	ret = AR_MPI_NPU_MallocBuff((AR_CHAR*)"argbBuff", &argbBuff);
	if(ret)
		printf("malloc argbBuff fail~ \n");
	memset((void*)argbBuff.u64VirtAddr, 0, argbBuff.u64Len);
	

	tranBuff.u64Len = yolov8Param.mask_w * yolov8Param.mask_h * 4;
	ret = AR_MPI_NPU_MallocBuff((AR_CHAR*)"tranBuff", &tranBuff);
	if(ret)
		printf("malloc tranBuff fail~ \n");
	memset((void*)tranBuff.u64VirtAddr, 0, tranBuff.u64Len);


	buff0.u64Len = inputImgSize;
	ret = AR_MPI_NPU_MallocBuff((AR_CHAR*)"buff0", &buff0);
	if(ret)
		printf("malloc buff0 fail~ \n");


	buff1.u64Len = inputImgSize;
	ret = AR_MPI_NPU_MallocBuff((AR_CHAR*)"buff1", &buff1);
	if(ret)
		printf("malloc buff1 fail~ \n");

}

CARCNN::~CARCNN()
{
    //unload model
    if(handle)
    {
        AR_MPI_NPU_UnloadModel(handle);
		AR_MPI_NPU_FreeBuff(&argbBuff);
		AR_MPI_NPU_FreeBuff(&tranBuff);
		AR_MPI_NPU_FreeBuff(&buff0);
		AR_MPI_NPU_FreeBuff(&buff1);
    }

    handle = NULL;
}

AR_U32 CARCNN::GetMode()
{
    return u32Mode;
}

AR_U32 CARCNN::IsIFCEnabled()
{
    return u32IfcPreProcess;
}

AR_U32 CARCNN::IsCbEnabled()
{
    return u32CbEnable;
}

AR_S32 CARCNN::StartViVo()
{
	//this part from vgs module
	AR_S32             s32Ret;
    AR_S32             s32ViCnt       = 1;
    VI_DEV             ViDev          = 0;
    AR_S32             s32WorkSnsId   = 0;
    SAMPLE_VI_CONFIG_S stViConfig;

    SIZE_S             stSize;
    VB_CONFIG_S        stVbConf;
    PIC_SIZE_E         enPicSize;
    AR_U32             u32BlkSize;

    SAMPLE_VO_CONFIG_S stVoConfig;

    WDR_MODE_E         enWDRMode      = WDR_MODE_NONE;
    DYNAMIC_RANGE_E    enDynamicRange = DYNAMIC_RANGE_SDR8;
    PIXEL_FORMAT_E     enPixFormat    = PIXEL_FORMAT_YVU_PLANAR_420;
    VIDEO_FORMAT_E     enVideoFormat  = VIDEO_FORMAT_LINEAR;
    COMPRESS_MODE_E    enCompressMode = COMPRESS_MODE_NONE;

    /*config vi*/
    SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

    stViConfig.s32WorkingViNum                                   = s32ViCnt;
    stViConfig.as32WorkingViId[0]                                = 0;
    stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev         = ViDev;
    stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev           = ViDev;
    stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode       = enWDRMode;
    stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
    stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0]       = ViPipe;
    stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1]       = -1;
    stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2]       = -1;
    stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3]       = -1;
    stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn           = ViChn;
    stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat     = enPixFormat;
    stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange  = enDynamicRange;
    stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat   = enVideoFormat;
    stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode  = enCompressMode;

    /*get picture size*/
    s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
    if (AR_SUCCESS != s32Ret)
    {
        printf("get picture size by sensor failed!\n");
        return s32Ret;
    }

    s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
    if (AR_SUCCESS != s32Ret)
    {
        printf("get picture size failed!\n");
        return s32Ret;
    }

    /*config vb*/
    ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
    stVbConf.u32MaxPoolCnt              = 2;

    u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 256);
    stVbConf.astCommPool[0].u64BlkSize  = u32BlkSize;
    stVbConf.astCommPool[0].u32BlkCnt   = 10;

    u32BlkSize *= 3;
    stVbConf.astCommPool[1].u64BlkSize  = u32BlkSize;
    stVbConf.astCommPool[1].u32BlkCnt   = 12;

    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (AR_SUCCESS != s32Ret)
    {
        printf("system init failed with %d!\n", s32Ret);
        return s32Ret;
    }

    SAMPLE_AR_MPI_VIN_OpenDev(0);

    /*start vi*/
    s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
    if (AR_SUCCESS != s32Ret)
    {
        printf("start vi failed.s32Ret:0x%x !\n", s32Ret);
        return -1;
    }

    SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
    stVoConfig.VoDev									= SAMPLE_VO_DEV_DHD0;
    stVoConfig.enVoIntfType 							= VO_INTF_HDMI;
    stVoConfig.enIntfSync								= VO_OUTPUT_1080P60;
    stVoConfig.enPicSize								= enPicSize;
    stVoConfig.u32DisBufLen 							= 3;
    stVoConfig.enDstDynamicRange						= enDynamicRange;
    stVoConfig.enVoMode 								= VO_MODE_1MUX;

    s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);
    if (AR_SUCCESS != s32Ret)
    {
    	printf("SAMPLE_COMM_VO_StartVO failed with %d!\n", s32Ret);
    	return -1;
    }
	printf("Sample start vi&vo successed! \n");
	
	return 0;
}


AR_U32 CARCNN::GetNetPerf(AR_U32 u32RunLoop)
{
    if(u32ProfileEnable)
    {
   		AR_U32 u32AvgPreTime = 0;
		AR_U32 u32AvgFwTime = 0;
		AR_U32 u32AvgCBTime = 0;
			
		if(u32RunLoop == 1)
		{
			 u32AvgPreTime = u32TotalPreTime/u32RunLoop;
			 u32AvgFwTime = u32TotalFwTime/u32RunLoop;
			 u32AvgCBTime = u32TotalCBTime/u32RunLoop;
		}
		else
		{
			 u32AvgPreTime = (u32TotalPreTime-u32FirstPreTime)/(u32RunLoop-1);
			 u32AvgFwTime = (u32TotalFwTime-u32FirstFwTime)/(u32RunLoop-1);
			 u32AvgCBTime = (u32TotalCBTime-u32FirstCBTime)/(u32RunLoop-1);
		}
		
		Log(ARCNN_LOG_INFO, "NPUAvgFWPre: %d x 0.01ms  NPUAvgFW: %d x 0.01ms  NPUAvgCBTime: %d x 0.01ms \r\n",u32AvgPreTime,u32AvgFwTime, u32AvgCBTime);
    }
    return 0;
}


AR_S32 CARCNN::RunThread()
{
	readImgThread = std::thread(&CARCNN::ReadImgThread, this);
	return 0;
}


void CARCNN::ReadImgThread()
{
	AR_S32 s32Ret;
	int job_id = 10;
	PIXEL_FORMAT_E enPixelFmt = PIXEL_FORMAT_ARGB_8888;

	while(1)
	{			
		if(GetLiveImage() != 0)
		{
			Log(ARCNN_LOG_ERR, "Get Live Img fail !");
		}
		
		osdMutex.lock();
		s32Ret = AR_MPI_VGS_BeginJob(&job_id);
        if (AR_SUCCESS != s32Ret)
        {
            printf("AR_MPI_VGS_BeginJob failed with %d!\n", s32Ret);
        }

		VGS_ADD_OSD_S  osd_array[1] = {0};
        osd_array[0].stRect.s32X = 0;
        osd_array[0].stRect.s32Y = 0;
        osd_array[0].stRect.u32Width  = yolov8Param.img_w;
        osd_array[0].stRect.u32Height = yolov8Param.img_h;
        osd_array[0].u32BgColor = 0xffffffff;
        osd_array[0].enPixelFmt = enPixelFmt;
        osd_array[0].u64PhyAddr = (AR_U64)tranBuff.u64PhyAddr;
        osd_array[0].u32Stride  = yolov8Param.img_w * 4;
        osd_array[0].bOsdRevert = AR_FALSE;
		
        s32Ret = AR_MPI_VGS_AddOsdTaskArray(job_id, &task_attr, osd_array, 1);
        if (AR_SUCCESS != s32Ret)
        {
            printf("AR_MPI_VGS_AddOsdTask failed with 0x%x!\n", s32Ret);
        }

		s32Ret = AR_MPI_VGS_EndJob(job_id);		
		if(s32Ret)
		{
            printf("AR_MPI_VGS_EndJob failed with 0x%x!\n", s32Ret);
        }

		s32Ret = AR_MPI_VO_SendFrame(ViPipe, ViChn, &FrameInfo, -1);
		if(s32Ret)
		{
			printf("AR_MPI_VO_SendFrame failed with 0x%x!\n", s32Ret);
		}

		AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
		if(s32Ret)
		{
			printf("AR_MPI_VO_SendFrame failed with 0x%x!\n", s32Ret);
		}

		osdMutex.unlock();
	}
}


AR_S32 CARCNN::GetLiveImage()
{
    ProfileStart("GetLiveImage");
    memset(&FrameInfo,0,sizeof(VIDEO_FRAME_INFO_S));

	AR_S32 s32Ret=AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, -1);
	if(s32Ret<0)
	{
		Log(ARCNN_LOG_ERR,"AR_MPI_VPSS_GetChnFrame ERR ret=%x\r\n", s32Ret);
		return -1;
	}

	task_attr.stImgIn = FrameInfo;
    task_attr.stImgOut = FrameInfo;

	AR_IMG_FORMAT_E FrameFmt = AR_IMG_YUV420P;

	AR_IMG_S pstSrcImg;
	pstSrcImg.u32FrameId = FrameInfo.stVFrame.u32FrameFlag;
    pstSrcImg.enFormat = FrameFmt;
    pstSrcImg.u32Width = FrameInfo.stVFrame.u32Width;
    pstSrcImg.u32Height = FrameInfo.stVFrame.u32Height;
	pstSrcImg.u32ChannelNum = 3;
    pstSrcImg.astChannels[0].u32Stride = FrameInfo.stVFrame.u32Stride[0];
	pstSrcImg.astChannels[0].u32AddrPhy = (AR_U32)FrameInfo.stVFrame.u64PhyAddr[0];
	pstSrcImg.astChannels[0].uptrAddrVirt = (AR_U32)FrameInfo.stVFrame.u64VirAddr[0];
    pstSrcImg.astChannels[1].u32Stride = FrameInfo.stVFrame.u32Stride[1];
	pstSrcImg.astChannels[1].u32AddrPhy = (AR_U32)FrameInfo.stVFrame.u64PhyAddr[1];
	pstSrcImg.astChannels[1].uptrAddrVirt = (AR_U32)FrameInfo.stVFrame.u64VirAddr[1];
	pstSrcImg.astChannels[2].u32Stride = FrameInfo.stVFrame.u32Stride[2];
	pstSrcImg.astChannels[2].u32AddrPhy = (AR_U32)FrameInfo.stVFrame.u64PhyAddr[2];
	pstSrcImg.astChannels[2].uptrAddrVirt = (AR_U32)FrameInfo.stVFrame.u64VirAddr[2];

	yolov8Param.img_h = pstSrcImg.u32Height;
	yolov8Param.img_w = pstSrcImg.u32Width;
	int max_src = GET_MAX(yolov8Param.img_w, yolov8Param.img_h);
	yolov8Param.ratio = (AR_FLOAT)max_src/(AR_FLOAT)yolov8Param.net_w;

	buffMutex.lock();
	memset((void*)buff0.u64VirtAddr, 0, buff0.u64Len);
	
	AR_IMG_S pstDstImg;
	pstDstImg.u32FrameId = FrameInfo.stVFrame.u32FrameFlag;
	pstDstImg.enFormat = AR_IMG_YUV420P;
	pstDstImg.u32Width = yolov8Param.net_w;
	pstDstImg.u32Height = yolov8Param.net_h;
	pstDstImg.u32ChannelNum = 3;
	pstDstImg.astChannels[0].u32Stride = ifcParam.u32YStride;
	pstDstImg.astChannels[0].u32AddrPhy = (AR_UINTPTR)buff0.u64PhyAddr;
	pstDstImg.astChannels[1].u32Stride = ifcParam.u32UStride;
	pstDstImg.astChannels[1].u32AddrPhy = (AR_UINTPTR)(buff0.u64PhyAddr + ifcParam.u32YStride * ifcParam.u32YuvHeight);
	pstDstImg.astChannels[2].u32Stride = ifcParam.u32VStride;
	pstDstImg.astChannels[2].u32AddrPhy = (AR_UINTPTR)(buff0.u64PhyAddr + ifcParam.u32YStride*ifcParam.u32YuvHeight + ifcParam.u32UStride*ifcParam.u32YuvHeight/2);
	
	AR_HAL_SCALER_CROP_S stCrop;
	stCrop.u32X = 0;
	stCrop.u32Y = 0;
	stCrop.u32W = pstSrcImg.u32Width;
	stCrop.u32H = pstSrcImg.u32Height;
	
	s32Ret = AR_MPI_SCALER_CropResizeRatio(&pstSrcImg, &stCrop, &pstDstImg, 1, SCALER_MODE_BILINEAR, AR_IMG_ALIGN_TO_MID);
	if(s32Ret < 0)
	{
		Log(ARCNN_LOG_ERR, "AR_MPI_SCALER_CropResizeRatio error: %d.\n", s32Ret);
		return -1;
	}

	STRU_SYS_DMA_TRAN_INFO tran_info0 = {0};
	tran_info0.s32_disable_cci = 1;
	tran_info0.s32_tran_cnt_sum = 1;
	tran_info0.st_addr_size[0].pv_src_pa = (AR_VOID*)buff0.u64PhyAddr;
	tran_info0.st_addr_size[0].pv_dst_pa = (AR_VOID*)buff1.u64PhyAddr;
	tran_info0.st_addr_size[0].u32_size = buff1.u64Len;
	s32Ret = ar_hal_sys_mem_transfer_pa(&tran_info0);

	buffMutex.unlock();
	ProfileEnd("GetLiveImage");
	return 0;
}



string CARCNN::GetLocalTestDir()
{
    return strLocalDir;
}


AR_S32 CARCNN::GetLocalImage(AR_CHAR * pchFileName, AR_IMG_S * pstImg, AR_U32 u32InputId)
{
    AR_U32 u32FileType = 0;
    static AR_U32 u32FrameId = 0;
	AR_U16 u16Stride = ifcParam.u32YStride;
	AR_CHAR* pchDstBuffVirt = (AR_CHAR*)buff1.u64VirtAddr;
	memset((void*)pchDstBuffVirt, 0, inputImgSize);

    ProfileStart("GetLocalImage");
    u32FileType = GetImageFileType(pchFileName);
    if(u32FileType == JPG_BMP_PNG_IMG_FILE)
    {
        src_img = cv::imread(pchFileName, cv::IMREAD_ANYCOLOR);
		yolov8Param.img_h  = src_img.rows;
		yolov8Param.img_w  = src_img.cols;
		
        if(src_img.empty())
        {
           Log(ARCNN_LOG_ERR, "Imread file %s failed: %s!\r\n", pchFileName, strerror(errno));
           return -1;
        }
		printf("src img w/h/c %d %d %d\r\n", src_img.cols, src_img.rows, src_img.channels());
		
		if(((AR_U32)src_img.cols != yolov8Param.net_w) ||((AR_U32)src_img.rows != yolov8Param.net_h))
		{
			cv::resize(src_img, dst_img, Size(yolov8Param.net_w, yolov8Param.net_h));
			printf("resize dst img w/h/c %d %d %d\r\n",dst_img.cols,dst_img.rows,dst_img.channels());
		}
		else
		{
           dst_img = src_img;
		   printf("Noneed resize dst img w/h/c %d %d %d\r\n",dst_img.cols,dst_img.rows,dst_img.channels());
		}

	    cv::Mat channels[3];
		cv::split(dst_img, channels);
		AR_CHAR *pchR = (AR_CHAR *)channels[2].data;
		AR_CHAR *pchG = (AR_CHAR *)channels[1].data;
		AR_CHAR *pchB = (AR_CHAR *)channels[0].data;

		for(int i = 0;i< dst_img.rows;i++)
		{
			for(int j = 0;j< dst_img.cols;j++)
			{
				pchDstBuffVirt[i*u16Stride + j] 				  = pchR[i*dst_img.cols + j];
				pchDstBuffVirt[(i+dst_img.rows)*u16Stride + j]	  = pchG[i*dst_img.cols + j];
				pchDstBuffVirt[(i+dst_img.rows*2)*u16Stride + j]  = pchB[i*dst_img.cols + j];
			}
			for(int j = dst_img.cols;j< u16Stride;j++)
			{
				pchDstBuffVirt[i*u16Stride + j] 				  = 0;
				pchDstBuffVirt[(i+dst_img.rows)*u16Stride + j]	  = 0;
				pchDstBuffVirt[(i+dst_img.rows*2)*u16Stride + j]  = 0;
			}
		}
		
		AR_U32 u32ChSize = u16Stride * dst_img.rows;
		pstImg->u32FrameId = u32FrameId++;
		pstImg->enFormat= AR_IMG_RGB;
		pstImg->astChannels[0].u32AddrPhy  	= (AR_UINTPTR)buff1.u64PhyAddr;
		pstImg->astChannels[0].uptrAddrVirt = (AR_UINTPTR)pchDstBuffVirt;
		pstImg->astChannels[1].u32AddrPhy 	= (AR_UINTPTR)(buff1.u64PhyAddr + u32ChSize);
		pstImg->astChannels[1].uptrAddrVirt = (AR_UINTPTR)(pchDstBuffVirt + u32ChSize);
		pstImg->astChannels[2].u32AddrPhy 	= (AR_UINTPTR)(buff1.u64PhyAddr + 2 * u32ChSize);
		pstImg->astChannels[2].uptrAddrVirt = (AR_UINTPTR)(pchDstBuffVirt + 2 *u32ChSize);

    }
    else
    {
		return -1;
    }

    ProfileEnd("GetLocalImage");
    printf("Get image %s..\n", pchFileName);
    return 0;
}

AR_S32 CARCNN::ReleaseLocalImage(AR_IMG_SET_S * pstImg)
{
    ProfileStart("ReleaseLocalImage");
	
    ProfileEnd("ReleaseLocalImage");
    return 0;
}


AR_S32 CARCNN::Forward(AR_IMG_SET_S * pstImg, AR_BOOL bInstant, AR_BOOL bDebug)
{
    ProfileStart("Forward");
	AR_U32 u32Chn1Size = ifcParam.u32YStride * ifcParam.u32YuvHeight;
	AR_U32 u32Chn2Size = u32Mode==ARCNN_INPUT_LOCAL ? ifcParam.u32UStride*ifcParam.u32YuvHeight : ifcParam.u32UStride*ifcParam.u32YuvHeight/2;
	
	buffMutex.lock();
	pstImg->astInputImg[0].astBatchImg[0].u32FrameId = 1;
	pstImg->astInputImg[0].bPreIfcProcess = AR_TRUE;
	pstImg->astInputImg[0].astBatchImg[0].enFormat = AR_IMG_YUV420P;
	pstImg->astInputImg[0].astBatchImg[0].u32Width = yolov8Param.net_w;
	pstImg->astInputImg[0].astBatchImg[0].u32Height = yolov8Param.net_h;
	pstImg->astInputImg[0].astBatchImg[0].u32ChannelNum = 3;
	pstImg->astInputImg[0].astBatchImg[0].astChannels[0].u32Stride = ifcParam.u32YStride;
	pstImg->astInputImg[0].astBatchImg[0].astChannels[0].u32AddrPhy = (AR_UINTPTR)buff1.u64PhyAddr;
	pstImg->astInputImg[0].astBatchImg[0].astChannels[1].u32Stride = ifcParam.u32UStride;
	pstImg->astInputImg[0].astBatchImg[0].astChannels[1].u32AddrPhy = (AR_UINTPTR)(buff1.u64PhyAddr + u32Chn1Size);
	pstImg->astInputImg[0].astBatchImg[0].astChannels[2].u32Stride = ifcParam.u32VStride;
	pstImg->astInputImg[0].astBatchImg[0].astChannels[2].u32AddrPhy = (AR_UINTPTR)(buff1.u64PhyAddr + u32Chn1Size + u32Chn2Size);
	

	AR_S32 s32Ret = AR_MPI_NPU_Forward(handle, pstImg, &stNPUInBuff, &stNPUOutBuff, bInstant, bDebug);
	buffMutex.unlock();
	
    ProfileEnd("Forward");
	
    if(u32ProfileEnable)
    {
		AR_U32 u32CBTime =0;
		AR_U32 *pCBTime = NULL;
		pCBTime =AR_MPI_NPU_GetCbTime(handle);
		for(AR_U32 i=0;i<128;i++){
	        if(pCBTime[i])
	        {
				printf("CB[%d] Time: %d x 0.01ms\r\n",i,pCBTime[i]);
			    u32CBTime = u32CBTime +pCBTime[i];
	        }
		}
		AR_U32 u32FwPreTime = AR_MPI_NPU_GetNPUPreTime(handle);
		AR_U32 u32FwTime = AR_MPI_NPU_GetNPURunTime(handle);

		u32TotalPreTime = u32TotalPreTime + u32FwPreTime;
		u32TotalFwTime = u32TotalFwTime + u32FwTime;
		u32TotalCBTime = u32TotalCBTime + u32CBTime;
		
		if(u32FisrtRun==0)
		{
			 u32FisrtRun = 1;
			 u32FirstPreTime = u32FwPreTime;
			 u32FirstFwTime = u32FwTime;
			 u32FirstCBTime = u32CBTime;
		}

		Log(ARCNN_LOG_INFO, "NPUFWPre: %d x 0.01ms  NPUFW: %d x 0.01ms  NPUCBTime: %d x 0.01ms \r\n",u32FwPreTime,u32FwTime, u32CBTime);
    }

    return s32Ret;
}


AR_S32 CARCNN::PostProcess(void * args)
{
	AR_S32 	s32Ret = 0;
	AR_U32 	obj_num = 0;
	candidate_Result candResult[MAX_NUM_CAN] = {0};
    AR_UINTPTR outputBuff = stNPUOutBuff.u64VirtAddr;
    AR_U32 size = AR_MPI_NPU_GetOutputBuffSize(handle);
    AR_MPI_NPU_InvalidCachedBuff(&stNPUOutBuff);
	
	ProfileStart("PostProcess");

    if(u32Mode == ARCNN_INPUT_LOCAL)
    {
	    AR_CHAR name[512] = {0};
	    sprintf(name, "%s%s", (AR_CHAR *)args, "_output.bin");
	    ofstream ofile(name, ios::binary);
	    if(ofile)
	    {
	        ofile.write((AR_CHAR *)outputBuff, size);
	        Log(ARCNN_LOG_DEBUG, "Save output to %s finished.\n", name);
	    }
    }
	
	obj_num = yolov8_seg_process(handle, (AR_CHAR*)outputBuff, yolov8Param, candResult, tensorsId, argbBuff);
	
	if(u32Mode == ARCNN_INPUT_LOCAL)
	{	
		for(AR_U32 h=0; h<yolov8Param.mask_h; h++)
		{
			for(AR_U32 w=0; w<yolov8Param.mask_w; w++)
			{
				if(((AR_UCHAR*)argbBuff.u64VirtAddr)[h*yolov8Param.mask_w*4 + w*4 +0]!=0 || 
						((AR_UCHAR*)argbBuff.u64VirtAddr)[h*yolov8Param.mask_w*4 + w*4 +1]!=0 || 
							((AR_UCHAR*)argbBuff.u64VirtAddr)[h*yolov8Param.mask_w*4 + w*4 +2]!=0)
				{
					dst_img.at<cv::Vec3b>(h,w) = cv::Vec3b(((AR_UCHAR*)argbBuff.u64VirtAddr)[h*yolov8Param.mask_w*4 + w*4 +0],
																((AR_UCHAR*)argbBuff.u64VirtAddr)[h*yolov8Param.mask_w*4 + w*4 +1], 
																		((AR_UCHAR*)argbBuff.u64VirtAddr)[h*yolov8Param.mask_w*4 + w*4 +2]);
				}
			}
		}
		cv::Mat saveImg;
		cv::resize(dst_img, saveImg, cv::Size(yolov8Param.img_w, yolov8Param.img_h));
		char saveName[256];
		snprintf(saveName, 256, "%s_%s", (AR_CHAR *)args, "mask.jpg");
		cv::imwrite(saveName, saveImg);
		Log(ARCNN_LOG_DEBUG, "Save output to %s finished.\n", saveName);
	}
	else if(u32Mode == ARCNN_INPUT_LIVE)
	{
		osdMutex.lock();
		STRU_SYS_DMA_TRAN_INFO tran_info0 = {0};
		tran_info0.s32_disable_cci = 1;
		tran_info0.s32_tran_cnt_sum = 1;
		tran_info0.st_addr_size[0].pv_src_pa = (AR_VOID*)argbBuff.u64PhyAddr;
		tran_info0.st_addr_size[0].pv_dst_pa = (AR_VOID*)tranBuff.u64PhyAddr;
		tran_info0.st_addr_size[0].u32_size = argbBuff.u64Len;
		s32Ret = ar_hal_sys_mem_transfer_pa(&tran_info0);
		osdMutex.unlock();
	}
	
	ProfileEnd("PostProcess");
    return s32Ret;
}


AR_S32 CARCNN::Log(AR_U32 u32LogLevel, const AR_CHAR * fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    char tmp[1024] = {0};
    vsnprintf(tmp, 1024, fmt, args);

    if(u32LogLevelCtrl >= u32LogLevel)
    {
        printf("[ARCNN][%s]: %s\n",
            (u32LogLevel == ARCNN_LOG_ERR)?"ERR":
            (u32LogLevel == ARCNN_LOG_WARNING)?"WARN":
            (u32LogLevel == ARCNN_LOG_DEBUG)?"DBG":"INFO",
             tmp);
    }

    va_end(args);
    return 0;
}

AR_S32 CARCNN::SetLogLevel(AR_U32 u32LogLevel)
{
    u32LogLevelCtrl = u32LogLevel;

    return 0;
}

AR_S32 CARCNN::EnableProfile()
{
    u32ProfileEnable = 1;
    return 0;
}

AR_S32 CARCNN::DisbleProfile()
{
    u32ProfileEnable = 0;
    return 0;
}

AR_S32 CARCNN::ProfileStart(string pchName)
{
    AR_U32 i = 0;

    if(u32ProfileEnable)
    {
        for(i = 0; i < ARCNN_MAX_PROFILE; i++)
        {
            if(astProfile[i].strName.length() == 0)
                break;
            if(astProfile[i].strName == pchName)
                break;
        }

        if(i == ARCNN_MAX_PROFILE)
        {
            Log(ARCNN_LOG_ERR, "Profile has run out.\n");
            return 0;
        }

        astProfile[i].strName = pchName;
        struct timeval tm;
        gettimeofday(&tm, NULL);
        astProfile[i].u64Tic = tm.tv_sec * 1000000 + tm.tv_usec;
    }

    return 0;
}

AR_S32 CARCNN::ProfileEnd(string pchName)
{
    AR_U32 i = 0;

    if(u32ProfileEnable)
    {
        struct timeval tm;
        gettimeofday(&tm, NULL);

        for(i = 0; i < ARCNN_MAX_PROFILE; i++)
        {
            if(astProfile[i].strName == pchName)
                break;
        }
        if(i == ARCNN_MAX_PROFILE)
        {
            Log(ARCNN_LOG_ERR, "Profile has run out.\n");
            return 0;
        }

        astProfile[i].u64Toc = tm.tv_sec * 1000000 + tm.tv_usec;

		AR_FLOAT dTime = (astProfile[i].u64Toc - astProfile[i].u64Tic)*1.0/1000;
        printf("[ARCNN][PROF]%s, line %d: %s takes %f ms\n",
            __FUNCTION__, __LINE__, astProfile[i].strName.c_str(), dTime);

        astProfile[i].strName.clear();
        astProfile[i].u64Tic = 0;
        astProfile[i].u64Toc = 0;
    }

    return 0;
}

AR_S32 CARCNN::EnableNPUDebug(AR_U32 u32Start, AR_U32 u32End)
{
	AR_S32 s32Ret = 0;
    AR_CHAR cStr[128] = {0};

	strcpy(cStr, strLocalDir.c_str());
    s32Ret = AR_MPI_NPU_EnableNpuDebug(handle, u32Start, u32End, cStr);
    return s32Ret;
}

AR_S32 CARCNN::DisableNPUDebug()
{
	AR_S32 s32Ret = 0;

	s32Ret = AR_MPI_NPU_DisableNpuDebug(handle);
    return s32Ret;
}

AR_S32 CARCNN::RegisterLayerDebugFunc(AR_NPU_LayerDebugFunc pDebugFunc)
{
    return AR_MPI_NPU_RegisterLayerDebugfunc(handle, pDebugFunc);
}

AR_S32 CARCNN::RegisterCallbackFunc(AR_CHAR * chName, AR_NPU_CallbackFunc pCBFunc)
{
    return AR_MPI_NPU_RegisterCallback(handle, chName, pCBFunc);
}

AR_S32 CARCNN::RegisterParseFunc(AR_CHAR * chName, AR_NPU_ParseFunc pParseFunc)
{
	return AR_MPI_NPU_RegisterParseFunc(handle, chName, pParseFunc);
}

AR_S32 CARCNN::RegisterIFCDebugFunc(AR_NPU_IFCDebugFunc pCBFunc)
{
	//debug temp
	AR_S32 s32Ret = 0;
	AR_CHAR cStr[128] = {0};

	strcpy(cStr, strLocalDir.c_str());
	s32Ret = AR_MPI_NPU_RegisterIFCDebugfunc(handle, pCBFunc, cStr);
   	return s32Ret;
}

void CARCNN::RegisterPostCB(PostProcessCB pCBFunc)
{
    cbFunc = pCBFunc;
}

AR_U32 CARCNN::IsInitialized()
{
    return u32Initialized;
}

AR_U32 CARCNN::GetInputNum()
{
    return AR_MPI_NPU_GetInputTensorNum(handle);
}

AR_U32 CARCNN::GetBatchNum()
{
    return AR_MPI_NPU_GetBatchNum(handle);
}

AR_S32 CARCNN::MallocNPUBuff()
{
    AR_U32 u32Size = 0;
    AR_S32 s32Ret = 0;

    u32Size = AR_MPI_NPU_GetInputBuffSize(handle);
    if(!u32Size)
    {
        Log(ARCNN_LOG_ERR, "Get NPU input size error.\n");
        return -1;
    }

	stNPUInBuff.u64Len = u32Size;
    s32Ret = AR_MPI_NPU_MallocCachedBuff((AR_CHAR *)"input", &stNPUInBuff);
    if(s32Ret)
    {
        Log(ARCNN_LOG_ERR, "Malloc NPU input error: size %d.\n", u32Size);
        return -1;
    }

    u32Size = AR_MPI_NPU_GetOutputBuffSize(handle);
    if(!u32Size)
    {
        Log(ARCNN_LOG_ERR, "Get NPU output size error.\n");
        AR_MPI_NPU_FreeBuff(&stNPUInBuff);
        return -1;
    }

	stNPUOutBuff.u64Len = u32Size;
    s32Ret = AR_MPI_NPU_MallocCachedBuff((AR_CHAR *)"output", &stNPUOutBuff);
    if(s32Ret)
    {
        Log(ARCNN_LOG_ERR, "Malloc NPU output error: size %d.\n", u32Size);
        AR_MPI_NPU_FreeBuff(&stNPUInBuff);
        return -1;
    }

	memset((void*)stNPUInBuff.u64VirtAddr,0,stNPUInBuff.u64Len);
	memset((void*)stNPUOutBuff.u64VirtAddr,0,stNPUOutBuff.u64Len);
	
	s32Ret = AR_MPI_NPU_InvalidCachedBuff(&stNPUOutBuff);
	if(s32Ret != 0)
		printf("invalid error! \n");

    return 0;
}

AR_S32 CARCNN::FreeNPUBuff()
{
    AR_S32 s32Ret = 0;
    s32Ret = AR_MPI_NPU_FreeBuff(&stNPUInBuff);
    if(s32Ret)
    {
        Log(ARCNN_LOG_ERR, "Free NPU input buffer error.\n");
    }

    s32Ret = AR_MPI_NPU_FreeBuff(&stNPUOutBuff);
    if(s32Ret)
    {
        Log(ARCNN_LOG_ERR, "Free NPU output buffer error.\n");
    }

    return s32Ret;
}


AR_S32 CARCNN::MallocRuntimeBuff()
{
    AR_U32 u32RuntimeSize = 0;
    AR_S32 s32Ret = 0;
	s32Ret = AR_MPI_NPU_GetRuntimeSize(handle, &u32RuntimeSize);
    if(s32Ret < 0) 
    {
        Log(ARCNN_LOG_ERR, "Get NPU RUNTIME size error.\n");
        return -1;
    }

	if(u32RuntimeSize ==0)
    {
        Log(ARCNN_LOG_ERR, "Get NPU RUNTIME size is ZERO.\n");
        return -1;
    }
	
	stNPURtBuff.u64Len = u32RuntimeSize;
    s32Ret = AR_MPI_NPU_MallocBuff((AR_CHAR *)"runtime", &stNPURtBuff);
    if(s32Ret)
    {
        Log(ARCNN_LOG_ERR, "Malloc NPU input error: size %d.\n", u32RuntimeSize);
        return -1;
    }

	memset((void*)stNPURtBuff.u64VirtAddr,0,stNPURtBuff.u64Len);

	s32Ret = AR_MPI_NPU_SetRuntimeBuffer(handle, (AR_UINTPTR)stNPURtBuff.u64VirtAddr, (AR_U64)stNPURtBuff.u64PhyAddr);
	if((s32Ret < 0) || (u32RuntimeSize ==0))
	{
		Log(ARCNN_LOG_ERR, "Get NPU RUNTIME size error.\n");
		return -1;
	}

    return 0;
}

AR_S32 CARCNN::FreeRuntimeBuff()
{
    AR_S32 s32Ret = 0;
	AR_U32 u32RuntimeSize = 0;
	s32Ret = AR_MPI_NPU_GetRuntimeSize(handle, &u32RuntimeSize);
    if(s32Ret < 0) 
    {
        Log(ARCNN_LOG_ERR, "Get NPU RUNTIME size error.\n");
        return -1;
    }

	if(u32RuntimeSize ==0)
    {
        Log(ARCNN_LOG_ERR, "Get NPU RUNTIME size is ZERO.\n");
        return -1;
    }
	
    s32Ret = AR_MPI_NPU_FreeBuff(&stNPURtBuff);
    if(s32Ret)
    {
        Log(ARCNN_LOG_ERR, "Free NPU runtime buffer error.\n");
    }

    return s32Ret;
}


