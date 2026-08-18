/****************************************************************************
 * Copyright (C) 2019 Shanghai Artosyn Microelectronics Limited.            *
 ****************************************************************************/

/** \addtogroup bsp 
 *  @{
 */

/**
 * @file ar_wdt.h
 * @author Artosyn
 * @date 9 May 2019
 * @brief artosyn watchdog control API for start, stop, set watchdog timeout, and feed watchdog.
 */


#ifndef _AR_WATCHDOG_H_
#define _AR_WATCHDOG_H_

#ifdef __cplusplus
extern "C" {
#endif


/**
* @brief watchdog start. if not feed watchdog in time, watchdog will reset system.
* @return 0	 ok.
* 		  <0 error.
*/
int ar_hal_wdt_start(void);


/**
* @brief stop watchdog.
* @return 0	 ok
* 		  <0 error
*/
int ar_hal_wdt_stop(void);


/**
* @brief get current watchdog timeout setting.
* @param  timeout: [out] pointer to be filled with current watchdog timeout value setting, timeunit: second
* @return 0	 ok
* 		  <0 error
*/
int ar_hal_wdt_get_timeout_threshhold(int *timeout);



/**
* @brief set watchdog timeout setting.
* @param  timeout: [in, out] the pointer of expecting timeout setting for watchdog; 
*         and if ok, store the actual watchdog timeout settting value.
* @return 0	 ok
* 		  <0 error
*/
int ar_hal_wdt_set_timeout_threshhold(int *timeout);


/**
* @brief feed watchdog, watchdog timeout recount from zero.
* @return 0	 ok
* 		  <0 error
*/
int ar_hal_wdt_feed(void);



/**
* @brief force watchdog reset immediately
* @return 0	 ok
* 		  <0 error
*/
//int ar_hal_wdt_reset(void);


#ifdef __cplusplus
}
#endif

#endif // _AR_WATCHDOG_H_
/** @}*/
