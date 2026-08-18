#ifndef __HAL_DSP_API_H__
#define __HAL_DSP_API_H__
#include "hal_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


/**
* @brief  dsp 数据处理参数信息
* @param  Name[32] dsp注册callback名称
* @param  Priority 任务处理优先级
* @param  MsgBodyAddr 消息地址
* @param  MsgLen 消息大小
*/
typedef struct
{
	AR_CHAR	 s8Name[32]; 
	AR_U32 u32Priority;
	AR_U64 u64MsgBodyAddr;
	AR_U32 u32MsgLen;
} AR_DSP_TASK_S;

/**
* @brief  dsp 数据处理参数信息
* @param  Name[32] dsp注册callback名称
* @param  Priority 任务处理优先级
* @param  MsgBodyAddr 消息地址
* @param  MsgLen 消息大小
* @param  s32ErrCode dsp 返回error code
* @param  u32Time dsp 返回callback函数处理时间
*/

typedef struct 
{
	AR_CHAR 	s8Name[32];
	AR_U32 		u32Priority;
	AR_U64 		u64MsgBodyAddr;
	AR_U32		u32MsgLen;
	AR_S32		s32ErrCode;
	AR_U32		u32Time;
}AR_DSP_TASK_ACK_S;


/**
* @brief  dsp bin文件加载接口
* @param  u32DspId dsp对应核心ID 默认为 0，pcFilePath ceva.bin文件地址 
* @retval retval > 0 成功，其他加载失败
*/
AR_S32 ar_hal_dsp_load_bin(AR_U32 u32DspId, AR_CHAR* pcFilePath);
/**
* @brief  dsp 上电接口
* @param  u32DspId dsp对应核心ID 默认为 0
* @retval 
*/
void ar_hal_dsp_poweron(AR_U32 u32DspId);
/**
* @brief  dsp 下电接口
* @param  u32DspId dsp对应核心ID 默认为 0
* @retval
*/
void ar_hal_dsp_poweroff(AR_U32 u32DspId);
/**
* @brief  dsp 核心使能接口
* @param  u32DspId dsp对应核心ID 默认为 0
* @retval
*/
void ar_hal_dsp_enable_core(AR_U32 u32DspId);
/**
* @brief  dsp 核心去使能接口
* @param  u32DspId dsp对应核心ID 默认为 0
* @retval
*/
void ar_hal_dsp_disable_core(AR_U32 u32DspId);
/**
* @brief  dsp 核心频率设置接口
* @param  u32DspId dsp对应核心ID 默认为 0，u32Frequency dsp核心工作频率
* @retval
*/
void ar_hal_dsp_set_frequency(AR_U32 u32DspId, AR_U32 u32Frequency);
/**
* @brief  dsp 数据处理callback接口
* @param  u32DspId dsp对应核心ID 默认为 0，pstTask dsp处理数据信息，IsBlocked 0:nonblock 1:block
* @retval retval >= 0 成功，其他数据处理失败
*/
AR_S32 ar_hal_dsp_remotecall(AR_U32 u32DspId, AR_DSP_TASK_S *pstTask, AR_U32 u32Blocked);
/**
* @brief  dsp 数据处理查询接口
* @param  u32DspId dsp对应核心ID 默认为 0，pstTask dsp当前处理数据信息，IsBlocked 0:nonblock 1:block
* @retval retval >= 0 成功，其他查询失败
*/
AR_S32 ar_hal_dsp_query(AR_U32 u32DspId, AR_DSP_TASK_ACK_S *pstTask, AR_U32 u32Blocked);
/**
* @brief  dsp 任务状态打印物理地址设置接口
* @param  u32DspId dsp对应核心ID 默认为 0，u32StatusAddr dsp在DDR中预留的起始物理地址
* @retval retval >= 0 成功，其他设置失败
*/
AR_S32 ar_hal_dsp_set_status_addr(AR_U32 u32DspId, AR_U32 u32StatusAddr);

AR_S32 ar_read_file_len(AR_CHAR* filepath);

/**
* @brief  dsp 获取Apb Timer计数接口
* @param  pu32TickCnt 对应Apb Timer计数值指针
* @param  u32Index 对应Apb Timer索引 0 - 7
* @retval retval >= 0 成功，其他设置失败
*/
AR_S32 ar_dsp_get_system_timer(AR_U32 *pu32TickCnt, AR_U32 u32Index);

#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif /* End of #ifdef __cplusplus */


#endif 
