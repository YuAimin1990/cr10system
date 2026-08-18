//this is the base class
#include <iostream>
#include <fstream>
#include <string>
#include <sys/time.h>
#include <stdarg.h>
#include "cJSON.h"
#include "hal_sys.h"
#include "opencv2/opencv.hpp"

#include "mpi_npu_api.h"
#include "mpi_scaler_api.h"
#include "ar_cnn_channelcvt.h"

using namespace std;
using namespace cv;

#include "armbssd_base.h"
#include "armbssd_common.h"
#include "ssd.h"

static const char * std_ssd_labels[] =
{
	"__background__",
	"aeroplane",
	"bicycle",
	"bird",
	"boat",
	"bottle",
	"bus",
	"car",
	"cat",
	"chair",
	"cow",
	"diningtable",
	"dog",
	"horse",
	"motorbike",
	"person",
	"pottedplant",
	"sheep",
	"sofa",
	"train",
	"tvmonitor"
};

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

    pchLocalImage = NULL;
    u32ProfileEnable = 0;
    cbFunc = NULL;

	VpssGrp = 0;
	VpssChn = 0;
	memset(&VFrameInfo, 0, sizeof(VIDEO_FRAME_INFO_S));

	pstIpcClient = NULL;
    u32ClientHandle = 0;
	u32DrawOSD =0;

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


	stCNNDesc.uptrNpubinVirtAddr = 0;

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

    u32Initialized = 1;

}

CARCNN::~CARCNN()
{
    if(handle)
    {
        Log(ARCNN_LOG_INFO, "UnLoad Model begin.\n");
        AR_MPI_NPU_UnloadModel(handle);
		Log(ARCNN_LOG_INFO, "UnLoad Model end.\n");
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


	AR_IMG_S pstSrcImg;
	pstSrcImg.u32FrameId = VFrameInfo.stVFrame.u32FrameFlag;
    pstSrcImg.enFormat = AR_IMG_YUV420P;
    pstSrcImg.u32Width = VFrameInfo.stVFrame.u32Width;
    pstSrcImg.u32Height = VFrameInfo.stVFrame.u32Height;

    pstSrcImg.u32ChannelNum = 3;
    pstSrcImg.astChannels[0].u32Stride = VFrameInfo.stVFrame.u32Stride[0];
    pstSrcImg.astChannels[1].u32Stride = VFrameInfo.stVFrame.u32Stride[1];
    pstSrcImg.astChannels[2].u32Stride = VFrameInfo.stVFrame.u32Stride[2];
    pstSrcImg.astChannels[0].u32AddrPhy = (AR_U32)VFrameInfo.stVFrame.u64PhyAddr[0];
    pstSrcImg.astChannels[1].u32AddrPhy = (AR_U32)VFrameInfo.stVFrame.u64PhyAddr[1];
    pstSrcImg.astChannels[2].u32AddrPhy = (AR_U32)VFrameInfo.stVFrame.u64PhyAddr[2];

   	void * pDstAddrVirt = 0;
	AR_U64 pDstAddrPhy = 0;
	AR_U16 u16DstStride = ALIGNED_256B(pstSrcImg.u32Width);


	AR_U32 u32Size = u16DstStride*pstSrcImg.u32Height*3;
	GetMMZMemory(&pDstAddrPhy, &pDstAddrVirt, (AR_CHAR *)"ScalerDst", u32Size);
	if(!pDstAddrVirt)
	{
	    Log(ARCNN_LOG_ERR,"Malloc DstBuffer failed.\n");
		AR_MPI_VPSS_ReleaseChnFrame(VpssGrp, VpssChn, &VFrameInfo);
	    return -1;
	}

	AR_U16 u16Width = AR_MBSSD_IMG_WIDTH;
	AR_U16 u16Height = AR_MBSSD_IMG_HEIGHT;
	AR_NPU_TENSOR_S stTensor ={0};
	ret = AR_MPI_NPU_GetInputTensorParam(handle, 0, &stTensor);
	if(ret)
	{
	   Log(ARCNN_LOG_ERR, "AR_MPI_NPU_GetInputTensorParam error.\n");
	   return -1;
	}
	AR_NPU_IFC_PARAM_S stIFCParam = {0};
	ret = AR_MPI_NPU_GetIFCParamByName(handle, stTensor.achName,  &stIFCParam);
	if(ret)
	{
	   Log(ARCNN_LOG_ERR, "AR_MPI_NPU_GetIFCParamByName error.\n");
	   return -1;
	}
	AR_U16 u16Stride = stIFCParam.u32YStride;

	pstImg->u32FrameId = VFrameInfo.stVFrame.u32FrameFlag;
	pstImg->enFormat = AR_IMG_YUV420P;
	pstImg->u32Width = u16Width;
	pstImg->u32Height = u16Height;

	pstImg->u32ChannelNum = 3;
	pstImg->astChannels[0].u32Stride = u16Stride;
	pstImg->astChannels[1].u32Stride = u16Stride/2;
	pstImg->astChannels[2].u32Stride = u16Stride/2;

	pstImg->astChannels[0].u32AddrPhy = (AR_UINTPTR)pDstAddrPhy;
	pstImg->astChannels[0].uptrAddrVirt = (AR_UINTPTR)pDstAddrVirt;
	pstImg->astChannels[1].u32AddrPhy = (AR_UINTPTR)(pDstAddrPhy + u16Stride * u16Height);
	pstImg->astChannels[1].uptrAddrVirt = (AR_UINTPTR)((AR_CHAR *)pDstAddrVirt + u16Stride * u16Height);
	pstImg->astChannels[2].u32AddrPhy = (AR_UINTPTR)(pDstAddrPhy + u16Stride * u16Height + u16Stride * u16Height/4 );
	pstImg->astChannels[2].uptrAddrVirt = (AR_UINTPTR)((AR_CHAR *)pDstAddrVirt + u16Stride * u16Height + u16Stride * u16Height/4 );


	AR_HAL_SCALER_CROP_S stCrop;
	stCrop.u32X = 0;
	stCrop.u32Y = 0;
	stCrop.u32W = pstSrcImg.u32Width;
	stCrop.u32H = pstSrcImg.u32Height;
	AR_S32 s32Ret = AR_MPI_SCALER_CropResize(&pstSrcImg, &stCrop, pstImg, 1, 1);
	if(s32Ret < 0)
	{
	    ReleaseMMZMemory((AR_U64)pstImg->astChannels[0].u32AddrPhy, (void *)pstImg->astChannels[0].uptrAddrVirt);
		AR_MPI_VPSS_ReleaseChnFrame(VpssGrp, VpssChn, &VFrameInfo);
		Log(ARCNN_LOG_ERR,"Do crop/resize error.\n");
		return -1;
	}

	ProfileEnd("GetLiveImage");

	ret = AR_MPI_VPSS_ReleaseChnFrame(VpssGrp, VpssChn, &VFrameInfo);
	if(ret < 0)
	{
		Log(ARCNN_LOG_ERR,"AR_MPI_VPSS_ReleaseChnFrame ERR ret=%x.\n",ret);
		return -1;
	}

    return 0;
}

AR_S32 CARCNN::ReleaseLiveImage(AR_IMG_S * pstImg)
{
    ProfileStart("ReleaseLiveImage");

	ReleaseMMZMemory((AR_U64)pstImg->astChannels[0].u32AddrPhy, (void *)pstImg->astChannels[0].uptrAddrVirt);

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

AR_S32 CARCNN::FillInputBuff(AR_IMG_S *stImg, AR_CHAR * pchFileName, AR_U32 u32InputId, AR_U32 u32BatchId)
{
    AR_U32 u32FileType = 0;
    AR_U32 s32FileSize = 0;
    AR_UINTPTR pcInputBufferVirt = stNPUInBuff.u64VirtAddr;
    //AR_U32 pcInputBufferPhy = stNPUInBuff.u64PhyAddr;

    ProfileStart("FillInputBuff");
    u32FileType = GetImageFileType(pchFileName);
    if(u32FileType == RGB_RAW_DATA_FILE || u32FileType == BGR_RAW_DATA_FILE || u32FileType == RGBD_RAW_DATA_FILE )
    {

	   ifstream file(pchFileName, ios::in|ios::binary|ios::ate);
	   if(file.is_open())
	   {
		   s32FileSize = file.tellg();
		   file.seekg(0, ios::beg);

		   //Input offset: Users should know the Img/Tensor position in NPU input buffer
		   AR_NPU_TENSOR_S stTensor ={0};
		   AR_MPI_NPU_GetInputTensorParam(handle, u32InputId, &stTensor);
		   AR_U32 u32BatchTensorStep = AR_MPI_NPU_GetBatchTensorStep(handle,u32InputId);
		   AR_CHAR *InputOffsetAddr = (AR_CHAR *)pcInputBufferVirt +
				                      (stTensor.u32Bank*AR_NPU_BANK_SZIE + stTensor.u32Offset) +
						              (u32BatchId * u32BatchTensorStep);

		   file.read((AR_CHAR *)InputOffsetAddr, s32FileSize);
		   file.close();
	   }

    }
    else if(u32FileType == FLOAT_RAW_DATA_FILE)
    {
		AR_NPU_TENSOR_S stTensor ={0};
		AR_MPI_NPU_GetInputTensorParam(handle, u32InputId, &stTensor);
		FILE * fp = fopen( pchFileName, "rb");
		if(fp)
		{
			fseek(fp, 0, SEEK_END);
			s32FileSize = ftell(fp)/sizeof(float);
			if(s32FileSize!= stTensor.u32Size)
			{
				Log(ARCNN_LOG_ERR,"file size = %d, != tensor length = %d\n", s32FileSize, stTensor.u32Size);
				fclose(fp);
				return -1;
			}
		}
		fseek(fp, 0, SEEK_SET);

		AR_U64 pchMMZBuffPhy = 0;
		void * pchMMZBuffVirt = NULL;

		GetMMZMemory(&pchMMZBuffPhy, &pchMMZBuffVirt, (AR_CHAR *)"Pchbuff", stTensor.u32Size * sizeof(AR_FLOAT));
		if((pchMMZBuffPhy == 0) || !pchMMZBuffVirt)
		{
			Log(ARCNN_LOG_ERR, "Malloc MMZ failed.\n");
			return -1;
		}

		fread((AR_FLOAT *)pchMMZBuffVirt, sizeof(AR_FLOAT), stTensor.u32Size , fp);
		stImg->enFormat = AR_IMG_CHW_FLOAT_DATA;
		stImg->astChannels[0].u32AddrPhy = (AR_UINTPTR)pchMMZBuffPhy;
		stImg->astChannels[0].uptrAddrVirt = (AR_UINTPTR)pchMMZBuffVirt;
		fclose(fp);
    }
    else if(u32FileType == FIX_RAW_DATA_FILE)
    {
		AR_NPU_TENSOR_S stTensor ={0};
		AR_MPI_NPU_GetInputTensorParam(handle, u32InputId, &stTensor);
		FILE * fp = fopen( pchFileName, "rb");
		if(fp)
		{
			fseek(fp, 0, SEEK_END);
			s32FileSize = ftell(fp)/sizeof(AR_U8);
			if(s32FileSize != stTensor.u32Size)
			{
				Log(ARCNN_LOG_ERR,"file size = %d, != tensor length = %d\n", s32FileSize, stTensor.u32Size);
				fclose(fp);
				return -1;
			}
		}
		fseek(fp, 0, SEEK_SET);

		AR_U64 pchMMZBuffPhy = 0;
		void * pchMMZBuffVirt = NULL;

		GetMMZMemory(&pchMMZBuffPhy, &pchMMZBuffVirt, (AR_CHAR *)"Pchbuff", stTensor.u32Size * sizeof(AR_U8));
		if((pchMMZBuffPhy == 0) || !pchMMZBuffVirt)
		{
			Log(ARCNN_LOG_ERR, "Malloc MMZ failed.\n");
			return -1;
		}

		fread((AR_U8 *)pchMMZBuffVirt, sizeof(AR_U8), stTensor.u32Size , fp);
		stImg->enFormat = AR_IMG_CHW_FIX_DATA;
		stImg->astChannels[0].u32AddrPhy = (AR_UINTPTR)pchMMZBuffPhy;
		stImg->astChannels[0].uptrAddrVirt = (AR_UINTPTR)pchMMZBuffVirt;
		fclose(fp);
    }
    else
    {
        return -1;
    }

    //2022-04-11,For cacheable input buffer, we need to flush it 
    AR_MPI_NPU_FlushCachedBuff(&stNPUInBuff);

    ProfileEnd("FillInputBuff");
	Log(ARCNN_LOG_DEBUG, "Get data %s size %d to input ..\n", pchFileName, s32FileSize);
    return 0;
}


AR_S32 CARCNN::GetLocalImage(AR_CHAR * pchFileName, AR_IMG_S * pstImg)
{
    AR_U32 u32FileType = 0;
    AR_S32 s32FileSize = 0;
    void * pchMMZBuffVirt = NULL;
    AR_U64 pchMMZBuffPhy = 0;
    static AR_U32 u32FrameId = 0;
    //AR_UINTPTR pcInputBufferVirt = stNPUInBuff.u64VirtAddr;
    //AR_U32 pcInputBufferPhy = stNPUInBuff.u64PhyAddr;
    //AR_U32 u32InputSize = 0;

    ProfileStart("GetLocalImage");
    u32FileType = GetImageFileType(pchFileName);
    if(u32FileType == RGB_RAW_DATA_FILE || u32FileType == BGR_RAW_DATA_FILE || u32FileType == RGBD_RAW_DATA_FILE)
    {
        ifstream file(pchFileName, ios::in|ios::binary|ios::ate);
        if(file.is_open())
        {
            s32FileSize = file.tellg();
            file.seekg(0, ios::beg);
            GetMMZMemory(&pchMMZBuffPhy, &pchMMZBuffVirt, (AR_CHAR *)"Pchbuff", s32FileSize);
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

            if((u32ImgWidth == AR_MBSSD_IMG_WIDTH) && (u32ImgHeight==AR_MBSSD_IMG_HEIGHT))
            {
               pstImg->astChannels[0].u32AddrPhy = (AR_UINTPTR)pchMMZBuffPhy;
               pstImg->astChannels[0].uptrAddrVirt = (AR_UINTPTR)pchMMZBuffVirt;
            }
            else
            {
			   AR_U16 u16Width = AR_MBSSD_IMG_WIDTH;
			   AR_U16 u16Height = AR_MBSSD_IMG_HEIGHT;

			   AR_NPU_TENSOR_S stTensor ={0};
			   int ret = AR_MPI_NPU_GetInputTensorParam(handle, 0, &stTensor);
			   if(ret)
			   {
			      Log(ARCNN_LOG_ERR, "AR_MPI_NPU_GetInputTensorParam error.\n");
			      return -1;
			   }
			   AR_NPU_IFC_PARAM_S stIFCParam = {0};
			   ret = AR_MPI_NPU_GetIFCParamByName(handle, stTensor.achName,  &stIFCParam);
			   if(ret)
			   {
			      Log(ARCNN_LOG_ERR, "AR_MPI_NPU_GetIFCParamByName error.\n");
			      return -1;
			   }
			   AR_U16 u16Stride = stIFCParam.u32YStride;

		       AR_IMG_S pstSrcImg;
			   pstSrcImg.u32FrameId = 0;
			   pstSrcImg.enFormat = AR_IMG_RGB;
			   pstSrcImg.u32Width = u16Width;
			   pstSrcImg.u32Height = u16Height;

			   pstSrcImg.u32ChannelNum = 3;
			   pstSrcImg.astChannels[0].u32Stride = u16Stride;
			   pstSrcImg.astChannels[1].u32Stride = u16Stride;
			   pstSrcImg.astChannels[2].u32Stride = u16Stride;
			   pstSrcImg.astChannels[0].u32AddrPhy = (AR_UINTPTR)pchMMZBuffPhy;
			   pstSrcImg.astChannels[1].u32AddrPhy = (AR_UINTPTR)(pchMMZBuffPhy + u16Stride * u16Height );
			   pstSrcImg.astChannels[2].u32AddrPhy = (AR_UINTPTR)(pchMMZBuffPhy + u16Stride * u16Height * 2);


			   void * pDstAddrVirt = 0;
			   AR_U64 pDstAddrPhy = 0;
			   GetMMZMemory(&pDstAddrPhy, &pDstAddrVirt, (AR_CHAR *)"ScalerDST", 0x10000);
			   if(!pDstAddrVirt)
			   {
				   Log(ARCNN_LOG_ERR,"Malloc DstBuffer failed.\n");
				   return -1;
			   }

			   pstImg->u32Height = u16Height;
			   pstImg->u32Width = u16Width;
			   pstImg->u32ChannelNum = 3;

			   pstImg->astChannels[0].u32Stride = u16Stride;
			   pstImg->astChannels[1].u32Stride = u16Stride;
			   pstImg->astChannels[2].u32Stride = u16Stride;
			   pstImg->astChannels[0].u32AddrPhy = (AR_UINTPTR)pDstAddrPhy;
			   pstImg->astChannels[0].uptrAddrVirt = (AR_UINTPTR)pDstAddrVirt;
			   pstImg->astChannels[1].u32AddrPhy = (AR_UINTPTR)(pDstAddrPhy + u16Stride * u16Height);
			   pstImg->astChannels[1].uptrAddrVirt = (AR_UINTPTR)((AR_CHAR *)pDstAddrVirt + u16Stride * u16Height);
			   pstImg->astChannels[2].u32AddrPhy = (AR_UINTPTR)(pDstAddrPhy + u16Stride * u16Height * 2);
			   pstImg->astChannels[2].uptrAddrVirt = (AR_UINTPTR)((AR_CHAR *)pDstAddrVirt + u16Stride * u16Height * 2);


			   AR_HAL_SCALER_CROP_S stCrop;
			   stCrop.u32X = 0;
			   stCrop.u32Y = 0;
			   stCrop.u32W = pstSrcImg.u32Width;
			   stCrop.u32H = pstSrcImg.u32Height;
			   AR_S32 s32Ret = AR_MPI_SCALER_CropResize(&pstSrcImg, &stCrop, pstImg, 1, 1);
			   if(s32Ret < 0)
			   {
			       ReleaseMMZMemory(pchMMZBuffPhy, pchMMZBuffVirt);
				   Log(ARCNN_LOG_ERR,"Do crop/resize error.\n");
				   return -1;
			   }
			   ReleaseMMZMemory(pchMMZBuffPhy, pchMMZBuffVirt);

            }
        }
    }
	else if(u32FileType == GRAY_RAW_DATA_FILE)
    {
        ifstream file(pchFileName, ios::in|ios::binary|ios::ate);
        if(file.is_open())
        {
            //1. load original img file  original fileSizeBuffer = width*height*channel
			s32FileSize = file.tellg();
			file.seekg(0, ios::beg);
			AR_CHAR* fileOrgBuff = (AR_CHAR*)malloc(s32FileSize);
			memset(fileOrgBuff, 0,s32FileSize);
			file.read((AR_CHAR *)fileOrgBuff, s32FileSize);
			file.close();

			//2. width should algn,  fileSizeBuffer = stride*height*channel
			AR_NPU_TENSOR_S stTensor ={0};
			int ret = AR_MPI_NPU_GetInputTensorParam(handle, 0, &stTensor);
			if(ret)
			{
			   Log(ARCNN_LOG_ERR, "AR_MPI_NPU_GetInputTensorParam error.\n");
			   return -1;
			}
			AR_NPU_IFC_PARAM_S stIFCParam = {0};
			ret = AR_MPI_NPU_GetIFCParamByName(handle, stTensor.achName,  &stIFCParam);
			if(ret)
			{
			   Log(ARCNN_LOG_ERR, "AR_MPI_NPU_GetIFCParamByName error.\n");
			   return -1;
			}
			AR_U16 u16Stride = stIFCParam.u32YStride;
			
            AR_U32 u32Channel = 3;
	        s32FileSize = u16Stride *u32ImgHeight*u32Channel ;
		    GetMMZMemory(&pchMMZBuffPhy, &pchMMZBuffVirt, (AR_CHAR *)"Pchbuff", s32FileSize);
		    if((pchMMZBuffPhy == 0) || !pchMMZBuffVirt)
	        {
	            Log(ARCNN_LOG_ERR, "Malloc MMZ failed.\n");
	            return -1;
	        }
			memset(pchMMZBuffVirt,0,s32FileSize);

            //3. padding fileOrgBuff with 0 each row,  and saves to pchMMZBuffVirt
			AR_CHAR* pchDstBuffVirt = (AR_CHAR*)pchMMZBuffVirt;
            for(AR_U32 i = 0;i< u32ImgHeight;i++)
			{
				for(AR_U32 j = 0;j< u32ImgWidth;j++)
				{
					pchDstBuffVirt[i*u16Stride + j] = fileOrgBuff[i*u16Stride + j];
				}
				for(AR_U32 j = u32ImgWidth;j< u16Stride;j++)
				{
					pchDstBuffVirt[i*u16Stride + j] = 0;
				}
			}

            //4. add the pchMMZBuffVirt for IFC &NPU
			pstImg->astChannels[0].u32AddrPhy = (AR_UINTPTR)pchMMZBuffPhy;
            pstImg->astChannels[0].uptrAddrVirt = (AR_UINTPTR)pchMMZBuffVirt;
			pstImg->astChannels[1].u32AddrPhy = (AR_UINTPTR)pchMMZBuffPhy;
            pstImg->astChannels[1].uptrAddrVirt = (AR_UINTPTR)pchMMZBuffVirt;
			pstImg->astChannels[2].u32AddrPhy = (AR_UINTPTR)pchMMZBuffPhy;
            pstImg->astChannels[2].uptrAddrVirt = (AR_UINTPTR)pchMMZBuffVirt;
            pstImg->enFormat = AR_IMG_GRAY;
			pstImg->u32FrameId = u32FrameId++;

			//5. FREE fileOrgBuff
			free(fileOrgBuff);

        }
    }
	else if(u32FileType == JPG_BMP_PNG_IMG_FILE)
    {
        cv::Mat src_img = cv::imread(pchFileName, cv::IMREAD_ANYCOLOR);
        if(src_img.empty())
        {
           Log(ARCNN_LOG_ERR, "Imread file %s failed!\r\n", pchFileName);
           return -1;
        }

		AR_NPU_TENSOR_S stTensor ={0};
		AR_MPI_NPU_GetInputTensorParam(handle, 0, &stTensor);
		cv::Mat dst_img;

		if(((AR_U32)src_img.cols !=stTensor.u32Width) ||((AR_U32)src_img.rows != stTensor.u32Height))
		{
			cv::resize(src_img,dst_img,Size(stTensor.u32Width, stTensor.u32Height));
			Log(ARCNN_LOG_INFO,"resize dst img w/h/c %d %d %d\r\n",dst_img.cols,dst_img.rows,dst_img.channels());
		}
		else
		{
           dst_img = src_img;
		   Log(ARCNN_LOG_INFO,"Noneed resize dst img w/h/c %d %d %d\r\n",dst_img.cols,dst_img.rows,dst_img.channels());
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
	    GetMMZMemory(&pchMMZBuffPhy, &pchMMZBuffVirt, (AR_CHAR *)"Pchbuff", s32FileSize);
	    if((pchMMZBuffPhy == 0) || !pchMMZBuffVirt)
        {
            Log(ARCNN_LOG_ERR, "Malloc MMZ failed.\n");
            return -1;
        }

        if(src_img.channels()==1)
        {
	        cv::Mat channels[1];
	        cv::split(dst_img, channels);
	        AR_CHAR *pchR = (AR_CHAR *)channels[0].data;
			//AR_CHAR *pchG = (AR_CHAR *)channels[0].data;
			//AR_CHAR *pchB = (AR_CHAR *)channels[0].data;

			AR_CHAR* pchDstBuffVirt = (AR_CHAR*)pchMMZBuffVirt;
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
	        pstImg->astChannels[0].u32AddrPhy  = (AR_UINTPTR)pchMMZBuffPhy;
	        pstImg->astChannels[0].uptrAddrVirt = (AR_UINTPTR)pchDstBuffVirt;
	        pstImg->astChannels[1].u32AddrPhy = (AR_UINTPTR)pchMMZBuffPhy;
	        pstImg->astChannels[1].uptrAddrVirt = (AR_UINTPTR)pchDstBuffVirt;
	        pstImg->astChannels[2].u32AddrPhy = (AR_UINTPTR)pchMMZBuffPhy;
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

			AR_CHAR* pchDstBuffVirt = (AR_CHAR*)pchMMZBuffVirt;
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
			pstImg->astChannels[0].u32AddrPhy  = (AR_UINTPTR)pchMMZBuffPhy;
			pstImg->astChannels[0].uptrAddrVirt = (AR_UINTPTR)pchDstBuffVirt;
			pstImg->astChannels[1].u32AddrPhy = (AR_UINTPTR)(pchMMZBuffPhy + u32ChSize);
			pstImg->astChannels[1].uptrAddrVirt = (AR_UINTPTR)pchDstBuffVirt;
			pstImg->astChannels[2].u32AddrPhy = (AR_UINTPTR)(pchMMZBuffPhy + 2 * u32ChSize);
			pstImg->astChannels[2].uptrAddrVirt = (AR_UINTPTR)pchDstBuffVirt;
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
		        ReleaseMMZMemory((AR_U64)pstImgTmp->astChannels[0].u32AddrPhy, (void *)pstImgTmp->astChannels[0].uptrAddrVirt);
			     pstImgTmp->astChannels[0].uptrAddrVirt = 0;
		    }
	    }
	}

    ProfileEnd("ReleaseLocalImage");
    return 0;
}


AR_S32 CARCNN::Forward(AR_IMG_SET_S * pstImg, AR_BOOL bInstant, AR_BOOL bDebug)
{
    //ProfileStart("Forward");
    AR_S32 s32Ret = AR_MPI_NPU_Forward(handle, pstImg, &stNPUInBuff, &stNPUOutBuff, bInstant, bDebug);
    //ProfileEnd("Forward"); //this time consume including npupre(ex: ifc/tensor preprocess),so disable it

	if(u32ProfileEnable)
	{
		AR_U32 u32CBTime =0;
		AR_U32 *pCBTime = NULL;
		pCBTime =AR_MPI_NPU_GetCbTime(handle);
		for(AR_U32 i=0;i<128;i++){
	        if(pCBTime[i])
	        {
				Log(ARCNN_LOG_INFO,"CB[%d] Time: %d x 0.01ms\r\n",i,pCBTime[i]);
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
AR_S32 CARCNN::PostProcess(void * args)
{
    AR_S32 s32Ret = 0;
    AR_U32 size = 0;
    AR_S32 u32ExeSoftMax = 0;
    AR_U32 u32PostTensorNum = 0;
    AR_U32 u32OutputTensorNum = 0;
    AR_UINTPTR outputBuff = stNPUOutBuff.u64VirtAddr;
    //AR_U32 outputBuffPhy = stNPUOutBuff.u64PhyAddr;
    AR_NPU_TENSOR_S stOutputTensor;
    AR_NPU_POST_CB_PARAM_S stPostTensor;

    //2022-04-11, Need to invalid cache before read output buffer because we malloced a cachable buffer.
    AR_MPI_NPU_InvalidCachedBuff(&stNPUOutBuff);

    size = AR_MPI_NPU_GetOutputBuffSize(handle);
	//0. dump raw result buffer to file
	if(u32Mode != ARCNN_INPUT_LIVE)
    {
	    AR_CHAR name[128] = {0};
	    if(!args)
	    {
	        sprintf(name, "%s",  "fpga_output.bin");
	    }
	    else
	    {
	        sprintf(name, "%s%s", (AR_CHAR *)args, "_output.bin");
	    }

		ofstream ofile(name, ios::binary);
		if(ofile)
		{
			ofile.write((AR_CHAR *)outputBuff, size);
			Log(ARCNN_LOG_DEBUG, "Save output to %s finished.\n", name);
		}
    }

    //1. postprocess raw result buffer,like NCkHWCK->NCHW, SoftMax(if post_process.json defined getconfidence operator)..
    ProfileStart("PostProcessCVRT");
    AR_CHAR * NCHWBuff = (AR_CHAR *)malloc(size);
    memset(NCHWBuff, 0, size);
    u32OutputTensorNum = AR_MPI_NPU_GetOutputTensorNum(handle);


    for(AR_U32 i=0; i <u32OutputTensorNum; i++ )
    {
    	s32Ret =AR_MPI_NPU_GetOutputTensorParam(handle, i, &stOutputTensor);
    	if(s32Ret<0)
    	{
            Log(ARCNN_LOG_DEBUG,"get_output_tensor_params error. \n");
            return -1;
    	}

		u32ExeSoftMax = 0;
		u32PostTensorNum = AR_MPI_NPU_GetPostProcessTensorNum(handle);

		for(AR_U32 s32Index = 0; s32Index < u32PostTensorNum; s32Index++)
		{
	            s32Ret =AR_MPI_NPU_GetPostTensorParam(handle, i, &stPostTensor);
	            if(strcmp(stPostTensor.achOperatorType, "getconfidence"))
	            {
	            	continue;
	            }

	            if(stPostTensor.u32Id == stOutputTensor.u32ID)
	            {
	            	AR_CHAR * InBuffAddr = (AR_CHAR *)outputBuff +(stOutputTensor.u32Bank* 0x2000000 + stOutputTensor.u32Offset);
	        	    AR_CHAR * OutBuffAddr = (AR_CHAR *)NCHWBuff +(stOutputTensor.u32Bank* 0x2000000 + stOutputTensor.u32Offset);
	            	AR_NPU_Exe_Softmax_With_NhwcCvtNchw_Neon(InBuffAddr, OutBuffAddr, &stOutputTensor);
	            	u32ExeSoftMax = 1;
	            	break;
	            }
		}
		if(!u32ExeSoftMax)
	    {
	       	    AR_CHAR * InBuffAddr = (AR_CHAR *)outputBuff +(stOutputTensor.u32Bank* 0x2000000 + stOutputTensor.u32Offset);
	        	AR_CHAR * OutBuffAddr = (AR_CHAR *)NCHWBuff +(stOutputTensor.u32Bank* 0x2000000 + stOutputTensor.u32Offset);
			    AR_NPU_Outbuff_NhwcCvtNchw(InBuffAddr, OutBuffAddr, &stOutputTensor);
		}
    }
    ProfileEnd("PostProcessCVRT");


	ProfileStart("PostProcessSSD");
	AR_U32 u32ClassNum = 21;
	AR_FLOAT fIouThres = 0.45f;
	AR_FLOAT fConfThres = 0.50f;
	AR_U32 u32NetWidth =AR_MBSSD_IMG_WIDTH;
	AR_U32 u32NetHeight =AR_MBSSD_IMG_HEIGHT;


	AR_NPU_TENSOR_S *pOutputTensors =(AR_NPU_TENSOR_S *)malloc(u32OutputTensorNum * sizeof(AR_NPU_TENSOR_S));
    if(!pOutputTensors)
    {
		Log(ARCNN_LOG_ERR, "pOutputTensors malloc failed.\n");
		return -1;
	}
	memset(pOutputTensors,0,u32OutputTensorNum * sizeof(AR_NPU_TENSOR_S));
	for(AR_U32 i=0; i <u32OutputTensorNum; i++ )
	{
		AR_MPI_NPU_GetOutputTensorParam(handle,i,&pOutputTensors[i]);
	}

	SSD_detection SSD = SSD_detection(MOBILENETSSD_300, pOutputTensors, u32OutputTensorNum, u32ClassNum, fIouThres, fConfThres, u32NetWidth, u32NetHeight);

	vector<detection_output> Dets;
	#if 0 //if do not perform NCkHWCK->NCHW before, use SSD_PostProcess(including NCkHWCK->NCHW internal)
	    AR_CHAR * pSSDBuff = (AR_CHAR *)outputBuff;
		if(pOutputTensors[0].u32Precision == 8)
			SSD.SSD_PostProcess((char*)pSSDBuff, Dets);
		else if(pOutputTensors[0].u32Precision == 16)
			SSD.SSD_PostProcess((short*)pSSDBuff, Dets);
	#else //if perform NCkHWCK->NCHW before, use SSD_CHW_PostProcess(which not including NCkHWCK->NCHW internal)
	    AR_CHAR * pSSDBuff = (AR_CHAR *)NCHWBuff;
		if(pOutputTensors[0].u32Precision == 8)
			SSD.SSD_CHW_PostProcess((char*)pSSDBuff, Dets);
		else if(pOutputTensors[0].u32Precision == 16)
			SSD.SSD_CHW_PostProcess((short*)pSSDBuff, Dets);
    #endif
	ProfileEnd("PostProcessSSD");

	ProfileStart("PostProcessDrawOSD");

#if 0
	for(AR_U32 id=0;id<Dets.size();id++)
	{
	    Log(ARCNN_LOG_INFO, "Dets[%d].x =%f\r\n",id,Dets[id].x);
		Log(ARCNN_LOG_INFO, "Dets[%d].y =%f\r\n",id,Dets[id].y);
		Log(ARCNN_LOG_INFO, "Dets[%d].w =%f\r\n",id,Dets[id].w);
		Log(ARCNN_LOG_INFO, "Dets[%d].h =%f\r\n",id,Dets[id].h);
		Log(ARCNN_LOG_INFO, "Dets[%d].classId =%d\r\n",id,Dets[id].classId);
		Log(ARCNN_LOG_INFO, "Dets[%d].confidence =%f\r\n",id,Dets[id].confidence);
	}
#endif

    if(u32DrawOSD)
    {
	   IPC_CTRL_OSD_S stOsd = {0};
	   stOsd.u32VpssGrp = VpssGrp;
	   stOsd.u32StrNum = Dets.size();
	   for(AR_U32 strID=0; strID < stOsd.u32StrNum; strID++)
	   {
		   char scores[32]={0};
		   sprintf(scores, "%s:%f", std_ssd_labels[Dets[strID].classId],Dets[strID].confidence);
		   strcpy(stOsd.stOsdStr[strID].achStr, scores);
		   stOsd.stOsdStr[strID].u32X = Dets[strID].x;
		   stOsd.stOsdStr[strID].u32Y = Dets[strID].y;
           stOsd.stOsdStr[strID].u32RGB = 0x00FF0000; //red, but you can change the color according to different class.
	   }
	   stOsd.u32RectNum= Dets.size();
	   for(AR_U32 recID=0; recID < stOsd.u32RectNum; recID++)
	   {
		   stOsd.stOsdRect[recID].u32X = Dets[recID].x;
		   stOsd.stOsdRect[recID].u32Y = Dets[recID].y;
		   stOsd.stOsdRect[recID].u32W = Dets[recID].w;
		   stOsd.stOsdRect[recID].u32H = Dets[recID].h;
           stOsd.stOsdRect[recID].u32RGB = 0x00FF;//blue
	   }
	   stOsd.u32ImgHeight = AR_MBSSD_IMG_HEIGHT;
	   stOsd.u32ImgWidth = AR_MBSSD_IMG_WIDTH;

	   BINDER_RET_S stBinderRet = {0};
	   s32Ret = ar_ipc_send_buffer(pstIpcClient, u32ClientHandle, IPC_RPC_CMD_DRAW_VPSS_OSD, &stOsd, sizeof(stOsd), &stBinderRet, 0);
	   if(s32Ret<0)
	   {
           Log(ARCNN_LOG_ERR, "draw osd err ret=%d\r\n",s32Ret);
	   }

    }
	ProfileEnd("PostProcessDrawOSD");

	free(pOutputTensors);
	free(NCHWBuff);


	if(cbFunc)
    {
        s32Ret = cbFunc(args);
    }

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
	AR_BOOL bEnable = AR_TRUE;
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


