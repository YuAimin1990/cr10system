#include <errno.h>

#include "ar_mov_def.h"
#include "ar_mov_utility.h"
#include "ar_mov_api.h"

//#include "avi_read.h"

#define WRITE_ONCE_NUM 15
#define WRITE_EDTS 1
#define SIGNAL_TEST 0

AR_U8 matrix_struct[36] = {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,\
                         0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,\
                         0, 0, 0, 0, 0, 0, 0, 0, 64, 0, 0, 0};

AR_U8 stsd_data[10] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0xff, 0xff};

AR_U8 stsd_a_data[20] = {0x63, 0x68, 0x61, 0x6e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x64,
                       0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

AR_U8 udta_data[25] = {0x00, 0x00, 0x00, 0x19, 0xa9, 0x73, 0x77, 0x72, 0x00, 0x0d,
                     0x55, 0xc4, 0x4c, 0x61, 0x76, 0x66, 0x35, 0x36, 0x2e, 0x34,
                     0x30, 0x2e, 0x31, 0x30, 0x31};

#if 0
AR_U8 mhlr_name[] = "VideoHandler";
AR_U8 dhlr_name[] = "DataHandler";
AR_U8 mhlr_a_name[] = "SoundHandler";
#else
char mhlr_name[] = "VideoHandler";
char dhlr_name[] = "DataHandler";
char mhlr_a_name[] = "SoundHandler";
#endif

int AR_MOV_WriteHead(MovWriteHandle *w_handle)
{
    if (!w_handle)
    {
        printf("parameter error.\n");
        return -1;
    }

    int ret = 0;
    char buf[1000] = {0};
    int offset = 0;

    //ftyp
    hton_set_u32(buf + offset, 20);
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('f','t','y','p'));
    offset += 4;
    hton_set_u32(buf + offset, 0x71742020);
    offset += 4;
    hton_set_u32(buf + offset, 0x00000200);
    offset += 4;
    hton_set_u32(buf + offset, 0x71742020);
    offset += 4;

    //wide
    hton_set_u32(buf + offset, 8);
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('w','i','d','e'));
    offset += 4;

    //mdat
    w_handle->mdat_offset = offset;
    hton_set_u32(buf + offset, 0);
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('m','d','a','t'));
    offset += 4;

    //frame data
    w_handle->frame_offset = offset;

    /* write data */
    ret = fwrite(buf, 1, offset, w_handle->fp);
    if (ret != offset)
    {
        printf("write file failed, ret %d, err: %s.\n", ret, strerror(errno));
        return -1;
    }

    return 0;
}

static int mov_write_tail_add_v_track(MovWriteHandle *handle, char *buf, int offset)
{
    float coe = 1; // 系数，如 1200, 1, 0.3
    AR_U32 mdhd_time_scale = handle->time_scale * coe;
    AR_U32 mdhd_time_duration = mdhd_time_scale / handle->fps * handle->video_count;

    AR_U32 stts_sample_duration = mdhd_time_scale / handle->fps;
    // stts_sample_counts = mdhd_time_duration / stts_sample_duration
    AR_U32 stts_sample_counts = handle->video_count;

    U_VersionFlags v_f;
    memset(&v_f, 0, sizeof(v_f));

    //trak
    AR_U32 trak_offset = offset;
    hton_set_u32(buf + offset, 0); //size
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('t','r','a','k'));
    offset += 4;

    //tkhd
    hton_set_u32(buf + offset, 92); //size
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('t','k','h','d'));
    offset += 4;
    v_f.v_f_s.version = 0;
    v_f.v_f_c[3] = 3;
    memcpy(buf + offset, &v_f, 4);
    offset += 4;
    hton_set_u32(buf + offset, 0); //creation_time
    offset += 4;
    hton_set_u32(buf + offset, 0); //modification_time
    offset += 4;
    hton_set_u32(buf + offset, 1); //track_id
    offset += 4;
    offset += 4; //res1
    hton_set_u32(buf + offset, handle->time_duration); //duration
    offset += 4;
    offset += 8; //res2
    hton_set_u16(buf + offset, 0); //layer
    offset += 2;
    hton_set_u16(buf + offset, 0); //alternate_group
    offset += 2;
    hton_set_u16(buf + offset, 0); //volume
    offset += 2;
    offset += 2; //res3
    memcpy(buf + offset, matrix_struct, 36);
    offset += 36;
    hton_set_u16(buf + offset, handle->width); //track_width
    offset += 2;
    offset += 2;
    hton_set_u16(buf + offset, handle->height); //track_height
    offset += 2;
    offset += 2;

#if WRITE_EDTS
    //edts
    AR_U32 edts_offset = offset;
    hton_set_u32(buf + offset, 0); //size 36
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('e','d','t','s'));
    offset += 4;

    //elst
    hton_set_u32(buf + offset, 28); //size
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('e','l','s','t'));
    offset += 4;
    hton_set_u32(buf + offset, 0); //version & flags
    offset += 4;
    hton_set_u32(buf + offset, 1); //counts
    offset += 4;
    hton_set_u32(buf + offset, handle->time_duration); //track_duration
    offset += 4;
    hton_set_u32(buf + offset, 0); //media_time
    offset += 4;
    hton_set_u32(buf + offset, handle->rate); //media_rate
    offset += 4;
    //update edts size
    AR_U32 edts_size = offset - edts_offset;
    hton_set_u32(buf + edts_offset, edts_size);
#endif

    //mdia
    AR_U32 mdia_offset = offset;
    hton_set_u32(buf + offset, 0); //size
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('m','d','i','a'));
    offset += 4;

    //mdhd
    hton_set_u32(buf + offset, 32); //size
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('m','d','h','d'));
    offset += 4;
    hton_set_u32(buf + offset, 0); //version & flags
    offset += 4;
    hton_set_u32(buf + offset, 0); //creation_time
    offset += 4;
    hton_set_u32(buf + offset, 0); //modification_time
    offset += 4;
    hton_set_u32(buf + offset, mdhd_time_scale); //time_scale
    offset += 4;
    hton_set_u32(buf + offset, mdhd_time_duration); //track_duration
    offset += 4;
    hton_set_u16(buf + offset, 0); //language
    offset += 2;
    hton_set_u16(buf + offset, 0); //quality
    offset += 2;

    //hdlr
    hton_set_u32(buf + offset, 45); //size
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('h','d','l','r'));
    offset += 4;
    hton_set_u32(buf + offset, 0); //version & flags
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('m','h','l','r')); //component_type
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('v','i','d','e')); //component_subtype
    offset += 4;
    offset += 12; //component_manufacturer, component_flags, component_flags_mask
    hton_set_u8(buf + offset, (AR_U8)strlen(mhlr_name)); //component_subtype
    offset += 1;
    memcpy(buf + offset, mhlr_name, strlen(mhlr_name));
    offset += strlen(mhlr_name);

    //minf
    AR_U32 minf_offset = offset;
    hton_set_u32(buf + offset, 0); //size
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('m','i','n','f'));
    offset += 4;

    //vmhd
    hton_set_u32(buf + offset, 0x14); //size
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('v','m','h','d'));
    offset += 4;
    v_f.v_f_s.version = 0;
    v_f.v_f_c[3] = 1;
    memcpy(buf + offset, &v_f, 4);
    offset += 4;
    hton_set_u32(buf + offset, 0); //graphics_mode, opcolor_red
    offset += 4;
    hton_set_u32(buf + offset, 0); //opcolor_green, opcolor_blue
    offset += 4;

    //hdlr
    hton_set_u32(buf + offset, 44); //size
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('h','d','l','r'));
    offset += 4;
    hton_set_u32(buf + offset, 0); //version & flags
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('d','h','l','r')); //component_type
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('u','r','l',' ')); //component_subtype
    offset += 4;
    offset += 12; //component_manufacturer, component_flags, component_flags_mask
    hton_set_u8(buf + offset, strlen(dhlr_name)); //component_subtype
    offset += 1;
    memcpy(buf + offset, dhlr_name, strlen(dhlr_name));
    offset += strlen(dhlr_name);

    //dinf
    hton_set_u32(buf + offset, 36); //size
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('d','i','n','f'));
    offset += 4;
    //dref
    hton_set_u32(buf + offset, 28); //size
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('d','r','e','f'));
    offset += 4;
    hton_set_u32(buf + offset, 0); //version & flags
    offset += 4;
    hton_set_u32(buf + offset, 1); //count
    offset += 4;
    //url
    hton_set_u32(buf + offset, 12); //size
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('u','r','l',' '));
    offset += 4;
    v_f.v_f_s.version = 0;
    v_f.v_f_c[3] = 1;
    memcpy(buf + offset, &v_f, 4);
    offset += 4;

    //stbl
    AR_U32 stbl_offset = offset;
    hton_set_u32(buf + offset, 0); //size
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('s','t','b','l'));
    offset += 4;

    //stsd
    AR_U32 stsd_offset = offset;
    hton_set_u32(buf + offset, 0); //size
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('s','t','s','d'));
    offset += 4;
    hton_set_u32(buf + offset, 0); //version & flags
    offset += 4;
    hton_set_u32(buf + offset, 1); //count
    offset += 4;
    //stsd sample
    AR_U32 stsd_sample_offset = offset;
    hton_set_u32(buf + offset, 0); //description_size
    if(handle->codec_type == MOV_H265_TYPE)
    {
	    offset += 4;
	    hton_set_u32(buf + offset, MAKE_FCC('h','v','c','1')); //data_format : h265
    }
    else
    {
	    offset += 4;
	    hton_set_u32(buf + offset, MAKE_FCC('a','v','c','1')); //data_format : h264
    }
    offset += 4;
    offset += 6; //res
    hton_set_u16(buf + offset, 1); //data_reference_index
    offset += 2;
    hton_set_u32(buf + offset, 0); //version, revision_level
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('F','F','M','P')); //vendor
    offset += 4;
    hton_set_u32(buf + offset, 512); //temporal_qulity
    offset += 4;
    hton_set_u32(buf + offset, 512); //spatial_qulity
    offset += 4;
    hton_set_u16(buf + offset, handle->width); //width
    offset += 2;
    hton_set_u16(buf + offset, handle->height); //height
    offset += 2;
    hton_set_u32(buf + offset, 0x480000); //h_resolution
    offset += 4;
    hton_set_u32(buf + offset, 0x480000); //v_resolution
    offset += 4;
    hton_set_u32(buf + offset, 0); //data_size
    offset += 4;
    hton_set_u16(buf + offset, 1); //frame_count(frame count in each sample)
    offset += 2;
    offset += 26; //res
    memcpy(buf + offset, stsd_data, 10);
    offset += 10;

	if(handle->codec_type == MOV_H265_TYPE)
	{
		AR_U32 tmp1 = handle->vps_len + handle->sps_len + handle->pps_len + 46;
		hton_set_u32(buf + offset, tmp1); //sps + pps + vps +?
		offset += 4;

		hton_set_u32(buf + offset, MAKE_FCC('h','v','c','C'));
		offset += 4;

		hton_set_u8(buf + offset, 1);  //configurationVersion
		offset += 1;
		AR_U8 general_offset = handle->hvcc.general_profile_space << 6 | handle->hvcc.general_tier_flag << 5 | handle->hvcc.general_profile_idc;
		hton_set_u8(buf + offset, general_offset); //?? general_profile_space << 6 | general_tier_flag << 5 | general_profile_idc
		offset += 1;
		hton_set_u32(buf + offset, handle->hvcc.general_profile_compatibility_flags); //general_profile_compatibility_flags
		offset += 4;
		hton_set_u32(buf + offset, handle->hvcc.general_profile_compatibility_flags >> 16); //general_constraint_indicator_flags >> 16
		offset += 4;
		hton_set_u16(buf + offset, handle->hvcc.general_constraint_indicator_flags); //general_constraint_indicator_flags
		offset += 2;
		hton_set_u8(buf + offset, handle->hvcc.general_level_idc);  //??general_level_idc
		offset += 1;
		hton_set_u16(buf + offset, handle->hvcc.min_spatial_segmentation_idc | 0xf000); //?? min_spatial_segmentation_idc | 0xf000
		offset += 2;
		hton_set_u8(buf + offset, handle->hvcc.parallelismType | 0xfc);  //??parallelismType | 0xfc
		offset += 1;
		hton_set_u8(buf + offset, handle->hvcc.chromaFormat | 0xfc);  //??chromaFormat | 0xfc
		offset += 1;
		hton_set_u8(buf + offset, handle->hvcc.bitDepthLumaMinus8 | 0xf8);  //??bitDepthLumaMinus8 | 0xf8
		offset += 1;
		hton_set_u8(buf + offset, handle->hvcc.bitDepthChromaMinus8 | 0xf8);  //??bitDepthChromaMinus8 | 0xf8
		offset += 1;
		hton_set_u16(buf + offset, handle->hvcc.avgFrameRate); //avgFrameRate
		offset += 2;
		AR_U8 constant_offset = handle->hvcc.constantFrameRate << 6 | handle->hvcc.numTemporalLayers << 3 | handle->hvcc.temporalIdNested  << 2 | handle->hvcc.lengthSizeMinusOne;
		hton_set_u8(buf + offset, constant_offset);  //??constantFrameRate << 6 | numTemporalLayers << 3 | temporalIdNested  << 2 | lengthSizeMinusOne
		offset += 1;

		hton_set_u8(buf + offset, 3);  //numOfArrays : vps/sps/pps
		offset += 1;
		hton_set_u8(buf + offset, (1 << 7) | (0x20 & 0x3f));  //vps
		offset += 1;
		hton_set_u16(buf + offset, 1);  //vps numNalus
		offset += 2;
		hton_set_u16(buf + offset, handle->vps_len);  //vps nalUnitLength
		offset += 2;
		memcpy(buf + offset, handle->vps, handle->vps_len);
		
		offset += handle->vps_len;
		hton_set_u8(buf + offset, (1 << 7) | (0x21 & 0x3f));  //sps
		offset += 1;
		hton_set_u16(buf + offset, 1);  //sps numNalus
		offset += 2;
		hton_set_u16(buf + offset, handle->sps_len);  //sps nalUnitLength
		offset += 2;
		memcpy(buf + offset, handle->sps, handle->sps_len);
		
		offset += handle->sps_len;
		hton_set_u8(buf + offset, (1 << 7)  | (0x22 & 0x3f));  //pps
		offset += 1;
		hton_set_u16(buf + offset, 1);  //pps numNalus
		offset += 2;
		hton_set_u16(buf + offset, handle->pps_len);  //pps nalUnitLength
		offset += 2;
		memcpy(buf + offset, handle->pps, handle->pps_len);
		offset += handle->pps_len;
	}
	else
	{
		AR_U32 tmp1 = handle->sps_len + handle->pps_len + 19;
		hton_set_u32(buf + offset, tmp1); //sps + pps + ?
		offset += 4;
		//hton_set_u32(buf + offset, 0x61766343);
		//??
		hton_set_u32(buf + offset, MAKE_FCC('a','v','c','C'));
		offset += 4;
		hton_set_u8(buf + offset, 1);
		offset += 1;
		hton_set_u8(buf + offset, handle->sps[1]);
		offset += 1;
		hton_set_u32(buf + offset, 0x001fffe1);
		offset += 4;
		//sps + pps
		hton_set_u16(buf + offset, handle->sps_len);
		offset += 2;
		memcpy(buf + offset, handle->sps, handle->sps_len);
		offset += handle->sps_len;
		hton_set_u8(buf + offset, 1);
		offset += 1;
		hton_set_u16(buf + offset, handle->pps_len);
		offset += 2;
		memcpy(buf + offset, handle->pps, handle->pps_len);
		offset += handle->pps_len;	
	}
	//update stsd size
	AR_U32 stsd_sample_size = offset - stsd_sample_offset;
	hton_set_u32(buf + stsd_sample_offset, stsd_sample_size);
	AR_U32 stsd_size = offset - stsd_offset;
	hton_set_u32(buf + stsd_offset, stsd_size);
	
    //stts
    hton_set_u32(buf + offset, 24); //size
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('s','t','t','s'));
    offset += 4;
    hton_set_u32(buf + offset, 0); //version & flags
    offset += 4;
    hton_set_u32(buf + offset, 1); //count
    offset += 4;
    //stts sample
    hton_set_u32(buf + offset, stts_sample_counts); //count
    offset += 4;
    hton_set_u32(buf + offset, stts_sample_duration); //duration
    offset += 4;

    //stss
    AR_U32 stss_sample_counts = round((handle->video_count - 1) / handle->gop);
    AR_U32 stss_size = 16 + stss_sample_counts * 4;
    hton_set_u32(buf + offset, stss_size); //size
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('s','t','s','s'));
    offset += 4;
    hton_set_u32(buf + offset, 0); //version & flags
    offset += 4;
    hton_set_u32(buf + offset, stss_sample_counts); //count
    offset += 4;
    //stss sample
    int i;
    for (i = 0; i < stss_sample_counts; i ++)
    {
        hton_set_u32(buf + offset, i * handle->gop + 1); //number
        offset += 4;
    }

    //stsc
    //AR_U32 stsc_samples_per_chunk = 1;
    AR_U32 stsc_count = 1;
    AR_U32 stsc_size = 16 + stsc_count * 12;
    hton_set_u32(buf + offset, stsc_size); //size
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('s','t','s','c'));
    offset += 4;
    hton_set_u32(buf + offset, 0); //version & flags
    offset += 4;
    hton_set_u32(buf + offset, stsc_count); //count
    offset += 4;
    //stsc sample
    for (i = 0; i < stsc_count; i ++)
    {
        hton_set_u32(buf + offset, i + 1); //first_chunk
        offset += 4;
        hton_set_u32(buf + offset, 1); //samples_per_chunk
        offset += 4;
        hton_set_u32(buf + offset, 1); //samples_description_id
        offset += 4;
    }

    //stsz
    AR_U32 stsz_size = 20 + handle->video_count * 4;
    hton_set_u32(buf + offset, stsz_size); //size
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('s','t','s','z'));
    offset += 4;
    hton_set_u32(buf + offset, 0); //version & flags
    offset += 4;
    hton_set_u32(buf + offset, 0); //设置成0，则每个sample大小都不一样
    offset += 4;
    hton_set_u32(buf + offset, handle->video_count); //count
    offset += 4;
    //stsz sample
    for (i = 0; i < handle->video_count; i ++)
    {
        hton_set_u32(buf + offset, handle->idx_array[i].size); //size
        offset += 4;
    }

    //stco
    //AR_U32 stco_count = (handle->video_count - 1) / stsc_samples_per_chunk + 1;
    AR_U32 stco_count = handle->video_count;
    AR_U32 stco_size = 16 + stco_count * 4;
    hton_set_u32(buf + offset, stco_size); //size
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('s','t','c','o'));
    offset += 4;
    hton_set_u32(buf + offset, 0); //version & flags
    offset += 4;
    hton_set_u32(buf + offset, stco_count); //count
    offset += 4;
    //stco sample
    for (i = 0; i < stco_count; i ++)
    {
        hton_set_u32(buf + offset, handle->idx_array[i].offset); //offset
        offset += 4;
    }

    AR_U32 track_end_offset = offset;

    //trak_size
    AR_U32 trak_size = track_end_offset - trak_offset;
    hton_set_u32(buf + trak_offset, trak_size);

    //mdia_size
    AR_U32 mdia_size = track_end_offset - mdia_offset;
    hton_set_u32(buf + mdia_offset, mdia_size);

    //minf_size
    AR_U32 minf_size = track_end_offset - minf_offset;
    hton_set_u32(buf + minf_offset, minf_size);

    //stbl_size
    AR_U32 stbl_size = track_end_offset - stbl_offset;
    hton_set_u32(buf + stbl_offset, stbl_size);

    return offset;
}

static int mov_write_tail_add_a_track(MovWriteHandle *handle, char *buf, int offset)
{
    printf("a_count:%d a_bytes:%d\n", handle->a_count, handle->a_bytes);

    //AR_U32 a_frame_len = 320;
    AR_U32 cnt_per_sec = 50; // 16000 / 320 = 50

    //类似 handle->time_duration 的算法
    AR_U32 tkhd_duration = handle->a_count * handle->time_scale / cnt_per_sec;

    AR_U32 mdhd_time_scale = 8000;
    AR_U32 mdhd_time_duration = handle->a_count * mdhd_time_scale / cnt_per_sec ;

    AR_U32 stts_sample_duration = 1;
    AR_U32  stts_sample_counts = mdhd_time_duration / stts_sample_duration;

    U_VersionFlags v_f;
    memset(&v_f, 0, sizeof(v_f));

    //trak
    AR_U32 trak_offset = offset;
    hton_set_u32(buf + offset, 0); //size
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('t','r','a','k'));
    offset += 4;

    //tkhd
    hton_set_u32(buf + offset, 92); //size
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('t','k','h','d'));
    offset += 4;
    v_f.v_f_s.version = 0;
    v_f.v_f_c[3] = 3;
    memcpy(buf + offset, &v_f, 4);
    offset += 4;
    hton_set_u32(buf + offset, 0); //creation_time
    offset += 4;
    hton_set_u32(buf + offset, 0); //modification_time
    offset += 4;
    hton_set_u32(buf + offset, 2); //track_id
    offset += 4;
    offset += 4; //res1
    hton_set_u32(buf + offset, tkhd_duration); //duration
    offset += 4;
    offset += 8; //res2
    hton_set_u16(buf + offset, 0); //layer
    offset += 2;
    hton_set_u16(buf + offset, 1); //alternate_group
    offset += 2;
    hton_set_u16(buf + offset, handle->volume); //volume
    offset += 2;
    offset += 2; //res3
    memcpy(buf + offset, matrix_struct, 36);
    offset += 36;
    hton_set_u16(buf + offset, 0); //track_width
    offset += 2;
    offset += 2;
    hton_set_u16(buf + offset, 0); //track_height
    offset += 2;
    offset += 2;

#if WRITE_EDTS
    //edts
    AR_U32 edts_offset = offset;
    hton_set_u32(buf + offset, 0); //size 36
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('e','d','t','s'));
    offset += 4;

    //elst
    hton_set_u32(buf + offset, 28); //size
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('e','l','s','t'));
    offset += 4;
    hton_set_u32(buf + offset, 0); //version & flags
    offset += 4;
    hton_set_u32(buf + offset, 1); //counts
    offset += 4;
    hton_set_u32(buf + offset, tkhd_duration); //track_duration
    offset += 4;
    hton_set_u32(buf + offset, 0); //media_time
    offset += 4;
    hton_set_u32(buf + offset, handle->rate); //media_rate
    offset += 4;
    //update edts size
    AR_U32 edts_size = offset - edts_offset;
    hton_set_u32(buf + edts_offset, edts_size);
#endif

    //mdia
    AR_U32 mdia_offset = offset;
    hton_set_u32(buf + offset, 0); //size
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('m','d','i','a'));
    offset += 4;

    //mdhd
    hton_set_u32(buf + offset, 32); //size
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('m','d','h','d'));
    offset += 4;
    hton_set_u32(buf + offset, 0); //version & flags
    offset += 4;
    hton_set_u32(buf + offset, 0); //creation_time
    offset += 4;
    hton_set_u32(buf + offset, 0); //modification_time
    offset += 4;
    hton_set_u32(buf + offset, mdhd_time_scale); //time_scale
    offset += 4;
    hton_set_u32(buf + offset, mdhd_time_duration); //track_duration
    offset += 4;
    hton_set_u16(buf + offset, 0); //language
    offset += 2;
    hton_set_u16(buf + offset, 0); //quality
    offset += 2;

    //hdlr
    hton_set_u32(buf + offset, 45); //size
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('h','d','l','r'));
    offset += 4;
    hton_set_u32(buf + offset, 0); //version & flags
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('m','h','l','r')); //component_type
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('s','o','u','n')); //component_subtype
    offset += 4;
    offset += 12; //component_manufacturer, component_flags, component_flags_mask
    hton_set_u8(buf + offset, strlen(mhlr_a_name)); //component_subtype
    offset += 1;
    memcpy(buf + offset, mhlr_a_name, strlen(mhlr_a_name));
    offset += strlen(mhlr_a_name);

    //minf
    AR_U32 minf_offset = offset;
    hton_set_u32(buf + offset, 0); //size
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('m','i','n','f'));
    offset += 4;

    //smhd
    hton_set_u32(buf + offset, 16); //size
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('s','m','h','d'));
    offset += 4;
    v_f.v_f_s.version = 0;
    v_f.v_f_c[3] = 0;
    memcpy(buf + offset, &v_f, 4);
    offset += 4;
    hton_set_u16(buf + offset, 0); //balance
    offset += 2;
    offset += 2; //res

    //hdlr
    hton_set_u32(buf + offset, 44); //size
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('h','d','l','r'));
    offset += 4;
    hton_set_u32(buf + offset, 0); //version & flags
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('d','h','l','r')); //component_type
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('u','r','l',' ')); //component_subtype
    offset += 4;
    offset += 12; //component_manufacturer, component_flags, component_flags_mask
    hton_set_u8(buf + offset, strlen(dhlr_name)); //component_subtype
    offset += 1;
    memcpy(buf + offset, dhlr_name, strlen(dhlr_name));
    offset += strlen(dhlr_name);

    //dinf
    hton_set_u32(buf + offset, 36); //size
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('d','i','n','f'));
    offset += 4;
    //dref
    hton_set_u32(buf + offset, 28); //size
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('d','r','e','f'));
    offset += 4;
    hton_set_u32(buf + offset, 0); //version & flags
    offset += 4;
    hton_set_u32(buf + offset, 1); //count
    offset += 4;
    //url
    hton_set_u32(buf + offset, 12); //size
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('u','r','l',' '));
    offset += 4;
    v_f.v_f_s.version = 0;
    v_f.v_f_c[3] = 1;
    memcpy(buf + offset, &v_f, 4);
    offset += 4;

    //stbl
    AR_U32 stbl_offset = offset;
    hton_set_u32(buf + offset, 0); //size
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('s','t','b','l'));
    offset += 4;

    //stsd
    AR_U32 stsd_offset = offset;
    hton_set_u32(buf + offset, 0); //size
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('s','t','s','d'));
    offset += 4;
    hton_set_u32(buf + offset, 0); //version & flags
    offset += 4;
    hton_set_u32(buf + offset, 1); //count
    offset += 4;
    //stsd sample
    AR_U32 stsd_sample_offset = offset;
    hton_set_u32(buf + offset, 0); //description_size
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('s','o','w','t')); //data_format
    offset += 4;
    offset += 6; //res
    hton_set_u16(buf + offset, 1); //data_reference_index
    offset += 2;
    hton_set_u32(buf + offset, 0); //version, revision_level
    offset += 4;
    hton_set_u32(buf + offset, 0); //vendor
    offset += 4;

    hton_set_u16(buf + offset, handle->a_chans); //number_of_channels
    offset += 2;
    hton_set_u16(buf + offset, handle->a_bits); //sample_size
    offset += 2;
    hton_set_u16(buf + offset, 0); //compression_id
    offset += 2;
    hton_set_u16(buf + offset, 0); //packet_size
    offset += 2;
    hton_set_u16(buf + offset, handle->a_rate); //sample_rate
    offset += 2;
    offset += 2; //res
    //stsd description data
    hton_set_u32(buf + offset, 24); //size
    offset += 4;
    memcpy(buf + offset, stsd_a_data, 20);
    offset += 20;
    //update stsd size
    AR_U32 stsd_sample_size = offset - stsd_sample_offset;
    hton_set_u32(buf + stsd_sample_offset, stsd_sample_size);
    AR_U32 stsd_size = offset - stsd_offset;
    hton_set_u32(buf + stsd_offset, stsd_size);

    //stts
    hton_set_u32(buf + offset, 24); //size
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('s','t','t','s'));
    offset += 4;
    hton_set_u32(buf + offset, 0); //version & flags
    offset += 4;
    hton_set_u32(buf + offset, 1); //count
    offset += 4;
    //stts sample
    hton_set_u32(buf + offset, stts_sample_counts); //count
    offset += 4;
    hton_set_u32(buf + offset, stts_sample_duration); //duration
    offset += 4;

    //no stss
    int i = 0;

    //stsc
    //AR_U32 stsc_samples_per_chunk = 1;
    AR_U32 stsc_count = 1;
    AR_U32 stsc_size = 16 + stsc_count * 12;
    hton_set_u32(buf + offset, stsc_size); //size
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('s','t','s','c'));
    offset += 4;
    hton_set_u32(buf + offset, 0); //version & flags
    offset += 4;
    hton_set_u32(buf + offset, stsc_count); //count
    offset += 4;
    //stsc sample
    AR_U32 stsc_samples_per_chunk = 160;
    for (i = 0; i < stsc_count; i ++)
    {
        hton_set_u32(buf + offset, i + 1); //first_chunk
        offset += 4;
        hton_set_u32(buf + offset, stsc_samples_per_chunk); //samples_per_chunk
        offset += 4;
        hton_set_u32(buf + offset, 1); //samples_description_id
        offset += 4;
    }

    //stsz
    AR_U32 stsz_size = 20 + handle->a_count * 4;
    hton_set_u32(buf + offset, stsz_size); //size
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('s','t','s','z'));
    offset += 4;
    hton_set_u32(buf + offset, 0); //version & flags
    offset += 4;
    AR_U32 stts_sample_size = 0;
    hton_set_u32(buf + offset, stts_sample_size); //设置成0，则每个sample大小都不一样
    offset += 4;
    hton_set_u32(buf + offset, handle->a_count); //count
    offset += 4;

	int audio_size = 0;
	for (i = 0; i < handle->a_count; i ++)
    {
    	audio_size = handle->a_idx_array[i].size;
        hton_set_u32(buf + offset, audio_size); //size
        offset += 4;
    }

    //stco
    AR_U32 stco_count = 0;
    if (handle->a_count > 0)
        stco_count = (handle->a_count - 1) / 1 + 1;
    else
        stco_count = 0;
    AR_U32 stco_size = 16 + stco_count * 4;
    hton_set_u32(buf + offset, stco_size); //size
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('s','t','c','o'));
    offset += 4;
    hton_set_u32(buf + offset, 0); //version & flags
    offset += 4;
    hton_set_u32(buf + offset, stco_count); //count
    offset += 4;
    //stco sample
    for (i = 0; i < stco_count; i ++)
    {
        hton_set_u32(buf + offset, handle->a_idx_array[i].offset); //offset
        offset += 4;
    }


    AR_U32 track_end_offset = offset;

    //trak_size
    AR_U32 trak_size = track_end_offset - trak_offset;
    hton_set_u32(buf + trak_offset, trak_size);

    //mdia_size
    AR_U32 mdia_size = track_end_offset - mdia_offset;
    hton_set_u32(buf + mdia_offset, mdia_size);

    //minf_size
    AR_U32 minf_size = track_end_offset - minf_offset;
    hton_set_u32(buf + minf_offset, minf_size);

    //stbl_size
    AR_U32 stbl_size = track_end_offset - stbl_offset;
    hton_set_u32(buf + stbl_offset, stbl_size);

    return offset;
}


int AR_MOV_WriteTail(MovWriteHandle *handle)
{
    if (!handle)
    {
        printf("parameter error.\n");
        return -1;
    }

    int ret = 0;
    //将buff里面最后的数据写入文件中
    if(handle->buffer_pos != 0)
    {
        ret = fwrite(handle->buffer, handle->buffer_pos, 1, handle->fp);
        if(ret != 1)
        {
            printf("write file failed.\n");
            return -1;
        }
        handle->buffer_frame_cnt = 0;
        handle->buffer_pos = 0;
    }


    /* calc param */
    printf("fps:%u v_count:%u\n", handle->fps, handle->video_count);
    AR_U64 tmp64 = ((AR_U64)handle->video_count) * ((AR_U64)handle->time_scale) / handle->fps;
    handle->time_duration = (AR_U32)tmp64;
   
    if(handle->audio_enable)
    {
		AR_U64 audio_tmp64 = ((AR_U64)handle->a_count) * ((AR_U64)handle->time_scale) / 50;  //audio frame rate : 50fps

		handle->time_duration = FFMAX(audio_tmp64, handle->time_duration);
    }	
    printf("time_duration:%u\n", handle->time_duration);

    char buf[1024 * 1024] = {0};
    int offset = 0;

    //moov
    //handle->moov_offset = handle->frame_offset;
    AR_U32 moov_offset = 0;
    hton_set_u32(buf + offset, 0); //size
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('m','o','o','v'));
    offset += 4;

    //mvhd
    hton_set_u32(buf + offset, 108); //size
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('m','v','h','d'));
    offset += 4;
    hton_set_u32(buf + offset, 0); //version & flags
    offset += 4;
    hton_set_u32(buf + offset, 0); //creation_time
    offset += 4;
    hton_set_u32(buf + offset, 0); //modification_time
    offset += 4;
    hton_set_u32(buf + offset, handle->time_scale); //time_scale
    offset += 4;
    hton_set_u32(buf + offset, handle->time_duration); //duration
    offset += 4;
    hton_set_u32(buf + offset, handle->rate); //rate
    offset += 4;
    hton_set_u16(buf + offset, handle->volume); //volume
    offset += 2;
    offset += 10; //reserved
    memcpy(buf + offset, matrix_struct, 36);
    offset += 36;
    offset += 24; //全是0
    if (handle->audio_enable)
        hton_set_u32(buf + offset, 3); //next_track_id
    else
        hton_set_u32(buf + offset, 2); //next_track_id
    offset += 4;

    offset = mov_write_tail_add_v_track(handle, buf, offset);

    if (handle->audio_enable)
    {
        offset = mov_write_tail_add_a_track(handle, buf, offset);
    }

    //udta
    //AR_U32 udta_offset = offset;
    hton_set_u32(buf + offset, 33); //size
    offset += 4;
    hton_set_u32(buf + offset, MAKE_FCC('u','d','t','a'));
    offset += 4;
    memcpy(buf + offset, udta_data, 33 - 8);
    offset += 25;

    //end
    AR_U32 end_offset = offset;
    handle->file_size = end_offset + handle->frame_offset;
    printf("file size:%x\n", handle->file_size);

    //moov_size
    handle->moov_size = end_offset - moov_offset;
    hton_set_u32(buf + moov_offset, handle->moov_size);

    /*******************************************************************/
    /* write data */
    ret = fwrite(buf, 1, offset, handle->fp);
    if (ret != offset)
    {
        printf("write file failed.\n");
        return -1;
    }

    /* update size */
    char size_buf[4] = {0};

    /* update mdat size */
    handle->mdat_size = handle->frame_offset - handle->mdat_offset;
    hton_set_u32(size_buf, handle->mdat_size); //size
    ret = fseek(handle->fp, handle->mdat_offset, SEEK_SET);
    if(ret != 0)
    {
        printf("fseek failed.\n");
        return -1;
    }
    ret = fwrite(size_buf, 1, 4, handle->fp);
    if (ret != 4)
    {
        printf("write file failed.\n");
        return -1;
    }

    fflush(handle->fp);
    return 0;

}


int mov_get_vps_sps_pps(char *frame, int frame_len, MovWriteHandle *handle)
{
	int pos = 0;

	if (!handle)
	{
	    printf("parameter error.\n");
	    return -1;
	}

	memset(handle->pps, 0, sizeof(handle->pps));
	memset(handle->sps, 0, sizeof(handle->sps));
	memset(handle->vps_sps_pps, 0, sizeof(handle->vps_sps_pps));
		
	if(handle->codec_type == MOV_H265_TYPE)
	{
		//printf("MOV_H265_TYPE\r\n");
		iframe_get_vps_sps_pps((AR_U8 *)frame, frame_len,handle->vps, &(handle->vps_len), handle->sps, &(handle->sps_len), handle->pps, &(handle->pps_len));
		hton_set_u32(handle->vps_sps_pps + pos, handle->vps_len);
		pos += 4;
		memcpy(handle->vps_sps_pps + pos, handle->vps, handle->vps_len);
		pos += handle->vps_len;
		hton_set_u32(handle->vps_sps_pps + pos, handle->sps_len);
		pos += 4;
		memcpy(handle->vps_sps_pps + pos, handle->sps, handle->sps_len);
		pos += handle->sps_len;
		hton_set_u32(handle->vps_sps_pps + pos, handle->pps_len);
		pos += 4;
		memcpy(handle->vps_sps_pps + pos, handle->pps, handle->pps_len);
		pos += handle->pps_len;

		iframe_write_hvcc(handle); 
	}
	else
	{
		iframe_get_pps_sps((AR_U8 *)frame, frame_len, handle->sps, &(handle->sps_len), handle->pps, &(handle->pps_len));

		hton_set_u32(handle->vps_sps_pps + pos, handle->sps_len);
		pos += 4;
		memcpy(handle->vps_sps_pps + pos, handle->sps, handle->sps_len);
		pos += handle->sps_len;
		hton_set_u32(handle->vps_sps_pps + pos, handle->pps_len);
		pos += 4;
		memcpy(handle->vps_sps_pps + pos, handle->pps, handle->pps_len);
		pos += handle->pps_len;
	}
	//printf("pps_len:0x%x sps_len:0x%x\n", handle->pps_len, handle->sps_len);
	//print_buf(handle->pps, handle->pps_len);
	//print_buf(handle->sps, handle->sps_len);

	handle->vps_sps_pps_len = pos;
	//printf("sps_len:0x%x pps_len:0x%x sps_pps_len:0x%x \n", handle->sps_len, handle->pps_len, handle->sps_pps_len);
	//print_buf(handle->sps_pps, handle->sps_pps_len);

	return 0;
}

AR_S32 AR_MOV_WriteFrame(AR_CHAR *frame, AR_S32 frame_len, AR_S32 frame_type, MovWriteHandle *handle, AR_U32 time_ms)
{
	int ret = 0;
	if( (!handle) || (!frame))
	{
		printf("parameter error.\n");
		return -1;
	}

	if((handle->buffer_pos + frame_len) > REC_BUF_SIZE)
	{
		printf("REC_BUF_SIZE = %d is too small. handle->buffer_pos = %d\n", REC_BUF_SIZE, handle->buffer_pos);
		return -1;
	}

	if (frame_type == MOV_FRAME_TYPE_IDR || (frame_type == MOV_FRAME_TYPE_I && handle->codec_type == MOV_H265_TYPE)) //IDR
	{
		if (handle->vps_sps_pps_len == 0)
		{
			mov_get_vps_sps_pps(frame, frame_len, handle);
		}

		/* write (vps + sps + pps) */
		memcpy(handle->buffer + handle->buffer_pos, handle->vps_sps_pps, handle->vps_sps_pps_len);
		handle->buffer_pos += handle->vps_sps_pps_len;
		/* write frame size + data */
		hton_set_u32(handle->buffer + handle->buffer_pos, frame_len - handle->vps_sps_pps_len - 4);
		handle->buffer_pos += 4;
		memcpy(handle->buffer + handle->buffer_pos, frame + handle->vps_sps_pps_len + 4, frame_len - handle->vps_sps_pps_len - 4);
		handle->buffer_pos += frame_len - handle->vps_sps_pps_len - 4;

        handle->video_count ++;
    }
    else if (frame_type == MOV_FRAME_TYPE_P || (frame_type == MOV_FRAME_TYPE_I && handle->codec_type == MOV_H264_TYPE) ) //p
    {
        /* write frame size + data */
        hton_set_u32(handle->buffer + handle->buffer_pos, frame_len - 4);
        handle->buffer_pos += 4;
        memcpy(handle->buffer + handle->buffer_pos, frame + 4, frame_len - 4);
        handle->buffer_pos += frame_len - 4;

        handle->video_count ++;
    }
    else //A
    {
        if (handle->audio_enable)
        {
            memcpy(handle->buffer + handle->buffer_pos, frame, frame_len);
            handle->buffer_pos += frame_len;
            handle->a_count ++;
            handle->a_bytes += frame_len;
        }
        else
            return 0;
    }

    handle->buffer_frame_cnt ++;
    if(handle->buffer_frame_cnt == WRITE_ONCE_NUM)
    {
        if (handle->fp)
        {
            ret = fwrite(handle->buffer, 1, handle->buffer_pos, handle->fp);
            if (ret != handle->buffer_pos)
            {
                printf("write file failed.\n");
                return -1;
            }
        }
        else
        {
            printf("avi file not exist.\n");
            return -1;
        }

        //memset(handle->buffer, 0, sizeof(handle->buffer));
        handle->buffer_pos = 0;
        handle->buffer_frame_cnt = 0;
    }

    if (frame_type != MOV_FRAME_TYPE_A)
    {
        if( handle->idx_array_count <= handle->index_count)
        {
            handle->idx_array_count += 5000;
            handle->idx_array = (FrameInfo *)realloc(handle->idx_array, handle->idx_array_count * sizeof(FrameInfo) );
            if (handle->idx_array == NULL)
            {
                printf("realloc failed.\n");
                return -1;
            }
        }

        handle->idx_array[handle->index_count].no = handle->index_count;
        handle->idx_array[handle->index_count].type = frame_type;
        handle->idx_array[handle->index_count].offset = handle->frame_offset;
        handle->idx_array[handle->index_count].size= frame_len;

        handle->index_count ++;

    }
    else
    {
        if (handle->audio_enable)
        {
            if( handle->a_idx_array_count <= handle->a_idx_index)
            {
                handle->a_idx_array_count += 5000;
                handle->a_idx_array = (FrameInfo *)realloc(handle->a_idx_array, handle->a_idx_array_count * sizeof(FrameInfo) );
                if (handle->a_idx_array == NULL)
                {
                    printf("realloc failed.\n");
                    return -1;
                }
            }

            handle->a_idx_array[handle->a_idx_index].no = handle->index_count;
            handle->a_idx_array[handle->a_idx_index].type = frame_type;
            handle->a_idx_array[handle->a_idx_index].offset = handle->frame_offset;
            handle->a_idx_array[handle->a_idx_index].size= frame_len;

            handle->a_idx_index ++;

        }
    }


    handle->frame_offset += frame_len;

    return 0;
}




AR_S32 AR_MOV_WriteOpen(MovWriteHandle *handle)
{
    if ((!handle) || (strlen(handle->file_path) == 0))
    {
        printf("parameter error.\n");
        return -1;
    }

    if(handle->fp)
    {
        fclose(handle->fp);        handle->fp = NULL;
   }

    handle->fp = fopen(handle->file_path, "wb+");
    if(NULL == handle->fp)
    {
        printf("open %s failed.\n", handle->file_path);
        return -1;
    }

    handle->index_count = 0;
    if (handle->idx_array_count == 0)
        handle->idx_array_count = 3000;

    handle->idx_array = (FrameInfo *)malloc(handle->idx_array_count * sizeof(FrameInfo));
    if (handle->idx_array == NULL)
    {
        printf("malloc failed.\n");
        return -1;
    }
    memset(handle->idx_array, 0, handle->idx_array_count * sizeof(FrameInfo));

    handle->a_idx_index= 0;
    if (handle->a_idx_array_count == 0)
        handle->a_idx_array_count = 3000;

    handle->a_idx_array = (FrameInfo *)malloc(handle->a_idx_array_count * sizeof(FrameInfo));
    if (handle->a_idx_array == NULL)
    {
        printf("malloc failed.\n");
        return -1;
    }
	memset(handle->a_idx_array, 0, handle->a_idx_array_count * sizeof(FrameInfo));

    if(!handle->buffer)
    {
        handle->buffer = (AR_CHAR *)malloc(REC_BUF_SIZE);
        handle->buffer_frame_cnt = 0;
        handle->buffer_pos = 0;
    }

    return 0;
}

AR_S32 AR_MOV_WriteClose(MovWriteHandle *handle)
{
    if (!handle)
    {
        printf("parameter error.\n");
        return -1;
    }

    if(handle->fp)
    {
        fflush(handle->fp);
        fsync(fileno(handle->fp));

        fclose(handle->fp);
        handle->fp = NULL;
    }

    if(handle->idx_array)
    {
        free(handle->idx_array);
        handle->idx_array = NULL;
    }

    if(handle->a_idx_array)
    {
        free(handle->a_idx_array);
        handle->a_idx_array = NULL;
    }

    if(handle->buffer)
    {
        free(handle->buffer);
    }

    memset(handle, 0, sizeof(MovWriteHandle));

    return 0;
}

AR_S32 AR_MOV_Init(MovWriteHandle *handle, MovWriteInitParam *param)
{
    if( (!handle) || (!param))
    {
        printf("parameter error.\n");
        return -1;
    }

    #if SIGNAL_TEST
    param->fps = 25;
    param->gop = 20;
    param->time_scale = 1000;
    param->rate = 0x00010000;
    param->volume = 0x0100;
    param->width = 1280;
    param->height  = 720;
    #endif


    handle->fps = param->fps;
    handle->gop = param->gop;
    handle->time_scale = param->time_scale;
    handle->rate = param->rate;
    handle->volume = param->volume;
    handle->width = param->width;
    handle->height = param->height;

    handle->audio_enable = param->audio_enable;
    handle->a_enc_type = param->a_enc_type;

    handle->a_rate = param->a_rate;
    handle->a_chans = param->a_chans;
    handle->a_bits = param->a_bits;
    return 0;
}

void AR_MOV_ClearWriteBuff(MovWriteHandle * handle)
{
    memset(handle->buffer, 0, REC_BUF_SIZE);
    handle->buffer_pos = 0;
    handle->buffer_frame_cnt = 0;
}

void AR_MOV_PrintHandle(MovWriteHandle *handle)
{
    if (!handle)
    {
        printf("parameter error.\n");
        return;
    }

    printf("mov write handle info:\n");
    printf("fps:%u\n", handle->fps);
    printf("gop:%u\n", handle->gop);
    printf("time_scale:%u\n", handle->time_scale);
    printf("rate:0x%x\n", handle->rate);
    printf("volume:0x%x\n", handle->volume);
    printf("width:%u\n", handle->width);
    printf("height:%u\n", handle->height);
    printf("audio_enable:%u\n", handle->audio_enable);
    printf("a_enc_type:%u\n", handle->a_enc_type);
    printf("file path:%s\n", handle->file_path);
    printf("video_count:%u\n", handle->video_count);
    printf("index_count:%u\n", handle->index_count);
    printf("a_count:%d\n", handle->a_count);
    printf("a_idx_index:%u\n", handle->a_idx_index);
    printf("a_bytes:%d\n", handle->a_bytes);
    printf("a_rate:%d\n", handle->a_rate);
    printf("a_chans:%d\n", handle->a_chans);
    printf("a_bits:%d\n", handle->a_bits);

}




/***************************************************************/
//#if SIGNAL_TEST
#if 0

int h264_to_mov(H264ReadHandle *r_handle, MovWriteHandle *w_handle)
{
    AR_MOV_WriteHead(w_handle);
    int i;
    char frame[1024 * 1024];
    int frame_len;
    int is_key;
    printf("r_handle->index_count: %d\n",  r_handle->index_count);
    for(i = 0; i < r_handle->index_count; i ++)
    {
        h264_get_frame_by_index(frame, &frame_len, &is_key, r_handle, i);
        printf("%d - offset:%u\n", i, w_handle->frame_offset);
        AR_MOV_WriteFrame(frame, frame_len, is_key, w_handle);
    }

    AR_MOV_WriteTail(w_handle);

    return 0;
}

int h264_to_mov_by_file(char *input_file, char *output_file)
{
    H264ReadHandle r_handle = {0};
    h264_read_open(input_file, &r_handle);

    h264_get_index_info(&r_handle);
    h264_print_index_info(&r_handle);

    MovWriteHandle w_handle = {0};
    MovWriteInitParam init_param = {0};
    AR_MOV_Init(&w_handle, &init_param);
    strcpy(w_handle->file_path, output_file);
    AR_MOV_WriteOpen(&w_handle);
    h264_to_mov(&r_handle, &w_handle);
    AR_MOV_WriteClose(&w_handle);

    h264_read_close(&r_handle);

}

int main()
{
    //char *input_file = "test_1.264";
    //char *output_file = "test_1_convert.mov";

    //char *input_file = "720.h264";
    //char *output_file = "720_convert.mov";

    char *input_file = "test_2.h264";
    char *output_file = "test_2_convert.mov";


    h264_to_mov_by_file(input_file, output_file);

    return 0;
}
#endif

