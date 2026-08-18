#include "vo/display_hal/display_hal.h"
#include "vo/ar_display.h"
#include "vo/util.h"

const STRU_DISP_RES disp_res_tbl[AR_HAL_VO_OUTPUT_USER] = {
    {720, 576, 25, AR_SYSTEM_INTERLACE_MOD_720_576I_PAL},  /* PAL */
    {720, 480, 30, AR_SYSTEM_INTERLACE_MOD_720_480I_NTSC}, /* NTSC */

    {1920, 1080, 24, AR_SYSTEM_INTERLACE_MOD_NULL},  /* 1920 x 1080 at 24 Hz. */
    {1920, 1080, 25, AR_SYSTEM_INTERLACE_MOD_NULL},  /* 1920 x 1080 at 25 Hz. */
    {1920, 1080, 30, AR_SYSTEM_INTERLACE_MOD_NULL},  /* 1920 x 1080 at 30 Hz. */
    {1920, 1080, 50, AR_SYSTEM_INTERLACE_MOD_NULL},  /* 1920 x 1080 at 50 Hz. */
    {1920, 1080, 60, AR_SYSTEM_INTERLACE_MOD_NULL},  /* 1920 x 1080 at 60 Hz. */
    {1920, 1080, 48, AR_SYSTEM_INTERLACE_MOD_1080I}, /* 1920 x 1080I at 48 Hz. */
    {1920, 1080, 50, AR_SYSTEM_INTERLACE_MOD_1080I}, /* 1920 x 1080I at 50 Hz. */
    {1920, 1080, 60, AR_SYSTEM_INTERLACE_MOD_1080I}, /* 1920 x 1080I at 60 Hz. */

    {1280, 720, 25, AR_SYSTEM_INTERLACE_MOD_NULL}, /* 1280 x  720 at 25 Hz. */
    {1280, 720, 30, AR_SYSTEM_INTERLACE_MOD_NULL}, /* 1280 x  720 at 30 Hz. */
    {1280, 720, 50, AR_SYSTEM_INTERLACE_MOD_NULL}, /* 1280 x  720 at 50 Hz. */
    {1280, 720, 60, AR_SYSTEM_INTERLACE_MOD_NULL}, /* 1280 x  720 at 60 Hz. */

    {720, 576, 50, AR_SYSTEM_INTERLACE_MOD_NULL},  /* 720  x  576 at 50 Hz. */
    {720, 480, 60, AR_SYSTEM_INTERLACE_MOD_NULL},  /* 720  x  480 at 60 Hz. */
    {400, 960, 60, AR_SYSTEM_INTERLACE_MOD_NULL},  /* 400  x  960 at 60 Hz. */
    {720, 1440, 60, AR_SYSTEM_INTERLACE_MOD_NULL}, /* 720  x  1440 at 60 Hz. */

    {800, 600, 60, AR_SYSTEM_INTERLACE_MOD_NULL},   /* VESA 800 x 600 at 60 Hz */
    {1024, 600, 60, AR_SYSTEM_INTERLACE_MOD_NULL},  /* 1024 x 600 at 60 Hz */
    {1280, 1024, 60, AR_SYSTEM_INTERLACE_MOD_NULL}, /* VESA 1280 x 1024 at 60 Hz*/
    {1280, 768, 60, AR_SYSTEM_INTERLACE_MOD_NULL},  /* 1280*768@60Hz VGA@60Hz*/
    {640, 480, 60, AR_SYSTEM_INTERLACE_MOD_NULL},   /* VESA 640 x 480 at 60 Hz*/
    {3840, 2160, 60, AR_SYSTEM_INTERLACE_MOD_NULL}, /* 3840x2160_60 */
    {800, 480, 60, AR_SYSTEM_INTERLACE_MOD_NULL}    /* 1920 x 1080 at 60 Hz. */
};

const STRU_DISP_RES *display_hal_get_res_tbl(ENUM_AR_HAL_VO_DEV_TIMING_TEMPLATE e_timing)
{
    return (e_timing < AR_HAL_VO_OUTPUT_USER) ? &disp_res_tbl[e_timing] : NULL;
}

int ar_display_hal_csc_trans(ENUM_AR_HAL_VO_CSC vo_csc)
{
    int ar_csc = AR_VIDEO_MATRIX_NODATA_LIMIT;

    switch (vo_csc)
    {
    case AR_HAL_VO_CSC_BT709_LIMIT:
        ar_csc = AR_VIDEO_MATRIX_BT709_LIMIT;
        break;

    case AR_HAL_VO_CSC_BT709_FULL:
        ar_csc = AR_VIDEO_MATRIX_BT709_LIMIT;
        break;

    case AR_HAL_VO_CSC_BT601_LIMIT:
        ar_csc = AR_VIDEO_MATRIX_BT601_LIMIT;
        break;

    case AR_HAL_VO_CSC_BT601_FULL:
        ar_csc = AR_VIDEO_MATRIX_BT601_FULL;
        break;

    default:
        break;
    }

    return ar_csc;
}

int ar_display_hal_format_trans(ENUM_AR_HAL_VO_FMT vo_csc)
{
    int ar_format = Input_YV12;

    switch (vo_csc)
    {
    case AR_HAL_VO_FMT_ARGB1555:
        ar_format = Input_ARGB1555;
        break;

    case AR_HAL_VO_FMT_RGB565:
        ar_format = Input_RGB565;
        break;

    case AR_HAL_VO_FMT_ARGB8888:
        ar_format = Input_ARGB8888;
        break;

    case AR_HAL_VO_FMT_YV12:
        ar_format = Input_YV12;
        break;

    default:
        break;
    }

    return ar_format;
}
