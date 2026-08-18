

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>

#include "sample_comm.h"





static AR_S32 s_s32SampleMemDev = -1;
#define SAMPLE_MEM_DEV_OPEN if (s_s32SampleMemDev <= 0)\
{\
    s_s32SampleMemDev = open("/dev/mem", O_RDWR|O_SYNC);\
    if (s_s32SampleMemDev < 0)\
    {\
        perror("Open dev/mem error");\
        return NULL;\
    }\
}\


AR_VOID * SAMPLE_SYS_IOMmap(AR_U64 u64PhyAddr, AR_U32 u32Size)
{
    AR_U32 u32Diff;
    AR_U64 u64PagePhy;
    AR_U8 * pPageAddr;
    AR_U64    ulPageSize;

    SAMPLE_MEM_DEV_OPEN;

    /**********************************************************
    PageSize will be 0 when u32size is 0 and u32Diff is 0,
    and then mmap will be error (error: Invalid argument)
    ***********************************************************/
    if (!u32Size)
    {
        printf("Func: %s u32Size can't be 0.\n", __FUNCTION__);
        return NULL;
    }

    /* The mmap address should align with page */
    u64PagePhy = u64PhyAddr & 0xfffffffffffff000ULL;
    u32Diff    = u64PhyAddr - u64PagePhy;

    /* The mmap size shuld be mutliples of 1024 */
    ulPageSize = ((u32Size + u32Diff - 1) & 0xfffff000UL) + 0x1000;

    pPageAddr    = mmap ((void *)0, ulPageSize, PROT_READ|PROT_WRITE,
                                    MAP_SHARED, s_s32SampleMemDev, u64PagePhy);
    if (MAP_FAILED == pPageAddr )
    {
        perror("mmap error");
        return NULL;
    }
    return (AR_VOID *) (pPageAddr + u32Diff);
}


AR_S32 SAMPLE_SYS_Munmap(AR_VOID* pVirAddr, AR_U32 u32Size)
{
    AR_U64 u64PageAddr;
    AR_U32 u32PageSize;
    AR_U32 u32Diff;

    u64PageAddr = (((AR_U64)pVirAddr) & 0xfffffffffffff000ULL);
    u32Diff     = (AR_U64)pVirAddr - u64PageAddr;
    u32PageSize = ((u32Size + u32Diff - 1) & 0xfffff000UL) + 0x1000;

    return munmap((AR_VOID*)(AR_U64)u64PageAddr, u32PageSize);
}



AR_S32 SAMPLE_SYS_SetReg(AR_U64 u64Addr, AR_U32 u32Value)
{
    AR_U32 *pu32RegAddr = NULL;
    AR_U32 u32MapLen = sizeof(u32Value);

    pu32RegAddr = (AR_U32 *)SAMPLE_SYS_IOMmap(u64Addr, u32MapLen);
    if(NULL == pu32RegAddr)
    {
        return AR_FAILURE;
    }

    *pu32RegAddr = u32Value;

    return SAMPLE_SYS_Munmap(pu32RegAddr, u32MapLen);
}

AR_S32 SAMPLE_SYS_GetReg(AR_U64 u64Addr, AR_U32 *pu32Value)
{
    AR_U32 *pu32RegAddr = NULL;
    AR_U32 u32MapLen;

    if (NULL == pu32Value)
    {
        return AR_ERR_SYS_NULL_PTR;
    }

    u32MapLen = sizeof(*pu32Value);
    pu32RegAddr = (AR_U32 *)SAMPLE_SYS_IOMmap(u64Addr, u32MapLen);
    if(NULL == pu32RegAddr)
    {
        return AR_FAILURE;
    }

    *pu32Value = *pu32RegAddr;

    return SAMPLE_SYS_Munmap(pu32RegAddr, u32MapLen);
}



/******************************************************************************
* function : get picture size(w*h), according enPicSize
******************************************************************************/
AR_S32 SAMPLE_COMM_SYS_GetPicSize(PIC_SIZE_E enPicSize, SIZE_S* pstSize)
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

        case PIC_1080P_BIG_PIC:  /* 960 * 1080 */
            pstSize->u32Width  = 960;
            pstSize->u32Height = 1080;
            break;

        case PIC_8K_BIG_PIC:  /* 4032 * 6048 */
            pstSize->u32Width  = 4032;
            pstSize->u32Height = 6048;
            break;

        case PIC_2592x1520:
            pstSize->u32Width  = 2592;
            pstSize->u32Height = 1520;
            break;
        case PIC_2688x1520:
            pstSize->u32Width = 2688;
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
		case PIC_2712x1538:
			pstSize->u32Width = 2712;
			pstSize->u32Height = 1538;
			break;
		case PIC_1920x2880:  /* 1920 * 2880 */
			pstSize->u32Width  = 1920;
			pstSize->u32Height = 2880;
			break;
        default:
            return AR_FAILURE;
    }

    return AR_SUCCESS;
}




/******************************************************************************
* function : Set system memory location
******************************************************************************/
AR_S32 SAMPLE_COMM_SYS_MemConfig(AR_VOID)
{
    AR_S32 i, j;
    AR_S32 s32Ret = AR_SUCCESS;
    AR_CHAR* pcMmzName = NULL;
    MPP_CHN_S stMppChn;

    /*config memory for vi*/
    for (i = 0; i < VI_MAX_PIPE_NUM; i++)
    {
        for (j = 0; j < VI_MAX_CHN_NUM; j++)
        {
            stMppChn.enModId  = AR_ID_VI;
            stMppChn.s32DevId = i;
            stMppChn.s32ChnId = j;
            s32Ret = AR_MPI_SYS_SetMemConfig(&stMppChn, pcMmzName);

            if (s32Ret)
            {
                SAMPLE_PRT("AR_MPI_SYS_SetMemConfig ERR !\n");
                return AR_FAILURE;
            }
        }
    }

    /*config memory for avs */
    for (i = 0; i < AVS_MAX_GRP_NUM; i++)
    {
        stMppChn.enModId  = AR_ID_AVS;
        stMppChn.s32DevId = i;
        stMppChn.s32ChnId = 0;
        s32Ret = AR_MPI_SYS_SetMemConfig(&stMppChn, pcMmzName);

        if (s32Ret)
        {
            SAMPLE_PRT("AR_MPI_SYS_SetMemConfig ERR !\n");
            return AR_FAILURE;
        }
    }

    /*config memory for vpss */
    for (i = 0; i < VPSS_MAX_GRP_NUM; i++)
    {
        stMppChn.enModId  = AR_ID_VPSS;
        stMppChn.s32DevId = i;
        stMppChn.s32ChnId = 0;
        s32Ret = AR_MPI_SYS_SetMemConfig(&stMppChn, pcMmzName);

        if (s32Ret)
        {
            SAMPLE_PRT("AR_MPI_SYS_SetMemConfig ERR !\n");
            return AR_FAILURE;
        }
    }

    /*config memory for venc */
    for (i = 0; i < VENC_MAX_CHN_NUM; i++)
    {

        stMppChn.enModId  = AR_ID_VENC;
        stMppChn.s32DevId = 0;
        stMppChn.s32ChnId = i;
        s32Ret = AR_MPI_SYS_SetMemConfig(&stMppChn, pcMmzName);

        if (s32Ret)
        {
            SAMPLE_PRT("AR_MPI_SYS_SetMemConf ERR !\n");
            return AR_FAILURE;
        }
    }

    /*config memory for vo*/
    for (i = 0; i < VO_MAX_LAYER_NUM; i++)
    {
        for (j = 0; j < VO_MAX_CHN_NUM; j++)
        {
            stMppChn.enModId    = AR_ID_VO;
            stMppChn.s32DevId = i;
            stMppChn.s32ChnId = j;
            s32Ret = AR_MPI_SYS_SetMemConfig(&stMppChn, pcMmzName);

            if (s32Ret)
            {
                SAMPLE_PRT("AR_MPI_SYS_SetMemConfig ERR !\n");
                return AR_FAILURE;
            }
        }
    }

    /*config memory for vdec */
    for (i = 0; i < VDEC_MAX_CHN_NUM; i++)
    {

        stMppChn.enModId  = AR_ID_VDEC;
        stMppChn.s32DevId = 0;
        stMppChn.s32ChnId = i;
        s32Ret = AR_MPI_SYS_SetMemConfig(&stMppChn, pcMmzName);

        if (s32Ret)
        {
            SAMPLE_PRT("AR_MPI_SYS_SetMemConf ERR !\n");
            return AR_FAILURE;
        }
    }


    return s32Ret;
}

/******************************************************************************
* function : vb init & MPI system init
******************************************************************************/
AR_S32 SAMPLE_COMM_SYS_Init(VB_CONFIG_S* pstVbConfig)
{
    AR_S32 s32Ret = AR_FAILURE;

    AR_MPI_SYS_Exit();
    AR_MPI_VB_Exit();

    if (NULL == pstVbConfig)
    {
        SAMPLE_PRT("input parameter is null, it is invaild!\n");
        return AR_FAILURE;
    }

    s32Ret = AR_MPI_VB_SetConfig(pstVbConfig);

    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("AR_MPI_VB_SetConf failed!\n");
        return AR_FAILURE;
    }

    s32Ret = AR_MPI_VB_Init();

    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("AR_MPI_VB_Init failed!\n");
        return AR_FAILURE;
    }

    s32Ret = AR_MPI_SYS_Init();

    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("AR_MPI_SYS_Init failed!\n");
        AR_MPI_VB_Exit();
        return AR_FAILURE;
    }

    return AR_SUCCESS;
}

/******************************************************************************
* function : vb init with VbSupplement & MPI system init
******************************************************************************/
AR_S32 SAMPLE_COMM_SYS_InitWithVbSupplement(VB_CONFIG_S* pstVbConf, AR_U32 u32SupplementConfig)
{
    VB_SUPPLEMENT_CONFIG_S stSupplementConf = {0};
    AR_S32 s32Ret = AR_FAILURE;

    AR_MPI_SYS_Exit();
    AR_MPI_VB_Exit();

    if (NULL == pstVbConf)
    {
        SAMPLE_PRT("input parameter is null, it is invaild!\n");
        return AR_FAILURE;
    }

    s32Ret = AR_MPI_VB_SetConfig(pstVbConf);

    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("AR_MPI_VB_SetConf failed!\n");
        return AR_FAILURE;
    }

    stSupplementConf.u32SupplementConfig = u32SupplementConfig;

    s32Ret = AR_MPI_VB_SetSupplementConfig(&stSupplementConf);

    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("AR_MPI_VB_SetSupplementConf failed!\n");
        return AR_FAILURE;
    }

    s32Ret = AR_MPI_VB_Init();

    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("AR_MPI_VB_Init failed!\n");
        return AR_FAILURE;
    }

    s32Ret = AR_MPI_SYS_Init();

    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("AR_MPI_SYS_Init failed!\n");
        return AR_FAILURE;
    }

    return AR_SUCCESS;
}


/******************************************************************************
* function : vb exit & MPI system exit
******************************************************************************/
AR_VOID SAMPLE_COMM_SYS_Exit(void)
{
    AR_MPI_SYS_Exit();
    AR_MPI_VB_ExitModCommPool(VB_UID_VDEC);
    AR_MPI_VB_Exit();
    return;
}
AR_S32 SAMPLE_COMM_VI_Bind_VO(VI_PIPE ViPipe, VI_CHN ViChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = AR_ID_VI;
    stSrcChn.s32DevId  = ViPipe;
    stSrcChn.s32ChnId  = ViChn;

    stDestChn.enModId  = AR_ID_VO;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    CHECK_RET(AR_MPI_SYS_Bind(&stSrcChn, &stDestChn), "AR_MPI_SYS_Bind(VI-VO)");

    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_VI_UnBind_VO(VI_PIPE ViPipe, VI_CHN ViChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = AR_ID_VI;
    stSrcChn.s32DevId  = ViPipe;
    stSrcChn.s32ChnId  = ViChn;

    stDestChn.enModId  = AR_ID_VO;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    CHECK_RET(AR_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "AR_MPI_SYS_UnBind(VI-VO)");

    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_VI_Bind_VPSS(VI_PIPE ViPipe, VI_CHN ViChn, VPSS_GRP VpssGrp)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = AR_ID_VI;
    stSrcChn.s32DevId  = ViPipe;
    stSrcChn.s32ChnId  = ViChn;

    stDestChn.enModId  = AR_ID_VPSS;
    stDestChn.s32DevId = VpssGrp;
    stDestChn.s32ChnId = 0;

    CHECK_RET(AR_MPI_SYS_Bind(&stSrcChn, &stDestChn), "AR_MPI_SYS_Bind(VI-VPSS)");

    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_VI_UnBind_VPSS(VI_PIPE ViPipe, VI_CHN ViChn, VPSS_GRP VpssGrp)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = AR_ID_VI;
    stSrcChn.s32DevId  = ViPipe;
    stSrcChn.s32ChnId  = ViChn;

    stDestChn.enModId  = AR_ID_VPSS;
    stDestChn.s32DevId = VpssGrp;
    stDestChn.s32ChnId = 0;

    CHECK_RET(AR_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "AR_MPI_SYS_UnBind(VI-VPSS)");

    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_VI_Bind_VENC(VI_PIPE ViPipe, VI_CHN ViChn, VENC_CHN VencChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = AR_ID_VI;
    stSrcChn.s32DevId  = ViPipe;
    stSrcChn.s32ChnId  = ViChn;

    stDestChn.enModId  = AR_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VencChn;

    CHECK_RET(AR_MPI_SYS_Bind(&stSrcChn, &stDestChn), "AR_MPI_SYS_Bind(VI-VENC)");

    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_VI_UnBind_VENC(VI_PIPE ViPipe, VI_CHN ViChn, VENC_CHN VencChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = AR_ID_VI;
    stSrcChn.s32DevId  = ViPipe;
    stSrcChn.s32ChnId  = ViChn;

    stDestChn.enModId  = AR_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VencChn;

    CHECK_RET(AR_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "AR_MPI_SYS_UnBind(VI-VENC)");

    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_VPSS_Bind_AVS(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, AVS_GRP AvsGrp, AVS_PIPE AvsPipe)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = AR_ID_VPSS;
    stSrcChn.s32DevId  = VpssGrp;
    stSrcChn.s32ChnId  = VpssChn;

    stDestChn.enModId  = AR_ID_AVS;
    stDestChn.s32DevId = AvsGrp;
    stDestChn.s32ChnId = AvsPipe;

    CHECK_RET(AR_MPI_SYS_Bind(&stSrcChn, &stDestChn), "AR_MPI_SYS_Bind(VPSS-AVS)");

    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_VPSS_UnBind_AVS(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, AVS_GRP AvsGrp, AVS_PIPE AvsPipe)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = AR_ID_VPSS;
    stSrcChn.s32DevId  = VpssGrp;
    stSrcChn.s32ChnId  = VpssChn;

    stDestChn.enModId  = AR_ID_AVS;
    stDestChn.s32DevId = AvsGrp;
    stDestChn.s32ChnId = AvsPipe;

    CHECK_RET(AR_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "AR_MPI_SYS_UnBind(VPSS-AVS)");

    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_VPSS_Bind_VO(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = AR_ID_VPSS;
    stSrcChn.s32DevId  = VpssGrp;
    stSrcChn.s32ChnId  = VpssChn;

    stDestChn.enModId  = AR_ID_VO;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    CHECK_RET(AR_MPI_SYS_Bind(&stSrcChn, &stDestChn), "AR_MPI_SYS_Bind(VPSS-VO)");

    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_VDEC_Bind_VO(VDEC_CHN VdecChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = AR_ID_VDEC;
    stSrcChn.s32DevId  = 0;
    stSrcChn.s32ChnId  = VdecChn;

    stDestChn.enModId  = AR_ID_VO;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    CHECK_RET(AR_MPI_SYS_Bind(&stSrcChn, &stDestChn), "AR_MPI_SYS_Bind(VDEC-VO)");

    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_VDEC_UnBind_VO(VDEC_CHN VdecChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = AR_ID_VDEC;
    stSrcChn.s32DevId  = 0;
    stSrcChn.s32ChnId  = VdecChn;

    stDestChn.enModId  = AR_ID_VO;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    CHECK_RET(AR_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "AR_MPI_SYS_UnBind(VDEC-VO)");

    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_VPSS_UnBind_VO(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = AR_ID_VPSS;
    stSrcChn.s32DevId  = VpssGrp;
    stSrcChn.s32ChnId  = VpssChn;

    stDestChn.enModId  = AR_ID_VO;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    CHECK_RET(AR_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "AR_MPI_SYS_UnBind(VPSS-VO)");

    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_VPSS_Bind_VENC(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VENC_CHN VencChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = AR_ID_VPSS;
    stSrcChn.s32DevId  = VpssGrp;
    stSrcChn.s32ChnId  = VpssChn;

    stDestChn.enModId  = AR_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VencChn;

    CHECK_RET(AR_MPI_SYS_Bind(&stSrcChn, &stDestChn), "AR_MPI_SYS_Bind(VPSS-VENC)");

    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_VPSS_UnBind_VENC(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VENC_CHN VencChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = AR_ID_VPSS;
    stSrcChn.s32DevId  = VpssGrp;
    stSrcChn.s32ChnId  = VpssChn;

    stDestChn.enModId  = AR_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VencChn;

    CHECK_RET(AR_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "AR_MPI_SYS_UnBind(VPSS-VENC)");

    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_AVS_Bind_AVS(AVS_GRP AvsSrcGrp, AVS_CHN AvsSrcChn, AVS_GRP AvsDestGrp, AVS_CHN AvsDestChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = AR_ID_AVS;
    stSrcChn.s32DevId  = AvsSrcGrp;
    stSrcChn.s32ChnId  = AvsSrcChn;

    stDestChn.enModId  = AR_ID_AVS;
    stDestChn.s32DevId = AvsDestGrp;
    stDestChn.s32ChnId = AvsDestChn;

    CHECK_RET(AR_MPI_SYS_Bind(&stSrcChn, &stDestChn), "AR_MPI_SYS_Bind(AVS-AVS)");

    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_AVS_UnBind_AVS(AVS_GRP AvsSrcGrp, AVS_CHN AvsSrcChn, AVS_GRP AvsDestGrp, AVS_CHN AvsDestChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = AR_ID_AVS;
    stSrcChn.s32DevId  = AvsSrcGrp;
    stSrcChn.s32ChnId  = AvsSrcChn;

    stDestChn.enModId  = AR_ID_AVS;
    stDestChn.s32DevId = AvsDestGrp;
    stDestChn.s32ChnId = AvsDestChn;

    CHECK_RET(AR_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "AR_MPI_SYS_UnBind(AVS-AVS)");

    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_AVS_Bind_VPSS(AVS_GRP AvsGrp, AVS_CHN AvsChn, VPSS_GRP VpssGrp)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = AR_ID_AVS;
    stSrcChn.s32DevId  = AvsGrp;
    stSrcChn.s32ChnId  = AvsChn;

    stDestChn.enModId  = AR_ID_VPSS;
    stDestChn.s32DevId = VpssGrp;
    stDestChn.s32ChnId = 0;

    CHECK_RET(AR_MPI_SYS_Bind(&stSrcChn, &stDestChn), "AR_MPI_SYS_Bind(AVS-VPSS)");

    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_AVS_UnBind_VPSS(AVS_GRP AvsGrp, AVS_CHN AvsChn, VPSS_GRP VpssGrp)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = AR_ID_AVS;
    stSrcChn.s32DevId  = AvsGrp;
    stSrcChn.s32ChnId  = AvsChn;

    stDestChn.enModId  = AR_ID_VPSS;
    stDestChn.s32DevId = VpssGrp;
    stDestChn.s32ChnId = 0;

    CHECK_RET(AR_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "AR_MPI_SYS_UnBind(AVS-VPSS)");

    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_AVS_Bind_VENC(AVS_GRP AvsGrp, AVS_CHN AvsChn, VENC_CHN VencChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = AR_ID_AVS;
    stSrcChn.s32DevId  = AvsGrp;
    stSrcChn.s32ChnId  = AvsChn;

    stDestChn.enModId  = AR_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VencChn;

    CHECK_RET(AR_MPI_SYS_Bind(&stSrcChn, &stDestChn), "AR_MPI_SYS_Bind(AVS-VENC)");

    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_AVS_UnBind_VENC(AVS_GRP AvsGrp, AVS_CHN AvsChn, VENC_CHN VencChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = AR_ID_AVS;
    stSrcChn.s32DevId  = AvsGrp;
    stSrcChn.s32ChnId  = AvsChn;

    stDestChn.enModId  = AR_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VencChn;

    CHECK_RET(AR_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "AR_MPI_SYS_UnBind(AVS-VENC)");

    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_AVS_Bind_VO(AVS_GRP AvsGrp, AVS_CHN AvsChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = AR_ID_AVS;
    stSrcChn.s32DevId  = AvsGrp;
    stSrcChn.s32ChnId  = AvsChn;

    stDestChn.enModId  = AR_ID_VO;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    CHECK_RET(AR_MPI_SYS_Bind(&stSrcChn, &stDestChn), "AR_MPI_SYS_Bind(AVS-VO)");

    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_AVS_UnBind_VO(AVS_GRP AvsGrp, AVS_CHN AvsChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = AR_ID_AVS;
    stSrcChn.s32DevId  = AvsGrp;
    stSrcChn.s32ChnId  = AvsChn;

    stDestChn.enModId  = AR_ID_VO;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    CHECK_RET(AR_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "AR_MPI_SYS_UnBind(AVS-VO)");

    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_VDEC_Bind_VPSS(VDEC_CHN VdecChn, VPSS_GRP VpssGrp)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = AR_ID_VDEC;
    stSrcChn.s32DevId  = 0;
    stSrcChn.s32ChnId  = VdecChn;

    stDestChn.enModId  = AR_ID_VPSS;
    stDestChn.s32DevId = VpssGrp;
    stDestChn.s32ChnId = 0;

    CHECK_RET(AR_MPI_SYS_Bind(&stSrcChn, &stDestChn), "AR_MPI_SYS_Bind(VDEC-VPSS)");

    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_VDEC_UnBind_VPSS(VDEC_CHN VdecChn, VPSS_GRP VpssGrp)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId  = AR_ID_VDEC;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = VdecChn;

    stDestChn.enModId  = AR_ID_VPSS;
    stDestChn.s32DevId = VpssGrp;
    stDestChn.s32ChnId = 0;

    CHECK_RET(AR_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "AR_MPI_SYS_UnBind(VDEC-VPSS)");

    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_VO_Bind_VO(VO_LAYER  SrcVoLayer, VO_CHN SrcVoChn, VO_LAYER DstVoLayer, VO_CHN DstVoChn)
{
    MPP_CHN_S stSrcChn, stDestChn;
    stSrcChn.enModId    = AR_ID_VO;
    stSrcChn.s32DevId   = SrcVoLayer;
    stSrcChn.s32ChnId   = SrcVoChn;

    stDestChn.enModId   = AR_ID_VO;
    stDestChn.s32DevId  = DstVoLayer;
    stDestChn.s32ChnId  = DstVoChn;

    return AR_MPI_SYS_Bind(&stSrcChn, &stDestChn);
}

AR_S32 SAMPLE_COMM_VO_UnBind_VO(VO_LAYER DstVoLayer, VO_CHN DstVoChn)
{
    MPP_CHN_S stDestChn;
    stDestChn.enModId   = AR_ID_VO;
    stDestChn.s32DevId  = DstVoLayer;
    stDestChn.s32ChnId  = DstVoChn;

    return AR_MPI_SYS_UnBind(NULL, &stDestChn);
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
