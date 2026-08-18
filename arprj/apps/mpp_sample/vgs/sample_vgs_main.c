#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "ar_common.h"
#include "sample_vgs.h"
#include "mpi_vgs.h"
#include "hal_vin_log.h"
#include "mpi_sysctl.h"
#include "mpi_vo.h"


MPI_SYSCTL_HANDLE g_phandle = NULL;
/******************************************************************************
* function : show usage
******************************************************************************/
void SAMPLE_VGS_Usage(char *sPrgNm)
{
    printf("Usage : %s [index] [filename]\n", sPrgNm);
    printf("index:\n");
    printf("\t 0)vi -> vgs ->vo(hdmi).\n");
    printf("\t 1)vb -> vgs(osd) -> vo(hdmi).\n");
    printf("\t 2)vb -> vgs(128*128 square) -> vo(hdmi).\n");
    printf("\t 3)vb -> vgs(osd rot90) -> vo(hdmi).\n");
    printf("\t 4)vb -> vgs(osd rot180) -> vo(hdmi).\n");
    printf("\t 5)vb -> vgs(osd scale) -> vo(hdmi).\n\n");

    printf("filename: osd file name.defalut is ./2.bmp\n");
    return;
}

#ifndef AR9341
void SAMPLE_fast_boot_func(SYSCTL_EVENT_E eSysctlEvent, void *pData, AR_S32 s32Len)
{
    int ret;
    int flag = -1;
    AR_U64 u64Tic = 0;
    struct timeval tm_start;
    struct timeval tm_end;

    if(SYSCTL_EVENT_FAST_SUSPEND == eSysctlEvent ||
        SYSCTL_EVENT_SUSPEND == eSysctlEvent)
    {
        /* suspend */
        gettimeofday(&tm_start, NULL);

        printf("app suspend.\n");
        SAMPLE_VGS_Suspend();
        printf("app suspend finished.\n");
        ret = SAMPLE_VGS_Vo_DeInit();
        if(ret)
            printf("destory vo failed, ret:%d!\n", ret);
        else
            printf("destory vo success!\n");

        ret = AR_MPI_VGS_Suspend();
        if(ret)
            printf("vgs suspend failed, ret:%d!\n", ret);
        else
            printf("vgs suspend success!\n");

        ret = AR_MPI_VO_Suspend(0);
        if(ret)
            printf("suspend failed, ret:%d!\n", ret);
        else
            printf("suspend success!\n");

        gettimeofday(&tm_end, NULL);
        flag = 1;
    }
    else if(SYSCTL_EVENT_FAST_RESUME == eSysctlEvent ||
        SYSCTL_EVENT_RESUME == eSysctlEvent)
    {
        gettimeofday(&tm_start, NULL);
        ret = AR_MPI_VO_Resume(0);
        if(ret)
            printf("resume failed, ret:%d!\n", ret);
        else
            printf("resume success!\n");

        ret = AR_MPI_VGS_Resume();
        if(ret)
            printf("vgs resume failed, ret:%d!\n", ret);
        else
            printf("vgs resume success!\n");

        ret = SAMPLE_VGS_Vo_Init();
        if(ret)
            printf("create vo failed, ret:%d!\n", ret);
        else
            printf("create vo success!\n");

        SAMPLE_VGS_Rusume();
        printf("app resume!\n");

        gettimeofday(&tm_end, NULL);
        flag = 0;
    }
    else
    {
        printf("fault sysctl event:%d\n", eSysctlEvent);
        return;
    }

    ret = AR_MPI_SYSCTL_Event_done(g_phandle, eSysctlEvent);
    if(ret)
        printf("event SYSCTL_Event_done fail!\n");
    else
        printf("event SYSCTL_Event_done!\n");

    if(-1 != flag)
    {
        u64Tic = tm_end.tv_sec * 1000000 + tm_end.tv_usec - tm_start.tv_sec * 1000000 - tm_start.tv_usec;
        printf(" %s takes %f ms\n", flag==1 ? "SUSPEND" : "RESUME", u64Tic*1.0/1000);
    }

}
#endif

/******************************************************************************
* function    : main()
* Description : main
******************************************************************************/
int main(int argc, char *argv[])
{
    AR_S32 s32Ret = AR_FAILURE;
    AR_S32 s32Index = 0;
    char  *stFileName = NULL;

    ar_hal_log_init();
  	ar_log_shm_set_enabled(0);

    if (argc > 3 ){
        SAMPLE_VGS_Usage(argv[0]);
        return AR_FAILURE;
    }

    signal(SIGINT, SAMPLE_VGS_HandleSig);
    signal(SIGTERM, SAMPLE_VGS_HandleSig);

#ifndef AR9341
     /*** register in app to sysctl ***/
     g_phandle = AR_MPI_SYSCTL_Register(argv[0], 0, &SAMPLE_fast_boot_func);
     if(!g_phandle){
         printf("register sysctl %s failed!\n", argv[0]);
         return -1;
     }
     printf("register sysctl %s success!\n", argv[0]);
#endif

     if (argc == 1)
         s32Index = 0;
     else
     {
         s32Index = atoi(argv[1]);
         if (!strcmp(argv[1], "--help"))
         {
             SAMPLE_VGS_Usage(argv[0]);
             return AR_SUCCESS;
         }
     }

     if (argc == 3)
         stFileName = argv[2];
     else
         stFileName = "/mnt/2.bmp";

     switch (s32Index)
     {
         case 0:
             s32Ret = SAMPLE_VGS_Start_With_VI(stFileName);
             break;
         case 1:
             s32Ret = SAMPLE_VGS_Start(stFileName);
             break;

         default:
             s32Ret = SAMPLE_VGS_Start_Draw_Test(s32Index-2, stFileName);
             break;
     }

    if (AR_SUCCESS == s32Ret)
    {
        SAMPLE_PRT("sample_vio exit success!\n");
    }
    else
    {
        SAMPLE_PRT("sample_vio exit abnormally!\n");
    }

    return s32Ret;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
