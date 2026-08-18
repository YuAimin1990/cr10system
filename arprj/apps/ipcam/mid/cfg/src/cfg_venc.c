#include "cfg_venc.h"

IPC_CFG_VENC_S g_stRunVencCfg;
IPC_VENC_MAX_RESOURCE_S g_stVencMaxResource = {0};

CFG_MAP vencMap[IPC_MAX_PIPELINE_NUM][IPC_MAX_VENC_STREAM_PER_CHN][MAX_VENC_ITEM_NUM] =
{
    //channel 0
    {
        //stream 0
        {
    		{"enable",       &(g_stRunVencCfg.vencStream[0][0].enable),           CFG_DATA_TYPE_S32, "1",    "rw", 0, 1, NULL},
            {"id",           &(g_stRunVencCfg.vencStream[0][0].id),               CFG_DATA_TYPE_S16, "0",    "rw", 0, 3, "0-3"},
            {"vi_band_id",	 &(g_stRunVencCfg.vencStream[0][0].vi_band_id),	   CFG_DATA_TYPE_S16, "0",    "rw", 0, 3, NULL},
            {"width",   &(g_stRunVencCfg.vencStream[0][0].cfg.width),    CFG_DATA_TYPE_U16, "1920", "rw", 0, 5000, NULL},
            {"height",  &(g_stRunVencCfg.vencStream[0][0].cfg.height),   CFG_DATA_TYPE_U16, "1080",  "rw", 0, 5000, NULL},
            {"roi_width",   &(g_stRunVencCfg.vencStream[0][0].cfg.roi_width),    CFG_DATA_TYPE_U16, "1920", "rw", 0, 5000, NULL},
            {"roi_height",  &(g_stRunVencCfg.vencStream[0][0].cfg.roi_height),   CFG_DATA_TYPE_U16, "1080",  "rw", 0, 5000, NULL},
            {"roi_x",       &(g_stRunVencCfg.vencStream[0][0].cfg.roi_x),        CFG_DATA_TYPE_U16, "0",    "rw", 0, 5000, NULL},
            {"roi_y",       &(g_stRunVencCfg.vencStream[0][0].cfg.roi_y),        CFG_DATA_TYPE_U16, "0",    "rw", 0,  5000, NULL},
    		{"encodeType",   &(g_stRunVencCfg.vencStream[0][0].cfg.encodeType),   CFG_DATA_TYPE_U8,  "2",    "rw", 0, 5, "0: none, 1: H.264, 2: H.265, 3: MJPEG, 4: jpeg"},
    		{"chromaFormat", &(g_stRunVencCfg.vencStream[0][0].cfg.chromaFormat), CFG_DATA_TYPE_U8,  "1",    "rw", 0, 1,  "0: YUV 422, 1: YUV 420."},
    		{"fps",          &(g_stRunVencCfg.vencStream[0][0].cfg.fps),          CFG_DATA_TYPE_U8,  "25",   "rw", 1, 60,   NULL},
    		{"gop",          &(g_stRunVencCfg.vencStream[0][0].cfg.gop),          CFG_DATA_TYPE_U8,  "30",   "rw", 1, 200,  NULL},
            {"quality",      &(g_stRunVencCfg.vencStream[0][0].cfg.quality),      CFG_DATA_TYPE_U8,  "3",    "rw", 0, 3,  "0: poor, 3: best"},
            {"brcMode",      &(g_stRunVencCfg.vencStream[0][0].cfg.brcMode),      CFG_DATA_TYPE_U8,  "0",    "rw", 0, 6,  "0: CBR; 1: VBR; 2: CVBR, 3: AVBR, 4: QPMAP, 5: FIXQP, 6 QVBR"},
            {"cbrAvgBps",    &(g_stRunVencCfg.vencStream[0][0].cfg.cbrAvgBps),    CFG_DATA_TYPE_U32, "2000", "rw", 128, 16000,   NULL},
            {"vbrMinBps",    &(g_stRunVencCfg.vencStream[0][0].cfg.vbrMinBps),    CFG_DATA_TYPE_U32, "750", "rw", 128, 16000,   NULL},
            {"vbrMaxBps",    &(g_stRunVencCfg.vencStream[0][0].cfg.vbrMaxBps),    CFG_DATA_TYPE_U32, "3000", "rw", 128, 16000,   NULL},
            {"qpMinI",       &(g_stRunVencCfg.vencStream[0][0].cfg.qpMinI),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMinP",       &(g_stRunVencCfg.vencStream[0][0].cfg.qpMinP),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
    		{"qpMaxI",       &(g_stRunVencCfg.vencStream[0][0].cfg.qpMaxI),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
    		{"qpMaxP",       &(g_stRunVencCfg.vencStream[0][0].cfg.qpMaxP),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
    		{"qpIWeight",    &(g_stRunVencCfg.vencStream[0][0].cfg.qpIWeight),    CFG_DATA_TYPE_U8,  "3",    "rw", 0, 100,  NULL},
            {"qpPWeight",    &(g_stRunVencCfg.vencStream[0][0].cfg.qpPWeight),    CFG_DATA_TYPE_U8,  "5",    "rw", 0, 100,  NULL},
    		{"adaptQp",      &(g_stRunVencCfg.vencStream[0][0].cfg.adaptQp),      CFG_DATA_TYPE_U8,  "2",    "rw", 0, 100,  NULL},
    		{"statTime",     &(g_stRunVencCfg.vencStream[0][0].cfg.statTime),      CFG_DATA_TYPE_U8,  "30",  "rw", 1, 60,  NULL},
        },
        //stream 1
        {
    		{"enable",       &(g_stRunVencCfg.vencStream[0][1].enable),           CFG_DATA_TYPE_S32, "1",    "rw", 0, 1, NULL},
            {"id",           &(g_stRunVencCfg.vencStream[0][1].id),               CFG_DATA_TYPE_S16, "1",    "rw", 0, 3, "0-3"},
            {"vi_band_id",	 &(g_stRunVencCfg.vencStream[0][1].vi_band_id),	   CFG_DATA_TYPE_S16, "1",    "rw", 0, 3, NULL},
            {"width",   &(g_stRunVencCfg.vencStream[0][1].cfg.width),    CFG_DATA_TYPE_U16, "640", "rw", 0, 5000, NULL},
            {"height",  &(g_stRunVencCfg.vencStream[0][1].cfg.height),   CFG_DATA_TYPE_U16, "480",  "rw", 0, 5000, NULL},
            {"roi_width",   &(g_stRunVencCfg.vencStream[0][1].cfg.roi_width),    CFG_DATA_TYPE_U16, "640", "rw", 0, 5000, NULL},
            {"roi_height",  &(g_stRunVencCfg.vencStream[0][1].cfg.roi_height),   CFG_DATA_TYPE_U16, "480",  "rw", 0, 5000, NULL},
            {"roi_x",       &(g_stRunVencCfg.vencStream[0][1].cfg.roi_x),        CFG_DATA_TYPE_U16, "0",    "rw", 0, 5000, NULL},
            {"roi_y",       &(g_stRunVencCfg.vencStream[0][1].cfg.roi_y),        CFG_DATA_TYPE_U16, "0",    "rw", 0,  5000, NULL},
    		{"encodeType",   &(g_stRunVencCfg.vencStream[0][1].cfg.encodeType),   CFG_DATA_TYPE_U8,  "2",    "rw", 0, 5, "0: none, 1: H.264, 2: H.265, 3: MJPEG, 4: jpeg"},
    		{"chromaFormat", &(g_stRunVencCfg.vencStream[0][1].cfg.chromaFormat), CFG_DATA_TYPE_U8,  "1",    "rw", 0, 1,  "0: YUV 422, 1: YUV 420."},
    		{"fps",          &(g_stRunVencCfg.vencStream[0][1].cfg.fps),          CFG_DATA_TYPE_U8,  "25",   "rw", 1, 60,   NULL},
    		{"gop",          &(g_stRunVencCfg.vencStream[0][1].cfg.gop),          CFG_DATA_TYPE_U8,  "30",   "rw", 1, 200,  NULL},
            {"quality",      &(g_stRunVencCfg.vencStream[0][1].cfg.quality),      CFG_DATA_TYPE_U8,  "3",    "rw", 0, 3,  "0: poor, 3: best"},
            {"brcMode",      &(g_stRunVencCfg.vencStream[0][1].cfg.brcMode),      CFG_DATA_TYPE_U8,  "0",    "rw", 0, 6,  "0: CBR; 1: VBR; 2: CVBR, 3: AVBR, 4: QPMAP, 5: FIXQP, 6 QVBR"},
            {"cbrAvgBps",    &(g_stRunVencCfg.vencStream[0][1].cfg.cbrAvgBps),    CFG_DATA_TYPE_U32, "1000", "rw", 128, 16000,   NULL},
            {"vbrMinBps",    &(g_stRunVencCfg.vencStream[0][1].cfg.vbrMinBps),    CFG_DATA_TYPE_U32, "400", "rw", 128, 16000,   NULL},
            {"vbrMaxBps",    &(g_stRunVencCfg.vencStream[0][1].cfg.vbrMaxBps),    CFG_DATA_TYPE_U32, "1200", "rw", 128, 16000,   NULL},
            {"qpMinI",       &(g_stRunVencCfg.vencStream[0][1].cfg.qpMinI),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMinP",       &(g_stRunVencCfg.vencStream[0][1].cfg.qpMinP),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMaxI",       &(g_stRunVencCfg.vencStream[0][1].cfg.qpMaxI),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
            {"qpMaxP",       &(g_stRunVencCfg.vencStream[0][1].cfg.qpMaxP),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
    		{"qpIWeight",    &(g_stRunVencCfg.vencStream[0][1].cfg.qpIWeight),    CFG_DATA_TYPE_U8,  "3",    "rw", 0, 100,  NULL},
            {"qpPWeight",    &(g_stRunVencCfg.vencStream[0][1].cfg.qpPWeight),    CFG_DATA_TYPE_U8,  "5",    "rw", 0, 100,  NULL},
    		{"adaptQp",      &(g_stRunVencCfg.vencStream[0][1].cfg.adaptQp),      CFG_DATA_TYPE_U8,  "2",    "rw", 0, 100,  NULL},
            {"statTime",	 &(g_stRunVencCfg.vencStream[0][1].cfg.statTime),	   CFG_DATA_TYPE_U8,  "30",  "rw", 1, 60,  NULL},
        },
        //stream 2
        {
    		{"enable",       &(g_stRunVencCfg.vencStream[0][2].enable),           CFG_DATA_TYPE_S32, "0",    "rw", 0, 1, NULL},
            {"id",           &(g_stRunVencCfg.vencStream[0][2].id),               CFG_DATA_TYPE_S16, "2",    "rw", 0, 3, "0-3"},
            {"vi_band_id",	 &(g_stRunVencCfg.vencStream[0][2].vi_band_id),	   CFG_DATA_TYPE_S16, "2",    "rw", 0, 3, NULL},
            {"width",   &(g_stRunVencCfg.vencStream[0][2].cfg.width),    CFG_DATA_TYPE_U16, "320", "rw", 0, 5000, NULL},
            {"height",  &(g_stRunVencCfg.vencStream[0][2].cfg.height),   CFG_DATA_TYPE_U16, "240",  "rw", 0, 5000, NULL},
            {"roi_width",   &(g_stRunVencCfg.vencStream[0][2].cfg.roi_width),    CFG_DATA_TYPE_U16, "320", "rw", 0, 5000, NULL},
            {"roi_height",  &(g_stRunVencCfg.vencStream[0][2].cfg.roi_height),   CFG_DATA_TYPE_U16, "240",  "rw", 0, 5000, NULL},
            {"roi_x",       &(g_stRunVencCfg.vencStream[0][2].cfg.roi_x),        CFG_DATA_TYPE_U16, "0",    "rw", 0, 5000, NULL},
            {"roi_y",       &(g_stRunVencCfg.vencStream[0][2].cfg.roi_y),        CFG_DATA_TYPE_U16, "0",    "rw", 0,  5000, NULL},
    		{"encodeType",   &(g_stRunVencCfg.vencStream[0][2].cfg.encodeType),   CFG_DATA_TYPE_U8,  "2",    "rw", 0, 5, "0: none, 1: H.264, 2: H.265, 3: MJPEG, 4: jpeg"},
    		{"chromaFormat", &(g_stRunVencCfg.vencStream[0][2].cfg.chromaFormat), CFG_DATA_TYPE_U8,  "1",    "rw", 0, 1,  "0: YUV 422, 1: YUV 420."},
    		{"fps",          &(g_stRunVencCfg.vencStream[0][2].cfg.fps),          CFG_DATA_TYPE_U8,  "25",   "rw", 1, 60,   NULL},
    		{"gop",          &(g_stRunVencCfg.vencStream[0][2].cfg.gop),          CFG_DATA_TYPE_U8,  "30",   "rw", 1, 200,  NULL},
            {"quality",      &(g_stRunVencCfg.vencStream[0][2].cfg.quality),      CFG_DATA_TYPE_U8,  "3",    "rw", 0, 3,  "0: poor, 3: best"},
            {"brcMode",      &(g_stRunVencCfg.vencStream[0][2].cfg.brcMode),      CFG_DATA_TYPE_U8,  "0",    "rw", 0, 3,  "0: CBR; 1: VBR; 2: FIXQP; 3: AVBR"},
            {"cbrAvgBps",    &(g_stRunVencCfg.vencStream[0][2].cfg.cbrAvgBps),    CFG_DATA_TYPE_U32, "500", "rw", 128, 16000,   NULL},
            {"vbrMinBps",    &(g_stRunVencCfg.vencStream[0][2].cfg.vbrMinBps),    CFG_DATA_TYPE_U32, "300", "rw", 128, 16000,   NULL},
            {"vbrMaxBps",    &(g_stRunVencCfg.vencStream[0][2].cfg.vbrMaxBps),    CFG_DATA_TYPE_U32, "500", "rw", 128, 16000,   NULL},
            {"qpMinI",       &(g_stRunVencCfg.vencStream[0][2].cfg.qpMinI),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMinP",       &(g_stRunVencCfg.vencStream[0][2].cfg.qpMinP),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMaxI",       &(g_stRunVencCfg.vencStream[0][2].cfg.qpMaxI),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
            {"qpMaxP",       &(g_stRunVencCfg.vencStream[0][2].cfg.qpMaxP),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
    		{"qpIWeight",    &(g_stRunVencCfg.vencStream[0][2].cfg.qpIWeight),    CFG_DATA_TYPE_U8,  "3",    "rw", 0, 100,  NULL},
            {"qpPWeight",    &(g_stRunVencCfg.vencStream[0][2].cfg.qpPWeight),    CFG_DATA_TYPE_U8,  "5",    "rw", 0, 100,  NULL},
    		{"adaptQp",      &(g_stRunVencCfg.vencStream[0][2].cfg.adaptQp),      CFG_DATA_TYPE_U8,  "2",    "rw", 0, 100,  NULL},
    		{"statTime",     &(g_stRunVencCfg.vencStream[0][2].cfg.statTime),      CFG_DATA_TYPE_U8,  "30",  "rw", 1, 60,  NULL},
        },
#ifdef THERMAL_ENABLE
        //stream 3
        {
            {"enable",       &(g_stRunVencCfg.vencStream[0][3].enable),           CFG_DATA_TYPE_S32, "0",    "rw", 0, 1, NULL},
            {"id",           &(g_stRunVencCfg.vencStream[0][3].id),               CFG_DATA_TYPE_S16, "3",    "rw", 0, 3, "0-3"},
            {"vi_band_id",   &(g_stRunVencCfg.vencStream[0][3].vi_band_id),    CFG_DATA_TYPE_S16, "3",    "rw", 0, 3, NULL},
            {"width",   &(g_stRunVencCfg.vencStream[0][3].cfg.width),    CFG_DATA_TYPE_U16, "320", "rw", 0, 5000, NULL},
            {"height",  &(g_stRunVencCfg.vencStream[0][3].cfg.height),   CFG_DATA_TYPE_U16, "240",  "rw", 0, 5000, NULL},
            {"roi_width",   &(g_stRunVencCfg.vencStream[0][3].cfg.roi_width),    CFG_DATA_TYPE_U16, "320", "rw", 0, 5000, NULL},
            {"roi_height",  &(g_stRunVencCfg.vencStream[0][3].cfg.roi_height),   CFG_DATA_TYPE_U16, "240",  "rw", 0, 5000, NULL},
            {"roi_x",       &(g_stRunVencCfg.vencStream[0][3].cfg.roi_x),        CFG_DATA_TYPE_U16, "0",    "rw", 0, 5000, NULL},
            {"roi_y",       &(g_stRunVencCfg.vencStream[0][3].cfg.roi_y),        CFG_DATA_TYPE_U16, "0",    "rw", 0,  5000, NULL},
            {"encodeType",   &(g_stRunVencCfg.vencStream[0][3].cfg.encodeType),   CFG_DATA_TYPE_U8,  "2",    "rw", 0, 5, "0: none, 1: H.264, 2: H.265, 3: MJPEG, 4: jpeg"},
            {"chromaFormat", &(g_stRunVencCfg.vencStream[0][3].cfg.chromaFormat), CFG_DATA_TYPE_U8,  "1",    "rw", 0, 1,  "0: YUV 422, 1: YUV 420."},
            {"fps",          &(g_stRunVencCfg.vencStream[0][3].cfg.fps),          CFG_DATA_TYPE_U8,  "25",   "rw", 1, 60,   NULL},
            {"gop",          &(g_stRunVencCfg.vencStream[0][3].cfg.gop),          CFG_DATA_TYPE_U8,  "25",   "rw", 1, 200,  NULL},
            {"quality",      &(g_stRunVencCfg.vencStream[0][3].cfg.quality),      CFG_DATA_TYPE_U8,  "3",    "rw", 0, 3,  "0: poor, 3: best"},
            {"brcMode",      &(g_stRunVencCfg.vencStream[0][3].cfg.brcMode),      CFG_DATA_TYPE_U8,  "0",    "rw", 0, 3,  "0: CBR; 1: VBR; 2: FIXQP; 3: AVBR"},
            {"cbrAvgBps",    &(g_stRunVencCfg.vencStream[0][3].cfg.cbrAvgBps),    CFG_DATA_TYPE_U32, "500", "rw", 128, 16000,   NULL},
            {"vbrMinBps",    &(g_stRunVencCfg.vencStream[0][3].cfg.vbrMinBps),    CFG_DATA_TYPE_U32, "300", "rw", 128, 16000,   NULL},
            {"vbrMaxBps",    &(g_stRunVencCfg.vencStream[0][3].cfg.vbrMaxBps),    CFG_DATA_TYPE_U32, "500", "rw", 128, 16000,   NULL},
            {"qpMinI",       &(g_stRunVencCfg.vencStream[0][3].cfg.qpMinI),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMinP",       &(g_stRunVencCfg.vencStream[0][3].cfg.qpMinP),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMaxI",       &(g_stRunVencCfg.vencStream[0][3].cfg.qpMaxI),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
            {"qpMaxP",       &(g_stRunVencCfg.vencStream[0][3].cfg.qpMaxP),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
            {"qpIWeight",    &(g_stRunVencCfg.vencStream[0][3].cfg.qpIWeight),    CFG_DATA_TYPE_U8,  "3",    "rw", 0, 100,  NULL},
            {"qpPWeight",    &(g_stRunVencCfg.vencStream[0][3].cfg.qpPWeight),    CFG_DATA_TYPE_U8,  "5",    "rw", 0, 100,  NULL},
            {"adaptQp",      &(g_stRunVencCfg.vencStream[0][3].cfg.adaptQp),      CFG_DATA_TYPE_U8,  "2",    "rw", 0, 100,  NULL},
            {"statTime",     &(g_stRunVencCfg.vencStream[0][3].cfg.statTime),      CFG_DATA_TYPE_U8,  "30",  "rw", 1, 60,  NULL},
        },
#endif

    },
    //channel 1
    {
        //stream 0
        {
    		{"enable",       &(g_stRunVencCfg.vencStream[1][0].enable),           CFG_DATA_TYPE_S32, "0",    "rw", 0, 1, NULL},
            {"id",           &(g_stRunVencCfg.vencStream[1][0].id),               CFG_DATA_TYPE_S16, "0",    "rw", 0, 3, "0-3"},
            {"vi_band_id",	 &(g_stRunVencCfg.vencStream[1][0].vi_band_id),	   CFG_DATA_TYPE_S16, "0",    "rw", 0, 3, NULL},
            {"width",   &(g_stRunVencCfg.vencStream[1][0].cfg.width),    CFG_DATA_TYPE_U16, "1920", "rw", 0, 5000, NULL},
            {"height",  &(g_stRunVencCfg.vencStream[1][0].cfg.height),   CFG_DATA_TYPE_U16, "1080",  "rw", 0, 5000, NULL},
            {"roi_width",   &(g_stRunVencCfg.vencStream[1][0].cfg.roi_width),    CFG_DATA_TYPE_U16, "1920", "rw", 0, 5000, NULL},
            {"roi_height",  &(g_stRunVencCfg.vencStream[1][0].cfg.roi_height),   CFG_DATA_TYPE_U16, "1080",  "rw", 0, 5000, NULL},
            {"roi_x",       &(g_stRunVencCfg.vencStream[1][0].cfg.roi_x),        CFG_DATA_TYPE_U16, "0",    "rw", 0, 5000, NULL},
            {"roi_y",       &(g_stRunVencCfg.vencStream[1][0].cfg.roi_y),        CFG_DATA_TYPE_U16, "0",    "rw", 0,  5000, NULL},
    		{"encodeType",   &(g_stRunVencCfg.vencStream[1][0].cfg.encodeType),   CFG_DATA_TYPE_U8,  "2",    "rw", 0, 5, "0: none, 1: H.264, 2: H.265, 3: MJPEG, 4: jpeg"},
    		{"chromaFormat", &(g_stRunVencCfg.vencStream[1][0].cfg.chromaFormat), CFG_DATA_TYPE_U8,  "1",    "rw", 0, 1,  "0: YUV 422, 1: YUV 420."},
    		{"fps",          &(g_stRunVencCfg.vencStream[1][0].cfg.fps),          CFG_DATA_TYPE_U8,  "25",   "rw", 1, 60,   NULL},
    		{"gop",          &(g_stRunVencCfg.vencStream[1][0].cfg.gop),          CFG_DATA_TYPE_U8,  "30",   "rw", 1, 200,  NULL},
            {"quality",      &(g_stRunVencCfg.vencStream[1][0].cfg.quality),      CFG_DATA_TYPE_U8,  "3",    "rw", 0, 3,  "0: poor, 3: best"},
            {"brcMode",      &(g_stRunVencCfg.vencStream[1][0].cfg.brcMode),      CFG_DATA_TYPE_U8,  "0",    "rw", 0, 6,  "0: CBR; 1: VBR; 2: CVBR, 3: AVBR, 4: QPMAP, 5: FIXQP, 6 QVBR"},
            {"cbrAvgBps",    &(g_stRunVencCfg.vencStream[1][0].cfg.cbrAvgBps),    CFG_DATA_TYPE_U32, "2000", "rw", 128, 16000,   NULL},
            {"vbrMinBps",    &(g_stRunVencCfg.vencStream[1][0].cfg.vbrMinBps),    CFG_DATA_TYPE_U32, "750", "rw", 128, 16000,   NULL},
            {"vbrMaxBps",    &(g_stRunVencCfg.vencStream[1][0].cfg.vbrMaxBps),    CFG_DATA_TYPE_U32, "3000", "rw", 128, 16000,   NULL},
            {"qpMinI",       &(g_stRunVencCfg.vencStream[1][0].cfg.qpMinI),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMinP",       &(g_stRunVencCfg.vencStream[1][0].cfg.qpMinP),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
    		{"qpMaxI",       &(g_stRunVencCfg.vencStream[1][0].cfg.qpMaxI),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
    		{"qpMaxP",       &(g_stRunVencCfg.vencStream[1][0].cfg.qpMaxP),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
    		{"qpIWeight",    &(g_stRunVencCfg.vencStream[1][0].cfg.qpIWeight),    CFG_DATA_TYPE_U8,  "3",    "rw", 0, 100,  NULL},
            {"qpPWeight",    &(g_stRunVencCfg.vencStream[1][0].cfg.qpPWeight),    CFG_DATA_TYPE_U8,  "5",    "rw", 0, 100,  NULL},
    		{"adaptQp",      &(g_stRunVencCfg.vencStream[1][0].cfg.adaptQp),      CFG_DATA_TYPE_U8,  "2",    "rw", 0, 100,  NULL},
    		{"statTime",     &(g_stRunVencCfg.vencStream[1][0].cfg.statTime),      CFG_DATA_TYPE_U8,  "30",  "rw", 1, 60,  NULL},
        },
        //stream 1
        {
    		{"enable",       &(g_stRunVencCfg.vencStream[1][1].enable),           CFG_DATA_TYPE_S32, "0",    "rw", 0, 1, NULL},
            {"id",           &(g_stRunVencCfg.vencStream[1][1].id),               CFG_DATA_TYPE_S16, "1",    "rw", 0, 3, "0-3"},
            {"vi_band_id",	 &(g_stRunVencCfg.vencStream[1][1].vi_band_id),	   CFG_DATA_TYPE_S16, "1",    "rw", 0, 3, NULL},
            {"width",   &(g_stRunVencCfg.vencStream[1][1].cfg.width),    CFG_DATA_TYPE_U16, "640", "rw", 0, 5000, NULL},
            {"height",  &(g_stRunVencCfg.vencStream[1][1].cfg.height),   CFG_DATA_TYPE_U16, "480",  "rw", 0, 5000, NULL},
            {"roi_width",   &(g_stRunVencCfg.vencStream[1][1].cfg.roi_width),    CFG_DATA_TYPE_U16, "640", "rw", 0, 5000, NULL},
            {"roi_height",  &(g_stRunVencCfg.vencStream[1][1].cfg.roi_height),   CFG_DATA_TYPE_U16, "480",  "rw", 0, 5000, NULL},
            {"roi_x",       &(g_stRunVencCfg.vencStream[1][1].cfg.roi_x),        CFG_DATA_TYPE_U16, "0",    "rw", 0, 5000, NULL},
            {"roi_y",       &(g_stRunVencCfg.vencStream[1][1].cfg.roi_y),        CFG_DATA_TYPE_U16, "0",    "rw", 0,  5000, NULL},
    		{"encodeType",   &(g_stRunVencCfg.vencStream[1][1].cfg.encodeType),   CFG_DATA_TYPE_U8,  "2",    "rw", 0, 5, "0: none, 1: H.264, 2: H.265, 3: MJPEG, 4: jpeg"},
    		{"chromaFormat", &(g_stRunVencCfg.vencStream[1][1].cfg.chromaFormat), CFG_DATA_TYPE_U8,  "1",    "rw", 0, 1,  "0: YUV 422, 1: YUV 420."},
    		{"fps",          &(g_stRunVencCfg.vencStream[1][1].cfg.fps),          CFG_DATA_TYPE_U8,  "25",   "rw", 1, 60,   NULL},
    		{"gop",          &(g_stRunVencCfg.vencStream[1][1].cfg.gop),          CFG_DATA_TYPE_U8,  "30",   "rw", 1, 200,  NULL},
            {"quality",      &(g_stRunVencCfg.vencStream[1][1].cfg.quality),      CFG_DATA_TYPE_U8,  "3",    "rw", 0, 3,  "0: poor, 3: best"},
            {"brcMode",      &(g_stRunVencCfg.vencStream[1][1].cfg.brcMode),      CFG_DATA_TYPE_U8,  "0",    "rw", 0, 6,  "0: CBR; 1: VBR; 2: CVBR, 3: AVBR, 4: QPMAP, 5: FIXQP, 6 QVBR"},
            {"cbrAvgBps",    &(g_stRunVencCfg.vencStream[1][1].cfg.cbrAvgBps),    CFG_DATA_TYPE_U32, "1000", "rw", 128, 16000,   NULL},
            {"vbrMinBps",    &(g_stRunVencCfg.vencStream[1][1].cfg.vbrMinBps),    CFG_DATA_TYPE_U32, "400", "rw", 128, 16000,   NULL},
            {"vbrMaxBps",    &(g_stRunVencCfg.vencStream[1][1].cfg.vbrMaxBps),    CFG_DATA_TYPE_U32, "1200", "rw", 128, 16000,   NULL},
            {"qpMinI",       &(g_stRunVencCfg.vencStream[1][1].cfg.qpMinI),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMinP",       &(g_stRunVencCfg.vencStream[1][1].cfg.qpMinP),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
    		{"qpMaxI",       &(g_stRunVencCfg.vencStream[1][1].cfg.qpMaxI),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
    		{"qpMaxP",       &(g_stRunVencCfg.vencStream[1][1].cfg.qpMaxP),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
    		{"qpIWeight",    &(g_stRunVencCfg.vencStream[1][1].cfg.qpIWeight),    CFG_DATA_TYPE_U8,  "3",    "rw", 0, 100,  NULL},
            {"qpPWeight",    &(g_stRunVencCfg.vencStream[1][1].cfg.qpPWeight),    CFG_DATA_TYPE_U8,  "5",    "rw", 0, 100,  NULL},
    		{"adaptQp",      &(g_stRunVencCfg.vencStream[1][1].cfg.adaptQp),      CFG_DATA_TYPE_U8,  "2",    "rw", 0, 100,  NULL},
    		{"statTime",     &(g_stRunVencCfg.vencStream[1][1].cfg.statTime),      CFG_DATA_TYPE_U8,  "30",  "rw", 1, 60,  NULL},
        },
        //stream 2
        {
    		{"enable",       &(g_stRunVencCfg.vencStream[1][2].enable),           CFG_DATA_TYPE_S32, "0",    "rw", 0, 1, NULL},
            {"id",           &(g_stRunVencCfg.vencStream[1][2].id),               CFG_DATA_TYPE_S16, "2",    "rw", 0, 3, "0-3"},
            {"vi_band_id",	 &(g_stRunVencCfg.vencStream[1][2].vi_band_id),	   CFG_DATA_TYPE_S16, "2",    "rw", 0, 3, NULL},
            {"width",   &(g_stRunVencCfg.vencStream[1][2].cfg.width),    CFG_DATA_TYPE_U16, "320", "rw", 0, 5000, NULL},
            {"height",  &(g_stRunVencCfg.vencStream[1][2].cfg.height),   CFG_DATA_TYPE_U16, "240",  "rw", 0, 5000, NULL},
            {"roi_width",   &(g_stRunVencCfg.vencStream[1][2].cfg.roi_width),    CFG_DATA_TYPE_U16, "320", "rw", 0, 5000, NULL},
            {"roi_height",  &(g_stRunVencCfg.vencStream[1][2].cfg.roi_height),   CFG_DATA_TYPE_U16, "240",  "rw", 0, 5000, NULL},
            {"roi_x",       &(g_stRunVencCfg.vencStream[1][2].cfg.roi_x),        CFG_DATA_TYPE_U16, "0",    "rw", 0, 5000, NULL},
            {"roi_y",       &(g_stRunVencCfg.vencStream[1][2].cfg.roi_y),        CFG_DATA_TYPE_U16, "0",    "rw", 0,  5000, NULL},
    		{"encodeType",   &(g_stRunVencCfg.vencStream[1][2].cfg.encodeType),   CFG_DATA_TYPE_U8,  "2",    "rw", 0, 5, "0: none, 1: H.264, 2: H.265, 3: MJPEG, 4: jpeg"},
    		{"chromaFormat", &(g_stRunVencCfg.vencStream[1][2].cfg.chromaFormat), CFG_DATA_TYPE_U8,  "1",    "rw", 0, 1,  "0: YUV 422, 1: YUV 420."},
    		{"fps",          &(g_stRunVencCfg.vencStream[1][2].cfg.fps),          CFG_DATA_TYPE_U8,  "25",   "rw", 1, 60,   NULL},
    		{"gop",          &(g_stRunVencCfg.vencStream[1][2].cfg.gop),          CFG_DATA_TYPE_U8,  "30",   "rw", 1, 200,  NULL},
            {"quality",      &(g_stRunVencCfg.vencStream[1][2].cfg.quality),      CFG_DATA_TYPE_U8,  "3",    "rw", 0, 3,  "0: poor, 3: best"},
            {"brcMode",      &(g_stRunVencCfg.vencStream[1][2].cfg.brcMode),      CFG_DATA_TYPE_U8,  "0",    "rw", 0, 3,  "0: CBR; 1: VBR; 2: FIXQP; 3: AVBR"},
            {"cbrAvgBps",    &(g_stRunVencCfg.vencStream[1][2].cfg.cbrAvgBps),    CFG_DATA_TYPE_U32, "500", "rw", 128, 16000,   NULL},
            {"vbrMinBps",    &(g_stRunVencCfg.vencStream[1][2].cfg.vbrMinBps),    CFG_DATA_TYPE_U32, "300", "rw", 128, 16000,   NULL},
            {"vbrMaxBps",    &(g_stRunVencCfg.vencStream[1][2].cfg.vbrMaxBps),    CFG_DATA_TYPE_U32, "500", "rw", 128, 16000,   NULL},
            {"qpMinI",       &(g_stRunVencCfg.vencStream[1][2].cfg.qpMinI),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMinP",       &(g_stRunVencCfg.vencStream[1][2].cfg.qpMinP),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMaxI",       &(g_stRunVencCfg.vencStream[1][2].cfg.qpMaxI),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
            {"qpMaxP",       &(g_stRunVencCfg.vencStream[1][2].cfg.qpMaxP),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
    		{"qpIWeight",    &(g_stRunVencCfg.vencStream[1][2].cfg.qpIWeight),    CFG_DATA_TYPE_U8,  "3",    "rw", 0, 100,  NULL},
            {"qpPWeight",    &(g_stRunVencCfg.vencStream[1][2].cfg.qpPWeight),    CFG_DATA_TYPE_U8,  "5",    "rw", 0, 100,  NULL},
    		{"adaptQp",      &(g_stRunVencCfg.vencStream[1][2].cfg.adaptQp),      CFG_DATA_TYPE_U8,  "2",    "rw", 0, 100,  NULL},
    		{"statTime",     &(g_stRunVencCfg.vencStream[1][2].cfg.statTime),      CFG_DATA_TYPE_U8,  "30",  "rw", 1, 60,  NULL},
        },
#ifdef THERMAL_ENABLE
        //stream 3
        {
            {"enable",       &(g_stRunVencCfg.vencStream[1][3].enable),           CFG_DATA_TYPE_S32, "0",    "rw", 0, 1, NULL},
            {"id",           &(g_stRunVencCfg.vencStream[1][3].id),               CFG_DATA_TYPE_S16, "3",    "rw", 0, 3, "0-3"},
            {"vi_band_id",   &(g_stRunVencCfg.vencStream[1][3].vi_band_id),    CFG_DATA_TYPE_S16, "3",    "rw", 0, 3, NULL},
            {"width",   &(g_stRunVencCfg.vencStream[1][3].cfg.width),    CFG_DATA_TYPE_U16, "320", "rw", 0, 5000, NULL},
            {"height",  &(g_stRunVencCfg.vencStream[1][3].cfg.height),   CFG_DATA_TYPE_U16, "240",  "rw", 0, 5000, NULL},
            {"roi_width",   &(g_stRunVencCfg.vencStream[1][3].cfg.roi_width),    CFG_DATA_TYPE_U16, "320", "rw", 0, 5000, NULL},
            {"roi_height",  &(g_stRunVencCfg.vencStream[1][3].cfg.roi_height),   CFG_DATA_TYPE_U16, "240",  "rw", 0, 5000, NULL},
            {"roi_x",       &(g_stRunVencCfg.vencStream[1][3].cfg.roi_x),        CFG_DATA_TYPE_U16, "0",    "rw", 0, 5000, NULL},
            {"roi_y",       &(g_stRunVencCfg.vencStream[1][3].cfg.roi_y),        CFG_DATA_TYPE_U16, "0",    "rw", 0,  5000, NULL},
            {"encodeType",   &(g_stRunVencCfg.vencStream[1][3].cfg.encodeType),   CFG_DATA_TYPE_U8,  "2",    "rw", 0, 5, "0: none, 1: H.264, 2: H.265, 3: MJPEG, 4: jpeg"},
            {"chromaFormat", &(g_stRunVencCfg.vencStream[1][3].cfg.chromaFormat), CFG_DATA_TYPE_U8,  "1",    "rw", 0, 1,  "0: YUV 422, 1: YUV 420."},
            {"fps",          &(g_stRunVencCfg.vencStream[1][3].cfg.fps),          CFG_DATA_TYPE_U8,  "25",   "rw", 1, 60,   NULL},
            {"gop",          &(g_stRunVencCfg.vencStream[1][3].cfg.gop),          CFG_DATA_TYPE_U8,  "25",   "rw", 1, 200,  NULL},
            {"quality",      &(g_stRunVencCfg.vencStream[1][3].cfg.quality),      CFG_DATA_TYPE_U8,  "3",    "rw", 0, 3,  "0: poor, 3: best"},
            {"brcMode",      &(g_stRunVencCfg.vencStream[1][3].cfg.brcMode),      CFG_DATA_TYPE_U8,  "0",    "rw", 0, 3,  "0: CBR; 1: VBR; 2: FIXQP; 3: AVBR"},
            {"cbrAvgBps",    &(g_stRunVencCfg.vencStream[1][3].cfg.cbrAvgBps),    CFG_DATA_TYPE_U32, "500", "rw", 0, 32000,   NULL},
            {"vbrMinBps",    &(g_stRunVencCfg.vencStream[1][3].cfg.vbrMinBps),    CFG_DATA_TYPE_U32, "300", "rw", 0, 32000,   NULL},
            {"vbrMaxBps",    &(g_stRunVencCfg.vencStream[1][3].cfg.vbrMaxBps),    CFG_DATA_TYPE_U32, "500", "rw", 0, 32000,   NULL},
            {"qpMinI",       &(g_stRunVencCfg.vencStream[1][3].cfg.qpMinI),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMinP",       &(g_stRunVencCfg.vencStream[1][3].cfg.qpMinP),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMaxI",       &(g_stRunVencCfg.vencStream[1][3].cfg.qpMaxI),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
            {"qpMaxP",       &(g_stRunVencCfg.vencStream[1][3].cfg.qpMaxP),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
            {"qpIWeight",    &(g_stRunVencCfg.vencStream[1][3].cfg.qpIWeight),    CFG_DATA_TYPE_U8,  "3",    "rw", 0, 100,  NULL},
            {"qpPWeight",    &(g_stRunVencCfg.vencStream[1][3].cfg.qpPWeight),    CFG_DATA_TYPE_U8,  "5",    "rw", 0, 100,  NULL},
            {"adaptQp",      &(g_stRunVencCfg.vencStream[1][3].cfg.adaptQp),      CFG_DATA_TYPE_U8,  "2",    "rw", 0, 100,  NULL},
            {"statTime",     &(g_stRunVencCfg.vencStream[1][3].cfg.statTime),      CFG_DATA_TYPE_U8,  "30",  "rw", 1, 60,  NULL},
        }
#endif
    },
    //channel 2
    {
        //stream 0
        {
    		{"enable",       &(g_stRunVencCfg.vencStream[2][0].enable),           CFG_DATA_TYPE_S32, "0",    "rw", 0, 1, NULL},
            {"id",           &(g_stRunVencCfg.vencStream[2][0].id),               CFG_DATA_TYPE_S16, "0",    "rw", 0, 3, "0-3"},
            {"vi_band_id",	 &(g_stRunVencCfg.vencStream[2][0].vi_band_id),	   CFG_DATA_TYPE_S16, "0",    "rw", 0, 3, NULL},
            {"width",   &(g_stRunVencCfg.vencStream[2][0].cfg.width),    CFG_DATA_TYPE_U16, "1920", "rw", 0, 5000, NULL},
            {"height",  &(g_stRunVencCfg.vencStream[2][0].cfg.height),   CFG_DATA_TYPE_U16, "1080",  "rw", 0, 5000, NULL},
            {"roi_width",   &(g_stRunVencCfg.vencStream[2][0].cfg.roi_width),    CFG_DATA_TYPE_U16, "1920", "rw", 0, 5000, NULL},
            {"roi_height",  &(g_stRunVencCfg.vencStream[2][0].cfg.roi_height),   CFG_DATA_TYPE_U16, "1080",  "rw", 0, 5000, NULL},
            {"roi_x",       &(g_stRunVencCfg.vencStream[2][0].cfg.roi_x),        CFG_DATA_TYPE_U16, "0",    "rw", 0, 5000, NULL},
            {"roi_y",       &(g_stRunVencCfg.vencStream[2][0].cfg.roi_y),        CFG_DATA_TYPE_U16, "0",    "rw", 0,  5000, NULL},
    		{"encodeType",   &(g_stRunVencCfg.vencStream[2][0].cfg.encodeType),   CFG_DATA_TYPE_U8,  "2",    "rw", 0, 5, "0: none, 1: H.264, 2: H.265, 3: MJPEG, 4: jpeg"},
    		{"chromaFormat", &(g_stRunVencCfg.vencStream[2][0].cfg.chromaFormat), CFG_DATA_TYPE_U8,  "1",    "rw", 0, 1,  "0: YUV 422, 1: YUV 420."},
    		{"fps",          &(g_stRunVencCfg.vencStream[2][0].cfg.fps),          CFG_DATA_TYPE_U8,  "25",   "rw", 1, 60,   NULL},
    		{"gop",          &(g_stRunVencCfg.vencStream[2][0].cfg.gop),          CFG_DATA_TYPE_U8,  "30",   "rw", 1, 200,  NULL},
            {"quality",      &(g_stRunVencCfg.vencStream[2][0].cfg.quality),      CFG_DATA_TYPE_U8,  "3",    "rw", 0, 3,  "0: poor, 3: best"},
            {"brcMode",      &(g_stRunVencCfg.vencStream[2][0].cfg.brcMode),      CFG_DATA_TYPE_U8,  "0",    "rw", 0, 6,  "0: CBR; 1: VBR; 2: CVBR, 3: AVBR, 4: QPMAP, 5: FIXQP, 6 QVBR"},
            {"cbrAvgBps",    &(g_stRunVencCfg.vencStream[2][0].cfg.cbrAvgBps),    CFG_DATA_TYPE_U32, "2000", "rw", 128, 16000,   NULL},
            {"vbrMinBps",    &(g_stRunVencCfg.vencStream[2][0].cfg.vbrMinBps),    CFG_DATA_TYPE_U32, "750", "rw", 128, 16000,   NULL},
            {"vbrMaxBps",    &(g_stRunVencCfg.vencStream[2][0].cfg.vbrMaxBps),    CFG_DATA_TYPE_U32, "3000", "rw", 128, 16000,   NULL},
            {"qpMinI",       &(g_stRunVencCfg.vencStream[2][0].cfg.qpMinI),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMinP",       &(g_stRunVencCfg.vencStream[2][0].cfg.qpMinP),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
    		{"qpMaxI",       &(g_stRunVencCfg.vencStream[2][0].cfg.qpMaxI),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
    		{"qpMaxP",       &(g_stRunVencCfg.vencStream[2][0].cfg.qpMaxP),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
    		{"qpIWeight",    &(g_stRunVencCfg.vencStream[2][0].cfg.qpIWeight),    CFG_DATA_TYPE_U8,  "3",    "rw", 0, 100,  NULL},
            {"qpPWeight",    &(g_stRunVencCfg.vencStream[2][0].cfg.qpPWeight),    CFG_DATA_TYPE_U8,  "5",    "rw", 0, 100,  NULL},
    		{"adaptQp",      &(g_stRunVencCfg.vencStream[2][0].cfg.adaptQp),      CFG_DATA_TYPE_U8,  "2",    "rw", 0, 100,  NULL},
    		{"statTime",     &(g_stRunVencCfg.vencStream[2][0].cfg.statTime),      CFG_DATA_TYPE_U8,  "30",  "rw", 1, 60,  NULL},
        },
        //stream 1
        {
    		{"enable",       &(g_stRunVencCfg.vencStream[2][1].enable),           CFG_DATA_TYPE_S32, "0",    "rw", 0, 1, NULL},
            {"id",           &(g_stRunVencCfg.vencStream[2][1].id),               CFG_DATA_TYPE_S16, "1",    "rw", 0, 3, "0-3"},
            {"vi_band_id",	 &(g_stRunVencCfg.vencStream[2][1].vi_band_id),	   CFG_DATA_TYPE_S16, "1",    "rw", 0, 3, NULL},
            {"width",   &(g_stRunVencCfg.vencStream[2][1].cfg.width),    CFG_DATA_TYPE_U16, "640", "rw", 0, 5000, NULL},
            {"height",  &(g_stRunVencCfg.vencStream[2][1].cfg.height),   CFG_DATA_TYPE_U16, "480",  "rw", 0, 5000, NULL},
            {"roi_width",   &(g_stRunVencCfg.vencStream[2][1].cfg.roi_width),    CFG_DATA_TYPE_U16, "640", "rw", 0, 5000, NULL},
            {"roi_height",  &(g_stRunVencCfg.vencStream[2][1].cfg.roi_height),   CFG_DATA_TYPE_U16, "480",  "rw", 0, 5000, NULL},
            {"roi_x",       &(g_stRunVencCfg.vencStream[2][1].cfg.roi_x),        CFG_DATA_TYPE_U16, "0",    "rw", 0, 5000, NULL},
            {"roi_y",       &(g_stRunVencCfg.vencStream[2][1].cfg.roi_y),        CFG_DATA_TYPE_U16, "0",    "rw", 0,  5000, NULL},
    		{"encodeType",   &(g_stRunVencCfg.vencStream[2][1].cfg.encodeType),   CFG_DATA_TYPE_U8,  "2",    "rw", 0, 5, "0: none, 1: H.264, 2: H.265, 3: MJPEG, 4: jpeg"},
    		{"chromaFormat", &(g_stRunVencCfg.vencStream[2][1].cfg.chromaFormat), CFG_DATA_TYPE_U8,  "1",    "rw", 0, 1,  "0: YUV 422, 1: YUV 420."},
    		{"fps",          &(g_stRunVencCfg.vencStream[2][1].cfg.fps),          CFG_DATA_TYPE_U8,  "25",   "rw", 1, 60,   NULL},
    		{"gop",          &(g_stRunVencCfg.vencStream[2][1].cfg.gop),          CFG_DATA_TYPE_U8,  "30",   "rw", 1, 200,  NULL},
            {"quality",      &(g_stRunVencCfg.vencStream[2][1].cfg.quality),      CFG_DATA_TYPE_U8,  "3",    "rw", 0, 3,  "0: poor, 3: best"},
            {"brcMode",      &(g_stRunVencCfg.vencStream[2][1].cfg.brcMode),      CFG_DATA_TYPE_U8,  "0",    "rw", 0, 6,  "0: CBR; 1: VBR; 2: CVBR, 3: AVBR, 4: QPMAP, 5: FIXQP, 6 QVBR"},
            {"cbrAvgBps",    &(g_stRunVencCfg.vencStream[2][1].cfg.cbrAvgBps),    CFG_DATA_TYPE_U32, "1000", "rw", 128, 16000,   NULL},
            {"vbrMinBps",    &(g_stRunVencCfg.vencStream[2][1].cfg.vbrMinBps),    CFG_DATA_TYPE_U32, "400", "rw", 128, 16000,   NULL},
            {"vbrMaxBps",    &(g_stRunVencCfg.vencStream[2][1].cfg.vbrMaxBps),    CFG_DATA_TYPE_U32, "1200", "rw", 128, 16000,   NULL},
            {"qpMinI",       &(g_stRunVencCfg.vencStream[2][1].cfg.qpMinI),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMinP",       &(g_stRunVencCfg.vencStream[2][1].cfg.qpMinP),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
    		{"qpMaxI",       &(g_stRunVencCfg.vencStream[2][1].cfg.qpMaxI),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
    		{"qpMaxP",       &(g_stRunVencCfg.vencStream[2][1].cfg.qpMaxP),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
    		{"qpIWeight",    &(g_stRunVencCfg.vencStream[2][1].cfg.qpIWeight),    CFG_DATA_TYPE_U8,  "3",    "rw", 0, 100,  NULL},
            {"qpPWeight",    &(g_stRunVencCfg.vencStream[2][1].cfg.qpPWeight),    CFG_DATA_TYPE_U8,  "5",    "rw", 0, 100,  NULL},
    		{"adaptQp",      &(g_stRunVencCfg.vencStream[2][1].cfg.adaptQp),      CFG_DATA_TYPE_U8,  "2",    "rw", 0, 100,  NULL},
    		{"statTime",     &(g_stRunVencCfg.vencStream[2][1].cfg.statTime),      CFG_DATA_TYPE_U8,  "30",  "rw", 1, 60,  NULL},
        },
        //stream 2
        {
    		{"enable",       &(g_stRunVencCfg.vencStream[2][2].enable),           CFG_DATA_TYPE_S32, "0",    "rw", 0, 1, NULL},
            {"id",           &(g_stRunVencCfg.vencStream[2][2].id),               CFG_DATA_TYPE_S16, "2",    "rw", 0, 3, "0-3"},
            {"vi_band_id",	 &(g_stRunVencCfg.vencStream[2][2].vi_band_id),	   CFG_DATA_TYPE_S16, "2",    "rw", 0, 3, NULL},
            {"width",   &(g_stRunVencCfg.vencStream[2][2].cfg.width),    CFG_DATA_TYPE_U16, "320", "rw", 0, 5000, NULL},
            {"height",  &(g_stRunVencCfg.vencStream[2][2].cfg.height),   CFG_DATA_TYPE_U16, "240",  "rw", 0, 5000, NULL},
            {"roi_width",   &(g_stRunVencCfg.vencStream[2][2].cfg.roi_width),    CFG_DATA_TYPE_U16, "320", "rw", 0, 5000, NULL},
            {"roi_height",  &(g_stRunVencCfg.vencStream[2][2].cfg.roi_height),   CFG_DATA_TYPE_U16, "240",  "rw", 0, 5000, NULL},
            {"roi_x",       &(g_stRunVencCfg.vencStream[2][2].cfg.roi_x),        CFG_DATA_TYPE_U16, "0",    "rw", 0, 5000, NULL},
            {"roi_y",       &(g_stRunVencCfg.vencStream[2][2].cfg.roi_y),        CFG_DATA_TYPE_U16, "0",    "rw", 0,  5000, NULL},
    		{"encodeType",   &(g_stRunVencCfg.vencStream[2][2].cfg.encodeType),   CFG_DATA_TYPE_U8,  "2",    "rw", 0, 5, "0: none, 1: H.264, 2: H.265, 3: MJPEG, 4: jpeg"},
    		{"chromaFormat", &(g_stRunVencCfg.vencStream[2][2].cfg.chromaFormat), CFG_DATA_TYPE_U8,  "1",    "rw", 0, 1,  "0: YUV 422, 1: YUV 420."},
    		{"fps",          &(g_stRunVencCfg.vencStream[2][2].cfg.fps),          CFG_DATA_TYPE_U8,  "25",   "rw", 1, 60,   NULL},
    		{"gop",          &(g_stRunVencCfg.vencStream[2][2].cfg.gop),          CFG_DATA_TYPE_U8,  "30",   "rw", 1, 200,  NULL},
            {"quality",      &(g_stRunVencCfg.vencStream[2][2].cfg.quality),      CFG_DATA_TYPE_U8,  "3",    "rw", 0, 3,  "0: poor, 3: best"},
            {"brcMode",      &(g_stRunVencCfg.vencStream[2][2].cfg.brcMode),      CFG_DATA_TYPE_U8,  "0",    "rw", 0, 3,  "0: CBR; 1: VBR; 2: FIXQP; 3: AVBR"},
            {"cbrAvgBps",    &(g_stRunVencCfg.vencStream[2][2].cfg.cbrAvgBps),    CFG_DATA_TYPE_U32, "500", "rw", 128, 16000,   NULL},
            {"vbrMinBps",    &(g_stRunVencCfg.vencStream[2][2].cfg.vbrMinBps),    CFG_DATA_TYPE_U32, "300", "rw", 128, 16000,   NULL},
            {"vbrMaxBps",    &(g_stRunVencCfg.vencStream[2][2].cfg.vbrMaxBps),    CFG_DATA_TYPE_U32, "500", "rw", 128, 16000,   NULL},
            {"qpMinI",       &(g_stRunVencCfg.vencStream[2][2].cfg.qpMinI),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMinP",       &(g_stRunVencCfg.vencStream[2][2].cfg.qpMinP),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMaxI",       &(g_stRunVencCfg.vencStream[2][2].cfg.qpMaxI),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
           {"qpMaxP",       &(g_stRunVencCfg.vencStream[2][2].cfg.qpMaxP),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
    		{"qpIWeight",    &(g_stRunVencCfg.vencStream[2][2].cfg.qpIWeight),    CFG_DATA_TYPE_U8,  "3",    "rw", 0, 100,  NULL},
            {"qpPWeight",    &(g_stRunVencCfg.vencStream[2][2].cfg.qpPWeight),    CFG_DATA_TYPE_U8,  "5",    "rw", 0, 100,  NULL},
    		{"adaptQp",      &(g_stRunVencCfg.vencStream[2][2].cfg.adaptQp),      CFG_DATA_TYPE_U8,  "2",    "rw", 0, 100,  NULL},
    		{"statTime",     &(g_stRunVencCfg.vencStream[2][2].cfg.statTime),      CFG_DATA_TYPE_U8,  "30",  "rw", 1, 60,  NULL},
        },
#ifdef THERMAL_ENABLE
        //stream 3
        {
            {"enable",       &(g_stRunVencCfg.vencStream[2][3].enable),           CFG_DATA_TYPE_S32, "0",    "rw", 0, 1, NULL},
            {"id",           &(g_stRunVencCfg.vencStream[2][3].id),               CFG_DATA_TYPE_S16, "3",    "rw", 0, 3, "0-3"},
            {"vi_band_id",   &(g_stRunVencCfg.vencStream[2][3].vi_band_id),    CFG_DATA_TYPE_S16, "3",    "rw", 0, 3, NULL},
            {"width",   &(g_stRunVencCfg.vencStream[2][3].cfg.width),    CFG_DATA_TYPE_U16, "320", "rw", 0, 5000, NULL},
            {"height",  &(g_stRunVencCfg.vencStream[2][3].cfg.height),   CFG_DATA_TYPE_U16, "240",  "rw", 0, 5000, NULL},
            {"roi_width",   &(g_stRunVencCfg.vencStream[2][3].cfg.roi_width),    CFG_DATA_TYPE_U16, "320", "rw", 0, 5000, NULL},
            {"roi_height",  &(g_stRunVencCfg.vencStream[2][3].cfg.roi_height),   CFG_DATA_TYPE_U16, "240",  "rw", 0, 5000, NULL},
            {"roi_x",       &(g_stRunVencCfg.vencStream[2][3].cfg.roi_x),        CFG_DATA_TYPE_U16, "0",    "rw", 0, 5000, NULL},
            {"roi_y",       &(g_stRunVencCfg.vencStream[2][3].cfg.roi_y),        CFG_DATA_TYPE_U16, "0",    "rw", 0,  5000, NULL},
            {"encodeType",   &(g_stRunVencCfg.vencStream[2][3].cfg.encodeType),   CFG_DATA_TYPE_U8,  "2",    "rw", 0, 5, "0: none, 1: H.264, 2: H.265, 3: MJPEG, 4: jpeg"},
            {"chromaFormat", &(g_stRunVencCfg.vencStream[2][3].cfg.chromaFormat), CFG_DATA_TYPE_U8,  "1",    "rw", 0, 1,  "0: YUV 422, 1: YUV 420."},
            {"fps",          &(g_stRunVencCfg.vencStream[2][3].cfg.fps),          CFG_DATA_TYPE_U8,  "25",   "rw", 1, 60,   NULL},
            {"gop",          &(g_stRunVencCfg.vencStream[2][3].cfg.gop),          CFG_DATA_TYPE_U8,  "25",   "rw", 1, 200,  NULL},
            {"quality",      &(g_stRunVencCfg.vencStream[2][3].cfg.quality),      CFG_DATA_TYPE_U8,  "3",    "rw", 0, 3,  "0: poor, 3: best"},
            {"brcMode",      &(g_stRunVencCfg.vencStream[2][3].cfg.brcMode),      CFG_DATA_TYPE_U8,  "0",    "rw", 0, 3,  "0: CBR; 1: VBR; 2: FIXQP; 3: AVBR"},
            {"cbrAvgBps",    &(g_stRunVencCfg.vencStream[2][3].cfg.cbrAvgBps),    CFG_DATA_TYPE_U32, "500", "rw", 0, 32000,   NULL},
            {"vbrMinBps",    &(g_stRunVencCfg.vencStream[2][3].cfg.vbrMinBps),    CFG_DATA_TYPE_U32, "300", "rw", 0, 32000,   NULL},
            {"vbrMaxBps",    &(g_stRunVencCfg.vencStream[2][3].cfg.vbrMaxBps),    CFG_DATA_TYPE_U32, "500", "rw", 0, 32000,   NULL},
            {"qpMinI",       &(g_stRunVencCfg.vencStream[2][3].cfg.qpMinI),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMinP",       &(g_stRunVencCfg.vencStream[2][3].cfg.qpMinP),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMaxI",       &(g_stRunVencCfg.vencStream[2][3].cfg.qpMaxI),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
            {"qpMaxP",       &(g_stRunVencCfg.vencStream[2][3].cfg.qpMaxP),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
            {"qpIWeight",    &(g_stRunVencCfg.vencStream[2][3].cfg.qpIWeight),    CFG_DATA_TYPE_U8,  "3",    "rw", 0, 100,  NULL},
            {"qpPWeight",    &(g_stRunVencCfg.vencStream[2][3].cfg.qpPWeight),    CFG_DATA_TYPE_U8,  "5",    "rw", 0, 100,  NULL},
            {"adaptQp",      &(g_stRunVencCfg.vencStream[2][3].cfg.adaptQp),      CFG_DATA_TYPE_U8,  "2",    "rw", 0, 100,  NULL},
            {"statTime",     &(g_stRunVencCfg.vencStream[2][3].cfg.statTime),      CFG_DATA_TYPE_U8,  "30",  "rw", 1, 60,  NULL},
        }
#endif
    },
	//channel 3
    {
        //stream 0
        {
            {"enable",       &(g_stRunVencCfg.vencStream[3][0].enable),           CFG_DATA_TYPE_S32, "0",    "rw", 0, 1, NULL},
            {"id",           &(g_stRunVencCfg.vencStream[3][0].id),               CFG_DATA_TYPE_S16, "0",    "rw", 0, 3, "0-3"},
            {"vi_band_id",   &(g_stRunVencCfg.vencStream[3][0].vi_band_id),    CFG_DATA_TYPE_S16, "0",    "rw", 0, 3, NULL},
            {"width",   &(g_stRunVencCfg.vencStream[3][0].cfg.width),    CFG_DATA_TYPE_U16, "1920", "rw", 0, 5000, NULL},
            {"height",  &(g_stRunVencCfg.vencStream[3][0].cfg.height),   CFG_DATA_TYPE_U16, "1080",  "rw", 0, 5000, NULL},
            {"roi_width",   &(g_stRunVencCfg.vencStream[3][0].cfg.roi_width),    CFG_DATA_TYPE_U16, "1920", "rw", 0, 5000, NULL},
            {"roi_height",  &(g_stRunVencCfg.vencStream[3][0].cfg.roi_height),   CFG_DATA_TYPE_U16, "1080",  "rw", 0, 5000, NULL},
            {"roi_x",       &(g_stRunVencCfg.vencStream[3][0].cfg.roi_x),        CFG_DATA_TYPE_U16, "0",    "rw", 0, 5000, NULL},
            {"roi_y",       &(g_stRunVencCfg.vencStream[3][0].cfg.roi_y),        CFG_DATA_TYPE_U16, "0",    "rw", 0,  5000, NULL},
            {"encodeType",   &(g_stRunVencCfg.vencStream[3][0].cfg.encodeType),   CFG_DATA_TYPE_U8,  "2",    "rw", 0, 5, "0: none, 1: H.264, 2: H.265, 3: MJPEG, 4: jpeg"},
            {"chromaFormat", &(g_stRunVencCfg.vencStream[3][0].cfg.chromaFormat), CFG_DATA_TYPE_U8,  "1",    "rw", 0, 1,  "0: YUV 422, 1: YUV 420."},
            {"fps",          &(g_stRunVencCfg.vencStream[3][0].cfg.fps),          CFG_DATA_TYPE_U8,  "25",   "rw", 1, 60,   NULL},
            {"gop",          &(g_stRunVencCfg.vencStream[3][0].cfg.gop),          CFG_DATA_TYPE_U8,  "30",   "rw", 1, 200,  NULL},
            {"quality",      &(g_stRunVencCfg.vencStream[3][0].cfg.quality),      CFG_DATA_TYPE_U8,  "3",    "rw", 0, 3,  "0: poor, 3: best"},
            {"brcMode",      &(g_stRunVencCfg.vencStream[3][0].cfg.brcMode),      CFG_DATA_TYPE_U8,  "0",    "rw", 0, 6,  "0: CBR; 1: VBR; 2: CVBR, 3: AVBR, 4: QPMAP, 5: FIXQP, 6 QVBR"},
            {"cbrAvgBps",    &(g_stRunVencCfg.vencStream[3][0].cfg.cbrAvgBps),    CFG_DATA_TYPE_U32, "2000", "rw", 128, 16000,   NULL},
            {"vbrMinBps",    &(g_stRunVencCfg.vencStream[3][0].cfg.vbrMinBps),    CFG_DATA_TYPE_U32, "750", "rw", 128, 16000,   NULL},
            {"vbrMaxBps",    &(g_stRunVencCfg.vencStream[3][0].cfg.vbrMaxBps),    CFG_DATA_TYPE_U32, "3000", "rw", 128, 16000,   NULL},
            {"qpMinI",       &(g_stRunVencCfg.vencStream[3][0].cfg.qpMinI),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMinP",       &(g_stRunVencCfg.vencStream[3][0].cfg.qpMinP),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMaxI",       &(g_stRunVencCfg.vencStream[3][0].cfg.qpMaxI),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
            {"qpMaxP",       &(g_stRunVencCfg.vencStream[3][0].cfg.qpMaxP),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
            {"qpIWeight",    &(g_stRunVencCfg.vencStream[3][0].cfg.qpIWeight),    CFG_DATA_TYPE_U8,  "3",    "rw", 0, 100,  NULL},
            {"qpPWeight",    &(g_stRunVencCfg.vencStream[3][0].cfg.qpPWeight),    CFG_DATA_TYPE_U8,  "5",    "rw", 0, 100,  NULL},
            {"adaptQp",      &(g_stRunVencCfg.vencStream[3][0].cfg.adaptQp),      CFG_DATA_TYPE_U8,  "2",    "rw", 0, 100,  NULL},
            {"statTime",     &(g_stRunVencCfg.vencStream[3][0].cfg.statTime),      CFG_DATA_TYPE_U8,  "30",  "rw", 1, 60,  NULL},
        },
        //stream 1
        {
            {"enable",       &(g_stRunVencCfg.vencStream[3][1].enable),           CFG_DATA_TYPE_S32, "0",    "rw", 0, 1, NULL},
            {"id",           &(g_stRunVencCfg.vencStream[3][1].id),               CFG_DATA_TYPE_S16, "1",    "rw", 0, 3, "0-3"},
            {"vi_band_id",   &(g_stRunVencCfg.vencStream[3][1].vi_band_id),    CFG_DATA_TYPE_S16, "1",    "rw", 0, 3, NULL},
            {"width",   &(g_stRunVencCfg.vencStream[3][1].cfg.width),    CFG_DATA_TYPE_U16, "640", "rw", 0, 5000, NULL},
            {"height",  &(g_stRunVencCfg.vencStream[3][1].cfg.height),   CFG_DATA_TYPE_U16, "480",  "rw", 0, 5000, NULL},
            {"roi_width",   &(g_stRunVencCfg.vencStream[3][1].cfg.roi_width),    CFG_DATA_TYPE_U16, "640", "rw", 0, 5000, NULL},
            {"roi_height",  &(g_stRunVencCfg.vencStream[3][1].cfg.roi_height),   CFG_DATA_TYPE_U16, "480",  "rw", 0, 5000, NULL},
            {"roi_x",       &(g_stRunVencCfg.vencStream[3][1].cfg.roi_x),        CFG_DATA_TYPE_U16, "0",    "rw", 0, 5000, NULL},
            {"roi_y",       &(g_stRunVencCfg.vencStream[3][1].cfg.roi_y),        CFG_DATA_TYPE_U16, "0",    "rw", 0,  5000, NULL},
            {"encodeType",   &(g_stRunVencCfg.vencStream[3][1].cfg.encodeType),   CFG_DATA_TYPE_U8,  "2",    "rw", 0, 5, "0: none, 1: H.264, 2: H.265, 3: MJPEG, 4: jpeg"},
            {"chromaFormat", &(g_stRunVencCfg.vencStream[3][1].cfg.chromaFormat), CFG_DATA_TYPE_U8,  "1",    "rw", 0, 1,  "0: YUV 422, 1: YUV 420."},
            {"fps",          &(g_stRunVencCfg.vencStream[3][1].cfg.fps),          CFG_DATA_TYPE_U8,  "25",   "rw", 1, 60,   NULL},
            {"gop",          &(g_stRunVencCfg.vencStream[3][1].cfg.gop),          CFG_DATA_TYPE_U8,  "30",   "rw", 1, 200,  NULL},
            {"quality",      &(g_stRunVencCfg.vencStream[3][1].cfg.quality),      CFG_DATA_TYPE_U8,  "3",    "rw", 0, 3,  "0: poor, 3: best"},
            {"brcMode",      &(g_stRunVencCfg.vencStream[3][1].cfg.brcMode),      CFG_DATA_TYPE_U8,  "0",    "rw", 0, 6,  "0: CBR; 1: VBR; 2: CVBR, 3: AVBR, 4: QPMAP, 5: FIXQP, 6 QVBR"},
            {"cbrAvgBps",    &(g_stRunVencCfg.vencStream[3][1].cfg.cbrAvgBps),    CFG_DATA_TYPE_U32, "1000", "rw", 128, 16000,   NULL},
            {"vbrMinBps",    &(g_stRunVencCfg.vencStream[3][1].cfg.vbrMinBps),    CFG_DATA_TYPE_U32, "400", "rw", 128, 16000,   NULL},
            {"vbrMaxBps",    &(g_stRunVencCfg.vencStream[3][1].cfg.vbrMaxBps),    CFG_DATA_TYPE_U32, "1200", "rw", 128, 16000,   NULL},
            {"qpMinI",       &(g_stRunVencCfg.vencStream[3][1].cfg.qpMinI),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMinP",       &(g_stRunVencCfg.vencStream[3][1].cfg.qpMinP),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMaxI",       &(g_stRunVencCfg.vencStream[3][1].cfg.qpMaxI),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
            {"qpMaxP",       &(g_stRunVencCfg.vencStream[3][1].cfg.qpMaxP),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
            {"qpIWeight",    &(g_stRunVencCfg.vencStream[3][1].cfg.qpIWeight),    CFG_DATA_TYPE_U8,  "3",    "rw", 0, 100,  NULL},
            {"qpPWeight",    &(g_stRunVencCfg.vencStream[3][1].cfg.qpPWeight),    CFG_DATA_TYPE_U8,  "5",    "rw", 0, 100,  NULL},
            {"adaptQp",      &(g_stRunVencCfg.vencStream[3][1].cfg.adaptQp),      CFG_DATA_TYPE_U8,  "2",    "rw", 0, 100,  NULL},
            {"statTime",     &(g_stRunVencCfg.vencStream[3][1].cfg.statTime),      CFG_DATA_TYPE_U8,  "30",  "rw", 1, 60,  NULL},
        },
        //stream 2
        {
            {"enable",       &(g_stRunVencCfg.vencStream[3][2].enable),           CFG_DATA_TYPE_S32, "0",    "rw", 0, 1, NULL},
            {"id",           &(g_stRunVencCfg.vencStream[3][2].id),               CFG_DATA_TYPE_S16, "2",    "rw", 0, 3, "0-3"},
            {"vi_band_id",   &(g_stRunVencCfg.vencStream[3][2].vi_band_id),    CFG_DATA_TYPE_S16, "2",    "rw", 0, 3, NULL},
            {"width",   &(g_stRunVencCfg.vencStream[3][2].cfg.width),    CFG_DATA_TYPE_U16, "320", "rw", 0, 5000, NULL},
            {"height",  &(g_stRunVencCfg.vencStream[3][2].cfg.height),   CFG_DATA_TYPE_U16, "240",  "rw", 0, 5000, NULL},
            {"roi_width",   &(g_stRunVencCfg.vencStream[3][2].cfg.roi_width),    CFG_DATA_TYPE_U16, "320", "rw", 0, 5000, NULL},
            {"roi_height",  &(g_stRunVencCfg.vencStream[3][2].cfg.roi_height),   CFG_DATA_TYPE_U16, "240",  "rw", 0, 5000, NULL},
            {"roi_x",       &(g_stRunVencCfg.vencStream[3][2].cfg.roi_x),        CFG_DATA_TYPE_U16, "0",    "rw", 0, 5000, NULL},
            {"roi_y",       &(g_stRunVencCfg.vencStream[3][2].cfg.roi_y),        CFG_DATA_TYPE_U16, "0",    "rw", 0,  5000, NULL},
            {"encodeType",   &(g_stRunVencCfg.vencStream[3][2].cfg.encodeType),   CFG_DATA_TYPE_U8,  "2",    "rw", 0, 5, "0: none, 1: H.264, 2: H.265, 3: MJPEG, 4: jpeg"},
            {"chromaFormat", &(g_stRunVencCfg.vencStream[3][2].cfg.chromaFormat), CFG_DATA_TYPE_U8,  "1",    "rw", 0, 1,  "0: YUV 422, 1: YUV 420."},
            {"fps",          &(g_stRunVencCfg.vencStream[3][2].cfg.fps),          CFG_DATA_TYPE_U8,  "25",   "rw", 1, 60,   NULL},
            {"gop",          &(g_stRunVencCfg.vencStream[3][2].cfg.gop),          CFG_DATA_TYPE_U8,  "30",   "rw", 1, 200,  NULL},
            {"quality",      &(g_stRunVencCfg.vencStream[3][2].cfg.quality),      CFG_DATA_TYPE_U8,  "3",    "rw", 0, 3,  "0: poor, 3: best"},
            {"brcMode",      &(g_stRunVencCfg.vencStream[3][2].cfg.brcMode),      CFG_DATA_TYPE_U8,  "0",    "rw", 0, 3,  "0: CBR; 1: VBR; 2: FIXQP; 3: AVBR"},
            {"cbrAvgBps",    &(g_stRunVencCfg.vencStream[3][2].cfg.cbrAvgBps),    CFG_DATA_TYPE_U32, "500", "rw", 128, 16000,   NULL},
            {"vbrMinBps",    &(g_stRunVencCfg.vencStream[3][2].cfg.vbrMinBps),    CFG_DATA_TYPE_U32, "300", "rw", 128, 16000,   NULL},
            {"vbrMaxBps",    &(g_stRunVencCfg.vencStream[3][2].cfg.vbrMaxBps),    CFG_DATA_TYPE_U32, "500", "rw", 128, 16000,   NULL},
            {"qpMinI",       &(g_stRunVencCfg.vencStream[3][2].cfg.qpMinI),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMinP",       &(g_stRunVencCfg.vencStream[3][2].cfg.qpMinP),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMaxI",       &(g_stRunVencCfg.vencStream[3][2].cfg.qpMaxI),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
            {"qpMaxP",       &(g_stRunVencCfg.vencStream[3][2].cfg.qpMaxP),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
            {"qpIWeight",    &(g_stRunVencCfg.vencStream[3][2].cfg.qpIWeight),    CFG_DATA_TYPE_U8,  "3",    "rw", 0, 100,  NULL},
            {"qpPWeight",    &(g_stRunVencCfg.vencStream[3][2].cfg.qpPWeight),    CFG_DATA_TYPE_U8,  "5",    "rw", 0, 100,  NULL},
            {"adaptQp",      &(g_stRunVencCfg.vencStream[3][2].cfg.adaptQp),      CFG_DATA_TYPE_U8,  "2",    "rw", 0, 100,  NULL},
            {"statTime",     &(g_stRunVencCfg.vencStream[3][2].cfg.statTime),      CFG_DATA_TYPE_U8,  "30",  "rw", 1, 60,  NULL},
        },
#ifdef THERMAL_ENABLE
        //stream 3
        {
            {"enable",       &(g_stRunVencCfg.vencStream[3][3].enable),           CFG_DATA_TYPE_S32, "0",    "rw", 0, 1, NULL},
            {"id",           &(g_stRunVencCfg.vencStream[3][3].id),               CFG_DATA_TYPE_S16, "3",    "rw", 0, 3, "0-3"},
            {"vi_band_id",   &(g_stRunVencCfg.vencStream[3][3].vi_band_id),    CFG_DATA_TYPE_S16, "3",    "rw", 0, 3, NULL},
            {"width",   &(g_stRunVencCfg.vencStream[3][3].cfg.width),    CFG_DATA_TYPE_U16, "320", "rw", 0, 5000, NULL},
            {"height",  &(g_stRunVencCfg.vencStream[3][3].cfg.height),   CFG_DATA_TYPE_U16, "240",  "rw", 0, 5000, NULL},
            {"roi_width",   &(g_stRunVencCfg.vencStream[3][3].cfg.roi_width),    CFG_DATA_TYPE_U16, "320", "rw", 0, 5000, NULL},
            {"roi_height",  &(g_stRunVencCfg.vencStream[3][3].cfg.roi_height),   CFG_DATA_TYPE_U16, "240",  "rw", 0, 5000, NULL},
            {"roi_x",       &(g_stRunVencCfg.vencStream[3][3].cfg.roi_x),        CFG_DATA_TYPE_U16, "0",    "rw", 0, 5000, NULL},
            {"roi_y",       &(g_stRunVencCfg.vencStream[3][3].cfg.roi_y),        CFG_DATA_TYPE_U16, "0",    "rw", 0,  5000, NULL},
            {"encodeType",   &(g_stRunVencCfg.vencStream[3][3].cfg.encodeType),   CFG_DATA_TYPE_U8,  "2",    "rw", 0, 5, "0: none, 1: H.264, 2: H.265, 3: MJPEG, 4: jpeg"},
            {"chromaFormat", &(g_stRunVencCfg.vencStream[3][3].cfg.chromaFormat), CFG_DATA_TYPE_U8,  "1",    "rw", 0, 1,  "0: YUV 422, 1: YUV 420."},
            {"fps",          &(g_stRunVencCfg.vencStream[3][3].cfg.fps),          CFG_DATA_TYPE_U8,  "25",   "rw", 1, 60,   NULL},
            {"gop",          &(g_stRunVencCfg.vencStream[3][3].cfg.gop),          CFG_DATA_TYPE_U8,  "25",   "rw", 1, 200,  NULL},
            {"quality",      &(g_stRunVencCfg.vencStream[3][3].cfg.quality),      CFG_DATA_TYPE_U8,  "3",    "rw", 0, 3,  "0: poor, 3: best"},
            {"brcMode",      &(g_stRunVencCfg.vencStream[3][3].cfg.brcMode),      CFG_DATA_TYPE_U8,  "0",    "rw", 0, 3,  "0: CBR; 1: VBR; 2: FIXQP; 3: AVBR"},
            {"cbrAvgBps",    &(g_stRunVencCfg.vencStream[3][3].cfg.cbrAvgBps),    CFG_DATA_TYPE_U32, "500", "rw", 0, 32000,   NULL},
            {"vbrMinBps",    &(g_stRunVencCfg.vencStream[3][3].cfg.vbrMinBps),    CFG_DATA_TYPE_U32, "300", "rw", 0, 32000,   NULL},
            {"vbrMaxBps",    &(g_stRunVencCfg.vencStream[3][3].cfg.vbrMaxBps),    CFG_DATA_TYPE_U32, "500", "rw", 0, 32000,   NULL},
            {"qpMinI",       &(g_stRunVencCfg.vencStream[3][3].cfg.qpMinI),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMinP",       &(g_stRunVencCfg.vencStream[3][3].cfg.qpMinP),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMaxI",       &(g_stRunVencCfg.vencStream[3][3].cfg.qpMaxI),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
            {"qpMaxP",       &(g_stRunVencCfg.vencStream[3][3].cfg.qpMaxP),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
            {"qpIWeight",    &(g_stRunVencCfg.vencStream[3][3].cfg.qpIWeight),    CFG_DATA_TYPE_U8,  "3",    "rw", 0, 100,  NULL},
            {"qpPWeight",    &(g_stRunVencCfg.vencStream[3][3].cfg.qpPWeight),    CFG_DATA_TYPE_U8,  "5",    "rw", 0, 100,  NULL},
            {"adaptQp",      &(g_stRunVencCfg.vencStream[3][3].cfg.adaptQp),      CFG_DATA_TYPE_U8,  "2",    "rw", 0, 100,  NULL},
            {"statTime",     &(g_stRunVencCfg.vencStream[3][3].cfg.statTime),      CFG_DATA_TYPE_U8,  "30",  "rw", 1, 60,  NULL},
        }
#endif
    },
	//channel 4
    {
        //stream 0
        {
            {"enable",       &(g_stRunVencCfg.vencStream[4][0].enable),           CFG_DATA_TYPE_S32, "0",    "rw", 0, 1, NULL},
            {"id",           &(g_stRunVencCfg.vencStream[4][0].id),               CFG_DATA_TYPE_S16, "0",    "rw", 0, 3, "0-3"},
            {"vi_band_id",   &(g_stRunVencCfg.vencStream[4][0].vi_band_id),    CFG_DATA_TYPE_S16, "0",    "rw", 0, 3, NULL},
            {"width",   &(g_stRunVencCfg.vencStream[4][0].cfg.width),    CFG_DATA_TYPE_U16, "1920", "rw", 0, 5000, NULL},
            {"height",  &(g_stRunVencCfg.vencStream[4][0].cfg.height),   CFG_DATA_TYPE_U16, "1080",  "rw", 0, 5000, NULL},
            {"roi_width",   &(g_stRunVencCfg.vencStream[4][0].cfg.roi_width),    CFG_DATA_TYPE_U16, "1920", "rw", 0, 5000, NULL},
            {"roi_height",  &(g_stRunVencCfg.vencStream[4][0].cfg.roi_height),   CFG_DATA_TYPE_U16, "1080",  "rw", 0, 5000, NULL},
            {"roi_x",       &(g_stRunVencCfg.vencStream[4][0].cfg.roi_x),        CFG_DATA_TYPE_U16, "0",    "rw", 0, 5000, NULL},
            {"roi_y",       &(g_stRunVencCfg.vencStream[4][0].cfg.roi_y),        CFG_DATA_TYPE_U16, "0",    "rw", 0,  5000, NULL},
            {"encodeType",   &(g_stRunVencCfg.vencStream[4][0].cfg.encodeType),   CFG_DATA_TYPE_U8,  "2",    "rw", 0, 5, "0: none, 1: H.264, 2: H.265, 3: MJPEG, 4: jpeg"},
            {"chromaFormat", &(g_stRunVencCfg.vencStream[4][0].cfg.chromaFormat), CFG_DATA_TYPE_U8,  "1",    "rw", 0, 1,  "0: YUV 422, 1: YUV 420."},
            {"fps",          &(g_stRunVencCfg.vencStream[4][0].cfg.fps),          CFG_DATA_TYPE_U8,  "25",   "rw", 1, 60,   NULL},
            {"gop",          &(g_stRunVencCfg.vencStream[4][0].cfg.gop),          CFG_DATA_TYPE_U8,  "30",   "rw", 1, 200,  NULL},
            {"quality",      &(g_stRunVencCfg.vencStream[4][0].cfg.quality),      CFG_DATA_TYPE_U8,  "3",    "rw", 0, 3,  "0: poor, 3: best"},
            {"brcMode",      &(g_stRunVencCfg.vencStream[4][0].cfg.brcMode),      CFG_DATA_TYPE_U8,  "0",    "rw", 0, 6,  "0: CBR; 1: VBR; 2: CVBR, 3: AVBR, 4: QPMAP, 5: FIXQP, 6 QVBR"},
            {"cbrAvgBps",    &(g_stRunVencCfg.vencStream[4][0].cfg.cbrAvgBps),    CFG_DATA_TYPE_U32, "2000", "rw", 128, 16000,   NULL},
            {"vbrMinBps",    &(g_stRunVencCfg.vencStream[4][0].cfg.vbrMinBps),    CFG_DATA_TYPE_U32, "750", "rw", 128, 16000,   NULL},
            {"vbrMaxBps",    &(g_stRunVencCfg.vencStream[4][0].cfg.vbrMaxBps),    CFG_DATA_TYPE_U32, "3000", "rw", 128, 16000,   NULL},
            {"qpMinI",       &(g_stRunVencCfg.vencStream[4][0].cfg.qpMinI),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMinP",       &(g_stRunVencCfg.vencStream[4][0].cfg.qpMinP),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMaxI",       &(g_stRunVencCfg.vencStream[4][0].cfg.qpMaxI),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
            {"qpMaxP",       &(g_stRunVencCfg.vencStream[4][0].cfg.qpMaxP),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
            {"qpIWeight",    &(g_stRunVencCfg.vencStream[4][0].cfg.qpIWeight),    CFG_DATA_TYPE_U8,  "3",    "rw", 0, 100,  NULL},
            {"qpPWeight",    &(g_stRunVencCfg.vencStream[4][0].cfg.qpPWeight),    CFG_DATA_TYPE_U8,  "5",    "rw", 0, 100,  NULL},
            {"adaptQp",      &(g_stRunVencCfg.vencStream[4][0].cfg.adaptQp),      CFG_DATA_TYPE_U8,  "2",    "rw", 0, 100,  NULL},
            {"statTime",     &(g_stRunVencCfg.vencStream[4][0].cfg.statTime),      CFG_DATA_TYPE_U8,  "30",  "rw", 1, 60,  NULL},
        },
        //stream 1
        {
            {"enable",       &(g_stRunVencCfg.vencStream[4][1].enable),           CFG_DATA_TYPE_S32, "0",    "rw", 0, 1, NULL},
            {"id",           &(g_stRunVencCfg.vencStream[4][1].id),               CFG_DATA_TYPE_S16, "1",    "rw", 0, 3, "0-3"},
            {"vi_band_id",   &(g_stRunVencCfg.vencStream[4][1].vi_band_id),    CFG_DATA_TYPE_S16, "1",    "rw", 0, 3, NULL},
            {"width",   &(g_stRunVencCfg.vencStream[4][1].cfg.width),    CFG_DATA_TYPE_U16, "640", "rw", 0, 5000, NULL},
            {"height",  &(g_stRunVencCfg.vencStream[4][1].cfg.height),   CFG_DATA_TYPE_U16, "480",  "rw", 0, 5000, NULL},
            {"roi_width",   &(g_stRunVencCfg.vencStream[4][1].cfg.roi_width),    CFG_DATA_TYPE_U16, "640", "rw", 0, 5000, NULL},
            {"roi_height",  &(g_stRunVencCfg.vencStream[4][1].cfg.roi_height),   CFG_DATA_TYPE_U16, "480",  "rw", 0, 5000, NULL},
            {"roi_x",       &(g_stRunVencCfg.vencStream[4][1].cfg.roi_x),        CFG_DATA_TYPE_U16, "0",    "rw", 0, 5000, NULL},
            {"roi_y",       &(g_stRunVencCfg.vencStream[4][1].cfg.roi_y),        CFG_DATA_TYPE_U16, "0",    "rw", 0,  5000, NULL},
            {"encodeType",   &(g_stRunVencCfg.vencStream[4][1].cfg.encodeType),   CFG_DATA_TYPE_U8,  "2",    "rw", 0, 5, "0: none, 1: H.264, 2: H.265, 3: MJPEG, 4: jpeg"},
            {"chromaFormat", &(g_stRunVencCfg.vencStream[4][1].cfg.chromaFormat), CFG_DATA_TYPE_U8,  "1",    "rw", 0, 1,  "0: YUV 422, 1: YUV 420."},
            {"fps",          &(g_stRunVencCfg.vencStream[4][1].cfg.fps),          CFG_DATA_TYPE_U8,  "25",   "rw", 1, 60,   NULL},
            {"gop",          &(g_stRunVencCfg.vencStream[4][1].cfg.gop),          CFG_DATA_TYPE_U8,  "30",   "rw", 1, 200,  NULL},
            {"quality",      &(g_stRunVencCfg.vencStream[4][1].cfg.quality),      CFG_DATA_TYPE_U8,  "3",    "rw", 0, 3,  "0: poor, 3: best"},
            {"brcMode",      &(g_stRunVencCfg.vencStream[4][1].cfg.brcMode),      CFG_DATA_TYPE_U8,  "0",    "rw", 0, 6,  "0: CBR; 1: VBR; 2: CVBR, 3: AVBR, 4: QPMAP, 5: FIXQP, 6 QVBR"},
            {"cbrAvgBps",    &(g_stRunVencCfg.vencStream[4][1].cfg.cbrAvgBps),    CFG_DATA_TYPE_U32, "1000", "rw", 128, 16000,   NULL},
            {"vbrMinBps",    &(g_stRunVencCfg.vencStream[4][1].cfg.vbrMinBps),    CFG_DATA_TYPE_U32, "400", "rw", 128, 16000,   NULL},
            {"vbrMaxBps",    &(g_stRunVencCfg.vencStream[4][1].cfg.vbrMaxBps),    CFG_DATA_TYPE_U32, "1200", "rw", 128, 16000,   NULL},
            {"qpMinI",       &(g_stRunVencCfg.vencStream[4][1].cfg.qpMinI),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMinP",       &(g_stRunVencCfg.vencStream[4][1].cfg.qpMinP),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMaxI",       &(g_stRunVencCfg.vencStream[4][1].cfg.qpMaxI),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
            {"qpMaxP",       &(g_stRunVencCfg.vencStream[4][1].cfg.qpMaxP),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
            {"qpIWeight",    &(g_stRunVencCfg.vencStream[4][1].cfg.qpIWeight),    CFG_DATA_TYPE_U8,  "3",    "rw", 0, 100,  NULL},
            {"qpPWeight",    &(g_stRunVencCfg.vencStream[4][1].cfg.qpPWeight),    CFG_DATA_TYPE_U8,  "5",    "rw", 0, 100,  NULL},
            {"adaptQp",      &(g_stRunVencCfg.vencStream[4][1].cfg.adaptQp),      CFG_DATA_TYPE_U8,  "2",    "rw", 0, 100,  NULL},
            {"statTime",     &(g_stRunVencCfg.vencStream[4][1].cfg.statTime),      CFG_DATA_TYPE_U8,  "30",  "rw", 1, 60,  NULL},
        },
        //stream 2
        {
            {"enable",       &(g_stRunVencCfg.vencStream[4][2].enable),           CFG_DATA_TYPE_S32, "0",    "rw", 0, 1, NULL},
            {"id",           &(g_stRunVencCfg.vencStream[4][2].id),               CFG_DATA_TYPE_S16, "2",    "rw", 0, 3, "0-3"},
            {"vi_band_id",   &(g_stRunVencCfg.vencStream[4][2].vi_band_id),    CFG_DATA_TYPE_S16, "2",    "rw", 0, 3, NULL},
            {"width",   &(g_stRunVencCfg.vencStream[4][2].cfg.width),    CFG_DATA_TYPE_U16, "320", "rw", 0, 5000, NULL},
            {"height",  &(g_stRunVencCfg.vencStream[4][2].cfg.height),   CFG_DATA_TYPE_U16, "240",  "rw", 0, 5000, NULL},
            {"roi_width",   &(g_stRunVencCfg.vencStream[4][2].cfg.roi_width),    CFG_DATA_TYPE_U16, "320", "rw", 0, 5000, NULL},
            {"roi_height",  &(g_stRunVencCfg.vencStream[4][2].cfg.roi_height),   CFG_DATA_TYPE_U16, "240",  "rw", 0, 5000, NULL},
            {"roi_x",       &(g_stRunVencCfg.vencStream[4][2].cfg.roi_x),        CFG_DATA_TYPE_U16, "0",    "rw", 0, 5000, NULL},
            {"roi_y",       &(g_stRunVencCfg.vencStream[4][2].cfg.roi_y),        CFG_DATA_TYPE_U16, "0",    "rw", 0,  5000, NULL},
            {"encodeType",   &(g_stRunVencCfg.vencStream[4][2].cfg.encodeType),   CFG_DATA_TYPE_U8,  "2",    "rw", 0, 5, "0: none, 1: H.264, 2: H.265, 3: MJPEG, 4: jpeg"},
            {"chromaFormat", &(g_stRunVencCfg.vencStream[4][2].cfg.chromaFormat), CFG_DATA_TYPE_U8,  "1",    "rw", 0, 1,  "0: YUV 422, 1: YUV 420."},
            {"fps",          &(g_stRunVencCfg.vencStream[4][2].cfg.fps),          CFG_DATA_TYPE_U8,  "25",   "rw", 1, 60,   NULL},
            {"gop",          &(g_stRunVencCfg.vencStream[4][2].cfg.gop),          CFG_DATA_TYPE_U8,  "30",   "rw", 1, 200,  NULL},
            {"quality",      &(g_stRunVencCfg.vencStream[4][2].cfg.quality),      CFG_DATA_TYPE_U8,  "3",    "rw", 0, 3,  "0: poor, 3: best"},
            {"brcMode",      &(g_stRunVencCfg.vencStream[4][2].cfg.brcMode),      CFG_DATA_TYPE_U8,  "0",    "rw", 0, 3,  "0: CBR; 1: VBR; 2: FIXQP; 3: AVBR"},
            {"cbrAvgBps",    &(g_stRunVencCfg.vencStream[4][2].cfg.cbrAvgBps),    CFG_DATA_TYPE_U32, "500", "rw", 128, 16000,   NULL},
            {"vbrMinBps",    &(g_stRunVencCfg.vencStream[4][2].cfg.vbrMinBps),    CFG_DATA_TYPE_U32, "300", "rw", 128, 16000,   NULL},
            {"vbrMaxBps",    &(g_stRunVencCfg.vencStream[4][2].cfg.vbrMaxBps),    CFG_DATA_TYPE_U32, "500", "rw", 128, 16000,   NULL},
            {"qpMinI",       &(g_stRunVencCfg.vencStream[4][2].cfg.qpMinI),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMinP",       &(g_stRunVencCfg.vencStream[4][2].cfg.qpMinP),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMaxI",       &(g_stRunVencCfg.vencStream[4][2].cfg.qpMaxI),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
            {"qpMaxP",       &(g_stRunVencCfg.vencStream[4][2].cfg.qpMaxP),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
            {"qpIWeight",    &(g_stRunVencCfg.vencStream[4][2].cfg.qpIWeight),    CFG_DATA_TYPE_U8,  "3",    "rw", 0, 100,  NULL},
            {"qpPWeight",    &(g_stRunVencCfg.vencStream[4][2].cfg.qpPWeight),    CFG_DATA_TYPE_U8,  "5",    "rw", 0, 100,  NULL},
            {"adaptQp",      &(g_stRunVencCfg.vencStream[4][2].cfg.adaptQp),      CFG_DATA_TYPE_U8,  "2",    "rw", 0, 100,  NULL},
            {"statTime",     &(g_stRunVencCfg.vencStream[4][2].cfg.statTime),      CFG_DATA_TYPE_U8,  "30",  "rw", 1, 60,  NULL},
        },
#ifdef THERMAL_ENABLE
        //stream 3
        {
            {"enable",       &(g_stRunVencCfg.vencStream[4][3].enable),           CFG_DATA_TYPE_S32, "0",    "rw", 0, 1, NULL},
            {"id",           &(g_stRunVencCfg.vencStream[4][3].id),               CFG_DATA_TYPE_S16, "3",    "rw", 0, 3, "0-3"},
            {"vi_band_id",   &(g_stRunVencCfg.vencStream[4][3].vi_band_id),    CFG_DATA_TYPE_S16, "3",    "rw", 0, 3, NULL},
            {"width",   &(g_stRunVencCfg.vencStream[4][3].cfg.width),    CFG_DATA_TYPE_U16, "320", "rw", 0, 5000, NULL},
            {"height",  &(g_stRunVencCfg.vencStream[4][3].cfg.height),   CFG_DATA_TYPE_U16, "240",  "rw", 0, 5000, NULL},
            {"roi_width",   &(g_stRunVencCfg.vencStream[4][3].cfg.roi_width),    CFG_DATA_TYPE_U16, "320", "rw", 0, 5000, NULL},
            {"roi_height",  &(g_stRunVencCfg.vencStream[4][3].cfg.roi_height),   CFG_DATA_TYPE_U16, "240",  "rw", 0, 5000, NULL},
            {"roi_x",       &(g_stRunVencCfg.vencStream[4][3].cfg.roi_x),        CFG_DATA_TYPE_U16, "0",    "rw", 0, 5000, NULL},
            {"roi_y",       &(g_stRunVencCfg.vencStream[4][3].cfg.roi_y),        CFG_DATA_TYPE_U16, "0",    "rw", 0,  5000, NULL},
            {"encodeType",   &(g_stRunVencCfg.vencStream[4][3].cfg.encodeType),   CFG_DATA_TYPE_U8,  "2",    "rw", 0, 5, "0: none, 1: H.264, 2: H.265, 3: MJPEG, 4: jpeg"},
            {"chromaFormat", &(g_stRunVencCfg.vencStream[4][3].cfg.chromaFormat), CFG_DATA_TYPE_U8,  "1",    "rw", 0, 1,  "0: YUV 422, 1: YUV 420."},
            {"fps",          &(g_stRunVencCfg.vencStream[4][3].cfg.fps),          CFG_DATA_TYPE_U8,  "25",   "rw", 1, 60,   NULL},
            {"gop",          &(g_stRunVencCfg.vencStream[4][3].cfg.gop),          CFG_DATA_TYPE_U8,  "25",   "rw", 1, 200,  NULL},
            {"quality",      &(g_stRunVencCfg.vencStream[4][3].cfg.quality),      CFG_DATA_TYPE_U8,  "3",    "rw", 0, 3,  "0: poor, 3: best"},
            {"brcMode",      &(g_stRunVencCfg.vencStream[4][3].cfg.brcMode),      CFG_DATA_TYPE_U8,  "0",    "rw", 0, 3,  "0: CBR; 1: VBR; 2: FIXQP; 3: AVBR"},
            {"cbrAvgBps",    &(g_stRunVencCfg.vencStream[4][3].cfg.cbrAvgBps),    CFG_DATA_TYPE_U32, "500", "rw", 0, 32000,   NULL},
            {"vbrMinBps",    &(g_stRunVencCfg.vencStream[4][3].cfg.vbrMinBps),    CFG_DATA_TYPE_U32, "300", "rw", 0, 32000,   NULL},
            {"vbrMaxBps",    &(g_stRunVencCfg.vencStream[4][3].cfg.vbrMaxBps),    CFG_DATA_TYPE_U32, "500", "rw", 0, 32000,   NULL},
            {"qpMinI",       &(g_stRunVencCfg.vencStream[4][3].cfg.qpMinI),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMinP",       &(g_stRunVencCfg.vencStream[4][3].cfg.qpMinP),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMaxI",       &(g_stRunVencCfg.vencStream[4][3].cfg.qpMaxI),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
            {"qpMaxP",       &(g_stRunVencCfg.vencStream[4][3].cfg.qpMaxP),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
            {"qpIWeight",    &(g_stRunVencCfg.vencStream[4][3].cfg.qpIWeight),    CFG_DATA_TYPE_U8,  "3",    "rw", 0, 100,  NULL},
            {"qpPWeight",    &(g_stRunVencCfg.vencStream[4][3].cfg.qpPWeight),    CFG_DATA_TYPE_U8,  "5",    "rw", 0, 100,  NULL},
            {"adaptQp",      &(g_stRunVencCfg.vencStream[4][3].cfg.adaptQp),      CFG_DATA_TYPE_U8,  "2",    "rw", 0, 100,  NULL},
            {"statTime",     &(g_stRunVencCfg.vencStream[4][3].cfg.statTime),      CFG_DATA_TYPE_U8,  "30",  "rw", 1, 60,  NULL},
        }
#endif
    },
	//channel 5
    {
        //stream 0
        {
    		{"enable",       &(g_stRunVencCfg.vencStream[5][0].enable),           CFG_DATA_TYPE_S32, "0",    "rw", 0, 1, NULL},
            {"id",           &(g_stRunVencCfg.vencStream[5][0].id),               CFG_DATA_TYPE_S16, "0",    "rw", 0, 3, "0-3"},
            {"vi_band_id",	 &(g_stRunVencCfg.vencStream[5][0].vi_band_id),	   CFG_DATA_TYPE_S16, "0",    "rw", 0, 3, NULL},
            {"width",   &(g_stRunVencCfg.vencStream[5][0].cfg.width),    CFG_DATA_TYPE_U16, "1920", "rw", 0, 5000, NULL},
            {"height",  &(g_stRunVencCfg.vencStream[5][0].cfg.height),   CFG_DATA_TYPE_U16, "1080",  "rw", 0, 5000, NULL},
            {"roi_width",   &(g_stRunVencCfg.vencStream[5][0].cfg.roi_width),    CFG_DATA_TYPE_U16, "1920", "rw", 0, 5000, NULL},
            {"roi_height",  &(g_stRunVencCfg.vencStream[5][0].cfg.roi_height),   CFG_DATA_TYPE_U16, "1080",  "rw", 0, 5000, NULL},
            {"roi_x",       &(g_stRunVencCfg.vencStream[5][0].cfg.roi_x),        CFG_DATA_TYPE_U16, "0",    "rw", 0, 5000, NULL},
            {"roi_y",       &(g_stRunVencCfg.vencStream[5][0].cfg.roi_y),        CFG_DATA_TYPE_U16, "0",    "rw", 0,  5000, NULL},
    		{"encodeType",   &(g_stRunVencCfg.vencStream[5][0].cfg.encodeType),   CFG_DATA_TYPE_U8,  "2",    "rw", 0, 5, "0: none, 1: H.264, 2: H.265, 3: MJPEG, 4: jpeg"},
    		{"chromaFormat", &(g_stRunVencCfg.vencStream[5][0].cfg.chromaFormat), CFG_DATA_TYPE_U8,  "1",    "rw", 0, 1,  "0: YUV 422, 1: YUV 420."},
    		{"fps",          &(g_stRunVencCfg.vencStream[5][0].cfg.fps),          CFG_DATA_TYPE_U8,  "25",   "rw", 1, 60,   NULL},
    		{"gop",          &(g_stRunVencCfg.vencStream[5][0].cfg.gop),          CFG_DATA_TYPE_U8,  "30",   "rw", 1, 200,  NULL},
            {"quality",      &(g_stRunVencCfg.vencStream[5][0].cfg.quality),      CFG_DATA_TYPE_U8,  "3",    "rw", 0, 3,  "0: poor, 3: best"},
            {"brcMode",      &(g_stRunVencCfg.vencStream[5][0].cfg.brcMode),      CFG_DATA_TYPE_U8,  "0",    "rw", 0, 6,  "0: CBR; 1: VBR; 2: CVBR, 3: AVBR, 4: QPMAP, 5: FIXQP, 6 QVBR"},
            {"cbrAvgBps",    &(g_stRunVencCfg.vencStream[5][0].cfg.cbrAvgBps),    CFG_DATA_TYPE_U32, "2000", "rw", 128, 16000,   NULL},
            {"vbrMinBps",    &(g_stRunVencCfg.vencStream[5][0].cfg.vbrMinBps),    CFG_DATA_TYPE_U32, "750", "rw", 128, 16000,   NULL},
            {"vbrMaxBps",    &(g_stRunVencCfg.vencStream[5][0].cfg.vbrMaxBps),    CFG_DATA_TYPE_U32, "3000", "rw", 128, 16000,   NULL},
            {"qpMinI",       &(g_stRunVencCfg.vencStream[5][0].cfg.qpMinI),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMinP",       &(g_stRunVencCfg.vencStream[5][0].cfg.qpMinP),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
    		{"qpMaxI",       &(g_stRunVencCfg.vencStream[5][0].cfg.qpMaxI),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
    		{"qpMaxP",       &(g_stRunVencCfg.vencStream[5][0].cfg.qpMaxP),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
    		{"qpIWeight",    &(g_stRunVencCfg.vencStream[5][0].cfg.qpIWeight),    CFG_DATA_TYPE_U8,  "3",    "rw", 0, 100,  NULL},
            {"qpPWeight",    &(g_stRunVencCfg.vencStream[5][0].cfg.qpPWeight),    CFG_DATA_TYPE_U8,  "5",    "rw", 0, 100,  NULL},
    		{"adaptQp",      &(g_stRunVencCfg.vencStream[5][0].cfg.adaptQp),      CFG_DATA_TYPE_U8,  "2",    "rw", 0, 100,  NULL},
    		{"statTime",     &(g_stRunVencCfg.vencStream[5][0].cfg.statTime),      CFG_DATA_TYPE_U8,  "30",  "rw", 1, 60,  NULL},
        },
        //stream 1
        {
    		{"enable",       &(g_stRunVencCfg.vencStream[5][1].enable),           CFG_DATA_TYPE_S32, "0",    "rw", 0, 1, NULL},
            {"id",           &(g_stRunVencCfg.vencStream[5][1].id),               CFG_DATA_TYPE_S16, "1",    "rw", 0, 3, "0-3"},
            {"vi_band_id",	 &(g_stRunVencCfg.vencStream[5][1].vi_band_id),	   CFG_DATA_TYPE_S16, "1",    "rw", 0, 3, NULL},
            {"width",   &(g_stRunVencCfg.vencStream[5][1].cfg.width),    CFG_DATA_TYPE_U16, "640", "rw", 0, 5000, NULL},
            {"height",  &(g_stRunVencCfg.vencStream[5][1].cfg.height),   CFG_DATA_TYPE_U16, "480",  "rw", 0, 5000, NULL},
            {"roi_width",   &(g_stRunVencCfg.vencStream[5][1].cfg.roi_width),    CFG_DATA_TYPE_U16, "640", "rw", 0, 5000, NULL},
            {"roi_height",  &(g_stRunVencCfg.vencStream[5][1].cfg.roi_height),   CFG_DATA_TYPE_U16, "480",  "rw", 0, 5000, NULL},
            {"roi_x",       &(g_stRunVencCfg.vencStream[5][1].cfg.roi_x),        CFG_DATA_TYPE_U16, "0",    "rw", 0, 5000, NULL},
            {"roi_y",       &(g_stRunVencCfg.vencStream[5][1].cfg.roi_y),        CFG_DATA_TYPE_U16, "0",    "rw", 0,  5000, NULL},
    		{"encodeType",   &(g_stRunVencCfg.vencStream[5][1].cfg.encodeType),   CFG_DATA_TYPE_U8,  "2",    "rw", 0, 5, "0: none, 1: H.264, 2: H.265, 3: MJPEG, 4: jpeg"},
    		{"chromaFormat", &(g_stRunVencCfg.vencStream[5][1].cfg.chromaFormat), CFG_DATA_TYPE_U8,  "1",    "rw", 0, 1,  "0: YUV 422, 1: YUV 420."},
    		{"fps",          &(g_stRunVencCfg.vencStream[5][1].cfg.fps),          CFG_DATA_TYPE_U8,  "25",   "rw", 1, 60,   NULL},
    		{"gop",          &(g_stRunVencCfg.vencStream[5][1].cfg.gop),          CFG_DATA_TYPE_U8,  "30",   "rw", 1, 200,  NULL},
            {"quality",      &(g_stRunVencCfg.vencStream[5][1].cfg.quality),      CFG_DATA_TYPE_U8,  "3",    "rw", 0, 3,  "0: poor, 3: best"},
            {"brcMode",      &(g_stRunVencCfg.vencStream[5][1].cfg.brcMode),      CFG_DATA_TYPE_U8,  "0",    "rw", 0, 6,  "0: CBR; 1: VBR; 2: CVBR, 3: AVBR, 4: QPMAP, 5: FIXQP, 6 QVBR"},
            {"cbrAvgBps",    &(g_stRunVencCfg.vencStream[5][1].cfg.cbrAvgBps),    CFG_DATA_TYPE_U32, "1000", "rw", 128, 16000,   NULL},
            {"vbrMinBps",    &(g_stRunVencCfg.vencStream[5][1].cfg.vbrMinBps),    CFG_DATA_TYPE_U32, "400", "rw", 128, 16000,   NULL},
            {"vbrMaxBps",    &(g_stRunVencCfg.vencStream[5][1].cfg.vbrMaxBps),    CFG_DATA_TYPE_U32, "1200", "rw", 128, 16000,   NULL},
            {"qpMinI",       &(g_stRunVencCfg.vencStream[5][1].cfg.qpMinI),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMinP",       &(g_stRunVencCfg.vencStream[5][1].cfg.qpMinP),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
    		{"qpMaxI",       &(g_stRunVencCfg.vencStream[5][1].cfg.qpMaxI),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
    		{"qpMaxP",       &(g_stRunVencCfg.vencStream[5][1].cfg.qpMaxP),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
    		{"qpIWeight",    &(g_stRunVencCfg.vencStream[5][1].cfg.qpIWeight),    CFG_DATA_TYPE_U8,  "3",    "rw", 0, 100,  NULL},
            {"qpPWeight",    &(g_stRunVencCfg.vencStream[5][1].cfg.qpPWeight),    CFG_DATA_TYPE_U8,  "5",    "rw", 0, 100,  NULL},
    		{"adaptQp",      &(g_stRunVencCfg.vencStream[5][1].cfg.adaptQp),      CFG_DATA_TYPE_U8,  "2",    "rw", 0, 100,  NULL},
    		{"statTime",     &(g_stRunVencCfg.vencStream[5][1].cfg.statTime),      CFG_DATA_TYPE_U8,  "30",  "rw", 1, 60,  NULL},
        },
        //stream 2
        {
    		{"enable",       &(g_stRunVencCfg.vencStream[5][2].enable),           CFG_DATA_TYPE_S32, "0",    "rw", 0, 1, NULL},
            {"id",           &(g_stRunVencCfg.vencStream[5][2].id),               CFG_DATA_TYPE_S16, "2",    "rw", 0, 3, "0-3"},
            {"vi_band_id",	 &(g_stRunVencCfg.vencStream[5][2].vi_band_id),	   CFG_DATA_TYPE_S16, "2",    "rw", 0, 3, NULL},
            {"width",   &(g_stRunVencCfg.vencStream[5][2].cfg.width),    CFG_DATA_TYPE_U16, "320", "rw", 0, 5000, NULL},
            {"height",  &(g_stRunVencCfg.vencStream[5][2].cfg.height),   CFG_DATA_TYPE_U16, "240",  "rw", 0, 5000, NULL},
            {"roi_width",   &(g_stRunVencCfg.vencStream[5][2].cfg.roi_width),    CFG_DATA_TYPE_U16, "320", "rw", 0, 5000, NULL},
            {"roi_height",  &(g_stRunVencCfg.vencStream[5][2].cfg.roi_height),   CFG_DATA_TYPE_U16, "240",  "rw", 0, 5000, NULL},
            {"roi_x",       &(g_stRunVencCfg.vencStream[5][2].cfg.roi_x),        CFG_DATA_TYPE_U16, "0",    "rw", 0, 5000, NULL},
            {"roi_y",       &(g_stRunVencCfg.vencStream[5][2].cfg.roi_y),        CFG_DATA_TYPE_U16, "0",    "rw", 0,  5000, NULL},
    		{"encodeType",   &(g_stRunVencCfg.vencStream[5][2].cfg.encodeType),   CFG_DATA_TYPE_U8,  "2",    "rw", 0, 5, "0: none, 1: H.264, 2: H.265, 3: MJPEG, 4: jpeg"},
    		{"chromaFormat", &(g_stRunVencCfg.vencStream[5][2].cfg.chromaFormat), CFG_DATA_TYPE_U8,  "1",    "rw", 0, 1,  "0: YUV 422, 1: YUV 420."},
    		{"fps",          &(g_stRunVencCfg.vencStream[5][2].cfg.fps),          CFG_DATA_TYPE_U8,  "25",   "rw", 1, 60,   NULL},
    		{"gop",          &(g_stRunVencCfg.vencStream[5][2].cfg.gop),          CFG_DATA_TYPE_U8,  "30",   "rw", 1, 200,  NULL},
            {"quality",      &(g_stRunVencCfg.vencStream[5][2].cfg.quality),      CFG_DATA_TYPE_U8,  "3",    "rw", 0, 3,  "0: poor, 3: best"},
            {"brcMode",      &(g_stRunVencCfg.vencStream[5][2].cfg.brcMode),      CFG_DATA_TYPE_U8,  "0",    "rw", 0, 3,  "0: CBR; 1: VBR; 2: FIXQP; 3: AVBR"},
            {"cbrAvgBps",    &(g_stRunVencCfg.vencStream[5][2].cfg.cbrAvgBps),    CFG_DATA_TYPE_U32, "500", "rw", 128, 16000,   NULL},
            {"vbrMinBps",    &(g_stRunVencCfg.vencStream[5][2].cfg.vbrMinBps),    CFG_DATA_TYPE_U32, "300", "rw", 128, 16000,   NULL},
            {"vbrMaxBps",    &(g_stRunVencCfg.vencStream[5][2].cfg.vbrMaxBps),    CFG_DATA_TYPE_U32, "500", "rw", 128, 16000,   NULL},
            {"qpMinI",       &(g_stRunVencCfg.vencStream[5][2].cfg.qpMinI),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMinP",       &(g_stRunVencCfg.vencStream[5][2].cfg.qpMinP),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMaxI",       &(g_stRunVencCfg.vencStream[5][2].cfg.qpMaxI),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
            {"qpMaxP",       &(g_stRunVencCfg.vencStream[5][2].cfg.qpMaxP),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
    		{"qpIWeight",    &(g_stRunVencCfg.vencStream[5][2].cfg.qpIWeight),    CFG_DATA_TYPE_U8,  "3",    "rw", 0, 100,  NULL},
            {"qpPWeight",    &(g_stRunVencCfg.vencStream[5][2].cfg.qpPWeight),    CFG_DATA_TYPE_U8,  "5",    "rw", 0, 100,  NULL},
    		{"adaptQp",      &(g_stRunVencCfg.vencStream[5][2].cfg.adaptQp),      CFG_DATA_TYPE_U8,  "2",    "rw", 0, 100,  NULL},
    		{"statTime",     &(g_stRunVencCfg.vencStream[5][2].cfg.statTime),      CFG_DATA_TYPE_U8,  "30",  "rw", 1, 60,  NULL},
        },
#ifdef THERMAL_ENABLE
        //stream 3
        {
            {"enable",       &(g_stRunVencCfg.vencStream[5][3].enable),           CFG_DATA_TYPE_S32, "0",    "rw", 0, 1, NULL},
            {"id",           &(g_stRunVencCfg.vencStream[5][3].id),               CFG_DATA_TYPE_S16, "3",    "rw", 0, 3, "0-3"},
            {"vi_band_id",   &(g_stRunVencCfg.vencStream[5][3].vi_band_id),    CFG_DATA_TYPE_S16, "3",    "rw", 0, 3, NULL},
            {"width",   &(g_stRunVencCfg.vencStream[5][3].cfg.width),    CFG_DATA_TYPE_U16, "320", "rw", 0, 5000, NULL},
            {"height",  &(g_stRunVencCfg.vencStream[5][3].cfg.height),   CFG_DATA_TYPE_U16, "240",  "rw", 0, 5000, NULL},
            {"roi_width",   &(g_stRunVencCfg.vencStream[5][3].cfg.roi_width),    CFG_DATA_TYPE_U16, "320", "rw", 0, 5000, NULL},
            {"roi_height",  &(g_stRunVencCfg.vencStream[5][3].cfg.roi_height),   CFG_DATA_TYPE_U16, "240",  "rw", 0, 5000, NULL},
            {"roi_x",       &(g_stRunVencCfg.vencStream[5][3].cfg.roi_x),        CFG_DATA_TYPE_U16, "0",    "rw", 0, 5000, NULL},
            {"roi_y",       &(g_stRunVencCfg.vencStream[5][3].cfg.roi_y),        CFG_DATA_TYPE_U16, "0",    "rw", 0,  5000, NULL},
            {"encodeType",   &(g_stRunVencCfg.vencStream[5][3].cfg.encodeType),   CFG_DATA_TYPE_U8,  "2",    "rw", 0, 5, "0: none, 1: H.264, 2: H.265, 3: MJPEG, 4: jpeg"},
            {"chromaFormat", &(g_stRunVencCfg.vencStream[5][3].cfg.chromaFormat), CFG_DATA_TYPE_U8,  "1",    "rw", 0, 1,  "0: YUV 422, 1: YUV 420."},
            {"fps",          &(g_stRunVencCfg.vencStream[5][3].cfg.fps),          CFG_DATA_TYPE_U8,  "25",   "rw", 1, 60,   NULL},
            {"gop",          &(g_stRunVencCfg.vencStream[5][3].cfg.gop),          CFG_DATA_TYPE_U8,  "25",   "rw", 1, 200,  NULL},
            {"quality",      &(g_stRunVencCfg.vencStream[5][3].cfg.quality),      CFG_DATA_TYPE_U8,  "3",    "rw", 0, 3,  "0: poor, 3: best"},
            {"brcMode",      &(g_stRunVencCfg.vencStream[5][3].cfg.brcMode),      CFG_DATA_TYPE_U8,  "0",    "rw", 0, 3,  "0: CBR; 1: VBR; 2: FIXQP; 3: AVBR"},
            {"cbrAvgBps",    &(g_stRunVencCfg.vencStream[5][3].cfg.cbrAvgBps),    CFG_DATA_TYPE_U32, "500", "rw", 0, 32000,   NULL},
            {"vbrMinBps",    &(g_stRunVencCfg.vencStream[5][3].cfg.vbrMinBps),    CFG_DATA_TYPE_U32, "300", "rw", 0, 32000,   NULL},
            {"vbrMaxBps",    &(g_stRunVencCfg.vencStream[5][3].cfg.vbrMaxBps),    CFG_DATA_TYPE_U32, "500", "rw", 0, 32000,   NULL},
            {"qpMinI",       &(g_stRunVencCfg.vencStream[5][3].cfg.qpMinI),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMinP",       &(g_stRunVencCfg.vencStream[5][3].cfg.qpMinP),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMaxI",       &(g_stRunVencCfg.vencStream[5][3].cfg.qpMaxI),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
            {"qpMaxP",       &(g_stRunVencCfg.vencStream[5][3].cfg.qpMaxP),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
            {"qpIWeight",    &(g_stRunVencCfg.vencStream[5][3].cfg.qpIWeight),    CFG_DATA_TYPE_U8,  "3",    "rw", 0, 100,  NULL},
            {"qpPWeight",    &(g_stRunVencCfg.vencStream[5][3].cfg.qpPWeight),    CFG_DATA_TYPE_U8,  "5",    "rw", 0, 100,  NULL},
            {"adaptQp",      &(g_stRunVencCfg.vencStream[5][3].cfg.adaptQp),      CFG_DATA_TYPE_U8,  "2",    "rw", 0, 100,  NULL},
            {"statTime",     &(g_stRunVencCfg.vencStream[5][3].cfg.statTime),      CFG_DATA_TYPE_U8,  "30",  "rw", 1, 60,  NULL},
        }
#endif
    },
	//channel 6
    {
        //stream 0
        {
            {"enable",       &(g_stRunVencCfg.vencStream[6][0].enable),           CFG_DATA_TYPE_S32, "0",    "rw", 0, 1, NULL},
            {"id",           &(g_stRunVencCfg.vencStream[6][0].id),               CFG_DATA_TYPE_S16, "0",    "rw", 0, 3, "0-3"},
            {"vi_band_id",   &(g_stRunVencCfg.vencStream[6][0].vi_band_id),    CFG_DATA_TYPE_S16, "0",    "rw", 0, 3, NULL},
            {"width",   &(g_stRunVencCfg.vencStream[6][0].cfg.width),    CFG_DATA_TYPE_U16, "1920", "rw", 0, 5000, NULL},
            {"height",  &(g_stRunVencCfg.vencStream[6][0].cfg.height),   CFG_DATA_TYPE_U16, "1080",  "rw", 0, 5000, NULL},
            {"roi_width",   &(g_stRunVencCfg.vencStream[6][0].cfg.roi_width),    CFG_DATA_TYPE_U16, "1920", "rw", 0, 5000, NULL},
            {"roi_height",  &(g_stRunVencCfg.vencStream[6][0].cfg.roi_height),   CFG_DATA_TYPE_U16, "1080",  "rw", 0, 5000, NULL},
            {"roi_x",       &(g_stRunVencCfg.vencStream[6][0].cfg.roi_x),        CFG_DATA_TYPE_U16, "0",    "rw", 0, 5000, NULL},
            {"roi_y",       &(g_stRunVencCfg.vencStream[6][0].cfg.roi_y),        CFG_DATA_TYPE_U16, "0",    "rw", 0,  5000, NULL},
            {"encodeType",   &(g_stRunVencCfg.vencStream[6][0].cfg.encodeType),   CFG_DATA_TYPE_U8,  "2",    "rw", 0, 5, "0: none, 1: H.264, 2: H.265, 3: MJPEG, 4: jpeg"},
            {"chromaFormat", &(g_stRunVencCfg.vencStream[6][0].cfg.chromaFormat), CFG_DATA_TYPE_U8,  "1",    "rw", 0, 1,  "0: YUV 422, 1: YUV 420."},
            {"fps",          &(g_stRunVencCfg.vencStream[6][0].cfg.fps),          CFG_DATA_TYPE_U8,  "25",   "rw", 1, 60,   NULL},
            {"gop",          &(g_stRunVencCfg.vencStream[6][0].cfg.gop),          CFG_DATA_TYPE_U8,  "30",   "rw", 1, 200,  NULL},
            {"quality",      &(g_stRunVencCfg.vencStream[6][0].cfg.quality),      CFG_DATA_TYPE_U8,  "3",    "rw", 0, 3,  "0: poor, 3: best"},
            {"brcMode",      &(g_stRunVencCfg.vencStream[6][0].cfg.brcMode),      CFG_DATA_TYPE_U8,  "0",    "rw", 0, 6,  "0: CBR; 1: VBR; 2: CVBR, 3: AVBR, 4: QPMAP, 5: FIXQP, 6 QVBR"},
            {"cbrAvgBps",    &(g_stRunVencCfg.vencStream[6][0].cfg.cbrAvgBps),    CFG_DATA_TYPE_U32, "2000", "rw", 128, 16000,   NULL},
            {"vbrMinBps",    &(g_stRunVencCfg.vencStream[6][0].cfg.vbrMinBps),    CFG_DATA_TYPE_U32, "750", "rw", 128, 16000,   NULL},
            {"vbrMaxBps",    &(g_stRunVencCfg.vencStream[6][0].cfg.vbrMaxBps),    CFG_DATA_TYPE_U32, "3000", "rw", 128, 16000,   NULL},
            {"qpMinI",       &(g_stRunVencCfg.vencStream[6][0].cfg.qpMinI),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMinP",       &(g_stRunVencCfg.vencStream[6][0].cfg.qpMinP),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMaxI",       &(g_stRunVencCfg.vencStream[6][0].cfg.qpMaxI),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
            {"qpMaxP",       &(g_stRunVencCfg.vencStream[6][0].cfg.qpMaxP),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
            {"qpIWeight",    &(g_stRunVencCfg.vencStream[6][0].cfg.qpIWeight),    CFG_DATA_TYPE_U8,  "3",    "rw", 0, 100,  NULL},
            {"qpPWeight",    &(g_stRunVencCfg.vencStream[6][0].cfg.qpPWeight),    CFG_DATA_TYPE_U8,  "5",    "rw", 0, 100,  NULL},
            {"adaptQp",      &(g_stRunVencCfg.vencStream[6][0].cfg.adaptQp),      CFG_DATA_TYPE_U8,  "2",    "rw", 0, 100,  NULL},
            {"statTime",     &(g_stRunVencCfg.vencStream[6][0].cfg.statTime),      CFG_DATA_TYPE_U8,  "30",  "rw", 1, 60,  NULL},
        },
        //stream 1
        {
            {"enable",       &(g_stRunVencCfg.vencStream[6][1].enable),           CFG_DATA_TYPE_S32, "0",    "rw", 0, 1, NULL},
            {"id",           &(g_stRunVencCfg.vencStream[6][1].id),               CFG_DATA_TYPE_S16, "1",    "rw", 0, 3, "0-3"},
            {"vi_band_id",   &(g_stRunVencCfg.vencStream[6][1].vi_band_id),    CFG_DATA_TYPE_S16, "1",    "rw", 0, 3, NULL},
            {"width",   &(g_stRunVencCfg.vencStream[6][1].cfg.width),    CFG_DATA_TYPE_U16, "640", "rw", 0, 5000, NULL},
            {"height",  &(g_stRunVencCfg.vencStream[6][1].cfg.height),   CFG_DATA_TYPE_U16, "480",  "rw", 0, 5000, NULL},
            {"roi_width",   &(g_stRunVencCfg.vencStream[6][1].cfg.roi_width),    CFG_DATA_TYPE_U16, "640", "rw", 0, 5000, NULL},
            {"roi_height",  &(g_stRunVencCfg.vencStream[6][1].cfg.roi_height),   CFG_DATA_TYPE_U16, "480",  "rw", 0, 5000, NULL},
            {"roi_x",       &(g_stRunVencCfg.vencStream[6][1].cfg.roi_x),        CFG_DATA_TYPE_U16, "0",    "rw", 0, 5000, NULL},
            {"roi_y",       &(g_stRunVencCfg.vencStream[6][1].cfg.roi_y),        CFG_DATA_TYPE_U16, "0",    "rw", 0,  5000, NULL},
            {"encodeType",   &(g_stRunVencCfg.vencStream[6][1].cfg.encodeType),   CFG_DATA_TYPE_U8,  "2",    "rw", 0, 5, "0: none, 1: H.264, 2: H.265, 3: MJPEG, 4: jpeg"},
            {"chromaFormat", &(g_stRunVencCfg.vencStream[6][1].cfg.chromaFormat), CFG_DATA_TYPE_U8,  "1",    "rw", 0, 1,  "0: YUV 422, 1: YUV 420."},
            {"fps",          &(g_stRunVencCfg.vencStream[6][1].cfg.fps),          CFG_DATA_TYPE_U8,  "25",   "rw", 1, 60,   NULL},
            {"gop",          &(g_stRunVencCfg.vencStream[6][1].cfg.gop),          CFG_DATA_TYPE_U8,  "30",   "rw", 1, 200,  NULL},
            {"quality",      &(g_stRunVencCfg.vencStream[6][1].cfg.quality),      CFG_DATA_TYPE_U8,  "3",    "rw", 0, 3,  "0: poor, 3: best"},
            {"brcMode",      &(g_stRunVencCfg.vencStream[6][1].cfg.brcMode),      CFG_DATA_TYPE_U8,  "0",    "rw", 0, 6,  "0: CBR; 1: VBR; 2: CVBR, 3: AVBR, 4: QPMAP, 5: FIXQP, 6 QVBR"},
            {"cbrAvgBps",    &(g_stRunVencCfg.vencStream[6][1].cfg.cbrAvgBps),    CFG_DATA_TYPE_U32, "1000", "rw", 128, 16000,   NULL},
            {"vbrMinBps",    &(g_stRunVencCfg.vencStream[6][1].cfg.vbrMinBps),    CFG_DATA_TYPE_U32, "400", "rw", 128, 16000,   NULL},
            {"vbrMaxBps",    &(g_stRunVencCfg.vencStream[6][1].cfg.vbrMaxBps),    CFG_DATA_TYPE_U32, "1200", "rw", 128, 16000,   NULL},
            {"qpMinI",       &(g_stRunVencCfg.vencStream[6][1].cfg.qpMinI),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMinP",       &(g_stRunVencCfg.vencStream[6][1].cfg.qpMinP),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMaxI",       &(g_stRunVencCfg.vencStream[6][1].cfg.qpMaxI),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
            {"qpMaxP",       &(g_stRunVencCfg.vencStream[6][1].cfg.qpMaxP),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
            {"qpIWeight",    &(g_stRunVencCfg.vencStream[6][1].cfg.qpIWeight),    CFG_DATA_TYPE_U8,  "3",    "rw", 0, 100,  NULL},
            {"qpPWeight",    &(g_stRunVencCfg.vencStream[6][1].cfg.qpPWeight),    CFG_DATA_TYPE_U8,  "5",    "rw", 0, 100,  NULL},
            {"adaptQp",      &(g_stRunVencCfg.vencStream[6][1].cfg.adaptQp),      CFG_DATA_TYPE_U8,  "2",    "rw", 0, 100,  NULL},
            {"statTime",     &(g_stRunVencCfg.vencStream[6][1].cfg.statTime),      CFG_DATA_TYPE_U8,  "30",  "rw", 1, 60,  NULL},
        },
        //stream 2
        {
            {"enable",       &(g_stRunVencCfg.vencStream[6][2].enable),           CFG_DATA_TYPE_S32, "0",    "rw", 0, 1, NULL},
            {"id",           &(g_stRunVencCfg.vencStream[6][2].id),               CFG_DATA_TYPE_S16, "2",    "rw", 0, 3, "0-3"},
            {"vi_band_id",   &(g_stRunVencCfg.vencStream[6][2].vi_band_id),    CFG_DATA_TYPE_S16, "2",    "rw", 0, 3, NULL},
            {"width",   &(g_stRunVencCfg.vencStream[6][2].cfg.width),    CFG_DATA_TYPE_U16, "320", "rw", 0, 5000, NULL},
            {"height",  &(g_stRunVencCfg.vencStream[6][2].cfg.height),   CFG_DATA_TYPE_U16, "240",  "rw", 0, 5000, NULL},
            {"roi_width",   &(g_stRunVencCfg.vencStream[6][2].cfg.roi_width),    CFG_DATA_TYPE_U16, "320", "rw", 0, 5000, NULL},
            {"roi_height",  &(g_stRunVencCfg.vencStream[6][2].cfg.roi_height),   CFG_DATA_TYPE_U16, "240",  "rw", 0, 5000, NULL},
            {"roi_x",       &(g_stRunVencCfg.vencStream[6][2].cfg.roi_x),        CFG_DATA_TYPE_U16, "0",    "rw", 0, 5000, NULL},
            {"roi_y",       &(g_stRunVencCfg.vencStream[6][2].cfg.roi_y),        CFG_DATA_TYPE_U16, "0",    "rw", 0,  5000, NULL},
            {"encodeType",   &(g_stRunVencCfg.vencStream[6][2].cfg.encodeType),   CFG_DATA_TYPE_U8,  "2",    "rw", 0, 5, "0: none, 1: H.264, 2: H.265, 3: MJPEG, 4: jpeg"},
            {"chromaFormat", &(g_stRunVencCfg.vencStream[6][2].cfg.chromaFormat), CFG_DATA_TYPE_U8,  "1",    "rw", 0, 1,  "0: YUV 422, 1: YUV 420."},
            {"fps",          &(g_stRunVencCfg.vencStream[6][2].cfg.fps),          CFG_DATA_TYPE_U8,  "25",   "rw", 1, 60,   NULL},
            {"gop",          &(g_stRunVencCfg.vencStream[6][2].cfg.gop),          CFG_DATA_TYPE_U8,  "30",   "rw", 1, 200,  NULL},
            {"quality",      &(g_stRunVencCfg.vencStream[6][2].cfg.quality),      CFG_DATA_TYPE_U8,  "3",    "rw", 0, 3,  "0: poor, 3: best"},
            {"brcMode",      &(g_stRunVencCfg.vencStream[6][2].cfg.brcMode),      CFG_DATA_TYPE_U8,  "0",    "rw", 0, 3,  "0: CBR; 1: VBR; 2: FIXQP; 3: AVBR"},
            {"cbrAvgBps",    &(g_stRunVencCfg.vencStream[6][2].cfg.cbrAvgBps),    CFG_DATA_TYPE_U32, "500", "rw", 128, 16000,   NULL},
            {"vbrMinBps",    &(g_stRunVencCfg.vencStream[6][2].cfg.vbrMinBps),    CFG_DATA_TYPE_U32, "300", "rw", 128, 16000,   NULL},
            {"vbrMaxBps",    &(g_stRunVencCfg.vencStream[6][2].cfg.vbrMaxBps),    CFG_DATA_TYPE_U32, "500", "rw", 128, 16000,   NULL},
            {"qpMinI",       &(g_stRunVencCfg.vencStream[6][2].cfg.qpMinI),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMinP",       &(g_stRunVencCfg.vencStream[6][2].cfg.qpMinP),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMaxI",       &(g_stRunVencCfg.vencStream[6][2].cfg.qpMaxI),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
            {"qpMaxP",       &(g_stRunVencCfg.vencStream[6][2].cfg.qpMaxP),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
            {"qpIWeight",    &(g_stRunVencCfg.vencStream[6][2].cfg.qpIWeight),    CFG_DATA_TYPE_U8,  "3",    "rw", 0, 100,  NULL},
            {"qpPWeight",    &(g_stRunVencCfg.vencStream[6][2].cfg.qpPWeight),    CFG_DATA_TYPE_U8,  "5",    "rw", 0, 100,  NULL},
            {"adaptQp",      &(g_stRunVencCfg.vencStream[6][2].cfg.adaptQp),      CFG_DATA_TYPE_U8,  "2",    "rw", 0, 100,  NULL},
            {"statTime",     &(g_stRunVencCfg.vencStream[6][2].cfg.statTime),      CFG_DATA_TYPE_U8,  "30",  "rw", 1, 60,  NULL},
        },
#ifdef THERMAL_ENABLE
        //stream 3
        {
            {"enable",       &(g_stRunVencCfg.vencStream[6][3].enable),           CFG_DATA_TYPE_S32, "0",    "rw", 0, 1, NULL},
            {"id",           &(g_stRunVencCfg.vencStream[6][3].id),               CFG_DATA_TYPE_S16, "3",    "rw", 0, 3, "0-3"},
            {"vi_band_id",   &(g_stRunVencCfg.vencStream[6][3].vi_band_id),    CFG_DATA_TYPE_S16, "3",    "rw", 0, 3, NULL},
            {"width",   &(g_stRunVencCfg.vencStream[6][3].cfg.width),    CFG_DATA_TYPE_U16, "320", "rw", 0, 5000, NULL},
            {"height",  &(g_stRunVencCfg.vencStream[6][3].cfg.height),   CFG_DATA_TYPE_U16, "240",  "rw", 0, 5000, NULL},
            {"roi_width",   &(g_stRunVencCfg.vencStream[6][3].cfg.roi_width),    CFG_DATA_TYPE_U16, "320", "rw", 0, 5000, NULL},
            {"roi_height",  &(g_stRunVencCfg.vencStream[6][3].cfg.roi_height),   CFG_DATA_TYPE_U16, "240",  "rw", 0, 5000, NULL},
            {"roi_x",       &(g_stRunVencCfg.vencStream[6][3].cfg.roi_x),        CFG_DATA_TYPE_U16, "0",    "rw", 0, 5000, NULL},
            {"roi_y",       &(g_stRunVencCfg.vencStream[6][3].cfg.roi_y),        CFG_DATA_TYPE_U16, "0",    "rw", 0,  5000, NULL},
            {"encodeType",   &(g_stRunVencCfg.vencStream[6][3].cfg.encodeType),   CFG_DATA_TYPE_U8,  "2",    "rw", 0, 5, "0: none, 1: H.264, 2: H.265, 3: MJPEG, 4: jpeg"},
            {"chromaFormat", &(g_stRunVencCfg.vencStream[6][3].cfg.chromaFormat), CFG_DATA_TYPE_U8,  "1",    "rw", 0, 1,  "0: YUV 422, 1: YUV 420."},
            {"fps",          &(g_stRunVencCfg.vencStream[6][3].cfg.fps),          CFG_DATA_TYPE_U8,  "25",   "rw", 1, 60,   NULL},
            {"gop",          &(g_stRunVencCfg.vencStream[6][3].cfg.gop),          CFG_DATA_TYPE_U8,  "25",   "rw", 1, 200,  NULL},
            {"quality",      &(g_stRunVencCfg.vencStream[6][3].cfg.quality),      CFG_DATA_TYPE_U8,  "3",    "rw", 0, 3,  "0: poor, 3: best"},
            {"brcMode",      &(g_stRunVencCfg.vencStream[6][3].cfg.brcMode),      CFG_DATA_TYPE_U8,  "0",    "rw", 0, 3,  "0: CBR; 1: VBR; 2: FIXQP; 3: AVBR"},
            {"cbrAvgBps",    &(g_stRunVencCfg.vencStream[6][3].cfg.cbrAvgBps),    CFG_DATA_TYPE_U32, "500", "rw", 0, 32000,   NULL},
            {"vbrMinBps",    &(g_stRunVencCfg.vencStream[6][3].cfg.vbrMinBps),    CFG_DATA_TYPE_U32, "300", "rw", 0, 32000,   NULL},
            {"vbrMaxBps",    &(g_stRunVencCfg.vencStream[6][3].cfg.vbrMaxBps),    CFG_DATA_TYPE_U32, "500", "rw", 0, 32000,   NULL},
            {"qpMinI",       &(g_stRunVencCfg.vencStream[6][3].cfg.qpMinI),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMinP",       &(g_stRunVencCfg.vencStream[6][3].cfg.qpMinP),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMaxI",       &(g_stRunVencCfg.vencStream[6][3].cfg.qpMaxI),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
            {"qpMaxP",       &(g_stRunVencCfg.vencStream[6][3].cfg.qpMaxP),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
            {"qpIWeight",    &(g_stRunVencCfg.vencStream[6][3].cfg.qpIWeight),    CFG_DATA_TYPE_U8,  "3",    "rw", 0, 100,  NULL},
            {"qpPWeight",    &(g_stRunVencCfg.vencStream[6][3].cfg.qpPWeight),    CFG_DATA_TYPE_U8,  "5",    "rw", 0, 100,  NULL},
            {"adaptQp",      &(g_stRunVencCfg.vencStream[6][3].cfg.adaptQp),      CFG_DATA_TYPE_U8,  "2",    "rw", 0, 100,  NULL},
            {"statTime",     &(g_stRunVencCfg.vencStream[6][3].cfg.statTime),      CFG_DATA_TYPE_U8,  "30",  "rw", 1, 60,  NULL},
        }
#endif
    },
	//channel 7
    {
        //stream 0
        {
            {"enable",       &(g_stRunVencCfg.vencStream[7][0].enable),           CFG_DATA_TYPE_S32, "0",    "rw", 0, 1, NULL},
            {"id",           &(g_stRunVencCfg.vencStream[7][0].id),               CFG_DATA_TYPE_S16, "0",    "rw", 0, 3, "0-3"},
            {"vi_band_id",   &(g_stRunVencCfg.vencStream[7][0].vi_band_id),    CFG_DATA_TYPE_S16, "0",    "rw", 0, 3, NULL},
            {"width",   &(g_stRunVencCfg.vencStream[7][0].cfg.width),    CFG_DATA_TYPE_U16, "1920", "rw", 0, 5000, NULL},
            {"height",  &(g_stRunVencCfg.vencStream[7][0].cfg.height),   CFG_DATA_TYPE_U16, "1080",  "rw", 0, 5000, NULL},
            {"roi_width",   &(g_stRunVencCfg.vencStream[7][0].cfg.roi_width),    CFG_DATA_TYPE_U16, "1920", "rw", 0, 5000, NULL},
            {"roi_height",  &(g_stRunVencCfg.vencStream[7][0].cfg.roi_height),   CFG_DATA_TYPE_U16, "1080",  "rw", 0, 5000, NULL},
            {"roi_x",       &(g_stRunVencCfg.vencStream[7][0].cfg.roi_x),        CFG_DATA_TYPE_U16, "0",    "rw", 0, 5000, NULL},
            {"roi_y",       &(g_stRunVencCfg.vencStream[7][0].cfg.roi_y),        CFG_DATA_TYPE_U16, "0",    "rw", 0,  5000, NULL},
            {"encodeType",   &(g_stRunVencCfg.vencStream[7][0].cfg.encodeType),   CFG_DATA_TYPE_U8,  "2",    "rw", 0, 5, "0: none, 1: H.264, 2: H.265, 3: MJPEG, 4: jpeg"},
            {"chromaFormat", &(g_stRunVencCfg.vencStream[7][0].cfg.chromaFormat), CFG_DATA_TYPE_U8,  "1",    "rw", 0, 1,  "0: YUV 422, 1: YUV 420."},
            {"fps",          &(g_stRunVencCfg.vencStream[7][0].cfg.fps),          CFG_DATA_TYPE_U8,  "25",   "rw", 1, 60,   NULL},
            {"gop",          &(g_stRunVencCfg.vencStream[7][0].cfg.gop),          CFG_DATA_TYPE_U8,  "30",   "rw", 1, 200,  NULL},
            {"quality",      &(g_stRunVencCfg.vencStream[7][0].cfg.quality),      CFG_DATA_TYPE_U8,  "3",    "rw", 0, 3,  "0: poor, 3: best"},
            {"brcMode",      &(g_stRunVencCfg.vencStream[7][0].cfg.brcMode),      CFG_DATA_TYPE_U8,  "0",    "rw", 0, 6,  "0: CBR; 1: VBR; 2: CVBR, 3: AVBR, 4: QPMAP, 5: FIXQP, 6 QVBR"},
            {"cbrAvgBps",    &(g_stRunVencCfg.vencStream[7][0].cfg.cbrAvgBps),    CFG_DATA_TYPE_U32, "2000", "rw", 128, 16000,   NULL},
            {"vbrMinBps",    &(g_stRunVencCfg.vencStream[7][0].cfg.vbrMinBps),    CFG_DATA_TYPE_U32, "750", "rw", 128, 16000,   NULL},
            {"vbrMaxBps",    &(g_stRunVencCfg.vencStream[7][0].cfg.vbrMaxBps),    CFG_DATA_TYPE_U32, "3000", "rw", 128, 16000,   NULL},
            {"qpMinI",       &(g_stRunVencCfg.vencStream[7][0].cfg.qpMinI),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMinP",       &(g_stRunVencCfg.vencStream[7][0].cfg.qpMinP),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMaxI",       &(g_stRunVencCfg.vencStream[7][0].cfg.qpMaxI),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
            {"qpMaxP",       &(g_stRunVencCfg.vencStream[7][0].cfg.qpMaxP),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
            {"qpIWeight",    &(g_stRunVencCfg.vencStream[7][0].cfg.qpIWeight),    CFG_DATA_TYPE_U8,  "3",    "rw", 0, 100,  NULL},
            {"qpPWeight",    &(g_stRunVencCfg.vencStream[7][0].cfg.qpPWeight),    CFG_DATA_TYPE_U8,  "5",    "rw", 0, 100,  NULL},
            {"adaptQp",      &(g_stRunVencCfg.vencStream[7][0].cfg.adaptQp),      CFG_DATA_TYPE_U8,  "2",    "rw", 0, 100,  NULL},
            {"statTime",     &(g_stRunVencCfg.vencStream[7][0].cfg.statTime),      CFG_DATA_TYPE_U8,  "30",  "rw", 1, 60,  NULL},
        },
        //stream 1
        {
            {"enable",       &(g_stRunVencCfg.vencStream[7][1].enable),           CFG_DATA_TYPE_S32, "0",    "rw", 0, 1, NULL},
            {"id",           &(g_stRunVencCfg.vencStream[7][1].id),               CFG_DATA_TYPE_S16, "1",    "rw", 0, 3, "0-3"},
            {"vi_band_id",   &(g_stRunVencCfg.vencStream[7][1].vi_band_id),    CFG_DATA_TYPE_S16, "1",    "rw", 0, 3, NULL},
            {"width",   &(g_stRunVencCfg.vencStream[7][1].cfg.width),    CFG_DATA_TYPE_U16, "640", "rw", 0, 5000, NULL},
            {"height",  &(g_stRunVencCfg.vencStream[7][1].cfg.height),   CFG_DATA_TYPE_U16, "480",  "rw", 0, 5000, NULL},
            {"roi_width",   &(g_stRunVencCfg.vencStream[7][1].cfg.roi_width),    CFG_DATA_TYPE_U16, "640", "rw", 0, 5000, NULL},
            {"roi_height",  &(g_stRunVencCfg.vencStream[7][1].cfg.roi_height),   CFG_DATA_TYPE_U16, "480",  "rw", 0, 5000, NULL},
            {"roi_x",       &(g_stRunVencCfg.vencStream[7][1].cfg.roi_x),        CFG_DATA_TYPE_U16, "0",    "rw", 0, 5000, NULL},
            {"roi_y",       &(g_stRunVencCfg.vencStream[7][1].cfg.roi_y),        CFG_DATA_TYPE_U16, "0",    "rw", 0,  5000, NULL},
            {"encodeType",   &(g_stRunVencCfg.vencStream[7][1].cfg.encodeType),   CFG_DATA_TYPE_U8,  "2",    "rw", 0, 5, "0: none, 1: H.264, 2: H.265, 3: MJPEG, 4: jpeg"},
            {"chromaFormat", &(g_stRunVencCfg.vencStream[7][1].cfg.chromaFormat), CFG_DATA_TYPE_U8,  "1",    "rw", 0, 1,  "0: YUV 422, 1: YUV 420."},
            {"fps",          &(g_stRunVencCfg.vencStream[7][1].cfg.fps),          CFG_DATA_TYPE_U8,  "25",   "rw", 1, 60,   NULL},
            {"gop",          &(g_stRunVencCfg.vencStream[7][1].cfg.gop),          CFG_DATA_TYPE_U8,  "30",   "rw", 1, 200,  NULL},
            {"quality",      &(g_stRunVencCfg.vencStream[7][1].cfg.quality),      CFG_DATA_TYPE_U8,  "3",    "rw", 0, 3,  "0: poor, 3: best"},
            {"brcMode",      &(g_stRunVencCfg.vencStream[7][1].cfg.brcMode),      CFG_DATA_TYPE_U8,  "0",    "rw", 0, 6,  "0: CBR; 1: VBR; 2: CVBR, 3: AVBR, 4: QPMAP, 5: FIXQP, 6 QVBR"},
            {"cbrAvgBps",    &(g_stRunVencCfg.vencStream[7][1].cfg.cbrAvgBps),    CFG_DATA_TYPE_U32, "1000", "rw", 128, 16000,   NULL},
            {"vbrMinBps",    &(g_stRunVencCfg.vencStream[7][1].cfg.vbrMinBps),    CFG_DATA_TYPE_U32, "400", "rw", 128, 16000,   NULL},
            {"vbrMaxBps",    &(g_stRunVencCfg.vencStream[7][1].cfg.vbrMaxBps),    CFG_DATA_TYPE_U32, "1200", "rw", 128, 16000,   NULL},
            {"qpMinI",       &(g_stRunVencCfg.vencStream[7][1].cfg.qpMinI),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMinP",       &(g_stRunVencCfg.vencStream[7][1].cfg.qpMinP),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMaxI",       &(g_stRunVencCfg.vencStream[7][1].cfg.qpMaxI),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
            {"qpMaxP",       &(g_stRunVencCfg.vencStream[7][1].cfg.qpMaxP),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
            {"qpIWeight",    &(g_stRunVencCfg.vencStream[7][1].cfg.qpIWeight),    CFG_DATA_TYPE_U8,  "3",    "rw", 0, 100,  NULL},
            {"qpPWeight",    &(g_stRunVencCfg.vencStream[7][1].cfg.qpPWeight),    CFG_DATA_TYPE_U8,  "5",    "rw", 0, 100,  NULL},
            {"adaptQp",      &(g_stRunVencCfg.vencStream[7][1].cfg.adaptQp),      CFG_DATA_TYPE_U8,  "2",    "rw", 0, 100,  NULL},
            {"statTime",     &(g_stRunVencCfg.vencStream[7][1].cfg.statTime),      CFG_DATA_TYPE_U8,  "30",  "rw", 1, 60,  NULL},
        },
        //stream 2
        {
            {"enable",       &(g_stRunVencCfg.vencStream[7][2].enable),           CFG_DATA_TYPE_S32, "0",    "rw", 0, 1, NULL},
            {"id",           &(g_stRunVencCfg.vencStream[7][2].id),               CFG_DATA_TYPE_S16, "2",    "rw", 0, 3, "0-3"},
            {"vi_band_id",   &(g_stRunVencCfg.vencStream[7][2].vi_band_id),    CFG_DATA_TYPE_S16, "2",    "rw", 0, 3, NULL},
            {"width",   &(g_stRunVencCfg.vencStream[7][2].cfg.width),    CFG_DATA_TYPE_U16, "320", "rw", 0, 5000, NULL},
            {"height",  &(g_stRunVencCfg.vencStream[7][2].cfg.height),   CFG_DATA_TYPE_U16, "240",  "rw", 0, 5000, NULL},
            {"roi_width",   &(g_stRunVencCfg.vencStream[7][2].cfg.roi_width),    CFG_DATA_TYPE_U16, "320", "rw", 0, 5000, NULL},
            {"roi_height",  &(g_stRunVencCfg.vencStream[7][2].cfg.roi_height),   CFG_DATA_TYPE_U16, "240",  "rw", 0, 5000, NULL},
            {"roi_x",       &(g_stRunVencCfg.vencStream[7][2].cfg.roi_x),        CFG_DATA_TYPE_U16, "0",    "rw", 0, 5000, NULL},
            {"roi_y",       &(g_stRunVencCfg.vencStream[7][2].cfg.roi_y),        CFG_DATA_TYPE_U16, "0",    "rw", 0,  5000, NULL},
            {"encodeType",   &(g_stRunVencCfg.vencStream[7][2].cfg.encodeType),   CFG_DATA_TYPE_U8,  "2",    "rw", 0, 5, "0: none, 1: H.264, 2: H.265, 3: MJPEG, 4: jpeg"},
            {"chromaFormat", &(g_stRunVencCfg.vencStream[7][2].cfg.chromaFormat), CFG_DATA_TYPE_U8,  "1",    "rw", 0, 1,  "0: YUV 422, 1: YUV 420."},
            {"fps",          &(g_stRunVencCfg.vencStream[7][2].cfg.fps),          CFG_DATA_TYPE_U8,  "25",   "rw", 1, 60,   NULL},
            {"gop",          &(g_stRunVencCfg.vencStream[7][2].cfg.gop),          CFG_DATA_TYPE_U8,  "30",   "rw", 1, 200,  NULL},
            {"quality",      &(g_stRunVencCfg.vencStream[7][2].cfg.quality),      CFG_DATA_TYPE_U8,  "3",    "rw", 0, 3,  "0: poor, 3: best"},
            {"brcMode",      &(g_stRunVencCfg.vencStream[7][2].cfg.brcMode),      CFG_DATA_TYPE_U8,  "0",    "rw", 0, 3,  "0: CBR; 1: VBR; 2: FIXQP; 3: AVBR"},
            {"cbrAvgBps",    &(g_stRunVencCfg.vencStream[7][2].cfg.cbrAvgBps),    CFG_DATA_TYPE_U32, "500", "rw", 128, 16000,   NULL},
            {"vbrMinBps",    &(g_stRunVencCfg.vencStream[7][2].cfg.vbrMinBps),    CFG_DATA_TYPE_U32, "300", "rw", 128, 16000,   NULL},
            {"vbrMaxBps",    &(g_stRunVencCfg.vencStream[7][2].cfg.vbrMaxBps),    CFG_DATA_TYPE_U32, "500", "rw", 128, 16000,   NULL},
            {"qpMinI",       &(g_stRunVencCfg.vencStream[7][2].cfg.qpMinI),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMinP",       &(g_stRunVencCfg.vencStream[7][2].cfg.qpMinP),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMaxI",       &(g_stRunVencCfg.vencStream[7][2].cfg.qpMaxI),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
            {"qpMaxP",       &(g_stRunVencCfg.vencStream[7][2].cfg.qpMaxP),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
            {"qpIWeight",    &(g_stRunVencCfg.vencStream[7][2].cfg.qpIWeight),    CFG_DATA_TYPE_U8,  "3",    "rw", 0, 100,  NULL},
            {"qpPWeight",    &(g_stRunVencCfg.vencStream[7][2].cfg.qpPWeight),    CFG_DATA_TYPE_U8,  "5",    "rw", 0, 100,  NULL},
            {"adaptQp",      &(g_stRunVencCfg.vencStream[7][2].cfg.adaptQp),      CFG_DATA_TYPE_U8,  "2",    "rw", 0, 100,  NULL},
            {"statTime",     &(g_stRunVencCfg.vencStream[7][2].cfg.statTime),      CFG_DATA_TYPE_U8,  "30",  "rw", 1, 60,  NULL},
        },
#ifdef THERMAL_ENABLE
        //stream 3
        {
            {"enable",       &(g_stRunVencCfg.vencStream[7][3].enable),           CFG_DATA_TYPE_S32, "0",    "rw", 0, 1, NULL},
            {"id",           &(g_stRunVencCfg.vencStream[7][3].id),               CFG_DATA_TYPE_S16, "3",    "rw", 0, 3, "0-3"},
            {"vi_band_id",   &(g_stRunVencCfg.vencStream[7][3].vi_band_id),    CFG_DATA_TYPE_S16, "3",    "rw", 0, 3, NULL},
            {"width",   &(g_stRunVencCfg.vencStream[7][3].cfg.width),    CFG_DATA_TYPE_U16, "320", "rw", 0, 5000, NULL},
            {"height",  &(g_stRunVencCfg.vencStream[7][3].cfg.height),   CFG_DATA_TYPE_U16, "240",  "rw", 0, 5000, NULL},
            {"roi_width",   &(g_stRunVencCfg.vencStream[7][3].cfg.roi_width),    CFG_DATA_TYPE_U16, "320", "rw", 0, 5000, NULL},
            {"roi_height",  &(g_stRunVencCfg.vencStream[7][3].cfg.roi_height),   CFG_DATA_TYPE_U16, "240",  "rw", 0, 5000, NULL},
            {"roi_x",       &(g_stRunVencCfg.vencStream[7][3].cfg.roi_x),        CFG_DATA_TYPE_U16, "0",    "rw", 0, 5000, NULL},
            {"roi_y",       &(g_stRunVencCfg.vencStream[7][3].cfg.roi_y),        CFG_DATA_TYPE_U16, "0",    "rw", 0,  5000, NULL},
            {"encodeType",   &(g_stRunVencCfg.vencStream[7][3].cfg.encodeType),   CFG_DATA_TYPE_U8,  "2",    "rw", 0, 5, "0: none, 1: H.264, 2: H.265, 3: MJPEG, 4: jpeg"},
            {"chromaFormat", &(g_stRunVencCfg.vencStream[7][3].cfg.chromaFormat), CFG_DATA_TYPE_U8,  "1",    "rw", 0, 1,  "0: YUV 422, 1: YUV 420."},
            {"fps",          &(g_stRunVencCfg.vencStream[7][3].cfg.fps),          CFG_DATA_TYPE_U8,  "25",   "rw", 1, 60,   NULL},
            {"gop",          &(g_stRunVencCfg.vencStream[7][3].cfg.gop),          CFG_DATA_TYPE_U8,  "25",   "rw", 1, 200,  NULL},
            {"quality",      &(g_stRunVencCfg.vencStream[7][3].cfg.quality),      CFG_DATA_TYPE_U8,  "3",    "rw", 0, 3,  "0: poor, 3: best"},
            {"brcMode",      &(g_stRunVencCfg.vencStream[7][3].cfg.brcMode),      CFG_DATA_TYPE_U8,  "0",    "rw", 0, 3,  "0: CBR; 1: VBR; 2: FIXQP; 3: AVBR"},
            {"cbrAvgBps",    &(g_stRunVencCfg.vencStream[7][3].cfg.cbrAvgBps),    CFG_DATA_TYPE_U32, "500", "rw", 0, 32000,   NULL},
            {"vbrMinBps",    &(g_stRunVencCfg.vencStream[7][3].cfg.vbrMinBps),    CFG_DATA_TYPE_U32, "300", "rw", 0, 32000,   NULL},
            {"vbrMaxBps",    &(g_stRunVencCfg.vencStream[7][3].cfg.vbrMaxBps),    CFG_DATA_TYPE_U32, "500", "rw", 0, 32000,   NULL},
            {"qpMinI",       &(g_stRunVencCfg.vencStream[7][3].cfg.qpMinI),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMinP",       &(g_stRunVencCfg.vencStream[7][3].cfg.qpMinP),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
            {"qpMaxI",       &(g_stRunVencCfg.vencStream[7][3].cfg.qpMaxI),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
            {"qpMaxP",       &(g_stRunVencCfg.vencStream[7][3].cfg.qpMaxP),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
            {"qpIWeight",    &(g_stRunVencCfg.vencStream[7][3].cfg.qpIWeight),    CFG_DATA_TYPE_U8,  "3",    "rw", 0, 100,  NULL},
            {"qpPWeight",    &(g_stRunVencCfg.vencStream[7][3].cfg.qpPWeight),    CFG_DATA_TYPE_U8,  "5",    "rw", 0, 100,  NULL},
            {"adaptQp",      &(g_stRunVencCfg.vencStream[7][3].cfg.adaptQp),      CFG_DATA_TYPE_U8,  "2",    "rw", 0, 100,  NULL},
            {"statTime",     &(g_stRunVencCfg.vencStream[7][3].cfg.statTime),      CFG_DATA_TYPE_U8,  "30",  "rw", 1, 60,  NULL},
        }
#endif
    }
};

CFG_MAP vencSnapShot[MAX_VENC_ITEM_NUM] =
{
	//stream snap
	{"enable",       &(g_stRunVencCfg.vencStreamSnap.enable),           CFG_DATA_TYPE_S32, "1",    "rw", 0, 1, NULL},
	{"id",           &(g_stRunVencCfg.vencStreamSnap.id),               CFG_DATA_TYPE_S16, "0",    "rw", 0, 3, "0-3"},
	{"vi_band_id",	 &(g_stRunVencCfg.vencStreamSnap.vi_band_id),	   CFG_DATA_TYPE_S16, "0",    "rw", 0, 3, NULL},
	{"width",   &(g_stRunVencCfg.vencStreamSnap.cfg.width),    CFG_DATA_TYPE_U16, "1920", "rw", 0, 5000, NULL},
	{"height",  &(g_stRunVencCfg.vencStreamSnap.cfg.height),   CFG_DATA_TYPE_U16, "1080",  "rw", 0, 5000, NULL},
	{"roi_width",   &(g_stRunVencCfg.vencStreamSnap.cfg.roi_width),    CFG_DATA_TYPE_U16, "1920", "rw", 0, 5000, NULL},
	{"roi_height",  &(g_stRunVencCfg.vencStreamSnap.cfg.roi_height),   CFG_DATA_TYPE_U16, "1080",  "rw", 0, 5000, NULL},
	{"roi_x",       &(g_stRunVencCfg.vencStreamSnap.cfg.roi_x),        CFG_DATA_TYPE_U16, "0",    "rw", 0, 5000, NULL},
	{"roi_y",       &(g_stRunVencCfg.vencStreamSnap.cfg.roi_y),        CFG_DATA_TYPE_U16, "0",    "rw", 0,  5000, NULL},
	{"encodeType",   &(g_stRunVencCfg.vencStreamSnap.cfg.encodeType),   CFG_DATA_TYPE_U8,  "4",    "rw", 0, 5, "0: none, 1: H.264, 2: H.265, 3: MJPEG, 4: jpeg"},
	{"chromaFormat", &(g_stRunVencCfg.vencStreamSnap.cfg.chromaFormat), CFG_DATA_TYPE_U8,  "1",    "rw", 0, 1,  "0: YUV 422, 1: YUV 420."},
	{"fps",          &(g_stRunVencCfg.vencStreamSnap.cfg.fps),          CFG_DATA_TYPE_U8,  "25",   "rw", 1, 60,   NULL},
	{"gop",          &(g_stRunVencCfg.vencStreamSnap.cfg.gop),          CFG_DATA_TYPE_U8,  "30",   "rw", 1, 200,  NULL},
	{"quality",      &(g_stRunVencCfg.vencStreamSnap.cfg.quality),      CFG_DATA_TYPE_U8,  "3",    "rw", 0, 3,  "0: poor, 3: best"},
	{"brcMode",      &(g_stRunVencCfg.vencStreamSnap.cfg.brcMode),      CFG_DATA_TYPE_U8,  "0",    "rw", 0, 6,  "0: CBR; 1: VBR; 2: CVBR, 3: AVBR, 4: QPMAP, 5: FIXQP, 6 QVBR"},
	{"cbrAvgBps",    &(g_stRunVencCfg.vencStreamSnap.cfg.cbrAvgBps),    CFG_DATA_TYPE_U32, "2000", "rw", 128, 16000,   NULL},
	{"vbrMinBps",    &(g_stRunVencCfg.vencStreamSnap.cfg.vbrMinBps),    CFG_DATA_TYPE_U32, "750", "rw", 128, 16000,   NULL},
	{"vbrMaxBps",    &(g_stRunVencCfg.vencStreamSnap.cfg.vbrMaxBps),    CFG_DATA_TYPE_U32, "3000", "rw", 128, 16000,   NULL},
    {"qpMinI",       &(g_stRunVencCfg.vencStreamSnap.cfg.qpMinI),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
	{"qpMinP",       &(g_stRunVencCfg.vencStreamSnap.cfg.qpMinP),       CFG_DATA_TYPE_U8,  "0",   "rw", 0, 51,  NULL},
	{"qpMaxI",       &(g_stRunVencCfg.vencStreamSnap.cfg.qpMaxI),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
	{"qpMaxP",       &(g_stRunVencCfg.vencStreamSnap.cfg.qpMaxP),       CFG_DATA_TYPE_U8,  "51",   "rw", 0, 51,  NULL},
	{"qpIWeight",    &(g_stRunVencCfg.vencStreamSnap.cfg.qpIWeight),    CFG_DATA_TYPE_U8,  "3",    "rw", 0, 100,  NULL},
	{"qpPWeight",    &(g_stRunVencCfg.vencStreamSnap.cfg.qpPWeight),    CFG_DATA_TYPE_U8,  "5",    "rw", 0, 100,  NULL},
	{"adaptQp",      &(g_stRunVencCfg.vencStreamSnap.cfg.adaptQp),      CFG_DATA_TYPE_U8,  "2",    "rw", 0, 100,  NULL},
	{"statTime",     &(g_stRunVencCfg.vencStreamSnap.cfg.statTime),      CFG_DATA_TYPE_U8,  "30",  "rw", 1, 60,  NULL},
};

void IPC_CFG_VENC_Print()
{
    int i, j;
    printf("********** Venc *********\n");
    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i ++)
    {
        for(j = 0; j < IPC_MAX_VENC_STREAM_PER_CHN; j++)
        {
            printf("channel %d stream %d:\n", i, j);
            CfgPrintMap(vencMap[i][j]);
            printf("\n");
        }
    }
    printf("********** Venc *********\n\n");
}

int IPC_CFG_VENC_Save()
{
    cJSON *root, *node, *item;
    char *out;
    int i, j;
    char tmp[32] = {0};

    printf("Save venc parameters...\n");

    root = cJSON_CreateObject();

    for (i = 0; i < IPC_MAX_PIPELINE_NUM; i ++)
    {
        node = cJSON_CreateObject();
        for(j = 0; j < IPC_MAX_VENC_STREAM_PER_CHN; j++)
        {
            item = CfgDataToCjsonByMap(vencMap[i][j]);
            sprintf(tmp, "stream_%d", j);
            cJSON_AddItemToObject(node, tmp, item);
        }
        printf("Channel %d venc...\n", i);
        sprintf(tmp, "channel_%d", i);
        cJSON_AddItemToObject(root, tmp, node);
    }

    //save snapshot venc paramters...
    item = CfgDataToCjsonByMap(vencSnapShot);
    sprintf(tmp, "channel_snap");
    cJSON_AddItemToObject(root, tmp, item);

    out = cJSON_Print(root);

    int ret = CfgWriteToFile(VENC_CFG_FILE, out);
    if (ret != 0) {
        PRINT_ERR("CfgWriteToFile %s error.", VENC_CFG_FILE);
        return -1;
    }

    free(out);
    cJSON_Delete(root);

    return 0;

}

void IPC_CFG_VENC_SetDual307()
{
    IPC_CFG_VENC_S * pVencCfg = IPC_CFG_VENC_GetParam();

    pVencCfg->vencStream[1][0].enable = 1;
    pVencCfg->vencStream[1][1].enable = 1;
}

void IPC_CFG_VENC_SetFour307()
{
    IPC_CFG_VENC_S * pVencCfg = IPC_CFG_VENC_GetParam();

    pVencCfg->vencStream[1][0].enable = 1;
    pVencCfg->vencStream[2][0].enable = 1;
    pVencCfg->vencStream[3][0].enable = 1;
    pVencCfg->vencStream[1][1].enable = 1;
    pVencCfg->vencStream[2][1].enable = 1;
    pVencCfg->vencStream[3][1].enable = 1;
}

void IPC_CFG_VENC_SetSingle464()
{
    IPC_CFG_VENC_S * pVencCfg = IPC_CFG_VENC_GetParam();

    pVencCfg->vencStream[0][0].cfg.fps = 25;
    pVencCfg->vencStream[0][0].cfg.width = 2688;
    pVencCfg->vencStream[0][0].cfg.height = 1520;
    pVencCfg->vencStream[0][0].cfg.roi_width = 2688;
    pVencCfg->vencStream[0][0].cfg.roi_height = 1520;
    pVencCfg->vencStream[0][1].cfg.fps = 25;
    pVencCfg->vencStream[0][1].cfg.width = 1920;
    pVencCfg->vencStream[0][1].cfg.height = 1080;
    pVencCfg->vencStream[0][1].cfg.roi_width = 1920;
    pVencCfg->vencStream[0][1].cfg.roi_height = 1080;
}

void IPC_CFG_VENC_SetSingle415()
{
    IPC_CFG_VENC_S * pVencCfg = IPC_CFG_VENC_GetParam();

    pVencCfg->vencStream[0][0].cfg.fps = 30;
    pVencCfg->vencStream[0][0].cfg.width = 3840;
    pVencCfg->vencStream[0][0].cfg.height = 2160;
    pVencCfg->vencStream[0][0].cfg.roi_width = 3840;
    pVencCfg->vencStream[0][0].cfg.roi_height = 2160;

    pVencCfg->vencStream[0][1].cfg.fps = 30;
    pVencCfg->vencStream[0][1].cfg.width = 1920;
    pVencCfg->vencStream[0][1].cfg.height = 1080;
    pVencCfg->vencStream[0][1].cfg.roi_width = 1920;
    pVencCfg->vencStream[0][1].cfg.roi_height = 1080;
}

void IPC_CFG_VENC_SetDvp8x1080P_25()
{
    int i = 0;
    IPC_CFG_VENC_S * pVencCfg = IPC_CFG_VENC_GetParam();

    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        pVencCfg->vencStream[i][0].enable = 1;
        pVencCfg->vencStream[i][0].cfg.fps = 25;
        pVencCfg->vencStream[i][1].enable = 1;
        pVencCfg->vencStream[i][1].cfg.fps = 25;
    }
}

void IPC_CFG_VENC_SetDvp8x1080P_30()
{
    int i = 0;
    IPC_CFG_VENC_S * pVencCfg = IPC_CFG_VENC_GetParam();

    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        pVencCfg->vencStream[i][0].enable = 1;
        pVencCfg->vencStream[i][0].cfg.fps = 30;
        pVencCfg->vencStream[i][1].enable = 1;
        pVencCfg->vencStream[i][1].cfg.fps = 30;
    }
}

void IPC_CFG_VENC_SetSingle530ai_30()
{
    IPC_CFG_VENC_S * pVencCfg = IPC_CFG_VENC_GetParam();

    pVencCfg->vencStream[0][0].cfg.fps = 30;
    pVencCfg->vencStream[0][0].cfg.width = 2880;
    pVencCfg->vencStream[0][0].cfg.height = 1616;
    pVencCfg->vencStream[0][0].cfg.roi_width = 2880;
    pVencCfg->vencStream[0][0].cfg.roi_height = 1616;

	pVencCfg->vencStream[0][1].enable = 1;
}

void IPC_CFG_VENC_SetSingleOV04A10()
{
    IPC_CFG_VENC_S * pVencCfg = IPC_CFG_VENC_GetParam();

    pVencCfg->vencStream[0][0].cfg.fps = 25;
    pVencCfg->vencStream[0][0].cfg.width = 2560;
    pVencCfg->vencStream[0][0].cfg.height = 1440;
    pVencCfg->vencStream[0][0].cfg.roi_width = 2560;
    pVencCfg->vencStream[0][0].cfg.roi_height = 1440;
    pVencCfg->vencStream[0][1].cfg.fps = 25;
    pVencCfg->vencStream[0][1].cfg.width = 1920;
    pVencCfg->vencStream[0][1].cfg.height = 1080;
    pVencCfg->vencStream[0][1].cfg.roi_width = 1920;
    pVencCfg->vencStream[0][1].cfg.roi_height = 1080;
}

void IPC_CFG_VENC_SetSingleGst417w()
{
    IPC_CFG_VENC_S * pVencCfg = IPC_CFG_VENC_GetParam();

    pVencCfg->vencStream[0][0].cfg.fps = 25;
    pVencCfg->vencStream[0][0].cfg.width = 408;
    pVencCfg->vencStream[0][0].cfg.height = 300;
    pVencCfg->vencStream[0][0].cfg.roi_width = 408;
    pVencCfg->vencStream[0][0].cfg.roi_height = 300;
    pVencCfg->vencStream[0][1].enable = 0;
}
void IPC_CFG_VENC_SetSingleGst212w4()
{
    IPC_CFG_VENC_S * pVencCfg = IPC_CFG_VENC_GetParam();

    pVencCfg->vencStream[0][0].cfg.fps = 25;
    pVencCfg->vencStream[0][0].cfg.width = 256;
    pVencCfg->vencStream[0][0].cfg.height = 192;
    pVencCfg->vencStream[0][0].cfg.roi_width = 256;
    pVencCfg->vencStream[0][0].cfg.roi_height = 192;
    pVencCfg->vencStream[0][1].enable = 0;
}


void IPC_CFG_VENC_SetFussion_OV04A10_Gst417w()
{
    IPC_CFG_VENC_S * pVencCfg = IPC_CFG_VENC_GetParam();

    pVencCfg->vencStream[0][0].cfg.fps = 25;
    pVencCfg->vencStream[0][0].cfg.width = 2560;
    pVencCfg->vencStream[0][0].cfg.height = 1440;
    pVencCfg->vencStream[0][0].cfg.roi_width = 2560;
    pVencCfg->vencStream[0][0].cfg.roi_height = 1440;
    pVencCfg->vencStream[0][1].cfg.fps = 25;
    pVencCfg->vencStream[0][1].cfg.width = 1920;
    pVencCfg->vencStream[0][1].cfg.height = 1080;
    pVencCfg->vencStream[0][1].cfg.roi_width = 1920;
    pVencCfg->vencStream[0][1].cfg.roi_height = 1080;
    pVencCfg->vencStream[0][1].enable = 1;

    pVencCfg->vencStream[1][0].cfg.fps = 25;
    pVencCfg->vencStream[1][0].cfg.width = 408;
    pVencCfg->vencStream[1][0].cfg.height = 300;
    pVencCfg->vencStream[1][0].cfg.roi_width = 408;
    pVencCfg->vencStream[1][0].cfg.roi_height = 300;
    pVencCfg->vencStream[1][0].enable = 1;
}

void IPC_CFG_VENC_SetFussion_OV04A10_Gst212w()
{
    IPC_CFG_VENC_S * pVencCfg = IPC_CFG_VENC_GetParam();

    pVencCfg->vencStream[0][0].cfg.fps = 25;
    pVencCfg->vencStream[0][0].cfg.width = 2560;
    pVencCfg->vencStream[0][0].cfg.height = 1440;
    pVencCfg->vencStream[0][0].cfg.roi_width = 2560;
    pVencCfg->vencStream[0][0].cfg.roi_height = 1440;
    pVencCfg->vencStream[0][1].cfg.fps = 25;
    pVencCfg->vencStream[0][1].cfg.width = 1920;
    pVencCfg->vencStream[0][1].cfg.height = 1080;
    pVencCfg->vencStream[0][1].cfg.roi_width = 1920;
    pVencCfg->vencStream[0][1].cfg.roi_height = 1080;
    pVencCfg->vencStream[0][1].enable = 1;

    pVencCfg->vencStream[1][0].cfg.fps = 25;
    pVencCfg->vencStream[1][0].cfg.width = 256;
    pVencCfg->vencStream[1][0].cfg.height = 192;
    pVencCfg->vencStream[1][0].cfg.roi_width = 256;
    pVencCfg->vencStream[1][0].cfg.roi_height = 192;
    pVencCfg->vencStream[1][0].enable = 1;
}

void IPC_CFG_VENC_SetSingleOV05A20()
{
    IPC_CFG_VENC_S * pVencCfg = IPC_CFG_VENC_GetParam();

    pVencCfg->vencStream[0][0].cfg.fps = 30;
    pVencCfg->vencStream[0][0].cfg.width = 2688;
    pVencCfg->vencStream[0][0].cfg.height = 1944;
    pVencCfg->vencStream[0][0].cfg.roi_width = 2688;
    pVencCfg->vencStream[0][0].cfg.roi_height = 1944;
    pVencCfg->vencStream[0][1].cfg.fps = 30;
    pVencCfg->vencStream[0][1].cfg.width = 1920;
    pVencCfg->vencStream[0][1].cfg.height = 1080;
    pVencCfg->vencStream[0][1].cfg.roi_width = 1920;
    pVencCfg->vencStream[0][1].cfg.roi_height = 1080;
}

void IPC_CFG_VENC_SetFussion_OV05A20_Gst212w()
{
    IPC_CFG_VENC_S * pVencCfg = IPC_CFG_VENC_GetParam();

    pVencCfg->vencStream[0][0].cfg.fps = 30;
    pVencCfg->vencStream[0][0].cfg.width = 2688;
    pVencCfg->vencStream[0][0].cfg.height = 1944;
    pVencCfg->vencStream[0][0].cfg.roi_width = 2688;
    pVencCfg->vencStream[0][0].cfg.roi_height = 1944;
    pVencCfg->vencStream[0][1].cfg.fps = 30;
    pVencCfg->vencStream[0][1].cfg.width = 1920;
    pVencCfg->vencStream[0][1].cfg.height = 1080;
    pVencCfg->vencStream[0][1].cfg.roi_width = 1920;
    pVencCfg->vencStream[0][1].cfg.roi_height = 1080;
    pVencCfg->vencStream[0][1].enable = 0;

    pVencCfg->vencStream[1][0].cfg.fps = 25;
    pVencCfg->vencStream[1][0].cfg.width = 256;
    pVencCfg->vencStream[1][0].cfg.height = 192;
    pVencCfg->vencStream[1][0].cfg.roi_width = 256;
    pVencCfg->vencStream[1][0].cfg.roi_height = 192;
    pVencCfg->vencStream[1][0].enable = 1;
}

void IPC_CFG_VENC_SetSingleH3812c1sg()
{
    IPC_CFG_VENC_S * pVencCfg = IPC_CFG_VENC_GetParam();
    
    pVencCfg->vencStream[0][0].cfg.fps = 25;
    pVencCfg->vencStream[0][0].cfg.width = 384;
    pVencCfg->vencStream[0][0].cfg.height = 288;
    pVencCfg->vencStream[0][0].cfg.roi_width = 384;
    pVencCfg->vencStream[0][0].cfg.roi_height = 288;
    pVencCfg->vencStream[0][1].enable = 0;
}

void IPC_CFG_VENC_SetFussion_307_H3812c1sg()
{
    IPC_CFG_VENC_S * pVencCfg = IPC_CFG_VENC_GetParam();
    
    pVencCfg->vencStream[0][0].cfg.fps = 25;
    pVencCfg->vencStream[0][0].cfg.width = 1920;
    pVencCfg->vencStream[0][0].cfg.height = 1080;
    pVencCfg->vencStream[0][0].cfg.roi_width = 1920;
    pVencCfg->vencStream[0][0].cfg.roi_height = 1080;
    pVencCfg->vencStream[0][1].cfg.fps = 25;
    pVencCfg->vencStream[0][1].cfg.width = 640;
    pVencCfg->vencStream[0][1].cfg.height = 480;
    pVencCfg->vencStream[0][1].cfg.roi_width = 640;
    pVencCfg->vencStream[0][1].cfg.roi_height = 480;
    pVencCfg->vencStream[0][1].enable = 1;
    
    pVencCfg->vencStream[1][0].cfg.fps = 25;
    pVencCfg->vencStream[1][0].cfg.width = 384;
    pVencCfg->vencStream[1][0].cfg.height = 288;
    pVencCfg->vencStream[1][0].cfg.roi_width = 384;
    pVencCfg->vencStream[1][0].cfg.roi_height = 288;
    pVencCfg->vencStream[1][0].enable = 1;
}

AR_S32 IPC_CFG_VENC_LoadMaxResource()
{
	AR_S32 i = 0, j = 0;
	IPC_CFG_VENC_S * pVencCfg = IPC_CFG_VENC_GetParam();

	for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
	{
		for(j = 0; j < IPC_MAX_VENC_STREAM_PER_CHN; j++)
		{
			g_stVencMaxResource.stVpssMaxResource[i][j].u32Width = pVencCfg->vencStream[i][j].cfg.width;
			g_stVencMaxResource.stVpssMaxResource[i][j].u32Height = pVencCfg->vencStream[i][j].cfg.height;
		}
	}
	
	return 0;
}

AR_S32 IPC_CFG_VENC_LoadDefault(CFG_RUN_MODE_E enMode)
{
    int i, j;

    printf("Enter %s \n", __FUNCTION__);

    for (i = 0; i < IPC_MAX_PIPELINE_NUM; i ++)
    {
        for(j = 0; j < IPC_MAX_VENC_STREAM_PER_CHN; j++)
        {
            CfgLoadDefValue(vencMap[i][j]);
        }
    }

    //load snapshot
    CfgLoadDefValue(vencSnapShot);

    switch(enMode)
    {
        case CFG_RUN_MODE_DUAL_SC230AI_LOW_BW:
        case CFG_RUN_MODE_DUAL_307_LOW_BW:
        case CFG_RUN_MODE_DUAL_307_ARS31:
            IPC_CFG_VENC_SetDual307();
            break;
        case CFG_RUN_MODE_SINGLE_464_LOW_BW:
        case CFG_RUN_MODE_SINGLE_464_HDR_LOW_BW:
        case CFG_RUN_MODE_SINGLE_464_ARIPC:
        case CFG_RUN_MODE_SINGLE_464_HDR_ARIPC:
        case CFG_RUN_MODE_SINGLE_347:
            IPC_CFG_VENC_SetSingle464();
            break;
        case CFG_RUN_MODE_SINGLE_415_LOW_BW:
        case CFG_RUN_MODE_SINGLE_415_ARIPC:
            IPC_CFG_VENC_SetSingle415();
            break;
        case CFG_RUN_MODE_DVP_8X1080P_25_LOW_BW:
            IPC_CFG_VENC_SetDvp8x1080P_25();
            break;
        case CFG_RUN_MODE_DVP_8X1080P_30_LOW_BW:
            IPC_CFG_VENC_SetDvp8x1080P_30();
            break;
        case CFG_RUN_MODE_FOUR_307_LOW_BW:
            IPC_CFG_VENC_SetFour307();
            break;
        case CFG_RUN_MODE_SINGLE_530AI_30:
            IPC_CFG_VENC_SetSingle530ai_30();
            break;
        case CFG_RUN_MODE_SINGLE_OV04A10_25:
        case CFG_RUN_MODE_SINGLE_OV04A10_25_HDR:
        case CFG_RUN_MODE_SINGLE_OV04A10_25_ARIPC:
        case CFG_RUN_MODE_SINGLE_OV04A10_25_HDR_ARIPC:
        case CFG_RUN_MODE_SINGLE_OV04A10_ARS31:
        case CFG_RUN_MODE_SINGLE_OV04A10_HDR_ARS31:
            IPC_CFG_VENC_SetSingleOV04A10();
            break;
        case CFG_RUN_MODE_SINGLE_GST417W_ARS31:
            IPC_CFG_VENC_SetSingleGst417w();
            break;
        case CFG_RUN_MODE_OV04A10_GST417W_ARS31:
            IPC_CFG_VENC_SetFussion_OV04A10_Gst417w();
            break;
        case CFG_RUN_MODE_SINGLE_GST212W4_ARS31:
            IPC_CFG_VENC_SetSingleGst212w4();
            break;
        case CFG_RUN_MODE_OV04A10_GST212W_ARS31:
            IPC_CFG_VENC_SetFussion_OV04A10_Gst212w();
            break;
        case CFG_RUN_MODE_SINGLE_OV05A20_ARS31:
            IPC_CFG_VENC_SetSingleOV05A20();
            break;  
        case CFG_RUN_MODE_OV05A20_GST212W_ARS31:
            IPC_CFG_VENC_SetFussion_OV05A20_Gst212w();
            break;
        case CFG_RUN_MODE_SINGLE_H3812C1SG_ARS31:
            IPC_CFG_VENC_SetSingleH3812c1sg();
            break;
        case CFG_RUN_MODE_307_H3812C1SG_ARS31:
            IPC_CFG_VENC_SetFussion_307_H3812c1sg();
            break;
        default: /*single 307 w/o hdr*/
            break;
    }

    return 0;
}

AR_S32 IPC_CFG_VENC_Load(CFG_RUN_MODE_E enMode)
{
    char *data = NULL;
    char tmp[32] = {0};
    int i, j;

    data = CfgReadFromFile(VENC_CFG_FILE);
    if (data == NULL)
    {
        PRINT_INFO("load %s error, so to load default cfg param.\n", VENC_CFG_FILE);
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
            for(j = 0; j < IPC_MAX_VENC_STREAM_PER_CHN; j++)
            {
                CfgLoadDefValue(vencMap[i][j]);
            }
        }
        else
        {
            for(j = 0; j < IPC_MAX_VENC_STREAM_PER_CHN; j++)
            {
                sprintf(tmp, "stream_%d", j);
                stream = cJSON_GetObjectItem(chn, tmp);
                if(!stream)
                {
                    CfgLoadDefValue(vencMap[i][j]);
                }
                else
                {
                    CfgCjsonToDataByMap(vencMap[i][j], stream);
                }
            }
        }
    }

    //load snapshot venc param
    cJSON *Snapshot_channel = NULL;
    sprintf(tmp, "channel_snap");
    Snapshot_channel = cJSON_GetObjectItem(json, tmp);
    if(!Snapshot_channel)
    {
        CfgLoadDefValue(vencSnapShot);
    }
    else
    {
        CfgCjsonToDataByMap(vencSnapShot, Snapshot_channel);
    }

    cJSON_Delete(json);
    free(data);

    IPC_CFG_VENC_LoadMaxResource();
    return 0;

err:
    IPC_CFG_VENC_LoadDefault(enMode);
    IPC_CFG_VENC_Save();
    IPC_CFG_VENC_LoadMaxResource();
    return 0;

}

IPC_CFG_VENC_S * IPC_CFG_VENC_GetParam()
{
    return &g_stRunVencCfg;
}

IPC_VENC_MAX_RESOURCE_S * IPC_CFG_VENC_GetMaxResource()
{
    return &g_stVencMaxResource;
}

