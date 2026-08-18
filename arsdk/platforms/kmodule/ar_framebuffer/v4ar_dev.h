#ifndef __V4AR_DEV_H__
#define __V4AR_DEV_H__
#include <linux/types.h>
#include <linux/ioctl.h>
#include "rpc_fs_private_buffer.h"

#define COMMON_CTL_DATA_LEN 256
#define COMMON_EVENT_DATA_LEN 256

enum {
	V4AR_DEV_ENUM_SIZE,
	V4AR_DEV_SET_SIZE,
	V4AR_DEV_GET_SIZE,
	V4AR_DEV_ENUM_FOAMAT,
	V4AR_DEV_SET_FOAMAT,
	V4AR_DEV_GET_FOAMAT,
	V4AR_DEV_GET_EVENT,
	V4AR_DEV_QBUF,
	V4AR_DEV_DQBUF,
	V4AR_DEV_STREAM_ON,
	V4AR_DEV_STREAM_OFF,
	V4AR_DEV_REQUEST_BUF,
	V4AR_DEV_RELEASE_BUF,
	V4AR_DEV_CACHE_FLUSH,
	V4AR_DEV_CACHE_INVALIDE,
	V4AR_DEV_COMMON_CTL_EXT,
	V4AR_DEV_PRIV_START,
};

enum {
	V4AR_BUFFER_NORMA,
	V4AR_BUFFER_RING,
};

enum {
	V4AR_MEM_MALLOC_USR,
	V4AR_MEM_MALLOC_FW,
};

enum {
	V4AR_CACHE_INVALIDE,
	V4AR_CACHE_FLUSH,
};

// TODO, leave for pipeline test
// may be not used in mpi branch
typedef struct
{
	int index;
	int width;
	int height;
} v4ar_dev_enum_size_t;

typedef struct
{
	int width;
	int height;
    int y_stride;
    int chroma_stride;
}v4ar_dev_size_t;

typedef struct
{
	int index;
	int format;
} v4ar_dev_enum_format_t;

typedef struct
{
	int format;
} v4ar_dev_format_t;

typedef struct
{
	int event;
	int len;
	char data[COMMON_EVENT_DATA_LEN];
} v4ar_event_t;

typedef struct
{
	void *pa;
	void *va;
	int len;
} v4ar_pannel_t;

typedef v4ar_pannel_t v4ar_mem_t;

typedef struct
{
	void *pa;
	void *va;
	int len;
	int offset;
	int used;
} v4ar_ring_pannel_t;

typedef struct {
	unsigned int format; /* refer to different color space definition*/
	unsigned int width;
	unsigned int height;
	unsigned int fps_num;
	unsigned int fps_den;
	unsigned int luma_stride;
	unsigned int chroma_stride;
    int      is_interlace;
} video_format_info_t;

typedef struct
{
	int frame_id;
	unsigned long long pts;
	unsigned int duration;
	int  buffer_type;
	int  mem_malloc_type;
	int pannle_num;
	union {
		v4ar_pannel_t pannel[3];
		v4ar_ring_pannel_t ring_pannel[3];
	} mem;
	v4ar_private_buffer_info_t private_buffer_info;
	unsigned long data[4];
} v4ar_buffer_t;

typedef struct
{
	int ctl_code;
	int len;
    int ret;
#ifdef CONFIG_BIT_32BITS
	int pad0;
	char* data;
	int pad1;
#else
	char* data;
#endif
} v4ar_common_ctl_ext_t;

#define VIDIOC_V4AR_DEV_ENUM_SIZE _IOWR('A', V4AR_DEV_ENUM_SIZE, v4ar_common_ctl_ext_t)
#define VIDIOC_V4AR_DEV_SET_SIZE _IOWR('A', V4AR_DEV_SET_SIZE, v4ar_common_ctl_ext_t)
#define VIDIOC_V4AR_DEV_GET_SIZE _IOWR('A', V4AR_DEV_GET_SIZE, v4ar_common_ctl_ext_t)
#define VIDIOC_V4AR_DEV_ENUM_FOAMAT _IOWR('A', V4AR_DEV_ENUM_FOAMAT, v4ar_common_ctl_ext_t)
#define VIDIOC_V4AR_DEV_SET_FOAMAT _IOWR('A', V4AR_DEV_SET_FOAMAT, v4ar_common_ctl_ext_t)
#define VIDIOC_V4AR_DEV_GET_FOAMAT _IOWR('A', V4AR_DEV_GET_FOAMAT, v4ar_common_ctl_ext_t)
#define VIDIOC_V4AR_DEV_GET_EVENT _IOWR('A', V4AR_DEV_GET_EVENT, v4ar_common_ctl_ext_t)
#define VIDIOC_V4AR_DEV_QBUF _IOWR('A', V4AR_DEV_QBUF, v4ar_common_ctl_ext_t)
#define VIDIOC_V4AR_DEV_DQBUF _IOWR('A', V4AR_DEV_DQBUF, v4ar_common_ctl_ext_t)
#define VIDIOC_V4AR_DEV_STREAM_ON _IOWR('A', V4AR_DEV_STREAM_ON, v4ar_common_ctl_ext_t)
#define VIDIOC_V4AR_DEV_STREAM_OFF _IOWR('A', V4AR_DEV_STREAM_OFF, v4ar_common_ctl_ext_t)
#define VIDIOC_V4AR_DEV_REQUEST_BUF _IOWR('A', V4AR_DEV_REQUEST_BUF, v4ar_common_ctl_ext_t)
#define VIDIOC_V4AR_DEV_RELEASE_BUF _IOWR('A', V4AR_DEV_RELEASE_BUF, v4ar_common_ctl_ext_t)
#define VIDIOC_V4AR_DEV_CACHE_FLUSH _IOWR('A', V4AR_DEV_CACHE_FLUSH, v4ar_common_ctl_ext_t)
#define VIDIOC_V4AR_DEV_CACHE_INVALIDE _IOWR('A', V4AR_DEV_CACHE_INVALIDE, v4ar_common_ctl_ext_t)
#define VIDIOC_V4AR_DEV_COMMON_CTL_EXT _IOWR('A', V4AR_DEV_COMMON_CTL_EXT, v4ar_common_ctl_ext_t)
#endif
