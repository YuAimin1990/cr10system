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
#include "sample_vio.h"

static AR_VOID usage(char* name)
{
    printf("Usage:\n");
    printf("%s -o out_mode(0:hdmi 1:rtsp)\n", name);
    return;
}


/******************************************************************************
* function    : main()
* Description : main
******************************************************************************/
int main(int argc, char *argv[])
{
    AR_S32 s32Ret = AR_FAILURE;
    //AR_S32 s32Index;
    AR_S32 s32OutMode = 0; //0:hdmi 1:rtsp
    AR_S32 s32Bitrate = 160000;

    signal(SIGINT, SAMPLE_VIO_HandleSig);
    signal(SIGTERM, SAMPLE_VIO_HandleSig);

    SAMPLE_VIO_MsgInit();

    int opt;
    while ((opt=getopt(argc, argv, "o:b:")) != -1)
    {
        switch (opt) {
            case 'o':
                s32OutMode = atoi(optarg);
                printf("output mode: %d\n", s32OutMode);
                break;
            case 'b':
                s32Bitrate = atoi(optarg);
                printf("s32Bitrate: %d\n", s32Bitrate);
                if(s32Bitrate<128/*||s32Bitrate>160000*/)
                {
                    exit(1);
                }
                break;
            default:
                usage(argv[0]);
                exit(1);
        }
    }

    s32Ret = SAMPLE_plug617_test(s32OutMode);

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
