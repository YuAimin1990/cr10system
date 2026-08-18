/**
 * @file hal_type.h
 * @brief  hal 基础数据类型定义
 * @author Artosyn Software Team
 * @version 0.0.1
 * @date 2021/04/21
 * @license 2021-2025, Artosyn. Co., Ltd.
**/

#ifndef __HAL_TYPE_H__
#define __HAL_TYPE_H__
#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/*----------------------------------------------*
 * The common data type, will be used in the whole project.*
 *----------------------------------------------*/
#ifndef AR_U8
typedef unsigned char           AR_U8;
#endif
#ifndef AR_U16
typedef unsigned short          AR_U16;
#endif
#ifndef AR_U32
typedef unsigned int            AR_U32;
#endif
#ifndef AR_S8
typedef signed char             AR_S8;
#endif
#ifndef AR_S16
typedef short                   AR_S16;
#endif
#ifndef AR_S32
typedef int                     AR_S32;
#endif
#ifndef AR_FLOAT
typedef float                   AR_FLOAT;
#endif
#ifndef AR_DOUBLE
typedef double                  AR_DOUBLE;
#endif
#ifndef AR_U64
typedef unsigned long long      AR_U64;
#endif
#ifndef AR_S64
typedef long long               AR_S64;
#endif
#ifndef AR_CHAR
typedef char                    AR_CHAR;
#endif
#ifndef AR_UCHAR
typedef unsigned char           AR_UCHAR;
#endif
#ifndef AR_HANDLE
typedef unsigned int            AR_HANDLE;
#endif

#ifndef AR_VOID
#define AR_VOID                 void
#endif

#if 0
//on 64bit platform
#ifndef AR_INTPTR
typedef intptr_t     AR_INTPTR;
#endif
#ifndef AR_UINTPTR
typedef uintptr_t    AR_UINTPTR;
#endif
#endif
/*----------------------------------------------*
 * const defination                             *
 *----------------------------------------------*/
typedef enum {
    AR_FALSE = 0,
    AR_TRUE  = 1,
} AR_BOOL;


#ifndef NULL
    #define NULL        0L
#endif

#define AR_NULL         0L
#define AR_SUCCESS      0
#define AR_FAILURE      (-1)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __AR_TYPE_H__ */

