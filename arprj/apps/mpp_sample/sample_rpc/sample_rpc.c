#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sample_comm.h"


int test_rpc_init_deinit(AR_U32 u32Round)
{
    AR_S32 s32Ret = 0;
    FILE *fp = NULL;
    VB_CONFIG_S stVbConf = {0};

    //s32Ret = AR_MPI_SYS_Exit();
    //CHECK_RET(s32Ret,"");


    s32Ret = AR_MPI_VB_SetConfig(&stVbConf);
    s32Ret = AR_MPI_VB_Init();
    CHECK_RET(s32Ret,"");
    printf("%s VB inited\n", __func__);

    s32Ret = AR_MPI_SYS_Init();
    CHECK_RET(s32Ret,"");
    printf("%s MPI sys inited\n", __func__);

    s32Ret = AR_MPI_SYS_Exit();
    printf("%s MPI sys exit, ret=%d\n", __func__, s32Ret);

    s32Ret = AR_MPI_VB_Exit();
    CHECK_RET(s32Ret,"");
    printf("%s VB exit\n", __func__);

    char * pTest = (char *)malloc(4096);
    if (!pTest) {
        printf("malloc failed!\n");
        return -1;
    }
    free(pTest);

    fp = fopen("/tmp/test", "a");
    if (!fp) return -1;

    if (fprintf(fp, "%s", "hello") < 0) {
        printf("Failed to write file\n");
        s32Ret = -1;
    }

    fclose(fp);
    printf("write file ok\n");

    return s32Ret;
}

int main(int argc, char *argv[])
{
    AR_S32 s32Ret = 0;
    AR_U32 u32Round = 600;

    if (argc > 1) {
        u32Round = atoi(argv[1]);
    }

    printf("========Start to run the case for %u times========\n", u32Round);
    for (int i=0; i<u32Round; i++) {
        printf("--------TestRound %d--------\n", i);
        s32Ret = test_rpc_init_deinit(i);

        if (s32Ret != 0) {
            printf("test_rpc_init_deinit failed!\n");
            return s32Ret;
		}
    }

    return 0;
}
