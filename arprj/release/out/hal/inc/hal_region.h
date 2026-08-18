#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "osal.h"
#include "hal_errno.h"
#include "hal_ge2d.h"
#include "hal_dbglog.h"

#ifndef _HAL_REGION_H_
#define _HAL_REGION_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define AR_HAL_REGION_DEV_NAME                 "ar_region"
#define AR_HAL_MOD_REGION                      HAL_TAG_ID(AR_SYS_ID_RGN)

#define MAX_REGION_NUM                         1024 //64
#define MAX_PANNEL_NUM                         3
#define MAX_REGION_BUFFER                      2
#define MAX_REGION_REG_MOD_NUM                 5
#define REGION_ALIGN_BYTES                     256
#define REGION_COLOR_LUT_NUM                   2

typedef enum
{
    RGN_ERR_INPUT_PARA               =   0x40,
    RGN_ERR_RGN_ID,
    RGN_ERR_HAS_CREATED,
    RGN_ERR_HAS_DESTORYED,
    RGN_ERR_BUFFER_QUEUE_FULL,
    RGN_ERR_BUFFER_QUEUE_EMPTY,
    RGN_ERR_FORMAT_NOT_MATCH,
    RGN_ERR_GE2D_OPERATION,
    RGN_ERR_HAS_ATTACHED,
    RGN_ERR_HAS_DETACHED,
    RGN_ERR_NO_MEM,
    RGN_ERR_OPEN_DEV,
    RGN_ERR_CLOSE_DEV,
    RGN_ERR_RGN_TYPE,
    RGN_ERR_GET_ATTR,
    RGN_ERR_GET_DESTORY,
    RGN_ERR_COVER_TYPE                = 0x50,
    RGN_ERR_MOD,
    RGN_ERR_PIXEL_FORMAT,
    RGN_ERR_HAS_REGISTED,
    RGN_ERR_NOT_REGIST,
    RGN_ERR_REGIST_FULL,
    RGN_ERR_ATTACH,
    RGN_ERR_DETACH,
    RGN_ERR_CHAN_NOT_FOUND,
}ENUM_REGION_ERRNO;

#define  HAL_ERR_RGN_INPUT_PARA                     AR_HAL_DEF_ERR(AR_SYS_ID_RGN, HAL_ERR_LEVEL_ERROR, RGN_ERR_INPUT_PARA)
#define  HAL_ERR_RGN_ID                             AR_HAL_DEF_ERR(AR_SYS_ID_RGN, HAL_ERR_LEVEL_ERROR, RGN_ERR_RGN_ID)
#define  HAL_ERR_RGN_HAS_CREATED                    AR_HAL_DEF_ERR(AR_SYS_ID_RGN, HAL_ERR_LEVEL_ERROR, RGN_ERR_HAS_CREATED)
#define  HAL_ERR_RGN_HAS_DESTORYED                  AR_HAL_DEF_ERR(AR_SYS_ID_RGN, HAL_ERR_LEVEL_ERROR, RGN_ERR_HAS_DESTORYED)
#define  HAL_ERR_RGN_BUFFER_QUEUE_FULL              AR_HAL_DEF_ERR(AR_SYS_ID_RGN, HAL_ERR_LEVEL_ERROR, RGN_ERR_BUFFER_QUEUE_FULL)
#define  HAL_ERR_RGN_BUFFER_QUEUE_EMPTY             AR_HAL_DEF_ERR(AR_SYS_ID_RGN, HAL_ERR_LEVEL_ERROR, RGN_ERR_BUFFER_QUEUE_EMPTY)
#define  HAL_ERR_RGN_GE2D_OPERATION                 AR_HAL_DEF_ERR(AR_SYS_ID_RGN, HAL_ERR_LEVEL_ERROR, RGN_ERR_GE2D_OPERATION)
#define  HAL_ERR_RGN_HAS_ATTACHED                   AR_HAL_DEF_ERR(AR_SYS_ID_RGN, HAL_ERR_LEVEL_ERROR, RGN_ERR_HAS_ATTACHED)
#define  HAL_ERR_RGN_HAS_DETACHED                   AR_HAL_DEF_ERR(AR_SYS_ID_RGN, HAL_ERR_LEVEL_ERROR, RGN_ERR_HAS_DETACHED)
#define  HAL_ERR_RGN_NO_MEM                         AR_HAL_DEF_ERR(AR_SYS_ID_RGN, HAL_ERR_LEVEL_ERROR, RGN_ERR_NO_MEM)
#define  HAL_ERR_RGN_OPEN_DEV                       AR_HAL_DEF_ERR(AR_SYS_ID_RGN, HAL_ERR_LEVEL_ERROR, RGN_ERR_OPEN_DEV)
#define  HAL_ERR_RGN_CLOSE_DEV                      AR_HAL_DEF_ERR(AR_SYS_ID_RGN, HAL_ERR_LEVEL_ERROR, RGN_ERR_CLOSE_DEV)
#define  HAL_ERR_RGN_TYPE                           AR_HAL_DEF_ERR(AR_SYS_ID_RGN, HAL_ERR_LEVEL_ERROR, RGN_ERR_RGN_TYPE)
#define  HAL_ERR_RGN_GET_ATTR                       AR_HAL_DEF_ERR(AR_SYS_ID_RGN, HAL_ERR_LEVEL_ERROR, RGN_ERR_GET_ATTR)
#define  HAL_ERR_RGN_DESTORY                        AR_HAL_DEF_ERR(AR_SYS_ID_RGN, HAL_ERR_LEVEL_ERROR, RGN_ERR_GET_DESTORY)
#define  HAL_ERR_RGN_COVER_TYPE                     AR_HAL_DEF_ERR(AR_SYS_ID_RGN, HAL_ERR_LEVEL_ERROR, RGN_ERR_COVER_TYPE)
#define  HAL_ERR_RGN_MOD                            AR_HAL_DEF_ERR(AR_SYS_ID_RGN, HAL_ERR_LEVEL_ERROR, RGN_ERR_MOD)
#define  HAL_ERR_RGN_PIXEL_FORMAT                   AR_HAL_DEF_ERR(AR_SYS_ID_RGN, HAL_ERR_LEVEL_ERROR, RGN_ERR_PIXEL_FORMAT)
#define  HAL_ERR_RGN_HAS_REGISTED                   AR_HAL_DEF_ERR(AR_SYS_ID_RGN, HAL_ERR_LEVEL_ERROR, RGN_ERR_HAS_REGISTED)
#define  HAL_ERR_RGN_NOT_REGIST                     AR_HAL_DEF_ERR(AR_SYS_ID_RGN, HAL_ERR_LEVEL_ERROR, RGN_ERR_NOT_REGIST)
#define  HAL_ERR_RGN_REGIST_FULL                    AR_HAL_DEF_ERR(AR_SYS_ID_RGN, HAL_ERR_LEVEL_ERROR, RGN_ERR_REGIST_FULL)
#define  HAL_ERR_RGN_ATTACH                         AR_HAL_DEF_ERR(AR_SYS_ID_RGN, HAL_ERR_LEVEL_ERROR, RGN_ERR_ATTACH)
#define  HAL_ERR_RGN_DETACH                         AR_HAL_DEF_ERR(AR_SYS_ID_RGN, HAL_ERR_LEVEL_ERROR, RGN_ERR_DETACH)
#define  HAL_ERR_RGN_CHAN_NOT_FOUND                 AR_HAL_DEF_ERR(AR_SYS_ID_RGN, HAL_ERR_LEVEL_ERROR, RGN_ERR_CHAN_NOT_FOUND)

typedef enum
{
    RGN_TYPE_OVERLAY = 0,
    RGN_TYPE_COVER,
    RGN_TYPE_MOSAIC,
    RGN_TYPE_MAX
} ENUM_REGION_TYPE;

typedef enum
{
    RGN_AREA_RECT = 0,
    RGN_AREA_QUAD_RANGLE,
    RGN_AREA_MAX
} ENUM_RGN_AREA_TYPE;

typedef struct
{
    ENMU_SYS_MOD_ID   mod_id;
    AR_U32            dev_id;
    AR_U32            chn_id;
}STRU_AR_RGN_CHN_ID;

typedef struct
{
    AR_S32  x;
    AR_S32  y;
} STRU_POINT;

typedef struct
{
    AR_U32  width;
    AR_U32  height;
} STRU_SIZE;

typedef struct
{
    AR_S32  x;
    AR_S32  y;
    AR_U32  width;
    AR_U32  height;
} STRU_RECT;

typedef struct
{
    AR_BOOL    solid;            /* whether solid or dashed quadrangle */
    AR_U32     line_width;       /* Line Width of quadrangle, valid when dashed quadrangle */
    STRU_POINT point[4];         /* points of quadrilateral */
} STRU_QUADRANGLE;

typedef struct
{
    AR_VOID   *vrt_addr;          // 虚拟地址，对齐后
    AR_VOID   *vrt_addr_orign;    // 虚拟地址，对齐前，用于释放
    AR_VOID   *phy_addr;          // 物理地址，对齐后
    AR_VOID   *phy_addr_orign;    // 物理地址，对齐前，用于释放
    AR_U32     length;            // 数据内容长度
    AR_U32     stride;            // 数据内容步长
}STRU_AR_PANNEL    ;

typedef struct
{
    ENUM_GE2D_FOMART  e_format;
    ENUM_GE2D_CSC     e_csc_mode;
    AR_U32            width;
    AR_U32            height;
    AR_U32            pannel_num;
    STRU_AR_PANNEL    pannel[MAX_PANNEL_NUM];
}STRU_AR_CANVAS    ;

typedef struct
{
    ENUM_GE2D_FOMART  e_format;
    AR_U32            backgound_color;
    STRU_SIZE         size;
    AR_U32            buffer_num;
    STRU_AR_CANVAS    buffer[MAX_REGION_BUFFER];
    AR_BOOL           buffer_cached;
}STRU_OVERLAY_ATTR;

typedef union
{
    STRU_OVERLAY_ATTR overlay;
}STRU_REGION_ATTR_U;

typedef struct
{
    ENUM_REGION_TYPE    type;
    STRU_REGION_ATTR_U  attr;
}STRU_REGION_ATTR;

typedef struct
{
    AR_BOOL        abs_qp;
    AR_S32         qp;
    AR_BOOL        qp_disable;
}STRU_OVERLAY_QP_INFO;

typedef enum
{
    INVERT_COLOR_MODE_FULL = 0,
    INVERT_COLOR_MODE_USER_DEF,
    INVERT_COLOR_MODE_MAX
}STRU_INVERT_COLOR_MODE;

typedef struct
{
    STRU_SIZE               inv_col_area;
    AR_U32                  lum_thresh;
    STRU_INVERT_COLOR_MODE  inv_mod;
    AR_BOOL                 inv_col_en;
}STRU_OVERLAY_INVERT_COLOR;

typedef struct
{
    STRU_POINT                 point;
    AR_U32                     fg_alpha;  // ARGB1555 Alpha
    AR_U32                     bg_alpha;  // ARGB1555 Alpha
    AR_U32                     layer;     // priority
    STRU_OVERLAY_QP_INFO       qp_info;
    STRU_OVERLAY_INVERT_COLOR  inv_color;
    AR_U16                     color_lut[REGION_COLOR_LUT_NUM];
}STRU_OVERLAY_CHN_ATTR;

typedef struct
{
    ENUM_RGN_AREA_TYPE      cover_type;
    union
    {
        STRU_RECT           rect;             /* config of rect */
        STRU_QUADRANGLE     quad_rangle;      /* config of arbitary quadrilateral COVER */
    };
    AR_U32     color;
    AR_U32     layer;            // priority
}STRU_COVER_CHN_ATTR;

typedef enum
{
    MOSAIC_BLK_LEVEL_0 = 0,
    MOSAIC_BLK_LEVEL_1,
    MOSAIC_BLK_LEVEL_2,
    MOSAIC_BLK_LEVEL_3,
    MOSAIC_BLK_LEVEL_4,
    MOSAIC_BLK_LEVEL_5,
    MOSAIC_BLK_LEVEL_6,
    MOSAIC_BLK_LEVEL_7,
    MOSAIC_BLK_LEVEL_MAX
}ENUM_MOSAIC_BLK_LEVEL;

typedef struct
{
    STRU_RECT              rect;
    ENUM_MOSAIC_BLK_LEVEL  blk_level;
    AR_U32                 layer;
}STRU_MOSAIC_CHN_ATTR;

typedef union
{
    STRU_OVERLAY_CHN_ATTR      overlay_chn;      /* attribute of overlay region */
    STRU_COVER_CHN_ATTR        cover_chn;        /* attribute of cover region */
    STRU_MOSAIC_CHN_ATTR       mosaic_chn;       /* attribute of mosic region */
} STRU_RGN_CHN_ATTR_U;

/* attribute of a region */
typedef struct
{
    AR_BOOL              is_show;
    ENUM_REGION_TYPE     rgn_type;     /* region type */
    STRU_RGN_CHN_ATTR_U  chn_attr;     /* region attribute */
} STRU_RGN_CHN_ATTR;

typedef struct
{
    AR_BOOL            copy_buf_enable;
}STRU_AR_RGN_CHN_PARA;

typedef AR_S32 (*FUNC_RGN_DRAW_CB)(STRU_AR_RGN_CHN_ID *chn_id, STRU_AR_CANVAS *canvas);

typedef struct {
    ENMU_SYS_MOD_ID e_mod_id;
    AR_S32 (*attach_call_back)(FUNC_RGN_DRAW_CB cb);
    AR_S32 (*detach_call_back)(void);
} STRU_RGN_REGISTER_CB;

typedef struct {
    STRU_RGN_REGISTER_CB   call_back[MAX_REGION_REG_MOD_NUM];
    AR_U32                 ref[MAX_REGION_REG_MOD_NUM];
} STRU_RGN_MNG;


AR_S32 ar_hal_region_open_dev(void);
AR_S32 ar_hal_region_close_dev(AR_S32 fd);
AR_S32 ar_hal_region_create(AR_S32 fd, AR_U32 rgn_id, STRU_REGION_ATTR *attr);
AR_S32 ar_hal_region_destory(AR_S32 fd, AR_U32 rgn_id);
AR_S32 ar_hal_region_get_attr(AR_S32 fd, AR_U32 rgn_id, STRU_REGION_ATTR *attr);
AR_S32 ar_hal_region_set_attr(AR_S32 fd, AR_U32 rgn_id, STRU_REGION_ATTR *attr);
AR_S32 ar_hal_region_set_bitmap(AR_S32 fd, AR_U32 rgn_id, STRU_AR_CANVAS *bmp);
AR_S32 ar_hal_region_attach_to_chn(AR_S32 fd, AR_U32 rgn_id, STRU_AR_RGN_CHN_ID *chn_id,
                                            STRU_RGN_CHN_ATTR *attr);
AR_S32 ar_hal_region_detach_from_chn(AR_S32 fd, AR_U32 rgn_id, STRU_AR_RGN_CHN_ID *chn_id);
AR_S32 ar_hal_region_set_display_attr(AR_S32 fd, AR_U32 rgn_id, STRU_AR_RGN_CHN_ID *chn_id,
                                                STRU_RGN_CHN_ATTR *attr);
AR_S32 ar_hal_region_get_display_attr(AR_S32 fd, AR_U32 rgn_id, STRU_AR_RGN_CHN_ID *chn_id,
                                                STRU_RGN_CHN_ATTR *attr);
AR_S32 ar_hal_region_get_canvas_info(AR_S32 fd, AR_U32 rgn_id, STRU_AR_CANVAS *canvas);
AR_S32 ar_hal_region_update_canvas(AR_S32 fd, AR_U32 rgn_id);
AR_S32 ar_hal_region_draw_chn(AR_S32 fd, STRU_AR_RGN_CHN_ID *chn_id, STRU_AR_CANVAS *canvas,
                                        AR_U64 rgn_mask);

AR_S32 ar_hal_region_register_cb(STRU_RGN_REGISTER_CB          *cb);
AR_S32 ar_hal_region_unregister_cb(ENMU_SYS_MOD_ID e_mod_id);

#if 0
AR_S32 ar_hal_region_get_current_canva(AR_S32 fd, AR_U32 rgn_id, STRU_AR_RGN_CHN_ID *chn_id,
                                                 STRU_AR_CANVAS *canvas);
#endif
AR_S32 ar_hal_region_set_chn_para(AR_S32 fd, STRU_AR_RGN_CHN_ID *chn_id,
                                                STRU_AR_RGN_CHN_PARA *para);
AR_S32 ar_hal_region_get_chn_para(AR_S32 fd, STRU_AR_RGN_CHN_ID *chn_id,
                                                STRU_AR_RGN_CHN_PARA *para);
AR_S32 ar_hal_region_suspend(AR_S32 fd);
AR_S32 ar_hal_region_resume(AR_S32 fd);

#ifdef __cplusplus
}
#endif

#endif

