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
#include "opencv2/opencv.hpp"
#include "ar_cnn_channelcvt.h"
#include "mpi_scaler_api.h"

//#include "mpi_npu_api.h"

using namespace std;
using namespace cv;

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
    return 0;
}

AR_S32 CARCNN::GetLiveImage(AR_IMG_S * pstImg)
{
    ProfileStart("GetLiveImage");

    ProfileEnd("GetLiveImage");
    return 0;
}

AR_S32 CARCNN::ReleaseLiveImage(AR_IMG_S * pstImg)
{
    ProfileStart("ReleaseLiveImage");

    ProfileEnd("ReleaseLiveImage");
    return 0;
}

AR_S32 CARCNN::DeBindVideoDev()
{
    return 0;
}

string CARCNN::GetLocalTestDir()
{
    return strLocalDir;
}

#define NHWC_NCHW_NEON
AR_S32 CARCNN::FillInputBuff(AR_CHAR * pchFileName, AR_U32 u32InputId, AR_U32 u32BatchId)
{
    AR_U32 u32FileType = 0;
    AR_S32 s32FileSize = 0;
    //AR_UINTPTR pcInputBufferVirt = stNPUInBuff.u64VirtAddr;
    //AR_U32 pcInputBufferPhy = stNPUInBuff.u64PhyAddr;
    AR_MEM_S stTensorAddr;
    //AR_U32 u32TensorSize = 0;
    //AR_CHAR cFileName[32];
    AR_U32 i = 0;

    ProfileStart("FillInputBuff");

    u32FileType = GetImageFileType(pchFileName);
    if(u32FileType == RGB_RAW_DATA_FILE || u32FileType == BGR_RAW_DATA_FILE || u32FileType == RGBD_RAW_DATA_FILE )
    {

        AR_NPU_TENSOR_S stTensor ={0};
        AR_U32 u32InputTensorNum = AR_MPI_NPU_GetInputTensorNum(handle);

        for(i = 0; i < u32InputTensorNum; i++)
        {
        	AR_MPI_NPU_GetInputTensorParam(handle, i, &stTensor);
			if(u32InputTensorNum ==1)
			{
				break;
			}

        	if(CmpImageAndTensorName(pchFileName, stTensor.achName) == 0)
        	{
        		break;
        	}
        }
        if(i == u32InputTensorNum)
        {
        	Log(ARCNN_LOG_ERR, "Input File %s Not Compare With Tensor %s !", pchFileName, stTensor.achName);
        	return -1;
        }

	    //u32TensorSize = stTensor.u32Size * (stTensor.u32Precision/8);
        ifstream file(pchFileName, ios::in|ios::binary|ios::ate);
        if(file.is_open())
        {
        	s32FileSize = file.tellg();
        	//if(s32FileSize != u32TensorSize)  //no need for check, case padding
        	//{
        		//Log(ARCNN_LOG_ERR, "%s size not same with tensor length,file size = %d, tensor length = %d\n", pchFileName, s32FileSize, stTensor.u32Size);
        		//file.close();
        		//return -1;
        	//}
        }
        file.seekg(0, ios::beg);

        AR_MPI_NPU_GetInputTensorAddrByName(handle, stNPUInBuff, stTensor.achName, u32BatchId, &stTensorAddr);

        file.read((AR_CHAR *)stTensorAddr.u64VirtAddr, s32FileSize);
        file.close();
    }
    else if(u32FileType == FLOAT_RAW_DATA_FILE)
    {
        AR_NPU_TENSOR_S stTensor ={0};
        AR_U32 u32InputTensorNum = AR_MPI_NPU_GetInputTensorNum(handle);

        for(i = 0; i < u32InputTensorNum; i++)
        {
        	AR_MPI_NPU_GetInputTensorParam(handle, i, &stTensor);
			if(u32InputTensorNum ==1)
			{
				break;
			}

        	if(CmpImageAndTensorName(pchFileName, stTensor.achName) == 0)
        	{
        		break;
        	}
        }
        if(i == u32InputTensorNum)
        {
        	Log(ARCNN_LOG_ERR, "Input File %s Not Compare With Tensor %s !", pchFileName, stTensor.achName);
        	return -1;
        }

        ifstream file(pchFileName, ios::in|ios::binary|ios::ate);
        if(file.is_open())
        {
        	s32FileSize = file.tellg();
        	if(s32FileSize/sizeof(AR_FLOAT) != stTensor.u32Size)
        	{
        		Log(ARCNN_LOG_ERR, "%s size not same with tensor length,file size = %d, tensor length = %d\n", pchFileName, s32FileSize, stTensor.u32Size);
        		file.close();
        		return -1;
        	}
        }
        file.seekg(0, ios::beg);

        AR_MPI_NPU_GetInputTensorAddrByName(handle, stNPUInBuff, stTensor.achName, u32BatchId, &stTensorAddr);

        AR_CHAR * fpReadBuff = (AR_CHAR *)malloc(s32FileSize);
        memset(fpReadBuff, 0, s32FileSize);
        file.read((AR_CHAR *)fpReadBuff, s32FileSize);
        file.close();

        AR_U32 u32TensorSize = stTensor.u32Size * stTensor.u32Precision/8;
        AR_CHAR * pcTemp = (AR_CHAR *)malloc(u32TensorSize);
        memset(pcTemp, 0, u32TensorSize);

#ifdef NHWC_NCHW_NEON
       AR_NPU_FloatCvtFix((AR_FLOAT *)fpReadBuff, pcTemp, stTensor.u32Size,&stTensor);
	 AR_NPU_Exe_NchwCvtNhwc_Fix_Neon(pcTemp, (AR_CHAR *)stTensorAddr.u64VirtAddr, &stTensor);
#else
        AR_NPU_FloatCvtFix((AR_FLOAT *)fpReadBuff, pcTemp, stTensor.u32Size,&stTensor);
        AR_NPU_Exe_NchwCvtNhwc_Fix(pcTemp, (AR_CHAR *)stTensorAddr.u64VirtAddr, &stTensor);
#endif
        free(fpReadBuff);
        free(pcTemp);

	#if 0
        FILE * fp1 = NULL;
        char name[32] = {0};
        sprintf(name, "%s.txtc", stTensor.achName);
        fp1 = fopen(name, "wb+");
        fwrite((AR_CHAR *)stNPUInBuff.u64VirtAddr, 1, u32TensorSize, fp1);
        fclose(fp1);
	#endif
    }
    else if(u32FileType == FIX_RAW_DATA_FILE)
    {
	 AR_NPU_TENSOR_S stTensor ={0};
        AR_U32 u32InputTensorNum = AR_MPI_NPU_GetInputTensorNum(handle);

        for(i = 0; i < u32InputTensorNum; i++)
        {
        	AR_MPI_NPU_GetInputTensorParam(handle, i, &stTensor);
			if(u32InputTensorNum ==1)
			{
				break;
			}

        	if(CmpImageAndTensorName(pchFileName, stTensor.achName) == 0)
        	{
        		break;
        	}
        }
        if(i == u32InputTensorNum)
        {
        	Log(ARCNN_LOG_ERR, "Input File %s Not Compare With Tensor %s !", pchFileName, stTensor.achName);
        	return -1;
        }

        ifstream file(pchFileName, ios::in|ios::binary|ios::ate);
        if(file.is_open())
        {
        	s32FileSize = file.tellg();
        	if((s32FileSize/sizeof(AR_U8) != stTensor.u32Size) && ((s32FileSize/sizeof(AR_U16) != stTensor.u32Size)))
        	{
        		Log(ARCNN_LOG_ERR, "%s size not same with tensor length,file size = %d, tensor length = %d\n", pchFileName, s32FileSize, stTensor.u32Size);
        		file.close();
        		return -1;
        	}
        }
        file.seekg(0, ios::beg);

        AR_MPI_NPU_GetInputTensorAddrByName(handle, stNPUInBuff, stTensor.achName, u32BatchId, &stTensorAddr);

        AR_CHAR * fpReadBuff = (AR_CHAR *)malloc(s32FileSize);
        memset(fpReadBuff, 0, s32FileSize);
        file.read((AR_CHAR *)fpReadBuff, s32FileSize);
        file.close();

#ifdef NHWC_NCHW_NEON
        AR_NPU_Exe_NchwCvtNhwc_Fix_Neon(fpReadBuff, (AR_CHAR *)stTensorAddr.u64VirtAddr, &stTensor);
#else
        AR_NPU_Exe_NchwCvtNhwc_Fix(fpReadBuff, (AR_CHAR *)stTensorAddr.u64VirtAddr, &stTensor);
#endif
        free(fpReadBuff);
#if 0
        FILE * fp1 = NULL;
        char name[32] = {0};
        sprintf(name, "%s.txtc", stTensor.achName);
        fp1 = fopen(name, "wb+");
        fwrite((AR_CHAR *)stNPUInBuff.u64VirtAddr, 1, u32TensorSize, fp1);
        fclose(fp1);
#endif
    }
    else
    {
        return -1;
    }

    //2022-04-11,For cacheable input buffer, we need to flush it 
    AR_MPI_NPU_FlushCachedBuff(&stNPUInBuff);

    ProfileEnd("FillInputBuff");
    Log(ARCNN_LOG_DEBUG, "Get data %s[%d] to input %d..\n", pchFileName, s32FileSize, i);
    return 0;
}



//#define CALLBACK_DATA_LOAD
//For local image test. Need to add RGBD or RGBIntlv format later.
AR_S32 CARCNN::GetLocalImage(AR_CHAR * pchFileName, AR_IMG_S * pstImg, AR_U32 u32InputId)
{
    AR_U32 u32FileType = 0;
    AR_S32 s32FileSize = 0;
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

			AR_MEM_S stPchbuff;
			stPchbuff.u64Len= s32FileSize;
			int ret = AR_MPI_NPU_MallocBuff((AR_CHAR *)"Pchbuff", &stPchbuff);
			if(ret)
			{
			   Log(ARCNN_LOG_ERR, "Malloc NPU input error: size %d.\n", stPchbuff.u64Len);
			   return -1;
			}

            file.read((AR_CHAR *)stPchbuff.u64VirtAddr, s32FileSize);
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

            //No need to set w/h/s since ifc.json will set these parameters.
            pstImg->astChannels[0].u32AddrPhy = (AR_UINTPTR)stPchbuff.u64PhyAddr;
            pstImg->astChannels[0].uptrAddrVirt = (AR_UINTPTR)stPchbuff.u64VirtAddr;
       #ifdef CALLBACK_DATA_LOAD
           u32InputSize = AR_MPI_NPU_GetInputBuffSize(handle);
           //load the callback data to ddr before any real image is loaded.
           Log(ARCNN_LOG_DEBUG, "Load callback data.\n");
           ifstream cbfile(strLocalDir + "/input_callback.bin");
	       cbfile.read((char *)pcInputBufferVirt, u32InputSize);
           cbfile.close();
       #endif

        }
    }
    else if(u32FileType == NV12_RAW_DATA_FILE || u32FileType == NV21_RAW_DATA_FILE)
    {
        ifstream file(pchFileName, ios::in|ios::binary|ios::ate);
        if(file.is_open())
        {
            //1. load original img file
    	    s32FileSize = file.tellg();
    	    file.seekg(0, ios::beg);
    	    AR_CHAR* fileOrgBuff = (AR_CHAR*)malloc(s32FileSize);
            memset(fileOrgBuff, 0,s32FileSize);
    	    file.read((AR_CHAR *)fileOrgBuff, s32FileSize);
    	    file.close();
    	    //printf("s32FileSize=%d\r\n",s32FileSize);

            //2. get input tensor w/h, w align 
            AR_NPU_TENSOR_S stTensor ={0};
            int ret = AR_MPI_NPU_GetInputTensorParam(handle, u32InputId, &stTensor);
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
            AR_U16 u32DstStride = stIFCParam.u32YStride;
	
            s32FileSize = u32DstStride * stTensor.u32Height + u32DstStride * stTensor.u32Height/2;
            AR_MEM_S stPchbuff;
            stPchbuff.u64Len= s32FileSize;
            ret = AR_MPI_NPU_MallocBuff((AR_CHAR *)"Pchbuff", &stPchbuff);
            if(ret)
            {
                Log(ARCNN_LOG_ERR, "Malloc NPU input error: size %d.\n", stPchbuff.u64Len);
                return -1;
            }

            //3. if Image w/h != tensor w/h, need resize YUV420 is continuously stored in fileOrgBuff
            if(stTensor.u32Width != u32ImgWidth || stTensor.u32Height != u32ImgHeight)
            {
                AR_IMG_S stSrcImgs = {0};
                AR_IMG_S stDstImgs = {0};
                AR_MEM_S stResizeIn = {0};
                
                //2. width should 256 algn,  fileSizeBuffer = stride*height + stride*height/2
                AR_U32 u32SrcStride = ALIGNED_256B(u32ImgWidth);
				if(AR_MPI_NPU_GetSocVersion() == 2) //1: AR9341, 2: AR9311
					u32SrcStride = ALIGNED_32B(u32ImgWidth);
                printf("Y/UV u32SrcStride = %d, u32DstStride = %d\r\n", u32SrcStride, u32DstStride);				
                AR_U32 u32SrcSize = u32SrcStride * u32ImgHeight + u32SrcStride * u32ImgHeight/2;
                
                stResizeIn.u64Len = u32SrcSize;
                ret = AR_MPI_NPU_MallocBuff((AR_CHAR *)"ResizeIn", &stResizeIn);
                if(ret)
                {
                    Log(ARCNN_LOG_ERR, "Malloc NPU input error: size %d.\n", stResizeIn.u64Len);
                    return -1;
                }
                for(int h = 0; h < u32ImgHeight + u32ImgHeight / 2; h++){
					memcpy((void *)(stResizeIn.u64VirtAddr + u32SrcStride * h), fileOrgBuff + h * u32ImgWidth, u32ImgWidth);
				}
			
                //Resize Crop, AR_IMG_NV21 and AR_IMG_NV12 with the same operate in scaler
                AR_HAL_SCALER_CROP_S stCrops = {0};	
                stCrops.u32X = 0;
                stCrops.u32Y = 0;
                stCrops.u32W = u32ImgWidth;
                stCrops.u32H = u32ImgHeight;
                
                stSrcImgs.u32Width = u32ImgWidth;
                stSrcImgs.u32Height = u32ImgHeight;
                stSrcImgs.u32ChannelNum = 2;
                stSrcImgs.enFormat = AR_IMG_NV12; 
                stSrcImgs.astChannels[0].u32Stride = u32SrcStride;
                stSrcImgs.astChannels[0].u32AddrPhy = stResizeIn.u64PhyAddr;
                stSrcImgs.astChannels[0].uptrAddrVirt = stResizeIn.u64VirtAddr;
                stSrcImgs.astChannels[1].u32Stride = u32SrcStride;
                stSrcImgs.astChannels[1].u32AddrPhy = stResizeIn.u64PhyAddr + u32SrcStride * u32ImgHeight;
                stSrcImgs.astChannels[1].uptrAddrVirt = stResizeIn.u64VirtAddr + u32SrcStride * u32ImgHeight;
            
                stDstImgs.u32Width = stTensor.u32Width;
                stDstImgs.u32Height = stTensor.u32Height;
                stDstImgs.u32ChannelNum = 2;
                stDstImgs.enFormat = AR_IMG_NV12; 
                stDstImgs.astChannels[0].u32Stride = u32DstStride;
                stDstImgs.astChannels[0].u32AddrPhy = stPchbuff.u64PhyAddr;
                stDstImgs.astChannels[0].uptrAddrVirt = stPchbuff.u64VirtAddr;
                stDstImgs.astChannels[1].u32Stride = u32DstStride;
                stDstImgs.astChannels[1].u32AddrPhy = stPchbuff.u64PhyAddr + u32DstStride * stTensor.u32Height;
                stDstImgs.astChannels[1].uptrAddrVirt = stPchbuff.u64VirtAddr + u32DstStride * stTensor.u32Height;
                ProfileStart("ScalerResize");
                ret = AR_MPI_SCALER_CropResize(&stSrcImgs, &stCrops, &stDstImgs, 1, 0);
                if(ret < 0)
                {
                    Log(ARCNN_LOG_ERR, "AR_MPI_SCALER_CropResize error: %d.\n", ret);
                    return -1;
                }
                ProfileEnd("ScalerResize");
                AR_MPI_NPU_FreeBuff(&stResizeIn);
            }
            else
            {
                //3. padding fileOrgBuff with 0 each row,  and saves to pchMMZBuffVirt
                AR_CHAR* pchDstBuffVirt = (AR_CHAR*)stPchbuff.u64VirtAddr;
                AR_CHAR * pcSrcBuff = fileOrgBuff;
                for(AR_U32 i = 0;i< u32ImgHeight;i++)
                {
                    memcpy(pchDstBuffVirt, pcSrcBuff, u32ImgWidth);
                    pchDstBuffVirt += u32DstStride;
                    pcSrcBuff += u32ImgWidth;	
                }
                
                for(AR_U32 i = 0;i< u32ImgHeight/2;i++)
                {
                    memcpy(pchDstBuffVirt, pcSrcBuff, u32ImgWidth);
                    pchDstBuffVirt += u32DstStride;
                    pcSrcBuff += u32ImgWidth;	
                }
            }		
            
    	    //4. add the pchMMZBuffVirt for IFC & NPU
    	    pstImg->astChannels[0].u32AddrPhy = (AR_UINTPTR)stPchbuff.u64PhyAddr;
            pstImg->astChannels[0].uptrAddrVirt = (AR_UINTPTR)stPchbuff.u64VirtAddr;
    	    pstImg->astChannels[1].u32AddrPhy = (AR_UINTPTR)stPchbuff.u64PhyAddr + u32DstStride * stTensor.u32Height;
            pstImg->astChannels[1].uptrAddrVirt = (AR_UINTPTR)stPchbuff.u64VirtAddr + u32DstStride * stTensor.u32Height;
    
    	    if(u32FileType == NV12_RAW_DATA_FILE)
    	    {
    	        pstImg->enFormat = AR_IMG_NV12;
    	    }
    	    else
            {
                pstImg->enFormat = AR_IMG_NV21;
    	    }
    
            pstImg->u32FrameId = u32FrameId++;
    	    //printf("load img NV12/NV21 finished\r\n");
    
    	    //5. FREE fileOrgBuff
    	    free(fileOrgBuff);
        }
    }
    else if(u32FileType == GRAY_RAW_DATA_FILE)
    {
 		if(AR_MPI_NPU_GetSocVersion() == 1) //1:AR9341 2:AR9311
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

				printf("s32FileSize=%d\r\n",s32FileSize);

				//2. width should algn,  fileSizeBuffer = stride*height*channel
				AR_NPU_TENSOR_S stTensor ={0};
				int ret = AR_MPI_NPU_GetInputTensorParam(handle, u32InputId, &stTensor);
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
				printf("Y u16Stride =%d \r\n",u16Stride);
	            AR_U32 u32Channel = 3;
		        s32FileSize = u16Stride *u32ImgHeight*u32Channel ;

				AR_MEM_S stPchbuff;
				stPchbuff.u64Len= s32FileSize;
				ret = AR_MPI_NPU_MallocBuff((AR_CHAR *)"Pchbuff", &stPchbuff);
				if(ret)
				{
				   Log(ARCNN_LOG_ERR, "Malloc NPU input error: size %d.\n", stPchbuff.u64Len);
				   return -1;
				}

	            //3. padding fileOrgBuff with 0 each row,  and saves to pchMMZBuffVirt
				AR_CHAR* pchDstBuffVirt = (AR_CHAR*)stPchbuff.u64VirtAddr;
	            for(AR_U32 i = 0;i< u32ImgHeight;i++)
				{
					for(AR_U32 j = 0;j < u32ImgWidth;j++)
					{
						pchDstBuffVirt[i*u16Stride + j] = fileOrgBuff[i*u16Stride + j];
					}
					for(AR_U32 j = u32ImgWidth;j < u16Stride;j++)
					{
						pchDstBuffVirt[i*u16Stride + j] = 0;
					}
				}

	            //4. add the pchMMZBuffVirt for IFC &NPU
				pstImg->astChannels[0].u32AddrPhy = (AR_UINTPTR)stPchbuff.u64PhyAddr;
	            pstImg->astChannels[0].uptrAddrVirt = (AR_UINTPTR)stPchbuff.u64VirtAddr;
				pstImg->astChannels[1].u32AddrPhy = (AR_UINTPTR)stPchbuff.u64PhyAddr;
	            pstImg->astChannels[1].uptrAddrVirt = (AR_UINTPTR)stPchbuff.u64VirtAddr;
				pstImg->astChannels[2].u32AddrPhy = (AR_UINTPTR)stPchbuff.u64PhyAddr;
	            pstImg->astChannels[2].uptrAddrVirt = (AR_UINTPTR)stPchbuff.u64VirtAddr;
	            pstImg->enFormat = AR_IMG_GRAY;

				pstImg->u32FrameId = u32FrameId++;
				printf("load img gray finished\r\n");

				//5. FREE fileOrgBuff
				free(fileOrgBuff);

	        }
	 	}
	    else
	    {
		
			ifstream file(pchFileName, ios::in|ios::binary|ios::ate);
	        if(file.is_open())
	        {
	            s32FileSize = file.tellg();
	            file.seekg(0, ios::beg);

	            AR_MEM_S stPchbuff;
	            stPchbuff.u64Len= s32FileSize;
	            int ret = AR_MPI_NPU_MallocBuff((AR_CHAR *)"Pchbuff", &stPchbuff);
	            if(ret)
	            {
	               Log(ARCNN_LOG_ERR, "Malloc NPU input error: size %d.\n", stPchbuff.u64Len);
	               return -1;
	            }

	            file.read((AR_CHAR *)stPchbuff.u64VirtAddr, s32FileSize);
	            file.close();
	        
	            printf("s32FileSize=%d\r\n",s32FileSize);
	              
	            //4. add the pchMMZBuffVirt for IFC &NPU
	            pstImg->astChannels[0].u32AddrPhy = (AR_UINTPTR)stPchbuff.u64PhyAddr;
	            pstImg->astChannels[0].uptrAddrVirt = (AR_UINTPTR)stPchbuff.u64VirtAddr;
	            pstImg->astChannels[1].u32AddrPhy = (AR_UINTPTR)stPchbuff.u64PhyAddr + s32FileSize/3;
	            pstImg->astChannels[1].uptrAddrVirt = (AR_UINTPTR)stPchbuff.u64VirtAddr + s32FileSize/3;
	            pstImg->astChannels[2].u32AddrPhy = (AR_UINTPTR)stPchbuff.u64PhyAddr + (s32FileSize/3)*2;
	            pstImg->astChannels[2].uptrAddrVirt = (AR_UINTPTR)stPchbuff.u64VirtAddr + (s32FileSize/3)*2;
	            pstImg->enFormat = AR_IMG_GRAY;
	            
	            pstImg->u32FrameId = u32FrameId++;
	            printf("load img gray finished\r\n");
	            
	        }
	    }	
    }
	else if(u32FileType == JPG_BMP_PNG_IMG_FILE)
    {

        //read image and resize it, TBD
        cv::Mat src_img = cv::imread(pchFileName, cv::IMREAD_ANYCOLOR);
        if(src_img.empty())
        {
           Log(ARCNN_LOG_ERR, "Imread file %s failed: %s!\r\n", pchFileName, strerror(errno));
           return -1;
        }

		printf("src img w/h/c %d %d %d\r\n",src_img.cols,src_img.rows,src_img.channels());

		AR_NPU_TENSOR_S stTensor ={0};
		AR_MPI_NPU_GetInputTensorParam(handle, u32InputId, &stTensor);
		cv::Mat dst_img;

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
			//AR_CHAR *pchG = (AR_CHAR *)channels[0].data;
			//AR_CHAR *pchB = (AR_CHAR *)channels[0].data;

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
		AR_NPU_CB_PARAM_S stCbParam = {0};
		pCBTime =AR_MPI_NPU_GetCbTime(handle);
		for(AR_U32 i=0;i<128;i++){
			if(pCBTime[i] ==0xffffffff)
	        {
				AR_MPI_NPU_GetCBParamByCBId(handle, i, &stCbParam);
				printf("CB[%d] %s Not Registered\r\n",i,stCbParam.achOperatorType);
	        }
	        else if(pCBTime[i])
	        {
	        	AR_MPI_NPU_GetCBParamByCBId(handle, i, &stCbParam);
				printf("CB[%d] %s Time: %d x 0.01ms\r\n",i,stCbParam.achOperatorType, pCBTime[i]);
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
    AR_S32 s32Ret = 0;
    AR_U32 size = 0;
    //AR_U32 u32BatchNum = 0;
    AR_U32 u32PostTensorNum = 0;
    AR_U32 u32OutputTensorNum = 0;
    AR_UINTPTR outputBuff = stNPUOutBuff.u64VirtAddr;
    //AR_U32 outputBuffPhy = stNPUOutBuff.u64PhyAddr;
    AR_NPU_TENSOR_S stOutputTensor;
    AR_NPU_POST_CB_PARAM_S stPostTensor;
    AR_MEM_S stTensorAddr;
    //AR_CHAR * pcNchwAddr  = NULL;
    AR_CHAR * pcSoftmaxAddr  = NULL;

    size = AR_MPI_NPU_GetOutputBuffSize(handle);
    //2022-04-11, Need to invalid cache before read output buffer because we malloced a cachable buffer.
    AR_MPI_NPU_InvalidCachedBuff(&stNPUOutBuff);

    //0. dump raw result buffer to file
    AR_CHAR name[512] = {0};
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

    ProfileStart("PostProcess");

    //1. postprocess raw result buffer,like NCkHWCK->NCHW(has done in artstudio, no need again here)
    #if 0
    u32OutputTensorNum = AR_MPI_NPU_GetOutputTensorNum(handle);

    for(AR_U32 i=0; i <u32OutputTensorNum; i++ )
    {
    	s32Ret =AR_MPI_NPU_GetOutputTensorParam(handle, i, &stOutputTensor);
    	if(s32Ret<0)
    	{
            Log(ARCNN_LOG_DEBUG,"get_output_tensor_params error. \n");
            return -1;
    	}

   	AR_MPI_NPU_GetOutputTensorAddrByName(handle, stNPUOutBuff, stOutputTensor.achName, 0, &stTensorAddr);
	AR_U32 u32OutTensorLen = 0;
	if(strcmp(stOutputTensor.achType, "float")==0)
    	{
    		u32OutTensorLen = stOutputTensor.u32Width * stOutputTensor.u32Height *
				(stOutputTensor.u32KSizeNorm * stOutputTensor.u32KNormNum +
					stOutputTensor.u32KSizeLast) * sizeof(AR_FLOAT);
    	}
	else if((strcmp(stOutputTensor.achType,"integer")==0) && (stOutputTensor.u32Precision ==16))
    	{
    		u32OutTensorLen = stOutputTensor.u32Width * stOutputTensor.u32Height *
				(stOutputTensor.u32KSizeNorm * stOutputTensor.u32KNormNum +
					stOutputTensor.u32KSizeLast) * sizeof(AR_S16);
       }
	else
	{
		u32OutTensorLen = stOutputTensor.u32Width * stOutputTensor.u32Height *
				(stOutputTensor.u32KSizeNorm * stOutputTensor.u32KNormNum +
					stOutputTensor.u32KSizeLast) * sizeof(AR_CHAR);
	}

	//pcNchwAddr = (AR_CHAR *)malloc(u32OutTensorLen);
	pcNchwAddr = new char [u32OutTensorLen];
	memset(pcNchwAddr, 0, u32OutTensorLen);

	ProfileStart(stOutputTensor.achName);
	AR_NPU_Outbuff_NhwcCvtNchw((AR_CHAR *)stTensorAddr.u64VirtAddr, pcNchwAddr, &stOutputTensor);
	ProfileEnd(stOutputTensor.achName);

	sprintf(name, "%s%s", stOutputTensor.achName, "_output.bin");
	ofstream ofile1(name, ios::binary);
	if(ofile1)
	{
		ofile1.write((AR_CHAR *)pcNchwAddr, u32OutTensorLen);
		Log(ARCNN_LOG_DEBUG, "Save output to %s finished.\n", name);
	}

	delete []pcNchwAddr;
    }
	#endif


    //2. postprocess raw result buffer, like SoftMax(if post_process.json defined getconfidence operator)..

    u32PostTensorNum = AR_MPI_NPU_GetPostProcessTensorNum(handle);
    for(AR_U32 i=0; i <u32PostTensorNum; i++ )
    {
        s32Ret = AR_MPI_NPU_GetPostTensorParam(handle, i, &stPostTensor);
        if(s32Ret<0)
        {
             Log(ARCNN_LOG_DEBUG,"get_postprocess_tensor_params error. \n");
             return -1;
        }

        if(strcmp(stPostTensor.achOperatorType, "getconfidence"))
        {
            continue;
        }

        AR_MPI_NPU_GetOutputTensorParamById(handle, stPostTensor.u32Id, &stOutputTensor);
        AR_MPI_NPU_GetOutputTensorAddrByName(handle, stNPUOutBuff, stOutputTensor.achName, 0, &stTensorAddr);
        AR_U32 u32OutTensorLen = 0;

        u32OutTensorLen = stOutputTensor.u32Width * stOutputTensor.u32Height * (stOutputTensor.u32KSizeNorm *
                        stOutputTensor.u32KNormNum + stOutputTensor.u32KSizeLast) * sizeof(AR_FLOAT);

        pcSoftmaxAddr = new char [u32OutTensorLen];
        memset(pcSoftmaxAddr, 0, u32OutTensorLen);
        
        ProfileStart(stOutputTensor.achName);
        AR_NPU_Exe_Softmax_With_NhwcCvtNchw_Neon((AR_CHAR*)stTensorAddr.u64VirtAddr, pcSoftmaxAddr, &stOutputTensor);
        ProfileEnd(stOutputTensor.achName);
        
        sprintf(name, "%s%s", stOutputTensor.achName, "_softmax.bin");
        ofstream ofile1(name, ios::binary);
        if(ofile1)
        {
            ofile1.write((AR_CHAR *)pcSoftmaxAddr, u32OutTensorLen);
            Log(ARCNN_LOG_DEBUG, "Save output to %s finished.\n", name);
        }
        
        delete []pcSoftmaxAddr;
    }

    ProfileEnd("PostProcess");

    for(AR_U32 i=0; i <u32OutputTensorNum; i++ )
    {
        AR_MPI_NPU_GetOutputTensorParam(handle,i,&stOutputTensor);
        AR_CHAR * OutOffsetAddr = (AR_CHAR *)outputBuff +(stOutputTensor.u32Bank*0x2000000 + stOutputTensor.u32Offset);
        
        Log(ARCNN_LOG_INFO,"  Output Name %s Bank=%x Offset=%x \n",stOutputTensor.achName,stOutputTensor.u32Bank, stOutputTensor.u32Offset);
        if(strcmp(stOutputTensor.achType,"float")==0)
        {
            Log(ARCNN_LOG_INFO," [%d]Output %lx: %f %f %f %f\n",i,OutOffsetAddr, *(AR_FLOAT *)OutOffsetAddr, *(AR_FLOAT *)(OutOffsetAddr + 4), *(AR_FLOAT *)(OutOffsetAddr + 8), *(AR_FLOAT *)(OutOffsetAddr + 12));
        }
        else
        {
            Log(ARCNN_LOG_INFO," [%d]Output %lx: %x %x %x %x\n",i,OutOffsetAddr, *(AR_U32*)OutOffsetAddr, *(AR_U32 *)(OutOffsetAddr + 4), *(AR_U32 *)(OutOffsetAddr + 8), *(AR_U32 *)(OutOffsetAddr + 12));
        }
    }

    if(cbFunc)
    {
        s32Ret = cbFunc(handle);
    }
    //memset((void*)outputBuff,0,stNPUOutBuff.u64Len);
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

AR_S32 CARCNN::SetDSPCallbackMask(AR_U32 *pu32CBId, AR_U32 u32CBNum)
{
	return AR_MPI_NPU_SetDSPCBMask(handle, pu32CBId, u32CBNum);
}

AR_S32 CARCNN::RegisterDSPCallback()
{
	return AR_MPI_NPU_Register_DSPCB(handle);
}

AR_S32 CARCNN::UnRegisterDSPCallback()
{
	return AR_MPI_NPU_UnRegister_DSPCB(handle);
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
  AR_MPI_NPU_FlushCachedBuff(&stNPUInBuff);
	AR_MPI_NPU_FlushCachedBuff(&stNPUOutBuff);
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


