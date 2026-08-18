#ifndef __PF_DRV_H__
#define __PF_DRV_H__


#ifdef __cplusplus
extern "C"
{
#endif
#include "hal_gpio.h"

typedef struct PF_RTC_TIME_
{
    AR_U32  year;
    AR_U32  month;
    AR_U32  date;
    AR_U32  hour;
    AR_U32  minute;
    AR_U32  second;
    AR_U32  weekday;
}PF_RTC_TIME_S;

AR_U32 IPC_PF_ReadReg(AR_U32 base, AR_U32 offset);
AR_S32 IPC_PF_WriteReg(AR_U32 base, AR_U32 offset, AR_U32 value);


void IPC_PF_SysShutdown();

AR_S32 PF_AOSetVol(AR_S32 vol);

AR_S32 IPC_PF_ADC_Init(void);
AR_S32 IPC_PF_ADC_UnInit(void);
AR_S32 IPC_PF_ADC_Read(AR_S32 *value);

AR_S32 IPC_PF_RTC_Init(void);
AR_S32 IPC_PF_RTC_UnInit(void);
AR_S32 IPC_PF_RTC_Read(PF_RTC_TIME_S *tm);
AR_S32 IPC_PF_RTC_Write(PF_RTC_TIME_S *tm);
AR_S32 IPC_PF_RTC_UpdateLocalTime();
AR_S32 IPC_PF_RTC_UpdateFromLocal();

AR_S32 IPC_PF_WDT_Init(void);
AR_S32 IPC_PF_WDT_UnInit(void);
AR_S32 IPC_PF_WDT_Enable(void);
AR_S32 IPC_PF_WDT_Disable(void);
AR_S32 IPC_PF_WDT_SetLiveTime(AR_S32 livetime_sec);
AR_S32 IPC_PF_WDT_KeepAlive(void);

AR_S32 IPC_PF_GPIO_SetValue(AR_S32 pin, AR_S32 value);
AR_S32 IPC_PF_GPIO_GetValue(AR_S32 pin, AR_S32 *value);

AR_S32 AR_GPIO_Trans(AR_CHAR * strGpio, ENUM_GPIO_GROUP * pGrp, ENUM_GPIO_PORT * pPort, AR_U32 * pNum);
AR_S32 IPC_PF_GPIO_SetByName(AR_CHAR * strName, ENUM_GPIO_DIR enDir, AR_U32 u32Value);
AR_S32 IPC_PF_GPIO_GetByName(AR_CHAR * strName, ENUM_GPIO_DIR * penDir, AR_U32 * pu32Value);

#ifdef __cplusplus
}
#endif
#endif
