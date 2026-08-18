

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "Uart.h"
#include "SystemCommon.h"

#define LOCK_PRINT (0)
#define PRINT_BUF_LEN (512)

#if LOCK_PRINT
lock_t  dsp_print_lock __attribute__((section (".DSECT print_lock_section"))) = 0;
#endif

#define LENGTH_FOR_EACH_LOG         (256)
#define TOTAL_LOG_NUM				(2046)
#define LOG_TO_LINUX_UART			(1)
#define LOG_TO_CEVA_UART			(0)

volatile unsigned char * dsp_print_buf_addr = NULL;
static volatile unsigned int *lock ; // = dsp_print_buf_addr;
static volatile unsigned int *tx_idx ; //= dsp_print_buf_addr+4;
static volatile unsigned int *dsp_inited_flag ; // = dsp_print_buf_addr+8;
static volatile unsigned int *linux_inited_flag ; //= dsp_print_buf_addr+12;
static volatile unsigned int *write_finish ; //= dsp_print_buf_addr+16;
static char *dspmsg ;

//extern void icc_shm_acquire_lock(unsigned int *plock);
//extern void icc_shm_release_lock(unsigned int *plock);

void vUartAcquireLock(lock_t *plock)
{
	__asm__("dint");
	__asm__("nop");
	vSpinLock(plock);
}

void vUartReleaseLock(lock_t *plock)
{
	vSpinUnlock(plock);
	__asm__("eint");
	__asm__("nop");
}


int xDebugInit(char * buffAddr)
{
	*(unsigned int*)UART_CFG = 0x2B;

	#if LOG_TO_LINUX_UART
	dsp_print_buf_addr = buffAddr;
	lock = (volatile unsigned int *)dsp_print_buf_addr;
	tx_idx = (volatile unsigned int *)(dsp_print_buf_addr+4);
	dsp_inited_flag = (volatile unsigned int *)(dsp_print_buf_addr+8);
	linux_inited_flag = (volatile unsigned int *)(dsp_print_buf_addr+12);
	write_finish = (volatile unsigned int *)(dsp_print_buf_addr+16);
	dspmsg = (dsp_print_buf_addr + 256); //512K -256 for each msg, totally have 2047 msgs.

	if (*dsp_inited_flag != 0x12345678)
	{
		*lock = 0;
		*tx_idx = 0;
		*write_finish = 1;
		*(write_finish+1) = 1;
		*(write_finish+2) = 1;
		*(write_finish+3) = 1;
		*dsp_inited_flag = 0x12345678;
	}
    #endif
	return 0;
}
static void vSendToLinux(char * content,int len)
{
#if LOG_TO_LINUX_UART
	unsigned long id = xGetSelfCoreId();
	unsigned int v, tx_idx_save;

	if (*dsp_inited_flag != 0x12345678)
	{
		return;
	}

	vUartAcquireLock(lock);
	v = *tx_idx;
	tx_idx_save = v;
	v++;
	if (v > TOTAL_LOG_NUM)
		v = 0;
	*tx_idx = v;
	vUartReleaseLock(lock);
	*(write_finish + id) = 0;

	memset(dspmsg + LENGTH_FOR_EACH_LOG * tx_idx_save, 0, LENGTH_FOR_EACH_LOG);
	memcpy(dspmsg + LENGTH_FOR_EACH_LOG * tx_idx_save, content, len >= (LENGTH_FOR_EACH_LOG-1) ? (LENGTH_FOR_EACH_LOG-1) : len);
	*(write_finish + id) = 1;
#endif
}

void vSerialPutc(char c)
{
	//apb_out(UART_DAT,c);
	volatile unsigned int *ptr=(unsigned int*)UART_DAT;
	*ptr=c;
}

void vSerialPuts(char *s)
{
#if LOCK_PRINT
	vSpinLock(&dsp_print_lock);
#endif

	volatile unsigned int *ptr=(unsigned int*)UART_DAT;
	while (*s)
	{
		vBlockInstructuon(10000);
		*ptr=*s++;
	}

#if LOCK_PRINT
	vSpinUnlock(&dsp_print_lock);
#endif
}

//For some historical reason, there are two functions debug_msg and debug_print, they are fucking same! they're both used in other libs, so we just leave it as it is.

int xDebugMsg(const char *format,...)
{
	va_list args;
	char input_buf1[PRINT_BUF_LEN];
	int core_id = xGetSelfCoreId();

	va_start(args, format);
	memset(input_buf1, 0x0,sizeof(input_buf1));
	switch(core_id)
	{
		case (0):
		{
			memcpy(input_buf1,"[core 0]", sizeof("[core 0]"));
			break;
		}
		case (1):
		{
			memcpy(input_buf1,"[core 1]", sizeof("[core 1]"));
			break;
		}
		case (2):
		{
			memcpy(input_buf1,"[core 2]", sizeof("[core 2]"));
			break;
		}
		case (3):
		{
			memcpy(input_buf1,"[core 3]", sizeof("[core 3]"));
			break;
		}
		default:
		{

		}
	}
	vsprintf(input_buf1+strlen(input_buf1),format, args);
#if LOG_TO_CEVA_UART
#if LOCK_PRINT
	vSpinLock(&dsp_print_lock);
#endif
	vSerialPuts(input_buf1);
	vSerialPuts("\r\n");
#if LOCK_PRINT
	vSpinUnlock(&dsp_print_lock);
#endif
#endif

	va_end(args);
	vSendToLinux(input_buf1, strlen(input_buf1));

	return 0;
}


int xDebugPrint(const char *format,...)
{
	va_list args;
	char input_buf1[PRINT_BUF_LEN];
	int core_id = xGetSelfCoreId();

	va_start(args, format);
	memset(input_buf1, 0x0, sizeof(input_buf1));
	switch(core_id)
	{
		case (0):
		{
			memcpy(input_buf1,"[core 0]", sizeof("[core 0]"));
			break;
		}
		case (1):
		{
			memcpy(input_buf1,"[core 1]", sizeof("[core 1]"));
			break;
		}
		case (2):
		{
			memcpy(input_buf1,"[core 2]", sizeof("[core 2]"));
			break;
		}
		case (3):
		{
			memcpy(input_buf1,"[core 3]", sizeof("[core 3]"));
			break;
		}
		default:
		{

		}
	}
	vsprintf(input_buf1+strlen(input_buf1),format, args);

#if LOG_TO_CEVA_UART
#if LOCK_PRINT
		spin_lock(&dsp_print_lock);
#endif
		vSerialPuts(input_buf1);
		vSerialPuts("\r\n");
#if LOCK_PRINT
		vSpinUnlock(&dsp_print_lock);
#endif
#endif

	va_end(args);
	vSendToLinux(input_buf1, strlen(input_buf1));

	return 0;
}

