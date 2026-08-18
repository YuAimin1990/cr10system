#ifndef __AR_AVI_API_H__
#define __AR_AVI_API_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <time.h>
#include <assert.h>
#include <sys/vfs.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <pthread.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdint.h>

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
typedef unsigned long long     AR_U64;
#endif
#ifndef AR_S64
typedef long long      AR_S64;
#endif



/* FOURCC */
#define MAKE_FOURCC(a,b,c,d)         (AR_S32)((a)|(b)<<8|(c)<<16|(d)<<24)

//video
#define AVI_TYPE_H264     MAKE_FOURCC('H','2','6','4')
#define AVI_TYPE_H265     MAKE_FOURCC('H','2','6','5') //hev1 or hevc/h265??
#define AVI_TYPE_MPEG4    MAKE_FOURCC('D','I','V','X')
#define AVI_TYPE_MJPEG    MAKE_FOURCC('M','J','P','G')
#define AVI_TYPE_GMTAG    MAKE_FOURCC('G','M','T','G')
//audio
#define AVI_TYPE_PCM      MAKE_FOURCC('P','C','M',' ')
#define AVI_TYPE_MP3      MAKE_FOURCC('M','P','E','G')

#define REC_BUF_SIZE (1024*1024*3)
#define PB_BUF_SIZE (1024*700)

/* AVI flag, defined by aviriff.h */
#define AVIF_HASINDEX        0x00000010
#define AVIF_MUSTUSEINDEX    0x00000020
#define AVIF_ISINTERLEAVED   0x00000100
#define AVIF_TRUSTCKTYPE     0x00000800 // Use CKType to find key frames
#define AVIF_WASCAPTUREFILE  0x00010000
#define AVIF_COPYRIGHTED     0x00020000

typedef enum
{
    AVI_FRAME_TYPE_I = 0,
    AVI_FRAME_TYPE_P = 1,
    AVI_FRAME_TYPE_IDR = 2,
    AVI_FRAME_TYPE_A = 3,
    AVI_FRAME_TYPE_MAX = 4,
} AVI_FRAME_TYPE_E;

//======================================================//
/* AVI Datastruct */
typedef struct AviFileTag { //AviWriteHandle
    FILE    *file;
    AR_CHAR   filename[128];

    AR_S32    data_offset;

    AR_S32    video_count;
    //AR_S32    audio_enable;
    AR_S32    audio_bytes;
    AR_S32    samplesize;
    //AR_S32    audio_type; // 0 a-law; 1 u-law; 2 pcm

    AR_S32 bps;
    AR_S32 fps;
    AR_S32 gop;
    AR_S32 width;
    AR_S32 height;
    AR_S32 codec_type;

    AR_S32 audio_enable;
	AR_S32 a_enc_type; // 0 a-law; 1 u-law; 2- pcm
    AR_S32 a_chans;    /* Audio channels, 0 for no audio */
    AR_S32 a_rate;     /* Rate in Hz */
    AR_S32 a_bits;     /* bits per audio sample */

    //index
    AR_S32    index_count;   //real index count
    AR_S32    *idx_array;
    AR_S32    idx_array_count;

    //BUFFER
    AR_CHAR  * buffer;
    AR_S32   buffer_pos;
    AR_S32   buffer_frame_cnt; //write to file every 15 frames.
}AviFile;


typedef struct tagAviInitParam {
    AR_S32 bps;
    AR_S32 fps;
    AR_S32 width;
    AR_S32 height;
    AR_S32 codec_type;

    AR_S32 audio_enable;
	  AR_S32 a_enc_type; // 0 a-law; 1 u-law; 2- pcm
    AR_S32 a_chans;    /* Audio channels, 0 for no audio */
    AR_S32 a_rate;     /* Rate in Hz */
    AR_S32 a_bits;     /* bits per audio sample */

    AR_S32 ch_num;
    AR_S32 mode;
    AR_S32 size_m;
    AR_S32 duration; // min
} AviInitParam;

#ifndef HAVE_AVIMAINHEADER
#define HAVE_AVIMAINHEADER
typedef struct tagAviMainHeader {
    AR_S32  fcc;
    AR_S32  cb;
    AR_S32  dwMicroSecPerFrame;
    AR_S32  dwMaxBytesPerSec;
    AR_S32  dwPaddingGranularity;
    AR_S32  dwFlags;
    AR_S32  dwTotalFrames;
    AR_S32  dwInitialFrames;
    AR_S32  dwStreams;
    AR_S32  dwSuggestedBufferSize;
    AR_S32  dwWidth;
    AR_S32  dwHeight;
    AR_S32  dwReserved[4];
} AviMainHeader;
#endif

#ifndef HAVE_AVISTREAMHEADER
#define HAVE_AVISTREAMHEADER
typedef struct tagAviStreamHeader {
     AR_S32  fcc;
     AR_S32  cb;
     AR_S32  fccType;
     AR_S32  fccHandler;
     AR_S32  dwFlags;
     AR_S16   wPriority;
     AR_S16   wLanguage;
     AR_S32  dwInitialFrames;
     AR_S32  dwScale;
     AR_S32  dwRate;
     AR_S32  dwStart;
     AR_S32  dwLength;
     AR_S32  dwSuggestedBufferSize;
     AR_S32  dwQuality;
     AR_S32  dwSampleSize;
     struct {
         AR_S16 left;
         AR_S16 top;
         AR_S16 right;
         AR_S16 bottom;
     }  rcFrame;
} AviStreamHeader;
#endif

#ifndef HAVE_BITMAPINFOHEADER
#define HAVE_BITMAPINFOHEADER
typedef struct tagBitMapInfoHeader{
  AR_S32   biSize;
  AR_S32   biWidth;
  AR_S32   biHeight;
  AR_S16   biPlanes;
  AR_S16   biBitCount;
  AR_S32   biCompression;
  AR_S32   biSizeImage;
  AR_S32   biXPelsPerMeter;
  AR_S32   biYPelsPerMeter;
  AR_S32   biClrUsed;
  AR_S32   biClrImportant;
} BitMapInfoHeader;
#endif

#ifndef HAVE_RGBQUAD
#define HAVE_RGBQUAD
typedef struct tagRGBQuad {
  AR_UCHAR    rgbBlue;
  AR_UCHAR    rgbGreen;
  AR_UCHAR    rgbRed;
  AR_UCHAR    rgbReserved;
} RGBQuad ;
#endif

#ifndef HAVE_BITMAPINFO
#define HAVE_BITMAPINFO
typedef struct tagBitmapInfo {
  BitMapInfoHeader bmiHeader;
  //RGBQuad          bmiColors[1];
} BitmapInfo;
#endif

#ifndef HAVE_WAVEFORMATEX
#define HAVE_WAVEFORMATEX
typedef struct tagWaveFormateX{
  AR_S16  wFormatTag;
  AR_S16  nChannels;
  AR_S32 nSamplesPerSec;
  AR_S32 nAvgBytesPerSec;
  AR_S16  nBlockAlign;
  AR_S16  wBitsPerSample;
  AR_S16  cbSize;
} WaveFormateX;
#endif

#ifndef HAVE_AUDIOFORMATEX
#define HAVE_AUDIOFORMATEX
typedef struct tagAudioFormateX{
  AR_S16  wFormatTag;
  AR_S16  nChannels;
  AR_S32 nSamplesPerSec;
  AR_S32 nAvgBytesPerSec;
  AR_S16  nBlockAlign;
  AR_S16  wBitsPerSample;
  //AR_S16  cbSize;
} AudioFormateX;
#endif


#ifndef HAVE_AVIINDEX
#define HAVE_AVIINDEX
typedef struct tagAviIndex {
  AR_S32   dwChunkId;
  AR_S32   dwFlags;
  AR_S32   dwOffset;
  AR_S32   dwSize;
} AviIndex;
#endif

typedef union AviStreamFormatTag {
    BitmapInfo    video_format;
    WaveFormateX  audio_format;
}AviStreamFormat;

#define ONE_FRAME_BUF_SIZE 700000

typedef struct tagAviFrameNode {
    AR_S32 frame_type; // 0: audio; 1:video
    AR_S32 offset;
    AR_S32 size;       //帧大小
    AR_U32 timetick;  //帧时间信息
    AR_CHAR *one_frame_buf;  //帧数据
} AviFrameNode;

typedef struct tagFILE_NODE
{
    AR_CHAR path[128];
    AR_U64 start;
    AR_U64 stop;
    AR_U64 size;
    struct tagFILE_NODE *next;
} FILE_NODE;

typedef struct tagFILE_LIST
{
    AR_U64 start;
    AR_U64 stop;
    AR_U32 type;
    AR_U32 ch_num;

    FILE_NODE *head;
    AR_U32 len;
    AR_U32 index;
}FILE_LIST;


typedef struct tagAviPBHandle {
    AviFrameNode node;
    FILE_LIST *list;
    
    AR_S32 no;
    AR_S32 video_no;
    AR_S32 audio_no;
    
    AR_S32 a_enc_type;
    AR_S32 v_enc_type;
    AR_S32 gop;
    AR_S32 fps;
    AR_S32 bps;
    AR_S32 video_width;
    AR_S32 video_height;
    
    FILE    *file;
    AR_CHAR   file_path[128];
    
    AR_S32    video_count;
    
    //index
    AR_S32    index_count;   //real index count
    AR_S32    *idx_array;
    AR_S32    idx_array_count;
    
    //buffer for playback
    AR_CHAR *pb_buf;
    AR_S32 pb_buf_size;
    AR_S32 pb_buf_index;
    AR_S32 pb_buf_pos;
    AR_U32 pb_timetick_tmp;
} AviPBHandle;

typedef struct tagAviHeader {
    AR_S32 fcc_riff;  //RIFF
    AR_S32 avi_len;
    AR_S32 fcc_avi;  //AVI

    AR_S32 fcc_list_hdrl;  //LIST
    AR_S32 hdrl_len;
    AR_S32 fcc_hdrl;  //hdrl

    AviMainHeader main_header;

    AR_S32 fcc_list_strl_video;  //LIST
    AR_S32 size_strl_video;
    AR_S32 fcc_strl_video;  //strl
    AviStreamHeader video_header;
    AR_S32 fcc_strf_video;  //strf
    AR_S32 size_video_format;
    //AviStreamFormat video_format;
    BitMapInfoHeader bmiHeader;

    AR_S32 fcc_list_strl_audio;  //LIST
    AR_S32 size_strl_audio;
    AR_S32 fcc_strl_audio;  //strl
    AviStreamHeader audio_header;
    AR_S32 fcc_strf_audio;  //strf
    AR_S32 size_audio_format;
    AudioFormateX audio_format;

    AR_S32 fcc_list_movi;
    AR_S32 movi_len;
    AR_S32 fcc_movi;
} AviHeader;

void AR_AVI_CalcStopTime(AR_CHAR *time_stop, AR_CHAR *file_path, AR_S32 video_count, AR_S32 fps);
void AR_AVI_ClearWriteBuff(AviFile *avi_file);
AR_S32 AR_AVI_WriteHeader(AviFile *avi_file);
AR_S32 AR_AVI_AddIndex(AviFile *avi_file);
AR_S32 AR_AVI_WriteClose(AviFile *avi_file);
AR_S32 AR_AVI_WriteOpen(AviFile *handle);
AR_S32 AR_AVI_WriteFrame(AR_CHAR *frame, AR_S32 length, AR_S32 intra, AviFile *avi_file, AR_U32 time_ms);
AR_S32 AR_AVI_FindLastIndex(AR_CHAR *str, AR_CHAR c);

AR_S32 AR_AVI_ReadGetFrame(AviPBHandle *pPBHandle);
AviPBHandle *AR_AVI_ReadPbHandleCreate(char *filename);
AR_S32 AR_AVI_ReadPbHandleDestory(AviPBHandle *pPBHandle);

#ifdef __cplusplus
}
#endif

#endif /* __GRD_AVI_TYPEDEF_H__ */

