#include <stdio.h>

#include "sample_vo_config.h"
#include "binder_ipc.h"

typedef struct {
    ar_ipc_state_t* ipc;
    uint32_t handle;
} ipc_client_context_t;


static int AR_SAMPLE_VO_client_ops(ipc_client_context_t* context, SAMPLE_VO_CONFIG_CMD_E cmd, void* buffer, int len, void* out)
{
    assert(context);
    assert(cmd > CMD_VO_CONFIG_NONE && cmd < CMD_VO_CONFIG_BUTT);
    assert(buffer);
    assert(len);

    int ret = ar_ipc_send_buffer(context->ipc, context->handle, cmd, buffer, len, out, 0);

    return ret;
}

static int AR_SAMPLE_VO_create_ipc_client(ipc_client_context_t* context)
{
    context->ipc = ar_ipc_create();
    if (!context->ipc) {
        printf("failed to create ipc client");
        return -1;
    }
    printf("ar ipc create client\n");

    context->handle = ar_ipc_get_service(context->ipc, "vo_control");
    if (!context->handle) {
        printf("failed to get vo_control service");
        return -1;
    }
    printf("ar ipc get vo_control service\n");
    return 0;
}

static AR_VOID usage(const char* argv0)
{
    printf("Usage: \n");
    printf("%s -e 0 \n", argv0);
    printf("Options:\n");
    printf("   -e  <int>\n");
    printf("       vo switch, 0: off , 1: on\n");
    return 0;
}

int main(int argc, char *argv[])
{
    if(argc < 2) {
        usage(argv[0]);
    }

    AR_S32 s32Ret = AR_FAILURE;
    AR_BOOL bSwitch = AR_FALSE;
    SAMPLE_VO_CONTROL_S stParam = {0};

    int opt;
    while((opt=getopt(argc, argv, "e:")) != -1)
    {
        switch(opt) {
            case 'e' :
                bSwitch = AR_TRUE;
                stParam.bVoEnable = atoi(optarg) ? AR_TRUE : AR_FALSE;
                break;
            default:
                usage(argv[0]);
                exit(1);
                break;
        }
    }

    ipc_client_context_t context;
    assert(!AR_SAMPLE_VO_create_ipc_client(&context));

    SAMPLE_VO_REPLY_S replyRet = {"unknown", -1};

    if(bSwitch){
        s32Ret = AR_SAMPLE_VO_client_ops(&context, CMD_VO_CONFIG_SWITCH, &stParam, sizeof(SAMPLE_VO_CONTROL_S), &replyRet);
        if(s32Ret == AR_SUCCESS && replyRet.s32Ret == AR_SUCCESS){
            printf("success\n");
        }else{
            printf("failed!\n");
        }
    }

    ar_ipc_close_service(context.ipc, context.handle);
    ar_ipc_close(context.ipc);

    return 0;
}