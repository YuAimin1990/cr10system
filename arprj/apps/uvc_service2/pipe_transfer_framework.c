#define _GNU_SOURCE

#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>


#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>


#include "pipe_transfer_framework.h"


#if (__IPC_MODE__ == __SHR_MEM_USE__)

int user_data_shr_mem_create(key_t ipc_key, unsigned int size, unsigned int num, transfer_id * t_id)
{
    int ret = -1;
    int value = 0;
    int i;

    t_id->sema_id = semget(ipc_key, 1, IPC_CREAT | IPC_NOWAIT | 0666);
    if(t_id->sema_id < 0)
    {
        printf("create semaphore failed!(%d)\n", t_id->sema_id);
        return ret;
    }

    ret = semctl(t_id->sema_id, 0, SETVAL, value);
    if(ret < 0)
    {
        printf("set semget failed!\n");
        return ret;
    }

    t_id->bell_id = semget(ipc_key + 1, 1, IPC_CREAT | 0666);
    if(t_id->bell_id < 0)
    {
        printf("create semaphore failed!(%d)\n", t_id->bell_id);
        return ret;
    }

    ret = semctl(t_id->bell_id, 0, SETVAL, value);
    if(ret < 0)
    {
        printf("set semget failed!\n");
        return ret;
    }

    t_id->shm_num = num;
    t_id->shm_id = (int *)calloc(num, sizeof(int));
    for(i = 0; i < num; ++i)
    {
        t_id->shm_id[i] = shmget(ipc_key + i, size + sizeof(memory_head), IPC_CREAT | 0666);
        if(t_id->shm_id[i] < 0)
        {
            printf("create share memory failed!\n");
            return -1;
        }
    }
    t_id->shm_size = size;
    t_id->shm_red = 0;
    t_id->shm_wrt = 0;

    /*t_id->shm_id = shmget(ipc_key, size + sizeof(memory_head), IPC_CREAT | 0666);
    if(t_id->shm_id < 0)
    {
        printf("create share memory failed!\n");
        return -1;
    }
    t_id->shm_size = size;*/

    /*t_id->shm_ptr = shmat(t_id->shm_id, NULL, 0);
    if(NULL == t_id->shm_ptr)
    {
        printf("shmat failed!\n");
        return -1;
    }*/

    return 0;
}

int user_data_shr_mem_acquire(transfer_id t_id, void ** shm_ptr, memory_head * phead, struct timespec * timeout)
{
    int             value, i, ret;
    unsigned char * tmp_ptr;
    struct sembuf   sema_b;

    if(t_id.sema_id < 0 || t_id.bell_id < 0 || t_id.shm_id[t_id.shm_red] < 0)
    {
        return -1;
    }

    sema_b.sem_num = 0;
    sema_b.sem_flg = 0;//SEM_UNDO;
    sema_b.sem_op = -1;

    ret = semtimedop(t_id.bell_id, &sema_b, 1, timeout);//wait belling
    if(ret < 0)
    {
        if(EAGAIN != errno)
            printf("semaphore:%d release failed!(ret:%d err num:%d %s)\n", \
                    semctl(t_id.bell_id, 0, GETVAL), ret, errno, strerror(errno));
        return ret;
    }

    value = semctl(t_id.sema_id, 0, GETVAL);
    if(value > 0)//if(1 == value)
    {
        for(i = 0; i < value; ++i)
        {
            unsigned int tmp_index = (t_id.shm_red + i) % t_id.shm_num;
            tmp_ptr = (unsigned char *)shmat(t_id.shm_id[tmp_index], 0, 0);
            if(NULL == tmp_ptr)
            {
                printf("shmat failed!\n");
                return -1;
            }

            phead->lenght = ((memory_head *)tmp_ptr)->lenght;
            phead->reserved = ((memory_head *)tmp_ptr)->reserved;

            if(0 == ((memory_head *)tmp_ptr)->lenght)
            {
                if(shmdt((void *)tmp_ptr) < 0)
                {
                    printf("shmdt failed! %p\n", (void *)tmp_ptr);
                    return -1;
                }
                continue;
            }
            else
            {
                ((memory_head *)tmp_ptr)->lenght = 0;
                *shm_ptr = (void *)(tmp_ptr + sizeof(memory_head));
                break;
            }
        }

        if(i < value)
            return 0;
        else
            return -1;
    }
    return -1;
}


int user_data_shr_mem_release(transfer_id * t_id, void ** shm_ptr)
{
    struct sembuf sema_b;
    unsigned char * tmp_ptr;
    int ret;
    int timeout_count = 0;

    if(t_id->sema_id < 0 || t_id->shm_id[t_id->shm_red] < 0)
    {
        return -1;
    }

    sema_b.sem_num = 0;
    sema_b.sem_flg = 0;//SEM_UNDO;
    sema_b.sem_op = -1;

    while(-1 == (ret = semop(t_id->sema_id, &sema_b, 1)) && EAGAIN == errno && timeout_count < 50)
    {
        ++timeout_count;
        usleep(1);
    }

    if(ret < 0)
    {
        printf("semaphore:%d release failed!(ret:%d err num:%d %s)\n", \
                semctl(t_id->sema_id, 0, GETVAL), ret, errno, strerror(errno));
        return ret;
    }

    if(NULL != *shm_ptr)
    {
        tmp_ptr = (unsigned char *)(*shm_ptr);
        if(shmdt((void *)(tmp_ptr - sizeof(memory_head))) < 0)
        {
            printf("shmdt failed! %p\n", (void *)(tmp_ptr - sizeof(memory_head)));
            return -1;
        }
        *shm_ptr = NULL;
        ++(t_id->shm_red);
        t_id->shm_red %= t_id->shm_num;
    }

    return 0;
}


int user_data_shr_mem_close(transfer_id t_id)
{
    int ret, i;

    if(t_id.sema_id < 0 || t_id.bell_id < 0 || t_id.shm_id != NULL)
    {
        return -1;
    }

    for(i = 0; i < t_id.shm_num; ++i)
    {
        ret = shmctl(t_id.shm_id[i], IPC_RMID, NULL);
        if(ret < 0)
        {
            printf("shm[%d] IPC_RMID failed!\n", i);
        }
    }
    free(t_id.shm_id);

    ret = semctl(t_id.sema_id, 0, IPC_RMID);
    if(ret < 0)
    {
        printf("sema IPC_RMID failed!\n");
    }

    ret = semctl(t_id.bell_id, 0, IPC_RMID);
    if(ret < 0)
    {
        printf("bell_id IPC_RMID failed!\n");
    }

    return ret;
}


int user_data_shr_mem_get_ptr(transfer_id t_id, void ** shm_ptr, unsigned int send_size, unsigned int reserved)
{
    int             value;
    memory_head *   phead;
    unsigned char * tmp_ptr;

    if(t_id.sema_id < 0 || t_id.shm_id[t_id.shm_wrt] < 0 || t_id.shm_size < send_size)
    {
        return -1;
    }

    value = semctl(t_id.sema_id, 0, GETVAL);
    if(value < t_id.shm_num)//if(0 == value)
    {
        tmp_ptr = (unsigned char *)shmat(t_id.shm_id[t_id.shm_wrt], 0, 0);
        if(NULL == tmp_ptr)
        {
            printf("shmat failed!\n");
            return -1;
        }
        //printf("shmat tmp_ptr:%p\n", tmp_ptr);

        phead = (memory_head *)tmp_ptr;
        phead->lenght = send_size;
        phead->reserved = 0;

        *shm_ptr = (void *)(tmp_ptr + sizeof(memory_head));
        //printf("shmat shm_ptr:%p\n", *shm_ptr);

        return 0;
    }
    return -1;
}

int user_data_shr_mem_send(transfer_id * t_id, void ** shm_ptr)
{
    struct sembuf sema_b;
    unsigned char * tmp_ptr;
    int ret;

    if(t_id->sema_id < 0 || t_id->bell_id < 0 || t_id->shm_id[t_id->shm_wrt] < 0)
    {
        return -1;
    }

    sema_b.sem_num = 0;
    sema_b.sem_flg = 0;//SEM_UNDO;
    sema_b.sem_op = 1;

    ret = semop(t_id->sema_id, &sema_b, 1);
    if(ret < 0)
    {
        printf("send failed!(%d)\n", ret);
        return ret;
    }

    ret = semop(t_id->bell_id, &sema_b, 1);//belling
    if(ret < 0)
    {
        printf("send failed!(%d)\n", ret);
        return ret;
    }

    if(NULL != *shm_ptr)
    {
        tmp_ptr = (unsigned char *)(*shm_ptr);
        if(shmdt((void *)(tmp_ptr - sizeof(memory_head))) < 0)
        {
            printf("shmdt failed! %p\n", (void *)(tmp_ptr - sizeof(memory_head)));
            return -1;
        }
        *shm_ptr = NULL;
        ++(t_id->shm_wrt);
        t_id->shm_wrt %= t_id->shm_num;
    }

    return 0;
}


#else

int user_data_pipe_create(char *pipe_len_name, char *pipe_data_name, char *pipe_complete_name)
{
    int ret;
    if(access(pipe_len_name, F_OK) == -1)
    {
        ret = mkfifo(pipe_len_name, 0777);
        if(ret != 0)
        {
            printf("Could not create fifo %s\n", pipe_len_name);
            return ret;
        }
    }

    if(access(pipe_data_name, F_OK) == -1)
    {
        ret = mkfifo(pipe_data_name, 0777);
        if(ret != 0)
        {
            printf("Could not create fifo %s\n", pipe_data_name);
            return ret;
        }
    }

    if(access(pipe_complete_name, F_OK) == -1)
    {
        ret = mkfifo(pipe_complete_name, 0777);
        if(ret != 0)
        {
            printf("Could not create fifo %s\n", pipe_complete_name);
            return ret;
        }
    }
    return ret;
}

int user_data_pipe_server_open(int *pipe_len_fd, int *pipe_data_fd, int *pipe_complete_fd,
                               char *pipe_len_name, char *pipe_data_name, char *pipe_complete_name)
{
    *pipe_len_fd = open(pipe_len_name, O_RDONLY|O_NONBLOCK);
    if(*pipe_len_fd < 0)
    {
        printf("open %s failed\n", pipe_len_name);
        return -1;
    }
    printf("pipe_len_fd: %d\n", *pipe_len_fd);

    *pipe_data_fd = open(pipe_data_name, O_RDONLY|O_NONBLOCK);
    if(*pipe_data_fd < 0)
    {
        printf("open %s failed\n", pipe_data_name);
        return -1;
    }
    printf("pipe_data_fd: %d\n", *pipe_data_fd);

    *pipe_complete_fd = open(pipe_complete_name, O_WRONLY|O_NONBLOCK);//|O_NONBLOCK
    if(*pipe_complete_fd < 0)
    {
        printf("open %s failed\n", pipe_complete_name);
        //return -1;
    }
    printf("pipe_complete_fd: %d\n", *pipe_complete_fd);

    return 0;
}

void user_data_pipe_server_close(int pipe_len_fd, int pipe_data_fd, int pipe_complete_fd)
{
    close(pipe_len_fd);
    close(pipe_data_fd);
    close(pipe_complete_fd);
}



int user_date_acquire_buffer(void * p, unsigned int bufferlen, int pipe_len_fd, int pipe_data_fd)
{
    int             tmp_len;
    char            len_buff[20] = "";

    tmp_len = read(pipe_len_fd, len_buff, sizeof(len_buff));
    if(tmp_len > 0)
    {
        unsigned int * datalen = (unsigned int * )len_buff;
        int count = 0;
        int ret;

        //printf("datalen: %d %x%x%x%x tmp_len:%d\n", *datalen, len_buff[0], len_buff[1], len_buff[2], len_buff[3], tmp_len);

        if(*datalen < bufferlen)
        {
            while(count < *datalen)
            {
                ret = read(pipe_data_fd, p + count, bufferlen);
                if(ret <= 0 && 0 == count)
                    usleep(1);
                else if(ret > 0)
                    count += ret;
                else
                    break;
            }
            return count;
        }
        else// drop data
        {
            while(count < *datalen)
            {
                ret = read(pipe_data_fd, p, bufferlen);
                if(ret <= 0 && 0 == count)
                    usleep(1);
                else if(ret > 0)
                    count += ret;
                else
                    break;
            }
            return -1;
        }
    }
    else//no data
    {
        return 0;
    }
}

int user_date_release_buffer(char *pipe_complete_name, int * pipe_complete_fd, int ret)
{
    fd_set wfds;
    struct timeval tv;
    int fun_ret;

    tv.tv_sec = 0;
    tv.tv_usec = 1000;

    if(*pipe_complete_fd < 0)
    {
        *pipe_complete_fd = open(pipe_complete_name, O_WRONLY|O_NONBLOCK);//open in block
        if(*pipe_complete_fd < 0)
        {
            printf("open %s failed\n", pipe_complete_name);
            return -1;
        }
        printf("pipe_complete_fd: %d\n", *pipe_complete_fd);
    }
    return write(*pipe_complete_fd, &ret, sizeof(ret));

    /*FD_ZERO(&wfds);
    FD_SET(*pipe_complete_fd, &wfds);

    fun_ret = select(*pipe_complete_fd + 1, NULL, &wfds, NULL, &tv);
    if(fun_ret > 0)
    {
        return write(*pipe_complete_fd, &ret, sizeof(ret));
    }
    else
    {
        printf("write failed!(%d)\n", fun_ret);
        return fun_ret;
    }*/

}


int user_data_pipe_client_open(int *pipe_len_fd, int *pipe_data_fd, int *pipe_complete_fd,
                               char *pipe_len_name, char *pipe_data_name, char *pipe_complete_name)
{
    while(1)
    {
        *pipe_len_fd = open(pipe_len_name, O_WRONLY);
        if(*pipe_len_fd >= 0)
        {
            break;
        }
        sleep(1);
    }
    printf("pipe_len_fd: %d\n", *pipe_len_fd);

    while(1)
    {
        *pipe_data_fd = open(pipe_data_name, O_WRONLY);
        if(*pipe_data_fd >= 0)
        {
            break;
        }
        sleep(1);
    }
    printf("pipe_data_fd: %d\n", *pipe_data_fd);

    while(1)
    {
        *pipe_complete_fd = open(pipe_complete_name, O_RDONLY|O_NONBLOCK);//|O_NONBLOCK
        if(*pipe_complete_fd >= 0)
        {
            break;
        }
        sleep(1);
    }
    printf("pipe_complete_fd: %d\n", *pipe_complete_fd);

    return 0;
}


int user_date_send_buffer(void * data, unsigned int len, int pipe_len_fd, int pipe_data_fd, int pipe_complete_fd)
{
    int ret, result;

    ret = write(pipe_data_fd, data, len);
    if(ret < 0)
        return ret;

    ret = write(pipe_len_fd, &len, sizeof(len));
    if(ret < 0)
        return ret;

    /*ret = read(pipe_complete_fd, &result, sizeof(result));
    if(ret < 0)
        return ret;*/
    do
    {
        usleep(100);
        ret = read(pipe_complete_fd, &result, sizeof(result));
    }while(ret <= 0);
    return result;
}

#endif

