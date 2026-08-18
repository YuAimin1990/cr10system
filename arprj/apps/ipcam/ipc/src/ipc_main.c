#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "pf_type.h"
#include "pf_video.h"
#include "pf_audio.h"
#include "pf_osd.h"
#include "pf_drv.h"
#include "pf_svp.h"

#include "util_common.h"
#include "util_timer.h"
#include "cfg_all.h"
#include "eventalarm.h"

#include "ipc_video.h"
#include "ipc_gpio.h"
#include "ipc_net.h"
#include "ipc_md.h"
#include "ipc_rtsp.h"
#include "ipc_ctrl.h"

#ifdef MODULE_SUPPORT_SEARCH
#include "ipc_search.h"
#include "ipc_tool.h"
#endif

#include "recmng_rec.h"
#include "recmng_sd.h"
#include "util_profile.h"

#include "util_profile.h"
#include "ar_comm_vo.h"
#include "mpi_sysctl.h"

static sem_t  g_semMainRun;
int g_dvp_out_mode = VO_INTF_LCD_16BIT;
static MPI_SYSCTL_HANDLE g_phandle = NULL;

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

    IPC_MID_ALARM_Init();
	
    //P2P
    IPC_NET_P2P_Start();

    //printf("start local record \n");
    IPC_MID_RECMNG_Init();

    //IPC_PF_WDT_Init();

    //IPC_PF_ADC_Init();
    IPC_PF_SVP_Init();

    //IPC_MID_UTIL_TIMER_AddTask(IPC_ADC_Read, 4, TIMER_TRUE, TIMER_FALSE);

    IPC_GPIO_Init();

    IPC_MD_Start();
	IPC_OD_Start();

    IPC_CTRL_StartRPCService();

    return 0;
}

static void IPC_MAIN_StopModules()
{
    IPC_CTRL_StopRPCService();
	
    IPC_OD_Stop();
    IPC_MD_Stop();

    IPC_GPIO_UnInit();    

	IPC_PF_SVP_UnInit();
    //IPC_MID_UTIL_TIMER_DelTask(IPC_ADC_Read);

    IPC_NET_UnInit();

    IPC_NET_P2P_Stop();

    IPC_MID_RECMNG_UnInit();

    IPC_MID_ALARM_UnInit();

    //rtc
    IPC_PF_RTC_UnInit();

    //adc
    IPC_PF_ADC_UnInit();

    IPC_PF_WDT_UnInit();

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

void IPC_MAIN_SuspendModule()
{
    IPC_CTRL_Suspend();
    
    IPC_OD_Thread_Suspend();
    IPC_MD_Thread_Suspend();  
    
    IPC_PF_SVP_Suspend();
    
    IPC_MID_RECMNG_Suspend();
      
    IPC_MID_UTIL_TIMER_Suspend();
}

void IPC_MAIN_ResumeModule()
{
    IPC_MID_UTIL_TIMER_Resume();
    
    IPC_MID_RECMNG_Resume();
	
    IPC_PF_SVP_Resume();
	
    IPC_MD_Thread_Resume(); 	
    IPC_OD_Thread_Resume();
	
    IPC_CTRL_Resume();
}

void IPC_MAIN_FastBootFunc(SYSCTL_EVENT_E eSysctlEvent, void *pData, AR_S32 s32Len)
{
    int ret;

    if(SYSCTL_EVENT_FAST_SUSPEND == eSysctlEvent ||
        SYSCTL_EVENT_SUSPEND == eSysctlEvent)
    {
        IPC_MAIN_SuspendModule();
		sleep(1);
		IPC_AV_Suspend();
    }
    else if(SYSCTL_EVENT_FAST_RESUME == eSysctlEvent ||
        SYSCTL_EVENT_RESUME == eSysctlEvent)
    {
    	IPC_AV_Resume();
       	IPC_MAIN_ResumeModule();
    }
    else
    {
        printf("fault sysctl event:%d\n", eSysctlEvent);
        return;
    }

    ret = AR_MPI_SYSCTL_Event_done(g_phandle, eSysctlEvent);
    if(ret)
        printf("event SYSCTL_Event_done fail!\n");
    else
        printf("event SYSCTL_Event_done!\n");

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
    printf("Usage: %s [options] [target] ...\n", name);
    printf("Options:\n");
    printf("\t-h       Print help info \n");
    printf("\t-m mode options:\n");
    //printf("\t\t0: single IMX307, default\n");
    //printf("\t\t1: dual IMX307\n");
    //printf("\t\t2: single IMX464\n");
    //printf("\t\t3: single IMX415\n");
    //printf("\t\t4: DVP 8x1080P@25fps(PAL), only on EVBEXT board\n");
    //printf("\t\t5: single IMX307 with HDR\n");
    //printf("\t\t6: single IMX464 with HDR\n");
    printf("\t\t7:   single SC230AI\n");
    printf("\t\t8:   dual SC230AI\n");
    printf("\t\t9:   single IMX307, <-- DEFAULT\n");
    printf("\t\t10:  dual IMX307\n");
    printf("\t\t11:  single IMX464\n");
    printf("\t\t12:  single IMX415\n");
    printf("\t\t13:  DVP 8x1080P@25fps(PAL), only on EVBEXT board\n");
    printf("\t\t14:  single IMX307 with HDR\n");
    printf("\t\t15:  single IMX464 with HDR\n");
    printf("\t\t16:  four IMX307\n");
    printf("\t\t17:  DVP 8x1080P@30fps(NTSC), only on EVBEXT board\n");
    printf("\t\t18:  single IMX307 for ArIpc\n");
    printf("\t\t19:  single IMX415 for ArIpc\n");
    printf("\t\t20:  single IMX464 for ArIpc\n");
    printf("\t\t21:  single IMX307 for ArIpc with HDR\n");
    printf("\t\t22:  single IMX464 for ArIpc with HDR\n");
    printf("\t\t23:  single SC530AI\n");
    printf("\t\t24:  single OV04A10\n");
    printf("\t\t25:  single OV04A10 with HDR\n");
    printf("\t\t26:  single IMX347\n");
    printf("\t\t27:  single OV04A10 for ArIpc\n");
    printf("\t\t28:  single OV04A10 for ArIpc with HDR\n");
    printf("\t\t29:  single IMX307 for ARS31\n");
    printf("\t\t30:  single IMX307 with HDR for ARS31\n");    
    printf("\t\t31:  single OV04A10 for ARS31\n");
    printf("\t\t32:  single OV04A10 with HDR for ARS31\n");
    printf("\t\t33:  single TIMO256 for ARS31\n");
    printf("\t\t34:  single GST417W for ARS31\n");
    printf("\t\t35:  IMX307 + TIMO256 for ARS31\n");
    printf("\t\t36:  dual imx307 for ARS31\n");
    printf("\t\t37:  OV04A10 + GST417W for ARS31\n");
    printf("\t\t38:  single GST212W4 for ARS31\n");
    printf("\t\t39:  OV04A10 + GST212W4 for ARS31\n");
    printf("\t\t40:  single OV05A20 for ARS31\n");
    printf("\t\t41:  OV05A20 + GST212W4 for ARS31\n");
    printf("\t\t42:  single H3812C1SG for ARS31\n");
    printf("\t\t43:  IMX307 + H3812C1SG for ARS31\n");
    printf("\t\tNull: run as json configured, if no json file, run as mode 9(single 307)\n");
    printf("\t-p  enable profiling, default disable\n");
    printf("\t\t0  enable profiling\n");
    printf("\t\t1  enable profiling\n");
    printf("\t-o mode       dvp output mode, supported modes: \n\
    \t\t0: VO_INTF_BT1120      (0x01L<<4) \n\
    \t\t1: VO_INTF_LCD_16BIT   (0x01L<<11) \n\
    \t\t2: VO_INTF_LCD_24BIT   (0x01L<<13) ");
    printf("Examples:\n");
    printf("%s &: run with /local/ipc_cfg/cfg_*.json settings(if they exist) or run with single 307(if no cfg file found), disable profiling\n", name);
    printf("%s -m 12 &: run with IMX415, disable profiling.\n", name);
    printf("%s -m 9 -p 1 &: run with IMX307, enable profiling.\n", name);
}

int main(int argc, char ** argv)
{
    AR_S32 s32Ret;
    int mode = 9;
    int prof = 0;
	int fast_boot = 0;
    CFG_RUN_MODE_E enRunMode = CFG_RUN_MODE_SINGLE_307_LOW_BW;
	
    sem_init(&g_semMainRun, 0, 0);

    /* Register signal handler */
    IPC_MAIN_CaptureSignal();
    DisbleProfile();

    if(argc == 2){
	    IPC_Usage(argv[0]);
		return -1;
	}
	    
    while((s32Ret = getopt(argc, argv, "m:o:p:f:h")) != -1) 
    {
        switch(s32Ret) {
        case 'o':
            mode = atoi(optarg);
            if(mode == 0)
                g_dvp_out_mode = VO_INTF_BT1120;
            else if(mode == 1)
                g_dvp_out_mode = VO_INTF_LCD_16BIT;
            else if(mode == 2)
                g_dvp_out_mode = VO_INTF_LCD_24BIT;
            printf("dvp out mode : %d\n", g_dvp_out_mode);
            break;
        case 'm':
            enRunMode = atoi(optarg);
            printf("enRunMode : %d\n", enRunMode);
            enRunMode = enRunMode < CFG_RUN_MODE_BUTT ? enRunMode : CFG_RUN_MODE_SINGLE_307_LOW_BW;
            if(enRunMode < CFG_RUN_MODE_SINGLE_SC230AI_LOW_BW)
            {
                PRINT_ERR("Mode %d not supported any more, see %s -h for help.\n", enRunMode, argv[0]);
                return -1;
            }
            break;			
        case 'p':
		    prof = atoi(optarg);
			if(prof){
			    EnableProfile();
			}
		    break;
		 case 'f':
		    fast_boot = atoi(optarg);
			if(fast_boot){
                g_phandle = AR_MPI_SYSCTL_Register(argv[0], 0, &IPC_MAIN_FastBootFunc);
                if(!g_phandle){
                    printf("register sysctl %s failed!\n", argv[0]);
                    return -1;
                }
                printf("register sysctl %s success!\n", argv[0]);
			}
		    break;
        case 'h':
            IPC_Usage(argv[0]);
            return 0;
        default:
            printf("unknow cmd param !\n");
			IPC_Usage(argv[0]);
            return -1;
        }
    }

    //PRINT_INFO("%s only runs on EVB and EVBExt board for now.\n", argv[0]);
    PRINT_INFO("%s compiled at: %s %s\n", argv[0], __DATE__, __TIME__);

    IPC_PF_RTC_Init();
    IPC_PF_RTC_UpdateLocalTime();

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

        IPC_PF_WDT_KeepAlive();
    }

#endif

    //IPC_MAIN_StopAll();
    PRINT_INFO("====ipcam exit !!!\n");
    exit(0);
}



