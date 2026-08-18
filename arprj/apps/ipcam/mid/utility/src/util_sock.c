#include "util_common.h"
#include "util_sock.h"
#include "util_api.h"

//exist,return 1 . not exist, return 0
int IsSessionIdExist(int value, IPC_MID_UTIL_SOCK_Session *session)
{
    int i = 0;
    int ret = 0;
    pthread_mutex_lock(&(session->global_mutex));
    for (i = 0; i < BZ_MAX_LINK_NUM; i++) {
        if (session->session_array[i].session_int == value) {
            ret = 1;
            break;
        }
    }
    pthread_mutex_unlock(&(session->global_mutex));

	return ret;
}

int GenSessionInt(IPC_MID_UTIL_SOCK_Session *session)
{
	int value = 0;
    do {
        value = IPC_MID_UTIL_Rand(10000, 20000);
    } while (IsSessionIdExist(value, session) == 1);

	return value;
}

int IPC_MID_UTIL_SOCK_SessionGlobalInit(IPC_MID_UTIL_SOCK_Session *session)
{
    memset(session, 0, sizeof(IPC_MID_UTIL_SOCK_Session));
	pthread_mutex_init(&(session->global_mutex), NULL);
    session->online_session_num = 0;
    session->main_stream_num = 0;
    session->sub_stream_num = 0;
    session->main_is_set_key = 0;
    session->sub_is_set_key = 0;
    session->control_index = -1;

    int i;
	pthread_mutex_lock(&(session->global_mutex));
    for (i = 0; i < BZ_MAX_LINK_NUM; i++) {
        //memset(&session->session_array[i], 0, sizeof(IPC_MID_UTIL_SOCK_SessionGlobal));
		session->session_array[i].flag = 0;
		session->session_array[i].session_int = -1;
		session->session_array[i].cmd_sock = -1;
        session->session_array[i].notice_sock = -1;
		session->session_array[i].is_encrypt = 0;
		session->session_array[i].is_send_audio = 0;

		session->session_array[i].lv_sock.main_stream_socket.n_socket = -1;
		session->session_array[i].lv_sock.main_stream_socket.n_flag = 0;
        session->session_array[i].lv_sock.main_stream_socket.v_no = 0;
        session->session_array[i].lv_sock.main_stream_socket.is_wait_i = 0;

		session->session_array[i].lv_sock.sub_stream_socket.n_socket = -1;
		session->session_array[i].lv_sock.sub_stream_socket.n_flag = 0;
        session->session_array[i].lv_sock.sub_stream_socket.v_no = 0;
        session->session_array[i].lv_sock.sub_stream_socket.is_wait_i = 0;

		session->session_array[i].talk_sock.n_socket = -1;
        session->session_array[i].pb_sock.n_socket = -1;
    }
	pthread_mutex_unlock(&(session->global_mutex));

	return 0;
}

void IPC_MID_UTIL_SOCK_SessionGlobalUninit(IPC_MID_UTIL_SOCK_Session *session)
{
    pthread_mutex_destroy(&(session->global_mutex));
}

int IPC_MID_UTIL_SOCK_SetControlIndex(IPC_MID_UTIL_SOCK_Session *session)
{
    int i = 0;
    pthread_mutex_lock(&(session->global_mutex));
	//判断是否已经存在数组中
	//更新控制权给最早(i最小)的那个session
    for (i = 0; i < BZ_MAX_LINK_NUM; i++) {
        if ((session->session_array[i].flag == 1)  &&
            (session->session_array[i].cmd_sock > 0)) {
            session->control_index = i;
			break;
		}
    }
    if (i == BZ_MAX_LINK_NUM)
        session->control_index = -1;

    PRINT_INFO("control_index:%d\n", session->control_index);
    pthread_mutex_unlock(&(session->global_mutex));
    return 0;
}

int IPC_MID_UTIL_SOCK_GetControlIndex(IPC_MID_UTIL_SOCK_Session *session)
{
    int index = -1;
    pthread_mutex_lock(&(session->global_mutex));
    index = session->control_index;
    pthread_mutex_unlock(&(session->global_mutex));
    return index;
}


/* 添加成功返回0; 连接达到最大数，返回-1;  已经存在，返回1 */
int IPC_MID_UTIL_SOCK_AddCmdSock(int socket, int session_id, IPC_MID_UTIL_SOCK_Session *session)
{
	int i;
	int ret = -1; // 连接达到最大数，返回-1
	int is_exist = 0;

	pthread_mutex_lock(&(session->global_mutex));
	//判断是否已经存在数组中
    for (i = 0; i < BZ_MAX_LINK_NUM; i++) {
        if( (session->session_array[i].flag == 1)  &&
			(session->session_array[i].session_int == session_id) &&
			(session->session_array[i].cmd_sock == socket) ) {
			is_exist = 1; //已经存在
			ret = 1; //已经存在返回1
			break;
        }
    }

	//如果不存在，则添加进数组
	if (is_exist == 0) {
		ret = -1; // 连接达到最大数，返回-1
	    for (i = 0; i < BZ_MAX_LINK_NUM; i++) {
	        if (session->session_array[i].flag == 0) {
				session->session_array[i].session_int = session_id;
				session->session_array[i].cmd_sock = socket;
				session->session_array[i].flag = 1;
                session->online_session_num ++;

                /* 如果没有control_index，则设置成 control_index */
                PRINT_INFO("add cmd sock %d, online:%d, control_index:%d, add_cmd_index:%d\n",
                    socket, session->online_session_num, session->control_index, i);

                /* control_index 默认是 -1 */
                if (session->control_index < 0)
                    session->control_index = i;

				ret = 0; //添加成功返回0
				break;
	        }
	    }
	}

	pthread_mutex_unlock(&(session->global_mutex));
	return ret;
}

int IPC_MID_UTIL_SOCK_CloseCmdSock(int socket, IPC_MID_UTIL_SOCK_Session *session)
{
    int i;
    int ret = -1;
    pthread_mutex_lock(&(session->global_mutex));

    //判断是否已经存在数组中
    for (i = 0; i < BZ_MAX_LINK_NUM; i++) {
        if (session->session_array[i].cmd_sock == socket) {
			/* 关闭liveview 主码流socket */
			if (session->session_array[i].lv_sock.main_stream_socket.n_socket > 0) {
				close(session->session_array[i].lv_sock.main_stream_socket.n_socket);
                session->main_stream_num--;
				PRINT_INFO("close liveview main stream socket:%d\n", session->session_array[i].lv_sock.main_stream_socket.n_socket);
			}
			session->session_array[i].lv_sock.main_stream_socket.n_socket = -1;
			session->session_array[i].lv_sock.main_stream_socket.n_flag = 0;

			/* 关闭liveview 子码流socket */
			if (session->session_array[i].lv_sock.sub_stream_socket.n_socket > 0) {
				close(session->session_array[i].lv_sock.sub_stream_socket.n_socket);
                session->sub_stream_num--;
				PRINT_INFO("close liveview sub stream socket:%d\n", session->session_array[i].lv_sock.sub_stream_socket.n_socket);
			}
			session->session_array[i].lv_sock.sub_stream_socket.n_socket = -1;
			session->session_array[i].lv_sock.sub_stream_socket.n_flag = 0;

			/* 关闭session 主会话 socket */
			if (session->session_array[i].cmd_sock > 0) {
				close(session->session_array[i].cmd_sock);
				PRINT_INFO("close main socket:%d, index:%d\n", session->session_array[i].cmd_sock, i);
			}
			session->session_array[i].cmd_sock = -1;
			session->session_array[i].session_int = -1;
			session->session_array[i].flag = 0;

        	if (session->session_array[i].notice_sock > 0) {
				close(session->session_array[i].notice_sock);
				PRINT_INFO("close notice socket:%d\n", session->session_array[i].notice_sock);
        		session->session_array[i].notice_sock = -1;
        	}

        	if (session->session_array[i].talk_sock.n_socket > 0) {
				close(session->session_array[i].talk_sock.n_socket);
				PRINT_INFO("close talk socket:%d\n", session->session_array[i].talk_sock.n_socket);
        		session->session_array[i].talk_sock.n_socket = -1;
        	}

        	if (session->session_array[i].pb_sock.n_socket > 0) {
				close(session->session_array[i].pb_sock.n_socket);
				PRINT_INFO("close pb socket:%d\n", session->session_array[i].pb_sock.n_socket);
        		session->session_array[i].pb_sock.n_socket = -1;
        	}

            session->online_session_num --;
			ret = 0; //删除成功返回0
			break;
        }
    }

	pthread_mutex_unlock(&(session->global_mutex));

    IPC_MID_UTIL_SOCK_SetControlIndex(session);
	return ret;
}

int IPC_MID_UTIL_SOCK_CloseAllSock(IPC_MID_UTIL_SOCK_Session *session)
{
	int i;
	int ret = -1;
	pthread_mutex_lock(&(session->global_mutex));

	//判断是否已经存在数组中
    for (i = 0; i < BZ_MAX_LINK_NUM; i++) {
		/* 关闭liveview 主码流socket */
		if (session->session_array[i].lv_sock.main_stream_socket.n_socket > 0) {
			close(session->session_array[i].lv_sock.main_stream_socket.n_socket);
			PRINT_INFO("close liveview main stream socket:%d\n", session->session_array[i].lv_sock.main_stream_socket.n_socket);
            //session->main_stream_num--;
		}
		session->session_array[i].lv_sock.main_stream_socket.n_socket = -1;
		session->session_array[i].lv_sock.main_stream_socket.n_flag = 0;

		/* 关闭liveview 子码流socket */
		if (session->session_array[i].lv_sock.sub_stream_socket.n_socket > 0) {
			close(session->session_array[i].lv_sock.sub_stream_socket.n_socket);
			PRINT_INFO("close liveview sub stream socket:%d\n", session->session_array[i].lv_sock.sub_stream_socket.n_socket);
            //session->sub_stream_num--;
		}
		session->session_array[i].lv_sock.sub_stream_socket.n_socket = -1;
		session->session_array[i].lv_sock.sub_stream_socket.n_flag = 0;

		/* 关闭session 主会话 socket */
		if (session->session_array[i].cmd_sock > 0) {
			close(session->session_array[i].cmd_sock);
			PRINT_INFO("close main socket:%d \n", session->session_array[i].cmd_sock);
		}
		session->session_array[i].cmd_sock = -1;
		session->session_array[i].session_int = -1;
		session->session_array[i].flag = 0;

        if (session->session_array[i].notice_sock > 0) {
            close(session->session_array[i].notice_sock);
            PRINT_INFO("close notice socket:%d\n", session->session_array[i].notice_sock);
            session->session_array[i].notice_sock = -1;
        }

    	if (session->session_array[i].talk_sock.n_socket > 0) {
			close(session->session_array[i].talk_sock.n_socket);
			PRINT_INFO("close talk socket:%d\n", session->session_array[i].talk_sock.n_socket);
    		session->session_array[i].talk_sock.n_socket = -1;
    	}

    	if (session->session_array[i].pb_sock.n_socket > 0) {
			close(session->session_array[i].pb_sock.n_socket);
			PRINT_INFO("close pb socket:%d\n", session->session_array[i].pb_sock.n_socket);
    		session->session_array[i].pb_sock.n_socket = -1;
    	}
    }

    session->main_stream_num = 0;
    session->sub_stream_num = 0;
    session->online_session_num = 0;
    session->main_is_set_key = 0;
    session->sub_is_set_key = 0;
    session->control_index = -1;
	pthread_mutex_unlock(&(session->global_mutex));
	return ret;
}


/* 没有对应sessionID返回-1; 设置成功返回0 */
int IPC_MID_UTIL_SOCK_AddNoticeSock(int socket, int session_id, IPC_MID_UTIL_SOCK_Session *session)
{
	int i;
	int ret = -1;

	pthread_mutex_lock(&(session->global_mutex));
	//覆盖添加
    for (i = 0; i < BZ_MAX_LINK_NUM; i++) {
        if (session->session_array[i].session_int == session_id) {
        	session->session_array[i].notice_sock = socket;
			ret = 0;
            PRINT_INFO("session_int:%d, cmd sock :%d add notice sock:%d \n",
                session->session_array[i].session_int,
                session->session_array[i].cmd_sock,
                session->session_array[i].notice_sock);

			break;
        }
    }
	pthread_mutex_unlock(&(session->global_mutex));
	return ret;
}


/* 没有对应sessionID返回-1; 设置成功返回0 */
int IPC_MID_UTIL_SOCK_AddTalkSock(int socket, int session_id, IPC_MID_UTIL_SOCK_Session *session)
{
	int i;
	int ret = -1;

	pthread_mutex_lock(&(session->global_mutex));
	//覆盖添加
    for (i = 0; i < BZ_MAX_LINK_NUM; i++) {
        if (session->session_array[i].session_int == session_id) {
        	session->session_array[i].talk_sock.n_socket = socket;
			ret = 0;
            PRINT_INFO("session_int:%d, cmd sock :%d, talk sock:%d \n",
                session->session_array[i].session_int,
                session->session_array[i].cmd_sock,
                session->session_array[i].talk_sock.n_socket);

			break;
        }
    }
	pthread_mutex_unlock(&(session->global_mutex));
	return ret;
}

/* 没有对应sessionID返回-1; 设置成功返回0 */
int IPC_MID_UTIL_SOCK_AddPbSock(int socket, int session_id, IPC_MID_UTIL_SOCK_Session *session)
{
	int i;
	int ret = -1;

	pthread_mutex_lock(&(session->global_mutex));
	//覆盖添加
    for (i = 0; i < BZ_MAX_LINK_NUM; i++) {
        if (session->session_array[i].session_int == session_id) {
        	session->session_array[i].pb_sock.n_socket = socket;
			ret = 0;
            PRINT_INFO("session_int:%d, cmd sock :%d, pb sock:%d \n",
                session->session_array[i].session_int,
                session->session_array[i].cmd_sock,
                session->session_array[i].pb_sock.n_socket);

            break;
        }
    }
	pthread_mutex_unlock(&(session->global_mutex));
	return ret;
}

int IPC_MID_UTIL_SOCK_GetNoticeSock(int session_id, IPC_MID_UTIL_SOCK_Session *session)
{
	int i;
	int sock = -1;

	pthread_mutex_lock(&(session->global_mutex));
	//判断是否已经存在数组中
    for (i = 0; i < BZ_MAX_LINK_NUM; i++) {
        if (session->session_array[i].session_int == session_id) {
			sock = session->session_array[i].notice_sock;
			break;
		}
    }
	pthread_mutex_unlock(&(session->global_mutex));
	return sock;
}

int IPC_MID_UTIL_SOCK_GetTalkSock(int session_id, IPC_MID_UTIL_SOCK_Session *session)
{
	int i;
	int sock = -1;

	pthread_mutex_lock(&(session->global_mutex));
	//判断是否已经存在数组中
    for (i = 0; i < BZ_MAX_LINK_NUM; i++) {
        if (session->session_array[i].session_int == session_id) {
			sock = session->session_array[i].talk_sock.n_socket;
			break;
		}
    }
	pthread_mutex_unlock(&(session->global_mutex));
	return sock;
}

int IPC_MID_UTIL_SOCK_GetPbSock(int session_id, IPC_MID_UTIL_SOCK_Session *session)
{
	int i;
	int sock = -1;

	pthread_mutex_lock(&(session->global_mutex));
	//判断是否已经存在数组中
    for (i = 0; i < BZ_MAX_LINK_NUM; i++) {
        if (session->session_array[i].session_int == session_id) {
			sock = session->session_array[i].pb_sock.n_socket;
			break;
		}
    }
	pthread_mutex_unlock(&(session->global_mutex));
	return sock;
}

int IPC_MID_UTIL_SOCK_CloseNoticeSock(int session_id, IPC_MID_UTIL_SOCK_Session *session)
{
	int i;
	int ret = -1;

	pthread_mutex_lock(&(session->global_mutex));
	//判断是否已经存在数组中
    for (i = 0; i < BZ_MAX_LINK_NUM; i++) {
        if (session->session_array[i].session_int == session_id) {
        	if (session->session_array[i].notice_sock > 0) {
				close(session->session_array[i].notice_sock);
				PRINT_INFO("close notice socket:%d\n", session->session_array[i].notice_sock);
        		session->session_array[i].notice_sock = -1;
        	}

			ret = 0; //删除成功返回0
			break;
		}
    }
	pthread_mutex_unlock(&(session->global_mutex));
	return ret;
}


int IPC_MID_UTIL_SOCK_CloseTalkSock(int session_id, IPC_MID_UTIL_SOCK_Session *session)
{
	int i;
	int ret = -1;

	pthread_mutex_lock(&(session->global_mutex));
	//判断是否已经存在数组中
    for (i = 0; i < BZ_MAX_LINK_NUM; i++) {
        if (session->session_array[i].session_int == session_id) {
        	if (session->session_array[i].talk_sock.n_socket > 0) {
				close(session->session_array[i].talk_sock.n_socket);
				PRINT_INFO("close talk socket:%d\n", session->session_array[i].talk_sock.n_socket);

        		session->session_array[i].talk_sock.n_socket = -1;
        	}

			ret = 0; //删除成功返回0
			break;
		}
    }
	pthread_mutex_unlock(&(session->global_mutex));
	return ret;
}

int IPC_MID_UTIL_SOCK_ClosePbSock(int session_id, IPC_MID_UTIL_SOCK_Session *session)
{
	int i;
	int ret = -1;

	pthread_mutex_lock(&(session->global_mutex));
	//判断是否已经存在数组中
    for (i = 0; i < BZ_MAX_LINK_NUM; i++) {
        if (session->session_array[i].session_int == session_id) {
        	if (session->session_array[i].pb_sock.n_socket > 0) {
				close(session->session_array[i].pb_sock.n_socket);
				PRINT_INFO("close pb socket:%d\n", session->session_array[i].pb_sock.n_socket);

        		session->session_array[i].pb_sock.n_socket = -1;
        	}

			ret = 0; //删除成功返回0
			break;
		}
    }
	pthread_mutex_unlock(&(session->global_mutex));
	return ret;
}

int IPC_MID_UTIL_SOCK_SetPbStatus(int session_id, int conntrol, IPC_MID_UTIL_SOCK_Session *session)
{
	int i;
	int ret = -1;

	pthread_mutex_lock(&(session->global_mutex));
	//判断是否已经存在数组中
    for (i = 0; i < BZ_MAX_LINK_NUM; i++) {
        if (session->session_array[i].session_int == session_id) {
            session->session_array[i].pb_sock.control = conntrol;
			ret = 0;
			break;
		}
    }
	pthread_mutex_unlock(&(session->global_mutex));
	return ret;
}

int IPC_MID_UTIL_SOCK_GetPbStatus(int session_id, IPC_MID_UTIL_SOCK_Session *session)
{
	int i;
	int conntrol = -1;

	pthread_mutex_lock(&(session->global_mutex));
	//判断是否已经存在数组中
    for (i = 0; i < BZ_MAX_LINK_NUM; i++) {
        if (session->session_array[i].session_int == session_id) {
			conntrol = session->session_array[i].pb_sock.control;
			break;
		}
    }
	pthread_mutex_unlock(&(session->global_mutex));
	return conntrol;
}

int IPC_MID_UTIL_SOCK_SetDlStatus(int session_id, int conntrol, IPC_MID_UTIL_SOCK_Session *session)
{
	int i;
	int ret = -1;

	pthread_mutex_lock(&(session->global_mutex));
	//判断是否已经存在数组中
    for (i = 0; i < BZ_MAX_LINK_NUM; i++) {
        if (session->session_array[i].session_int == session_id) {
            session->session_array[i].dl_status = conntrol;
			ret = 0;
			break;
		}
    }
	pthread_mutex_unlock(&(session->global_mutex));
	return ret;
}

int IPC_MID_UTIL_SOCK_GetDlStatus(int session_id, IPC_MID_UTIL_SOCK_Session *session)
{
	int i;
	int conntrol = -1;

	pthread_mutex_lock(&(session->global_mutex));
	//判断是否已经存在数组中
    for (i = 0; i < BZ_MAX_LINK_NUM; i++) {
        if (session->session_array[i].session_int == session_id) {
			conntrol = session->session_array[i].dl_status;
			break;
		}
    }
	pthread_mutex_unlock(&(session->global_mutex));
	return conntrol;
}

/* 没有对应sessionID返回-1; 设置成功返回0 */
int IPC_MID_UTIL_SOCK_AddLvSock(int socket, int session_id, BZ_CMS_STREAM_TYPE type, IPC_MID_UTIL_SOCK_Session *session)
{
	int i;
	int ret = -1;

	pthread_mutex_lock(&(session->global_mutex));
	//判断是否已经存在数组中
    for (i = 0; i < BZ_MAX_LINK_NUM; i++) {
        if(session->session_array[i].session_int == session_id) {
			if (type == BZ_MAIN_STREAM) {
				session->session_array[i].lv_sock.main_stream_socket.n_socket = socket;
				session->session_array[i].lv_sock.main_stream_socket.n_flag = 0; //默认先关闭，0关闭1打开
	//			session->session_array[i].lv_sock.main_stream_socket.n_flag = 1; // by bruce
                session->main_stream_num++;
                PRINT_ERR("cms sock %d add lv main sock %d,sessionNum:%d\n",
                    session->session_array[i].cmd_sock,
                    socket,
                    session->main_stream_num);
			} else if(type == BZ_SUB_STREAM) {
				session->session_array[i].lv_sock.sub_stream_socket.n_socket = socket;
				session->session_array[i].lv_sock.sub_stream_socket.n_flag = 0; //默认先关闭，0关闭1打开
	//			session->session_array[i].lv_sock.sub_stream_socket.n_flag = 1; // by bruce
                session->sub_stream_num++;
                PRINT_ERR("cmd sock %d add lv sub sock %d, sub stream num:%d\n",
                    session->session_array[i].cmd_sock,
                    socket,
                    session->sub_stream_num);
			}

			ret = 0; //添加成功返回0
			break;
        }
    }
	pthread_mutex_unlock(&(session->global_mutex));
	return ret;
}

int IPC_MID_UTIL_SOCK_GetLvSocket(int session_id, BZ_CMS_STREAM_TYPE type, IPC_MID_UTIL_SOCK_Session *session)
{
	int i;
	int sock = -1;

	pthread_mutex_lock(&(session->global_mutex));
	//判断是否已经存在数组中
    for (i = 0; i < BZ_MAX_LINK_NUM; i++) {
        if (session->session_array[i].session_int == session_id) {
			if (type == BZ_MAIN_STREAM) {
				sock = session->session_array[i].lv_sock.main_stream_socket.n_socket;
			} else if(type == BZ_SUB_STREAM) {
				sock = session->session_array[i].lv_sock.sub_stream_socket.n_socket;
			}

			break;
		}
    }
	pthread_mutex_unlock(&(session->global_mutex));
	return sock;
}


int IPC_MID_UTIL_SOCK_CloseLvSock(int session_id, BZ_CMS_STREAM_TYPE type, IPC_MID_UTIL_SOCK_Session *session)
{
	int i;
	int ret = -1;

	pthread_mutex_lock(&(session->global_mutex));
	//判断是否已经存在数组中
    for (i = 0; i < BZ_MAX_LINK_NUM; i++) {
        if (session->session_array[i].session_int == session_id) {
			if (type == BZ_MAIN_STREAM) {
				if (session->session_array[i].lv_sock.main_stream_socket.n_socket > 0) {
					close(session->session_array[i].lv_sock.main_stream_socket.n_socket);
                    session->main_stream_num--;
					PRINT_INFO("cmd %d close liveview main stream socket:%d\n",
                        session->session_array[i].cmd_sock,
                        session->session_array[i].lv_sock.main_stream_socket.n_socket);
				}

				session->session_array[i].lv_sock.main_stream_socket.n_socket = -1;
				session->session_array[i].lv_sock.main_stream_socket.n_flag = 0;
			} else if(type == BZ_SUB_STREAM) {
				if (session->session_array[i].lv_sock.sub_stream_socket.n_socket > 0) {
					close(session->session_array[i].lv_sock.sub_stream_socket.n_socket);
					session->sub_stream_num--;
					PRINT_INFO("cmd %d close liveview sub stream socket:%d\n",
                        session->session_array[i].cmd_sock,
                        session->session_array[i].lv_sock.sub_stream_socket.n_socket);
                }

				session->session_array[i].lv_sock.sub_stream_socket.n_socket = -1;
				session->session_array[i].lv_sock.sub_stream_socket.n_flag = 0;
			}

			ret = 0; //删除成功返回0
			break;
		}
    }
	pthread_mutex_unlock(&(session->global_mutex));
	return ret;
}

int IPC_MID_UTIL_SOCK_SetLvSockStatus(int session_id, int lv_sock_flag, BZ_CMS_STREAM_TYPE type, IPC_MID_UTIL_SOCK_Session *session)
{
	int i;
	int ret = -1;

	pthread_mutex_lock(&(session->global_mutex));
	//判断是否已经存在数组中
    for (i = 0; i < BZ_MAX_LINK_NUM; i++) {
        if (session->session_array[i].session_int == session_id) {
			if (type == BZ_MAIN_STREAM) {
				session->session_array[i].lv_sock.main_stream_socket.n_flag = lv_sock_flag;
			} else if(type == BZ_SUB_STREAM) {
				session->session_array[i].lv_sock.sub_stream_socket.n_flag = lv_sock_flag;
			}

			ret = 0;
			break;
		}
    }
	pthread_mutex_unlock(&(session->global_mutex));
	return ret;
}


int IPC_MID_UTIL_SOCK_GetLvSockStatus(int session_id, BZ_CMS_STREAM_TYPE type, IPC_MID_UTIL_SOCK_Session *session)
{
	int i;
	int status = -1;

	pthread_mutex_lock(&(session->global_mutex));
	//判断是否已经存在数组中
    for (i = 0; i < BZ_MAX_LINK_NUM; i++) {
        if (session->session_array[i].session_int == session_id) {
			if (type == BZ_MAIN_STREAM) {
				status = session->session_array[i].lv_sock.main_stream_socket.n_flag;
			} else if(type == BZ_SUB_STREAM) {
				status = session->session_array[i].lv_sock.sub_stream_socket.n_flag;
			}

			break;
		}
    }
	pthread_mutex_unlock(&(session->global_mutex));
	return status;
}

int IPC_MID_UTIL_SOCK_SetLvWaitKeyFrame(int session_id, BZ_CMS_STREAM_TYPE type, int is_wait_i, IPC_MID_UTIL_SOCK_Session *session)
{
	int i;
	int ret = -1;

	pthread_mutex_lock(&(session->global_mutex));
	//判断是否已经存在数组中
    for (i = 0; i < BZ_MAX_LINK_NUM; i++) {
        if (session->session_array[i].session_int == session_id) {
			if (type == BZ_MAIN_STREAM) {
				session->session_array[i].lv_sock.main_stream_socket.is_wait_i = is_wait_i;
			} else if(type == BZ_SUB_STREAM) {
				session->session_array[i].lv_sock.sub_stream_socket.is_wait_i = is_wait_i;
			}

			ret = 0;
			break;
		}
    }
	pthread_mutex_unlock(&(session->global_mutex));
	return ret;
}

int IPC_MID_UTIL_SOCK_GetLvWaitKeyFrame(int session_id, BZ_CMS_STREAM_TYPE type, IPC_MID_UTIL_SOCK_Session *session)
{
	int i;
	int is_wait_i = -1;

	pthread_mutex_lock(&(session->global_mutex));
	//判断是否已经存在数组中
    for (i = 0; i < BZ_MAX_LINK_NUM; i++) {
        if (session->session_array[i].session_int == session_id) {
			if (type == BZ_MAIN_STREAM) {
				is_wait_i = session->session_array[i].lv_sock.main_stream_socket.is_wait_i;
			} else if(type == BZ_SUB_STREAM) {
				is_wait_i = session->session_array[i].lv_sock.sub_stream_socket.is_wait_i;
			}

			break;
		}
    }
	pthread_mutex_unlock(&(session->global_mutex));
	return is_wait_i;
}


int IPC_MID_UTIL_SOCK_SetLvKeyFrameStatus(int is_set_key, int type, IPC_MID_UTIL_SOCK_Session *session)
{
	pthread_mutex_lock(&(session->global_mutex));
    if (type == BZ_MAIN_STREAM) {
        session->main_is_set_key = is_set_key;
    } else if(type == BZ_SUB_STREAM) {
        session->sub_is_set_key = is_set_key;
    }
	pthread_mutex_unlock(&(session->global_mutex));

	return 0;
}

int IPC_MID_UTIL_SOCK_GetLvKeyFrameStatus(int type, IPC_MID_UTIL_SOCK_Session *session)
{
	int is_set_key = 0;

	pthread_mutex_lock(&(session->global_mutex));
    if (type == BZ_MAIN_STREAM) {
        is_set_key = session->main_is_set_key;
    } else if(type == BZ_SUB_STREAM) {
        is_set_key = session->sub_is_set_key;
    }
	pthread_mutex_unlock(&(session->global_mutex));

	return is_set_key;
}



///////////////////////////////////

int IPC_MID_UTIL_SOCK_GetSessionIdByCmdSock(int cmd_sock, IPC_MID_UTIL_SOCK_Session *session)
{
	int i;
	int session_id = -1;

	pthread_mutex_lock(&(session->global_mutex));
	//判断是否已经存在数组中
    for (i = 0; i < BZ_MAX_LINK_NUM; i++) {
        if (session->session_array[i].cmd_sock == cmd_sock) {
			session_id = session->session_array[i].session_int;
			break;
		}
    }
	pthread_mutex_unlock(&(session->global_mutex));
	return session_id;
}

int IPC_MID_UTIL_SOCK_GetSessionIdByIndex(int i, IPC_MID_UTIL_SOCK_Session *session)
{
    int sid = -1;

    if (i < 0 || i >= BZ_MAX_LINK_NUM) {
        PRINT_ERR("index error. index:%d max_link_num:%d\n", i, BZ_MAX_LINK_NUM);
        return -1;
    }

	pthread_mutex_lock(&(session->global_mutex));
    if (session->session_array[i].flag == 1) {
		sid = session->session_array[i].session_int;
	}
	pthread_mutex_unlock(&(session->global_mutex));

    return sid;
}

int IPC_MID_UTIL_SOCK_GetSessionIdByIndex2(int i, int type, IPC_MID_UTIL_SOCK_Session *session)
{
    int sid = -1;

    if (i < 0 || i >= BZ_MAX_LINK_NUM) {
        PRINT_ERR("index error. index:%d max_link_num:%d\n", i, BZ_MAX_LINK_NUM);
        return -1;
    }

	pthread_mutex_lock(&(session->global_mutex));
    if (session->session_array[i].flag == 1) {

        if (type == BZ_MAIN_STREAM) {
            if (session->session_array[i].lv_sock.main_stream_socket.n_flag) {
                sid = session->session_array[i].session_int;
            }
        } else if(type == BZ_SUB_STREAM) {
            if (session->session_array[i].lv_sock.sub_stream_socket.n_flag) {
                sid = session->session_array[i].session_int;
            }
        }
	}
	pthread_mutex_unlock(&(session->global_mutex));

    return sid;
}

int IPC_MID_UTIL_SOCK_GetEncryByIndex(int i, IPC_MID_UTIL_SOCK_Session *session)
{
    if (i < 0 || i >= BZ_MAX_LINK_NUM) {
        PRINT_ERR("index error. index:%d max_link_num:%d\n", i, BZ_MAX_LINK_NUM);
        return -1;
    }

	int is_encrypt = -1;
	pthread_mutex_lock(&(session->global_mutex));
	is_encrypt = session->session_array[i].is_encrypt;
	pthread_mutex_unlock(&(session->global_mutex));

    return is_encrypt;
}

int IPC_MID_UTIL_SOCK_GetLvSockByIndex(int i, int type, IPC_MID_UTIL_SOCK_Session *session)
{
    int sock = -1;

    if (i < 0 || i >= BZ_MAX_LINK_NUM) {
        PRINT_ERR("index error. index:%d max_link_num:%d\n", i, BZ_MAX_LINK_NUM);
        return -1;
    }

	pthread_mutex_lock(&(session->global_mutex));
    if (session->session_array[i].flag == 1) {
        if (type == BZ_MAIN_STREAM) {
            if (session->session_array[i].lv_sock.main_stream_socket.n_socket > 0) {
                sock = session->session_array[i].lv_sock.main_stream_socket.n_socket;
            }
        } else if (type == BZ_SUB_STREAM) {
            if (session->session_array[i].lv_sock.sub_stream_socket.n_socket > 0) {
                sock = session->session_array[i].lv_sock.sub_stream_socket.n_socket;
            }
        }
	}
	pthread_mutex_unlock(&(session->global_mutex));

    return sock;
}


int IPC_MID_UTIL_SOCK_SetLvSockVNoByIndex(int i, int type, int vno, IPC_MID_UTIL_SOCK_Session *session)
{
    if (i < 0 || i >= BZ_MAX_LINK_NUM) {
        PRINT_ERR("index error. index:%d max_link_num:%d\n", i, BZ_MAX_LINK_NUM);
        return -1;
    }

	pthread_mutex_lock(&(session->global_mutex));
    if (session->session_array[i].flag == 1) {
        if (type == BZ_MAIN_STREAM) {
            //if (session->session_array[i].lv_sock.main_stream_socket.n_socket > 0) {
                session->session_array[i].lv_sock.main_stream_socket.v_no = vno;
            //}
        } else if (type == BZ_SUB_STREAM) {
            //if (session->session_array[i].lv_sock.sub_stream_socket.n_socket > 0) {
                session->session_array[i].lv_sock.sub_stream_socket.v_no = vno;
            //}
        }
	}
	pthread_mutex_unlock(&(session->global_mutex));

    return 0;
}


int IPC_MID_UTIL_SOCK_GetLvSockVNoByIndex(int i, int type, IPC_MID_UTIL_SOCK_Session *session)
{
    int vno = -1;

    if (i < 0 || i >= BZ_MAX_LINK_NUM) {
        PRINT_ERR("index error. index:%d max_link_num:%d\n", i, BZ_MAX_LINK_NUM);
        return -1;
    }

	pthread_mutex_lock(&(session->global_mutex));
    if (session->session_array[i].flag == 1) {
        if (type == BZ_MAIN_STREAM) {
            //if (session->session_array[i].lv_sock.main_stream_socket.n_socket > 0) {
                vno = session->session_array[i].lv_sock.main_stream_socket.v_no;
            //}
        } else if (type == BZ_SUB_STREAM) {
            //if (session->session_array[i].lv_sock.sub_stream_socket.n_socket > 0) {
                vno = session->session_array[i].lv_sock.sub_stream_socket.v_no;
            //}
        }
	}
	pthread_mutex_unlock(&(session->global_mutex));

    return vno;
}


int IPC_MID_UTIL_SOCK_CloseLvSockByIndex(int i, int type, IPC_MID_UTIL_SOCK_Session *session)
{
    if (i < 0 || i >= BZ_MAX_LINK_NUM) {
        PRINT_ERR("index error. index:%d max_link_num:%d\n", i, BZ_MAX_LINK_NUM);
        return -1;
    }

	pthread_mutex_lock(&(session->global_mutex));
    if (session->session_array[i].flag == 1) {
		if (type == BZ_MAIN_STREAM) {
			if (session->session_array[i].lv_sock.main_stream_socket.n_socket > 0) {
				close(session->session_array[i].lv_sock.main_stream_socket.n_socket);
				PRINT_INFO("close liveview main stream socket:%d\n", session->session_array[i].lv_sock.main_stream_socket.n_socket);
                session->main_stream_num--;
			}
			session->session_array[i].lv_sock.main_stream_socket.n_socket = -1;
			session->session_array[i].lv_sock.main_stream_socket.n_flag = 0;
		} else if(type == BZ_SUB_STREAM) {
			if (session->session_array[i].lv_sock.sub_stream_socket.n_socket > 0) {
				close(session->session_array[i].lv_sock.sub_stream_socket.n_socket);
				PRINT_INFO("close liveview sub stream socket:%d\n", session->session_array[i].lv_sock.sub_stream_socket.n_socket);
                session->sub_stream_num--;
            }
			session->session_array[i].lv_sock.sub_stream_socket.n_socket = -1;
			session->session_array[i].lv_sock.sub_stream_socket.n_flag = 0;
		}
	}
	pthread_mutex_unlock(&(session->global_mutex));

    return 0;
}


int IPC_MID_UTIL_SOCK_GetCmdSockByIndex(int i, IPC_MID_UTIL_SOCK_Session *session)
{
    if (i < 0 || i >= BZ_MAX_LINK_NUM) {
        PRINT_ERR("index error. index:%d max_link_num:%d\n", i, BZ_MAX_LINK_NUM);
        return -1;
    }

    int sock = -1;

	pthread_mutex_lock(&(session->global_mutex));
    if (session->session_array[i].flag == 1) {
        if (session->session_array[i].cmd_sock > 0) {
            sock = session->session_array[i].cmd_sock;
        }
	}
	pthread_mutex_unlock(&(session->global_mutex));

    return sock;
}


int IPC_MID_UTIL_SOCK_CloseCmdSockByIndex(int i, IPC_MID_UTIL_SOCK_Session *session)
{
    if (i < 0 || i >= BZ_MAX_LINK_NUM) {
        PRINT_ERR("index error. index:%d max_link_num:%d\n", i, BZ_MAX_LINK_NUM);
        return -1;
    }

	pthread_mutex_lock(&(session->global_mutex));

    /* 关闭liveview 主码流socket */
    if (session->session_array[i].lv_sock.main_stream_socket.n_socket > 0) {
    	close(session->session_array[i].lv_sock.main_stream_socket.n_socket);
    	PRINT_INFO("close liveview main stream socket:%d\n", session->session_array[i].lv_sock.main_stream_socket.n_socket);
        session->main_stream_num--;
    }
    session->session_array[i].lv_sock.main_stream_socket.n_socket = -1;
    session->session_array[i].lv_sock.main_stream_socket.n_flag = 0;

    /* 关闭liveview 子码流socket */
    if (session->session_array[i].lv_sock.sub_stream_socket.n_socket > 0) {
    	close(session->session_array[i].lv_sock.sub_stream_socket.n_socket);
    	PRINT_INFO("close liveview sub stream socket:%d\n", session->session_array[i].lv_sock.sub_stream_socket.n_socket);
        session->sub_stream_num--;
    }
    session->session_array[i].lv_sock.sub_stream_socket.n_socket = -1;
    session->session_array[i].lv_sock.sub_stream_socket.n_flag = 0;

    /* 关闭session 主会话 socket */
    if (session->session_array[i].cmd_sock > 0) {
    	close(session->session_array[i].cmd_sock);
    	PRINT_INFO("close main socket:%d \n", session->session_array[i].cmd_sock);
    }
    session->session_array[i].cmd_sock = -1;
    session->session_array[i].session_int = -1;
    session->session_array[i].flag = 0;

    if (session->session_array[i].notice_sock > 0) {
        close(session->session_array[i].notice_sock);
        PRINT_INFO("close notice socket:%d\n", session->session_array[i].notice_sock);
        session->session_array[i].notice_sock = -1;
    }

    if (session->session_array[i].talk_sock.n_socket > 0) {
    	close(session->session_array[i].talk_sock.n_socket);
    	PRINT_INFO("close talk socket:%d\n", session->session_array[i].talk_sock.n_socket);
    	session->session_array[i].talk_sock.n_socket = -1;
    }

    if (session->session_array[i].pb_sock.n_socket > 0) {
    	close(session->session_array[i].pb_sock.n_socket);
    	PRINT_INFO("close talk socket:%d\n", session->session_array[i].pb_sock.n_socket);
    	session->session_array[i].pb_sock.n_socket = -1;
    }

    session->online_session_num --;
	pthread_mutex_unlock(&(session->global_mutex));

    IPC_MID_UTIL_SOCK_SetControlIndex(session);

    return 0;
}

int IPC_MID_UTIL_SOCK_CloseSessionSockByIndex(int i, IPC_MID_UTIL_SOCK_Session *session)
{
    if (i < 0 || i >= BZ_MAX_LINK_NUM) {
        PRINT_ERR("index error. index:%d max_link_num:%d\n", i, BZ_MAX_LINK_NUM);
        return -1;
    }

	pthread_mutex_lock(&(session->global_mutex));

	/* 关闭liveview 主码流socket */
	if (session->session_array[i].lv_sock.main_stream_socket.n_socket > 0) {
		close(session->session_array[i].lv_sock.main_stream_socket.n_socket);
		PRINT_INFO("close liveview main stream socket:%d\n", session->session_array[i].lv_sock.main_stream_socket.n_socket);
        session->main_stream_num--;
	}
	session->session_array[i].lv_sock.main_stream_socket.n_socket = -1;
	session->session_array[i].lv_sock.main_stream_socket.n_flag = 0;

	/* 关闭liveview 子码流socket */
	if (session->session_array[i].lv_sock.sub_stream_socket.n_socket > 0) {
		close(session->session_array[i].lv_sock.sub_stream_socket.n_socket);
		PRINT_INFO("close liveview sub stream socket:%d\n", session->session_array[i].lv_sock.sub_stream_socket.n_socket);
        session->sub_stream_num--;
	}
	session->session_array[i].lv_sock.sub_stream_socket.n_socket = -1;
	session->session_array[i].lv_sock.sub_stream_socket.n_flag = 0;

	/* 关闭session cmd socket */
	if (session->session_array[i].cmd_sock > 0) {
		close(session->session_array[i].cmd_sock);
		PRINT_INFO("close main socket:%d \n", session->session_array[i].cmd_sock);
	}
	session->session_array[i].cmd_sock = -1;
	session->session_array[i].session_int = -1;
	session->session_array[i].flag = 0;

    if (session->session_array[i].notice_sock > 0) {
        close(session->session_array[i].notice_sock);
        PRINT_INFO("close notice socket:%d\n", session->session_array[i].notice_sock);
        session->session_array[i].notice_sock = -1;
    }

    /* 关闭session talk socket */
	if (session->session_array[i].talk_sock.n_socket > 0) {
		close(session->session_array[i].talk_sock.n_socket);
		PRINT_INFO("close talk socket:%d\n", session->session_array[i].talk_sock.n_socket);
		session->session_array[i].talk_sock.n_socket = -1;
	}

    /* 关闭session playback socket */
	if (session->session_array[i].pb_sock.n_socket > 0) {
		close(session->session_array[i].pb_sock.n_socket);
		PRINT_INFO("close pb socket:%d\n", session->session_array[i].pb_sock.n_socket);
		session->session_array[i].pb_sock.n_socket = -1;
	}

    session->online_session_num --;
	pthread_mutex_unlock(&(session->global_mutex));

    IPC_MID_UTIL_SOCK_SetControlIndex(session);

    return 0;
}

int IPC_MID_UTIL_SOCK_GetFlagByIndex(int i, IPC_MID_UTIL_SOCK_Session *session)
{
    if (i < 0 || i >= BZ_MAX_LINK_NUM) {
        PRINT_ERR("index error. index:%d max_link_num:%d\n", i, BZ_MAX_LINK_NUM);
        return -1;
    }

	int flag = -1;
	pthread_mutex_lock(&(session->global_mutex));
	flag = session->session_array[i].flag;
	pthread_mutex_unlock(&(session->global_mutex));

    return flag;
}

/* flag:0, 1。 如果sid不存在，flag:-1 */
int IPC_MID_UTIL_SOCK_GetFlagBySid(int session_id, IPC_MID_UTIL_SOCK_Session *session)
{
	int i;
	int flag = -1;
	pthread_mutex_lock(&(session->global_mutex));
    for (i = 0; i < BZ_MAX_LINK_NUM; i++) {
        if (session->session_array[i].session_int == session_id) {
			flag = session->session_array[i].flag;
			break;
		}
    }
	pthread_mutex_unlock(&(session->global_mutex));

    return flag;
}

int IPC_MID_UTIL_SOCK_CloseSessionIdByIndex(int i, IPC_MID_UTIL_SOCK_Session *session)
{
	int j;

    if (i < 0 || i >= BZ_MAX_LINK_NUM) {
        PRINT_ERR("index error. index:%d max_link_num:%d\n", i, BZ_MAX_LINK_NUM);
        return -1;
    }

	pthread_mutex_lock(&(session->global_mutex));
	session->session_array[i].session_int = -1;
	session->session_array[i].flag = 0;
	session->online_session_num --;

	//本身拥有控制权，才需要更新控制权
	if (session->control_index == i) {
		//更新控制权给最早(i最小)的那个session
	    for (j = 0; j < BZ_MAX_LINK_NUM; j ++) {
	        if (session->session_array[j].flag == 1) {
	            session->control_index = j;
				break;
			}
	    }
	    if (j == BZ_MAX_LINK_NUM)
	        session->control_index = -1;
	}

	pthread_mutex_unlock(&(session->global_mutex));

    return 0;
}

int IPC_MID_UTIL_SOCK_CloseSessionId(int session_id, IPC_MID_UTIL_SOCK_Session *session)
{
	int i, j;
	int ret = -1;

	pthread_mutex_lock(&(session->global_mutex));
	//判断是否已经存在数组中
    for (i = 0; i < BZ_MAX_LINK_NUM; i ++) {
        if (session->session_array[i].session_int == session_id) {
            session->online_session_num --;

            memset(&(session->session_array[i]), 0, sizeof(IPC_MID_UTIL_SOCK_SessionGlobal));
            session->session_array[i].flag = 0;
            session->session_array[i].session_int = -1;
            session->session_array[i].cmd_sock = -1;
            session->session_array[i].notice_sock = -1;
            session->session_array[i].lv_sock.main_stream_socket.n_socket = -1;
            session->session_array[i].lv_sock.sub_stream_socket.n_socket = -1;
            session->session_array[i].talk_sock.n_socket = -1;
            session->session_array[i].pb_sock.n_socket = -1;

			//如果本身拥有控制权，才需要更新控制权
			if (session->control_index == i) {
				//更新控制权给最早(i最小)的那个session
			    for (j = 0; j < BZ_MAX_LINK_NUM; j ++) {
			        if (session->session_array[j].flag == 1) {
			            session->control_index = j;
						break;
					}
			    }
			    if (j == BZ_MAX_LINK_NUM)
			        session->control_index = -1;
			}

			ret = 0; //删除成功返回0
			break;
		}
    }
	pthread_mutex_unlock(&(session->global_mutex));

    return ret;
}

/* 添加成功返回0; 连接达到最大数，返回-1;  已经存在，返回1 */
int IPC_MID_UTIL_SOCK_AddSessionId(int session_id, IPC_MID_UTIL_SOCK_Session *session)
{
	int i;
	int ret = -1; // 连接达到最大数，返回-1
	int is_exist = 0;

	pthread_mutex_lock(&(session->global_mutex));
	//判断是否已经存在数组中
    for (i = 0; i < BZ_MAX_LINK_NUM; i++) {
        if( (session->session_array[i].flag == 1)  &&
			(session->session_array[i].session_int == session_id)) {
			is_exist = 1; //已经存在
			ret = 1; //已经存在返回1
			break;
        }
    }

	//如果不存在，则添加进数组
	if (is_exist == 0) {
		ret = -1; // 连接达到最大数，返回-1
	    for (i = 0; i < BZ_MAX_LINK_NUM; i++) {
	        if (session->session_array[i].flag == 0) {
				session->session_array[i].session_int = session_id;
				session->session_array[i].flag = 1;


                /* 如果没有control_index，则设置成 control_index */
				PRINT_INFO("old control_index:%d, online:%d\n",
					session->control_index, session->online_session_num);

				session->online_session_num ++;

                /* control_index 默认是 -1 */
                if (session->control_index < 0) {
                    session->control_index = i;
				}

                PRINT_INFO("add session_id %d, control_index:%d, online:%d\n",
                    session_id, session->control_index, session->online_session_num);

				ret = 0; //添加成功返回0
				break;
	        }
	    }
	}

	pthread_mutex_unlock(&(session->global_mutex));
	return ret;
}


//获取主次码流有多少个客户端请求
int IPC_MID_UTIL_SOCK_GetSessionNum(IPC_MID_UTIL_SOCK_Session *session)
{
    int num = 0;
    pthread_mutex_lock(&(session->global_mutex));
    num = session->online_session_num;
    pthread_mutex_unlock(&(session->global_mutex));
    return num;
}


//获取主次码流有多少个客户端请求
int IPC_MID_UTIL_SOCK_GetStreamOnlineNum(int stream_type, IPC_MID_UTIL_SOCK_Session *session)
{
    int num = 0;
    pthread_mutex_lock(&(session->global_mutex));
    if (stream_type == BZ_MAIN_STREAM) {
        num = session->main_stream_num;
    } else if(stream_type == BZ_SUB_STREAM) {
        num = session->sub_stream_num;
    } else {
        PRINT_ERR("stream_type :%d error.\n", stream_type);
    }

    pthread_mutex_unlock(&(session->global_mutex));
    return num;
}

int IPC_MID_UTIL_SOCK_GetStreamOpenNum(int stream_type, IPC_MID_UTIL_SOCK_Session *session)
{
    int num = 0;
    int i = 0;

    pthread_mutex_lock(&(session->global_mutex));

    for (i = 0; i < BZ_MAX_LINK_NUM; i++) {
		if (session->session_array[i].flag == 1) {
	        if (stream_type == BZ_MAIN_STREAM) {
	            if (session->session_array[i].lv_sock.main_stream_socket.n_flag)
	                num ++;
	        } else if(stream_type == BZ_SUB_STREAM) {
	            if (session->session_array[i].lv_sock.sub_stream_socket.n_flag)
	                num ++;
	        } else {
	            PRINT_ERR("stream_type :%d error.\n", stream_type);
	        }
		}
    }

    pthread_mutex_unlock(&(session->global_mutex));
    return num;
}

int IPC_MID_UTIL_SOCK_GetAudioOpenNum(IPC_MID_UTIL_SOCK_Session *session)
{
    int num = 0;
    int i = 0;

    pthread_mutex_lock(&(session->global_mutex));

    for (i = 0; i < BZ_MAX_LINK_NUM; i++) {
		if (session->session_array[i].flag == 1) {
			if (session->session_array[i].is_send_audio == 1) {
				num ++;
			}
		}
    }

    pthread_mutex_unlock(&(session->global_mutex));
    return num;
}

int IPC_MID_UTIL_SOCK_SetAudioSendFlag(int session_id, int flag, IPC_MID_UTIL_SOCK_Session *session)
{
	int i;
	int ret = -1;

	pthread_mutex_lock(&(session->global_mutex));
	//判断是否已经存在数组中
    for (i = 0; i < BZ_MAX_LINK_NUM; i++) {
        if (session->session_array[i].session_int == session_id) 
		{
			session->session_array[i].is_send_audio = flag;
			ret = 0;
			break;
		}
    }
	pthread_mutex_unlock(&(session->global_mutex));
	return ret;
}


//是的话，返回1； 不是则返回0; 错误返回 -1
int IPC_MID_UTIL_SOCK_IsControlSession(int session_id, IPC_MID_UTIL_SOCK_Session *session)
{
    //int i = 0;
    int index = -1;
    int is_control_session = 0;

    pthread_mutex_lock(&(session->global_mutex));
    index = session->control_index;
    if ((index >=0) && (index < BZ_MAX_LINK_NUM)) {
        if (session->session_array[index].session_int == session_id)
            is_control_session = 1;
        else
            is_control_session = 0;
    } else {
        is_control_session = -1; // 错误返回 -1
    }
    //PRINT_INFO("is_control_session:%d, control_index:%d, session_id:%d\n", is_control_session, index, session_id);
    pthread_mutex_unlock(&(session->global_mutex));

    return is_control_session;
}


int IPC_MID_UTIL_SOCK_GetNoticeSockByIndex(int i, IPC_MID_UTIL_SOCK_Session *session)
{
    int sock = -1;

    if (i < 0 || i >= BZ_MAX_LINK_NUM) {
        PRINT_ERR("index error. index:%d max_link_num:%d\n", i, BZ_MAX_LINK_NUM);
        return -1;
    }

	pthread_mutex_lock(&(session->global_mutex));
    if ((session->session_array[i].flag == 1) &&
        (session->session_array[i].cmd_sock > 0)) {
        sock = session->session_array[i].notice_sock;
	}
	pthread_mutex_unlock(&(session->global_mutex));

    return sock;
}




