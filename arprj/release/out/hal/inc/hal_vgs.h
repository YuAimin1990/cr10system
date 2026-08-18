#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "osal.h"
#include "hal_errno.h"
#include "hal_ge2d.h"
#include "hal_region.h"
#include "hal_dbglog.h"

#ifndef _HAL_VGS_H_
#define _HAL_VGS_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


#define AR_HAL_VGS_DEV_NAME                   "ar_vgs"
#define AR_HAL_MOD_VGS                        HAL_TAG_ID(AR_SYS_ID_VGS)

#define MAX_VGS_JOB_NUM                         400
#define MAX_VGS_TASK_NUM                        800

#define MAX_VGS_TASK_LINE_NUM                   100
#define MAX_VGS_TASK_COVER_NUM                  100
#define MAX_VGS_TASK_OSD_NUM                    30
#define MAX_VGS_TASK_LUMA_NUM                   100


typedef enum
{
    VGS_ERR_INPUT_PARA                    =   0x40,
    VGS_ERR_NOT_READY,
    VGS_ERR_GE2D_CLIENT,
    VGS_ERR_HAS_STARTED,
    VGS_ERR_NOT_START,
    VGS_ERR_QUEUE_CREATE,
    VGS_ERR_SIGNAL_CREATE,
    VGS_ERR_NO_MEM,
    VGS_ERR_JOB_ALLOC,
    VGS_ERR_JOB_NOT_FOUND,
    VGS_ERR_TASK_ALLOC,
    VGS_ERR_JOB_ENQUEUE,
    VGS_ERR_PIXEL_FORMAT,

    VGS_ERR_GE2D_MALLOC_FRAME,
}ENUM_VGS_ERRNO;


#define  HAL_ERR_VGS_INPUT_PARA                     AR_HAL_DEF_ERR(AR_SYS_ID_VGS, HAL_ERR_LEVEL_ERROR, VGS_ERR_INPUT_PARA)
#define  HAL_ERR_VGS_NOT_READY                      AR_HAL_DEF_ERR(AR_SYS_ID_VGS, HAL_ERR_LEVEL_ERROR, VGS_ERR_NOT_READY)
#define  HAL_ERR_VGS_GE2D_CLIENT                    AR_HAL_DEF_ERR(AR_SYS_ID_VGS, HAL_ERR_LEVEL_ERROR, VGS_ERR_GE2D_CLIENT)
#define  HAL_ERR_VGS_HAS_STARTED                    AR_HAL_DEF_ERR(AR_SYS_ID_VGS, HAL_ERR_LEVEL_ERROR, VGS_ERR_HAS_STARTED)
#define  HAL_ERR_VGS_NOT_START                      AR_HAL_DEF_ERR(AR_SYS_ID_VGS, HAL_ERR_LEVEL_ERROR, VGS_ERR_NOT_START)
#define  HAL_ERR_VGS_QUEUE_CREATE                   AR_HAL_DEF_ERR(AR_SYS_ID_VGS, HAL_ERR_LEVEL_ERROR, VGS_ERR_QUEUE_CREATE)
#define  HAL_ERR_VGS_SIGNAL_CREATE                  AR_HAL_DEF_ERR(AR_SYS_ID_VGS, HAL_ERR_LEVEL_ERROR, VGS_ERR_SIGNAL_CREATE)
#define  HAL_ERR_VGS_NO_MEM                         AR_HAL_DEF_ERR(AR_SYS_ID_VGS, HAL_ERR_LEVEL_ERROR, VGS_ERR_NO_MEM)
#define  HAL_ERR_VGS_JOB_ALLOC                      AR_HAL_DEF_ERR(AR_SYS_ID_VGS, HAL_ERR_LEVEL_ERROR, VGS_ERR_JOB_ALLOC)
#define  HAL_ERR_VGS_JOB_NOT_FOUND                  AR_HAL_DEF_ERR(AR_SYS_ID_VGS, HAL_ERR_LEVEL_ERROR, VGS_ERR_JOB_NOT_FOUND)
#define  HAL_ERR_VGS_TASK_ALLOC                     AR_HAL_DEF_ERR(AR_SYS_ID_VGS, HAL_ERR_LEVEL_ERROR, VGS_ERR_TASK_ALLOC)
#define  HAL_ERR_VGS_JOB_ENQUEUE                    AR_HAL_DEF_ERR(AR_SYS_ID_VGS, HAL_ERR_LEVEL_ERROR, VGS_ERR_JOB_ENQUEUE)
#define  HAL_ERR_VGS_PIXEL_FORMAT                   AR_HAL_DEF_ERR(AR_SYS_ID_VGS, HAL_ERR_LEVEL_ERROR, VGS_ERR_PIXEL_FORMAT)
#define  HAL_ERR_VGS_GE2D_MALLOC_FRAME              AR_HAL_DEF_ERR(AR_SYS_ID_VGS, HAL_ERR_LEVEL_ERROR, VGS_ERR_GE2D_MALLOC_FRAME)


typedef enum
{
    VGS_TASK_TYPE_NONE  = 0,
    VGS_TASK_TYPE_SCALE,
    VGS_TASK_TYPE_DRAW_LINE,
    VGS_TASK_TYPE_COVER,
    VGS_TASK_TYPE_OSD,
    VGS_TASK_TYPE_ROTATION,
    VGS_TASK_TYPE_GET_LUMA,
    VGS_TASK_TYPE_MAX
} ENUM_VGS_TYPE_TYPE;

typedef enum
{
    VGS_COVER_RECT = 0,
    VGS_COVER_QUAD_RANGLE,
    VGS_COVER_MAX
} ENUM_VGS_COVER_TYPE;

typedef enum
{
    AR_ROTATION_0      = 0,
    AR_ROTATION_90     = 1,
    AR_ROTATION_180    = 2,
    AR_ROTATION_270    = 3,
    AR_ROTATION_MAX,
} ENUM_ROTATION;

typedef struct
{
    STRU_POINT        start_point;
    STRU_POINT        end_point;
    AR_U32            thick;
    AR_U32            color;            /* ARGB8888 */
}STRU_VGS_DRAW_LINE;

typedef struct
{
    ENUM_VGS_COVER_TYPE  cover_type;
    union
    {
        STRU_RECT        rect;
        STRU_QUADRANGLE  quad_rangle;
    };
    AR_U32            color;
} STRU_VGS_COVER;

typedef struct
{
   // STRU_RECT          rect;
    int                threshold;
}STRU_VGS_OSD_REVERT;

typedef struct
{
    STRU_RECT            rect;
    AR_U32               bg_color;
    ENUM_GE2D_FOMART     e_format;
    AR_U32               pannel_num;
    STRU_AR_PANNEL       pannel[MAX_PANNEL_NUM];
    AR_U32               bg_alpha;
    AR_U32               fg_alpha;
    AR_BOOL              need_revert;
    STRU_VGS_OSD_REVERT  revert_info;
    AR_U16               color_lut[2];
    AR_U32               close_alpha;        /**< ¹Ø±Õalpha»ìºÏ */
} STRU_VGS_OSD;

typedef struct
{
    STRU_AR_CANVAS       in;
    STRU_AR_CANVAS       out;
    AR_U64               priv[4];
    AR_U32               reserved;
}STRU_VGS_TASK_ATTR;


/*********************   rpc call *********************************/
typedef enum
{
    VGS_RPC_ID_START       = 0,
    VGS_RPC_ID_BEGIN_JOB,
    VGS_RPC_ID_ADD_SCALE_TASK,
    VGS_RPC_ID_ADD_DRAW_LINE_TASK,
    VGS_RPC_ID_ADD_DRAW_LINE_TASK_ARRAY,
    VGS_RPC_ID_ADD_COVER_TASK,
    VGS_RPC_ID_ADD_COVER_TASK_ARRAY,
    VGS_RPC_ID_ADD_OSD_TASK,
    VGS_RPC_ID_ADD_OSD_TASK_ARRAY,
    VGS_RPC_ID_ADD_ROTATION_TASK,
    VGS_RPC_ID_ADD_LUMA_TASK_ARRAY,
    VGS_RPC_ID_END_JOB,
    VGS_RPC_ID_CANCEL_JOB,
    VGS_RPC_ID_SUSPEND,
    VGS_RPC_ID_RESUME
} ENUM_VGS_RPC_ID;

typedef struct
{
    AR_U16  job_num;
    AR_U16  task_num;
}STRU_VGS_RPC_START;

typedef struct
{
    AR_U32              job_id;
    STRU_VGS_TASK_ATTR  task;
}STRU_VGS_RPC_ADD_SCALE_TASK;

typedef struct
{
    AR_U32              job_id;
    STRU_VGS_TASK_ATTR  task;
    STRU_VGS_DRAW_LINE  line;
}STRU_VGS_RPC_ADD_DRAW_LINE_TASK;

typedef struct
{
    AR_U32              job_id;
    STRU_VGS_TASK_ATTR  task;
    AR_U32              line_num;
    STRU_VGS_DRAW_LINE  line_array[0];
}STRU_VGS_RPC_ADD_DRAW_LINE_TASK_ARRAY;

typedef struct
{
    AR_U32              job_id;
    STRU_VGS_TASK_ATTR  task;
    STRU_VGS_COVER      cover;
}STRU_VGS_RPC_ADD_COVER_TASK;

typedef struct
{
    AR_U32              job_id;
    STRU_VGS_TASK_ATTR  task;
    AR_U32              cover_num;
    STRU_VGS_COVER      cover_array[0];
}STRU_VGS_RPC_ADD_COVER_TASK_ARRAY;

typedef struct
{
    AR_U32              job_id;
    STRU_VGS_TASK_ATTR  task;
    STRU_VGS_OSD        osd;
}STRU_VGS_RPC_ADD_OSD_TASK;

typedef struct
{
    AR_U32              job_id;
    STRU_VGS_TASK_ATTR  task;
    AR_U32              osd_num;
    STRU_VGS_OSD        osd_array[0];
}STRU_VGS_RPC_ADD_OSD_TASK_ARRAY;

typedef struct
{
    AR_U32              job_id;
    STRU_VGS_TASK_ATTR  task;
    ENUM_ROTATION       rotaion;
    AR_U32              close_alpha;
}STRU_VGS_RPC_ADD_ROTAION_TASK;

typedef struct
{
    AR_U32              job_id;
    STRU_VGS_TASK_ATTR  task;
    AR_U32              luma_num;
    AR_VOID            *luma_data;
    STRU_RECT           rect_array[0];
}STRU_VGS_RPC_ADD_LUMA_TASK_ARRAY;


AR_S32 ar_hal_vgs_open_dev(void);
AR_S32 ar_hal_vgs_close_dev(AR_S32 fd);
AR_S32 ar_hal_vgs_start(AR_S32 fd, STRU_VGS_RPC_START *start);
AR_S32 ar_hal_vgs_begin_job(AR_S32 fd, AR_U32 *job_id);
AR_S32 ar_hal_vgs_add_scale_task(AR_S32 fd, STRU_VGS_RPC_ADD_SCALE_TASK *scale);
AR_S32 ar_hal_vgs_add_draw_line_task(AR_S32 fd, STRU_VGS_RPC_ADD_DRAW_LINE_TASK *line);
AR_S32 ar_hal_vgs_add_draw_line_task_array(AR_S32 fd, STRU_VGS_RPC_ADD_DRAW_LINE_TASK_ARRAY *line);
AR_S32 ar_hal_vgs_add_cover_task(AR_S32 fd, STRU_VGS_RPC_ADD_COVER_TASK *cover);
AR_S32 ar_hal_vgs_add_cover_task_array(AR_S32 fd, STRU_VGS_RPC_ADD_COVER_TASK_ARRAY *cover);
AR_S32 ar_hal_vgs_add_osd_task(AR_S32 fd, STRU_VGS_RPC_ADD_OSD_TASK *osd);
AR_S32 ar_hal_vgs_add_osd_task_array(AR_S32 fd, STRU_VGS_RPC_ADD_OSD_TASK_ARRAY *osd);
AR_S32 ar_hal_vgs_add_rotation_task(AR_S32 fd, STRU_VGS_RPC_ADD_ROTAION_TASK *rotation);
AR_S32 ar_hal_vgs_end_job(AR_S32 fd, AR_U32 job_id);
AR_S32 ar_hal_vgs_cancel_job(AR_S32 fd, AR_U32 job_id);
AR_S32 ar_hal_vgs_suspend(AR_S32 fd);
AR_S32 ar_hal_vgs_resume(AR_S32 fd);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif

