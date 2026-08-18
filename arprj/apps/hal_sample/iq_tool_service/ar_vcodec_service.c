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
#include "ar_vcodec_service.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "vcodec-service"

static vcodec_server_t stVcodecServer = {NULL, -1, NULL};

static int send_vcodec_ack_func(ENUM_IQ_REQ_ID eReqId, unsigned char *pAckPayload, int s32PayloadLength, int ack_ret)
{
    int /*i = 0,*/ s32AckHeaderLength = 0;
    vcodec_ack_header_t *pAckHeader = NULL;
    unsigned char *pPayloadAddr = NULL;
    s32AckHeaderLength = sizeof(vcodec_ack_header_t);
    pAckHeader = (vcodec_ack_header_t *)malloc(s32AckHeaderLength + s32PayloadLength);
    pPayloadAddr = (unsigned char*)(pAckHeader) + s32AckHeaderLength;
    pAckHeader->request_id = eReqId;
    pAckHeader->ret_code = ack_ret;
    pAckHeader->len = s32PayloadLength;
    log_tag_info("ack cmd id 0x%x payload length %d.", *(int*)pAckHeader, s32PayloadLength);
    if (s32PayloadLength > 0)
        memcpy(pPayloadAddr, pAckPayload, s32PayloadLength);
    if (stVcodecServer.send_func && stVcodecServer.client)
        stVcodecServer.send_func(stVcodecServer.client, (unsigned char*)pAckHeader, (s32AckHeaderLength + s32PayloadLength), stVcodecServer.module_id, 0);
    else
        log_tag_err("send func or client is NULL.");
    free(pAckHeader);
    return 0;
}

int process_vcodec_tuning_func(void *client, unsigned char *msg, unsigned int len)
{
    int ret = 0, s32ReqHeaderLen = sizeof(vcodec_request_header_t), s32AckLen = 0;
    vcodec_request_header_t *req = (vcodec_request_header_t*)msg;
    unsigned char *pAck = NULL;
    switch(req->cmd_id)
    {
        case VENC_REQ_GET_VENC_INFO:
        {
            log_tag_info("VENC_REQ_GET_VENC_INFO length %d.", req->len);
            STRU_VENC_CTRL_INFO stVencCtrlInfo = { 0 };
            ret = ar_hal_venc_get_dev_info(&stVencCtrlInfo);
            pAck = (unsigned char*)&stVencCtrlInfo;
            s32AckLen = sizeof(STRU_VENC_CTRL_INFO);
        }
        break;
        case VENC_REQ_CONNECT:
        {
            int *venc_id = (int*)(msg + s32ReqHeaderLen);
            log_tag_info("VENC_REQ_CONNECT length %d venc%d.", req->len, *venc_id);
            ret = ar_hal_venc_dev_open(*venc_id);
        }
        break;
        case VENC_REQ_DIS_CONNECT:
        {
            log_tag_info("VENC_REQ_DIS_CONNECT length %d.", req->len);
            int *venc_id = (int*)(msg + s32ReqHeaderLen);
            ret = ar_hal_venc_dev_close(*venc_id);
        }
        break;
        case VENC_REQ_GET_DYNAMIC_PARAMS:
        {
            int *venc_id = (int*)(msg + s32ReqHeaderLen);
            log_tag_info("VENC_REQ_GET_DYNAMIC_PARAMS length %d venc%d.", req->len, *venc_id);
            STRU_VENC_DPARAM stVencDparam = { 0 };
            ret = ar_hal_venc_get_dparam(*venc_id, &stVencDparam);
            log_tag_info("option 0x%x bitrate %dkpbs.", stVencDparam.s32EnableOption, stVencDparam.s32BitRate);
            pAck = (unsigned char*)&stVencDparam;
            s32AckLen = sizeof(STRU_VENC_DPARAM);
        }
        break;
        case VENC_REQ_SET_DYNAMIC_PARAMS:
        {
            int *venc_id = (int*)(msg + s32ReqHeaderLen);
            log_tag_info("VENC_REQ_SET_DYNAMIC_PARAMS length %d venc%d.", req->len, *venc_id);
            STRU_VENC_DPARAM *pDparam = (STRU_VENC_DPARAM*)(venc_id + 1);
            log_tag_info("option 0x%x bitrate %dkpbs.", pDparam->s32EnableOption, pDparam->s32BitRate);
            ret = ar_hal_venc_set_dparam(*venc_id, pDparam);
        }
        break;
        default:
            log_tag_info("not suppported ctl 0x%x.", req->cmd_id);
        break;
    }
    log_tag_info("ack length %d.", s32AckLen);
    send_vcodec_ack_func(req->cmd_id, pAck, s32AckLen, ret);
    return 0;
}

int set_vcodec_tuning_send_func(socket_send func, void *client, unsigned char module_id)
{
    if(!stVcodecServer.send_func) {
        stVcodecServer.send_func = func;
        stVcodecServer.client = client;
        stVcodecServer.module_id = module_id;
    } else {
        log_tag_err("the send function have registered");
    }
    return 0;
}

int clear_vcodec_tuning_send_func(void * client)
{
    if(stVcodecServer.send_func) {
        stVcodecServer.send_func = NULL;
        stVcodecServer.client = NULL;
        stVcodecServer.module_id = -1;
    } else {
        log_tag_err("the send function is NULL");
    }
    return 0;
}

