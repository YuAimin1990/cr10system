#ifndef __EVENTALARM_H__
#define __EVENTALARM_H__

#ifdef __cplusplus
extern "C"
{
#endif
#include "pf_type.h"

/************************************************************************/
/*       设备报警事件类型                                               */
/************************************************************************/
typedef enum __ALARM_TYPE_E
{
    ALARM_TYPE_ALARMIN = 0,     //0:信号量报警开始
    ALARM_TYPE_DISK_FULL,       //1:硬盘满
    ALARM_TYPE_VLOST,           //2:信号丢失
    ALARM_TYPE_VMOTION,         //3:移动侦测
    ALARM_TYPE_DISK_UNFORMAT,   //4:硬盘未格式化
    ALARM_TYPE_DISK_RWERR,      //5:读写硬盘出错,
    ALARM_TYPE_VSHELTER,        //6:遮挡报警
    ALARM_TYPE_VFORMAT_UNMATCH, //7:制式不匹配
    ALARM_TYPE_ILLEGAL_ACCESS,  //8:非法访问
    ALARM_TYPE_ALARMIN_RESUME,  //9:信号量报警恢复
    ALARM_TYPE_VLOST_RESUME,    //10:视频丢失报警恢复
    ALARM_TYPE_VMOTION_RESUME,  //11:视频移动侦测报警恢复
    ALARM_TYPE_NET_BROKEN,      //12:网络断开
    ALARM_TYPE_IP_CONFLICT,     //13:IP冲突
    ALARM_TYPE_ITEV,			//14:智能分析
    ALARM_TYPE_ITEV_RESUME,		//15:智能分析报警恢复
	ALARM_TYPE_TEMPERATURE,		//16:温度
	ALARM_TYPE_HUMIDITY,		//17:湿度
    ALARM_TYPE_ITEV_TRIPWIRE,  //16:绊线
    ALARM_TYPE_ITEV_TRIPWIRE_RESUME,  //17:绊线恢复
    ALARM_TYPE_ITEV_VIDEOFOOL,  //16:视频异常
    ALARM_TYPE_ITEV_VIDEOFOOL_RESUME,  //17:异常恢复恢复
    ALARM_TYPE_BUTT,
}ALARM_TYPE_E;

/*
** 参数说明：
** Channel：通道号，从0开始
** AlarmType：告警类型 ALARM_TYPE_E
** Action：告警状态 1：告警  0：消除
** param: 扩充指针暂不用
*/
typedef AR_S32 (*event_alarm_cb_func)(AR_S32 nChannel, AR_S32 nAlarmType, AR_S32 nAction, void* pParam);

/* 初始化事件监听 */
AR_S32 IPC_MID_ALARM_Init(void);


/* 注册事件监听，alarm_cb为事件回调函数，返回值为事件句柄，返回NULL，表示失败 */
void* IPC_MID_ALARM_Open(event_alarm_cb_func alarm_cb);

/* 关闭事件监听*/
AR_S32 IPC_MID_ALARM_Close(void *eventhandle);

/* 其他模块调用该函数触发一个事件回调函数
** 参数说明：
** Channel：通道号，从0开始
** AlarmType：告警类型 ALARM_TYPE_E
** Action：告警状态 1：告警  0：消除
** param: 扩充指针暂不用
*/
AR_S32 IPC_MID_ALARM_Trigger(AR_S32 channel, ALARM_TYPE_E alarmType, AR_S32 action, void* param);

/* 销毁事件监听 */
AR_S32 IPC_MID_ALARM_UnInit(void);


#ifdef __cplusplus
}
#endif
#endif
