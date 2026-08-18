#ifndef __MPI_DSP_API_H__
#define __MPI_DSP_API_H__
#include "mpi_type.h"
#include "hal_dsp_api.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */



/**
* @brief  dsp bin文件加载接口
* @param  u32DspId dsp对应核心ID 默认为 0，filepath ceva.bin文件地址
* @retval retval > 0 成功，其他加载失败
*/
AR_S32 AR_MPI_DSP_LoadBin(AR_U32 u32DspId, AR_CHAR* filepath);
/**
* @brief  dsp 上电接口
* @param  u32DspId dsp对应核心ID 默认为 0
* @retval
*/
void AR_MPI_DSP_PowerOn(AR_U32 u32DspId);
/**
* @brief  dsp 下电接口
* @param  u32DspId dsp对应核心ID 默认为 0
* @retval
*/
void AR_MPI_DSP_PowerOff(AR_U32 u32DspId);
/**
* @brief  dsp 核心使能接口
* @param  u32DspId dsp对应核心ID 默认为 0
* @retval
*/
void AR_MPI_DSP_EnableCore(AR_U32 u32DspId);
/**
* @brief  dsp 核心去使能接口
* @param  u32DspId dsp对应核心ID 默认为 0
* @retval
*/
void AR_MPI_DSP_DisableCore(AR_U32 u32DspId);
/**
* @brief  dsp 核心频率设置接口
* @param  u32DspId dsp对应核心ID 默认为 0，u32Frequency dsp核心工作频率
* @retval
*/
void AR_MPI_DSP_SetFrequency(AR_U32 u32DspId, AR_U32 u32Frequency);

/**
* @brief  dsp 数据处理callback接口
* @param  u32DspId dsp对应核心ID 默认为 0，pstTask dsp处理数据信息，u32Blocked 0:nonblock 1:block
* @retval retval >= 0 成功，其他数据处理失败
*/
AR_S32 AR_MPI_DSP_RemoteCall(AR_U32 u32DspId, AR_DSP_TASK_S *pstTask, AR_U32 u32Blocked);
/**
* @brief  dsp 数据处理查询接口
* @param  u32DspId dsp对应核心ID 默认为 0，pstTask dsp当前处理数据信息，u32Blocked 0:nonblock 1:block
* @retval retval >= 0 成功，其他查询失败
*/
AR_S32 AR_MPI_DSP_Query(AR_U32 u32DspId, AR_DSP_TASK_ACK_S *pstTask, AR_U32 u32Blocked);
/**
* @brief  dsp 任务状态打印物理地址设置接口
* @param  u32DspId dsp对应核心ID 默认为 0，u32StatusAddr dsp在DDR中预留的起始物理地址
* @retval retval >= 0 成功，其他设置失败
*/
AR_S32 AR_MPI_DSP_Set_Status_Phyaddr(AR_U32 u32DspId, AR_U32 u32StatusAddr);
/**
* @brief  dsp 获取系统Apb Timer接口
* @param  u32Index 系统timer索引[0-8]
* @param  pu32TickCnt 对应索引值timer计数
* @retval retval >= 0 成功，其他设置失败
*/
AR_S32 AR_MPI_DSP_Get_System_Timer(AR_U32 *pu32TickCnt, AR_U32 u32Index);

#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif /* End of #ifdef __cplusplus */


#endif 
