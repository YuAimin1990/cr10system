//this is the base class
#include <iostream>
#include <fstream>
#include <string>
#include <sys/time.h>
#include <stdarg.h>
#include "cJSON.h"
#include "opencv2/opencv.hpp"
#include "ar_cnn_channelcvt.h"

#include "mpi_dsp_api.h"

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

	memset(u32IfcPreProcess, 0, sizeof(AR_U32)*MAX_INPUT_IMG_NUM);
    memset(astProfile, 0, sizeof(astProfile));
    memset(stNPUInBuff, 0, MAX_INPUT_NUM * sizeof(AR_MEM_S));
    memset(&stNPURuntimeBuff, 0, sizeof(AR_MEM_S));

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

    SetLogLevel(u32LogLevelCtrl);
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
    return u32IfcPreProcess[u32InputId];
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
//The file name must be same with tensor name for easy test
AR_S32 CARCNN::GetLocalInput(AR_CHAR * pchFileName, AR_IMG_SET_S *stImg, AR_U32 u32BatchId)
{
	AR_IMG_S * pstImg = NULL;
	AR_S32 i = 0;
    AR_S32 s32FileSize = 0;
    static AR_U32 u32FrameId = 0;
	AR_MEM_S stTensorAddr ={0};

    ProfileStart("GetLocalInput");
    AR_U32 u32FileType = GetImageFileType(pchFileName);
    AR_U32 u32Idx = stImg->u32InputNum;
    AR_NPU_TENSOR_S stTensor ={0};
    AR_NPU_TENSOR_EXT_S stTensorExt ={0};
    AR_U32 u32InputTensorNum = AR_MPI_NPU_GetInputTensorNum(handle);
    for(i = 0; i < u32InputTensorNum; i++)
    {
        AR_MPI_NPU_GetInputTensorParam(handle, i, &stTensor);
        AR_MPI_NPU_GetInputTensorExtParam(handle, i, &stTensorExt);
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
        Log(ARCNN_LOG_ERR, "File %s name not match with any tensor!", pchFileName);
        return -1;
    }
    
    //for img file, put them in stImg, for tensor data, fill them into npu input bank_x directly
    if(u32FileType == RGB_RAW_DATA_FILE || u32FileType == BGR_RAW_DATA_FILE || u32FileType == RGBD_RAW_DATA_FILE)
    {
        stImg->astInputImg[u32Idx].bPreIfcProcess = AR_TRUE;
        strcpy(stImg->astInputImg[u32Idx].achTensorName, stTensor.achName);
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
        
            pstImg = &(stImg->astInputImg[u32Idx].astBatchImg[u32BatchId]);
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
            stImg->u32InputNum++;
        }
    }
	else if(u32FileType == JPG_BMP_PNG_IMG_FILE)
    {
        pstImg = &(stImg->astInputImg[u32Idx].astBatchImg[u32BatchId]);
        strcpy(stImg->astInputImg[u32Idx].achTensorName, stTensor.achName);
        stImg->astInputImg[u32Idx].bPreIfcProcess = AR_TRUE;

        //read image and resize it, TBD
        cv::Mat src_img = cv::imread(pchFileName, cv::IMREAD_ANYCOLOR);
        if(src_img.empty())
        {
           Log(ARCNN_LOG_ERR, "Imread file %s failed: %s!\r\n", pchFileName, strerror(errno));
           return -1;
        }

        cv::Mat dst_img;
        
        if(((AR_U32)src_img.cols !=stTensor.u32Width) ||((AR_U32)src_img.rows != stTensor.u32Height))
        {
            cv::resize(src_img,dst_img,Size(stTensor.u32Width, stTensor.u32Height));
            Log(ARCNN_LOG_DEBUG,"resize dst img w/h/c %d %d %d\r\n",dst_img.cols,dst_img.rows,dst_img.channels());
        }
        else
        {
           dst_img = src_img;
        }

        //need to re-order the data according to stride requirements.
        AR_NPU_IFC_PARAM_S stIFCParam = {0};
        int ret = AR_MPI_NPU_GetIFCParamByName(handle, stTensor.achName, &stIFCParam);
        if(ret)
        {
           Log(ARCNN_LOG_ERR, "AR_MPI_NPU_GetIFCParamByName error.\n");
           return -1;
        }
        AR_U16 u16Stride = stIFCParam.u32YStride;
        //printf("Y u16Stride =%d \r\n",u16Stride);	
        
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
                    pchDstBuffVirt[i*u16Stride + j] = pchR[i*dst_img.cols + j];
                }
                for(int j = dst_img.cols;j< u16Stride;j++)
                {
                    pchDstBuffVirt[i*u16Stride + j] = 0;
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
                    pchDstBuffVirt[i*u16Stride + j] = pchR[i*dst_img.cols + j];
                    pchDstBuffVirt[(i+dst_img.rows)*u16Stride + j] = pchG[i*dst_img.cols + j];
                    pchDstBuffVirt[(i+dst_img.rows*2)*u16Stride + j] = pchB[i*dst_img.cols + j];
                }
                for(int j = dst_img.cols;j< u16Stride;j++)
                {
                    pchDstBuffVirt[i*u16Stride + j] = 0;
                    pchDstBuffVirt[(i+dst_img.rows)*u16Stride + j] = 0;
                    pchDstBuffVirt[(i+dst_img.rows*2)*u16Stride + j] = 0;
                }
            }
            AR_U32 u32ChSize = u16Stride * dst_img.rows;
            pstImg->u32FrameId = u32FrameId++;
            pstImg->enFormat= AR_IMG_RGB;
            pstImg->astChannels[0].u32AddrPhy  = (AR_UINTPTR)stPchbuff.u64PhyAddr;
            pstImg->astChannels[0].uptrAddrVirt = (AR_UINTPTR)pchDstBuffVirt;
            pstImg->astChannels[1].u32AddrPhy = (AR_UINTPTR)(stPchbuff.u64PhyAddr+ u32ChSize);
            pstImg->astChannels[1].uptrAddrVirt = (AR_UINTPTR)(pchDstBuffVirt + u32ChSize);
            pstImg->astChannels[2].u32AddrPhy = (AR_UINTPTR)(stPchbuff.u64PhyAddr + 2 * u32ChSize);
            pstImg->astChannels[2].uptrAddrVirt = (AR_UINTPTR)(pchDstBuffVirt + 2 *u32ChSize);
            pstImg->astChannels[3].u32AddrPhy = 0;
        }

        stImg->u32InputNum++;
    }
    else if(u32FileType == FLOAT_RAW_DATA_FILE)
    {
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
        
        AR_CHAR * fpReadBuff = (AR_CHAR *)malloc(s32FileSize);
        memset(fpReadBuff, 0, s32FileSize);
        file.read((AR_CHAR *)fpReadBuff, s32FileSize);
        file.close();
        
        AR_U32 u32TensorSize = stTensor.u32Size * stTensor.u32Precision/8;
        AR_CHAR * pcTemp = (AR_CHAR *)malloc(u32TensorSize);
        memset(pcTemp, 0, u32TensorSize);

         //Fill the tensor data to the right bank
        AR_U32 u32BankId = stTensor.u32Bank;
        
    #ifdef NHWC_NCHW_NEON
        AR_NPU_FloatCvtFix((AR_FLOAT *)fpReadBuff, pcTemp, stTensor.u32Size,&stTensor);
        AR_NPU_Exe_NchwCvtNhwc_Fix_Neon(pcTemp, (AR_CHAR *)stNPUInBuff[u32BankId].u64VirtAddr, &stTensor);
    #else
        AR_NPU_FloatCvtFix((AR_FLOAT *)fpReadBuff, pcTemp, stTensor.u32Size,&stTensor);
        AR_NPU_Exe_NchwCvtNhwc_Fix(pcTemp, (AR_CHAR *)stNPUInBuff[u32BankId].u64VirtAddr, &stTensor);
    #endif
        free(fpReadBuff);
        free(pcTemp);

        //dump tensor data
    #if 0
        FILE * fp1 = NULL;
        char name[32] = {0};
        sprintf(name, "%s.tensor", stTensor.achName);
        fp1 = fopen(name, "wb+");
        fwrite((AR_CHAR *)stNPUInBuff[u32BankId].u64VirtAddr, 1, u32TensorSize, fp1);
        fclose(fp1);
    #endif
        AR_MPI_NPU_FlushCachedBuff(&stNPUInBuff[u32BankId]);
        Log(ARCNN_LOG_DEBUG, "Load %s into bank %d\n", pchFileName, u32BankId);
    }    
    else
    {
        return -1;
    }

    ProfileEnd("GetLocalInput");
    Log(ARCNN_LOG_DEBUG, "Get [%s][%d] to input\n", pchFileName, s32FileSize);
    return 0;
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
    AR_U32 u32BankNum = GetInputNum();
    AR_S32 s32Ret = AR_MPI_NPU_Forward_WithMultiBuff(handle, pstImg, stNPUInBuff, &stNPUOutBuff, u32BankNum, bInstant, bDebug);
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
    //2022-04-11, Need to invali cache before read output buffer because we malloced a cachable buffer.
    AR_MPI_NPU_InvalidCachedBuff(&stNPUOutBuff);

	//0. dump raw result buffer to file
    AR_CHAR name[512] = {0};
	if(!args)
    {
        sprintf(name, "%s",  "npu_output.bin");
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
    AR_MPI_NPU_SetLogLevel(u32LogLevelCtrl);
    
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
    AR_U32 u32BankSize[MAX_INPUT_NUM] = {0};
    AR_U32 u32OutSize = 0, u32RuntimeSize = 0;
    AR_S32 s32Ret = 0;
    AR_S32 i = 0;
    AR_BOOL bEnable = AR_TRUE;

    u32OutSize = AR_MPI_NPU_GetOutputBuffSize(handle);
    if(!u32OutSize)
    {
        Log(ARCNN_LOG_ERR, "Get NPU output size error.\n");
        return -1;
    }

    stNPUOutBuff.u64Len = u32OutSize;
    s32Ret = AR_MPI_NPU_MallocCachedBuff((AR_CHAR *)"output", &stNPUOutBuff);
    if(s32Ret)
    {
        Log(ARCNN_LOG_ERR, "Malloc NPU output error: size %d.\n", u32OutSize);
        return -1;
    }
    memset((void*)stNPUOutBuff.u64VirtAddr,0,stNPUOutBuff.u64Len);
    AR_MPI_NPU_InvalidCachedBuff(&stNPUOutBuff);

    AR_MPI_NPU_GetRuntimeSize(handle, &u32RuntimeSize);
    stNPURuntimeBuff.u64Len = u32RuntimeSize;
    s32Ret = AR_MPI_NPU_MallocBuff((AR_CHAR *)"runtime", &stNPURuntimeBuff);
    if(s32Ret)
    {
        Log(ARCNN_LOG_ERR, "Malloc NPU runtime buff error: size %d.\n", u32RuntimeSize);
        return -1;
    }
    memset((void*)stNPURuntimeBuff.u64VirtAddr,0,stNPURuntimeBuff.u64Len);

    //set runtime buffer to npu
    AR_MPI_NPU_SetRuntimeBuffer(handle, (AR_UINTPTR)stNPURuntimeBuff.u64VirtAddr, stNPURuntimeBuff.u64PhyAddr);
    
    for(i = 0; i < MAX_INPUT_NUM; i++)
    {
        u32BankSize[i] = AR_MPI_NPU_GetInputBankSize(handle, i);
        if(!u32BankSize[i])
        {
            continue;
        }
        
        stNPUInBuff[i].u64Len = u32BankSize[i];
        s32Ret = AR_MPI_NPU_MallocCachedBuff((AR_CHAR *)"input", &stNPUInBuff[i]);
        if(s32Ret)
        {
            Log(ARCNN_LOG_ERR, "Malloc NPU input bank %d error: size %d.\n", i, u32BankSize[i]);
            AR_MPI_NPU_FreeBuff(&stNPUOutBuff);
            AR_MPI_NPU_FreeBuff(&stNPURuntimeBuff);
            return -1;
        }
        Log(ARCNN_LOG_DEBUG, "Malloc NPU input bank %d at 0x%x: size %d.\n", i, stNPUInBuff[i].u64PhyAddr, u32BankSize[i]);
        memset((void*)stNPUInBuff[i].u64VirtAddr,0,stNPUInBuff[i].u64Len);
    }

    return 0;
}

AR_S32 CARCNN::FreeNPUBuff()
{
    AR_S32 s32Ret = 0;
    AR_S32 i = 0;

    for(i = 0; i < MAX_INPUT_NUM; i++)
    {
        if(stNPUInBuff[i].u64Len == 0)
        {
            continue;
        }
        
        s32Ret = AR_MPI_NPU_FreeBuff(&stNPUInBuff[i]);
        if(s32Ret)
        {
            Log(ARCNN_LOG_ERR, "Free NPU input buffer %d error.\n", i);
        }
    }
    

    s32Ret = AR_MPI_NPU_FreeBuff(&stNPUOutBuff);
    if(s32Ret)
    {
        Log(ARCNN_LOG_ERR, "Free NPU output buffer error.\n");
    }

    s32Ret = AR_MPI_NPU_FreeBuff(&stNPURuntimeBuff);
    if(s32Ret)
    {
        Log(ARCNN_LOG_ERR, "Free NPU runtime buffer error.\n");
    }

    return s32Ret;
}

