/**
 * @file hal_ge2d_rpc.h
 * @brief  ge2d rpc implement
 * @author Artosyn Software Team
 * @version 0.0.1
 * @date 2021/07/14
 * @license 2021-2025, Artosyn. Co., Ltd.
**/
#ifndef __HAL_GE2D_RPC__H__
#define __HAL_GE2D_RPC__H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


#include "hal_ge2d.h"

typedef enum
{
    GE2D_RPC_ID_SET_CLIENT_PARA           = 1,
    GE2D_RPC_ID_MALLOC_FRAME,
    GE2D_RPC_ID_FREE_FRAME,
    GE2D_RPC_ID_PROCESS_FRAME,
    GE2D_RPC_ID_PROCESS_FRAME_EX,
    GE2D_RPC_ID_ADD_START,
    GE2D_RPC_ID_ADD_START_EX,
    GE2D_RPC_ID_ADD_END,
    GE2D_RPC_ID_ADD_LINE_START,
    GE2D_RPC_ID_APPEND_LINE               = 10,
    GE2D_RPC_ID_ADD_LINE_END,
    GE2D_RPC_ID_ADD_BMP,
    GE2D_RPC_ID_ADD_ROTATION,
    GE2D_RPC_ID_ADD_MOSAIC,
    GE2D_RPC_ID_ADD_COLOR_KEY,
    GE2D_RPC_ID_ADD_STRETCH_BLIT,
    GE2D_RPC_ID_ADD_DOT_MATRIX,
    GE2D_RPC_ID_ADD_REGION_STATS,
    GE2D_RPC_ID_GET_REGION_STATS          = 19,
}ENUM_GE2D_RPC_ID;

typedef struct
{
    /* input */
    AR_S32  frame_id;
    STRU_GE2D_START_FRAME_PARA  addr;
    STRU_GE2D_CLIENT_PARA       para;
}GE2D_RPC_PROCESS_FRAME_EX;

typedef struct
{
    /* input */
    AR_S32  frame_id;
    STRU_GE2D_START_FRAME_PARA  para;
}GE2D_RPC_ADD_START_FRAME;

typedef struct
{
    /* input */
    AR_S32  frame_id;
    STRU_GE2D_LINE_PARA  para;
}GE2D_RPC_ADD_LINE_START;

typedef struct
{
    /* input */
    AR_S32  frame_id;
    STRU_GE2D_LINE_APPEND_PARA  para;
}GE2D_RPC_APPEND_LINE;

typedef struct
{
    /* input */
    AR_S32  frame_id;
    STRU_GE2D_BMP_PARA  para;
}GE2D_RPC_ADD_BMP;

typedef struct
{
    /* input */
    AR_S32  frame_id;
    STRU_GE2D_ROTATION_PARA  para;
}GE2D_RPC_ADD_ROTATION;

typedef struct
{
    /* input */
    AR_S32  frame_id;
    STRU_GE2D_MOSAIC_PARA  para;
}GE2D_RPC_ADD_MOSAIC;

typedef struct
{
    /* input */
    AR_S32  frame_id;
    STRU_GE2D_COLOR_KEY_PARA  para;
}GE2D_RPC_ADD_COLOR_KEY;

typedef struct
{
    /* input */
    AR_S32  frame_id;
    STRU_GE2D_STRETCH_BLIT_PARA  para;
}GE2D_RPC_STRETCH_BLIT;

typedef struct
{
    /* input */
    AR_S32  frame_id;
    STRU_GE2D_DOT_MAT_PARA  para;
}GE2D_RPC_DOT_MAT;

typedef struct
{
    /* input */
    AR_S32  frame_id;
    STRU_GE2D_REGION_PARA  para;
}GE2D_RPC_ADD_REGION_STATS;

#if 0
typedef struct
{
    /* input */
    AR_S32  frame_id;
    STRU_GE2D_REGION_STATS  result;
}GE2D_RPC_GET_REGION_STATS;
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif

