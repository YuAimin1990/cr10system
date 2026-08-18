#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "hal_sys.h"
#include "mpi_sysctl.h"
#include "hal_vo.h"

typedef struct
{
    AR_S32                   chan_id;
    AR_S32                      chan_fd;
	STRU_AR_HAL_VO_RECT      pos;
}STRU_SURFACE_OBJ_T;

typedef struct
{
    STRU_SURFACE_OBJ_T       surface[24];
	//dislpay_obj
    ENUM_AR_HAL_VO_DEV_ID    e_dev_id;
    ENUM_AR_HAL_VO_LAYER_ID  e_layer_id;
    AR_S32                      dev_fd;
    AR_S32                      layer_fd;
    AR_S32 width;
    AR_S32 height;
    AR_S32 y_stride;
    AR_S32 uv_stride;
	AR_S32 cf50;
	AR_S32 cf50_mode;
	AR_S32 format;
}STRU_DISPLAY_OBJ_T;


int ar_vo_dev_init_for_mipi_720_1440(STRU_DISPLAY_OBJ_T *vo_obj, int width, int height, int y_stride,
    int uv_stride, float fps, AR_BOOL is_mipi_tx, AR_BOOL is_interlace);
int ar_vo_dev_init_for_mipi(STRU_DISPLAY_OBJ_T *vo_obj, int width, int height, int y_stride,
    int uv_stride, float fps, AR_BOOL is_mipi_tx, AR_BOOL is_interlace);

AR_S32 ar_vo_dev_init(STRU_DISPLAY_OBJ_T *vo_obj,
    AR_S32 width,
    AR_S32 height,
    AR_S32 y_stride,
    AR_S32 uv_stride);
AR_S32 ar_vo_surface_init(STRU_DISPLAY_OBJ_T *vo_obj, AR_S32 ch,
    STRU_AR_HAL_VO_RECT *chan_pos);

AR_S32 close_display_surface(STRU_DISPLAY_OBJ_T *vo_obj, AR_S32 ch);
AR_S32 close_display(STRU_DISPLAY_OBJ_T *vo_obj);

AR_S32 send_to_fw_display(STRU_DISPLAY_OBJ_T *p_display, AR_S32 ch, VIDEO_FRAME_INFO_S *p_frame);

#ifdef __cplusplus
}
#endif
