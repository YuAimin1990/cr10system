
#ifndef __AR_COMM_HDMI_H__
#define __AR_COMM_HDMI_H__

#include "hal_type.h"
#ifndef HDMI_BUILD_IN_BOOT
#include "hal_errno.h"
#endif
#include "ar_common.h"
#include "ar_comm_video.h"

/**the max audio capbility count*/
#define AR_HDMI_MAX_AUDIO_CAP_COUNT         16

/**the max audio smprate count*/
#define AR_HDMI_MAX_SAMPE_RATE_NUM          8

/**the max audio bit depth count*/
#define AR_HDMI_MAX_BIT_DEPTH_NUM           6


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

typedef enum arHDMI_VIDEO_FMT_E
{
    AR_HDMI_VIDEO_FMT_1080P_60 = 0,
    AR_HDMI_VIDEO_FMT_1080P_50,
    AR_HDMI_VIDEO_FMT_1080P_30,
    AR_HDMI_VIDEO_FMT_1080P_25,
    AR_HDMI_VIDEO_FMT_1080P_24,

    AR_HDMI_VIDEO_FMT_1080i_60,
    AR_HDMI_VIDEO_FMT_1080i_50,

    AR_HDMI_VIDEO_FMT_720P_60,
    AR_HDMI_VIDEO_FMT_720P_50,

    AR_HDMI_VIDEO_FMT_576P_50,
    AR_HDMI_VIDEO_FMT_480P_60,

    AR_HDMI_VIDEO_FMT_PAL,
    AR_HDMI_VIDEO_FMT_PAL_N,
    AR_HDMI_VIDEO_FMT_PAL_Nc,

    AR_HDMI_VIDEO_FMT_NTSC,
    AR_HDMI_VIDEO_FMT_NTSC_J,
    AR_HDMI_VIDEO_FMT_NTSC_PAL_M,

    AR_HDMI_VIDEO_FMT_SECAM_SIN,
    AR_HDMI_VIDEO_FMT_SECAM_COS,

    AR_HDMI_VIDEO_FMT_861D_640X480_60,
    AR_HDMI_VIDEO_FMT_VESA_800X600_60,
    AR_HDMI_VIDEO_FMT_VESA_1024X768_60,
    AR_HDMI_VIDEO_FMT_VESA_1280X720_60,
    AR_HDMI_VIDEO_FMT_VESA_1280X800_60,
    AR_HDMI_VIDEO_FMT_VESA_1280X1024_60,
    AR_HDMI_VIDEO_FMT_VESA_1366X768_60,
    AR_HDMI_VIDEO_FMT_VESA_1440X900_60,
    AR_HDMI_VIDEO_FMT_VESA_1440X900_60_RB,
    AR_HDMI_VIDEO_FMT_VESA_1600X900_60_RB,
    AR_HDMI_VIDEO_FMT_VESA_1600X1200_60,
    AR_HDMI_VIDEO_FMT_VESA_1680X1050_60,
    AR_HDMI_VIDEO_FMT_VESA_1920X1080_60,
    AR_HDMI_VIDEO_FMT_VESA_1920X1200_60,
    AR_HDMI_VIDEO_FMT_VESA_2048X1152_60,
    AR_HDMI_VIDEO_FMT_2560x1440_30,
    AR_HDMI_VIDEO_FMT_2560x1440_60,
    AR_HDMI_VIDEO_FMT_2560x1600_60,
    AR_HDMI_VIDEO_FMT_1920x2160_30,

    AR_HDMI_VIDEO_FMT_3840X2160P_24,
    AR_HDMI_VIDEO_FMT_3840X2160P_25,
    AR_HDMI_VIDEO_FMT_3840X2160P_30,
    AR_HDMI_VIDEO_FMT_3840X2160P_50,
    AR_HDMI_VIDEO_FMT_3840X2160P_60,

    AR_HDMI_VIDEO_FMT_4096X2160P_24,
    AR_HDMI_VIDEO_FMT_4096X2160P_25,
    AR_HDMI_VIDEO_FMT_4096X2160P_30,
    AR_HDMI_VIDEO_FMT_4096X2160P_50,
    AR_HDMI_VIDEO_FMT_4096X2160P_60,

    AR_HDMI_VIDEO_FMT_3840X2160P_120,
    AR_HDMI_VIDEO_FMT_4096X2160P_120,
    AR_HDMI_VIDEO_FMT_7680X4320P_30,

    AR_HDMI_VIDEO_FMT_VESA_CUSTOMER_DEFINE,

    AR_HDMI_VIDEO_FMT_BUTT
}AR_HDMI_VIDEO_FMT_E;

typedef enum arHDMI_PIC_ASPECT_RATIO_E
{
    AR_HDMI_PIC_ASP_RATIO_NO_DATA,        /**<Aspect Ratio unknown*/
    AR_HDMI_PIC_ASP_RATIO_4TO3,           /**<Aspect Ratio 4:3*/
    AR_HDMI_PIC_ASP_RATIO_16TO9,          /**<Aspect Ratio 16:9*/
    AR_HDMI_PIC_ASP_RATIO_64TO27,
    AR_HDMI_PIC_ASP_RATIO_256TO135,
    AR_HDMI_PIC_ASP_RATIO_RESERVED,
}AR_HDMI_PIC_ASPECT_RATIO_E;

/* see CEA-861-D P67&P158 for details ,we base on DVB */
typedef enum arHDMI_ACT_ASPECT_RATIO_E
{
    AR_HDMI_ACT_ASP_RATIO_RESERVED_0,
    AR_HDMI_ACT_ASP_RATIO_RESERVED_1,
    AR_HDMI_ACT_ASP_RATIO_16TO9_TOP,
    AR_HDMI_ACT_ASP_RATIO_14TO9_TOP,
    AR_HDMI_ACT_ASP_RATIO_16TO9_BOX_CENTER,
    AR_HDMI_ACT_ASP_RATIO_RESERVED_5,
    AR_HDMI_ACT_ASP_RATIO_RESERVED_6,
    AR_HDMI_ACT_ASP_RATIO_RESERVED_7,
    AR_HDMI_ACT_ASP_RATIO_SAME_PIC,
    AR_HDMI_ACT_ASP_RATIO_4TO3_CENTER,
    AR_HDMI_ACT_ASP_RATIO_16TO9_CENTER,
    AR_HDMI_ACT_ASP_RATIO_14TO9_CENTER,
    AR_HDMI_ACT_ASP_RATIO_RESERVED_12,
    AR_HDMI_ACT_ASP_RATIO_4TO3_14_9,
    AR_HDMI_ACT_ASP_RATIO_16TO9_14_9,
    AR_HDMI_ACT_ASP_RATIO_16TO9_4_3,
}AR_HDMI_ACT_ASPECT_RATIO_E;


typedef enum arHDMI_SAMPLE_RATE_E
{
    AR_HDMI_SAMPLE_RATE_UNKNOWN=0,          /**< unknown sample rate */
    AR_HDMI_SAMPLE_RATE_8K    = 8000,       /**< 8K sample rate      */
    AR_HDMI_SAMPLE_RATE_11K   = 11025,      /**< 11.025K sample rate */
    AR_HDMI_SAMPLE_RATE_12K   = 12000,      /**< 12K sample rate     */
    AR_HDMI_SAMPLE_RATE_16K   = 16000,      /**< 16K sample rate     */
    AR_HDMI_SAMPLE_RATE_22K   = 22050,      /**< 22.050K sample rate */
    AR_HDMI_SAMPLE_RATE_24K   = 24000,      /**< 24K sample rate     */
    AR_HDMI_SAMPLE_RATE_32K   = 32000,      /**< 32K sample rate     */
    AR_HDMI_SAMPLE_RATE_44K   = 44100,      /**< 44.1K sample rate   */
    AR_HDMI_SAMPLE_RATE_48K   = 48000,      /**< 48K sample rate     */
    AR_HDMI_SAMPLE_RATE_88K   = 88200,      /**< 88.2K sample rate   */
    AR_HDMI_SAMPLE_RATE_96K   = 96000,      /**< 96K sample rate     */
    AR_HDMI_SAMPLE_RATE_176K  = 176400,     /**< 176K sample rate     */
    AR_HDMI_SAMPLE_RATE_192K  = 192000,     /**< 192K sample rate    */
    AR_HDMI_SAMPLE_RATE_768K  = 768000,     /**< 768K sample rate    */
    AR_HDMI_SAMPLE_RATE_BUTT        ,
}AR_HDMI_SAMPLE_RATE_E;

/**EDID Audio type enum*/
typedef enum arHDMI_AUDIO_FORMAT_CODE_E
{
    AR_HDMI_AUDIO_FORMAT_CODE_RESERVED  = 0x00,  /**<Audio coding type, refer stream,default type*/
    AR_HDMI_AUDIO_FORMAT_CODE_PCM,               /**<Audio coding PCM type*/
    AR_HDMI_AUDIO_FORMAT_CODE_AC3,               /**<Audio coding AC3 type*/
    AR_HDMI_AUDIO_FORMAT_CODE_MPEG1,             /**<Audio coding MPEG1 type*/
    AR_HDMI_AUDIO_FORMAT_CODE_MP3,               /**<Audio coding MP3 type*/
    AR_HDMI_AUDIO_FORMAT_CODE_MPEG2,             /**<Audio coding MPEG2 type*/
    AR_HDMI_AUDIO_FORMAT_CODE_AAC,               /**<Audio coding AAC type*/
    AR_HDMI_AUDIO_FORMAT_CODE_DTS,               /**<Audio coding DTS type*/
    AR_HDMI_AUDIO_FORMAT_CODE_ATRAC,             /**<Audio coding ATRAC type*/
    AR_HDMI_AUDIO_FORMAT_CODE_ONE_BIT,           /**<Audio coding ONE BIT AUDIO type*/
    AR_HDMI_AUDIO_FORMAT_CODE_DDP,               /**<Audio coding DDPLUS type*/
    AR_HDMI_AUDIO_FORMAT_CODE_DTS_HD,            /**<Audio coding DTS HD type*/
    AR_HDMI_AUDIO_FORMAT_CODE_MAT,               /**<Audio coding MAT type*/
    AR_HDMI_AUDIO_FORMAT_CODE_DST,               /**<Audio coding DST type*/
    AR_HDMI_AUDIO_FORMAT_CODE_WMA_PRO,           /**<Audio coding WMA PRO type*/
    AR_HDMI_AUDIO_FORMAT_CODE_BUTT,
}AR_HDMI_AUDIO_FORMAT_CODE_E;

typedef enum arHDMI_BIT_DEPTH_E
{
    AR_HDMI_BIT_DEPTH_UNKNOWN =0,           /**< unknown bit width  */
    AR_HDMI_BIT_DEPTH_8  = 8,               /**< 8 bits width  */
    AR_HDMI_BIT_DEPTH_16 = 16,              /**< 16 bits width */
    AR_HDMI_BIT_DEPTH_18 = 18,              /**< 18 bits width */
    AR_HDMI_BIT_DEPTH_20 = 20,              /**< 20 bits width */
    AR_HDMI_BIT_DEPTH_24 = 24,              /**< 24 bits width */
    AR_HDMI_BIT_DEPTH_32 = 32,              /**< 32 bits width */

    AR_HDMI_BIT_DEPTH_BUTT,
}AR_HDMI_BIT_DEPTH_E;

typedef enum arHDMI_SND_INTERFACE_E
{
    AR_HDMI_SND_INTERFACE_I2S,              /**<I2S(Inter-IC Sound)*/
    AR_HDMI_SND_INTERFACE_SPDIF,            /**<SPDIF(Sony/Philips Digital Interface) */
    AR_HDMI_SND_INTERFACE_HBR,              /**<SPDIF(Sony/Philips Digital Interface) */
    AR_HDMI_SND_INTERFACE_BUTT
}AR_HDMI_SND_INTERFACE_E;

typedef enum arHDMI_ID_E
{
    AR_HDMI_ID_0         = 0,               /**< HDMI 0 */
    AR_HDMI_ID_BUTT
} AR_HDMI_ID_E;

/** HDMI EVENT TYPE */
typedef enum arHDMI_EVENT_TYPE_E
{
    AR_HDMI_EVENT_HOTPLUG = 0x10,           /**<HDMI hot-plug event */
    AR_HDMI_EVENT_NO_PLUG,                  /**<HDMI cable disconnection event */
    AR_HDMI_EVENT_EDID_FAIL,                /**<HDMI EDID read failure event */
    AR_HDMI_EVENT_HDCP_FAIL,                /**<HDCP verification failure event */
    AR_HDMI_EVENT_HDCP_SUCCESS,             /**<HDCP verification success event */
    AR_HDMI_EVENT_HDCP_USERSETTING = 0x17,  /**<HDMI reset event,user should re-verification HDCP */
    AR_HDMI_EVENT_BUTT
}AR_HDMI_EVENT_TYPE_E;

/**VIDEO_MODE Attr(Color Space) enum*/
typedef enum arHDMI_VIDEO_MODE
{
    AR_HDMI_VIDEO_MODE_RGB444,
    AR_HDMI_VIDEO_MODE_YCBCR422,
    AR_HDMI_VIDEO_MODE_YCBCR444,
    AR_HDMI_VIDEO_MODE_YCBCR420,
    AR_HDMI_VIDEO_MODE_BUTT
}AR_HDMI_VIDEO_MODE_E;

/** Color Component Sample format and chroma sampling format enum,see EIA-CEA-861-D/F */
typedef enum arHDMI_COLOR_SPACE_E
{
    AR_HDMI_COLOR_SPACE_RGB444,
    AR_HDMI_COLOR_SPACE_YCBCR422,
    AR_HDMI_COLOR_SPACE_YCBCR444,
    /*following is new featrue of CEA-861-F*/
    AR_HDMI_COLOR_SPACE_YCBCR420,
    //AR_HDMI_COLOR_SPACE_RSERVED1,
    //AR_HDMI_COLOR_SPACE_RSERVED2,
    //AR_HDMI_COLOR_SPACE_RSERVED3,
    //AR_HDMI_COLOR_SPACE_IDO_DEFINED,
}AR_HDMI_COLOR_SPACE_E;


typedef enum arHDMI_DEEP_COLOR_E
{
    AR_HDMI_DEEP_COLOR_24BIT = 0x00,        /**< HDMI Deep Color 24bit mode  */
    AR_HDMI_DEEP_COLOR_30BIT,               /**< HDMI Deep Color 30bit mode  */
    AR_HDMI_DEEP_COLOR_36BIT,               /**< HDMI Deep Color 36bit mode  */
    AR_HDMI_DEEP_COLOR_OFF   = 0xff,        /**< HDMI Deep Color OFF mode(24bit).  */
    AR_HDMI_DEEP_COLOR_BUTT
}AR_HDMI_DEEP_COLOR_E;

typedef enum arHDMI_BARINFO_E
{
   HDMI_BAR_INFO_NOT_VALID,                 /**< Bar Data not valid */
   HDMI_BAR_INFO_V,                         /**< Vertical bar data valid */
   HDMI_BAR_INFO_H,                         /**< Horizental bar data valid */
   HDMI_BAR_INFO_VH                         /**< Horizental and Vertical bar data valid */
}AR_HDMI_BARINFO_E;

typedef enum arHDMI_SCANINFO_E
{
    HDMI_SCAN_INFO_NO_DATA      = 0,        /**< No Scan information*/
    HDMI_SCAN_INFO_OVERSCANNED  = 1,        /**< Scan information, Overscanned (for television) */
    HDMI_SCAN_INFO_UNDERSCANNED = 2,        /**< Scan information, Underscanned (for computer) */
    HDMI_SCAN_INFO_FUTURE
}AR_HDMI_SCANINFO_E;

typedef enum arHDMI_PICTURE_SCALING_E
{
    HDMI_PICTURE_NON_UNIFORM_SCALING,       /**< No Known, non-uniform picture scaling  */
    HDMI_PICTURE_SCALING_H,                 /**< Picture has been scaled horizentally */
    HDMI_PICTURE_SCALING_V,                 /**< Picture has been scaled Vertically */
    HDMI_PICTURE_SCALING_HV                 /**< Picture has been scaled Horizentally and Vertically   */
 }AR_HDMI_PICTURE_SCALING_E;


typedef enum arHDMI_COLORIMETRY_E
{
    HDMI_COLORIMETRY_NO_DATA,               /**<Colorimetry No Data option*/
    HDMI_COLORIMETRY_ITU601,                /**<Colorimetry ITU601 option*/
    HDMI_COLORIMETRY_ITU709,                /**<Colorimetry ITU709 option*/
    HDMI_COLORIMETRY_EXTENDED,              /**<Colorimetry extended option */
} AR_HDMI_COLORIMETRY_E;

typedef enum arHDMI_EXT_COLORIMETRY_E
{
    HDMI_COLORIMETRY_XVYCC_601,             /**<Colorimetry xvYCC601 extened option*/
    HDMI_COLORIMETRY_XVYCC_709,             /**<Colorimetry xvYCC709 extened option*/
    HDMI_COLORIMETRY_S_YCC_601,             /**<Colorimetry S YCC 601 extened option*/
    HDMI_COLORIMETRY_ADOBE_YCC_601,         /**<Colorimetry ADOBE YCC 601 extened option*/
    HDMI_COLORIMETRY_ADOBE_RGB,             /**<Colorimetry ADOBE RGB extened option*/
    HDMI_COLORIMETRY_2020_CONST_LUMINOUS,    /**<Colorimetry ITU2020 extened option*/
    HDMI_COLORIMETRY_2020_NON_CONST_LUMINOUS,/**<Colorimetry ITU2020 extened option*/
    HDMI_COLORIMETRY_RESERVED
} AR_HDMI_EXT_COLORIMETRY_E;


/**see EIA-CEA-861-D/F*/
typedef enum arHDMI_RGB_QUAN_RAGE_E
{
    HDMI_RGB_QUANTIZATION_DEFAULT_RANGE,    /**< Defaulr range, it depends on the video format */
    HDMI_RGB_QUANTIZATION_LIMITED_RANGE,    /**< Limited quantization range of 220 levels when receiving a CE video format*/
    HDMI_RGB_QUANTIZATION_FULL_RANGE   ,    /**< Full quantization range of 256 levels when receiving an IT video format*/
    HDMI_RGB_QUANTIZATION_FULL_RESERVED
}AR_HDMI_RGB_QUAN_RAGE_E;

/**see EIA-CEA-861-D/F*/
typedef enum arHDMI_YCC_QUAN_RAGE_E
{
    HDMI_YCC_QUANTIZATION_LIMITED_RANGE,    /**< Limited quantization range of 220 levels when receiving a CE video format*/
    HDMI_YCC_QUANTIZATION_FULL_RANGE,       /**< Full quantization range of 256 levels when receiving an IT video format*/
    HDMI_YCC_QUANTIZATION_RESERVED_2,
    HDMI_YCC_QUANTIZATION_RESERVED_3
}AR_HDMI_YCC_QUAN_RAGE_E;

typedef enum arHDMI_QUANTIZATION_E
{
    HDMI_QUANTIZATION_LIMITED_RANGE,    /**< Limited quantization range of 220 levels when receiving a CE video format*/
    HDMI_QUANTIZATION_FULL_RANGE,       /**< Full quantization range of 256 levels when receiving an IT video format*/
    HDMI_QUANTIZATION_BUTT
}AR_HDMI_QUANTIZATION_E;

/**Pixel Repetition Factor  enum,see EIA-CEA-861-D*/
typedef enum arHDMI_PIXEL_REPETITION_E
{
    HDMI_PIXEL_REPET_NO,
    HDMI_PIXEL_REPET_2_TIMES,
    HDMI_PIXEL_REPET_3_TIMES,
    HDMI_PIXEL_REPET_4_TIMES,
    HDMI_PIXEL_REPET_5_TIMES,
    HDMI_PIXEL_REPET_6_TIMES,
    HDMI_PIXEL_REPET_7_TIMES,
    HDMI_PIXEL_REPET_8_TIMES,
    HDMI_PIXEL_REPET_9_TIMES,
    HDMI_PIXEL_REPET_10_TIMES,
    HDMI_PIXEL_REPET_RESERVED_A,
    HDMI_PIXEL_REPET_RESERVED_B,
    HDMI_PIXEL_REPET_RESERVED_C,
    HDMI_PIXEL_REPET_RESERVED_D,
    HDMI_PIXEL_REPET_RESERVED_E,
    HDMI_PIXEL_REPET_RESERVED_F,
}AR_HDMI_PIXEL_REPETITION_E;

/**IT Contents Type  enum,see EIA-CEA-861-F*/
typedef enum arHDMI_CONTENT_TYPE_E
{
    HDMI_CONTNET_GRAPHIC,                   /**< Graphics type*/
    HDMI_CONTNET_PHOTO,                     /**< Photo type*/
    HDMI_CONTNET_CINEMA,                    /**< Cinema type*/
    HDMI_CONTNET_GAME                       /**< Game type*/
}AR_HDMI_CONTENT_TYPE_E;


/**Audio Coding Type   enum,see EIA-CEA-861-D*/
typedef enum arHDMI_CODING_TYPE_E
{
    HDMI_AUDIO_CODING_REFER_STREAM_HEAD,
    HDMI_AUDIO_CODING_PCM,
    HDMI_AUDIO_CODING_AC3,
    HDMI_AUDIO_CODING_MPEG1,
    HDMI_AUDIO_CODING_MP3,
    HDMI_AUDIO_CODING_MPEG2,
    HDMI_AUDIO_CODING_AACLC,
    HDMI_AUDIO_CODING_DTS,
    HDMI_AUDIO_CODING_ATRAC,
    HDMI_AUDIO_CODIND_ONE_BIT_AUDIO,
    HDMI_AUDIO_CODING_ENAHNCED_AC3,
    HDMI_AUDIO_CODING_DTS_HD,
    HDMI_AUDIO_CODING_MAT,
    HDMI_AUDIO_CODING_DST,
    HDMI_AUDIO_CODING_WMA_PRO,
    HDMI_AUDIO_CODING_MAX
}AR_HDMI_CODING_TYPE_E;

/**EDID Audio speaker enum*/
typedef enum arHDMI_AUDIO_SPEAKER_E
{
    HDMI_AUDIO_SPEAKER_FL_FR   ,
    HDMI_AUDIO_SPEAKER_LFE     ,
    HDMI_AUDIO_SPEAKER_FC      ,
    HDMI_AUDIO_SPEAKER_RL_RR   ,
    HDMI_AUDIO_SPEAKER_RC      ,
    HDMI_AUDIO_SPEAKER_FLC_FRC ,
    HDMI_AUDIO_SPEAKER_RLC_RRC ,
    HDMI_AUDIO_SPEAKER_FLW_FRW ,
    HDMI_AUDIO_SPEAKER_FLH_FRH ,
    HDMI_AUDIO_SPEAKER_TC      ,
    HDMI_AUDIO_SPEAKER_FCH     ,
    HDMI_AUDIO_SPEAKER_BUTT    ,
} AR_HDMI_AUDIO_SPEAKER_E;


typedef enum arHDMI_FORCE_ACTION_E
{
    AR_HDMI_FORCE_NULL,                     /**< Standard mode */
    AR_HDMI_FORCE_HDMI,                     /**< Forcibly output video in HDMI mode */
    AR_HDMI_FORCE_DVI,                      /**< Forcibly output video in DVI mode */
    AR_HDMI_INIT_BOOT_CONFIG                /**< Test Only */
}AR_HDMI_FORCE_ACTION_E;

typedef enum arAR_HDMI_CSC_MATRIX_E
{
    AR_HDMI_CSC_MATRIX_IDENTITY = 0,

    AR_HDMI_CSC_MATRIX_BT601_TO_BT709,
    AR_HDMI_CSC_MATRIX_BT709_TO_BT601,

    AR_HDMI_CSC_MATRIX_BT601_TO_RGB_PC,
    AR_HDMI_CSC_MATRIX_BT709_TO_RGB_PC,

    AR_HDMI_CSC_MATRIX_RGB_TO_BT601_PC,
    AR_HDMI_CSC_MATRIX_RGB_TO_BT709_PC,

    AR_HDMI_CSC_MATRIX_BUTT
} AR_HDMI_CSC_MATRIX_E;


/** hdmi event handling callback function */
typedef void (*AR_HDMI_CallBack)(AR_HDMI_EVENT_TYPE_E event, AR_VOID *pPrivateData);

typedef struct arHDMI_CALLBACK_FUNC_S
{
    AR_HDMI_CallBack     pfnHdmiEventCallback;    /**< Event handling callback function */
    AR_VOID             *pPrivateData;            /**< Private data of the callback functions and parameters */
}AR_HDMI_CALLBACK_FUNC_S;


/**EDID Audio Info struct*/
typedef struct arHDMI_AUDIO_INFO_S
{
    AR_HDMI_AUDIO_FORMAT_CODE_E     enAudFmtCode;                                         /**<Audio coding type*/
    AR_HDMI_SAMPLE_RATE_E           enSupportSampleRate[AR_HDMI_MAX_SAMPE_RATE_NUM];     /**<Audio sample rate*/
    AR_U8                           u8AudChannel;                                          /**<Aud Channel of the coding type*/
    AR_HDMI_BIT_DEPTH_E             enSupportBitDepth[AR_HDMI_MAX_BIT_DEPTH_NUM];        /**< sample bit depth,when audio format code is AR_UNF_EDID_AUDIO_FORMAT_CODE_PCM*/
    AR_U32                          u32SupportBitDepthNum;                                 /**< sample bit depth Num,when audio format code is AR_UNF_EDID_AUDIO_FORMAT_CODE_PCM*/
    AR_U32                          u32MaxBitRate;                                      /**< enter max bit rate,when audio format code is AR_UNF_EDID_AUDIO_FORMAT_CODE_AC3 - AR_UNF_EDID_AUDIO_FORMAT_CODE_ATRAC**/
}AR_HDMI_AUDIO_INFO_S;

/**EDID EOTF support. */
typedef struct arHDMI_EOTF_S
{
    AR_BOOL bEotfSdr;               /**<Traditional gamma - SDR Luminance Range. */
    AR_BOOL bEotfHdr;               /**<Traditional gamma - HDR Luminance Range. */
    AR_BOOL bEotfSmpteSt2084;       /**<SMPTE ST 2084 */
    AR_BOOL bEotfHLG;               /**<Hybrid Log-Gamma(HLG) based on Recommendation ITU-R BT.2100-0 */
    AR_BOOL bEotfFuture;            /**<Future EOTF */
}AR_HDMI_EOTF_S;

/**EDID Supported Static Metadata Descriptor */
typedef struct arHDMI_HDR_METADATA_TYPE_S
{
    AR_BOOL bDescriptorType1;       /**<Support Static Metadata Type 1 or not.*/
}AR_HDMI_HDR_METADATA_TYPE_S;

/**EDID HDR capability, see <CEA-861.3-2015> */
typedef struct arHDMI_HDR_CAP_S{

    AR_HDMI_EOTF_S                  stEotf;                 /**<EOTF support. */
    AR_HDMI_HDR_METADATA_TYPE_S     stMetadata;             /**<static metadata descriptor type(ID) support. */
    AR_U8                           u8MaxLuminance_CV;      /**<Desired Content Max Luminance Data. Real value(in 1cd/m^2) = 50.2^(CV/32) */
    AR_U8                           u8AverageLumin_CV;      /**<Desired Content Max Frame-average Luminance. Real value(in 1cd/m^2) = 50.2^(CV/32) */
    AR_U8                           u8MinLuminance_CV;      /**<Desired Content Min Luminance Data. Real value(in 1cd/m^2) = u8MaxLuminance_CV * (u8MinLuminance_CV/255)^2 / 100 */
}AR_HDMI_HDR_CAP_S;

/**EDID detailed Timing Info struct*/
typedef struct arHDMI_TIMING_INFO_S
{
    AR_U32                        u32VFB;          /**<vertical front blank*/
    AR_U32                        u32VBB;          /**<vertical back blank*/
    AR_U32                        u32VACT;         /**<vertical active area*/
    AR_U32                        u32HFB;          /**<horizonal front blank*/
    AR_U32                        u32HBB;          /**<horizonal back blank*/
    AR_U32                        u32HACT;         /**<horizonal active area*/
    AR_U32                        u32VPW;          /**<vertical sync pluse width*/
    AR_U32                        u32HPW;          /**<horizonal sync pluse width*/
    AR_BOOL                       bIDV;            /**<flag of data valid signal is needed flip*/
    AR_BOOL                       bIHS;            /**<flag of horizonal sync pluse is needed flip*/
    AR_BOOL                       bIVS;            /**<flag of vertical sync pluse is needed flip*/
    AR_U32                        u32ImageWidth;   /**<image width */
    AR_U32                        u32ImageHeight;  /**<image height */
    AR_U32                        u32AspectRatioW; /**<aspect ratio width */
    AR_U32                        u32AspectRatioH; /**<aspect ratio height */
    AR_BOOL                       bInterlace;      /**<flag of interlace */
    AR_S32                        u32PixelClk;     /**<pixelc clk for this timing, unit:KHz */
} AR_HDMI_TIMING_INFO_S;

typedef struct arHDMI_DET_TIMING_S
{
    AR_U32                      u32DetTimingNum;
    AR_HDMI_TIMING_INFO_S       astDetTiming[10];
}AR_HDMI_DET_TIMING_S;

typedef struct arHDMI_SINK_CAPABILITY_S
{
    AR_BOOL             bConnected;                                                       /**< Whether the devices are connected. */
    AR_BOOL             bSupportHdmi;                                                     /**< Whether the HDMI is supported by the device. If the HDMI is not supported by the device, the device is DVI.*/
    AR_BOOL             bIsSinkPowerOn;                                                   /**< Whether the sink device is powered on. */
    AR_BOOL             bIsRealEDID;                                                      /**< Whether the EDID obtains the flag from the sink device.AR_TRUE: The EDID information is correctly read.AR_FASE: default settings */

    AR_HDMI_VIDEO_FMT_E enNativeVideoFormat;                                              /**< Physical resolution of the display device. */
    AR_BOOL             bVideoFmtSupported[AR_HDMI_VIDEO_FMT_BUTT];                     /**< Video capability set.AR_TRUE: This display format is supported.AR_FALSE: This display format is not supported. */
    AR_BOOL             bSupportYCbCr;                                                    /**< Whether the YCBCR display is supported.AR_TRUE: The YCBCR display is supported.AR_FALSE: Only red-green-blue (RGB) is supported. */

    AR_BOOL             bSupportxvYCC601;                                                 /**< Whether the xvYCC601 color format is supported. */
    AR_BOOL             bSupportxvYCC709;                                                 /**< Whether the xvYCC709 color format is supported. */
    AR_U8               u8MDBit;                                                          /**< Transfer profile supported by xvYCC601. 1: P0; 2: P1; 4: P2. */
    AR_U32              u32AudioInfoNum;                                                  /**< Num of audio Info*/
    AR_HDMI_AUDIO_INFO_S stAudioInfo[AR_HDMI_MAX_AUDIO_CAP_COUNT];                         /**< Audio Info.For details, see Table 37 in EIA-CEA-861-D. */

    AR_BOOL             bSpeaker[HDMI_AUDIO_SPEAKER_BUTT];                                /**< Speaker position. For details, see the definition of SpeakerDATABlock in EIA-CEA-861-D. */
    AR_U8               u8IDManufactureName[4];                                           /**< Device vendor flag */
    AR_U32              u32IDProductCode;                                                 /**< Device ID. */
    AR_U32              u32IDSerialNumber;                                                /**< Device sequence number. */
    AR_U32              u32WeekOfManufacture;                                             /**< Device production data (week). */
    AR_U32              u32YearOfManufacture;                                             /**< Set the production data (year). */
    AR_U8               u8Version;                                                        /**< Device version number */
    AR_U8               u8Revision;                                                       /**< Device sub version number */
    AR_U8               u8EDIDExternBlockNum;                                             /**< EDID extended block number */

    AR_BOOL             bIsPhyAddrValid;                                                  /**< Valid flag of the consumer electronics control (CEC) physical address */
    AR_U8               u8PhyAddr_A;                                                      /**< CEC physical address A */
    AR_U8               u8PhyAddr_B;                                                      /**< CEC physical address B */
    AR_U8               u8PhyAddr_C;                                                      /**< CEC physical address C */
    AR_U8               u8PhyAddr_D;                                                      /**< CEC physical address D */
    AR_BOOL             bSupportDVIDual;                                                  /**< Whether to support the DVI dual-link operation. */
    AR_BOOL             bSupportDeepColorYCBCR444;                                        /**< Whether to support the YCBCR 4:4:4 deep-color mode. */
    AR_BOOL             bSupportDeepColor30Bit;                                           /**< Whether to support the deep-color 30-bit mode.*/
    AR_BOOL             bSupportDeepColor36Bit;                                           /**< Whether to support the deep-color 36-bit mode. */
    AR_BOOL             bSupportDeepColor48Bit;                                           /**< Whether to support the deep-color 48-bit mode. */
    AR_BOOL             bSupportAI;                                                       /**< Whether to support the Supports_AI mode. */
    AR_U32              u32MaxTMDSClock;                                                   /**< Maximum TMDS clock. */

    AR_BOOL             bI_Latency_Fields_Present;                                        /**< Delay flag bit. */
    AR_BOOL             bLatency_Fields_Present;                                          /**< whether Video_Latency and Audio_Latency fields are present */
    AR_BOOL             bHDMI_Video_Present;                                              /**< Special video format */
    AR_U8               u8Video_Latency;                                                  /**< Video delay */
    AR_U8               u8Audio_Latency;                                                  /**< Audio delay */
    AR_U8               u8Interlaced_Video_Latency;                                        /**< Video delay in interlaced video mode */
    AR_U8               u8Interlaced_Audio_Latency;                                        /**< Audio delay in interlaced video mode */
    AR_BOOL             bSupportY420DC30Bit;                                               /**< Whether to support the YUV420 deep-color 30-bit mode.*/
    AR_BOOL             bSupportY420DC36Bit;                                               /**< Whether to support the YUV420 deep-color 36-bit mode. */
    AR_BOOL             bSupportY420DC48Bit;                                               /**< Whether to support the YUV420 deep-color 48-bit mode. */
    AR_BOOL             bSupportHdmi_2_0;                                                  /**< Whether to support HDMI2.0. */
    AR_BOOL             bSupportY420Format[AR_HDMI_VIDEO_FMT_BUTT];                     /**< the format support YUV420 */
    AR_BOOL             bOnlySupportY420Format[AR_HDMI_VIDEO_FMT_BUTT];                    /**< the format only support YUV420 */

    AR_BOOL             bYccQrangeSelectable;                                           /**<flag of RGB Quantization Range selectable.when AR_TRUE,sink expect full/limited range is selectable;or sink expect default range. */
    AR_BOOL             bRgbQrangeSelectable;                                           /**<flag of YCC Quantization Range selectable.when AR_TRUE,sink expect full/limited range is selectable;or sink expect default range. */
    AR_BOOL             bHdrSupport;                                                    /**<HDR support flag.When HDR Static Metadata Data Block is indicated,flag AR_TRUE;or,AR_FALSE.*/
    AR_HDMI_HDR_CAP_S   stHdr;                                                          /**<HDR capability*/
    AR_HDMI_DET_TIMING_S    stDetailedTiming;                                           /**<Detailed timing infomation*/
} AR_HDMI_SINK_CAPABILITY_S;

typedef struct arHDMI_ATTR_S
{
    AR_BOOL                 bEnableHdmi;         /**< Whether to forcibly output the video over the HDMI.  */

    AR_BOOL                 bEnableVideo;        /**< Whether to output video.The value must be AR_TRUE.If the value is AR_FALSE, the HDMI is forcibly set to AR_TRUE. */
    AR_HDMI_VIDEO_FMT_E     enVideoFmt;          /**< Video norm. This value of the video norm must be consistent with the norm of the video output. */
    AR_HDMI_VIDEO_MODE_E    enVidInMode;        /**< HDMI input video input mode.VIDEO_MODE_YCBCR444,VIDEO_MODE_YCBCR422,VIDEO_MODE_YCBCR420,VIDEO_MODE_RGB444 */
    AR_HDMI_VIDEO_MODE_E    enVidOutMode;        /**< HDMI output video output mode.VIDEO_MODE_YCBCR444,VIDEO_MODE_YCBCR422,VIDEO_MODE_YCBCR420,VIDEO_MODE_RGB444 */
    AR_HDMI_QUANTIZATION_E  enOutCscQuantization; /**< CSC output quantization range HDMI_QUANTIZATION_RANGE_LIMITED, HDMI_QUANTIZATION_RANGE_FULL>**/
    AR_HDMI_DEEP_COLOR_E    enDeepColorMode;     /**< DeepColor output mode.It is AR_HDMI_DEEP_COLOR_24BIT by default. */
    AR_BOOL                 bxvYCCMode;          /**< Whether to enable the xvYCC output mode.It is AR_FALSE by default. */

    AR_BOOL                 bEnableAudio;        /**< Whether to enable the audio. */
    AR_HDMI_SND_INTERFACE_E enSoundIntf;         /**< HDMI audio source.It is set to AR_HDMI_SND_INTERFACE_I2S, which needs to be consistent with the VO interface. */
    AR_BOOL                 bIsMultiChannel;     /**< Multi-channel or stereo 0: stereo 1: eight-channel fixed for multi-channel */
    AR_HDMI_SAMPLE_RATE_E   enSampleRate;        /**< Audio sampling rate. This parameter needs to be consistent with that of the VO. */
    AR_U8                   u8DownSampleParm;    /**< Audio down sampling rate parameter. It is 0 by default. */

    AR_HDMI_BIT_DEPTH_E     enBitDepth;          /**< Audio bit width. It is 16 by default. This parameter needs to be consistent with that of the VO. */
    AR_U8                   u8I2SCtlVbit;        /**< Reserved. It is set to 0. I2S control (0x7A:0x1D) */

    AR_BOOL                 bEnableAviInfoFrame; /**< Whether to enable AVI InfoFrame.It is recommended to enable this function. */
    AR_BOOL                 bEnableAudInfoFrame; /**< Whether to enable AUDIO InfoFrame.It is recommended to enable this function. */
    AR_BOOL                 bEnableSpdInfoFrame; /**< Whether to enable SPD InfoFrame.It is recommended to disable this function. */
    AR_BOOL                 bEnableMpegInfoFrame;/**< Whether to enable MPEG InfoFrame.It is recommended to disable this function. */

    AR_BOOL                 bDebugFlag;          /**< Whether to enable the debug information in the HDMI.It is recommended to disable this function. */
    AR_BOOL                 bHDCPEnable;         /**< Whether to enable the HDCP.0: disabled 1: enabled */

    AR_BOOL                 b3DEnable;           /**< Whether to enable 3D mode. 0: disabled 1: enabled */
    AR_U8                   u83DParam;           /**< 3D Parameter. The default value is 9. */
    AR_HDMI_FORCE_ACTION_E  enDefaultMode;         /**< When get capability fail,HDMI forcily(priority) judgments output video mode.Default is AR_HDMI_FORCE_HDMI.*/
    AR_BOOL                 bAuthMode;           /**< Whether to enable auth mode. 0: disabled 1: enabled */
    AR_BOOL                 bEnableVidModeAdapt; /**< Enable flag of vedio mode & DVI adapting case of user setting incorrect,default:AR_TRUE.When user have no any adapting strategy,suggestion AR_TRUE*/
    AR_BOOL                 bEnableDeepClrAdapt; /**< Enable flag of deep color mode adapting case of user setting incorrect,default: AR_FALSE.When user have no any adapting strategy,suggestion AR_TRUE*/
    AR_U32                  u32PixClk;           /**< Pixclk of enVideoFmt(unit is kHz).(This param is valid only when enVideoFmt is AR_HDMI_VIDEO_FMT_VESA_CUSTOMER_DEFINE) */
}AR_HDMI_ATTR_S;

typedef struct arAR_HDMI_EDID_S
{
    AR_BOOL                 bEdidValid;
    AR_U32                  u32Edidlength;
    AR_U8                   u8Edid[512];
}AR_HDMI_EDID_S;

typedef enum arHDMI_INFOFRAME_TYPE_E
{
    AR_INFOFRAME_TYPE_AVI,          /**< HDMI AVI InfoFrame */
    AR_INFOFRAME_TYPE_SPD,          /**< HDMI SPD InfoFrame */
    AR_INFOFRAME_TYPE_AUDIO,        /**< HDMI AUDIO InfoFrame */
    AR_INFOFRAME_TYPE_MPEG,         /**< HDMI MPEG InfoFrame */
    AR_INFOFRAME_TYPE_VENDORSPEC,   /**< HDMI Vendor Specific InfoFrame */
    AR_INFOFRAME_TYPE_BUTT
}AR_HDMI_INFOFRAME_TYPE_E;

/** HDMI AVI InfoFrame struct, see EIA-CEA-861-D */
typedef struct ar_HDMI_AVI_INFOFRAME_VER2_S
{
    AR_HDMI_VIDEO_FMT_E        enTimingMode;
    AR_HDMI_COLOR_SPACE_E      enColorSpace;
    AR_BOOL                    bActive_Infor_Present;
    AR_HDMI_BARINFO_E          enBarInfo;
    AR_HDMI_SCANINFO_E         enScanInfo;

    AR_HDMI_COLORIMETRY_E      enColorimetry;
    AR_HDMI_EXT_COLORIMETRY_E  enExtColorimetry;
    AR_HDMI_PIC_ASPECT_RATIO_E enAspectRatio;
    AR_HDMI_ACT_ASPECT_RATIO_E enActiveAspectRatio;
    AR_HDMI_PICTURE_SCALING_E  enPictureScaling;

    AR_HDMI_RGB_QUAN_RAGE_E    enRGBQuantization;
    AR_BOOL                    bIsITContent;
    AR_HDMI_PIXEL_REPETITION_E enPixelRepetition;
    AR_HDMI_CONTENT_TYPE_E     enContentType;
    AR_HDMI_YCC_QUAN_RAGE_E    enYCCQuantization;

    AR_U16                     u16LineNEndofTopBar;
    AR_U16                     u16LineNStartofBotBar;
    AR_U16                     u16PixelNEndofLeftBar;
    AR_U16                     u16PixelNStartofRightBar;
}AR_HDMI_AVI_INFOFRAME_VER2_S;

/**Audio chanel count enum,see EIA-CEA-861-D/F*/
typedef enum arHDMI_AUDIO_CHANEL_CNT_E
{
    AR_HDMI_AUDIO_CHANEL_CNT_STREAM,
    AR_HDMI_AUDIO_CHANEL_CNT_2,
    AR_HDMI_AUDIO_CHANEL_CNT_3,
    AR_HDMI_AUDIO_CHANEL_CNT_4,
    AR_HDMI_AUDIO_CHANEL_CNT_5,
    AR_HDMI_AUDIO_CHANEL_CNT_6,
    AR_HDMI_AUDIO_CHANEL_CNT_7,
    AR_HDMI_AUDIO_CHANEL_CNT_8,
}AR_HDMI_AUDIO_CHANEL_CNT_E;

/**Audio Sample Size  enum,see EIA-CEA-861-D/F*/
typedef enum arHDMI_AUDIO_SAMPLE_SIZE_E
{
    AR_HDMI_AUDIO_SAMPLE_SIZE_STREAM,
    AR_HDMI_AUDIO_SAMPLE_SIZE_16,
    AR_HDMI_AUDIO_SAMPLE_SIZE_20,
    AR_HDMI_AUDIO_SAMPLE_SIZE_24,
}AR_HDMI_AUDIO_SAMPLE_SIZE_E;

/**Audio Sampling Frequency enum,see EIA-CEA-861-D/F*/
typedef enum arHDMI_AUDIO_SAMPLE_FREQ_E
{
    AR_HDMI_AUDIO_SAMPLE_FREQ_STREAM,
    AR_HDMI_AUDIO_SAMPLE_FREQ_32000,
    AR_HDMI_AUDIO_SAMPLE_FREQ_44100,
    AR_HDMI_AUDIO_SAMPLE_FREQ_48000,
    AR_HDMI_AUDIO_SAMPLE_FREQ_88200,
    AR_HDMI_AUDIO_SAMPLE_FREQ_96000,
    AR_HDMI_AUDIO_SAMPLE_FREQ_176400,
    AR_HDMI_AUDIO_SAMPLE_FREQ_192000,
}AR_HDMI_AUDIO_SAMPLE_FREQ_E;

/** Level Shift Values (LSV0-LSV3) enum,see EIA-CEA-861-D/F*/
typedef enum arHDMI_LEVEL_SHIFT_VALUE_E
{
    AR_HDMI_LEVEL_SHIFT_VALUE_0_DB,
    AR_HDMI_LEVEL_SHIFT_VALUE_1_DB,
    AR_HDMI_LEVEL_SHIFT_VALUE_2_DB,
    AR_HDMI_LEVEL_SHIFT_VALUE_3_DB,
    AR_HDMI_LEVEL_SHIFT_VALUE_4_DB,
    AR_HDMI_LEVEL_SHIFT_VALUE_5_DB,
    AR_HDMI_LEVEL_SHIFT_VALUE_6_DB,
    AR_HDMI_LEVEL_SHIFT_VALUE_7_DB,
    AR_HDMI_LEVEL_SHIFT_VALUE_8_DB,
    AR_HDMI_LEVEL_SHIFT_VALUE_9_DB,
    AR_HDMI_LEVEL_SHIFT_VALUE_10_DB,
    AR_HDMI_LEVEL_SHIFT_VALUE_11_DB,
    AR_HDMI_LEVEL_SHIFT_VALUE_12_DB,
    AR_HDMI_LEVEL_SHIFT_VALUE_13_DB,
    AR_HDMI_LEVEL_SHIFT_VALUE_14_DB,
    AR_HDMI_LEVEL_SHIFT_VALUE_15_DB,
}AR_HDMI_LEVEL_SHIFT_VALUE_E;

/**LFE Playback Level enum,see EIA-CEA-861-D/F*/
typedef enum arHDMI_LFE_PLAYBACK_LEVEL_E
{
    AR_HDMI_LFE_PLAYBACK_NO,
    AR_HDMI_LFE_PLAYBACK_0_DB,
    AR_HDMI_LFE_PLAYBACK_10_DB,
    AR_HDMI_LFE_PLAYBACK_RESEVED,
}AR_HDMI_LFE_PLAYBACK_LEVEL_E;


/** HDMI AUDIO InfoFrame struct, see EIA-CEA-861-D */
typedef struct arHDMI_AUD_INFOFRAME_VER1_S
{
    AR_HDMI_AUDIO_CHANEL_CNT_E          enChannelCount;
    AR_HDMI_CODING_TYPE_E               enCodingType;
    AR_HDMI_AUDIO_SAMPLE_SIZE_E         enSampleSize;
    AR_HDMI_AUDIO_SAMPLE_FREQ_E         enSamplingFrequency;
    AR_U8                               u8ChannelAlloc;            /* Channel/Speaker Allocation.Range [0,255] */
    AR_HDMI_LEVEL_SHIFT_VALUE_E         enLevelShift;
    AR_HDMI_LFE_PLAYBACK_LEVEL_E        enLfePlaybackLevel;
    AR_BOOL                             bDownmixInhibit;
}AR_HDMI_AUD_INFOFRAME_VER1_S;

/** HDMI Source Product Description InfoFrame struct , see EIA-CEA-861-D */
typedef struct arHDMI_SPD_INFOFRAME_S
{
    AR_U8                          u8VendorName[8];
    AR_U8                          u8ProductDescription[16];
}AR_HDMI_SPD_INFOFRAME_S;

/** HDMI MEPG SOURCE Description InfoFrame struct , see EIA-CEA-861-D */
typedef struct arHDMI_MPEGSOURCE_INFOFRAME_S
{
    AR_U32                         u32MPEGBitRate;
    AR_BOOL                        bIsFieldRepeated;
}AR_HDMI_MPEGSOURCE_INFOFRAME_S;

/** HDMI Vendor Specific InfoFrame struct , see EIA-CEA-861-D */
typedef struct arHDMI_VENDORSPEC_INFOFRAME_S
{
    AR_U32                         u32RegistrationId;
}AR_HDMI_VENDORSPEC_INFOFRAME_S;

typedef union arHDMI_INFOFRAME_UNIT_U
{
    AR_HDMI_AVI_INFOFRAME_VER2_S   stAVIInfoFrame;
    AR_HDMI_AUD_INFOFRAME_VER1_S   stAUDInfoFrame;
    AR_HDMI_SPD_INFOFRAME_S        stSPDInfoFrame;
    AR_HDMI_MPEGSOURCE_INFOFRAME_S stMPEGSourceInfoFrame;
    AR_HDMI_VENDORSPEC_INFOFRAME_S stVendorSpecInfoFrame;
}AR_HMDI_INFORFRAME_UNIT_U;

typedef struct arUNF_HDMI_INFOFRAME_S
{
    AR_HDMI_INFOFRAME_TYPE_E    enInfoFrameType;  /**<InfoFrame type */
    AR_HMDI_INFORFRAME_UNIT_U   unInforUnit;      /**<InfoFrame date */
}AR_HDMI_INFOFRAME_S;

/**<  CEC */
/**< CEC's Opcode:Please refer to CEC 15 Message Descriptions */
/**< General Protocol messages*/
#define CEC_OPCODE_FEATURE_ABORT                  0X00  /**< <Feature Abort> Used as a response to indicate that the device does not support the requested message type, or that it cannot execute it at the present time. */
#define CEC_OPCODE_ABORT_MESSAGE                  0XFF  /**< <Abort> Message This message is reserved for testing purposes. */
/**< One Touch Play Feature*/
#define CEC_OPCODE_ACTIVE_SOURCE                  0X82  /**< <Active Source> Used by a new source to indicate that it has started to transmit a stream OR used in response to a <Request Active Source>  */
#define CEC_OPCODE_IMAGE_VIEW_ON                  0X04  /**< <Image View On> Sent by a source device to the TV whenever it enters the active state (alternatively it may send <Text View On>). */
#define CEC_OPCODE_TEXT_VIEW_ON                   0X0D  /**< <Text View On> As <Image View On>, but should also remove any text, menus and PIP windows from the TV's display. */
/**< Routing Control Feature*/
#define CEC_OPCODE_INACTIVE_SOURCE                0X9D  /**< <Inactive Source> Used by the currently active source to inform the TV that it has no video to be presented to the user, or is going into standby as the result of a local user command on the device. */
#define CEC_OPCODE_REQUEST_ACTIVE_SOURCE          0X85  /**< <Request Active Source> Used by a new device to discover the status of the system. */
#define CEC_OPCODE_ROUTING_CHANGE                 0X80  /**< <Routing Change> Sent by a CEC Switch when it is manually switched to inform all other devices on the network that the active route below the switch has changed. */
#define CEC_OPCODE_ROUTING_INFORMATION            0X81  /**< <Routing Information> Sent by a CEC Switch to indicate the active route below the switch. */
#define CEC_OPCODE_SET_STREAM_PATH                0X86  /**< <Set Stream Path> Used by the TV to request a streaming path from the specified physical address. */
/**< Standby Feature*/
#define CEC_OPCODE_STANDBY                        0X36  /**< <Standby> Switches one or all devices into standby mode. Can be used as a broadcast message or be addressed to a specific device. See section CEC 13.3 for important notes on the use of this message */
/**< One Touch Record Feature*/
#define CEC_OPCODE_RECORD_OFF                     0X0B  /**< <Record Off> Requests a device to stop a recording. */
#define CEC_OPCODE_RECORD_ON                      0X09  /**< <Record On> Attempt to record the specified source. */
#define CEC_OPCODE_RECORD_STATUS                  0X0A  /**< <Record Status> Used by a Recording Device to inform the initiator of the message <Record On> about its status. */
#define CEC_OPCODE_RECORD_TV_SCREEN               0X0F  /**< <Record TV Screen> Request by the Recording Device to record the presently displayed source. */
/**< Timer Programming Feature*/
#define CEC_OPCODE_CLEAR_ANALOGUE_TIMER           0X33  /**< <Clear Analogue Timer> Used to clear an Analogue timer block of a device. */
#define CEC_OPCODE_CLEAR_DIGITAL_TIMER            0X99  /**< <Clear Digital Timer> Used to clear a Digital timer block of a device. */
#define CEC_OPCODE_CLEAR_EXTERNAL_TIMER           0XA1  /**< <Clear External Timer> Used to clear an External timer block of a device. */
#define CEC_OPCODE_SET_ANALOGUE_TIMER             0X34  /**< <Set Analogue Timer> Used to set a single timer block on an Analogue Recording Device. */
#define CEC_OPCODE_SET_DIGITAL_TIMER              0X97  /**< <Set Digital Timer> Used to set a single timer block on a Digital Recording Device. */
#define CEC_OPCODE_SET_EXTERNAL_TIMER             0XA2  /**< <Set External Timer> Used to set a single timer block to record from an external device. */
#define CEC_OPCODE_SET_TIMER_PROGRAM_TITLE        0X67  /**< <Set Timer Program Title> Used to set the name of a program associated with a timer block. Sent directly after sending a <Set Analogue Timer> or <Set Digital Timer> message. The name is then associated with that timer block. */
#define CEC_OPCODE_TIMER_CLEARED_STATUS           0X43  /**< <Timer Cleared Status> Used to give the status of a <Clear Analogue Timer>, <Clear Digital Timer> or <Clear External Timer> message. */
#define CEC_OPCODE_TIMER_STATUS                   0X35  /**< <Timer Status> Used to send timer status to the initiator of a <Set Timer> message. */
/**< System Information Feature*/
#define CEC_OPCODE_CEC_VERSION                    0X9E  /**< <CEC Version> Used to indicate the supported CEC version, in response to a <Get CEC Version> */
#define CEC_OPCODE_GET_CEC_VERSION                0X9F  /**< <Get CEC Version> Used by a device to enquire which version of CEC the target supports */
#define CEC_OPCODE_GIVE_PHYSICAL_ADDRESS          0X83  /**< <Give Physical Address> A request to a device to return its physical address. */
#define CEC_OPCODE_REPORT_PHYSICAL_ADDRESS        0X84  /**< <Report Physical Address> Used to inform all other devices of the mapping between physical and logical address of the initiator. */
#define CEC_OPCODE_GET_MENU_LANGUAGE              0X91  /**< <Get Menu Language> Sent by a device capable of character generation (for OSD and Menus) to a TV in order to discover the currently selected Menu language. Also used by a TV during installation to discover the currently set menu language of other devices. */
#define CEC_OPCODE_SET_MENU_LANGUAGE              0X32  /**< <Set Menu Language> Used by a TV or another device to indicate the menu language. */
/**< Deck Control Feature*/
#define CEC_OPCODE_DECK_CONTROL                   0X42  /**< <Deck Control> Used to control a device's media functions. */
#define CEC_OPCODE_DECK_STATUS                    0X1B  /**< <Deck Status> Used to provide a deck's status to the initiator of the <Give Deck Status> message. */
#define CEC_OPCODE_GIVE_DECK_STATUS               0X1A  /**< <Give Deck Status> Used to request the status of a device, regardless of whether or not it is the current active source. */
#define CEC_OPCODE_PLAY                           0X41  /**< <Play> Used to control the playback behaviour of a source device. */
/**< Tuner Control Feature*/
#define CEC_OPCODE_GIVE_TUNER_DEVICE_STATUS       0X08  /**< <Give Tuner Device Status> Used to request the status of a tuner device. */
#define CEC_OPCODE_SELECT_ANALOGUE_SERVICE        0X92  /**< <Select Analogue Service> Directly selects an Analogue TV service */
#define CEC_OPCODE_SELECT_DIGITAL_SERVICE         0X93  /**< <Select Digital Service> Directly selects a Digital TV, Radio or Data Broadcast Service */
#define CEC_OPCODE_TUNER_DEVICE_STATUS            0X07  /**< <Tuner Device Status> Use by a tuner device to provide its status to the initiator of the <Give Tuner Device Status> message. */
#define CEC_OPCODE_TUNER_STEP_DECREMENT           0X06  /**< <Tuner Step Decrement> Used to tune to next lowest service in a tuner's service list. Can be used for PIP. */
#define CEC_OPCODE_TUNER_STEP_INCREMENT           0X05  /**< <Tuner Step Increment> Used to tune to next highest service in a tuner's service list. Can be used for PIP. */
/**< Vendor Specific Command*/
#define CEC_OPCODE_DEVICE_VENDOR_ID               0X87  /**< <Device Vendor ID> Reports the vendor ID of this device. */
#define CEC_OPCODE_GIVE_DEVICE_VENDOR_ID          0X8C  /**< <Give Device Vendor ID> Requests the Vendor ID from a device. */
#define CEC_OPCODE_VENDOR_COMMAND                 0X89  /**< <Vendor Command> Allows vendor specific commands to be sent between two devices. */
#define CEC_OPCODE_VENDOR_COMMAND_WITH_ID         0XA0  /**< <Vendor Command With ID> Allows vendor specific commands to be sent between two devices or broadcast. */
#define CEC_OPCODE_VENDOR_REMOTE_BUTTON_DOWN      0X8A  /**< <Vendor Remote Button Down> Indicates that a remote control button has been depressed. */
#define CEC_OPCODE_VENDOR_REMOTE_BUTTON_UP        0X8B  /**< <Vendor Remote Button Up> Indicates that a remote control button (the last button pressed indicated by the Vendor Remote Button Down message) has been released. */
/**< OSD Display Feature*/
#define CEC_OPCODE_SET_OSD_STRING                 0X64  /**< <Set OSD String> Used to send a text message to output on a TV. */
#define CEC_OPCODE_GIVE_OSD_NAME                  0X46  /**< <Give OSD Name> Used to request the preferred OSD name of a device for use in menus associated with that device. */
#define CEC_OPCODE_SET_OSD_NAME                   0X47  /**< <Set OSD Name> Used to set the preferred OSD name of a device for use in menus associated with that device. */
/**< Device Menu Control Feature*/
#define CEC_OPCODE_MENU_REQUEST                   0X8D  /**< <Menu Request> A request from the TV for a device to show/remove a menu or to query if a device is currently showing a menu. */
#define CEC_OPCODE_MENU_STATUS                    0X8E  /**< <Menu Status> Used to indicate to the TV that the device is showing/has removed a menu and requests the remote control keys to be passed though. */
#define CEC_OPCODE_USER_CONTROL_PRESSED           0X44  /**< <User Control Pressed> Used to indicate that the user pressed a remote control button or switched from one remote control button to another. */
#define CEC_OPCODE_USER_CONTROL_RELEASED          0X45  /**< <User Control Released> Indicates that user released a remote control button (the last one indicated by the <User Control Pressed> message) */
/**< Power Status Feature*/
#define CEC_OPCODE_GIVE_DEVICE_POWER_STATUS       0X8F  /**< <Give Device Power Status> Used to determine the current power status of a target device */
#define CEC_OPCODE_REPORT_POWER_STATUS            0X90  /**< <Report Power Status> Used to inform a requesting device of the current power status */
/**< System Audio Control Feature*/
#define CEC_OPCODE_GIVE_AUDIO_STATUS              0X71  /**< <Give Audio Status> Requests an amplifier to send its volume and mute status */
#define CEC_OPCODE_GIVE_SYSTEM_AUDIO_MODE_STATUS  0x7D  /**< <Give System Audio Mode Status> Requests the status of the System Audio Mode */
#define CEC_OPCODE_REPORT_AUDIO_STATUS            0X7A  /**< <Report Audio Status> Reports an amplifier's volume and mute status */
#define CEC_OPCODE_SET_SYSTEM_AUDIO_MODE          0X72  /**< <Set System Audio Mode> Turns the System Audio Mode On or Off. */
#define CEC_OPCODE_SYSTEM_AUDIO_MODE_REQUEST      0X70  /**< <System Audio Mode Request> A device implementing System Audio Control and which has volume control RC buttons (eg TV or STB) requests to use System Audio Mode to the amplifier */
#define CEC_OPCODE_SYSTEM_AUDIO_MODE_STATUS       0X7E  /**< <System Audio Mode Status> Reports the current status of the System Audio Mode */
/**< Audio Rate Control Feature*/
#define CEC_OPCODE_SET_AUDIO_RATE                 0X9A  /**< <Set Audio Rate> Used to control audio rate from Source Device. */

#define CEC_OPCODE_POLLING_MESSAGE                0XFE  /**< <POLL message have no opcode, So, we just use this value. */

typedef enum arUNF_CEC_LOGICALADD_E
{
    AR_CEC_LOGICALADD_TV               = 0X00,
    AR_CEC_LOGICALADD_RECORDDEV_1      = 0X01,
    AR_CEC_LOGICALADD_RECORDDEV_2      = 0X02,
    AR_CEC_LOGICALADD_TUNER_1          = 0X03,
    AR_CEC_LOGICALADD_PLAYDEV_1        = 0X04,
    AR_CEC_LOGICALADD_AUDIOSYSTEM      = 0X05,
    AR_CEC_LOGICALADD_TUNER_2          = 0X06,
    AR_CEC_LOGICALADD_TUNER_3          = 0X07,
    AR_CEC_LOGICALADD_PLAYDEV_2        = 0X08,
    AR_CEC_LOGICALADD_RECORDDEV_3      = 0X09,
    AR_CEC_LOGICALADD_TUNER_4          = 0X0A,
    AR_CEC_LOGICALADD_PLAYDEV_3        = 0X0B,
    AR_CEC_LOGICALADD_RESERVED_1       = 0X0C,
    AR_CEC_LOGICALADD_RESERVED_2       = 0X0D,
    AR_CEC_LOGICALADD_SPECIALUSE       = 0X0E,
    AR_CEC_LOGICALADD_BROADCAST        = 0X0F,
    AR_CEC_LOGICALADD_BUTT
}AR_CEC_LOGICALADD_E;

typedef enum arUNF_CEC_CMDTYPE_E
{
    AR_CEC_STRUCTCOMMAND,
    AR_CEC_RAWCOMMAND,
    AR_CEC_BUTT
}AR_CEC_CMDTYPE_E;

typedef struct arUNF_CEC_RAWDATA_S
{
    AR_U8                              u8Length;
    AR_U8                              u8Data[15];
}AR_CEC_RAWDATA_S;

typedef enum arUNF_CEC_UICMD_E
{
    AR_CEC_UICMD_SELECT                      = 0x00,
    AR_CEC_UICMD_UP                          = 0x01,
    AR_CEC_UICMD_DOWN                        = 0x02,
    AR_CEC_UICMD_LEFT                        = 0x03,
    AR_CEC_UICMD_RIGHT                       = 0x04,
    AR_CEC_UICMD_RIGHT_UP                    = 0x05,
    AR_CEC_UICMD_RIGHT_DOWN                  = 0x06,
    AR_CEC_UICMD_LEFT_UP                     = 0x07,
    AR_CEC_UICMD_LEFT_DOWN                   = 0x08,
    AR_CEC_UICMD_ROOT_MENU                   = 0x09,
    AR_CEC_UICMD_SETUP_MENU                  = 0x0A,
    AR_CEC_UICMD_CONTENTS_MENU               = 0x0B,
    AR_CEC_UICMD_FAVORITE_MENU               = 0x0C,
    AR_CEC_UICMD_EXIT                        = 0x0D,
    AR_CEC_UICMD_NUM_0                       = 0x20,
    AR_CEC_UICMD_NUM_1                       = 0x21,
    AR_CEC_UICMD_NUM_2                       = 0x22,
    AR_CEC_UICMD_NUM_3                       = 0x23,
    AR_CEC_UICMD_NUM_4                       = 0x24,
    AR_CEC_UICMD_NUM_5                       = 0x25,
    AR_CEC_UICMD_NUM_6                       = 0x26,
    AR_CEC_UICMD_NUM_7                       = 0x27,
    AR_CEC_UICMD_NUM_8                       = 0x28,
    AR_CEC_UICMD_NUM_9                       = 0x29,
    AR_CEC_UICMD_DOT                         = 0x2A,
    AR_CEC_UICMD_ENTER                       = 0x2B,
    AR_CEC_UICMD_CLEAR                       = 0x2C,
    AR_CEC_UICMD_NEXT_FAVORITE               = 0x2F,
    AR_CEC_UICMD_CHANNEL_UP                  = 0x30,
    AR_CEC_UICMD_CHANNEL_DOWN                = 0x31,
    AR_CEC_UICMD_PREVIOUS_CHANNEL            = 0x32,
    AR_CEC_UICMD_SOUND_SELECT                = 0x33,
    AR_CEC_UICMD_INPUT_SELECT                = 0x34,
    AR_CEC_UICMD_DISPLAY_INFORMATION         = 0x35,
    AR_CEC_UICMD_HELP                        = 0x36,
    AR_CEC_UICMD_PAGE_UP                     = 0x37,
    AR_CEC_UICMD_PAGE_DOWN                   = 0x38,
    AR_CEC_UICMD_POWER                       = 0x40,
    AR_CEC_UICMD_VOLUME_UP                   = 0x41,
    AR_CEC_UICMD_VOLUME_DOWN                 = 0x42,
    AR_CEC_UICMD_MUTE                        = 0x43,
    AR_CEC_UICMD_PLAY                        = 0x44,
    AR_CEC_UICMD_STOP                        = 0x45,
    AR_CEC_UICMD_PAUSE                       = 0x46,
    AR_CEC_UICMD_RECORD                      = 0x47,
    AR_CEC_UICMD_REWIND                      = 0x48,
    AR_CEC_UICMD_FAST_FORWARD                = 0x49,
    AR_CEC_UICMD_EJECT                       = 0x4A,
    AR_CEC_UICMD_FORWARD                     = 0x4B,
    AR_CEC_UICMD_BACKWARD                    = 0x4C,
    AR_CEC_UICMD_STOP_RECORD                 = 0x4D,
    AR_CEC_UICMD_PAUSE_RECORD                = 0x4E,
    AR_CEC_UICMD_ANGLE                       = 0x50,
    AR_CEC_UICMD_SUBPICTURE                  = 0x51,
    AR_CEC_UICMD_VIDEO_ON_DEMAND             = 0x52,
    AR_CEC_UICMD_ELECTRONIC_PROGRAM_GUIDE    = 0x53,
    AR_CEC_UICMD_TIMER_PROGRAMMING           = 0x54,
    AR_CEC_UICMD_INITIAL_CONFIGURATION       = 0x55,
    AR_CEC_UICMD_PLAY_FUNCTION               = 0x60,
    AR_CEC_UICMD_PAUSE_PLAY_FUNCTION         = 0x61,
    AR_CEC_UICMD_RECORD_FUNCTION             = 0x62,
    AR_CEC_UICMD_PAUSE_RECORD_FUNCTION       = 0x63,
    AR_CEC_UICMD_STOP_FUNCTION               = 0x64,
    AR_CEC_UICMD_MUTE_FUNCTION               = 0x65,
    AR_CEC_UICMD_RESTORE_VOLUME_FUNCTION     = 0x66,
    AR_CEC_UICMD_TUNE_FUNCTION               = 0x67,
    AR_CEC_UICMD_SELECT_MEDIA_FUNCTION       = 0x68,
    AR_CEC_UICMD_SELECT_AV_INPUT_FUNCTION    = 0x69,
    AR_CEC_UICMD_SELECT_AUDIO_INPUT_FUNCTION = 0x6A,
    AR_CEC_UICMD_POWER_TOGGLE_FUNCTION       = 0x6B,
    AR_CEC_UICMD_POWER_OFF_FUNCTION          = 0x6C,
    AR_CEC_UICMD_POWER_ON_FUNCTION           = 0x6D,
    AR_CEC_UICMD_F1_BLUE                     = 0x71,
    AR_CEC_UICMD_F2_RED                      = 0x72,
    AR_CEC_UICMD_F3_GREEN                    = 0x73,
    AR_CEC_UICMD_F4_YELLOW                   = 0x74,
    AR_CEC_UICMD_F5                          = 0x75,
    AR_CEC_UICMD_DATA                        = 0x76
}AR_CEC_UICMD_E;          /**< User Control Code */

typedef struct arUNF_HDMI_CEC_CMD_S
{
    AR_CEC_LOGICALADD_E  enSrcAdd;
    AR_CEC_LOGICALADD_E  enDstAdd;
    AR_U8                u8Opcode;
    AR_CEC_RAWDATA_S     stRawData;
}AR_HDMI_CEC_CMD_S;

typedef struct arUNF_HDMI_CEC_STATUS_S
{
    AR_BOOL bEnable;
    AR_U8   u8PhysicalAddr[4];                       /**< CEC physic addr */
    AR_U8   u8LogicalAddr;                           /**< CEC logic addr, default:0x03. */
    AR_U8   u8Network[AR_CEC_LOGICALADD_BUTT];       /**< CEC's net work, 1:this device can answer cec command */
}AR_HDMI_CEC_STATUS_S;

typedef AR_VOID (*AR_HDMI_CECCALLBACK)(AR_HDMI_ID_E enHdmi, AR_HDMI_CEC_CMD_S *pstCECCmd, AR_VOID *pData);

typedef struct arHDMI_CECCALLBACK_FUNC_S
{
    AR_HDMI_CECCALLBACK  pfnCecCallback;             /**< Event handling callback function */
    AR_VOID             *pPrivateData;               /**< Private data of the CEC callback functions and parameters */
}AR_HDMI_CECCALLBACK_FUNC_S;

typedef enum arHDMI_TRACE_LEN_E
{
    AR_HDMI_TRACE_LEN_0,
    AR_HDMI_TRACE_LEN_1,
    AR_HDMI_TRACE_LEN_2,
    AR_HDMI_TRACE_LEN_BUTT,
}AR_HDMI_TRACE_LEN_E;

typedef struct arHDMI_MOD_PARAM_S
{
    AR_HDMI_TRACE_LEN_E enTraceLen;                   /**< Phy parameters selection. */
}AR_HDMI_MOD_PARAM_S;

typedef enum arERR_HDMI_CODE_E
{
    ERR_HDMI_NOT_INIT              = 1,
    ERR_HDMI_INVALID_PARA          = 2,
    ERR_HDMI_NUL_PTR               = 3,
    ERR_HDMI_DEV_NOT_OPEN          = 4,
    ERR_HDMI_DEV_NOT_CONNECT       = 5,
    ERR_HDMI_READ_SINK_FAILED      = 6,
    ERR_HDMI_INIT_ALREADY          = 7,
    ERR_HDMI_CALLBACK_ALREADY      = 8,
    ERR_HDMI_INVALID_CALLBACK      = 9,
    ERR_HDMI_FEATURE_NO_SUPPORT    = 10,
    ERR_HDMI_BUS_BUSY              = 11,
    ERR_HDMI_READ_EVENT_FAILED     = 12,
    ERR_HDMI_NOT_START             = 13,
    ERR_HDMI_READ_EDID_FAILED      = 14,
    ERR_HDMI_INIT_FAILED           = 15,
    ERR_HDMI_CREATE_TESK_FAILED    = 16,
    ERR_HDMI_MALLOC_FAILED         = 17,
    ERR_HDMI_FREE_FAILED           = 18,
    ERR_HDMI_PTHREAD_CREATE_FAILED = 19,
    ERR_HDMI_PTHREAD_JOIN_FAILED   = 20,
    ERR_HDMI_STRATEGY_FAILED       = 21,
    ERR_HDMI_SET_ATTR_FAILED       = 22,
    ERR_HDMI_CALLBACK_NOT_REGISTER = 23,
    ERR_HDMI_CEC_CALLBACK_REREGISTER = 24,
    ERR_HDMI_UNKNOWN_COMMAND       = 25,
    ERR_HDMI_MUTEX_LOCK_FAILED     = 26,
    ERR_HDMI_CEC_NOT_ENABLE        = 27,
    ERR_HDMI_CECCALLBACK_NOT_REG   = 28,

    ERR_HDMI_BUTT,
} ERR_HDMI_CODE_E;

#define AR_ERR_HDMI_NOT_INIT                AR_MPP_DEF_ERR(AR_ID_HDMI, HAL_ERR_LEVEL_ERROR, ERR_HDMI_NOT_INIT)
#define AR_ERR_HDMI_INVALID_PARA            AR_MPP_DEF_ERR(AR_ID_HDMI, HAL_ERR_LEVEL_ERROR, ERR_HDMI_INVALID_PARA)
#define AR_ERR_HDMI_NUL_PTR                 AR_MPP_DEF_ERR(AR_ID_HDMI, HAL_ERR_LEVEL_ERROR, ERR_HDMI_NUL_PTR)
#define AR_ERR_HDMI_DEV_NOT_OPEN            AR_MPP_DEF_ERR(AR_ID_HDMI, HAL_ERR_LEVEL_ERROR, ERR_HDMI_DEV_NOT_OPEN)
#define AR_ERR_HDMI_DEV_NOT_CONNECT         AR_MPP_DEF_ERR(AR_ID_HDMI, HAL_ERR_LEVEL_ERROR, ERR_HDMI_DEV_NOT_CONNECT)
#define AR_ERR_HDMI_READ_SINK_FAILED        AR_MPP_DEF_ERR(AR_ID_HDMI, HAL_ERR_LEVEL_ERROR, ERR_HDMI_READ_SINK_FAILED)
#define AR_ERR_HDMI_INIT_ALREADY            AR_MPP_DEF_ERR(AR_ID_HDMI, HAL_ERR_LEVEL_ERROR, ERR_HDMI_INIT_ALREADY)
#define AR_ERR_HDMI_CALLBACK_ALREADY        AR_MPP_DEF_ERR(AR_ID_HDMI, HAL_ERR_LEVEL_ERROR, ERR_HDMI_CALLBACK_ALREADY)
#define AR_ERR_HDMI_INVALID_CALLBACK        AR_MPP_DEF_ERR(AR_ID_HDMI, HAL_ERR_LEVEL_ERROR, ERR_HDMI_INVALID_CALLBACK)
#define AR_ERR_HDMI_FEATURE_NO_SUPPORT      AR_MPP_DEF_ERR(AR_ID_HDMI, HAL_ERR_LEVEL_ERROR, ERR_HDMI_FEATURE_NO_SUPPORT)
#define AR_ERR_HDMI_BUS_BUSY                AR_MPP_DEF_ERR(AR_ID_HDMI, HAL_ERR_LEVEL_ERROR, ERR_HDMI_BUS_BUSY)
#define AR_ERR_HDMI_READ_EVENT_FAILED       AR_MPP_DEF_ERR(AR_ID_HDMI, HAL_ERR_LEVEL_ERROR, ERR_HDMI_READ_EVENT_FAILED)
#define AR_ERR_HDMI_NOT_START               AR_MPP_DEF_ERR(AR_ID_HDMI, HAL_ERR_LEVEL_ERROR, ERR_HDMI_NOT_START)
#define AR_ERR_HDMI_READ_EDID_FAILED        AR_MPP_DEF_ERR(AR_ID_HDMI, HAL_ERR_LEVEL_ERROR, ERR_HDMI_READ_EDID_FAILED)
#define AR_ERR_HDMI_INIT_FAILED             AR_MPP_DEF_ERR(AR_ID_HDMI, HAL_ERR_LEVEL_ERROR, ERR_HDMI_INIT_FAILED)
#define AR_ERR_HDMI_CREATE_TESK_FAILED      AR_MPP_DEF_ERR(AR_ID_HDMI, HAL_ERR_LEVEL_ERROR, ERR_HDMI_CREATE_TESK_FAILED)
#define AR_ERR_HDMI_MALLOC_FAILED           AR_MPP_DEF_ERR(AR_ID_HDMI, HAL_ERR_LEVEL_ERROR, ERR_HDMI_MALLOC_FAILED)
#define AR_ERR_HDMI_FREE_FAILED             AR_MPP_DEF_ERR(AR_ID_HDMI, HAL_ERR_LEVEL_ERROR, ERR_HDMI_FREE_FAILED)
#define AR_ERR_HDMI_PTHREAD_CREATE_FAILED   AR_MPP_DEF_ERR(AR_ID_HDMI, HAL_ERR_LEVEL_ERROR, ERR_HDMI_PTHREAD_CREATE_FAILED)
#define AR_ERR_HDMI_PTHREAD_JOIN_FAILED     AR_MPP_DEF_ERR(AR_ID_HDMI, HAL_ERR_LEVEL_ERROR, ERR_HDMI_PTHREAD_JOIN_FAILED)
#define AR_ERR_HDMI_STRATEGY_FAILED         AR_MPP_DEF_ERR(AR_ID_HDMI, HAL_ERR_LEVEL_ERROR, ERR_HDMI_STRATEGY_FAILED)
#define AR_ERR_HDMI_SET_ATTR_FAILED         AR_MPP_DEF_ERR(AR_ID_HDMI, HAL_ERR_LEVEL_ERROR, ERR_HDMI_SET_ATTR_FAILED)
#define AR_ERR_HDMI_CEC_CALLBACK_REREGISTER AR_MPP_DEF_ERR(AR_ID_HDMI, HAL_ERR_LEVEL_ERROR, ERR_HDMI_CEC_CALLBACK_REREGISTER)
#define AR_ERR_HDMI_CALLBACK_NOT_REGISTER   AR_MPP_DEF_ERR(AR_ID_HDMI, HAL_ERR_LEVEL_ERROR, ERR_HDMI_CALLBACK_NOT_REGISTER)
#define AR_ERR_HDMI_UNKNOWN_COMMAND         AR_MPP_DEF_ERR(AR_ID_HDMI, HAL_ERR_LEVEL_ERROR, ERR_HDMI_UNKNOWN_COMMAND)
#define AR_ERR_HDMI_MUTEX_LOCK_FAILED       AR_MPP_DEF_ERR(AR_ID_HDMI, HAL_ERR_LEVEL_ERROR, ERR_HDMI_MUTEX_LOCK_FAILED)
#define AR_ERR_HDMI_CEC_NOT_ENABLE          AR_MPP_DEF_ERR(AR_ID_HDMI, HAL_ERR_LEVEL_ERROR, ERR_HDMI_CEC_NOT_ENABLE)
#define AR_ERR_HDMI_CECCALLBACK_NOT_REG     AR_MPP_DEF_ERR(AR_ID_HDMI, HAL_ERR_LEVEL_ERROR, ERR_HDMI_CECCALLBACK_NOT_REG)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __AR_COMM_VO_H__ */

