#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <sys/select.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <sys/prctl.h>

#include "mpi_vb.h"
#include "mpi_vdec.h"
#include "mpi_venc.h"
#include "vctrl_common.h"
#include "ar_buffer.h"

#define WDR_MAX_PIPE_NUM 1
#define H26X_CORE_CLOCK 500
#define H26X_BPU_CLOCK 360
#define JPEG_CORE_CLOCK 400

FILE *testFp = NULL;

PIC_SIZE_E VCTRL_COMM_SYS_GetEnPicSize(SIZE_S* pstSize)
{
    if(pstSize->u32Width == 352 && pstSize->u32Height == 288)
        return PIC_CIF;

    if(pstSize->u32Width == 720 && pstSize->u32Height == 576)
        return PIC_D1_PAL;

    if(pstSize->u32Width == 720 && pstSize->u32Height == 480)
        return PIC_D1_NTSC;

    if(pstSize->u32Width == 1280 && pstSize->u32Height == 720)
        return PIC_720P;

    if(pstSize->u32Width == 1920 && pstSize->u32Height == 1080)
        return PIC_1080P;

    if(pstSize->u32Width == 2592 && pstSize->u32Height == 1944)
        return PIC_2592x1944;

    if(pstSize->u32Width == 3820 && pstSize->u32Height == 2160)
        return PIC_3840x2160;

    if(pstSize->u32Width == 3000 && pstSize->u32Height == 3000)
        return PIC_3000x3000;

    if(pstSize->u32Width == 4000 && pstSize->u32Height == 3000)
        return PIC_4000x3000;

    if(pstSize->u32Width == 4096 && pstSize->u32Height == 2160)
        return PIC_4096x2160;

    if(pstSize->u32Width == 7680 && pstSize->u32Height == 4320)
        return PIC_7680x4320;

    if(pstSize->u32Width == 3840 && pstSize->u32Height == 8640)
        return PIC_3840x8640;

    return AR_FAILURE;
}

AR_S32 VCTRL_COMM_SYS_GetPicSize(PIC_SIZE_E enPicSize, SIZE_S* pstSize)
{
    switch (enPicSize)
    {
        case PIC_CIF:   /* 352 * 288 */
            pstSize->u32Width  = 352;
            pstSize->u32Height = 288;
            break;

        case PIC_D1_PAL:   /* 720 * 576 */
            pstSize->u32Width  = 720;
            pstSize->u32Height = 576;
            break;

        case PIC_D1_NTSC:   /* 720 * 480 */
            pstSize->u32Width  = 720;
            pstSize->u32Height = 480;
            break;

        case PIC_720P:   /* 1280 * 720 */
            pstSize->u32Width  = 1280;
            pstSize->u32Height = 720;
            break;

        case PIC_1080P:  /* 1920 * 1080 */
            pstSize->u32Width  = 1920;
            pstSize->u32Height = 1080;
            break;

        case PIC_2592x1520:
            pstSize->u32Width  = 2592;
            pstSize->u32Height = 1520;
            break;

        case PIC_2592x1944:
            pstSize->u32Width  = 2592;
            pstSize->u32Height = 1944;
            break;

        case PIC_3840x2160:
            pstSize->u32Width  = 3840;
            pstSize->u32Height = 2160;
            break;

        case PIC_3000x3000:
            pstSize->u32Width  = 3000;
            pstSize->u32Height = 3000;
            break;

        case PIC_4000x3000:
            pstSize->u32Width  = 4000;
            pstSize->u32Height = 3000;
            break;

        case PIC_4096x2160:
            pstSize->u32Width  = 4096;
            pstSize->u32Height = 2160;
            break;

        case PIC_7680x4320:
            pstSize->u32Width  = 7680;
            pstSize->u32Height = 4320;
            break;
        case PIC_3840x8640:
            pstSize->u32Width = 3840;
            pstSize->u32Height = 8640;
            break;
        default:
            return AR_FAILURE;
    }

    return AR_SUCCESS;
}

AR_S32 VCTRL_COMM_VB_Init(VB_CONFIG_S* pstVbConfig)
{
    AR_S32 s32Ret = AR_FAILURE;

    AR_MPI_VB_Exit();

    if (NULL == pstVbConfig)
    {
        VCTRL_PRT("input parameter is null, it is invaild!\n");
        return AR_FAILURE;
    }

    s32Ret = AR_MPI_VB_SetConfig(pstVbConfig);

    if (AR_SUCCESS != s32Ret)
    {
        VCTRL_PRT("AR_MPI_VB_SetConf failed!\n");
        return AR_FAILURE;
    }

    s32Ret = AR_MPI_VB_Init();

    if (AR_SUCCESS != s32Ret)
    {
        VCTRL_PRT("AR_MPI_VB_Init failed!\n");
        return AR_FAILURE;
    }

    return AR_SUCCESS;
}

AR_VOID VCTRL_COMM_VB_Exit(void)
{
    AR_MPI_VB_ExitModCommPool(VB_UID_VDEC);
    AR_MPI_VB_Exit();
    return;
}

AR_S32 VCTRL_COMM_SYS_Init(VB_CONFIG_S* pstVbConfig)
{
    AR_S32 s32Ret = AR_FAILURE;
    static int AR_MPI_VB_SetConfig_called = 0;

    if (NULL == pstVbConfig) {
        VCTRL_PRT("input parameter is null, it is invaild!\n");
        return AR_FAILURE;
    }

    if (AR_MPI_VB_SetConfig_called == 0) {
        s32Ret = AR_MPI_VB_SetConfig(pstVbConfig);

        if (AR_SUCCESS != s32Ret) {
            VCTRL_PRT("AR_MPI_VB_SetConf failed s32Ret = %d !\n", s32Ret);
            //this function only set onetime
            //return AR_FAILURE;
        }

        AR_MPI_VB_SetConfig_called = 1;

        s32Ret = AR_MPI_VB_Init();

        if (AR_SUCCESS != s32Ret) {
            VCTRL_PRT("AR_MPI_VB_Init failed!\n");
            return AR_FAILURE;
        }

    }

    return AR_SUCCESS;
}

AR_VOID VCTRL_COMM_SYS_Exit(void)
{
    AR_MPI_SYS_Exit();
    AR_MPI_VB_ExitModCommPool(VB_UID_VDEC);
    AR_MPI_VB_Exit();
    return;
}

AR_S32 VCTRL_COMM_CORE_Init(PAYLOAD_TYPE_E type, VIDEO_PROCESSOR_TYPE_E processor,
                                AR_U32 coreClock, AR_U32 bpuClock, AR_U32 jpuClock)
{
    static int enc_jpeg_core_inited = 0;
    static int enc_h26x_core_inited = 0;
    static int dec_jpeg_core_inited = 0;
    static int dec_h26x_core_inited = 0;
    VENC_PARAM_MOD_S stParamMod = { 0 };
    VDEC_MOD_PARAM_S stModParam = {0};
    AR_U32 u32CoreClock = 0, u32BpuClock = 0, u32JpuClock = 0;

    if (coreClock != 0 && coreClock < 75) {
        u32CoreClock = 75;
    } else {
        u32CoreClock = coreClock;
        if (u32CoreClock == 0) {
            u32CoreClock = H26X_CORE_CLOCK;
        }
    }

    if (bpuClock != 0 && bpuClock < 75) {
        u32BpuClock = 75;
    } else {
        u32BpuClock = bpuClock;
        if (u32BpuClock == 0) {
            u32BpuClock = H26X_BPU_CLOCK;
        }
    }

    if (jpuClock != 0 && jpuClock < 75) {
        u32JpuClock = 75;
    } else {
        u32JpuClock = jpuClock;
        if (u32JpuClock == 0) {
            u32JpuClock = JPEG_CORE_CLOCK;
        }
    }

    switch(type) {
        case PT_H264:
        case PT_H265:
            if (!enc_h26x_core_inited || !dec_h26x_core_inited) {
                printf("VCTRL_COMM_CORE_Init !\n");
                if (VIDEO_ENCODER == processor) {
                    stParamMod.enVencModType = MODTYPE_H265E;
                    AR_MPI_VENC_GetModParam(&stParamMod);
                    stParamMod.stH265eModParam.u32CoreClock = u32CoreClock;
                    stParamMod.stH265eModParam.u32BpuClock = u32BpuClock;//max 1080p@240fps
                    stParamMod.stEventModParam.u32VencIrqQueueSize = 128;
                    stParamMod.stEventModParam.u32VencTaskQueueSize = 128;
                    stParamMod.stEventModParam.u32VencDoneQueueSize = 512;
                    stParamMod.stEventModParam.u32VencOutQueueSize = 512;
                    stParamMod.stEventModParam.u32VencEventQueueSize = 512;
                    AR_MPI_VENC_SetModParam(&stParamMod);
                    enc_h26x_core_inited = 1;
                } else if (VIDEO_DECODER == processor) {
                    AR_MPI_VDEC_GetModParam(&stModParam);
                    /*
                    stModParam.enVdecVBSource = VB_SOURCE_MODULE;
                    */
                    stModParam.enVdecVBSource = VB_SOURCE_PRIVATE;
                    stModParam.stVideoModParam.u32CoreClock = u32CoreClock;
                    stModParam.stVideoModParam.u32BpuClock = u32BpuClock;//max 1080p@240fps
                    stModParam.stEventModParam.u32VdecIrqQueueSize = 128;
                    stModParam.stEventModParam.u32VdecTaskQueueSize = 128;
                    stModParam.stEventModParam.u32VdecDoneQueueSize = 512;// dec 32 channel
                    stModParam.stEventModParam.u32VdecOutQueueSize = 512;
                    stModParam.stEventModParam.u32VdecEventQueueSize = 512;
                    AR_MPI_VDEC_SetModParam(&stModParam);
                    dec_h26x_core_inited = 1;
                }
            }
            break;
        case PT_JPEG:
            if (!enc_jpeg_core_inited || !dec_jpeg_core_inited) {
                if (VIDEO_ENCODER == processor) {
                    stParamMod.enVencModType = MODTYPE_JPEGE;
                    AR_MPI_VENC_GetModParam(&stParamMod);
                    stParamMod.stJpegeModParam.u32CoreClock = u32JpuClock;//1080p@30fps
                    stParamMod.stEventModParam.u32VencIrqQueueSize = 128;
                    stParamMod.stEventModParam.u32VencTaskQueueSize = 128;
                    stParamMod.stEventModParam.u32VencDoneQueueSize = 512;
                    stParamMod.stEventModParam.u32VencOutQueueSize = 512;
                    stParamMod.stEventModParam.u32VencEventQueueSize = 512;
                    AR_MPI_VENC_SetModParam(&stParamMod);
                    enc_jpeg_core_inited = 1;
                } else if (VIDEO_DECODER == processor) {
                    AR_MPI_VDEC_GetModParam(&stModParam);
                    stModParam.enVdecVBSource = VB_SOURCE_MODULE;
                    stModParam.stPictureModParam.u32CoreClock = u32JpuClock;//1080p@30fps
                    stModParam.stEventModParam.u32VdecIrqQueueSize = 128;
                    stModParam.stEventModParam.u32VdecTaskQueueSize = 128;
                    stModParam.stEventModParam.u32VdecDoneQueueSize = 512;
                    stModParam.stEventModParam.u32VdecOutQueueSize = 512;
                    stModParam.stEventModParam.u32VdecEventQueueSize = 512;
                    AR_MPI_VDEC_SetModParam(&stModParam);
                    dec_jpeg_core_inited = 1;
                }
            }
            break;
        default:
            break;
    }

    return AR_SUCCESS;
}


AR_VOID VCTRL_VI_ReadOneFrame( FILE* fp, AR_U8* pY, AR_U8* pU, AR_U8* pV,
                               AR_U32 width, AR_U32 height, AR_U32 stride, AR_U32 stride2)
{
    AR_U8 *pDst;
    AR_U32 u32Row;
    pDst = pY;

    //printf("pY=%p pU=%p pV=%p, width=%d,height=%d,stride=%d,stride2=%d\n",pY, pU, pV,width,height,stride,stride2);

    for (u32Row = 0; u32Row < height; u32Row++)
    {
        fread(pDst, width, 1, fp);
        pDst += stride;
    }

    pDst = pU;
    for (u32Row = 0; u32Row < height / 2; u32Row++)
    {
        fread(pDst, width / 2, 1, fp);
        pDst += stride2;
    }

    pDst = pV;
    for (u32Row = 0; u32Row < height / 2; u32Row++)
    {
        fread(pDst, width / 2, 1, fp);
        pDst += stride2;
    }

    char temp[1];
    fread(temp, 1, 1, fp);

    if (feof(fp) != 0)
    {
        //printf("file is EOF!\n");
        rewind(fp);
    }
    else
    {
        fseek(fp, ftell(fp) - 1, SEEK_SET);
    }
}


AR_VOID VCTRL_VI_ReadOneFrame_422( FILE* fp, AR_U8* pY, AR_U8* pU, AR_U8* pV,
                                   AR_U32 width, AR_U32 height, AR_U32 stride, AR_U32 stride2)
{
    AR_U8 *pDst;
    AR_U32 u32Row;
    pDst = pY;


    for (u32Row = 0; u32Row < height; u32Row++)
    {
        fread(pDst, width, 1, fp);
        pDst += stride;
    }

    pDst = pU;
    for (u32Row = 0; u32Row < height; u32Row++)
    {
        fread(pDst, width >> 1, 1, fp);
        pDst += stride2;
    }

    pDst = pV;
    for (u32Row = 0; u32Row < height; u32Row++)
    {
        fread(pDst, width >> 1, 1, fp);
        pDst += stride2;
    }

    char temp[1];
    fread(temp, 1, 1, fp);

    if (feof(fp) != 0)
    {
        printf("the End-of-file is reached\n");
    }
    else
    {
        fseek(fp, ftell(fp) - 1, SEEK_SET);
    }

}

AR_S32 VCTRL_VI_PlanToSemi(AR_U8* pY, AR_S32 yStride,
                           AR_U8* pU, AR_S32 uStride,
                           AR_U8* pV, AR_S32 vStride,
                           AR_S32 picWidth, AR_S32 picHeight)
{
    AR_S32 i;
    AR_U8 *pTmpU = NULL, *ptu = NULL;
    AR_U8 *pTmpV = NULL, *ptv = NULL;

    AR_S32 s32HafW = uStride >> 1 ;
    AR_S32 s32HafH = picHeight >> 1 ;
    AR_S32 s32Size = s32HafW * s32HafH;

    pTmpU = malloc(s32Size);
    if (!pTmpU) {
        return AR_FAILURE;
    }
    ptu = pTmpU;
    pTmpV = malloc(s32Size);
    if (!pTmpV) {
        free(ptu);
        pTmpU = NULL;
        return AR_FAILURE;
    }
    ptv = pTmpV;

    memcpy(pTmpU, pU, s32Size);
    memcpy(pTmpV, pV, s32Size);

    for (i = 0; i<s32Size >> 1; i++)
    {
        *pU++ = *pTmpV++;
        *pU++ = *pTmpU++;

    }
    for (i = 0; i<s32Size >> 1; i++)
    {
        *pV++ = *pTmpV++;
        *pV++ = *pTmpU++;
    }

    free(ptu);
    free(ptv);

    pTmpU = NULL;
    pTmpV = NULL;

    return AR_SUCCESS;
}


AR_S32 VCTRL_VI_PlanToSemi_422(AR_U8* pY, AR_S32 yStride,
                               AR_U8* pU, AR_S32 uStride,
                               AR_U8* pV, AR_S32 vStride,
                               AR_S32 picWidth, AR_S32 picHeight)
{
    AR_S32 i;
    AR_U8 *pTmpU = NULL, *ptu = NULL;
    AR_U8 *pTmpV = NULL, *ptv = NULL;
    AR_S32 s32HafW = uStride >> 1;
    AR_S32 s32HafH = picHeight;
    AR_S32 s32Size = s32HafW * s32HafH;

    pTmpU = malloc(s32Size);
    if (!pTmpU) {
        return AR_FAILURE;
    }
    ptu = pTmpU;
    pTmpV = malloc(s32Size);
    if (!pTmpV) {
        free(ptu);
        pTmpU = NULL;
        return AR_FAILURE;
    }
    ptv = pTmpV;

    memcpy(pTmpU, pU, s32Size);
    memcpy(pTmpV, pV, s32Size);

    for (i = 0; i<s32Size >> 1; i++)
    {
        *pU++ = *pTmpV++;
        *pU++ = *pTmpU++;

    }

    for (i = 0; i<s32Size >> 1; i++)
    {
        *pV++ = *pTmpV++;
        *pV++ = *pTmpU++;
    }

    free(ptu);
    free(ptv);

    pTmpU = NULL;
    pTmpV = NULL;

    return AR_SUCCESS;
}

AR_VOID VCTRL_VI_ReadOneFrame_400( FILE* fp, AR_U8* pY, AR_U8* pU, AR_U8* pV,
                                   AR_U32 width, AR_U32 height, AR_U32 stride, AR_U32 stride2)
{
    AR_U8 *pDst;
    AR_U32 u32Row;

    pDst = pY;
    for (u32Row = 0; u32Row < height; u32Row++)
    {
        fread(pDst, width, 1, fp);
        pDst += stride;
    }

    char temp[1];
    fread(temp, 1, 1, fp);

    if (feof(fp) != 0)
    {
        printf("file is EOF!\n");
    }
    else
    {
        fseek(fp, ftell(fp) - 1, SEEK_SET);
    }

}


static AR_VOID VCTRL_VI_COMM_ReadYuvFile(FILE *pfd, VIDEO_FRAME_INFO_S *pstVideoFrameInfo)
{
    if (pstVideoFrameInfo->stVFrame.enPixelFormat == PIXEL_FORMAT_YVU_SEMIPLANAR_422) {
        VCTRL_VI_ReadOneFrame_422(pfd, (AR_U8 *)(AR_U64)pstVideoFrameInfo->stVFrame.u64VirAddr[0],
                                   (AR_U8 *)(AR_U64)pstVideoFrameInfo->stVFrame.u64VirAddr[1], (AR_U8 *)(AR_U64)pstVideoFrameInfo->stVFrame.u64VirAddr[2],
                                   pstVideoFrameInfo->stVFrame.u32Width, pstVideoFrameInfo->stVFrame.u32Height,
                                   pstVideoFrameInfo->stVFrame.u32Stride[0], pstVideoFrameInfo->stVFrame.u32Stride[1] >> 1);

        VCTRL_VI_PlanToSemi_422((AR_U8 *)(AR_U64)pstVideoFrameInfo->stVFrame.u64VirAddr[0], pstVideoFrameInfo->stVFrame.u32Stride[0],
                                 (AR_U8 *)(AR_U64)pstVideoFrameInfo->stVFrame.u64VirAddr[1], pstVideoFrameInfo->stVFrame.u32Stride[1],
                                 (AR_U8 *)(AR_U64)pstVideoFrameInfo->stVFrame.u64VirAddr[2], pstVideoFrameInfo->stVFrame.u32Stride[1],
                                 pstVideoFrameInfo->stVFrame.u32Width, pstVideoFrameInfo->stVFrame.u32Height);
    } else if (pstVideoFrameInfo->stVFrame.enPixelFormat == PIXEL_FORMAT_YVU_SEMIPLANAR_420) {
        VCTRL_VI_ReadOneFrame(pfd, (AR_U8 *)(AR_U64)pstVideoFrameInfo->stVFrame.u64VirAddr[0],
                               (AR_U8 *)(AR_U64)pstVideoFrameInfo->stVFrame.u64VirAddr[1], (AR_U8 *)(AR_U64)pstVideoFrameInfo->stVFrame.u64VirAddr[2],
                               pstVideoFrameInfo->stVFrame.u32Width, pstVideoFrameInfo->stVFrame.u32Height,
                               pstVideoFrameInfo->stVFrame.u32Stride[0], pstVideoFrameInfo->stVFrame.u32Stride[1] >> 1);

        VCTRL_VI_PlanToSemi((AR_U8 *)(AR_U64)pstVideoFrameInfo->stVFrame.u64VirAddr[0], pstVideoFrameInfo->stVFrame.u32Stride[0],
                             (AR_U8 *)(AR_U64)pstVideoFrameInfo->stVFrame.u64VirAddr[1], pstVideoFrameInfo->stVFrame.u32Stride[1],
                             (AR_U8 *)(AR_U64)pstVideoFrameInfo->stVFrame.u64VirAddr[2], pstVideoFrameInfo->stVFrame.u32Stride[1],
                             pstVideoFrameInfo->stVFrame.u32Width, pstVideoFrameInfo->stVFrame.u32Height);
    } else {
    /*
        VCTRL_VI_ReadOneFrame_400(pfd, (AR_U8 *)(AR_U64)pstVideoFrameInfo->stVFrame.u64VirAddr[0],
                                   (AR_U8 *)(AR_U64)pstVideoFrameInfo->stVFrame.u64VirAddr[1], (AR_U8 *)(AR_U64)pstVideoFrameInfo->stVFrame.u64VirAddr[2],
                                   pstVideoFrameInfo->stVFrame.u32Width, pstVideoFrameInfo->stVFrame.u32Height,
                                   pstVideoFrameInfo->stVFrame.u32Stride[0], pstVideoFrameInfo->stVFrame.u32Stride[1] >> 1);
                                   */
        VCTRL_VI_ReadOneFrame(pfd, (AR_U8 *)(AR_U64)pstVideoFrameInfo->stVFrame.u64VirAddr[0],
                               (AR_U8 *)(AR_U64)pstVideoFrameInfo->stVFrame.u64VirAddr[1], (AR_U8 *)(AR_U64)pstVideoFrameInfo->stVFrame.u64VirAddr[2],
                               pstVideoFrameInfo->stVFrame.u32Width, pstVideoFrameInfo->stVFrame.u32Height,
                               pstVideoFrameInfo->stVFrame.u32Stride[0], ALIGN_UP(pstVideoFrameInfo->stVFrame.u32Width / 2, 32));
    }
}

AR_S32 VCTRL_VI_GetFrameBlkInfo(VCTRL_VI_FRAME_CONFIG_S *pstFrmCfg, AR_S32 s32FrmCnt, VCTRL_VI_FRAME_INFO_S *pastViFrameInfo)
{
    AR_U32 i = 0;
    AR_U32 u32Stride;
    AR_U32 u32LStride;
    AR_U32 u32CStride;
    AR_U32 u32LumaSize = 0;
    AR_U32 u32ChrmSize = 0;
    AR_U32 u32Size;
    AR_U64 u64PhyAddr;
    AR_U8 *pVirAddr;
    VB_POOL u32PoolId;
    VB_BLK VbBlk;
    PIXEL_FORMAT_E enPixelFormat;
    /*
    VB_POOL_CONFIG_S stVbPoolCfg;
    */
    AR_S32 s32Ret;

    enPixelFormat = pstFrmCfg->enPixelFormat;

    if (enPixelFormat == PIXEL_FORMAT_YVU_SEMIPLANAR_422) {
        u32Stride = ALIGN_UP((pstFrmCfg->u32Width * 8 + 7) >> 3, DEFAULT_ALIGN);
        u32LStride  = u32Stride;
        u32CStride  = u32Stride;
        u32Size = u32Stride * pstFrmCfg->u32Height * 2;
        u32LumaSize = u32Stride * pstFrmCfg->u32Height;
        u32ChrmSize = u32Stride * pstFrmCfg->u32Height / 2;
    } else if (enPixelFormat == PIXEL_FORMAT_YVU_SEMIPLANAR_420) {
        u32Stride = ALIGN_UP((pstFrmCfg->u32Width * 8 + 7) >> 3, DEFAULT_ALIGN);
        u32LStride  = u32Stride;
        u32CStride  = u32Stride;
        u32Size = u32Stride * pstFrmCfg->u32Height * 3 / 2;
        u32LumaSize = u32Stride * pstFrmCfg->u32Height;
        u32ChrmSize = u32Stride * pstFrmCfg->u32Height / 4;
    } else if (enPixelFormat == PIXEL_FORMAT_YUV_400) {
        u32Stride = ALIGN_UP((pstFrmCfg->u32Width * 8 + 7) >> 3, DEFAULT_ALIGN);
        u32LStride  = u32Stride;
        u32CStride  = u32Stride;
        u32Size = u32Stride * pstFrmCfg->u32Height;
        u32LumaSize = u32Size;
        u32ChrmSize = 0;
    } else {
        u32Stride = ALIGN_UP((pstFrmCfg->u32Width * 8 + 7) >> 3, DEFAULT_ALIGN);
        u32LStride  = u32Stride;
        u32CStride  = u32Stride;
        u32Size = u32Stride * pstFrmCfg->u32Height * 3 / 2;
        u32LumaSize = u32Stride * pstFrmCfg->u32Height;
        u32ChrmSize = u32Stride * pstFrmCfg->u32Height / 4;

        AR_S32 alignedHeight = ALIGN_UP(pstFrmCfg->u32Height, 32);

        u32Stride   = ALIGN_UP(pstFrmCfg->u32Width, 32);
        u32CStride  = ALIGN_UP(pstFrmCfg->u32Width / 2, 32);


        u32LumaSize = u32Stride * alignedHeight;
        u32ChrmSize = u32CStride * alignedHeight / 2;

        u32Size     = u32LumaSize + u32ChrmSize * 2;
    }

#if 0 //not work
    memset(&stVbPoolCfg, 0, sizeof(VB_POOL_CONFIG_S));
    stVbPoolCfg.u64BlkSize  = u32Size;
    stVbPoolCfg.u32BlkCnt   = s32FrmCnt;
    stVbPoolCfg.enRemapMode = VB_REMAP_MODE_NONE;
    u32PoolId = AR_MPI_VB_CreatePool(&stVbPoolCfg);
printf("emma u32PoolId=%d,u64BlkSize=%d,u32BlkCnt=%d\n",u32PoolId,stVbPoolCfg.u64BlkSize,stVbPoolCfg.u32BlkCnt);

    if (VB_INVALID_POOLID == u32PoolId)
    {
        VCTRL_PRT("AR_MPI_VB_CreatePool failed!\n");
        return AR_FAILURE;
    }
#endif

    for (i = 0; i < s32FrmCnt; i++)
    {

#if 0
        VbBlk = AR_MPI_VB_GetBlock(u32PoolId, u32Size, AR_NULL);
        if (VB_INVALID_HANDLE == VbBlk)
        {
            VCTRL_PRT("AR_MPI_VB_GetBlock err! size:%d\n", u32Size);
            return AR_FAILURE;
        }

        u64PhyAddr = AR_MPI_VB_Handle2PhysAddr(VbBlk);
        if (0 == u64PhyAddr)
        {
            VCTRL_PRT("AR_MPI_VB_Handle2PhysAddr err!\n");
            return AR_FAILURE;
        }
        pVirAddr = (AR_U8 *)AR_MPI_SYS_Mmap(u64PhyAddr, u32Size);
        if (NULL == pVirAddr)
        {
            VCTRL_PRT("AR_MPI_SYS_Mmap err!\n");
            return AR_FAILURE;
        }

#else
        u32Size = pastViFrameInfo->u32Size;
        VbBlk = AR_MPI_VB_GetBlock(pastViFrameInfo->u32PoolId, u32Size, AR_NULL);
        if (VB_INVALID_HANDLE == VbBlk) {
            VCTRL_PRT("AR_MPI_VB_GetBlock err! size:%d\n", u32Size);
            return AR_FAILURE;
        }

        u32PoolId  = AR_MPI_VB_Handle2PoolId(VbBlk);
        if (VB_INVALID_POOLID == u32PoolId) {
            VCTRL_PRT("AR_MPI_VB_CreatePool failed!\n");
            return AR_FAILURE;
        }

/*
        s32Ret = AR_MPI_VB_MmapPool(u32PoolId);
        if (s32Ret) {
            VCTRL_PRT("mmap pool failed\n");
            return AR_FAILURE;
        } else
            logline("AR_MPI_VB_MmapPool on pool %u ok", u32PoolId);

*/

        u64PhyAddr = AR_MPI_VB_Handle2PhysAddr(VbBlk);
        if (0 == u64PhyAddr) {
            VCTRL_PRT("AR_MPI_VB_Handle2PhysAddr err!\n");
            return AR_FAILURE;
        }

        s32Ret = AR_MPI_VB_GetBlockVirAddr(/*u32PoolId*/pastViFrameInfo->u32PoolId, u64PhyAddr, (AR_VOID **)(&pVirAddr));
        if (s32Ret) {
            logline("get frame virtual address failed: %#x", s32Ret);
            return AR_FAILURE;
        }
/*
        AR_U8 *pTestVirAddr = (AR_U8 *)AR_MPI_SYS_Mmap(u64PhyAddr, u32Size);
        if (NULL == pTestVirAddr)
        {
            VCTRL_PRT("AR_MPI_SYS_Mmap err!\n");
            return AR_FAILURE;
        }
*/
#endif
        //VCTRL_PRT("u64PhyAddr = %p, pVirAddr= %p, pTestVirAddr= %p\n",u64PhyAddr, pVirAddr, pTestVirAddr);

        pastViFrameInfo[i].stVideoFrameInfo.u32PoolId = u32PoolId;
        pastViFrameInfo[i].stVideoFrameInfo.enModId = AR_ID_VI;
        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u64PhyAddr[0]   = u64PhyAddr;
        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u64PhyAddr[1]   = pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u64PhyAddr[0] + u32LumaSize;
        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u64PhyAddr[2]   = pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u64PhyAddr[1] + u32ChrmSize;

        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u64VirAddr[0]   = (AR_U64)pVirAddr;
        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u64VirAddr[1]   = pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u64VirAddr[0] + u32LumaSize;
        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u64VirAddr[2]   = pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u64VirAddr[1] + u32ChrmSize;

        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u32Stride[0]    = u32LStride;
        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u32Stride[1]    = u32CStride;
        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u32Stride[2]    = u32CStride;

        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u32Width        = pstFrmCfg->u32Width;
        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u32Height       = pstFrmCfg->u32Height;
        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.enPixelFormat   = pstFrmCfg->enPixelFormat;
        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.enVideoFormat   = pstFrmCfg->enVideoFormat;
        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.enCompressMode  = pstFrmCfg->enCompressMode;
        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.enField         = VIDEO_FIELD_FRAME;
        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.enDynamicRange  = DYNAMIC_RANGE_SDR8;
        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.enColorGamut    = COLOR_GAMUT_BT709;

        pastViFrameInfo[i].VbBlk   = VbBlk;
        pastViFrameInfo[i].u32Size = u32Size;
    }

    return AR_SUCCESS;
}

AR_S32 VCTRL_VI_COMM_ReleaseFrameBlkInfo(AR_S32 s32FrmCnt, VCTRL_VI_FRAME_INFO_S *pastViFrameInfo)
{
    AR_S32 s32Ret = AR_SUCCESS;
    AR_U32 i;
    VB_POOL u32PoolId;
    VB_BLK VbBlk;
    /*
    AR_U32 u32Size;
    */

    for (i = 0; i < s32FrmCnt; i++)
    {
        VbBlk = pastViFrameInfo[i].VbBlk;
        s32Ret = AR_MPI_VB_ReleaseBlock(VbBlk);
        if (s32Ret != AR_SUCCESS)
        {
            VCTRL_PRT("AR_MPI_VB_ReleaseBlock block 0x%x failure\n", VbBlk);
        }

        //u32Size = pastViFrameInfo[i].u32Size;
        //s32Ret = AR_MPI_SYS_Munmap((AR_VOID*)(AR_U64)pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u64VirAddr[0], u32Size);
        //if (s32Ret != AR_SUCCESS)
        //{
        //    VCTRL_PRT("AR_MPI_SYS_Munmap failure!\n");
        //}
    }

#if 0
    u32PoolId = pastViFrameInfo[0].stVideoFrameInfo.u32PoolId;


    s32Ret = AR_MPI_VB_MunmapPool(u32PoolId);
    if (s32Ret) {
        VCTRL_PRT("mmap pool failed\n");
    }
    //AR_MPI_VB_DestroyPool(u32PoolId);
#endif

    return AR_SUCCESS;
}


AR_S32 VCTRL_COMM_VI_Load_UserPic(FILE* pfd, VI_USERPIC_ATTR_S *pstUsrPic, VCTRL_VI_FRAME_INFO_S *pstViFrameInfo)
{
    VCTRL_VI_FRAME_CONFIG_S stFrmCfg;
    AR_S32 s32Ret;

    stFrmCfg.u32Width = pstUsrPic->unUsrPic.stUsrPicFrm.stVFrame.u32Width;
    stFrmCfg.u32Height = pstUsrPic->unUsrPic.stUsrPicFrm.stVFrame.u32Height;
    stFrmCfg.u32ByteAlign = 0;
    stFrmCfg.enPixelFormat = pstUsrPic->unUsrPic.stUsrPicFrm.stVFrame.enPixelFormat;
    stFrmCfg.enCompressMode = COMPRESS_MODE_NONE;
    stFrmCfg.enVideoFormat = VIDEO_FORMAT_LINEAR;
    stFrmCfg.enDynamicRange = DYNAMIC_RANGE_SDR8;

    s32Ret = VCTRL_VI_GetFrameBlkInfo(&stFrmCfg, 1, pstViFrameInfo);

    if (AR_SUCCESS != s32Ret)
    {
        VCTRL_PRT("VCTRL_VI_GetFrameBlkInfo failed!\n");
        return s32Ret;
    }

    memcpy(&pstUsrPic->unUsrPic.stUsrPicFrm, &pstViFrameInfo->stVideoFrameInfo, sizeof(VIDEO_FRAME_INFO_S));
    VCTRL_VI_COMM_ReadYuvFile(pfd, &pstUsrPic->unUsrPic.stUsrPicFrm);
    return AR_SUCCESS;

/*
EXIT:
*/
    VCTRL_VI_COMM_ReleaseFrameBlkInfo(1, pstViFrameInfo);
    return s32Ret;
}

AR_VOID VCTRL_COMM_VI_Release_UserPic(VCTRL_VI_FRAME_INFO_S *pstViFrameInfo)
{
    VCTRL_VI_COMM_ReleaseFrameBlkInfo(1, pstViFrameInfo);
}

