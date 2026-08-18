#ifndef __SAMPLE_VPSS_VIO_CONFIG_H__
#define __SAMPLE_VPSS_VIO_CONFIG_H__

#include "sample_comm.h"

#include "binder_ipc.h"



#define SAMPLE_VIDEO_REGION_MAX_CNT   16
typedef struct arSAMPLE_VIDEO_REGION_INFO_S
{
    AR_U32           u32RegionNum;       /* W; count of the region */
    RECT_S ATTRIBUTE pstRegion[SAMPLE_VIDEO_REGION_MAX_CNT];         /* W; region attribute */
} SAMPLE_VIDEO_REGION_INFO_S;

typedef enum arSAMPLE_VPSS_CONFIG_CMD_E{
    CMD_VPSS_CONFIG_NONE = 0,
    CMD_VPSS_CONFIG_SCALE = 1,
    CMD_VPSS_CONFIG_MIRROR,
    CMD_VPSS_CONFIG_FLIP,
    CMD_VPSS_CONFIG_CROP,
    CMD_VPSS_CONFIG_ROTATE,
    CMD_VPSS_CONFIG_REGION_LUMA,
    CMD_VPSS_CONFIG_PHY_CHN_LDC,
    CMD_VPSS_CONFIG_PHY_CHN_SPREAD,
    CMD_VPSS_CONFIG_EXT_CHN_FISHEYE,
    CMD_VPSS_START,
    CMD_VPSS_STOP,
    CMD_VPSS_QUIT,
    CMD_VPSS_CONFIG_BUTT
} SAMPLE_VPSS_CONFIG_CMD_E;

typedef struct arSAMPLE_VPSS_CONFIG_S{
    SAMPLE_VPSS_CONFIG_CMD_E    cmd;
    VPSS_GRP                    grpId;
    VPSS_CHN                    chnId;
    VPSS_GRP_ATTR_S             grpAttr;
    VPSS_CHN_ATTR_S             chnAttr;
    VPSS_EXT_CHN_ATTR_S         extChnAttr;
    VPSS_CROP_INFO_S            cropInfo;
    ROTATION_E                  rotateAngle;
    VPSS_LDC_ATTR_S             ldcAttr;
    VPSS_SPREAD_ATTR_S          spreadAttr;
    VPSS_FISHEYE_ATTR_S         fisheyeAttr;
    AR_BOOL                     startVpss;
    AR_BOOL                     stopVpss;
    AR_BOOL                     quitVpss;
    SAMPLE_VIDEO_REGION_INFO_S  regionLuma;
} SAMPLE_VPSS_CONFIG_S;


typedef struct arSAMPLE_VPSS_REPLY_S{
    char result[16];
    AR_S32 s32Ret;
} SAMPLE_VPSS_REPLY_S;
#endif /* End of #ifndef __SAMPLE_VPSS_CONFIG_H__*/
