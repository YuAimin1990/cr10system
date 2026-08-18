#ifndef __MEDIA_FIFO_H__
#define __MEDIA_FIFO_H__

#include "pf_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MEDIA_BUF_PRINT_ERR(fmt, args...)  \
    do{\
        printf("\033[0;31m[MEDIABUF ERR] [ %s, Line:%d ]  " fmt "\033[0;0m\n", __FILE__, __LINE__,  ##args); \
    }while(0)

#define MEDIA_BUF_PRINT_INFO(fmt, args...)   printf("[MEDIABUF INFO] [ %s, Line:%d ]" fmt, __FILE__, __LINE__, ##args)


#define VENC_MAX_STREAM_NUM 4
#define MAX_ONE_FRAME_SIZE 700000

typedef void* MEDIABUF_HANDLE;

typedef struct _ZFIFO_NODE_ {
    void *base;
    AR_S32 len;
} ZFIFO_NODE;

typedef enum {
    IPC_STREAM_TYPE_STREAM0 = 0,
    IPC_STREAM_TYPE_STREAM1,
    IPC_STREAM_TYPE_STREAM2,
    IPC_STREAM_TYPE_STREAM3,
} IPC_STREAM_TYPE_E;

typedef enum {
    IPC_FRAME_TYPE_I = 0,
    IPC_FRAME_TYPE_P = 1,
    IPC_FRAME_TYPE_IDR = 2,
    IPC_FRAME_TYPE_A = 3,
    IPC_FRAME_TYPE_MAX = 4,
} IPC_FRAME_TYPE_E;

#define MAGIC_TEST  0XF3B0A4B8

/*! Media buffer frame header */
typedef struct {
    AR_U32 magic;         /* MAGIC_TEST */
    AR_U32 device_type;   /* user defined */
    AR_U32 frame_size;    /* frame size */
    AR_U32 frame_no;      /* frame number */
    AR_U32 width;         /* width << 16 */
	AR_U32 height;        /* height */
    AR_UCHAR reserved;     /* user defined */
    AR_UCHAR frame_type;   /* frame type  IPC_FRAME_TYPE_E*/
    AR_UCHAR frame_rate;   /* fps, not frame rate video:kbps, audio:HZ*/
    AR_UCHAR video_standard; /*video format*/
    AR_U32 sec;             /*second of frame*/
    AR_U64 usec;            /*usecond of frame*/
    AR_S64 pts;             /*pts of frame*/
} IPC_FRAME_HEADER_S;


/*!
*******************************************************************************
** \brief Media buffer init
**
** \param[in]  fifo_id    fifo id of IPC_STREAM_TYPE_E
** \param[in]  size       size of medeia buffer
**
** \return
** - #0         ok
** - #other     failed
**
**
*******************************************************************************
*/

AR_S32 IPC_MID_MBUF_Init(AR_S32 fifo_id, AR_S32 size);

/*!
*******************************************************************************
** \brief Media buffer uninit
**
** \param[in]  fifo_id    fifo id of IPC_STREAM_TYPE_E
**
** \return
** - #0         ok
** - #other     failed
**
*******************************************************************************
*/
AR_S32 IPC_MID_MBUF_UnInit(AR_S32 fifo_id);


/*!
*******************************************************************************
** \brief Media buffer add one writer for write stream frame by type
**
** \param[in]  size       stream type of reader
**
** \return
** - # MEDIABUF_HANDLE      writer's handle
** - # 0                    failed
**
*******************************************************************************
*/
MEDIABUF_HANDLE IPC_MID_MBUF_AddWriter(AR_S32 fifo_id);

/*!
*******************************************************************************
** \brief Media buffer delete one writer
**
** \param[in]  size       writer
**
** \return
**
*******************************************************************************
*/
void IPC_MID_MBUF_DelWriter(MEDIABUF_HANDLE writerid);

/*!
*******************************************************************************
** \brief Media buffer write one frame data and header info
**
** \param[in]       writerid       writer's handle
** \param[in]       data           poAR_S32er of input frame,
** \param[in]       size           length of frame
** \param[out]      header         frame info
**
** \return
** - # < 0      write failed
** - # >=0      success
**
*******************************************************************************
*/
AR_S32 IPC_MID_MBUF_WriteFrame(MEDIABUF_HANDLE writerid, void *data, AR_S32 size, IPC_FRAME_HEADER_S *header);


/*!
*******************************************************************************
** \brief Media buffer add one reader for read stream by type
**
** \param[in]  size       stream type of reader
**
** \return
** - # MEDIABUF_HANDLE      reader's handle
** - # 0                    failed
**
*******************************************************************************
*/

MEDIABUF_HANDLE IPC_MID_MBUF_AddReader(AR_S32 fifo_id);

/*!
*******************************************************************************
** \brief Media buffer delete one reader
**
** \param[in]  readerid       reader's handle
**
** \return
**
*******************************************************************************
*/
void IPC_MID_MBUF_DelReader(MEDIABUF_HANDLE readerid);


/*!
*******************************************************************************
** \brief read one frame from reader,timeout 1 second,  <= 0 failed , > 0 success
**
** \param[in]       readerid       reader's handle
** \param[in/out]   data           poAR_S32er of frame
**                      Attation:
**                          1. if input value of *data is not NULL, use buffer of caller.
**                          2.if input value of *data is NULL, use share buffer of mediabuf'size,
**                              the output value of *data is the start poAR_S32er of buffer.
**
** \param[in/out]   size            length of frame
**                      Attation:
**                        1.if input value of *size is not equal 0, it means buffer length of caller,
**                          if the frame'length > *size,it only copy *size data to caller's buffer.
**                        2.if input value of *size is equal 0, use buffer of mediabuf,
**                          the output value of *size is the size of frame.
**
** param[out]       header          frame info if success
**
** \return
** - # <= 0      read failed
** - # >0        success
*/
AR_S32 IPC_MID_MBUF_ReadFrame(MEDIABUF_HANDLE readerid, void **data, AR_S32 *size, IPC_FRAME_HEADER_S *header);

/*!
*******************************************************************************
** \brief read one frame from reader,no timeout, <=0 failed , >0 success
**
** \param[in]       readerid       reader's handle
** \param[in/out]   data           poAR_S32er of frame,
**                      Attation:
**                          1. if input value of *data is not NULL, use buffer of caller.
**                          2.if input value of *data is NULL, use share buffer of mediabuf'size,
**                              the output value of *data is the start poAR_S32er of buffer.
**
** \param[in/out]   size            length of frame
**                      Attation:
**                        1.if input value of *size is not equal 0, it means buffer length of caller,
**                          if the frame'length > *size,it only copy *size data to caller's buffer.
**                        2.if input value of *size is equal 0, use buffer of mediabuf,
**                          the output value of *size is the size of frame.
**
** param[out]       header          frame info if success
**
** \return
** - # <= 0      read failed
** - # >0        success
*/
AR_S32 IPC_MID_MBUF_ReadIFrame(MEDIABUF_HANDLE readerid, void **data, AR_S32 *size, IPC_FRAME_HEADER_S *header);

AR_S32 IPC_MID_MBUF_ReadNextIFrame(MEDIABUF_HANDLE readerid,  void **data, AR_S32 *size, IPC_FRAME_HEADER_S *header);


/*!
*******************************************************************************
** \brief set reader to read newset frame
**
** \param[in]       readerid       reader's handle
**
** \return
** - # other    failed
** - # 0        success
*/
AR_S32 IPC_MID_MBUF_SetNewestFrame(MEDIABUF_HANDLE readerid);


/*!
*******************************************************************************
** \brief set reader to read oldest frame
**
** \param[in]   readerid    reader's handle
**
** \return
** - # other    failed
** - # 0        success
*/
AR_S32 IPC_MID_MBUF_SetOldestFrame(MEDIABUF_HANDLE readerid);


AR_S32 IPC_MID_MBUF_GetPreSeconds(MEDIABUF_HANDLE readerId, AR_S32 *pre_seconds);

#ifdef __cplusplus
};
#endif
#endif


