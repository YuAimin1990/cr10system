#include <sys/prctl.h>
#include "ar_comm_vpss.h"
#include "pf_svp.h"
#include "pf_osd.h"
#include "cfg_common.h"
#include "ar_cnn_parser.h"
#include "ar_cnn_operator.h"
#include "mpi_type.h"
#include "hal_ifc_api.h"
#include "mpi_scaler_api.h"
#include "mpi_sys.h"
#include "mpi_vpss.h"
#include "pf_osd.h"
#include "ssd.h"

static AR_NPU_CNN_DESC_S stCNNDesc;
static AR_S32 s32SvpThreadExit = 0;
static AR_S32 g_s32SvpSuspend = 0;
extern pthread_mutex_t g_VpssMutex[IPC_MAX_VPSS_GRP_NEEDED * IPC_MAX_STREAM_PER_PIPE];

static AR_S32 IPC_PF_SVP_GetLiveImage(void *pvNpuHandle, AR_IMG_S *pstLiveImg)
{
	AR_S32 ret = 0;
	VIDEO_FRAME_INFO_S VFrameInfo = {0};
	VPSS_GRP VpssGrp = 0;
	VPSS_CHN VpssChn = 0;
	AR_IMG_S pstSrcImg = {0};
	AR_NPU_TENSOR_S stTensor ={0};
	AR_NPU_IFC_PARAM_S stIFCParam = {0};
	AR_HAL_SCALER_CROP_S stCrop = {0};
	IPC_CFG_SVP_S *pstSvpCfg = IPC_CFG_SVP_GetParam();

	VpssGrp = pstSvpCfg->u32VpssGrp;
	VpssChn = pstSvpCfg->u32VpssChn;

	ret = AR_MPI_NPU_GetInputTensorParam(pvNpuHandle, 0, &stTensor);
	if(ret)
	{
	   PRINT_ERR("AR_MPI_NPU_GetInputTensorParam error.\n");
	   return -1;
	}
	
	ret = AR_MPI_NPU_GetIFCParamByName(pvNpuHandle, stTensor.achName, &stIFCParam);
	if(ret)
	{
	   PRINT_ERR("AR_MPI_NPU_GetIFCParamByName error.\n");
	   return -1;
	}

	AR_U32 u32DstWidth = stIFCParam.u32YuvWidth;
	AR_U32 u32DstHeight = stIFCParam.u32YuvHeight;
	AR_U32 u32DstStride = stIFCParam.u32YStride;

    void * pDstAddrVirt = NULL;
    AR_U64 pDstAddrPhy = 0;  
    AR_U32 u32Size = u32DstStride * u32DstHeight * 3/2;
    AR_MPI_SYS_MmzAlloc(&pDstAddrPhy, &pDstAddrVirt, (AR_CHAR *)"ScalerDst", NULL, u32Size);
    if(!pDstAddrVirt)
    {
        PRINT_ERR("Malloc DstBuffer failed, buffer_size: %d \n", u32Size);
        return -1;
    }

    memset(&VFrameInfo,0,sizeof(VIDEO_FRAME_INFO_S));
    ret = AR_MPI_VPSS_GetChnFrame(VpssGrp, VpssChn, &VFrameInfo, -1);
    if(ret<0)
    {
        AR_MPI_SYS_MmzFree((AR_U64)pDstAddrPhy, (void *)pDstAddrVirt);
        PRINT_ERR("AR_MPI_VPSS_GetChnFrame ERR ret=%x\r\n", ret);
        return -1;
    }
    
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
    
    pstLiveImg->u32FrameId = VFrameInfo.stVFrame.u32FrameFlag;
    pstLiveImg->enFormat = AR_IMG_YUV420P;
    pstLiveImg->u32Width = u32DstWidth;
    pstLiveImg->u32Height = u32DstHeight;
    pstLiveImg->u32ChannelNum = 3;
    pstLiveImg->astChannels[0].u32Stride = u32DstStride;
    pstLiveImg->astChannels[1].u32Stride = u32DstStride/2;
    pstLiveImg->astChannels[2].u32Stride = u32DstStride/2;   
    pstLiveImg->astChannels[0].u32AddrPhy = (AR_UINTPTR)pDstAddrPhy;
    pstLiveImg->astChannels[0].uptrAddrVirt = (AR_UINTPTR)pDstAddrVirt;
    pstLiveImg->astChannels[1].u32AddrPhy = (AR_UINTPTR)(pDstAddrPhy + u32DstStride * u32DstHeight);
    pstLiveImg->astChannels[1].uptrAddrVirt = (AR_UINTPTR)((AR_CHAR *)pDstAddrVirt + u32DstStride * u32DstHeight);
    pstLiveImg->astChannels[2].u32AddrPhy = (AR_UINTPTR)(pDstAddrPhy + u32DstStride * u32DstHeight + u32DstStride * u32DstHeight/4 );
    pstLiveImg->astChannels[2].uptrAddrVirt = (AR_UINTPTR)((AR_CHAR *)pDstAddrVirt + u32DstStride * u32DstHeight + u32DstStride * u32DstHeight/4 );
    
    stCrop.u32X = 0;
    stCrop.u32Y = 0;
    stCrop.u32W = pstSrcImg.u32Width;
    stCrop.u32H = pstSrcImg.u32Height;

	//printf("crop x:%d, y:%d, w:%d, h:%d\n", stCrop.u32X, stCrop.u32Y, stCrop.u32W, stCrop.u32H); 
	//printf("src w:%d, h:%d, s:%d\n", pstSrcImg.u32Width, pstSrcImg.u32Height, pstSrcImg.astChannels[0].u32Stride);	
	//printf("dst w:%d, h:%d, s:%d\n", pstLiveImg->u32Width, pstLiveImg->u32Height, pstLiveImg->astChannels[0].u32Stride);
	
    AR_S32 s32Ret = AR_MPI_SCALER_CropResize(&pstSrcImg, &stCrop, pstLiveImg, 1, 1);
    if(s32Ret < 0)
    {
        AR_MPI_SYS_MmzFree((AR_U64)pstLiveImg->astChannels[0].u32AddrPhy, (void *)pstLiveImg->astChannels[0].uptrAddrVirt);
        pstLiveImg->astChannels[0].u32AddrPhy = 0;
        pstLiveImg->astChannels[0].uptrAddrVirt = 0;
        AR_MPI_VPSS_ReleaseChnFrame(VpssGrp, VpssChn, &VFrameInfo);
        PRINT_ERR("Do crop/resize error.\n");
        return -1;
    }
    
    ret = AR_MPI_VPSS_ReleaseChnFrame(VpssGrp, VpssChn, &VFrameInfo);
    if(ret < 0)
    {
        AR_MPI_SYS_MmzFree((AR_U64)pstLiveImg->astChannels[0].u32AddrPhy, (void *)pstLiveImg->astChannels[0].uptrAddrVirt);
        pstLiveImg->astChannels[0].u32AddrPhy = 0;
        pstLiveImg->astChannels[0].uptrAddrVirt = 0;
        PRINT_ERR("AR_MPI_VPSS_ReleaseChnFrame ERR ret=%x.\n",ret);
        return -1;
    }

#if 0
	if(cnt < 2)
	{
    	FILE * fp1 = NULL;
        char name[32] = {0};
        sprintf(name, "/mnt/live_img_%d.yuv", cnt);
        fp1 = fopen(name, "wb+");
        fwrite((AR_CHAR *)pstLiveImg->astChannels[0].uptrAddrVirt, 1, u32Size, fp1);
        fclose(fp1);
		cnt++;
	}
#endif	
    return 0;
}
static AR_S32 IPC_PF_SVP_GetLocalImageTest(void *pvNpuHandle, AR_IMG_S *pstLiveImg)
{
	AR_S32 ret = 0;
	//AR_IMG_S pstSrcImg = {0};
	AR_NPU_TENSOR_S stTensor ={0};
	AR_NPU_IFC_PARAM_S stIFCParam = {0};
	//AR_HAL_SCALER_CROP_S stCrop = {0};
	//IPC_CFG_SVP_S *pstSvpCfg = IPC_CFG_SVP_GetParam();

	ret = AR_MPI_NPU_GetInputTensorParam(pvNpuHandle, 0, &stTensor);
	if(ret)
	{
	   PRINT_ERR("AR_MPI_NPU_GetInputTensorParam error.\n");
	   return -1;
	}
	
	ret = AR_MPI_NPU_GetIFCParamByName(pvNpuHandle, stTensor.achName, &stIFCParam);
	if(ret)
	{
	   PRINT_ERR("AR_MPI_NPU_GetIFCParamByName error.\n");
	   return -1;
	}

	AR_U32 u32DstWidth = stIFCParam.u32YuvWidth;
	AR_U32 u32DstHeight = stIFCParam.u32YuvHeight;
	AR_U32 u32DstStride = stIFCParam.u32YStride;

    void * pDstAddrVirt = NULL;
    AR_U64 pDstAddrPhy = 0;  
    AR_U32 u32Size = u32DstStride * u32DstHeight * 3/2;
    AR_MPI_SYS_MmzAlloc(&pDstAddrPhy, &pDstAddrVirt, (AR_CHAR *)"ScalerDst", NULL, u32Size);
    if(!pDstAddrVirt)
    {
        PRINT_ERR("Malloc DstBuffer failed, buffer_size: %d \n", u32Size);
        return -1;
    }

	pstLiveImg->u32FrameId = 1;
    pstLiveImg->enFormat = AR_IMG_YUV420P;
    pstLiveImg->u32Width = u32DstWidth;
    pstLiveImg->u32Height = u32DstHeight;
    pstLiveImg->u32ChannelNum = 3;
    pstLiveImg->astChannels[0].u32Stride = u32DstStride;
    pstLiveImg->astChannels[1].u32Stride = u32DstStride/2;
    pstLiveImg->astChannels[2].u32Stride = u32DstStride/2;   
    pstLiveImg->astChannels[0].u32AddrPhy = (AR_UINTPTR)pDstAddrPhy;
    pstLiveImg->astChannels[0].uptrAddrVirt = (AR_UINTPTR)pDstAddrVirt;
    pstLiveImg->astChannels[1].u32AddrPhy = (AR_UINTPTR)(pDstAddrPhy + u32DstStride * u32DstHeight);
    pstLiveImg->astChannels[1].uptrAddrVirt = (AR_UINTPTR)((AR_CHAR *)pDstAddrVirt + u32DstStride * u32DstHeight);
    pstLiveImg->astChannels[2].u32AddrPhy = (AR_UINTPTR)(pDstAddrPhy + u32DstStride * u32DstHeight + u32DstStride * u32DstHeight/4 );
    pstLiveImg->astChannels[2].uptrAddrVirt = (AR_UINTPTR)((AR_CHAR *)pDstAddrVirt + u32DstStride * u32DstHeight + u32DstStride * u32DstHeight/4 );

	FILE * fp1 = NULL;
    //char name[32] = {0};
    fp1 = fopen("/mnt/ssd_test.yuv", "rb+");
    fread((AR_CHAR *)pstLiveImg->astChannels[0].uptrAddrVirt, 1, u32Size, fp1);
    fclose(fp1);

	return 0;
}


static void IPC_PF_SVP_ReleaseLiveImage(AR_IMG_S *pstLiveImg)
{
    if(pstLiveImg->astChannels[0].u32AddrPhy)
    {
        AR_MPI_SYS_MmzFree((AR_U64)pstLiveImg->astChannels[0].u32AddrPhy, (AR_VOID *)pstLiveImg->astChannels[0].uptrAddrVirt);
    }
}


AR_S32 IPC_PF_SVP_NpuMemMalloc(void *pvNpuHandle, AR_MEM_S *pstNpuIn, AR_MEM_S *pstNpuOut)
{
    AR_S32 s32Ret = 0;
    
    pstNpuIn->u64Len = AR_MPI_NPU_GetInputBuffSize(pvNpuHandle);
    if(!pstNpuIn->u64Len)
    {
        PRINT_ERR("Get NPU input size error.\n");
        return -1;
    }
    
    s32Ret = AR_MPI_NPU_MallocCachedBuff((AR_CHAR *)"input", pstNpuIn);
    if(s32Ret)
    {
        PRINT_ERR("Malloc NPU input error: size %lld.\n", pstNpuIn->u64Len);
        return -1;
    }
    
    pstNpuOut->u64Len = AR_MPI_NPU_GetOutputBuffSize(pvNpuHandle);
    if(!pstNpuOut->u64Len)
    {
        PRINT_ERR("Get NPU output size error.\n");
        AR_MPI_NPU_FreeBuff(pstNpuIn);
        return -1;
    }
    
    s32Ret = AR_MPI_NPU_MallocCachedBuff((AR_CHAR *)"output", pstNpuOut);
    if(s32Ret)
    {
        PRINT_ERR("Malloc NPU output error: size %lld.\n", pstNpuOut->u64Len);
        AR_MPI_NPU_FreeBuff(pstNpuIn);
        return -1;
    }
    
    memset((void*)pstNpuIn->u64VirtAddr, 0,  pstNpuIn->u64Len);
    memset((void*)pstNpuOut->u64VirtAddr, 0,  pstNpuOut->u64Len);
    AR_MPI_NPU_InvalidCachedBuff(pstNpuOut);
    
    return 0;
}

void IPC_PF_SVP_NpuMemFree(AR_MEM_S *pstNpuIn, AR_MEM_S *pstNpuOut)
{
    if(pstNpuIn->u64PhyAddr)
    {
        AR_MPI_SYS_MmzFree(pstNpuIn->u64PhyAddr, (AR_VOID *)pstNpuIn->u64VirtAddr);
        memset(pstNpuIn, 0, sizeof(AR_MEM_S));
    }
    
    if(pstNpuOut->u64PhyAddr)
    {
        AR_MPI_SYS_MmzFree(pstNpuOut->u64PhyAddr, (AR_VOID *)pstNpuOut->u64VirtAddr);
        memset(pstNpuOut, 0, sizeof(AR_MEM_S));
    }
}

AR_S32 IPC_PF_SVP_NpuParseCallback(void *pvHandle, AR_CHAR * pcCBParseName, AR_NPU_ParseFunc pCBParseFunc)
{
    return AR_MPI_NPU_RegisterParseFunc(pvHandle, pcCBParseName, pCBParseFunc);
}

AR_S32 IPC_PF_SVP_NpuRegisterCallback(void *pvHandle, AR_CHAR * pcCBName, AR_NPU_CallbackFunc pCBFunc)
{
    return AR_MPI_NPU_RegisterCallback(pvHandle, pcCBName, pCBFunc);
}

AR_S32 IPC_PF_SVP_NpuForward(void *pvNpuHandle, AR_IMG_SET_S *pstImg, AR_MEM_S *pstNpuInBuff, AR_MEM_S *pstNpuOutBuff)
{
    AR_S32 s32Ret = 0; 
    
    s32Ret = AR_MPI_NPU_Forward(pvNpuHandle, pstImg, pstNpuInBuff, pstNpuOutBuff, 1, 0);
    
    return s32Ret;
}

AR_S32 IPC_PF_SVP_NpuPostProcess(void * handle, AR_MEM_S *stNpuOutBuffer)
{
	AR_S32 s32Ret = 0;
	IPC_CFG_SVP_S *pstSvpCfg = IPC_CFG_SVP_GetParam();

    if(strcmp(pstSvpCfg->achNetName, "ssd") == 0)
    {
    	IPC_SSD_OUTPUT_S stSsdOutput;
    	memset(&stSsdOutput, 0, sizeof(IPC_SSD_OUTPUT_S));
    	s32Ret = IPC_SVP_GetSsdPostProcessResult(handle, stNpuOutBuffer, &stSsdOutput);
    	if(s32Ret < 0)
    	{
    		PRINT_ERR("IPC_SVP_GetSsdPostProcessResult failed .\n");
    		return -1;
    	}
    
    	#if 1
    	IPC_SSD_OUTPUT_INFO_S *ssdTmp = stSsdOutput.pstSsdInfo;
    	if(pstSvpCfg->u32OsdEnable && stSsdOutput.u32OutNum)
        {
    	   IPC_OSD_STRING_S stOsdStr[IPC_ALG_OSD_MAX_NUM] = {0};
    	   IPC_OSD_RECT_S stOsdRect[IPC_ALG_OSD_MAX_NUM] = {0};
    
    	   for(AR_U32 strID=0; strID < stSsdOutput.u32OutNum; strID++)
    	   {
    		   strcpy(stOsdStr[strID].achStr, ssdTmp->scores);
    		   stOsdStr[strID].u32X = ssdTmp->x;
    		   stOsdStr[strID].u32Y = ssdTmp->y;
               stOsdStr[strID].u32RGB = 0x00FF0000; //red, but you can change the color according to different class.
    		   ssdTmp++;
    	   }
    
    	   ssdTmp = stSsdOutput.pstSsdInfo;
    
    	   for(AR_U32 recID=0; recID < stSsdOutput.u32OutNum; recID++)
    	   {
    		   stOsdRect[recID].u32X = ssdTmp->x;
    		   stOsdRect[recID].u32Y = ssdTmp->y;
    		   stOsdRect[recID].u32W = ssdTmp->w;
    		   stOsdRect[recID].u32H = ssdTmp->h;
               stOsdRect[recID].u32RGB = 0x00FF;//blue
    		   ssdTmp++;
    	   }
    	   
    	   AR_U32 u32ImgHeight = 300;
    	   AR_U32 u32ImgWidth = 300;
    
    	   IPC_PF_OSD_DrawString(pstSvpCfg->u32OsdGrp, stSsdOutput.u32OutNum, stOsdStr, u32ImgWidth, u32ImgHeight);
    	   IPC_PF_OSD_DrawRect(pstSvpCfg->u32OsdGrp, stSsdOutput.u32OutNum, stOsdRect, u32ImgWidth, u32ImgHeight);
        }
    	#endif
    	IPC_SVP_ReleaseSsdPostProcessResult(&stSsdOutput);
 	}
	return 0;
}

void IPC_PF_SVP_NpuCBRegister(void *pvHandle)
{
    // cb parser func
    IPC_PF_SVP_NpuParseCallback(pvHandle, (AR_CHAR *)"lrn", AR_CNN_ParseLrn);
    IPC_PF_SVP_NpuParseCallback(pvHandle, (AR_CHAR *)"maxunpool", AR_CNN_ParseMaxUnpool);
    IPC_PF_SVP_NpuParseCallback(pvHandle, (AR_CHAR *)"upsample", AR_CNN_ParseUpsample);
    IPC_PF_SVP_NpuParseCallback(pvHandle, (AR_CHAR *)"deformable", AR_CNN_ParseDeformable);
    IPC_PF_SVP_NpuParseCallback(pvHandle, (AR_CHAR *)"sevcallback", AR_CNN_ParseSevCallback);
    IPC_PF_SVP_NpuParseCallback(pvHandle, (AR_CHAR *)"feature_to_weight", AR_CNN_ParseFeatureToWeight);
    IPC_PF_SVP_NpuParseCallback(pvHandle, (AR_CHAR *)"pad", AR_CNN_ParsePad);
    IPC_PF_SVP_NpuParseCallback(pvHandle, (AR_CHAR *)"permute", AR_CNN_ParsePermute);
    IPC_PF_SVP_NpuParseCallback(pvHandle, (AR_CHAR *)"Transpose", AR_CNN_ParseTranspose);
    IPC_PF_SVP_NpuParseCallback(pvHandle, (AR_CHAR *)"Concat", AR_CNN_ParseConcat);
    IPC_PF_SVP_NpuParseCallback(pvHandle, (AR_CHAR *)"art_add", AR_CNN_ParseCustomAdd);
    //usr parser json func
    IPC_PF_SVP_NpuParseCallback(pvHandle, (AR_CHAR *)"LRN", AR_CNN_ParseCustomLrn);
    
    //cb inference func
    IPC_PF_SVP_NpuRegisterCallback(pvHandle, (AR_CHAR *)"lrn", AR_CNN_OperatorLrn);
    IPC_PF_SVP_NpuRegisterCallback(pvHandle, (AR_CHAR *)"getconfidence", AR_CNN_OperatorGetConf);
    IPC_PF_SVP_NpuRegisterCallback(pvHandle, (AR_CHAR *)"maxunpool", AR_CNN_OperatorMaxUnPool);
    IPC_PF_SVP_NpuRegisterCallback(pvHandle, (AR_CHAR *)"reshape", AR_CNN_OperatorReshape);
    IPC_PF_SVP_NpuRegisterCallback(pvHandle, (AR_CHAR *)"Reshape", AR_CNN_OperatorOnnxReshape);
    IPC_PF_SVP_NpuRegisterCallback(pvHandle, (AR_CHAR *)"upsample", AR_CNN_OperatorUpsample);
    IPC_PF_SVP_NpuRegisterCallback(pvHandle, (AR_CHAR *)"reciprocal", AR_CNN_OperatorReciprocal);
    IPC_PF_SVP_NpuRegisterCallback(pvHandle, (AR_CHAR *)"deformable", AR_CNN_OperatorDeformable);
    IPC_PF_SVP_NpuRegisterCallback(pvHandle, (AR_CHAR *)"sevcallback", AR_CNN_OperatorSevCallback);
    IPC_PF_SVP_NpuRegisterCallback(pvHandle, (AR_CHAR *)"feature_to_weight", AR_CNN_OperatorFeatureToWeight);
    IPC_PF_SVP_NpuRegisterCallback(pvHandle, (AR_CHAR *)"pad", AR_CNN_OperatorPad);
    IPC_PF_SVP_NpuRegisterCallback(pvHandle, (AR_CHAR *)"permute", AR_CNN_OperatorPermute);
    IPC_PF_SVP_NpuRegisterCallback(pvHandle, (AR_CHAR *)"Transpose", AR_CNN_OperatorTranspose);
    IPC_PF_SVP_NpuRegisterCallback(pvHandle, (AR_CHAR *)"Concat", AR_CNN_OperatorConcat);
    IPC_PF_SVP_NpuRegisterCallback(pvHandle, (AR_CHAR *)"art_add", AR_CNN_OperatorCustomAdd);
    // usr-cb inference func
    IPC_PF_SVP_NpuRegisterCallback(pvHandle, (AR_CHAR *)"LRN", AR_CNN_OperatorCustomLrn);
}

static void* IPC_PF_SVP_NpuThread(void *args)
{
    AR_S32 s32Ret = 0;
    AR_NPU_CNN_DESC_S stCNNDesc = {0};
    AR_CHAR achName[32] = {0};
    void * pvHandle = NULL;
    AR_MEM_S stNpuBufferIn = {0};
    AR_MEM_S stNpuBufferOut = {0};
	AR_IMG_SET_S stImg = {0};

    strcpy(achName, "svp_thread");
    prctl(PR_SET_NAME, achName, 0,0,0);
    
    PRINT_INFO("IPC_PF_SVP_NpuThread start.\n");
    
    memcpy(&stCNNDesc, args, sizeof(AR_NPU_CNN_DESC_S));
    pvHandle = AR_MPI_NPU_LoadModel(&stCNNDesc);
    if(!pvHandle)
    {
        PRINT_ERR("Load Model failed.\n");
        return NULL;
    }

    IPC_PF_SVP_NpuCBRegister(pvHandle);
    
    s32Ret = IPC_PF_SVP_NpuMemMalloc(pvHandle, &stNpuBufferIn, &stNpuBufferOut);
    if(s32Ret < 0)
    {
        PRINT_ERR("IPC_PF_SVP_MallocNpuMem failed.\n");
        AR_MPI_NPU_UnloadModel(pvHandle);
        return NULL;
    }
  
    while(1)
    {
        if(s32SvpThreadExit)
        {
            break;
        }

        if(g_s32SvpSuspend)
        {
            usleep(500*1000);
            continue;
        }
        
        memset(&stImg, 0, sizeof(AR_IMG_SET_S));
        
        stImg.u32InputNum = 1;
        stImg.astInputImg[0].bPreIfcProcess = AR_TRUE;
        s32Ret = IPC_PF_SVP_GetLiveImage(pvHandle, &stImg.astInputImg[0].astBatchImg[0]);
        //s32Ret = IPC_PF_SVP_GetLocalImageTest(pvHandle, &stImg.astInputImg[0].astBatchImg[0]);
        if(s32Ret < 0)
        {
            PRINT_ERR("IPC_PF_SVP_GetLiveImage failed.\n");
            IPC_PF_SVP_ReleaseLiveImage(&stImg.astInputImg[0].astBatchImg[0]);
            break;
        }
        s32Ret = IPC_PF_SVP_NpuForward(pvHandle, &stImg, &stNpuBufferIn, &stNpuBufferOut);
        if(s32Ret < 0)
        {
            PRINT_ERR("IPC_PF_SVP_NpuForward failed.\n");
            IPC_PF_SVP_ReleaseLiveImage(&stImg.astInputImg[0].astBatchImg[0]);
            break;
        }
  
        IPC_PF_SVP_ReleaseLiveImage(&stImg.astInputImg[0].astBatchImg[0]);
#if 1
		s32Ret = IPC_PF_SVP_NpuPostProcess(pvHandle, &stNpuBufferOut);
		if(s32Ret < 0)
        {
            PRINT_ERR("IPC_PF_SVP_NpuPostProcess failed.\n");
            break;
        }
#endif
    }

    IPC_PF_SVP_NpuMemFree(&stNpuBufferIn, &stNpuBufferOut);
    
    s32Ret = AR_MPI_NPU_UnloadModel(pvHandle);
    if(s32Ret < 0)
    {
        PRINT_ERR("UnLoad Model failed.\n");
        return NULL;
    }
    
    PRINT_INFO("IPC_PF_SVP_NpuThread exit.\n");
    
    return NULL;
}

AR_S32 IPC_PF_SVP_Init()
{
    AR_S32 s32Ret = 0;
    VPSS_GRP VpssGrp = 0;
    VPSS_CHN VpssChn = 0;
    VI_PIPE ViPipe = 0;
    VPSS_CHN_ATTR_S    astVpssChnAttr = {0};
    VPSS_CROP_INFO_S   stCrop = {0};
    IPC_CFG_SVP_S *pstSvpCfg = IPC_CFG_SVP_GetParam();
    IPC_CFG_VPSS_S * pstVpssCfg = IPC_CFG_VPSS_GetParam();
    IPC_CFG_VIO_S * pstVioCfg = IPC_CFG_VIO_GetParam();
    
    memset(&stCNNDesc, 0, sizeof(AR_NPU_CNN_DESC_S));
    stCNNDesc.u16NetworkID = 123;
    stCNNDesc.u32CBToArm = 1;
    strcpy(stCNNDesc.au8NpubinFileName, pstSvpCfg->achNpuBinName);

    if(!pstSvpCfg->u32Enable)
    {
    	PRINT_INFO("not enable svp .\n");
        return 0;
    }

    VpssGrp = pstSvpCfg->u32VpssGrp;
    VpssChn = pstSvpCfg->u32VpssChn;
    ViPipe = VpssGrp/2;
    //enable vpss grp/chn if not
    if(!pstVpssCfg->stGrpCfg[VpssGrp].stChnCfg[VpssChn].bEnable)
    {		
        astVpssChnAttr.bFlip = 0;
        astVpssChnAttr.bMirror = 0;
        astVpssChnAttr.enChnMode = VPSS_CHN_MODE_USER;
        astVpssChnAttr.u32Width = pstVioCfg->vi[ViPipe].channel_1_width;
        astVpssChnAttr.u32Height = pstVioCfg->vi[ViPipe].channel_1_height;
        astVpssChnAttr.enCompressMode = COMPRESS_MODE_NONE;
        astVpssChnAttr.enDynamicRange = DYNAMIC_RANGE_SDR8;
        astVpssChnAttr.enPixelFormat = PIXEL_FORMAT_YVU_PLANAR_420;
        astVpssChnAttr.enVideoFormat = VIDEO_FORMAT_LINEAR;
        astVpssChnAttr.stAspectRatio.enMode = ASPECT_RATIO_NONE;
        astVpssChnAttr.stFrameRate.s32SrcFrameRate = pstVioCfg->vi[ViPipe].channel_1_fps;
        astVpssChnAttr.stFrameRate.s32DstFrameRate = pstVioCfg->vi[ViPipe].channel_1_fps;
        astVpssChnAttr.u32Depth = 1;
        astVpssChnAttr.u32ScaleMode = 1; //1 bilinear mode
        
        s32Ret = AR_MPI_VPSS_SetChnAttr(VpssGrp, VpssChn, &astVpssChnAttr);
        if (s32Ret != AR_SUCCESS)
        {
            PRINT_ERR("AR_MPI_VPSS_SetChnAttr failed with %#x\n", s32Ret);
            return -1;
        }
        
        s32Ret = AR_MPI_VPSS_EnableChn(VpssGrp, VpssChn);
        if (s32Ret != AR_SUCCESS)
        {
            PRINT_ERR("AR_MPI_VPSS_EnableChn failed with %#x\n", s32Ret);
            return -1;
        }
        
        pthread_mutex_init(&g_VpssMutex[VpssGrp * IPC_MAX_STREAM_PER_PIPE + VpssChn], 0);
        
        stCrop.bEnable = 0;
        stCrop.enCropCoordinate = VPSS_CROP_ABS_COOR;
        stCrop.stCropRect.s32X = 0;
        stCrop.stCropRect.s32Y = 0;
        stCrop.stCropRect.u32Width = pstVioCfg->vi[ViPipe].channel_1_width;
        stCrop.stCropRect.u32Height = pstVioCfg->vi[ViPipe].channel_1_height;
        
        s32Ret = AR_MPI_VPSS_SetChnCrop(VpssGrp, VpssChn, &stCrop);
        if(s32Ret != AR_SUCCESS)
        {
           PRINT_ERR("Set vpss grp %d chn %d crop attr failed.\n", VpssGrp, VpssChn);
           return -1;
        }
	}

	s32SvpThreadExit = 0;
    s32Ret = IPC_MID_UTIL_CreateDetachThread(IPC_PF_SVP_NpuThread, &stCNNDesc, NULL);
    if(s32Ret < 0)
    {
        PRINT_ERR("IPC_MID_UTIL_CreateDetachThread failed\n");
        return -1;
    }
    
    return 0;
}

AR_S32 IPC_PF_SVP_UnInit()
{
    AR_S32 s32Ret = 0;
    VPSS_GRP VpssGrp = 0;
    VPSS_CHN VpssChn = 0;
    IPC_CFG_SVP_S *pstSvpCfg = IPC_CFG_SVP_GetParam();
    IPC_CFG_VPSS_S * pstVpssCfg = IPC_CFG_VPSS_GetParam();

	if(!pstSvpCfg->u32Enable)
    {
    	PRINT_INFO("not enable svp .\n");
        return 0;
    }
    
    s32SvpThreadExit = 1;
    
    VpssGrp = pstSvpCfg->u32VpssGrp;
    VpssChn = pstSvpCfg->u32VpssChn;
    if(!pstVpssCfg->stGrpCfg[VpssGrp].stChnCfg[VpssChn].bEnable)
    {
        s32Ret = AR_MPI_VPSS_DisableChn(VpssGrp, VpssChn);
        if (s32Ret != AR_SUCCESS)
        {
            PRINT_ERR("failed with %#x!\n", s32Ret);
            return -1;
        }
    }
    
    return 0;
}

void IPC_PF_SVP_Suspend()
{
	g_s32SvpSuspend = 1;
	usleep(500*1000);
	IPC_PF_SVP_UnInit();
	printf("IPC_PF_SVP_Suspend. \n");
}

void IPC_PF_SVP_Resume()
{
	IPC_PF_SVP_Init();

	g_s32SvpSuspend = 0;
	printf("IPC_PF_SVP_Resume. \n");
}

