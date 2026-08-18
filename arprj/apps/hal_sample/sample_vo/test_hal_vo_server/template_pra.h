#ifndef __TEMPLATE_PRA_H__
#define __TEMPLATE_PRA_H__

#include "hal_vo.h"

typedef enum
{
    DSI_PRA_DEFAULT,        /* */
}ENUM_DSI_PRA_TEMPLATE;

typedef enum
{
    LCD_GPIO_DEFAULT,        /* */
}ENUM_LCD_GPIO_TEMPLATE;

void set_dsi_config(ENUM_DSI_PRA_TEMPLATE dsi_pra_temp);

void lcd_gpio_init(ENUM_LCD_GPIO_TEMPLATE lcd_gpio);

STRU_AR_HAL_VO_DSI_ATTR* get_dsi_attr();

#endif
