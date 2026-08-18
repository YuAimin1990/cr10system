#include "cfg_osd.h"

#if 0
extern char device_local_time[128];
extern char device_local_week;
#endif

IPC_CFG_OSD_S g_stRunOsdCfg;

CFG_MAP osdMap[IPC_MAX_PIPELINE_NUM][IPC_MAX_STREAM_PER_PIPE][MAX_CHANNEL_ITEM_NUM] =
{
    //channel 0:
    {
        //stream 0: main
        {
            {"x",       &(g_stRunOsdCfg.osdInfo[0][0].x_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "x ratio: 0.0 - 1.0"},
            {"y",       &(g_stRunOsdCfg.osdInfo[0][0].y_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "y ratio: 0.0 - 1.0"},
            {"osdChannelName_enable",  &(g_stRunOsdCfg.osdInfo[0][0].osdChannelName.enable),  CFG_DATA_TYPE_S32,    "1",   "rw", 0, 1,               NULL},
            {"osdChannelName_text",    &(g_stRunOsdCfg.osdInfo[0][0].osdChannelName.text),    CFG_DATA_TYPE_STRING, "CH0-0", "rw", 1, MAX_STR_LEN_128, NULL},
            {"osdDatetime_enable",     &(g_stRunOsdCfg.osdInfo[0][0].osdDatetime.enable),     CFG_DATA_TYPE_S32, "1",   "rw", 0, 1,    NULL},
            {"osdDatetime_dateFormat", &(g_stRunOsdCfg.osdInfo[0][0].osdDatetime.dateFormat), CFG_DATA_TYPE_U8,  "0",   "rw", 1, 120,  "0:XXXX-XX-XX year mon date; 1:XX-XX-XXXX mon date year 2:XX-XX-XXXX date mon year"},
            {"osdDatetime_dateSprtr",  &(g_stRunOsdCfg.osdInfo[0][0].osdDatetime.dateSprtr),  CFG_DATA_TYPE_U8,  "0",   "rw", 0, 3,    "0 :, 1 -, 2 //, 3 ."},
            {"osdDatetime_timeFmt",    &(g_stRunOsdCfg.osdInfo[0][0].osdDatetime.timeFmt),    CFG_DATA_TYPE_U8,  "0",   "rw", 0, 1,    "0 - 24,1 - 12"},
    	    {"osdDatetime_displayWeek", &(g_stRunOsdCfg.osdInfo[0][0].osdDatetime.displayWeek),    CFG_DATA_TYPE_U8,  "1",	"rw", 0, 1, "0 - off,1 - on"},
            {"attached_to", &(g_stRunOsdCfg.osdInfo[0][0].achAttachedTo),    CFG_DATA_TYPE_STRING,  "vpss",  "rw", 1, 16, "attached to which module"},
            {"alg_osd_enable", &(g_stRunOsdCfg.osdInfo[0][0].u32AlgOsdEnable),    CFG_DATA_TYPE_S32,  "0",  "rw", 0, 1, "Allow algrithom to draw on this channel"},
            {NULL,},
        },
        //stream 1: sub
        {
            {"x",       &(g_stRunOsdCfg.osdInfo[0][1].x_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "x ratio: 0.0 - 1.0"},
            {"y",       &(g_stRunOsdCfg.osdInfo[0][1].y_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "y ratio: 0.0 - 1.0"},
            {"osdChannelName_enable",  &(g_stRunOsdCfg.osdInfo[0][1].osdChannelName.enable),  CFG_DATA_TYPE_S32,    "1",   "rw", 0, 1,               NULL},
            {"osdChannelName_text",    &(g_stRunOsdCfg.osdInfo[0][1].osdChannelName.text),    CFG_DATA_TYPE_STRING, "CH0-1", "rw", 1, MAX_STR_LEN_128, NULL},
            {"osdDatetime_enable",     &(g_stRunOsdCfg.osdInfo[0][1].osdDatetime.enable),     CFG_DATA_TYPE_S32, "1",   "rw", 0, 1,    NULL},
            {"osdDatetime_dateFormat", &(g_stRunOsdCfg.osdInfo[0][1].osdDatetime.dateFormat), CFG_DATA_TYPE_U8,  "0",   "rw", 1, 120,  "0:XXXX-XX-XX year mon date; 1:XX-XX-XXXX mon date year 2:XX-XX-XXXX date mon year"},
            {"osdDatetime_dateSprtr",  &(g_stRunOsdCfg.osdInfo[0][1].osdDatetime.dateSprtr),  CFG_DATA_TYPE_U8,  "0",   "rw", 0, 3,    "0 :, 1 -, 2 //, 3 ."},
            {"osdDatetime_timeFmt",    &(g_stRunOsdCfg.osdInfo[0][1].osdDatetime.timeFmt),    CFG_DATA_TYPE_U8,  "0",   "rw", 0, 1,    "0 - 24,1 - 12"},
            {"osdDatetime_displayWeek", &(g_stRunOsdCfg.osdInfo[0][1].osdDatetime.displayWeek),    CFG_DATA_TYPE_U8,  "1",	"rw", 0, 1, "0 - off,1 - on"},
            {"attached_to", &(g_stRunOsdCfg.osdInfo[0][1].achAttachedTo),    CFG_DATA_TYPE_STRING,  "vpss",  "rw", 1, 16, "attached to which module"},
            {"alg_osd_enable", &(g_stRunOsdCfg.osdInfo[0][1].u32AlgOsdEnable),    CFG_DATA_TYPE_S32,  "0",  "rw", 0, 1, "Allow algrithom to draw on this channel"},
            {NULL,},
        },
        //stream 2 if it exists.
        {
            {"x",       &(g_stRunOsdCfg.osdInfo[0][2].x_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "x ratio: 0.0 - 1.0"},
            {"y",       &(g_stRunOsdCfg.osdInfo[0][2].y_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "y ratio: 0.0 - 1.0"},
            {"osdChannelName_enable",  &(g_stRunOsdCfg.osdInfo[0][2].osdChannelName.enable),  CFG_DATA_TYPE_S32,    "0",   "rw", 0, 1,               NULL},
            {"osdChannelName_text",    &(g_stRunOsdCfg.osdInfo[0][2].osdChannelName.text),    CFG_DATA_TYPE_STRING, "CH0-2", "rw", 1, MAX_STR_LEN_128, NULL},
            {"osdDatetime_enable",     &(g_stRunOsdCfg.osdInfo[0][2].osdDatetime.enable),     CFG_DATA_TYPE_S32, "0",   "rw", 0, 1,    NULL},
            {"osdDatetime_dateFormat", &(g_stRunOsdCfg.osdInfo[0][2].osdDatetime.dateFormat), CFG_DATA_TYPE_U8,  "0",   "rw", 1, 120,  "0:XXXX-XX-XX year mon date; 1:XX-XX-XXXX mon date year 2:XX-XX-XXXX date mon year"},
            {"osdDatetime_dateSprtr",  &(g_stRunOsdCfg.osdInfo[0][2].osdDatetime.dateSprtr),  CFG_DATA_TYPE_U8,  "0",   "rw", 0, 3,    "0 :, 1 -, 2 //, 3 ."},
            {"osdDatetime_timeFmt",    &(g_stRunOsdCfg.osdInfo[0][2].osdDatetime.timeFmt),    CFG_DATA_TYPE_U8,  "0",   "rw", 0, 1,    "0 - 24,1 - 12"},
            {"osdDatetime_displayWeek", &(g_stRunOsdCfg.osdInfo[0][2].osdDatetime.displayWeek),    CFG_DATA_TYPE_U8,  "0",	"rw", 0, 1, "0 - off, 1 - on"},
            {"attached_to", &(g_stRunOsdCfg.osdInfo[0][2].achAttachedTo),    CFG_DATA_TYPE_STRING,  "vpss",  "rw", 1, 16, "attached to which module"},
            {"alg_osd_enable", &(g_stRunOsdCfg.osdInfo[0][2].u32AlgOsdEnable),    CFG_DATA_TYPE_S32,  "0",  "rw", 0, 1, "Allow algrithom to draw on this channel"},
            {NULL,},
        },
        //stream 3 if it exists
        {
            {"x",       &(g_stRunOsdCfg.osdInfo[0][3].x_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "x ratio: 0.0 - 1.0"},
            {"y",       &(g_stRunOsdCfg.osdInfo[0][3].y_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "y ratio: 0.0 - 1.0"},
            {"osdChannelName_enable",  &(g_stRunOsdCfg.osdInfo[0][3].osdChannelName.enable),  CFG_DATA_TYPE_S32,    "0",   "rw", 0, 1,               NULL},
            {"osdChannelName_text",    &(g_stRunOsdCfg.osdInfo[0][3].osdChannelName.text),    CFG_DATA_TYPE_STRING, "CH0-3", "rw", 1, MAX_STR_LEN_128, NULL},
            {"osdDatetime_enable",     &(g_stRunOsdCfg.osdInfo[0][3].osdDatetime.enable),     CFG_DATA_TYPE_S32, "0",   "rw", 0, 1,    NULL},
            {"osdDatetime_dateFormat", &(g_stRunOsdCfg.osdInfo[0][3].osdDatetime.dateFormat), CFG_DATA_TYPE_U8,  "0",   "rw", 1, 120,  "0:XXXX-XX-XX year mon date; 1:XX-XX-XXXX mon date year 2:XX-XX-XXXX date mon year"},
            {"osdDatetime_dateSprtr",  &(g_stRunOsdCfg.osdInfo[0][3].osdDatetime.dateSprtr),  CFG_DATA_TYPE_U8,  "0",   "rw", 0, 3,    "0 :, 1 -, 2 //, 3 ."},
            {"osdDatetime_timeFmt",    &(g_stRunOsdCfg.osdInfo[0][3].osdDatetime.timeFmt),    CFG_DATA_TYPE_U8,  "0",   "rw", 0, 1,    "0 - 24,1 - 12"},
            {"osdDatetime_displayWeek", &(g_stRunOsdCfg.osdInfo[0][3].osdDatetime.displayWeek),    CFG_DATA_TYPE_U8,  "1",	"rw", 0, 1, "0 - off,1 - on"},
            {"attached_to", &(g_stRunOsdCfg.osdInfo[0][3].achAttachedTo),    CFG_DATA_TYPE_STRING,  "vpss",  "rw", 1, 16, "attached to which module"},
            {"alg_osd_enable", &(g_stRunOsdCfg.osdInfo[0][3].u32AlgOsdEnable),    CFG_DATA_TYPE_S32,  "0",  "rw", 0, 1, "Allow algrithom to draw on this channel"},
            {NULL,},
        }
    },
    //channel 1:
    {
        //stream 0: main
        {
            {"x",       &(g_stRunOsdCfg.osdInfo[1][0].x_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "x ratio: 0.0 - 1.0"},
            {"y",       &(g_stRunOsdCfg.osdInfo[1][0].y_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "y ratio: 0.0 - 1.0"},
            {"osdChannelName_enable",  &(g_stRunOsdCfg.osdInfo[1][0].osdChannelName.enable),  CFG_DATA_TYPE_S32,    "1",   "rw", 0, 1,               NULL},
            {"osdChannelName_text",    &(g_stRunOsdCfg.osdInfo[1][0].osdChannelName.text),    CFG_DATA_TYPE_STRING, "CH1-0", "rw", 1, MAX_STR_LEN_128, NULL},
            {"osdDatetime_enable",     &(g_stRunOsdCfg.osdInfo[1][0].osdDatetime.enable),     CFG_DATA_TYPE_S32, "1",   "rw", 0, 1,    NULL},
            {"osdDatetime_dateFormat", &(g_stRunOsdCfg.osdInfo[1][0].osdDatetime.dateFormat), CFG_DATA_TYPE_U8,  "0",   "rw", 1, 120,  "0:XXXX-XX-XX year mon date; 1:XX-XX-XXXX mon date year 2:XX-XX-XXXX date mon year"},
            {"osdDatetime_dateSprtr",  &(g_stRunOsdCfg.osdInfo[1][0].osdDatetime.dateSprtr),  CFG_DATA_TYPE_U8,  "0",   "rw", 0, 3,    "0 :, 1 -, 2 //, 3 ."},
            {"osdDatetime_timeFmt",    &(g_stRunOsdCfg.osdInfo[1][0].osdDatetime.timeFmt),    CFG_DATA_TYPE_U8,  "0",   "rw", 0, 1,    "0 - 24,1 - 12"},
    	    {"osdDatetime_displayWeek", &(g_stRunOsdCfg.osdInfo[1][0].osdDatetime.displayWeek),    CFG_DATA_TYPE_U8,  "1",	"rw", 0, 1, "0 - off,1 - on"},
            {"attached_to", &(g_stRunOsdCfg.osdInfo[1][0].achAttachedTo),    CFG_DATA_TYPE_STRING,  "vpss",  "rw", 1, 16, "attached to which module"},
            {"alg_osd_enable", &(g_stRunOsdCfg.osdInfo[1][0].u32AlgOsdEnable),    CFG_DATA_TYPE_S32,  "0",  "rw", 0, 1, "Allow algrithom to draw on this channel"},
            {NULL,},
        },
        //stream 1: sub
        {
            {"x",       &(g_stRunOsdCfg.osdInfo[1][1].x_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "x ratio: 0.0 - 1.0"},
            {"y",       &(g_stRunOsdCfg.osdInfo[1][1].y_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "y ratio: 0.0 - 1.0"},
            {"osdChannelName_enable",  &(g_stRunOsdCfg.osdInfo[1][1].osdChannelName.enable),  CFG_DATA_TYPE_S32,    "1",   "rw", 0, 1,               NULL},
            {"osdChannelName_text",    &(g_stRunOsdCfg.osdInfo[1][1].osdChannelName.text),    CFG_DATA_TYPE_STRING, "CH1-1", "rw", 1, MAX_STR_LEN_128, NULL},
            {"osdDatetime_enable",     &(g_stRunOsdCfg.osdInfo[1][1].osdDatetime.enable),     CFG_DATA_TYPE_S32, "1",   "rw", 0, 1,    NULL},
            {"osdDatetime_dateFormat", &(g_stRunOsdCfg.osdInfo[1][1].osdDatetime.dateFormat), CFG_DATA_TYPE_U8,  "0",   "rw", 1, 120,  "0:XXXX-XX-XX year mon date; 1:XX-XX-XXXX mon date year 2:XX-XX-XXXX date mon year"},
            {"osdDatetime_dateSprtr",  &(g_stRunOsdCfg.osdInfo[1][1].osdDatetime.dateSprtr),  CFG_DATA_TYPE_U8,  "0",   "rw", 0, 3,    "0 :, 1 -, 2 //, 3 ."},
            {"osdDatetime_timeFmt",    &(g_stRunOsdCfg.osdInfo[1][1].osdDatetime.timeFmt),    CFG_DATA_TYPE_U8,  "0",   "rw", 0, 1,    "0 - 24,1 - 12"},
            {"osdDatetime_displayWeek", &(g_stRunOsdCfg.osdInfo[1][1].osdDatetime.displayWeek),    CFG_DATA_TYPE_U8,  "1",	"rw", 0, 1, "0 - off,1 - on"},
            {"attached_to", &(g_stRunOsdCfg.osdInfo[1][1].achAttachedTo),    CFG_DATA_TYPE_STRING,  "vpss",  "rw", 1, 16, "attached to which module"},
            {"alg_osd_enable", &(g_stRunOsdCfg.osdInfo[1][1].u32AlgOsdEnable),    CFG_DATA_TYPE_S32,  "0",  "rw", 0, 1, "Allow algrithom to draw on this channel"},
            {NULL,},
        },
        //stream 2 if it exists.
        {
            {"x",       &(g_stRunOsdCfg.osdInfo[1][2].x_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "x ratio: 0.0 - 1.0"},
            {"y",       &(g_stRunOsdCfg.osdInfo[1][2].y_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "y ratio: 0.0 - 1.0"},
            {"osdChannelName_enable",  &(g_stRunOsdCfg.osdInfo[1][2].osdChannelName.enable),  CFG_DATA_TYPE_S32,    "0",   "rw", 0, 1,               NULL},
            {"osdChannelName_text",    &(g_stRunOsdCfg.osdInfo[1][2].osdChannelName.text),    CFG_DATA_TYPE_STRING, "CH1-2", "rw", 1, MAX_STR_LEN_128, NULL},
            {"osdDatetime_enable",     &(g_stRunOsdCfg.osdInfo[1][2].osdDatetime.enable),     CFG_DATA_TYPE_S32, "0",   "rw", 0, 1,    NULL},
            {"osdDatetime_dateFormat", &(g_stRunOsdCfg.osdInfo[1][2].osdDatetime.dateFormat), CFG_DATA_TYPE_U8,  "0",   "rw", 1, 120,  "0:XXXX-XX-XX year mon date; 1:XX-XX-XXXX mon date year 2:XX-XX-XXXX date mon year"},
            {"osdDatetime_dateSprtr",  &(g_stRunOsdCfg.osdInfo[1][2].osdDatetime.dateSprtr),  CFG_DATA_TYPE_U8,  "0",   "rw", 0, 3,    "0 :, 1 -, 2 //, 3 ."},
            {"osdDatetime_timeFmt",    &(g_stRunOsdCfg.osdInfo[1][2].osdDatetime.timeFmt),    CFG_DATA_TYPE_U8,  "0",   "rw", 0, 1,    "0 - 24,1 - 12"},
            {"osdDatetime_displayWeek", &(g_stRunOsdCfg.osdInfo[1][2].osdDatetime.displayWeek),    CFG_DATA_TYPE_U8,  "0",	"rw", 0, 1, "0 - off, 1 - on"},
            {"attached_to", &(g_stRunOsdCfg.osdInfo[1][2].achAttachedTo),    CFG_DATA_TYPE_STRING,  "vpss",  "rw", 1, 16, "attached to which module"},
            {"alg_osd_enable", &(g_stRunOsdCfg.osdInfo[1][2].u32AlgOsdEnable),    CFG_DATA_TYPE_S32,  "0",  "rw", 0, 1, "Allow algrithom to draw on this channel"},
            {NULL,},
        },
        //stream 3 if it exists
        {
            {"x",       &(g_stRunOsdCfg.osdInfo[1][3].x_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "x ratio: 0.0 - 1.0"},
            {"y",       &(g_stRunOsdCfg.osdInfo[1][3].y_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "y ratio: 0.0 - 1.0"},
            {"osdChannelName_enable",  &(g_stRunOsdCfg.osdInfo[1][3].osdChannelName.enable),  CFG_DATA_TYPE_S32,    "0",   "rw", 0, 1,               NULL},
            {"osdChannelName_text",    &(g_stRunOsdCfg.osdInfo[1][3].osdChannelName.text),    CFG_DATA_TYPE_STRING, "CH1-3", "rw", 1, MAX_STR_LEN_128, NULL},
            {"osdDatetime_enable",     &(g_stRunOsdCfg.osdInfo[1][3].osdDatetime.enable),     CFG_DATA_TYPE_S32, "0",   "rw", 0, 1,    NULL},
            {"osdDatetime_dateFormat", &(g_stRunOsdCfg.osdInfo[1][3].osdDatetime.dateFormat), CFG_DATA_TYPE_U8,  "0",   "rw", 1, 120,  "0:XXXX-XX-XX year mon date; 1:XX-XX-XXXX mon date year 2:XX-XX-XXXX date mon year"},
            {"osdDatetime_dateSprtr",  &(g_stRunOsdCfg.osdInfo[1][3].osdDatetime.dateSprtr),  CFG_DATA_TYPE_U8,  "0",   "rw", 0, 3,    "0 :, 1 -, 2 //, 3 ."},
            {"osdDatetime_timeFmt",    &(g_stRunOsdCfg.osdInfo[1][3].osdDatetime.timeFmt),    CFG_DATA_TYPE_U8,  "0",   "rw", 0, 1,    "0 - 24,1 - 12"},
            {"osdDatetime_displayWeek", &(g_stRunOsdCfg.osdInfo[1][3].osdDatetime.displayWeek),    CFG_DATA_TYPE_U8,  "1",	"rw", 0, 1, "0 - off,1 - on"},
            {"attached_to", &(g_stRunOsdCfg.osdInfo[1][3].achAttachedTo),    CFG_DATA_TYPE_STRING,  "vpss",  "rw", 1, 16, "attached to which module"},
            {"alg_osd_enable", &(g_stRunOsdCfg.osdInfo[1][3].u32AlgOsdEnable),    CFG_DATA_TYPE_S32,  "0",  "rw", 0, 1, "Allow algrithom to draw on this channel"},
            {NULL,},
        }
    },
    //channel 2:
    {
        //stream 0: main
        {
            {"x",       &(g_stRunOsdCfg.osdInfo[2][0].x_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "x ratio: 0.0 - 1.0"},
            {"y",       &(g_stRunOsdCfg.osdInfo[2][0].y_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "y ratio: 0.0 - 1.0"},
            {"osdChannelName_enable",  &(g_stRunOsdCfg.osdInfo[2][0].osdChannelName.enable),  CFG_DATA_TYPE_S32,    "1",   "rw", 0, 1,               NULL},
            {"osdChannelName_text",    &(g_stRunOsdCfg.osdInfo[2][0].osdChannelName.text),    CFG_DATA_TYPE_STRING, "CH2-0", "rw", 1, MAX_STR_LEN_128, NULL},
            {"osdDatetime_enable",     &(g_stRunOsdCfg.osdInfo[2][0].osdDatetime.enable),     CFG_DATA_TYPE_S32, "1",   "rw", 0, 1,    NULL},
            {"osdDatetime_dateFormat", &(g_stRunOsdCfg.osdInfo[2][0].osdDatetime.dateFormat), CFG_DATA_TYPE_U8,  "0",   "rw", 1, 120,  "0:XXXX-XX-XX year mon date; 1:XX-XX-XXXX mon date year 2:XX-XX-XXXX date mon year"},
            {"osdDatetime_dateSprtr",  &(g_stRunOsdCfg.osdInfo[2][0].osdDatetime.dateSprtr),  CFG_DATA_TYPE_U8,  "0",   "rw", 0, 3,    "0 :, 1 -, 2 //, 3 ."},
            {"osdDatetime_timeFmt",    &(g_stRunOsdCfg.osdInfo[2][0].osdDatetime.timeFmt),    CFG_DATA_TYPE_U8,  "0",   "rw", 0, 1,    "0 - 24,1 - 12"},
    	    {"osdDatetime_displayWeek", &(g_stRunOsdCfg.osdInfo[2][0].osdDatetime.displayWeek),    CFG_DATA_TYPE_U8,  "1",	"rw", 0, 1, "0 - off,1 - on"},
            {"attached_to", &(g_stRunOsdCfg.osdInfo[2][0].achAttachedTo),    CFG_DATA_TYPE_STRING,  "vpss",  "rw", 1, 16, "attached to which module"},
            {"alg_osd_enable", &(g_stRunOsdCfg.osdInfo[2][0].u32AlgOsdEnable),    CFG_DATA_TYPE_S32,  "0",  "rw", 0, 1, "Allow algrithom to draw on this channel"},
            {NULL,},
        },
        //stream 1: sub
        {
            {"x",       &(g_stRunOsdCfg.osdInfo[2][1].x_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "x ratio: 0.0 - 1.0"},
            {"y",       &(g_stRunOsdCfg.osdInfo[2][1].y_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "y ratio: 0.0 - 1.0"},
            {"osdChannelName_enable",  &(g_stRunOsdCfg.osdInfo[2][1].osdChannelName.enable),  CFG_DATA_TYPE_S32,    "1",   "rw", 0, 1,               NULL},
            {"osdChannelName_text",    &(g_stRunOsdCfg.osdInfo[2][1].osdChannelName.text),    CFG_DATA_TYPE_STRING, "CH2-1", "rw", 1, MAX_STR_LEN_128, NULL},
            {"osdDatetime_enable",     &(g_stRunOsdCfg.osdInfo[2][1].osdDatetime.enable),     CFG_DATA_TYPE_S32, "1",   "rw", 0, 1,    NULL},
            {"osdDatetime_dateFormat", &(g_stRunOsdCfg.osdInfo[2][1].osdDatetime.dateFormat), CFG_DATA_TYPE_U8,  "0",   "rw", 1, 120,  "0:XXXX-XX-XX year mon date; 1:XX-XX-XXXX mon date year 2:XX-XX-XXXX date mon year"},
            {"osdDatetime_dateSprtr",  &(g_stRunOsdCfg.osdInfo[2][1].osdDatetime.dateSprtr),  CFG_DATA_TYPE_U8,  "0",   "rw", 0, 3,    "0 :, 1 -, 2 //, 3 ."},
            {"osdDatetime_timeFmt",    &(g_stRunOsdCfg.osdInfo[2][1].osdDatetime.timeFmt),    CFG_DATA_TYPE_U8,  "0",   "rw", 0, 1,    "0 - 24,1 - 12"},
            {"osdDatetime_displayWeek", &(g_stRunOsdCfg.osdInfo[2][1].osdDatetime.displayWeek),    CFG_DATA_TYPE_U8,  "1",	"rw", 0, 1, "0 - off,1 - on"},
            {"attached_to", &(g_stRunOsdCfg.osdInfo[2][1].achAttachedTo),    CFG_DATA_TYPE_STRING,  "vpss",  "rw", 1, 16, "attached to which module"},
            {"alg_osd_enable", &(g_stRunOsdCfg.osdInfo[2][1].u32AlgOsdEnable),    CFG_DATA_TYPE_S32,  "0",  "rw", 0, 1, "Allow algrithom to draw on this channel"},
            {NULL,},
        },
        //stream 2 if it exists.
        {
            {"x",       &(g_stRunOsdCfg.osdInfo[2][2].x_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "x ratio: 0.0 - 1.0"},
            {"y",       &(g_stRunOsdCfg.osdInfo[2][2].y_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "y ratio: 0.0 - 1.0"},
            {"osdChannelName_enable",  &(g_stRunOsdCfg.osdInfo[2][2].osdChannelName.enable),  CFG_DATA_TYPE_S32,    "1",   "rw", 0, 1,               NULL},
            {"osdChannelName_text",    &(g_stRunOsdCfg.osdInfo[2][2].osdChannelName.text),    CFG_DATA_TYPE_STRING, "CH2-2", "rw", 1, MAX_STR_LEN_128, NULL},
            {"osdDatetime_enable",     &(g_stRunOsdCfg.osdInfo[2][2].osdDatetime.enable),     CFG_DATA_TYPE_S32, "1",   "rw", 0, 1,    NULL},
            {"osdDatetime_dateFormat", &(g_stRunOsdCfg.osdInfo[2][2].osdDatetime.dateFormat), CFG_DATA_TYPE_U8,  "0",   "rw", 1, 120,  "0:XXXX-XX-XX year mon date; 1:XX-XX-XXXX mon date year 2:XX-XX-XXXX date mon year"},
            {"osdDatetime_dateSprtr",  &(g_stRunOsdCfg.osdInfo[2][2].osdDatetime.dateSprtr),  CFG_DATA_TYPE_U8,  "0",   "rw", 0, 3,    "0 :, 1 -, 2 //, 3 ."},
            {"osdDatetime_timeFmt",    &(g_stRunOsdCfg.osdInfo[2][2].osdDatetime.timeFmt),    CFG_DATA_TYPE_U8,  "0",   "rw", 0, 1,    "0 - 24,1 - 12"},
            {"osdDatetime_displayWeek", &(g_stRunOsdCfg.osdInfo[2][2].osdDatetime.displayWeek),    CFG_DATA_TYPE_U8,  "0",	"rw", 0, 1, "0 - off, 1 - on"},
            {"attached_to", &(g_stRunOsdCfg.osdInfo[2][2].achAttachedTo),    CFG_DATA_TYPE_STRING,  "vpss",  "rw", 1, 16, "attached to which module"},
            {"alg_osd_enable", &(g_stRunOsdCfg.osdInfo[2][2].u32AlgOsdEnable),    CFG_DATA_TYPE_S32,  "0",  "rw", 0, 1, "Allow algrithom to draw on this channel"},
            {NULL,},
        },
        //stream 3 if it exists
        {
            {"x",       &(g_stRunOsdCfg.osdInfo[2][3].x_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "x ratio: 0.0 - 1.0"},
            {"y",       &(g_stRunOsdCfg.osdInfo[2][3].y_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "y ratio: 0.0 - 1.0"},
            {"osdChannelName_enable",  &(g_stRunOsdCfg.osdInfo[2][3].osdChannelName.enable),  CFG_DATA_TYPE_S32,    "0",   "rw", 0, 1,               NULL},
            {"osdChannelName_text",    &(g_stRunOsdCfg.osdInfo[2][3].osdChannelName.text),    CFG_DATA_TYPE_STRING, "CH2-3", "rw", 1, MAX_STR_LEN_128, NULL},
            {"osdDatetime_enable",     &(g_stRunOsdCfg.osdInfo[2][3].osdDatetime.enable),     CFG_DATA_TYPE_S32, "0",   "rw", 0, 1,    NULL},
            {"osdDatetime_dateFormat", &(g_stRunOsdCfg.osdInfo[2][3].osdDatetime.dateFormat), CFG_DATA_TYPE_U8,  "0",   "rw", 1, 120,  "0:XXXX-XX-XX year mon date; 1:XX-XX-XXXX mon date year 2:XX-XX-XXXX date mon year"},
            {"osdDatetime_dateSprtr",  &(g_stRunOsdCfg.osdInfo[2][3].osdDatetime.dateSprtr),  CFG_DATA_TYPE_U8,  "0",   "rw", 0, 3,    "0 :, 1 -, 2 //, 3 ."},
            {"osdDatetime_timeFmt",    &(g_stRunOsdCfg.osdInfo[2][3].osdDatetime.timeFmt),    CFG_DATA_TYPE_U8,  "0",   "rw", 0, 1,    "0 - 24,1 - 12"},
            {"osdDatetime_displayWeek", &(g_stRunOsdCfg.osdInfo[2][3].osdDatetime.displayWeek),    CFG_DATA_TYPE_U8,  "1",	"rw", 0, 1, "0 - off,1 - on"},
            {"attached_to", &(g_stRunOsdCfg.osdInfo[2][3].achAttachedTo),    CFG_DATA_TYPE_STRING,  "vpss",  "rw", 1, 16, "attached to which module"},
            {"alg_osd_enable", &(g_stRunOsdCfg.osdInfo[2][3].u32AlgOsdEnable),    CFG_DATA_TYPE_S32,  "0",  "rw", 0, 1, "Allow algrithom to draw on this channel"},
            {NULL,},
        }
    },
    //channel 3:
    {
        //stream 0: main
        {
            {"x",       &(g_stRunOsdCfg.osdInfo[3][0].x_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "x ratio: 0.0 - 1.0"},
            {"y",       &(g_stRunOsdCfg.osdInfo[3][0].y_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "y ratio: 0.0 - 1.0"},
            {"osdChannelName_enable",  &(g_stRunOsdCfg.osdInfo[3][0].osdChannelName.enable),  CFG_DATA_TYPE_S32,    "1",   "rw", 0, 1,               NULL},
            {"osdChannelName_text",    &(g_stRunOsdCfg.osdInfo[3][0].osdChannelName.text),    CFG_DATA_TYPE_STRING, "CH3-0", "rw", 1, MAX_STR_LEN_128, NULL},
            {"osdDatetime_enable",     &(g_stRunOsdCfg.osdInfo[3][0].osdDatetime.enable),     CFG_DATA_TYPE_S32, "1",   "rw", 0, 1,    NULL},
            {"osdDatetime_dateFormat", &(g_stRunOsdCfg.osdInfo[3][0].osdDatetime.dateFormat), CFG_DATA_TYPE_U8,  "0",   "rw", 1, 120,  "0:XXXX-XX-XX year mon date; 1:XX-XX-XXXX mon date year 2:XX-XX-XXXX date mon year"},
            {"osdDatetime_dateSprtr",  &(g_stRunOsdCfg.osdInfo[3][0].osdDatetime.dateSprtr),  CFG_DATA_TYPE_U8,  "0",   "rw", 0, 3,    "0 :, 1 -, 2 //, 3 ."},
            {"osdDatetime_timeFmt",    &(g_stRunOsdCfg.osdInfo[3][0].osdDatetime.timeFmt),    CFG_DATA_TYPE_U8,  "0",   "rw", 0, 1,    "0 - 24,1 - 12"},
    	    {"osdDatetime_displayWeek", &(g_stRunOsdCfg.osdInfo[3][0].osdDatetime.displayWeek),    CFG_DATA_TYPE_U8,  "1",	"rw", 0, 1, "0 - off,1 - on"},
            {"attached_to", &(g_stRunOsdCfg.osdInfo[3][0].achAttachedTo),    CFG_DATA_TYPE_STRING,  "vpss",  "rw", 1, 16, "attached to which module"},
            {"alg_osd_enable", &(g_stRunOsdCfg.osdInfo[3][0].u32AlgOsdEnable),    CFG_DATA_TYPE_S32,  "0",  "rw", 0, 1, "Allow algrithom to draw on this channel"},
            {NULL,},
        },
        //stream 1: sub
        {
            {"x",       &(g_stRunOsdCfg.osdInfo[3][1].x_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "x ratio: 0.0 - 1.0"},
            {"y",       &(g_stRunOsdCfg.osdInfo[3][1].y_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "y ratio: 0.0 - 1.0"},
            {"osdChannelName_enable",  &(g_stRunOsdCfg.osdInfo[3][1].osdChannelName.enable),  CFG_DATA_TYPE_S32,    "1",   "rw", 0, 1,               NULL},
            {"osdChannelName_text",    &(g_stRunOsdCfg.osdInfo[3][1].osdChannelName.text),    CFG_DATA_TYPE_STRING, "CH3-1", "rw", 1, MAX_STR_LEN_128, NULL},
            {"osdDatetime_enable",     &(g_stRunOsdCfg.osdInfo[3][1].osdDatetime.enable),     CFG_DATA_TYPE_S32, "1",   "rw", 0, 1,    NULL},
            {"osdDatetime_dateFormat", &(g_stRunOsdCfg.osdInfo[3][1].osdDatetime.dateFormat), CFG_DATA_TYPE_U8,  "0",   "rw", 1, 120,  "0:XXXX-XX-XX year mon date; 1:XX-XX-XXXX mon date year 2:XX-XX-XXXX date mon year"},
            {"osdDatetime_dateSprtr",  &(g_stRunOsdCfg.osdInfo[3][1].osdDatetime.dateSprtr),  CFG_DATA_TYPE_U8,  "0",   "rw", 0, 3,    "0 :, 1 -, 2 //, 3 ."},
            {"osdDatetime_timeFmt",    &(g_stRunOsdCfg.osdInfo[3][1].osdDatetime.timeFmt),    CFG_DATA_TYPE_U8,  "0",   "rw", 0, 1,    "0 - 24,1 - 12"},
            {"osdDatetime_displayWeek", &(g_stRunOsdCfg.osdInfo[3][1].osdDatetime.displayWeek),    CFG_DATA_TYPE_U8,  "1",	"rw", 0, 1, "0 - off,1 - on"},
            {"attached_to", &(g_stRunOsdCfg.osdInfo[3][1].achAttachedTo),    CFG_DATA_TYPE_STRING,  "vpss",  "rw", 1, 16, "attached to which module"},
            {"alg_osd_enable", &(g_stRunOsdCfg.osdInfo[3][1].u32AlgOsdEnable),    CFG_DATA_TYPE_S32,  "0",  "rw", 0, 1, "Allow algrithom to draw on this channel"},
            {NULL,},
        },
        //stream 2 if it exists.
        {
            {"x",       &(g_stRunOsdCfg.osdInfo[3][2].x_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "x ratio: 0.0 - 1.0"},
            {"y",       &(g_stRunOsdCfg.osdInfo[3][2].y_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "y ratio: 0.0 - 1.0"},
            {"osdChannelName_enable",  &(g_stRunOsdCfg.osdInfo[3][2].osdChannelName.enable),  CFG_DATA_TYPE_S32,    "0",   "rw", 0, 1,               NULL},
            {"osdChannelName_text",    &(g_stRunOsdCfg.osdInfo[3][2].osdChannelName.text),    CFG_DATA_TYPE_STRING, "CH3-2", "rw", 1, MAX_STR_LEN_128, NULL},
            {"osdDatetime_enable",     &(g_stRunOsdCfg.osdInfo[3][2].osdDatetime.enable),     CFG_DATA_TYPE_S32, "0",   "rw", 0, 1,    NULL},
            {"osdDatetime_dateFormat", &(g_stRunOsdCfg.osdInfo[3][2].osdDatetime.dateFormat), CFG_DATA_TYPE_U8,  "0",   "rw", 1, 120,  "0:XXXX-XX-XX year mon date; 1:XX-XX-XXXX mon date year 2:XX-XX-XXXX date mon year"},
            {"osdDatetime_dateSprtr",  &(g_stRunOsdCfg.osdInfo[3][2].osdDatetime.dateSprtr),  CFG_DATA_TYPE_U8,  "0",   "rw", 0, 3,    "0 :, 1 -, 2 //, 3 ."},
            {"osdDatetime_timeFmt",    &(g_stRunOsdCfg.osdInfo[3][2].osdDatetime.timeFmt),    CFG_DATA_TYPE_U8,  "0",   "rw", 0, 1,    "0 - 24,1 - 12"},
            {"osdDatetime_displayWeek", &(g_stRunOsdCfg.osdInfo[3][2].osdDatetime.displayWeek),    CFG_DATA_TYPE_U8,  "0",	"rw", 0, 1, "0 - off, 1 - on"},
            {"attached_to", &(g_stRunOsdCfg.osdInfo[3][2].achAttachedTo),    CFG_DATA_TYPE_STRING,  "vpss",  "rw", 1, 16, "attached to which module"},
            {"alg_osd_enable", &(g_stRunOsdCfg.osdInfo[3][2].u32AlgOsdEnable),    CFG_DATA_TYPE_S32,  "0",  "rw", 0, 1, "Allow algrithom to draw on this channel"},
            {NULL,},
        },
        //stream 3 if it exists
        {
            {"x",       &(g_stRunOsdCfg.osdInfo[3][3].x_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "x ratio: 0.0 - 1.0"},
            {"y",       &(g_stRunOsdCfg.osdInfo[3][3].y_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "y ratio: 0.0 - 1.0"},
            {"osdChannelName_enable",  &(g_stRunOsdCfg.osdInfo[3][3].osdChannelName.enable),  CFG_DATA_TYPE_S32,    "0",   "rw", 0, 1,               NULL},
            {"osdChannelName_text",    &(g_stRunOsdCfg.osdInfo[3][3].osdChannelName.text),    CFG_DATA_TYPE_STRING, "CH3-3", "rw", 1, MAX_STR_LEN_128, NULL},
            {"osdDatetime_enable",     &(g_stRunOsdCfg.osdInfo[3][3].osdDatetime.enable),     CFG_DATA_TYPE_S32, "0",   "rw", 0, 1,    NULL},
            {"osdDatetime_dateFormat", &(g_stRunOsdCfg.osdInfo[3][3].osdDatetime.dateFormat), CFG_DATA_TYPE_U8,  "0",   "rw", 1, 120,  "0:XXXX-XX-XX year mon date; 1:XX-XX-XXXX mon date year 2:XX-XX-XXXX date mon year"},
            {"osdDatetime_dateSprtr",  &(g_stRunOsdCfg.osdInfo[3][3].osdDatetime.dateSprtr),  CFG_DATA_TYPE_U8,  "0",   "rw", 0, 3,    "0 :, 1 -, 2 //, 3 ."},
            {"osdDatetime_timeFmt",    &(g_stRunOsdCfg.osdInfo[3][3].osdDatetime.timeFmt),    CFG_DATA_TYPE_U8,  "0",   "rw", 0, 1,    "0 - 24,1 - 12"},
            {"osdDatetime_displayWeek", &(g_stRunOsdCfg.osdInfo[3][3].osdDatetime.displayWeek),    CFG_DATA_TYPE_U8,  "1",	"rw", 0, 1, "0 - off,1 - on"},
            {"attached_to", &(g_stRunOsdCfg.osdInfo[3][3].achAttachedTo),    CFG_DATA_TYPE_STRING,  "vpss",  "rw", 1, 16, "attached to which module"},
            {"alg_osd_enable", &(g_stRunOsdCfg.osdInfo[3][3].u32AlgOsdEnable),    CFG_DATA_TYPE_S32,  "0",  "rw", 0, 1, "Allow algrithom to draw on this channel"},
            {NULL,},
        }
    },
    //channel 4:
    {
        //stream 0: main
        {
            {"x",       &(g_stRunOsdCfg.osdInfo[4][0].x_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "x ratio: 0.0 - 1.0"},
            {"y",       &(g_stRunOsdCfg.osdInfo[4][0].y_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "y ratio: 0.0 - 1.0"},
            {"osdChannelName_enable",  &(g_stRunOsdCfg.osdInfo[4][0].osdChannelName.enable),  CFG_DATA_TYPE_S32,    "1",   "rw", 0, 1,               NULL},
            {"osdChannelName_text",    &(g_stRunOsdCfg.osdInfo[4][0].osdChannelName.text),    CFG_DATA_TYPE_STRING, "CH4-0", "rw", 1, MAX_STR_LEN_128, NULL},
            {"osdDatetime_enable",     &(g_stRunOsdCfg.osdInfo[4][0].osdDatetime.enable),     CFG_DATA_TYPE_S32, "1",   "rw", 0, 1,    NULL},
            {"osdDatetime_dateFormat", &(g_stRunOsdCfg.osdInfo[4][0].osdDatetime.dateFormat), CFG_DATA_TYPE_U8,  "0",   "rw", 1, 120,  "0:XXXX-XX-XX year mon date; 1:XX-XX-XXXX mon date year 2:XX-XX-XXXX date mon year"},
            {"osdDatetime_dateSprtr",  &(g_stRunOsdCfg.osdInfo[4][0].osdDatetime.dateSprtr),  CFG_DATA_TYPE_U8,  "0",   "rw", 0, 3,    "0 :, 1 -, 2 //, 3 ."},
            {"osdDatetime_timeFmt",    &(g_stRunOsdCfg.osdInfo[4][0].osdDatetime.timeFmt),    CFG_DATA_TYPE_U8,  "0",   "rw", 0, 1,    "0 - 24,1 - 12"},
    	    {"osdDatetime_displayWeek", &(g_stRunOsdCfg.osdInfo[4][0].osdDatetime.displayWeek),    CFG_DATA_TYPE_U8,  "1",	"rw", 0, 1, "0 - off,1 - on"},
            {"attached_to", &(g_stRunOsdCfg.osdInfo[4][0].achAttachedTo),    CFG_DATA_TYPE_STRING,  "vpss",  "rw", 1, 16, "attached to which module"},
            {"alg_osd_enable", &(g_stRunOsdCfg.osdInfo[4][0].u32AlgOsdEnable),    CFG_DATA_TYPE_S32,  "0",  "rw", 0, 1, "Allow algrithom to draw on this channel"},
            {NULL,},
        },
        //stream 1: sub
        {
            {"x",       &(g_stRunOsdCfg.osdInfo[4][1].x_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "x ratio: 0.0 - 1.0"},
            {"y",       &(g_stRunOsdCfg.osdInfo[4][1].y_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "y ratio: 0.0 - 1.0"},
            {"osdChannelName_enable",  &(g_stRunOsdCfg.osdInfo[4][1].osdChannelName.enable),  CFG_DATA_TYPE_S32,    "1",   "rw", 0, 1,               NULL},
            {"osdChannelName_text",    &(g_stRunOsdCfg.osdInfo[4][1].osdChannelName.text),    CFG_DATA_TYPE_STRING, "CH4-1", "rw", 1, MAX_STR_LEN_128, NULL},
            {"osdDatetime_enable",     &(g_stRunOsdCfg.osdInfo[4][1].osdDatetime.enable),     CFG_DATA_TYPE_S32, "1",   "rw", 0, 1,    NULL},
            {"osdDatetime_dateFormat", &(g_stRunOsdCfg.osdInfo[4][1].osdDatetime.dateFormat), CFG_DATA_TYPE_U8,  "0",   "rw", 1, 120,  "0:XXXX-XX-XX year mon date; 1:XX-XX-XXXX mon date year 2:XX-XX-XXXX date mon year"},
            {"osdDatetime_dateSprtr",  &(g_stRunOsdCfg.osdInfo[4][1].osdDatetime.dateSprtr),  CFG_DATA_TYPE_U8,  "0",   "rw", 0, 3,    "0 :, 1 -, 2 //, 3 ."},
            {"osdDatetime_timeFmt",    &(g_stRunOsdCfg.osdInfo[4][1].osdDatetime.timeFmt),    CFG_DATA_TYPE_U8,  "0",   "rw", 0, 1,    "0 - 24,1 - 12"},
            {"osdDatetime_displayWeek", &(g_stRunOsdCfg.osdInfo[4][1].osdDatetime.displayWeek),    CFG_DATA_TYPE_U8,  "1",	"rw", 0, 1, "0 - off,1 - on"},
            {"attached_to", &(g_stRunOsdCfg.osdInfo[4][1].achAttachedTo),    CFG_DATA_TYPE_STRING,  "vpss",  "rw", 1, 16, "attached to which module"},
            {"alg_osd_enable", &(g_stRunOsdCfg.osdInfo[4][1].u32AlgOsdEnable),    CFG_DATA_TYPE_S32,  "0",  "rw", 0, 1, "Allow algrithom to draw on this channel"},
            {NULL,},
        },
        //stream 2 if it exists.
        {
            {"x",       &(g_stRunOsdCfg.osdInfo[4][2].x_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "x ratio: 0.0 - 1.0"},
            {"y",       &(g_stRunOsdCfg.osdInfo[4][2].y_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "y ratio: 0.0 - 1.0"},
            {"osdChannelName_enable",  &(g_stRunOsdCfg.osdInfo[4][2].osdChannelName.enable),  CFG_DATA_TYPE_S32,    "0",   "rw", 0, 1,               NULL},
            {"osdChannelName_text",    &(g_stRunOsdCfg.osdInfo[4][2].osdChannelName.text),    CFG_DATA_TYPE_STRING, "CH4-2", "rw", 1, MAX_STR_LEN_128, NULL},
            {"osdDatetime_enable",     &(g_stRunOsdCfg.osdInfo[4][2].osdDatetime.enable),     CFG_DATA_TYPE_S32, "0",   "rw", 0, 1,    NULL},
            {"osdDatetime_dateFormat", &(g_stRunOsdCfg.osdInfo[4][2].osdDatetime.dateFormat), CFG_DATA_TYPE_U8,  "0",   "rw", 1, 120,  "0:XXXX-XX-XX year mon date; 1:XX-XX-XXXX mon date year 2:XX-XX-XXXX date mon year"},
            {"osdDatetime_dateSprtr",  &(g_stRunOsdCfg.osdInfo[4][2].osdDatetime.dateSprtr),  CFG_DATA_TYPE_U8,  "0",   "rw", 0, 3,    "0 :, 1 -, 2 //, 3 ."},
            {"osdDatetime_timeFmt",    &(g_stRunOsdCfg.osdInfo[4][2].osdDatetime.timeFmt),    CFG_DATA_TYPE_U8,  "0",   "rw", 0, 1,    "0 - 24,1 - 12"},
            {"osdDatetime_displayWeek", &(g_stRunOsdCfg.osdInfo[4][2].osdDatetime.displayWeek),    CFG_DATA_TYPE_U8,  "0",	"rw", 0, 1, "0 - off, 1 - on"},
            {"attached_to", &(g_stRunOsdCfg.osdInfo[4][2].achAttachedTo),    CFG_DATA_TYPE_STRING,  "vpss",  "rw", 1, 16, "attached to which module"},
            {"alg_osd_enable", &(g_stRunOsdCfg.osdInfo[4][2].u32AlgOsdEnable),    CFG_DATA_TYPE_S32,  "0",  "rw", 0, 1, "Allow algrithom to draw on this channel"},
            {NULL,},
        },
        //stream 3 if it exists
        {
            {"x",       &(g_stRunOsdCfg.osdInfo[4][3].x_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "x ratio: 0.0 - 1.0"},
            {"y",       &(g_stRunOsdCfg.osdInfo[4][3].y_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "y ratio: 0.0 - 1.0"},
            {"osdChannelName_enable",  &(g_stRunOsdCfg.osdInfo[4][3].osdChannelName.enable),  CFG_DATA_TYPE_S32,    "0",   "rw", 0, 1,               NULL},
            {"osdChannelName_text",    &(g_stRunOsdCfg.osdInfo[4][3].osdChannelName.text),    CFG_DATA_TYPE_STRING, "CH4-3", "rw", 1, MAX_STR_LEN_128, NULL},
            {"osdDatetime_enable",     &(g_stRunOsdCfg.osdInfo[4][3].osdDatetime.enable),     CFG_DATA_TYPE_S32, "0",   "rw", 0, 1,    NULL},
            {"osdDatetime_dateFormat", &(g_stRunOsdCfg.osdInfo[4][3].osdDatetime.dateFormat), CFG_DATA_TYPE_U8,  "0",   "rw", 1, 120,  "0:XXXX-XX-XX year mon date; 1:XX-XX-XXXX mon date year 2:XX-XX-XXXX date mon year"},
            {"osdDatetime_dateSprtr",  &(g_stRunOsdCfg.osdInfo[4][3].osdDatetime.dateSprtr),  CFG_DATA_TYPE_U8,  "0",   "rw", 0, 3,    "0 :, 1 -, 2 //, 3 ."},
            {"osdDatetime_timeFmt",    &(g_stRunOsdCfg.osdInfo[4][3].osdDatetime.timeFmt),    CFG_DATA_TYPE_U8,  "0",   "rw", 0, 1,    "0 - 24,1 - 12"},
            {"osdDatetime_displayWeek", &(g_stRunOsdCfg.osdInfo[4][3].osdDatetime.displayWeek),    CFG_DATA_TYPE_U8,  "1",	"rw", 0, 1, "0 - off,1 - on"},
            {"attached_to", &(g_stRunOsdCfg.osdInfo[4][3].achAttachedTo),    CFG_DATA_TYPE_STRING,  "vpss",  "rw", 1, 16, "attached to which module"},
            {"alg_osd_enable", &(g_stRunOsdCfg.osdInfo[4][3].u32AlgOsdEnable),    CFG_DATA_TYPE_S32,  "0",  "rw", 0, 1, "Allow algrithom to draw on this channel"},
            {NULL,},
        }
    },
    //channel 5:
    {
        //stream 0: main
        {
            {"x",       &(g_stRunOsdCfg.osdInfo[5][0].x_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "x ratio: 0.0 - 1.0"},
            {"y",       &(g_stRunOsdCfg.osdInfo[5][0].y_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "y ratio: 0.0 - 1.0"},
            {"osdChannelName_enable",  &(g_stRunOsdCfg.osdInfo[5][0].osdChannelName.enable),  CFG_DATA_TYPE_S32,    "1",   "rw", 0, 1,               NULL},
            {"osdChannelName_text",    &(g_stRunOsdCfg.osdInfo[5][0].osdChannelName.text),    CFG_DATA_TYPE_STRING, "CH5-0", "rw", 1, MAX_STR_LEN_128, NULL},
            {"osdDatetime_enable",     &(g_stRunOsdCfg.osdInfo[5][0].osdDatetime.enable),     CFG_DATA_TYPE_S32, "1",   "rw", 0, 1,    NULL},
            {"osdDatetime_dateFormat", &(g_stRunOsdCfg.osdInfo[5][0].osdDatetime.dateFormat), CFG_DATA_TYPE_U8,  "0",   "rw", 1, 120,  "0:XXXX-XX-XX year mon date; 1:XX-XX-XXXX mon date year 2:XX-XX-XXXX date mon year"},
            {"osdDatetime_dateSprtr",  &(g_stRunOsdCfg.osdInfo[5][0].osdDatetime.dateSprtr),  CFG_DATA_TYPE_U8,  "0",   "rw", 0, 3,    "0 :, 1 -, 2 //, 3 ."},
            {"osdDatetime_timeFmt",    &(g_stRunOsdCfg.osdInfo[5][0].osdDatetime.timeFmt),    CFG_DATA_TYPE_U8,  "0",   "rw", 0, 1,    "0 - 24,1 - 12"},
    	    {"osdDatetime_displayWeek", &(g_stRunOsdCfg.osdInfo[5][0].osdDatetime.displayWeek),    CFG_DATA_TYPE_U8,  "1",	"rw", 0, 1, "0 - off,1 - on"},
            {"attached_to", &(g_stRunOsdCfg.osdInfo[5][0].achAttachedTo),    CFG_DATA_TYPE_STRING,  "vpss",  "rw", 1, 16, "attached to which module"},
            {"alg_osd_enable", &(g_stRunOsdCfg.osdInfo[5][0].u32AlgOsdEnable),    CFG_DATA_TYPE_S32,  "0",  "rw", 0, 1, "Allow algrithom to draw on this channel"},
            {NULL,},
        },
        //stream 1: sub
        {
            {"x",       &(g_stRunOsdCfg.osdInfo[5][1].x_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "x ratio: 0.0 - 1.0"},
            {"y",       &(g_stRunOsdCfg.osdInfo[5][1].y_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "y ratio: 0.0 - 1.0"},
            {"osdChannelName_enable",  &(g_stRunOsdCfg.osdInfo[5][1].osdChannelName.enable),  CFG_DATA_TYPE_S32,    "1",   "rw", 0, 1,               NULL},
            {"osdChannelName_text",    &(g_stRunOsdCfg.osdInfo[5][1].osdChannelName.text),    CFG_DATA_TYPE_STRING, "CH5-1", "rw", 1, MAX_STR_LEN_128, NULL},
            {"osdDatetime_enable",     &(g_stRunOsdCfg.osdInfo[5][1].osdDatetime.enable),     CFG_DATA_TYPE_S32, "1",   "rw", 0, 1,    NULL},
            {"osdDatetime_dateFormat", &(g_stRunOsdCfg.osdInfo[5][1].osdDatetime.dateFormat), CFG_DATA_TYPE_U8,  "0",   "rw", 1, 120,  "0:XXXX-XX-XX year mon date; 1:XX-XX-XXXX mon date year 2:XX-XX-XXXX date mon year"},
            {"osdDatetime_dateSprtr",  &(g_stRunOsdCfg.osdInfo[5][1].osdDatetime.dateSprtr),  CFG_DATA_TYPE_U8,  "0",   "rw", 0, 3,    "0 :, 1 -, 2 //, 3 ."},
            {"osdDatetime_timeFmt",    &(g_stRunOsdCfg.osdInfo[5][1].osdDatetime.timeFmt),    CFG_DATA_TYPE_U8,  "0",   "rw", 0, 1,    "0 - 24,1 - 12"},
            {"osdDatetime_displayWeek", &(g_stRunOsdCfg.osdInfo[5][1].osdDatetime.displayWeek),    CFG_DATA_TYPE_U8,  "1",	"rw", 0, 1, "0 - off,1 - on"},
            {"attached_to", &(g_stRunOsdCfg.osdInfo[5][1].achAttachedTo),    CFG_DATA_TYPE_STRING,  "vpss",  "rw", 1, 16, "attached to which module"},
            {"alg_osd_enable", &(g_stRunOsdCfg.osdInfo[5][1].u32AlgOsdEnable),    CFG_DATA_TYPE_S32,  "0",  "rw", 0, 1, "Allow algrithom to draw on this channel"},
            {NULL,},
        },
        //stream 2 if it exists.
        {
            {"x",       &(g_stRunOsdCfg.osdInfo[5][2].x_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "x ratio: 0.0 - 1.0"},
            {"y",       &(g_stRunOsdCfg.osdInfo[5][2].y_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "y ratio: 0.0 - 1.0"},
            {"osdChannelName_enable",  &(g_stRunOsdCfg.osdInfo[5][2].osdChannelName.enable),  CFG_DATA_TYPE_S32,    "0",   "rw", 0, 1,               NULL},
            {"osdChannelName_text",    &(g_stRunOsdCfg.osdInfo[5][2].osdChannelName.text),    CFG_DATA_TYPE_STRING, "CH5-2", "rw", 1, MAX_STR_LEN_128, NULL},
            {"osdDatetime_enable",     &(g_stRunOsdCfg.osdInfo[5][2].osdDatetime.enable),     CFG_DATA_TYPE_S32, "0",   "rw", 0, 1,    NULL},
            {"osdDatetime_dateFormat", &(g_stRunOsdCfg.osdInfo[5][2].osdDatetime.dateFormat), CFG_DATA_TYPE_U8,  "0",   "rw", 1, 120,  "0:XXXX-XX-XX year mon date; 1:XX-XX-XXXX mon date year 2:XX-XX-XXXX date mon year"},
            {"osdDatetime_dateSprtr",  &(g_stRunOsdCfg.osdInfo[5][2].osdDatetime.dateSprtr),  CFG_DATA_TYPE_U8,  "0",   "rw", 0, 3,    "0 :, 1 -, 2 //, 3 ."},
            {"osdDatetime_timeFmt",    &(g_stRunOsdCfg.osdInfo[5][2].osdDatetime.timeFmt),    CFG_DATA_TYPE_U8,  "0",   "rw", 0, 1,    "0 - 24,1 - 12"},
            {"osdDatetime_displayWeek", &(g_stRunOsdCfg.osdInfo[5][2].osdDatetime.displayWeek),    CFG_DATA_TYPE_U8,  "0",	"rw", 0, 1, "0 - off, 1 - on"},
            {"attached_to", &(g_stRunOsdCfg.osdInfo[5][2].achAttachedTo),    CFG_DATA_TYPE_STRING,  "vpss",  "rw", 1, 16, "attached to which module"},
            {"alg_osd_enable", &(g_stRunOsdCfg.osdInfo[5][2].u32AlgOsdEnable),    CFG_DATA_TYPE_S32,  "0",  "rw", 0, 1, "Allow algrithom to draw on this channel"},
            {NULL,},
        },
        //stream 3 if it exists
        {
            {"x",       &(g_stRunOsdCfg.osdInfo[5][3].x_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "x ratio: 0.0 - 1.0"},
            {"y",       &(g_stRunOsdCfg.osdInfo[5][3].y_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "y ratio: 0.0 - 1.0"},
            {"osdChannelName_enable",  &(g_stRunOsdCfg.osdInfo[5][3].osdChannelName.enable),  CFG_DATA_TYPE_S32,    "0",   "rw", 0, 1,               NULL},
            {"osdChannelName_text",    &(g_stRunOsdCfg.osdInfo[5][3].osdChannelName.text),    CFG_DATA_TYPE_STRING, "CH5-3", "rw", 1, MAX_STR_LEN_128, NULL},
            {"osdDatetime_enable",     &(g_stRunOsdCfg.osdInfo[5][3].osdDatetime.enable),     CFG_DATA_TYPE_S32, "0",   "rw", 0, 1,    NULL},
            {"osdDatetime_dateFormat", &(g_stRunOsdCfg.osdInfo[5][3].osdDatetime.dateFormat), CFG_DATA_TYPE_U8,  "0",   "rw", 1, 120,  "0:XXXX-XX-XX year mon date; 1:XX-XX-XXXX mon date year 2:XX-XX-XXXX date mon year"},
            {"osdDatetime_dateSprtr",  &(g_stRunOsdCfg.osdInfo[5][3].osdDatetime.dateSprtr),  CFG_DATA_TYPE_U8,  "0",   "rw", 0, 3,    "0 :, 1 -, 2 //, 3 ."},
            {"osdDatetime_timeFmt",    &(g_stRunOsdCfg.osdInfo[5][3].osdDatetime.timeFmt),    CFG_DATA_TYPE_U8,  "0",   "rw", 0, 1,    "0 - 24,1 - 12"},
            {"osdDatetime_displayWeek", &(g_stRunOsdCfg.osdInfo[5][3].osdDatetime.displayWeek),    CFG_DATA_TYPE_U8,  "1",	"rw", 0, 1, "0 - off,1 - on"},
            {"attached_to", &(g_stRunOsdCfg.osdInfo[5][3].achAttachedTo),    CFG_DATA_TYPE_STRING,  "vpss",  "rw", 1, 16, "attached to which module"},
            {"alg_osd_enable", &(g_stRunOsdCfg.osdInfo[5][3].u32AlgOsdEnable),    CFG_DATA_TYPE_S32,  "0",  "rw", 0, 1, "Allow algrithom to draw on this channel"},
            {NULL,},
        }
    },
    //channel 6:
    {
        //stream 0: main
        {
            {"x",       &(g_stRunOsdCfg.osdInfo[6][0].x_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "x ratio: 0.0 - 1.0"},
            {"y",       &(g_stRunOsdCfg.osdInfo[6][0].y_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "y ratio: 0.0 - 1.0"},
            {"osdChannelName_enable",  &(g_stRunOsdCfg.osdInfo[6][0].osdChannelName.enable),  CFG_DATA_TYPE_S32,    "1",   "rw", 0, 1,               NULL},
            {"osdChannelName_text",    &(g_stRunOsdCfg.osdInfo[6][0].osdChannelName.text),    CFG_DATA_TYPE_STRING, "CH6-0", "rw", 1, MAX_STR_LEN_128, NULL},
            {"osdDatetime_enable",     &(g_stRunOsdCfg.osdInfo[6][0].osdDatetime.enable),     CFG_DATA_TYPE_S32, "1",   "rw", 0, 1,    NULL},
            {"osdDatetime_dateFormat", &(g_stRunOsdCfg.osdInfo[6][0].osdDatetime.dateFormat), CFG_DATA_TYPE_U8,  "0",   "rw", 1, 120,  "0:XXXX-XX-XX year mon date; 1:XX-XX-XXXX mon date year 2:XX-XX-XXXX date mon year"},
            {"osdDatetime_dateSprtr",  &(g_stRunOsdCfg.osdInfo[6][0].osdDatetime.dateSprtr),  CFG_DATA_TYPE_U8,  "0",   "rw", 0, 3,    "0 :, 1 -, 2 //, 3 ."},
            {"osdDatetime_timeFmt",    &(g_stRunOsdCfg.osdInfo[6][0].osdDatetime.timeFmt),    CFG_DATA_TYPE_U8,  "0",   "rw", 0, 1,    "0 - 24,1 - 12"},
    	    {"osdDatetime_displayWeek", &(g_stRunOsdCfg.osdInfo[6][0].osdDatetime.displayWeek),    CFG_DATA_TYPE_U8,  "1",	"rw", 0, 1, "0 - off,1 - on"},
            {"attached_to", &(g_stRunOsdCfg.osdInfo[6][0].achAttachedTo),    CFG_DATA_TYPE_STRING,  "vpss",  "rw", 1, 16, "attached to which module"},
            {"alg_osd_enable", &(g_stRunOsdCfg.osdInfo[6][0].u32AlgOsdEnable),    CFG_DATA_TYPE_S32,  "0",  "rw", 0, 1, "Allow algrithom to draw on this channel"},
            {NULL,},
        },
        //stream 1: sub
        {
            {"x",       &(g_stRunOsdCfg.osdInfo[6][1].x_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "x ratio: 0.0 - 1.0"},
            {"y",       &(g_stRunOsdCfg.osdInfo[6][1].y_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "y ratio: 0.0 - 1.0"},
            {"osdChannelName_enable",  &(g_stRunOsdCfg.osdInfo[6][1].osdChannelName.enable),  CFG_DATA_TYPE_S32,    "1",   "rw", 0, 1,               NULL},
            {"osdChannelName_text",    &(g_stRunOsdCfg.osdInfo[6][1].osdChannelName.text),    CFG_DATA_TYPE_STRING, "CH6-1", "rw", 1, MAX_STR_LEN_128, NULL},
            {"osdDatetime_enable",     &(g_stRunOsdCfg.osdInfo[6][1].osdDatetime.enable),     CFG_DATA_TYPE_S32, "1",   "rw", 0, 1,    NULL},
            {"osdDatetime_dateFormat", &(g_stRunOsdCfg.osdInfo[6][1].osdDatetime.dateFormat), CFG_DATA_TYPE_U8,  "0",   "rw", 1, 120,  "0:XXXX-XX-XX year mon date; 1:XX-XX-XXXX mon date year 2:XX-XX-XXXX date mon year"},
            {"osdDatetime_dateSprtr",  &(g_stRunOsdCfg.osdInfo[6][1].osdDatetime.dateSprtr),  CFG_DATA_TYPE_U8,  "0",   "rw", 0, 3,    "0 :, 1 -, 2 //, 3 ."},
            {"osdDatetime_timeFmt",    &(g_stRunOsdCfg.osdInfo[6][1].osdDatetime.timeFmt),    CFG_DATA_TYPE_U8,  "0",   "rw", 0, 1,    "0 - 24,1 - 12"},
            {"osdDatetime_displayWeek", &(g_stRunOsdCfg.osdInfo[6][1].osdDatetime.displayWeek),    CFG_DATA_TYPE_U8,  "1",	"rw", 0, 1, "0 - off,1 - on"},
            {"attached_to", &(g_stRunOsdCfg.osdInfo[6][1].achAttachedTo),    CFG_DATA_TYPE_STRING,  "vpss",  "rw", 1, 16, "attached to which module"},
            {"alg_osd_enable", &(g_stRunOsdCfg.osdInfo[6][1].u32AlgOsdEnable),    CFG_DATA_TYPE_S32,  "0",  "rw", 0, 1, "Allow algrithom to draw on this channel"},
            {NULL,},
        },
        //stream 2 if it exists.
        {
            {"x",       &(g_stRunOsdCfg.osdInfo[6][2].x_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "x ratio: 0.0 - 1.0"},
            {"y",       &(g_stRunOsdCfg.osdInfo[6][2].y_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "y ratio: 0.0 - 1.0"},
            {"osdChannelName_enable",  &(g_stRunOsdCfg.osdInfo[6][2].osdChannelName.enable),  CFG_DATA_TYPE_S32,    "0",   "rw", 0, 1,               NULL},
            {"osdChannelName_text",    &(g_stRunOsdCfg.osdInfo[6][2].osdChannelName.text),    CFG_DATA_TYPE_STRING, "CH6-2", "rw", 1, MAX_STR_LEN_128, NULL},
            {"osdDatetime_enable",     &(g_stRunOsdCfg.osdInfo[6][2].osdDatetime.enable),     CFG_DATA_TYPE_S32, "0",   "rw", 0, 1,    NULL},
            {"osdDatetime_dateFormat", &(g_stRunOsdCfg.osdInfo[6][2].osdDatetime.dateFormat), CFG_DATA_TYPE_U8,  "0",   "rw", 1, 120,  "0:XXXX-XX-XX year mon date; 1:XX-XX-XXXX mon date year 2:XX-XX-XXXX date mon year"},
            {"osdDatetime_dateSprtr",  &(g_stRunOsdCfg.osdInfo[6][2].osdDatetime.dateSprtr),  CFG_DATA_TYPE_U8,  "0",   "rw", 0, 3,    "0 :, 1 -, 2 //, 3 ."},
            {"osdDatetime_timeFmt",    &(g_stRunOsdCfg.osdInfo[6][2].osdDatetime.timeFmt),    CFG_DATA_TYPE_U8,  "0",   "rw", 0, 1,    "0 - 24,1 - 12"},
            {"osdDatetime_displayWeek", &(g_stRunOsdCfg.osdInfo[6][2].osdDatetime.displayWeek),    CFG_DATA_TYPE_U8,  "0",	"rw", 0, 1, "0 - off, 1 - on"},
            {"attached_to", &(g_stRunOsdCfg.osdInfo[6][2].achAttachedTo),    CFG_DATA_TYPE_STRING,  "vpss",  "rw", 1, 16, "attached to which module"},
            {"alg_osd_enable", &(g_stRunOsdCfg.osdInfo[6][2].u32AlgOsdEnable),    CFG_DATA_TYPE_S32,  "0",  "rw", 0, 1, "Allow algrithom to draw on this channel"},
            {NULL,},
        },
        //stream 3 if it exists
        {
            {"x",       &(g_stRunOsdCfg.osdInfo[6][3].x_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "x ratio: 0.0 - 1.0"},
            {"y",       &(g_stRunOsdCfg.osdInfo[6][3].y_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "y ratio: 0.0 - 1.0"},
            {"osdChannelName_enable",  &(g_stRunOsdCfg.osdInfo[6][3].osdChannelName.enable),  CFG_DATA_TYPE_S32,    "0",   "rw", 0, 1,               NULL},
            {"osdChannelName_text",    &(g_stRunOsdCfg.osdInfo[6][3].osdChannelName.text),    CFG_DATA_TYPE_STRING, "CH6-3", "rw", 1, MAX_STR_LEN_128, NULL},
            {"osdDatetime_enable",     &(g_stRunOsdCfg.osdInfo[6][3].osdDatetime.enable),     CFG_DATA_TYPE_S32, "0",   "rw", 0, 1,    NULL},
            {"osdDatetime_dateFormat", &(g_stRunOsdCfg.osdInfo[6][3].osdDatetime.dateFormat), CFG_DATA_TYPE_U8,  "0",   "rw", 1, 120,  "0:XXXX-XX-XX year mon date; 1:XX-XX-XXXX mon date year 2:XX-XX-XXXX date mon year"},
            {"osdDatetime_dateSprtr",  &(g_stRunOsdCfg.osdInfo[6][3].osdDatetime.dateSprtr),  CFG_DATA_TYPE_U8,  "0",   "rw", 0, 3,    "0 :, 1 -, 2 //, 3 ."},
            {"osdDatetime_timeFmt",    &(g_stRunOsdCfg.osdInfo[6][3].osdDatetime.timeFmt),    CFG_DATA_TYPE_U8,  "0",   "rw", 0, 1,    "0 - 24,1 - 12"},
            {"osdDatetime_displayWeek", &(g_stRunOsdCfg.osdInfo[6][3].osdDatetime.displayWeek),    CFG_DATA_TYPE_U8,  "1",	"rw", 0, 1, "0 - off,1 - on"},
            {"attached_to", &(g_stRunOsdCfg.osdInfo[6][3].achAttachedTo),    CFG_DATA_TYPE_STRING,  "vpss",  "rw", 1, 16, "attached to which module"},
            {"alg_osd_enable", &(g_stRunOsdCfg.osdInfo[6][3].u32AlgOsdEnable),    CFG_DATA_TYPE_S32,  "0",  "rw", 0, 1, "Allow algrithom to draw on this channel"},
            {NULL,},
        }
    },
    //channel 7:
    {
        //stream 0: main
        {
            {"x",       &(g_stRunOsdCfg.osdInfo[7][0].x_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "x ratio: 0.0 - 1.0"},
            {"y",       &(g_stRunOsdCfg.osdInfo[7][0].y_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "y ratio: 0.0 - 1.0"},
            {"osdChannelName_enable",  &(g_stRunOsdCfg.osdInfo[7][0].osdChannelName.enable),  CFG_DATA_TYPE_S32,    "1",   "rw", 0, 1,               NULL},
            {"osdChannelName_text",    &(g_stRunOsdCfg.osdInfo[7][0].osdChannelName.text),    CFG_DATA_TYPE_STRING, "CH7-0", "rw", 1, MAX_STR_LEN_128, NULL},
            {"osdDatetime_enable",     &(g_stRunOsdCfg.osdInfo[7][0].osdDatetime.enable),     CFG_DATA_TYPE_S32, "1",   "rw", 0, 1,    NULL},
            {"osdDatetime_dateFormat", &(g_stRunOsdCfg.osdInfo[7][0].osdDatetime.dateFormat), CFG_DATA_TYPE_U8,  "0",   "rw", 1, 120,  "0:XXXX-XX-XX year mon date; 1:XX-XX-XXXX mon date year 2:XX-XX-XXXX date mon year"},
            {"osdDatetime_dateSprtr",  &(g_stRunOsdCfg.osdInfo[7][0].osdDatetime.dateSprtr),  CFG_DATA_TYPE_U8,  "0",   "rw", 0, 3,    "0 :, 1 -, 2 //, 3 ."},
            {"osdDatetime_timeFmt",    &(g_stRunOsdCfg.osdInfo[7][0].osdDatetime.timeFmt),    CFG_DATA_TYPE_U8,  "0",   "rw", 0, 1,    "0 - 24,1 - 12"},
    	    {"osdDatetime_displayWeek", &(g_stRunOsdCfg.osdInfo[7][0].osdDatetime.displayWeek),    CFG_DATA_TYPE_U8,  "1",	"rw", 0, 1, "0 - off,1 - on"},
            {"attached_to", &(g_stRunOsdCfg.osdInfo[7][0].achAttachedTo),    CFG_DATA_TYPE_STRING,  "vpss",  "rw", 1, 16, "attached to which module"},
            {"alg_osd_enable", &(g_stRunOsdCfg.osdInfo[7][0].u32AlgOsdEnable),    CFG_DATA_TYPE_S32,  "0",  "rw", 0, 1, "Allow algrithom to draw on this channel"},
            {NULL,},
        },
        //stream 1: sub
        {
            {"x",       &(g_stRunOsdCfg.osdInfo[7][1].x_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "x ratio: 0.0 - 1.0"},
            {"y",       &(g_stRunOsdCfg.osdInfo[7][1].y_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "y ratio: 0.0 - 1.0"},
            {"osdChannelName_enable",  &(g_stRunOsdCfg.osdInfo[7][1].osdChannelName.enable),  CFG_DATA_TYPE_S32,    "1",   "rw", 0, 1,               NULL},
            {"osdChannelName_text",    &(g_stRunOsdCfg.osdInfo[7][1].osdChannelName.text),    CFG_DATA_TYPE_STRING, "CH7-1", "rw", 1, MAX_STR_LEN_128, NULL},
            {"osdDatetime_enable",     &(g_stRunOsdCfg.osdInfo[7][1].osdDatetime.enable),     CFG_DATA_TYPE_S32, "1",   "rw", 0, 1,    NULL},
            {"osdDatetime_dateFormat", &(g_stRunOsdCfg.osdInfo[7][1].osdDatetime.dateFormat), CFG_DATA_TYPE_U8,  "0",   "rw", 1, 120,  "0:XXXX-XX-XX year mon date; 1:XX-XX-XXXX mon date year 2:XX-XX-XXXX date mon year"},
            {"osdDatetime_dateSprtr",  &(g_stRunOsdCfg.osdInfo[7][1].osdDatetime.dateSprtr),  CFG_DATA_TYPE_U8,  "0",   "rw", 0, 3,    "0 :, 1 -, 2 //, 3 ."},
            {"osdDatetime_timeFmt",    &(g_stRunOsdCfg.osdInfo[7][1].osdDatetime.timeFmt),    CFG_DATA_TYPE_U8,  "0",   "rw", 0, 1,    "0 - 24,1 - 12"},
            {"osdDatetime_displayWeek", &(g_stRunOsdCfg.osdInfo[7][1].osdDatetime.displayWeek),    CFG_DATA_TYPE_U8,  "1",	"rw", 0, 1, "0 - off,1 - on"},
            {"attached_to", &(g_stRunOsdCfg.osdInfo[7][1].achAttachedTo),    CFG_DATA_TYPE_STRING,  "vpss",  "rw", 1, 16, "attached to which module"},
            {"alg_osd_enable", &(g_stRunOsdCfg.osdInfo[7][1].u32AlgOsdEnable),    CFG_DATA_TYPE_S32,  "0",  "rw", 0, 1, "Allow algrithom to draw on this channel"},
            {NULL,},
        },
        //stream 2 if it exists.
        {
            {"x",       &(g_stRunOsdCfg.osdInfo[7][2].x_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "x ratio: 0.0 - 1.0"},
            {"y",       &(g_stRunOsdCfg.osdInfo[7][2].y_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "y ratio: 0.0 - 1.0"},
            {"osdChannelName_enable",  &(g_stRunOsdCfg.osdInfo[7][2].osdChannelName.enable),  CFG_DATA_TYPE_S32,    "0",   "rw", 0, 1,               NULL},
            {"osdChannelName_text",    &(g_stRunOsdCfg.osdInfo[7][2].osdChannelName.text),    CFG_DATA_TYPE_STRING, "CH7-2", "rw", 1, MAX_STR_LEN_128, NULL},
            {"osdDatetime_enable",     &(g_stRunOsdCfg.osdInfo[7][2].osdDatetime.enable),     CFG_DATA_TYPE_S32, "0",   "rw", 0, 1,    NULL},
            {"osdDatetime_dateFormat", &(g_stRunOsdCfg.osdInfo[7][2].osdDatetime.dateFormat), CFG_DATA_TYPE_U8,  "0",   "rw", 1, 120,  "0:XXXX-XX-XX year mon date; 1:XX-XX-XXXX mon date year 2:XX-XX-XXXX date mon year"},
            {"osdDatetime_dateSprtr",  &(g_stRunOsdCfg.osdInfo[7][2].osdDatetime.dateSprtr),  CFG_DATA_TYPE_U8,  "0",   "rw", 0, 3,    "0 :, 1 -, 2 //, 3 ."},
            {"osdDatetime_timeFmt",    &(g_stRunOsdCfg.osdInfo[7][2].osdDatetime.timeFmt),    CFG_DATA_TYPE_U8,  "0",   "rw", 0, 1,    "0 - 24,1 - 12"},
            {"osdDatetime_displayWeek", &(g_stRunOsdCfg.osdInfo[7][2].osdDatetime.displayWeek),    CFG_DATA_TYPE_U8,  "0",	"rw", 0, 1, "0 - off, 1 - on"},
            {"attached_to", &(g_stRunOsdCfg.osdInfo[7][2].achAttachedTo),    CFG_DATA_TYPE_STRING,  "vpss",  "rw", 1, 16, "attached to which module"},
            {"alg_osd_enable", &(g_stRunOsdCfg.osdInfo[7][2].u32AlgOsdEnable),    CFG_DATA_TYPE_S32,  "0",  "rw", 0, 1, "Allow algrithom to draw on this channel"},
            {NULL,},
        },
        //stream 3 if it exists
        {
            {"x",       &(g_stRunOsdCfg.osdInfo[7][3].x_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "x ratio: 0.0 - 1.0"},
            {"y",       &(g_stRunOsdCfg.osdInfo[7][3].y_ratio),       CFG_DATA_TYPE_FLOAT,    "0",  "rw", 0, 1.0,  "y ratio: 0.0 - 1.0"},
            {"osdChannelName_enable",  &(g_stRunOsdCfg.osdInfo[7][3].osdChannelName.enable),  CFG_DATA_TYPE_S32,    "0",   "rw", 0, 1,               NULL},
            {"osdChannelName_text",    &(g_stRunOsdCfg.osdInfo[7][3].osdChannelName.text),    CFG_DATA_TYPE_STRING, "CH7-3", "rw", 1, MAX_STR_LEN_128, NULL},
            {"osdDatetime_enable",     &(g_stRunOsdCfg.osdInfo[7][3].osdDatetime.enable),     CFG_DATA_TYPE_S32, "0",   "rw", 0, 1,    NULL},
            {"osdDatetime_dateFormat", &(g_stRunOsdCfg.osdInfo[7][3].osdDatetime.dateFormat), CFG_DATA_TYPE_U8,  "0",   "rw", 1, 120,  "0:XXXX-XX-XX year mon date; 1:XX-XX-XXXX mon date year 2:XX-XX-XXXX date mon year"},
            {"osdDatetime_dateSprtr",  &(g_stRunOsdCfg.osdInfo[7][3].osdDatetime.dateSprtr),  CFG_DATA_TYPE_U8,  "0",   "rw", 0, 3,    "0 :, 1 -, 2 //, 3 ."},
            {"osdDatetime_timeFmt",    &(g_stRunOsdCfg.osdInfo[7][3].osdDatetime.timeFmt),    CFG_DATA_TYPE_U8,  "0",   "rw", 0, 1,    "0 - 24,1 - 12"},
            {"osdDatetime_displayWeek", &(g_stRunOsdCfg.osdInfo[7][3].osdDatetime.displayWeek),    CFG_DATA_TYPE_U8,  "1",	"rw", 0, 1, "0 - off,1 - on"},
            {"attached_to", &(g_stRunOsdCfg.osdInfo[7][3].achAttachedTo),    CFG_DATA_TYPE_STRING,  "vpss",  "rw", 1, 16, "attached to which module"},
            {"alg_osd_enable", &(g_stRunOsdCfg.osdInfo[7][3].u32AlgOsdEnable),    CFG_DATA_TYPE_S32,  "0",  "rw", 0, 1, "Allow algrithom to draw on this channel"},
            {NULL,},
        }
    },
};

void IPC_CFG_OSD_Print()
{
    int i,j;

    printf("********** Channel osd *********\n");
    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i ++)
    {
        for(j = 0; j < IPC_MAX_STREAM_PER_PIPE; j++)
        {
            printf("Channel_%d osd:\n", i);
            CfgPrintMap(osdMap[i][j]);
            printf("\n");
        }
    }
    printf("********** Channel osd *********\n\n");
}

int IPC_CFG_OSD_Save()
{
    cJSON *root, *node, *item;
    char *out;
    int i, j;
    char tmp[32] = {0};

    root = cJSON_CreateObject();

    printf("Save osd params...\n");

    for (i = 0; i < IPC_MAX_PIPELINE_NUM; i ++)
    {
        node = cJSON_CreateObject();
        for(j = 0; j < IPC_MAX_STREAM_PER_PIPE; j++)
        {
            item = CfgDataToCjsonByMap(osdMap[i][j]);
            sprintf(tmp, "stream_%d", j);
            cJSON_AddItemToObject(node, tmp, item);
        }

        sprintf(tmp, "channel_%d", i);
        cJSON_AddItemToObject(root, tmp, node);
    }

    out = cJSON_Print(root);

    int ret = CfgWriteToFile(OSD_CFG_FILE, out);
    if (ret != 0) {
        PRINT_ERR("CfgWriteToFile %s error.", OSD_CFG_FILE);
        return -1;
    }

    free(out);
    cJSON_Delete(root);

    return 0;
}

int IPC_CFG_OSD_LoadDefault()
{
    int i, j;

    printf("Enter %s \n", __FUNCTION__);
    for (i = 0; i < IPC_MAX_PIPELINE_NUM; i ++)
    {
        for(j = 0; j < IPC_MAX_STREAM_PER_PIPE; j++)
        {
            CfgLoadDefValue(osdMap[i][j]);
        }
    }

    return 0;
}


int IPC_CFG_OSD_Load(CFG_RUN_MODE_E enMode)
{
    char *data = NULL;
    char tmp[32] = {0};
    int i, j;

    data = CfgReadFromFile(OSD_CFG_FILE);
    if (data == NULL)
    {
        PRINT_INFO("load %s error, so to load default cfg param.\n", OSD_CFG_FILE);
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

    cJSON *chn = NULL;
    cJSON *stream = NULL;

    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        sprintf(tmp, "channel_%d", i);
        chn = cJSON_GetObjectItem(json, tmp);
        if(!chn)
        {
            for(j = 0; j < IPC_MAX_STREAM_PER_PIPE; j++)
            {
                CfgLoadDefValue(osdMap[i][j]);
            }
        }
        else
        {
            for(j = 0; j < IPC_MAX_STREAM_PER_PIPE; j++)
            {
                sprintf(tmp, "stream_%d", j);
                stream = cJSON_GetObjectItem(chn, tmp);
                if(!stream)
                {
                    CfgLoadDefValue(osdMap[i][j]);
                }
                else
                {
                    CfgCjsonToDataByMap(osdMap[i][j], stream);
                }
            }
        }
    }

    cJSON_Delete(json);
    free(data);
    return 0;

err:
    IPC_CFG_OSD_LoadDefault();
    IPC_CFG_OSD_Save();
    return 0;

}

IPC_CFG_OSD_S * IPC_CFG_OSD_GetParam()
{
    return &g_stRunOsdCfg;
}

