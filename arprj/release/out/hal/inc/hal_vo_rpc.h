/**
 * @file hal_vo_rpc.h
 * @brief  vo rpc implement
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


#include "hal_vo.h"

typedef enum
{
    VO_RPC_ID_DEV_ENABLE             = 1,
    VO_RPC_ID_DEV_DISABLE,
    VO_RPC_ID_DEV_SET_ATTR,
    VO_RPC_ID_DEV_GET_ATTR,
    VO_RPC_ID_DEV_SET_BACKGROUND,
    VO_RPC_ID_DEV_SET_CSC,
    VO_RPC_ID_DEV_GET_CSC,
    VO_RPC_ID_DEV_CFG_CURSOR,
    VO_RPC_ID_DEV_ENABLE_CURSOR,
    VO_RPC_ID_DEV_DISABLE_CURSOR,
    VO_RPC_ID_DEV_SET_CURSOR_POS,
    VO_RPC_ID_DEV_GET_CURSOR_POS,
    VO_RPC_ID_DEV_DSI_SET_ATTR,
    VO_RPC_ID_DEV_DSI_CMD,
    VO_RPC_ID_DEV_DSI_EXIT_CMD,
    VO_RPC_ID_DEV_DSI_READ,
    VO_RPC_ID_DEV_SUSPEND,
    VO_RPC_ID_DEV_RESUME,
    VO_RPC_ID_DEV_SUB_REG,
    VO_RPC_ID_DEV_GET_SUB_INFO,
    VO_RPC_ID_DEV_SUB_CLEAR,
    VO_RPC_ID_DEV_SET_LOWDELAY,
    VO_RPC_ID_DEV_GET_LOWDELAY,
    VO_RPC_ID_DEV_RESET_SKEWING_HIST,
    VO_RPC_ID_DEV_GET_SKEWING_HIST,

    VO_RPC_ID_LAYER_INIT             = 1000,
    VO_RPC_ID_LAYER_ENABLE,
    VO_RPC_ID_LAYER_DISABLE,
    VO_RPC_ID_LAYER_SET_ATTR,
    VO_RPC_ID_LAYER_GET_ATTR,
    VO_RPC_ID_LAYER_SET_CSC,
    VO_RPC_ID_LAYER_GET_CSC,
    VO_RPC_ID_LAYER_GET_BUF,
    VO_RPC_ID_LAYER_RELEASE_BUF,
    VO_RPC_ID_LAYER_SET_POS,
    VO_RPC_ID_LAYER_GET_POS,

    VO_RPC_ID_CHN_ENABLE             = 2000,
    VO_RPC_ID_CHN_DISABLE,
    VO_RPC_ID_CHN_SET_ATTR,
    VO_RPC_ID_CHN_GET_ATTR,
    VO_RPC_ID_CHN_SET_PARA,
    VO_RPC_ID_CHN_GET_PARA,
    VO_RPC_ID_CHN_SET_POS,
    VO_RPC_ID_CHN_GET_POS,
    VO_RPC_ID_CHN_RELEASE_FRAME,
    VO_RPC_ID_CHN_SEND_FRAME,
    VO_RPC_ID_CHN_GET_FRAME,
    VO_RPC_ID_CHN_WAIT_FRAME_DONE,
    VO_RPC_ID_CHN_PAUSE,
    VO_RPC_ID_CHN_RESUME,
    VO_RPC_ID_CHN_STEP,
    VO_RPC_ID_CHN_SHOW,
    VO_RPC_ID_CHN_HIDE,
    VO_RPC_ID_CHN_CLEAR_FRAME
}ENUM_GE2D_RPC_ID;

typedef struct
{
    /* input */
    ENUM_AR_HAL_VO_DEV_ID  e_dev_id;
}STRU_VO_RPC_DEV_ENABLE;

typedef struct
{
    /* input */
    ENUM_AR_HAL_VO_DEV_ID  e_dev_id;
}STRU_VO_RPC_DEV_DISABLE;

typedef struct
{
    /* input */
    ENUM_AR_HAL_VO_DEV_ID     e_dev_id;
    STRU_AR_HAL_VO_DEV_ATTR   attr;
}STRU_VO_RPC_DEV_SET_ATTR;

typedef struct
{
    /* input */
    ENUM_AR_HAL_VO_DEV_ID     e_dev_id;

    /* output */
    STRU_AR_HAL_VO_DEV_ATTR   attr;
}STRU_VO_RPC_DEV_GET_ATTR;

typedef struct
{
    /* input */
    ENUM_AR_HAL_VO_DEV_ID     e_dev_id;
    ENUM_AR_HAL_VO_IMAGE      e_image;
}STRU_VO_RPC_DEV_SET_BACKGROUND;

typedef struct
{
    /* input */
    ENUM_AR_HAL_VO_DEV_ID  e_dev_id;
    ENUM_AR_HAL_VO_CSC     e_csc;
}STRU_VO_RPC_DEV_SET_CSC;

typedef struct
{
    /* input */
    ENUM_AR_HAL_VO_DEV_ID  e_dev_id;

    /* output */
    ENUM_AR_HAL_VO_CSC     e_csc;
}STRU_VO_RPC_DEV_GET_CSC;

typedef struct
{
    /* input */
    AR_CHAR                file_name[MAX_VO_CURSOR_FILE_NAME];
}STRU_VO_RPC_DEV_CURSOR_FILE;

typedef struct
{
    /* input */
    ENUM_AR_HAL_VO_DEV_ID  e_dev_id;
}STRU_VO_RPC_DEV_ENABLE_CURSOR;

typedef struct
{
    /* input */
    ENUM_AR_HAL_VO_DEV_ID  e_dev_id;
}STRU_VO_RPC_DEV_DISABLE_CURSOR;

typedef struct
{
    /* input */
    ENUM_AR_HAL_VO_DEV_ID  e_dev_id;
    STRU_AR_HAL_VO_POS     position;
}STRU_VO_RPC_DEV_SET_CURSOR_POS;

typedef struct
{
    /* input */
    ENUM_AR_HAL_VO_DEV_ID  e_dev_id;

    /* output */
    STRU_AR_HAL_VO_POS     position;
}STRU_VO_RPC_DEV_GET_CURSOR_POS;


typedef struct
{
    /* input */
    ENUM_AR_HAL_VO_LAYER_ID  e_layer_id;
}STRU_VO_RPC_LAYER_INIT;

typedef struct
{
    /* input */
    ENUM_AR_HAL_VO_LAYER_ID  e_layer_id;
}STRU_VO_RPC_LAYER_ENABLE;

typedef struct
{
    /* input */
    ENUM_AR_HAL_VO_LAYER_ID  e_layer_id;
}STRU_VO_RPC_LAYER_DISABLE;

typedef struct
{
    /* input */
    ENUM_AR_HAL_VO_LAYER_ID    e_layer_id;
    STRU_AR_HAL_VO_LAYER_ATTR  attr;
}STRU_VO_RPC_LAYER_SET_ATTR;

typedef struct
{
    /* input */
    ENUM_AR_HAL_VO_LAYER_ID    e_layer_id;

    /* output */
    STRU_AR_HAL_VO_LAYER_ATTR  attr;
}STRU_VO_RPC_LAYER_GET_ATTR;

typedef struct
{
    /* input */
    ENUM_AR_HAL_VO_LAYER_ID  e_layer_id;
    ENUM_AR_HAL_VO_CSC       e_csc;
}STRU_VO_RPC_LAYER_SET_CSC;

typedef struct
{
    /* input */
    ENUM_AR_HAL_VO_LAYER_ID  e_layer_id;

    /* output */
    ENUM_AR_HAL_VO_CSC       e_csc;
}STRU_VO_RPC_LAYER_GET_CSC;

typedef struct
{
    /* input */
    AR_S32                   milli_sec;

    /* output */
    STRU_AR_HAL_VO_DISP_BUF  buffer;
}STRU_VO_RPC_LAYER_GET_BUF;

typedef struct
{
    /* input */
    AR_S32                   milli_sec;
    STRU_AR_HAL_VO_DISP_BUF  buffer;
}STRU_VO_RPC_LAYER_RELEASE_BUF;

typedef struct
{
    /* input */
    ENUM_AR_HAL_VO_LAYER_ID  e_layer_id;
    STRU_AR_HAL_VO_POS       pos;
}STRU_VO_RPC_LAYER_SET_POS;

typedef struct
{
    /* input */
    ENUM_AR_HAL_VO_LAYER_ID  e_layer_id;
    AR_U32                   chan_id;
}STRU_VO_RPC_CHN_ENABLE;

typedef struct
{
    /* input */
    ENUM_AR_HAL_VO_LAYER_ID  e_layer_id;
    AR_U32                   chan_id;
}STRU_VO_RPC_CHN_DISABLE;

typedef struct
{
    /* input */
    ENUM_AR_HAL_VO_LAYER_ID  e_layer_id;
    AR_U32                   chan_id;
    STRU_AR_HAL_VO_CHN_ATTR  attr;
}STRU_VO_RPC_CHN_SET_ATTR;

typedef struct
{
    /* input */
    ENUM_AR_HAL_VO_LAYER_ID  e_layer_id;
    AR_U32                   chan_id;

    /* output */
    STRU_AR_HAL_VO_CHN_ATTR  attr;
}STRU_VO_RPC_CHN_GET_ATTR;

typedef struct
{
    /* input */
    ENUM_AR_HAL_VO_LAYER_ID  e_layer_id;
    AR_U32                   chan_id;
    STRU_AR_VO_CHN_PARAM_S   para;
}STRU_VO_RPC_CHN_SET_PARA;

typedef struct
{
    /* input */
    ENUM_AR_HAL_VO_LAYER_ID  e_layer_id;
    AR_U32                   chan_id;

    /* output */
    STRU_AR_VO_CHN_PARAM_S   para;
}STRU_VO_RPC_CHN_GET_PARA;

typedef struct
{
    /* input */
    ENUM_AR_HAL_VO_LAYER_ID  e_layer_id;
    AR_U32                   chan_id;
    STRU_AR_HAL_VO_RECT      position;
}STRU_VO_RPC_CHN_SET_POSITION;

typedef struct
{
    /* input */
    ENUM_AR_HAL_VO_LAYER_ID  e_layer_id;
    AR_U32                   chan_id;

    /* output */
    STRU_AR_HAL_VO_RECT      position;
}STRU_VO_RPC_CHN_GET_POSITION;

typedef struct
{
    /* input */
    ENUM_AR_HAL_VO_LAYER_ID  e_layer_id;
    AR_U32                   chan_id;

    /* output */
    STRU_AR_HAL_VO_DISP_BUF  buffer;
}STRU_VO_RPC_CHN_CAPTURE_BUF;

typedef struct
{
    /* input */
    STRU_AR_HAL_VO_DISP_BUF  buffer;
}STRU_VO_RPC_CHN_RELEASE_BUF;

typedef struct
{
    /* input */
    AR_S32                   milli_sec;
    STRU_AR_HAL_VO_DISP_BUF  buffer;
}STRU_VO_RPC_CHN_ENQUEUE;

typedef struct
{
    /* input */
    AR_S32                   milli_sec;

    /* output */
    STRU_AR_HAL_VO_DISP_BUF  buffer;
}STRU_VO_RPC_CHN_DEQUEUE;

typedef struct
{
    /* input */
    ENUM_AR_HAL_VO_LAYER_ID  e_layer_id;
    AR_U32                   chan_id;
}STRU_VO_RPC_CHN_WAIT_FRAME_DONE;

typedef struct
{
    /* input */
    ENUM_AR_HAL_VO_LAYER_ID  e_layer_id;
    AR_U32                   chan_id;
}STRU_VO_RPC_CHN_PAUSE;

typedef struct
{
    /* input */
    ENUM_AR_HAL_VO_LAYER_ID  e_layer_id;
    AR_U32                   chan_id;
}STRU_VO_RPC_CHN_RESUME;

typedef struct
{
    /* input */
    ENUM_AR_HAL_VO_LAYER_ID  e_layer_id;
    AR_U32                   chan_id;
}STRU_VO_RPC_CHN_STEP;

typedef struct
{
    /* input */
    ENUM_AR_HAL_VO_LAYER_ID  e_layer_id;
    AR_U32                   chan_id;
}STRU_VO_RPC_CHN_SHOW;

typedef struct
{
    /* input */
    ENUM_AR_HAL_VO_LAYER_ID  e_layer_id;
    AR_U32                   chan_id;
}STRU_VO_RPC_CHN_HIDE;

typedef struct
{
    AR_U32               dev_no;
    AR_U16               data_type;
    AR_U16               cmd_size;
    AR_U8                cmd[0];
}STRU_VO_DSI_RPC_CMD;

typedef struct
{
    AR_U32               dev_no;
    AR_U16               data_type;
    AR_U16               data_size;
    AR_U32               para;
    AR_U8                data[0];
}STRU_VO_DSI_RPC_READ;

typedef struct
{
    /* input */
    AR_U32               dev_no;
    AR_U32               count;
    /* output */
    AR_U8                data[0];
}STRU_VO_GET_SKEWING_HISTOGRAM;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif

