#ifndef __PARSE_PRA_H__
#define __PARSE_PRA_H__

#include "hal_vo.h"
#include "cfg_vo.h"

typedef struct
{
    ENUM_AR_HAL_VO_DPI_FORMAT           dev_dpi_format;              // DPI输出位宽
    ENUM_AR_HAL_VO_DPI_COLOR_MODE       dev_color_mode;              // 输出格式
    ENUM_AR_HAL_VO_SYNC_MODE            dev_sync_mode;               // 同步模式
    AR_S32                              dev_pin_mode;                // 输出线序
    STRU_AR_HAL_VO_SYNC_INFO            dev_sync_info;               // 客户定制时序
    AR_S32                              layer_luma_stride;         // y步幅
    AR_S32                              layer_chroma_stride;       // uv步幅
    ENUM_AR_HAL_VO_ROTATION             layer_rotation;
    AR_S32                              layer_ch_map;
} CFG_VO_EXT_S;

void print_senior_usage(const char *prog);

void parse_paras(int argc, char *argv[]);

void print_all_paras();

int is_use_senior_paras(int argc, char *argv[]);

int is_help(int argc, char *argv[], char *help, char *h);

void init_ext_paras();

CFG_VO_EXT_S *get_vo_ext_paras();

#endif
