#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdarg.h>

#include "vctrl_msg.h"
#include "vctrl_list.h"

static list_declare(venc_list);

VCHN_NODE* get_venchn_by_id(AR_U32 u32ChnId)
{
    struct listnode* node;
    VCHN_NODE* pvchn;
    list_for_each(node, &venc_list)
    {
        pvchn = node_to_item(node, VCHN_NODE, vlist);
        if(pvchn->chnId == u32ChnId)
        {
            return pvchn;
        }
    }
    return NULL;
}

void venchn_for_each(void (*func)(VCHN_NODE* pvchn))
{
    struct listnode* node;
    VCHN_NODE* pvchn;
    list_for_each(node, &venc_list)
    {
        pvchn = node_to_item(node, VCHN_NODE, vlist);
        func(pvchn);
    }
}

void queue_vchnnel_list(VCHN_NODE *pInvchn)
{
    if(pInvchn == NULL)
    {
        printf("no vailid input chnInfo\n");
        return;
    }
    struct listnode* node;
    VCHN_NODE* pvchn;
    list_for_each(node, &venc_list)
    {
        pvchn = node_to_item(node, VCHN_NODE, vlist);
        if(pvchn->chnId == pInvchn->chnId)
        {
            if (pvchn->chState == STATE_STOP) {
                printf("list already added chn%d, restart it if it's stopped\n",pvchn->chnId);
                pvchn->chState = STATE_IDLE;
            }

            return;
        }
    }

    VCHN_INFO* pChnInfo = &pInvchn->vchnInfo;

    pvchn = (VCHN_NODE*)malloc(sizeof(VCHN_NODE));
    if (!pvchn) {
        printf("malloc VCHN_NODE failed.\n");
        return;
    }
    memset(pvchn, 0x0, sizeof(VCHN_NODE));
    memcpy(pvchn, pInvchn, sizeof(VCHN_NODE));
    list_init(&pvchn->vlist);
    vchn_add_queue_tail(pvchn);
}

VCHN_NODE* dequeue_vchnnel_list(AR_U32 u32ChnId)
{
    struct listnode* node;
    VCHN_NODE* pvchn;
    list_for_each(node, &venc_list)
    {
        pvchn = node_to_item(node, VCHN_NODE, vlist);
        if(pvchn->chnId == u32ChnId)
        {
            list_remove(node);
            list_init(node);
            return pvchn;
        }
    }
    printf("can't find chn%d\n",u32ChnId);
    return NULL;
}

void vchn_add_queue_tail(VCHN_NODE* pvchn)
{
    if(list_empty(&pvchn->vlist))
    {
        list_add_tail(&venc_list, &pvchn->vlist);
    }
}

bool is_queue_empty()
{
    return list_empty(&venc_list);
}

VCHN_NODE* remove_queue_vchn_head(void)
{
    if(list_empty(&venc_list))
    {
        return NULL;
    }
    else
    {
        struct listnode* node = list_head(&venc_list);
        VCHN_NODE* pvchn = node_to_item(node, VCHN_NODE, vlist);
        list_remove(node);
        list_init(node);
        return pvchn;
    }
}

void destory_queue()
{
    while(!list_empty(&venc_list))
    {
        VCHN_NODE* pvchn = remove_queue_vchn_head();
        free(pvchn);
    }
}

void start_vchnnel_work()
{
    struct listnode* node;
    VCHN_NODE* pvchn;

    list_for_each(node, &venc_list) {
        pvchn = node_to_item(node, VCHN_NODE, vlist);

        if(pvchn->chState == STATE_IDLE) {
            //printf("start chn%d, state %d\n",pvchn->chnId, pvchn->chState);
            pvchn->chState = STATE_START;

            if(pvchn->vcodec_start)
                pvchn->vcodec_start(&pvchn->vchnInfo);

            //printf("vctrl_server start chn%d, node pvchn %p\n",pvchn->chnId, pvchn);
        }
    }
}

void stop_vchnnel_work(AR_U32 u32ChnId)
{
    struct listnode* node;
    VCHN_NODE* pvchn;
    list_for_each(node, &venc_list)
    {
        pvchn = node_to_item(node, VCHN_NODE, vlist);
        if(pvchn->chnId == u32ChnId && pvchn->chState == STATE_START)
        {
            pvchn->chState = STATE_STOP;
            if(pvchn->vcodec_stop){
                pvchn->vcodec_stop(&pvchn->vchnInfo);
            }
            //printf("stop chn%d state %d\n",pvchn->chnId, pvchn->chState);
            break;
        }
    }

    if (pvchn->chnId == u32ChnId && pvchn->chState == STATE_STOP) {
        pvchn = dequeue_vchnnel_list(pvchn->chnId);
        //printf("free chn%d pvchn %p tid %p\n",pvchn->chnId, pvchn, pvchn->vchnInfo.ptVencStartTid);
        if(NULL != pvchn) {
            free(pvchn);
            pvchn = NULL;
        }
    }
}
