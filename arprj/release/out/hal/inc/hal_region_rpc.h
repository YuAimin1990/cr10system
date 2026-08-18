/**
 * @file hal_region_rpc.h
 * @brief  region rpc implement
 * @author Artosyn Software Team
 * @version 0.0.1
 * @date 2021/07/14
 * @license 2021-2025, Artosyn. Co., Ltd.
**/
#ifndef __HAL_REGION_RPC__H__
#define __HAL_REGION_RPC__H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


#include "hal_region.h"

typedef enum
{
    REGION_RPC_ID_CREATE           = 1,
    REGION_RPC_ID_DESTROY,
    REGION_RPC_ID_GET_ATTR,
    REGION_RPC_ID_SET_ATTR,
    REGION_RPC_ID_SET_BITMAP,
    REGION_RPC_ID_ATTACH_TO_CHN,
    REGION_RPC_ID_DETACH_FROM_CHN,
    REGION_RPC_ID_SET_DISP_ATTR,
    REGION_RPC_ID_GET_DISP_ATTR,
    REGION_RPC_ID_GET_CANVAS_INFO,
    REGION_RPC_ID_GET_CURRENT_CANVA,
    REGION_RPC_ID_UPDATE_CANVAS,
    REGION_RPC_ID_BATCH_BEGIN,
    REGION_RPC_ID_BATCH_END,
    REGION_RPC_ID_DRAW_CHN,
    REGION_RPC_ID_SET_CHN_PARA,
    REGION_RPC_ID_GET_CHN_PARA,
    REGION_RPC_ID_SUSPEND,
    REGION_RPC_ID_RESUME,
}ENUM_GE2D_RPC_ID;

typedef struct
{
    AR_U32               rgn_id;
    STRU_REGION_ATTR     attr;
}STRU_RGN_RPC_CREATE;

typedef struct
{
    AR_U32               rgn_id;
    STRU_REGION_ATTR     attr;
}STRU_RGN_RPC_GET_ATTR;

typedef struct
{
    AR_U32               rgn_id;
    STRU_REGION_ATTR     attr;
}STRU_RGN_RPC_SET_ATTR;

typedef struct
{
    AR_U32               rgn_id;
    STRU_AR_CANVAS       canvas;
}STRU_RGN_RPC_SET_BMP;

typedef struct
{
    AR_U32               rgn_id;
    STRU_AR_RGN_CHN_ID   chn_id;
    STRU_RGN_CHN_ATTR    attr;
}STRU_RGN_RPC_ATTACH_TO_CHN;

typedef struct
{
    AR_U32               rgn_id;
    STRU_AR_RGN_CHN_ID   chn_id;
}STRU_RGN_RPC_DETACH_FROM_CHN;

typedef struct
{
    AR_U32               rgn_id;
    STRU_AR_RGN_CHN_ID   chn_id;
    STRU_RGN_CHN_ATTR    attr;
}STRU_RGN_RPC_SET_DISP_ATTR;

typedef struct
{
    AR_U32               rgn_id;
    STRU_AR_RGN_CHN_ID   chn_id;
    STRU_RGN_CHN_ATTR    attr;
}STRU_RGN_RPC_GET_DISP_ATTR;

typedef struct
{
    AR_U32               rgn_id;
    STRU_AR_CANVAS       canvas;
}STRU_RGN_RPC_GET_CANVAS_INFO;

typedef struct
{
    STRU_AR_RGN_CHN_ID   chn_id;
    STRU_AR_CANVAS       canvas;
    AR_U64               rgn_mask;
}STRU_RGN_RPC_DRAW_CHN;

typedef struct
{
    STRU_AR_RGN_CHN_ID     chn_id;
    STRU_AR_RGN_CHN_PARA   para;
}STRU_RGN_RPC_CHN_PARA;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif

