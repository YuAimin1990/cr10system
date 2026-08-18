#ifndef __IPC_GPIO_H__
#define __IPC_GPIO_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void IPC_ADC_Read(int * value);

void IPC_GPIO_SysRestoreHandle();
void IPC_GPIO_RedLedOn();
void IPC_GPIO_RedLedOff();
void IPC_GPIO_BlueLedOn();
void IPC_GPIO_BlueLedOff();
void IPC_GPIO_GreenLedOn();
void IPC_GPIO_GreenLedOff();
void IPC_GPIO_InfraOn();
void IPC_GPIO_InfraOff();

int IPC_GPIO_GetRedLedStatus();
int IPC_GPIO_GetBlueLedStatus();
int IPC_GPIO_GetGreenLedStatus();
int IPC_GPIO_GetInfraStatus();

void IPC_GPIO_DisableLed();
void IPC_GPIO_EnableLed();

void IPC_GPIO_LedOn();
void IPC_GPIO_LedOff();

int IPC_GPIO_Init();
void IPC_GPIO_UnInit();

#ifdef __cplusplus
}
#endif

#endif /* _GK_ZBAR_H__ */


