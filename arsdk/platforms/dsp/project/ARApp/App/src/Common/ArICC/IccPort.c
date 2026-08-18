#include <stdarg.h>
#include <stdio.h>
#include "IccCore.h"
#include "IccPort.h"
#include "ThreadSynchronization.h"
#include "SystemCommon.h"

#define MAX_WAIT_FLAG_NUM (4)
#define PRINT_BUF_LEN (256)

#define osflagswaitinit (-1) ///< Wait for any flag (default).

static char print_buf[PRINT_BUF_LEN] = {0};
static volatile int32_t thread_wait_flag[MAX_WAIT_FLAG_NUM] = {osflagswaitinit, osflagswaitinit, osflagswaitinit, osflagswaitinit};

int osMutexAcquire(osMutexId_t mutex, int flag)
{

#if _FREERTOS_VERSION_ON_

	xSemaphoreTake(mutex, flag);

#else

	__asm__("dint");
	__asm__("nop");
	vSpinLock(mutex);

#endif
    return 0;
}

int osMutexRelease(osMutexId_t mutex)
{
#if _FREERTOS_VERSION_ON_

	xSemaphoreGive(mutex);

#else

	vSpinUnlock(mutex);
	__asm__("eint");
	__asm__("nop");

#endif

    return 0;
}

lock_t * lxSpinlockInitialization()
{
	lock_t * lock = NULL;

	lock = (lock_t * )malloc(sizeof(lock_t));
	//debug_print("lock:%x\r\n", lock);
	memset((void *)lock, 0x0, sizeof(lock_t));

	return lock;
}

osMutexId_t osMutexNew(const osMutexAttr_t *attr)
{
#if _FREERTOS_VERSION_ON_

	SemaphoreHandle_t xSemaphore = NULL;

    vSemaphoreCreateBinary(xSemaphore);

    return xSemaphore;
#else
    osMutexId_t mutex;

    mutex = lxSpinlockInitialization();

    return mutex;
#endif

}

int xDbgPrintf(int uart, const char *fmt, ...)
{
    va_list ap;
    int n = 0;

    memset(print_buf, 0x0, PRINT_BUF_LEN);
    /* Try to print in the allocated space. */
    va_start(ap, fmt);
    n = vsprintf(print_buf, fmt, ap);
    va_end(ap);
    xDebugPrint("%s\r\n", print_buf);

    return 0;
}

int osThreadGetId()
{
    return xGetSelfCoreId();
}

int osThreadFlagsSet(osThreadId_t owner, int32_t flag)
{
    int thread_flag = -1;

    switch (flag)
    {
    case ICC_WAIT_FLAG:
    {
        thread_flag = 0;
        break;
    }
    default:
    {
    	xDebugPrint("flag error:%d,func :%s\r\n", __LINE__, __func__);
        break;
    }
    }
    thread_wait_flag[thread_flag]++;
    //debug_print("thread_wait_flag[flag]:%d\r\n", thread_wait_flag[thread_flag]);

    return 0;
}

int osThreadFlagsWait(int flag, int32_t wait_status, int32_t opt)
{
    int thread_flag = -1;

    switch (flag)
    {
    case (ICC_WAIT_FLAG):
    {
        thread_flag = 0;
        break;
    }
    default:
    {
    	xDebugPrint("flag error:%d,func :%s\r\n", __LINE__, __func__);
        break;
    }
    }
   // debug_print("before wait:%d\r\n", thread_wait_flag[thread_flag]);
    while (wait_status != thread_wait_flag[thread_flag])
    {
    	vBlockInstructuon(10000);
    }

    return flag;
}

int osThreadFlagsClear(int flag)
{
    int thread_flag = -1;

    switch (flag)
    {
    case (ICC_WAIT_FLAG):
    {
        thread_flag = 0;
        break;
    }
    default:
    {
    	xDebugPrint("flag error:%d,func :%s\r\n", __LINE__, __func__);
        break;
    }
    }

    thread_wait_flag[thread_flag]--;

    return 0;
}
