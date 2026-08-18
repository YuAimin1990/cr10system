#ifndef __AR_ISP_DEBUG_H__
#define __AR_ISP_DEBUG_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#include "ar_debug.h"

#if 0
#define PRINT_DEBUG_INFO
#endif

#if 0
#define PRINT_INFO_2FILE
#endif

#define ISP_TRACE(level, fmt, ...)                                \
do{                                                               \
    switch(level)                                                 \
    {                                                             \
    case AR_DBG_EMERG:  ar_err(fmt, ##__VA_ARGS__);               \
        break;                                                    \
    case AR_DBG_ALERT:  ar_err(fmt, ##__VA_ARGS__);               \
        break;                                                    \
    case AR_DBG_CRIT :  ar_err(fmt, ##__VA_ARGS__);               \
        break;                                                    \
    case AR_DBG_ERR  :  ar_err(fmt, ##__VA_ARGS__);               \
        break;                                                    \
    case AR_DBG_WARN :  ar_warning(fmt, ##__VA_ARGS__);           \
        break;                                                    \
    case AR_DBG_NOTICE: ar_always(fmt, ##__VA_ARGS__);            \
        break;                                                    \
    case AR_DBG_INFO :  ar_info(fmt, ##__VA_ARGS__);              \
        break;                                                    \
    case AR_DBG_DEBUG:  ar_debug(fmt, ##__VA_ARGS__);             \
        break;                                                    \
    default:                                                      \
        break;                                                    \
    }                                                             \
}while(0)

/* To avoid divide-0 exception in code. */
#define DIV_0_TO_1(a)   ( (0 == (a)) ? 1 : (a) )
#define DIV_0_TO_1_FLOAT(a) ((((a) < 1E-10) && ((a) > -1E-10)) ? 1 : (a))

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif     /* __AR_ISP_DEBUG_H__ */
