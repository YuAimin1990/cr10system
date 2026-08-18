#ifndef __CFG_VENC_H__
#define __CFG_VENC_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "cfg_common.h"

#define MAX_VENC_ITEM_NUM 30
typedef enum
{
    VENC_ENCODE_TYPE_H264 = 1,
    VENC_ENCODE_TYPE_H265 = 2,
    VENC_ENCODE_TYPE_MJPEG = 3,
    VENC_ENCODE_TYPE_JPEG = 4
} IPC_VENC_CODEC_TYPE_E;

/*0: CBR; 1: VBR; 2: CVBR, 3: AVBR, 4: QPMAP, 5: FIXQP, 6 QVBR*/
#define VENC_BRCMODE_CBR 0
#define VENC_BRCMODE_VBR 1
#define VENC_BRCMODE_CVBR 2
#define VENC_BRCMODE_AVBR 3
#define VENC_BRCMODE_QPMAP 4
#define VENC_BRCMODE_FIXQP 5
#define VENC_BRCMODE_QVBR 6

typedef struct {
    /*encode width.*/
    AR_U16                    width;
    /*encode height.*/
    AR_U16                    height;

    AR_U16                    roi_width;
    AR_U16                    roi_height;
    AR_U16                    roi_x;
    AR_U16                    roi_y;

    /*0: none, 1: H.264, 2: H.265, 3: MJPEG, 4: JPEG*/
    IPC_VENC_CODEC_TYPE_E     encodeType;
    /*0: YUV 422, 1: YUV 420.*/
    AR_U8             chromaFormat;
    /*encode frame rate.*/
    AR_U8    fps;
    /*gop  value.*/
    AR_U8             gop;
    /*picure quality 0,3,0: poor, 3: best*/
    AR_U8             quality; /* not used for now*/
    /*0: CBR; 1: VBR; 2: CVBR, 3: AVBR, 4: QPMAP, 5: FIXQP, 6 QVBR*/
    AR_U8             brcMode;
    /*cbr mode, bit rate.*/
    AR_U32            cbrAvgBps;
    /*vbr mode, min bit rate.*/
    AR_U32            vbrMinBps;
    /*vbr mode, max bit rate.*/
    AR_U32            vbrMaxBps;
	/*rate control factor. value:0~51, qpMinI <= qpMinP.*/
    AR_U8             qpMinI;
    /*rate control factor. value:0~51, qpMinI <= qpMinP.*/
    AR_U8             qpMinP;
	AR_U8             qpMaxI;
	AR_U8             qpMaxP;
    /*rate control factor: I frame qp weight, range: 1~10,*/
    AR_U8             qpIWeight;
    /*rate control factor. P frame qp weight, range: 1~5.*/
    AR_U8             qpPWeight;
	/*picture quality consistency, range: 0~2. 2:will be best.*/
	AR_U8			   adaptQp;
	AR_U32             statTime; /*1-60*/
}VENC_PARAM_S;

typedef struct {
    AR_S32 enable;
    AR_S16 id;
    AR_S16 vi_band_id;
    VENC_PARAM_S cfg;
} VENC_STREAM_CFG_S;

typedef struct {
    VENC_STREAM_CFG_S vencStream[IPC_MAX_PIPELINE_NUM][IPC_MAX_VENC_STREAM_PER_CHN]; //ch N stream M
    VENC_STREAM_CFG_S vencStreamSnap;
} IPC_CFG_VENC_S;

typedef struct
{
    AR_U32 u32Width;
    AR_U32 u32Height;
} IPC_CFG_VENC_MAX_S;

typedef struct
{
    IPC_CFG_VENC_MAX_S stVpssMaxResource[IPC_MAX_PIPELINE_NUM][IPC_MAX_VENC_STREAM_PER_CHN];
} IPC_VENC_MAX_RESOURCE_S;


extern AR_S32 IPC_CFG_VENC_Save();
extern AR_S32 IPC_CFG_VENC_Load(CFG_RUN_MODE_E enMode);
extern AR_S32 IPC_CFG_VENC_LoadDefault(CFG_RUN_MODE_E enMode);
extern void IPC_CFG_VENC_Print();
extern IPC_CFG_VENC_S * IPC_CFG_VENC_GetParam();
extern IPC_VENC_MAX_RESOURCE_S * IPC_CFG_VENC_GetMaxResource();
#define VENC_CFG_FILE "cfg_venc.json"

#ifdef __cplusplus
}
#endif
#endif

