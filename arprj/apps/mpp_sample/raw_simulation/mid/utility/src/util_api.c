#include "pf_type.h"
#include "stdarg.h"
#include "util_api.h"
#include "util_net.h"

#define TCP_SELECT_SEND_TIMEOUT_DEF_SEC (6)
#define TCP_SELECT_SEND_TIMEOUT_DEF_COUNT (2)
#define TCP_SELECT_RECV_TIMEOUT_DEF_SEC (6)
#define TCP_SELECT_RECV_TIMEOUT_DEF_COUNT (2)

#define TCP_SEND_RECOUNT (1000)


AR_S32 IPC_MID_UTIL_GetSysTime(struct tm* t)
{
    time_t tt;
    time(&tt);
    struct tm tm_t;
    struct tm *p_tm = localtime_r(&tt, &tm_t);
    memcpy(t, p_tm, sizeof(struct tm));
    t->tm_year += 1900;
    t->tm_mon += 1;

    return 0;
}

int IPC_MID_UTIL_Sleep(int nDelay)
{
    fd_set rfds;
    struct timeval tv;
    int fd = 1;
    FD_ZERO (&rfds);
    FD_SET (fd, &rfds);
    tv.tv_sec = 0;
    tv.tv_usec = nDelay;
    int ret = select(0, NULL, NULL, NULL, &tv);
    if (-1 == ret) {
        PRINT_ERR("select    %d   not support\n", nDelay);
        return -1;
    }
    return 0;
}

pid_t IPC_MID_UTIL_GetTid()
{
     return syscall(SYS_gettid);
}

AR_S32 IPC_MID_UTIL_SetThreadName(const char *name)
{
    prctl(PR_SET_NAME, (unsigned long)name, 0,0,0);
    PRINT_INFO("%s pid:%d tid:%d\n", name, getpid(), IPC_MID_UTIL_GetTid());

    return 0;
}

int IPC_MID_UTIL_Rand(int min, int max)
{
	int value = 0;

	srand( (unsigned int)time(NULL) );

	value = rand() % (max + 1 - min) + min;

	return value;
}
#if 0
unsigned long ip_to_dw(const char *ip_src)
{
    char ip_tmp[16] = {0};
    strcpy(ip_tmp, ip_src);

    char *buf = ip_tmp;
    unsigned char dst[4] = {0};
    int i = 0;
    char *p[4];
    char *saveptr = NULL;
    while ((p[i] = strtok_r(buf, ".", &saveptr)) != NULL) {
        dst[i] = (unsigned char)strtoul(p[i], NULL, 0);
        //printf("%u ", dst[i]);
        buf = NULL;
        i ++;
    }
    //printf("\n");
    unsigned long ret = (dst[3]<<24) | (dst[2]<<16) | (dst[1]<<8) | dst[0];

    return ret;
}

void mac_to_dw(unsigned char *dst, char *mac_src)
{
    char mac_tmp[20] = {0};
    strcpy(mac_tmp, mac_src);

    char *buf = mac_tmp;
    int i = 0;
    char *p[6];
    char *saveptr = NULL;
    while ((p[i] = strtok_r(buf, ":", &saveptr)) != NULL) {
        dst[i] = (unsigned char)strtoul(p[i], NULL, 16);
        //printf("%u ", dst[i]);
        buf = NULL;
        i ++;
    }
    //printf("\n");
}
#endif

int IPC_MID_UTIL_CreateDetachThread(ThreadEntryPtrType entry, void *para, pthread_t *pid)
{
    pthread_t ThreadId;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);//绑定
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);//分离
    if(pthread_create(&ThreadId, &attr, entry, para) == 0) { //创建线程
        pthread_attr_destroy(&attr);
		if(pid)
			*pid = ThreadId;
		return 0;
    }
    pthread_attr_destroy(&attr);
    return -1;
}


void * sub_thread_entry(void * param)
{
    ThreadArgs *pArgs = (ThreadArgs *)param;

    void *ret = pArgs->callback(pArgs->arg);

    free(pArgs->arg);
    free(pArgs);

    return ret;
}


int CreateDetachThread2(ThreadEntryPtrType fuc_call, void *arg1, int arg1_size, pthread_t *pid)
{
    /* push args into buffer */
    ThreadArgs *args = (ThreadArgs *)malloc(sizeof(ThreadArgs));
    if (NULL == args)
        return -1;
    args->callback = fuc_call;
    args->arg_size = arg1_size;
    args->arg = malloc(arg1_size);
    if (NULL == args->arg)
        return -1;
    memcpy(args->arg, arg1, arg1_size);

    pthread_t ThreadId;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);//绑定
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);//分离
    if(pthread_create(&ThreadId, &attr, sub_thread_entry, args) == 0) { //创建线程
        pthread_attr_destroy(&attr);
		if(pid)
			*pid = ThreadId;
		return 0;
    }
    pthread_attr_destroy(&attr);

    return -1;
}


int IPC_MID_UTIL_CreateThread(ThreadEntryPtrType entry, void *para, pthread_t *pid)
{
    pthread_t ThreadId;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);//绑定
    //pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);//分离
    if (pthread_create(&ThreadId, &attr, entry, para) == 0) { //创建线程
        pthread_attr_destroy(&attr);
		if(pid)
			*pid = ThreadId;
		return 0;
    }
    pthread_attr_destroy(&attr);
    return -1;
}

int IPC_MID_UTIL_SysCall(const char *cmd, char *buff, int bufsize)
{
    FILE *fp = NULL;
    //printf("==> %s\n", cmd);

    fp = popen(cmd, "r");
    if (fp == NULL) {
        PRINT_ERR("popen error");
        return -1;
    }

    int ret = 0;
    if ((buff != NULL) && (bufsize > 0))
    {
        ret = fread(buff, 1, bufsize, fp);
    }

    if (ret >= 0) {
        ret = 0;
    } else {
        ret = -1;
    }

    pclose(fp);

    return ret;
}

int IPC_MID_UTIL_ForkSysCall(const char *cmdline)
{
	pid_t pid;
	int ret;

	if(cmdline == NULL) {
		return 1;
	}

	if((pid = vfork()) < 0) {
		printf("create new proccess failed\n");
		ret = -1;
	} else if (pid == 0) {
		//printf("  #%s\n", cmdline);
		execl("/bin/sh", "sh", "-c", cmdline, (char *)0);
		exit(127);
	} else {
		//printf("enter waitpid\n");
		while(waitpid(pid, &ret, 0) < 0){
			if (errno != EINTR) {
				ret = -1;
				break;
			}
		}
		//printf("exit waitpid[%d]\n", ret);
	}
	return ret;
}


int CreateTcpSock(char *ipaddr, int port)
{
	int ret;
	int sockfd;
	in_addr_t s_addr;
	int reuseaddr = 1;
	struct sockaddr_in localaddr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		return -1;

	if (ipaddr && ipaddr[0] && inet_pton(AF_INET, ipaddr, &s_addr) != 1) {
		goto fail;
	} else {
		s_addr = htonl(INADDR_ANY);
	}

	memset(&localaddr, 0, sizeof(localaddr));
	localaddr.sin_family = AF_INET;
	localaddr.sin_port = htons(port);
	localaddr.sin_addr.s_addr = s_addr;

	/* Create socket for listening connections. */
	ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr));
	if (ret < 0)
		goto fail;

	ret = bind(sockfd, (struct sockaddr *)&localaddr, sizeof(localaddr));
	if (ret < 0)
		goto fail;

	ret = listen(sockfd, 5);
	if (ret < 0)
		goto fail;

	return sockfd;

fail:
	close(sockfd);
	return -1;
}


int CreateUdpSock(char *ipaddr, int port)
{
	int ret;
	int sockfd;
	in_addr_t s_addr;
	int reuseaddr = 1;
	struct sockaddr_in localaddr;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)
		return -1;


	if (ipaddr && ipaddr[0] && inet_pton(AF_INET, ipaddr, &s_addr) != 1) {
		goto fail;
	} else {
		s_addr = htonl(INADDR_ANY);
	}

	memset(&localaddr, 0, sizeof(localaddr));
	localaddr.sin_family = AF_INET;
	localaddr.sin_port = htons(port);
	localaddr.sin_addr.s_addr = s_addr;

	/* Create socket for listening connections. */
	ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr));
	if (ret < 0)
		goto fail;

	ret = bind(sockfd, (struct sockaddr *)&localaddr, sizeof(localaddr));
	if (ret < 0)
		goto fail;

	return sockfd;
fail:
	close(sockfd);
	return -1;
}

int CreateBroadcastSock(int recv_port)
{
	int ret = 0;
	int opt;

	//得到该套接字
	//jh: m_LANSearchSock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    int sockSvr = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockSvr < 0) {
        PRINT_ERR("socket fail, error:%s\n", strerror(errno));
        goto cleanup;
    }

#if 1
	//设置该套接字
    opt = 1;
    ret = setsockopt(sockSvr, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (ret < 0) {
        PRINT_ERR("setsockopt  SO_REUSEADDR fail, error:%s\n", strerror(errno));
        goto cleanup;
    }
#endif

#if 0
	//设置该套接字接收超时退出(UDP发送没有超时)
	struct timeval timeout;
	timeout.tv_sec = 10; // 10秒
	timeout.tv_usec = 0;
	if (setsockopt(sockSvr, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(struct timeval)) < 0) {
        PRINT_ERR("setsockopt SO_RCVTIMEO fail, error:%s\n", strerror(errno));
        goto cleanup;
	}
#endif

	//设置该套接字为广播类型
    opt = 1;
    ret = setsockopt(sockSvr, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt));
    if (ret < 0) {
        PRINT_ERR("setsockopt SO_BROADCAST fail, error:%s\n", strerror(errno));
        goto cleanup;
    }

	struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(recv_port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

	//绑定该套接字
    ret = bind(sockSvr, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) {
        PRINT_ERR("bind  fail, error:%s\n", strerror(errno));
        goto cleanup;
    }

#if 0
	//设置该套接字为非阻塞
    curFlags = fcntl(sockSvr, F_GETFL, 0);
    curFlags = fcntl(sockSvr, F_SETFL, curFlags |O_NONBLOCK);
#endif

    return sockSvr;

cleanup:
    if(sockSvr >= 0)
        close(sockSvr);
    return -1;
}


int SetSockAttr(int fd)
{
    #if 1
    int opt = 1;
    int ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (ret < 0) {
        PRINT_ERR("setsockopt SO_REUSEADDR failed, error:%s\n", strerror(errno));
        return -1;
    }
    #endif
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1) {
		PRINT_ERR("Fail to get old flags [set_nonblocking]! errno[%d] errinfo[%s]\n",
			errno, strerror(errno));
		return -1;
	}
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
		PRINT_ERR("Fail to set flags to O_NONBLOCK [set_nonblocking]! errno[%d] errinfo[%s]\n",
			errno, strerror(errno));
		return -1;
	}

    #if 0
    //设置回阻塞
    flags = fcntl(fd, F_GETFL, 0);
    fcntl(connectfd, F_SETFL, flags & ~O_NONBLOCK);
    #endif

#if 0
    //set send buffer
	int size = 0;
    int len = sizeof(size);
    getsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char *)&size, (socklen_t * )&len);
    PRINT_INFO("Socket: the default tcp send buffer size is %d\n",size);

    size = 204800;
    setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char *)&size, len);
    PRINT_INFO("Socket: set the tcp send buffer size to %d\n",size);

    getsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char *)&size, (socklen_t *)&len);
    PRINT_INFO("Socket: after set, the tcp send buffer size is %d\n",size);

    //set recv buffer
    getsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char *)&size, (socklen_t *)&len);
    PRINT_INFO("Socket: the default tcp recv buffer size is %d\n",size);

    size = 204800;
    setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char *)&size, len);
    PRINT_INFO("Socket: set the tcp recv buffer size to %d\n",size);

    getsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char *)&size, (socklen_t *)&len);
    PRINT_INFO("Socket: after set, the tcp recv buffer size is %d\n",size);
#endif
    #if 1
    // set nodelay
    int on = 1;
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void *)&on, sizeof(on));
    #endif
	return 0;
}

int BoardCastSendTo(int fSockSearchfd, char *multi_group, int send_port, const char* inData, const int inLength)
{
	int ret = 0;
	int sendlen = 0;
    struct sockaddr_in addr;

    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    //addr.sin_addr.s_addr = inet_addr("255.255.255.255");
    addr.sin_addr.s_addr = inet_addr(multi_group);
    addr.sin_port = htons(send_port);

	//因为UDP 发送时，没有缓冲区，故需要对发送后的返回值进行判断后，多次发送
	while(sendlen < inLength) {
	    ret = sendto(fSockSearchfd, inData + sendlen, inLength - sendlen, 0, (struct sockaddr*)&addr,  sizeof(addr));
		if (ret < 0) {
			//perror("Send error");
			//非阻塞才有 EAGAIN
			if (errno != EINTR && errno != EAGAIN ) {
				PRINT_ERR("Send() socket %d error :%s\n", fSockSearchfd, strerror(errno));
				return -1;
			}
			else
				continue;
		}

		sendlen += ret;
	}

	PRINT_INFO("ip search SendLen %s: %d,port:%d ret:%s\n", multi_group, sendlen,send_port, strerror(errno));

	return sendlen;
}


int BoardCastSendTo2(int fSockSearchfd, struct sockaddr_in *paddr, const char* inData, const int inLength)
{
	int ret = 0;
	int sendlen = 0;

	//因为UDP 发送时，没有缓冲区，故需要对发送后的返回值进行判断后，多次发送
	while(sendlen < inLength) {
	    ret = sendto(fSockSearchfd, inData + sendlen, inLength - sendlen, 0, (struct sockaddr*)paddr,  sizeof(struct sockaddr_in));
		if (ret < 0) {
			//perror("Send error");
			//非阻塞才有 EAGAIN
			if (errno != EINTR && errno != EAGAIN ) {
				PRINT_ERR("Send() socket %d error :%s\n", fSockSearchfd, strerror(errno));
				return -1;
			}
			else
				continue;
		}

		sendlen += ret;
	}

    PRINT_INFO("search sendto ip:%s, port:%d, len:%d, ret:%d(%s)\n",
        (char *)inet_ntoa(paddr->sin_addr), ntohs(paddr->sin_port), sendlen, ret, strerror(errno));

	return sendlen;
}



void produce_random_mac(char *randomMac)
{
	char HEXCHAR[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	char fmtMACAddr[]={'0','0',':','0','0',':','0','0',':','0','0',':','0','0',':','0','0',0};
	int i = 0, n = 0;
	unsigned long int seed = 0;
	seed = (unsigned long int)HEXCHAR+(unsigned long int)fmtMACAddr+(unsigned long int)&i +(unsigned long int)&n;
	PRINT_INFO("seed: %lu\n", seed);
	srand(seed);
	for(i = 0; i < strlen(fmtMACAddr); i++)
	{
		n = (rand()% 16);
		if(((i+1)% 3) !=0)
		{
			fmtMACAddr[i] = HEXCHAR[n];
		}
	}

	strcpy(randomMac, fmtMACAddr);
	PRINT_INFO("Random mac: %s\n",randomMac);
}

int tcp_send_slice(int sock, char *data, int data_len, int flags)
{
    int payload = 1448;     //wifi:1448
    int send_len = 0;

    if (data_len > payload)
        send_len = payload;
    else
        send_len = data_len;

    return send(sock, data, send_len, flags);
}

int tcp_select_send(int sock, char *szbuf, int len, int timeout)
{
    int ret = 0;
    fd_set send_set;
    struct timeval tv;
    int send_total = 0;
    int n_timeout_num = 0;
    if (timeout == 0)
        timeout = TCP_SELECT_SEND_TIMEOUT_DEF_SEC;
    while(send_total < len) {
        if(TCP_SELECT_SEND_TIMEOUT_DEF_COUNT == n_timeout_num) {//如果超时3次则关闭此次会话
            PRINT_ERR("sock:%d -- select send timeout. timeout is set %d seconds and counts is set %d\n", sock, timeout, n_timeout_num);
            return -2;
        }

    	FD_ZERO(&send_set);
    	FD_SET(sock, &send_set);
    	tv.tv_sec = timeout;
    	tv.tv_usec = 0;
        ret = select(sock + 1, NULL, &send_set, NULL, &tv);
       	if (ret < 0) {
            PRINT_ERR("sock:%d select send error:%s\n", sock, strerror(errno));
          	return -1;
    	}
        if(0 == ret) {
            n_timeout_num++;
            PRINT_ERR("sock:%d select send timeout. count:%d\n", sock, n_timeout_num);
            sleep(1);
            continue;
        }

		if(FD_ISSET(sock, &send_set)) {
        	ret = send(sock, szbuf + send_total, len - send_total, 0);
            if(ret <= 0) {
                PRINT_ERR("SockSend Failed ret:%d, sock:%d, len:%d error:%s\n", ret, sock, len, strerror(errno));
                return -3;
            }

    		n_timeout_num = 0;
			//break;
		}

        send_total += ret;
    }

    //PRINT_INFO("Leave ==========> SockSend sock:%d, send_total:%d\n",sock,  send_total);
	return send_total;
}

int tcp_select_slice_send(int sock, char *szbuf, int len, int timeout)
{
    int ret = 0;
    fd_set send_set;
    struct timeval tv;
    int send_total = 0;
    int n_timeout_num = 0;
    //if (timeout == 0)
        timeout = TCP_SELECT_SEND_TIMEOUT_DEF_SEC;
    while(send_total < len) {
        if(TCP_SELECT_SEND_TIMEOUT_DEF_COUNT == n_timeout_num) {//如果超时3次则关闭此次会话
            PRINT_ERR("sock:%d -- select send timeout. timeout is set %d seconds and counts is set %d\n", sock, timeout, n_timeout_num);
            return -2;
        }

    	FD_ZERO(&send_set);
    	FD_SET(sock, &send_set);
    	tv.tv_sec = timeout;
    	tv.tv_usec = 0;
        ret = select(sock + 1, NULL, &send_set, NULL, &tv);
       	if (ret < 0) {
            PRINT_ERR("sock:%d select send error:%s\n", sock, strerror(errno));
          	return -1;
    	}
        if(0 == ret) {
            n_timeout_num++;
            PRINT_ERR("sock:%d select send timeout. count:%d\n", sock, n_timeout_num);
            sleep(1);
            continue;
        }

		if(FD_ISSET(sock, &send_set)) {
            ret = tcp_send_slice(sock, szbuf + send_total, len - send_total, 0);
            if(ret <= 0) {
                PRINT_ERR("SockSend Failed ret:%d, sock:%d, len:%d error:%s\n", ret, sock, len, strerror(errno));
                return -3;
            }

    		n_timeout_num = 0;
			//break;
		}

        send_total += ret;
    }

    //PRINT_INFO("Leave ==========> SockSend sock:%d, send_total:%d\n",sock,  send_total);
	return send_total;
}


int tcp_send(int sock, char *szbuf, int len, int re_counts)
{
    int ret = 0;
    int send_total = 0;
    int re_send_count = 0;
    while (send_total < len) {
    	ret = send(sock, szbuf + send_total, len - send_total, 0);
        if (ret <= 0) {
            re_send_count ++;
            if (re_send_count > re_counts) {
                PRINT_ERR("sock %d send error, to break. send_count:%d\n", sock, re_counts);
                return -1;
            }

            if(errno == EINTR) {
                PRINT_ERR("errno==EINTR continue, sock:%d, re_send_count:%d\n", sock, re_send_count);
                continue;
            } else if(errno == EAGAIN) {/* EAGAIN : Resource temporarily unavailable*/
                usleep(40000);
                PRINT_ERR("error errno==EAGAIN continue, sock:%d, re_send_count:%d\n", sock, re_send_count);
                continue;
            } else {
                PRINT_ERR("SockSend Failed ret:%d, sock:%d, len:%d error:%s\n", ret, sock, len, strerror(errno));
                return -3;
            }
        }
        re_send_count = 0;
        send_total += ret;
    }

    //PRINT_INFO("Leave ==========> SockSend sock:%d, send_total:%d\n",sock,  send_total);
	return send_total;
}



static int safe_tcp_recv_n (int sockfd, void *buf, int total)
{
    int recv_bytes, cur_len;
    for (recv_bytes = 0; recv_bytes < total; recv_bytes += cur_len) {
        cur_len = recv (sockfd, buf + recv_bytes, total - recv_bytes, 0);
        //closed by client
        if (cur_len == 0) {
            PRINT_ERR("remote close %d", sockfd);
            return -1;
        } else if (cur_len == -1) {
            if (errno == EINTR) {
                //PRINT_INFO("continue recv.");
                cur_len = 0;
            }  else if (errno == EAGAIN) {
                //PRINT_INFO("read complete, read %d", recv_bytes);
                return recv_bytes;
            } else {
                PRINT_ERR("recv tcp packet error, socket=%d, error:%s", sockfd, strerror(errno));
                return -1;
            }
        }
    }
    return recv_bytes;
}

/*
return:
	>=0:  read bytes
	-2:   remote closed
	<0:	  error
*/
int tcp_recv_data (int sockfd, void *buf, int buf_size)
{
	int ret = -1;
    ret = recv (sockfd, buf, buf_size, 0);

    if (ret == 0) {
    //closed by client
        PRINT_ERR("remote close %d", sockfd);
        return -2;
    } else if (ret < 0) {
        if (errno == EINTR) {
            //PRINT_INFO("continue recv.");
			return 0;
        }  else if (errno == EAGAIN) {
            //PRINT_INFO("read complete, read %d", recv_bytes);
            return 0;
        } else {
            PRINT_ERR("recv tcp packet error, socket=%d, error:%s", sockfd, strerror(errno));
            return -1;
        }
    }
    return ret;
}


int tcp_select_recv(int sock, char *szbuf, int len, int timeout)
{
    //select方式接收
    int ret = 0;
    int recv_len = 0;
	fd_set read_set;
	struct timeval tv;
    int n_timeout_num = 0;
 	//if (timeout == 0)
        timeout = TCP_SELECT_RECV_TIMEOUT_DEF_SEC; //sec
	while (1) {
        if(TCP_SELECT_RECV_TIMEOUT_DEF_COUNT == n_timeout_num) {
            PRINT_ERR("sock:%d -- select recv timeout. timeout is set %d seconds and counts is set %d\n", sock, timeout, n_timeout_num);
            return -2;
        }

		FD_ZERO(&read_set);
		FD_SET(sock, &read_set);
		tv.tv_sec = timeout;
		tv.tv_usec = 0;
	   	if ((ret = select(sock + 1, &read_set, NULL, NULL, &tv)) < 0) {
            PRINT_ERR("sock:%d select recv error:%s\n", sock, strerror(errno));
	      	return -1;
		}
        if(0 == ret) {
            n_timeout_num++;
            PRINT_ERR("sock:%d select recv timeout. count:%d\n", sock, n_timeout_num);
            continue;
        }

		if(FD_ISSET(sock, &read_set)) {
    		//recv_len = recv(sock, szbuf, len, 0);
            recv_len = safe_tcp_recv_n (sock, szbuf, len);
            if (recv_len < 0) {
                return -1;
            }
            //PRINT_INFO("recv have data, recv %d\n", recv_len);
    		n_timeout_num = 0;
			break;
		}
	};
    return recv_len;
}

void IPC_MID_UTIL_SetBit(unsigned int *x, int bit, pthread_mutex_t *p_mutex)
{
    pthread_mutex_lock(p_mutex);
    (*x) = (*x) | (1 << bit);
    pthread_mutex_unlock(p_mutex);
}

void IPC_MID_UTIL_ClearBit(unsigned int *x, int bit, pthread_mutex_t *p_mutex)
{
    pthread_mutex_lock(p_mutex);
    (*x) = (*x) & (~(1 << bit));
    pthread_mutex_unlock(p_mutex);
}

int IPC_MID_UTIL_CheckBit(unsigned int x, int bit, pthread_mutex_t *p_mutex)
{
    int ret = 0;
    pthread_mutex_lock(p_mutex);
    //printf("x=%x, offset:%d\n", x, offset);
    ret = (x) & (1 << bit);
    pthread_mutex_unlock(p_mutex);

    return ret;
}

static FILE *sd_log_fp = NULL;

int sd_printf(char *buf, int buf_size)
{
    if (sd_log_fp == NULL) {
        sd_log_fp = fopen("/mnt/sd_card/sd_log.txt", "ab+");
        if (sd_log_fp == NULL)
                return -1;
        char str_hello[10] = {"hello--"};
        fwrite(str_hello, 1, strlen(str_hello), sd_log_fp);
    }

    fwrite(buf, 1, buf_size, sd_log_fp);

    return 0;
}

int IPC_MID_UTIL_GetLabelValue(const char *label, char *path, char *value, int size)
{
	int ret = 0;
	char cmd[200] = {0};
	if(value == NULL || label == NULL) {
		PRINT_ERR("Bad parm");
		return -1;
	}
	snprintf(cmd, sizeof(cmd),"awk '/^%s/{print $1}' %s|sed -e \"2d\" -e \"s/^%s//\"",
		label, path, label);

    printf("get cmd:%s\n", cmd);

	ret = IPC_MID_UTIL_SysCall(cmd, value, size);
	if(ret < 0){
		PRINT_ERR("Net wifi get configure error. file:%s label:%s cmd:%s", path, label, cmd);
	}
	return ret;
}

int IPC_MID_UTIL_SetLabelValue(const char *label, char *path, char *value)
{
	int ret = 0;
	char cmd[200] = {0};
	if(value == NULL || label == NULL) {
		PRINT_ERR("Bad parm");
		return -1;
	}
	snprintf(cmd, sizeof(cmd),"sed -i \"s/^%s.*$/%s%s/\" %s",
		label, label, value, path);

    printf("set cmd:%s\n", cmd);

	ret = IPC_MID_UTIL_SysCall(cmd, value, 0);
	if(ret < 0){
		PRINT_ERR("Net wifi get configure error. file:%s label:%s cmd:%s", path, label, cmd);
	}
	return ret;
}


static const unsigned int uti_crc32_table[256] = {
	0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
	0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
	0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
	0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
	0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
	0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
	0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
	0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
	0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
	0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
	0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
	0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
	0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
	0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
	0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
	0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
	0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
	0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
	0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
	0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
	0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
	0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
	0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
	0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
	0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
	0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
	0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
	0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
	0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
	0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
	0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
	0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
	0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
	0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
	0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
	0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
	0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
	0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
	0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
	0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
	0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
	0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
	0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
	0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
	0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
	0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
	0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
	0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
	0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
	0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
	0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
	0x2d02ef8dL
};

unsigned int IPC_MID_UTIL_Crc32(uint32_t val, const void *ss, int len)
{
	const unsigned char *s = ss;

	while (--len >= 0)
		val = uti_crc32_table[(val ^ *s++) & 0xff] ^ (val >> 8);
	return val;
}

unsigned long getTickCount(void)
{
	struct timeval current;
	gettimeofday(&current, NULL);
	return current.tv_sec*1000 + current.tv_usec/1000;
}

int IPC_MID_UTIL_EncryptStream(char *cData, int len, char *cKey)
{
	int i;
	for (i=0;i+8<len&&i<32;i++)
	{
		cData[i+8] = cData[i+8] ^ cKey[i];
	}
    return len;
}

int IPC_MID_UTIL_DecryptStream(char *cData, int len, char *cKey)
{
	int i;
	for (i=0;i+8<len&&i<32;i++)
	{
		cData[i+8] = cData[i+8] ^ cKey[i];
	}
    return len;
}

/*
 * pszSrc中获取pszParamName字段的值给pszParamValue，
 * 以&或者空格或者结束作为分隔符
 * 成功返回0，失败则返回-1;
 */
int GetStrParamValue(const char *pszSrc, const char *pszParamName, char *pszParamValue, int maxlen)
{
    const char* pos1, *pos = pszSrc;
    int len = 0;

    //check src and pszParam
    if ( !pszSrc || !pszParamName )
    {
        return -1;
    }

    //find param
    pos1 = strstr( pos, pszParamName );

    if ( !pos1 )
    {
        return -1;
    }

    //find param end
    pos = pos1 + strlen( pszParamName ) + 1;
    pos1 = strstr( pos, "&" );

    //find
    if ( pos1 )
    {
        len = pos1 - pos;

        if ( len > maxlen )
        {
            len = maxlen;
        }

        memcpy( pszParamValue, pos, len );
    }
    else	//not find
    {
        //find space
        pos1 = strstr( pos, " " );

        if ( pos1 != NULL )
        {
            len = pos1 - pos;

            if ( len > maxlen )
            {
                len = maxlen;
            }

            memcpy( pszParamValue, pos, len );
        }
        else
        {
            //find cgi end
            len = strlen( pos );

            if ( len > maxlen )
            {
                len = maxlen;
            }

            memcpy( pszParamValue, pos, len );
        }
    }

    return 0;
}

/*
 * pszSrc中获取pszParamName字段的值,转换成整型给iValue，
 * 以&或者空格或者结束作为分隔符
 * 成功返回0，失败则返回-1;
 */
int GetIntParamValue(const char* pszSrc, const char* pszParamName, int* iValue)
{
    char        szParamValue[256];
    const char*  pos1, *pos = pszSrc;
    *iValue = -1;
    memset( szParamValue, 0, sizeof( szParamValue ) );

    if ( !pszSrc || !pszParamName )
    {
        return -1;
    }

    pos1 = strstr( pos, pszParamName );

    if ( !pos1 )
    {
        return -1;
    }

    pos = pos1 + strlen( pszParamName ) + 1;
    pos1 = strstr( pos, "&" );

    if ( pos1 )
    {
        memcpy( szParamValue, pos, pos1 - pos );
    }
    else
    {
        pos1 = strstr( pos, " " );

        if ( pos1 )
        {
            //printf("len %d\n",pos1-pos);
            memcpy( szParamValue, pos, pos1 - pos );
        }
    }

    if ( pos1 )
    {
        //printf("len:%s\n",szParamValue);
        *iValue = atoi( szParamValue );
    }
    else
    {
        //printf("len:%s\n",pos);
        *iValue = atoi( pos );
    }

    return 0;
}


////////////////////////////////////////////////

//====================================================
//fuction:hex to num
//auth:zxh
//date:2013-2-24
//====================================================
#define 	NON_NUM 	'0'
int hex2num(char c)
{
    if (c>='0' && c<='9') return c - '0';
    if (c>='a' && c<='z') return c - 'a' + 10;
    if (c>='A' && c<='Z') return c - 'A' + 10;
    return NON_NUM;
}

//====================================================
//fuction:utf-8 to string
//auth:zxh
//date:2013-2-24
//====================================================
int URLDecode(const char* str, const int strSize, char* result, const int resultSize)
{
    char ch,ch1,ch2;
    int i;
    int j = 0;//record result index

    if ((str==NULL) || (result==NULL) || (strSize<=0) || (resultSize<=0)) {
        return 0;
    }

    for ( i=0; (i<strSize) && (j<resultSize); ++i) {
        ch = str[i];
        switch (ch) {
            case '+':
                result[j++] = ' ';
                break;
            case '%':
                if (i+2<strSize) {
                    ch1 = hex2num(str[i+1]);//high 4 bit
                    ch2 = hex2num(str[i+2]);//low 4 bit
                    if ((ch1!=NON_NUM) && (ch2!=NON_NUM))
                        result[j++] = (char)((ch1<<4) | ch2);
                    i += 2;
                    break;
                } else {
                    break;
                }
            default:
                result[j++] = ch;
                break;
        }
    }
    result[j] = 0;
    return j;
}

//====================================================
//fuction:find value of key  from string
//purl->url param,pkey->key string,pkeyvalue->return value of key,maxlen->max string
//auth:zxh
//date:2013-2-24
//====================================================
int GetStrParamValue2(const char *purl, const char *pkey, char *pkeyvalue,int maxlen)
{
	char 		*phead;
	const char 		*pdst = purl;
	const char		*pend = purl;
	char 			*presult = NULL;
	int 				len = 0;

	//find key
	//printf("purl:%s pkey:%s\n",purl,pkey);
	while(1){
		phead = strstr(pdst, pkey);
		if (!phead)
		{
		    return -1;
		}
		presult = phead - 1;		//split chart
		//printf("presult %x\n",presult[0]);
		if ((presult[0] == 0x3f) || (presult[0] == 0x26))	//0x3f->? 0x26->&
		{
			break;
		}
		pdst += strlen(pkey);
	}
	//printf("phead0:%s\n",phead);
	//move phead to start of value,eg:value=12345;,then move phead=12345;
	phead = phead + strlen(pkey) ;
	//find key end
	pend = strstr(phead, "&");
    	//if find end of key
	if (pend)
	{	//find end of key
		len = pend - phead;
		if (len > maxlen){
		        len = maxlen;
		}
		memcpy(pkeyvalue, phead, len);
		//printf("phead1:%s pend:%s===len=%d\n",phead,pend,len);
		return 0;
	}
	//not find end of key,this is cgi end
        len = strlen(phead);
        if (len > maxlen){
                len = maxlen;
        }
        memcpy(pkeyvalue, phead, len);
//	printf("keyvalue:%s phead2:%s\n",pkey,phead);
    	return 0;
}

//====================================================
//fuction:find a key
//auth:zxh
//date:2013-2-24
//====================================================
int GetKeyValue(char *pparam,char *pkey,char *pvalue)
{
	int					iRet = -1;
	int					len = strlen(pkey);
	char					keyname[CGI_MAX_LEN];
	char					keyvalue[CGI_MAX_LEN];
	char                            decoderbuf[CGI_MAX_LEN];

	//printf("get key value=%d value:%s\n",len,pkey);
	//printf("pparam:%s pkey:%s pvalue:%s\n",pparam,pkey,pvalue);
	//check maxlen
	if( (!pparam) || (!pkey) || (!pvalue)){
			return iRet;
	}
	memset(keyname,0x00,CGI_MAX_LEN);
	strcpy(keyname,pkey);
	keyname[len]='=';
	// printf("get key value1 pkey=%s\n",keyname);
	 memset(keyvalue,0x00,CGI_MAX_LEN);
        iRet = GetStrParamValue2(pparam, keyname, keyvalue,CGI_MAX_LEN - 1);
        if (iRet == 0x00){
                memset(decoderbuf,0x00,CGI_MAX_LEN);
                URLDecode(keyvalue,CGI_MAX_LEN,decoderbuf,CGI_MAX_LEN-1);
		 strcpy(pvalue,decoderbuf);
	//	 printf("pvalue:%s\n",pvalue);
        }
	//printf("get key value2=%d\n",iRet);
	return iRet;
}

//====================================================
//fuction:find a key to int
//auth:zxh
//date:2013-2-24
//====================================================
int  GetKeyInt(char *pparam,char *pkey,int *pvalue)
{
	int 					iRet = -1;
	char                            keyvalue[CGI_MAX_LEN];

	memset( keyvalue, 0x00, CGI_MAX_LEN );
	iRet = GetKeyValue(pparam, pkey, keyvalue);
	//printf("iRet %d pkey:%s keyvalue:%s\n",iRet,pkey,keyvalue);
	if (iRet == 0x00){
		*pvalue = atoi(keyvalue);
	}
	return iRet;
}

//====================================================
//fuction:find a key to int
//auth:zxh
//date:2013-2-24
//====================================================
int GetKeyStr(char *pparam,char *pkey,char *pvalue,int maxlen)
{
	int 					iRet = -1;
	int					len = 0;
	char                            keyvalue[CGI_MAX_LEN];

	memset( keyvalue, 0x00, CGI_MAX_LEN );
	//printf("pparam00:%s pkey:%s keyvalue:%s\n",pparam,pkey,keyvalue);
	iRet = GetKeyValue(pparam, pkey, keyvalue);
	if (iRet == 0x00){
		len = strlen(keyvalue);
		memset(pvalue,0x00,maxlen);
		if (len > maxlen){
			len = maxlen;
		}
		memcpy(pvalue,keyvalue,len);
		iRet = 0x00;
	}
	return iRet;
}

int IPC_MID_UTIL_CopyFile(char *src_name, char *des_name)
{
#if 0
	int fd_old, fd_new, ret;
	char bufer[1024];
	fd_old = open(src_name, O_RDONLY);
	if(fd_old < 0)
	{
		PRINT_ERR("open %s fail", src_name);
		return -1;
	}
	fd_new = open(des_name, O_WRONLY|O_TRUNC|O_CREAT, S_IRWXU|S_IRWXG|S_IRWXO);
	if(fd_new < 0)
	{
		PRINT_ERR("open %s fail", des_name);
		return -1;
	}
	while((ret = read(fd_old, bufer, sizeof(bufer))) > 0)
	{
		if(write(fd_new, bufer, ret) <= 0)
		{
			close(fd_old);
			close(fd_new);
			remove(des_name);
			return -1;
		}
	}
	close(fd_old);
	close(fd_new);
	return 0;
#else
    char cmd[128] = {0};
    sprintf(cmd, "cp -rf %s %s", src_name, des_name);
    IPC_MID_UTIL_SysCall(cmd, NULL, 0);

    return 0;
#endif
}

#if 0
static int is_dir(char * filename)
{
    struct stat buf;
    int ret = stat(filename,&buf);
    if(0 == ret)
    {
        if(buf.st_mode & S_IFDIR)
        {
            //printf("%s is folder\n",filename);
            return 0;
        }
        else
        {
            //printf("%s is file\n",filename);
            return 1;
        }
    }
    PRINT_ERR("stat [%s] error[%s]\n", filename, strerror(errno));
    return -1;
}
#endif

int IPC_MID_UTIL_DeleteFile(char * path)
{
#if 0
    char chBuf[128];
    DIR * dir = NULL;
    struct dirent *ptr;
    int ret = 0;

    ret = is_dir(path);
    if(1 == ret)
    {//file
        if(remove(path))
            PRINT_ERR("remove %s error:%s\n", path,strerror(errno));
        return 0;
    }
    dir = opendir(path);
    if(NULL == dir)
    {
        PRINT_ERR("opendir:%s error:%s\n", path,strerror(errno));
        return -1;
    }
    while((ptr = readdir(dir)) != NULL)
    {
        ret = strcmp(ptr->d_name, ".");
        if(0 == ret)
        {
            continue;
        }
        ret = strcmp(ptr->d_name, "..");
        if(0 == ret)
        {
            continue;
        }
        memset(chBuf, 0, sizeof(chBuf));
        snprintf(chBuf, sizeof(chBuf) - 1, "%s/%s", path, ptr->d_name);
        ret = is_dir(chBuf);
        if(0 == ret)
        {
            //printf("%s is dir\n", chBuf);
            ret = delete_path(chBuf);
            if(0 != ret)
            {
                PRINT_ERR("delete path:%s error:%s\n", chBuf,strerror(errno));
                goto error;
            }
        }
        else if(1 == ret)
        {
            //printf("%s is file\n", chBuf);
            ret = remove(chBuf);
            if(0 != ret)
            {
                PRINT_ERR("remove %s error:%s\n", path,strerror(errno));
                goto error;
            }
        }
    }
    closedir(dir);
    ret = remove(path);
    if(0 != ret)
    {
        PRINT_ERR("remove %s error:%s\n", path,strerror(errno));
        char cmd[128];
        sprintf(cmd, "rm -rf %s", path);
        if(IPC_MID_UTIL_ForkSysCall(cmd) != 0){
            PRINT_ERR("%s error:%s\n", cmd,strerror(errno));
            return -1;
        }
    }
    return 0;

error:
    closedir(dir);
    char cmd[128];
    sprintf(cmd, "rm -rf %s", path);
    if(IPC_MID_UTIL_ForkSysCall(cmd) != 0){
        PRINT_ERR("%s error:%s\n", cmd,strerror(errno));
        return -1;
    }

    return 0;
#else
    char cmd[128] = {0};
    sprintf(cmd, "rm -rf %s", path);
    IPC_MID_UTIL_SysCall(cmd, NULL, 0);

    return 0;
#endif
}


AR_S32 IPC_MID_UTIL_SetZone(AR_S32 gmt)
{
    char *retChar = NULL;
    if(gmt >= -720 && gmt <= 720)
    {
        int const hour = abs(gmt) / 60;
        int const min = abs(gmt) % 60;

        if(min >= 0 && min < 60)
        {
            char text[32] = {""};
            //struct timezone tz;
            //tz.tz_minuteswest = gmt;
            //tz.tz_dsttime = 0;

            retChar = getenv("TZ");
            if(retChar != NULL)
            {
                PRINT_INFO("befor se TZ:get env TZ:%s\n",retChar);
            }
            else
            {
                PRINT_ERR("befor se TZ:get env TZ IS NULL\n");
            }

            snprintf(text, sizeof(text), "UTC%c%02d:%02d", gmt < 0 ? '+' : '-',  hour, min);
            //snprintf(text, sizeof(text), "GMT%c%02d:%02d", gmt < 0 ? '+' : '-',  hour, min); // opposite to GMT
            PRINT_INFO("set TZ :%s\n",text);

            setenv("TZ", text, 1);
            tzset();

            char cmd[128] = {0};

            snprintf(cmd, sizeof(cmd), "echo %s > /opt/custom/cfg/TZ", text);
            printf("cmd:%s\n", cmd);
            IPC_MID_UTIL_SysCall(cmd, NULL, 0);

            sleep(1);

            retChar = getenv("TZ");
            if(retChar != NULL)
            {
                PRINT_INFO("after set TZ:get env TZ:%s\n",retChar);
            }
            else
            {
                PRINT_ERR("after set TZ:get env TZ IS NULL\n");
            }
            //gettimeofday(NULL,&tz);
            //printf("tz:%d,%d\n",tz.tz_minuteswest,tz.tz_dsttime);
        }
    }

    return 0;
}

AR_S32 IPC_MID_UTIL_ShowTime()
{
    time_t timep;
    struct tm *p;
    time(&timep);
    struct tm tt = {0};
    p = localtime_r(&timep, &tt);

    time_t timep2;
    struct tm *p2;
    time(&timep2);
    struct tm tt2 = {0};
    p2 = gmtime_r(&timep2, &tt2);

	PRINT_INFO("localtime: %04d%02d%02d%02d%02d%02d\n", p->tm_year + 1900, p->tm_mon + 1, p->tm_mday,
                                                   p->tm_hour, p->tm_min, p->tm_sec);

	PRINT_INFO("gmtime: %04d%02d%02d%02d%02d%02d\n", p2->tm_year + 1900, p2->tm_mon + 1, p2->tm_mday,
                                                   p2->tm_hour, p2->tm_min, p2->tm_sec);

    return 0;
}

#if 0
static void update_time()
{
    time_t timep;
    struct tm *p;
    time(&timep);
    struct tm tt = {0};
    p = localtime_r(&timep, &tt);

    char command[128] = {0};
    snprintf(command, sizeof(command), "date -s %d.%d.%d-%d:%d:%d",
        p->tm_year + 1900, p->tm_mon + 1, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
    IPC_MID_UTIL_ForkSysCall(command);
    IPC_MID_UTIL_ForkSysCall("hwclock -w");
}
#endif

/*
const int zone[] = {
	-12*60, -11*60, -10*60, -9*60, -8*60, -7*60, -6*60,
	-5*60, -4*60-30, -4*60, -3*60-30, -3*60, -2*60,
	-60, 0, 60, 2*60, 3*60, 3*60+30, 4*60, 4*60+30, 5*60,
	5*60+30, 5*60+45, 6*60, 6*60+30, 7*60, 8*60, 9*60,
	9*60+30, 10*60, 11*60, 12*60
};
zone: minutes, utcOffset: seconds
*/
AR_S32 IPC_MID_UTIL_SetTimeAndZone(int utcOffset, int zone)
{
    //time_t timep;
    struct timeval tval;

    //timep = time(NULL);
    //PRINT_INFO("time() from 1970 :%ld\n",timep);
    IPC_MID_UTIL_ShowTime();
    IPC_MID_UTIL_SetZone(zone);
    IPC_MID_UTIL_ShowTime();

    #if 0
    //tval.tv_sec必须转为UTC时间
    tval.tv_sec = utcOffset + (zone * 60);
    PRINT_INFO("tv_sec :%d, utcOffset:%d, timezone:%d\n", tval.tv_sec, utcOffset, zone);
    #else
    tval.tv_sec = utcOffset;
    //PRINT_INFO("tv_sec :%d, utcOffset:%d, timezone:%d\n", tval.tv_sec, utcOffset, zone);
    #endif

    struct timezone tz;
    tz.tz_minuteswest = -zone;
    //tz.tz_minuteswest = -480;
    PRINT_INFO("tz_minuteswest:%d, zone:%d, utcOffset:%d\n", tz.tz_minuteswest, zone, utcOffset);
    tz.tz_dsttime = 0;

    tval.tv_usec = 0;
    //settimeofday(&tval, NULL);
    settimeofday(&tval, &tz);

    IPC_MID_UTIL_ShowTime();

    return 0;
}

AR_S32 IPC_MID_UTIL_SetLocalTime(IPC_SYS_TIME_INFO_S *t)
{
    char command[128] = {0};
    snprintf(command, sizeof(command), "date -s %d.%d.%d-%d:%d:%d",
        t->Year, t->Mon, t->Day, t->Hour, t->Min, t->Sec);
    IPC_MID_UTIL_ForkSysCall(command);
    IPC_MID_UTIL_ForkSysCall("hwclock -w");

    return 0;
}

AR_S32 IPC_MID_UTIL_GetLocalTime(IPC_SYS_TIME_INFO_S *t)
{
    time_t timep;
    struct tm *p;
    time(&timep);
    struct tm tt = {0};
    p = localtime_r(&timep, &tt);

    t->Year = p->tm_year + 1900;
    t->Mon = p->tm_mon + 1;
    t->Day = p->tm_mday;
    t->Hour = p->tm_hour;
    t->Min = p->tm_min;
    t->Sec = p->tm_sec;

    return 0;
}

AR_S32 IPC_MID_UTIL_GetDateAndTimeStr(AR_CHAR *str_date, AR_CHAR *str_time, AR_S32 preSeconds)
{
    struct tm newtime;
    time_t ltime;
    ltime = time(NULL);

    //previous time
    ltime = ltime - preSeconds;

    localtime_r(&ltime, &newtime);

    if(str_date)
    {
        sprintf(str_date, "%04d-%02d-%02d",
                 newtime.tm_year+1900,
                 newtime.tm_mon+1,
                 newtime.tm_mday);
    }

    if(str_time)
    {
        sprintf(str_time, "%02d-%02d-%02d",
                 newtime.tm_hour,
                 newtime.tm_min,
                 newtime.tm_sec);
    }
    return 0;
}

int IPC_MID_UTIL_ReadFile(char *path, int filesize, void *dst)
{
    FILE *pfd = NULL;

    if( dst == NULL || path == NULL )
    {
        PRINT_ERR("dst or path not exist !!\n");
        goto QUIT_LOAD;
    }

    pfd = fopen(path, "rb");
    if ( pfd == NULL )
    {
        PRINT_ERR("open file error!!\n");
        goto QUIT_LOAD;
    }

    if( fseek( pfd, 0, SEEK_SET ) < 0)
    {
        PRINT_ERR("fseek file error!!\n");
        goto QUIT_LOAD;
    }

    if( fread(dst, filesize, 1, pfd) < 0)
    {
        PRINT_ERR("fread file error!!\n");
        goto QUIT_LOAD;
    }

    if ( pfd != NULL )
    {
        fclose(pfd);
    }

    return 0;

QUIT_LOAD:
    if( pfd != NULL )
    {
        fclose(pfd);
    }

    return -1;
}


int IPC_MID_UTIL_GetFileSize(char *path)
{
    FILE *pfd = NULL;
    int filesize = 0;

    if(path == NULL)
    {
        PRINT_ERR("path not exist !!\n");
        return -1;
    }

    pfd = fopen(path, "rb");
    if ( pfd == NULL )
    {
        PRINT_ERR("open file %s error!!\n", path);
        goto QUIT_GET;
    }


    if ( fseek( pfd, 0, SEEK_END ) < 0)
    {
        PRINT_ERR("fseek file error!!\n");
        goto QUIT_GET;
    }

    filesize = ftell(pfd);

    if ( pfd != NULL )
    {
        fclose(pfd);
    }

    return filesize;

QUIT_GET:
    if( pfd != NULL )
    {
        fclose(pfd);
    }

    return -1;
}

void *IPC_MID_UTIL_OpenFile(char* path, int *size)
{
    int fileSize = 0;
    void *out = NULL;

    if (size)
    {
        *size = 0;
    }

    fileSize = IPC_MID_UTIL_GetFileSize(path);
    if (fileSize < 0)
    {
        PRINT_ERR("IPC_MID_UTIL_GetFileSize error !!\n");
        return NULL;
    }

    fileSize += 1;
    out  = malloc(fileSize);
    if ( !out )
    {
        PRINT_ERR("memory not enough !!\n");
        return NULL;
    }

    if ( IPC_MID_UTIL_ReadFile(path, fileSize - 1, out) < 0 )
    {
        free ( out );
        PRINT_ERR("LoadFile fail !!\n");
        return NULL;
    }

    char *p = (char *)out;
    p[fileSize - 1] = '\0';

    if (size)
    {
        *size = fileSize;
    }

    return out;
}

void IPC_MID_UTIL_FreeMem(void **mem)
{
    if (mem && *mem)
    {
        free(*mem);
        *mem = NULL;
    }
}

AR_S32 IPC_MID_UTIL_GetPid(const char * name)
{
    AR_S32 pid = -1;
    char cmd[128] = {0};
    char buff[10] = {0};

    snprintf(cmd, sizeof(cmd), "pgrep %s", name);
    IPC_MID_UTIL_SysCall(cmd, buff, sizeof(buff));

    pid = atoi(buff);

    return pid;
}

void IPC_MID_UTIL_Killall(const char * name)
{
	char cmd[128] = {0};
    int pid = -1;

    // kill -9 `pgrep hostapd`
    // killall hostapd
    // ps -ef |grep "hostapd" |awk '{print $1}'

    while (1)
    {
        pid = IPC_MID_UTIL_GetPid(name);
        printf("%s pid:%d\n", name, pid);
        if (pid <= 0)
            break;
    	snprintf(cmd, sizeof(cmd), "killall %s", name);
    	printf("cmd:%s\n", cmd);
    	IPC_MID_UTIL_SysCall(cmd, NULL, 0);
        usleep(200*1000);
    }

    return;
}

AR_S32 IPC_MID_UTIL_ExtractSubstr(AR_CHAR *dest, AR_CHAR *src, int start, int end)
{
    int i=start;
    if(start>strlen(src))
        return -1;

    if(end>strlen(src))
        end=strlen(src);

    while(i<end)
    {
        dest[i-start]=src[i];
        i++;
    }
    dest[i-start]='\0';

    return 0;
}


int64_t getTickInterval(int64_t begin)
{
    int64_t end = getTickCount();
    return end - begin;
}

void dump_to_file(const char* file_path, const void* data, int len)
{
	FILE* fp = fopen(file_path, "w");
	if (fp == NULL)
	{
	    printf("open file %s error\n", file_path);
	    return;
	}
	fwrite(data, len, 1, fp);
	fclose(fp);
	printf("save to file %s success\n", file_path);
}

void read_from_file(char* file_path, void* data, int len){
	FILE* fp = fopen(file_path, "r");
	if (fp == NULL)
	{
	    printf("open file %s error\n", file_path);
	    return;
	}
	fread(data, len, 1, fp);
	fclose(fp);
	//printf("read from file %s success\n", file_path);

}

void dump_yuv_to_file(const char* file_path, void *y, void *u, void *v, int w, int h, int y_stride, int u_stride, int v_stride)
{
	FILE* fp = fopen(file_path, "w");
	if (fp == NULL)
	{
	    printf("open file %s error\n", file_path);
	    return;
	}
	fwrite(y, y_stride*h, 1, fp);
    fwrite(u, u_stride*h/2, 1, fp);
    fwrite(v, v_stride*h/2, 1, fp);
	fclose(fp);
	printf("save yuv to file %s success\n", file_path);
}

static int callSystem(const char* command)
{
	int status, ret = 0;
	pid_t pid;
	pid = vfork();
	if (pid < 0) {
		printf("vfork failed\n");
		return -1;
	} else if (pid == 0) {
		ret = execl("/bin/sh", "sh", "-c", command, (char *)0);
		if (ret < 0) {
			printf("execl failed\n");
			return -1;
		}
		return ret;
	} else {
		ret = waitpid(pid, &status, 0);
		if (ret != pid) {
			printf("waitpid failed, ret = %d\n", ret);
			return -1;
		}

		if (WIFEXITED(status)) {
			if (WEXITSTATUS(status)) {
				printf("exit status = %d\n", WEXITSTATUS(status));
				return -1;
			}
		} else {
			printf("abort exceptly\n");
			return -1;
		}
	}
	return 0;
}

int doSystem(const char *fmt, ...)
{
	char cmd_buf[1024];
	int ret;
	va_list args;

	va_start(args, fmt);
	vsprintf(cmd_buf, fmt, args);
	va_end(args);

    //	strcat(cmd_buf, " &2 > 1");
	ret = callSystem(cmd_buf);
    //printf("cmd: %s,ret=%d\n", cmd_buf, ret);
	return ret;
}

