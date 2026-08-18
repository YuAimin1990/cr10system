#ifndef __TEST_VO_COMMON_H__
#define __TEST_VO_COMMON_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define MAX_DUMP_FILE_NAME         20
#define VOTEST_RPC_CMD_BASE        0x76580000

typedef enum
{
    VOTEST_RPC_CHN_PAUSE = VOTEST_RPC_CMD_BASE,
    VOTEST_RPC_CHN_RESUME,
    VOTEST_RPC_CHN_SHOW,
    VOTEST_RPC_CHN_HIDE,
    VOTEST_RPC_CHN_STEP,
    VOTEST_RPC_LAYER_DUMP_FRAME,
    VOTEST_RPC_DEV_ENABLE,
    VOTEST_RPC_DEV_DISABLE,
    VOTEST_RPC_LAYER_ENABLE,
    VOTEST_RPC_LAYER_DISABLE,
    VOTEST_RPC_CHN_ENABLE,
    VOTEST_RPC_CHN_DISABLE,
    VOTEST_RPC_SET_DEV_ATTR,
    VOTEST_RPC_SET_LAYER_ATTR,
    VOTEST_RPC_SET_CHN_ATTR,
    VOTEST_RPC_CMD_END
} VOTEST_RPC_CMD_E;

typedef struct
{
    ENUM_AR_HAL_VO_DEV_ID     eDevId;
    ENUM_AR_HAL_VO_LAYER_ID   eLayerId;
} VOTEST_RPC_LAYER;

typedef struct
{
    ENUM_AR_HAL_VO_DEV_ID     eDevId;
    ENUM_AR_HAL_VO_LAYER_ID   eLayerId;
    AR_U32                    u32ChanId;
} VOTEST_RPC_CHANNEL;

typedef struct
{
    ENUM_AR_HAL_VO_DEV_ID     eDevId;
    ENUM_AR_HAL_VO_CSC        eDevCsc;
    STRU_AR_HAL_VO_DEV_ATTR   sDevAttr;
} VOTEST_RPC_DEV_ATTR;

typedef struct
{
    ENUM_AR_HAL_VO_DEV_ID     eDevId;
    ENUM_AR_HAL_VO_LAYER_ID   eLayerId;
    STRU_AR_HAL_VO_LAYER_CSC  sLayerCsc;
    STRU_AR_HAL_VO_POS        sLayerPos;
    STRU_AR_HAL_VO_LAYER_ATTR sLayerAttr;
} VOTEST_RPC_LAYER_ATTR;

typedef struct
{
    VOTEST_RPC_CHANNEL      sChn;
    STRU_AR_VO_CHN_PARAM_S  sChnPara;
    STRU_AR_HAL_VO_CHN_ATTR sChnAttr;
} VOTEST_RPC_CHN_ATTR;

typedef struct
{
    ENUM_AR_HAL_VO_LAYER_ID   eLayerId;
    char                      sFileName[MAX_DUMP_FILE_NAME];
} VOTEST_RPC_LAYER_DUMP;

#ifdef __cplusplus
}
#endif

#endif
