#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "cfg_common.h"
#include "media_fifo.h"
#include "zfifo.h"
//#include "common.h"

static ZFIFO *s_media_fifo[IPC_TOTAL_STREAM_NUM] = {0};

//#define ZFIFO_SIZE_0 (2 * 1024 * 1024)
//#define ZFIFO_SIZE_1 (1 * 1024 * 1024)
//#define ZFIFO_SIZE_2 (512 * 1024)
//#define ZFIFO_SIZE_3 (256 * 1024)

AR_S32 IPC_MID_MBUF_Init(AR_S32 fifo_id, AR_S32 size)
{
    if (fifo_id < 0 || fifo_id > IPC_TOTAL_STREAM_NUM || size <= 0)
    {
        MEDIA_BUF_PRINT_ERR("param error.");
        return -1;
    }

    if (s_media_fifo[fifo_id] != NULL)
    {
        MEDIA_BUF_PRINT_ERR("Mbuf %d has been initialized.\n", fifo_id);
        return 0;
    }

    char name[10] = {0};
    sprintf(name, "stream%d", fifo_id);
    s_media_fifo[fifo_id] = zfifo_init(name, size);
    if (s_media_fifo[fifo_id] == NULL)
    {
        MEDIA_BUF_PRINT_ERR("zfifo_init %s failed.", name);
        return -1;
    }

    return 0;
}


AR_S32 IPC_MID_MBUF_UnInit(AR_S32 fifo_id)
{
    if (fifo_id < 0 || fifo_id > IPC_TOTAL_STREAM_NUM)
    {
        MEDIA_BUF_PRINT_ERR("param error.");
        return -1;
    }

    if (s_media_fifo[fifo_id] == NULL)
    {
        return 0;
    }

    zfifo_uninit(s_media_fifo[fifo_id]);
	s_media_fifo[fifo_id] = NULL;

    return 0;
}

#if 0
int mediabuf_add(int index)
{
    if (index < 0 || index > 3) {
        MEDIA_BUF_PRINT_ERR("param error.");
        return -1;
    }
    if (s_media_fifo[index] != NULL) {
        MEDIA_BUF_PRINT_ERR("param error.");
        return -1;
    }
    int size = 0;

    switch (index) {
        case 0:
            size = ZFIFO_SIZE_0;
            break;
        case 1:
            size = ZFIFO_SIZE_1;
            break;
        case 2:
            size = ZFIFO_SIZE_2;
            break;
        case 3:
            size = ZFIFO_SIZE_3;
            break;
        default:
            break;
    }

    char name[10] = {0};
    sprintf(name, "stream%d", index);
    s_media_fifo[index] = zfifo_init(name, size);
    if (s_media_fifo[index] == NULL) {
        MEDIA_BUF_PRINT_ERR("zfifo_init %s failed.", name);
        return -1;
    }

    return 0;
}

int IPC_MID_MBUF_Init(int num)
{
    if (num < 1 || num > 4) {
        MEDIA_BUF_PRINT_ERR("param %d error.", num);
        return -1;
    }

    int i, ret;
    for (i = 0; i < VENC_MAX_STREAM_NUM; i ++) {
        s_media_fifo[i] = NULL;
    }

    if (num == 4) {
        ret = mediabuf_add(3);
        if (ret < 0)
            return -1;
    }
    if (num >= 3) {
        ret = mediabuf_add(2);
        if (ret < 0)
            return -1;
    }
    if (num >= 2) {
        ret = mediabuf_add(1);
        if (ret < 0)
            return -1;
    }
    if (num >= 1) {
        ret = mediabuf_add(0);
        if (ret < 0)
            return -1;
    }

    return 0;
}

int mediabuf_del(int index)
{
    if (index < 0 || index > 3) {
        MEDIA_BUF_PRINT_ERR("param error.");
        return -1;
    }
    if (s_media_fifo[index] == NULL) {
        MEDIA_BUF_PRINT_ERR("param error.");
        return -1;
    }

    zfifo_uninit(s_media_fifo[index]);

    return 0;
}

void IPC_MID_MBUF_UnInit()
{
    int i;
    for (i = 0; i < VENC_MAX_STREAM_NUM; i ++) {
        if (s_media_fifo[i] != NULL)
            zfifo_uninit(s_media_fifo[i]);
    }
}
#endif

MEDIABUF_HANDLE IPC_MID_MBUF_AddReader(AR_S32 fifo_id)
{
    ZFIFO * zfifo = s_media_fifo[fifo_id];
    ZFIFO_DESC *id = zfifo_open(zfifo);
    if (id == NULL)
    {
        MEDIA_BUF_PRINT_ERR("zfifo_open stream%d failed.", fifo_id);
        return NULL;
    }

    return (MEDIABUF_HANDLE)id;
}

void IPC_MID_MBUF_DelReader(MEDIABUF_HANDLE readerid)
{
    ZFIFO_DESC *zdesc = (ZFIFO_DESC *)readerid;
    if (readerid == NULL)
    {
        MEDIA_BUF_PRINT_ERR("param error.");
        return;
    }
    zfifo_close(zdesc);
}

AR_S32 IPC_MID_MBUF_ReadFrame(MEDIABUF_HANDLE readerid, void **data, AR_S32 *size, IPC_FRAME_HEADER_S *header)
{
    if (header == NULL || data == NULL || readerid == NULL || size == NULL)
    {
        MEDIA_BUF_PRINT_ERR("param error.");
        return -1;
    }

    ZFIFO_NODE node[2];
    node[0].base = header;
    node[0].len = sizeof(IPC_FRAME_HEADER_S);

    ZFIFO_DESC *zfifo_desc = (ZFIFO_DESC *)readerid;
    if (*data == NULL) {
        //printf("IPC_MID_MBUF_ReadFrame data null\n");
        node[1].base = zfifo_desc->frame_data;
        node[1].len = zfifo_desc->frame_len;
    } else {
        //printf("IPC_MID_MBUF_ReadFrame data not null\n");

        node[1].base = *data;
        node[1].len = *size;
    }

	int n = 0;
    n = zfifo_readv_plus((ZFIFO_DESC *)readerid, node, 2, 1000);
    if (n < 0) {
        MEDIA_BUF_PRINT_ERR("zfifo_readv error.");
        exit(-1);
    }
    if (n == 0) {
        //printf("IPC_MID_MBUF_ReadFrame n == %d\n",n);
        *size = 0;
        return 0;
    }
    if (n > 0 && header->magic != MAGIC_TEST ) {
        MEDIA_BUF_PRINT_ERR("zfifo read magic error.");
        exit(-1);
    }

    if (*data == NULL) {
        if (node[1].len > zfifo_desc->frame_len) {
            MEDIA_BUF_PRINT_INFO("data buf size %d is too small, so to realloc %d len", zfifo_desc->frame_len, node[1].len);
            zfifo_desc->frame_len = node[1].len + FRAME_LEN_ADD;
            zfifo_desc->frame_data = realloc(zfifo_desc->frame_data, zfifo_desc->frame_len);
            if (zfifo_desc->frame_data == NULL) {
                MEDIA_BUF_PRINT_ERR("realloc %d error.", zfifo_desc->frame_len);
                exit(-1);
            }
            //zfifo_desc->frame_len = node[1].len;

            memcpy(zfifo_desc->frame_data, node[1].base, node[1].len);
            free(node[1].base);
        }
        *data = zfifo_desc->frame_data;
        *size = node[1].len;
    } else {
        if(node[1].len > *size) {
            MEDIA_BUF_PRINT_ERR("data buf size is too small.");
            memcpy(*data, node[1].base, *size);
            free(node[1].base);
        } else {
            *size = node[1].len;
        }
    }

    return n;
}

AR_S32 mediabuf_read_next_frame(MEDIABUF_HANDLE readerid, void **data, AR_S32 *size, IPC_FRAME_HEADER_S *header)
{
    if (header == NULL || data == NULL || readerid == NULL || size == NULL)
    {
        MEDIA_BUF_PRINT_ERR("param error.");
        return -1;
    }

    ZFIFO_NODE node[2];
    node[0].base = header;
    node[0].len = sizeof(IPC_FRAME_HEADER_S);

    ZFIFO_DESC *zfifo_desc = (ZFIFO_DESC *)readerid;
    if (*data == NULL) {
        node[1].base = zfifo_desc->frame_data;
        node[1].len = zfifo_desc->frame_len;
    } else {
        node[1].base = *data;
        node[1].len = *size;
    }

	int n = 0;
    n = zfifo_readv_next_plus((ZFIFO_DESC *)readerid, node, 2, 1000);
    if (n < 0) {
        MEDIA_BUF_PRINT_ERR("zfifo_readv error.");
        exit(-1);
    }
    if (n == 0) {
        *size = 0;
        return 0;
    }
    if (n > 0 && header->magic != MAGIC_TEST ) {
        MEDIA_BUF_PRINT_ERR("zfifo read magic error.");
        exit(-1);
    }

    if (*data == NULL) {
        if (node[1].len > zfifo_desc->frame_len) {
            MEDIA_BUF_PRINT_INFO("data buf size %d is too small, so to realloc %d len", zfifo_desc->frame_len, node[1].len);
            zfifo_desc->frame_len = node[1].len + FRAME_LEN_ADD;
            zfifo_desc->frame_data = realloc(zfifo_desc->frame_data, zfifo_desc->frame_len);
            if (zfifo_desc->frame_data == NULL) {
                MEDIA_BUF_PRINT_ERR("realloc %d error.", zfifo_desc->frame_len);
                exit(-1);
            }
            //zfifo_desc->frame_len = node[1].len;

            memcpy(zfifo_desc->frame_data, node[1].base, node[1].len);
            free(node[1].base);
        }
        *data = zfifo_desc->frame_data;
        *size = node[1].len;
    } else {
        if(node[1].len > *size) {
            MEDIA_BUF_PRINT_ERR("data buf size is too small.");
            memcpy(*data, node[1].base, *size);
            free(node[1].base);
        } else {
            *size = node[1].len;
        }
    }

    return n;
}


AR_S32 IPC_MID_MBUF_ReadNextIFrame(MEDIABUF_HANDLE readerid,  void **data, AR_S32 *size, IPC_FRAME_HEADER_S *header)
{
    int old_size = *size;
    if (header == NULL || data == NULL || readerid == NULL)
    {
        MEDIA_BUF_PRINT_ERR("param error.");
        return -1;
    }

    int flag = 0;
    if (*data == NULL)
        flag = 1;

    int ret;
    do {
        if (flag == 1) {
            *data = NULL;
            *size = 0;
        } else
            *size = old_size;
        ret = mediabuf_read_next_frame(readerid, data, size, header);
        if (ret < 0)
            return -1;
    } while (header->frame_type != IPC_FRAME_TYPE_I && header->frame_type != IPC_FRAME_TYPE_IDR);

    //printf("size1:%d, size2:%d\n", *size, header->frame_size);
    return ret;
}

AR_S32 IPC_MID_MBUF_ReadIFrame(MEDIABUF_HANDLE readerid, void **data, AR_S32 *size, IPC_FRAME_HEADER_S *header)
{
    if (header == NULL || data == NULL || readerid == NULL || size == NULL)
    {
        MEDIA_BUF_PRINT_ERR("param error.");
        return -1;
    }

    ZFIFO_NODE node[2];
    node[0].base = header;
    node[0].len = sizeof(IPC_FRAME_HEADER_S);

    ZFIFO_DESC *zfifo_desc = (ZFIFO_DESC *)readerid;
    if (*data == NULL) {
        node[1].base = zfifo_desc->frame_data;
        node[1].len = zfifo_desc->frame_len;
    } else {
        node[1].base = *data;
        node[1].len = *size;
    }

    int n = 0;
    while (1) {
        n = zfifo_readv_flag_plus((ZFIFO_DESC *)readerid, node, 2, 1000);
        if (n < 0) {
            MEDIA_BUF_PRINT_ERR("zfifo_readv error.");
            exit(-1);
        }
        if (n == 0) {
            usleep(100 * 1000);
            continue;
        }
        if (n > 0 && header->magic != MAGIC_TEST ) {
            MEDIA_BUF_PRINT_ERR("zfifo read magic error.");
            exit(-1);
        } else {
            MEDIA_BUF_PRINT_INFO("read I frame\n");
            break;
        }
    }

    if (*data == NULL) {
        if (node[1].len > zfifo_desc->frame_len) {
            MEDIA_BUF_PRINT_INFO("data buf size %d is too small, so to realloc %d len", zfifo_desc->frame_len, node[1].len);
            zfifo_desc->frame_len = node[1].len + FRAME_LEN_ADD;
            zfifo_desc->frame_data = realloc(zfifo_desc->frame_data, zfifo_desc->frame_len);
            if (zfifo_desc->frame_data == NULL) {
                MEDIA_BUF_PRINT_ERR("realloc %d error.", zfifo_desc->frame_len);
                exit(-1);
            }
            //zfifo_desc->frame_len = node[1].len;

            memcpy(zfifo_desc->frame_data, node[1].base, node[1].len);
            free(node[1].base);
        }
        *data = zfifo_desc->frame_data;
        *size = node[1].len;
    } else {
        if(node[1].len > *size) {
            MEDIA_BUF_PRINT_ERR("data buf size is too small.");
            memcpy(*data, node[1].base, *size);
            free(node[1].base);
        } else {
            *size = node[1].len;
        }
    }

    return n;
}



MEDIABUF_HANDLE IPC_MID_MBUF_AddWriter(AR_S32 fifo_id)
{
    ZFIFO *zfifo = s_media_fifo[fifo_id];
    ZFIFO_DESC *id = zfifo_open(zfifo);
    if (id == NULL)
    {
        MEDIA_BUF_PRINT_ERR("zfifo_open stream%d failed.", fifo_id);
        return NULL;
    }

    return (MEDIABUF_HANDLE)id;
}

void IPC_MID_MBUF_DelWriter(MEDIABUF_HANDLE writerid)
{
    ZFIFO_DESC *zdesc = (ZFIFO_DESC *)writerid;
    if (writerid == NULL) {
        MEDIA_BUF_PRINT_ERR("param error.");
        return;
    }

    zfifo_close(zdesc);
}

AR_S32 IPC_MID_MBUF_WriteFrame(MEDIABUF_HANDLE writerid, void *data, AR_S32 size, IPC_FRAME_HEADER_S *header)
{
    if (header == NULL || data == NULL || writerid == NULL) {
        MEDIA_BUF_PRINT_ERR("param error\n");
        return -1;
    }
    ZFIFO_NODE node[2];
    node[0].base = header;
    node[0].len = sizeof(IPC_FRAME_HEADER_S);
    node[1].base = data;
    node[1].len = size;
	/* check zfifo */
    header->magic = MAGIC_TEST;

    int ret;
    #if 0
    ret = zfifo_writev((ZFIFO_DESC *)writerid, node, 2);
    if (ret < 0) {
        MEDIA_BUF_PRINT_ERR("zfifo_writev error.");
        exit(-1);
    }
    #else
    if (header->frame_type == IPC_FRAME_TYPE_I || header->frame_type == IPC_FRAME_TYPE_IDR) {
        ret = zfifo_writev_plus((ZFIFO_DESC *)writerid, node, 2, 1);
        if (ret < 0) {
            MEDIA_BUF_PRINT_ERR("zfifo_writev error.");
            exit(-1);
        }
    } else {
        ret = zfifo_writev_plus((ZFIFO_DESC *)writerid, node, 2, 0);
        if (ret < 0) {
            MEDIA_BUF_PRINT_ERR("zfifo_writev error.");
            exit(-1);
        }
    }
    #endif
    return ret;
}

AR_S32 IPC_MID_MBUF_SetNewestFrame(MEDIABUF_HANDLE readerid)
{
    if (readerid == NULL) {
        MEDIA_BUF_PRINT_ERR("param error.");
        return -1;
    }

    zfifo_set_newest_frame((ZFIFO_DESC *)readerid);

    return 0;
}

AR_S32 IPC_MID_MBUF_SetOldestFrame(MEDIABUF_HANDLE readerid)
{
    if (readerid == NULL) {
        MEDIA_BUF_PRINT_ERR("param error.");
        return -1;
    }

    zfifo_set_oldest_frame((ZFIFO_DESC *)readerid);

    return 0;
}

int mediabuf_toal_size(MEDIABUF_HANDLE w_r_id)
{
    return ((ZFIFO_DESC *)w_r_id)->zfifo->buf_size;
}


AR_S32 IPC_MID_MBUF_GetPreSeconds(MEDIABUF_HANDLE readerId, AR_S32 *pre_seconds)
{
    int ret;
    IPC_MID_MBUF_SetNewestFrame(readerId);

    char *recv_buf = NULL;
    int data_length = 0;
    IPC_FRAME_HEADER_S header;
    memset(&header, 0, sizeof(header));
    ret = IPC_MID_MBUF_ReadFrame(readerId, (void **)&recv_buf, &data_length, &header);
    if (ret < 0) {
		MEDIA_BUF_PRINT_ERR("IPC_MID_MBUF_ReadFrame newest fail.\n");
		return -1;
    } else if (ret == 0) {
        MEDIA_BUF_PRINT_ERR("read newest no data.\n");
        return -1;
    }

    unsigned int msec0 = header.sec * 1000 + header.usec / 1000;
    MEDIA_BUF_PRINT_INFO("newest, %u - %llu,  %u\n", header.sec, header.usec, msec0);

    IPC_MID_MBUF_SetOldestFrame(readerId);

    recv_buf = NULL;
    data_length = 0;
    memset(&header, 0, sizeof(header));
    ret = IPC_MID_MBUF_ReadFrame(readerId, (void **)&recv_buf, &data_length, &header);
    if (ret < 0) {
		MEDIA_BUF_PRINT_ERR("IPC_MID_MBUF_ReadFrame oldest fail.\n");
		return -1;
    } else if (ret == 0) {
        MEDIA_BUF_PRINT_ERR("read oldest no data.\n");
        return -1;
    }

    unsigned int  msec1 = header.sec * 1000 + header.usec / 1000;
    MEDIA_BUF_PRINT_INFO("oldest, %u - %llu,  %u\n", header.sec, header.usec, msec1);

    int sec = (msec0 - msec1) / 1000;
    if (sec == 0) {
        *pre_seconds = 1;
    } else {
        *pre_seconds = sec;
    }

    MEDIA_BUF_PRINT_INFO("pre_seconds:%d, sec:%d\n", *pre_seconds, sec);

    IPC_MID_MBUF_SetOldestFrame(readerId);

    return 0;
}

