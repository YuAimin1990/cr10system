#ifndef _IQ_MAIN_SERVICE_H
#define _IQ_MAIN_SERVICE_H
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

#define AR_DBG_MAX_CLIENT 10
#define AR_DBG_CLIENT_BUF_SIZE 0x2000000
#define AR_DBG_PORT 1234
#define AR_MAX_LISTEN_CLIENT (10)

enum {
    AR_DBG_MSG_SYS=0,
    AR_DBG_MSG_REG,
    AR_DBG_MSG_BB,
    AR_DBG_MSG_CMR,
    AR_DBG_MSG_VENC,
    AR_DBG_MSG_INF,    
    AR_DBG_MSG_CMR1,
    AR_DBG_MSG_CMR2, 
    AR_DBG_MSG_CMR3,
    AR_DBG_MSG_CMR4,
    AR_DBG_MSG_MAX,
} enum_msg_module_id;

struct ar_dbg_client
{
    struct sockaddr_in addr;
    uint8_t *req_buffer;
    uint8_t *ack_buffer;
    int server_socket;
    int client_socket;
    uint32_t status;
    pthread_t handler;
    pthread_mutex_t lock;
    uint16_t seq_num;
};

struct ar_dbg_msg
{
    uint8_t  header1;//0xff
    uint8_t  header2;//0x5a
    uint8_t  version;
    uint8_t  msg_id;//enum_msg_module_id
    uint16_t seq_num;
    uint32_t msg_len;// payload length
    uint8_t  header_sum;//head checksum
    uint16_t checksum;//payload checksum
    uint8_t  revert[3];
    uint8_t  payload[];
}__attribute__((packed));

typedef struct _iq_service  STU_IQ_SERVICE;

struct _iq_service
{
    struct ar_dbg_client *clients[AR_DBG_MAX_CLIENT];
    struct sockaddr_in service_addr;
    int socket;
    int port;
    pthread_t accept_thread;
    int clients_num;
    pthread_mutex_t service_lock;
    pthread_cond_t cond;
    int (*start_service)(STU_IQ_SERVICE *iq_service);
    int (*close_service)(STU_IQ_SERVICE *iq_service);
    void (*signal_handle)(int signo);
};
int client_send_ack(void *client, uint8_t *payload, uint32_t payload_size,
        uint8_t msg_id, uint8_t need_checksum);

#endif
