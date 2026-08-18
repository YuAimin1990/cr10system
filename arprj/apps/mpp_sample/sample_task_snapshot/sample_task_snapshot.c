#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <dirent.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>

#include "ar_common.h"
#include "ar_comm_video.h"
#include "sample_comm.h"
#include "hal_vo.h"
#include "hal_vin_log.h"
#include "hal_region.h"
#include "mpi_vb.h"
#include "osal.h"
#include "hal_gpio.h"

#include "yuv2jpg.h"

#define MAX_STR_LEN 256

void usage(char* name)
{
    printf("Usage:\n");
    printf("%s -c [num] -d [outdir]\n", name);
    return;
}

AR_VOID SAMPLE_AR_MPI_VIN_OpenDev(AR_S32 mode,AR_S32 hdr_freq, AR_S32 vif_freq, AR_S32 isp_freq,AR_S32 mipi_freq)
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
   Prop.hdr_fre_hz=hdr_freq;
   Prop.vif_fre_mod=1;
   Prop.vif_fre_hz=vif_freq;
   Prop.isp_fre_mod=1;
   Prop.isp_fre_hz=isp_freq;
   Prop.mipi_fre_mod=1;
   Prop.mipi_fre_hz=mipi_freq;

   AR_MPI_VIN_OpenDev(&Prop);
   return;
}

AR_VOID SAMPLE_AR_MPI_VIN_CloseDev()
{
	return;
}

AR_S32 SAMPLE_VI_Snapshot(char *dir, int num)
{
    AR_S32             s32Ret;

    AR_S32             s32ViCnt       = 1;
    VI_DEV             ViDev          = 0;
    VI_PIPE            ViPipe         = 0;
    VI_CHN             ViChn          = 0;
    AR_S32             s32WorkSnsId   = 0;
    SAMPLE_VI_CONFIG_S stViConfig;

    SIZE_S             stSize;
    VB_CONFIG_S        stVbConf;
    PIC_SIZE_E         enPicSize;
    AR_U32             u32BlkSize;

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
        SAMPLE_PRT("get picture size by sensor failed!\n");
        return s32Ret;
    }

    s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("get picture size failed!\n");
        return s32Ret;
    }

    /*config vb*/
    ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
    stVbConf.u32MaxPoolCnt              = 2;

    u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
    stVbConf.astCommPool[0].u64BlkSize  = u32BlkSize;
    stVbConf.astCommPool[0].u32BlkCnt   = 10;

    u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
    stVbConf.astCommPool[1].u64BlkSize  = u32BlkSize;
    stVbConf.astCommPool[1].u32BlkCnt   = 4;

    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("system init failed with %d!\n", s32Ret);
        return s32Ret;
    }

    SAMPLE_AR_MPI_VIN_OpenDev(1,100000000,300000000,100000000,100000000);

    stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = 0;
    stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = 0;
    stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = 0;
    stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = 0;

    /*start vi*/
    s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
        goto EXIT;
    }

	sleep(1);

    VIDEO_FRAME_INFO_S FrameInfo;
	AR_U64 last_pts=0;

	int i = 0;
	for(i = 0; i < num; i++)
	{
		char out[MAX_STR_LEN] = {0};

		AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 500000);
		last_pts=FrameInfo.stVFrame.u64PTS;

		sprintf(out, "%s/snapshot-%d.jpg", dir, i);

		yuv2jpg(&FrameInfo, out, FrameInfo.stVFrame.u32Stride[0], FrameInfo.stVFrame.u32Height);

		AR_MPI_VI_ReleaseChnFrame(ViPipe,ViChn,&FrameInfo);
	}

    SAMPLE_COMM_VI_StopVi(&stViConfig);
EXIT:
    SAMPLE_COMM_SYS_Exit();
    SAMPLE_AR_MPI_VIN_CloseDev();

    return s32Ret;
}

int main(int argc, char *argv[])
{
    int opt;

	int num = 0;
	char outdir[MAX_STR_LEN] = "/tmp";

    while((opt = getopt(argc, argv, "c:d:")) != -1)
    {
        switch (opt) {
            case 'd':
            	memset(outdir, 0, MAX_STR_LEN);
                strcpy(outdir, optarg);
                break;
            case 'c':
                num = atoi(optarg);
                break;
            default:
                usage((char *)argv[0]);
                exit(1);
        }
    }

	if(num <= 0)
	{
		usage((char *)argv[0]);
		exit(1);
	}

	SAMPLE_VI_Snapshot(outdir, num);

	return 0;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
