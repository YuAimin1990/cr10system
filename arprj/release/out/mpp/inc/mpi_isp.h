/**
 * \file
 * \brief 描述ISP相关的接口.
 */

#ifndef __MPI_ISP_H__
#define __MPI_ISP_H__

#include "ar_comm_isp.h"
//#include "ar_comm_sns.h"
#include "ar_comm_3a.h"
#include "ar_comm_video.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */
/******************************* API declaration *****************************/
/** \addtogroup      MPI_VI */
/** @{ */  /** <!--  [MPI_VI]*/

/* Firmware Main Operation */
/**
\brief isp 初始化。
\param[in] ViPipe   :   PIPE号。
\retval ::0         :   成功。
\retval ::non-zero  :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_ISP_Init(VI_PIPE ViPipe);
/**
\brief isp 内存初始化。
\param[in] ViPipe   :   PIPE号。
\retval ::0         :   成功。
\retval ::non-zero  :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_ISP_MemInit(VI_PIPE ViPipe);
/**
\brief 启动运行isp。
\param[in] ViPipe   :   PIPE号。
\retval ::0         :   成功。
\retval ::non-zero  :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_ISP_Run(VI_PIPE ViPipe);
/**
\brief 停止退出isp。
\param[in] ViPipe   :   PIPE号。
\retval ::0         :   成功。
\retval ::non-zero  :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_ISP_Exit(VI_PIPE ViPipe);
/**
\brief 向isp 注册sensor的曝光函数组。
\param[in] ViPipe   :   PIPE号。
\param[in] pstSnsAttrInfo   :   sensor 信息。
\param[in] pstRegister   :   sensor 曝光函数组。
\retval ::0         :   成功。
\retval ::non-zero  :   失败，其值为错误码。
\see \n
N/A
*/

AR_S32 AR_MPI_ISP_SensorRegCallBack(VI_PIPE ViPipe, ISP_SNS_ATTR_INFO_S *pstSnsAttrInfo, ISP_SENSOR_REGISTER_S *pstRegister);
/**
\brief 取消掉AR_MPI_ISP_SensorRegCallBack 注册的曝光函数组
\param[in] ViPipe   :   PIPE号。
\param[in] SensorId   :   sensor 信息。
\retval ::0         :   成功。
\retval ::non-zero  :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_ISP_SensorUnRegCallBack(VI_PIPE ViPipe, SENSOR_ID SensorId);
/**
\brief 设置isp的公共属性
\param[in] ViPipe   :   PIPE号。
\param[in] pstPubAttr   :  ISP 公共属性。
\retval ::0         :   成功。
\retval ::non-zero  :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_ISP_SetPubAttr(VI_PIPE ViPipe, const ISP_PUB_ATTR_S *pstPubAttr);
/**
\brief  获取isp的公共属性
\param[in] ViPipe   :   PIPE号。
\param[out] pstPubAttr   :  ISP 公共属性。
\retval ::0         :   成功。
\retval ::non-zero  :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_ISP_GetPubAttr(VI_PIPE ViPipe, ISP_PUB_ATTR_S *pstPubAttr);
/**
\brief  获取vedio 信号
\param[in] ViPipe   :   PIPE号。
\param[in] enIspVDType   :  信号类型
\param[in] u32MilliSec   :  超时ms
\retval ::0         :   成功。
\retval ::non-zero  :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_ISP_GetVDTimeOut(VI_PIPE ViPipe, ISP_VD_TYPE_E enIspVDType, AR_U32 u32MilliSec);
/**
\brief  获取isp 内被状态信息
@attention 未实现
\retval ::0         :   成功。
\retval ::non-zero  :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_ISP_QueryInnerStateInfo(VI_PIPE ViPipe, ISP_INNER_STATE_INFO_S *pstInnerStateInfo);
/**
\brief  获取isp aec 的统计数据
@attention 转义，参数内容新加
\param[in] ViPipe   :   PIPE号。
\param[out] pstAeStat   :  aec 的统计数据
\retval ::0         :   成功。
\retval ::non-zero  :   失败，其值为错误码。
\see \n
N/A
*/

AR_S32 AR_MPI_ISP_GetAEStatistics(VI_PIPE ViPipe, ISP_AE_STATISTICS_S *pstAeStat);
/**
\brief  获取isp awb 的统计数据
@attention 转义，参数内容新加
\param[in] ViPipe   :   PIPE号。
\param[out] pstWBStat   :  awb 的统计数据
\retval ::0         :   成功。
\retval ::non-zero  :   失败，其值为错误码。
\see \n
N/A
*/

AR_S32 AR_MPI_ISP_GetWBStatistics(VI_PIPE ViPipe, ISP_WB_STATISTICS_S *pstWBStat);
/**
\brief  获取isp af 的统计数据
@attention 转义，参数内容新加
\param[in] ViPipe   :   PIPE号。
\param[out] pstAfStat   :  af 的统计数据
\retval ::0         :   成功。
\retval ::non-zero  :   失败，其值为错误码。
\see \n
N/A
*/

AR_S32 AR_MPI_ISP_GetFocusStatistics(VI_PIPE ViPipe, ISP_AF_STATISTICS_S *pstAfStat);
/**
\brief  配置isp统计
@attention 转义，参数内容新加
\param[in] ViPipe   :   PIPE号。
\param[in] pstStatCfg   :  统计配置参数
\retval ::0         :   成功。
\retval ::non-zero  :   失败，其值为错误码。
\see \n
N/A
*/

AR_S32 AR_MPI_ISP_SetStatisticsConfig(VI_PIPE ViPipe, const ISP_STATISTICS_CFG_S *pstStatCfg);
/**
\brief  获取isp af 的统计数据
@attention 转义，参数内容新加
\param[in] ViPipe   :   PIPE号。
\param[out] pstStatCfg   :  统计配置参数
\retval ::0         :   成功。
\retval ::non-zero  :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_ISP_GetStatisticsConfig(VI_PIPE ViPipe, ISP_STATISTICS_CFG_S *pstStatCfg);
/**
\brief  获取isp af 的统计数据
\param[in] ViPipe   :   PIPE号。
\param[out] enState   :  固件状态
\retval ::0         :   成功。
\retval ::non-zero  :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_ISP_SetFMWState(VI_PIPE ViPipe, const ISP_FMW_STATE_E enState);
/**
\brief  获取isp af 的统计数据
\param[in] ViPipe   :   PIPE号。
\param[out] penState   :   固件状态
\retval ::0         :   成功。
\retval ::non-zero  :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_ISP_GetFMWState(VI_PIPE ViPipe, ISP_FMW_STATE_E *penState);

/**
\brief 启动sensor自动检测插拔。
@attention 新添加接口
\param[in] ViPipe   :   PIPE号。
\retval ::0         :   成功。
\retval ::non-zero  :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_ISP_StartSnsDetect(VI_PIPE ViPipe);

/**
\brief 停止sensor自动检测插拔。
@attention 新添加接口
\param[in] ViPipe   :   PIPE号。
\retval ::0         :   成功。
\retval ::non-zero  :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_ISP_StopSnsDetect(VI_PIPE ViPipe);

/**
\brief 获取sensor自动检测插拔的信息。
@attention 新添加接口
\param[in] ViPipe               :   PIPE号。
\param[out] pstSnsDetectInfo    :   Sensor自动检测插拔的信息的指针。
\retval ::0                     :   成功。
\retval ::non-zero              :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_ISP_GetSnsDetectInfo(VI_PIPE ViPipe, ISP_SNS_DETECT_INFO_S *pstSnsDetectInfo);

/**
   \brief 设置wdr属性
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstFSWDRAttr  :   wdr属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetFSWDRAttr(VI_PIPE ViPipe, const ISP_WDR_FS_ATTR_S *pstFSWDRAttr);

/**
   \brief 获取wdr属性
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstFSWDRAttr  :   wdr属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetFSWDRAttr(VI_PIPE ViPipe, ISP_WDR_FS_ATTR_S *pstFSWDRAttr);

/**
   \brief 获取drc属性
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstDRCAttr  :   drc属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetDRCAttr(VI_PIPE ViPipe, const ISP_DRC_ATTR_S *pstDRC);

/**
   \brief 获取drc属性
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstDRCAttr  :   drc属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetDRCAttr(VI_PIPE ViPipe, ISP_DRC_ATTR_S *pstDRC);

/**
   \brief 设置dehaze属性
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstDehazeAttr  :   dehaze属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetDehazeAttr(VI_PIPE ViPipe, const ISP_DEHAZE_ATTR_S *pstDehazeAttr);

/**
   \brief 获取dehaze属性
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstDehazeAttr :   dehaze属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetDehazeAttr(VI_PIPE ViPipe, ISP_DEHAZE_ATTR_S *pstDehazeAttr);

/**
   \brief 设置dp属性
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstDPDynamicAttr  :   dp属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetDPDynamicAttr(VI_PIPE ViPipe, const ISP_DP_DYNAMIC_ATTR_S *pstDPDynamicAttr);

/**
   \brief 获取dp属性
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstDPDynamicAttr  :   dp属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetDPDynamicAttr(VI_PIPE ViPipe, ISP_DP_DYNAMIC_ATTR_S *pstDPDynamicAttr);

/**
   \brief 设置shading属性
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstShadingAttr  :   shading属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetMeshShadingAttr(VI_PIPE ViPipe, const ISP_SHADING_ATTR_S *pstShadingAttr);

/**
   \brief 获取shading属性
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstShadingAttr :   shading属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetMeshShadingAttr(VI_PIPE ViPipe, ISP_SHADING_ATTR_S *pstShadingAttr);

/**
   \brief 设置hdr shading属性
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstHdrShadingAttr  :   hdr shading属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetMeshHdrShadingAttr(VI_PIPE ViPipe, const ISP_HDR_SHADING_ATTR_S *pstHdrShadingAttr);

/**
   \brief 获取hdr shading属性
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstHdrShadingAttr :   hdr shading属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetMeshHdrShadingAttr(VI_PIPE ViPipe, ISP_HDR_SHADING_ATTR_S *pstHdrShadingAttr);

/**
   \brief 设置nr属性
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstNrAttr  :   nr属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetNRAttr(VI_PIPE ViPipe, const ISP_NR_ATTR_S *pstNRAttr);

/**
   \brief 获取nr属性
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstNRAttr :   nr属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetNRAttr(VI_PIPE ViPipe, ISP_NR_ATTR_S *pstNRAttr);

/**
   \brief 设置gamma属性
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstGammaAttr  :   gamma属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetGammaAttr(VI_PIPE ViPipe, const ISP_GAMMA_ATTR_S *pstGammaAttr);

/**
   \brief 获取gamma属性
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstGammaAttr :   gamma属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetGammaAttr(VI_PIPE ViPipe, ISP_GAMMA_ATTR_S *pstGammaAttr);

/**
   \brief 设置cac属性
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstCacAttr  :   cac属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetCacAttr(VI_PIPE ViPipe, const ISP_CAC_ATTR_S *pstCacAttr);

/**
   \brief 获取cac属性
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstCacAttr  :   cac属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetCacAttr(VI_PIPE ViPipe, ISP_CAC_ATTR_S *pstCacAttr);

/**
   \brief 设置sharpen属性
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstIspshpAttr  :   Sharpen属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetIspSharpenAttr(VI_PIPE ViPipe, const ISP_SHARPEN_ATTR_S *pstIspShpAttr);

/**
   \brief 获取Sharpen属性
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstIspShpAttr  :   Sharpen属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetIspSharpenAttr(VI_PIPE ViPipe, ISP_SHARPEN_ATTR_S *pstIspShpAttr);

/**
   \brief 设置ca属性
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstCAAttr  :   ca属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetCAAttr(VI_PIPE ViPipe, const ISP_CA_ATTR_S *pstCAAttr);

/**
   \brief 获取ca属性
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstCAAttr  :   ca属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetCAAttr(VI_PIPE ViPipe, ISP_CA_ATTR_S *pstCAAttr);

/**
   \brief 设置demosaic属性
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstDemosaicAttr  :   demosaic属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetDemosaicAttr(VI_PIPE ViPipe, const ISP_DEMOSAIC_ATTR_S *pstDemosaicAttr);

/**
   \brief 获取demosaic属性
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstDemosaicAttr  :   demosaic属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetDemosaicAttr(VI_PIPE ViPipe, ISP_DEMOSAIC_ATTR_S *pstDemosaicAttr);

/**
   \brief 设置black level属性
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstBlackLevelAttr  :   black level属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetBlackLevelAttr(VI_PIPE ViPipe, const ISP_BLACK_LEVEL_S *pstBlackLevel);

/**
   \brief 获取black level属性
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstBlackLevelAttr  :   black level属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetBlackLevelAttr(VI_PIPE ViPipe, ISP_BLACK_LEVEL_S *pstBlackLevel);

/**
   \brief 设置clut lut属性
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstClutLutAttr  :   clut lut属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetClutCoeff(VI_PIPE ViPipe, const ISP_CLUT_LUT_S *pstClutLUT);

/**
   \brief 获取clut lut属性
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstClutLutAttr  :   clut lut属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetClutCoeff(VI_PIPE ViPipe, ISP_CLUT_LUT_S *pstClutLUT);
/**
   \brief 设置DCFInfo属性
   @attention 新修改接口不支持
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstClutLutAttr  :   pstIspDCF属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/

AR_S32 AR_MPI_ISP_SetDCFInfo(VI_PIPE ViPipe, const ISP_DCF_INFO_S *pstIspDCF);
/**
   \brief 获取DCFInfo属性
   @attention 新修改接口，不支持
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstIspDCF  :   pstIspDCF属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/

AR_S32 AR_MPI_ISP_GetDCFInfo(VI_PIPE ViPipe, ISP_DCF_INFO_S *pstIspDCF);
/**
   \brief 设置属性
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstCSCAttr  :   csc属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/

AR_S32 AR_MPI_ISP_SetCSCAttr(VI_PIPE ViPipe, const ISP_CSC_ATTR_S *pstCSCAttr);
/**
   \brief 获取clut lut属性
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstCSCAttr  :   csc属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/

AR_S32 AR_MPI_ISP_GetCSCAttr(VI_PIPE ViPipe, ISP_CSC_ATTR_S *pstCSCFAttr);
/**
   \brief 设置寄存器数据
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] u32Addr  :   物理地址  
   \param[in] u32Value  :   数据
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/

AR_S32 AR_MPI_ISP_SetRegister(VI_PIPE ViPipe, AR_U32 u32Addr, AR_U32 u32Value);
/**
   \brief 获取寄存器数据
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] u32Addr  :   物理地址  
   \param[out] u32Value  :   数据
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/

AR_S32 AR_MPI_ISP_GetRegister(VI_PIPE ViPipe, AR_U32 u32Addr, AR_U32 *pu32Value);

/**
   \brief 获取寄存器数据
    @attention 未实现
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/

AR_S32 AR_MPI_ISP_GetDngImageStaticInfo(VI_PIPE ViPipe, DNG_IMAGE_STATIC_INFO_S *pstDngImageStaticInfo);
/**
   \brief 获取寄存器数据
    @attention 未实现
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/

AR_S32 AR_MPI_ISP_SetDngColorParam(VI_PIPE ViPipe, const ISP_DNG_COLORPARAM_S *pstDngColorParam);
/**
   \brief 获取寄存器数据
    @attention 未实现
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetDngColorParam(VI_PIPE ViPipe, ISP_DNG_COLORPARAM_S *pstDngColorParam);

/**
   \brief 设置ccm属性
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstCCMAttr  :   ccm属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetCCMAttr(VI_PIPE ViPipe, const ISP_COLORMATRIX_ATTR_S *pstCCMAttr);

/**
   \brief 获取ccm属性
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstCCMAttr  :   ccm属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetCCMAttr(VI_PIPE ViPipe, ISP_COLORMATRIX_ATTR_S *pstCCMAttr);

/**
   \brief 设置cnf属性
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstCnfAttr  :   cnf属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetCnfAttr(VI_PIPE ViPipe, const ISP_CNF_ATTR_S *pstCnfAttr);

/**
   \brief 获取cnf属性
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstCnfAttr  :   cnf属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetCnfAttr(VI_PIPE ViPipe, ISP_CNF_ATTR_S *pstCnfAttr);

/**
   \brief 查询expo信息
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstExpoInfo  :   expo信息
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_QueryExposureInfo(VI_PIPE ViPipe, ISP_EXP_INFO_S *pstExpInfo);
/**
   \brief 设置exposure属性
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstExposureAttr  :   exposure属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetExposureAttr(VI_PIPE ViPipe, const ISP_EXPOSURE_ATTR_S *pstExposureAttr);
/**
   \brief 获取exposure属性
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstExposureAttr  :   exposure属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetExposureAttr(VI_PIPE ViPipe, ISP_EXPOSURE_ATTR_S *pstExposureAttr);

/**
   \brief 查询wb信息
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstWbInfo  :   wb信息
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_QueryWbInfo(VI_PIPE ViPipe, ISP_WB_INFO_S *pstWbInfo);
/**
   \brief 设置wb属性
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstWbAttr  :   wb属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetWbAttr(VI_PIPE ViPipe, const ISP_WB_ATTR_S *pstWbAttr);
/**
   \brief 获取wb属性
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstWbAttr  :   wb属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetWbAttr(VI_PIPE ViPipe, ISP_WB_ATTR_S *pstWbAttr);
/**
   \brief 获取wb增益系数
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstWbAttr  :   wb属性
   \param[in] u16ColorTemp  :   色温属性
   \param[in] s16Shift  :   无效
   \param[out] pu16AWBGain  :   R,Gr,Gb,B 四通道增益, 大小为4的数组
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_CalGainByTemp(VI_PIPE ViPipe, ISP_WB_ATTR_S *pstWBAttr, AR_U16 u16ColorTemp, AR_S16 s16Shift, AR_U16 *pu16AWBGain);

/**
   \brief 查询focus信息
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstFocusInfo  :   focus信息
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_QueryFocusInfo(VI_PIPE ViPipe, ISP_FOCUS_INFO_S *pstFocusInfo);
/**
   \brief 设置focus属性
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstFocusAttr  :   focus属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetFocusAttr(VI_PIPE ViPipe, const ISP_FOCUS_ATTR_S *pstFocusAttr);
/**
   \brief 获取focus属性
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstFocusAttr  :   focus属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetFocusAttr(VI_PIPE ViPipe, ISP_FOCUS_ATTR_S *pstFocusAttr);
/**
   \brief 触发一次对焦
   @attention 新添加接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_Trigger_Focus(VI_PIPE ViPipe);


/**
   \brief 设置saturation属性
   @attention 新添加接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstSaturationTidyAttr  :   saturation属性  0--100
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetSaturationTidyAttr(VI_PIPE ViPipe, const ISP_SATURATION_TIDY_ATTR_S *pstSaturationTidyAttr);

/**
   \brief 获取saturation属性
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstSaturationTidyAttr  :   saturation属性0--100
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetSaturationTidyAttr(VI_PIPE ViPipe, ISP_SATURATION_TIDY_ATTR_S *pstSaturationTidyAttr);

/**
   \brief 设置brightness属性
   @attention 新添加接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstBrightnessTidyAttr  :   brightness属性0--100
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetBrightnessTidyAttr(VI_PIPE ViPipe, const ISP_BRIGHTNESS_TIDY_ATTR_S *pstBrightnessTidyAttr);

/**
   \brief 获取brightness属性
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstBrightnessTidyAttr  :   brightness属性0--100
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetBrightnessTidyAttr(VI_PIPE ViPipe, ISP_BRIGHTNESS_TIDY_ATTR_S *pstBrightnessTidyAttr);

/**
   \brief 设置contrast属性
   @attention 新添加接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstContrastTidyAttr  :   contrast属性0--100
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetContrastTidyAttr(VI_PIPE ViPipe, const ISP_CONTRAST_TIDY_ATTR_S *pstContrastTidyAttr);

/**
   \brief 获取contrast属性
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstContrastTidyAttr  :   contrast属性0--100
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetContrastTidyAttr(VI_PIPE ViPipe, ISP_CONTRAST_TIDY_ATTR_S *pstContrastTidyAttr);
/**
   \brief 设置gamma属性
   @attention 新添加接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstGammaTidyAttr  :  gamma属性0--100
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/



AR_S32 AR_MPI_ISP_SetGammaTidyAttr(VI_PIPE ViPipe, const ISP_GAMMA_TIDY_ATTR_S *pstGammaTidyAttr);
/**
   \brief 获取Gamma属性
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstGammaTidyAttr  :   gamma--100
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/

AR_S32 AR_MPI_ISP_GetGammaTidyAttr(VI_PIPE ViPipe, ISP_GAMMA_TIDY_ATTR_S *pstGammaTidyAttr);



/**
   \brief 设置全局对比度属性（目前只支持红外，可见光不支持）
   @attention 新添加接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstContrastTidyAttr  :   全局对比度属性0--100
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetGlobalContrastTidyAttr(VI_PIPE ViPipe, const ISP_GLOBAL_CONTRAST_TIDY_ATTR_S *pstGlobalContrastTidyAttr);

/**
   \brief 获取全局对比度属性（目前只支持红外，可见光不支持）
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstContrastTidyAttr  :   全局对比度属性0--100
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetGlobalContrastTidyAttr(VI_PIPE ViPipe, ISP_GLOBAL_CONTRAST_TIDY_ATTR_S *pstGlobalContrastTidyAttr);

/**
   \brief 设置sharpness属性
   @attention 新添加接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstSharpnessTidyAttr  :   sharpness属性0--100
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetSharpnessTidyAttr(VI_PIPE ViPipe, const ISP_SHARPNESS_TIDY_ATTR_S *pstSharpnessTidyAttr);

/**
   \brief 获取sharpness属性
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstSharpnessTidyAttr  :   sharpness属性0--100
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetSharpnessTidyAttr(VI_PIPE ViPipe, ISP_SHARPNESS_TIDY_ATTR_S *pstSharpnessTidyAttr);

/**
   \brief 设置细节增强属性（目前只支持红外，可见光不支持）
   @attention 新添加接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstDetailEnhanceTidyAttr  :   细节增强属性0--100
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetDetailEnhanceTidyAttr(VI_PIPE ViPipe, const ISP_DETAIL_ENH_TIDY_ATTR_S *pstDetailEnhTidyAttr);

/**
   \brief 获取细节增强属性（目前只支持红外，可见光不支持）
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstDetailEnhanceTidyAttr  :   细节增强属性0--100
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetDetailEnhanceTidyAttr(VI_PIPE ViPipe, ISP_DETAIL_ENH_TIDY_ATTR_S *pstDetailEnhTidyAttr);

/**
   \brief 设置aec mode属性
   @attention 新添加接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstAecModeTidyAttr  :   aec mode属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetAecModeTidyAttr(VI_PIPE ViPipe, const ISP_AEC_MODE_TIDY_ATTR_S *pstAecModeTidyAttr);

/**
   \brief 获取aec mode属性
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstAecModeTidyAttr  :   aec mode属性
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetAecModeTidyAttr(VI_PIPE ViPipe, ISP_AEC_MODE_TIDY_ATTR_S *pstAecModeTidyAttr);

/**
   \brief 设置aec manu属性
   @attention 新添加接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstAecManuTidyAttr  :   aec manu属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetAecManuTidyAttr(VI_PIPE ViPipe, ISP_AEC_MANU_TIDY_ATTR_S *pstAecManuTidyAttr);

/**
   \brief 获取aec manu属性
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstAecManuTidyAttr  :   aec manu属性
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetAecManuTidyAttr(VI_PIPE ViPipe, ISP_AEC_MANU_TIDY_ATTR_S *pstAecManuTidyAttr);

/**
   \brief 设置awb mode属性
   @attention 新添加接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstAwbModeTidyAttr  :   awb mode属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetAwbModeTidyAttr(VI_PIPE ViPipe, const ISP_AWB_MODE_TIDY_ATTR_S *pstAwbModeTidyAttr);

/**
   \brief 获取awb mode属性
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstAwbModeTidyAttr  :   awb mode属性
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetAwbModeTidyAttr(VI_PIPE ViPipe, ISP_AWB_MODE_TIDY_ATTR_S *pstAwbModeTidyAttr);

/**
   \brief 设置awb manu属性
   @attention 新添加接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstAwbManuTidyAttr  :   awb manu属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetAwbManuTidyAttr(VI_PIPE ViPipe, ISP_AWB_MANU_TIDY_ATTR_S *pstAwbManuTidyAttr);

/**
   \brief 获取awb manu属性
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstAwbManuTidyAttr  :   awb manu属性
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetAwbManuTidyAttr(VI_PIPE ViPipe, ISP_AWB_MANU_TIDY_ATTR_S *pstAwbManuTidyAttr);

/**
   \brief 设置af mode属性
   @attention 新添加接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstAfModeTidyAttr  :   af mode属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetAfModeTidyAttr(VI_PIPE ViPipe, const ISP_AF_MODE_TIDY_ATTR_S *pstAfModeTidyAttr);

/**
   \brief 获取af mode属性
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstAfModeTidyAttr  :   af mode属性
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetAfModeTidyAttr(VI_PIPE ViPipe, ISP_AF_MODE_TIDY_ATTR_S *pstAfModeTidyAttr);

/**
   \brief 设置af roi属性
   @attention 新添加接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstAfRoiTidyAttr  :   af roi属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetAfRoiTidyAttr(VI_PIPE ViPipe, ISP_AF_ROI_TIDY_ATTR_S *pstAfRoiTidyAttr);

/**
   \brief 获取af roi属性
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstAfRoiTidyAttr  :   af roi属性
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetAfRoiTidyAttr(VI_PIPE ViPipe, ISP_AF_ROI_TIDY_ATTR_S *pstAfRoiTidyAttr);

/**
   \brief 获取3a info属性
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pst3aInfoTidyAttr  :   3a info属性
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_Get3aInfoTidyAttr(VI_PIPE ViPipe, ISP_3A_INFO_TIDY_ATTR_S *pst3aInfoTidyAttr);

/**
   \brief 获取3a info ext属性
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pst3aInfoExtTidyAttr  :   3a info ext属性
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_Get3aInfoExtTidyAttr(VI_PIPE ViPipe, ISP_3A_INFO_EXT_TIDY_ATTR_S *pst3aInfoExtTidyAttr);

/**
   \brief 设置hue属性
   @attention 新添加接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstHueTidyAttr  :   hue属性0--100
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetHueTidyAttr(VI_PIPE ViPipe, const ISP_HUE_TIDY_ATTR_S *pstHueTidyAttr);

/**
   \brief 获取hue属性
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstHueTidyAttr  :   hue属性0--100
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetHueTidyAttr(VI_PIPE ViPipe, ISP_HUE_TIDY_ATTR_S *pstHueTidyAttr);

/**
   \brief 设置flip state属性
   @attention 新添加接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstFlipStateTidyAttr  :   flip state属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetFlipStateTidyAttr(VI_PIPE ViPipe, const ISP_FLIP_STATE_TIDY_ATTR_S *pstFlipStateTidyAttr);

/**
   \brief 获取flip state属性
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstFlipStateTidyAttr  :   flip state属性
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetFlipStateTidyAttr(VI_PIPE ViPipe, ISP_FLIP_STATE_TIDY_ATTR_S *pstFlipStateTidyAttr);

/**
   \brief 设置mirror state属性
   @attention 新添加接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstMirrorStateTidyAttr  :   mirror state属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetMirrorStateTidyAttr(VI_PIPE ViPipe, const ISP_MIRROR_STATE_TIDY_ATTR_S *pstMirrorStateTidyAttr);

/**
   \brief 获取mirror state属性
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstMirrorStateTidyAttr  :   mirror state属性
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetMirrorStateTidyAttr(VI_PIPE ViPipe, ISP_MIRROR_STATE_TIDY_ATTR_S *pstMirrorStateTidyAttr);

/**
   \brief 设置banding state属性
   @attention 新添加接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstBandingStateTidyAttr  :   banding state属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetBandingStateTidyAttr(VI_PIPE ViPipe, const ISP_BANDING_STATE_TIDY_ATTR_S *pstBandingStateTidyAttr);

/**
   \brief 获取banding state属性
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstBandingStateTidyAttr  :   banding state属性
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetBandingStateTidyAttr(VI_PIPE ViPipe, ISP_BANDING_STATE_TIDY_ATTR_S *pstBandingStateTidyAttr);

/**
   \brief 设置ir cutter state属性
   @attention 新添加接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstIrCutterStateTidyAttr  :   ir cutter state属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetIrCutterStateTidyAttr(VI_PIPE ViPipe, const ISP_IR_CUTTER_STATE_TIDY_ATTR_S *pstIrCutterStateTidyAttr);

/**
   \brief 获取ir cutter state属性
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstIrCutterStateTidyAttr  :   ir cutter state属性
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetIrCutterStateTidyAttr(VI_PIPE ViPipe, ISP_IR_CUTTER_STATE_TIDY_ATTR_S *pstIrCutterStateTidyAttr);

/**
   \brief 设置led state属性
   @attention 新添加接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstLedStateTidyAttr  :   led state属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetLedStateTidyAttr(VI_PIPE ViPipe, const ISP_LED_STATE_TIDY_ATTR_S *pstLedStateTidyAttr);

/**
   \brief 获取led state属性
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstLedStateTidyAttr  :   led state属性
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetLedStateTidyAttr(VI_PIPE ViPipe, ISP_LED_STATE_TIDY_ATTR_S *pstLedStateTidyAttr);

/**
   \brief 设置ae roi属性
   @attention 新添加接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstAeRoiTidyAttr  :   ae roi属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetAeRoiTidyAttr(VI_PIPE ViPipe, ISP_AE_ROI_TIDY_ATTR_S *pstAeRoiTidyAttr);

/**
   \brief 获取ae roi属性
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstAeRoiTidyAttr  :   ae roi属性
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetAeRoiTidyAttr(VI_PIPE ViPipe, ISP_AE_ROI_TIDY_ATTR_S *pstAeRoiTidyAttr);

/**
   \brief 设置fps range属性
   @attention 新添加接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstFpsRangeTidyAttr  :   fps range属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetFpsRangeTidyAttr(VI_PIPE ViPipe, ISP_FPS_RANGE_TIDY_ATTR_S *pstFpsRangeTidyAttr);

/**
   \brief 获取fps range属性
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstFpsRangeTidyAttr  :   fps range属性
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetFpsRangeTidyAttr(VI_PIPE ViPipe, ISP_FPS_RANGE_TIDY_ATTR_S *pstFpsRangeTidyAttr);

/**
   \brief 设置exp limit属性
   @attention 新添加接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstExpLimitTidyAttr  :   exp limit属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetExpLimitTidyAttr(VI_PIPE ViPipe, const ISP_EXP_LIMIT_TIDY_ATTR_S *pstExpLimitTidyAttr);

/**
   \brief 获取exp limit属性
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstExpLimitTidyAttr  :   exp limit属性
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetExpLimitTidyAttr(VI_PIPE ViPipe, ISP_EXP_LIMIT_TIDY_ATTR_S *pstExpLimitTidyAttr);

/**
   \brief 获取pipe 的 一些limit属性
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstExpLimitTidyAttr  :   pipe limit属性
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetPipeLimitAttr(VI_PIPE ViPipe, ISP_PIPE_LIMIT_TIDY_ATTR_S *pstExpLimitTidyAttr);


/**
   \brief 获取exp poloicy属性
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstExpPolicyTidyAttr  :   exp poloicy属性
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/

AR_S32 AR_MPI_ISP_GetExpPolicyTidyAttr(VI_PIPE ViPipe, ISP_EXP_POLICY_TIDY_ATTR_S *pstExpPolicyTidyAttr);

/**
   \brief 设置exp poloicy属性
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstExpPolicyTidyAttr  :   exp poloicy属性
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/

AR_S32 AR_MPI_ISP_SetExpPolicyTidyAttr(VI_PIPE ViPipe, const ISP_EXP_POLICY_TIDY_ATTR_S *pstExpPolicyTidyAttr);

/**
   \brief 设置BackLight属性
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstBackLightTidyAttr  :   BackLight属性
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/


AR_S32 AR_MPI_ISP_SetBackLightTidyAttr(VI_PIPE ViPipe, const ISP_BackLight_TIDY_ATTR_S *pstBackLightTidyAttr);
/**
   \brief 获取BackLight属性
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstBackLightTidyAttr  :   BackLight属性
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/

AR_S32 AR_MPI_ISP_GetBackLightTidyAttr(VI_PIPE ViPipe, ISP_BackLight_TIDY_ATTR_S *pstBackLightTidyAttr);


/**
   \brief 设置face aec属性
   @attention 新添加接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstFaceAecTidyAttr  :   face aec属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetFaceAecTidyAttr(VI_PIPE ViPipe, ISP_FACE_AEC_TIDY_ATTR_S *pstFaceAecTidyAttr);

/**
   \brief 获取face aec属性
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstFaceAecTidyAttr  :   face aec属性
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetFaceAecTidyAttr(VI_PIPE ViPipe, ISP_FACE_AEC_TIDY_ATTR_S *pstFaceAecTidyAttr);

/**
   \brief 设置zoom属性
   @attention 新添加接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstZoomTidyAttr  :   zoom属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetZoomTidyAttr(VI_PIPE ViPipe, ISP_ZOOM_TIDY_ATTR_S *pstZoomTidyAttr);

/**
   \brief 获取zoom属性
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstZoomTidyAttr  :   zoom属性
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetZoomTidyAttr(VI_PIPE ViPipe, ISP_ZOOM_TIDY_ATTR_S *pstZoomTidyAttr);

/**
   \brief 设置csc属性
   @attention 新添加接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstCscTidyAttr  :   csc属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetCscTidyAttr(VI_PIPE ViPipe, ISP_CSC_TIDY_ATTR_S *pstCscTidyAttr);

/**
   \brief 获取csc属性
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstCscTidyAttr  :   csc属性
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetCscTidyAttr(VI_PIPE ViPipe, ISP_CSC_TIDY_ATTR_S *pstCscTidyAttr);

/**
   \brief 设置de3d strength属性
   @attention 新添加接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstDe3dStrengthTidyAttr  :   de3d strength属性0--100
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetDe3dStrengthTidyAttr(VI_PIPE ViPipe, const ISP_DE3D_STRENGTH_TIDY_ATTR_S *pstDe3dStrengthTidyAttr);

/**
   \brief 获取de3d strength属性
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstDe3dStrengthTidyAttr  :   de3d strength属性0--100
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetDe3dStrengthTidyAttr(VI_PIPE ViPipe, ISP_DE3D_STRENGTH_TIDY_ATTR_S *pstDe3dStrengthTidyAttr);

/**
   \brief 设置rnr strength属性
   @attention 新添加接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstRnrStrengthTidyAttr  :   rnr strength属性0--100
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetRnrStrengthTidyAttr(VI_PIPE ViPipe, const ISP_RNR_STRENGTH_TIDY_ATTR_S *pstRnrStrengthTidyAttr);

/**
   \brief 获取rnr strength属性
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstRnrStrengthTidyAttr  :   rnr strength属性0--100
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetRnrStrengthTidyAttr(VI_PIPE ViPipe, ISP_RNR_STRENGTH_TIDY_ATTR_S *pstRnrStrengthTidyAttr);

/**
   \brief 设置de2d strength属性
   @attention 新添加接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstDe2dStrengthTidyAttr  :   de2d strength属性0--100
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetDe2dStrengthTidyAttr(VI_PIPE ViPipe, ISP_DE2D_STRENGTH_TIDY_ATTR_S *pstDe2dStrengthTidyAttr);

/**
   \brief 获取de2d strength属性
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstDe2dStrengthTidyAttr  :   de2d strength属性0--100
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetDe2dStrengthTidyAttr(VI_PIPE ViPipe, ISP_DE2D_STRENGTH_TIDY_ATTR_S *pstDe2dStrengthTidyAttr);

/**
   \brief 设置defog strength属性
   @attention 新添加接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstDefogStrengthTidyAttr  :   defog strength属性0--100
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetDefogStrengthTidyAttr(VI_PIPE ViPipe, const ISP_DEFOG_STRENGTH_TIDY_ATTR_S *pstDefogStrengthTidyAttr);

/**
   \brief 获取defog strength属性
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstDefogStrengthTidyAttr  :   defog strength属性0--100
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetDefogStrengthTidyAttr(VI_PIPE ViPipe, ISP_DEFOG_STRENGTH_TIDY_ATTR_S *pstDefogStrengthTidyAttr);

/**
   \brief 设置数值宽动态属性
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstStrengthTidyAttr  :   数值宽动 strength属性0--100
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/

AR_S32 AR_MPI_ISP_SetDWDRStrengthTidyAttr(VI_PIPE ViPipe, ISP_DWDR_STRENGTH_TIDY_ATTR_S *pstStrengthTidyAttr);

/**
   \brief 获取数值宽动态属性
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstStrengthTidyAttr  :   数值宽动 strength属性0--100
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetDWDRStrengthTidyAttr(VI_PIPE ViPipe, ISP_DWDR_STRENGTH_TIDY_ATTR_S *pstStrengthTidyAttr);



/**
   \brief 设置src sensor stats属性
   @attention 新添加接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstSrcSensorStatsTidyAttr  :   src sensor stats属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetSrcSensorStatsTidyAttr(VI_PIPE ViPipe, const ISP_SRC_SENSOR_STATS_TIDY_ATTR_S *pstSrcSensorStatsTidyAttr);

/**
   \brief 获取src sensor stats属性
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstSrcSensorStatsTidyAttr  :   src sensor stats属性
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetSrcSensorStatsTidyAttr(VI_PIPE ViPipe, ISP_SRC_SENSOR_STATS_TIDY_ATTR_S *pstSrcSensorStatsTidyAttr);

/**
   \brief 设置sensce属性
   @attention 新添加接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstSensceTidyAttr  :   sensce属性，属性中的名字必须是带路径的
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetSensceTidyAttr(VI_PIPE ViPipe, ISP_SENSCE_TIDY_ATTR_S *pstSensceTidyAttr);
/**
   \brief 获取sensce属性
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pstSensceTidyAttr  :   sensce属性
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetSensceTidyAttr(VI_PIPE ViPipe, ISP_SENSCE_TIDY_ATTR_S *pstSensceTidyAttr);


/**
   \brief 手动的发送vsync，同时屏蔽掉系统的自动vysnc 处理
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pSofProp：      ：  vsync 属性
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/

AR_S32  AR_MPI_ISP_VsyncUpdate(VI_PIPE ViPipe, STRU_AR_CAMERA_SOF_PRO_T *pSofProp);
/**
   \brief 给pipe 注册一个通知回调
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pNotifyRegister      ：  注册的通知回调函数
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_RegisterNotifyCallBack(VI_PIPE ViPipe,ISP_NOTIFY_REGISTER_S *pNotifyRegister);
/**
   \brief 给pipe 注销一个通知回调
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pNotifyRegister      ：  注册的通知回调函数
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_UnRegisterNotifyCallBack(VI_PIPE ViPipe,ISP_NOTIFY_REGISTER_S *pNotifyRegister);
/**
   \brief 发送一次手动的aec 更新，建议使用的时候，设置pipe属性，屏蔽掉指定aec 更新
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pAecOut      ：  aec 更新的属性值
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/

AR_S32  AR_MPI_ISP_AecUpdate(VI_PIPE ViPipe, STRU_AEC_OUT_T *pAecOut);
/**
   \brief 发送一次手动的awb 更新，建议使用的时候，设置pipe属性，屏蔽掉指定awb 更新
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pAwbOut      ：  awb 更新的属性值
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/

AR_S32  AR_MPI_ISP_AwbUpdate(VI_PIPE ViPipe, STRU_AWB_OUT_T *pAwbOut);
/**
   \brief 发送一次手动的af 更新，建议使用的时候，设置pipe属性，屏蔽掉指定af 更新
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pAfOut      ：  af 更新的属性值
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/

AR_S32  AR_MPI_ISP_AfUpdate(VI_PIPE ViPipe, STRU_AF_OUT_T *pAfOut);
/**
   \brief 获取到当前的aec 更新
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pAecOut      ：  aec 更新的属性值
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/

AR_S32  AR_MPI_ISP_GetAecUpdate(VI_PIPE ViPipe, STRU_AEC_OUT_T *pAecOut);
/**
   \brief 获取到当前的awb 更新
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pAwbOut      ：  awb 更新的属性值
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/

AR_S32  AR_MPI_ISP_GetAwbUpdate(VI_PIPE ViPipe, STRU_AWB_OUT_T *pAwbOut);
/**
   \brief 获取到当前的af 更新
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pAfOut      ：  af 更新的属性值
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/
AR_S32  AR_MPI_ISP_GetAfUpdate(VI_PIPE ViPipe, STRU_AF_OUT_T *pAfOut);
/**
   \brief 手动的运行aec
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pPra      ：  aec 运行的输入参数
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/

AR_S32  AR_MPI_ISP_AecRun(VI_PIPE ViPipe, STRU_AEC_CB_EVENT_RUN_PRA_T *pPra);
/**
   \brief 手动的运行awb
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pPra          ：  awb 运行的输入参数
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/

AR_S32  AR_MPI_ISP_AwbRun(VI_PIPE ViPipe, STRU_AWB_CB_EVENT_RUN_PRA_T *pPra);
/**
   \brief 手动的运行af
   @attention 新添加接口
   \param[in] ViPipe         :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[out] pPra          ：  awb 运行的输入参数
   \retval ::AR_SUCCESS      :   成功
   \retval ::AR_FAILURE      :   失败
   \see \n
   N/A
*/
AR_S32  AR_MPI_ISP_AfRun(VI_PIPE ViPipe, STRU_AF_CB_EVENT_RUN_PRA_T *pPra);

/**
   \brief 设置ir_lms属性
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstIrLmsAttr  :   ir_lms属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetIrLmsAttr(VI_PIPE ViPipe, const ISP_IR_LMS_ATTR_S *pstIrLmsAttr);

/**
   \brief 获取ir_lms属性
   @attention 新修改接口
   \param[in] ViPipe        :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstIrLmsAttr  :   ir_lms属性
   \retval ::AR_SUCCESS     :   成功
   \retval ::AR_FAILURE     :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetIrLmsAttr(VI_PIPE ViPipe, ISP_IR_LMS_ATTR_S *pstIrLmsAttr);

/**
   \brief 设置ir_raw_3dnr属性
   @attention 新修改接口
   \param[in] ViPipe            :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstIrRaw3DnrAttr  :   ir_raw_3dnr属性
   \retval ::AR_SUCCESS         :   成功
   \retval ::AR_FAILURE         :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetIrRaw3DnrAttr(VI_PIPE ViPipe, const ISP_IR_RAW_3DNR_ATTR_S *pstIrRaw3DnrAttr);

/**
   \brief 获取ir_raw_3dnr属性
   @attention 新修改接口
   \param[in] ViPipe            :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstIrRaw3DnrAttr  :   ir_raw_3dnr属性
   \retval ::AR_SUCCESS         :   成功
   \retval ::AR_FAILURE         :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetIrRaw3DnrAttr(VI_PIPE ViPipe, ISP_IR_RAW_3DNR_ATTR_S *pstIrRaw3DnrAttr);

/**
   \brief 设置ir_raw_2dnr属性
   @attention 新修改接口
   \param[in] ViPipe            :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstIrRaw2DnrAttr  :   ir_raw_2dnr属性
   \retval ::AR_SUCCESS         :   成功
   \retval ::AR_FAILURE         :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetIrRaw2DnrAttr(VI_PIPE ViPipe, const ISP_IR_RAW_2DNR_ATTR_S *pstIrRaw2DnrAttr);

/**
   \brief 获取ir_raw_2dnr属性
   @attention 新修改接口
   \param[in] ViPipe            :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstIrRaw2DnrAttr  :   ir_raw_2dnr属性
   \retval ::AR_SUCCESS         :   成功
   \retval ::AR_FAILURE         :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetIrRaw2DnrAttr(VI_PIPE ViPipe, ISP_IR_RAW_2DNR_ATTR_S *pstIrRaw2DnrAttr);

/**
   \brief 设置ir_raw_gtm属性
   @attention 新修改接口
   \param[in] ViPipe            :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstIrRawGtmAttr   :   ir_raw_gtm属性
   \retval ::AR_SUCCESS         :   成功
   \retval ::AR_FAILURE         :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetIrRawGtmAttr(VI_PIPE ViPipe, const ISP_IR_RAW_GTM_ATTR_S *pstIrRawGtmAttr);

/**
   \brief 获取ir_raw_gtm属性
   @attention 新修改接口
   \param[in] ViPipe            :   PIPE号, 取值范围：[0, VI_MAX_PIPE_NUM]
   \param[in] pstIrRawGtmAttr   :   ir_raw_gtm属性
   \retval ::AR_SUCCESS         :   成功
   \retval ::AR_FAILURE         :   失败
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetIrRawGtmAttr(VI_PIPE ViPipe, ISP_IR_RAW_GTM_ATTR_S *pstIrRawGtmAttr);

/**
   \brief 设置IR Sensor及IR ISP参数。
   @attention 新添加接口
   \param[in] ViPipe        :   PIPE号。
   \param[in] pstIrParams   :   IR Sensor及IR ISP参数指针。
   \retval ::0              :   成功。
   \retval ::non-zero       :   失败，其值为错误码。
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SetIrParams(VI_PIPE ViPipe, ISP_IR_PARAMS_S *pstIrParams);

/**
   \brief 获取IR Sensor及IR ISP参数。
   @attention 新添加接口
   \param[in] ViPipe        :   PIPE号。
   \param[out] pstIrParams  :   IR Sensor及IR ISP参数指针。
   \retval ::0              :   成功。
   \retval ::non-zero       :   失败，其值为错误码。
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetIrParams(VI_PIPE ViPipe, ISP_IR_PARAMS_S *pstIrParams);

/**
   \brief 获取Sensor的温度。
   @attention 新添加接口
   \param[in] ViPipe            :   PIPE号。
   \param[out] ps32Temperature  :   Sensor温度指针。
   \retval ::0                  :   成功。
   \retval ::non-zero           :   失败，其值为错误码。
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetSnsTemperature(VI_PIPE ViPipe, AR_S32 *ps32Temperature);

/**
   \brief 开关sensor快门。
   @attention 新添加接口
   \param[in] ViPipe            :   PIPE号。
   \param[in] bOpen             :   AR_TRUE: 打开快门；AR_FALSE: 关闭快门。
   \retval ::0                  :   成功。
   \retval ::non-zero           :   失败，其值为错误码。
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_SnsSwitchShutter(VI_PIPE ViPipe, AR_BOOL bOpen);

/**
   \brief 红外软件算法。
   @attention 新添加接口
   \param[in] ViPipe            :   PIPE号。
   \param[in] pstInput          :   红外软件算法输入信息。
   \param[out] pstOutput        :   红外软件算法输出信息。
   \retval ::0                  :   成功。
   \retval ::non-zero           :   失败，其值为错误码。
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_IrAlgoProcess(VI_PIPE ViPipe, IR_ALGO_INPUT_S *pstInput, IR_ALGO_OUTPUT_S *pstOutput);

/**
   \brief 获取红外sensor属性（主要包括红外标定参数size、occ类型、occ单调性、sensor输出图像size等等）。
   @attention 新添加接口
   \param[in] ViPipe            :   PIPE号。
   \param[out] pstIrSnsAttr     :   红外sensor属性指针。
   \retval ::0                  :   成功。
   \retval ::non-zero           :   失败，其值为错误码。
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetIrSnsAttr(VI_PIPE ViPipe, IR_SNS_ATTR_S *pstIrSnsAttr);

/**
   \brief 启动红外sensor参数标定。
   @attention 新添加接口
   \param[in] ViPipe            :   PIPE号。
   \param[in] pstCalibCfg       :   红外sensor参数标定的配置信息（包括帧间隔、目标范围等）。
   \retval ::0                  :   成功。
   \retval ::non-zero           :   失败，其值为错误码。
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_StartIrSnsCalib(VI_PIPE ViPipe, IR_SNS_CALIB_CFG_S *pstCalibCfg);

/**
   \brief 获取红外sensor参数标定结果。
   @attention 新添加接口
   \param[in] ViPipe            :   PIPE号。
   \param[out] pstCalibResult   :   红外sensor参数标定结果指针。
   \retval ::0                  :   成功。
   \retval ::non-zero           :   失败，其值为错误码。
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetIrSnsCalibResult(VI_PIPE ViPipe, IR_SNS_CALIB_RESULT_S *pstCalibResult);

/**
   \brief 取消红外sensor参数标定（取消正在进行中的标定）。
   @attention 新添加接口
   \param[in] ViPipe            :   PIPE号。
   \retval ::0                  :   成功。
   \retval ::non-zero           :   失败，其值为错误码。
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_CancelIrSnsCalib(VI_PIPE ViPipe);

/**
   \brief 启动红外occ标定。
   @attention 新添加接口
   \param[in] ViPipe            :   PIPE号。
   \param[in] pstCalibCfg       :   红外occ标定的配置信息（包括帧间隔、目标范围等）。
   \retval ::0                  :   成功。
   \retval ::non-zero           :   失败，其值为错误码。
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_StartIrOccCalib(VI_PIPE ViPipe, IR_OCC_CALIB_CFG_S *pstCalibCfg);

/**
   \brief 获取红外occ标定结果。
   @attention 新添加接口
   \param[in] ViPipe            :   PIPE号。
   \param[out] pstCalibResult   :   红外occ标定结果指针。
   \retval ::0                  :   成功。
   \retval ::non-zero           :   失败，其值为错误码。
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetIrOccCalibResult(VI_PIPE ViPipe, IR_OCC_CALIB_RESULT_S *pstCalibResult);

/**
   \brief 取消红外occ标定（取消正在进行中的标定）。
   @attention 新添加接口
   \param[in] ViPipe            :   PIPE号。
   \retval ::0                  :   成功。
   \retval ::non-zero           :   失败，其值为错误码。
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_CancelIrOccCalib(VI_PIPE ViPipe);

/**
   \brief 启动红外b标定。
   @attention 新添加接口
   \param[in] ViPipe            :   PIPE号。
   \param[in] pstCalibCfg       :   红外b标定的配置信息（包括叠加帧数、目标值等）。
   \retval ::0                  :   成功。
   \retval ::non-zero           :   失败，其值为错误码。
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_StartIrBCalib(VI_PIPE ViPipe, IR_B_CALIB_CFG_S *pstCalibCfg);

/**
   \brief 获取红外b标定结果。
   @attention 新添加接口
   \param[in] ViPipe            :   PIPE号。
   \param[out] pstCalibResult   :   红外b标定结果指针。
   \retval ::0                  :   成功。
   \retval ::non-zero           :   失败，其值为错误码。
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_GetIrBCalibResult(VI_PIPE ViPipe, IR_B_CALIB_RESULT_S *pstCalibResult);

/**
   \brief 取消红外b标定（取消正在进行中的标定）。
   @attention 新添加接口
   \param[in] ViPipe            :   PIPE号。
   \retval ::0                  :   成功。
   \retval ::non-zero           :   失败，其值为错误码。
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_CancelIrBCalib(VI_PIPE ViPipe);

/**
   \brief 红外kb标定。
   @attention 新添加接口
   \param[in] ViPipe            :   PIPE号。
   \param[in] pstCalibCfg       :   红外KB标定配置信息。
   \param[in] pstLowTempFrm     :   低温raw图。
   \param[in] pstHithTempFrm    :   高温raw图。
   \param[out] pstKbBuf         :   标定后的KB数据buffer指针。如不需要得到标定后的KB数据，指针可为NULL。
   \retval ::0                  :   成功。
   \retval ::non-zero           :   失败，其值为错误码。
   \see \n
   N/A
*/
AR_S32 AR_MPI_ISP_IrKBCalib(VI_PIPE ViPipe, IR_KB_CALIB_CFG_S *pstCalibCfg,
    VIDEO_FRAME_S *pstLowTempFrm, VIDEO_FRAME_S *pstHighTempFrm, VIDEO_FRAME_S *pstKbBuf);

/** @} */  /** <!-- ==== API declaration end ==== */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /*__MPI_ISP_H__ */
