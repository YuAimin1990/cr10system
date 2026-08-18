#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pf_type.h"
#include "pf_drv.h"
#include "pf_video.h"

#include "ipc_gpio.h"
#include "ipc_net.h"

#include "cfg_all.h"
#include "util_api.h"
#include "recmng_rec.h"
#include "recmng_sd.h"

typedef enum {
	LX_RUN_WAIT = 1,		
	LX_RUN_COMM,			
	LX_RUN_RECORD,			
	LX_RUN_SNAP,			
}LX_RUN_STATUS;


typedef enum {
	LX_LED_THREAD_EXIT = 0,
	LX_LED_THREAD_RUN ,
	LX_LED_THREAD_SUSPEND ,
} LX_LED_THREAD_STATUS;

#define	LED_SLOW_FLASH			(1000 * 1000)
#define	LED_NORMAL_FLASH		(500 * 1000)
#define	LED_FAST_FLASH			(200 * 1000)


#define IPC_BUTTON_ACTION_SNAPSHOT   1
#define IPC_BUTTON_ACTION_REC_START  2
#define IPC_BUTTON_ACTION_REC_STOP   3

static LX_LED_THREAD_STATUS		s_enumLedRedThreadRun = LX_LED_THREAD_EXIT;
static LX_LED_THREAD_STATUS		s_enumLedBlueThreadRun = LX_LED_THREAD_EXIT;

static int s_u32ButtonLoop = 0;

static int adc_value = 0;

static int s_u32RedOn = 0;
static int s_u32BlueOn = 0;
static int s_u32InfraOn = 0;
static int s_u32GreenOn = 0;

void IPC_GPIO_SysRestoreHandle()
{

    IPC_MID_RECMNG_StopRecAll();

    #if 0
    g_stRunNetworkCfg.wireless.mode = 0;
    IPC_CFG_NETWORK_Save();
    UserClean();
    IPC_CFG_USER_Save();
    #else
    IPC_CFG_ClearAll();
    #endif

    IPC_CFG_SYSTEM_Save();
    IPC_CFG_VENDOR_Save();

    sleep(1);

    PRINT_INFO("reboot 0\n");
    IPC_MID_UTIL_SysCall("reboot", NULL, 0);
    sleep(1);

    PRINT_INFO("reboot 1\n");
    system("reboot");
    sleep(1);

    PRINT_INFO("reboot 2\n");
    system("reboot");
    sleep(1);

}


//is_press_rec: 1-sanp, 2- rec start, 3- rec stop
static int IPC_GPIO_DoAction(int delta)
{
    if(delta < 1)
    {
        //To capture which channel ? TBD...
        IPC_PF_VENC_SnapProcess(0, 0, 1, "/mnt/buttonsnap/");
        return 0;
    }
    else if(delta < 5)
    {
        IPC_MID_RECMNG_StartRecAll();
    }
    else //longer than 5 sec, stop all
    {
        IPC_MID_RECMNG_StopRecAll();
    }

    return 0;
}

static void IPC_GPIO_HandleButton()
{
    struct timeval tv1, tv2;
    IPC_CFG_GPIO_S * pstGpioCfg = NULL;
    int value = -1;
    int dir = 0;

    pstGpioCfg = IPC_CFG_GPIO_GetParam();

    if (!pstGpioCfg->rec.enable)
    {
        return;
    }

    gettimeofday(&tv1, NULL);
    tv2.tv_sec = tv1.tv_sec;

    IPC_PF_GPIO_GetByName(pstGpioCfg->rec.gpio_name, &dir, &value);
    if(value != pstGpioCfg->rec.gpio_value) //not pressed
    {
        return;
    }

//    if(button_is_pressed)
//     then we start the waiting.
    while (1)
    {
        value = -1;
        IPC_PF_GPIO_GetByName(pstGpioCfg->rec.gpio_name,&dir, &value);
        if(value != pstGpioCfg->rec.gpio_value) //has released
        {
            gettimeofday(&tv2, NULL);
            break;
        }

        usleep(200*1000);
    }

    int delta = tv2.tv_sec - tv1.tv_sec;

    IPC_GPIO_DoAction(delta);

}

void * IPC_GPIO_CheckButtonLoop(void *arg)
{
    prctl(PR_SET_NAME, "CheckButton", 0, 0, 0);

    while (s_u32ButtonLoop)
    {
        IPC_GPIO_HandleButton();
        usleep(500 * 1000);
    }

    return NULL;
}

void IPC_ADC_Read(int * value)
{
    int ret;

    ret = IPC_PF_ADC_Read(&adc_value);
    if (ret < 0)
    {
        *value = 0xffffffff;
        return;
    }

    *value = adc_value;

    //shutdown
    #if 0
    if ((adc_value < adc_limit) && (adc_value > 0)) {
        PRINT_INFO("adc value:%d < %d, to power down\n", adc_value, adc_limit);

        IPC_PF_SysShutdown();
    }
    #endif
}


static int led_ctrl(int gpioNum, int up_down)
{
    if (gpioNum <= 0 || gpioNum >= 65) {
        PRINT_ERR("gpioNum %d error.\n", gpioNum);
        return -1;
    }

    IPC_PF_GPIO_SetValue(gpioNum, up_down);

    return 0;
}
#if 0
void IPC_GPIO_RedLedOn()
{
    IPC_CFG_GPIO_S * pstGpioCfg = IPC_CFG_GPIO_GetParam();

    if (pstGpioCfg->red.enable)
    {
        led_ctrl(pstGpioCfg->red.gpio_num, pstGpioCfg->red.gpio_value);
        s_u32RedOn = 1;
    }
}

void IPC_GPIO_RedLedOff()
{
    IPC_CFG_GPIO_S * pstGpioCfg = IPC_CFG_GPIO_GetParam();

    led_ctrl(pstGpioCfg->red.gpio_num, 1 - pstGpioCfg->red.gpio_value);
    s_u32RedOn = 0;
}

void IPC_GPIO_BlueLedOn()
{
    IPC_CFG_GPIO_S * pstGpioCfg = IPC_CFG_GPIO_GetParam();

    if (pstGpioCfg->blue.enable)
    {
        led_ctrl(pstGpioCfg->blue.gpio_num, pstGpioCfg->blue.gpio_value);
        s_u32BlueOn = 1;
    }
}

void IPC_GPIO_BlueLedOff()
{
    IPC_CFG_GPIO_S * pstGpioCfg = IPC_CFG_GPIO_GetParam();

    led_ctrl(pstGpioCfg->blue.gpio_num, 1 - pstGpioCfg->blue.gpio_value);
    s_u32BlueOn = 0;
}

void IPC_GPIO_GreenLedOn()
{
    IPC_CFG_GPIO_S * pstGpioCfg = IPC_CFG_GPIO_GetParam();

    if (pstGpioCfg->green.enable)
    {
        led_ctrl(pstGpioCfg->green.gpio_num, pstGpioCfg->green.gpio_value);
        s_u32GreenOn = 1;
    }
}

void IPC_GPIO_GreenLedOff()
{
    IPC_CFG_GPIO_S * pstGpioCfg = IPC_CFG_GPIO_GetParam();

    led_ctrl(pstGpioCfg->green.gpio_num, 1 - pstGpioCfg->green.gpio_value);
    s_u32GreenOn = 0;
}

void IPC_GPIO_InfraOn()
{
    IPC_CFG_GPIO_S * pstGpioCfg = IPC_CFG_GPIO_GetParam();

    if (pstGpioCfg->infra.enable)
    {
        led_ctrl(pstGpioCfg->infra.gpio_num, pstGpioCfg->infra.gpio_value);
        s_u32InfraOn = 1;
    }
}

void IPC_GPIO_InfraOff()
{
    IPC_CFG_GPIO_S * pstGpioCfg = IPC_CFG_GPIO_GetParam();

    led_ctrl(pstGpioCfg->infra.gpio_num, 1 - pstGpioCfg->infra.gpio_value);
    s_u32InfraOn = 0;
}

int IPC_GPIO_GetRedLedStatus()
{
    return s_u32RedOn;
}

int IPC_GPIO_GetBlueLedStatus()
{
    return s_u32BlueOn;
}

int IPC_GPIO_GetGreenLedStatus()
{
    return s_u32GreenOn;
}

int IPC_GPIO_GetInfraStatus()
{
    return s_u32InfraOn;
}

void led_twinkle(int gpioNum, int value, int times)
{
    while (times > 0) {
        led_ctrl(gpioNum, value);
        usleep(1000*200);
        led_ctrl(gpioNum, 1 - value);
        usleep(1000*200);
        times --;
    }
}

void led_red_twinkle(int times)
{
    if (g_stRunGpioCfg.red.enable) {
        s_enumLedRedThreadRun = LX_LED_THREAD_SUSPEND;
        led_twinkle(g_stRunGpioCfg.red.gpio_num, g_stRunGpioCfg.red.gpio_value, times);
        s_enumLedRedThreadRun = LX_LED_THREAD_RUN;
    }
}

void led_blue_twinkle(int times)
{
    if (g_stRunGpioCfg.blue.enable) {
        s_enumLedBlueThreadRun = LX_LED_THREAD_SUSPEND;
        led_twinkle(g_stRunGpioCfg.blue.gpio_num, g_stRunGpioCfg.blue.gpio_value, times);
        s_enumLedBlueThreadRun = LX_LED_THREAD_RUN;
    }
}

void IPC_GPIO_DisableLed()
{
    IPC_CFG_GPIO_S * pstGpioCfg = IPC_CFG_GPIO_GetParam();

    IPC_GPIO_BlueLedOff();
    IPC_GPIO_RedLedOff();
    IPC_GPIO_GreenLedOff();
    IPC_GPIO_InfraOff();

    pstGpioCfg->blue.enable = 0;
    pstGpioCfg->red.enable = 0;
    pstGpioCfg->green.enable = 0;
    pstGpioCfg->infra.enable = 0;
}

void IPC_GPIO_EnableLed()
{
    IPC_CFG_GPIO_S * pstGpioCfg = IPC_CFG_GPIO_GetParam();

    pstGpioCfg->blue.enable = 1;
    pstGpioCfg->red.enable = 1;
    pstGpioCfg->green.enable = 1;
    pstGpioCfg->infra.enable = 1;
}

void IPC_GPIO_LedOn()
{
    IPC_CFG_GPIO_S * pstGpioCfg = IPC_CFG_GPIO_GetParam();

    pstGpioCfg->blue.enable = 1;
    pstGpioCfg->red.enable = 1;
    pstGpioCfg->green.enable = 1;
    pstGpioCfg->infra.enable = 1;

    IPC_GPIO_BlueLedOn();
    IPC_GPIO_RedLedOn();
    IPC_GPIO_GreenLedOn();
    IPC_GPIO_InfraOn();
}

void IPC_GPIO_LedOff()
{
    IPC_CFG_GPIO_S * pstGpioCfg = IPC_CFG_GPIO_GetParam();

    pstGpioCfg->blue.enable = 0;
    pstGpioCfg->red.enable = 0;
    pstGpioCfg->green.enable = 0;
    pstGpioCfg->infra.enable = 0;

    IPC_GPIO_BlueLedOff();
    IPC_GPIO_RedLedOff();
    IPC_GPIO_GreenLedOff();
    IPC_GPIO_InfraOff();
}

void *IPC_GPIO_RedLedThread(void *arg)
{
    prctl(PR_SET_NAME, "RedLed", 0, 0, 0);

	LX_RUN_STATUS 	enRunStatus = LX_RUN_WAIT;
	AR_S32			s32FlashInterval = LED_SLOW_FLASH;		//LED_NORMAL_FLASH;

    AR_U32 u32RedLed = 0;

	while (s_enumLedRedThreadRun)
    {
		if (LX_LED_THREAD_SUSPEND == s_enumLedRedThreadRun)
        {
			usleep(1000*500);
			continue;
		}

		u32RedLed = 1 - u32RedLed;

        int sd_exist = 0;
        if ((IPC_MID_RECMNG_SDStatusIs(SD_STATUS_IS_INSERT_OFFSET)) && (IPC_MID_RECMNG_SDStatusIs(SD_STATUS_IS_WRITABLE_OFFSET)))
        {
            sd_exist = 1;
        }

        //printf("sd_exist:%d\n", sd_exist);

        if (sd_exist)
        {
    		if (IPC_MID_RECMNG_SDStatusIs(SD_STATUS_IS_SNAP_OFFSET))
            {
    			enRunStatus = LX_RUN_SNAP;
    		}
            else if (IPC_MID_RECMNG_SDStatusIs(SD_STATUS_IS_REC_OFFSET))
            {
    			enRunStatus = LX_RUN_RECORD;
    		}
            else
            {
    			enRunStatus = LX_RUN_COMM;
    		}
            //printf("enRunStatus:%d\n", enRunStatus);

    		switch(enRunStatus)
            {
    			case LX_RUN_WAIT:
    			case LX_RUN_COMM:
                {
    				IPC_GPIO_RedLedOn();
    				break;
    			}
    			case LX_RUN_RECORD:
                {
    				if (u32RedLed)
                    {
                        IPC_GPIO_RedLedOn();
                    }
                    else
                    {
                        IPC_GPIO_RedLedOff();
                    }
    				break;
    			}
    			case LX_RUN_SNAP:
                {
    				IPC_GPIO_RedLedOff();
    				break;
    			}
    			default:
                {
    				PRINT_ERR("not support the status:%d\n", enRunStatus);
    				break;
    			}
    		}
        }
        else
        {
            //PRINT_INFO("sd is not exist, led red off\n");
            IPC_GPIO_RedLedOff();
		}

		usleep(s32FlashInterval);
	}

	return 0;
}

void *IPC_GPIO_BlueLedThread(void *arg)
{
    prctl(PR_SET_NAME, "BlueLed", 0, 0, 0);

	E_NET_STATUS 	enRunStatus = E_NET_WAIT;
	AR_S32			s32FlashInterval = LED_SLOW_FLASH;		//LED_NORMAL_FLASH;

    AR_U32 u32BlueLed = 0;

	while (s_enumLedBlueThreadRun)
    {
		if (LX_LED_THREAD_SUSPEND == s_enumLedBlueThreadRun)
        {
			usleep(1000*500);
			continue;
		}

		u32BlueLed = 1- u32BlueLed;

        enRunStatus = IPC_NET_GetStatus();

		switch(enRunStatus)
        {
			case E_NET_WAIT:
            {
				IPC_GPIO_BlueLedOn();
				break;
			}
			case E_NET_AP_BEGIN:
            {
                s32FlashInterval = LED_SLOW_FLASH;
				if (u32BlueLed)
                {
                    IPC_GPIO_BlueLedOn();
                }
                else
                {
                    IPC_GPIO_BlueLedOff();
                }
				break;
			}
			case E_NET_AP_END:
            {
				IPC_GPIO_BlueLedOn();
				break;
			}
			case E_NET_STA_BEGIN:
            {
                s32FlashInterval = LED_FAST_FLASH;
				if (u32BlueLed)
                {
                    IPC_GPIO_BlueLedOn();
                }
                else
                {
                    IPC_GPIO_BlueLedOff();
                }
				break;
			}
			case E_NET_STA_END:
            {
				IPC_GPIO_BlueLedOn();
				break;
			}
			case E_NET_STA_NFS:
            {
				IPC_GPIO_BlueLedOn();
				break;
			}
			case E_NET_STA_TEST:
            {
				IPC_GPIO_BlueLedOn();
				break;
			}
			default:
            {
				PRINT_ERR("not support the status:%d\n", enRunStatus);
				break;
			}
		}

		usleep(s32FlashInterval);
	}

	return 0;
}



void IPC_GPIO_InitLed()
{

}
#endif //if 0, keep for reference

int IPC_GPIO_Init()
{
#if 0
    IPC_CFG_GPIO_Print();
    IPC_GPIO_InitLed();

    //IPC_CFG_GPIO_S * pstGpioCfg = IPC_CFG_GPIO_GetParam();

    s_enumLedRedThreadRun = LX_LED_THREAD_RUN;
    IPC_MID_UTIL_CreateDetachThread(IPC_GPIO_RedLedThread, NULL, NULL);
    s_enumLedBlueThreadRun = LX_LED_THREAD_RUN;
    IPC_MID_UTIL_CreateDetachThread(IPC_GPIO_BlueLedThread, NULL, NULL);

    s_u32ButtonLoop = 1;
    IPC_MID_UTIL_CreateDetachThread(IPC_GPIO_CheckButtonLoop, NULL, NULL);
#endif
    return 0;
}

void IPC_GPIO_UnInit()
{
#if 0
    s_enumLedRedThreadRun = LX_LED_THREAD_EXIT;
    s_enumLedBlueThreadRun = LX_LED_THREAD_RUN;
    s_u32ButtonLoop = 0;
#endif
}

