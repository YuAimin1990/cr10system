#ifndef __AR_MOV_DEF_H__
#define __AR_MOV_DEF_H__

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
#include <math.h>

#define MAX_SPATIAL_SEGMENTATION 4096 // max. value of u(12) field
#define AV_INPUT_BUFFER_PADDING_SIZE 64

#define FFMIN(a,b) ((a) > (b) ? (b) : (a))
#define FFMAX(a,b) ((a) > (b) ? (a) : (b))
#define FFABS(a) ((a) >= 0 ? (a) : (-(a)))


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
typedef long long  AR_S64;
#endif


#define MAKE_FCC(a,b,c,d)         (AR_U32)((d)|(c)<<8|(b)<<16|(a)<<24)

typedef struct MovFileTag {
    FILE    *file;
    AR_CHAR   filename[128];

    AR_S32    mdat_offset; // = 0x1b
    AR_S32    mdat_size;
    AR_S32    moov_offset; // = mdat_offset + mdat_size
    AR_S32    moov_size;
    AR_S32    file_size; // = moov_offset + moov_size

    AR_S32    mvhd_offset;
    AR_S32    mvhd_size;

    AR_S32    video_count;
    AR_S32    audio_enable;
    AR_S32    audio_bytes;
    AR_S32    samplesize;
    AR_S32    a_enc_type; // 0 a-law; 1 u-law; 2 pcm

    //index
    AR_S32    index_count;   //real index count
    AR_S32    *idx_array;
    AR_S32    idx_array_count;
}MovFile;

typedef struct tagMovFrameNode {
    AR_S32 frame_type; 
    AR_S32 size;       //帧大小
    AR_CHAR *one_frame_buf;  //帧数据
} MovFrameNode;

typedef struct tagMovHeaderInfo{
    AR_U32 ftyp_size;
    AR_U32 wide_size;
    AR_U32 mdat_offset;
    AR_U32 mdat_size;
    AR_U32 moov_offset;
    AR_U32 moov_size;
    AR_U32 minf_offset;
    AR_U32 minf_size;
    AR_U32 stbl_offset;
    AR_U32 stbl_size;
    AR_U32 stsd_offset;
    AR_U32 stsd_size;
    AR_U32 stts_offset;
    AR_U32 stts_size;
    AR_U32 stss_offset;
    AR_U32 stss_size;
    AR_U32 stsc_offset;
    AR_U32 stsc_size;
    AR_U32 stsz_offset;
    AR_U32 stsz_size;
    AR_U32 stco_offset;
    AR_U32 stco_size;
} MovHeaderInfo;

typedef struct tagFrameInfo {
    AR_U32  no; 
    AR_U32  type;
    AR_U32  offset;
    AR_U32  size;
} FrameInfo;

typedef struct tagMovPBHandle {
    FILE    *file;
    
    MovFrameNode node;
    MovFrameNode a_node;
    AR_S32 no;
    AR_S32 video_no;
    AR_S32 audio_no;
    
    AR_S32 v_enc_type;//0:H265 1:H264
    AR_S32 gop;
    AR_S32 fps;
    AR_S32 video_width;
    AR_S32 video_height;
    
    AR_S32 audio_enable;
    AR_S32 a_enc_type;
    AR_S32 a_rate;
    AR_S32 a_channels;
    AR_S32 a_bits;
    
    FrameInfo *idx_array;
    AR_U32 idx_array_count;
    AR_U32 index_count;
    
    FrameInfo *a_idx_array;
    AR_U32 a_idx_array_count;
    AR_U32 a_idx_index;
    
    AR_CHAR *pb_buf;
    AR_S32 pb_buf_size;
    
    AR_CHAR *a_pb_buf;
    AR_S32 a_pb_buf_size;
    
    MovHeaderInfo pMovHead;
} MovPBHandle;

typedef struct tagMovFtypAtom {
    AR_U32  ftyp_size; //20
    AR_U32  type; //ftyp
    AR_U32  major_brand;
    AR_U32  minor_version;
    AR_U32  compatible_brands;
} MovFtypAtom;

typedef struct tagMovAtomHeader {
    AR_U32  size;
    AR_U32  type;
} MovAtomHeader, MovWideAtom, MovMdatAtom, MovEdtsAtom,
MovMoovAtom, MovTrakAtom, MovMdiaAtom, MovMinfAtom,
MovStblAtom;

typedef union {
    struct {
        AR_U32  version:8;
        AR_U32  flags:24;
    } v_f_s;
    AR_U8 v_f_c[4];
} U_VersionFlags;


typedef struct tagMovMvhdAtom {
    AR_U32  mvhd_size;
    AR_U32  type; //mvhd
	union {
		struct {
            AR_U32  version:8; // 0
            AR_U32  flags:24;  // 0
        } v_f_s;
		AR_U8 v_f_c[4];
	};

    AR_S32   creation_time;
    AR_S32   modification_time;
    AR_U32  time_scale;
    AR_U32  duration;
    AR_U32  rate;
    AR_U16  volume;
    AR_U8   reserved[10];
    AR_U8   matrix_struct[36];
    AR_U32  preview_time;
    AR_U32  preview_duration;
    AR_U32  poster_time;
    AR_U32  select_time;
    AR_U32  select_duration;
    AR_U32  current_time;
    AR_U32  next_track_id;
} MovMvhdAtom;


typedef struct tagMovTkhdAtom {
    AR_U32  tkhd_size;
    AR_U32  type; //tkhd

	union
    {
		struct
        {
            AR_U32  version:8; // 0
            AR_U32  flags:24;  // 0x 00 00 0f
                             //0x000001 track_enabled
                             //0x000002 track_in_movie
                             //0x000004 track_in_preview

		} v_f_s;
		AR_U8 v_f_c[4];
	};

    AR_S32   creation_time;
    AR_S32   modification_time;
    AR_U32  track_id;
    AR_U32  res1;
    AR_U32  duration;
    AR_U8   res2[8];
    AR_U16  layer;
    AR_U16  alternate_group;
    AR_U16  volume; //	[8.8]
    AR_U16  res3;
    AR_U8   matrix_struct[36];
    AR_U32  track_width;  // 1280=0x05000000
    AR_U32  track_height; // 720=0x02d00000
} MovTkhdAtom;

typedef struct tagMovElstSample {
    AR_U32  track_duration;
    AR_U32  media_time;
    AR_U32  media_rate;
} MovElstSample;


typedef struct tagMovElstAtom {
    AR_U32  size;
    AR_U32  type;
	union {
		struct {
            AR_U32  version:8; // 0
            AR_U32  flags:24;  // 0
        } v_f_s;
		AR_U8 v_f_c[4];
	};

    AR_U32  counts;
    MovElstSample *tables;
} MovElstAtom;


typedef struct tagMovMdhdAtom {
    AR_U32  mdhd_size;
    AR_U32  type; //mdhd
	union {
		struct {
            AR_U32  version:8; // 0
            AR_U32  flags:24;  // 0
        } v_f_s;
		AR_U8 v_f_c[4];
	};

    AR_S32   creation_time;
    AR_S32   modification_time;
    AR_U32  time_scale;
    AR_U32  duration;
    AR_U16  language;
    AR_U16  quality;
} MovMdhdAtom;

//hdlr
typedef struct tagMovHdlrAtom {
    AR_U32  hdlr_size;  //
    AR_U32  type; //hdlr
	union {
		struct {
            AR_U32  version:8; // 0
            AR_U32  flags:24;  // 0
        } v_f_s;
		AR_U8 v_f_c[4];
	};

    AR_U32  component_type; //mhlr for media handlers, dhlr for data handlers

    AR_U32  component_subtype;
    AR_U32  component_manufacturer;
    AR_U32  component_flags;
    AR_U32  component_flags_mask;
} MovHdlrAtom;


typedef struct tagMovVmhdAtom {
    AR_U32  vmhd_size;  //
    AR_U32  type; //hdlr
	union {
		struct {
            AR_U32  version:8; // 0
            AR_U32  flags:24;  // always set this flag to 1
        } v_f_s;
		AR_U8 v_f_c[4];
	};
    AR_U16  graphics_mode;
    AR_U16  opcolor_red;
    AR_U16  opcolor_green;
    AR_U16  opcolor_blue;
} MovVmhdAtom;

typedef struct tagMovUrlAtom {
    AR_U32  url_size; // 8
    AR_U32  type; //wide
	union {
		struct {
            AR_U32  version:8;
            AR_U32  flags:24;
        } v_f_s;
		AR_U8 v_f_c[4];
	};
} MovUrlAtom;

typedef struct tagMovDrefAtom {
    AR_U32  dref_size; // 8
    AR_U32  type; //wide
	union {
		struct {
            AR_U32  version:8;
            AR_U32  flags:24;
        } v_f_s;
		AR_U8 v_f_c[4];
	};
    AR_U32  entry_count; // url_atom
    MovUrlAtom url_atom;
} MovDrefAtom;

typedef struct tagMovDinfAtom {
    AR_U32  dinf_size; // 8
    AR_U32  type; //wide
    MovDrefAtom dref_atom;
} MovDinfAtom;


typedef struct tagMovStsdSample {
    AR_U32  description_size;
    AR_U32  data_format;
    AR_U8   res[6];
    AR_U16  data_reference_index; //0x 00 01
    AR_U16  version; // 0
    AR_U16  revision_level; // must be 0
    AR_U32  vendor; //FFMP
    AR_U32  temporal_qulity; //0X 00 00 02 00
    AR_U32  spatial_qulity;  //0X 00 00 02 00
    AR_U16  width;
    AR_U16  height;
    AR_U32  h_resolution; //0X 00 48 00 00
    AR_U32  v_resolution; //0X 00 48 00 00
    AR_U32  data_size; //must be 0
    AR_U16  frame_count; // frame count in each sample, usually set to 1
    AR_U16  res1;
    AR_U8   res2[24];
    AR_U8   data[50];
} MovStsdSample;


typedef struct tagMovSttsSample {
    AR_U32  count;
    AR_U32  duration;
} MovSttsSample;

typedef struct tagMovSttsAtom {
    AR_U32  size; // 8
    AR_U32  type; //wide
	union {
		struct {
            AR_U32  version:8;
            AR_U32  flags:24;
        } v_f_s;
		AR_U8 v_f_c[4];
	};
    AR_U32 counts;
    MovSttsSample *tables;
} MovSttsAtom;

typedef struct tagMovStssSample {
    AR_U32  number;
} MovStssSample;


typedef struct tagMovStssAtom {
    AR_U32  size; // 8
    AR_U32  type; //wide
	union {
		struct {
            AR_U32  version:8;
            AR_U32  flags:24;
        } v_f_s;
		AR_U8 v_f_c[4];
	};
    AR_U32 counts;
    MovStssSample *tables;
} MovStssAtom;

typedef struct tagMovStscSample {
    AR_U32  first_chunk;
    AR_U32  samples_per_chunk;
    AR_U32  samples_description_id;
} MovStscSample;

typedef struct tagMovStscAtom {
    AR_U32  size; // 8
    AR_U32  type; //wide
	union {
		struct {
            AR_U32  version:8;
            AR_U32  flags:24;
        } v_f_s;
		AR_U8 v_f_c[4];
	};
    AR_U32 counts;
    MovStscSample *tables;
} MovStscAtom;

typedef struct tagMovStszSample {
    AR_U32  size;
} MovStszSample;

typedef struct tagMovStszAtom {
    AR_U32  size; // 8
    AR_U32  type; //wide
	union {
		struct {
            AR_U32  version:8;
            AR_U32  flags:24;
        } v_f_s;
		AR_U8 v_f_c[4];
	};
    AR_U32 sample_size;
    AR_U32 counts;
    MovStszSample *tables;
} MovStszAtom;

typedef struct tagMovStcoSample {
    AR_U32  offset;
} MovStcoSample;

typedef struct tagMovStcoAtom {
    AR_U32  size; // 8
    AR_U32  type; //wide
	union {
		struct {
            AR_U32  version:8;
            AR_U32  flags:24;
        } v_f_s;
		AR_U8 v_f_c[4];
	};
    AR_U32 counts;
    MovStcoSample *tables;
} MovStcoAtom;


typedef enum {
    STBL_STSD = 0,
    STBL_STTS,
    STBL_STSS,
    STBL_STSC,
    STBL_STSZ,
    STBL_STCO,
} STBL_TYPE;

typedef enum {
    MOV_FRAME_TYPE_I = 0,
    MOV_FRAME_TYPE_P,
    MOV_FRAME_TYPE_IDR,
    MOV_FRAME_TYPE_A,
} MOV_FRAME_TYPE;

typedef enum {
    MOV_H264_TYPE = 0,
    MOV_H265_TYPE,
    MOV_NULL_TYPE,
} MOV_CODEC_TYPE;

typedef struct tagChunkInfo {
    AR_U32  offset;
    AR_U32  size;
    AR_U32  samples_count_in_chunk;
} ChunkInfo;


typedef struct tagH264ReadHandle {
    FILE *fp;
    AR_CHAR file_path[1024];
    AR_U32 file_size;

    AR_U8 pps[1024];
    AR_U8 sps[1024];
    AR_U32 pps_len;
    AR_U32 sps_len;


    FrameInfo *idx_array;
    AR_U32 idx_array_count;
    AR_U32 index_count;
} H264ReadHandle;

typedef struct HVCCProfileTierLevel {
    AR_U8  	profile_space;
    AR_U8  	tier_flag;
    AR_U8  	profile_idc;
    AR_U32 	profile_compatibility_flags;
    AR_U64 	constraint_indicator_flags;
    AR_U8  	level_idc;
} HVCCProfileTierLevel;

typedef struct HVCCNALUnitArray {
    AR_U8  	array_completeness;
    AR_U8  	NAL_unit_type;
    AR_U16 	numNalus;
    AR_U16 	*nalUnitLength;
    AR_U8  	**nalUnit;
} HVCCNALUnitArray;

typedef struct HEVCDecoderConfigurationRecord {
    AR_U8  configurationVersion;
    AR_U8  general_profile_space;
    AR_U8  general_tier_flag;
    AR_U8  general_profile_idc;
    AR_U32 general_profile_compatibility_flags;
    AR_U64 general_constraint_indicator_flags;
    AR_U8  general_level_idc;
    AR_U16 min_spatial_segmentation_idc;
    AR_U8  parallelismType;
    AR_U8  chromaFormat;
    AR_U8  bitDepthLumaMinus8;
    AR_U8  bitDepthChromaMinus8;
    AR_U16 avgFrameRate;
    AR_U8  constantFrameRate;
    AR_U8  numTemporalLayers;
    AR_U8  temporalIdNested;
    AR_U8  lengthSizeMinusOne;
    AR_U8  numOfArrays;
    HVCCNALUnitArray *array;
} HEVCDecoderConfigurationRecord;

typedef enum
{
	HEVC_NAL_NULL = 0,
	HEVC_NAL_VPS = 32,
	HEVC_NAL_SPS = 33,
	HEVC_NAL_PPS = 34,
} HEVC_NAL_TYPE;


typedef struct tagMovWriteHandle {
    FILE *fp;
    AR_CHAR file_path[1024];

    AR_U32 codec_type;

    AR_U32 mdat_offset;
    AR_U32 frame_offset;
    AR_U32 file_size;

    AR_U32 mdat_size;
    AR_U32 moov_size;

    AR_U32 video_count;
    AR_U32 fps;
    AR_U32 gop;
    AR_U32 time_scale;
    AR_U32 time_duration;
    AR_U32 rate;
    AR_U16 volume;
    AR_U16 width;
    AR_U16 height;

    AR_U8 pps[1024];
    AR_U8 sps[1024];
    AR_U8 vps[1024];	//h.265
    AR_U32 pps_len;
    AR_U32 sps_len;
    AR_U32 vps_len;	//h.265
    //AR_U8 sps_pps[1024];
    //AR_U32 sps_pps_len;
    AR_U8 vps_sps_pps[1024];
    AR_U32 vps_sps_pps_len;
    HEVCDecoderConfigurationRecord hvcc;

    AR_S32 audio_enable;
	AR_S32 a_enc_type; // 0 a-law; 1 u-law; 2- pcm
    AR_S32 a_count;
    AR_S32 a_bytes;
    AR_S32 a_rate; // 8000
    AR_S32 a_chans; // 1
    AR_S32 a_bits;  // 16

    FrameInfo *idx_array;
    AR_U32 idx_array_count;
    AR_U32 index_count;

    FrameInfo *a_idx_array;
    AR_U32 a_idx_array_count;
    AR_U32 a_idx_index;
    AR_CHAR * buffer;
    AR_S32    buffer_pos;
    AR_S32    buffer_frame_cnt;
} MovWriteHandle;


typedef struct tagMovWriteInitParam {
    AR_U32 fps;
    AR_U32 gop;
    AR_U32 time_scale;
    AR_U32 rate;
    AR_U16 volume;
    AR_U16 width;
    AR_U16 height;

    AR_S32 audio_enable;
    AR_S32 a_enc_type; // 0 a-law; 1 u-law; 2- pcm
    AR_S32 a_count;
    AR_S32 a_bytes;
    AR_S32 a_rate; // 8000
    AR_S32 a_chans; // 1
    AR_S32 a_bits;  // 16
} MovWriteInitParam;

typedef struct tagMovSmhdAtom {
    AR_U32  size;  //
    AR_U32  type; //smhd
	union {
		struct {
            AR_U32  version:8; // 0
            AR_U32  flags:24;  // always set this flag to 1
        } v_f_s;
		AR_U8 v_f_c[4];
	};
    AR_U16  balance;
    AR_U16  res;
} MovSmhdAtom;

typedef struct tagMovStsdSounSample {
    AR_U32  description_size; //0x 3c
    AR_U32  data_format; // sowt
    AR_U8   res[6];
    AR_U16  data_reference_index; //0x 00 01

    AR_U16  version; // must be set to 0 or 1
    AR_U16  revision_level; // must be set to 0
    AR_U32  vendor; // must be set to 0
    AR_U16  number_of_channels;
    AR_U16  sample_size; //8: 8bit   16:16bit
    AR_U16  compression_id; // must be set to 0 (or, for version 1, may be -2)
    AR_U16  packet_size; // must be set to 0
    AR_U16  sample_rate; // 8000
    AR_U16  res2;
} MovStsdSounSample;

typedef struct tagMovStsdAtom {
    AR_U32  size; // 8
    AR_U32  type; //wide
	union {
		struct {
            AR_U32  version:8;
            AR_U32  flags:24;
        } v_f_s;
		AR_U8 v_f_c[4];
	};
    AR_U32 counts;
    MovStsdSample *tables;
    MovStsdSounSample *s_tables;
} MovStsdAtom;

#define REC_BUF_SIZE (1024*1024*3)


#ifdef __cplusplus
}
#endif

#endif /* __Z_MOV_H__ */


