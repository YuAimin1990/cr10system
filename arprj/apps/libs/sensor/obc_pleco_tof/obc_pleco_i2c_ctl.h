#ifndef __OBC_PLECO_I2C_CTL_H__
#define __OBC_PLECO_I2C_CTL_H__

#include <stdbool.h>










/**
* @brief  register synchronization
* @return int
*/
int pleco_group_hold(VI_PIPE ViPipe, bool enable);

/**
* @brief  pleco_set_integration_time 积分时间设置
* @param  [in] uint16_t integrationTime (us)
* @return int
*/
int pleco_set_integration_time(VI_PIPE ViPipe, uint16_t integrationTime);

/**
* @brief  pleco_get_integration_time 获取当前积分时间
* @param  [out] uint16_t *integrationTime (us)
* @return int
*/
int pleco_get_integration_time(VI_PIPE ViPipe, uint16_t *integrationTime);




















#endif


