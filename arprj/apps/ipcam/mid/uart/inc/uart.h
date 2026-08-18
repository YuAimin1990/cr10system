#ifndef __MID_UART_H__  
#define __MID_UART_H__  

#ifdef __cplusplus
extern "C"
{
#endif

AR_S32 IPC_MID_UART_Open(const AR_CHAR *devicePath);  
AR_S32 IPC_MID_UART_Config(AR_S32 fd, AR_S32 nSpeed, AR_S32 nBits, AR_CHAR nEvent, AR_S32 nStop);  //(fd, 115200, 8, 'n', 1)
AR_S32 IPC_MID_UART_Receive(AR_S32 fd, AR_UCHAR *buf, AR_S32 size);  
AR_S32 IPC_MID_UART_Send(AR_S32 fd, AR_UCHAR *buf, AR_S32 DataLength);  
AR_S32 IPC_MID_UART_Close(AR_S32 fd);

#ifdef __cplusplus
}
#endif

#endif 

