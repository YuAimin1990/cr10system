/****************************************************************************
 * Copyright (C) 2019 Shanghai Artosyn Microelectronics Limited.            *
 ****************************************************************************/
/** \addtogroup bsp
 *  @{
 */

/**
 * @file ar_gpio.h
 * @author Artosyn
 * @date 9 May 2019
 * @brief File containing the APIs to do gpio operation.
 *        Users can refer to app/artosyn/usr_test/test_gpio/test_gpio.c for sample code, or contact Artosyn for help.
 * @example <app/artosyn/usr_test/test_gpio/test_gpio.c>
 */


#ifndef __SIRIUS_LIBGPIO_H__
#define __SIRIUS_LIBGPIO_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */



/**
* @brief  gpio port enum
*/
#ifdef PROXIMA
typedef enum
{
	PORT_A = 0,
	PORT_B,
	PORT_C,
	PORT_D,
	PORT_E,
	PORT_F,
	PORT_G,
	PORT_MAX
}ENUM_GPIO_PORT;

typedef enum
{
	GROUP_0 = 0,
	GROUP_1,
	GROUP_2,
	GROUP_3,
	GROUP_MAX
}ENUM_GPIO_GROUP;
#else
typedef enum
{
	PORT_A = 0,
	PORT_B,
	PORT_C,
	PORT_D,
	PORT_MAX
}ENUM_GPIO_PORT;

typedef enum
{
	GROUP_0 = 0,
	GROUP_1,
	GROUP_2,
	GROUP_3,
	GROUP_MAX
}ENUM_GPIO_GROUP;
#endif
typedef enum
{
	INTPUT = 0,
	OUTPUT = 1,
	DIR_MAX
}ENUM_GPIO_DIR;

/**
* @brief  initialize  gpio component
* @param  gpio gpio component
* @return 0	ok.
* @note echo gpio_num > sys/class/gpio/export.
*/
int ar_hal_gpio_export(unsigned int gpio);

/**
* @brief  uninitialize  gpio component
* @param  gpio gpio component
* @return 0	ok.
* @note echo gpio_num > sys/class/gpio/unexport.
*/
int ar_hal_gpio_unexport(unsigned int gpio);

/**
* @brief  set gpio direction
* @param  gpio gpio component
* @param  dir 1 output 0 input
* @return 0	ok.
* @note echo in/out > sys/class/gpio/gpioXXX/direction.
*/
int ar_hal_gpio_set_dir(unsigned int gpio, ENUM_GPIO_DIR dir);

/**
* @brief  get gpio direction
* @param  gpio gpio component
* @param  dir 1 output 0 input
* @return 0	ok.
* @note echo in/out > sys/class/gpio/gpioXXX/direction.
*/
int ar_hal_gpio_get_dir(unsigned int gpio, ENUM_GPIO_DIR *dir);

/**
* @brief  set gpio high or low
* @param  gpio gpio component
* @param  value  0 set gpio low
				 1 set gpio high
* @return 0	ok.
* @note echo 1/0 > sys/class/gpio/gpioXXX/value.
*/
int ar_hal_gpio_set_value(unsigned int gpio, unsigned int value);

/**
* @brief  get gpio value status
* @param  gpio gpio component
* @param  value  return gpio status
* @return 0	ok.
* @note cat sys/class/gpio/gpioXXX/value.
*/
int ar_hal_gpio_get_value(unsigned int gpio, unsigned int *value);

/**
* @brief  Write these strings to select the signal edge
* @param  gpio gpio component
* @param  edge  "rising", "falling"
* @return 0	ok.
* @note echo  "rising", "falling" > sys/class/gpio/gpioXXX/edge.
*/
int ar_hal_gpio_set_edge(unsigned int gpio, char *edge);


/**
* @brief  convert gpio gourp, port, pin to gpio component
* @param  group gpio group
* @param  port gpio port
* @param  num gpio pin
* @return gpio component.
* @note group0-4, port0-3, pin0-7
*/
int ar_hal_gpio_name_to_num(ENUM_GPIO_GROUP group, ENUM_GPIO_PORT port, unsigned int num);

#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif /* End of #ifdef __cplusplus */


#endif //__SIRIUS_LIBGPIO_H__
/** @}*/
