//this is the base class
#include <iostream>
#include <fstream>
#include <string>
#include <sys/time.h>
#include <stdarg.h>
#include <unistd.h>
//#include <sys/io.h>
#include <sys/stat.h>
#include "cJSON.h"
#include "opencv2/opencv.hpp"
#include "ar_cnn_channelcvt.h"

#include "mpi_scaler_api.h"
#include "mpi_ifc_api.h"
#include "ar_comm_video.h"

//#include "mpi_npu_api.h"

using namespace std;
using namespace cv;

#include "arcnn_base.h"
#include "arcnn_common.h"

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

    #if 0   //test for npubin virtaddr START
    AR_CHAR * npuBinBuff = NULL;
	FILE*fNpu = fopen(stCNNDesc.au8NpubinFileName, "rb");
	if(!fNpu) {
		printf("Open file %s failed.\n", stCNNDesc.au8NpubinFileName);
		return;
	}
	fseek(fNpu, 0L, SEEK_END);
	AR_S32 fileLen = ftell(fNpu);
	rewind(fNpu);

	npuBinBuff =(AR_CHAR*)malloc(fileLen);
	if(!npuBinBuff){
		printf("malloc npuBinBuff err\r\n");
		fclose(fNpu);
		return;
	}
	memset(npuBinBuff,0,fileLen);

	ret = fread(npuBinBuff, 1, fileLen, fNpu);
    if(ret != fileLen) {
        printf("Failed to read npubinfile\n");
		free(npuBinBuff);
		fclose(fNpu);
		return;
    }

	stCNNDesc.uptrNpubinVirtAddr =(AR_UINTPTR)npuBinBuff;
	printf("read npubin file succeed addr =%p fileLen=%d\r\n",npuBinBuff,fileLen);

	#endif //test for npubin virtaddr END

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
        u32Mode = ARCNN_INPUT_LIVE;
        cJSON * devname = cJSON_GetObjectItemCaseSensitive(pNet, "devname");
        if(!devname)
        {
            Log(ARCNN_LOG_ERR, "Parse devname failed.\n");
            delete[] pchBuff;
            cJSON_Delete(pJson);
            return;
        }

        strDevName = devname->valuestring;
		cJSON * LiveVpssGrp = cJSON_GetObjectItemCaseSensitive(pNet, "vpss_grp");
		if(!LiveVpssGrp)
		{
			Log(ARCNN_LOG_ERR, "Parse vpss_grp failed.\n");
			VpssGrp = 0;
		}
		else
		{
			VpssGrp = atoi(LiveVpssGrp->valuestring);
		}
		
		cJSON * LiveVpssChn = cJSON_GetObjectItemCaseSensitive(pNet, "vpss_chn");
		if(!LiveVpssChn)
		{
			Log(ARCNN_LOG_ERR, "Parse vpss_chn failed.\n");
			VpssChn = 0;
		}
		else
		{
			VpssChn = atoi(LiveVpssChn->valuestring);
		}

		Log(ARCNN_LOG_DEBUG, "VpssGrp:%d VpssChn:%d.\n",VpssGrp,VpssChn);

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
            stCNNDesc.u32SramSize = strtoul(sramsize->valuestring, NULL, 16);
			ret= AR_MPI_NPU_SetSecurity(1);
			if(ret<0)
			{
				Log(ARCNN_LOG_ERR, "ar_hal_npu_set_security failed.\n");
			}
        }
    }


#if YOLOv5_POSTPROC
	cJSON * conf = cJSON_GetObjectItemCaseSensitive(pNet, "conf_thr");
	if(!mode)
	{
		Log(ARCNN_LOG_ERR, "Parse conf_thr node failed.\n");
		return;
	}
	yolov5_params.conf_thr = atof(conf->valuestring);
		
	cJSON * nms = cJSON_GetObjectItemCaseSensitive(pNet, "nms_thr");
	if(!mode)
	{
		Log(ARCNN_LOG_ERR, "Parse nms_thr node failed.\n");
		return;
	}
	yolov5_params.nms_thr = atof(nms->valuestring);

	cJSON * postproc_version = cJSON_GetObjectItemCaseSensitive(pNet, "postproc_ver");
	if(!mode)
	{
		Log(ARCNN_LOG_ERR, "Parse postproc version node failed.\n");
		return;
	}
	yolov5_params.postproc_version = atoi(postproc_version->valuestring);
	
    cJSON * rslt_path = cJSON_GetObjectItemCaseSensitive(pNet, "rslt_path");
    if(!rslt_path && u32Mode == ARCNN_INPUT_LOCAL)
    {
        Log(ARCNN_LOG_ERR, "Parse rslt path failed.\n");
        delete[] pchBuff;
        cJSON_Delete(pJson);
        return;
    }
	strcpy(rsltDir, rslt_path->valuestring);
#endif

    delete[] pchBuff;
    cJSON_Delete(pJson);

    Log(ARCNN_LOG_INFO, "Load Model begin.\n");
    //Loadmodel
    handle = AR_MPI_NPU_LoadModel(&stCNNDesc);
    if(!handle)
    {
        Log(ARCNN_LOG_ERR, "Load Model failed.\n");
        return;
    }

    Log(ARCNN_LOG_INFO, "Load net %d finished.\n", AR_MPI_NPU_GetNetworkId(handle));

	params_init(handle, &yolov5_params);

	InFlag = 0;

	/*********get IFC parameter for YUV align*********/
	AR_NPU_TENSOR_S stTensor ={0};
    ret = AR_MPI_NPU_GetInputTensorParam(handle, 0, &stTensor);
    if(ret)
    {
       Log(ARCNN_LOG_ERR, "AR_MPI_NPU_GetInputTensorParam error.\n");
    }
    ret = AR_MPI_NPU_GetIFCParamByName(handle, stTensor.achName, &stIFCParam);
    if(ret)
    {
       Log(ARCNN_LOG_ERR, "AR_MPI_NPU_GetIFCParamByName error.\n");
    }
	/*************************************************/

	s32FileSize = stIFCParam.u32YStride * yolov5_params.net_h * 3;
	stPchbuff.u64Len= s32FileSize;
	ret = AR_MPI_NPU_MallocBuff((AR_CHAR *)"Pchbuff", &stPchbuff);
	if(ret)
	{
	   Log(ARCNN_LOG_ERR, "Malloc NPU input error: size %d.\n", stPchbuff.u64Len);
	}

    u32Initialized = 1;

}

CARCNN::~CARCNN()
{
    //unload model
    if(handle)
    {
        AR_MPI_NPU_UnloadModel(handle);
    }
	
    handle = NULL;

	params_uninit(&yolov5_params);
	AR_MPI_NPU_FreeBuff(&stPchbuff);
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

string CARCNN::GetLocalTestDir()
{
    return strLocalDir;
}

void CARCNN::SetFileName(AR_CHAR *value)
{
    testFileName = value;
}

AR_S32 CARCNN::BindVideoDev()
{
	if(u32DrawOSD)
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
    return 0;
}


AR_S32 CARCNN::GetLiveImage(AR_IMG_S * pstImg)
{
    ProfileStart("GetLiveImage");

    memset(&VFrameInfo,0,sizeof(VIDEO_FRAME_INFO_S));
	AR_S32 ret=AR_MPI_VPSS_GetChnFrame(VpssGrp, VpssChn, &VFrameInfo, -1);
	if(ret<0)
	{
		Log(ARCNN_LOG_ERR,"AR_MPI_VPSS_GetChnFrame ERR ret=%x\r\n", ret);
		return -1;
	}

	AR_IMG_FORMAT_E FrameFmt = AR_IMG_YUV420P;
	if(VFrameInfo.stVFrame.enPixelFormat == PIXEL_FORMAT_YUV_SEMIPLANAR_420)
		FrameFmt = AR_IMG_NV12;
	else if(VFrameInfo.stVFrame.enPixelFormat == PIXEL_FORMAT_YVU_PLANAR_420)
		FrameFmt = AR_IMG_YUV420P;
	else{
		Log(ARCNN_LOG_ERR, "This example does not currently support other format parsing tests!\n");
		return -1;
	}
	//printf("======================VFrameInfo frame id = %d.======================\n", VFrameInfo.stVFrame.u32FrameId);
	AR_IMG_S pstSrcImg;
	pstSrcImg.u32FrameId = VFrameInfo.stVFrame.u32FrameFlag;
    pstSrcImg.enFormat = FrameFmt;
    pstSrcImg.u32Width = VFrameInfo.stVFrame.u32Width;
    pstSrcImg.u32Height = VFrameInfo.stVFrame.u32Height;
	if(FrameFmt == AR_IMG_YUV420P){
    	pstSrcImg.u32ChannelNum = 3;
	}else{
		pstSrcImg.u32ChannelNum = 2;
	}
    pstSrcImg.astChannels[0].u32Stride = VFrameInfo.stVFrame.u32Stride[0];
	pstSrcImg.astChannels[0].u32AddrPhy = (AR_U32)VFrameInfo.stVFrame.u64PhyAddr[0];
	pstSrcImg.astChannels[0].uptrAddrVirt = (AR_U32)VFrameInfo.stVFrame.u64VirAddr[0];
    pstSrcImg.astChannels[1].u32Stride = VFrameInfo.stVFrame.u32Stride[1];
	pstSrcImg.astChannels[1].u32AddrPhy = (AR_U32)VFrameInfo.stVFrame.u64PhyAddr[1];
	pstSrcImg.astChannels[1].uptrAddrVirt = (AR_U32)VFrameInfo.stVFrame.u64VirAddr[1];
	if(FrameFmt == AR_IMG_YUV420P){
    	pstSrcImg.astChannels[2].u32Stride = VFrameInfo.stVFrame.u32Stride[2];
    	pstSrcImg.astChannels[2].u32AddrPhy = (AR_U32)VFrameInfo.stVFrame.u64PhyAddr[2];
		pstSrcImg.astChannels[2].uptrAddrVirt = (AR_U32)VFrameInfo.stVFrame.u64VirAddr[2];
	}
	if(!InFlag){
		yolov5_params.img_h = pstSrcImg.u32Height;
		yolov5_params.img_w = pstSrcImg.u32Width;
		int max_src = GET_MAX(yolov5_params.img_w, yolov5_params.img_h);
		yolov5_params.ratio = (float)max_src / (float)yolov5_params.net_w;
		InFlag = 1;
	}	

	pstImg->u32FrameId = VFrameInfo.stVFrame.u32FrameFlag;
	pstImg->enFormat = FrameFmt;
	pstImg->u32Width = yolov5_params.net_w;
	pstImg->u32Height = yolov5_params.net_h;
	if(FrameFmt == AR_IMG_YUV420P){
		pstImg->u32ChannelNum = 3;
	}else{
		pstImg->u32ChannelNum = 2;
	}
	pstImg->astChannels[0].u32AddrPhy = (AR_UINTPTR)stPchbuff.u64PhyAddr;
	pstImg->astChannels[0].uptrAddrVirt = (AR_UINTPTR)stPchbuff.u64VirtAddr;
	pstImg->astChannels[0].u32Stride = stIFCParam.u32YStride;
	pstImg->astChannels[1].u32AddrPhy = (AR_UINTPTR)(stPchbuff.u64PhyAddr +  stIFCParam.u32YStride*yolov5_params.net_h);
	pstImg->astChannels[1].uptrAddrVirt = (AR_UINTPTR)(stPchbuff.u64VirtAddr +  stIFCParam.u32YStride*yolov5_params.net_h);
	pstImg->astChannels[1].u32Stride = stIFCParam.u32UStride;
	if(FrameFmt == AR_IMG_YUV420P){
		pstImg->astChannels[2].u32AddrPhy = (AR_UINTPTR)(stPchbuff.u64PhyAddr + stIFCParam.u32YStride*yolov5_params.net_h + stIFCParam.u32UStride*yolov5_params.net_h/2);
		pstImg->astChannels[2].uptrAddrVirt = (AR_UINTPTR)(stPchbuff.u64VirtAddr + stIFCParam.u32YStride*yolov5_params.net_h + stIFCParam.u32UStride*yolov5_params.net_h/2);
		pstImg->astChannels[2].u32Stride = stIFCParam.u32VStride;
	}
	AR_HAL_SCALER_CROP_S stCrop;
	stCrop.u32X = 0;
	stCrop.u32Y = 0;
	stCrop.u32W = pstSrcImg.u32Width;
	stCrop.u32H = pstSrcImg.u32Height;
	ret = AR_MPI_SCALER_CropResizeRatio(&pstSrcImg, &stCrop, pstImg, 1, SCALER_MODE_BILINEAR, 0x11);
	if(ret < 0)
	{
		AR_MPI_VPSS_ReleaseChnFrame(VpssGrp, VpssChn, &VFrameInfo);
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

#if 0
	char yuvSaveDir[256];
	char yuvSavePath[256];
	sprintf(yuvSaveDir, "%s/%s", rsltDir, "LiveScalerYUV");
	if(access(yuvSaveDir, F_OK) == -1)
		mkdir(yuvSaveDir, 0777);
	sprintf(yuvSavePath, "%s/%s_%d.yuv420p", yuvSaveDir, "frameid", VFrameInfo.stVFrame.u32FrameId);
	ofstream yuvfile(yuvSavePath, ios::binary);
	if(yuvfile)
	{
		if(FrameFmt == AR_IMG_YUV420P){
			//Note the width of the visualized YUV420p is yolov5_params.net_w
			AR_CHAR *dumpYUV420pBuf = (AR_CHAR *)malloc(yolov5_params.net_h * yolov5_params.net_w * 3 / 2);
			AR_ConvertDump_YUV420PImg(pstImg, dumpYUV420pBuf);
			yuvfile.write((AR_CHAR *)dumpYUV420pBuf, yolov5_params.net_h * yolov5_params.net_w * 3 / 2);
			free(dumpYUV420pBuf);
		}
		else{
			//Note the width of the visualized YUV420sp NV12 is stIFCParam.u32YStride
			yuvfile.write((AR_CHAR *)stPchbuff.u64VirtAddr, stIFCParam.u32YStride * yolov5_params.net_w * 3 / 2);
		}
		Log(ARCNN_LOG_DEBUG, "Save output to %s finished.\n", yuvSavePath);
	}			
#endif	
	
    return 1;
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

//#define CALLBACK_DATA_LOAD
//For local image test. Need to add RGBD or RGBIntlv format later.
AR_S32 CARCNN::GetLocalImage(AR_CHAR * pchFileName, AR_IMG_S * pstImg, AR_U32 u32InputID)
{
    static AR_U32 u32FrameId = 0;
	AR_IMG_S stRGBSrcImg = {0};
	AR_IMG_S stYUVSrcImg = {0};
	AR_MEM_S stRGBSchbuff = {0};
	AR_MEM_S stYUVSchbuff = {0};
	AR_U16 u16InStride = 0;
	AR_S32 s32InFileSize = 0;
	AR_HAL_SCALER_CROP_S stCrops = {0};
	AR_S32 ret = 0;

    ProfileStart("GetLocalImage");
    u32FileType = GetImageFileType(pchFileName);

	AR_CHAR* pchDstBuffVirt = (AR_CHAR *)stPchbuff.u64VirtAddr;
	
	if(u32FileType == JPG_BMP_PNG_IMG_FILE)
    {
        //read image and resize it, TBD
        cv::Mat src_img = cv::imread(pchFileName, cv::IMREAD_ANYCOLOR);
        if(src_img.empty())
        {
           Log(ARCNN_LOG_ERR, "Imread file %s failed: %s!\r\n", pchFileName, strerror(errno));
           return -1;
        }
#if YOLOv5_POSTPROC
		yolov5_params.img_w = src_img.cols;
		yolov5_params.img_h = src_img.rows;
		yolov5_params.buf.oriImg = src_img;
#endif
		printf("src img w/h/c %d %d %d\r\n",src_img.cols,src_img.rows,src_img.channels());

		int max_src = GET_MAX(yolov5_params.img_w, yolov5_params.img_h);
		yolov5_params.ratio = (float)max_src / (float)yolov5_params.net_w;

		/*******************initializer input buf addr*******************/
		pstImg->u32Width = yolov5_params.net_w;
		pstImg->u32Height = yolov5_params.net_h;
		pstImg->u32FrameId = u32FrameId++;
		pstImg->enFormat= AR_IMG_RGB;
		pstImg->u32ChannelNum = 3;
		pstImg->astChannels[0].u32AddrPhy  = (AR_UINTPTR)stPchbuff.u64PhyAddr;
		pstImg->astChannels[0].uptrAddrVirt = (AR_UINTPTR)pchDstBuffVirt;
		pstImg->astChannels[0].u32Stride = stIFCParam.u32YStride;
		pstImg->astChannels[1].u32AddrPhy = (AR_UINTPTR)(stPchbuff.u64PhyAddr + stIFCParam.u32YStride * yolov5_params.net_h);
		pstImg->astChannels[1].uptrAddrVirt = (AR_UINTPTR)(pchDstBuffVirt + stIFCParam.u32YStride * yolov5_params.net_h);
		pstImg->astChannels[1].u32Stride = stIFCParam.u32YStride;
		pstImg->astChannels[2].u32AddrPhy = (AR_UINTPTR)(stPchbuff.u64PhyAddr + 2 * stIFCParam.u32YStride * yolov5_params.net_h);
		pstImg->astChannels[2].uptrAddrVirt = (AR_UINTPTR)(pchDstBuffVirt + 2 * stIFCParam.u32YStride * yolov5_params.net_h);
		pstImg->astChannels[2].u32Stride = stIFCParam.u32YStride;

		cv::Mat Channels[3];
		cv::split(src_img, Channels);
		AR_CHAR *pchR = (AR_CHAR *)Channels[2].data;
		AR_CHAR *pchG = (AR_CHAR *)Channels[1].data;
		AR_CHAR *pchB = (AR_CHAR *)Channels[0].data;
		/******************************************************************/

		if((yolov5_params.img_w != yolov5_params.net_w) || (yolov5_params.img_h != yolov5_params.net_h))
		{
			/*******************get source align RGB img buff*******************/
			u16InStride = ALIGNED_16B(yolov5_params.img_w);
			s32InFileSize = u16InStride * yolov5_params.img_h * 3;
			stRGBSchbuff.u64Len = s32InFileSize;
			ret = AR_MPI_NPU_MallocBuff((AR_CHAR *)"RGBSchbuff", &stRGBSchbuff);
			if(ret)
			{
			   Log(ARCNN_LOG_ERR, "Malloc ALIGN RGB buf error: size %d.\n", stRGBSchbuff.u64Len);
			   goto exit;
			}
			/********************************************************************/
			/*******************push Original JPG img to ALIGN RGB buf*******************/
			AR_CHAR* pchSrcBuffVirt = (AR_CHAR *)stRGBSchbuff.u64VirtAddr;
			for(int h = 0; h<yolov5_params.img_h; h++)
			{
				memcpy((void *)(pchSrcBuffVirt + h * u16InStride), (void *)(pchR + h*yolov5_params.img_w), yolov5_params.img_w);
				memcpy((void *)(pchSrcBuffVirt + (h+yolov5_params.img_h)*u16InStride), (void *)(pchG + h*yolov5_params.img_w), yolov5_params.img_w);
				memcpy((void *)(pchSrcBuffVirt + (h+yolov5_params.img_h*2)*u16InStride), (void *)(pchB + h*yolov5_params.img_w), yolov5_params.img_w);
			}
			/*****************************************************************************/
			/*******************ratio scale resize img to net input size******************/
			stRGBSrcImg.u32Width = yolov5_params.img_w;
			stRGBSrcImg.u32Height = yolov5_params.img_h;
			stRGBSrcImg.u32FrameId = u32FrameId;
			stRGBSrcImg.enFormat= AR_IMG_RGB;
			stRGBSrcImg.u32ChannelNum = 3;
			stRGBSrcImg.astChannels[0].u32AddrPhy  = (AR_UINTPTR)stRGBSchbuff.u64PhyAddr;
			stRGBSrcImg.astChannels[0].uptrAddrVirt = (AR_UINTPTR)pchSrcBuffVirt;
			stRGBSrcImg.astChannels[0].u32Stride = u16InStride;
			stRGBSrcImg.astChannels[1].u32AddrPhy = (AR_UINTPTR)(stRGBSchbuff.u64PhyAddr + u16InStride * yolov5_params.img_h);
			stRGBSrcImg.astChannels[1].uptrAddrVirt = (AR_UINTPTR)(pchSrcBuffVirt + u16InStride * yolov5_params.img_h);
			stRGBSrcImg.astChannels[1].u32Stride = u16InStride;
			stRGBSrcImg.astChannels[2].u32AddrPhy = (AR_UINTPTR)(stRGBSchbuff.u64PhyAddr + 2 * u16InStride * yolov5_params.img_h);
			stRGBSrcImg.astChannels[2].uptrAddrVirt = (AR_UINTPTR)(pchSrcBuffVirt + 2 * u16InStride * yolov5_params.img_h);
			stRGBSrcImg.astChannels[2].u32Stride = u16InStride;

			stCrops.u32X = 0;
			stCrops.u32Y = 0;
			stCrops.u32W = yolov5_params.img_w;
			stCrops.u32H = yolov5_params.img_h;

	        ret = AR_MPI_SCALER_CropResizeRatio(&stRGBSrcImg, &stCrops, pstImg, 1, 0, 0x11); //up and left
	        if(ret < 0)
	        {
	            Log(ARCNN_LOG_ERR, "AR_MPI_SCALER_CropResizeRatio error: %d.\n", ret);
	            goto exit;
	        }
			/*****************************************************************************/
		}
		else{
			for(int h = 0; h<yolov5_params.img_h; h++)
			{
				memcpy((void *)(pchDstBuffVirt + h * stIFCParam.u32YStride), (void *)(pchR + h*yolov5_params.img_w), yolov5_params.img_w);
				memcpy((void *)(pchDstBuffVirt + (h+yolov5_params.img_h)*stIFCParam.u32YStride), (void *)(pchG + h*yolov5_params.img_w), yolov5_params.img_w);
				memcpy((void *)(pchDstBuffVirt + (h+yolov5_params.img_h*2)*stIFCParam.u32YStride), (void *)(pchB + h*yolov5_params.img_w), yolov5_params.img_w);
			}
		}
#if 0
		char rgbSaveDir[256];
		char rgbSavePath[256];
		sprintf(rgbSaveDir, "%s/%s", rsltDir, "ScalerRGB");
		if(access(rgbSaveDir, F_OK) == -1)
			mkdir(rgbSaveDir, 0777);
		sprintf(rgbSavePath, "%s/%s", rgbSaveDir, (char *)testFileName);
		cv::Mat RGBScalerImg;
		cv::Mat scalerR(yolov5_params.net_h, u16Stride, CV_8UC1, (AR_CHAR*)pstImg->astChannels[0].uptrAddrVirt);
		cv::Mat scalerG(yolov5_params.net_h, u16Stride, CV_8UC1, (AR_CHAR*)pstImg->astChannels[1].uptrAddrVirt);
		cv::Mat scalerB(yolov5_params.net_h, u16Stride, CV_8UC1, (AR_CHAR*)pstImg->astChannels[2].uptrAddrVirt);
		std::vector<cv::Mat> channels{scalerB, scalerG, scalerR};
		cv::merge(channels, RGBScalerImg);
		cv::imwrite(rgbSavePath, RGBScalerImg);
#endif
    }
	else if(u32FileType == YUV420P_FILE || u32FileType == NV12_RAW_DATA_FILE){
		ifstream file(pchFileName, ios::in|ios::binary|ios::ate);
        if(file.is_open())
        {
            //1. load original img file
    	    s32FileSize = file.tellg();
			if(!s32FileSize){
				Log(ARCNN_LOG_ERR, "Input YUV file error: %s. please check it!\n", pchFileName);
				ret = -1;
				goto exit;
			}
    	    file.seekg(0, ios::beg);
    	    AR_CHAR* fileOrgBuff = (AR_CHAR*)malloc(s32FileSize);
            memset(fileOrgBuff, 0,s32FileSize);
    	    file.read((AR_CHAR *)fileOrgBuff, s32FileSize);
    	    file.close();
			string FileNameStr = testFileName;
			GetImageSize(FileNameStr, &yolov5_params.img_w, &yolov5_params.img_h);

			int max_src = GET_MAX(yolov5_params.img_w, yolov5_params.img_h);
			yolov5_params.ratio = (float)max_src / (float)yolov5_params.net_w;

			/*********get source input YUV420P convert to RGB img buff*********/
			
			u16InStride = ALIGNED_256B(yolov5_params.img_w);
			if(AR_MPI_NPU_GetSocVersion() == 2)
				u16InStride = ALIGNED_32B(yolov5_params.img_w);
			s32InFileSize = u16InStride * yolov5_params.img_h * 3;
			stRGBSchbuff.u64Len = s32InFileSize;
			ret = AR_MPI_NPU_MallocBuff((AR_CHAR *)"RGBSchbuff", &stRGBSchbuff);
			if(ret)
			{
			   Log(ARCNN_LOG_ERR, "Malloc YUV2RGB buf error: size %d.\n", stRGBSchbuff.u64Len);
			   goto exit;
			}
			/*******************************************************************/

			/**********************IMG BUF initializer paramters****************/
			pstImg->u32Width = yolov5_params.net_w;
			pstImg->u32Height = yolov5_params.net_h;
			pstImg->u32FrameId = u32FrameId++;
			if(u32FileType == NV12_RAW_DATA_FILE){
				pstImg->u32ChannelNum = 2;
				pstImg->enFormat= AR_IMG_NV12;
			}else{
				pstImg->u32ChannelNum = 3;
				pstImg->enFormat= AR_IMG_YUV420P;
			}
			pstImg->astChannels[0].u32AddrPhy  = (AR_UINTPTR)stPchbuff.u64PhyAddr;
			pstImg->astChannels[0].uptrAddrVirt = (AR_UINTPTR)pchDstBuffVirt;
			pstImg->astChannels[0].u32Stride = stIFCParam.u32YStride;
			pstImg->astChannels[1].u32AddrPhy = (AR_UINTPTR)(stPchbuff.u64PhyAddr + stIFCParam.u32YStride * yolov5_params.net_h);
			pstImg->astChannels[1].uptrAddrVirt = (AR_UINTPTR)(pchDstBuffVirt + stIFCParam.u32YStride * yolov5_params.net_h);
			pstImg->astChannels[1].u32Stride = stIFCParam.u32UStride;
			if(u32FileType == YUV420P_FILE){
				pstImg->astChannels[2].u32AddrPhy = (AR_UINTPTR)(stPchbuff.u64PhyAddr + stIFCParam.u32YStride * yolov5_params.net_h + stIFCParam.u32UStride * yolov5_params.net_h / 2);
				pstImg->astChannels[2].uptrAddrVirt = (AR_UINTPTR)(pchDstBuffVirt + stIFCParam.u32YStride * yolov5_params.net_h + stIFCParam.u32UStride * yolov5_params.net_h / 2);
				pstImg->astChannels[2].u32Stride = stIFCParam.u32VStride;
			}
			
			stRGBSrcImg.u32Width = yolov5_params.img_w;
			stRGBSrcImg.u32Height = yolov5_params.img_h;
			stRGBSrcImg.u32FrameId = u32FrameId;
			stRGBSrcImg.u32ChannelNum = 3;
			stRGBSrcImg.enFormat= AR_IMG_RGB;
			stRGBSrcImg.astChannels[0].u32AddrPhy  = (AR_UINTPTR)stRGBSchbuff.u64PhyAddr;
			stRGBSrcImg.astChannels[0].uptrAddrVirt = (AR_UINTPTR)(stRGBSchbuff.u64VirtAddr);
			stRGBSrcImg.astChannels[0].u32Stride = u16InStride;
			stRGBSrcImg.astChannels[1].u32AddrPhy = (AR_UINTPTR)(stRGBSchbuff.u64PhyAddr + u16InStride * yolov5_params.img_h);
			stRGBSrcImg.astChannels[1].uptrAddrVirt = (AR_UINTPTR)(stRGBSchbuff.u64VirtAddr + u16InStride * yolov5_params.img_h);
			stRGBSrcImg.astChannels[1].u32Stride = u16InStride;
			stRGBSrcImg.astChannels[2].u32AddrPhy = (AR_UINTPTR)(stRGBSchbuff.u64PhyAddr + u16InStride * yolov5_params.img_h * 2);
			stRGBSrcImg.astChannels[2].uptrAddrVirt = (AR_UINTPTR)(stRGBSchbuff.u64VirtAddr + u16InStride * yolov5_params.img_h * 2);
			stRGBSrcImg.astChannels[2].u32Stride = u16InStride;
			/*******************************************************************/

			if((yolov5_params.img_h != yolov5_params.net_h) ||(yolov5_params.img_w != yolov5_params.net_w))
			{
				/*************get source input YUV420P align img buff**************/
				s32InFileSize = u16InStride * yolov5_params.img_h * 3 / 2;
				stYUVSchbuff.u64Len = s32InFileSize;
				ret = AR_MPI_NPU_MallocBuff((AR_CHAR *)"YUVSchbuff", &stYUVSchbuff);
				if(ret)
				{
				   Log(ARCNN_LOG_ERR, "Malloc Original YUV420P sync buf error: size %d.\n", stYUVSchbuff.u64Len);
				   goto exit;
				}
				memset((void *)stPchbuff.u64VirtAddr, 0, stPchbuff.u64Len);
				/*******************************************************************/
				
				/***********************source YUV img align************************/
				if(u32FileType == YUV420P_FILE){
					for(int h=0; h<yolov5_params.img_h; h++)
					{
						memcpy((void *)(stYUVSchbuff.u64VirtAddr + h * u16InStride), (void *)(fileOrgBuff + h * yolov5_params.img_w), yolov5_params.img_w);
						if(h < yolov5_params.img_h/2){
							memcpy((void *)(stYUVSchbuff.u64VirtAddr + u16InStride*yolov5_params.img_h + h*u16InStride/2), 
									(void *)(fileOrgBuff + yolov5_params.img_w*yolov5_params.img_h + h*yolov5_params.img_w/2), yolov5_params.img_w/2);
							memcpy((void *)(stYUVSchbuff.u64VirtAddr + u16InStride*yolov5_params.img_h + u16InStride*yolov5_params.img_h/4 + h*u16InStride/2), 
									(void *)(fileOrgBuff + yolov5_params.img_w*yolov5_params.img_h + yolov5_params.img_w*yolov5_params.img_h/4 
										+ h*yolov5_params.img_w/2), yolov5_params.img_w/2);	
						}
					}
				}
				else{
					for(int h=0; h<yolov5_params.img_h*3/2; h++)
						memcpy((void *)(stYUVSchbuff.u64VirtAddr + h * u16InStride), (void *)(fileOrgBuff + h * yolov5_params.img_w), yolov5_params.img_w);
				}
				/******************************************************************/

				/*************************YUV resize ratio*************************/				
				stYUVSrcImg.u32Width = yolov5_params.img_w;
				stYUVSrcImg.u32Height = yolov5_params.img_h;
				stYUVSrcImg.u32FrameId = u32FrameId;
				if(u32FileType == NV12_RAW_DATA_FILE){
					stYUVSrcImg.u32ChannelNum = 2;
					stYUVSrcImg.enFormat= AR_IMG_NV12;
				}else{
					stYUVSrcImg.u32ChannelNum = 3;
					stYUVSrcImg.enFormat= AR_IMG_YUV420P;
				}
				stYUVSrcImg.astChannels[0].u32AddrPhy  = (AR_UINTPTR)stYUVSchbuff.u64PhyAddr;
				stYUVSrcImg.astChannels[0].uptrAddrVirt = (AR_UINTPTR)stYUVSchbuff.u64VirtAddr;
				stYUVSrcImg.astChannels[0].u32Stride = u16InStride;
				stYUVSrcImg.astChannels[1].u32AddrPhy = (AR_UINTPTR)(stYUVSchbuff.u64PhyAddr + u16InStride * yolov5_params.img_h);
				stYUVSrcImg.astChannels[1].uptrAddrVirt = (AR_UINTPTR)(stYUVSchbuff.u64VirtAddr + u16InStride * yolov5_params.img_h);
				if(u32FileType == NV12_RAW_DATA_FILE){
					stYUVSrcImg.astChannels[1].u32Stride = u16InStride;
				}
				else{
					stYUVSrcImg.astChannels[1].u32Stride = u16InStride / 2;
					stYUVSrcImg.astChannels[2].u32AddrPhy = (AR_UINTPTR)(stYUVSchbuff.u64PhyAddr + u16InStride*yolov5_params.img_h + u16InStride*yolov5_params.img_h/4);
					stYUVSrcImg.astChannels[2].uptrAddrVirt = (AR_UINTPTR)(stYUVSchbuff.u64VirtAddr + u16InStride * yolov5_params.img_h + u16InStride*yolov5_params.img_h/4);
					stYUVSrcImg.astChannels[2].u32Stride = u16InStride / 2;
				}
				stCrops.u32X = 0;
				stCrops.u32Y = 0;
				stCrops.u32W = yolov5_params.img_w;
				stCrops.u32H = yolov5_params.img_h;

	            ret = AR_MPI_SCALER_CropResizeRatio(&stYUVSrcImg, &stCrops, pstImg, 1, 0, 0x11); //up and left
	            if(ret < 0)
	            {
	                Log(ARCNN_LOG_ERR, "AR_MPI_SCALER_CropResizeRatio error: %d.\n", ret);
	                goto exit;
	            }
				/*******************************************************************/

				/************************YUV convert to RGB*************************/
				ret = AR_MPI_IFC_CvtColor(&stYUVSrcImg, &stRGBSrcImg, AR_MPI_IFC_BT601_VIDEO);
				if(ret<0)
				{
					Log(ARCNN_LOG_ERR, "CvtColor Image Error (stYUVSrcImg Convert)\n");
					goto exit;
				}
				/*******************************************************************/

#if 0
				char yuvSaveDir[256];
				char yuvSavePath[256];
				sprintf(yuvSaveDir, "%s/%s", rsltDir, "ScalerYUV");
				if(access(yuvSaveDir, F_OK) == -1)
					mkdir(yuvSaveDir, 0777);
				sprintf(yuvSavePath, "%s/%s", yuvSaveDir, (char *)testFileName);
				ofstream yuvfile(yuvSavePath, ios::binary);
				if(yuvfile)
				{
					if(u32FileType == YUV420P_FILE){
						//Note the width of the visualized YUV420p is yolov5_params.net_w
						AR_CHAR *dumpYUV420pBuf = (AR_CHAR *)malloc(yolov5_params.net_h * yolov5_params.net_w * 3 / 2);
						AR_ConvertDump_YUV420PImg(pstImg, dumpYUV420pBuf);
						yuvfile.write((AR_CHAR *)dumpYUV420pBuf, yolov5_params.net_h * yolov5_params.net_w * 3 / 2);
						free(dumpYUV420pBuf);
					}
					else{
						//Note the width of the visualized YUV420sp NV12 is stIFCParam.u32YStride
						yuvfile.write((AR_CHAR *)pchDstBuffVirt, stIFCParam.u32YStride * yolov5_params.net_w * 3 / 2);
					}
					Log(ARCNN_LOG_DEBUG, "Save output to %s finished.\n", yuvSavePath);
				}			
#endif
			}
			else{
				/***********************copy src YUV to stPchbuff************************/
				if(u32FileType == YUV420P_FILE){
					for(int h=0; h<yolov5_params.img_h; h++)
					{
						memcpy((void *)(stPchbuff.u64VirtAddr + h * stIFCParam.u32YStride), (void *)(fileOrgBuff + h * yolov5_params.img_w), yolov5_params.img_w);
						if(h < yolov5_params.img_h/2){
							memcpy((void *)(stPchbuff.u64VirtAddr + stIFCParam.u32YStride*yolov5_params.img_h + h*stIFCParam.u32UStride), 
									(void *)(fileOrgBuff + yolov5_params.img_w*yolov5_params.img_h + h*yolov5_params.img_w/2), yolov5_params.img_w/2);
							memcpy((void *)(stPchbuff.u64VirtAddr + stIFCParam.u32YStride*yolov5_params.img_h + stIFCParam.u32UStride*yolov5_params.img_h/2 + h*stIFCParam.u32VStride), 
									(void *)(fileOrgBuff + yolov5_params.img_w*yolov5_params.img_h + yolov5_params.img_w*yolov5_params.img_h/4 
										+ h*yolov5_params.img_w/2), yolov5_params.img_w/2);	
						}
					}
				}
				else{
					for(int h=0; h<yolov5_params.img_h*3/2; h++)
						memcpy((void *)(stPchbuff.u64VirtAddr + h * stIFCParam.u32YStride), (void *)(fileOrgBuff + h * yolov5_params.img_w), yolov5_params.img_w);
				}
				/******************************************************************/
				
				/************************YUV convert to RGB*************************/
				ret = AR_MPI_IFC_CvtColor(pstImg, &stRGBSrcImg, AR_MPI_IFC_BT601_VIDEO);
				if(ret<0)
				{
					Log(ARCNN_LOG_ERR, "CvtColor Image Error (pstImg Convert)\n");
					goto exit;
				}
				/*******************************************************************/
			}
			
			cv::Mat RGBImg;
			cv::Mat MergedImg;
			cv::Mat R(yolov5_params.img_h, u16InStride, CV_8UC1, (AR_CHAR*)stRGBSrcImg.astChannels[0].uptrAddrVirt);
			cv::Mat G(yolov5_params.img_h, u16InStride, CV_8UC1, (AR_CHAR*)stRGBSrcImg.astChannels[1].uptrAddrVirt);
			cv::Mat B(yolov5_params.img_h, u16InStride, CV_8UC1, (AR_CHAR*)stRGBSrcImg.astChannels[2].uptrAddrVirt);
			std::vector<cv::Mat> channels{B, G, R};
			cv::merge(channels, MergedImg);			
			RGBImg = MergedImg(cv::Rect(0, 0, yolov5_params.img_w, yolov5_params.img_h));
			yolov5_params.buf.oriImg = RGBImg;	
		
			free(fileOrgBuff);
        }
	}
    else
    {
    	Log(ARCNN_LOG_ERR, "Unsupported image format! (%s)\n", pchFileName);
        goto exit;
    }	
exit:
	if(stRGBSchbuff.u64Len > 0)
		AR_MPI_NPU_FreeBuff(&stRGBSchbuff);
	if(stYUVSchbuff.u64Len > 0)
		AR_MPI_NPU_FreeBuff(&stYUVSchbuff);
	if(ret < 0)
		return ret;
    Log(ARCNN_LOG_DEBUG, "Get image %s..\n", pchFileName);
    return 1;
}

AR_S32 CARCNN::Forward(AR_IMG_SET_S * pstImg, AR_BOOL bInstant, AR_BOOL bDebug)
{
    //ProfileStart("Forward");
    AR_S32 s32Ret = AR_MPI_NPU_Forward(handle, pstImg, &stNPUInBuff, &stNPUOutBuff, bInstant, bDebug);
    //ProfileEnd("Forward");
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

		Log(ARCNN_LOG_INFO, "NPUFWPre: %d x 0.01ms  NPUFW: %d x 0.01ms  NPUCBTime: %d x 0.01ms \r\n",u32FwPreTime,u32FwTime, u32CBTime);
    }
    return s32Ret;
}

//here, we pass the input image name to generate a output file.
AR_S32 CARCNN::PostProcess()
{
	AR_S32 ret = 1;
    AR_UINTPTR outputBuff = stNPUOutBuff.u64VirtAddr;
	//2022-04-11, Need to invalid cache before read output buffer because we malloced a cachable buffer.
    AR_MPI_NPU_InvalidCachedBuff(&stNPUOutBuff);
	
#if YOLOv5_POSTPROC
	ProfileStart("YOLOv5 PostProcess");
	int obj_num = yolov5_postprocess((char *)outputBuff, handle, yolov5_params, detRslts);
	//Only support float NCHW output!
	ProfileEnd("YOLOv5 PostProcess");
	int *colors = yolov5_params.buf.colors;
	int *colorAddr = colors;
	if(u32Mode == ARCNN_INPUT_LOCAL){
		if(access(rsltDir, F_OK) == -1)
			mkdir(rsltDir, 0777);
		char save_path[256];
		char save_dir[256];
		sprintf(save_dir, "%s/%s", rsltDir, "txtBoxes");
		if(access(save_dir, F_OK) == -1)
			mkdir(save_dir, 0777);
		sprintf(save_path, "%s/%s%s", save_dir, (char *)testFileName, ".txt");
		FILE *fp;
		fp = fopen(save_path, "w");
		cv::Mat img = yolov5_params.buf.oriImg;
		char titlech[100] = {0};
		for(int k = 0; k < obj_num; k++){
			fprintf(fp, "%d, %f, %f, %f, %f, %f\n", detRslts[k].cls_id, detRslts[k].score, detRslts[k].xmin, detRslts[k].ymin, detRslts[k].xmax, detRslts[k].ymax);
			colorAddr = colors + 3 * detRslts[k].cls_id;
			cv::rectangle(img, cvPoint((int)detRslts[k].xmin, (int)detRslts[k].ymin), cvPoint((int)detRslts[k].xmax, (int)detRslts[k].ymax), 
				Scalar(colorAddr[0], colorAddr[1], colorAddr[2]), 2);
			cv::rectangle(img, cvPoint((int)detRslts[k].xmin, (int)detRslts[k].ymin - 30), cvPoint((int)detRslts[k].xmin + 80, (int)detRslts[k].ymin),
				Scalar(colorAddr[0], colorAddr[1], colorAddr[2]), -1);
			sprintf(titlech, "%d--%.2f", detRslts[k].cls_id, detRslts[k].score);
			string titlestr = titlech;
			cv::putText(img, titlestr, cvPoint((int)detRslts[k].xmin, (int)detRslts[k].ymin - 8), FONT_HERSHEY_COMPLEX, 0.5, Scalar(0, 0, 0), 2);
		}	
		char imgSaveDir[256];
		char imgSavePath[256];
		sprintf(imgSaveDir, "%s/%s", rsltDir, "visBoxes");
		if(access(imgSaveDir, F_OK) == -1)
			mkdir(imgSaveDir, 0777);
		if(u32FileType == YUV420P_FILE || u32FileType == NV12_RAW_DATA_FILE)
			sprintf(imgSavePath, "%s/%s%s", imgSaveDir, (char *)testFileName, ".jpg");
		else
			sprintf(imgSavePath, "%s/%s", imgSaveDir, (char *)testFileName);
		cv::imwrite(imgSavePath, img);
		fclose(fp);
	}
	else if(u32Mode == ARCNN_INPUT_LIVE){
		AR_U32 *OSDColors = (AR_U32 *)yolov5_params.buf.livecolors;
		IPC_CTRL_OSD_S stOsd = {0};
		stOsd.u32VpssGrp = VpssGrp;
		stOsd.u32StrNum = obj_num;
		//printf("obj_num = %d \n", obj_num);
		for(AR_U32 strID=0; strID < stOsd.u32StrNum; strID++)
		{
			//printf("00 : %d, %.3f, %.3f, %.3f\n", detRslts[strID].cls_id, detRslts[strID].score, detRslts[strID].xmin, detRslts[strID].ymin);
		   	char scores[32]={0};
		   	sprintf(scores, "%d:%.2f", detRslts[strID].cls_id, detRslts[strID].score);
		   	//sprintf(scores, "%d:%.4f", detRslts[strID].cls_id, detRslts[strID].score);
		   	strcpy(stOsd.stOsdStr[strID].achStr, scores);
		   	stOsd.stOsdStr[strID].u32X = detRslts[strID].xmin;
		   	stOsd.stOsdStr[strID].u32Y = detRslts[strID].ymin;		   
		   	stOsd.stOsdStr[strID].u32RGB = OSDColors[detRslts[strID].cls_id];
			//printf("object cls id = %d, livecolor = %x\n", detRslts[strID].cls_id, OSDColors[detRslts[strID].cls_id]);
		}
		stOsd.u32RectNum= obj_num;
		for(AR_U32 recID=0; recID < stOsd.u32RectNum; recID++)
		{
			//printf("01 : %.3f, %.3f, %.3f, %.3f \n", detRslts[recID].xmin, detRslts[recID].ymin, detRslts[recID].xmax, detRslts[recID].ymax);
		   	stOsd.stOsdRect[recID].u32X = detRslts[recID].xmin;
		   	stOsd.stOsdRect[recID].u32Y = detRslts[recID].ymin;
		   	stOsd.stOsdRect[recID].u32W = detRslts[recID].xmax - detRslts[recID].xmin;
		   	stOsd.stOsdRect[recID].u32H = detRslts[recID].ymax - detRslts[recID].ymin;
		   	stOsd.stOsdRect[recID].u32RGB = OSDColors[detRslts[recID].cls_id];
			//printf("object cls id = %d, livecolor = 0x%x\n", detRslts[recID].cls_id, OSDColors[detRslts[recID].cls_id]);
		}
		stOsd.u32ImgHeight = yolov5_params.img_h;
		stOsd.u32ImgWidth  = yolov5_params.img_w;

		BINDER_RET_S stBinderRet = {0};
		ret = ar_ipc_send_buffer(pstIpcClient, u32ClientHandle, IPC_RPC_CMD_DRAW_VPSS_OSD, &stOsd, sizeof(stOsd), &stBinderRet, 0);
		if(ret<0)
		{
		   Log(ARCNN_LOG_ERR, "draw osd err ret=%d\r\n",ret);
		   return -1;
		}
	}
	else{
		Log(ARCNN_LOG_ERR, "Unsupported input u32Mode!\n");
		return -1;
	}
#endif
    memset((void*)outputBuff,0,stNPUOutBuff.u64Len);
	return ret;
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

AR_S32 CARCNN::RegisterCallbackFunc(AR_CHAR * chName, AR_NPU_CallbackFunc pCBFunc)
{
    return AR_MPI_NPU_RegisterCallback(handle, chName, pCBFunc);
}

AR_S32 CARCNN::RegisterParseFunc(AR_CHAR * chName, AR_NPU_ParseFunc pParseFunc)
{
	return AR_MPI_NPU_RegisterParseFunc(handle, chName, pParseFunc);
}

AR_U32 CARCNN::IsInitialized()
{
    return u32Initialized;
}

AR_S32 CARCNN::MallocNPUBuff()
{
    AR_U32 u32Size = 0;
    AR_S32 s32Ret = 0;
	//AR_BOOL bEnable = AR_TRUE;

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

