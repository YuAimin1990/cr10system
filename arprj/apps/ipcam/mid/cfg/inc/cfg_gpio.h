

#ifndef __CFG_GPIO_H__
#define __CFG_GPIO_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "cfg_common.h"


typedef struct {
    AR_S32 enable;
    AR_CHAR gpio_name[8];
    AR_S32 gpio_value;
} KEY_T;

typedef struct {
    AR_S32 enable;
    AR_CHAR gpio_name[8];
    AR_S32 gpio_value;
} LED_T;

typedef struct {
    AR_S32 enable;
    AR_CHAR fbc_gpio_name[8];
    AR_CHAR enable_gpio_name[8];
} IRCUT_T;

typedef struct {
    //AR_S32 mode;       //0: use num, 1: use group_index
    KEY_T rec;
    LED_T led; //The gpio to control led board
    IRCUT_T ircut; //ircut ctrl gpios
} IPC_CFG_GPIO_S;


extern AR_S32 IPC_CFG_GPIO_Save();
extern AR_S32 IPC_CFG_GPIO_Load(CFG_RUN_MODE_E enMode);
extern AR_S32 IPC_CFG_GPIO_LoadDefault();
extern void IPC_CFG_GPIO_Print();
extern IPC_CFG_GPIO_S * IPC_CFG_GPIO_GetParam();

#define GPIO_CFG_FILE "cfg_gpio.json"


#ifdef __cplusplus
}
#endif
#endif

