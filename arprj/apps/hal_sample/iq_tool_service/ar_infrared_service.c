#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "fifo.h"
#include "utils_debug.h"
#include "iq_tool_type.h"
#include "hal_venc.h"
#include "binder_ipc.h"
#include "binder_ipc_common.h"
#include "ar_infrared_service.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "infrared-service"

#define IPC_SERVICE_NAME                "inf_alg"

typedef enum {
    INF_ALG_IPC_CMD_GET_INFO,
    INF_ALG_IPC_CMD_GET_CONFIG,
    INF_ALG_IPC_CMD_SET_CONFIG,
}INF_ALG_IPC_CMD;

typedef struct arIPC_CONFIG_INFO_S
{
    int version;
    int data_size;
    int revert[4];
} IPC_CONFIG_INFO_S;


static infrared_server_t stInfraredServer = {NULL, -1, NULL};

static int send_infrared_ack_func(ENUM_IQ_REQ_ID eReqId, unsigned char *pAckPayload, int s32PayloadLength, int ack_ret)
{
    int /*i = 0,*/ s32AckHeaderLength = 0;
    infrared_ack_header_t *pAckHeader = NULL;
    unsigned char *pPayloadAddr = NULL;
    s32AckHeaderLength = sizeof(infrared_ack_header_t);
    pAckHeader = (infrared_ack_header_t *)malloc(s32AckHeaderLength + s32PayloadLength);
    pPayloadAddr = (unsigned char*)(pAckHeader) + s32AckHeaderLength;
    pAckHeader->request_id = eReqId;
    pAckHeader->ret_code = ack_ret;
    pAckHeader->len = s32PayloadLength;
    log_tag_info("ack cmd id 0x%x payload length %d.", *(int*)pAckHeader, s32PayloadLength);
    if (s32PayloadLength > 0)
        memcpy(pPayloadAddr, pAckPayload, s32PayloadLength);
    if (stInfraredServer.send_func && stInfraredServer.client)
        stInfraredServer.send_func(stInfraredServer.client, (unsigned char*)pAckHeader, (s32AckHeaderLength + s32PayloadLength), stInfraredServer.module_id, 0);
    else
        log_tag_err("send func or client is NULL.");
    free(pAckHeader);
    return 0;
}

static int init_ipc(ar_ipc_state_t** pipc, uint32_t *handle)
{
    // int ret = 0;
    ar_ipc_state_t* ipc = ar_ipc_create();
    if (!ipc) {
        log_tag_err("failed to create client ipc");
        return -1;
    }

    uint32_t config_handle;

    config_handle = ar_ipc_get_service(ipc, IPC_SERVICE_NAME);
    if (!config_handle) {
        ar_ipc_close(ipc);
        log_tag_err("failed to get service");
        return -1;
    }

    *pipc = ipc;
    *handle = config_handle;
    return 0;
}

static void release_ipc(ar_ipc_state_t** pipc, uint32_t *handle)
{
    if(*pipc==NULL)
        return;
    if(*handle) ar_ipc_close_service(*pipc, *handle);
    ar_ipc_close(*pipc);
}

int process_infrared_tuning_func(void *client, unsigned char *msg, unsigned int len)
{
    int ret = 0, s32ReqHeaderLen = sizeof(infrared_request_header_t), s32AckLen = 0;
    infrared_request_header_t *req = (infrared_request_header_t*)msg;
    unsigned char *pAck = NULL;
    IPC_CONFIG_INFO_S stInfo = {0};
    ar_ipc_state_t* ipc = NULL;
    uint32_t config_handle = 0;
    unsigned char* config_data = NULL;
    switch(req->cmd_id)
    {
        case INFRARED_REQ_CONNECT:
        {
            ret = init_ipc(&ipc,&config_handle);
            if(ret!=0)
            {
                break;
            }
            ret = ar_ipc_send_buffer(ipc, config_handle, INF_ALG_IPC_CMD_GET_INFO, &stInfo, sizeof(IPC_CONFIG_INFO_S), &stInfo, 0);
            if(ret){
                log_tag_err("ar_ipc_send_buffer failed, cmd=%d",INF_ALG_IPC_CMD_GET_INFO);
                break;
            }
            log_tag_info("config inf: version=%d data_size=%d",stInfo.version,stInfo.data_size);
            pAck = (unsigned char *)&stInfo;
            s32AckLen = sizeof(IPC_CONFIG_INFO_S);
        }
        break;
        case INFRARED_REQ_GET_SETTING:
        {
            ret = init_ipc(&ipc,&config_handle);
            if(ret!=0)
            {
                break;
            }
            ret = ar_ipc_send_buffer(ipc, config_handle, INF_ALG_IPC_CMD_GET_INFO, &stInfo, sizeof(IPC_CONFIG_INFO_S), &stInfo, 0);
            if(ret){
                log_tag_err("ar_ipc_send_buffer failed, cmd=%d",INF_ALG_IPC_CMD_GET_INFO);
                break;
            }
            log_tag_info("config inf: version=%d data_size=%d",stInfo.version,stInfo.data_size);
            config_data = (unsigned char*)malloc(stInfo.data_size);
            ret = ar_ipc_send_buffer(ipc, config_handle, INF_ALG_IPC_CMD_GET_CONFIG, config_data, stInfo.data_size, config_data, 0);
            if(ret){
                log_tag_err("ar_ipc_send_buffer failed, cmd=%d",INF_ALG_IPC_CMD_GET_CONFIG);
                break;
            }
            pAck = config_data;
            s32AckLen = stInfo.data_size;
        }
        break;
        case INFRARED_REQ_SET_SETTING:
        {
            ret = init_ipc(&ipc,&config_handle);
            if(ret!=0)
            {
                break;
            }
            unsigned char* set_data = msg + s32ReqHeaderLen;
            int set_data_len = len - s32ReqHeaderLen;
            config_data = (unsigned char*)malloc(stInfo.data_size);
            ret = ar_ipc_send_buffer(ipc, config_handle, INF_ALG_IPC_CMD_SET_CONFIG, set_data, set_data_len, config_data, 0);
            if(ret){
                log_tag_err("ar_ipc_send_buffer failed, cmd=%d data_len=%d",INF_ALG_IPC_CMD_GET_CONFIG,set_data_len);
                break;
            }
            log_tag_info("set config ok, data_len=%d", set_data_len);
        }
	break;
        default:
            log_tag_info("not suppported ctl 0x%x.", req->cmd_id);
        break;
    }
    
    log_tag_info("ack length %d.", s32AckLen);
    send_infrared_ack_func(req->cmd_id, pAck, s32AckLen, ret);

    release_ipc(&ipc,&config_handle);
    if(config_data) free(config_data);
    
    return 0;
}

int set_infrared_tuning_send_func(socket_send func, void *client, unsigned char module_id)
{
    if(!stInfraredServer.send_func) {
        stInfraredServer.send_func = func;
        stInfraredServer.client = client;
        stInfraredServer.module_id = module_id;
    } else {
        log_tag_err("the send function have registered");
    }
    return 0;
}

int clear_infrared_tuning_send_func(void * client)
{
    if(stInfraredServer.send_func) {
        stInfraredServer.send_func = NULL;
        stInfraredServer.client = NULL;
        stInfraredServer.module_id = -1;
    } else {
        log_tag_err("the send function is NULL");
    }
    return 0;
}

