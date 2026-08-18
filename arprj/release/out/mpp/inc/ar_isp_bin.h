
#ifndef __AR_ISP_BIN_H__
#define __AR_ISP_BIN_H__

#include "hal_type.h"
#include "ar_comm_isp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/****************************************************************************
 * MACRO DEFINITION                                                         *
 ****************************************************************************/
#define MAX_BIN_REG_NUM 5

/****************************************************************************
 * GENERAL STRUCTURES                                                       *
 ****************************************************************************/
typedef struct arISP_BIN_REG_ATTR_S
{
    AR_U32  u32Addr;    /* register addr */
    AR_U8   u8StartBit; /* start bit of register addr */
    AR_U8   u8EndBit;   /* end bit of register addr */
} ISP_BIN_REG_ATTR_S;

/*-------------------------------------------*
 The base addr of ISP logic register
 The base addr of ISP ext register
 The base addr of AE ext register
 The base addr of AWB ext register
*-------------------------------------------*/

ISP_BIN_REG_ATTR_S g_astIspBinRegAttr[ISP_MAX_PIPE_NUM][MAX_BIN_REG_NUM] =
{
    [0 ... (ISP_MAX_PIPE_NUM - 1)] = {0}
};


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /*__AR_ISP_BIN_H__ */
