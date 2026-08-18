/**
 * \file
 * \brief 描述视频编码相关的通用数据结构
 */

#ifndef __AR_COMM_VENC_H__
#define __AR_COMM_VENC_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#include "hal_type.h"
#include "ar_common.h"
#include "hal_errno.h"
#include "ar_comm_video.h"
#include "ar_comm_rc.h"
#include "ar_comm_vb.h"
/********************************Macro Definition********************************/

/**< 错误码：通道 ID 超出合法范围 */
#define AR_ERR_VENC_INVALID_CHNID     AR_MPP_DEF_ERR(AR_ID_VENC, HAL_ERR_LEVEL_ERROR, HAL_ERR_INVALID_CHNID)
/**< 错误码：参数超出合法范围  */
#define AR_ERR_VENC_ILLEGAL_PARAM     AR_MPP_DEF_ERR(AR_ID_VENC, HAL_ERR_LEVEL_ERROR, HAL_ERR_ILLEGAL_PARAM)
/**< 错误码：试图申请或者创建已经存在的设备、通道或者资源 */
#define AR_ERR_VENC_EXIST             AR_MPP_DEF_ERR(AR_ID_VENC, HAL_ERR_LEVEL_ERROR, HAL_ERR_EXIST)
/**< 错误码：试图使用或者销毁不存在的设备、通道或者资源 */
#define AR_ERR_VENC_UNEXIST           AR_MPP_DEF_ERR(AR_ID_VENC, HAL_ERR_LEVEL_ERROR, HAL_ERR_UNEXIST)
/**< 错误码：函数参数中有空指针 */
#define AR_ERR_VENC_NULL_PTR          AR_MPP_DEF_ERR(AR_ID_VENC, HAL_ERR_LEVEL_ERROR, HAL_ERR_NULL_PTR)
/**< 错误码：使用前未初始化系统，设备或通道。*/
#define AR_ERR_VENC_NOT_CONFIG        AR_MPP_DEF_ERR(AR_ID_VENC, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_CONFIG)
/**< 错误码：不支持的操作或者功能 */
#define AR_ERR_VENC_NOT_SUPPORT       AR_MPP_DEF_ERR(AR_ID_VENC, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_SUPPORT)
/**< 错误码： 该操作不允许，如试图修改静态配置参数 */
#define AR_ERR_VENC_NOT_PERM          AR_MPP_DEF_ERR(AR_ID_VENC, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_PERM)
/**< 错误码：分配内存失败，如系统内存不足 */
#define AR_ERR_VENC_NOMEM             AR_MPP_DEF_ERR(AR_ID_VENC, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOMEM)
/**< 错误码：分配缓存失败，如申请的数据缓存去太大 */
#define AR_ERR_VENC_NOBUF             AR_MPP_DEF_ERR(AR_ID_VENC, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOBUF)
/**< 错误码：缓冲区中无数据 */
#define AR_ERR_VENC_BUF_EMPTY         AR_MPP_DEF_ERR(AR_ID_VENC, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUF_EMPTY)
/**< 错误码：缓冲区中数据满 */
#define AR_ERR_VENC_BUF_FULL          AR_MPP_DEF_ERR(AR_ID_VENC, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUF_FULL)
/**< 错误码：系统没有初始化或没有加载相应模块 */
#define AR_ERR_VENC_SYS_NOTREADY      AR_MPP_DEF_ERR(AR_ID_VENC, HAL_ERR_LEVEL_ERROR, HAL_ERR_SYS_NOTREADY)
/**< 错误码：VENC系统忙 */
#define AR_ERR_VENC_BUSY              AR_MPP_DEF_ERR(AR_ID_VENC, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUSY)

/**
* @note  定义编码扩展参数比特位定义。
*/
typedef enum {
    AR_VIDEO_CODEC_PARAM_MPI_EXT_INSTANCE_PARALLESIM = 1, /**< 编码实例之间的并行编码开关, 26x有效，jpeg不支持*/
} ENUM_MPI_VENC_EXTENSION_FLAGS;

/**< 定义H.264码流NALU类型。*/
typedef enum arH264E_NALU_TYPE_E
{
    H264E_NALU_BSLICE = 0,                         /**< B SLICE 类型 */
    H264E_NALU_PSLICE = 1,                         /**< P SLICE 类型   */
    H264E_NALU_ISLICE = 2,                         /**< I SLICE 类型  ，帧类型为P帧       */
    H264E_NALU_IDRSLICE = 5,                       /**< IDR SLICE 类型，帧类型为IDR帧         */
    H264E_NALU_SEI    = 6,                         /**< SEI 类型   */
    H264E_NALU_SPS    = 7,                         /**< SPS 类型   */
    H264E_NALU_PPS    = 8,                         /**< PPS 类型   */
    H264E_NALU_BUTT
} H264E_NALU_TYPE_E;

/**< 定义H.265码流NALU类型。*/
typedef enum arH265E_NALU_TYPE_E
{
    H265E_NALU_BSLICE = 0,                          /**< B SLICE 类型 */
    H265E_NALU_PSLICE = 1,                          /**< P SLICE 类型 */
    H265E_NALU_ISLICE = 2,                          /**< I SLICE 类型  ，帧类型为P帧 */
    H265E_NALU_IDRSLICE = 19,                       /**< IDR SLICE 类型，帧类型为IDR帧 */
    H265E_NALU_VPS    = 32,                         /**< VPS 类型 */
    H265E_NALU_SPS    = 33,                         /**< SPS 类型 */
    H265E_NALU_PPS    = 34,                         /**< PPS 类型 */
    H265E_NALU_SEI    = 39,                         /**< SEI 类型 */

    H265E_NALU_BUTT
} H265E_NALU_TYPE_E;

/**< 定义H.264编码条带参考类型。*/
typedef enum arH264E_REFSLICE_TYPE_E
{
    H264E_REFSLICE_FOR_1X = 1,                     /**< H264E_REF_MODE_1X参考条带类型 */
    H264E_REFSLICE_FOR_2X = 2,                     /**< H264E_REF_MODE_2X参考条带类型 */
    H264E_REFSLICE_FOR_4X = 5,                     /**< H264E_REF_MODE_4X参考条带类型 */
    H264E_REFSLICE_FOR_BUTT
} H264E_REFSLICE_TYPE_E;

/**< 定义JPEG码流的PACK类型。*/
typedef enum arJPEGE_PACK_TYPE_E
{
    JPEGE_PACK_ECS = 5,                            /**< ECS 类型 */
    JPEGE_PACK_APP = 6,                            /**< APP 类型 */
    JPEGE_PACK_VDO = 7,                            /**< VDO 类型 */
    JPEGE_PACK_PIC = 8,                            /**< PIC 类型 */
    JPEGE_PACK_DCF = 9,                            /**< DCF ECS类型 */
    JPEGE_PACK_DCF_PIC = 10,                       /**< DCF 类型 */
    JPEGE_PACK_BUTT
} JPEGE_PACK_TYPE_E;

/**< 定义PRORES码流的PACK类型。*/
typedef enum arPRORES_PACK_TYPE_E
{
    PRORES_PACK_PIC = 1,                            /**< PRORES 数据包 */
    PRORES_PACK_BUTT
} PRORES_PACK_TYPE_E;

/**< 定义码流结果类型 */
typedef union arVENC_DATA_TYPE_U
{
    H264E_NALU_TYPE_E    enH264EType;               /**< H.264 码流包类型 */
    JPEGE_PACK_TYPE_E    enJPEGEType;               /**< JPEG 码流包类型 */
    H265E_NALU_TYPE_E    enH265EType;               /**< H.265 码流包类型 */
    PRORES_PACK_TYPE_E   enPRORESType;              /**< PRORES 码流包类型 */
} VENC_DATA_TYPE_U;

/**< 定义当前码流数据中包含的其他类型码流包数据的结构体 */
typedef struct arVENC_PACK_INFO_S
{
    VENC_DATA_TYPE_U  u32PackType;                    /**< 当前码流包数据包含其他码流包的类型。 */
    AR_U32 u32PackOffset;                             /**< 当前码流包数据包含其他码流包数据的偏移。 */
    AR_U32 u32PackLength;                             /**< 当前码流包数据包含其他码流包数据的大小。 */
} VENC_PACK_INFO_S;

/**< 定义帧码流包结构体 */
typedef struct arVENC_PACK_S
{
    AR_U64               u64PhyAddr;                 /**< 码流包物理地址。 */
    AR_U8                ATTRIBUTE* pu8Addr;         /**< 码流包首地址。 */
    AR_U32               ATTRIBUTE u32Len;           /**< 码流包长度。 */

    AR_U64               u64PTS;                     /**< 时间戳。单位：us。 */
    AR_BOOL              bFrameEnd;                  /**< 帧结束标识。
                                                          取值范围：
                                                          HI_TRUE：该码流包是该帧的最后一个包。
                                                          HI_FALSE：该码流包不是该帧的最后一个包。*/

    VENC_DATA_TYPE_U     DataType;                   /**< 码流类型 */
    AR_U32               u32Offset;                  /**< 码流包中有效数据与码流包首地址pu8Addr的偏移 */
    AR_U32               u32DataNum;                 /**< 当前码流包（当前包的类型有DataType指定）数据中包含其他类型码流包的个数 */
    VENC_PACK_INFO_S     stPackInfo[8];              /**< 当前码流报数据中包含其他类型码流包数据信息 */
} VENC_PACK_S;

/**< 定义H.264跳帧参考码流的帧类型以及参考属性。*/
typedef enum arH264E_REF_TYPE_E
{
    BASE_IDRSLICE = 0,                              /**< base层中的IDR帧。*/
    BASE_PSLICE_REFTOIDR,                           /**< base层中的P帧，用于base层中其他帧的参考且只参考IDR帧。*/
    BASE_PSLICE_REFBYBASE,                          /**< base层中的P帧，用于base层中其他帧的参考。*/
    BASE_PSLICE_REFBYENHANCE,                       /**< base层中的P帧，永远忽enhance层中的帧的参考。*/
    ENHANCE_PSLICE_REFBYENHANCE,                    /**< enhance层中的P帧，用于enhance层中其他帧的参考。*/
    ENHANCE_PSLICE_NOTFORREF,                       /**< enhance层中P帧，不用于参考。*/
    ENHANCE_PSLICE_BUTT
} H264E_REF_TYPE_E;

typedef enum arH264E_REF_TYPE_E H265E_REF_TYPE_E;

/**< 定义H.264协议码流特征信息。*/
typedef struct arVENC_STREAM_INFO_H264_S
{
    AR_U32                 u32PicBytesNum;              /**< 只读; 编码当前帧的自己（Byte）数。 */
    AR_U32                 u32Inter16x16MbNum;          /**< 只读; 编码当前帧中采用Inter16x16 预测模式的宏块数。 */
    AR_U32                 u32Inter8x8MbNum;            /**< 只读; 编码当前帧中采用Inter8x8 预测模式的宏块数。*/
    AR_U32                 u32Intra16MbNum;             /**< 只读; 编码当前帧中采用Intra16 预测模式的宏块数。 */
    AR_U32                 u32Intra8MbNum;              /**< 只读; 编码当前帧中采用Intra8 预测模式的宏块数。 */
    AR_U32                 u32Intra4MbNum;              /**< 只读; 编码当前帧中采用Inter4 预测模式的宏块数。 */

    H264E_REF_TYPE_E       enRefType;                   /**< 只读; 高级跳帧参考下的编码帧类型。 */
    AR_U32                 u32UpdateAttrCnt;            /**< 只读; 通道属性或参数（包含RC参数）被设置的次数。 */
    AR_U32                 u32StartQp;                  /**< 只读; 编码当前帧的startQp值。 */
    AR_U32                 u32MeanQp;                   /**< 只读; 编码当前帧的平均QP帧。 */
    AR_BOOL                bPSkip;                      /**< 只读; 标识当前帧是否为Pskip帧。 */
} VENC_STREAM_INFO_H264_S;

/**< 定义H.265协议码流特征信息。*/
typedef struct arVENC_STREAM_INFO_H265_S
{
    AR_U32                 u32PicBytesNum;              /**< 只读; 编码当前帧的自己（Byte）数。 */
    AR_U32                 u32Inter64x64CuNum;          /**< 只读; 编码当前帧中采用Inter64x64 预测模式的CU块数。*/
    AR_U32                 u32Inter32x32CuNum;          /**< 只读; 编码当前帧中采用Inter32x32 预测模式的CU块数                 。*/
    AR_U32                 u32Inter16x16CuNum;          /**< 只读; 编码当前帧中采用Inter16x16 预测模式的CU块数。    */
    AR_U32                 u32Inter8x8CuNum;            /**< 只读; 编码当前帧中采用Inter8x8 预测模式的CU块数。  */
    AR_U32                 u32Intra32x32CuNum;          /**< 只读; 编码当前帧中采用Intra32x32 预测模式的CU块数。  */
    AR_U32                 u32Intra16x16CuNum;          /**< 只读; 编码当前帧中采用Intra16x16 预测模式的CU块数。*/
    AR_U32                 u32Intra8x8CuNum;            /**< 只读; 编码当前帧中采用Intra8x8 预测模式的CU块数。  */
    AR_U32                 u32Intra4x4CuNum;            /**< 只读; 编码当前帧中采用Intra4x4 预测模式的CU块数。  */

    H265E_REF_TYPE_E       enRefType;                   /**< 只读; 高级跳帧参考下的编码帧类型。 */
    AR_U32                 u32UpdateAttrCnt;            /**< 只读; 通道属性或参数（包含RC参数）被设置的次数。 */
    AR_U32                 u32StartQp;                  /**< 只读; 编码当前帧的startQp值。 */
    AR_U32                 u32MeanQp;                   /**< 只读; 编码当前帧的平均QP帧 */
    AR_BOOL                bPSkip;                      /**< 只读; 标识当前帧是否为Pskip帧。 */
} VENC_STREAM_INFO_H265_S;

/**<定义H264/H265协议SSE信息。*/
typedef struct arVENC_SSE_INFO_S
{
    AR_BOOL bSSEEn;         /**< 可读写; 范围:[0,1]; 区域SSE使能。 */
    AR_U32  u32SSEVal;      /**< 只读;  区域SSE值。 */
} VENC_SSE_INFO_S;

/**< 定义H.264 协议码流高级特征信息。*/
typedef struct arVENC_STREAM_ADVANCE_INFO_H264_S
{
    AR_U32             u32ResidualBitNum;    /**< 只读; 编码当前帧残差（bit）数。*/
    AR_U32             u32HeadBitNum;        /**< 只读; 编码当前帧头信息的（bit）数。*/
    AR_U32             u32MadiVal;           /**< 只读; 编码当前帧空域纹理复杂度Madi值。*/
    AR_U32             u32MadpVal;           /**< 只读;   编码当前帧时域运动负责度Madp值。*/
    AR_DOUBLE          dPSNRVal;             /**< 只读; 编码当前帧的PSNR（峰值信噪比）值。*/
    AR_U32             u32MseLcuCnt;         /**< 只读; 编码当前帧中LCU个数。*/
    AR_U32             u32MseSum;            /**< 只读; 编码当前帧中MSE（均方差）值。*/
    VENC_SSE_INFO_S    stSSEInfo[8];         /**< 只读; 编码当前帧中8个区域的SSE（和方差）值。*/
    AR_U32             u32QpHstgrm[52];      /**< 只读; 编码当前帧Qp直方图。*/
    AR_U32             u32MoveScene16x16Num; /**< 只读; 判断为图像前景的16x16块的数目，需要开启前景宏块级码率控制。*/
    AR_U32             u32MoveSceneBits;     /**< 只读; 判断为图像前景区域编码bit数，需要开启前景宏块级码率控制。*/
} VENC_STREAM_ADVANCE_INFO_H264_S;

/**< 定义JPEG/MJPEG 协议码流高级特征信息。*/
typedef struct arVENC_STREAM_ADVANCE_INFO_JPEG_S
{
    // AR_U32 u32Reserved;
} VENC_STREAM_ADVANCE_INFO_JPEG_S;

/**< 定义PRORES 协议码流高级特征信息。*/
typedef struct arVENC_STREAM_ADVANCE_INFO_PRORES_S
{
    // AR_U32 u32Reserved;
} VENC_STREAM_ADVANCE_INFO_PRORES_S;

/**< 定义H.265 协议码流高级特征信息。*/
typedef struct arVENC_STREAM_ADVANCE_INFO_H265_S
{
    AR_U32             u32ResidualBitNum;          /**< 只读; 编码当前帧残差（bit）数。*/
    AR_U32             u32HeadBitNum;              /**< 只读; 编码当前帧头信息的（bit）数。*/
    AR_U32             u32MadiVal;                 /**< 只读; 编码当前帧空域纹理复杂度Madi值。*/
    AR_U32             u32MadpVal;                 /**< 只读; 编码当前帧时域运动负责度Madp值。*/
    AR_DOUBLE          dPSNRVal;                   /**< 只读; 编码当前帧的PSNR（峰值信噪比）值。*/
    AR_U32             u32MseLcuCnt;               /**< 只读; 编码当前帧中LCU个数。*/
    AR_U32             u32MseSum;                  /**< 只读; 编码当前帧中MSE（均方差）值。*/
    VENC_SSE_INFO_S    stSSEInfo[8];               /**< 只读; 编码当前帧中8个区域的SSE（和方差）值。*/
    AR_U32             u32QpHstgrm[52];            /**< 只读; 编码当前帧Qp直方图。*/
    AR_U32             u32MoveScene32x32Num;       /**< 只读; 判断为图像前景的32x32块的数目，需要开启前景宏块级码率控制 */
    AR_U32             u32MoveSceneBits;           /**< 只读; 判断为图像前景区域编码bit数，需要开启前景宏块级码率控制。*/
} VENC_STREAM_ADVANCE_INFO_H265_S;

/**< 定义PRORES 协议码流特征信息。*/
typedef struct arVENC_STREAM_INFO_PRORES_S
{
    AR_U32 u32PicBytesNum;                         /**< 编码当前帧的字节（bit）数。*/
    AR_U32 u32UpdateAttrCnt;                       /**< 通道属性或参数被设置的册数。*/
} VENC_STREAM_INFO_PRORES_S;

/**< 定义JPEG/MJPEG 协议码流特征信息。*/
typedef struct arVENC_STREAM_INFO_JPEG_S
{
    AR_U32 u32PicBytesNum;                      /**< 只读; 一帧JPEG码流大小，以字节（byte）为单位 */
    AR_U32 u32UpdateAttrCnt;                    /**< 只读; 通道属性或参数（包含RC参数）被设置的次数。*/
    AR_U32 u32Qfactor;                          /**< 只读; 编码当前帧的Qfacetor。*/
} VENC_STREAM_INFO_JPEG_S;

/**< 定义帧码流类型的结构体。*/
typedef struct arVENC_STREAM_S
{
    VENC_PACK_S ATTRIBUTE* pstPack;                                 /**< 只读; 帧码流包结构。*/
    AR_U32      ATTRIBUTE u32PackCount;                             /**< 只读; 一帧码流的所有包的个数。*/
    AR_U32      u32Seq;                                             /**< 只读; 码流序列号。按帧获取序列号; 按包获取包序列号。*/

    union
    {
        VENC_STREAM_INFO_H264_S   stH264Info;                        /**< 只读; h264码流特征信息。*/
        VENC_STREAM_INFO_JPEG_S   stJpegInfo;                        /**< 只读; jpeg码流特征信息。 */
        VENC_STREAM_INFO_H265_S   stH265Info;                        /**< 只读; h265码流特征信息。 */
        VENC_STREAM_INFO_PRORES_S stProresInfo;                      /**< 只读; prores码流特征信息。 */
    };

    union
    {
        VENC_STREAM_ADVANCE_INFO_H264_S   stAdvanceH264Info;         /**< 只读; h264码流高级特征信息。 */
        VENC_STREAM_ADVANCE_INFO_JPEG_S   stAdvanceJpegInfo;         /**< 只读; jpeg码流高级特征信息。 */
        VENC_STREAM_ADVANCE_INFO_H265_S   stAdvanceH265Info;         /**< 只读; h265码流高级特征信息。 */
        VENC_STREAM_ADVANCE_INFO_PRORES_S stAdvanceProresInfo;       /**< 只读; prores码流高级特征信息。 */
    };
} VENC_STREAM_S;

/**< 定义编码器码流特征信息，供外部查询使用。*/
typedef struct arVENC_STREAM_INFO_S
{
    H265E_REF_TYPE_E enRefType;  /**< 高级跳帧参考下的编码类型，H.264和H.265枚举的定义完全一样，所以H264编码通道也使用这个变量。*/

    AR_U32  u32PicBytesNum;      /**< 编码当前帧的自己（byte）数。*/
    AR_U32  u32PicCnt;           /**< 按帧获取码流使，表示编码帧计数; 按报获取码流使，表示编码包计数。*/
    AR_U32  u32StartQp;          /**< 编码当前帧的startQp值。*/
    AR_U32  u32MeanQp;           /**< 编码当前帧的平均QP值。*/
    AR_BOOL bPSkip;              /**< 当前帧是否为PSkip帧。*/

    AR_U32  u32ResidualBitNum;   /**< 编码当前帧残差（bit）数。*/
    AR_U32  u32HeadBitNum;       /**< 编码当前帧头信息的（bit）数。*/
    AR_U32  u32MadiVal;          /**< 编码当前帧空域纹理复杂度Madi值。*/
    AR_U32  u32MadpVal;          /**< 编码当前帧时域运动复杂度Madp值。*/
    AR_U32  u32MseSum;           /**< 编码当前帧中MSE（均方差）值。*/
    AR_U32  u32MseLcuCnt;        /**< 编码当前帧中LCU 个数。*/
    AR_DOUBLE dPSNRVal;          /**< 编码当前帧的PSNR（峰值信噪比）值。*/
} VENC_STREAM_INFO_S;

/**< 定义JPEG MPF结构体。*/
typedef struct arVENC_MPF_CFG_S
{
    AR_U8   u8LargeThumbNailNum;        /**< 可读写; 编码MPF缩略图的个数; 取值范围:[0,2]; 0: 代表不编码MPF; 静态属性。*/
    SIZE_S  astLargeThumbNailSize[2];   /**< 可读写; MPF缩略图的大小; 静态参数。*/
} VENC_MPF_CFG_S;

/**< 定义RECEIVE MODE结构体。*/
typedef enum arVENC_PIC_RECEIVE_MODE_E
{
    VENC_PIC_RECEIVE_SINGLE = 0,        /**< 当前编码通道值允许从一个源接收图像; 静态参数。*/
    VENC_PIC_RECEIVE_MULTI,             /**< 当前编码通道允许从多个源接收图像; 静态属性。*/

    VENC_PIC_RECEIVE_BUTT
}VENC_PIC_RECEIVE_MODE_E;

/**< 定义JPEG抓拍属性结构体。*/
typedef struct arVENC_ATTR_JPEG_S
{
    AR_BOOL                     bSupportDCF;        /**< 可读写; 是否是能DFC（Design rule for Camera File system）;
                                                         取值范围:[0,1]; 静态属性。*/
    VENC_MPF_CFG_S              stMPFCfg;           /**< 可读写; 编码JPEG MPF配置; 取值范围:[0,1]; 静态属性。*/
    VENC_PIC_RECEIVE_MODE_E     enReceiveMode;      /**< 可读写; JPEG通道的图像接收模式; 静态属性。*/
} VENC_ATTR_JPEG_S;

/**< 定义MJPEG抓拍属性结构体。*/
typedef struct arVENC_ATTR_MJPEG_S
{
    //reserved
} VENC_ATTR_MJPEG_S;

/**< 定义H.264编码属性结构体。*/
typedef struct arVENC_ATTR_H264_S
{
    AR_BOOL bRcnRefShareBuf;            /**<@note 暂不支持; 可读写; 取值范围:[0, 1]; Whether to enable the Share Buf of Rcn and Ref; */
    AR_U32  u32CmdQueueDepth;           /**<@attention 新添加参数; 设置基于帧级的并行，编码器最多可以同时编码的帧数; 仅对HEVC和AVC有效; 可读写; 取值范围:[2, 4]; */
    AR_S32  s32SubFrameSyncEnable;      /**<@attention 新添加参数; 是否使能编码器与vif或isp的帧同步编码; 需要lowdelay时使能; 可读写; 取值范围:[0,1]; */
    AR_S32  s32SubFrameSyncSrc;         /**<@attention 新添加参数; 需要VIN模块同步配置好; 可读写; 取值范围:[0,2]; 0:关闭 1:ISP 2:VIF; */
    AR_S32  s32Cframe50Enable;          /**<@attention 新添加参数; 源图像是否是cf50格式; 可读写; 静态属性 */
    AR_S32  s32Cframe50LosslessEnable;  /**<@attention 新添加参数; 是否使能cf50的无损压缩; 可读写; 静态属性 */
    AR_S32  s32Cframe50Tx16Y;           /**<@attention 新添加参数; 亮度的压缩比例; 取值范围:[1, 128]; 静态属性 */
    AR_S32  s32Cframe50Tx16C;           /**<@attention 新添加参数; 色度的压缩比例; 取值范围:[1, 128]; 静态属性 */
    AR_S32  s32Cframe50_422;            /**<@attention 新添加参数; 是否使能422到420的转化; 静态属性 */
    AR_U8   u8KeyFrameSizeMultiplier;   /**<@attention 新添加参数; 关键帧buffer的size与码率的乘子,计算公式 roundup (picWidth*picHeight*3/2)/(bitrate>>3); 静态属性 */
    AR_U8   u8NonKeyFrameSizeMultiplier;    /**<@attention 新添加参数; 非关键帧buffer的size与码率的乘子,计算公式roundup (picWidth*picHeight*3/2)/(bitrate>>3); 静态属性 */
} VENC_ATTR_H264_S;

/**< 定义H.265编码属性结构体。*/
typedef struct arVENC_ATTR_H265_S
{
    AR_BOOL bRcnRefShareBuf;            /**<@note 暂不支持; 可读写; 取值范围:[0, 1]; Whether to enable the Share Buf of Rcn and Ref; */
    AR_U32  u32CmdQueueDepth;           /**<@attention 新添加参数; 设置基于帧级的并行，编码器最多可以同时编码的帧数; 仅对HEVC和AVC有效; 可读写; 取值范围:[2, 4]; */
    AR_S32  s32SubFrameSyncEnable;      /**<@attention 新添加参数; 需要lowdelay时使能; 可读写; 取值范围:[0,1]; 是否使能编码器与vif或isp的帧同步编码; */
    AR_S32  s32SubFrameSyncSrc;         /**<@attention 新添加参数; 需要VIN模块同步配置好; 可读写; 取值范围:[0,2]; 0:关闭 1:ISP 2:VIF; */
    AR_S32  s32Cframe50Enable;          /**<@attention 新添加参数; 源图像是否是cf50格式; 可读写; 取值范围:[0,1];  静态属性 */
    AR_S32  s32Cframe50LosslessEnable;  /**<@attention 新添加参数; 是否使能cf50的无损压缩; 可读写; 取值范围:[0,1]; 静态属性 */
    AR_S32  s32Cframe50Tx16Y;           /**<@attention 新添加参数; 亮度的压缩比例; 可读写; 取值范围:[0,1]; 静态属性 */
    AR_S32  s32Cframe50Tx16C;           /**<@attention 新添加参数; 色度的压缩比例; 静态属性 */
    AR_S32  s32Cframe50_422;            /**<@attention 新添加参数; 是否使能422到420的转化; 静态属性 */
    AR_U8   u8KeyFrameSizeMultiplier;   /**<@attention 新添加参数; 关键帧buffer的size与码率的乘子; 静态属性 */
    AR_U8   u8NonKeyFrameSizeMultiplier;    /**<@attention 新添加参数; 非关键帧buffer的size与码率的乘子; 静态属性 */
} VENC_ATTR_H265_S;

/**< 定义PRORES帧定义。*/
typedef enum arPRORES_FRAMERATE
{
    PRORES_FR_UNKNOWN = 0,              /**< 未知/未声明 */
    PRORES_FR_23_976,                   /**< 24/1.001（23.976） */
    PRORES_FR_24,                       /**< 24 */
    PRORES_FR_25,                       /**< 25 */
    PRORES_FR_29_97,                    /**< 30/1.001（29.97） */
    PRORES_FR_30,                       /**< 30 */
    PRORES_FR_50,                       /**< 50 */
    PRORES_FR_59_94,                    /**< 60/1.001（59.94） */
    PRORES_FR_60,                       /**< 60 */
    PRORES_FR_100,                      /**< 100 */
    PRORES_FR_119_88,                   /**< 120/1.001（119.88） */
    PRORES_FR_120,                      /**< 120 */
    PRORES_FR_BUTT
}PRORES_FRAMERATE;

/**< 定义PRORES幅型比定义。*/
typedef enum arPRORES_ASPECT_RATIO
{
    PRORES_ASPECT_RATIO_UNKNOWN = 0,                /**< 未知/未声明*/
    PRORES_ASPECT_RATIO_SQUARE,                     /**< 方形 */
    PRORES_ASPECT_RATIO_4_3,                        /**< 4:3图像幅型比 */
    PRORES_ASPECT_RATIO_16_9,                       /**< 16:9图像幅型比 */
    PRORES_ASPECT_RATIO_BUTT
}PRORES_ASPECT_RATIO;

/**< 定义PRORES属性结构体。*/
typedef struct arVENC_ATTR_PRORES_S
{
    AR_CHAR             cIdentifier[4];             /**< 厂商名称。*/
    PRORES_FRAMERATE    enFrameRateCode;            /**< 帧率。*/
    PRORES_ASPECT_RATIO enAspectRatio;              /**< 幅型比。*/
} VENC_ATTR_PRORES_S;

/**<@note 参数有更新; 定义编码器属性结构体。*/
typedef struct arVENC_ATTR_S
{
    PAYLOAD_TYPE_E  enType;                         /**< 可读写; 编码属性协议; 静态属性。*/
    PIXEL_FORMAT_E  enPixelFormat;                  /**<@attention 新添加参数; 可读写; 像素格式类型; 静态属性。*/

    AR_U32  u32MaxPicWidth;                         /**< 可读写; 编码图像最大宽度; 以像素为单位; 静态属性。*/
    AR_U32  u32MaxPicHeight;                        /**< 可读写; 编码图像最大高度; 以像素为单位; 静态属性。*/

    AR_U32  u32BufSize;                             /**<@note 参数有更新, create channel后不支持修改; 可读写; 码流buffer大小; max bitrate * max buffer time + frameSize(yuv420:u32PicWidth*u32PicHeight*3/2),such as 3s; 静态属性*/
    AR_U32  u32Profile;                             /**<@note 参数有更新; 可读写; H.264取值范围: [0,4]; 0: 自适应; 1: baseline; 2: MP; 3: extend; 4: HP;
                                                         H.265取值范围:     [0,3]; 0: 自适应; 1: MP; 2: Main 10 Profile; 3: Main still pic;
                                                         Jpege/MJpege取值范围: 0; 0: Baseline;
                                                         prores取值范围: [0,3]; 0:ProRes Proxy; 1:ProRes 422(LT); 2:ProRes 422; 3:ProRes 422(HQ);
                                                         静态属性。*/
    AR_BOOL bByFrame;                               /**< 可读写; 取值范围:[0,1]; 帧/包模式获取码流;静态属性*/
    AR_U32  u32PicWidth;                            /**< 可读写; 编码图像宽度，以像素为单位; 静态属性 */
    AR_U32  u32PicHeight;                           /**< 可读写; 编码图像高度，以像素为单位; 静态属性 */
    union
    {
        VENC_ATTR_H264_S stAttrH264e;                /**<@note 暂不支持; H264编码协议。*/
        VENC_ATTR_H265_S stAttrH265e;                /**<@note 暂不支持; H265编码协议。*/
        VENC_ATTR_MJPEG_S stAttrMjpege;              /**< MJPEG编码协议。*/
        VENC_ATTR_JPEG_S  stAttrJpege;               /**< JPEG编码协议。*/
        VENC_ATTR_PRORES_S stAttrProres;             /**< PRORES编码协议。  */
    };
} VENC_ATTR_S;

/**< 定义H.264/H.265 GOP类型。*/
typedef enum arVENC_GOP_MODE_E
{
    VENC_GOPMODE_NORMALP    = 0,                    /* 编码单参考帧P帧GOP类型。*/
    VENC_GOPMODE_DUALP      = 1,                    /* 编码双参考帧P帧GOP类型。*/
    VENC_GOPMODE_SMARTP     = 2,                    /* 编码智能P帧GOP类型。*/
    VENC_GOPMODE_ADVSMARTP  = 3,                    /* 编码高级智能P帧GOP类型。*/
    VENC_GOPMODE_BIPREDB    = 4,                    /* 编码B帧GOP类型。 */
    VENC_GOPMODE_LOWDELAYB  = 5,                    /* 编码B帧GOP 类型，其中B帧只有前向参考帧。*/

    VENC_GOPMODE_BUTT,
} VENC_GOP_MODE_E;

/**< 定义编码单参考帧P帧GOP属性结构体。*/
typedef struct  arVENC_GOP_NORMALP_S
{
    AR_S32   s32IPQpDelta;            /**< 可读写; 取值范围:[-9,9]; I帧相对P帧的QP差值; 动态参数。*/
} VENC_GOP_NORMALP_S;

/**<定义编码双参考帧P帧GOP属性结构体。*/
typedef struct  arVENC_GOP_DUALP_S
{
    AR_U32 u32SPInterval;           /**< 可读写; 取值范围:[0, 1)U(1, u32Gop -1]; Special P帧的间隔。*/
    AR_S32 s32SPQpDelta;            /**< 可读写; 取值范围:[-10,30]; Special P帧相对普通P帧的QP差值。*/
    AR_S32 s32IPQpDelta;            /**< 可读写; 取值范围:[-10,30]; I帧相对普通P帧的QP差值; 动态属性 */
} VENC_GOP_DUALP_S;

/**< 定义编码智能P帧GOP属性结构体。*/
typedef struct  arVENC_GOP_SMARTP_S
{
    AR_U32  u32BgInterval;           /**< 可读写; 取值范围:[u32Gop,4294967295]; 长期参考帧的间隔。*/
    AR_S32  s32BgQpDelta;            /**< 可读写; 取值范围:[-10,30]; 长期参考帧和P帧的QP差值。*/
    AR_S32  s32ViQpDelta;            /**< 可读写; 取值范围:[-10,30]; 虚拟I帧和相对于普通P帧的QP差值。*/
} VENC_GOP_SMARTP_S;

/**< 定义编码高级智能P帧GOP属性结构体。*/
typedef struct  arVENC_GOP_ADVSMARTP_S
{
    AR_U32  u32BgInterval;           /**< 可读写; 取值范围:[u32Gop,4294967295]，且必须使u32Gop的整数倍; 长期参考帧的间隔*/
    AR_S32  s32BgQpDelta;            /**< 可读写; 取值范围:[-10,30]; 长期参考帧和P帧的QP差值。*/
    AR_S32  s32ViQpDelta;            /**< 可读写; 取值范围:[-10,30]; 虚拟I帧相对于普通P帧的QP差值。*/
} VENC_GOP_ADVSMARTP_S;

/**< 定义编码B帧GOP属性结构体。*/
typedef struct  arVENC_GOP_BIPREDB_S
{
    AR_U32 u32BFrmNum;                /**< 可读写; 取值范围:[1,3]; 编码B帧的个数 */
    AR_S32 s32BQpDelta;               /**< 可读写; 取值范围:[-10,30]; B帧相对P帧的QP差值。*/
    AR_S32 s32IPQpDelta;              /**< 可读写; 取值范围:[-10,30]; I帧相对普通P帧的QP差值; 动态属性 */
} VENC_GOP_BIPREDB_S;

/**< 定义编码器GOP属性结构体。*/
typedef struct arVENC_GOP_ATTR_S
{
    VENC_GOP_MODE_E enGopMode;                      /**< 可读写; 编码GOP类型 */
    union
    {
        VENC_GOP_NORMALP_S   stNormalP;             /**< 编码单参考帧P帧GOP属性结构体 */
        VENC_GOP_DUALP_S     stDualP;               /**< 编码双参考帧P帧GOP属性结构体 */
        VENC_GOP_SMARTP_S    stSmartP;              /**<@note 暂不支持; 编码智能P帧GOP属性结构体 */
        VENC_GOP_ADVSMARTP_S stAdvSmartP;           /**<@note 暂不支持; 编码高级智能P帧GOP属性结构体 */
        VENC_GOP_BIPREDB_S   stBipredB;             /**<@note 暂不支持; 编码B帧GOP属性结构体 */
    };

} VENC_GOP_ATTR_S;

/**< 定义编码通道属性结构体。*/
typedef struct arVENC_CHN_ATTR_S
{
    VENC_ATTR_S     stVencAttr;                     /**< 编码器属性。*/
    VENC_RC_ATTR_S  stRcAttr;                       /**< 码率控制器属性。*/
    VENC_GOP_ATTR_S stGopAttr;                      /**< Gop Mode类型的结构体。*/
} VENC_CHN_ATTR_S;


/**< 定义编码通道连续接收并编码的帧数结构体。*/
typedef struct arVENC_RECV_PIC_PARAM_S
{
    AR_S32 s32RecvPicNum;                           /**< 可读写; 取值范围:[-1,0)U(0 2147483647]; 编码通道连续接收并编码的帧数。*/
} VENC_RECV_PIC_PARAM_S;


/**< 定义编码通道的状态结构体。*/
typedef struct arVENC_CHN_STATUS_S
{
    AR_U32 u32LeftPics;                             /**< 只读; 待编码的图像数。*/
    AR_U32 u32LeftStreamBytes;                      /**< 只读; 码流buffer剩余的byte数。*/
    AR_U32 u32LeftStreamFrames;                     /**< 只读; 码流buffer剩余的帧数。*/
    AR_U32 u32CurPacks;                             /**< 只读; 当前帧的码流包个数。*/
    AR_U32 u32LeftRecvPics;                         /**< 只读; 剩余待接收的帧数，在用户调用接口AR_MPI_VENC_StartRecvFrame设置接收帧数后有效。*/
    AR_U32 u32LeftEncPics;                          /**< 只读; 剩余待编码的帧数，在用户调用接口AR_MPI_VENC_StartRecvFrame设置接收帧数后有效。*/
    AR_BOOL bJpegSnapEnd;                           /**< 只读; Jpege抓拍模式下指示抓拍过程是否结束。*/
    VENC_STREAM_INFO_S stVencStrmInfo;              /**< 只读; 编码器码流特征信息。*/
} VENC_CHN_STATUS_S;

/**< 定义H.264协议编码通道SLICE分割结构体。*/
typedef struct arVENC_H264_SLICE_SPLIT_S
{
    AR_BOOL bSplitEnable;                           /**< 可读写; 取值范围:[0,1]; Slice分割是否使能, AR_TRUE:使能, AR_FALSE:不使能, 默认设置:AR_FALSE; 动态属性 */
    AR_U32  u32MbLineNum;                           /**< 可读写; 取值范围:[1,(图像高 + 15)/16]; 每个slice占的宏块行数; 动态属性 */
} VENC_H264_SLICE_SPLIT_S;

/**< 定义H.264协议编码通道帧内预期结构体。*/
typedef struct arVENC_H264_INTRA_PRED_S
{
    AR_U32     constrained_intra_pred_flag;            /**< 可读写; 取值范围:[0,1], 系统默认值: AR_FALSE; 具体含义请参见H.264协议; 动态属性 */
} VENC_H264_INTRA_PRED_S;

/**< 定义H.264协议编码通道变换、量化结构体。*/
typedef struct arVENC_H264_TRANS_S
{
    AR_U32     u32IntraTransMode;                      /**< 可读写; 取值范围:[0,2], 0: 支持4x4, 8x8变换; 1: 4x4变换, 2: 8x8变换; 帧内预测的变化模式。*/
    AR_U32     u32InterTransMode;                      /**< 可读写; 取值范围:[0,2], 0: 支持4x4, 8x8变换; 1: 4x4变换, 2: 8x8变换; 帧间预测的变化模式。*/

    AR_BOOL    bScalingListValid;                      /**< 可读写; 取值范围:[0,1], 默认设置: AR_FALSE; InterScalingList8x8、IntraScalingList8x8 是否有效标识，只在high profile,svc-t下才有意义。*/
    AR_U8      InterScalingList8X8[64];                /**< 可读写; 取值范围:[1,255]; 帧间预测 8x8 的量化表，在 high profile,svc-t 下，用户可以使用自己的量化表，保留，暂不使用。*/
    AR_U8      IntraScalingList8X8[64];                /**< 可读写; 取值范围:[1,255]; 帧内预测 8x8 的量化表，在 high profile,svc-t 下，用户可以使用自己的量化表，保留，暂不使用。*/

    AR_S32     chroma_qp_index_offset;                 /**< 可读写; 取值范围:[-12,12], 系统默认值: 0;具体含义请参见 H.264 协议*/
} VENC_H264_TRANS_S;

/**< 定义H.264协议编码通道熵编码结构体。*/
typedef struct arVENC_H264_ENTROPY_S
{
    AR_U32 u32EntropyEncModeI;                     /**< 可读写; 取值范围:[0,1], 0:cavlc, 1:cabac, Baseline, Extend不支持 cabac; I帧熵编码模式; 动态属性 */
    AR_U32 u32EntropyEncModeP;                     /**< 可读写; 取值范围:[0,1], 0:cavlc, 1:cabac, Baseline, Extend不支持 cabac; P帧熵编码模式; 动态属性 */
    AR_U32 u32EntropyEncModeB;                     /**< 可读写; 取值范围:[0,1], 0:cavlc, 1:cabac, Baseline, Extend不支持 cabac,不支持B帧; B帧熵编码模式; 动态属性 */
    AR_U32 cabac_init_idc;                         /**<@note 暂不支持; 可读写; 取值范围:[0,2], 系统默认值: 0; 具体含义请参见H.264协议。*/
} VENC_H264_ENTROPY_S;

/**<@attention 新添加参数; 定义H.264协议编码通道图像播放顺序配置结构体。*/
typedef struct arVENC_H264_POC_S
{
    AR_U32 pic_order_cnt_type;                     /**< 可读写; 取值范围:[0,2]; 具体含义请参见H.264协议。*/

} VENC_H264_POC_S;

/**< 定义H.264协议编码通道Dblk结构体。*/
typedef struct arVENC_H264_DBLK_S
{
    AR_U32 disable_deblocking_filter_idc;          /**<  可读写; 取值范围:[0,2]; 具体含义请参见H.264协议; 动态属性 */
    AR_S32 slice_alpha_c0_offset_div2;             /**<  可读写; 取值范围:[-6,+6]; 具体含义请参见H.264协议; 动态属性 */
    AR_S32 slice_beta_offset_div2;                 /**<  可读写; 取值范围:[-6,+6]; 具体含义请参见H.264协议; 动态属性 */
} VENC_H264_DBLK_S;


/**< 定义H.264协议编码通道Vui中Time_Info信息的结构体。*/
typedef struct arVENC_H264_VUI_TIME_INFO_S
{
    AR_U8  timing_info_present_flag;               /**< 可读写; 取值范围:[0,1]，系统默认值: 0, 如果设1，下面的time_info信息将被编码到VUI中; 具体含义请参见H.264协议 动态属性 */
    AR_U8  fixed_frame_rate_flag;                  /**< 可读写; 取值范围:[0,1]; 具体含义请参见H.264协议; 动态属性 */
    AR_U32 num_units_in_tick;                      /**< 可读写; 取值范围:(0,4294967295]; 具体含义请参见H.264协议; 动态属性 */
    AR_U32 time_scale;                             /**< 可读写; 取值范围:(0,4294967295]; 具体含义请参见H.264协议; 动态属性 */
} VENC_VUI_H264_TIME_INFO_S;

/**< 定义H.264/H.265协议编码通道Vui中AspectRatio信息的结构体。*/
typedef struct arVENC_VUI_ASPECT_RATIO_S
{
    AR_U8  aspect_ratio_info_present_flag;          /**< 可读写; 取值范围:[0,1], 如果设1，下面的aspectratio信息将被编码到VUI中; 具体含义请参见H.264/H.265协议; 动态属性 */
    AR_U8  aspect_ratio_idc;                        /**< 可读写; 取值范围:[0,255], 17~254 保留; 具体含义请参见H.264/H.265协议; 动态属性 */
    AR_U8  overscan_info_present_flag;              /**< 可读写; 取值范围:[0,1], 如果设1，下面的oversacan信息将被编码到VUI中; 具体含义请参见H.264/H.265协议; 动态属性 */
    AR_U8  overscan_appropriate_flag;               /**< 可读写; 取值范围:[0,1]; 具体含义请参见H.264/H.265协议; 动态属性 */
    AR_U16 sar_width;                               /**< 可读写; 取值范围:[0, 65535], 并且与sar_height互质; 具体含义请参见H.264/H.265协议; 动态属性 */
    AR_U16 sar_height ;                             /**< 可读写; 取值范围:[0, 65535], 并且与sar_width互质; 具体含义请参见H.264/H.265协议; 动态属性 */
} VENC_VUI_ASPECT_RATIO_S;

/**< 定义H.264/H.265协议编码通道Vui中信息的Video_Signal结构体。*/
typedef struct arVENC_VUI_VIDEO_SIGNAL_S
{
    AR_U8  video_signal_type_present_flag;          /**< 可读写; 取值范围:[0,1], 如果设1，下面的video singnal信息将被编码到VUI中; 具体含义请参见H.264/H.265协议; 动态属性 */
    AR_U8  video_format ;                           /**< 可读写; H.264取值范围:[0,7], H.265取值范围:[0,5]; 具体含义请参见H.264/H.265协议; 动态属性 */
    AR_U8  video_full_range_flag;                   /**< 可读写; 取值范围: [0,1]; 具体含义请参见H.264/H.265协议; 动态属性 */
    AR_U8  colour_description_present_flag;         /**< 只读; 取值范围: [0,1]; 具体含义请参见H.264/H.265协议; 动态属性 */
    AR_U8  colour_primaries ;                       /**< 只读; 取值范围: [0,255]; 具体含义请参见H.264/H.265协议; 动态属性 */
    AR_U8  transfer_characteristics;                /**< 只读; 取值范围: [0,255]; 具体含义请参见H.264/H.265协议; 动态属性 */
    AR_U8  matrix_coefficients;                     /**< 只读; 取值范围: [0,255]; 具体含义请参见H.264/H.265协议; 动态属性 */
} VENC_VUI_VIDEO_SIGNAL_S;

/**< 定义H.264/H.265协议编码通道Vui中信息的Bitstream_Restriction结构体。*/
typedef struct arVENC_VUI_BITSTREAM_RESTRIC_S
{
    AR_U8  bitstream_restriction_flag ;             /**< 可读写; 取值范围: [0,1]; 具体含义请参见H.264/H.265协议; 动态属性 */
} VENC_VUI_BITSTREAM_RESTRIC_S;

/**< 定义H.264协议编码通道Vui接口体。*/
typedef struct arVENC_H264_VUI_S
{
    VENC_VUI_ASPECT_RATIO_S           stVuiAspectRatio;         /**< 具体含义请参见H.264协议。*/
    VENC_VUI_H264_TIME_INFO_S         stVuiTimeInfo;            /**< 具体含义请参见H.264协议。*/
    VENC_VUI_VIDEO_SIGNAL_S           stVuiVideoSignal;         /**< 具体含义请参见H.264协议。*/
    VENC_VUI_BITSTREAM_RESTRIC_S      stVuiBitstreamRestric;    /**< 具体含义请参见H.264协议。*/
} VENC_H264_VUI_S;

/**< 定义H.265协议编码通道Vui中Time_Info信息的结构体*/
typedef struct arVENC_VUI_H265_TIME_INFO_S
{
    AR_U32 timing_info_present_flag;               /**< 可读写; 取值范围:[0,1], 如果设1，下面的time_info信息将被编码到VUI中; 具体含义请参见H.265协议; 动态属性 */
    AR_U32 num_units_in_tick;                      /**< 可读写; 取值范围:[0,4294967295]; 具体含义请参见H.265协议; 动态属性 */
    AR_U32 time_scale;                             /**< 可读写; 取值范围:[0,4294967295]; 具体含义请参见H.265协议; 动态属性 */
    AR_U32 num_ticks_poc_diff_one_minus1;          /**< 可读写; 取值范围:[0,4294967294]; 具体含义请参见H.265协议; 动态属性 */
} VENC_VUI_H265_TIME_INFO_S;

/**< 定义H.264协议编码通道Vui接口体。*/
typedef struct arVENC_H265_VUI_S
{
    VENC_VUI_ASPECT_RATIO_S       stVuiAspectRatio;             /**< 具体含义请参见H.265协议。*/
    VENC_VUI_H265_TIME_INFO_S     stVuiTimeInfo;                /**< 具体含义请参见H.265协议。*/
    VENC_VUI_VIDEO_SIGNAL_S       stVuiVideoSignal;             /**< 具体含义请参见H.265协议。*/
    VENC_VUI_BITSTREAM_RESTRIC_S  stVuiBitstreamRestric;        /**< 具体含义请参见H.265协议。*/
} VENC_H265_VUI_S;

/**< 定义JPEG协议编码通道高级参数结构体。*/
typedef struct arVENC_JPEG_PARAM_S
{
    AR_U32 u32Qfactor;                     /**< 可读写; 取值范围:[1, 99]; 质量因子，值越小质量越差，越大质量越好，具体含义请参见RFC2435协议; 50代表用户使用自定义量化表; 动态属性*/
    AR_U8  u8YQt[64];                      /**< 可读写; 取值范围:[1, 255]; Y量化表; 动态属性 */
    AR_U8  u8CbQt[64];                     /**< 可读写; 取值范围:[1, 255]; Cb量化表; 动态属性 */
    AR_U8  u8CrQt[64];                     /**< 可读写; 取值范围:[1, 255]; Cr量化表; 动态属性 */
    AR_U32 u32MCUPerECS;                   /**<@note 暂不支持; 可读写; 取值范围:[0, (picwidth + 15) >> 4 x (picheight +
                                            15) >> 4 x 2], 系统默认值: 0, 表示不划分Ecs; 每个ECS中包含多少个MCU; 动态属性 */
} VENC_JPEG_PARAM_S;

/**< 定义JPEG协议编码通道高级参数结构体。*/
typedef struct arVENC_MJPEG_PARAM_S
{
    AR_U32 u32Qfactor;                     /**< 可读写; 取值范围:[1, 99]; 质量因子，值越小质量越差，越大质量越好, 具体含义请参见RFC2435协议; 动态属性 */
    AR_U8 u8YQt[64];                       /**< 可读写; 取值范围:[1, 255]; Y量化表; 动态属性 */
    AR_U8 u8CbQt[64];                      /**< 可读写; 取值范围:[1, 255]; Cb量化表; 动态属性 */
    AR_U8 u8CrQt[64];                      /**< 可读写; 取值范围:[1, 255]; Cr量化表; 动态属性 */
    AR_U32 u32MCUPerECS;                   /**<@note 暂不支持; 可读写; 取值范围:[0, (picwidth + 15) >> 4 x (picheight +
                                            15) >> 4 x 2], 系统默认值: 0, 表示不划分Ecs; 每个ECS中包含多少个MCU; 动态属性 */
} VENC_MJPEG_PARAM_S;

/**< 定义ProRes协议编码通道参数结构体。*/
typedef struct arVENC_PRORES_PARAM_S
{
    AR_U8 u8LumaQt[64];                       /**< 可读写; 取值范围:[1, 255]; 亮度量化表。*/
    AR_U8 u8ChromaQt[64];                     /**< 可读写; 取值范围:[1, 255]; 色度量化表。*/
    AR_CHAR encoder_identifier[4];            /**< 可读写; 标识生成压缩帧的编码器供应商或产品。*/
} VENC_PRORES_PARAM_S;


/**< 定义编码感兴趣区域信息。*/
typedef struct arVENC_ROI_ATTR_S
{
    AR_U32  u32Index;                     /**< 可读写; 取值范围:[0, 4294967295]; ROI区域的索引; 动态属性 */
    AR_BOOL bEnable;                      /**< 可读写; 取值范围:[0, 1]; 是否使能这个ROI区域; 动态属性 */
    AR_BOOL bAbsQp;                       /**<@note 暂不支持绝对QP; 可读写; 取值范围:[0, 1], AR_FALSE: 相对QP, AR_TURE: 绝对QP; ROI区域QP模式; 动态属性 */
    AR_S32  s32Qp;                        /**< 可读写; 取值范围:[-26, 25], 只有相对QP值可以小于0; QP值; 动态属性 */
    RECT_S  stRect;                       /**< 可读写; ROI区域; 动态属性 */
} VENC_ROI_ATTR_S;

/**< 定义编码通道感兴趣区域编码高级属性。*/
typedef struct arVENC_ROI_ATTR_EX_S
{
    AR_U32  u32Index;                              /**< 可读写; 取值范围:[0, 4294967295]; ROI区域的索引。*/
    AR_BOOL bEnable[3];                            /**< 可读写; 取值范围:[0, 1], 0: I帧; 1: P/B帧; 2: VI帧; 是否使能这个ROI区域;*/
    AR_BOOL bAbsQp[3];                             /**<@note 暂不支持绝对QP; 可读写; 取值范围:[0, 1], AR_FALSE: 相对QP, AR_TURE: 绝对QP; ROI区域QP模式; */
    AR_S32  s32Qp[3];                              /**< 可读写; 取值范围:[-26, 25], 只有相对QP值可以小于0; QP值; */
    RECT_S  stRect[3];                             /**< 可读写; ROI区域; */
}VENC_ROI_ATTR_EX_S;


/**< 定义非编码感兴趣区域帧率。*/
typedef struct arVENC_ROIBG_FRAME_RATE_S
{
    AR_S32 s32SrcFrmRate;                  /**< 可读写; 取值范围: [-1,0)U(0 2147483647]; 非Roi区域的源帧率。*/
    AR_S32 s32DstFrmRate;                  /**< 可读写; 取值范围: [-1, s32SrcFrmRate]; 非Roi区域的目标帧率。*/
} VENC_ROIBG_FRAME_RATE_S;


/**< 定义H.264/H.265编码的高级跳帧参考参数。*/
typedef struct arVENC_REF_PARAM_S
{
    AR_U32       u32Base;                          /**< 可读写; 取值范围:[0,4294967295]; base层的周期; 动态属性 */
    AR_U32       u32Enhance;                       /**< 可读写; 取值范围:NormalP [0,255] DualP [0,8]; enhance层的周期; 动态属性 */
    AR_BOOL      bEnablePred;                      /**< 可读写; 取值范围:[0, 1], 当bEnablePred设置为AR_FALSE时，等同于u32Base设置为无限大，base层的所有帧都参考IDR帧; 代表base层的帧是否被base层其他帧用作参考; 动态属性 */
} VENC_REF_PARAM_S;

/**< 定义JPEG编码通道的抓拍模式。*/
typedef enum arVENC_JPEG_ENCODE_MODE_E
{
    JPEG_ENCODE_ALL   = 0,                           /**< 全部抓拍模式, JPEG通道的默认抓拍模式。*/
    JPEG_ENCODE_SNAP  = 1,                           /**< 抓拍模式，JPEG通道仅编码以标记为抓拍帧的图像。*/
    JPEG_ENCODE_BUTT,
} VENC_JPEG_ENCODE_MODE_E;

/**< 定义码流buffer信息的结构体。*/
typedef struct arVENC_STREAM_BUF_INFO_S
{
    AR_U64   u64PhyAddr[MAX_TILE_NUM];               /**< 只读; 码流buffer的起始物理地址。*/
    AR_VOID ATTRIBUTE* pUserAddr[MAX_TILE_NUM];      /**< 只读; 码流buffer的虚拟地址。 */
    AR_U64  ATTRIBUTE u64BufSize[MAX_TILE_NUM];      /**< 只读; 码流buffer的大小。 */
} VENC_STREAM_BUF_INFO_S;

/**< 定义H.265协议编码通道SLICE分割结构体。*/
typedef struct arVENC_H265_SLICE_SPLIT_S
{
    AR_BOOL bSplitEnable;                           /**< 可读写; 取值范围:[0,1], 系统默认值: AR_FALSE; Slice分割是否使能; 动态属性 */
    AR_U32  u32LcuLineNum;                          /**< 可读写; 取值范围:(Picture height + 63)/64; 每个slice占的LCU行数; 动态属性 */
} VENC_H265_SLICE_SPLIT_S;

/**< 定义H.265协议编码通道PU参数结构体。*/
typedef struct arVENC_H265_PU_S
{
    AR_U32    constrained_intra_pred_flag;            /**< 可读写; 取值范围:[0,1]; 具体含义请参见H.265协议; 动态属性 */
    AR_U32    strong_intra_smoothing_enabled_flag;    /**< 可读写; 取值范围:[0,1]; 具体含义请参见H.265协议; 静态属性 */
} VENC_H265_PU_S;

/**< 定义H.265协议编码通道变换量化的结构体。*/
typedef struct arVENC_H265_TRANS_S
{
    AR_S32  cb_qp_offset;                           /**< 可读写; 取值范围:[-12,12]; 具体含义请参见H.265协议 */
    AR_S32  cr_qp_offset;                           /**< 可读写; 取值范围:[-12,12]; 具体含义请参见H.265协议 */
    AR_BOOL bScalingListEnabled;                    /**< 可读写; 取值范围:[0,1]; 如果设1，标识使用伸缩列表。*/

    AR_BOOL bScalingListTu4Valid;                   /**< 可读写; 取值范围:[0,1]; 如果设1，表示下面的4X4量化表将被编码。*/
    AR_U8   InterScalingList4X4[2][16];             /**< 可读写; 取值范围:[1,255]; 帧间预测4x4块量化表。*/
    AR_U8   IntraScalingList4X4[2][16];             /**< 可读写; 取值范围:[1,255]; 帧内预测4x4块量化表。*/

    AR_BOOL bScalingListTu8Valid;                   /**< 可读写; 取值范围:[0,1]; 如果设1，表示下面的8x8量化表将被编码。*/
    AR_U8   InterScalingList8X8[2][64];             /**< 可读写; 取值范围:[1,255]; 帧间预测8x8块量化表。*/
    AR_U8   IntraScalingList8X8[2][64];             /**< 可读写; 取值范围:[1,255]; 帧内预测8x8块量化表。*/

    AR_BOOL bScalingListTu16Valid;                  /**< 可读写; 取值范围:[0,1]; 如果设1，表示下面的16x16量化表将被编码。*/
    AR_U8   InterScalingList16X16[2][64];           /**< 可读写; 取值范围:[1,255]; 帧间预测16x16块量化表。*/
    AR_U8   IntraScalingList16X16[2][64];           /**< 可读写; 取值范围:[1,255]; 帧内预测16x16块量化表。*/

    AR_BOOL bScalingListTu32Valid;                  /**< 可读写; 取值范围:[0,1]; 如果设1，表示下面的16x16量化表将被编码。*/
    AR_U8   InterScalingList32X32[64];              /**< 可读写; 取值范围:[1,255]; 帧间预测32x32块量化表。*/
    AR_U8   IntraScalingList32X32[64];              /**< 可读写; 取值范围:[1,255]; 帧内预测32x32块量化表。*/

} VENC_H265_TRANS_S;

/**< 定义H.265协议编码通道熵编码的结构体。*/
typedef struct arVENC_H265_ENTROPY_S
{
    AR_U32 cabac_init_flag;                         /**< 可读写; 取值范围:[0,1]; 具体含义请参见H.265协议 */
} VENC_H265_ENTROPY_S;

/**< 定义H.265协议编码通道Deblocking的结构体。 */
typedef struct arVENC_H265_DBLK_S
{
    AR_U32 slice_deblocking_filter_disabled_flag;   /**< 可读写; 取值范围:[0,1]; 具体含义请参见H.265协议; 动态属性 */
    AR_S32 slice_beta_offset_div2;                  /**< 可读写; 取值范围:[-6,6]; 具体含义请参见H.265协议; 动态属性 */
    AR_S32 slice_tc_offset_div2;                    /**< 可读写; 取值范围:[-6,6]; 具体含义请参见H.265协议; 动态属性 */
} VENC_H265_DBLK_S;

/**< 定义H.265协议编码通道Sao的结构体 */
typedef struct arVENC_H265_SAO_S
{
    AR_U32  sample_adaptive_offset_enabled_flag;    /**< 可读写; 取值范围:[0,1]; 表示重构帧是否在去块滤波后使用SAO处理; 静态参数。*/
} VENC_H265_SAO_S;

/**< P帧刷I slice模式。*/
typedef enum arVENC_INTRA_REFRESH_MODE_E
{
    INTRA_REFRESH_ROW = 0,                      /**< 按行刷新 */
    INTRA_REFRESH_COLUMN,                       /**< 按列刷新 */
    INTRA_REFRESH_NUM,                          /**< CTU or MB Num */
    INTRA_REFRESH_BUTT
} VENC_INTRA_REFRESH_MODE_E;

/**< P帧刷I slice控制参数。*/
typedef struct arVENC_INTRA_REFRESH_S
{
    AR_BOOL                     bRefreshEnable;     /**< 可读写; 取值范围:[0,1], 0: 不使能; 1: 使能。默认值: AR_FALSE; 是否使能刷Islice功能。*/
    VENC_INTRA_REFRESH_MODE_E   enIntraRefreshMode; /**< 可读写; 取值范围:[0,2]; I宏块刷新模式,分为按行刷新、按列刷新和; 静态属性 */
    AR_U32                      u32RefreshNum;      /**<@note 参数更新; 可读写; 每次I宏块刷新行数、列数、ctu和mb的数量; 静态属性 */
    AR_U32                      u32ReqIQp;          /**< 可读写; 取值范围:[0,51]; I帧QP值, 在帧内刷新模式, 可能方案还需要插入IDR帧, 设置I帧QP用于控制插入的IDR帧的质量, 质量越好IDR帧大小越大; 质量越差IDR帧大小越小*/
} VENC_INTRA_REFRESH_S;

/**< 编码相关模块参数类型。*/
typedef enum arVENC_MODTYPE_E
{
    MODTYPE_VENC = 1,                   /**< VENC模块参数类型 */
    MODTYPE_H264E,                      /**< H264e模块参数类型 */
    MODTYPE_H265E,                      /**< H265e模块参数类型 */
    MODTYPE_JPEGE,                      /**< Jpege模块参数类型 */
    MODTYPE_RC,                         /**< Rc模块参数类型 */
    MODTYPE_BUTT
} VENC_MODTYPE_E;

/**< h264e模块参数。 */
typedef struct arVENC_MOD_H264E_S
{
    AR_U32          u32OneStreamBuffer;     /**<@note 只支持单包模式; 可读写; 取值范围:[1], 0: 多包模式, 1: 单包模式; 编码码流帧配置模式。*/
    AR_U32          u32H264eMiniBufMode;    /**<@note 暂不支持; 可读写; 取值范围:[0,1], 0: 码流buffer根据分辨率分配, 1：码流buffer下限为32k，用户保证合理; 编码码流buffer配置模式; */
    AR_U32          u32H264ePowerSaveEn;    /**<@note 暂不支持; 可读写; 取值范围:[0,1], 0：关闭低功耗模式, 1：使能低功耗模式, 2：使能极低功耗模式; 低功耗模式。*/
    VB_SOURCE_E     enH264eVBSource;        /**<@note 暂不支持; 可读写; 取值范围:VB_SOURCE_PRIVATE: 私有VB方式，驱动内部完成帧存分配, VB_SOURCE_USER: 用户VB, 需由用户分配帧存; 参考帧和重构帧的帧存分配方式。*/
    AR_BOOL         bQpHstgrmEn;            /**<@note 暂不支持; 可读写; 取值范围:[0,1]; QP直方图输出控制模块。*/
    AR_U32          u32CoreClock;           /**<@attention 新添加参数; 可读写; 取值范围:{75, 150, 200, 250, 300, 360, 400, 450, 500} Mhz, 根据解码的总pixel clock计算，比如1080p30=1920x1080x30=62M，选75M即可 */
    AR_U32          u32BpuClock;            /**<@attention 新添加参数; 可读写; 取值范围:{75, 150, 200, 250, 300, 360} Mhz, 比core低一档或者相等 */
    AR_U64          u64Flags;               /**<@attention 新添加参数; 可读写; 264扩展参数标志位，各bit位含义参见ENUM_VENC_EXTENSION_FLAGS*/
} VENC_MOD_H264E_S;

/**< h265e模块参数。*/
typedef struct arVENC_MOD_H265E_S
{
    AR_U32          u32OneStreamBuffer;      /**<@note 只支持单包模式; 可读写; 取值范围:[0,1], 0: 多包模式, 1: 单包模式; 编码码流帧配置模式; */
    AR_U32          u32H265eMiniBufMode;     /**<@note 暂不支持; 可读写; 取值范围:[0,1]; H265e MiniBufMode; */
    AR_U32          u32H265ePowerSaveEn;     /**<@note 暂不支持; 可读写; 取值范围:[0,2]; H265e PowerSaveEn; */
    VB_SOURCE_E     enH265eVBSource;         /**<@note 暂不支持; 可读写; 取值范围:VB_SOURCE_PRIVATE: 私有VB方式，驱动内部完成帧存分配, VB_SOURCE_USER: 用户VB, 需由用户分配帧存; 参考帧和重构帧的帧存分配方式。*/
    AR_BOOL         bQpHstgrmEn;             /**<@note 暂不支持; 可读写; 取值范围:[0,1]; QP直方图输出控制模块。*/
    AR_U32          u32CoreClock;            /**<@attention 新添加参数; 可读写; 取值范围:{75, 150, 200, 250, 300, 360, 400, 450, 500} Mhz, 根据编码的总pixel clock计算，比如1080p30=1920x1080x30=62M，选75M即可 signoff 500M*/
    AR_U32          u32BpuClock;             /**<@attention 新添加参数; 可读写; 取值范围:{75, 150, 200, 250, 300, 360} Mhz, 比core低一档或者相等 signoff 360M*/
    AR_U64          u64Flags;                /**<@attention 新添加参数; 只写; 265扩展参数标志位，各bit位含义参见ENUM_VENC_EXTENSION_FLAGS*/
} VENC_MOD_H265E_S;

/**< jpege模块参数。*/
typedef struct arVENC_MOD_JPEGE_S
{
    AR_U32  u32OneStreamBuffer;         /**<@note 只支持单包模式; 可读写; 取值范围:[0,1], 0: 多包模式, 1: 单包模式; 编码码流帧配置模式。*/
    AR_U32  u32JpegeMiniBufMode;        /**<@note 暂不支持; 可读写; 取值范围:[0,1]; Jpege MiniBufMode */
    AR_U32  u32JpegClearStreamBuf;      /**<@note 暂不支持; 可读写; 取值范围:[0,1]; JpegClearStreamBuf */
    AR_U32  u32CoreClock;               /**<@attention 新添加参数; 可读写; 取值范围:{75, 150, 200, 250, 300, 360, 400, 450} Mhz, 根据编码的总pixel clock计算，比如1080p30=1920x1080x30=62M，选75M即可 signoff 450M*/
    AR_U64  u64Flags;                   /**<@attention 新添加参数; 只写; jpeg扩展参数标志位各bit位含义参见ENUM_VENC_EXTENSION_FLAGS*/
} VENC_MOD_JPEGE_S;

/**< rc模块参数。*/
typedef struct arVENC_MOD_RC_S
{
    AR_U32  u32ClrStatAfterSetBr; /**< 可读写; 取值范围:[0,1], 0：设置通道码率后不清除RC的帧率和码率统计Buffer, 1：设置通道码率后清除RC的帧率和码率统计 Buffer; 设置通道码率时是否清楚RC相关统计。*/
} VENC_MOD_RC_S;

/**< venc模块参数。*/
typedef struct arVENC_MOD_VENC_S
{
    AR_U32 u32VencBufferCache;  /**< 可读写; 取值范围:[0,1], 默认值1 0：关闭编码码流buffer cached, 1：打开编码码流buffer cached; 码流获取是否支持cache方式。*/
    AR_U32 u32FrameBufRecycle;  /**<@note 暂不支持; 可读写; 取值范围:[0,1], 0：关闭编码帧存回收, 1：打开编码帧存回收; 帧存是否回收。*/
} VENC_MOD_VENC_S;

/**<@attention 新添加参数; venc模块队列大小参数结构体。*/
typedef struct arVENC_MOD_EVENT_S
{
    AR_U32 u32VencIrqQueueSize;    /**<@attention 新添加参数; 可读写; 取值范围:[2, 4294967295]; 编码器irq消息队列大小，存储硬件irq命令。*/
    AR_U32 u32VencTaskQueueSize;   /**<@attention 新添加参数; 可读写; 取值范围:[2, 4294967295]; 编码器cmd消息队列大小，存储app命令, 例如:start/stop。*/
    AR_U32 u32VencDoneQueueSize;   /**<@attention 新添加参数; 可读写; 取值范围:[2, 4294967295]; 编码器done消息队列大小，存储所有通道的编码流。*/
    AR_U32 u32VencOutQueueSize;    /**<@attention 新添加参数; 可读写; 取值范围:[2, 4294967295]; 编码器output队列大小, 存储编码流, 队列大小=(bufSize/bitrate*fps) */
    AR_U32 u32VencEventQueueSize;  /**<@attention 新添加参数; 可读写; 取值范围:[6, 4294967295]; 编码器实例输入队列的大小，是VI缓冲区数量的2倍。*/
} VENC_MOD_EVENT_S;

/**< 编码相关模块参数 */
typedef struct arVENC_MODPARAM_S
{
    VENC_MODTYPE_E       enVencModType;         /**< 可读写; 设置或者获取模块参数的类型。*/
    VENC_MOD_EVENT_S     stEventModParam;       /**<@attention 新添加参数; 可读写; venc模块队列大小参数。*/
    union
    {
        VENC_MOD_VENC_S  stVencModParam;        /**< venc模块参数。*/
        VENC_MOD_H264E_S stH264eModParam;       /**< h264e模块参数。*/
        VENC_MOD_H265E_S stH265eModParam;       /**< h265e模块参数。*/
        VENC_MOD_JPEGE_S stJpegeModParam;       /**< jpege模块参数。*/
        VENC_MOD_RC_S    stRcModParam;          /**< rc模块参数。*/
    };
} VENC_PARAM_MOD_S;

/**< 编码相关模块参数类型。*/
typedef enum arVENC_FRAME_TYPE_E
{
    VENC_FRAME_TYPE_NONE = 1,           /**< 当前帧编码类型不指定，实际帧类型由驱动内部决定。*/
    VENC_FRAME_TYPE_IDR,                /**< 指定当前帧编码为IDR帧。*/
    VENC_FRAME_TYPE_BUTT
} VENC_FRAME_TYPE_E;

/**<@note 参数更新; 编码相关模块参数类型。*/
typedef struct arUSER_RC_INFO_S
{
    AR_BOOL bQpMapValid;                /**< 可读写; 取值范围:[0,1]; QPMAP模式中，QP标是否生效。*/
    AR_U32  u32BlkStartQp;              /**< 可读写; 取值范围:[0,51]; QpMap模式中，第一次16x16块的QP值。*/
    AR_U64  u64QpMapPhyAddr;            /**< 可读写; QPMAP模式中，QP表的物理地址。*/
    VENC_FRAME_TYPE_E enFrameType;      /**< 指定当前帧编码帧类型。*/
} USER_RC_INFO_S;

/**< 用户发送图像信息结构体。*/
typedef struct arUSER_FRAME_INFO_S
{
    VIDEO_FRAME_INFO_S stUserFrame;         /**< 可读写; 取值范围:[0,1]; 用户发送图像帧信息结构体。*/
    USER_RC_INFO_S     stUserRcInfo;        /**< 可读写; 取值范围:[0,1]; 用户码控信息结构体。*/
} USER_FRAME_INFO_S;

/**< SSERegion模块中参数。*/
typedef struct arVENC_SSE_CFG_S
{
    AR_U32  u32Index;           /**< 可读写; 取值范围:[0, 7]; SSE区域的索引; 系统支持的索引范围为[0,7], 不支持超出这个范围的索引。*/
    AR_BOOL bEnable;            /**< 可读写; 取值范围:[0, 1]; 是否使能这个SSE区域。*/
    RECT_S  stRect;             /**< 可读写; SSE区域。*/
} VENC_SSE_CFG_S;

/**< 定义通道截取（Crop）参数。*/
typedef struct arVENC_CROP_INFO_S
{
    AR_BOOL bEnable;                       /**< 可读写; 取值范围:[0, 1]; 是否进行剪裁。*/
    RECT_S  stRect;                        /**< 可读写; 剪裁的区域; 注意: stRect.s32X: 必须16像素对齐, stRect.s32Y: 必须2像素对齐, stRect.u32Width, s32Rect.u32Height, 满足对应编码通道的宽高约束 */
} VENC_CROP_INFO_S;

/**< 定义通道帧率控制参数。*/
typedef struct arVENC_FRAME_RATE_S
{
    AR_S32 s32SrcFrmRate;                  /**< 可读写; 取值范围:[0, 240]; 通道输入的帧率, 以fps为单位。*/
    AR_S32 s32DstFrmRate;                  /**< 可读写; 取值范围:[0, 240], Jpeg: -1 或[1, s32SrcFrmRate]: H.264/H.265/Mjpeg: -1 或[1, 240]; 通道输出的帧率, 以fps为单位。*/
} VENC_FRAME_RATE_S;

/**< 定义Venc通道参数结构体。*/
typedef struct arVENC_CHN_PARAM_S
{
    AR_BOOL bColor2Grey;                  /**< 可读写; 取值范围:[0, 1]; 开启或者关闭一个通道的彩转灰功能; 静态参数。*/
    AR_U32  u32Priority;                  /**< 可读写; 取值范围:[0, 1]; 编码通道优先级参数。*/
    AR_U32  u32MaxStrmCnt;                /**<@note 暂不支持; 可读写: 取值范围:[0,4294967295]; 最大码流缓存帧数。*/
    AR_U32  u32PollWakeUpFrmCnt;          /**< 可读写; 取值范围:(0,4294967295]; 当通道使用超时或阻塞获取码流, 编码指定的帧u32PollWakeUpFrmCnt之后唤醒阻塞接口。*/
    VENC_CROP_INFO_S    stCropCfg;        /**< 可读写; 通道截取（Crop）参数。*/
    VENC_FRAME_RATE_S   stFrameRate;      /**<@note 暂不支持; 通道帧率控制参数。*/
} VENC_CHN_PARAM_S;

/**< 前景保护参数结构体。*/
typedef struct arVENC_FOREGROUND_PROTECT_S
{
    AR_BOOL bForegroundCuRcEn;                          /**< 可读写; 开启或关闭前景宏块级码控制。默认关闭。*/
    AR_U32  u32ForegroundDirectionThresh;               /**< 可读写; 取值范围:[0, 16]; 在前景宏块级码率控制时，用于控制减方向u32Thr的下标。*/
    AR_U32  u32ForegroundThreshGain;                    /**< 可读写; 取值范围:[0, 15]; 前景检测中用于计算sad的阈值。*/
    AR_U32  u32ForegroundThreshOffset;                  /**< 可读写; 取值范围:[0, 255]; 前景检测中用于计算sad的阈值。*/
    AR_U32  u32ForegroundThreshP[RC_TEXTURE_THR_SIZE];  /**< 可读写; 取值范围:[0, 255]; P帧在前景宏块级码率控制时的mad门限。*/
    AR_U32  u32ForegroundThreshB[RC_TEXTURE_THR_SIZE];  /**< 可读写; 取值范围:[0, 255]; P帧在前景宏块级码率控制时的mad门限。*/
}VENC_FOREGROUND_PROTECT_S;

/**< 编码模式场景。*/
typedef enum arVENC_SCENE_MODE_E
{
    SCENE_0  = 0,              /**< 可读写; 摄像机不运动或周期性连续运动的场景。*/
    SCENE_1  = 1,              /**< 可读写; 高码率下运动场景。*/
    SCENE_2  = 2,              /**< 可读写; 中等码率下有规律的连续运动，且编码压力比较大场景。*/
    SCENE_BUTT
}VENC_SCENE_MODE_E;

/**<@note 参数更新; 去除呼吸效应参数结构体。*/
typedef struct arVENC_DEBREATHEFFECT_S
{
    AR_S32    s32Strength1;             /**< 可读写; 取值范围:[-9, 9]; 值越小去呼吸效应越强 */
} VENC_DEBREATHEFFECT_S;

/**< CU模式选择的倾向性配置结构体。*/
typedef struct arVENC_CU_PREDICTION_S
{
     OPERATION_MODE_E enPredMode;       /**< 取值范围:[0, 1], 0: auto模式, 驱动内部完成倾向性配置, 1: manual模式: 设置改接口，用户完成倾向性配置; 倾向性选择模式。*/

     AR_U32 u32Intra32Cost;             /**< 取值范围:[0, 15]; Intra32倾向性调节，该值增大表示选择该模式的倾向性越小。H.264该值无效。*/
     AR_U32 u32Intra16Cost;             /**< 取值范围:[0, 15]; Intra16倾向性调节，该值增大表示选择该模式的倾向性越小。*/
     AR_U32 u32Intra8Cost;              /**< 取值范围:[0, 15]; Intra8倾向性调节，该值增大表示选择该模式的倾向性越小。*/
     AR_U32 u32Intra4Cost;              /**< 取值范围:[0, 15]; Intra4倾向性调节，该值增大表示选择该模式的倾向性越小。*/

     AR_U32 u32Inter64Cost;             /**< 取值范围:[0, 15]; Inter64倾向性调节，该值增大表示选择该模式的倾向性越小。H.264该值无效。*/
     AR_U32 u32Inter32Cost;             /**< 取值范围:[0, 15]; Inter32倾向性调节，该值增大表示选择该模式的倾向性越小。H.264该值无效。*/
     AR_U32 u32Inter16Cost;             /**< 取值范围:[0, 15]; Inter16倾向性调节，该值增大表示选择该模式的倾向性越小。*/
     AR_U32 u32Inter8Cost;              /**< 取值范围:[0, 15]; Inter8倾向性调节，该值增大表示选择该模式的倾向性越小。*/
} VENC_CU_PREDICTION_S;

/**< SKIP倾向性配置结构体。*/
typedef struct arVENC_SKIP_BIAS_S
{
     AR_BOOL   bSkipBiasEn;                 /**< 取值范围:[0, 1]; 标识skip倾向性是否使能。*/
     AR_U32    u32SkipThreshGain;           /**< 取值范围:[0, 15]; 前景检测中用于计算sad的阈值。*/
     AR_U32    u32SkipThreshOffset;         /**< 取值范围:[0, 255]; 前景检测中用于计算sad的阈值。*/
     AR_U32    u32SkipBackgroundCost;       /**< 取值范围:[0, 15]; 背景skip倾向性调节，该值增大表示选择skip的倾向性越小。*/
     AR_U32    u32SkipForegroundCost;       /**< 取值范围:[0, 15]; 前景skip倾向性调节，该值增大表示选择skip的倾向性越小。*/
} VENC_SKIP_BIAS_S;

/**< QP分层参数结构体。*/
typedef struct arVENC_HIERARCHICAL_QP_S
{
    AR_BOOL     bHierarchicalQpEn;                  /**< 取值范围:[0, 1]; QP分层是否使能。*/
    AR_S32      s32HierarchicalQpDelta[4];          /**< 取值范围:[-10, 10]; 每一层帧相对于第0层P帧的Qp差值。*/
    AR_S32      s32HierarchicalFrameNum[4];         /**< 取值范围:[0, 5]; 每一层中帧的数目。*/
}VENC_HIERARCHICAL_QP_S;

/**< 定义编码通道绑定的VB池结构体。*/
typedef struct arVENC_CHN_POOL_S
{
    VB_POOL hPicVbPool;             /**< 可读写; 用于存储Picture的VB池PoolId。*/
    VB_POOL hPicInfoVbPool;         /**< 可读写; 用于存储Picture的信息（pme、tmv、pmeinfo）的VB池PoolId。*/
}VENC_CHN_POOL_S;

/**< 定义RC模块的高级参数，此接口会包含与码流控制算法无关的功能，并且未来版本还有可能扩展。*/
typedef struct arVENC_RC_ADVPARAM_S
{
    AR_U32 u32ClearStatAfterSetAttr;            /**< 取值范围:[0, 1], 0: 设置通道码率后不清除 RC 的帧率和码率统计信息; 1: 设置通道码率后清除RC的帧率和码率统计信息; 默认值: 1; 设置新的通道码率后，是否清除码率控制的统计信息。*/
}VENC_RC_ADVPARAM_S;

/**< 定义VENC编码通道旋转角度参数*/
typedef struct arVENC_ROTATION_PARAM_S
{
    AR_S32 s32RotationEnable;  /**< 可读写；是否使能旋转，取值范围[0, 1] **/
    AR_S32 s32RotationAngle;   /**< 可读写；图像逆时针旋转的角度，取值范围[0, 90, 180, 270]。 **/
}VENC_ROTATION_PARAM_S;
/**< 定义VENC编码通道镜像参数*/
typedef struct arVENC_MIRROR_PARAM_S
{
    AR_S32 s32MirrorEnable;  /**< 可读写；是否使能镜像，取值范围[0, 1] **/
    AR_S32 s32MirrorDirection;  /**< 可读写；镜像的方向，取值范围[0:无镜像, 1:垂直镜像, 2:水平镜像, 3:水平加垂直镜像]。 **/
}VENC_MIRROR_PARAM_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __AR_COMM_VENC_H__ */
