/*
 * uart.h
 *
 *  Created on: Aug 2, 2016
 *      Author: cliu
 */


#ifndef UART_H_
#define UART_H_

#include "cpm_io.h"

#ifdef __cplusplus
extern "C"{
#endif


#include "RegisterAddr.h"
#include "SystemIO.h"
#include "SystemCommon.h"
#include "ThreadSynchronization.h"

#define UART_CFG	0x010C5000
#define UART_DAT	0x010C5004
#define UART_TRAN	0x010C5014


#define DEBUG_PRINT(fmrt, args...)     \
    do                                 \
    {                                  \
        if (1)                 \
        {                              \
            xDebugPrint(fmrt, ##args); \
        }                              \
    } while (0)


#define AR_ASSERT(expr) \
	if (!(expr)){ \
		xDebugMsg("\nASSERT failed at:\r\n"\
		"  >File name: %s\r\n"   \
		"  >Function : %s\r\n"   \
		"  >Line No. : %d\r\n"   \
		"  >Condition: %s\r\n", \
		__FILE__, __FUNCTION__, __LINE__, #expr); \
		while(1);\
	}

extern int xDebugPrint(const char *format,...);
extern void vSerialPuts(char *s);
extern int xDebugInit(char * buf_addr);
extern int xDebugMsg(const char *format,...);
#ifdef __cplusplus
}
#endif
#endif /* UART_H_ */

