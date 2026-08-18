//this is the base class
#include <iostream>
#include <fstream>
#include <string>
#include <dirent.h>
#include <sys/time.h>
#include <stdarg.h>
#include "cJSON.h"
#include "opencv2/opencv.hpp"
#include "ar_cnn_channelcvt.h"

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
	u32CbEnable = 1;
    strLocalDir.clear();

    pchLocalImage = NULL;
    u32ProfileEnable = 0;
    cbFunc = NULL;

	u32IfcPreProcess=0;
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


    AR_CHAR ifcname[128] = {0};
    sprintf(ifcname, "%s", "ifc-pre-process");

	cJSON * preproc = cJSON_GetObjectItemCaseSensitive(pNet, ifcname);
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

    cJSON * path = cJSON_GetObjectItemCaseSensitive(pNet, "local_path");
    if(!path)
    {
        Log(ARCNN_LOG_ERR, "Parse local path failed.\n");
        delete[] pchBuff;
        cJSON_Delete(pJson);
        return;
    }
	else
	{
		strLocalDir = path->valuestring;
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
    if(handle)
    {
        AR_MPI_NPU_UnloadModel(handle);
    }

    handle = NULL;
}

AR_U32 CARCNN::IsIFCEnabled(AR_U32 u32InputId)
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

#define NHWC_NCHW_NEON

AR_S32 CARCNN::GetLocalInput(AR_CHAR * pchFileName, AR_IMG_SET_S *stImg, AR_U32 u32InputId, AR_U32 u32BatchId, AR_U32 u32FrameId)
{
	AR_S32 ret = 0;
	AR_IMG_S * pstImg = NULL;

    ProfileStart("GetLocalInput");
    AR_U32 u32FileType = GetImageFileType(pchFileName);
    if(u32FileType == RGB_RAW_DATA_FILE || u32FileType == BGR_RAW_DATA_FILE || u32FileType == RGBD_RAW_DATA_FILE)
    {
		if(u32IfcPreProcess)
		{
			static AR_MEM_S stPchbuff ={0};
		    if(!stPchbuff.u64PhyAddr || !stPchbuff.u64VirtAddr)
			{   
			    // malloc total frame buffer for preprocess
				stPchbuff.u64Len= GetFrameNum()*GetYUVStep(u32InputId);
				ret = AR_MPI_NPU_MallocBuff((AR_CHAR *)"Pchbuff", &stPchbuff);
				if(ret)
				{
				   Log(ARCNN_LOG_ERR, "Malloc NPU input error: size %d.\n", stPchbuff.u64Len);
				   return -1;
				}
				
			    Log(ARCNN_LOG_INFO,"%d %x %x \r\n",stPchbuff.u64Len,stPchbuff.u64PhyAddr,stPchbuff.u64VirtAddr);		
			}
		    stImg->astInputImg[u32InputId].bPreIfcProcess = AR_TRUE;
			ifstream file(pchFileName, ios::in|ios::binary|ios::ate);
			if(file.is_open())
			{
				AR_S32 s32FileSize = file.tellg();
				AR_U64 u64OffsetAddr = stPchbuff.u64VirtAddr + GetYUVStep(u32InputId)*u32FrameId;;
				file.seekg(0, ios::beg);
				file.read((AR_CHAR *)u64OffsetAddr, s32FileSize);
				file.close();

				pstImg = &(stImg->astInputImg[u32InputId].astBatchImg[u32BatchId]);
				pstImg->u32FrameId = u32FrameId++;
				if(u32FileType == RGBD_RAW_DATA_FILE)
				{
					pstImg->enFormat = AR_IMG_RGBD; //Indicates that there's D data
				}
				else
				{
					pstImg->enFormat = AR_IMG_RGB;
				}

				pstImg->astChannels[0].u32AddrPhy = (AR_UINTPTR)stPchbuff.u64PhyAddr;
				pstImg->astChannels[0].uptrAddrVirt = (AR_UINTPTR)stPchbuff.u64VirtAddr;

		   }

		}
		else
		{
		    AR_NPU_TENSOR_S stTensor ={0};
			AR_MEM_S stTensorAddr ={0};
			AR_MPI_NPU_GetInputTensorParam(handle, u32InputId, &stTensor);
			AR_MPI_NPU_GetInputTensorAddrByName(handle, stNPUInBuff, stTensor.achName, u32BatchId, &stTensorAddr);
			
		    stImg->astInputImg[u32InputId].bPreIfcProcess = AR_FALSE;
            ifstream file(pchFileName, ios::in|ios::binary|ios::ate);
	        if(file.is_open())
	        {
	        	AR_U32 s32FileSize = file.tellg();
				AR_U64 u64OffsetAddr = stTensorAddr.u64VirtAddr + GetBatchTensorStep(u32InputId)*u32FrameId;
			
				file.seekg(0, ios::beg);
		        file.read((AR_CHAR *)u64OffsetAddr, s32FileSize);
		        file.close();
	        }
		}
    }
	else if(u32FileType == GRAY_RAW_DATA_FILE)
    {
		static AR_MEM_S stPchbuff ={0};
	    if(!stPchbuff.u64PhyAddr || !stPchbuff.u64VirtAddr)
		{   
		    // malloc total frame buffer for preprocess
			stPchbuff.u64Len= GetFrameNum()*GetYUVStep(u32InputId);
			ret = AR_MPI_NPU_MallocBuff((AR_CHAR *)"Pchbuff", &stPchbuff);
			if(ret)
			{
			   Log(ARCNN_LOG_ERR, "Malloc NPU input error: size %d.\n", stPchbuff.u64Len);
			   return -1;
			}
		}
	    stImg->astInputImg[u32InputId].bPreIfcProcess = AR_TRUE;
		ifstream file(pchFileName, ios::in|ios::binary|ios::ate);
		if(file.is_open())
		{
			AR_S32 s32FileSize = file.tellg();
			AR_U64 u64OffsetAddr = stPchbuff.u64VirtAddr + GetYUVStep(u32InputId)*u32FrameId;;				
			file.seekg(0, ios::beg);
			file.read((AR_CHAR *)u64OffsetAddr, s32FileSize);
			file.close();

			pstImg = &(stImg->astInputImg[u32InputId].astBatchImg[u32BatchId]);
			pstImg->u32FrameId = u32FrameId++;

			pstImg->astChannels[0].u32AddrPhy = (AR_UINTPTR)stPchbuff.u64PhyAddr;
            pstImg->astChannels[0].uptrAddrVirt = (AR_UINTPTR)stPchbuff.u64VirtAddr;
			pstImg->astChannels[1].u32AddrPhy = (AR_UINTPTR)stPchbuff.u64PhyAddr;
            pstImg->astChannels[1].uptrAddrVirt = (AR_UINTPTR)stPchbuff.u64VirtAddr;
			pstImg->astChannels[2].u32AddrPhy = (AR_UINTPTR)stPchbuff.u64PhyAddr;
            pstImg->astChannels[2].uptrAddrVirt = (AR_UINTPTR)stPchbuff.u64VirtAddr;
            pstImg->enFormat = AR_IMG_GRAY;

			pstImg->u32FrameId = u32FrameId++;
	   }
    }
	else if(u32FileType == JPG_BMP_PNG_IMG_FILE)
    {
    	static AR_MEM_S stPchbuff ={0};
	    if(!stPchbuff.u64PhyAddr || !stPchbuff.u64VirtAddr)
		{   
		    // malloc total frame buffer for preprocess
			stPchbuff.u64Len= GetFrameNum()*GetYUVStep(u32InputId);
			ret = AR_MPI_NPU_MallocBuff((AR_CHAR *)"Pchbuff", &stPchbuff);
			if(ret)
			{
			   Log(ARCNN_LOG_ERR, "Malloc NPU input error: size %d.\n", stPchbuff.u64Len);
			   return -1;
			}
		    Log(ARCNN_LOG_INFO, "%d %x %x \r\n",stPchbuff.u64Len,stPchbuff.u64PhyAddr,stPchbuff.u64VirtAddr);			  
		}

		AR_NPU_TENSOR_S stTensor ={0};
		AR_MPI_NPU_GetInputTensorParam(handle, u32InputId, &stTensor);
        cv::Mat src_img = cv::imread(pchFileName, cv::IMREAD_ANYCOLOR);
        if(src_img.empty())
        {
           Log(ARCNN_LOG_ERR, "Imread file %s failed: %s!\r\n", pchFileName, strerror(errno));
		   if(stPchbuff.u64PhyAddr && stPchbuff.u64VirtAddr)
		   {
		    	AR_MPI_NPU_FreeBuff(&stPchbuff);
		   }
		   return -1;
        }
		cv::Mat dst_img;
		if(((AR_U32)src_img.cols !=stTensor.u32Width) ||((AR_U32)src_img.rows != stTensor.u32Height))
		{
			cv::resize(src_img,dst_img,Size(stTensor.u32Width, stTensor.u32Height));
		}
		else
		{
           dst_img = src_img;
		}

		AR_NPU_IFC_PARAM_S stIFCParam = {0};
		ret = AR_MPI_NPU_GetIFCParamByName(handle, stTensor.achName, &stIFCParam);
		if(ret)
		{
		   Log(ARCNN_LOG_ERR, "AR_MPI_NPU_GetIFCParamByName error.\n");
		   return -1;
		}
		AR_U16 u16Stride = stIFCParam.u32YStride;

		Log(ARCNN_LOG_INFO, "Src w/h/c %d %d %d!\r\n", src_img.cols,src_img.rows, src_img.channels());
		Log(ARCNN_LOG_INFO, "Dst w/h/s %d %d %d!\r\n", dst_img.cols,dst_img.rows, u16Stride);
		  
		AR_U64 u64OffsetAddr = stPchbuff.u64VirtAddr + GetYUVStep(u32InputId)*u32FrameId;				
		Log(ARCNN_LOG_INFO, "GetYUVStep(u32InputId)*u32FrameId %x!\r\n", GetYUVStep(u32InputId)*u32FrameId);
		

		stImg->astInputImg[u32InputId].bPreIfcProcess = AR_TRUE;
		pstImg = &(stImg->astInputImg[u32InputId].astBatchImg[u32BatchId]);
		pstImg->u32FrameId = u32FrameId++;

        if(src_img.channels()==1)
        {
	        cv::Mat channels[1];
	        cv::split(dst_img, channels);
	        AR_CHAR *pchR = (AR_CHAR *)channels[0].data;
			//AR_CHAR *pchG = (AR_CHAR *)channels[0].data;
			//AR_CHAR *pchB = (AR_CHAR *)channels[0].data;

			AR_CHAR* pchDstBuffVirt = (AR_CHAR*)u64OffsetAddr;
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
	        pstImg->astChannels[0].uptrAddrVirt = (AR_UINTPTR)stPchbuff.u64VirtAddr;
	        pstImg->astChannels[1].u32AddrPhy = (AR_UINTPTR)stPchbuff.u64PhyAddr;
	        pstImg->astChannels[1].uptrAddrVirt = (AR_UINTPTR)stPchbuff.u64VirtAddr;
	        pstImg->astChannels[2].u32AddrPhy = (AR_UINTPTR)stPchbuff.u64PhyAddr;
	        pstImg->astChannels[2].uptrAddrVirt = (AR_UINTPTR)stPchbuff.u64VirtAddr;
	        pstImg->astChannels[3].u32AddrPhy = 0;

        }
		else
		{
		    cv::Mat channels[3];
			cv::split(dst_img, channels);
			AR_CHAR *pchR = (AR_CHAR *)channels[2].data;
			AR_CHAR *pchG = (AR_CHAR *)channels[1].data;
			AR_CHAR *pchB = (AR_CHAR *)channels[0].data;

			AR_CHAR* pchDstBuffVirt = (AR_CHAR*)u64OffsetAddr;
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
			pstImg->astChannels[0].uptrAddrVirt = (AR_UINTPTR)stPchbuff.u64VirtAddr;	
		}

    }
	else if(u32FileType == FLOAT_RAW_DATA_FILE)
    {
    	AR_U32 s32FileSize = 0;
		AR_CHAR * fpReadBuff = NULL;
		AR_NPU_TENSOR_S stTensor ={0};
		AR_MEM_S stTensorAddr ={0};
		AR_MPI_NPU_GetInputTensorParam(handle, u32InputId, &stTensor);
		AR_MPI_NPU_GetInputTensorAddrByName(handle, stNPUInBuff, stTensor.achName, u32BatchId, &stTensorAddr);
		
		
		stImg->astInputImg[u32InputId].bPreIfcProcess = AR_FALSE;
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

			file.seekg(0, ios::beg);
			fpReadBuff = (AR_CHAR *)malloc(s32FileSize);
	        memset(fpReadBuff, 0, s32FileSize);
	        file.read((AR_CHAR *)fpReadBuff, s32FileSize);
	        file.close();
		}

        AR_U32 u32TensorSize = stTensor.u32Size * stTensor.u32Precision/8;
        AR_CHAR * pcTemp = (AR_CHAR *)malloc(u32TensorSize);
        memset(pcTemp, 0, u32TensorSize);

		AR_U64 u64OffsetAddr = stTensorAddr.u64VirtAddr + GetBatchTensorStep(u32InputId)*u32FrameId;
			

#ifdef NHWC_NCHW_NEON
       AR_NPU_FloatCvtFix((AR_FLOAT *)fpReadBuff, pcTemp, stTensor.u32Size,&stTensor);
	 AR_NPU_Exe_NchwCvtNhwc_Fix_Neon(pcTemp, (AR_CHAR *)u64OffsetAddr, &stTensor);
#else
        AR_NPU_FloatCvtFix((AR_FLOAT *)fpReadBuff, pcTemp, stTensor.u32Size,&stTensor);
        AR_NPU_Exe_NchwCvtNhwc_Fix(pcTemp, (AR_CHAR *)u64OffsetAddr, &stTensor);
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
		AR_U32 s32FileSize = 0;
		AR_CHAR * fpReadBuff = NULL;
		AR_NPU_TENSOR_S stTensor ={0};
		AR_MEM_S stTensorAddr ={0};
		AR_MPI_NPU_GetInputTensorParam(handle, u32InputId, &stTensor);
		AR_MPI_NPU_GetInputTensorAddrByName(handle, stNPUInBuff, stTensor.achName, u32BatchId, &stTensorAddr);
	
		stImg->astInputImg[u32InputId].bPreIfcProcess = AR_FALSE;
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

			file.seekg(0, ios::beg);
			fpReadBuff = (AR_CHAR *)malloc(s32FileSize);
			memset(fpReadBuff, 0, s32FileSize);
			file.read((AR_CHAR *)fpReadBuff, s32FileSize);
			file.close();
		}

		AR_U64 u64OffsetAddr = stTensorAddr.u64VirtAddr + GetBatchTensorStep(u32InputId)*u32FrameId;
			

#ifdef NHWC_NCHW_NEON
        AR_NPU_Exe_NchwCvtNhwc_Fix_Neon(fpReadBuff, (AR_CHAR *)u64OffsetAddr, &stTensor);
#else
        AR_NPU_Exe_NchwCvtNhwc_Fix(fpReadBuff, (AR_CHAR *)u64OffsetAddr, &stTensor);
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

    ProfileEnd("GetLocalInput");
    Log(ARCNN_LOG_DEBUG, "Get [%s] to input\n", pchFileName);
    return u32FileType;
}

AR_S32 CARCNN::ReleaseLocalInput(AR_IMG_SET_S * pstImg)
{
    ProfileStart("ReleaseLocalInput");
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

    ProfileEnd("ReleaseLocalInput");
    return 0;
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
    //2022-04-11, Need to invali cache before read output buffer because we malloced a cachable buffer.
    AR_MPI_NPU_InvalidCachedBuff(&stNPUOutBuff);

	//0. dump raw result buffer to file
    AR_CHAR name[64] = {0};
	
    sprintf(name, "%s", "onboard_output.bin");
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
	memset((void*)outputBuff,0,stNPUOutBuff.u64Len);
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

AR_U32 CARCNN::GetBatchTensorStep(AR_U32 u32Index)
{
	return AR_MPI_NPU_GetBatchTensorStep(handle, u32Index);
}

AR_U32 CARCNN::GetBatchNum()
{
    return AR_MPI_NPU_GetBatchNum(handle);
}

AR_U32 CARCNN::GetFrameNum()
{
    return AR_MPI_NPU_GetFrameNum(handle);
}

AR_U32 CARCNN::GetYUVStep(AR_U32 u32Index)
{
   return AR_MPI_NPU_GetYUVStep(handle, u32Index);
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

