/**
 * @file hal_icc.h
 * @brief Inter Core Communication
 * @author Artosyn Software Team
 * @version 0.0.1
 * @date 2021/05/10
 * @license 2021-2025, Artosyn. Co., Ltd.
**/

#ifndef _HAL_ICC_H_
#define _HAL_ICC_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#include "hal_type.h"

enum {
	ICC_CORE_A_0,
	ICC_CORE_A_1,
	ICC_CORE_A_2,
	ICC_CORE_A_3,

	ICC_CORE_M_0,
	ICC_CORE_M_1,
	ICC_CORE_M_2,
	ICC_CORE_M_3,

	ICC_CORE_CEVA_0,
	ICC_CORE_CEVA_1,
	ICC_CORE_CEVA_2,
	ICC_CORE_CEVA_3,

	ICC_CORE_TOTAL,
};

/**
* @brief Open icc device node to create an icc client
* @param none
* @retval >=0  icc device fd
* @retval !0   fail
*/
AR_S32 ar_hal_icc_open_dev();

/**
* @brief Close icc device fd
* @param  fd icc device fd returned in ar_icc_open()
* @retval 0    success
* @retval !0   fail
*/
AR_S32 ar_hal_icc_close_dev(AR_S32 fd);

/**
* @brief Get a msgid from a string, the receiver and sender should use the SAME string as a token
* @param  path input string for hash
* @retval !0    message id generated from hased string
* @retval 0   fail
*/

AR_U16 ar_hal_icc_ftok(const AR_CHAR *pPath);

/**
* @brief Register a message id for the icc client to receive message
* @param  fd  icc client fd
* @param  u32MsgId message id that is going to be registered
* @retval 0    success
* @retval !0   fail
*/

AR_S32 ar_hal_icc_register_msgid(AR_S32 fd, AR_U32 u32MsgId);

/**
* @brief Unregister a message id for the icc client to to stop receiving message
* @param  fd  icc client fd
* @param  u32MsgId message id that is going to be unregistered
* @retval 0    success
* @retval !0   fail
*/

AR_S32 ar_hal_icc_unregister_msgid(AR_S32 fd, AR_U32 u32MsgId);

/**
* @brief Send a message throught the given icc client
* @param fd  icc client fd
* @param pBuf message buffer pointer
* @param u32Size message buffer size
* @param u32MsgId message id
* @param u32CoreId target core id
* @retval 0    success
* @retval !0   fail
*/

AR_S32 ar_hal_icc_send(AR_S32 fd, AR_UCHAR *pBuf, AR_U32 u32Size, AR_U32 u32MsgId, AR_U32 u32CoreId);

/**
* @brief Receive a message throught the given icc client
* @param fd  icc client fd
* @param pBuf message buffer pointer
* @param u32Size message buffer size
* @param u32MsgId message id
* @retval >0    received message size
* @retval <=0   fail
*/

AR_S32 ar_hal_icc_receive(AR_S32 fd, AR_UCHAR *pBuf, AR_U32 u32Size, AR_U32 u32MsgId);

/**
* @brief Receive a message throught the given icc client
* @param fd  icc client fd
* @param pBuf  message buffer pointer
* @param pSize message buffer size(input) and received message size(output)
* @param u32MsgId message id
* @param pCoreId core id from which the message is received
* @retval 0     success
* @retval <=0   fail
*/
AR_S32 ar_hal_icc_receive2(AR_S32 fd, AR_UCHAR *pBuf, AR_U32 *pSize, AR_U32 u32MsgId, AR_U32 *pCoreId);

/**
* @brief Set time out when receive message
* @param fd  icc client fd
* @param u32Timeout time out value in millisecond
* @retval 0        success
* @retval <=0   fail
*/

AR_S32 ar_hal_icc_set_timeout(AR_S32 fd, AR_U32 u32Timeout);

/**
* @brief Dump all icc token
* @param none
* @retval 0       success
* @retval <=0   fail
*/
void ar_hal_icc_dump_token_id(void);

#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif /* End of #ifdef __cplusplus */


#endif
/** @}*/
