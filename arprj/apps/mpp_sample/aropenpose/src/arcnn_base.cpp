//this is the base class
#include <iostream>
#include <fstream>
#include <string>
#include <sys/time.h>
#include <stdarg.h>
#include <unistd.h>
#include "cJSON.h"
//#include "ar_img_api.h"
//#include "hal_sys.h"
#include "opencv2/opencv.hpp"

#include "mpi_scaler_api.h"

#include "mpi_npu_api.h"
#include "mpi_ifc_api.h"
extern "C" {
#include "osal.h"
}


using namespace std;
using namespace cv;

#include "arcnn_base.h"
#include "arcnn_common.h"
#include "arcnn_process.h"

//#define USE_ALGAPI 0

void SaveRGBImg(AR_CHAR * pR, AR_CHAR * pG, AR_CHAR * pB, AR_U32 u32W, AR_U32 u32H, AR_CHAR * pName)
{
    cv::Mat img;
    cv::Mat R(u32H, u32W, CV_8UC1, pR);
    cv::Mat G(u32H, u32W, CV_8UC1, pG);
    cv::Mat B(u32H, u32W, CV_8UC1, pB);

    std::vector<cv::Mat> channels{B, G, R};

    cv::merge(channels, img);

    cv::imwrite(pName, img);
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
    u32Mode = 0;
    strLocalDir.clear();
    s32VpssGrp = 0;
    s32VpssChn = 0;
    s32OsdChn = 0;

    u32ImageWidth = 0;
    u32ImageHeight = 0;
    u32InputWidth =0;
    u32InputHeight = 0;
    
    pchLocalImage = NULL;
    u32ProfileEnable = 0;
    cbFunc = NULL;

    pstIpcClient = NULL;
    u32ClientHandle = 0;

    u32DrawOSD = 0;

    bContinued = false;
    
    pendingImage = NULL;
    currentImage = NULL;
    pstSwapImage = NULL;

    memset(astProfile, 0, sizeof(astProfile));

    if(!pchCfgFile)
    {
        Log(ARCNN_LOG_ERR, "Invalid json file name.\n");
        return;
    }

    ifstream cfgFile(pchCfgFile, ios::in|ios::binary|ios::ate);
    Log(ARCNN_LOG_INFO, "Open json file %s.\n", pchCfgFile);
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
        //set default id
        stCNNDesc.u16NetworkID = 0xDEFA;
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

		cJSON * LiveVpssGrp = cJSON_GetObjectItemCaseSensitive(pNet, "vpss_grp");
		if(LiveVpssGrp)
		{
            if(LiveVpssGrp->type & cJSON_Number){
                s32VpssGrp = LiveVpssGrp->valueint;
            }else if(LiveVpssGrp->type & cJSON_String){
                s32VpssGrp = atoi(LiveVpssGrp->valuestring);
            }
		}

		cJSON * LiveVpssChn = cJSON_GetObjectItemCaseSensitive(pNet, "vpss_chn");
		if(LiveVpssChn)
		{
            if(LiveVpssChn->type & cJSON_Number){
                s32VpssChn = LiveVpssChn->valueint;
            }else if(LiveVpssChn->type & cJSON_String){
                s32VpssChn = atoi(LiveVpssChn->valuestring);
            }
		}

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
        cJSON* osdname = cJSON_GetObjectItemCaseSensitive(pNet, "osdname");
        if(osdname){
            strOsdName = osdname->valuestring;
        }else{
            u32DrawOSD = 0;
        }
		cJSON* osdchn = cJSON_GetObjectItemCaseSensitive(pNet, "osd_chn");
        if(osdchn){
            if(osdchn->type & cJSON_Number){
                s32OsdChn = osdchn->valueint;
            }else if(osdchn->type & cJSON_String){
                s32OsdChn = atoi(osdchn->valuestring);
            }
        }else{
            s32OsdChn = 0;
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


	cJSON * imgwidth = cJSON_GetObjectItemCaseSensitive(pNet, "img_width");
	if(!imgwidth)
    {
	   Log(ARCNN_LOG_ERR, "Parse img_width node failed. SET DEFAULT 0\n");
	   u32ImageWidth = 0;
    }
	else
    {
        u32ImageWidth = atoi(imgwidth->valuestring);
    }

	cJSON * imgheight = cJSON_GetObjectItemCaseSensitive(pNet, "img_height");
	if(!imgheight)
	{
	   Log(ARCNN_LOG_ERR, "Parse img_height node failed.SET DEFAULT 0\n");
       u32ImageHeight =0;
	}
	else
	{
		u32ImageHeight= atoi(imgheight->valuestring);
	}

	cJSON *inputw = cJSON_GetObjectItemCaseSensitive(pNet, "input_width");
	if(!inputw)
	{
		Log(ARCNN_LOG_ERR, "Parse input_width node failed.SET DEFAULT 0\n");
		u32InputWidth = 0;
	}
	else
	{
		u32InputWidth = atoi(inputw->valuestring);
	}
	
	cJSON *inputh = cJSON_GetObjectItemCaseSensitive(pNet, "input_height");
	if(!inputh)
	{
		Log(ARCNN_LOG_ERR, "Parse input_height node failed.SET DEFAULT 0\n");
		u32InputHeight = 0;
	}
	else
	{
		u32InputHeight = atoi(inputh->valuestring);
	}


    Log(ARCNN_LOG_INFO, "Input Resolution %dx%d, Image Resolution %dx%d\n",u32InputWidth, u32InputHeight, AR_U32(u32ImageWidth), AR_U32(u32ImageHeight));

    delete[] pchBuff;
    cJSON_Delete(pJson);

    //Loadmodel
    handle = AR_MPI_NPU_LoadModel(&stCNNDesc);
    Log(ARCNN_LOG_INFO, "Load Model %p.\n",handle);
    if(!handle)
    {
        Log(ARCNN_LOG_ERR, "Load Model failed.\n");
    }

    if(u32DrawOSD){
		pstIpcClient = ar_ipc_create();
		if(!pstIpcClient){
			printf("failed to create client ipc\r\n");
		}else{
            u32ClientHandle = ar_ipc_get_service(pstIpcClient, "ipc_ctrl");
            if(!u32ClientHandle){
                ar_ipc_close(pstIpcClient);
                pstIpcClient = NULL;
                printf("failed to get service BINDER_IPCTOOL_NAME \r\n");
            }
        }
        Log(ARCNN_LOG_INFO, "Create osd binder success\n",handle);
    }
    u32Initialized = 1;
}

CARCNN::~CARCNN()
{
    if(u32ClientHandle && pstIpcClient){
        ar_ipc_close_service(pstIpcClient, u32ClientHandle);
        ar_ipc_close(pstIpcClient);
        pstIpcClient = NULL;
        u32ClientHandle = 0;
    }

    if(handle)
    {
        Log(ARCNN_LOG_INFO, "Unload Model %p.\n",handle);
        AR_MPI_NPU_UnloadModel(handle);
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


AR_S32 CARCNN::BindVideoDev()
{
    bContinued = true;
    readerThread = std::thread(&CARCNN::ReadImageThread, this);
    displayThread = std::thread(&CARCNN::DrawOutputThread, this);
    return 0;
}

AR_S32 CARCNN::GetLiveImage(AR_IMG_S * pstImg)
{
    int trying = 100;
    do{
        imageMutex.lock();
        if(pendingImage != NULL){
            *pstImg = *pendingImage;
            currentImage = pendingImage;
            pendingImage = NULL;
            imageMutex.unlock();
            //Log(ARCNN_LOG_ERR, "Get Live Image");
            return 0;
        }else{
            imageMutex.unlock();
            usleep(20*1000);
        }
    }while(trying-- > 0);
    return -1;
}


AR_S32 CARCNN::ReleaseLiveImage(AR_IMG_S * pstImg)
{
    imageMutex.lock();
    if(currentImage != NULL){
        imageQueue.push(currentImage);
        currentImage = NULL;
    }
    imageMutex.unlock();
    return 0;
}

AR_S32 CARCNN::GetLiveImageImpl(AR_IMG_S * pstImg)
{
    ProfileStart("GetLiveImage");
    AR_IMG_S stSrcImg;
    memset(&stSrcImg, 0, sizeof(stSrcImg));

    AR_S32 s32Ret;
    VIDEO_FRAME_INFO_S videoFrame;
    memset(&videoFrame, 0, sizeof(videoFrame));
    s32Ret = AR_MPI_VPSS_GetChnFrame(s32VpssGrp, s32VpssChn, &videoFrame, -1);
    if(s32Ret < 0){
        Log(ARCNN_LOG_ERR, "GetChnFrame(%d,%d) Error 0x%x\n", s32VpssGrp, s32VpssChn, s32Ret);
        return -1;
    }

    stSrcImg.u32FrameId = videoFrame.stVFrame.u32FrameFlag;
    stSrcImg.enFormat= AR_IMG_YUV420P;
    stSrcImg.u32Width = videoFrame.stVFrame.u32Width;
    stSrcImg.u32Height = videoFrame.stVFrame.u32Height;
    stSrcImg.u32ChannelNum = 3;
    stSrcImg.astChannels[0].u32AddrPhy  = (AR_U32)videoFrame.stVFrame.u64PhyAddr[0];
    stSrcImg.astChannels[0].uptrAddrVirt = 0;
    stSrcImg.astChannels[0].u32Stride = videoFrame.stVFrame.u32Stride[0];

    stSrcImg.astChannels[1].u32AddrPhy  = (AR_U32)videoFrame.stVFrame.u64PhyAddr[1];
    stSrcImg.astChannels[1].uptrAddrVirt = 0;
    stSrcImg.astChannels[1].u32Stride = videoFrame.stVFrame.u32Stride[1];

    stSrcImg.astChannels[2].u32AddrPhy  = (AR_U32)videoFrame.stVFrame.u64PhyAddr[2];
    stSrcImg.astChannels[2].uptrAddrVirt = 0;
    stSrcImg.astChannels[2].u32Stride = videoFrame.stVFrame.u32Stride[2];

    u32ImageWidth = stSrcImg.u32Width;
    u32ImageHeight = stSrcImg.u32Height;

	
    GetInputScale(u32ImageWidth, u32ImageHeight, u32InputWidth, u32InputHeight, f32InputScale, u32ScaleWidth, u32ScaleHeight);

	static int tmp0=0;
	if(tmp0 == 0)
	{
		tmp0++;
    	Log(ARCNN_LOG_INFO, "Image Size %dx%d, Input Size %dx%d, Scale %f Scale Size %dx%d\n",
        	AR_U32(u32ImageWidth), AR_U32(u32ImageHeight), u32InputWidth, u32InputHeight, f32InputScale, u32ScaleWidth, u32ScaleHeight);
	}
	
    if(pstSwapImage == NULL)
	{
        AR_CHAR *mmzBuffVirt[3] = {NULL};
        AR_U64 mmzBuffPhy[3] = {0};
        AR_U32 u32Stride;
        u32Stride = ALIGNED_256B(u32InputWidth);
		if(AR_MPI_NPU_GetSocVersion() == 2)		//1:AR9341  2:AR9311
		{
			u32Stride = ALIGNED_32B(u32InputWidth);
		}
		
        pstSwapImage = new AR_IMG_S;
        memset(pstSwapImage, 0, sizeof(AR_IMG_S));
		
        AR_S32 s32FileSize = u32Stride * u32InputHeight;

        GetMMZMemory(&mmzBuffPhy[0], (void**)&mmzBuffVirt[0], "Swp0buff", s32FileSize);
        GetMMZMemory(&mmzBuffPhy[1], (void**)&mmzBuffVirt[1], "Swp1buff", s32FileSize);
        GetMMZMemory(&mmzBuffPhy[2], (void**)&mmzBuffVirt[2], "Swp2buff", s32FileSize);
		
        if((mmzBuffPhy[0] == 0) || (mmzBuffPhy[1] == 0) || (mmzBuffPhy[2] == 0)){
            Log(ARCNN_LOG_ERR, "Malloc MMZ failed.\n");
            delete pstSwapImage;
            pstSwapImage = NULL;
            return -1;
        }

        pstSwapImage->astChannels[0].u32AddrPhy  = mmzBuffPhy[0];
        pstSwapImage->astChannels[0].uptrAddrVirt = (AR_UINTPTR)mmzBuffVirt[0];
        pstSwapImage->astChannels[0].u32Stride = u32Stride;

        pstSwapImage->astChannels[1].u32AddrPhy = mmzBuffPhy[1];
        pstSwapImage->astChannels[1].uptrAddrVirt = (AR_UINTPTR)mmzBuffVirt[1];
        pstSwapImage->astChannels[1].u32Stride = u32Stride/2;

        pstSwapImage->astChannels[2].u32AddrPhy = mmzBuffPhy[2];
        pstSwapImage->astChannels[2].uptrAddrVirt = (AR_UINTPTR)mmzBuffVirt[2];;
        pstSwapImage->astChannels[2].u32Stride = u32Stride/2;
    }

    pstSwapImage->u32FrameId = 1;//stSrcImg.u32FrameId;
    pstSwapImage->enFormat = stSrcImg.enFormat;
    pstSwapImage->u32Width = u32InputWidth;
    pstSwapImage->u32Height = u32InputHeight;
    pstSwapImage->u32ChannelNum = stSrcImg.u32ChannelNum;

    AR_HAL_SCALER_CROP_S stCropParm;
    stCropParm.u32X = 0;
    stCropParm.u32Y = 0;
    stCropParm.u32W = u32ImageWidth;
    stCropParm.u32H = u32ImageHeight;

	static int tmp=0;
	if(tmp == 0)
	{
		tmp++;
		Log(ARCNN_LOG_INFO, "Input Id %d Image %dx%d, Crop %dx%d-%dx%d, Output %dx%d \n", stSrcImg.u32FrameId,
			 stSrcImg.u32Width, stSrcImg.u32Height, stCropParm.u32X, stCropParm.u32Y, stCropParm.u32W, stCropParm.u32H,
			 pstSwapImage->u32Width, pstSwapImage->u32Height);
	};

	
    s32Ret = AR_MPI_SCALER_CropResizeRatio(&stSrcImg, &stCropParm, pstSwapImage, 1, SCALER_MODE_BILINEAR, AR_IMG_ALIGN_TO_UP);
    if(s32Ret < 0){
        Log(ARCNN_LOG_ERR, "CropResizeRatio Image Error \n");
        return -1;
    }
	
	*pstImg = *pstSwapImage;
	AR_MPI_VPSS_ReleaseChnFrame(s32VpssGrp, s32VpssChn, &videoFrame);
    return 0;
}


AR_S32 CARCNN::ReleaseLvieImageImpl(AR_IMG_S * pstImg)
{
    //ProfileStart("ReleaseLiveImage");
    //ProfileEnd("ReleaseLiveImage");
    return 0;
}


AR_S32 CARCNN::DeBindVideoDev()
{
    bContinued = false;
    Log(ARCNN_LOG_INFO, "Join reader thread");
    if(readerThread.joinable()){
        readerThread.join();
    }
    Log(ARCNN_LOG_INFO, "Join display thread");
    if(displayThread.joinable()){
        displayThread.join();
    }
    Log(ARCNN_LOG_INFO, "Join thread finish");

    if(pstSwapImage){
        ReleaseMMZMemory((AR_U64)pstSwapImage->astChannels[0].u32AddrPhy, (void *)pstSwapImage->astChannels[0].uptrAddrVirt);
        ReleaseMMZMemory((AR_U64)pstSwapImage->astChannels[1].u32AddrPhy, (void *)pstSwapImage->astChannels[1].uptrAddrVirt);
        ReleaseMMZMemory((AR_U64)pstSwapImage->astChannels[2].u32AddrPhy, (void *)pstSwapImage->astChannels[2].uptrAddrVirt);
        delete pstSwapImage;
        pstSwapImage = NULL;
    }
    if(pendingImage){
        ReleaseMMZMemory((AR_U64)pendingImage->astChannels[0].u32AddrPhy, (void *)pendingImage->astChannels[0].uptrAddrVirt);
        ReleaseMMZMemory((AR_U64)pendingImage->astChannels[1].u32AddrPhy, (void *)pendingImage->astChannels[1].uptrAddrVirt);
        ReleaseMMZMemory((AR_U64)pendingImage->astChannels[2].u32AddrPhy, (void *)pendingImage->astChannels[2].uptrAddrVirt);
        delete pendingImage;
        pendingImage = NULL;
    }

    while(!imageQueue.empty()){
        AR_IMG_S* pstImage = imageQueue.front();
        imageQueue.pop();
        ReleaseMMZMemory((AR_U64)pstImage->astChannels[0].u32AddrPhy, (void *)pstImage->astChannels[0].uptrAddrVirt);
        ReleaseMMZMemory((AR_U64)pstImage->astChannels[1].u32AddrPhy, (void *)pstImage->astChannels[1].uptrAddrVirt);
        ReleaseMMZMemory((AR_U64)pstImage->astChannels[2].u32AddrPhy, (void *)pstImage->astChannels[2].uptrAddrVirt);
        delete pstImage;
    }
	
    return 0;
}

string CARCNN::GetLocalTestDir()
{
    return strLocalDir;
}


AR_S32 CARCNN::FillInputBuff(AR_CHAR * pchFileName, AR_U32 u32IFCInputId, AR_U32 u32IFCBatchId)
{
    AR_U32 u32FileType = 0;
    AR_S32 s32FileSize = 0;
    AR_UINTPTR pcInputBufferVirt = stNPUInBuff.u64VirtAddr;
    AR_U32 pcInputBufferPhy = stNPUInBuff.u64PhyAddr;
    AR_U32 u32InputSize = 0;
    int i = 0;

    ProfileStart("FillInputBuff");
    u32FileType = GetImageFileType(pchFileName);
    if(u32FileType == RGB_RAW_DATA_FILE || u32FileType == BGR_RAW_DATA_FILE || u32FileType == RGBD_RAW_DATA_FILE)
    {
        ifstream file(pchFileName, ios::in|ios::binary|ios::ate);
        if(file.is_open())
        {
            s32FileSize = file.tellg();
            file.seekg(0, ios::beg);
            //Input offset
			AR_NPU_TENSOR_S stTensor ={0};
			AR_MPI_NPU_GetInputTensorParam(handle, u32IFCInputId, &stTensor);
			AR_U32 u32IFCBatchTensorStep = 0;
			u32IFCBatchTensorStep = AR_MPI_NPU_GetBatchTensorStep(handle, u32IFCInputId);
			AR_CHAR *InputOffsetAddr = NULL;
			InputOffsetAddr = (AR_CHAR *)pcInputBufferVirt +
				(stTensor.u32Bank*0x2000000 + stTensor.u32Offset) +
				(u32IFCBatchId * u32IFCBatchTensorStep);

            file.read((AR_CHAR *)InputOffsetAddr, s32FileSize);
            file.close();
        }
    }
    else
    {
        Log(ARCNN_LOG_ERR, "Invalid file type.\n");
        return -1;
    }

    ProfileEnd("FillInputBuff");
    Log(ARCNN_LOG_INFO, "Get data %s[%d] to input %d..\n", pchFileName, s32FileSize, i);
	
    return 0;
}

#define CALLBACK_DATA_LOAD
//For local image test. 
AR_S32 CARCNN::GetLocalImage(AR_CHAR * pchFileName, AR_IMG_S * pstImg, AR_U32 u32InputId)
{
    AR_S32 s32Ret;
    AR_U32 u32FileType = 0;
	AR_S32 s32SrcSize = 0;
	AR_S32 s32DstSize = 0;
    AR_S32 s32FileSize = 0;
    void * pchMMZBuffVirt = NULL;
    AR_U64 pchMMZBuffPhy = 0;
    static AR_U32 u32FrameId = 0;
    AR_UINTPTR pcInputBufferVirt = stNPUInBuff.u64VirtAddr;
    AR_U32 pcInputBufferPhy = stNPUInBuff.u64PhyAddr;
    AR_U32 u32InputSize = 0;

    ProfileStart("GetLocalImage");
    u32FileType = GetImageFileType(pchFileName);
    strCurrentImage = pchFileName;
    if(u32FileType == RGB_RAW_DATA_FILE || u32FileType == BGR_RAW_DATA_FILE || u32FileType == RGBD_RAW_DATA_FILE)
    {
        //AR_IMG_S stSrcImg, stDstImg;
        ifstream file(pchFileName, ios::in|ios::binary|ios::ate);
        if(file.is_open())
        {
            s32FileSize = file.tellg();
            file.seekg(0, ios::beg);

            GetMMZMemory(&pchMMZBuffPhy, &pchMMZBuffVirt, "Pchbuff", s32FileSize);
            if((pchMMZBuffPhy == 0) || !pchMMZBuffVirt)
            {
                Log(ARCNN_LOG_ERR, "Malloc MMZ failed.\n");
                return -1;
            }

            file.read((AR_CHAR *)pchMMZBuffVirt, s32FileSize);
            file.close();
            pstImg->u32FrameId = u32FrameId++;

            if(u32FileType == RGBD_RAW_DATA_FILE)
            {
                pstImg->enFormat = AR_IMG_RGBD; //Indicates that there's D data
            }
            else
            {
                pstImg->enFormat = AR_IMG_RGB;
            }
            pstImg->u32ChannelNum = 1;
            //No need to set w/h/s since ifc.json will set these parameters.
            pstImg->astChannels[0].u32AddrPhy = (AR_UINTPTR)pchMMZBuffPhy;
            pstImg->astChannels[0].uptrAddrVirt = (AR_UINTPTR)pchMMZBuffVirt;

       		#ifdef CALLBACK_DATA_LOAD
           //load the callback data to ddr before any real image is loaded.
           Log(ARCNN_LOG_INFO, "Load callback data.\n");
           ifstream cbfile(strLocalDir + "/input_callback.bin");
           cbfile.read((char *)pcInputBufferVirt, u32InputSize);
           cbfile.close();
       		#endif
        }
    }
	
    else if(u32FileType == JPG_BMP_PNG_IMG_FILE)
    {
        AR_U32 u32ScaleWidth;
		AR_U32 u32ScaleHeight;
        AR_IMG_S stSrcImg;
		AR_IMG_S stDstImg;
        memset(&stSrcImg, 0, sizeof(stSrcImg));
        memset(&stDstImg, 0, sizeof(stDstImg));

        AR_CHAR *mmzBuffVirt[3] = {NULL};
        AR_U64 mmzBuffPhy[3] = {0};

        //read image and resize it, TBD
        cv::Mat src_img = cv::imread(pchFileName, cv::IMREAD_ANYCOLOR);
        if(src_img.empty())
        {
           Log(ARCNN_LOG_ERR, "Imread file %s failed!\r\n", pchFileName);
           return -1;
        }
		
        u32ImageWidth = src_img.cols;
        u32ImageHeight = src_img.rows;
        GetInputScale(u32ImageWidth, u32ImageHeight, u32InputWidth, u32InputHeight, f32InputScale, u32ScaleWidth, u32ScaleHeight);
        //Log(ARCNN_LOG_INFO, "\nImage Size %dx%d, Input Size %dx%d, Scale %f Scale Size %dx%d",
        //    AR_U32(u32ImageWidth), AR_U32(u32ImageHeight), u32InputWidth, u32InputHeight, f32InputScale, u32ScaleWidth, u32ScaleHeight);

		AR_U32 u32SrcStride = ALIGNED_256B(src_img.cols);
        AR_U16 u32DstStride = ALIGNED_256B(u32InputWidth);
		if(AR_MPI_NPU_GetSocVersion() == 2)		//1:AR9341  2:AR9311
		{
			u32SrcStride = ALIGNED_32B(src_img.cols);
        	u32DstStride = ALIGNED_32B(u32InputWidth);
		}
        Log(ARCNN_LOG_INFO, "Src Stride %d, Dst Stride %d",u32SrcStride, u32DstStride);
        s32FileSize = u32SrcStride * (AR_U32)(u32ImageHeight);

	    GetMMZMemory(&mmzBuffPhy[0], (void**)&mmzBuffVirt[0], "Pch0buff", s32FileSize);
        GetMMZMemory(&mmzBuffPhy[1], (void**)&mmzBuffVirt[1], "Pch1buff", s32FileSize);
        GetMMZMemory(&mmzBuffPhy[2], (void**)&mmzBuffVirt[2], "Pch2buff", s32FileSize);
	    if((mmzBuffPhy[0] == 0) || (mmzBuffPhy[1] == 0) || (mmzBuffPhy[2] == 0)){
            Log(ARCNN_LOG_ERR, "Malloc MMZ failed.\n");
            return -1;
        }

        cv::Mat channels[3];
        cv::split(src_img, channels);
        AR_CHAR *pchR = (AR_CHAR *)channels[2].data;
		AR_CHAR *pchG = (AR_CHAR *)channels[1].data;
		AR_CHAR *pchB = (AR_CHAR *)channels[0].data;

		for(int i = 0;i< src_img.rows;i++)
		{
			for(int j = 0;j< src_img.cols;j++)
			{
                mmzBuffVirt[0][i*u32SrcStride + j] = pchR[i*src_img.cols + j];
                mmzBuffVirt[1][i*u32SrcStride + j] = pchG[i*src_img.cols + j];
                mmzBuffVirt[2][i*u32SrcStride + j] = pchB[i*src_img.cols + j];
			}
			for(int j = src_img.cols;j< u32SrcStride;j++)
			{
                mmzBuffVirt[0][i*u32SrcStride + j] = 0;
                mmzBuffVirt[1][i*u32SrcStride + j] = 0;
                mmzBuffVirt[2][i*u32SrcStride + j] = 0;
			}
    	}

        stSrcImg.u32FrameId = u32FrameId++;
        stSrcImg.enFormat= AR_IMG_RGB;
        stSrcImg.u32Width = u32ImageWidth;
        stSrcImg.u32Height = u32ImageHeight;
        stSrcImg.u32ChannelNum = 3;
        stSrcImg.astChannels[0].u32AddrPhy  = mmzBuffPhy[0];
        stSrcImg.astChannels[0].uptrAddrVirt = (AR_UINTPTR)mmzBuffVirt[0];
        stSrcImg.astChannels[0].u32Stride = u32SrcStride;

        stSrcImg.astChannels[1].u32AddrPhy = mmzBuffPhy[1];
        stSrcImg.astChannels[1].uptrAddrVirt = (AR_UINTPTR)mmzBuffVirt[1];
        stSrcImg.astChannels[1].u32Stride = u32SrcStride;

        stSrcImg.astChannels[2].u32AddrPhy = mmzBuffPhy[2];
        stSrcImg.astChannels[2].uptrAddrVirt = (AR_UINTPTR)mmzBuffVirt[2];
        stSrcImg.astChannels[2].u32Stride = u32SrcStride;

        stDstImg.u32FrameId = stSrcImg.u32FrameId;
        stDstImg.enFormat = stSrcImg.enFormat;
        stDstImg.u32Width = u32ScaleWidth;
        stDstImg.u32Height = u32ScaleHeight;
        stDstImg.u32ChannelNum = stSrcImg.u32ChannelNum;
        s32FileSize = u32DstStride * u32InputHeight;

        GetMMZMemory(&mmzBuffPhy[0], (void**)&mmzBuffVirt[0], "Dst0buff", s32FileSize);
        GetMMZMemory(&mmzBuffPhy[1], (void**)&mmzBuffVirt[1], "Dst1buff", s32FileSize);
        GetMMZMemory(&mmzBuffPhy[2], (void**)&mmzBuffVirt[2], "Dst2buff", s32FileSize);
	    if((mmzBuffPhy[0] == 0) || (mmzBuffPhy[1] == 0) || (mmzBuffPhy[2] == 0)){
            Log(ARCNN_LOG_ERR, "Malloc MMZ failed.\n");
            return -1;
        }
        
        stDstImg.astChannels[0].u32AddrPhy  = mmzBuffPhy[0];
        stDstImg.astChannels[0].uptrAddrVirt = (AR_UINTPTR)mmzBuffVirt[0];
        stDstImg.astChannels[0].u32Stride = u32DstStride;

        stDstImg.astChannels[1].u32AddrPhy = mmzBuffPhy[1];
        stDstImg.astChannels[1].uptrAddrVirt = (AR_UINTPTR)mmzBuffVirt[1];
        stDstImg.astChannels[1].u32Stride = u32DstStride;

        stDstImg.astChannels[2].u32AddrPhy = mmzBuffPhy[2];
        stDstImg.astChannels[2].uptrAddrVirt = (AR_UINTPTR)mmzBuffVirt[2];;
        stDstImg.astChannels[2].u32Stride = u32DstStride;

        AR_HAL_SCALER_CROP_S stCropParm;
        stCropParm.u32X = 0;
        stCropParm.u32Y = 0;
        stCropParm.u32W = u32ScaleWidth * f32InputScale;
        stCropParm.u32H = u32ScaleHeight * f32InputScale;

        Log(ARCNN_LOG_INFO, "Input Image %dx%d, Crop %dx%d-%dx%d, Output %dx%d",
            stSrcImg.u32Width, stSrcImg.u32Height, stCropParm.u32X, stCropParm.u32Y, stCropParm.u32W, stCropParm.u32H,
            stDstImg.u32Width, stDstImg.u32Height);

        s32Ret = AR_MPI_SCALER_CropResize(&stSrcImg, &stCropParm, &stDstImg, 1, SCALER_MODE_BILINEAR);
        if(s32Ret < 0)
		{
            Log(ARCNN_LOG_ERR, "CropResize Image Error\n");
        }
		
        stDstImg.u32Width = u32InputWidth;
        stDstImg.u32Height = u32InputHeight;
        *pstImg = stDstImg;

        ReleaseMMZMemory((AR_U64)stSrcImg.astChannels[0].u32AddrPhy, (void *)stSrcImg.astChannels[0].uptrAddrVirt);
        ReleaseMMZMemory((AR_U64)stSrcImg.astChannels[1].u32AddrPhy, (void *)stSrcImg.astChannels[1].uptrAddrVirt);
        ReleaseMMZMemory((AR_U64)stSrcImg.astChannels[2].u32AddrPhy, (void *)stSrcImg.astChannels[2].uptrAddrVirt);
    }
	
	else if(u32FileType == YUV420P_FILE)	// file must end with '.yuv420p'
	{
		ifstream file(pchFileName, ios::in|ios::binary|ios::ate);
        if(file.is_open())
        {
			AR_IMG_S stSrcImgs = {0};
			AR_IMG_S stDstImgs = {0};
			AR_MEM_S stResizeIn0 = {0};
			AR_MEM_S stPchbuff = {0};
		
            //1. load original img file
    	    s32FileSize = file.tellg();
    	    file.seekg(0, ios::beg);
    	    AR_CHAR* fileOrgBuff = (AR_CHAR*)malloc(s32FileSize);
            memset(fileOrgBuff, 0, s32FileSize);
    	    file.read((AR_CHAR *)fileOrgBuff, s32FileSize);
    	    file.close();

            //2. get input tensor w/h, w align to 256/32 Byte
            AR_NPU_TENSOR_S stTensor ={0};
            AR_MPI_NPU_GetInputTensorParam(handle, u32InputId, &stTensor);

			GetImageSize(pchFileName, u32ImageWidth, u32ImageHeight);

			//GetInputScale(u32ImageWidth, u32ImageHeight, u32InputWidth, u32InputHeight, f32InputScale, u32ScaleWidth, u32ScaleHeight);

			AR_U32 u32SrcStride = ALIGNED_256B(u32ImageWidth);
			s32SrcSize = ALIGNED_256B(u32SrcStride * u32ImageHeight * 3 / 2);
			if(AR_MPI_NPU_GetSocVersion() == 2) //1:AR9341 2:AR9311
			{
				u32SrcStride = ALIGNED_32B(u32ImageWidth);
				s32SrcSize = ALIGNED_32B(u32SrcStride * u32ImageHeight * 3 / 2);
			}
		
			stResizeIn0.u64Len = s32SrcSize;
			s32Ret = AR_MPI_NPU_MallocBuff((AR_CHAR *)"ResizeIn", &stResizeIn0);
			if(s32Ret)
			{
				Log(ARCNN_LOG_ERR, "Malloc NPU stResizeIn0 error: size %d.\n", stResizeIn0.u64Len);
				return -1;
			}

            AR_U32 u32DstStride = ALIGNED_256B(stTensor.u32Width);
			s32DstSize = ALIGNED_256B(u32DstStride * stTensor.u32Height * 3/2);
			if(AR_MPI_NPU_GetSocVersion() == 2) //1:AR9341 2:AR9311
			{
				u32DstStride = ALIGNED_32B(stTensor.u32Width);
				s32DstSize = ALIGNED_32B(u32DstStride * stTensor.u32Height * 3/2);
			}

            stPchbuff.u64Len= s32DstSize;
            s32Ret = AR_MPI_NPU_MallocBuff((AR_CHAR *)"Pchbuff", &stPchbuff);
            if(s32Ret)
            {
                Log(ARCNN_LOG_ERR, "Malloc NPU stPchbuff error: size %d.\n", stPchbuff.u64Len);
                return -1;
            }

			//copy image(yuv) data to buffer and do Align Op
			for(int i=0; i<u32ImageHeight; i++)
			{
				memcpy((AR_CHAR*)(stResizeIn0.u64VirtAddr + u32SrcStride * i),	fileOrgBuff + u32ImageWidth * i, u32ImageWidth);
			}
			
			for(int i=0; i<u32ImageHeight / 2; i++)
			{
				memcpy((AR_CHAR*)(stResizeIn0.u64VirtAddr + u32SrcStride * u32ImageHeight + u32SrcStride / 2 * i),  fileOrgBuff + 
						u32ImageWidth * u32ImageHeight +  u32ImageWidth / 2 * i, u32ImageWidth / 2);
			}
			
			for(int i=0; i<u32ImageHeight / 2; i++)
			{
				memcpy((AR_CHAR*)(stResizeIn0.u64VirtAddr + u32SrcStride * u32ImageHeight + u32SrcStride * u32ImageHeight/4 + u32SrcStride / 2 * i),  
					fileOrgBuff + u32ImageWidth * u32ImageHeight * 5/4 + u32ImageWidth / 2 * i, u32ImageWidth / 2);
			}

            //3. if Image w/h != tensor w/h, need resize
            if(stTensor.u32Width != u32ImageWidth || stTensor.u32Height != u32ImageHeight)	
            {
                //Resize Crop, AR_IMG_NV21 and AR_IMG_NV12 with the same operate in scaler
                AR_HAL_SCALER_CROP_S stCrops = {0};	
                stCrops.u32X = 0;
                stCrops.u32Y = 0;
                stCrops.u32W = u32ImageWidth;
                stCrops.u32H = u32ImageHeight;
                
                stSrcImgs.u32Width = u32ImageWidth;
                stSrcImgs.u32Height = u32ImageHeight;
                stSrcImgs.u32ChannelNum = 3;
                stSrcImgs.enFormat = AR_IMG_YUV420P; 
                stSrcImgs.astChannels[0].u32Stride    = u32SrcStride;
                stSrcImgs.astChannels[0].u32AddrPhy   = stResizeIn0.u64PhyAddr;
                stSrcImgs.astChannels[0].uptrAddrVirt = stResizeIn0.u64VirtAddr;
                stSrcImgs.astChannels[1].u32Stride    = u32SrcStride / 2;
                stSrcImgs.astChannels[1].u32AddrPhy   = stResizeIn0.u64PhyAddr + u32SrcStride * u32ImageHeight;
                stSrcImgs.astChannels[1].uptrAddrVirt = stResizeIn0.u64VirtAddr + u32SrcStride * u32ImageHeight;
                stSrcImgs.astChannels[2].u32Stride    = u32SrcStride / 2;
                stSrcImgs.astChannels[2].u32AddrPhy   = stResizeIn0.u64PhyAddr + u32SrcStride * u32ImageHeight + u32SrcStride * u32ImageHeight / 4;
                stSrcImgs.astChannels[2].uptrAddrVirt = stResizeIn0.u64VirtAddr + u32SrcStride * u32ImageHeight + u32SrcStride * u32ImageHeight / 4;
            
                stDstImgs.u32Width = stTensor.u32Width;
                stDstImgs.u32Height = stTensor.u32Height;
                stDstImgs.u32ChannelNum = 3;
                stDstImgs.enFormat = AR_IMG_YUV420P; 
                stDstImgs.astChannels[0].u32Stride    = u32DstStride;
                stDstImgs.astChannels[0].u32AddrPhy   = stPchbuff.u64PhyAddr;
                stDstImgs.astChannels[0].uptrAddrVirt = stPchbuff.u64VirtAddr;
                stDstImgs.astChannels[1].u32Stride    = u32DstStride / 2;
                stDstImgs.astChannels[1].u32AddrPhy   = stPchbuff.u64PhyAddr + u32DstStride * stTensor.u32Height;
                stDstImgs.astChannels[1].uptrAddrVirt = stPchbuff.u64VirtAddr + u32DstStride * stTensor.u32Height;
				stDstImgs.astChannels[2].u32Stride    = u32DstStride / 2;
                stDstImgs.astChannels[2].u32AddrPhy   = stPchbuff.u64PhyAddr + u32DstStride * stTensor.u32Height + u32DstStride * stTensor.u32Height / 4;
                stDstImgs.astChannels[2].uptrAddrVirt = stPchbuff.u64VirtAddr + u32DstStride * stTensor.u32Height + u32DstStride * stTensor.u32Height / 4;

				AR_U32 align_mode = AR_IMG_ALIGN_TO_UP;
				if(u32ImageWidth < u32ImageHeight)
					align_mode = AR_IMG_ALIGN_TO_LEFT;
                s32Ret = AR_MPI_SCALER_CropResizeRatio(&stSrcImgs, &stCrops, &stDstImgs, 1, 0, align_mode);
                if(s32Ret < 0)
                {
                    Log(ARCNN_LOG_ERR, "AR_MPI_SCALER_CropResizeRatio error: %d.\n", s32Ret);
                    return -1;
                }
				
                AR_MPI_NPU_FreeBuff(&stResizeIn0);
            }
            else	//stTensor.w == img.w && stTensor.h == img.h 
            {
                //3. padding fileOrgBuff with 0 each row,  and saves to pchMMZBuffVirt
                AR_CHAR* pchDstBuffVirt = (AR_CHAR*)stPchbuff.u64VirtAddr;
                AR_CHAR * pcSrcBuff = fileOrgBuff;
                for(AR_U32 i = 0;i< u32ImageHeight; i++)
                {
                    memcpy(pchDstBuffVirt, pcSrcBuff, u32ImageHeight);
                    pchDstBuffVirt += u32DstStride;
                    pcSrcBuff += u32ImageWidth;	
                }
                
                for(AR_U32 i = 0;i< u32ImageHeight/2;i++)
                {
                    memcpy(pchDstBuffVirt, pcSrcBuff, u32ImageWidth);
                    pchDstBuffVirt += u32DstStride;
                    pcSrcBuff += u32ImageWidth;	
                }
            }		
            
    	    //4. add the pchMMZBuffVirt for IFC & NPU
    	    pstImg->astChannels[0].u32AddrPhy = (AR_UINTPTR)stPchbuff.u64PhyAddr;
            pstImg->astChannels[0].uptrAddrVirt = (AR_UINTPTR)stPchbuff.u64VirtAddr;
    	    pstImg->astChannels[1].u32AddrPhy = (AR_UINTPTR)stPchbuff.u64PhyAddr + u32DstStride * stTensor.u32Height;
            pstImg->astChannels[1].uptrAddrVirt = (AR_UINTPTR)stPchbuff.u64VirtAddr + u32DstStride * stTensor.u32Height;
			pstImg->astChannels[2].u32AddrPhy = (AR_UINTPTR)stPchbuff.u64PhyAddr + u32DstStride * stTensor.u32Height + u32DstStride * stTensor.u32Height / 4;
            pstImg->astChannels[2].uptrAddrVirt = (AR_UINTPTR)stPchbuff.u64VirtAddr + u32DstStride * stTensor.u32Height + u32DstStride * stTensor.u32Height / 4;
            pstImg->enFormat = AR_IMG_YUV420P;
            pstImg->u32FrameId = u32FrameId++;
			
			
    	    //5. FREE fileOrgBuff
    	    free(fileOrgBuff);
			//AR_MPI_NPU_FreeBuff(stPchbuff);
        }
    }

    else
    {
        //Log(ARCNN_LOG_ERR, "Invalid file type.\n");
        return -1;
    }

    ProfileEnd("GetLocalImage");
    Log(ARCNN_LOG_INFO, "Get image %s..", pchFileName);
    return 0;
}

AR_S32 CARCNN::ReleaseLocalImage(AR_IMG_SET_S * pstImg)
{
    //ProfileStart("ReleaseLocalImage");
	if(!pstImg)
	{
		Log(ARCNN_LOG_ERR, "pstImg is NULL.\n");
        return -1;
	}
	
    //Only need to release RGB data, YUV is released by other apis.
    for(AR_U32 u32IFCInputId =0;u32IFCInputId <MAX_INPUT_IMG_NUM;u32IFCInputId++)
	{
	    for(AR_U32 u32IFCBatchId =0;u32IFCBatchId <MAX_BATCH_IMG_NUM;u32IFCBatchId++)
	    {
	        AR_IMG_S *pstImgTmp = &pstImg->astInputImg[u32IFCInputId].astBatchImg[u32IFCBatchId];
			AR_MEM_S stPchbuff;
			stPchbuff.u64VirtAddr = pstImgTmp->astChannels[0].uptrAddrVirt;
			stPchbuff.u64PhyAddr= pstImgTmp->astChannels[0].u32AddrPhy;
	        AR_MPI_NPU_FreeBuff(&stPchbuff);
	     	if((pstImgTmp->enFormat == AR_IMG_RGB || pstImgTmp->enFormat == AR_IMG_RGBD))
		    {	
			    pstImgTmp->astChannels[0].uptrAddrVirt = 0;
				
                for(int chn = 0; chn < pstImgTmp->u32ChannelNum; chn++){
                    if(pstImgTmp->astChannels[chn].u32AddrPhy && pstImgTmp->astChannels[chn].uptrAddrVirt){
                        ReleaseMMZMemory((AR_U64)pstImgTmp->astChannels[chn].u32AddrPhy, (void *)pstImgTmp->astChannels[chn].uptrAddrVirt);
                    }
			        pstImgTmp->astChannels[chn].uptrAddrVirt = NULL;
                }
		    }
	    }
	}

    //ProfileEnd("ReleaseLocalImage");
    return 0;
}


AR_S32 CARCNN::Forward(AR_IMG_SET_S * pstImg, AR_BOOL bInstant, AR_BOOL bDebug)
{
    AR_S32 s32Ret;
    ProfileStart("Forward");
    s32Ret = AR_MPI_NPU_Forward(handle, pstImg, &stNPUInBuff, &stNPUOutBuff, bInstant, bDebug);
    ProfileEnd("Forward");
    return s32Ret;
}


//here, we pass the input image name to generate a output file.
AR_S32 CARCNN::PostProcess(void * args)
{
    AR_S32 i = 0, s32Ret = 0;
	AR_S32 s32FileSize = 0;
	
    AR_UINTPTR outputBuff = stNPUOutBuff.u64VirtAddr;
    AR_U32 outputBuffPhy = stNPUOutBuff.u64PhyAddr;
	AR_U32 u32OutputLength = AR_MPI_NPU_GetOutputBuffSize(handle);
    AR_CHAR name[128] = {0};

    std::vector<float> aryKeypointPoint;
    std::vector<int> aryKeypointShape;

    AR_NPU_TENSOR_S stOutputTensor;
    memset(&stOutputTensor, 0, sizeof(stOutputTensor));
	
    AR_U32 u32OutputTensorNum = AR_MPI_NPU_GetOutputTensorNum(handle);
    s32Ret =  AR_MPI_NPU_GetOutputTensorParam(handle, 0, &stOutputTensor);	//Npu Output Tensor
    if(s32Ret < 0){
        Log(ARCNN_LOG_ERR, "GetOutputTensorParam Error %d\n");
        return s32Ret;
    }

    if(GetMode() != ARCNN_INPUT_LIVE)	//local mode
	{
		cv::Size cvInputSize = cv::Size(u32InputWidth, u32InputHeight);

        BlobData* pstOutputNmsData = createBlob(1, stOutputTensor.u32OriChannels - 1, POSE_MAX_PEOPLE + 1, 3);
        BlobData* pstInputImageData = createBlob(1, stOutputTensor.u32OriChannels, cvInputSize.height, cvInputSize.width);	
        BlobData* pstOutputNetData = createBlob(1,stOutputTensor.u32OriChannels, stOutputTensor.u32Height, stOutputTensor.u32Width);

        AR_CHAR* outputData = (AR_CHAR*)outputBuff + (stOutputTensor.u32Bank * 32 * 1024*1024 + stOutputTensor.u32Offset)/sizeof(AR_CHAR);

        hwcTochw(outputData, &stOutputTensor, pstOutputNetData->data);

        computeNms(pstOutputNetData, pstOutputNmsData, 0.05);

        connectBodyParts(pstOutputNetData->data, pstOutputNmsData->data, cv::Size(pstOutputNetData->width, pstOutputNetData->height), POSE_MAX_PEOPLE, 9, 0.05, 3, 0.4, 1.0, aryKeypointPoint, aryKeypointShape);

		Log(ARCNN_LOG_INFO, "Dectect %d people", aryKeypointShape[0]);

		//write pic with osd to file
		const char * p = strCurrentImage.data();
		AR_S32 u32FileType = GetImageFileType((AR_CHAR *)p);

		if(u32FileType == YUV420P_FILE)
		{
			ProfileStart("PostProcess");
			AR_U32 u32pSrcStride = 0;
			AR_U32 u32pDstStride = 0;
			AR_S32 s32pSrcFileSize = 0;
			AR_S32 s32pDstFileSize = 0;
			
			ifstream file(strCurrentImage, ios::in|ios::binary|ios::ate);
	        if(file.is_open())
	        {
	        	stpSrcImgs = new AR_IMG_S;
	        	stpDstImgs = new AR_IMG_S;
				memset(stpSrcImgs, 0, sizeof(AR_IMG_S));
				memset(stpDstImgs, 0, sizeof(AR_IMG_S));
	
				GetImageSize(strCurrentImage, u32ImageWidth, u32ImageHeight);

	            //1. load original img file
	    	    AR_S32 s32FileSize = file.tellg();
	    	    file.seekg(0, ios::beg);
	    	    AR_CHAR* pfileOrgBuff = (AR_CHAR*)malloc(s32FileSize);
	            memset(pfileOrgBuff, 0,s32FileSize);
	    	    file.read((AR_CHAR *)pfileOrgBuff, s32FileSize);
	    	    file.close();
				
	            //2. get image w/h, w align to 256/32 Byte
	            u32pSrcStride = ALIGNED_256B(u32ImageWidth);
				if(AR_MPI_NPU_GetSocVersion() == 2)
				{
					u32pSrcStride = ALIGNED_32B(u32ImageWidth);
				}
				
				u32pDstStride = u32pSrcStride;
				
				s32pSrcFileSize = ALIGNED_256B(u32pSrcStride * u32ImageHeight * 3 / 2);
				s32pDstFileSize = ALIGNED_256B(u32pDstStride * u32ImageHeight * 3);
				if(AR_MPI_NPU_GetSocVersion() == 2)
				{
					s32pSrcFileSize = ALIGNED_32B(u32pSrcStride * u32ImageHeight * 3 / 2);
					s32pDstFileSize = ALIGNED_32B(u32pDstStride * u32ImageHeight * 3);
				}


				AR_MEM_S stpResizeIn1;
				stpResizeIn1.u64Len = s32pSrcFileSize;
				s32Ret = AR_MPI_NPU_MallocBuff((AR_CHAR *)"pResizeInp", &stpResizeIn1);
				if(s32Ret)
				{
					Log(ARCNN_LOG_ERR, "Malloc NPU stpResizeIn1 error : size %d . \n", stpResizeIn1.u64Len);
					return -1;
				}
				
				AR_MEM_S stPchBuff1;
				stPchBuff1.u64Len = s32pDstFileSize;
				s32Ret = AR_MPI_NPU_MallocBuff((AR_CHAR*)"stPchBuff1", &stPchBuff1);
				if(s32Ret)
				{
					Log(ARCNN_LOG_ERR, "Malloc NPU stPchBuff1 error : size %d . \n", stPchBuff1.u64Len);
					return -1;
				}

				//memcpy ori image data into stpResizeIn buffer
				for(int i=0; i<u32ImageHeight; i++)
				{
					memcpy((AR_CHAR*)(stpResizeIn1.u64VirtAddr + u32pSrcStride * i),  pfileOrgBuff + u32ImageWidth * i, u32ImageWidth);
				}
				
				for(int i=0; i<u32ImageHeight / 2; i++)
				{
					memcpy((AR_CHAR*)(stpResizeIn1.u64VirtAddr + u32pSrcStride * u32ImageHeight + u32pSrcStride/2 * i),  pfileOrgBuff + u32ImageWidth * u32ImageHeight +  u32ImageWidth / 2 * i, u32ImageWidth / 2);
				}
				
				for(int i=0; i<u32ImageHeight / 2; i++)
				{
					memcpy((AR_CHAR*)(stpResizeIn1.u64VirtAddr + u32pSrcStride * u32ImageHeight + u32pSrcStride * u32ImageHeight /4  + u32pSrcStride/2 * i),  pfileOrgBuff + u32ImageWidth * u32ImageHeight 
						+ u32ImageHeight * u32ImageWidth / 4 + u32ImageWidth / 2 * i, u32ImageWidth / 2);
				}

				//src image format : YUV420P
				stpSrcImgs->u32Width = u32ImageWidth;
				stpSrcImgs->u32Height = u32ImageHeight;
				stpSrcImgs->u32ChannelNum = 3;
				stpSrcImgs->enFormat = AR_IMG_YUV420P;
				stpSrcImgs->astChannels[0].u32Stride = u32pSrcStride;
				stpSrcImgs->astChannels[0].u32AddrPhy = stpResizeIn1.u64PhyAddr;
				stpSrcImgs->astChannels[0].uptrAddrVirt = stpResizeIn1.u64VirtAddr;
				stpSrcImgs->astChannels[1].u32Stride = u32pSrcStride/2;
				stpSrcImgs->astChannels[1].u32AddrPhy = stpResizeIn1.u64PhyAddr + u32pSrcStride * u32ImageHeight;
				stpSrcImgs->astChannels[1].uptrAddrVirt = stpResizeIn1.u64VirtAddr + u32pSrcStride * u32ImageHeight;                
				stpSrcImgs->astChannels[2].u32Stride = u32pSrcStride/2;
				stpSrcImgs->astChannels[2].u32AddrPhy = stpResizeIn1.u64PhyAddr + u32pSrcStride * u32ImageHeight + u32pSrcStride * u32ImageHeight / 4;
				stpSrcImgs->astChannels[2].uptrAddrVirt = stpResizeIn1.u64VirtAddr + u32pSrcStride * u32ImageHeight + u32pSrcStride * u32ImageHeight / 4;

				//Dst image format : RGB
				stpDstImgs->u32FrameId = 1;
				stpDstImgs->u32Width = u32ImageWidth;
				stpDstImgs->u32Height = u32ImageHeight;
				stpDstImgs->enFormat = AR_IMG_RGB;
				stpDstImgs->u32ChannelNum = 3;
				stpDstImgs->astChannels[0].u32Stride = u32pDstStride;
				stpDstImgs->astChannels[0].u32AddrPhy = stPchBuff1.u64PhyAddr;
				stpDstImgs->astChannels[0].uptrAddrVirt = stPchBuff1.u64VirtAddr;
				stpDstImgs->astChannels[1].u32Stride = u32pDstStride;
				stpDstImgs->astChannels[1].u32AddrPhy = stPchBuff1.u64PhyAddr + u32pDstStride * u32ImageHeight;
				stpDstImgs->astChannels[1].uptrAddrVirt = stPchBuff1.u64VirtAddr + u32pDstStride * u32ImageHeight;
				stpDstImgs->astChannels[2].u32Stride = u32pDstStride;
				stpDstImgs->astChannels[2].u32AddrPhy = stPchBuff1.u64PhyAddr + u32pDstStride * u32ImageHeight * 2;
				stpDstImgs->astChannels[2].uptrAddrVirt = stPchBuff1.u64VirtAddr + u32pDstStride * u32ImageHeight * 2;

				s32Ret = AR_MPI_IFC_CvtColor(stpSrcImgs, stpDstImgs, AR_MPI_IFC_BT601_FULL);
				if(s32Ret<0)
				{
					Log(ARCNN_LOG_ERR, "CvtColor Image Error \n");
					return -1;
				}

				cv::Mat DstImg;
				cv::Mat MergedImg;
				cv::Mat R(u32ImageHeight, u32pDstStride, CV_8UC1, (AR_CHAR*)stpDstImgs->astChannels[0].uptrAddrVirt);
				cv::Mat G(u32ImageHeight, u32pDstStride, CV_8UC1, (AR_CHAR*)stpDstImgs->astChannels[1].uptrAddrVirt);
				cv::Mat B(u32ImageHeight, u32pDstStride, CV_8UC1, (AR_CHAR*)stpDstImgs->astChannels[2].uptrAddrVirt);
				std::vector<cv::Mat> channels{B, G, R};
				cv::merge(channels, MergedImg);

				DstImg = MergedImg(cv::Rect(0, 0, u32ImageWidth, u32ImageHeight));

				char picname[256];
				float keypointScale = 1.0;
				AR_U32 u32TmpWidth,u32TmpHeight;
		        GetInputScale(DstImg.cols,DstImg.rows, pstOutputNetData->width,pstOutputNetData->height, keypointScale, u32TmpWidth,u32TmpHeight);
		        renderPoseKeypointsCpu(DstImg, aryKeypointPoint, aryKeypointShape, 0.05, keypointScale);
				sprintf(picname, "%s_yuv_draw.jpg", (AR_CHAR *)args);
		        cv::imwrite(picname, DstImg);
				
				AR_MPI_NPU_FreeBuff(&stpResizeIn1);
				AR_MPI_NPU_FreeBuff(&stPchBuff1);

	        }
			ProfileEnd("PostProcess");
		}
		
		else		//input img format : jpg
		{
			ProfileStart("PostProcess");
			
			float keypointScale = 1.0;
			char picname[256];
	        AR_U32 u32TmpWidth,u32TmpHeight;
	        cv::Mat src_img = cv::imread(strCurrentImage, cv::IMREAD_ANYCOLOR);
	        GetInputScale(src_img.cols,src_img.rows, pstOutputNetData->width,pstOutputNetData->height, keypointScale, u32TmpWidth,u32TmpHeight);
	        renderPoseKeypointsCpu(src_img, aryKeypointPoint, aryKeypointShape, 0.05, keypointScale);
			sprintf(picname, "%dx%d__%s_draw.jpg", src_img.cols, src_img.rows, (AR_CHAR *)args);
	        cv::imwrite(picname, src_img);
			
			ProfileEnd("PostProcess");
		}
		
		releaseBlob(&pstOutputNetData);
        releaseBlob(&pstInputImageData);
        releaseBlob(&pstOutputNmsData);
    }
	else
	{
        AR_CHAR* outputData = (AR_CHAR*)outputBuff + (stOutputTensor.u32Bank * 32 * 1024*1024 + stOutputTensor.u32Offset)/sizeof(AR_CHAR);
        AR_CHAR* copyBuffer = new AR_CHAR[u32OutputLength];
        memcpy(copyBuffer, outputData, u32OutputLength);
        tensorMutex.lock();
        tensorQueue.push(std::make_pair(stOutputTensor,copyBuffer));
        tensorMutex.unlock();
    }
	printf("\n");
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

AR_S32 CARCNN::ProfileStart(AR_CHAR * pchName)
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
        astProfile[i].u64Tic = tm.tv_sec * 1000 + tm.tv_usec / 1000;
    }

    return 0;
}

AR_S32 CARCNN::ProfileEnd(AR_CHAR * pchName)
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

        astProfile[i].u64Toc = tm.tv_sec * 1000 + tm.tv_usec / 1000;
        printf("[ARCNN][PROF]  %s, line %d: %s takes %d ms\n",
            __FUNCTION__, __LINE__, astProfile[i].strName.c_str(), astProfile[i].u64Toc - astProfile[i].u64Tic);

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


void CARCNN::ReadImageThread()
{
    while(bContinued){
        AR_IMG_S* flowImage = NULL;
        imageMutex.lock();
        if(!imageQueue.empty()){
            flowImage = imageQueue.front();
            imageQueue.pop();
        }
        imageMutex.unlock();

        if(flowImage == NULL){
            AR_CHAR *mmzBuffVirt[3] = {NULL};
            AR_U64 mmzBuffPhy[3] = {0};
            AR_U32 u32DstStride = ALIGNED_256B(u32InputWidth);
			if(AR_MPI_NPU_GetSocVersion() == 2)		//1:AR9341  2:AR9311
			{
				u32DstStride = ALIGNED_32B(u32InputWidth);
			}
            flowImage = new AR_IMG_S;
            memset(flowImage, 0, sizeof(AR_IMG_S));
            AR_S32 s32FileSize = u32DstStride * ALIGNED_256B(u32InputHeight);
			if(AR_MPI_NPU_GetSocVersion() == 2)		//1:AR9341  2:AR9311
			{
				s32FileSize = u32DstStride * ALIGNED_32B(u32InputHeight);
			}
            GetMMZMemory(&mmzBuffPhy[0], (void**)&mmzBuffVirt[0], "Dst0buff", s32FileSize);
            GetMMZMemory(&mmzBuffPhy[1], (void**)&mmzBuffVirt[1], "Dst1buff", s32FileSize);
            GetMMZMemory(&mmzBuffPhy[2], (void**)&mmzBuffVirt[2], "Dst2buff", s32FileSize);
            if((mmzBuffPhy[0] == 0) || (mmzBuffPhy[1] == 0) || (mmzBuffPhy[2] == 0)){
                Log(ARCNN_LOG_ERR, "Malloc MMZ failed.\n");
                delete flowImage;
                flowImage = NULL;
                return ;
            }

            flowImage->astChannels[0].u32AddrPhy  = mmzBuffPhy[0];
            //flowImage->astChannels[0].uptrAddrVirt = (AR_UINTPTR)mmzBuffVirt[0];
            flowImage->astChannels[0].u32Stride = u32DstStride;

            flowImage->astChannels[1].u32AddrPhy = mmzBuffPhy[1];
            //flowImage->astChannels[1].uptrAddrVirt = (AR_UINTPTR)mmzBuffVirt[1];
            flowImage->astChannels[1].u32Stride = u32DstStride;

            flowImage->astChannels[2].u32AddrPhy = mmzBuffPhy[2];
            //flowImage->astChannels[2].uptrAddrVirt = (AR_UINTPTR)mmzBuffVirt[2];;
            flowImage->astChannels[2].u32Stride = u32DstStride;
        }
		
        if(GetLiveImageImpl(flowImage) != 0){
            Log(ARCNN_LOG_ERR, "Get Live Image Fail.\n");
            imageMutex.lock();
            imageQueue.push(flowImage);
            imageMutex.unlock();
            ReleaseLvieImageImpl(flowImage);
            Log(ARCNN_LOG_INFO, "Release Live Image.\n");
            return ;
        }
        ReleaseLvieImageImpl(flowImage);

        imageMutex.lock();
        if(pendingImage != NULL){
            imageQueue.push(pendingImage);
        }
        pendingImage = flowImage;
        imageMutex.unlock();
    }
}


void CARCNN::DrawOutputThread()
{
    while(bContinued){

        AR_NPU_TENSOR_S stOutputTensor;
        AR_CHAR* pOutputNetData = NULL;

        std::vector<float> aryKeypointPoint;
        std::vector<int> aryKeypointShape;

        tensorMutex.lock();
        if(!tensorQueue.empty()){
            stOutputTensor = tensorQueue.front().first;
            pOutputNetData = tensorQueue.front().second;
            tensorQueue.pop();
        }
        tensorMutex.unlock();

        if(pOutputNetData == NULL){
            usleep(10*1000);
            continue;
        }

		ProfileStart("PostProcess");
        cv::Size cvInputSize = cv::Size(u32InputWidth, u32InputHeight);

        BlobData* pstOutputNmsData = createBlob(1, stOutputTensor.u32OriChannels - 1, POSE_MAX_PEOPLE + 1, 3);
        BlobData* pstInputImageData = createBlob(1, stOutputTensor.u32OriChannels, cvInputSize.height, cvInputSize.width);	
        BlobData* pstOutputNetData = createBlob(1,stOutputTensor.u32OriChannels, stOutputTensor.u32Height, stOutputTensor.u32Width);

        hwcTochw(pOutputNetData, &stOutputTensor, pstOutputNetData->data);

        computeNms(pstOutputNetData, pstOutputNmsData, 0.05);

        connectBodyParts(pstOutputNetData->data, pstOutputNmsData->data, cv::Size(pstOutputNetData->width, pstOutputNetData->height), POSE_MAX_PEOPLE, 9, 0.05, 3, 0.4, 1.0, aryKeypointPoint, aryKeypointShape);

        //Log(ARCNN_LOG_DEBUG, "BodyParts %d Shape %d\n",aryKeypointPoint.size(), aryKeypointShape.size());
        //for(int i = 0;i < aryKeypointShape.size(); i++){
        //    Log(ARCNN_LOG_DEBUG, "Shape %d Value %d\n",i,aryKeypointShape[i]);
        //}
        ProfileEnd("PostProcess");
        Log(ARCNN_LOG_INFO, "Dectect %d people with %d parts!\n", aryKeypointShape[0], aryKeypointShape[1]);

        if(GetMode() == ARCNN_INPUT_LIVE){
            if(pstIpcClient && u32ClientHandle){
                float inputScale = 1.0;
                AR_U32 u32Width,u32Height;
                GetInputScale(u32ImageWidth, u32ImageHeight, pstOutputNetData->width, pstOutputNetData->height, inputScale, u32Width, u32Height);
                renderKeypointsSoc(pstIpcClient, u32ClientHandle, s32OsdChn, aryKeypointPoint, aryKeypointShape, 0.05, u32Width, u32Height);
            }
        }

        releaseBlob(&pstOutputNetData);
        releaseBlob(&pstInputImageData);
        releaseBlob(&pstOutputNmsData);
        delete pOutputNetData;

    }
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
    memset(&stNPUInBuff,0, sizeof(stNPUInBuff));
    stNPUInBuff.u64Len= u32Size;
    s32Ret = AR_MPI_NPU_MallocBuff("input", &stNPUInBuff);
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
    memset(&stNPUOutBuff, 0, sizeof(stNPUOutBuff));
    stNPUOutBuff.u64Len = u32Size;

    s32Ret = AR_MPI_NPU_MallocBuff("output", &stNPUOutBuff);
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
