#ifndef __AR_AVI_UTILITY_H__
#define __AR_AVI_UTILITY_H__

#ifdef __cplusplus
extern "C" {
#endif
typedef struct tagAVI_DMS_TIME
{
    AR_U32    dwYear;
    AR_U32    dwMonth;
    AR_U32    dwDay;
    AR_U32    dwHour;
    AR_U32    dwMinute;
    AR_U32    dwSecond;
} AVI_DMS_TIME;

typedef enum emRECORD_TYPE_E
{
    RECORD_TYPE_SCHED   = 0x000001,  //定时录像 by slice
    RECORD_TYPE_MOTION  = 0x000002,  //移到侦测录像
    RECORD_TYPE_ALARM   = 0x000004,  //报警录像
    RECORD_TYPE_CMD     = 0x000008,  //命令录像
    RECORD_TYPE_MANU    = 0x000010,  //手工录像
    RECORD_TYPE_SCHED_2 = 0x000011,  //定时录像 by period
    RECORD_TYPE_ALL     = 0xff,      //全部录像
} E_RECORD_TYPE;

typedef enum
{
	ENC_TYPE_H264 , //H264
	ENC_TYPE_H265,
	ENC_TYPE_MPEG,      //< MPEG4
	ENC_TYPE_MJPEG,    //< Motion JPEG
	ENC_TYPE_YUV422,
	ENC_TYPE_COUNT     //< do not use
}eEncType;

AR_S32 AR_AVI_WriteChar(FILE *file, AR_CHAR c);
AR_S32 AR_AVI_WriteInt32(FILE *file, AR_S32 i);
AR_S32 AR_AVI_WriteFourcc(FILE *file, AR_S32 fourcc);
AR_S32 AR_AVI_WriteData(FILE *file, AR_CHAR *data, AR_S32 size);
AR_S32 AR_AVI_ReadChar(FILE *file, AR_CHAR *c);
AR_S32 AR_AVI_ReadInt32(FILE *file, AR_S32 *i);
AR_S32 AR_AVI_ReadFourcc(FILE *file, AR_S32 *fourcc);
AR_S32 AR_AVI_ReadData(FILE *file, AR_UCHAR *data, AR_S32 size);
void AR_AVI_Write32(void *pp, AR_S32 w);
AR_S32 AR_AVI_Read32(void *pp);
void AR_AVI_ExtractSubstr(AR_CHAR *dest, AR_CHAR *src, AR_S32 start, AR_S32 end);
AR_S32 AR_AVI_FindLastIndex(AR_CHAR *str, AR_CHAR c);
AR_U32 AR_AVI_GetChnFromName(AR_CHAR *file_name);
AR_U64 AR_AVI_GetStartTimeFromName(AR_CHAR *dir, AR_CHAR *file_name);
AR_U64 AR_AVI_GetStopTimeFromName(AR_CHAR *dir, AR_CHAR *file_name);
AR_U64 AR_AVI_TimeToU64(AVI_DMS_TIME *p);
void AR_AVI_U64ToTime(AVI_DMS_TIME *ptime, AR_U64 tmp64);
void AR_AVI_GetDateAndTimeStr(AR_CHAR *str_date, AR_CHAR *str_time, AR_S32 preSeconds);
void AR_AVI_msleep(AR_U32 mSec);

#ifdef __cplusplus
}
#endif
#endif

