#ifndef _CFG_ALARM_H__
#define _CFG_ALARM_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "cfg_common.h"
#include "cfg_ptz.h"

/*****hbbai: need to re-define the alarm parameters*****/
/* alarm in: which gpio and which channels will recorded, which channels will take snapshot.*/
/* alarm out: which gpio will output when alarm happens*/
/* alarm global config: to control whether to record/send email/ftp/... actions when alarm happens */

/* 消息处理方式，可以同时多种处理方式，包括
* 0x00000000 - 无响应
* 0x00000001 - 报警上传中心
* 0x00000002 - 联动录象
* 0x00000004 - 云台联动
* 0x00000008 - 发送Email邮件，根据email配置是否带附件是否有效，决定是否图片。
* 0x00000010 - 本地轮巡（该版本不支持）
* 0x00000020 - 本地提示，监视器上警告
* 0x00000040 - 报警输出
* 0x00000080 - Ftp抓图上传
* 0x00000100 - 蜂鸣
* 0x00000200 - 语音提示
* 0x00000400 - 抓图本地保存
* 0x00000800 - 主动请求对讲
* FTP,EMAIL,抓图本地保存三个功能的抓图通道都是根据结构体成员bySnap决定。
* 如果bySnap没有指定抓图通道，则FTP，抓图本地保存功能失效；EMAIL只发送文本信息，不带图片附件。
*/
typedef enum {
    ALARM_EXCEPTION_NORESPONSE = 0x00000000,
    ALARM_EXCEPTION_UPTOCENTER = 0x00000001,
    ALARM_EXCEPTION_TOREC = 0x00000002,
    ALARM_EXCEPTION_TOPTZ = 0x00000004,
    ALARM_EXCEPTION_TOEMAIL = 0x00000008,
    ALARM_EXCEPTION_TOPOLL = 0x00000010,
    ALARM_EXCEPTION_TOSCREENTIP = 0x00000020,
    ALARM_EXCEPTION_TOALARMOUT = 0x00000040,
    ALARM_EXCEPTION_TOFTP = 0x00000080,
    ALARM_EXCEPTION_TOBEEP = 0x00000100,
    ALARM_EXCEPTION_TOVOICE = 0x00000200,
    ALARM_EXCEPTION_TOSNAP = 0x00000400,
    ALARM_EXCEPTION_TOTALK = 0x00000800,
} ALARM_EXCEPTION_TO;


/***********************************/
/***         alarm               ***/
/***********************************/
typedef struct {
    AR_U32               intervalTime;

    AR_U8                is_email;

    AR_U8                is_rec;
    AR_U32               recTime;
    AR_U32               recStreamNo;

    AR_U8                is_snap;
    AR_U8                isSnapUploadToFtp;
    AR_U8                isSnapUploadToWeb;
    AR_U8                isSnapUploadToCms;
    AR_U8                isSnapSaveToSd;
    AR_U32               snapNum;
    AR_U32 	          interval;

    AR_U8                is_alarmout;
    AR_U32               duration;

    AR_U8                is_beep;
    AR_U32               beepTime;

    AR_U8                is_ptz;
    PTZ_LINK           ptzLink;
} HANDLE_EXCEPTION;

typedef struct {
    AR_S32            enable;
    AR_S32            gpio;
    AR_S8             alarmInName[MAX_STR_LEN_32];
    AR_S8             defaultState;       /* 0-low 1-high */
    AR_S8             activeState;        /* 0-low 1-high*/
    HANDLE_EXCEPTION handle;
    AR_U32           u32RecChn; //which channel should be recorded and snaped when a button is pressed.
} NET_ALARMINCFG;

typedef struct {
    AR_S32   enable;
    AR_S32   gpio;
    AR_S8    alarmOutName[MAX_STR_LEN_32];
    AR_S8    defaultState;       /* 0-low 1-high */
    AR_S8    activeState;        /* 0-low 1-high*/
    AR_S8    powerOnState;       /* 0-pulse 1-continuous*/
    AR_S32   pulseDuration;      /* 1000 - 10000*/
} NET_ALARMOUTCFG;

typedef struct {
	AR_S8  gpio_enable;
	AR_S8  in_level;
	AR_S8  out_level;
	AR_S8  io_linkage;

	AR_S8 upload_interval;
	AR_S8 alarmbymail;
	AR_S8 schedule_enable;
	AR_S8 snapshot_enable;
	AR_S8 recorder_enable;
	AR_S8 move_perset;
	AR_S8 alarmMute;
} ALARMCFG;

//Currently we suppose there's one gpio in and one gpio out for alarm.
typedef struct {
    NET_ALARMINCFG  alarmIn;
    NET_ALARMOUTCFG alarmOut;
	ALARMCFG        cfg;
} IPC_CFG_ALARM_S;

extern AR_S32 IPC_CFG_ALARM_Save();
extern AR_S32 IPC_CFG_ALARM_Load(CFG_RUN_MODE_E enMode);
extern AR_S32 IPC_CFG_ALARM_LoadDefault();
extern void IPC_CFG_ALARM_Print();
extern IPC_CFG_ALARM_S * IPC_CFG_ALARM_GetParam();

#define ALARM_CFG_FILE "cfg_alarm.json"

#ifdef __cplusplus
}
#endif
#endif

