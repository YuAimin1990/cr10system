#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "pf_type.h"
#include "pf_video.h"

#include "util_common.h"
#include "util_timer.h"
#include "cfg_all.h"

#include "ipc_video.h"
#include "ipc_net.h"
#include "ipc_rtsp.h"

#ifdef MODULE_SUPPORT_SEARCH
#include "ipc_search.h"
#include "ipc_tool.h"
#endif

#include "recmng_rec.h"
#include "recmng_sd.h"
#include "util_profile.h"

#include "util_profile.h"
#include "ar_comm_vo.h"
static sem_t  g_semMainRun;
int g_dvp_out_mode = VO_INTF_LCD_16BIT;

//*****************************************************************************
//*****************************************************************************
//** Local Functions
//*****************************************************************************
//*****************************************************************************
static AR_S32 IPC_MAIN_StartPlatform()
{
    int ret = IPC_PF_SYS_Init();
    if (0 != ret)
    {
        PRINT_ERR("IPC_PF_SYS_Init failed!\n");
        return -1;
    }

	return IPC_AV_Init();
}

static void IPC_MAIN_StopPlatform()
{
    IPC_AV_UnInit();
	IPC_PF_SYS_UnInit();
}

static AR_S32 IPC_MAIN_StartModules()
{
    IPC_MID_UTIL_TIMER_Init();

    IPC_NET_Init();

    //P2P
    IPC_NET_P2P_Start();

    //printf("start local record \n");
    IPC_MID_RECMNG_Init();

    //IPC_PF_WDT_Init();

    //IPC_PF_ADC_Init();

    //IPC_MID_UTIL_TIMER_AddTask(IPC_ADC_Read, 4, TIMER_TRUE, TIMER_FALSE);

    return 0;
}

static void IPC_MAIN_StopModules()
{
    //IPC_MID_UTIL_TIMER_DelTask(IPC_ADC_Read);

    IPC_NET_UnInit();

    IPC_NET_P2P_Stop();

    IPC_MID_RECMNG_UnInit();

    IPC_MID_UTIL_TIMER_UnInit();
}

static void IPC_MAIN_StopAll()
{
    IPC_MAIN_StopModules();

    IPC_MAIN_StopPlatform();
}

static AR_S32 IPC_MAIN_StartAll()
{
    AR_S32 s32Ret;
    s32Ret = IPC_MAIN_StartPlatform();
    if (s32Ret < 0)
        return -1;

    s32Ret = IPC_MAIN_StartModules();
    if (s32Ret < 0)
        return -1;

    return 0;
}

/*
   -------------------------------------------------------------------------
       SIGHUP        1       Term    Hangup detected on controlling terminal
                                     or death of controlling process
       SIGINT        2       Term    Interrupt from keyboard
       SIGQUIT       3       Core    Quit from keyboard
       SIGILL        4       Core    Illegal Instruction
       SIGABRT       6       Core    Abort signal from abort(3)
       SIGFPE        8       Core    Floating point exception
       SIGKILL       9       Term    Kill signal
       SIGSEGV      11       Core    Invalid memory reference
       SIGPIPE      13       Term    Broken pipe: write to pipe with no readers
       SIGALRM      14       Term    Timer signal from alarm(2)
       SIGTERM      15       Term    Termination signal
       SIGUSR1   30,10,16    Term    User-defined signal 1
       SIGUSR2   31,12,17    Term    User-defined signal 2
       SIGCHLD   20,17,18    Ign     Child stopped or terminated
       SIGCONT   19,18,25            Continue if stopped
       SIGSTOP   17,19,23    Stop    Stop process
       SIGTSTP   18,20,24    Stop    Stop typed at tty
       SIGTTIN   21,21,26    Stop    tty input for background process
       SIGTTOU   22,22,27    Stop    tty output for background process
*/

static AR_S32 bExit = 0;
static void signal_hander(AR_S32 signal)
{
    char name[32];
    if(bExit == 0)
    { 
        printf("____________Application will exit by signal:%d, pid:%d\n", signal, getpid());
        pid_t tid;
        tid = syscall(SYS_gettid);
        prctl(PR_GET_NAME, (unsigned long)name);
        printf("____________Application exit thread tid: %d, name %s\n", tid, name);
        //IPC_MAIN_StopAll();
        
        #if 0
        if (signal == SIGSEGV) {
            SIG_DFL(signal);
        } else {
            IPC_MAIN_StopAll();
        }
        #else
        if (signal == SIGSEGV)
        {
            SIG_DFL(signal);
        }
        #endif
        bExit = 1;
    }

}

static void sig_chld(AR_S32 signo)
{
    pid_t   pid;
    AR_S32     stat;

    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
    {
        //printf("child %d terminated\n", pid);
    }
}

void IPC_MAIN_CaptureSignal()
{
    AR_S32 i = 0;
    for (i = 0; i < 32; i ++)
    {
        if (i == SIGCHLD)
        {
            //signal(i, SIG_DFL);
            signal(i, sig_chld);
        }
        else if ( (i == SIGPIPE)|| (i == SIGALRM))
        {
            signal(i, SIG_IGN);
        }
        else
        {
            signal(i, signal_hander);
        }
    }
}

//static void main_stop()
//{
//    sem_post(&g_semMainRun);
//}
void IPC_Usage(char * name)
{
    printf("Usage: %s & ...\n", name);
    printf("You can set parameters in /usrdata/local/raw_simu_cfg/*.json.\n");
}

int main(int argc, char ** argv)
{
    AR_S32 s32Ret;
    int mode = 9;
    int prof = 0;
    CFG_RUN_MODE_E enRunMode = CFG_RUN_MODE_ISP_RAW_SIMU;
	
    sem_init(&g_semMainRun, 0, 0);

    /* Register signal handler */
    IPC_MAIN_CaptureSignal();
    DisbleProfile();


    PRINT_INFO("%s is running on ISP Raw Simulation mode .\n", argv[0]);
    PRINT_INFO("%s compiled at: %s %s\n", argv[0], __DATE__, __TIME__);

    /* Load configure parameters from json files */
    /* This must be called at very first in order to get proper settings for modules*/
    IPC_CFG_LoadAll(enRunMode);
    IPC_CFG_PrintAll();

    /* Start the services */
    s32Ret = IPC_MAIN_StartAll();
    if (s32Ret < 0)
    {
        IPC_MAIN_StopAll();
        exit(-1);
    }

    atexit(IPC_MAIN_StopAll);

#if 0
    /* Blocked here */
    sem_wait(&g_semMainRun);
#else

    struct timespec timeout;

    while(1)
    {
        if(bExit)
        {
            PRINT_ERR("ipcam will exit!!!\n");
            break;
        }
		
        s32Ret = clock_gettime(CLOCK_REALTIME, &timeout);
        if(s32Ret < 0)
        {
            PRINT_ERR("gclock_gettime error\n");
            break;
        }

        timeout.tv_sec += 5; // Feed dog every 5s

        s32Ret = sem_timedwait(&g_semMainRun, &timeout);
        if(s32Ret == 0)
        {
            PRINT_INFO("get g_semMainRun, to break\n");
            break;
        }

        if (IPC_PF_VENC_CheckAlive() < 0)
        {
            PRINT_ERR("encode exception...");
            //system("cat /proc/interrupts ");
            continue;
        }
    }

#endif

    //IPC_MAIN_StopAll();
    PRINT_INFO("%s exit !!!\n", argv[0]);
    exit(0);
}



