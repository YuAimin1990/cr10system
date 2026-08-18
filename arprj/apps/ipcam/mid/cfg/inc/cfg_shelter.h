#ifndef __CFG_SHELTER_H__
#define __CFG_SHELTER_H__

#include "cfg_common.h"

#define MAX_SHELTER_RECT_PER_CHN  4
#define MAX_SHELTER_RECT_ITEM_NUM 7

/***********************************/
/***        shelter            ***/
/***********************************/
//遮挡区域设置
typedef struct {
    AR_S32    enable;     /* 遮挡使能 0-禁用, 1-使能 */
    AR_FLOAT    x;          /* 遮挡区域左上角的x坐标 */
    AR_FLOAT    y;          /* 遮挡区域左上角的y坐标 */
    AR_FLOAT    width;      /* 遮挡区域宽度 */
    AR_FLOAT    height;     /* 遮挡区域高度 */
    AR_U32    color;      /* 遮挡颜色, 默认 0:黑色 按RGB格式*/
} NET_SHELTER_RECT;

typedef struct
{
    NET_SHELTER_RECT stShelter[IPC_MAX_PIPELINE_NUM][MAX_SHELTER_RECT_PER_CHN];
} IPC_CFG_SHELTER_S;

AR_S32 IPC_CFG_SHELTER_Save();
AR_S32 IPC_CFG_SHELTER_Load();
void IPC_CFG_SHELTER_Print();
AR_S32 IPC_CFG_SHELTER_LoadDefault();
IPC_CFG_SHELTER_S * IPC_CFG_SHELTER_GetParam();

#define SHELTER_CFG_FILE "cfg_shelter.json"

#endif
