//this is the base class
#include <iostream>
#include <fstream>
#include <string>
#include <sys/time.h>
#include <stdarg.h>

#ifdef CONFIG_BIT_32BITS
#include <errno.h>
#endif
#include "cJSON.h"

#include "mpi_scaler_api.h"
#include "mpi_ifc_api.h"
#include "ar_comm_video.h"

#include "arcnn_base.h"
#include "arcnn_common.h"


AR_U32	  u32GSramAddrPhy =0;
AR_U32	  u32GSramSize =0;

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
        //Alloc id by driver
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
		cJSON * LiveVpssGrp = cJSON_GetObjectItemCaseSensitive(pNet, "vpss_grp");
		if(!LiveVpssGrp)
		{
			Log(ARCNN_LOG_ERR, "Parse vpss_grp failed.\n");
			VpssGrp = 1;
		}
		else
		{
			VpssGrp = atoi(LiveVpssGrp->valuestring);
		}
		
		cJSON * LiveVpssChn = cJSON_GetObjectItemCaseSensitive(pNet, "vpss_chn");
		if(!LiveVpssChn)
		{
			Log(ARCNN_LOG_ERR, "Parse vpss_chn failed.\n");
			VpssChn = 2;
		}
		else
		{
			VpssChn = atoi(LiveVpssChn->valuestring);
		}

		Log(ARCNN_LOG_DEBUG, "VpssGrp:%d VpssChn:%d.\n", VpssGrp, VpssChn);

        cJSON * drawosd = cJSON_GetObjectItemCaseSensitive(pNet, "draw_osd");
        if(!drawosd)
        {
            Log(ARCNN_LOG_ERR, "Parse drawosd failed.\n");
            u32DrawOSD = 0;
        }
        else
        {
            if(strcmp(drawosd->valuestring, "yes") == 0)
            {
                u32DrawOSD = 1;
            }
            else
            {
                u32DrawOSD = 0;
            }
        }
    }

    cJSON * loglevel = cJSON_GetObjectItemCaseSensitive(pNet, "log_level");
    if(!loglevel)
    {
        u32LogLevelCtrl = ARCNN_LOG_ERR;
    }
    else
    {
        u32LogLevelCtrl = atoi(loglevel->valuestring);
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
	ret = AR_MPI_NPU_GetIFCParamByName(handle, inputTensor.achName, &ifcParam);
	if(ret)
		printf("get ifc param fail~ \n");

	yolov8Param.net_h = inputTensor.u32Height;
	yolov8Param.net_w = inputTensor.u32Width;

	stPchbuff.u64Len = ifcParam.u32YStride * yolov8Param.net_h * 3;
	AR_MPI_NPU_MallocBuff((AR_CHAR *)"stPchBuff", &stPchbuff);
	if(ret)
		printf("malloc stPchBuff fail~ \n");

	getTensorId(handle, u32TensorsId);
	
    u32Initialized = 1;

}

CARCNN::~CARCNN()
{
    //unload model
    if(handle)
    {
        AR_MPI_NPU_UnloadModel(handle);
		AR_MPI_NPU_FreeBuff(&stPchbuff);
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


AR_S32 CARCNN::BindVideoDev()
{
	if(u32DrawOSD)		//for osd
	{
		pstIpcClient = ar_ipc_create();
		if(!pstIpcClient){
			printf("failed to create client ipc\r\n");
			return -1;
		}
		u32ClientHandle = ar_ipc_get_service(pstIpcClient, "ipc_ctrl");
		if(!u32ClientHandle){
			printf("failed to get service BINDER_IPCTOOL_NAME \r\n");
			return -1;
		}
	}

	drawOsdThread = std::thread(&CARCNN::DrawOsd, this);	//draw osd
    return 0;

}

AR_S32 CARCNN::GetLiveImage(AR_IMG_S * pstImg)
{
    ProfileStart("GetLiveImage");
	memset((AR_CHAR*)stPchbuff.u64VirtAddr, 0, stPchbuff.u64Len);
    memset(&VFrameInfo,0,sizeof(VIDEO_FRAME_INFO_S));
	
	AR_S32 ret=AR_MPI_VPSS_GetChnFrame(VpssGrp, VpssChn, &VFrameInfo, -1);
	if(ret<0)
	{
		Log(ARCNN_LOG_ERR,"AR_MPI_VPSS_GetChnFrame ERR ret=%x\r\n", ret);
		return -1;
	}

	AR_IMG_FORMAT_E FrameFmt = AR_IMG_YUV420P;

	AR_IMG_S pstSrcImg;
	pstSrcImg.u32FrameId = VFrameInfo.stVFrame.u32FrameFlag;
    pstSrcImg.enFormat = FrameFmt;
    pstSrcImg.u32Width = VFrameInfo.stVFrame.u32Width;
    pstSrcImg.u32Height = VFrameInfo.stVFrame.u32Height;
	pstSrcImg.u32ChannelNum = 3;
    pstSrcImg.astChannels[0].u32Stride = VFrameInfo.stVFrame.u32Stride[0];
	pstSrcImg.astChannels[0].u32AddrPhy = (AR_U32)VFrameInfo.stVFrame.u64PhyAddr[0];
	pstSrcImg.astChannels[0].uptrAddrVirt = (AR_U32)VFrameInfo.stVFrame.u64VirAddr[0];
    pstSrcImg.astChannels[1].u32Stride = VFrameInfo.stVFrame.u32Stride[1];
	pstSrcImg.astChannels[1].u32AddrPhy = (AR_U32)VFrameInfo.stVFrame.u64PhyAddr[1];
	pstSrcImg.astChannels[1].uptrAddrVirt = (AR_U32)VFrameInfo.stVFrame.u64VirAddr[1];
	pstSrcImg.astChannels[2].u32Stride = VFrameInfo.stVFrame.u32Stride[2];
	pstSrcImg.astChannels[2].u32AddrPhy = (AR_U32)VFrameInfo.stVFrame.u64PhyAddr[2];
	pstSrcImg.astChannels[2].uptrAddrVirt = (AR_U32)VFrameInfo.stVFrame.u64VirAddr[2];

	yolov8Param.img_h = pstSrcImg.u32Height;
	yolov8Param.img_w = pstSrcImg.u32Width;
	int max_src = GET_MAX(yolov8Param.img_w, yolov8Param.img_h);
	yolov8Param.ratio = (AR_FLOAT)max_src/(AR_FLOAT)yolov8Param.net_w;
	

	pstImg->u32FrameId = VFrameInfo.stVFrame.u32FrameFlag;
	pstImg->enFormat = FrameFmt;
	pstImg->u32Width = yolov8Param.net_w;
	pstImg->u32Height = yolov8Param.net_h;
	pstImg->u32ChannelNum = 3;
	pstImg->astChannels[0].u32AddrPhy = (AR_UINTPTR)stPchbuff.u64PhyAddr;
	pstImg->astChannels[0].uptrAddrVirt = (AR_UINTPTR)stPchbuff.u64VirtAddr;
	pstImg->astChannels[0].u32Stride = ifcParam.u32YStride;
	pstImg->astChannels[1].u32AddrPhy = (AR_UINTPTR)(stPchbuff.u64PhyAddr +  ifcParam.u32YStride*yolov8Param.net_h);
	pstImg->astChannels[1].uptrAddrVirt = (AR_UINTPTR)(stPchbuff.u64VirtAddr +  ifcParam.u32YStride*yolov8Param.net_h);
	pstImg->astChannels[1].u32Stride = ifcParam.u32UStride;
	pstImg->astChannels[2].u32AddrPhy = (AR_UINTPTR)(stPchbuff.u64PhyAddr + ifcParam.u32YStride*yolov8Param.net_h + ifcParam.u32UStride*yolov8Param.net_h/2);
	pstImg->astChannels[2].uptrAddrVirt = (AR_UINTPTR)(stPchbuff.u64VirtAddr + ifcParam.u32YStride*yolov8Param.net_h + ifcParam.u32UStride*yolov8Param.net_h/2);
	pstImg->astChannels[2].u32Stride = ifcParam.u32VStride;
	
	AR_HAL_SCALER_CROP_S stCrop;
	stCrop.u32X = 0;
	stCrop.u32Y = 0;
	stCrop.u32W = pstSrcImg.u32Width;
	stCrop.u32H = pstSrcImg.u32Height;
	ret = AR_MPI_SCALER_CropResizeRatio(&pstSrcImg, &stCrop, pstImg, 1, SCALER_MODE_BILINEAR, 0x11);
	if(ret < 0)
	{
		AR_MPI_VPSS_ReleaseChnFrame(1, 2, &VFrameInfo);
		Log(ARCNN_LOG_ERR, "AR_MPI_SCALER_CropResizeRatio error: %d.\n", ret);
		return -1;
	}

	ProfileEnd("GetLiveImage");

	ret = AR_MPI_VPSS_ReleaseChnFrame(VpssGrp, VpssChn, &VFrameInfo);
	if(ret < 0)
	{
		Log(ARCNN_LOG_ERR,"AR_MPI_VPSS_ReleaseChnFrame ERR ret=%x.\n",ret);
		return -1;
	}
	
    return 1;
}


AR_S32 CARCNN::ReleaseLiveImage(AR_IMG_S * pstImg)
{
    ProfileStart("ReleaseLiveImage");

    ProfileEnd("ReleaseLiveImage");
    return 0;
}

AR_S32 CARCNN::DeBindVideoDev()
{
    if(u32ClientHandle && pstIpcClient){
        ar_ipc_close_service(pstIpcClient, u32ClientHandle);
        ar_ipc_close(pstIpcClient);
        pstIpcClient = NULL;
        u32ClientHandle = 0;
    }
    return 0;
}

string CARCNN::GetLocalTestDir()
{
    return strLocalDir;
}



AR_S32 CARCNN::GetLocalImage(AR_CHAR * pchFileName, AR_IMG_S * pstImg, AR_U32 u32InputId)
{
    AR_U32 u32FileType = 0;
    AR_S32 s32FileSize = 0;
    static AR_U32 u32FrameId = 0;

    ProfileStart("GetLocalImage");
    u32FileType = GetImageFileType(pchFileName);
    if(u32FileType == JPG_BMP_PNG_IMG_FILE)
    {
        src_img = cv::imread(pchFileName, cv::IMREAD_ANYCOLOR);
        if(src_img.empty())
        {
           Log(ARCNN_LOG_ERR, "Imread file %s failed: %s!\r\n", pchFileName, strerror(errno));
           return -1;
        }
		printf("src img w/h/c %d %d %d\r\n",src_img.cols,src_img.rows,src_img.channels());

		AR_NPU_TENSOR_S stTensor ={0};
		AR_MPI_NPU_GetInputTensorParam(handle, u32InputId, &stTensor);
		yolov8Param.oriImg = src_img;
		yolov8Param.img_h  = src_img.rows;
		yolov8Param.img_w  = src_img.cols;
		yolov8Param.net_h  = stTensor.u32Height;
		yolov8Param.net_w  = stTensor.u32Width;
		
		if(((AR_U32)src_img.cols != stTensor.u32Width) ||((AR_U32)src_img.rows != stTensor.u32Height))
		{
			cv::resize(src_img,dst_img,Size(stTensor.u32Width, stTensor.u32Height));
			printf("resize dst img w/h/c %d %d %d\r\n",dst_img.cols,dst_img.rows,dst_img.channels());
		}
		else
		{
           dst_img = src_img;
		   printf("Noneed resize dst img w/h/c %d %d %d\r\n",dst_img.cols,dst_img.rows,dst_img.channels());
		}

		AR_NPU_IFC_PARAM_S stIFCParam = {0};
		int ret = AR_MPI_NPU_GetIFCParamByName(handle, stTensor.achName,  &stIFCParam);
		if(ret)
		{
		   Log(ARCNN_LOG_ERR, "AR_MPI_NPU_GetIFCParamByName error.\n");
		   return -1;
		}
		AR_U16 u16Stride = stIFCParam.u32YStride;
        s32FileSize = u16Stride * dst_img.rows * dst_img.channels();

		AR_MEM_S stPchbuff;
		stPchbuff.u64Len= s32FileSize;
		ret = AR_MPI_NPU_MallocBuff((AR_CHAR *)"Pchbuff", &stPchbuff);
		if(ret)
		{
		   Log(ARCNN_LOG_ERR, "Malloc NPU input error: size %d.\n", stPchbuff.u64Len);
		   return -1;
		}

        if(src_img.channels()==1)
        {
	        cv::Mat channels[1];
	        cv::split(dst_img, channels);
	        AR_CHAR *pchR = (AR_CHAR *)channels[0].data;

			AR_CHAR* pchDstBuffVirt = (AR_CHAR*)stPchbuff.u64VirtAddr;
			for(int i = 0;i< dst_img.rows;i++)
			{
				for(int j = 0;j< dst_img.cols;j++)
				{
					pchDstBuffVirt[i*u16Stride + j] 				  = pchR[i*dst_img.cols + j];
				}
				for(int j = dst_img.cols;j< u16Stride;j++)
				{
					pchDstBuffVirt[i*u16Stride + j] 				  = 0;
				}
	    	}

	        pstImg->u32FrameId = u32FrameId++;
	        pstImg->enFormat= AR_IMG_GRAY;
	        pstImg->astChannels[0].u32AddrPhy  = (AR_UINTPTR)stPchbuff.u64PhyAddr;
	        pstImg->astChannels[0].uptrAddrVirt = (AR_UINTPTR)pchDstBuffVirt;
	        pstImg->astChannels[1].u32AddrPhy = (AR_UINTPTR)stPchbuff.u64PhyAddr;
	        pstImg->astChannels[1].uptrAddrVirt = (AR_UINTPTR)pchDstBuffVirt;
	        pstImg->astChannels[2].u32AddrPhy = (AR_UINTPTR)stPchbuff.u64PhyAddr;
	        pstImg->astChannels[2].uptrAddrVirt = (AR_UINTPTR)pchDstBuffVirt;
	        pstImg->astChannels[3].u32AddrPhy = 0;

        }
		else
		{
		    cv::Mat channels[3];
			cv::split(dst_img, channels);
			AR_CHAR *pchR = (AR_CHAR *)channels[2].data;
			AR_CHAR *pchG = (AR_CHAR *)channels[1].data;
			AR_CHAR *pchB = (AR_CHAR *)channels[0].data;

			AR_CHAR* pchDstBuffVirt = (AR_CHAR*)stPchbuff.u64VirtAddr;
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
			pstImg->astChannels[0].u32AddrPhy  = (AR_UINTPTR)stPchbuff.u64PhyAddr;
			pstImg->astChannels[0].uptrAddrVirt = (AR_UINTPTR)pchDstBuffVirt;
			pstImg->astChannels[1].u32AddrPhy = (AR_UINTPTR)(stPchbuff.u64PhyAddr + u32ChSize);
			pstImg->astChannels[1].uptrAddrVirt = (AR_UINTPTR)(pchDstBuffVirt + u32ChSize);
			pstImg->astChannels[2].u32AddrPhy = (AR_UINTPTR)(stPchbuff.u64PhyAddr + 2 * u32ChSize);
			pstImg->astChannels[2].uptrAddrVirt = (AR_UINTPTR)(pchDstBuffVirt + 2 *u32ChSize);
			pstImg->astChannels[3].u32AddrPhy = 0;

		}

    }
    else
    {
		return -1;
    }

    ProfileEnd("GetLocalImage");
    Log(ARCNN_LOG_DEBUG, "Get image %s..\n", pchFileName);
    return 0;
}

AR_S32 CARCNN::ReleaseLocalImage(AR_IMG_SET_S * pstImg)
{
    ProfileStart("ReleaseLocalImage");
	if(!pstImg)
	{
		Log(ARCNN_LOG_ERR, "pstImg is NULL.\n");
        return -1;
	}
    for(AR_U32 u32IFCInputId =0;u32IFCInputId <MAX_INPUT_IMG_NUM;u32IFCInputId++)
	{
	    for(AR_U32 u32IFCBatchId =0;u32IFCBatchId <MAX_BATCH_IMG_NUM;u32IFCBatchId++)
	    {
	        AR_IMG_S *pstImgTmp = &pstImg->astInputImg[u32IFCInputId].astBatchImg[u32IFCBatchId];
			if( pstImgTmp->astChannels[0].uptrAddrVirt )
		    {
		        AR_MEM_S stPchbuff;
				stPchbuff.u64VirtAddr = pstImgTmp->astChannels[0].uptrAddrVirt;
				stPchbuff.u64PhyAddr= pstImgTmp->astChannels[0].u32AddrPhy;
		        AR_MPI_NPU_FreeBuff(&stPchbuff);
			    pstImgTmp->astChannels[0].uptrAddrVirt = 0;
		    }
	    }
	}
	
    ProfileEnd("ReleaseLocalImage");
    return 0;
}


AR_S32 CARCNN::Forward(AR_IMG_SET_S * pstImg, AR_BOOL bInstant, AR_BOOL bDebug)
{
    ProfileStart("Forward");
    AR_S32 s32Ret = AR_MPI_NPU_Forward(handle, pstImg, &stNPUInBuff, &stNPUOutBuff, bInstant, bDebug);
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


//here, we pass the input image name to generate a output file.
AR_S32 CARCNN::PostProcess(void * args)
{
    AR_S32 	s32Ret = 0;
	AR_U32 	obj_num = 0;
    AR_UINTPTR outputBuff = stNPUOutBuff.u64VirtAddr;
    AR_U32 size = AR_MPI_NPU_GetOutputBuffSize(handle);
    AR_MPI_NPU_InvalidCachedBuff(&stNPUOutBuff);

	ProfileStart("PostProcess");
    if(u32Mode == ARCNN_INPUT_LOCAL)
    {
	    AR_CHAR name[512] = {0};
	    snprintf(name, 512, "%s%s", (AR_CHAR *)args, "_output.bin");
	    ofstream ofile(name, ios::binary);
	    if(ofile)
	    {
	        ofile.write((AR_CHAR *)outputBuff, size);
	        Log(ARCNN_LOG_DEBUG, "Save output to %s finished.\n", name);
	    }
    }

	candidate_Result candidateResult[MAX_NUM_CAN] = {0};
	detector_Result  detResult[MAX_NUM_DET] = {0};
	
	obj_num = yolov8_postprocess(handle, (AR_CHAR*)outputBuff, yolov8Param, detResult, candidateResult, u32TensorsId);

	if(u32Mode == ARCNN_INPUT_LOCAL)
	{
		cv::Mat saveImg = yolov8Param.oriImg;
		AR_CHAR titlech[100] = {0};
		for(AR_U32 i=0; i<obj_num; i++)
		{
			AR_FLOAT x1 = detResult[i].x_min * ((AR_FLOAT)saveImg .cols / yolov8Param.net_h);
			AR_FLOAT y1 = detResult[i].y_min * ((AR_FLOAT)saveImg .rows / yolov8Param.net_h);
			AR_FLOAT x2 = detResult[i].x_max * ((AR_FLOAT)saveImg .cols / yolov8Param.net_h);
			AR_FLOAT y2 = detResult[i].y_max * ((AR_FLOAT)saveImg .rows / yolov8Param.net_h);
			
			cv::rectangle(saveImg, cvPoint((int)x1, (int)y1), cvPoint((int)x2, (int)y2), 
				Scalar(255, 255, 255), 2);
			cv::rectangle(saveImg, cvPoint((int)x1, (int)y1 + 10), cvPoint((int)x1 + 70, (int)y1),
				Scalar(255, 255, 255), -1);
			snprintf(titlech, 100, "%d-%.2f", detResult[i].clsId, detResult[i].score);
			string titlestr = titlech;
			cv::putText(saveImg, titlestr, cvPoint((int)x1, (int)y1 + 10), FONT_HERSHEY_COMPLEX, 0.5, Scalar(0, 0, 0), 1);
		}
		AR_CHAR imgSavePath[256];
		snprintf(imgSavePath, 256, "%s_%s", (AR_CHAR *)args, "out.jpg");
		cv::imwrite(imgSavePath, saveImg);
	}
	else if(u32Mode == ARCNN_INPUT_LIVE)
	{
		IPC_CTRL_OSD_S stOsd = {0};
		
		stOsd.u32VpssGrp = 1;
		stOsd.u32StrNum = obj_num;
		stOsd.u32RectNum= obj_num;
		stOsd.u32ImgHeight = yolov8Param.img_h;
		stOsd.u32ImgWidth  = yolov8Param.img_w;
		
		if(obj_num != 0)
		{			
			for(AR_U32 i=0; i<obj_num; i++)
			{
				AR_FLOAT x1 = detResult[i].x_min * yolov8Param.ratio;
				AR_FLOAT y1 = detResult[i].y_min * yolov8Param.ratio;

			   	AR_CHAR info[32]={0};
			   	snprintf(info, 32, "%d:%.2f", detResult[i].clsId, detResult[i].score);
			   	strcpy(stOsd.stOsdStr[i].achStr, info);
			   	stOsd.stOsdStr[i].u32X = (int)x1;
			   	stOsd.stOsdStr[i].u32Y = (int)y1;		   
			   	stOsd.stOsdStr[i].u32RGB = 0x00FF0000;
			}
			
			for(AR_U32 i=0; i<obj_num; i++)
			{
				AR_FLOAT x1 = detResult[i].x_min * yolov8Param.ratio;
				AR_FLOAT y1 = detResult[i].y_min * yolov8Param.ratio;
				AR_FLOAT x2 = detResult[i].x_max * yolov8Param.ratio;
				AR_FLOAT y2 = detResult[i].y_max * yolov8Param.ratio;
				
			   	stOsd.stOsdRect[i].u32X = (int)x1;
			   	stOsd.stOsdRect[i].u32Y = (int)y1;
			   	stOsd.stOsdRect[i].u32W = (int)(x2 - x1);
			   	stOsd.stOsdRect[i].u32H = (int)(y2 - y1);
			   	stOsd.stOsdRect[i].u32RGB = 0x00FF;
			}
		}

		osdMutex.lock();
		osdQueue.push(stOsd);
		osdMutex.unlock();

		#if 0	//this part will cost 15+ms, so we call a thread(DrawOsd) to draw osd
		BINDER_RET_S stBinderRet = {0};
		s32Ret = ar_ipc_send_buffer(pstIpcClient, u32ClientHandle, IPC_RPC_CMD_DRAW_VPSS_OSD, &stOsd, sizeof(stOsd), &stBinderRet, 0);
		if(s32Ret<0)
		{
		   Log(ARCNN_LOG_ERR, "draw osd err ret=%d\r\n", s32Ret);
		   return -1;
		}
		#endif
	}

	ProfileEnd("PostProcess");
    return s32Ret;
}


AR_S32 CARCNN::DrawOsd()
{
	while(AR_TRUE)
	{
		osdMutex.lock();
		if(osdQueue.size() != 0)
		{
			IPC_CTRL_OSD_S _stOsd = osdQueue.front();
			BINDER_RET_S stBinderRet = {0};
			AR_S32 s32Ret = ar_ipc_send_buffer(pstIpcClient, u32ClientHandle, IPC_RPC_CMD_DRAW_VPSS_OSD, &_stOsd, sizeof(_stOsd), &stBinderRet, 0);
			if(s32Ret<0)
			{
			   Log(ARCNN_LOG_ERR, "draw osd err ret=%d\r\n", s32Ret);
			   return -1;
			}
		}
		if(osdQueue.size() > 0)
		{
			osdQueue.pop();
		}
		osdMutex.unlock();
		usleep(2 * 1000);
	}
    return 0;
}



AR_S32 CARCNN::Log(AR_U32 u32LogLevel, const AR_CHAR * fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    AR_CHAR tmp[1024] = {0};
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
	AR_MPI_NPU_InvalidCachedBuff(&stNPUOutBuff);

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


