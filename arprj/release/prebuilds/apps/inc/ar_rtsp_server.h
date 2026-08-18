#ifndef __AR_RTSPSERVERLIB_H__
#define __AR_RTSPSERVERLIB_H__

#include <stdint.h>
#include <stddef.h>

#include "rtp.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef AR_U32
typedef uint32_t     AR_U32;
#endif
#ifndef AR_S32
typedef int32_t      AR_S32;
#endif
#ifndef AR_U16
typedef uint16_t     AR_U16;
#endif
#ifndef AR_S16
typedef int16_t      AR_S16;
#endif
#ifndef AR_S8
typedef int8_t       AR_S8;
#endif
#ifndef AR_U8
typedef uint8_t      AR_U8;
#endif
#ifndef AR_UCHAR
typedef unsigned char AR_UCHAR;
#endif
#ifndef AR_CHAR
typedef char         AR_CHAR;
#endif

#ifndef AR_DOUBLE
typedef double       AR_DOUBLE;
#endif
#ifndef AR_FLOAT
typedef float        AR_FLOAT;
#endif
#ifndef AR_U64
typedef unsigned long long  AR_U64;
#endif
#ifndef AR_S64
typedef long long    AR_S64;
#endif

#define DFL_RTSP_PORT       554	/* Default RTSP port. */
#define DFL_HTTP_PORT       8060	/* Default HTTP port. */

#define MEDIA_CODEC_H264	(0x0)
#define MEDIA_CODEC_PCMA	(0x1)
#define MEDIA_CODEC_PCMU	(0x2)
#define MEDIA_CODEC_PCM     (0x3)
#define MEDIA_CODEC_ADPCM     (0x4)
#define MEDIA_CODEC_H265     (0x5)
#define MEDIA_CODEC_NOT_SUPPORT (0x6)

/**
 * @strm_rpospp:    two level pointer to stream read position.
 */
AR_S32 AR_RTSP_Init(void);
AR_S32 AR_RTSP_UnInit(void);

/* Media types. */
enum
{
    MEDIA_TYPE_VIDEO    = 0,
    MEDIA_TYPE_AUDIO    = 1,
    MEDIA_TYPE_METADATA = 1,
};

typedef struct MediaDesc
{
    AR_S32 media;	/* 0 - video, 1 - audio, 2 - metadata */
    AR_S32 codec;	/* 0 - H.264, 1 - G.711ALaw, 2 - G.711uLaw, 3 pcm, 4 adpcm, 5 h265 */
    AR_S32 audio_samplerate;
    AR_S32 audio_samplewidth;
    AR_S32 audio_channle_num;
    AR_S32 clockrate;
    AR_S32 record;	/* 0 - Real-Time, 1 - Record */
    AR_S64 start_ms;
    AR_S64 end_ms;
    struct
    {
	AR_S32  enable;
	AR_CHAR ipaddr[32];
	AR_S32  port;
	AR_S32  ttl;
    } multicast;
} MediaDesc;

typedef struct RtspOps
{
    AR_S32     (*describe)(AR_CHAR *path, AR_CHAR *query, MediaDesc *descs, AR_S32 nmemb);
    void *     (*open)(AR_CHAR *path, AR_CHAR *query);
    AR_S32     (*close)(void *handle);
    AR_S32     (*read)(void *handle, void *buf, size_t size, AR_S32 backward);
    AR_S32     (*read_p)(void *handle, void **buf, size_t *size, AR_S32 backward,void *rtsp_header);
    AR_S64     (*seek)(void *handle, AR_S64 off_ms, AR_S32 whence);
    AR_S32     (*skip)(void *handle, AR_S32 numframes, AR_S32 frametype, AR_S32 backward);	/* frametype: 0 - any, 1 - intra */
} RtspOps;

AR_S32 AR_RTSP_AddRoute(const AR_CHAR *route, RtspOps *ops);

AR_S32 AR_RTSP_StartService(const AR_CHAR *ip, AR_S32 rtspport, AR_S32 httpport);
AR_S32 AR_RTSP_StopService(void);

AR_S32 AR_RTSP_Setup(AR_S32 enable, AR_S32 interval_s);
AR_S32 AR_RTSP_GetVersion(AR_CHAR *version, AR_CHAR *buildDate);

AR_S32 AR_RTSP_StartMulticastStream(AR_CHAR *uri);
AR_S32 AR_RTSP_StopMulticastStream(AR_CHAR *uri);

#ifdef __cplusplus
}
#endif

#endif
