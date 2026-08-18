
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

#include "sample_comm.h"


/*****************************************************************************
* function : start vpss grp.
*****************************************************************************/
AR_S32 SAMPLE_COMM_VPSS_Start(VPSS_GRP VpssGrp, AR_BOOL* pabChnEnable, VPSS_GRP_ATTR_S* pstVpssGrpAttr, VPSS_CHN_ATTR_S* pastVpssChnAttr)
{
    VPSS_CHN VpssChn;
    AR_S32 s32Ret;
    AR_S32 j;

    s32Ret = AR_MPI_VPSS_CreateGrp(VpssGrp, pstVpssGrpAttr);

    if (s32Ret != AR_SUCCESS)
    {
        SAMPLE_PRT("AR_MPI_VPSS_CreateGrp(grp:%d) failed with %#x!\n", VpssGrp, s32Ret);
        return AR_FAILURE;
    }

    s32Ret = AR_MPI_VPSS_StartGrp(VpssGrp);

    if (s32Ret != AR_SUCCESS)
    {
        SAMPLE_PRT("AR_MPI_VPSS_StartGrp failed with %#x\n", s32Ret);
        return AR_FAILURE;
    }

    for (j = 0; j < VPSS_MAX_PHY_CHN_NUM; j++)
    {
        if(AR_TRUE == pabChnEnable[j])
        {
            VpssChn = j;
            s32Ret = AR_MPI_VPSS_SetChnAttr(VpssGrp, VpssChn, &pastVpssChnAttr[VpssChn]);

            if (s32Ret != AR_SUCCESS)
            {
                SAMPLE_PRT("AR_MPI_VPSS_SetChnAttr failed with %#x\n", s32Ret);
                return AR_FAILURE;
            }

            AR_U32 u32Align = 64;
            s32Ret = AR_MPI_VPSS_SetChnAlign(VpssGrp, VpssChn, u32Align);
            if (s32Ret != AR_SUCCESS)
            {
                SAMPLE_PRT("AR_MPI_VPSS_SetChnAlign failed with %#x\n", s32Ret);
                return AR_FAILURE;
            }

            s32Ret = AR_MPI_VPSS_EnableChn(VpssGrp, VpssChn);

            if (s32Ret != AR_SUCCESS)
            {
                SAMPLE_PRT("AR_MPI_VPSS_EnableChn failed with %#x\n", s32Ret);
                return AR_FAILURE;
            }
        }
    }

    return AR_SUCCESS;
}

/*****************************************************************************
* function : stop vpss grp
*****************************************************************************/
AR_S32 SAMPLE_COMM_VPSS_Stop(VPSS_GRP VpssGrp, AR_BOOL* pabChnEnable)
{
    AR_S32 j;
    AR_S32 s32Ret = AR_SUCCESS;
    VPSS_CHN VpssChn;

    for (j = 0; j < VPSS_MAX_PHY_CHN_NUM; j++)
    {
        if(AR_TRUE == pabChnEnable[j])
        {
            VpssChn = j;
            s32Ret = AR_MPI_VPSS_DisableChn(VpssGrp, VpssChn);

            if (s32Ret != AR_SUCCESS)
            {
                SAMPLE_PRT("failed with %#x!\n", s32Ret);
                return AR_FAILURE;
            }
        }
    }

    s32Ret = AR_MPI_VPSS_StopGrp(VpssGrp);

    if (s32Ret != AR_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return AR_FAILURE;
    }

    s32Ret = AR_MPI_VPSS_DestroyGrp(VpssGrp);

    if (s32Ret != AR_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return AR_FAILURE;
    }

    return AR_SUCCESS;
}



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
