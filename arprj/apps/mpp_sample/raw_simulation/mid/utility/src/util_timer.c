#include "util_api.h"
#include "util_timer.h"

static int timerRun = 0;

static pthread_t timerId ;
static pthread_rwlock_t timer_lock ;

static TIMER_INFO_T g_timers[TIMEER_NUMBER];

unsigned int timer_get_ms()
{
    struct timeval tv;
    unsigned int time;
    gettimeofday(&tv, NULL);
    time = tv.tv_sec*1000  + tv.tv_usec / 1000;
    return time;
}

int IPC_MID_UTIL_TIMER_AddTask(TIMER_HANDLER const task_handler, int inter_250ms, TIMER_BOOL immediate,TIMER_BOOL oneshot)
{
    int i;
    int ret = 0;

    if(timerRun == 0)
    {
        IPC_MID_UTIL_TIMER_Init();
    }
    pthread_rwlock_wrlock(&timer_lock);
    for(i = 0; i < TIMEER_NUMBER; i++)
    {
        if(g_timers[i].id != 0 &&  task_handler == g_timers[i].fpcallback)
        {
            break;
        }
    }

    if(i == TIMEER_NUMBER)
    {
         for(i = 0; i < TIMEER_NUMBER; i++)
         {
            if(g_timers[i].id  == 0 )
            {
                break;
            }
         }
    }

    if(i != TIMEER_NUMBER)
    {
        memset(&g_timers[i],0,sizeof(TIMER_INFO_T));
        g_timers[i].fpcallback = task_handler;
        if(immediate == TIMER_TRUE)
        {
            g_timers[i].stamp = 0;
        }
        else
        {
            g_timers[i].stamp =  timer_get_ms();
        }

        g_timers[i].interval = inter_250ms * 250; // <= 400, in order to update real time to OSD.
        g_timers[i].id = i+1;
        g_timers[i].oneshot = oneshot;
        g_timers[i].argNum = 0;

    }

    pthread_rwlock_unlock(&timer_lock);
    if(i == TIMEER_NUMBER)
    {
        ret = -1;
    }
    return ret;

}

int IPC_MID_UTIL_TIMER_AddTask2(TIMER_HANDLER2 const task_handler, int interl,TIMER_BOOL immediate,TIMER_BOOL oneshot, void *lPara, void *pPara)
{
    int i;
    int ret = 0;
    if(timerRun == 0)
    {
        return -1;
    }

    if(task_handler == NULL)
    {
        return -1;
    }
    pthread_rwlock_wrlock(&timer_lock);
    for(i = 0; i < TIMEER_NUMBER; i++)
    {
        if(g_timers[i].id != 0 &&  task_handler == g_timers[i].fpcallback2)
        {
            break;
        }
    }

    if(i == TIMEER_NUMBER)
    {
         for(i = 0; i < TIMEER_NUMBER; i++)
         {
            if(g_timers[i].id  == 0 )
            {
                break;
            }
         }
    }

    if(i != TIMEER_NUMBER)
    {
        memset(&g_timers[i],0,sizeof(TIMER_INFO_T));
        g_timers[i].fpcallback2 = task_handler;
        if(immediate == TIMER_TRUE)
        {
            g_timers[i].stamp = 0;
        }
        else
        {
            g_timers[i].stamp =  timer_get_ms();
        }
        g_timers[i].id = i+1;
        g_timers[i].interval = interl*1000; // ms
        g_timers[i].oneshot = oneshot;
        g_timers[i].argNum = 2;
        g_timers[i].arg1 = lPara;
        g_timers[i].arg2 = pPara;
    }
    pthread_rwlock_unlock(&timer_lock);
    if(i == TIMEER_NUMBER)
    {
        ret = -1;
    }
    return ret;
}

void IPC_MID_UTIL_TIMER_DelTask(TIMER_HANDLER const task_handler)
{
    int i;
    if(timerRun == 0)
    {
        return;
    }
    for(i = 0; i < TIMEER_NUMBER; i++)
    {
        if(g_timers[i].id != 0 &&  task_handler == g_timers[i].fpcallback)
        {
            break;
        }
    }

    if(i != TIMEER_NUMBER)
    {
        pthread_rwlock_wrlock(&timer_lock);
        memset(&g_timers[i],0,sizeof(TIMER_INFO_T));
        pthread_rwlock_unlock(&timer_lock);
    }
}

void IPC_MID_UTIL_TIMER_DelTask2(TIMER_HANDLER2 const task_handler)
{
    int i;
    if(timerRun == 0)
    {
        return;
    }

    for(i = 0; i < TIMEER_NUMBER; i++)
    {
        if(g_timers[i].id != 0 &&  task_handler == g_timers[i].fpcallback2)
        {
            break;
        }
    }

    if(i != TIMEER_NUMBER)
    {
        pthread_rwlock_wrlock(&timer_lock);
        memset(&g_timers[i],0,sizeof(TIMER_INFO_T));
        pthread_rwlock_unlock(&timer_lock);
    }



}

static void *timer_thread(void *arg)
{
    int i;
    TIMER_INFO_T *timer;
    TIMER_INFO_T extimer;
    unsigned int timeStap ;

    IPC_MID_UTIL_SetThreadName("timer_thread");
    while(timerRun)
    {
        for(i = 0; i < TIMEER_NUMBER; i++)
        {
            pthread_rwlock_wrlock(&timer_lock);
            if(g_timers[i].id != 0 )
            {
                timer = &g_timers[i];
                timeStap = timer_get_ms();
                if( (timeStap < timer->stamp) || (timeStap >(timer->stamp+timer->interval)) )
                {
                    memcpy(&extimer,timer,sizeof(TIMER_INFO_T));
                    if(timer->oneshot == TIMER_TRUE)
                    {
                        memset(timer,0,sizeof(TIMER_INFO_T));
                    }
                    else
                    {
                        timer->stamp = timer_get_ms();
                    }

                    if(extimer.argNum == 0)
                    {
                        extimer.fpcallback();
                    }
                    else
                    {
                        extimer.fpcallback2(extimer.arg1,extimer.arg2);
                    }
                }
            }
            pthread_rwlock_unlock(&timer_lock);
        }

        usleep(100000);
    }
    return 0;
}

int  IPC_MID_UTIL_TIMER_Init(void)
{
    if(timerRun == 0)
    {
        pthread_rwlock_init(&timer_lock, NULL);
        memset(g_timers,0,sizeof(g_timers));
        timerRun = 1;
        pthread_create(&timerId, NULL, timer_thread, NULL);
    }
    return 0;

}
void IPC_MID_UTIL_TIMER_UnInit()
{
    if(timerRun == 1)
    {
        timerRun = 0;
        pthread_join(timerId,NULL);
        pthread_rwlock_wrlock(&timer_lock);
        memset(g_timers,0,sizeof(g_timers));
        pthread_rwlock_unlock(&timer_lock);
    }
}

