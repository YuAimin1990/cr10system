#ifndef __VCTRL_MSG__
#define __VCTRL_MSG__

#define VCTRL_NAME_MAX 128
#define VCTRL_VALUE_MAX 128
#define FILE_PATH_MAX 128

#include "ar_comm_video.h"

typedef enum
{
    UPDATE_BITRATE = 1 << 0,
    UPDATE_GOPSIZE = 1 << 1,
    UPDATE_RCMODE = 1 << 2,
    UPDATE_FPS = 1 << 3,
    UPDATE_QP = 1 << 4,
    SET_ROIMAP = 1 << 5,
    UPDATE_UNKNOWN
}CHN_UPDATE_E;

typedef enum
{
    UPDATE_MINIQP = 1 << 0,
    UPDATE_MAXIQP = 1 << 1,
    UPDATE_MINPQP = 1 << 2,
    UPDATE_MAXPQP = 1 << 3,
    UPDATE_MINBQP = 1 << 4,
    UPDATE_MAXBQP = 1 << 5,
    UPDATE_MINIPROP = 1 << 6,
    UPDATE_MAXIPROP = 1 << 7,
    UPDATE_FIXEDIQP = 1 << 8,
    UPDATE_FIXEDPQP = 1 << 9,
    UPDATE_FIXEDBQP = 1 << 10,
    UPDATE_FIRSTFRAMESTARTQP = 1 << 11,
    UPDATE_MINQFACTOR = 1 << 12,
    UPDATE_MAXQFACTOR = 1 << 13,
    UPDATE_RC_UNKNOWN
}RC_UPDATE_E;

typedef enum
{
    VIDEO_ENCODER = 0,
    VIDEO_DECODER = 1,
    VIDEO_UNKNOWN = 2,
} VIDEO_PROCESSOR_TYPE_E;

typedef struct vctrl_roi_map_s
{
    AR_U32 roiIndex;
    AR_U32 roiIndexEnable;
    AR_S32 roiQp;
    AR_S32 roiStartX;
    AR_S32 roiStartY;
    AR_U32 roiWidth;
    AR_U32 roiHeigth;
}ROI_MAP;

typedef struct vctrl_qp_change_s
{
    RC_UPDATE_E enRcUpate;
    AR_U32 minIQp;
    AR_U32 maxIQp;
    AR_U32 minPQp;
    AR_U32 maxPQp;
    AR_U32 minBQp;
    AR_U32 maxBQp;
    AR_U32 minIprop;
    AR_U32 maxIprop;
    AR_U32 fixedIQp;
    AR_U32 fixedPQp;
    AR_U32 fixedBQp;
    AR_S32 firstFrameStartQp;
    AR_U32 minQfactor;
    AR_U32 maxQfactor;
}QP_CHANGE;

typedef struct vctrl_send_s
{
    AR_S32 verbose;
    AR_BOOL transcoding;
    AR_CHAR yuvFile[FILE_PATH_MAX];
    AR_CHAR streamFile[FILE_PATH_MAX];
    AR_U8 chnId;
    AR_U8 transcodingChnId;
    AR_U8 u32ChnNum;
    AR_U8 preLoadPicNum;
    AR_U8 frameRate;
    AR_U8 srcframeRate;
    PAYLOAD_TYPE_E type;
    PAYLOAD_TYPE_E transcodingType;
    AR_S32 s32SaveFile;
    AR_U32 picNum;
    AR_U32 width;
    AR_U32 height;
    AR_U32 bitrate;
    AR_U32 gopsize;
    AR_U32 enRcMode;
    ROI_MAP roiMap;
    QP_CHANGE qpChange;
    CHN_UPDATE_E enUpdate;
    VIDEO_PROCESSOR_TYPE_E processor;
}VCTRL_SEND;

typedef struct vctrl_msg_s
{
    unsigned cmd;
    VCTRL_SEND send_data;
}vctrl_msg;

#ifndef ALIGN_UP
#define ALIGN_UP(x, a)           ( ( ((x) + ((a) - 1) ) / a ) * a )
#endif

#define VCTRL_MSG_START 1
#define VCTRL_MSG_STOP 2
#define VCTRL_MSG_SET 3
#define CTRL_SERVICE_NAME  "vctrl_service"
#define ARTOSYN_SOCKET_DIR "/tmp"

#ifndef TEMP_FAILURE_RETRY
#define TEMP_FAILURE_RETRY(exp)({ \
    typeof (exp) _rc;             \
    do{                           \
        _rc = (exp);              \
    } while (_rc == -1 && errno == EINTR);\
    _rc; })
#endif

#define VCTRL_PRT logline
/*
(fmt...)   \
    do {\
        printf("[%s]-%d: ", __FUNCTION__, __LINE__);\
        printf(fmt);\
    }while(0)
*/

#define CHECK_CHN_RET(express,Chn,name)\
    do{\
        AR_S32 Ret;\
        Ret = express;\
        if (AR_SUCCESS != Ret)\
        {\
            printf("\033[0;31m%s chn %d failed at %s: LINE: %d with %#x!\033[0;39m\n", name, Chn, __FUNCTION__, __LINE__, Ret);\
            fflush(stdout);\
            return Ret;\
        }\
    }while(0)

#define CHECK_RET(express,name)\
    do{\
        AR_S32 Ret;\
        Ret = express;\
        if (AR_SUCCESS != Ret)\
        {\
            printf("\033[0;31m%s failed at %s: LINE: %d with %#x!\033[0;39m\n", name, __FUNCTION__, __LINE__, Ret);\
            return Ret;\
        }\
    }while(0)


#endif//__VCTRL_MSG__
