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
#include "sample_ir.h"

static AR_VOID usage(char* name)
{
    printf("Usage:\n");
    printf("%s -o <out mode 0:hdmi 1:rtsp 2:hdmi&rtsp> \n", name);
    return;
}


/******************************************************************************
* function    : main()
* Description : main
******************************************************************************/
int main(int argc, char *argv[])
{
    AR_S32 s32Ret = AR_FAILURE;
    AR_CHAR achSensor[64] = "gst417w";
    AR_S32 s32Mode = 0;
    AR_FLOAT fDispScalerRatio = 1.0;
    IR_OFFLINE_CTX_S stIrOfflineCtx = {0};
    AR_S32 s32FusionEnable = 0;
    int argIndex = 0;

    stIrOfflineCtx.s32Circle = 1;
    stIrOfflineCtx.bImgNoDummy = AR_FALSE;

    signal(SIGINT, SAMPLE_VIO_HandleSig);
    signal(SIGTERM, SAMPLE_VIO_HandleSig);

    SAMPLE_VIO_MsgInit();

#if 0
    int opt;
    while ((opt=getopt(argc, argv, "o:b:")) != -1)
    {
        switch (opt) {
            case 'o':
                s32OutMode = atoi(optarg);
                printf("output mode: %d\n", s32OutMode);
                break;
            default:
                usage(argv[0]);
                exit(1);
        }
    }
#endif

    if (argc >= 2) {
        strcpy(achSensor, argv[1]);
    }

    if (argc >= 3) {
        s32Mode = atoi(argv[2]);
    }

    if (argc >= 4) {
        fDispScalerRatio = atof(argv[3]);
    }

    if(argc >= 5) {
        s32FusionEnable = atoi(argv[4]);
    }

    argIndex = 6; //offline
    if (argc >= argIndex) {
        stIrOfflineCtx.u32Width = atoi(argv[argIndex-1]);
        argIndex++;
    }

    if (argc >= argIndex) {
        stIrOfflineCtx.u32Height = atoi(argv[argIndex-1]);
        argIndex++;
    }

    if (argc >= argIndex) {
        stIrOfflineCtx.input_fname = argv[argIndex-1];
        argIndex++;
    }

    if (argc >= argIndex) {
        stIrOfflineCtx.thermal_output_fname = argv[argIndex-1];
        argIndex++;
    }

    if (argc >= argIndex) {
        stIrOfflineCtx.nr3d_output_fname = argv[argIndex-1];
        argIndex++;
    }

    if (argc >= argIndex) {
        stIrOfflineCtx.nr2d_output_fname = argv[argIndex-1];
        argIndex++;
    }

    if (argc >= argIndex) {
        stIrOfflineCtx.gtm_output_fname = argv[argIndex-1];
        argIndex++;
    }

    if (argc >= argIndex) {
        stIrOfflineCtx.isp1_output_fname = argv[argIndex-1];
        argIndex++;
    }

    if (argc >= argIndex) {
        stIrOfflineCtx.isp2_output_fname = argv[argIndex-1];
        argIndex++;
    }

    if (argc >= argIndex) {
        stIrOfflineCtx.s32Circle = atoi(argv[argIndex-1]);
        argIndex++;
    }

    if (argc >= argIndex) {
        if (strcmp(argv[argIndex-1], "-no_dummy") == 0) {
            stIrOfflineCtx.bImgNoDummy = AR_TRUE;
        }
        argIndex++;
    }

    s32Ret = SAMPLE_IR(achSensor, s32Mode, fDispScalerRatio, s32FusionEnable, &stIrOfflineCtx);

    if (AR_SUCCESS == s32Ret)
    {
        SAMPLE_PRT("sample_vio exit success!\n");
    }
    else
    {
        SAMPLE_PRT("sample_vio exit abnormally!\n");
    }

    SAMPLE_VIO_MsgExit();
	SAMPLE_AR_MPI_VIN_CloseDev();
    return s32Ret;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
