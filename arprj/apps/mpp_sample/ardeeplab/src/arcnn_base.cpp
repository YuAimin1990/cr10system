#include <iostream>
#include <fstream>
#include <string>
#include <sys/time.h>
#include <stdarg.h>
#include "cJSON.h"
#include "opencv2/opencv.hpp"
#include "ar_cnn_channelcvt.h"
#include <arm_neon.h>
#include "mpi_region.h"
#include "mpi_vgs.h"
#include "sample_comm.h"

#include "mpi_scaler_api.h"

#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "hal_sys.h"
#include "hal_vo.h"
#include "hal_vin_log.h"
#include "hal_region.h"
#include "hal_dbglog.h"
#include "mpi_vb.h"
extern "C"{
#include "osal.h"
}

using namespace std;
using namespace cv;

#include "arcnn_base.h"
#include "arcnn_common.h"

AR_UCHAR rr[22] = {0, 128, 0,	128, 0,   128, 0,	128, 64, 192, 64,  192, 64,  192, 64,  192, 0,	128, 0,   128, 0,	128};
AR_UCHAR gg[22] = {0, 0,   128, 128, 0,   0,   128, 128, 0,   0,  128, 128, 0,	 0,   128, 128, 64, 64,  192, 192, 64,	64};
AR_UCHAR bb[22] = {0, 0,   0,	0,	 128, 128, 128, 128, 0,   0,  0,   0,	128, 128, 128, 128, 0,	0,	 0,   0,   128, 12};

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
        stCNNDesc.u16NetworkID = 0xFFFF;	//Alloc id by driver
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

	
	cJSON * aChnInit = cJSON_GetObjectItemCaseSensitive(pNet, "aChn_InitValue");
	if(!aChnInit)
	{
	   Log(ARCNN_LOG_ERR, "Parse aChn_InitValue node failed.SET DEFAULT 128\n");
       u32aChnInitValue = 128;
	}
	else
	{
		u32aChnInitValue= atoi(aChnInit->valuestring);
	}

	cJSON * writeBin = cJSON_GetObjectItemCaseSensitive(pNet, "dump_bin");
	if(!writeBin)
	{
	   Log(ARCNN_LOG_ERR, "Parse aChn_InitValue node failed.SET DEFAULT 0\n");
       u32dumpBin = 0;
	}
	else
	{
		u32dumpBin= atoi(writeBin->valuestring);
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

	if(u32Mode == ARCNN_INPUT_LIVE)
	{
		AR_S32 	s32Ret = 0;
		u32ImgWidth 	= 1920;
		u32ImgHeight 	= 1080;
		u32InputWidth 	= 513;
		u32InputHeight 	= 513;
		u32NetOutHeight = 512;
		u32NetOutWidth 	= 512;

		u32SrcRgbStride = ALIGNED_256B(u32ImgWidth);
		if(AR_MPI_NPU_GetSocVersion()==2)
			u32SrcRgbStride = ALIGNED_16B(u32ImgWidth);
		u32SrcRgbSize = 3 * u32SrcRgbStride * u32ImgHeight;		
		SrcRgbBuff.u64Len = u32SrcRgbSize;
		s32Ret = AR_MPI_NPU_MallocBuff((AR_CHAR*)"RgbBuff", &SrcRgbBuff);
		if(s32Ret)
		{
			Log(ARCNN_LOG_ERR, "Malloc RgbBuff buff failed %d.", s32Ret);
		}
		memset((AR_CHAR *)SrcRgbBuff.u64VirtAddr, 0, u32SrcRgbSize);


		u32DstRgbStride = ALIGNED_256B(u32InputWidth);
		if(AR_MPI_NPU_GetSocVersion()==2)
			u32DstRgbStride = ALIGNED_16B(u32InputWidth);
		u32DstRgbSize = 3 * u32DstRgbStride * u32InputHeight;	

		
		u32yStride = ALIGNED_256B(u32ImgWidth * 4);	//argbBuff Stride
		if(AR_MPI_NPU_GetSocVersion()==2)
			u32yStride = ALIGNED_16B(u32ImgWidth * 4);
		u32argbSize = u32yStride * u32ImgHeight;

		u32resizeInStride = ALIGNED_256B(u32NetOutWidth);
		if(AR_MPI_NPU_GetSocVersion()==2)
			u32resizeInStride = ALIGNED_16B(u32NetOutWidth);
		
		u32ResizeInSize = u32resizeInStride * u32NetOutHeight * 3;
		resizeInbuff.u64Len = u32ResizeInSize;
		s32Ret = AR_MPI_NPU_MallocBuff((AR_CHAR *)"resize_in", &resizeInbuff);
		if(s32Ret)
		{
			Log(ARCNN_LOG_ERR, "Malloc resizeIn buff failed %d.", s32Ret);
		}

		u32resizeOutStride = ALIGNED_256B(u32ImgWidth);
		if(AR_MPI_NPU_GetSocVersion()==2)
			u32resizeOutStride = ALIGNED_16B(u32ImgWidth);

		u32ResizeOutSize = u32resizeOutStride * u32ImgHeight * 4;	//the 4th channel for argb with init value
		resizeOutbuff.u64Len = u32ResizeOutSize;
		s32Ret = AR_MPI_NPU_MallocBuff((AR_CHAR *)"resize_out", &resizeOutbuff);
		if(s32Ret)
		{
			Log(ARCNN_LOG_ERR, "Malloc resizeout buff failed %d.", s32Ret);
		}


		argbBuff.u64Len = u32argbSize;
		s32Ret = AR_MPI_NPU_MallocBuff((AR_CHAR*)"argbBuff", &argbBuff);
		if(s32Ret)
		{
			Log(ARCNN_LOG_ERR, "Malloc argbBuff failed %d.", s32Ret);
		}

		osdBuff.u64Len = u32argbSize;
		s32Ret = AR_MPI_NPU_MallocBuff((AR_CHAR*)"osdBuff", &osdBuff);
		if(s32Ret)
		{
			Log(ARCNN_LOG_ERR, "Malloc osdBuff failed %d.", s32Ret);
		}
		memset((AR_CHAR*)osdBuff.u64VirtAddr, 0, u32argbSize);

		classBuff.u64Len = u32ResizeOutSize;
		s32Ret = AR_MPI_NPU_MallocBuff((AR_CHAR*)"classBuff", &classBuff);
		if(s32Ret)
		{
			Log(ARCNN_LOG_ERR, "Malloc classBuff failed %d.", s32Ret);
		}
		memset((AR_CHAR*)classBuff.u64VirtAddr, 0, u32ResizeOutSize);

		pendingImage = NULL;
		currentImage = NULL;
		pstSwapImage = NULL;
	}

}

CARCNN::~CARCNN()
{
    if(handle)
    {
		AR_MPI_NPU_UnloadModel(handle);
		if(u32Mode == ARCNN_INPUT_LIVE)
		{
			AR_MPI_NPU_FreeBuff(&resizeInbuff);
			AR_MPI_NPU_FreeBuff(&resizeOutbuff);
			AR_MPI_NPU_FreeBuff(&SrcRgbBuff);
			AR_MPI_NPU_FreeBuff(&argbBuff);
			AR_MPI_NPU_FreeBuff(&osdBuff);
			AR_MPI_NPU_FreeBuff(&classBuff);
		}
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
	AR_S32             s32Ret;
    AR_S32             s32ViCnt       = 1;
    VI_DEV             ViDev          = 0;
    //VI_PIPE            ViPipe         = 0;
    //VI_CHN             ViChn          = 0;
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

AR_S32 CARCNN::RunThread()
{
	bContinued = true;
	readImgThread = std::thread(&CARCNN::ReadImgThread, this);
	displayThread = std::thread(&CARCNN::ConvertThread, this);
	return 0;
}

AR_S32 CARCNN::PushLiveImage(AR_IMG_S * pstImg)
{
	int trying = 100;
	do{
		imageMutex.lock();
		if(pendingImage != NULL)
		{
			*pstImg = *pendingImage;
			currentImage = pendingImage;
			pendingImage = NULL;
			
			imageMutex.unlock();
			return 0;
		}		
		else
		{
			imageMutex.unlock();
			usleep(200 * 1000);
		}
	}while(trying-->0);

	return -1;
}


AR_S32 CARCNN::GetLiveImage(AR_IMG_S * pstImg)
{
	ProfileStart("GetLiveImage");
	AR_S32 s32Ret;
	
	s32Ret=AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, -1);
	if(s32Ret<0)
	{
		Log(ARCNN_LOG_ERR,"AR_MPI_VI_GetChnFrame ERR ret=%x\r\n", s32Ret);
		return -1;
	}
	task_attr.stImgIn = FrameInfo;
    task_attr.stImgOut = FrameInfo;

	AR_U32 u32SrcStride = ALIGNED_256B(FrameInfo.stVFrame.u32Width);
	AR_U32 u32SrcSize = u32SrcStride * (FrameInfo.stVFrame.u32Height) * 3 / 2;

	AR_IMG_S pstSrcImg;

	pstSrcImg.u32FrameId = FrameInfo.stVFrame.u32FrameFlag;
    pstSrcImg.enFormat = AR_IMG_YUV420P;
    pstSrcImg.u32Width = FrameInfo.stVFrame.u32Width;
    pstSrcImg.u32Height = FrameInfo.stVFrame.u32Height;
    pstSrcImg.u32ChannelNum = 3;
    pstSrcImg.astChannels[0].u32Stride = FrameInfo.stVFrame.u32Stride[0];
    pstSrcImg.astChannels[1].u32Stride = FrameInfo.stVFrame.u32Stride[1];
    pstSrcImg.astChannels[2].u32Stride = FrameInfo.stVFrame.u32Stride[2];
    pstSrcImg.astChannels[0].u32AddrPhy = (AR_UINTPTR)(FrameInfo.stVFrame.u64PhyAddr[0]);
    pstSrcImg.astChannels[1].u32AddrPhy = (AR_UINTPTR)(FrameInfo.stVFrame.u64PhyAddr[1]);
    pstSrcImg.astChannels[2].u32AddrPhy = (AR_UINTPTR)(FrameInfo.stVFrame.u64PhyAddr[2]);
	
	#if 0	//debug video mode
	{
		static int imgId = 0;
		static int idx1=0;
		if(idx1==0)
		{
			//Read img from yuv binfile
			AR_CHAR *pchFileName = "./demo.yuv";		
		    ifstream file(pchFileName, ios::in|ios::binary|ios::ate);
		    AR_S32 s32FileSize = file.tellg();
		    file.seekg(0, ios::beg);
		    fileOrgBuff = (AR_CHAR*)malloc(s32FileSize);
		    memset(fileOrgBuff, 0,s32FileSize);
		    file.read((AR_CHAR *)fileOrgBuff, s32FileSize);
		    file.close();

			AR_CHAR *pchFileName1 = "./airplane.yuv";
		    ifstream file1(pchFileName1, ios::in|ios::binary|ios::ate);
		    AR_S32 s32FileSize1 = file1.tellg();
		    file1.seekg(0, ios::beg);
		    fileOrgBuff1 = (AR_CHAR*)malloc(s32FileSize1);
		    memset(fileOrgBuff1, 0,s32FileSize1);
		    file1.read((AR_CHAR *)fileOrgBuff1, s32FileSize1);
		    file1.close();

			AR_CHAR *pchFileName2 = "./car.yuv";
		    ifstream file2(pchFileName2, ios::in|ios::binary|ios::ate);
		    AR_S32 s32FileSize2 = file2.tellg();
		    file2.seekg(0, ios::beg);
		    fileOrgBuff2 = (AR_CHAR*)malloc(s32FileSize2);
		    memset(fileOrgBuff2, 0,s32FileSize2);
		    file2.read((AR_CHAR *)fileOrgBuff2, s32FileSize2);
		    file2.close();
			idx1++;

			printf("Read bin file done!\n");		
		}
		
		if((imgId/60)%3 == 0)
		{
			imgDataBuff = fileOrgBuff;
		}
		else if((imgId/100)%3 == 1)
		{
			imgDataBuff = fileOrgBuff1;
		}
		else
		{
			imgDataBuff = fileOrgBuff2;
		}
		imgId++;

		AR_MEM_S YUVfile;
		AR_U32 u32SrcStride = ALIGNED_256B(u32ImgWidth);
		AR_U32 u32SrcSize = u32SrcStride * u32ImgHeight * 3 / 2;
		YUVfile.u64Len = u32SrcSize;
		
		s32Ret = AR_MPI_NPU_MallocBuff((AR_CHAR *)"YUVfile", &YUVfile);
		if(s32Ret){
			Log(ARCNN_LOG_ERR, "Malloc YUVfile buff failed %d.", s32Ret);
		}
		memset((AR_CHAR *)YUVfile.u64VirtAddr, 0, u32SrcSize);

		//memcpy data from yuvfile to AR_MEM_S buffer
		for(AR_U32 h=0; h<u32ImgHeight; h++)	
		{
			memcpy((void*)(YUVfile.u64VirtAddr + h * u32SrcStride), (void*)(imgDataBuff + h * u32ImgWidth), u32ImgWidth);
		}
		for(AR_U32 h=0; h<u32ImgHeight/2; h++)
		{
			memcpy((void*)(YUVfile.u64VirtAddr + u32ImgHeight * u32SrcStride + h * u32SrcStride / 2), (void*)(imgDataBuff + u32ImgHeight * u32ImgWidth + h * u32ImgWidth / 2), u32ImgWidth / 2);
		}
		for(AR_U32 h=0; h<u32ImgHeight/2; h++)
		{
			memcpy((void*)(YUVfile.u64VirtAddr + u32ImgHeight * u32SrcStride * 5 / 4 + h * u32SrcStride / 2), (void*)(imgDataBuff + u32ImgHeight * u32ImgWidth * 5 / 4 + h * u32ImgWidth / 2), u32ImgWidth / 2);
		}

		//AR_IMG_S pstSrcImg;
		pstSrcImg.u32FrameId = 1;
	    pstSrcImg.enFormat = AR_IMG_YUV420P;
	    pstSrcImg.u32Width = FrameInfo.stVFrame.u32Width;
	    pstSrcImg.u32Height = FrameInfo.stVFrame.u32Height;
	    pstSrcImg.u32ChannelNum = 3;
		pstSrcImg.astChannels[0].u32Stride = u32SrcStride;
	    pstSrcImg.astChannels[1].u32Stride = u32SrcStride / 2;
	    pstSrcImg.astChannels[2].u32Stride = u32SrcStride / 2;
		pstSrcImg.astChannels[0].u32AddrPhy = (AR_UINTPTR)YUVfile.u64PhyAddr;
		pstSrcImg.astChannels[0].uptrAddrVirt = (AR_UINTPTR)YUVfile.u64VirtAddr;
		pstSrcImg.astChannels[1].u32AddrPhy = (AR_UINTPTR)(YUVfile.u64PhyAddr + u32SrcStride * u32ImgHeight);
		pstSrcImg.astChannels[1].uptrAddrVirt = (AR_UINTPTR)(YUVfile.u64VirtAddr + u32SrcStride * u32ImgHeight);
		pstSrcImg.astChannels[2].u32AddrPhy = (AR_UINTPTR)(YUVfile.u64PhyAddr + u32SrcStride * u32ImgHeight * 5 / 4);
		pstSrcImg.astChannels[2].uptrAddrVirt = (AR_UINTPTR)(YUVfile.u64VirtAddr + u32SrcStride * u32ImgHeight * 5 / 4);
	}
	#endif

	CvtRgbImg.u32FrameId = 1;
    CvtRgbImg.enFormat = AR_IMG_RGB;
    CvtRgbImg.u32Width = u32ImgWidth;	//1920 x 1080
    CvtRgbImg.u32Height = u32ImgHeight;
    CvtRgbImg.u32ChannelNum = 3;
	CvtRgbImg.astChannels[0].u32Stride = u32SrcRgbStride;
    CvtRgbImg.astChannels[1].u32Stride = u32SrcRgbStride;
    CvtRgbImg.astChannels[2].u32Stride = u32SrcRgbStride;
	CvtRgbImg.astChannels[0].u32AddrPhy = (AR_UINTPTR)SrcRgbBuff.u64PhyAddr;
	CvtRgbImg.astChannels[0].uptrAddrVirt = (AR_UINTPTR)SrcRgbBuff.u64VirtAddr;
	CvtRgbImg.astChannels[1].u32AddrPhy = (AR_UINTPTR)(SrcRgbBuff.u64PhyAddr + u32SrcRgbStride * u32ImgHeight);
	CvtRgbImg.astChannels[1].uptrAddrVirt = (AR_UINTPTR)(SrcRgbBuff.u64VirtAddr + u32SrcRgbStride * u32ImgHeight);
	CvtRgbImg.astChannels[2].u32AddrPhy = (AR_UINTPTR)(SrcRgbBuff.u64PhyAddr + u32SrcRgbStride * u32ImgHeight * 2);
	CvtRgbImg.astChannels[2].uptrAddrVirt = (AR_UINTPTR)(SrcRgbBuff.u64VirtAddr + u32SrcRgbStride * u32ImgHeight * 2);
	
	s32Ret = AR_MPI_IFC_CvtColor(&pstSrcImg, &CvtRgbImg, AR_MPI_IFC_BT601_FULL);  //yuv420p --> rgb;	//AR_MPI_IFC_BT601_FULL
	if(s32Ret < 0)
	{
		Log(ARCNN_LOG_ERR,"Do CvtColor error.\n");
		return -1;
	}

	max_src = GET_MAX(u32ImgHeight, u32ImgWidth);
	hwRatio = (AR_FLOAT)u32ImgHeight / (AR_FLOAT)u32ImgWidth;

	if(pstSwapImage == NULL)
	{
		AR_MEM_S ScalerOutBuff;
		ScalerOutBuff.u64Len = u32DstRgbSize;
		s32Ret = AR_MPI_NPU_MallocBuff((AR_CHAR*)"ScalerOutBuff", &ScalerOutBuff);
		if(s32Ret)
		{
			Log(ARCNN_LOG_ERR, "Malloc ScalerOutput Buff failed %d.", s32Ret);
		}
		memset((AR_CHAR *)ScalerOutBuff.u64VirtAddr, 0, u32DstRgbSize); 

		pstSwapImage = new AR_IMG_S;
		memset(pstSwapImage, 0, sizeof(AR_IMG_S));

		pstSwapImage->astChannels[0].u32Stride = u32DstRgbStride;
		pstSwapImage->astChannels[1].u32Stride = u32DstRgbStride;
		pstSwapImage->astChannels[2].u32Stride = u32DstRgbStride;
		pstSwapImage->astChannels[0].uptrAddrVirt = (AR_UINTPTR)ScalerOutBuff.u64VirtAddr;
		pstSwapImage->astChannels[1].uptrAddrVirt = (AR_UINTPTR)(ScalerOutBuff.u64VirtAddr + u32DstRgbStride * u32InputHeight);
		pstSwapImage->astChannels[2].uptrAddrVirt = (AR_UINTPTR)(ScalerOutBuff.u64VirtAddr + u32DstRgbStride * u32InputHeight * 2);
		pstSwapImage->astChannels[0].u32AddrPhy = (AR_UINTPTR)ScalerOutBuff.u64PhyAddr;
		pstSwapImage->astChannels[1].u32AddrPhy = (AR_UINTPTR)(ScalerOutBuff.u64PhyAddr + u32DstRgbStride * u32InputHeight);
		pstSwapImage->astChannels[2].u32AddrPhy = (AR_UINTPTR)(ScalerOutBuff.u64PhyAddr + u32DstRgbStride * u32InputHeight * 2);

	}
	pstSwapImage->u32FrameId = 1;
	pstSwapImage->enFormat = AR_IMG_RGB;
	pstSwapImage->u32Width = u32InputWidth;
	pstSwapImage->u32Height = u32InputHeight;
	pstSwapImage->u32ChannelNum = 3;

	AR_HAL_SCALER_CROP_S stCrop;
	stCrop.u32X = 0;
	stCrop.u32Y = 0;
	stCrop.u32W = u32ImgWidth;
	stCrop.u32H = u32ImgHeight;
	
	s32Ret = AR_MPI_SCALER_CropResizeRatio(&CvtRgbImg, &stCrop, pstSwapImage, 1, 1, AR_IMG_ALIGN_TO_MID);		//u32ImgWidth x u32Imgheight --> u32InputWidth x u32InputHeight
	if(s32Ret < 0)
	{
		Log(ARCNN_LOG_ERR, "Do crop/resize error.\n");
		return -1;
	}

	*pstImg = *pstSwapImage;
	//AR_MPI_NPU_FreeBuff(&YUVfile);
	ProfileEnd("GetLiveImage");
    return 0;
}


AR_S32 CARCNN::ReleaseLiveImage(AR_IMG_S * pstImg)
{
    ProfileStart("ReleaseLiveImage");
	imageMutex.lock();
	if(currentImage !=NULL)
	{
		imageQueue.push(currentImage);
		currentImage = NULL;
	}
	imageMutex.unlock();
	ProfileEnd("ReleaseLiveImage");
    return 0;
}

//AR_S32 CARCNN::DeBindVideoDev()
//{
//   return 0;
//

string CARCNN::GetLocalTestDir()
{
    return strLocalDir;
}


//#define CALLBACK_DATA_LOAD
//For local image test. Need to add RGBD or RGBIntlv format later.
AR_S32 CARCNN::GetLocalImage(AR_CHAR * pchFileName, AR_IMG_S * pstImg, AR_U32 u32InputId)
{
	AR_S32 s32Ret;
	AR_U32 u32FileType = 0;
    AR_S32 s32FileSize = 0;
    static AR_U32 u32FrameId = 0;
	AR_IMG_S stRGBSrcImg = {0};
	AR_MEM_S stRGBSchbuff = {0};

    ProfileStart("GetLocalImage");
    u32FileType = GetImageFileType(pchFileName);
	if(u32FileType == JPG_BMP_PNG_IMG_FILE)
    {
		AR_NPU_TENSOR_S stTensor ={0};
		AR_MPI_NPU_GetInputTensorParam(handle, 0, &stTensor);

		//read image and resize it, TBD
        cv::Mat src_img = cv::imread(pchFileName, cv::IMREAD_ANYCOLOR);
        if(src_img.empty())
        {
           Log(ARCNN_LOG_ERR, "Imread file %s failed: %s!\r\n", pchFileName, strerror(errno));
           return -1;
        }
		printf("Src img w/h/c : %d, %d, %d\n", src_img.cols, src_img.rows, src_img.channels());
		
		u32ImgWidth = src_img.cols;
		u32ImgHeight = src_img.rows;
		u32InputWidth = stTensor.u32Width;
		u32InputHeight = stTensor.u32Height;
		max_src =GET_MAX(u32ImgHeight, u32ImgWidth);
		hwRatio = (AR_FLOAT)u32ImgHeight / (AR_FLOAT)u32ImgWidth;	

		AR_U32 u32Stride = ALIGNED_256B(u32InputWidth);
		if(AR_MPI_NPU_GetSocVersion() == 2)
		{
			u32Stride = ALIGNED_16B(u32InputWidth);
		}
		s32FileSize = u32Stride * u32InputHeight * 3;
		
		AR_MEM_S stPchbuff;
		stPchbuff.u64Len = s32FileSize;
		s32Ret = AR_MPI_NPU_MallocBuff((AR_CHAR*)"Pchbuff", &stPchbuff);
		if(s32Ret)
		{
			Log(ARCNN_LOG_ERR, "Malloc NPU input error : size %d. \n", stPchbuff.u64Len);
		}
		memset((AR_CHAR *)stPchbuff.u64VirtAddr, 0, stPchbuff.u64Len);

		//initializer input buffer addr, Network input
		pstImg->u32Width = u32InputWidth;
		pstImg->u32Height = u32InputHeight;
		pstImg->u32FrameId = u32FrameId++;
		pstImg->enFormat  = AR_IMG_RGB;
		pstImg->u32ChannelNum = 3;
		pstImg->astChannels[0].u32Stride = u32Stride;
		pstImg->astChannels[1].u32Stride = u32Stride;
		pstImg->astChannels[2].u32Stride = u32Stride;
		pstImg->astChannels[0].u32AddrPhy = (AR_UINTPTR)stPchbuff.u64PhyAddr;
		pstImg->astChannels[1].u32AddrPhy = (AR_UINTPTR)(stPchbuff.u64PhyAddr + u32Stride * u32InputHeight);
		pstImg->astChannels[2].u32AddrPhy = (AR_UINTPTR)(stPchbuff.u64PhyAddr + u32Stride * u32InputHeight * 2);
		pstImg->astChannels[0].uptrAddrVirt = (AR_UINTPTR)stPchbuff.u64VirtAddr;
		pstImg->astChannels[1].uptrAddrVirt = (AR_UINTPTR)(stPchbuff.u64VirtAddr + u32Stride * u32InputHeight * 2);
		pstImg->astChannels[2].uptrAddrVirt = (AR_UINTPTR)(stPchbuff.u64VirtAddr + u32Stride * u32InputHeight * 2);
		
		cv::Mat channels[3];
		cv::split(src_img, channels);
		AR_CHAR *pchR = (AR_CHAR *)channels[2].data;
		AR_CHAR *pchG = (AR_CHAR *)channels[1].data;
		AR_CHAR *pchB = (AR_CHAR *)channels[0].data;

		AR_U32 u32ReInStride = ALIGNED_256B(u32ImgWidth);
		if(AR_MPI_NPU_GetSocVersion() == 2)
			u32ReInStride = ALIGNED_16B(u32ImgWidth);
	
		if((u32ImgWidth != u32InputWidth) ||(u32ImgHeight != u32InputHeight))
		{
			//Get source aligned RGB img buff
			AR_S32 s32ReInSize = u32ReInStride * u32ImgHeight * 3;
			stRGBSchbuff.u64Len = s32ReInSize;
			s32Ret = AR_MPI_NPU_MallocBuff((AR_CHAR *)"RGBSchBuff", &stRGBSchbuff);
			if(s32Ret)
			{
				Log(ARCNN_LOG_ERR, "Malloc ALIGN RGB Buffer Error %d.\n", stRGBSchbuff.u64Len);
				return -1;
			}

			//push Src_img to ALIGN RGB buffer
			memset((AR_CHAR*)stRGBSchbuff.u64VirtAddr, 0, stRGBSchbuff.u64Len);
			
			for(AR_U32 h=0; h<u32ImgHeight; h++)
			{
				memcpy((void *)(stRGBSchbuff.u64VirtAddr + h * u32ReInStride), (void *)(pchR + h * u32ImgWidth), u32ImgWidth);
				memcpy((void *)(stRGBSchbuff.u64VirtAddr + (h + u32ImgHeight) * u32ReInStride), (void *)(pchG + h * u32ImgWidth), u32ImgWidth);
				memcpy((void *)(stRGBSchbuff.u64VirtAddr + (h + u32ImgHeight*2) * u32ReInStride), (void *)(pchB + h * u32ImgWidth), u32ImgWidth);
			}
			
			stRGBSrcImg.u32Width = u32ImgWidth;
			stRGBSrcImg.u32Height = u32ImgHeight;
			stRGBSrcImg.u32FrameId = u32FrameId;
			stRGBSrcImg.enFormat = AR_IMG_RGB;
			stRGBSrcImg.u32ChannelNum = 3;
			stRGBSrcImg.astChannels[0].u32Stride = u32ReInStride;
			stRGBSrcImg.astChannels[1].u32Stride = u32ReInStride;
			stRGBSrcImg.astChannels[2].u32Stride = u32ReInStride;
			stRGBSrcImg.astChannels[0].u32AddrPhy = (AR_UINTPTR)stRGBSchbuff.u64PhyAddr;
			stRGBSrcImg.astChannels[1].u32AddrPhy = (AR_UINTPTR)(stRGBSchbuff.u64PhyAddr + u32ReInStride * u32ImgHeight);
			stRGBSrcImg.astChannels[2].u32AddrPhy = (AR_UINTPTR)(stRGBSchbuff.u64PhyAddr + u32ReInStride * u32ImgHeight * 2);
			stRGBSrcImg.astChannels[0].uptrAddrVirt = (AR_UINTPTR)stRGBSchbuff.u64VirtAddr;
			stRGBSrcImg.astChannels[1].uptrAddrVirt = (AR_UINTPTR)(stRGBSchbuff.u64VirtAddr + u32ReInStride * u32ImgHeight);
			stRGBSrcImg.astChannels[2].uptrAddrVirt = (AR_UINTPTR)(stRGBSchbuff.u64VirtAddr + u32ReInStride * u32ImgHeight * 2);
			
			AR_HAL_SCALER_CROP_S stCrops = {0};
			stCrops.u32X = 0;
			stCrops.u32Y = 0;
			stCrops.u32W = u32ImgWidth;
			stCrops.u32H = u32ImgHeight;

			s32Ret = AR_MPI_SCALER_CropResizeRatio(&stRGBSrcImg, &stCrops, pstImg, 1, 0, AR_IMG_ALIGN_TO_MID);
			if(s32Ret)
			{
				Log(ARCNN_LOG_ERR, "AR_MPI_SCALER_CropResizeRatio error: %d.\n", s32Ret);
				return -1;
			}

			AR_MPI_NPU_FreeBuff(&stRGBSchbuff);
		}
		else
		{
			printf("The srcImg size same with the dstImg size !\n");
			for(AR_U32 h=0; h<u32ImgHeight; h++)
			{
				memcpy((void*)(stPchbuff.u64VirtAddr + h * u32ReInStride), (void*)(pchR + h * u32InputWidth), u32ImgWidth);
				memcpy((void*)(stPchbuff.u64VirtAddr + (h + u32ImgHeight) * u32ReInStride), (void*)(pchG + h * u32ImgWidth), u32ImgWidth);
				memcpy((void*)(stPchbuff.u64VirtAddr + (h + u32ImgHeight*2) * u32ReInStride), (void*)(pchG + h * u32ImgWidth), u32ImgWidth);
			}
		}
	}
	else if(u32FileType == RGB_RAW_DATA_FILE || u32FileType == BGR_RAW_DATA_FILE || u32FileType == RGBD_RAW_DATA_FILE)
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
            pstImg->enFormat = AR_IMG_RGB;

            //No need to set w/h/s since ifc.json will set these parameters.
            pstImg->astChannels[0].u32AddrPhy = (AR_UINTPTR)stPchbuff.u64PhyAddr;
            pstImg->astChannels[0].uptrAddrVirt = (AR_UINTPTR)stPchbuff.u64VirtAddr;
        }
    }
	else
	{
		//printf("Unsupport Image Format !\n");
		return -1;
	}
		
    ProfileEnd("GetLocalImage");
    Log(ARCNN_LOG_INFO, "Get image %s..", pchFileName);
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
				printf("CB[%d] Time: %d x 0.01ms\n",i,pCBTime[i]);
			    u32CBTime = u32CBTime +pCBTime[i];
	        }
		}
		AR_U32 u32FwPreTime = AR_MPI_NPU_GetNPUPreTime(handle);
		AR_U32 u32FwTime = AR_MPI_NPU_GetNPURunTime(handle);

		Log(ARCNN_LOG_INFO, "NPUFWPre: %d x 0.01ms  NPUFW: %d x 0.01ms  NPUCBTime: %d x 0.01ms.",u32FwPreTime,u32FwTime, u32CBTime);
    }
	
    return s32Ret;
}


AR_S32 CARCNN::PostProcess(void * args)
{
	ProfileStart("Post Process");	
    AR_S32 s32Ret 	= 0;
	AR_S32 s32Ret_y = 0;
	AR_U32 featureW = 0;
	AR_U32 featureH = 0;
    AR_UINTPTR outputBuff = stNPUOutBuff.u64VirtAddr;
    AR_NPU_TENSOR_S stOutputTensor;
	AR_U32 size = AR_MPI_NPU_GetOutputBuffSize(handle);
	AR_MPI_NPU_GetOutputTensorParam(handle, 0, &stOutputTensor);
    AR_MPI_NPU_InvalidCachedBuff(&stNPUOutBuff);	//2022-04-11, Need to invalid cache before read output buffer because we malloced a cachable buffer.
	
	//Output Tensor param
	AR_U32 u32Batch = stOutputTensor.u32Num;
	AR_U32 u32OutHeight = stOutputTensor.u32Height;
	AR_U32 u32OutWidth = stOutputTensor.u32Width;
	AR_U32 u32OutChnSize = u32OutHeight * u32OutWidth;
	AR_U32 u32OutOriChannels = stOutputTensor.u32OriChannels;
	AR_U32 u32Precision = stOutputTensor.u32Precision;
	AR_CHAR* OutOffsetAddr = (AR_CHAR*)outputBuff + stOutputTensor.u32Bank * 0x2000000 + stOutputTensor.u32Offset;

	if(u32Mode == ARCNN_INPUT_LOCAL)	//save bin file for npu output
	{
		if(u32dumpBin)
		{
			AR_CHAR name[128] = {0};
			sprintf(name, "%s%s", (AR_CHAR *)args, "_output.bin");
			ofstream ofile(name, ios::binary);
			if(ofile)
			{
				ofile.write((AR_CHAR *)outputBuff, size);
				Log(ARCNN_LOG_INFO, "Save output to %s finished.", name);
			}
		}

		u32resizeInStride = ALIGNED_256B(u32OutWidth);
		if(AR_MPI_NPU_GetSocVersion()==2)
			u32resizeInStride = ALIGNED_16B(u32OutWidth);

		u32ResizeInSize = u32resizeInStride * u32OutHeight * 3;
		resizeInbuff.u64Len = u32ResizeInSize;
		s32Ret = AR_MPI_NPU_MallocBuff((AR_CHAR *)"resize_in", &resizeInbuff);
		if(s32Ret)
		{
			Log(ARCNN_LOG_ERR, "Malloc resizeIn buff failed %d.", s32Ret);
		}

		u32resizeOutStride = ALIGNED_256B(u32ImgWidth);
		if(AR_MPI_NPU_GetSocVersion()==2)
			u32resizeOutStride = ALIGNED_16B(u32ImgWidth);

		u32ResizeOutSize = u32resizeOutStride * u32ImgHeight * 3;
		resizeOutbuff.u64Len = u32ResizeOutSize;
		s32Ret = AR_MPI_NPU_MallocBuff((AR_CHAR *)"resize_out", &resizeOutbuff);
		if(s32Ret)
		{
			Log(ARCNN_LOG_ERR, "Malloc resizeout buff failed %d.", s32Ret);
		}

		memset((AR_CHAR *)resizeInbuff.u64VirtAddr, 0, u32ResizeInSize);
		memset((AR_CHAR *)resizeOutbuff.u64VirtAddr, 0, u32ResizeOutSize);
	}
	else
	{
		memset((AR_CHAR *)resizeInbuff.u64VirtAddr, 0, u32ResizeInSize);
		memset((AR_CHAR *)resizeOutbuff.u64VirtAddr, 0, u32ResizeOutSize * 3 / 4);
		memset((AR_CHAR *)resizeOutbuff.u64VirtAddr + u32ResizeOutSize * 3 / 4, u32aChnInitValue, u32ResizeOutSize / 4);		//u32aChnInitValue is 'a' of argb channel's init value.
	}


	if(u32Precision == 32)		//fp32 data in
	{
		float *addr;
		float *nowPtr;
		AR_FLOAT *ptr = (AR_FLOAT*)OutOffsetAddr;
		float max_value = -100000.0f;
		float32x4_t v0, v1;
		uint32x4_t  vidx0, vidx1, vmask;
		AR_U32 u32length = u32OutWidth / 4;

		for(AR_U32 b=0; b<u32Batch; b++)
		{
			for(AR_U32 h=0; h<u32OutHeight; h++)
			{
				addr = ptr + h * u32OutWidth;
				int widx = 0;
				for(AR_U32 l=0; l<u32length; l++)
				{
					v0 = vmovq_n_f32(max_value);
					vidx0 = vmovq_n_u32(0);

					for(AR_U32 c=0; c<u32OutOriChannels; c++)
					{
						nowPtr = addr + c * u32OutChnSize;
						v1 = vld1q_f32(nowPtr);
						vmask = vcgtq_f32(v1, v0);
						vidx1 = vmovq_n_u32(c);
						vidx0 = vbslq_u32(vmask, vidx1, vidx0);
						v0 = vmaxq_f32(v1, v0);	
					}
					
					AR_U32 *clsId = (AR_U32*)(&vidx0);
					if(clsId[0] != 0 || clsId[1] != 0 || clsId[2] != 0 || clsId[3] != 0)
					{
						for(AR_U32 pIdx=0; pIdx<4; pIdx++)
						{
							((AR_CHAR *)resizeInbuff.u64VirtAddr)[h * u32resizeInStride + widx + pIdx] = (AR_UCHAR)rr[clsId[pIdx]];
							((AR_CHAR *)resizeInbuff.u64VirtAddr)[(u32OutHeight + h) * u32resizeInStride + widx + pIdx] = (AR_UCHAR)gg[clsId[pIdx]];
							((AR_CHAR *)resizeInbuff.u64VirtAddr)[(u32OutHeight*2 + h) * u32resizeInStride + widx + pIdx] = (AR_UCHAR)bb[clsId[pIdx]];
						}
					}
					widx += 4;
					addr += 4;
					
				}
			}
		}	
	}
	else if(u32Precision == 16)		//int16 data in
	{
		AR_S16 *addr;
		AR_S16 *nowPtr;
		AR_S16 *ptr = (AR_S16*)OutOffsetAddr;
		AR_S16 max_value = -1000;
		int16x8_t v0, v1;
		uint16x8_t  vidx0, vidx1, vmask;
		AR_U32 u32length = u32OutWidth / 8;

		for(AR_U32 b=0; b<u32Batch; b++)	//1
		{
			for(AR_U32 h=0; h<u32OutHeight; h++)	//512
			{
				addr = ptr + h * u32OutWidth;
				AR_U32 widx = 0;
				
				for(AR_U32 l=0; l<u32length; l++)
				{
					v0 = vmovq_n_s16(max_value);
					vidx0 = vmovq_n_u16(0);

					for(AR_U32 c=0; c<u32OutOriChannels; c++)
					{
						nowPtr = addr + c * u32OutChnSize;
						v1 = vld1q_s16(nowPtr);
						vmask = vcgtq_s16(v1, v0);
						vidx1 = vmovq_n_u16(c);
						vidx0 = vbslq_u16(vmask, vidx1, vidx0);
						v0 = vmaxq_s16(v1, v0);			
					}
						
					AR_U16 *clsId = (AR_U16*)(&vidx0);
					if(clsId[0] != 0 || clsId[1] != 0 || clsId[2] != 0 || clsId[3] != 0 || clsId[4] != 0 || clsId[5] != 0 || clsId[6] != 0 || clsId[7] != 0)
					{
						for(AR_U32 pIdx=0; pIdx<8; pIdx++)
						{
							((AR_CHAR *)resizeInbuff.u64VirtAddr)[h * u32resizeInStride + widx + pIdx] = (AR_UCHAR)rr[clsId[pIdx]];
							((AR_CHAR *)resizeInbuff.u64VirtAddr)[(u32OutHeight + h) * u32resizeInStride + widx + pIdx] = (AR_UCHAR)gg[clsId[pIdx]];
							((AR_CHAR *)resizeInbuff.u64VirtAddr)[(u32OutHeight*2 + h) * u32resizeInStride + widx + pIdx] = (AR_UCHAR)bb[clsId[pIdx]];
						}
					}
					widx += 8;
					addr += 8;
					
				}
			}
		}
	}
	else	//int8 data in
	{
		AR_S8 *addr;
		AR_S8 *nowPtr;
		AR_S8 *ptr = (AR_S8*)OutOffsetAddr;
		AR_S8 max_value = -100;
		int8x16_t v0, v1;
		uint8x16_t  vidx0, vidx1, vmask;
		AR_U32 u32length = u32OutWidth / 16;

		for(AR_U32 b=0; b<u32Batch; b++)
		{
			for(AR_U32 h=0; h<u32OutHeight; h++)	//set h+=2 for the fast speed to finish the class process
			{
				addr = ptr + h * u32OutWidth;
				AR_U32 widx = 0;	
				for(AR_U32 l=0; l<u32length; l++)
				{
					v0 = vmovq_n_s8(max_value);
					vidx0 = vmovq_n_u8(0);

					for(AR_U32 c=0; c<u32OutOriChannels; c++)
					{
						nowPtr = addr + c * u32OutChnSize;
						v1 = vld1q_s8(nowPtr);
						vmask = vcgtq_s8(v1, v0);
						vidx1 = vmovq_n_u8(c);
						vidx0 = vbslq_u8(vmask, vidx1, vidx0);
						v0 = vmaxq_s8(v1, v0);			
					}
					
					AR_U8 *clsId = (AR_U8*)(&vidx0);
					if(clsId[0] != 0 || clsId[1] != 0 || clsId[2] != 0 || clsId[3] != 0 || clsId[4] != 0 || clsId[5] != 0 || clsId[6] != 0 || clsId[7] != 0 ||
					   clsId[8] != 0 || clsId[9] != 0 || clsId[10] != 0 || clsId[11] != 0 || clsId[12] != 0 || clsId[13] != 0 || clsId[14] != 0 || clsId[15] != 0)
					{
						for(AR_U32 pIdx=0; pIdx<16; pIdx++)
						{
							((AR_CHAR *)resizeInbuff.u64VirtAddr)[h * u32resizeInStride + widx + pIdx] = (AR_UCHAR)rr[clsId[pIdx]];
							((AR_CHAR *)resizeInbuff.u64VirtAddr)[(u32OutHeight + h) * u32resizeInStride + widx + pIdx] = (AR_UCHAR)gg[clsId[pIdx]];
							((AR_CHAR *)resizeInbuff.u64VirtAddr)[(u32OutHeight*2 + h) * u32resizeInStride + widx + pIdx] = (AR_UCHAR)bb[clsId[pIdx]];
						}
					}
					widx += 16;
					addr += 16;		
					
				}
			}
		}
	}

	if(max_src == u32ImgHeight)
	{
		featureH = u32OutHeight;
		featureW = u32OutHeight / hwRatio;
	}
	else
	{
		featureH = u32OutWidth * hwRatio;
		featureW = u32OutWidth;
	}
	
	//resize : u32OutWidth x u32OutHeight --> u32ImgWidth x u32ImgHeight
	resizeInImg.u32Width = u32OutWidth;
	resizeInImg.u32Height = u32OutHeight;
	resizeInImg.u32FrameId = 0;
	resizeInImg.enFormat = AR_IMG_RGB;
	resizeInImg.u32ChannelNum = 3;
	resizeInImg.astChannels[0].u32AddrPhy = (AR_UINTPTR)resizeInbuff.u64PhyAddr;
	resizeInImg.astChannels[0].uptrAddrVirt = (AR_UINTPTR)resizeInbuff.u64VirtAddr;
	resizeInImg.astChannels[0].u32Stride = u32resizeInStride;
	resizeInImg.astChannels[1].u32AddrPhy = (AR_UINTPTR)resizeInbuff.u64PhyAddr + u32resizeInStride * u32OutHeight;
	resizeInImg.astChannels[1].uptrAddrVirt = (AR_UINTPTR)resizeInbuff.u64VirtAddr + u32resizeInStride * u32OutHeight;
	resizeInImg.astChannels[1].u32Stride = u32resizeInStride;
	resizeInImg.astChannels[2].u32AddrPhy = (AR_UINTPTR)resizeInbuff.u64PhyAddr + u32resizeInStride * u32OutHeight * 2;
	resizeInImg.astChannels[2].uptrAddrVirt = (AR_UINTPTR)resizeInbuff.u64VirtAddr + u32resizeInStride * u32OutHeight * 2;
	resizeInImg.astChannels[2].u32Stride = u32resizeInStride;

	resizeOutImg.u32Width = u32ImgWidth;
	resizeOutImg.u32Height = u32ImgHeight;
	resizeOutImg.u32FrameId = 0;
	resizeOutImg.enFormat = AR_IMG_RGB;
	resizeOutImg.u32ChannelNum = 3;
	resizeOutImg.astChannels[0].u32AddrPhy = (AR_UINTPTR)resizeOutbuff.u64PhyAddr;
	resizeOutImg.astChannels[0].uptrAddrVirt = (AR_UINTPTR)resizeOutbuff.u64VirtAddr;
	resizeOutImg.astChannels[0].u32Stride = u32resizeOutStride;
	resizeOutImg.astChannels[1].u32AddrPhy = (AR_UINTPTR)resizeOutbuff.u64PhyAddr + u32resizeOutStride * u32ImgHeight;
	resizeOutImg.astChannels[1].uptrAddrVirt = (AR_UINTPTR)resizeOutbuff.u64VirtAddr + u32resizeOutStride * u32ImgHeight;
	resizeOutImg.astChannels[1].u32Stride = u32resizeOutStride;
	resizeOutImg.astChannels[2].u32AddrPhy = (AR_UINTPTR)resizeOutbuff.u64PhyAddr + u32resizeOutStride * u32ImgHeight * 2;
	resizeOutImg.astChannels[2].uptrAddrVirt = (AR_UINTPTR)resizeOutbuff.u64VirtAddr + u32resizeOutStride * u32ImgHeight * 2;
	resizeOutImg.astChannels[2].u32Stride = u32resizeOutStride;

	AR_HAL_SCALER_CROP_S stResizeCrop = {0};
	stResizeCrop.u32X = AR_U32(u32OutWidth / 2 - featureW / 2);
	stResizeCrop.u32Y = AR_U32(u32OutHeight / 2 - featureH / 2);
	stResizeCrop.u32W = featureW;
	stResizeCrop.u32H = featureH;
	
	s32Ret = AR_MPI_SCALER_CropResize(&resizeInImg, &stResizeCrop, &resizeOutImg, 1, AR_IMG_ALIGN_TO_MID);
	if(s32Ret){
		Log(ARCNN_LOG_ERR, "AR_MPI_SCALER_CropResizeRatio error : %d.", s32Ret);
		return -1;
	}

	if(u32Mode == ARCNN_INPUT_LOCAL)
	{
		AR_U32 saveSize = u32ImgWidth * u32ImgHeight * 3;
		AR_CHAR *savePtr = (AR_CHAR*)malloc(saveSize);
		memset(savePtr, 0, saveSize);
		
		ProfileStart("Save img file");
		for(AR_U32 h=0; h<u32ImgHeight; h++)
		{
			memcpy((void*)(savePtr + h * u32ImgWidth), (void*)(resizeOutImg.astChannels[0].uptrAddrVirt + h * resizeOutImg.astChannels[0].u32Stride), u32ImgWidth);
			memcpy((void*)(savePtr + (u32ImgHeight + h) * u32ImgWidth), (void*)(resizeOutImg.astChannels[1].uptrAddrVirt + h * resizeOutImg.astChannels[0].u32Stride), u32ImgWidth);
			memcpy((void*)(savePtr + (2 * u32ImgHeight + h) * u32ImgWidth), (void*)(resizeOutImg.astChannels[2].uptrAddrVirt + h * resizeOutImg.astChannels[0].u32Stride), u32ImgWidth);
		}
		
		char imgname[128];
		sprintf(imgname, "%s%s%s_%d%s", "./", (AR_CHAR*)args, "result", u32Precision, ".jpg");
		SaveRGBImg((AR_CHAR*)(savePtr + u32ImgHeight*u32ImgWidth * 2), 
					(AR_CHAR*)(savePtr + u32ImgHeight*u32ImgWidth), 
					(AR_CHAR*)(savePtr), u32ImgWidth, u32ImgHeight, imgname);
		ProfileEnd("Save img file");
		
		free(savePtr);
		AR_MPI_NPU_FreeBuff(&resizeInbuff);
		AR_MPI_NPU_FreeBuff(&resizeOutbuff);
	}
	else
	{
		classMutex.lock();
		STRU_SYS_DMA_TRAN_INFO tran_info0 = {0};	//Use Dma to move data 
		tran_info0.s32_disable_cci = 1;
		tran_info0.s32_tran_cnt_sum = 1;
		tran_info0.st_addr_size[0].pv_src_pa = (AR_VOID*)resizeOutbuff.u64PhyAddr;
		tran_info0.st_addr_size[0].pv_dst_pa = (AR_VOID*)classBuff.u64PhyAddr;
		tran_info0.st_addr_size[0].u32_size = u32ResizeOutSize;		//1920*1080*4
		s32Ret_y = ar_hal_sys_mem_transfer_pa(&tran_info0);
		classMutex.unlock();
	}
	
	memset((void*)outputBuff, 0, stNPUOutBuff.u64Len);
	ProfileEnd("Post Process");
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

void CARCNN::ReadImgThread()
{
	AR_S32 s32Ret;
	while(bContinued)
	{
		AR_IMG_S* followImg = NULL;
		u32FollowImgSize = u32DstRgbSize;
		
		imageMutex.lock();
		if(!imageQueue.empty())
		{
			followImg = imageQueue.front();
			imageQueue.pop();		
		}
		imageMutex.unlock();
		
		if(followImg == NULL)
		{
			followImg = new AR_IMG_S;
			memset(followImg, 0, sizeof(AR_IMG_S));
			AR_MEM_S ImgBuff;
			ImgBuff.u64Len = u32FollowImgSize;
			s32Ret = AR_MPI_NPU_MallocBuff((AR_CHAR*)"ImgBuff", &ImgBuff);
			if(s32Ret)
			{
				Log(ARCNN_LOG_ERR, "MallocBuff ImgBuff fail !");
			}
			memset((AR_CHAR*)ImgBuff.u64VirtAddr, 0, u32FollowImgSize);

			followImg->astChannels[0].u32Stride = u32DstRgbStride;
			followImg->astChannels[1].u32Stride = u32DstRgbStride;
			followImg->astChannels[2].u32Stride = u32DstRgbStride;
			followImg->astChannels[0].u32AddrPhy = (AR_UINTPTR)ImgBuff.u64PhyAddr;
			followImg->astChannels[1].u32AddrPhy = (AR_UINTPTR)ImgBuff.u64PhyAddr + u32DstRgbStride * u32InputHeight;
			followImg->astChannels[2].u32AddrPhy = (AR_UINTPTR)ImgBuff.u64PhyAddr + u32DstRgbStride * u32InputHeight * 2;
			followImg->astChannels[0].uptrAddrVirt = (AR_UINTPTR)ImgBuff.u64VirtAddr;
			followImg->astChannels[1].uptrAddrVirt = (AR_UINTPTR)ImgBuff.u64VirtAddr + u32DstRgbStride * u32InputHeight;
			followImg->astChannels[2].uptrAddrVirt = (AR_UINTPTR)ImgBuff.u64VirtAddr + u32DstRgbStride * u32InputHeight * 2;
		}
		
		if(GetLiveImage(followImg) != 0)
		{
			Log(ARCNN_LOG_ERR, "Get Live Img fail !");
			imageMutex.lock();
			imageQueue.push(followImg);
			imageMutex.unlock();
			ReleaseLiveImage(followImg);
			return ;
		}

		imageMutex.lock();
		if(pendingImage != NULL)
		{
			imageQueue.push(pendingImage);
		}
		pendingImage = followImg;
		imageMutex.unlock();

		osdMutex.lock();	
		int job_id = 10;
		PIXEL_FORMAT_E enPixelFmt = PIXEL_FORMAT_ARGB_8888;
		
		s32Ret = AR_MPI_VGS_BeginJob(&job_id);
        if (AR_SUCCESS != s32Ret)
        {
            printf("AR_MPI_VGS_BeginJob failed with %d!\n", s32Ret);
        }

		VGS_ADD_OSD_S  osd_array[1];
        osd_array[0].stRect.s32X = 0;
        osd_array[0].stRect.s32Y = 0;
        osd_array[0].stRect.u32Width  = u32ImgWidth;
        osd_array[0].stRect.u32Height = u32ImgHeight;
        osd_array[0].u32BgColor = 0xffffffff;
        osd_array[0].enPixelFmt = enPixelFmt;
        osd_array[0].u64PhyAddr = (AR_U64)osdBuff.u64PhyAddr;
        osd_array[0].u32Stride  = u32ImgWidth * 4;
        osd_array[0].u32BgAlpha = 0;	//argb888 not use;
        osd_array[0].u32FgAlpha = 0;	//argb888 not use;
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

		s32Ret = AR_MPI_VO_SendFrame(0, 0, &FrameInfo, -1);
		if(s32Ret)
		{
			printf("AR_MPI_VO_SendFrame failed with 0x%x!\n", s32Ret);
		}

		AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
		osdMutex.unlock();
	}
}


//rgb->argb
void CARCNN::ConvertThread()
{
	AR_S32 s32Ret_y;
	while(bContinued)
	{				
		memset((AR_CHAR *)argbBuff.u64VirtAddr, 0, u32argbSize);
		classMutex.lock();
		argbMutex.lock();
		AR_U32 u32LoopW = u32ImgWidth / 16;
		uint8x16x4_t s8In0;
		for(AR_U32 h=0; h<u32ImgHeight; h+=4)
		{
			AR_U32 outIdx = 0;
			for(AR_U32 w = 0; w<u32LoopW; w++)
			{
				s8In0.val[0] = vld1q_u8((uint8_t*)classBuff.u64VirtAddr + h * u32resizeOutStride + w * 16);
				s8In0.val[1] = vld1q_u8((uint8_t*)classBuff.u64VirtAddr + 1 * u32ImgHeight * u32resizeOutStride + h * u32resizeOutStride + w * 16);
				s8In0.val[2] = vld1q_u8((uint8_t*)classBuff.u64VirtAddr + 2 * u32ImgHeight * u32resizeOutStride + h * u32resizeOutStride + w * 16);
				s8In0.val[3] = vld1q_u8((uint8_t*)classBuff.u64VirtAddr + 3 * u32ImgHeight * u32resizeOutStride);
				vst4q_u8((uint8_t*)argbBuff.u64VirtAddr + h * u32yStride + outIdx, s8In0);
				vst4q_u8((uint8_t*)argbBuff.u64VirtAddr + (h+1) * u32yStride + outIdx, s8In0);
				vst4q_u8((uint8_t*)argbBuff.u64VirtAddr + (h+2) * u32yStride + outIdx, s8In0);
				vst4q_u8((uint8_t*)argbBuff.u64VirtAddr + (h+3) * u32yStride + outIdx, s8In0);
				outIdx += 64;
			}
		}
		argbMutex.unlock();
		classMutex.unlock();

		osdMutex.lock();
		STRU_SYS_DMA_TRAN_INFO tran_info = {0};
		tran_info.s32_disable_cci = 1;
		tran_info.s32_tran_cnt_sum = 1;
		tran_info.st_addr_size[0].pv_src_pa = (AR_VOID*)argbBuff.u64PhyAddr;
		tran_info.st_addr_size[0].pv_dst_pa = (AR_VOID*)osdBuff.u64PhyAddr;
		tran_info.st_addr_size[0].u32_size = u32argbSize;
		s32Ret_y = ar_hal_sys_mem_transfer_pa(&tran_info);
		if(s32Ret_y)
			printf("Dma transfer data failed ! \n");

		osdMutex.unlock();
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


