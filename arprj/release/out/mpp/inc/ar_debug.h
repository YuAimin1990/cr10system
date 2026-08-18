#ifndef __AR_DEBUG_H__
#define __AR_DEBUG_H__

#ifndef __KERNEL__
#include <stdio.h>
#include <stdarg.h>
#endif

#include "hal_type.h"
#include "ar_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define _EX__FILE_LINE(fxx,lxx) "[File]:"fxx"\n[Line]:"#lxx"\n[Info]:"
#define EX__FILE_LINE(fxx,lxx) _EX__FILE_LINE(fxx,lxx)
#define __FILE_LINE__ EX__FILE_LINE(__FILE__, __LINE__)

#define AR_DBG_EMERG      0   /* system is unusable                   */
#define AR_DBG_ALERT      1   /* action must be taken immediately     */
#define AR_DBG_CRIT       2   /* critical conditions                  */
#define AR_DBG_ERR        3   /* error conditions                     */
#define AR_DBG_WARN       4   /* warning conditions                   */
#define AR_DBG_NOTICE     5   /* normal but significant condition     */
#define AR_DBG_INFO       6   /* informational                        */
#define AR_DBG_DEBUG      7   /* debug-level messages                 */

typedef struct arLOG_LEVEL_CONF_S
{
    MOD_ID_E  enModId;
    AR_S32    s32Level;
    AR_CHAR   cModName[16];
} LOG_LEVEL_CONF_S;

#ifndef __KERNEL__
/******************************************************************************
** For User Mode : AR_PRINT, AR_ASSERT, AR_TRACE
******************************************************************************/

#define AR_PRINT printf

/* #ifdef AR_DEBUG */
#if 1
    /* Using samples:   AR_ASSERT(x>y); */
    #define AR_ASSERT(expr)               \
    do{                                   \
        if (!(expr)) {                    \
            printf("\nASSERT at:\n"\
                   "  >Function : %s\n"   \
                   "  >Line No. : %d\n"   \
                   "  >Condition: %s\n",  \
                   __FUNCTION__, __LINE__, #expr);\
            _exit(-1);\
        } \
    }while(0)

    /* Using samples:
    ** AR_TRACE(AR_DBG_DEBUG, AR_ID_CMPI, "Test %d, %s\n", 12, "Test");
    **/
    #define AR_TRACE(level, enModId, fmt...) fprintf(stderr,##fmt)
#else
    #define AR_ASSERT(expr)
    #define AR_TRACE(level, enModId, fmt...)
#endif

#else
/******************************************************************************
** For Linux Kernel : AR_PRINT, AR_ASSERT, AR_TRACE
******************************************************************************/
//#include "ar_osal.h"
#define AR_PRINT osal_printk

extern AR_S32 AR_ChkLogLevel(AR_S32 s32Levle, MOD_ID_E enModId);

int AR_LOG(AR_S32 level, MOD_ID_E enModId,const char *fmt, ...) __attribute__((format(printf,3,4)));

/* #ifdef AR_DEBUG */
#if 1
    /* Using samples:   AR_ASSERT(x>y); */
    #define AR_ASSERT(expr)               \
    do{                                   \
        if (!(expr)) {                    \
            osal_panic("\nASSERT at:\n" \
                  "  >Function : %s\n"    \
                  "  >Line No. : %d\n"    \
                  "  >Condition: %s\n",   \
                  __FUNCTION__, __LINE__, #expr);\
        } \
    }while(0)

    /* Using samples:
    ** AR_TRACE(AR_DBG_DEBUG, AR_ID_CMPI, "Test %d, %s\n", 12, "Test");
    **/
    #define AR_TRACE AR_LOG
#else
    #define AR_ASSERT(expr)
    #define AR_TRACE(level, enModId, fmt...)
#endif


#endif  /* end of __KERNEL__ */


#define AR_PRINT_BLOCK(pu8Datablock, u32Length)  \
{  \
    AR_U32 u32ii = 0;  \
    AR_U8* pu8VirAddr = (AR_U8*)(pu8Datablock);  \
    if(AR_NULL != pu8VirAddr)\
    {\
        AR_PRINT("\n[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, #pu8Datablock);  \
        for (u32ii = 0; u32ii < (u32Length);)  \
        {  \
            AR_PRINT(" %02X", *pu8VirAddr);\
            pu8VirAddr++;\
            u32ii++;\
            if(0 == (u32ii % 16))AR_PRINT("\n");\
        }  \
        AR_PRINT("\n\n");\
    }\
    else\
    {\
        AR_PRINT("\n[Func]:%s [Line]:%d [Info]:pointer(%s) is null!\n", __FUNCTION__, __LINE__, #pu8Datablock);  \
    }\
}




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __AR_DEBUG_H__ */

