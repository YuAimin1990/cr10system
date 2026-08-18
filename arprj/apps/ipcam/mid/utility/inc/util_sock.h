#ifndef __MID_UTIL_SOCK_H_
#define __MID_UTIL_SOCK_H_

#ifdef __cplusplus
extern "C"
{
#endif


#define BZ_MAX_LINK_NUM 10


typedef struct {
	int n_socket;
	int n_flag;	// 0关闭，1打开
	//int is_set_key; // 是否设置成发送关键帧
	int v_no;
    int is_wait_i; // 是否需要等待关键帧
} IPC_MID_UTIL_SOCK_LvSocketInfo;

typedef struct {
	IPC_MID_UTIL_SOCK_LvSocketInfo main_stream_socket;
	IPC_MID_UTIL_SOCK_LvSocketInfo sub_stream_socket;
} IPC_MID_UTIL_SOCK_LvSocket;

typedef struct {
	int n_socket;
} IPC_MID_UTIL_SOCK_TalkSocket;

typedef struct {
	int n_socket;
    int control; //0: close, 1:open, 2:pause
} IPC_MID_UTIL_SOCK_PbSocket;

typedef struct {
    int flag;
    int session_int;
    int cmd_sock;
    int notice_sock;
	int is_encrypt;
	int is_send_audio;
    IPC_MID_UTIL_SOCK_LvSocket lv_sock;
    IPC_MID_UTIL_SOCK_TalkSocket talk_sock;
    IPC_MID_UTIL_SOCK_PbSocket pb_sock;
    int dl_status; //0: download stop, 1:download start, 2 pause
} IPC_MID_UTIL_SOCK_SessionGlobal;

typedef struct {
    pthread_mutex_t global_mutex;
    IPC_MID_UTIL_SOCK_SessionGlobal session_array[BZ_MAX_LINK_NUM];
    int online_session_num;
    int main_stream_num;
    int sub_stream_num;
    int main_is_set_key;
    int sub_is_set_key;
    int control_index;
} IPC_MID_UTIL_SOCK_Session;


typedef enum {
    BZ_MAIN_STREAM = 0,
    BZ_SUB_STREAM,
    BZ_UNKNOWN,
} BZ_CMS_STREAM_TYPE;


int IsSessionIdExist(int value, IPC_MID_UTIL_SOCK_Session *session);
int GenSessionInt(IPC_MID_UTIL_SOCK_Session *session);
int IPC_MID_UTIL_SOCK_SessionGlobalInit(IPC_MID_UTIL_SOCK_Session *session);
void IPC_MID_UTIL_SOCK_SessionGlobalUninit(IPC_MID_UTIL_SOCK_Session *session);

int IPC_MID_UTIL_SOCK_AddCmdSock(int socket, int session_id, IPC_MID_UTIL_SOCK_Session *session);
int IPC_MID_UTIL_SOCK_CloseCmdSock(int socket, IPC_MID_UTIL_SOCK_Session *session);
int IPC_MID_UTIL_SOCK_CloseAllSock(IPC_MID_UTIL_SOCK_Session *session);

int IPC_MID_UTIL_SOCK_AddNoticeSock(int socket, int session_id, IPC_MID_UTIL_SOCK_Session *session);
int IPC_MID_UTIL_SOCK_AddTalkSock(int socket, int session_id, IPC_MID_UTIL_SOCK_Session *session);
int IPC_MID_UTIL_SOCK_AddPbSock(int socket, int session_id, IPC_MID_UTIL_SOCK_Session *session);
int IPC_MID_UTIL_SOCK_GetNoticeSock(int session_id, IPC_MID_UTIL_SOCK_Session *session);
int IPC_MID_UTIL_SOCK_GetTalkSock(int session_id, IPC_MID_UTIL_SOCK_Session *session);
int IPC_MID_UTIL_SOCK_GetPbSock(int session_id, IPC_MID_UTIL_SOCK_Session *session);
int IPC_MID_UTIL_SOCK_CloseNoticeSock(int session_id, IPC_MID_UTIL_SOCK_Session *session);
int IPC_MID_UTIL_SOCK_CloseTalkSock(int session_id, IPC_MID_UTIL_SOCK_Session *session);
int IPC_MID_UTIL_SOCK_ClosePbSock(int session_id, IPC_MID_UTIL_SOCK_Session *session);

int IPC_MID_UTIL_SOCK_SetPbStatus(int session_id, int conntrol, IPC_MID_UTIL_SOCK_Session *session);
int IPC_MID_UTIL_SOCK_GetPbStatus(int session_id, IPC_MID_UTIL_SOCK_Session *session);

int IPC_MID_UTIL_SOCK_SetDlStatus(int session_id, int conntrol, IPC_MID_UTIL_SOCK_Session *session);
int IPC_MID_UTIL_SOCK_GetDlStatus(int session_id, IPC_MID_UTIL_SOCK_Session *session);

int IPC_MID_UTIL_SOCK_AddLvSock(int socket, int session_id, BZ_CMS_STREAM_TYPE type, IPC_MID_UTIL_SOCK_Session *session);
int IPC_MID_UTIL_SOCK_GetLvSocket(int session_id, BZ_CMS_STREAM_TYPE type, IPC_MID_UTIL_SOCK_Session *session);
int IPC_MID_UTIL_SOCK_CloseLvSock(int session_id, BZ_CMS_STREAM_TYPE type, IPC_MID_UTIL_SOCK_Session *session);

int IPC_MID_UTIL_SOCK_SetLvSockStatus(int session_id, int lv_sock_flag, BZ_CMS_STREAM_TYPE type, IPC_MID_UTIL_SOCK_Session *session);
int IPC_MID_UTIL_SOCK_GetLvSockStatus(int session_id, BZ_CMS_STREAM_TYPE type, IPC_MID_UTIL_SOCK_Session *session);

int IPC_MID_UTIL_SOCK_SetLvKeyFrameStatus(int is_set_key, int type, IPC_MID_UTIL_SOCK_Session *session);
int IPC_MID_UTIL_SOCK_GetLvKeyFrameStatus(int type, IPC_MID_UTIL_SOCK_Session *session);


/////
int IPC_MID_UTIL_SOCK_GetSessionIdByCmdSock(int cmd_sock, IPC_MID_UTIL_SOCK_Session *session);
int IPC_MID_UTIL_SOCK_GetSessionIdByIndex(int i, IPC_MID_UTIL_SOCK_Session *session);

int IPC_MID_UTIL_SOCK_GetLvSockByIndex(int i, int type, IPC_MID_UTIL_SOCK_Session *session);
int IPC_MID_UTIL_SOCK_CloseLvSockByIndex(int i, int type, IPC_MID_UTIL_SOCK_Session *session);

int IPC_MID_UTIL_SOCK_SetLvSockVNoByIndex(int i, int type, int vno, IPC_MID_UTIL_SOCK_Session *session);
int IPC_MID_UTIL_SOCK_GetLvSockVNoByIndex(int i, int type, IPC_MID_UTIL_SOCK_Session *session);


int IPC_MID_UTIL_SOCK_GetCmdSockByIndex(int i, IPC_MID_UTIL_SOCK_Session *session);
int IPC_MID_UTIL_SOCK_CloseCmdSockByIndex(int i, IPC_MID_UTIL_SOCK_Session *session);

int IPC_MID_UTIL_SOCK_CloseSessionSockByIndex(int i, IPC_MID_UTIL_SOCK_Session *session);

int IPC_MID_UTIL_SOCK_GetStreamOnlineNum(int stream_type, IPC_MID_UTIL_SOCK_Session *session);
int IPC_MID_UTIL_SOCK_GetStreamOpenNum(int stream_type, IPC_MID_UTIL_SOCK_Session *session);


int IPC_MID_UTIL_SOCK_IsControlSession(int sock, IPC_MID_UTIL_SOCK_Session *session);
int IPC_MID_UTIL_SOCK_GetNoticeSockByIndex(int i, IPC_MID_UTIL_SOCK_Session *session);


int IPC_MID_UTIL_SOCK_SetControlIndex(IPC_MID_UTIL_SOCK_Session *session);
int IPC_MID_UTIL_SOCK_GetControlIndex(IPC_MID_UTIL_SOCK_Session *session);


int IPC_MID_UTIL_SOCK_GetFlagBySid(int session_id, IPC_MID_UTIL_SOCK_Session *session);
int IPC_MID_UTIL_SOCK_CloseSessionId(int session_id, IPC_MID_UTIL_SOCK_Session *session);
int IPC_MID_UTIL_SOCK_AddSessionId(int session_id, IPC_MID_UTIL_SOCK_Session *session);
int IPC_MID_UTIL_SOCK_GetSessionNum(IPC_MID_UTIL_SOCK_Session *session);

int IPC_MID_UTIL_SOCK_GetEncryByIndex(int i, IPC_MID_UTIL_SOCK_Session *session);

int IPC_MID_UTIL_SOCK_GetAudioOpenNum(IPC_MID_UTIL_SOCK_Session *session);
int IPC_MID_UTIL_SOCK_SetAudioSendFlag(int session_id, int flag, IPC_MID_UTIL_SOCK_Session *session);

#ifdef __cplusplus
}
#endif

#endif

