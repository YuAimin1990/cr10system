#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include "stddef.h"
#include "execinfo.h"
#include "signal.h"
#include "ar_tunning_service.h"
#include "ar_vcodec_service.h"
#include "ar_infrared_service.h"
#include "iq_main_service.h"

STU_IQ_SERVICE g_iq_service;

static uint16_t calc_checksum(uint8_t *req, uint32_t req_len)
{
    uint32_t cs = 0;
    int i;

    for(i = 0; i < req_len; i++) {
        cs += req[i];
    }
    return (uint16_t)cs;
}

static int client_handle_request(struct ar_dbg_client *client, uint32_t req_len)
{
    struct ar_dbg_msg *recv_msg = (struct ar_dbg_msg *)(client->req_buffer);
    uint8_t *payload = client->req_buffer + sizeof(*recv_msg);

    int ret;

    switch(recv_msg->msg_id)
    {
        case AR_DBG_MSG_CMR:
		case AR_DBG_MSG_CMR1:
		case AR_DBG_MSG_CMR2:
		case AR_DBG_MSG_CMR3:	
		case AR_DBG_MSG_CMR4:
            ret=dbg_handle_cam_tuning((void *)client,payload,recv_msg->msg_len,recv_msg->msg_id);
            break;
        case AR_DBG_MSG_VENC:
            ret = process_vcodec_tuning_func((void*)client, payload, recv_msg->msg_len);
            break;
        case AR_DBG_MSG_INF:
            ret = process_infrared_tuning_func((void*)client, payload, recv_msg->msg_len);
            break;
        default:
            printf("invalid msg id %u.\n", recv_msg->msg_id);
            ret = -EINVAL;
            break;
    }

    /* Encounter errro, return empty pack */
    if(ret){
        printf("Encounter errro, return empty pack");
        client_send_ack(client, NULL, 0, recv_msg->msg_id, 0);
    }

    return ret;
}
int client_recv_req(struct ar_dbg_client *client)
{
    int ret = 0;
    uint32_t recv_len = 0, cs = 0;
    uint8_t *payload;
    struct ar_dbg_msg *recv_msg = (struct ar_dbg_msg *)(client->req_buffer);

    /*
     * ret < 0: receive error
     * ret = 0: peer closed
     */
    while(1)
    {
        ret = recv(client->client_socket, client->req_buffer, sizeof(*recv_msg), 0);
        if (ret < 0) {
            //printf("client_recv_req Socket 1st receive failed errno:%d %s\n",errno,strerror(errno));
            if( errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN ){
                //printf("client_recv_req 1st warn\n");
            }else{
                printf("-client_recv_req 1st err, to close client\n");
                return -EINVAL;
            }

        }

        if (ret == 0) {
            printf("Peer closed\n");
            return -EINVAL;
        }

      if (ret > 0)
          recv_len += ret;
        if(recv_len >= sizeof(*recv_msg)) {
      //printf("hahahhah::recv header end : [recv leng = %d, msg_len = %d, recv_msg_len = %d]\n", recv_len, recv_msg->msg_len, sizeof(*recv_msg));
      break;
    }
    }
    while(1) {
        ret = recv(client->client_socket, client->req_buffer + recv_len,
                recv_msg->msg_len + sizeof(*recv_msg) - recv_len, 0);

        if (ret < 0) {
            //printf("-client_recv_req recv Socket receive failed errno:%d %s\n",errno,strerror(errno));

            if( errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN ){
                //printf("-client_recv_req warn\n");
            }else{
                printf("client_recv_req err, to close client\n");
                return -EINVAL;
            }
        }
        if (ret == 0) {
            printf("client_recv_req Peer closed\n");
            return -EINVAL;
        }

        if (ret > 0)
            recv_len += ret;

    if(recv_len >= recv_msg->msg_len + sizeof(*recv_msg)) {
      //printf("recv end : [recv leng = %d, msg_len = %d, recv_msg_len = %d]\n",
      //recv_len, recv_msg->msg_len, sizeof(*recv_msg));
      break;
    }

    }

    if(recv_msg->header1 != 0xff || recv_msg->header2 != 0x5a) {
        printf("bad magic\n");
        return -EINVAL;
    }

    if(sizeof(*recv_msg) + recv_msg->msg_len > recv_len) {
        printf("bad msg length %ld %d %d\n",
            sizeof(*recv_msg), recv_msg->msg_len, recv_len);
        return -EINVAL;
    }

    payload = client->req_buffer + sizeof(*recv_msg);

    cs = calc_checksum(payload, recv_msg->msg_len);
    if (cs != recv_msg->checksum) {
        printf("bad checksum  cs=%d,recv_msg->checksum=%d\n",cs,recv_msg->checksum);
         return -EINVAL;
    }

    return recv_len;
}

int client_send_ack(void *client, uint8_t *payload, uint32_t payload_size,
        uint8_t msg_id, uint8_t need_checksum)
{
    int ret;
    struct ar_dbg_client *dbg_client = (struct ar_dbg_client *)client;
    struct ar_dbg_msg *ack_msg = (struct ar_dbg_msg *)dbg_client->ack_buffer;

    pthread_mutex_lock(&dbg_client->lock);
    ack_msg->header1 = 0xff;
    ack_msg->header2 = 0x5a;
    ack_msg->version = 0;
    ack_msg->msg_id = msg_id;
    ack_msg->seq_num = dbg_client->seq_num++;
    ack_msg->msg_len = payload_size;
    ack_msg->header_sum = calc_checksum((uint8_t *)ack_msg,
            sizeof(*ack_msg) - sizeof(uint8_t) - sizeof(uint16_t));
    if(need_checksum)
        ack_msg->checksum = calc_checksum(payload, payload_size);
    else
        ack_msg->checksum = 0;

    if(payload != dbg_client->ack_buffer + sizeof(*ack_msg))
        memcpy(dbg_client->ack_buffer + sizeof(*ack_msg), payload, payload_size);

    ret = send(dbg_client->client_socket, dbg_client->ack_buffer,
        sizeof(*ack_msg) + payload_size, 0);
    pthread_mutex_unlock(&dbg_client->lock);

  if(ret < (sizeof(*ack_msg) + payload_size)) {
    if (ret < 0) {
      printf("client_send_ack Socket receive failed(%s)\n",strerror(errno));
      printf("client_send_ack Socket receive failed\n");
    } else {
      printf("no enough bytes sent(%lu:%u)\n",sizeof(*ack_msg) + payload_size, ret);
    }
    return -EINVAL;
  }
  return 0;
}

static void *client_handle_thread(void *p)
{
    STU_IQ_SERVICE *iq_service = (STU_IQ_SERVICE *)p;
    struct ar_dbg_client *client = iq_service->clients[iq_service->clients_num];
    int ret;
    //printf(" iq_service->clients[%d] = %p\n", iq_service->clients_num, client);
  //printf("client_handle_thread start :client_num=%d socket = %d\n", iq_service->clients_num, iq_service->clients[iq_service->clients_num]->client_socket);
    iq_service->clients_num++;
  while (1) {
    ret = client_recv_req(client);
    if(ret <= 0) {
            printf("-client_handle_req rcv msg failed\n");
      break;
    }

    ret = client_handle_request(client, ret);
    if(ret < 0) {
      printf("client_handle_request failed\n");
    }
  }
    printf("client_handle_thread stoped\n");
    iq_service->clients_num--;
  unregister_cam_tuning_client(client,AR_DBG_MSG_CMR);
  unregister_cam_tuning_client(client,AR_DBG_MSG_CMR1);
  unregister_cam_tuning_client(client,AR_DBG_MSG_CMR2);
  unregister_cam_tuning_client(client,AR_DBG_MSG_CMR3);
  unregister_cam_tuning_client(client,AR_DBG_MSG_CMR4);
  clear_vcodec_tuning_send_func((void*)client);
  clear_infrared_tuning_send_func((void *)client);
  close(client->client_socket);
  if (client->req_buffer)
      free(client->req_buffer);
  if (client->ack_buffer)
      free(client->ack_buffer);
  if (client)
      free(client);
    pthread_cond_signal(&iq_service->cond);
    return NULL;
}

static void *client_accept_thread(void *p)
{
    STU_IQ_SERVICE *iq_service = (STU_IQ_SERVICE *)p;
    int ret = -1;
    socklen_t sockaddr_in_size;
    pthread_t handler;
  struct timeval now;
  struct timespec outtime;
  struct timeval skt_timeout = {0, 500000};

    sockaddr_in_size = sizeof(struct sockaddr_in);

    printf("client accept thread running\n");

    pthread_cond_init(&iq_service->cond, NULL);

    while (1) {
        pthread_mutex_lock(&iq_service->service_lock);
        if (iq_service->clients_num >= AR_DBG_MAX_CLIENT) {
            gettimeofday(&now, NULL);
            outtime.tv_sec = now.tv_sec + 5;
            outtime.tv_nsec = now.tv_usec * 1000;
            pthread_cond_timedwait(&iq_service->cond, &iq_service->service_lock, &outtime);
            pthread_mutex_unlock(&iq_service->service_lock);
            iq_service->clients_num--;
            continue;
        }

        pthread_mutex_unlock(&iq_service->service_lock);

    printf("wait one client\n");
    ret = accept(iq_service->socket, (struct sockaddr *)&(iq_service->clients[iq_service->clients_num]->addr), (socklen_t *)&sockaddr_in_size);
    if (ret < 0) {
      printf("Accept socket failed\n");
      free(iq_service->clients[iq_service->clients_num]);
      iq_service->clients[iq_service->clients_num] = NULL;
      continue;
    }

        iq_service->clients[iq_service->clients_num] = (struct ar_dbg_client *)malloc(sizeof(struct ar_dbg_client));
        printf(" iq_service->clients[%d] = %p\n", iq_service->clients_num, iq_service->clients[iq_service->clients_num]);
        if(!iq_service->clients[iq_service->clients_num]) {
            printf("Malloc client failed\n");
            iq_service->clients[iq_service->clients_num] = NULL;
            continue;
        }

        //printf("accept one client : clients_num = %d, client_socket = %d\n", iq_service->clients_num, ret);

        memset(iq_service->clients[iq_service->clients_num], 0, sizeof(struct ar_dbg_client));
        iq_service->clients[iq_service->clients_num]->client_socket = ret;
        iq_service->clients[iq_service->clients_num]->server_socket = iq_service->socket;
        printf("client_socket = %d\n", iq_service->clients[iq_service->clients_num]->client_socket);

    ret = setsockopt(iq_service->clients[iq_service->clients_num]->client_socket, SOL_SOCKET, SO_RCVTIMEO, &skt_timeout, sizeof(skt_timeout));
    if (ret < 0)
      printf("set skt_timeout fail ret %d\n", ret);
    iq_service->clients[iq_service->clients_num]->req_buffer = malloc(AR_DBG_CLIENT_BUF_SIZE);
        if(!iq_service->clients[iq_service->clients_num]->req_buffer) {
            printf("Malloc client buffer failed\n");
            close(iq_service->clients[iq_service->clients_num]->client_socket);
            free(iq_service->clients[iq_service->clients_num]);
            iq_service->clients[iq_service->clients_num] = NULL;

            continue;
        }

        iq_service->clients[iq_service->clients_num]->ack_buffer = malloc(AR_DBG_CLIENT_BUF_SIZE);
        if(!iq_service->clients[iq_service->clients_num]->ack_buffer) {
            printf("Malloc client buffer failed\n");
            close(iq_service->clients[iq_service->clients_num]->client_socket);
            free(iq_service->clients[iq_service->clients_num]);
            iq_service->clients[iq_service->clients_num] = NULL;
            continue;
        }
        pthread_mutex_init(&(iq_service->clients[iq_service->clients_num]->lock), NULL);

        iq_service->clients[iq_service->clients_num]->handler = handler;
        register_cam_tuning_send_ops(client_send_ack,iq_service->clients[iq_service->clients_num],AR_DBG_MSG_CMR);
        register_cam_tuning_send_ops(client_send_ack,iq_service->clients[iq_service->clients_num],AR_DBG_MSG_CMR1);
        register_cam_tuning_send_ops(client_send_ack,iq_service->clients[iq_service->clients_num],AR_DBG_MSG_CMR2);
        register_cam_tuning_send_ops(client_send_ack,iq_service->clients[iq_service->clients_num],AR_DBG_MSG_CMR3);		
        register_cam_tuning_send_ops(client_send_ack,iq_service->clients[iq_service->clients_num],AR_DBG_MSG_CMR4);
        set_vcodec_tuning_send_func(client_send_ack, iq_service->clients[iq_service->clients_num], AR_DBG_MSG_VENC);
        set_infrared_tuning_send_func(client_send_ack, iq_service->clients[iq_service->clients_num], AR_DBG_MSG_INF);
        
        pthread_create(&handler, NULL, client_handle_thread, iq_service);

    }

    return NULL;
}

static int find_pra_index_by_name(char *name,int argc, char *argv[])
{
  int i=0;
  for(i=0;i<argc;i++)
  {
     if(!strcmp(name,argv[i]))
     {
      printf("find pra %s @ %d",name,i);
      return i;
     }
  }
  return 0;
}

static int start_iq_service(STU_IQ_SERVICE *iq_service)
{
    int ret = 0;

    iq_service->socket = socket(AF_INET, SOCK_STREAM, 0);
    if (iq_service->socket < 0) {
        printf("Create socked failed\n");
        return -1;
    }
    printf("Create socked success\n");

    pthread_mutex_init(&iq_service->service_lock, NULL);

    iq_service->service_addr.sin_family = AF_INET;
    iq_service->service_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    iq_service->service_addr.sin_port = iq_service->port;
    int enable = 1;

    if(setsockopt(iq_service->socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    {
        printf("setsockopt(SO_REUSEADDR) failed \n");
    }
    ret = bind(iq_service->socket, (struct sockaddr *)&iq_service->service_addr, sizeof(iq_service->service_addr));
    if(ret < 0) {
        printf("Bind socket failed\n");
        exit(-1);
    }

    printf("Bind socket success\n");

    ret = listen(iq_service->socket, AR_MAX_LISTEN_CLIENT);
    if (ret < 0) {
        printf("Listen socket fail\n");
        return ret;
    }

    printf("Listen socket success\n");
    //create accept client thread
    pthread_create(&iq_service->accept_thread, NULL, client_accept_thread, iq_service);

    return ret;

}

static int close_iq_service(STU_IQ_SERVICE *iq_service)
{
    printf("enter close iq service\n");
    if (iq_service == NULL)
    {
        printf("param is NULL \n");
        return -1;
    }

    if (iq_service->socket )
    {
        close(iq_service->socket);
        iq_service->socket = 0;
    }

    for (int i = 0; i < iq_service->clients_num; i++)
    {

        if (iq_service->clients[i] && iq_service->clients[i]->ack_buffer)
        {

            free(iq_service->clients[i]->ack_buffer);
            iq_service->clients[i]->ack_buffer = NULL;
        }

        if (iq_service->clients[i] && iq_service->clients[i]->req_buffer)
        {
            free(iq_service->clients[i]->req_buffer);
            iq_service->clients[i]->req_buffer = NULL;
        }

        if (iq_service->clients[i] && iq_service->clients[i]->client_socket)
        {
            close(iq_service->clients[i]->client_socket);
            iq_service->clients[i]->client_socket = 0;
        }

        if (iq_service->clients[i])
        {
            free(iq_service->clients[i]);
            iq_service->clients[i] = NULL;
        }
    }


    printf("close service finished\n");
    return 0;
}

static void main_signal_handle(AR_S32 signo)
{
    printf("signo=%d\n",signo);
    g_iq_service.close_service(&g_iq_service);
}

int main(int argc, char *argv[])
{
    int ret = 0;
    memset(&g_iq_service, 0, sizeof(STU_IQ_SERVICE));
    g_iq_service.port=htons(AR_DBG_PORT);
    g_iq_service.start_service = start_iq_service;
    g_iq_service.close_service = close_iq_service;
    g_iq_service.signal_handle = main_signal_handle;
    //signal(SIGKILL, g_iq_service.signal_handle);
    //signal(SIGSEGV, g_iq_service.signal_handle);
    int index=find_pra_index_by_name("--help",argc,argv);
  if(index>0)
  {
      printf("--port n [0-65535], note: please set it >= 1234\n");
    return 0;
  }

  index=find_pra_index_by_name("--port",argc,argv);
  if(index>0)
  {
      int port_val=atoi(argv[index+1]);
      printf("set port to %s port_val=%d\n",argv[index+1],port_val);
    g_iq_service.port=htons(port_val);
  }
  ret = g_iq_service.start_service(&g_iq_service);
    if (ret < 0)
    {
        printf("start service failed\n");
        exit(-1);
    }

    pthread_join(g_iq_service.accept_thread, NULL);
    printf("ar_iqtool_service exit\n");
    ret = g_iq_service.close_service(&g_iq_service);
    if (ret < 0)
    {
        printf("close service failed\n");
        exit(-1);
    }
    exit(0);
}
