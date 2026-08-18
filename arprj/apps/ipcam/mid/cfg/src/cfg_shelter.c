#include "cfg_shelter.h"
#include "cfg_common.h"

IPC_CFG_SHELTER_S g_stShelterCfg;

CFG_MAP shelterRectMap[IPC_MAX_PIPELINE_NUM][MAX_SHELTER_RECT_PER_CHN][MAX_SHELTER_RECT_ITEM_NUM] =
{
    //PIPELINE 0
    {
        //rect 0
        {
            {"rect_enable", &(g_stShelterCfg.stShelter[0][0].enable), CFG_DATA_TYPE_S32, 	"0", "rw", 0, 1,    NULL},
            {"rect_x",      &(g_stShelterCfg.stShelter[0][0].x),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_y",      &(g_stShelterCfg.stShelter[0][0].y),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_width",  &(g_stShelterCfg.stShelter[0][0].width),  CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_height", &(g_stShelterCfg.stShelter[0][0].height), CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_color",  &(g_stShelterCfg.stShelter[0][0].color),  CFG_DATA_TYPE_U32, 	"0", "rw", 0, 0x7FFFFFFF,  NULL},
            {NULL,},
        },
        //rect 1
        {
            {"rect_enable", &(g_stShelterCfg.stShelter[0][1].enable), CFG_DATA_TYPE_S32, 	"0", "rw", 0, 1,    NULL},
            {"rect_x",      &(g_stShelterCfg.stShelter[0][1].x),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_y",      &(g_stShelterCfg.stShelter[0][1].y),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_width",  &(g_stShelterCfg.stShelter[0][1].width),  CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_height", &(g_stShelterCfg.stShelter[0][1].height), CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_color",  &(g_stShelterCfg.stShelter[0][1].color),  CFG_DATA_TYPE_U32, 	"0", "rw", 0, 0x7FFFFFFF,  NULL},
            {NULL,},
        },
        //rect 2
        {
            {"rect_enable", &(g_stShelterCfg.stShelter[0][2].enable), CFG_DATA_TYPE_S32, 	"0", "rw", 0, 1,    NULL},
            {"rect_x",      &(g_stShelterCfg.stShelter[0][2].x),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_y",      &(g_stShelterCfg.stShelter[0][2].y),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_width",  &(g_stShelterCfg.stShelter[0][2].width),  CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_height", &(g_stShelterCfg.stShelter[0][2].height), CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_color",  &(g_stShelterCfg.stShelter[0][2].color),  CFG_DATA_TYPE_U32, 	"0", "rw", 0, 0x7FFFFFFF,  NULL},
            {NULL,},
        },
        //rect 3
        {
            {"rect_enable", &(g_stShelterCfg.stShelter[0][3].enable), CFG_DATA_TYPE_S32, 	"0", "rw", 0, 1,    NULL},
            {"rect_x",      &(g_stShelterCfg.stShelter[0][3].x),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_y",      &(g_stShelterCfg.stShelter[0][3].y),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_width",  &(g_stShelterCfg.stShelter[0][3].width),  CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_height", &(g_stShelterCfg.stShelter[0][3].height), CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_color",  &(g_stShelterCfg.stShelter[0][3].color),  CFG_DATA_TYPE_U32, 	"0", "rw", 0, 0x7FFFFFFF,  NULL},
            {NULL,},
        }
    },
    //PIPELINE 1
    {
        //rect 0
        {
            {"rect_enable", &(g_stShelterCfg.stShelter[1][0].enable), CFG_DATA_TYPE_S32, 	"0", "rw", 0, 1,    NULL},
            {"rect_x",      &(g_stShelterCfg.stShelter[1][0].x),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_y",      &(g_stShelterCfg.stShelter[1][0].y),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_width",  &(g_stShelterCfg.stShelter[1][0].width),  CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_height", &(g_stShelterCfg.stShelter[1][0].height), CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_color",  &(g_stShelterCfg.stShelter[1][0].color),  CFG_DATA_TYPE_U32, 	"0", "rw", 0, 0x7FFFFFFF,  NULL},
            {NULL,},
        },
        //rect 1
        {
            {"rect_enable", &(g_stShelterCfg.stShelter[1][1].enable), CFG_DATA_TYPE_S32, 	"0", "rw", 0, 1,    NULL},
            {"rect_x",      &(g_stShelterCfg.stShelter[1][1].x),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_y",      &(g_stShelterCfg.stShelter[1][1].y),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_width",  &(g_stShelterCfg.stShelter[1][1].width),  CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_height", &(g_stShelterCfg.stShelter[1][1].height), CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_color",  &(g_stShelterCfg.stShelter[1][1].color),  CFG_DATA_TYPE_U32, 	"0", "rw", 0, 0x7FFFFFFF,  NULL},
            {NULL,},
        },
        //rect 2
        {
            {"rect_enable", &(g_stShelterCfg.stShelter[1][2].enable), CFG_DATA_TYPE_S32, 	"0", "rw", 0, 1,    NULL},
            {"rect_x",      &(g_stShelterCfg.stShelter[1][2].x),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_y",      &(g_stShelterCfg.stShelter[1][2].y),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_width",  &(g_stShelterCfg.stShelter[1][2].width),  CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_height", &(g_stShelterCfg.stShelter[1][2].height), CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_color",  &(g_stShelterCfg.stShelter[1][2].color),  CFG_DATA_TYPE_U32, 	"0", "rw", 0, 0x7FFFFFFF,  NULL},
            {NULL,},
        },
        //rect 3
        {
            {"rect_enable", &(g_stShelterCfg.stShelter[1][3].enable), CFG_DATA_TYPE_S32, 	"0", "rw", 0, 1,    NULL},
            {"rect_x",      &(g_stShelterCfg.stShelter[1][3].x),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_y",      &(g_stShelterCfg.stShelter[1][3].y),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_width",  &(g_stShelterCfg.stShelter[1][3].width),  CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_height", &(g_stShelterCfg.stShelter[1][3].height), CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_color",  &(g_stShelterCfg.stShelter[1][3].color),  CFG_DATA_TYPE_U32, 	"0", "rw", 0, 0x7FFFFFFF,  NULL},
            {NULL,},
        }
    },
    //PIPELINE 2
    {
        //rect 0
        {
            {"rect_enable", &(g_stShelterCfg.stShelter[2][0].enable), CFG_DATA_TYPE_S32, 	"0", "rw", 0, 1,    NULL},
            {"rect_x",      &(g_stShelterCfg.stShelter[2][0].x),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_y",      &(g_stShelterCfg.stShelter[2][0].y),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_width",  &(g_stShelterCfg.stShelter[2][0].width),  CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_height", &(g_stShelterCfg.stShelter[2][0].height), CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_color",  &(g_stShelterCfg.stShelter[2][0].color),  CFG_DATA_TYPE_U32, 	"0", "rw", 0, 0x7FFFFFFF,  NULL},
            {NULL,},
        },
        //rect 1
        {
            {"rect_enable", &(g_stShelterCfg.stShelter[2][1].enable), CFG_DATA_TYPE_S32, 	"0", "rw", 0, 1,    NULL},
            {"rect_x",      &(g_stShelterCfg.stShelter[2][1].x),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_y",      &(g_stShelterCfg.stShelter[2][1].y),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_width",  &(g_stShelterCfg.stShelter[2][1].width),  CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_height", &(g_stShelterCfg.stShelter[2][1].height), CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_color",  &(g_stShelterCfg.stShelter[2][1].color),  CFG_DATA_TYPE_U32, 	"0", "rw", 0, 0x7FFFFFFF,  NULL},
            {NULL,},
        },
        //rect 2
        {
            {"rect_enable", &(g_stShelterCfg.stShelter[2][2].enable), CFG_DATA_TYPE_S32, 	"0", "rw", 0, 1,    NULL},
            {"rect_x",      &(g_stShelterCfg.stShelter[2][2].x),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_y",      &(g_stShelterCfg.stShelter[2][2].y),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_width",  &(g_stShelterCfg.stShelter[2][2].width),  CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_height", &(g_stShelterCfg.stShelter[2][2].height), CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_color",  &(g_stShelterCfg.stShelter[2][2].color),  CFG_DATA_TYPE_U32, 	"0", "rw", 0, 0x7FFFFFFF,  NULL},
            {NULL,},
        },
        //rect 3
        {
            {"rect_enable", &(g_stShelterCfg.stShelter[2][3].enable), CFG_DATA_TYPE_S32, 	"0", "rw", 0, 1,    NULL},
            {"rect_x",      &(g_stShelterCfg.stShelter[2][3].x),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_y",      &(g_stShelterCfg.stShelter[2][3].y),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_width",  &(g_stShelterCfg.stShelter[2][3].width),  CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_height", &(g_stShelterCfg.stShelter[2][3].height), CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_color",  &(g_stShelterCfg.stShelter[2][3].color),  CFG_DATA_TYPE_U32, 	"0", "rw", 0, 0x7FFFFFFF,  NULL},
            {NULL,},
        }
    },
    //PIPELINE 3
    {
        //rect 0
        {
            {"rect_enable", &(g_stShelterCfg.stShelter[3][0].enable), CFG_DATA_TYPE_S32, 	"0", "rw", 0, 1,    NULL},
            {"rect_x",      &(g_stShelterCfg.stShelter[3][0].x),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_y",      &(g_stShelterCfg.stShelter[3][0].y),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_width",  &(g_stShelterCfg.stShelter[3][0].width),  CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_height", &(g_stShelterCfg.stShelter[3][0].height), CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_color",  &(g_stShelterCfg.stShelter[3][0].color),  CFG_DATA_TYPE_U32, 	"0", "rw", 0, 0x7FFFFFFF,  NULL},
            {NULL,},
        },
        //rect 1
        {
            {"rect_enable", &(g_stShelterCfg.stShelter[3][1].enable), CFG_DATA_TYPE_S32, 	"0", "rw", 0, 1,    NULL},
            {"rect_x",      &(g_stShelterCfg.stShelter[3][1].x),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_y",      &(g_stShelterCfg.stShelter[3][1].y),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_width",  &(g_stShelterCfg.stShelter[3][1].width),  CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_height", &(g_stShelterCfg.stShelter[3][1].height), CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_color",  &(g_stShelterCfg.stShelter[3][1].color),  CFG_DATA_TYPE_U32, 	"0", "rw", 0, 0x7FFFFFFF,  NULL},
            {NULL,},
        },
        //rect 2
        {
            {"rect_enable", &(g_stShelterCfg.stShelter[3][2].enable), CFG_DATA_TYPE_S32, 	"0", "rw", 0, 1,    NULL},
            {"rect_x",      &(g_stShelterCfg.stShelter[3][2].x),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_y",      &(g_stShelterCfg.stShelter[3][2].y),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_width",  &(g_stShelterCfg.stShelter[3][2].width),  CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_height", &(g_stShelterCfg.stShelter[3][2].height), CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_color",  &(g_stShelterCfg.stShelter[3][2].color),  CFG_DATA_TYPE_U32, 	"0", "rw", 0, 0x7FFFFFFF,  NULL},
            {NULL,},
        },
        //rect 3
        {
            {"rect_enable", &(g_stShelterCfg.stShelter[3][3].enable), CFG_DATA_TYPE_S32, 	"0", "rw", 0, 1,    NULL},
            {"rect_x",      &(g_stShelterCfg.stShelter[3][3].x),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_y",      &(g_stShelterCfg.stShelter[3][3].y),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_width",  &(g_stShelterCfg.stShelter[3][3].width),  CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_height", &(g_stShelterCfg.stShelter[3][3].height), CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_color",  &(g_stShelterCfg.stShelter[3][3].color),  CFG_DATA_TYPE_U32, 	"0", "rw", 0, 0x7FFFFFFF,  NULL},
            {NULL,},
        }
    },
    //PIPELINE 4
    {
        //rect 0
        {
            {"rect_enable", &(g_stShelterCfg.stShelter[4][0].enable), CFG_DATA_TYPE_S32, 	"0", "rw", 0, 1,    NULL},
            {"rect_x",      &(g_stShelterCfg.stShelter[4][0].x),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_y",      &(g_stShelterCfg.stShelter[4][0].y),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_width",  &(g_stShelterCfg.stShelter[4][0].width),  CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_height", &(g_stShelterCfg.stShelter[4][0].height), CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_color",  &(g_stShelterCfg.stShelter[4][0].color),  CFG_DATA_TYPE_U32, 	"0", "rw", 0, 0x7FFFFFFF,  NULL},
            {NULL,},
        },
        //rect 1
        {
            {"rect_enable", &(g_stShelterCfg.stShelter[4][1].enable), CFG_DATA_TYPE_S32, 	"0", "rw", 0, 1,    NULL},
            {"rect_x",      &(g_stShelterCfg.stShelter[4][1].x),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_y",      &(g_stShelterCfg.stShelter[4][1].y),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_width",  &(g_stShelterCfg.stShelter[4][1].width),  CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_height", &(g_stShelterCfg.stShelter[4][1].height), CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_color",  &(g_stShelterCfg.stShelter[4][1].color),  CFG_DATA_TYPE_U32, 	"0", "rw", 0, 0x7FFFFFFF,  NULL},
            {NULL,},
        },
        //rect 2
        {
            {"rect_enable", &(g_stShelterCfg.stShelter[4][2].enable), CFG_DATA_TYPE_S32, 	"0", "rw", 0, 1,    NULL},
            {"rect_x",      &(g_stShelterCfg.stShelter[4][2].x),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_y",      &(g_stShelterCfg.stShelter[4][2].y),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_width",  &(g_stShelterCfg.stShelter[4][2].width),  CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_height", &(g_stShelterCfg.stShelter[4][2].height), CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_color",  &(g_stShelterCfg.stShelter[4][2].color),  CFG_DATA_TYPE_U32, 	"0", "rw", 0, 0x7FFFFFFF,  NULL},
            {NULL,},
        },
        //rect 3
        {
            {"rect_enable", &(g_stShelterCfg.stShelter[4][3].enable), CFG_DATA_TYPE_S32, 	"0", "rw", 0, 1,    NULL},
            {"rect_x",      &(g_stShelterCfg.stShelter[4][3].x),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_y",      &(g_stShelterCfg.stShelter[4][3].y),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_width",  &(g_stShelterCfg.stShelter[4][3].width),  CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_height", &(g_stShelterCfg.stShelter[4][3].height), CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_color",  &(g_stShelterCfg.stShelter[4][3].color),  CFG_DATA_TYPE_U32, 	"0", "rw", 0, 0x7FFFFFFF,  NULL},
            {NULL,},
        }
    },
    //PIPELINE 5
    {
        //rect 0
        {
            {"rect_enable", &(g_stShelterCfg.stShelter[5][0].enable), CFG_DATA_TYPE_S32, 	"0", "rw", 0, 1,    NULL},
            {"rect_x",      &(g_stShelterCfg.stShelter[5][0].x),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_y",      &(g_stShelterCfg.stShelter[5][0].y),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_width",  &(g_stShelterCfg.stShelter[5][0].width),  CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_height", &(g_stShelterCfg.stShelter[5][0].height), CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_color",  &(g_stShelterCfg.stShelter[5][0].color),  CFG_DATA_TYPE_U32, 	"0", "rw", 0, 0x7FFFFFFF,  NULL},
            {NULL,},
        },
        //rect 1
        {
            {"rect_enable", &(g_stShelterCfg.stShelter[5][1].enable), CFG_DATA_TYPE_S32, 	"0", "rw", 0, 1,    NULL},
            {"rect_x",      &(g_stShelterCfg.stShelter[5][1].x),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_y",      &(g_stShelterCfg.stShelter[5][1].y),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_width",  &(g_stShelterCfg.stShelter[5][1].width),  CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_height", &(g_stShelterCfg.stShelter[5][1].height), CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_color",  &(g_stShelterCfg.stShelter[5][1].color),  CFG_DATA_TYPE_U32, 	"0", "rw", 0, 0x7FFFFFFF,  NULL},
            {NULL,},
        },
        //rect 2
        {
            {"rect_enable", &(g_stShelterCfg.stShelter[5][2].enable), CFG_DATA_TYPE_S32, 	"0", "rw", 0, 1,    NULL},
            {"rect_x",      &(g_stShelterCfg.stShelter[5][2].x),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_y",      &(g_stShelterCfg.stShelter[5][2].y),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_width",  &(g_stShelterCfg.stShelter[5][2].width),  CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_height", &(g_stShelterCfg.stShelter[5][2].height), CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_color",  &(g_stShelterCfg.stShelter[5][2].color),  CFG_DATA_TYPE_U32, 	"0", "rw", 0, 0x7FFFFFFF,  NULL},
            {NULL,},
        },
        //rect 3
        {
            {"rect_enable", &(g_stShelterCfg.stShelter[5][3].enable), CFG_DATA_TYPE_S32, 	"0", "rw", 0, 1,    NULL},
            {"rect_x",      &(g_stShelterCfg.stShelter[5][3].x),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_y",      &(g_stShelterCfg.stShelter[5][3].y),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_width",  &(g_stShelterCfg.stShelter[5][3].width),  CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_height", &(g_stShelterCfg.stShelter[5][3].height), CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_color",  &(g_stShelterCfg.stShelter[5][3].color),  CFG_DATA_TYPE_U32, 	"0", "rw", 0, 0x7FFFFFFF,  NULL},
            {NULL,},
        }
    },
    //PIPELINE 6
    {
        //rect 0
        {
            {"rect_enable", &(g_stShelterCfg.stShelter[6][0].enable), CFG_DATA_TYPE_S32, 	"0", "rw", 0, 1,    NULL},
            {"rect_x",      &(g_stShelterCfg.stShelter[6][0].x),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_y",      &(g_stShelterCfg.stShelter[6][0].y),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_width",  &(g_stShelterCfg.stShelter[6][0].width),  CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_height", &(g_stShelterCfg.stShelter[6][0].height), CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_color",  &(g_stShelterCfg.stShelter[6][0].color),  CFG_DATA_TYPE_U32, 	"0", "rw", 0, 0x7FFFFFFF,  NULL},
            {NULL,},
        },
        //rect 1
        {
            {"rect_enable", &(g_stShelterCfg.stShelter[6][1].enable), CFG_DATA_TYPE_S32, 	"0", "rw", 0, 1,    NULL},
            {"rect_x",      &(g_stShelterCfg.stShelter[6][1].x),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_y",      &(g_stShelterCfg.stShelter[6][1].y),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_width",  &(g_stShelterCfg.stShelter[6][1].width),  CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_height", &(g_stShelterCfg.stShelter[6][1].height), CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_color",  &(g_stShelterCfg.stShelter[6][1].color),  CFG_DATA_TYPE_U32, 	"0", "rw", 0, 0x7FFFFFFF,  NULL},
            {NULL,},
        },
        //rect 2
        {
            {"rect_enable", &(g_stShelterCfg.stShelter[6][2].enable), CFG_DATA_TYPE_S32, 	"0", "rw", 0, 1,    NULL},
            {"rect_x",      &(g_stShelterCfg.stShelter[6][2].x),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_y",      &(g_stShelterCfg.stShelter[6][2].y),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_width",  &(g_stShelterCfg.stShelter[6][2].width),  CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_height", &(g_stShelterCfg.stShelter[6][2].height), CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_color",  &(g_stShelterCfg.stShelter[6][2].color),  CFG_DATA_TYPE_U32, 	"0", "rw", 0, 0x7FFFFFFF,  NULL},
            {NULL,},
        },
        //rect 3
        {
            {"rect_enable", &(g_stShelterCfg.stShelter[6][3].enable), CFG_DATA_TYPE_S32, 	"0", "rw", 0, 1,    NULL},
            {"rect_x",      &(g_stShelterCfg.stShelter[6][3].x),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_y",      &(g_stShelterCfg.stShelter[6][3].y),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_width",  &(g_stShelterCfg.stShelter[6][3].width),  CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_height", &(g_stShelterCfg.stShelter[6][3].height), CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_color",  &(g_stShelterCfg.stShelter[6][3].color),  CFG_DATA_TYPE_U32, 	"0", "rw", 0, 0x7FFFFFFF,  NULL},
            {NULL,},
        }
    },
    //PIPELINE 7
    {
        //rect 0
        {
            {"rect_enable", &(g_stShelterCfg.stShelter[7][0].enable), CFG_DATA_TYPE_S32, 	"0", "rw", 0, 1,    NULL},
            {"rect_x",      &(g_stShelterCfg.stShelter[7][0].x),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_y",      &(g_stShelterCfg.stShelter[7][0].y),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_width",  &(g_stShelterCfg.stShelter[7][0].width),  CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_height", &(g_stShelterCfg.stShelter[7][0].height), CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_color",  &(g_stShelterCfg.stShelter[7][0].color),  CFG_DATA_TYPE_U32, 	"0", "rw", 0, 0x7FFFFFFF,  NULL},
            {NULL,},
        },
        //rect 1
        {
            {"rect_enable", &(g_stShelterCfg.stShelter[7][1].enable), CFG_DATA_TYPE_S32, 	"0", "rw", 0, 1,    NULL},
            {"rect_x",      &(g_stShelterCfg.stShelter[7][1].x),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_y",      &(g_stShelterCfg.stShelter[7][1].y),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_width",  &(g_stShelterCfg.stShelter[7][1].width),  CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_height", &(g_stShelterCfg.stShelter[7][1].height), CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_color",  &(g_stShelterCfg.stShelter[7][1].color),  CFG_DATA_TYPE_U32, 	"0", "rw", 0, 0x7FFFFFFF,  NULL},
            {NULL,},
        },
        //rect 2
        {
            {"rect_enable", &(g_stShelterCfg.stShelter[7][2].enable), CFG_DATA_TYPE_S32, 	"0", "rw", 0, 1,    NULL},
            {"rect_x",      &(g_stShelterCfg.stShelter[7][2].x),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_y",      &(g_stShelterCfg.stShelter[7][2].y),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_width",  &(g_stShelterCfg.stShelter[7][2].width),  CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_height", &(g_stShelterCfg.stShelter[7][2].height), CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_color",  &(g_stShelterCfg.stShelter[7][2].color),  CFG_DATA_TYPE_U32, 	"0", "rw", 0, 0x7FFFFFFF,  NULL},
            {NULL,},
        },
        //rect 3
        {
            {"rect_enable", &(g_stShelterCfg.stShelter[7][3].enable), CFG_DATA_TYPE_S32, 	"0", "rw", 0, 1,    NULL},
            {"rect_x",      &(g_stShelterCfg.stShelter[7][3].x),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_y",      &(g_stShelterCfg.stShelter[7][3].y),      CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_width",  &(g_stShelterCfg.stShelter[7][3].width),  CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_height", &(g_stShelterCfg.stShelter[7][3].height), CFG_DATA_TYPE_FLOAT, 	"0", "rw", 0, 1.0, NULL},
            {"rect_color",  &(g_stShelterCfg.stShelter[7][3].color),  CFG_DATA_TYPE_U32, 	"0", "rw", 0, 0x7FFFFFFF,  NULL},
            {NULL,},
        }
    }
};


AR_S32 IPC_CFG_SHELTER_Save()
{
    int i = 0, pipe_num = 0;
    cJSON *root, *shelter;
    char *out;
    char tmp[32] = {0};

    root = cJSON_CreateObject();//创建项目

    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        pipe_num++;
        sprintf(tmp, "shelter_vi_%d", i);
        shelter = cJSON_CreateObject();
        CfgAddCjson(shelter, "rect_0", shelterRectMap[i][0]);
        CfgAddCjson(shelter, "rect_1", shelterRectMap[i][1]);
        CfgAddCjson(shelter, "rect_2", shelterRectMap[i][2]);
        CfgAddCjson(shelter, "rect_3", shelterRectMap[i][3]);

        cJSON_AddItemToObject(root, tmp, shelter);
    }

    cJSON_AddNumberToObject(root, "shelter_vi_num", pipe_num);

    out = cJSON_Print(root);

    int ret = CfgWriteToFile(SHELTER_CFG_FILE, out);
    if (ret != 0)
    {
        PRINT_ERR("CfgWriteToFile %s error.", SHELTER_CFG_FILE);
        return -1;
    }

    free(out);
    cJSON_Delete(root);

    return 0;
}

//Need to re write the code.
AR_S32 IPC_CFG_SHELTER_Load()
{
    int i = 0, j = 0, pipe_num = 0;
    char *data = NULL;

    data = CfgReadFromFile(SHELTER_CFG_FILE);
    if (data == NULL)
    {
        //从配置文件读取失败，则使用默认参数
        PRINT_INFO("load %s error, so to load default cfg param.\n", SHELTER_CFG_FILE);
        goto err;
    }

    cJSON *json = NULL;
    json = cJSON_Parse(data);
    if (!json)
    {
        PRINT_INFO("Error before: [%s]\n", cJSON_GetErrorPtr());
        free(data);
        goto err;
    }

    cJSON * num = cJSON_GetObjectItem(json, "shelter_vi_num");
    if(num)
    {
        pipe_num = num->valueint;
    }
    else
    {
        pipe_num = IPC_MAX_PIPELINE_NUM;
    }

    for(i = 0; i < pipe_num; i++)
    {
        char tmp[32] = {0};
        sprintf(tmp, "shelter_vi_%d", i);
        cJSON * shelter = cJSON_GetObjectItem(json, tmp);
        if(!shelter)
        {
            PRINT_ERR("get shelter error\n");
            goto err1;
        }

        for(j = 0; j < MAX_SHELTER_RECT_PER_CHN; j++)
        {
            sprintf(tmp, "rect_%d", j);

            CfgParseCjson(shelter, tmp, shelterRectMap[i][j]);
        }
    }

    cJSON_Delete(json);
    free(data);
    return 0;

err1:
    cJSON_Delete(json);
    free(data);
err:
    IPC_CFG_SHELTER_LoadDefault();
    IPC_CFG_SHELTER_Save();

    return 0;
}

void IPC_CFG_SHELTER_Print()
{
    int i = 0, j = 0;
    printf("********** shelter rect *********\n");
    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i ++)
    {
        printf("shelter pipe %d:\n", i);
        for(j = 0; j < MAX_SHELTER_RECT_PER_CHN; j++)
        {
            printf("rect_%d:\n", j);
            CfgPrintMap(shelterRectMap[i][j]);
            printf("\n");
        }
    }
    printf("********** shelter rect *********\n\n");
}

AR_S32 IPC_CFG_SHELTER_LoadDefault()
{
    return 0;
}

IPC_CFG_SHELTER_S * IPC_CFG_SHELTER_GetParam()
{
    return &g_stShelterCfg;
}
