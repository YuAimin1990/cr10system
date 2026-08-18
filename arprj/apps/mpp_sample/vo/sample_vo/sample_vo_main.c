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
#include "sample_vo.h"
#include "mpi_sys.h"
#include "binder_ipc.h"
#include "sample_vo_config.h"
#include "mpi_sysctl.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

MPI_SYSCTL_HANDLE g_phandle = NULL;
int s32Index = 0;

static int AR_SAMPLE_VO_ipc_callback(uint32_t cmd, ar_ipc_data_t* data, unsigned len, void* reply)
{
    AR_S32 s32Ret = AR_FAILURE;
    if(data == AR_NULL){
        printf("AR_SAMPLE_VPSS_ipc_callback data is NULL\n");
        return -1;
    }

    SAMPLE_VO_CONTROL_S *ctrlParam = (SAMPLE_VO_CONTROL_S*)data->blob;
    printf("cmd = %d\n", cmd);
    switch(cmd) {
        case CMD_VO_CONFIG_SWITCH:
            s32Ret = SAMPLE_VO_Handel_Control_Cmd(cmd, ctrlParam);
            break;
        default:
            printf("unkonwn cmd\n");
            break;
    }

    SAMPLE_VO_REPLY_S replyRet;
    replyRet.s32Ret = s32Ret;
    if(s32Ret == AR_SUCCESS){
        strcpy(replyRet.result, "SUCCESS");
    } else {
        strcpy(replyRet.result, "FAIL");
    }
    ar_ipc_return_buffer(reply, s32Ret, &replyRet, sizeof(SAMPLE_VO_REPLY_S));
    return s32Ret;
}


static ar_ipc_state_t* AR_SAMPLE_create_ipc_server()
{
    int ret;
    ar_ipc_state_t* ipc = ar_ipc_create();
    if (!ipc) {
        SAMPLE_PRT("failed to create server ipc");
        return NULL;
    }

    ret = ar_ipc_add_service(ipc, "vo_control", AR_SAMPLE_VO_ipc_callback);
    if (ret) {
    	ar_ipc_close(ipc);
        SAMPLE_PRT("failed to publish service vpss_control\n");
        return NULL;
    }

    ar_ipc_start_thread_pool(ipc);

    return ipc;
}

void AR_SAMPLE_fast_boot_func(SYSCTL_EVENT_E eSysctlEvent, void *pData, AR_S32 s32Len)
{
    int ret;
    int flag = -1;
    AR_U64 u64Tic = 0;
    struct timeval tm_start;
    struct timeval tm_end;
    gettimeofday(&tm_start, NULL);

    if(SYSCTL_EVENT_SUSPEND == eSysctlEvent)
    {
        ret = SAMPLE_VO_HDMI_1080P60_DESTORY();
        if(ret)
            printf("destory failed, ret:%d!\n", ret);
        else
            printf("destory success!\n");

        ret = AR_MPI_VO_Suspend(0);
        if(ret)
            printf("suspend failed, ret:%d!\n", ret);
        else
            printf("suspend success!\n");

        ret = AR_MPI_RGN_Suspend();
        if(ret)
            printf("rgn suspend failed, ret:%d!\n", ret);
        else
            printf("rgn suspend success!\n");

        flag = 1;
    }
    else if(SYSCTL_EVENT_FAST_RESUME == eSysctlEvent ||
        SYSCTL_EVENT_RESUME == eSysctlEvent)
    {
        ret = AR_MPI_RGN_Resume();
        if(ret)
            printf("rgn resume failed, ret:%d!\n", ret);
        else
            printf("rgn resume success!\n");

        ret = AR_MPI_VO_Resume(0);
        if(ret)
            printf("resume failed, ret:%d!\n", ret);
        else
            printf("resume success!\n");

        ret = SAMPLE_VO_HDMI_1080P60_CREATE();
        if(ret)
            printf("create failed, ret:%d!\n", ret);
        else
            printf("create success!\n");

        flag = 0;
    }
    else
    {
        printf("fault sysctl event:%d\n", eSysctlEvent);
        return;
    }

    if(ret)
        return;

    ret = AR_MPI_SYSCTL_Event_done(g_phandle, eSysctlEvent);
    if(ret)
        printf("event SYSCTL_Event_done fail!\n");
    else
        printf("event SYSCTL_Event_done!\n");

    gettimeofday(&tm_end, NULL);
    if(-1 != flag)
    {
        u64Tic = tm_end.tv_sec * 1000000 + tm_end.tv_usec - tm_start.tv_sec * 1000000 - tm_start.tv_usec;
        printf(" %s takes %f ms\n", flag==1 ? "SUSPEND" : "RESUME", u64Tic*1.0/1000);
    }

}

/******************************************************************************
* function : show usage
******************************************************************************/
void SAMPLE_VO_Usage(char *sPrgNm)
{
    printf("Usage : %s <index><fast_boot>\n", sPrgNm);
    printf("index:\n");
    printf("\t 0)simple   :      FILE -> VO ->HDMI_1080P60(it66121).\n");
    printf("\t 1)interlace:      FILE -> VO ->HDMI_1080I60(it66121).\n");
    printf("\t 2)mipi tx:        FILE -> VO ->MIPI_LCD_1024*768.\n");
    printf("\t 3)muti-chan:      FILE -> VO ->HDMI_1080P60(it66121).\n");
    printf("\t 4)cover region:   FILE -> VO ->HDMI_1080P60(it66121).\n");
    printf("\t 5)mosaic region:  FILE -> VO ->HDMI_1080P60(it66121).\n");
    printf("\t 6)muti device:    FILE -> VO ->MIPI_LCD_1024*768 & lcd_24bit.\n");
    printf("\t 7)monochrome test:FILE -> VO ->HDMI_1080P60(it66121).\n");
    printf("\t 8)tp2803 out:FILE -> VO ->dvp_1080P30(tp2803).\n");
    printf("\t 9)pip :          FILE -> VO -> HDMI_1080P60(it66121).\n");
    printf("ddr_retention support index:\n");
    printf("\t 0)simple   :      FILE -> VO ->HDMI_1080P60(it66121).\n");
    printf("fast_boot:\n");
    printf("\t 0)not register app for fast boot, default.\n");
    printf("\t 1)register app for fast boot, then sysctl module can control suspend or resume.\n");
    printf("\t If you have any questions, please look at readme.txt!\n");
    return;
}

/******************************************************************************
* function    : main()
* Description : main
******************************************************************************/
int main(int argc, char *argv[])
{
    AR_S32 s32Ret = AR_FAILURE;

    AR_S32 s32DddrRetention = 0;
    if (argc < 2 || argc > 3) {
        SAMPLE_VO_Usage(argv[0]);
        return AR_FAILURE;
    }

    if (!strncmp(argv[1], "-h", 2)) {
        SAMPLE_VO_Usage(argv[0]);
        return AR_SUCCESS;
    }

    signal(SIGINT, SAMPLE_VO_HandleSig);
    signal(SIGTERM, SAMPLE_VO_HandleSig);

    ar_ipc_state_t* ipc = AR_SAMPLE_create_ipc_server();

    /*** register in app to sysctl ***/
    if(3 == argc){
        if(argv[2]){
            g_phandle = AR_MPI_SYSCTL_Register(argv[0], 0, &AR_SAMPLE_fast_boot_func);
            if(!g_phandle){
                printf("register sysctl %s failed!\n", argv[0]);
                return -1;
            }
            printf("register sysctl %s success!\n", argv[0]);
            s32DddrRetention = 1;
        }
    }

    s32Index = atoi(argv[1]);

    if(s32DddrRetention)
    {
        printf("s32DddrRetention!\n");
        switch (s32Index) {
            case 0:
                SAMPLE_VO_HDMI_1080P60_DDR_RETENTION();
                break;
            case 11:
                s32Ret = SAMPLE_VO_MIPILCD_720_1440(AR_FALSE);
                break;
            default:
                SAMPLE_PRT("ddr retention the index %d is invaild!\n",s32Index);
                SAMPLE_VO_Usage(argv[0]);
                s32Ret = AR_FAILURE;
        }
    }
    else
    {
        switch (s32Index) {
            case 0:
                s32Ret = SAMPLE_VO_HDMI_1080P60(RGN_BUTT, AR_FALSE);
                break;
            case 1:
                s32Ret = SAMPLE_VO_HDMI_1080I60();
                break;
            case 2:
                s32Ret = SAMPLE_VO_MIPILCD_1024_600(AR_FALSE);
                break;
            case 3:
                s32Ret = SAMPLE_VO_HDMI_1080P60_MUTI_CHAN();
                break;
            case 4:
                s32Ret = SAMPLE_VO_HDMI_1080P60(COVER_RGN, AR_FALSE);
                break;
            case 5:
                s32Ret = SAMPLE_VO_HDMI_1080P60(MOSAIC_RGN, AR_FALSE);
                break;
            case 6:
                s32Ret = SAMPLE_VO_MIPILCD_1024_600(AR_TRUE);
                break;
            case 7:
                s32Ret = SAMPLE_VO_HDMI_1080P60(RGN_BUTT, AR_TRUE);
                break;
            case 8:
                s32Ret = SAMPLE_VO_TEST_DVP_1080P_TP2803();
                break;
            case 9:
                s32Ret = SAMPLE_VO_HDMI_PIP();
                break;
            case 10:
                s32Ret = SAMPLE_VO_HDMI_1080P60_SUBSCRIBE(RGN_BUTT, AR_FALSE);
                break;
            case 11:
                s32Ret = SAMPLE_VO_MIPILCD_720_1440(AR_FALSE);
                break;
            case 12:
                s32Ret = SAMPLE_VO_MIPILCD_400x900();
                break;

#if 0
            case 8:
                s32Ret = SAMPLE_VO_TEST();
                break;
#endif

            default:
                SAMPLE_PRT("the index %d is invaild!\n",s32Index);
                SAMPLE_VO_Usage(argv[0]);
                s32Ret = AR_FAILURE;
        }
    }

    if (s32Ret == AR_SUCCESS) {
        SAMPLE_PRT("sample_vo exit success!\n");
    } else {
        SAMPLE_PRT("sample_vo exit abnormally!\n");
    }

    if(g_phandle){
        s32Ret = AR_MPI_SYSCTL_Unregister(g_phandle);
        if(s32Ret){
            printf("unregister sysctl %s failed!\n", argv[0]);
        }
    }

	if(ipc) {
	    ar_ipc_stop_thread_pool(ipc);
	    ar_ipc_close(ipc);
    }
    return s32Ret;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
