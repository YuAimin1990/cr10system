#ifndef __PIPE_TRANSFER_FRAMEWORK_H__
#define __PIPE_TRANSFER_FRAMEWORK_H__

#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#define __PIPE_USE__            0
#define __SHR_MEM_USE__         1
#define __IPC_MODE__            __SHR_MEM_USE__

typedef struct
{
    int * shm_id;
    int sema_id;
    int bell_id;
    //void * shm_ptr;
    unsigned int shm_size;
    unsigned int shm_num;
    unsigned int shm_wrt;
    unsigned int shm_red;
}transfer_id;

typedef struct
{
    int lenght;
    int reserved;
}memory_head;


#if (__IPC_MODE__ == __SHR_MEM_USE__)

int user_data_shr_mem_create(key_t ipc_key, unsigned int size, unsigned int num, transfer_id * t_id);

int user_data_shr_mem_acquire(transfer_id t_id, void ** shm_ptr, memory_head * phead, struct timespec * timeout);
int user_data_shr_mem_release(transfer_id * t_id, void ** shm_ptr);
int user_data_shr_mem_close(transfer_id t_id);

int user_data_shr_mem_get_ptr(transfer_id t_id, void ** shm_ptr, unsigned int send_size, unsigned int reserved);
int user_data_shr_mem_send(transfer_id * t_id, void ** shm_ptr);

#else
int user_data_pipe_create(char *pipe_len_name, char *pipe_data_name, char *pipe_complete_name);
int user_data_pipe_server_open(int *pipe_len_fd, int *pipe_data_fd, int *pipe_complete_fd,
                               char *pipe_len_name, char *pipe_data_name, char *pipe_complete_name);
void user_data_pipe_server_close(int pipe_len_fd, int pipe_data_fd, int pipe_complete_fd);
int user_date_acquire_buffer(void *p, unsigned int bufferlen, int pipe_len_fd, int pipe_data_fd);
int user_date_release_buffer(char *pipe_complete_name, int * pipe_complete_fd, int ret);




int user_data_pipe_client_open(int *pipe_len_fd, int *pipe_data_fd, int *pipe_complete_fd,
                               char *pipe_len_name, char *pipe_data_name, char *pipe_complete_name);
int user_date_send_buffer(void *data, unsigned int len, int pipe_len_fd, int pipe_data_fd, int pipe_complete_fd);

#endif

#endif




