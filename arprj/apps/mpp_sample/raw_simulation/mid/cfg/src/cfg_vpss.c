#include "cfg_vpss.h"
#include "cfg_common.h"
IPC_CFG_VPSS_S g_stVpssCfg;
IPC_VPSS_MAX_RESOURCE_S g_stVpssMaxResource = {0};

CFG_MAP vpssGrpMiscMap[IPC_MAX_VPSS_GRP_NEEDED][6] =
{
    //vpss grp 0
    {
        {"enable",       &(g_stVpssCfg.stGrpCfg[0].bGroup),           CFG_DATA_TYPE_U32, "1",    "rw", 0, 1, NULL},
        {"eis_enable",       &(g_stVpssCfg.stGrpCfg[0].bEis),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"ldc_enable",            &(g_stVpssCfg.stGrpCfg[0].bLdc),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"flip_enable", &(g_stVpssCfg.stGrpCfg[0].bFlip),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"mirror_enable", &(g_stVpssCfg.stGrpCfg[0].bMirror),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {NULL,}
    },
    //vpss grp 1
    {
        {"enable",       &(g_stVpssCfg.stGrpCfg[1].bGroup),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"eis_enable",       &(g_stVpssCfg.stGrpCfg[1].bEis),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"ldc_enable",            &(g_stVpssCfg.stGrpCfg[1].bLdc),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"flip_enable", &(g_stVpssCfg.stGrpCfg[1].bFlip),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"mirror_enable", &(g_stVpssCfg.stGrpCfg[1].bMirror),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {NULL,}
    },
    //vpss grp 2
    {
        {"enable",       &(g_stVpssCfg.stGrpCfg[2].bGroup),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"eis_enable",       &(g_stVpssCfg.stGrpCfg[2].bEis),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"ldc_enable",            &(g_stVpssCfg.stGrpCfg[2].bLdc),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"flip_enable", &(g_stVpssCfg.stGrpCfg[2].bFlip),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"mirror_enable", &(g_stVpssCfg.stGrpCfg[2].bMirror),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {NULL,}
    },
    //vpss grp 3
    {
        {"enable",       &(g_stVpssCfg.stGrpCfg[3].bGroup),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"eis_enable",       &(g_stVpssCfg.stGrpCfg[3].bEis),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"ldc_enable",            &(g_stVpssCfg.stGrpCfg[3].bLdc),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"flip_enable", &(g_stVpssCfg.stGrpCfg[3].bFlip),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"mirror_enable", &(g_stVpssCfg.stGrpCfg[3].bMirror),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {NULL,}
    },
    //vpss grp 4
    {
        {"enable",       &(g_stVpssCfg.stGrpCfg[4].bGroup),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"eis_enable",       &(g_stVpssCfg.stGrpCfg[4].bEis),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"ldc_enable",            &(g_stVpssCfg.stGrpCfg[4].bLdc),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"flip_enable", &(g_stVpssCfg.stGrpCfg[4].bFlip),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"mirror_enable", &(g_stVpssCfg.stGrpCfg[4].bMirror),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {NULL,}
    },
    //vpss grp 5
    {
        {"enable",       &(g_stVpssCfg.stGrpCfg[5].bGroup),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"eis_enable",       &(g_stVpssCfg.stGrpCfg[5].bEis),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"ldc_enable",            &(g_stVpssCfg.stGrpCfg[5].bLdc),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"flip_enable", &(g_stVpssCfg.stGrpCfg[5].bFlip),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"mirror_enable", &(g_stVpssCfg.stGrpCfg[5].bMirror),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {NULL,}
    },
    //vpss grp 6
    {
        {"enable",       &(g_stVpssCfg.stGrpCfg[6].bGroup),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"eis_enable",       &(g_stVpssCfg.stGrpCfg[6].bEis),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"ldc_enable",            &(g_stVpssCfg.stGrpCfg[6].bLdc),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"flip_enable", &(g_stVpssCfg.stGrpCfg[6].bFlip),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"mirror_enable", &(g_stVpssCfg.stGrpCfg[6].bMirror),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {NULL,}
    },
    //vpss grp 7
    {
        {"enable",       &(g_stVpssCfg.stGrpCfg[7].bGroup),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"eis_enable",       &(g_stVpssCfg.stGrpCfg[7].bEis),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"ldc_enable",            &(g_stVpssCfg.stGrpCfg[7].bLdc),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"flip_enable", &(g_stVpssCfg.stGrpCfg[7].bFlip),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"mirror_enable", &(g_stVpssCfg.stGrpCfg[7].bMirror),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {NULL,}
    },
    //vpss grp 8
    {
        {"enable",       &(g_stVpssCfg.stGrpCfg[8].bGroup),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"eis_enable",       &(g_stVpssCfg.stGrpCfg[8].bEis),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"ldc_enable",            &(g_stVpssCfg.stGrpCfg[8].bLdc),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"flip_enable", &(g_stVpssCfg.stGrpCfg[8].bFlip),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"mirror_enable", &(g_stVpssCfg.stGrpCfg[8].bMirror),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {NULL,}
    },
    //vpss grp 9
    {
        {"enable",       &(g_stVpssCfg.stGrpCfg[9].bGroup),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"eis_enable",       &(g_stVpssCfg.stGrpCfg[9].bEis),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"ldc_enable",            &(g_stVpssCfg.stGrpCfg[9].bLdc),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"flip_enable", &(g_stVpssCfg.stGrpCfg[9].bFlip),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"mirror_enable", &(g_stVpssCfg.stGrpCfg[9].bMirror),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {NULL,}
    },
    //vpss grp 10
    {
        {"enable",       &(g_stVpssCfg.stGrpCfg[10].bGroup),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"eis_enable",       &(g_stVpssCfg.stGrpCfg[10].bEis),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"ldc_enable",            &(g_stVpssCfg.stGrpCfg[10].bLdc),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"flip_enable", &(g_stVpssCfg.stGrpCfg[10].bFlip),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"mirror_enable", &(g_stVpssCfg.stGrpCfg[10].bMirror),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {NULL,}
    },
    //vpss grp 11
    {
        {"enable",       &(g_stVpssCfg.stGrpCfg[11].bGroup),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"eis_enable",       &(g_stVpssCfg.stGrpCfg[11].bEis),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"ldc_enable",            &(g_stVpssCfg.stGrpCfg[11].bLdc),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"flip_enable", &(g_stVpssCfg.stGrpCfg[11].bFlip),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"mirror_enable", &(g_stVpssCfg.stGrpCfg[11].bMirror),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {NULL,}
    },
    //vpss grp 12
    {
        {"enable",       &(g_stVpssCfg.stGrpCfg[12].bGroup),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"eis_enable",       &(g_stVpssCfg.stGrpCfg[12].bEis),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"ldc_enable",            &(g_stVpssCfg.stGrpCfg[12].bLdc),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"flip_enable", &(g_stVpssCfg.stGrpCfg[12].bFlip),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"mirror_enable", &(g_stVpssCfg.stGrpCfg[12].bMirror),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {NULL,}
    },
    //vpss grp 13
    {
        {"enable",       &(g_stVpssCfg.stGrpCfg[13].bGroup),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"eis_enable",       &(g_stVpssCfg.stGrpCfg[13].bEis),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"ldc_enable",            &(g_stVpssCfg.stGrpCfg[13].bLdc),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"flip_enable", &(g_stVpssCfg.stGrpCfg[13].bFlip),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"mirror_enable", &(g_stVpssCfg.stGrpCfg[13].bMirror),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {NULL,}
    },
    //vpss grp 14
    {
        {"enable",       &(g_stVpssCfg.stGrpCfg[14].bGroup),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"eis_enable",       &(g_stVpssCfg.stGrpCfg[14].bEis),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"ldc_enable",            &(g_stVpssCfg.stGrpCfg[14].bLdc),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"flip_enable", &(g_stVpssCfg.stGrpCfg[14].bFlip),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"mirror_enable", &(g_stVpssCfg.stGrpCfg[14].bMirror),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {NULL,}
    },
    //vpss grp 15
    {
        {"enable",       &(g_stVpssCfg.stGrpCfg[15].bGroup),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"eis_enable",       &(g_stVpssCfg.stGrpCfg[15].bEis),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"ldc_enable",            &(g_stVpssCfg.stGrpCfg[15].bLdc),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"flip_enable", &(g_stVpssCfg.stGrpCfg[15].bFlip),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"mirror_enable", &(g_stVpssCfg.stGrpCfg[15].bMirror),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {NULL,}
    }
};

CFG_MAP vpssGrpRotateMap[IPC_MAX_VPSS_GRP_NEEDED][3] =
{
    //grp 0
    {
        {"enable",       &(g_stVpssCfg.stGrpCfg[0].stRotate.bEnable),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"angle",       &(g_stVpssCfg.stGrpCfg[0].stRotate.enAngle),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 3, NULL},
        {NULL,}
    },
    //grp 1
    {
        {"enable",       &(g_stVpssCfg.stGrpCfg[1].stRotate.bEnable),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"angle",       &(g_stVpssCfg.stGrpCfg[1].stRotate.enAngle),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 3, NULL},
        {NULL,}
    },
    //grp 2
    {
        {"enable",       &(g_stVpssCfg.stGrpCfg[2].stRotate.bEnable),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"angle",       &(g_stVpssCfg.stGrpCfg[2].stRotate.enAngle),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 3, NULL},
        {NULL,}
    },
    //grp 3
    {
        {"enable",       &(g_stVpssCfg.stGrpCfg[3].stRotate.bEnable),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"angle",       &(g_stVpssCfg.stGrpCfg[3].stRotate.enAngle),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 3, NULL},
        {NULL,}
    },
    //grp 4
    {
        {"enable",       &(g_stVpssCfg.stGrpCfg[4].stRotate.bEnable),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"angle",       &(g_stVpssCfg.stGrpCfg[4].stRotate.enAngle),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 3, NULL},
        {NULL,}
    },
    //grp 5
    {
        {"enable",       &(g_stVpssCfg.stGrpCfg[5].stRotate.bEnable),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"angle",       &(g_stVpssCfg.stGrpCfg[5].stRotate.enAngle),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 3, NULL},
        {NULL,}
    },
    //grp 6
    {
        {"enable",       &(g_stVpssCfg.stGrpCfg[6].stRotate.bEnable),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"angle",       &(g_stVpssCfg.stGrpCfg[6].stRotate.enAngle),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 3, NULL},
        {NULL,}
    },
    //grp 7
    {
        {"enable",       &(g_stVpssCfg.stGrpCfg[7].stRotate.bEnable),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"angle",       &(g_stVpssCfg.stGrpCfg[7].stRotate.enAngle),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 3, NULL},
        {NULL,}
    },
    //grp 8
    {
        {"enable",       &(g_stVpssCfg.stGrpCfg[8].stRotate.bEnable),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"angle",       &(g_stVpssCfg.stGrpCfg[8].stRotate.enAngle),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 3, NULL},
        {NULL,}
    },
    //grp 9
    {
        {"enable",       &(g_stVpssCfg.stGrpCfg[9].stRotate.bEnable),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"angle",       &(g_stVpssCfg.stGrpCfg[9].stRotate.enAngle),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 3, NULL},
        {NULL,}
    },
    //grp 10
    {
        {"enable",       &(g_stVpssCfg.stGrpCfg[10].stRotate.bEnable),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"angle",       &(g_stVpssCfg.stGrpCfg[10].stRotate.enAngle),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 3, NULL},
        {NULL,}
    },
    //grp 11
    {
        {"enable",       &(g_stVpssCfg.stGrpCfg[11].stRotate.bEnable),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"angle",       &(g_stVpssCfg.stGrpCfg[11].stRotate.enAngle),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 3, NULL},
        {NULL,}
    },
    //grp 12
    {
        {"enable",       &(g_stVpssCfg.stGrpCfg[12].stRotate.bEnable),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"angle",       &(g_stVpssCfg.stGrpCfg[12].stRotate.enAngle),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 3, NULL},
        {NULL,}
    },
    //grp 13
    {
        {"enable",       &(g_stVpssCfg.stGrpCfg[13].stRotate.bEnable),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"angle",       &(g_stVpssCfg.stGrpCfg[13].stRotate.enAngle),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 3, NULL},
        {NULL,}
    },
    //grp 14
    {
        {"enable",       &(g_stVpssCfg.stGrpCfg[14].stRotate.bEnable),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"angle",       &(g_stVpssCfg.stGrpCfg[14].stRotate.enAngle),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 3, NULL},
        {NULL,}
    },
    //grp 15
    {
        {"enable",       &(g_stVpssCfg.stGrpCfg[15].stRotate.bEnable),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 1, NULL},
        {"angle",       &(g_stVpssCfg.stGrpCfg[15].stRotate.enAngle),           CFG_DATA_TYPE_U32, "0",    "rw", 0, 3, NULL},
        {NULL,}
    }
};

static AR_S32 ParseMosaic(cJSON * pMosaicJson, VPSS_MOSAIC_S * pstMosaic)
{
    int i = 0;
    cJSON * pArray = NULL;
    cJSON * pSub = NULL;
    AR_U32 u32Num = 0;
    AR_CHAR chTmp[32] = {0};

    pSub = cJSON_GetObjectItem(pMosaicJson, "number");
    if(!pSub)
    {
        //no mosaic
        memset(pstMosaic, 0, sizeof(VPSS_MOSAIC_S));
        return 0;
    }

    u32Num = pSub->valueint;
    u32Num = (u32Num > 4) ? 4 : u32Num;
    pstMosaic->u32Num = u32Num;

    for(i = 0; i < u32Num; i++)
    {
        sprintf(chTmp, "rect_%d", i);
        pArray = cJSON_GetObjectItem(pMosaicJson, chTmp);
        if(!pArray)
        {
            continue;
        }

        pSub = cJSON_GetArrayItem(pArray, 0);
        if(pSub) pstMosaic->stRect[i].u32X = pSub->valueint;

        pSub = cJSON_GetArrayItem(pArray, 1);
        if(pSub) pstMosaic->stRect[i].u32Y = pSub->valueint;

        pSub = cJSON_GetArrayItem(pArray, 2);
        if(pSub) pstMosaic->stRect[i].u32W = pSub->valueint;

        pSub = cJSON_GetArrayItem(pArray, 3);
        if(pSub) pstMosaic->stRect[i].u32H = pSub->valueint;
    }

    return 0;
}

static cJSON * MosaicToJson(VPSS_MOSAIC_S * pstMosaic)
{
    int i = 0;
    AR_CHAR chTmp[32] = {0};
    cJSON * pMosaic = cJSON_CreateObject();

    cJSON_AddItemToObject(pMosaic, "number", cJSON_CreateNumber(pstMosaic->u32Num));

    for(i = 0; i < 4; i++)
    {
        cJSON * pArray = cJSON_CreateArray();
        cJSON_AddItemToArray(pArray, cJSON_CreateNumber(pstMosaic->stRect[i].u32X));
        cJSON_AddItemToArray(pArray, cJSON_CreateNumber(pstMosaic->stRect[i].u32Y));
        cJSON_AddItemToArray(pArray, cJSON_CreateNumber(pstMosaic->stRect[i].u32W));
        cJSON_AddItemToArray(pArray, cJSON_CreateNumber(pstMosaic->stRect[i].u32H));

        sprintf(chTmp, "rect_%d", i);
        cJSON_AddItemToObject(pMosaic, chTmp, pArray);
    }

    return pMosaic;
}

static cJSON * CoverToJson(VPSS_COVER_S * pstCover)
{
    int i = 0;
    AR_CHAR chTmp[32] = {0};
    cJSON * pCover = cJSON_CreateObject();

    cJSON_AddItemToObject(pCover, "number", cJSON_CreateNumber(pstCover->u32Num));

    for(i = 0; i < 4; i++)
    {
        sprintf(chTmp, "color_%d", i);
        cJSON_AddItemToObject(pCover, chTmp, cJSON_CreateNumber(pstCover->u32ARGB8888[i]));

        cJSON * pArray = cJSON_CreateArray();
        cJSON_AddItemToArray(pArray, cJSON_CreateNumber(pstCover->stRect[i].u32X));
        cJSON_AddItemToArray(pArray, cJSON_CreateNumber(pstCover->stRect[i].u32Y));
        cJSON_AddItemToArray(pArray, cJSON_CreateNumber(pstCover->stRect[i].u32W));
        cJSON_AddItemToArray(pArray, cJSON_CreateNumber(pstCover->stRect[i].u32H));

        sprintf(chTmp, "rect_%d", i);
        cJSON_AddItemToObject(pCover, chTmp, pArray);
    }

    return pCover;
}

static cJSON * LdcToJson(VPSS_LDC_COEFF_S * pstLdc)
{
    int i = 0;
    cJSON * pLdc = cJSON_CreateObject();

    cJSON_AddItemToObject(pLdc, "k0", cJSON_CreateNumber(pstLdc->f32K0));
    cJSON_AddItemToObject(pLdc, "k1", cJSON_CreateNumber(pstLdc->f32K1));
    cJSON_AddItemToObject(pLdc, "k2", cJSON_CreateNumber(pstLdc->f32K2));

    cJSON * pArray = cJSON_CreateArray();
    for(i = 0; i < 9; i++)
    {
        cJSON_AddItemToArray(pArray, cJSON_CreateNumber(pstLdc->f32K[i]));
    }
    cJSON_AddItemToObject(pLdc, "k", pArray);

    return pLdc;
}

static cJSON * ChnToJson(VPSS_CHN_CFG_S * pstChn)
{
    cJSON * pChn = cJSON_CreateObject();

    cJSON_AddItemToObject(pChn, "enable", cJSON_CreateNumber(pstChn->bEnable));
    cJSON_AddItemToObject(pChn, "width", cJSON_CreateNumber(pstChn->u32Width));
    cJSON_AddItemToObject(pChn, "height", cJSON_CreateNumber(pstChn->u32Height));

    cJSON_AddItemToObject(pChn, "crop_enable", cJSON_CreateNumber(pstChn->stCrop.bEnable));

    cJSON * pArray = cJSON_CreateArray();
    cJSON_AddItemToArray(pArray, cJSON_CreateNumber(pstChn->stCrop.stRect.u32X));
    cJSON_AddItemToArray(pArray, cJSON_CreateNumber(pstChn->stCrop.stRect.u32Y));
    cJSON_AddItemToArray(pArray, cJSON_CreateNumber(pstChn->stCrop.stRect.u32W));
    cJSON_AddItemToArray(pArray, cJSON_CreateNumber(pstChn->stCrop.stRect.u32H));

    cJSON_AddItemToObject(pChn, "crop_rect", pArray);

    cJSON_AddItemToObject(pChn, "flip_enable", cJSON_CreateNumber(pstChn->bFlip));
    cJSON_AddItemToObject(pChn, "mirror_enable", cJSON_CreateNumber(pstChn->bMirror));

    cJSON_AddItemToObject(pChn, "yuv2rgb_enable", cJSON_CreateNumber(pstChn->u32YUV2RGBEnable));

    return pChn;
}

static AR_S32 ParseCover(cJSON * pCoverJson, VPSS_COVER_S * pstCover)
{
    int i = 0;
    cJSON * pArray = NULL;
    cJSON * pSub = NULL;
    AR_U32 u32Num = 0;
    AR_CHAR chTmp[32] = {0};

    pSub = cJSON_GetObjectItem(pCoverJson, "number");
    if(!pSub)
    {
        //no mosaic
        memset(&pstCover, 0, sizeof(VPSS_COVER_S));
        return 0;
    }

    u32Num = pSub->valueint;
    u32Num = (u32Num > 4) ? 4 : u32Num;
    pstCover->u32Num = u32Num;

    for(i = 0; i < u32Num; i++)
    {
        sprintf(chTmp, "color_%d", i);
        pSub = cJSON_GetObjectItem(pCoverJson, chTmp);
        if(pSub) pstCover->u32ARGB8888[i] = pSub->valueint;

        sprintf(chTmp, "rect_%d", i);
        pArray = cJSON_GetObjectItem(pCoverJson, chTmp);
        if(!pArray)
        {
            continue;
        }

        pSub = cJSON_GetArrayItem(pArray, 0);
        if(pSub) pstCover->stRect[i].u32X = pSub->valueint;

        pSub = cJSON_GetArrayItem(pArray, 1);
        if(pSub) pstCover->stRect[i].u32Y = pSub->valueint;

        pSub = cJSON_GetArrayItem(pArray, 2);
        if(pSub) pstCover->stRect[i].u32W = pSub->valueint;

        pSub = cJSON_GetArrayItem(pArray, 3);
        if(pSub) pstCover->stRect[i].u32H = pSub->valueint;
    }

    return 0;
}

static AR_S32 ParseLdc(cJSON * pLdc, VPSS_LDC_COEFF_S * pstLdc)
{
    int i = 0;
    cJSON * pArray = NULL;
    cJSON * pSub = NULL;

    pSub = cJSON_GetObjectItem(pLdc, "k0");
    if(!pSub)
    {
        printf("Not found k0 param in json.\n");
        return -1;
    }
    pstLdc->f32K0 = pSub->valuedouble;

    pSub = cJSON_GetObjectItem(pLdc, "k1");
    if(!pSub)
    {
        printf("Not found k1 param in json.\n");
        return -1;
    }
    pstLdc->f32K1 = pSub->valuedouble;

    pSub = cJSON_GetObjectItem(pLdc, "k2");
    if(!pSub)
    {
        printf("Not found k2 param in json.\n");
        return -1;
    }
    pstLdc->f32K2 = pSub->valuedouble;

    pArray = cJSON_GetObjectItem(pLdc, "k");
    if(!pArray)
    {
        printf("Not found k param in json.\n");
        return -1;
    }

    for(i = 0; i < 9; i++)
    {
        pSub = cJSON_GetArrayItem(pArray, i);
        if(pSub) pstLdc->f32K[i] = pSub->valuedouble;
    }

    return 0;
}

static AR_S32 ParseChn(cJSON * pChnJson, VPSS_CHN_CFG_S * pstVpssChn)
{
    cJSON * pArray = NULL;
    cJSON * pSub = NULL;

    pSub = cJSON_GetObjectItem(pChnJson, "enable");
    if(!pSub || pSub->valueint == 0)
    {
        //no ch
        memset(pstVpssChn, 0, sizeof(VPSS_CHN_CFG_S));
        return 0;
    }

    pstVpssChn->bEnable = pSub->valueint;

    pSub = cJSON_GetObjectItem(pChnJson, "crop_enable");
    if(pSub && pSub->valueint)
    {
        pstVpssChn->stCrop.bEnable = pSub->valueint;

        pArray = cJSON_GetObjectItem(pChnJson, "crop_rect");
        if(pArray)
        {
            pSub = cJSON_GetArrayItem(pArray, 0);
            if(pSub) pstVpssChn->stCrop.stRect.u32X = pSub->valueint;
            pSub = cJSON_GetArrayItem(pArray, 1);
            if(pSub) pstVpssChn->stCrop.stRect.u32Y = pSub->valueint;
            pSub = cJSON_GetArrayItem(pArray, 2);
            if(pSub) pstVpssChn->stCrop.stRect.u32W = pSub->valueint;
            pSub = cJSON_GetArrayItem(pArray, 3);
            if(pSub) pstVpssChn->stCrop.stRect.u32H = pSub->valueint;
        }

    }

    pSub = cJSON_GetObjectItem(pChnJson, "width");
    if(pSub && pSub->valueint)
    {
        pstVpssChn->u32Width = pSub->valueint;
    }
    pSub = cJSON_GetObjectItem(pChnJson, "height");
    if(pSub && pSub->valueint)
    {
        pstVpssChn->u32Height = pSub->valueint;
    }

    pSub = cJSON_GetObjectItem(pChnJson, "yuv2rgb_enable");
    if(pSub && pSub->valueint)
    {
        pstVpssChn->u32YUV2RGBEnable = pSub->valueint;
    }
    pSub = cJSON_GetObjectItem(pChnJson, "flip_enable");
    if(pSub && pSub->valueint)
    {
        pstVpssChn->bFlip = pSub->valueint;
    }
    pSub = cJSON_GetObjectItem(pChnJson, "mirror_enable");
    if(pSub && pSub->valueint)
    {
        pstVpssChn->bMirror = pSub->valueint;
    }

    return 0;
}

AR_S32 IPC_CFG_VPSS_Save()
{
    cJSON * root;
    char *out;
    int i, j;
    char tmp[32] = {0};

    printf("Save vpss parameters...\n");

    root = cJSON_CreateObject();

    for (i = 0; i < IPC_MAX_VPSS_GRP_NEEDED; i ++)
    {
        cJSON * pGrp = CfgDataToCjsonByMap(vpssGrpMiscMap[i]);
        sprintf(tmp, "group_%d", i);
        cJSON_AddItemToObject(root, tmp, pGrp);

        cJSON * pRotate = CfgDataToCjsonByMap(vpssGrpRotateMap[i]);
        cJSON_AddItemToObject(pGrp, "rotate", pRotate);

        cJSON * pMosaic = MosaicToJson(&g_stVpssCfg.stGrpCfg[i].stMosaic);
        cJSON_AddItemToObject(pGrp, "mosaic", pMosaic);

        cJSON * pCover = CoverToJson(&g_stVpssCfg.stGrpCfg[i].stCover);
        cJSON_AddItemToObject(pGrp, "cover", pCover);

        cJSON * pLdc = LdcToJson(&g_stVpssCfg.stGrpCfg[i].stLdcCoeff);
        cJSON_AddItemToObject(pGrp, "ldc_coeff", pLdc);

        for(j = 0; j < IPC_MAX_STREAM_PER_PIPE; j++)
        {
            sprintf(tmp, "channel_%d", j);
            cJSON * pChn = ChnToJson(&g_stVpssCfg.stGrpCfg[i].stChnCfg[j]);
            cJSON_AddItemToObject(pGrp, tmp, pChn);
        }

    }

    out = cJSON_Print(root);

    int ret = CfgWriteToFile(VPSS_CFG_FILE, out);
    if (ret != 0)
    {
        PRINT_ERR("CfgWriteToFile %s error.", VPSS_CFG_FILE);
        return -1;
    }

    free(out);
    cJSON_Delete(root);

    return 0;
}

AR_S32 IPC_CFG_VPSS_Load(CFG_RUN_MODE_E enMode)
{
    char *data = NULL;
    char tmp[32] = {0};
    int i, j;

    data = CfgReadFromFile(VPSS_CFG_FILE);
    if (data == NULL)
    {
        PRINT_INFO("load %s error, so to load default cfg param.\n", VPSS_CFG_FILE);
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

    cJSON * pVpssGrp = NULL;

    for(i = 0; i < IPC_MAX_VPSS_GRP_NEEDED; i++)
    {
        sprintf(tmp, "group_%d", i);
        pVpssGrp = cJSON_GetObjectItem(json, tmp);
        if(!pVpssGrp)
        {
            CfgLoadDefValue(vpssGrpMiscMap[i]);
        }
        else
        {
            CfgCjsonToDataByMap(vpssGrpMiscMap[i], pVpssGrp);
        }

        cJSON * pRotate = cJSON_GetObjectItem(pVpssGrp, "rotate");
        if(!pRotate)
        {
            CfgLoadDefValue(vpssGrpRotateMap[i]);
        }
        else
        {
            CfgCjsonToDataByMap(vpssGrpRotateMap[i], pRotate);
        }

        cJSON * pMosaic = cJSON_GetObjectItem(pVpssGrp, "mosaic");
        if(pMosaic)
        {
            ParseMosaic(pMosaic, &g_stVpssCfg.stGrpCfg[i].stMosaic);
        }

        cJSON * pCover = cJSON_GetObjectItem(pVpssGrp, "cover");
        if(pCover)
        {
            ParseCover(pCover, &g_stVpssCfg.stGrpCfg[i].stCover);
        }

        cJSON * pLdc = cJSON_GetObjectItem(pVpssGrp, "ldc_coeff");
        if(pLdc)
        {
            ParseLdc(pLdc, &g_stVpssCfg.stGrpCfg[i].stLdcCoeff);
        }

        for(j = 0; j < IPC_MAX_STREAM_PER_PIPE; j++)
        {
            AR_CHAR chTmp[32] = {0};
            sprintf(chTmp, "channel_%d", j);
            cJSON * pChn = cJSON_GetObjectItem(pVpssGrp, chTmp);
            if(!pChn)
            {
                continue;
            }

            ParseChn(pChn, &g_stVpssCfg.stGrpCfg[i].stChnCfg[j]);
        }
    }

    cJSON_Delete(json);
    free(data);

    IPC_CFG_VPSS_LoadMaxResource();
    return 0;

err:
    IPC_CFG_VPSS_LoadDefault(enMode);
    IPC_CFG_VPSS_Save();
    IPC_CFG_VPSS_LoadMaxResource();
    return 0;

}
void IPC_CFG_VPSS_Print()
{
    int i;
    printf("********** VPSS *********\n");
    for(i = 0; i < IPC_MAX_VPSS_GRP_NEEDED; i ++)
    {
        printf("vpss grp %d basic:\n", i);
        CfgPrintMap(vpssGrpMiscMap[i]);
        printf("\n");
    }
    printf("********** VPSS *********\n\n");

}

//by default, we only enable vpss grp 0
//for each grp, we set:
//ch0: 1920 x 1080
//ch1: 1280 x 720
//ch2: 640 x 480
AR_S32 IPC_CFG_VPSS_LoadChnDefault(AR_U32 u32Grp)
{
    g_stVpssCfg.stGrpCfg[u32Grp].stChnCfg[0].bEnable = 1;
    g_stVpssCfg.stGrpCfg[u32Grp].stChnCfg[0].u32Width = 1920;
    g_stVpssCfg.stGrpCfg[u32Grp].stChnCfg[0].u32Height = 1080;

    g_stVpssCfg.stGrpCfg[u32Grp].stChnCfg[1].bEnable = 1;
    g_stVpssCfg.stGrpCfg[u32Grp].stChnCfg[1].u32Width = 640;
    g_stVpssCfg.stGrpCfg[u32Grp].stChnCfg[1].u32Height = 480;

    g_stVpssCfg.stGrpCfg[u32Grp].stChnCfg[2].bEnable = 0;
    g_stVpssCfg.stGrpCfg[u32Grp].stChnCfg[2].u32Width = 320;
    g_stVpssCfg.stGrpCfg[u32Grp].stChnCfg[2].u32Height = 240;

    //ch3 to VO
    g_stVpssCfg.stGrpCfg[u32Grp].stChnCfg[3].bEnable = 1;
    g_stVpssCfg.stGrpCfg[u32Grp].stChnCfg[3].u32Width = 1920;
    g_stVpssCfg.stGrpCfg[u32Grp].stChnCfg[3].u32Height = 1080;

    //ch4 for alg
    g_stVpssCfg.stGrpCfg[u32Grp].stChnCfg[4].bEnable = 0;
    g_stVpssCfg.stGrpCfg[u32Grp].stChnCfg[4].u32Width = 300;
    g_stVpssCfg.stGrpCfg[u32Grp].stChnCfg[4].u32Height = 300;

    return 0;
}

void IPC_CFG_VPSS_SetSingle307_LowBw()
{
    IPC_CFG_VPSS_S * pVpssCfg = IPC_CFG_VPSS_GetParam();

    //vi main channel use vpss grp 0
    pVpssCfg->stGrpCfg[0].bGroup = 1;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].bEnable = 1;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].u32Width = 1920;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].u32Height = 1080;
    pVpssCfg->stGrpCfg[0].stChnCfg[1].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[2].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[3].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[4].bEnable = 0;

    //vi sub channel use vpss grp 1
    pVpssCfg->stGrpCfg[1].bGroup = 1;
    pVpssCfg->stGrpCfg[1].stChnCfg[0].bEnable = 1; //to venc1
    pVpssCfg->stGrpCfg[1].stChnCfg[0].u32Width = 640;
    pVpssCfg->stGrpCfg[1].stChnCfg[0].u32Height = 480;
    pVpssCfg->stGrpCfg[1].stChnCfg[1].bEnable = 0; //to venc2
    pVpssCfg->stGrpCfg[1].stChnCfg[1].u32Width = 320;
    pVpssCfg->stGrpCfg[1].stChnCfg[1].u32Height = 240;
    pVpssCfg->stGrpCfg[1].stChnCfg[2].bEnable = 1; //to vo
    pVpssCfg->stGrpCfg[1].stChnCfg[2].u32Width = 640;
    pVpssCfg->stGrpCfg[1].stChnCfg[2].u32Height = 480;
    pVpssCfg->stGrpCfg[1].stChnCfg[3].bEnable = 0;
}

void IPC_CFG_VPSS_SetDual307_LowBw()
{
    IPC_CFG_VPSS_S * pVpssCfg = IPC_CFG_VPSS_GetParam();

    //vi main channel use vpss grp 0
    pVpssCfg->stGrpCfg[0].bGroup = 1;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].bEnable = 1;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].u32Width = 1920;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].u32Height = 1080;
    pVpssCfg->stGrpCfg[0].stChnCfg[1].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[2].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[3].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[4].bEnable = 0;

    //vi sub channel use vpss grp 1
    pVpssCfg->stGrpCfg[1].bGroup = 1;
    pVpssCfg->stGrpCfg[1].stChnCfg[0].bEnable = 1; //to venc1
    pVpssCfg->stGrpCfg[1].stChnCfg[0].u32Width = 640;
    pVpssCfg->stGrpCfg[1].stChnCfg[0].u32Height = 480;
    pVpssCfg->stGrpCfg[1].stChnCfg[1].bEnable = 0; //to venc2
    pVpssCfg->stGrpCfg[1].stChnCfg[1].u32Width = 320;
    pVpssCfg->stGrpCfg[1].stChnCfg[1].u32Height = 240;
    pVpssCfg->stGrpCfg[1].stChnCfg[2].bEnable = 1; //to vo
    pVpssCfg->stGrpCfg[1].stChnCfg[2].u32Width = 640;
    pVpssCfg->stGrpCfg[1].stChnCfg[2].u32Height = 480;
    pVpssCfg->stGrpCfg[1].stChnCfg[3].bEnable = 0;

     //vi 1 main channel use vpss grp 0
    pVpssCfg->stGrpCfg[2].bGroup = 1;
    pVpssCfg->stGrpCfg[2].stChnCfg[0].bEnable = 1;
    pVpssCfg->stGrpCfg[2].stChnCfg[0].u32Width = 1920;
    pVpssCfg->stGrpCfg[2].stChnCfg[0].u32Height = 1080;
    pVpssCfg->stGrpCfg[2].stChnCfg[1].bEnable = 0;
    pVpssCfg->stGrpCfg[2].stChnCfg[2].bEnable = 0;
    pVpssCfg->stGrpCfg[2].stChnCfg[3].bEnable = 0;
    pVpssCfg->stGrpCfg[2].stChnCfg[4].bEnable = 0;

    //vi 1 sub channel use vpss grp 1
    pVpssCfg->stGrpCfg[3].bGroup = 1;
    pVpssCfg->stGrpCfg[3].stChnCfg[0].bEnable = 1; //to venc1
    pVpssCfg->stGrpCfg[3].stChnCfg[0].u32Width = 640;
    pVpssCfg->stGrpCfg[3].stChnCfg[0].u32Height = 480;
    pVpssCfg->stGrpCfg[3].stChnCfg[1].bEnable = 0; //to venc2
    pVpssCfg->stGrpCfg[3].stChnCfg[1].u32Width = 320;
    pVpssCfg->stGrpCfg[3].stChnCfg[1].u32Height = 240;
    pVpssCfg->stGrpCfg[3].stChnCfg[2].bEnable = 1; //to vo
    pVpssCfg->stGrpCfg[3].stChnCfg[2].u32Width = 640;
    pVpssCfg->stGrpCfg[3].stChnCfg[2].u32Height = 480;
    pVpssCfg->stGrpCfg[3].stChnCfg[3].bEnable = 0;
}

void IPC_CFG_VPSS_SetFour307_LowBw()
{
    IPC_CFG_VPSS_S * pVpssCfg = IPC_CFG_VPSS_GetParam();

    //vi main channel use vpss grp 0
    pVpssCfg->stGrpCfg[0].bGroup = 1;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].bEnable = 1;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].u32Width = 1920;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].u32Height = 1080;
    pVpssCfg->stGrpCfg[0].stChnCfg[1].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[2].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[3].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[4].bEnable = 0;

    //vi sub channel use vpss grp 1
    pVpssCfg->stGrpCfg[1].bGroup = 1;
    pVpssCfg->stGrpCfg[1].stChnCfg[0].bEnable = 1; //to venc1
    pVpssCfg->stGrpCfg[1].stChnCfg[0].u32Width = 640;
    pVpssCfg->stGrpCfg[1].stChnCfg[0].u32Height = 480;
    pVpssCfg->stGrpCfg[1].stChnCfg[1].bEnable = 0; //to venc2
    pVpssCfg->stGrpCfg[1].stChnCfg[1].u32Width = 320;
    pVpssCfg->stGrpCfg[1].stChnCfg[1].u32Height = 240;
    pVpssCfg->stGrpCfg[1].stChnCfg[2].bEnable = 1; //to vo
    pVpssCfg->stGrpCfg[1].stChnCfg[2].u32Width = 640;
    pVpssCfg->stGrpCfg[1].stChnCfg[2].u32Height = 480;
    pVpssCfg->stGrpCfg[1].stChnCfg[3].bEnable = 0;

     //vi 1 main channel use vpss grp 2
    pVpssCfg->stGrpCfg[2].bGroup = 1;
    pVpssCfg->stGrpCfg[2].stChnCfg[0].bEnable = 1;
    pVpssCfg->stGrpCfg[2].stChnCfg[0].u32Width = 1920;
    pVpssCfg->stGrpCfg[2].stChnCfg[0].u32Height = 1080;
    pVpssCfg->stGrpCfg[2].stChnCfg[1].bEnable = 0;
    pVpssCfg->stGrpCfg[2].stChnCfg[2].bEnable = 0;
    pVpssCfg->stGrpCfg[2].stChnCfg[3].bEnable = 0;
    pVpssCfg->stGrpCfg[2].stChnCfg[4].bEnable = 0;

    //vi 1 sub channel use vpss grp 3
    pVpssCfg->stGrpCfg[3].bGroup = 1;
    pVpssCfg->stGrpCfg[3].stChnCfg[0].bEnable = 1; //to venc1
    pVpssCfg->stGrpCfg[3].stChnCfg[0].u32Width = 640;
    pVpssCfg->stGrpCfg[3].stChnCfg[0].u32Height = 480;
    pVpssCfg->stGrpCfg[3].stChnCfg[1].bEnable = 0; //to venc2
    pVpssCfg->stGrpCfg[3].stChnCfg[1].u32Width = 320;
    pVpssCfg->stGrpCfg[3].stChnCfg[1].u32Height = 240;
    pVpssCfg->stGrpCfg[3].stChnCfg[2].bEnable = 1; //to vo
    pVpssCfg->stGrpCfg[3].stChnCfg[2].u32Width = 640;
    pVpssCfg->stGrpCfg[3].stChnCfg[2].u32Height = 480;
    pVpssCfg->stGrpCfg[3].stChnCfg[3].bEnable = 0;

    //vi 2 main channel use vpss grp 4
    pVpssCfg->stGrpCfg[4].bGroup = 1;
    pVpssCfg->stGrpCfg[4].stChnCfg[0].bEnable = 1;
    pVpssCfg->stGrpCfg[4].stChnCfg[0].u32Width = 1920;
    pVpssCfg->stGrpCfg[4].stChnCfg[0].u32Height = 1080;
    pVpssCfg->stGrpCfg[4].stChnCfg[1].bEnable = 0;
    pVpssCfg->stGrpCfg[4].stChnCfg[2].bEnable = 0;
    pVpssCfg->stGrpCfg[4].stChnCfg[3].bEnable = 0;
    pVpssCfg->stGrpCfg[4].stChnCfg[4].bEnable = 0;

    //vi 2 sub channel use vpss grp 5
    pVpssCfg->stGrpCfg[5].bGroup = 1;
    pVpssCfg->stGrpCfg[5].stChnCfg[0].bEnable = 1; //to venc1
    pVpssCfg->stGrpCfg[5].stChnCfg[0].u32Width = 640;
    pVpssCfg->stGrpCfg[5].stChnCfg[0].u32Height = 480;
    pVpssCfg->stGrpCfg[5].stChnCfg[1].bEnable = 0; //to venc2
    pVpssCfg->stGrpCfg[5].stChnCfg[1].u32Width = 320;
    pVpssCfg->stGrpCfg[5].stChnCfg[1].u32Height = 240;
    pVpssCfg->stGrpCfg[5].stChnCfg[2].bEnable = 1; //to vo
    pVpssCfg->stGrpCfg[5].stChnCfg[2].u32Width = 640;
    pVpssCfg->stGrpCfg[5].stChnCfg[2].u32Height = 480;
    pVpssCfg->stGrpCfg[5].stChnCfg[3].bEnable = 0;

    //vi 3 main channel use vpss grp 4
    pVpssCfg->stGrpCfg[6].bGroup = 1;
    pVpssCfg->stGrpCfg[6].stChnCfg[0].bEnable = 1;
    pVpssCfg->stGrpCfg[6].stChnCfg[0].u32Width = 1920;
    pVpssCfg->stGrpCfg[6].stChnCfg[0].u32Height = 1080;
    pVpssCfg->stGrpCfg[6].stChnCfg[1].bEnable = 0;
    pVpssCfg->stGrpCfg[6].stChnCfg[2].bEnable = 0;
    pVpssCfg->stGrpCfg[6].stChnCfg[3].bEnable = 0;
    pVpssCfg->stGrpCfg[6].stChnCfg[4].bEnable = 0;

    //vi 3 sub channel use vpss grp 5
    pVpssCfg->stGrpCfg[7].bGroup = 1;
    pVpssCfg->stGrpCfg[7].stChnCfg[0].bEnable = 1; //to venc1
    pVpssCfg->stGrpCfg[7].stChnCfg[0].u32Width = 640;
    pVpssCfg->stGrpCfg[7].stChnCfg[0].u32Height = 480;
    pVpssCfg->stGrpCfg[7].stChnCfg[1].bEnable = 0; //to venc2
    pVpssCfg->stGrpCfg[7].stChnCfg[1].u32Width = 320;
    pVpssCfg->stGrpCfg[7].stChnCfg[1].u32Height = 240;
    pVpssCfg->stGrpCfg[7].stChnCfg[2].bEnable = 1; //to vo
    pVpssCfg->stGrpCfg[7].stChnCfg[2].u32Width = 640;
    pVpssCfg->stGrpCfg[7].stChnCfg[2].u32Height = 480;
    pVpssCfg->stGrpCfg[7].stChnCfg[3].bEnable = 0;
}

void IPC_CFG_VPSS_SetSingle464_LowBw()
{
    IPC_CFG_VPSS_S * pVpssCfg = IPC_CFG_VPSS_GetParam();

    //vi main channel use vpss grp 0
    pVpssCfg->stGrpCfg[0].bGroup = 1;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].bEnable = 1;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].u32Width = 2688;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].u32Height = 1520;
    pVpssCfg->stGrpCfg[0].stChnCfg[1].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[2].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[3].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[4].bEnable = 0;

    //vi sub channel use vpss grp 1
    pVpssCfg->stGrpCfg[1].bGroup = 1;
    pVpssCfg->stGrpCfg[1].stChnCfg[0].bEnable = 1; //to venc1
    pVpssCfg->stGrpCfg[1].stChnCfg[0].u32Width = 1920;
    pVpssCfg->stGrpCfg[1].stChnCfg[0].u32Height = 1080;
    pVpssCfg->stGrpCfg[1].stChnCfg[1].bEnable = 0; //to venc2
    pVpssCfg->stGrpCfg[1].stChnCfg[1].u32Width = 640;
    pVpssCfg->stGrpCfg[1].stChnCfg[1].u32Height = 480;
    pVpssCfg->stGrpCfg[1].stChnCfg[2].bEnable = 1; //to vo
    pVpssCfg->stGrpCfg[1].stChnCfg[2].u32Width = 1920;
    pVpssCfg->stGrpCfg[1].stChnCfg[2].u32Height = 1080;
    pVpssCfg->stGrpCfg[1].stChnCfg[3].bEnable = 0;
}

void IPC_CFG_VPSS_SetSingle415_LowBw()
{
    IPC_CFG_VPSS_S * pVpssCfg = IPC_CFG_VPSS_GetParam();

    //vi main channel use vpss grp 0
    pVpssCfg->stGrpCfg[0].bGroup = 1;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].bEnable = 1;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].u32Width = 3840;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].u32Height = 2160;
    pVpssCfg->stGrpCfg[0].stChnCfg[1].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[2].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[3].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[4].bEnable = 0;

    //vi sub channel use vpss grp 1
    pVpssCfg->stGrpCfg[1].bGroup = 1;
    pVpssCfg->stGrpCfg[1].stChnCfg[0].bEnable = 1; //to venc1
    pVpssCfg->stGrpCfg[1].stChnCfg[0].u32Width = 1920;
    pVpssCfg->stGrpCfg[1].stChnCfg[0].u32Height = 1080;
    pVpssCfg->stGrpCfg[1].stChnCfg[1].bEnable = 0; //to venc2
    pVpssCfg->stGrpCfg[1].stChnCfg[1].u32Width = 640;
    pVpssCfg->stGrpCfg[1].stChnCfg[1].u32Height = 480;
    pVpssCfg->stGrpCfg[1].stChnCfg[2].bEnable = 1; //to vo
    pVpssCfg->stGrpCfg[1].stChnCfg[2].u32Width = 1920;
    pVpssCfg->stGrpCfg[1].stChnCfg[2].u32Height = 1080;
    pVpssCfg->stGrpCfg[1].stChnCfg[3].bEnable = 0;
}

//To be implemented later
void IPC_CFG_VPSS_SetDvp8x1080P_LowBw()
{
    int i = 0;
    AR_U32 VpssGrp = 0;
    IPC_CFG_VPSS_S * pVpssCfg = IPC_CFG_VPSS_GetParam();

    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        VpssGrp = i * 2;
        pVpssCfg->stGrpCfg[VpssGrp].bGroup = 1;
        pVpssCfg->stGrpCfg[VpssGrp].stChnCfg[0].bEnable = 1;//venc0
        pVpssCfg->stGrpCfg[VpssGrp].stChnCfg[1].bEnable = 0;
        pVpssCfg->stGrpCfg[VpssGrp].stChnCfg[2].bEnable = 0;
        pVpssCfg->stGrpCfg[VpssGrp].stChnCfg[3].bEnable = 0;
        pVpssCfg->stGrpCfg[VpssGrp].stChnCfg[4].bEnable = 0;

        VpssGrp += 1;
        pVpssCfg->stGrpCfg[VpssGrp].bGroup = 1;
        pVpssCfg->stGrpCfg[VpssGrp].stChnCfg[0].bEnable = 1;//venc1
        pVpssCfg->stGrpCfg[VpssGrp].stChnCfg[0].u32Width = 640;
        pVpssCfg->stGrpCfg[VpssGrp].stChnCfg[0].u32Height = 480;
        pVpssCfg->stGrpCfg[VpssGrp].stChnCfg[2].bEnable = 1;//vo
        pVpssCfg->stGrpCfg[VpssGrp].stChnCfg[2].u32Width = 640;
        pVpssCfg->stGrpCfg[VpssGrp].stChnCfg[2].u32Height = 480;
        pVpssCfg->stGrpCfg[VpssGrp].stChnCfg[1].bEnable = 0;
        pVpssCfg->stGrpCfg[VpssGrp].stChnCfg[3].bEnable = 0;
        pVpssCfg->stGrpCfg[VpssGrp].stChnCfg[4].bEnable = 0;
    }
}

void IPC_CFG_VPSS_SetDual307()
{
    IPC_CFG_VPSS_S * pVpssCfg = IPC_CFG_VPSS_GetParam();

    memcpy(&pVpssCfg->stGrpCfg[1], &pVpssCfg->stGrpCfg[0], sizeof(pVpssCfg->stGrpCfg[0]));
}

void IPC_CFG_VPSS_SetFour307()
{
    IPC_CFG_VPSS_S * pVpssCfg = IPC_CFG_VPSS_GetParam();

    memcpy(&pVpssCfg->stGrpCfg[1], &pVpssCfg->stGrpCfg[0], sizeof(pVpssCfg->stGrpCfg[0]));
    memcpy(&pVpssCfg->stGrpCfg[2], &pVpssCfg->stGrpCfg[0], sizeof(pVpssCfg->stGrpCfg[0]));
    memcpy(&pVpssCfg->stGrpCfg[3], &pVpssCfg->stGrpCfg[0], sizeof(pVpssCfg->stGrpCfg[0]));
}

void IPC_CFG_VPSS_SetSingle464()
{
    IPC_CFG_VPSS_S * pVpssCfg = IPC_CFG_VPSS_GetParam();

    pVpssCfg->stGrpCfg[0].stChnCfg[0].u32Width = 2688;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].u32Height = 1520;
    pVpssCfg->stGrpCfg[0].stChnCfg[1].u32Width = 1920;
    pVpssCfg->stGrpCfg[0].stChnCfg[1].u32Height = 1080;

    pVpssCfg->stGrpCfg[0].stChnCfg[3].u32Width = 2688;
    pVpssCfg->stGrpCfg[0].stChnCfg[3].u32Height = 1520;
}

void IPC_CFG_VPSS_SetSingle415()
{
    IPC_CFG_VPSS_S * pVpssCfg = IPC_CFG_VPSS_GetParam();

    pVpssCfg->stGrpCfg[0].stChnCfg[0].u32Width = 3840;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].u32Height = 2160;
    pVpssCfg->stGrpCfg[0].stChnCfg[1].u32Width = 1920;
    pVpssCfg->stGrpCfg[0].stChnCfg[1].u32Height = 1080;

    pVpssCfg->stGrpCfg[0].stChnCfg[3].u32Width = 3840;
    pVpssCfg->stGrpCfg[0].stChnCfg[3].u32Height = 2160;
}

void IPC_CFG_VPSS_SetSingle307_ArIpc()
{
    IPC_CFG_VPSS_S * pVpssCfg = IPC_CFG_VPSS_GetParam();

    //vi main channel use vpss grp 0
    pVpssCfg->stGrpCfg[0].bGroup = 1;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].bEnable = 1;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].u32Width = 1920;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].u32Height = 1080;
    pVpssCfg->stGrpCfg[0].stChnCfg[1].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[2].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[3].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[4].bEnable = 0;

    //vi sub channel use vpss grp 1
    pVpssCfg->stGrpCfg[1].bGroup = 1;
    pVpssCfg->stGrpCfg[1].stChnCfg[0].bEnable = 1; //to venc1
    pVpssCfg->stGrpCfg[1].stChnCfg[0].u32Width = 640;
    pVpssCfg->stGrpCfg[1].stChnCfg[0].u32Height = 480;
    pVpssCfg->stGrpCfg[1].stChnCfg[1].bEnable = 0; //to venc2
    pVpssCfg->stGrpCfg[1].stChnCfg[1].u32Width = 320;
    pVpssCfg->stGrpCfg[1].stChnCfg[1].u32Height = 240;
    pVpssCfg->stGrpCfg[1].stChnCfg[2].bEnable = 0; //to vo
    pVpssCfg->stGrpCfg[1].stChnCfg[2].u32Width = 640;
    pVpssCfg->stGrpCfg[1].stChnCfg[2].u32Height = 480;
    pVpssCfg->stGrpCfg[1].stChnCfg[3].bEnable = 0;
}


void IPC_CFG_VPSS_SetSingle415_ArIpc()
{
    IPC_CFG_VPSS_S * pVpssCfg = IPC_CFG_VPSS_GetParam();

    //vi main channel use vpss grp 0
    pVpssCfg->stGrpCfg[0].bGroup = 1;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].bEnable = 1;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].u32Width = 3840;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].u32Height = 2160;
    pVpssCfg->stGrpCfg[0].stChnCfg[1].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[2].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[3].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[4].bEnable = 0;

    //vi sub channel use vpss grp 1
    pVpssCfg->stGrpCfg[1].bGroup = 1;
    pVpssCfg->stGrpCfg[1].stChnCfg[0].bEnable = 1; //to venc1
    pVpssCfg->stGrpCfg[1].stChnCfg[0].u32Width = 1920;
    pVpssCfg->stGrpCfg[1].stChnCfg[0].u32Height = 1080;
    pVpssCfg->stGrpCfg[1].stChnCfg[1].bEnable = 0; //to venc2
    pVpssCfg->stGrpCfg[1].stChnCfg[1].u32Width = 640;
    pVpssCfg->stGrpCfg[1].stChnCfg[1].u32Height = 480;
    pVpssCfg->stGrpCfg[1].stChnCfg[2].bEnable = 0; //to vo
    pVpssCfg->stGrpCfg[1].stChnCfg[2].u32Width = 1920;
    pVpssCfg->stGrpCfg[1].stChnCfg[2].u32Height = 1080;
    pVpssCfg->stGrpCfg[1].stChnCfg[3].bEnable = 0;
}

void IPC_CFG_VPSS_SetSingle464_ArIpc()
{
    IPC_CFG_VPSS_S * pVpssCfg = IPC_CFG_VPSS_GetParam();

    //vi main channel use vpss grp 0
    pVpssCfg->stGrpCfg[0].bGroup = 1;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].bEnable = 1;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].u32Width = 2688;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].u32Height = 1520;
    pVpssCfg->stGrpCfg[0].stChnCfg[1].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[2].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[3].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[4].bEnable = 0;

    //vi sub channel use vpss grp 1
    pVpssCfg->stGrpCfg[1].bGroup = 1;
    pVpssCfg->stGrpCfg[1].stChnCfg[0].bEnable = 1; //to venc1
    pVpssCfg->stGrpCfg[1].stChnCfg[0].u32Width = 1920;
    pVpssCfg->stGrpCfg[1].stChnCfg[0].u32Height = 1080;
    pVpssCfg->stGrpCfg[1].stChnCfg[1].bEnable = 0; //to venc2
    pVpssCfg->stGrpCfg[1].stChnCfg[1].u32Width = 640;
    pVpssCfg->stGrpCfg[1].stChnCfg[1].u32Height = 480;
    pVpssCfg->stGrpCfg[1].stChnCfg[2].bEnable = 0; //to vo
    pVpssCfg->stGrpCfg[1].stChnCfg[2].u32Width = 1920;
    pVpssCfg->stGrpCfg[1].stChnCfg[2].u32Height = 1080;
    pVpssCfg->stGrpCfg[1].stChnCfg[3].bEnable = 0;
}

void IPC_CFG_VPSS_SetDvp8x1080P()
{
    int i = 0;
    IPC_CFG_VPSS_S * pVpssCfg = IPC_CFG_VPSS_GetParam();

    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        pVpssCfg->stGrpCfg[i].bGroup = 1;
        pVpssCfg->stGrpCfg[i].stChnCfg[0].bEnable = 1;
        pVpssCfg->stGrpCfg[i].stChnCfg[3].bEnable = 1;
    }
}

void IPC_CFG_VPSS_SetSingle530ai()
{
    IPC_CFG_VPSS_S * pVpssCfg = IPC_CFG_VPSS_GetParam();

	pVpssCfg->stGrpCfg[0].bGroup = 1;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].u32Width = 2880;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].u32Height = 1616;

	pVpssCfg->stGrpCfg[1].bGroup = 1;
	pVpssCfg->stGrpCfg[1].stChnCfg[2].bEnable = 1;
    pVpssCfg->stGrpCfg[1].stChnCfg[2].u32Width = 2880;
    pVpssCfg->stGrpCfg[1].stChnCfg[2].u32Height = 1616;
}

void IPC_CFG_VPSS_SetSingleOV04A10()
{
    IPC_CFG_VPSS_S * pVpssCfg = IPC_CFG_VPSS_GetParam();

    //vi main channel use vpss grp 0
    pVpssCfg->stGrpCfg[0].bGroup = 1;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].bEnable = 1;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].u32Width = 2560;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].u32Height = 1440;
    pVpssCfg->stGrpCfg[0].stChnCfg[1].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[2].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[3].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[4].bEnable = 0;

    //vi sub channel use vpss grp 1
    pVpssCfg->stGrpCfg[1].bGroup = 1;
    pVpssCfg->stGrpCfg[1].stChnCfg[0].bEnable = 1; //to venc1
    pVpssCfg->stGrpCfg[1].stChnCfg[0].u32Width = 1920;
    pVpssCfg->stGrpCfg[1].stChnCfg[0].u32Height = 1080;
    pVpssCfg->stGrpCfg[1].stChnCfg[1].bEnable = 0; //to venc2
    pVpssCfg->stGrpCfg[1].stChnCfg[1].u32Width = 640;
    pVpssCfg->stGrpCfg[1].stChnCfg[1].u32Height = 480;
    pVpssCfg->stGrpCfg[1].stChnCfg[2].bEnable = 1; //to vo
    pVpssCfg->stGrpCfg[1].stChnCfg[2].u32Width = 1920;
    pVpssCfg->stGrpCfg[1].stChnCfg[2].u32Height = 1080;
    pVpssCfg->stGrpCfg[1].stChnCfg[3].bEnable = 0;
}

void IPC_CFG_VPSS_SetSingleOV04A10_ArIpc()
{
    IPC_CFG_VPSS_S * pVpssCfg = IPC_CFG_VPSS_GetParam();

    //vi main channel use vpss grp 0
    pVpssCfg->stGrpCfg[0].bGroup = 1;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].bEnable = 1;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].u32Width = 2560;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].u32Height = 1440;
    //vi sub channel use vpss grp 1
    pVpssCfg->stGrpCfg[1].bGroup = 1;
    pVpssCfg->stGrpCfg[1].stChnCfg[0].bEnable = 1; //to venc1
    pVpssCfg->stGrpCfg[1].stChnCfg[0].u32Width = 1920;
    pVpssCfg->stGrpCfg[1].stChnCfg[0].u32Height = 1080;
    pVpssCfg->stGrpCfg[1].stChnCfg[1].bEnable = 0; //to venc2
    pVpssCfg->stGrpCfg[1].stChnCfg[1].u32Width = 640;
    pVpssCfg->stGrpCfg[1].stChnCfg[1].u32Height = 480;
    pVpssCfg->stGrpCfg[1].stChnCfg[2].bEnable = 0; //to vo
    pVpssCfg->stGrpCfg[1].stChnCfg[2].u32Width = 1920;
    pVpssCfg->stGrpCfg[1].stChnCfg[2].u32Height = 1080;
    pVpssCfg->stGrpCfg[1].stChnCfg[3].bEnable = 0;
}

void IPC_CFG_VPSS_SetSingleTimo256()
{
}

void IPC_CFG_VPSS_SetSingleGst417w()
{
    IPC_CFG_VPSS_S * pVpssCfg = IPC_CFG_VPSS_GetParam();
    
    //vi main channel use vpss grp 0
    pVpssCfg->stGrpCfg[0].bGroup = 1;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].bEnable = 1;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].u32Width = 408;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].u32Height = 300;
    pVpssCfg->stGrpCfg[0].stChnCfg[1].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[2].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[3].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[4].bEnable = 0;
}
void IPC_CFG_VPSS_SetSingleGst212w4()
{
    IPC_CFG_VPSS_S * pVpssCfg = IPC_CFG_VPSS_GetParam();
    
    //vi main channel use vpss grp 0
    pVpssCfg->stGrpCfg[0].bGroup = 1;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].bEnable = 1;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].u32Width = 256;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].u32Height = 192;
    pVpssCfg->stGrpCfg[0].stChnCfg[1].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[2].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[3].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[4].bEnable = 0;
}

void IPC_CFG_VPSS_SetFussion_307_Timo256()
{
}

void IPC_CFG_VPSS_SetFussion_OV04A10_Gst417w()
{
    IPC_CFG_VPSS_S * pVpssCfg = IPC_CFG_VPSS_GetParam();
    
    //vi main channel use vpss grp 0
    pVpssCfg->stGrpCfg[0].bGroup = 1;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].bEnable = 1;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].u32Width = 2560;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].u32Height = 1440;
    pVpssCfg->stGrpCfg[0].stChnCfg[1].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[2].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[3].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[4].bEnable = 0;

    //vi sub channel use vpss grp 1
    pVpssCfg->stGrpCfg[1].bGroup = 1;
    pVpssCfg->stGrpCfg[1].stChnCfg[0].bEnable = 1; //to venc1
    pVpssCfg->stGrpCfg[1].stChnCfg[0].u32Width = 1920;
    pVpssCfg->stGrpCfg[1].stChnCfg[0].u32Height = 1080;
    pVpssCfg->stGrpCfg[1].stChnCfg[1].bEnable = 0; //to venc2
    pVpssCfg->stGrpCfg[1].stChnCfg[1].u32Width = 640;
    pVpssCfg->stGrpCfg[1].stChnCfg[1].u32Height = 480;
    pVpssCfg->stGrpCfg[1].stChnCfg[2].bEnable = 1; //to vo
    pVpssCfg->stGrpCfg[1].stChnCfg[2].u32Width = 1920;
    pVpssCfg->stGrpCfg[1].stChnCfg[2].u32Height = 1080;
    pVpssCfg->stGrpCfg[1].stChnCfg[3].bEnable = 1; //for fussion of rgb + ir
    pVpssCfg->stGrpCfg[1].stChnCfg[3].u32Width = 960;
    pVpssCfg->stGrpCfg[1].stChnCfg[3].u32Height = 540;
    
    pVpssCfg->stGrpCfg[2].bGroup = 1;
    pVpssCfg->stGrpCfg[2].stChnCfg[0].bEnable = 1;
    pVpssCfg->stGrpCfg[2].stChnCfg[0].u32Width = 408;
    pVpssCfg->stGrpCfg[2].stChnCfg[0].u32Height = 300;
    pVpssCfg->stGrpCfg[2].stChnCfg[1].bEnable = 0;
    pVpssCfg->stGrpCfg[2].stChnCfg[2].bEnable = 0;
    pVpssCfg->stGrpCfg[2].stChnCfg[3].bEnable = 0;
    pVpssCfg->stGrpCfg[2].stChnCfg[4].bEnable = 0;
    
    pVpssCfg->stGrpCfg[2].stChnCfg[0].stCrop.bEnable = 1;
    pVpssCfg->stGrpCfg[2].stChnCfg[0].stCrop.stRect.u32X = 0;
    pVpssCfg->stGrpCfg[2].stChnCfg[0].stCrop.stRect.u32Y = 0;
    pVpssCfg->stGrpCfg[2].stChnCfg[0].stCrop.stRect.u32W = 400;
    pVpssCfg->stGrpCfg[2].stChnCfg[0].stCrop.stRect.u32H = 300;
}

void IPC_CFG_VPSS_SetFussion_OV04A10_Gst212w()
{
    IPC_CFG_VPSS_S * pVpssCfg = IPC_CFG_VPSS_GetParam();
    
    //vi main channel use vpss grp 0
    pVpssCfg->stGrpCfg[0].bGroup = 1;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].bEnable = 1;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].u32Width = 2560;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].u32Height = 1440;
    pVpssCfg->stGrpCfg[0].stChnCfg[1].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[2].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[3].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[4].bEnable = 0;

    //vi sub channel use vpss grp 1
    pVpssCfg->stGrpCfg[1].bGroup = 1;
    pVpssCfg->stGrpCfg[1].stChnCfg[0].bEnable = 1; //to venc1
    pVpssCfg->stGrpCfg[1].stChnCfg[0].u32Width = 1920;
    pVpssCfg->stGrpCfg[1].stChnCfg[0].u32Height = 1080;
    pVpssCfg->stGrpCfg[1].stChnCfg[1].bEnable = 0; //to venc2
    pVpssCfg->stGrpCfg[1].stChnCfg[1].u32Width = 640;
    pVpssCfg->stGrpCfg[1].stChnCfg[1].u32Height = 480;
    pVpssCfg->stGrpCfg[1].stChnCfg[2].bEnable = 1; //to vo
    pVpssCfg->stGrpCfg[1].stChnCfg[2].u32Width = 1920;
    pVpssCfg->stGrpCfg[1].stChnCfg[2].u32Height = 1080;
    pVpssCfg->stGrpCfg[1].stChnCfg[3].bEnable = 0;
    
    pVpssCfg->stGrpCfg[2].bGroup = 1;
    pVpssCfg->stGrpCfg[2].stChnCfg[0].bEnable = 1;
    pVpssCfg->stGrpCfg[2].stChnCfg[0].u32Width = 256;
    pVpssCfg->stGrpCfg[2].stChnCfg[0].u32Height = 192;
    pVpssCfg->stGrpCfg[2].stChnCfg[1].bEnable = 0;
    pVpssCfg->stGrpCfg[2].stChnCfg[2].bEnable = 0;
    pVpssCfg->stGrpCfg[2].stChnCfg[3].bEnable = 0;
    pVpssCfg->stGrpCfg[2].stChnCfg[4].bEnable = 0;
    
}

void IPC_CFG_VPSS_SetSingleOV05A20()
{
    IPC_CFG_VPSS_S * pVpssCfg = IPC_CFG_VPSS_GetParam();

    //vi main channel use vpss grp 0
    pVpssCfg->stGrpCfg[0].bGroup = 1;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].bEnable = 1;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].u32Width = 2688;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].u32Height = 1944;
    pVpssCfg->stGrpCfg[0].stChnCfg[1].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[2].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[3].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[4].bEnable = 0;

    //vi sub channel use vpss grp 1
    pVpssCfg->stGrpCfg[1].bGroup = 1;
    pVpssCfg->stGrpCfg[1].stChnCfg[0].bEnable = 1; //to venc1
    pVpssCfg->stGrpCfg[1].stChnCfg[0].u32Width = 1920;
    pVpssCfg->stGrpCfg[1].stChnCfg[0].u32Height = 1080;
    pVpssCfg->stGrpCfg[1].stChnCfg[1].bEnable = 0; //to venc2
    pVpssCfg->stGrpCfg[1].stChnCfg[1].u32Width = 640;
    pVpssCfg->stGrpCfg[1].stChnCfg[1].u32Height = 480;
    pVpssCfg->stGrpCfg[1].stChnCfg[2].bEnable = 1; //to vo
    pVpssCfg->stGrpCfg[1].stChnCfg[2].u32Width = 1920;
    pVpssCfg->stGrpCfg[1].stChnCfg[2].u32Height = 1080;
    pVpssCfg->stGrpCfg[1].stChnCfg[3].bEnable = 0;
}

void IPC_CFG_VPSS_SetFussion_OV05A20_Gst212w()
{
    IPC_CFG_VPSS_S * pVpssCfg = IPC_CFG_VPSS_GetParam();
    
    //vi main channel use vpss grp 0
    pVpssCfg->stGrpCfg[0].bGroup = 1;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].bEnable = 1;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].u32Width = 2688;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].u32Height = 1944;
    pVpssCfg->stGrpCfg[0].stChnCfg[1].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[2].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[3].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[4].bEnable = 0;

    //vi sub channel use vpss grp 1
    pVpssCfg->stGrpCfg[1].bGroup = 1;
    pVpssCfg->stGrpCfg[1].stChnCfg[0].bEnable = 1; //to venc1
    pVpssCfg->stGrpCfg[1].stChnCfg[0].u32Width = 1920;
    pVpssCfg->stGrpCfg[1].stChnCfg[0].u32Height = 1080;
    pVpssCfg->stGrpCfg[1].stChnCfg[1].bEnable = 0; //to venc2
    pVpssCfg->stGrpCfg[1].stChnCfg[1].u32Width = 640;
    pVpssCfg->stGrpCfg[1].stChnCfg[1].u32Height = 480;
    pVpssCfg->stGrpCfg[1].stChnCfg[2].bEnable = 1; //to vo
    pVpssCfg->stGrpCfg[1].stChnCfg[2].u32Width = 1920;
    pVpssCfg->stGrpCfg[1].stChnCfg[2].u32Height = 1080;
    pVpssCfg->stGrpCfg[1].stChnCfg[3].bEnable = 0;
    
    pVpssCfg->stGrpCfg[2].bGroup = 1;
    pVpssCfg->stGrpCfg[2].stChnCfg[0].bEnable = 1;
    pVpssCfg->stGrpCfg[2].stChnCfg[0].u32Width = 256;
    pVpssCfg->stGrpCfg[2].stChnCfg[0].u32Height = 192;
    pVpssCfg->stGrpCfg[2].stChnCfg[1].bEnable = 0;
    pVpssCfg->stGrpCfg[2].stChnCfg[2].bEnable = 0;
    pVpssCfg->stGrpCfg[2].stChnCfg[3].bEnable = 0;
    pVpssCfg->stGrpCfg[2].stChnCfg[4].bEnable = 0;
    
}

void IPC_CFG_VPSS_SetSingleH3812c1sg()
{
    IPC_CFG_VPSS_S * pVpssCfg = IPC_CFG_VPSS_GetParam();
    
    //vi main channel use vpss grp 0
    pVpssCfg->stGrpCfg[0].bGroup = 1;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].bEnable = 1;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].u32Width = 384;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].u32Height = 288;
    pVpssCfg->stGrpCfg[0].stChnCfg[1].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[2].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[3].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[4].bEnable = 0;
}

void IPC_CFG_VPSS_SetFussion_307_H3812c1sg()
{
    IPC_CFG_VPSS_S * pVpssCfg = IPC_CFG_VPSS_GetParam();
    
    //vi main channel use vpss grp 0
    pVpssCfg->stGrpCfg[0].bGroup = 1;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].bEnable = 1;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].u32Width = 1920;
    pVpssCfg->stGrpCfg[0].stChnCfg[0].u32Height = 1080;
    pVpssCfg->stGrpCfg[0].stChnCfg[1].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[2].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[3].bEnable = 0;
    pVpssCfg->stGrpCfg[0].stChnCfg[4].bEnable = 0;
    
    //vi sub channel use vpss grp 1
    pVpssCfg->stGrpCfg[1].bGroup = 1;
    pVpssCfg->stGrpCfg[1].stChnCfg[0].bEnable = 1; //to venc1
    pVpssCfg->stGrpCfg[1].stChnCfg[0].u32Width = 640;
    pVpssCfg->stGrpCfg[1].stChnCfg[0].u32Height = 480;
    pVpssCfg->stGrpCfg[1].stChnCfg[1].bEnable = 0; //to venc2
    pVpssCfg->stGrpCfg[1].stChnCfg[1].u32Width = 640;
    pVpssCfg->stGrpCfg[1].stChnCfg[1].u32Height = 480;
    pVpssCfg->stGrpCfg[1].stChnCfg[2].bEnable = 1; //to vo
    pVpssCfg->stGrpCfg[1].stChnCfg[2].u32Width = 640;
    pVpssCfg->stGrpCfg[1].stChnCfg[2].u32Height = 480;
    pVpssCfg->stGrpCfg[1].stChnCfg[3].bEnable = 0;
    
    pVpssCfg->stGrpCfg[2].bGroup = 1;
    pVpssCfg->stGrpCfg[2].stChnCfg[0].bEnable = 1;
    pVpssCfg->stGrpCfg[2].stChnCfg[0].u32Width = 384;
    pVpssCfg->stGrpCfg[2].stChnCfg[0].u32Height = 288;
    pVpssCfg->stGrpCfg[2].stChnCfg[1].bEnable = 0;
    pVpssCfg->stGrpCfg[2].stChnCfg[2].bEnable = 0;
    pVpssCfg->stGrpCfg[2].stChnCfg[3].bEnable = 0;
    pVpssCfg->stGrpCfg[2].stChnCfg[4].bEnable = 0;
    
}

AR_S32 IPC_CFG_VPSS_LoadMaxResource()
{
    AR_S32 i = 0, j = 0;
    IPC_CFG_VPSS_S * pVpssCfg = IPC_CFG_VPSS_GetParam();
    
    for(i = 0; i < IPC_MAX_VPSS_GRP_NEEDED; i++)
    {
        for(j = 0; j < IPC_MAX_STREAM_PER_PIPE; j++)
        {
           g_stVpssMaxResource.stVpssMaxResource[i][j].u32Width = pVpssCfg->stGrpCfg[i].stChnCfg[j].u32Width;
           g_stVpssMaxResource.stVpssMaxResource[i][j].u32Height = pVpssCfg->stGrpCfg[i].stChnCfg[j].u32Height;
        }
    }
    
    return 0;
}


AR_S32 IPC_CFG_VPSS_LoadDefault(CFG_RUN_MODE_E enMode)
{
    int i;

    printf("Enter %s \n", __FUNCTION__);

    memset(&g_stVpssCfg, 0, sizeof(g_stVpssCfg));

    for (i = 0; i < IPC_MAX_VPSS_GRP_NEEDED; i ++)
    {
        CfgLoadDefValue(vpssGrpMiscMap[i]);
        IPC_CFG_VPSS_LoadChnDefault(i);
    }

    switch(enMode)
    {
        case CFG_RUN_MODE_SINGLE_SC230AI_LOW_BW:
        case CFG_RUN_MODE_SINGLE_307_LOW_BW:
        case CFG_RUN_MODE_SINGLE_307_HDR_LOW_BW:
        case CFG_RUN_MODE_SINGLE_307_ARS31:
        case CFG_RUN_MODE_SINGLE_307_HDR_ARS31:
            IPC_CFG_VPSS_SetSingle307_LowBw();
            break;
        case CFG_RUN_MODE_SINGLE_307_ARIPC:
        case CFG_RUN_MODE_SINGLE_307_HDR_ARIPC:
            IPC_CFG_VPSS_SetSingle307_ArIpc();
            break;
        case CFG_RUN_MODE_DUAL_SC230AI_LOW_BW:
        case CFG_RUN_MODE_DUAL_307_LOW_BW:
        case CFG_RUN_MODE_DUAL_307_ARS31:
            IPC_CFG_VPSS_SetDual307_LowBw();
            break;
        case CFG_RUN_MODE_FOUR_307_LOW_BW:
            IPC_CFG_VPSS_SetFour307_LowBw();
            break;
        case CFG_RUN_MODE_SINGLE_415_LOW_BW:
            IPC_CFG_VPSS_SetSingle415_LowBw();
            break;
        case CFG_RUN_MODE_SINGLE_415_ARIPC:
            IPC_CFG_VPSS_SetSingle415_ArIpc();
            break;
        case CFG_RUN_MODE_SINGLE_464_LOW_BW:
        case CFG_RUN_MODE_SINGLE_464_HDR_LOW_BW:
        case CFG_RUN_MODE_SINGLE_347:
            IPC_CFG_VPSS_SetSingle464_LowBw();
            break;
        case CFG_RUN_MODE_SINGLE_464_ARIPC:
        case CFG_RUN_MODE_SINGLE_464_HDR_ARIPC:
            IPC_CFG_VPSS_SetSingle464_ArIpc();
            break;
        case CFG_RUN_MODE_DVP_8X1080P_25_LOW_BW:
        case CFG_RUN_MODE_DVP_8X1080P_30_LOW_BW:
            IPC_CFG_VPSS_SetDvp8x1080P_LowBw();
            break;
        case CFG_RUN_MODE_SINGLE_530AI_30:
            IPC_CFG_VPSS_SetSingle530ai();
            break;
        case CFG_RUN_MODE_SINGLE_OV04A10_25:
        case CFG_RUN_MODE_SINGLE_OV04A10_25_HDR:
        case CFG_RUN_MODE_SINGLE_OV04A10_ARS31:
        case CFG_RUN_MODE_SINGLE_OV04A10_HDR_ARS31:
            IPC_CFG_VPSS_SetSingleOV04A10();
            break;
        case CFG_RUN_MODE_SINGLE_OV04A10_25_ARIPC:
        case CFG_RUN_MODE_SINGLE_OV04A10_25_HDR_ARIPC:
            IPC_CFG_VPSS_SetSingleOV04A10_ArIpc();
	    break;
        case CFG_RUN_MODE_SINGLE_TIMO256_ARS31:
            IPC_CFG_VPSS_SetSingleTimo256();
            break;
        case CFG_RUN_MODE_SINGLE_GST417W_ARS31:
            IPC_CFG_VPSS_SetSingleGst417w();
            break;
        case CFG_RUN_MODE_307_timo256_ARS31:
            IPC_CFG_VPSS_SetFussion_307_Timo256();
            break;
        case CFG_RUN_MODE_OV04A10_GST417W_ARS31:
            IPC_CFG_VPSS_SetFussion_OV04A10_Gst417w();
            break;
        case CFG_RUN_MODE_SINGLE_GST212W4_ARS31:
            IPC_CFG_VPSS_SetSingleGst212w4();
            break;
        case CFG_RUN_MODE_OV04A10_GST212W_ARS31:
            IPC_CFG_VPSS_SetFussion_OV04A10_Gst212w();
            break;
        case CFG_RUN_MODE_SINGLE_OV05A20_ARS31:
            IPC_CFG_VPSS_SetSingleOV05A20();
            break;
        case CFG_RUN_MODE_OV05A20_GST212W_ARS31:
            IPC_CFG_VPSS_SetFussion_OV05A20_Gst212w();
            break;
        case CFG_RUN_MODE_SINGLE_H3812C1SG_ARS31:
            IPC_CFG_VPSS_SetSingleH3812c1sg();
            break;
        case CFG_RUN_MODE_307_H3812C1SG_ARS31:
            IPC_CFG_VPSS_SetFussion_307_H3812c1sg();
            break;
        default:
            //single 307 and 307hdr
            break;
    }

    return 0;
}

IPC_CFG_VPSS_S * IPC_CFG_VPSS_GetParam()
{
    return &g_stVpssCfg;
}

IPC_VPSS_MAX_RESOURCE_S * IPC_CFG_VPSS_GetMaxResource()
{
    return &g_stVpssMaxResource;
}

