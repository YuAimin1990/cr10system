#ifndef __MID_UTIL_API_H__
#define __MID_UTIL_API_H__

#include "util_common.h"
#include "pf_type.h"

#ifdef __cplusplus
extern "C"
{
#endif


#define BZ_CMD_LEN 128

typedef struct
{
	AR_S32 Year;
	AR_S32 Mon;
	AR_S32 Day;
	AR_S32 Week;
	AR_S32 Hour;
	AR_S32 Min;
	AR_S32 Sec;
	AR_S32 mSec;
	unsigned long TimeTick_mSec;
} IPC_SYS_TIME_INFO_S;

typedef	void *(*ThreadEntryPtrType)(void *);
typedef struct {
   ThreadEntryPtrType callback;
   void * arg;
   AR_S32 arg_size;
} ThreadArgs;

AR_S32 IPC_MID_UTIL_Sleep(AR_S32 nDelay);
AR_S32 IPC_MID_UTIL_Rand(AR_S32 min, AR_S32 max);

AR_S32 IPC_MID_UTIL_SetThreadName(const AR_CHAR *name);
AR_S32 IPC_MID_UTIL_CreateDetachThread(ThreadEntryPtrType entry, void *para, pthread_t *pid);
AR_S32 IPC_MID_UTIL_CreateThread(ThreadEntryPtrType entry, void *para, pthread_t *pid);

AR_S32 IPC_MID_UTIL_SysCall(const AR_CHAR *cmd, AR_CHAR *buff, AR_S32 bufsize);
AR_S32 IPC_MID_UTIL_ForkSysCall(const AR_CHAR *cmdline);

#if 0 //used to connect PC tools
AR_S32 SetSockAttr(AR_S32 fd);
AR_S32 CreateTcpSock(AR_CHAR *ipaddr, AR_S32 port);
AR_S32 CreateUdpSock(AR_CHAR *ipaddr, AR_S32 port);
AR_S32 CreateBroadcastSock(AR_S32 recv_port);
AR_S32 BoardCastSendTo(AR_S32 fSockSearchfd, AR_CHAR *multi_group, AR_S32 send_port, const AR_CHAR* inData, const AR_S32 inLength);
AR_S32 BoardCastSendTo2(AR_S32 fSockSearchfd, struct sockaddr_in *paddr, const AR_CHAR* inData, const AR_S32 inLength);

AR_S32 tcp_select_send(AR_S32 sock, AR_CHAR *szbuf, AR_S32 len, AR_S32 timeout);
AR_S32 tcp_send(AR_S32 sock, AR_CHAR *szbuf, AR_S32 len, AR_S32 re_counts);
AR_S32 tcp_select_recv(AR_S32 sock, AR_CHAR *szbuf, AR_S32 len, AR_S32 timeout);
#endif

void IPC_MID_UTIL_SetBit(AR_U32 *x, AR_S32 offset, pthread_mutex_t *p_mutex);
void IPC_MID_UTIL_ClearBit(AR_U32 *x, AR_S32 offset, pthread_mutex_t *p_mutex);
AR_S32 IPC_MID_UTIL_CheckBit(AR_U32 x, AR_S32 offset, pthread_mutex_t *p_mutex);

AR_S32 IPC_MID_UTIL_GetLabelValue(const AR_CHAR *label, AR_CHAR *path, AR_CHAR *value, AR_S32 size);
AR_S32 IPC_MID_UTIL_SetLabelValue(const AR_CHAR *label, AR_CHAR *path, AR_CHAR *value);

AR_U32 IPC_MID_UTIL_Crc32(uint32_t val, const void *ss, AR_S32 len);

#define EV_STRING_ENC_KEY_STREAM "ev?ot139!@&ah6^lev?ot139zm&ah6*l"
AR_S32 IPC_MID_UTIL_EncryptStream(AR_CHAR *cData, AR_S32 len, AR_CHAR *cKey);
AR_S32 IPC_MID_UTIL_DecryptStream(AR_CHAR *cData, AR_S32 len, AR_CHAR *cKey);

#define CGI_MAX_LEN 	256

AR_S32 IPC_MID_UTIL_CopyFile(AR_CHAR *src_name, AR_CHAR *des_name);
AR_S32 IPC_MID_UTIL_DeleteFile(AR_CHAR * path);

AR_S32 IPC_MID_UTIL_GetSysTime(struct tm* t);
AR_S32 IPC_MID_UTIL_SetTimeAndZone(AR_S32 utcOffset, AR_S32 zone);
AR_S32 IPC_MID_UTIL_SetZone(AR_S32 gmt);
AR_S32 IPC_MID_UTIL_ShowTime();
AR_S32 IPC_MID_UTIL_SetLocalTime(IPC_SYS_TIME_INFO_S *t);
AR_S32 IPC_MID_UTIL_GetLocalTime(IPC_SYS_TIME_INFO_S *t);
AR_S32 IPC_MID_UTIL_GetDateAndTimeStr(AR_CHAR *str_date, AR_CHAR *str_time, AR_S32 preSeconds);

AR_S32 IPC_MID_UTIL_ReadFile(AR_CHAR *path, AR_S32 filesize, void *dst);
AR_S32 IPC_MID_UTIL_GetFileSize(AR_CHAR *path);
void * IPC_MID_UTIL_OpenFile(AR_CHAR* path, AR_S32 *size);
void IPC_MID_UTIL_Killall(const char * name);

void IPC_MID_UTIL_FreeMem(void **mem);

AR_S32 IPC_MID_UTIL_ExtractSubstr(AR_CHAR *dest, AR_CHAR *src, int start, int end);
int64_t getTickInterval(int64_t begin);
unsigned long getTickCount(void);

void dump_to_file(const char* file_path, const void* data, int len);

int doSystem(const char *fmt, ...);


#ifdef __cplusplus
}
#endif
#endif

