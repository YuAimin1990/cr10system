#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "eventalarm.h"

typedef struct EVNET_ALARM_API
{
    event_alarm_cb_func alarm_cb;

    struct EVNET_ALARM_API *next;

}ST_EVNET_ALARM_API, *PS_ST_EVNET_ALARM_API;

static int g_ptapi_stop = 1;
static ST_EVNET_ALARM_API *g_ptapi = NULL;
static pthread_mutex_t event_lock;

int IPC_MID_ALARM_Init(void)
{
    if(g_ptapi_stop == 0)
    {
        printf("has inited");
        return -1;
    }

    pthread_mutex_init(&event_lock, NULL);

    g_ptapi = NULL;
    g_ptapi_stop = 0;
    return 0;
}

int IPC_MID_ALARM_UnInit(void)
{
    ST_EVNET_ALARM_API *pPtApi = g_ptapi;
    ST_EVNET_ALARM_API *p1;

    if(g_ptapi_stop == 0)
    {
        g_ptapi_stop = 1;
        pthread_mutex_lock(&event_lock);
         while (pPtApi ) {
            p1 = pPtApi;
            pPtApi = pPtApi->next;
            free(p1);
        }
        pthread_mutex_unlock(&event_lock);
        pthread_mutex_destroy(&event_lock);

    }
    return 0;
}



void* IPC_MID_ALARM_Open(event_alarm_cb_func alarm_cb)
{
    ST_EVNET_ALARM_API *pPtApi = NULL;

    if (g_ptapi_stop != 0) {
        printf("not init");
        return NULL;
    }

    if(alarm_cb == NULL)
    {
        printf("parameter error");
        return NULL;
    }


    pPtApi = (ST_EVNET_ALARM_API *)malloc(sizeof(ST_EVNET_ALARM_API));
    if (pPtApi == NULL) {
        printf("malloc error");
        return NULL;
    }
    memset(pPtApi, 0, sizeof(ST_EVNET_ALARM_API));

    pthread_mutex_lock(&event_lock);
    pPtApi->alarm_cb = alarm_cb;

    if (g_ptapi == NULL) {
        g_ptapi = pPtApi;
    } else {
       pPtApi->next = g_ptapi;
       g_ptapi = pPtApi;
    }
    pthread_mutex_unlock(&event_lock);
    return (void*)(pPtApi);
}

int IPC_MID_ALARM_Close(void *handle)
{
    ST_EVNET_ALARM_API  *p1 = g_ptapi;
    ST_EVNET_ALARM_API  *p2 = NULL;
    if(handle == NULL || g_ptapi_stop )
    {
        return 0;
    }

    pthread_mutex_lock(&event_lock);

    while (p1 != NULL ) {
        if (p1 == handle) {
            break;
        }
        else
        {
            p2 = p1;
        }
        p1 = p1->next;
    }
    // ÕÒµ½handle
    if(p1 != NULL)
    {
        if(p1 == g_ptapi)
        {
            g_ptapi = g_ptapi->next;
        }
        else
        {
            p2->next = p1->next;
        }
        free(p1);
    }

    pthread_mutex_unlock(&event_lock);
    return 0;
}


int IPC_MID_ALARM_Trigger(int nChannel, ALARM_TYPE_E nAlarmType, int nAction, void* pParam)
{
    ST_EVNET_ALARM_API *pPtApi = g_ptapi;
    while (pPtApi && g_ptapi_stop == 0) {

        pPtApi->alarm_cb(nChannel, nAlarmType, nAction, pParam);
	    pPtApi = pPtApi->next;
    }

    return 0;
}



