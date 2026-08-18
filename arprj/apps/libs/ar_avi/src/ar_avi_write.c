#include <errno.h>
#include "ar_avi_api.h"
#include "ar_avi_utility.h"

#define CAL_TIME_REC 0
#define AVI_WRITE_ONCE_NUM 15

//According to Microsoft mmreg.h definition, or RFC2361
#define WAVE_FORMAT_PCM    1
#define WAVE_FORMAT_ADPCM  2
#define WAVE_FORMAT_ALAW   6
#define WAVE_FORMAT_ULAW   7

AR_S32 AR_AVI_WriteHeader(AviFile *avi_file)
{
    AR_S32 bitrate, framerate, width, height, ms_per_frame;
    AR_S32 ret;
    AR_S32 type, fcctype;
    AR_S32 a_chans, a_rate, a_bits, a_enc_type, samplesize;

    if (!avi_file)
    {
        printf("parameter error.\n");
        return -1;
    }

    //init param
    bitrate = avi_file->bps;
    framerate = avi_file->fps;
    //ms_per_frame = 1000000./framerate + 0.5;
    ms_per_frame = 1000000./framerate;
    width = avi_file->width;
    height = avi_file->height;

    type = avi_file->codec_type;   

    switch(type)
    {
        case AVI_TYPE_H264:
        case AVI_TYPE_H265:
        case AVI_TYPE_MPEG4:
        case AVI_TYPE_MJPEG:
            fcctype = MAKE_FOURCC('v','i','d','s');
            break;
        default:  break;
    }

    #if 0
    //audio param
    a_chans = 1; /* Audio channels, 0 for no audio */
    a_rate = 8000; /* Rate in Hz */
    a_bits = 8; /* bits per audio sample */
    a_enc_type = 0;
    #else
    a_chans = avi_file->a_chans;
    a_rate = avi_file->a_rate;
    a_bits = avi_file->a_bits;
    a_enc_type = avi_file->a_enc_type;
    #endif
    samplesize = ((a_bits + 7) / 8) * a_chans;
    if(samplesize == 0)
        samplesize=1; /* avoid possible zero divisions */
    avi_file->samplesize = samplesize;

    AviHeader avi_header;
    memset(&avi_header, 0, sizeof(AviHeader));

    //write RIFF_AVI
    avi_header.fcc_riff = MAKE_FOURCC('R','I','F','F');
    avi_header.avi_len = 0; //should be updated
    avi_header.fcc_avi = MAKE_FOURCC('A','V','I',' ');


    //write LIST_hdrl
    avi_header.fcc_list_hdrl = MAKE_FOURCC('L','I','S','T');
    avi_header.hdrl_len = 292;//(AR_U32)(&avi_header.fcc_list_movi) - (AR_U32)(&avi_header.fcc_hdrl);
    //printf("avi_header.hdrl_len = %d\r\n,", avi_header.hdrl_len );
    avi_header.fcc_hdrl = MAKE_FOURCC('h','d','r','l');


    //write AviMainHeader
    avi_header.main_header.fcc = MAKE_FOURCC('a','v','i','h');
    avi_header.main_header.cb = 56; //sizeof(AviMainHeader) - 8 = 64 -8 =56
    avi_header.main_header.dwMicroSecPerFrame = ms_per_frame;
    avi_header.main_header.dwMaxBytesPerSec = bitrate / 8;
    avi_header.main_header.dwPaddingGranularity = 0;
    avi_header.main_header.dwFlags = AVIF_HASINDEX | AVIF_WASCAPTUREFILE;
    avi_header.main_header.dwTotalFrames = avi_file->video_count;     //<==need update, framecount, main_header_start+24
    avi_header.main_header.dwInitialFrames = 0;
    avi_header.main_header.dwStreams = 2;
    avi_header.main_header.dwSuggestedBufferSize = width*height*3; //1000000
    avi_header.main_header.dwWidth = width;
    avi_header.main_header.dwHeight = height;
    memset(avi_header.main_header.dwReserved, 0 ,sizeof(avi_header.main_header.dwReserved));	

    /************ video start *******************/
    //write LIST_strl
    avi_header.fcc_list_strl_video = MAKE_FOURCC('L','I','S','T');
    avi_header.size_strl_video = 116;//(AR_U32)(&avi_header.fcc_list_strl_audio) - (AR_U32)(&avi_header.fcc_strl_video);
    //printf(" avi_header.size_strl_video = %d\r\n",avi_header.size_strl_video);
    avi_header.fcc_strl_video = MAKE_FOURCC('s','t','r','l');

    //write AviStreamHeader
    avi_header.video_header.fcc = MAKE_FOURCC('s','t','r','h');
    avi_header.video_header.cb = 56; //sizeof(AviStreamHeader)-8 = 64-8 =56
    avi_header.video_header.fccType = fcctype;  //vids
    avi_header.video_header.fccHandler = type; //MAKE_FOURCC('H','2','6','4');
    avi_header.video_header.dwFlags = 0x0;
    avi_header.video_header.wPriority = 0;
    avi_header.video_header.wLanguage = 0;
    avi_header.video_header.dwInitialFrames = 0;
    avi_header.video_header.dwScale = 1;
    avi_header.video_header.dwRate = avi_file->fps;
    avi_header.video_header.dwStart = 0;
    avi_header.video_header.dwLength = avi_file->video_count;  //<==need update, framecount, stream_header_start+52
    avi_header.video_header.dwSuggestedBufferSize = REC_BUF_SIZE;
    avi_header.video_header.dwQuality = -1;
    avi_header.video_header.dwSampleSize = 0;//width*height;
    avi_header.video_header.rcFrame.left = 0;
    avi_header.video_header.rcFrame.top = 0;
    avi_header.video_header.rcFrame.right = width;
    avi_header.video_header.rcFrame.bottom = height;

    //write strf+size+AviStreamFormat
    avi_header.fcc_strf_video = MAKE_FOURCC('s','t','r','f');
    avi_header.size_video_format = 40; //sizeof(BitMapInfoHeader)
    avi_header.bmiHeader.biSize = 40;
    avi_header.bmiHeader.biWidth = width;
    avi_header.bmiHeader.biHeight = height;
    avi_header.bmiHeader.biPlanes = 1;
    avi_header.bmiHeader.biBitCount = 24;
    avi_header.bmiHeader.biCompression = type; //MAKE_FOURCC('H','2','6','4'); H264,h264
    avi_header.bmiHeader.biSizeImage = width*height * 3; //width*height
    avi_header.bmiHeader.biXPelsPerMeter = 0;
    avi_header.bmiHeader.biYPelsPerMeter = 0;
    avi_header.bmiHeader.biClrUsed = 0;
    avi_header.bmiHeader.biClrImportant = 0;
    /************ video end *******************/

    /************ audio start *******************/
    //write LIST_strl
    avi_header.fcc_list_strl_audio = MAKE_FOURCC('L','I','S','T');
    avi_header.size_strl_audio = 92;//(AR_U32)(&avi_header.fcc_list_movi) - (AR_U32)(&avi_header.fcc_strl_audio);
    //printf("avi_header.size_strl_audio = %d\r\n",avi_header.size_strl_audio);
    avi_header.fcc_strl_audio = MAKE_FOURCC('s','t','r','l');


    //write AviStreamHeader
    avi_header.audio_header.fcc = MAKE_FOURCC('s','t','r','h');
    avi_header.audio_header.cb = 56; //sizeof(AviStreamHeader)-8 = 64-8 =56
    avi_header.audio_header.fccType = MAKE_FOURCC('a','u','d','s');  //auds
    avi_header.audio_header.fccHandler = MAKE_FOURCC('G','7','1','1');
    //avi_header.audio_header.fccHandler = MAKE_FOURCC('\0','\0','\0','\0');
    avi_header.audio_header.dwFlags = 0x0;
    avi_header.audio_header.wPriority = 0;
    avi_header.audio_header.wLanguage = 0;
    avi_header.audio_header.dwInitialFrames = 0;

    avi_header.audio_header.dwScale = samplesize; // a_bits = 16, 则2 ; a_bits = 8, 则1
    avi_header.audio_header.dwRate = samplesize * a_rate; // 2 * 8000
    avi_header.audio_header.dwStart = 0;

    avi_header.audio_header.dwLength = avi_file->audio_bytes / samplesize;
    avi_header.audio_header.dwSuggestedBufferSize = 0; //1024*1024
    avi_header.audio_header.dwQuality = -1;
    avi_header.audio_header.dwSampleSize = samplesize;
    avi_header.audio_header.rcFrame.left = 0;
    avi_header.audio_header.rcFrame.top = 0;
    avi_header.audio_header.rcFrame.right = 0;
    avi_header.audio_header.rcFrame.bottom = 0;

    //write strf+size+AviStreamFormat
    avi_header.fcc_strf_audio = MAKE_FOURCC('s','t','r','f');
    avi_header.size_audio_format = 16; //sizeof(AudioFormateX)

    /*The wFormatTag definition is in mmreg.h:
     * WAVE_FORMAT_PCM   1
     * WAVE_FORMAT_ADPCM 2
     * WAVE_FORMAT_ALAW  6
     * WAVE_FORMAT_ULAW  7*/
    if (a_enc_type == 0)
        avi_header.audio_format.wFormatTag = 6;
    else if (a_enc_type == 1)
        avi_header.audio_format.wFormatTag = 7;
    else if (a_enc_type == 2)
        avi_header.audio_format.wFormatTag = 1; //pcm
    else if (a_enc_type == 3)
        avi_header.audio_format.wFormatTag = 2; //adpcm
    avi_header.audio_format.nChannels = a_chans;
    avi_header.audio_format.nSamplesPerSec = a_rate;
    avi_header.audio_format.nAvgBytesPerSec = a_rate * samplesize;
    avi_header.audio_format.nBlockAlign = samplesize;
    avi_header.audio_format.wBitsPerSample = a_bits; //8, if pcm: 32
	//avi_write_uint16( a,  18);	//cbSize =?18
	//avi_write_uint16( a,  2);
    /************ audio end *******************/

    //write LIST_movi
    avi_header.fcc_list_movi = MAKE_FOURCC('L','I','S','T');
    avi_header.movi_len = 0;
    avi_header.fcc_movi = MAKE_FOURCC('m','o','v','i');

    avi_file->data_offset = sizeof(AviHeader);

    //write avi header
    ret = fwrite(&avi_header, sizeof(AviHeader), 1, avi_file->file);
    if(ret != 1)
    {
        printf("write avi header failed.\n");
        return -1;
    }

    //fflush(avi_file->file);

    return 0;
}

AR_S32 update_avi_header(AviFile *avi_file)
{
    AR_S32 ret;
    AR_S32 avi_len = ftell(avi_file->file) - 8;
    AR_S32 movi_len = avi_file->data_offset - sizeof(AviHeader) + 4;

    //printf("index offset = %d, avi_len = %d, movi_len = %d.\n", avi_file->data_offset, avi_len, movi_len);

    fseek(avi_file->file, 4, SEEK_SET);
    ret = AR_AVI_WriteInt32(avi_file->file, avi_len);
    if(ret != 1)
    {
        printf("write avi len failed.\n");
        return -1;
    }

    fseek(avi_file->file, sizeof(AviHeader) - 8, SEEK_SET);
    ret = AR_AVI_WriteInt32(avi_file->file, movi_len);
    if(ret != 1)
    {
        printf("write movi len failed.\n");
        return -1;
    }

    fseek(avi_file->file, (AR_S32)(uintptr_t)&(((AviHeader *)0)->main_header.dwTotalFrames), SEEK_SET);
	ret = AR_AVI_WriteInt32(avi_file->file, avi_file->video_count);
    if(ret != 1)
    {
        printf("write total frames count failed.\n");
        return -1;
    }

	fseek(avi_file->file, (AR_S32)(uintptr_t)&(((AviHeader *)0)->video_header.dwLength), SEEK_SET);
	ret = AR_AVI_WriteInt32(avi_file->file, avi_file->video_count);
    if(ret != 1)
    {
        printf("write video frames count failed.\n");
        return -1;
    }

    AR_S32 audio_count = avi_file->audio_bytes / avi_file->samplesize;
    #if 1
    if ( (avi_file->a_enc_type == 0) || (avi_file->a_enc_type == 1) )
    {
        audio_count = audio_count * 2;
    }
    else if (avi_file->a_enc_type == 2)
    {
    }
    else if (avi_file->a_enc_type == 3)
    {
        audio_count = audio_count * 4;
    }
    #endif

	fseek(avi_file->file, (AR_S32)(uintptr_t)&(((AviHeader *)0)->audio_header.dwLength), SEEK_SET);
	ret = AR_AVI_WriteInt32(avi_file->file, audio_count);
    if(ret != 1)
    {
        printf("write audio block count failed.\n");
        return -1;
    }

    //fflush(avi_file->file);
    return 0;
}

AR_S32 AR_AVI_AddIndex(AviFile *avi_file)
{
    AR_S32 ret;

    if(!avi_file)
    {
        printf("parameter is error.\n");
        return -1;
    }

    if(avi_file->buffer_pos != 0)
    {
        ret = fwrite(avi_file->buffer, avi_file->buffer_pos, 1, avi_file->file);
        if(ret != 1)
        {
            printf("fwrite failed.\n");
            return -1;
        }
        avi_file->buffer_frame_cnt = 0;
        avi_file->buffer_pos = 0;
    }

    //test
    //AR_S32 tmp32 = ftell(avi_file->file);
    //printf("add index,index offset = %d. data offset = %d\n", tmp32, avi_file->data_offset);

    //add index
    ret = AR_AVI_WriteFourcc(avi_file->file, MAKE_FOURCC('i','d','x','1'));
    if(ret != 1)
    {
        printf("write avi index fcc failed.\n");
        return -1;
    }
    ret = AR_AVI_WriteInt32(avi_file->file, 16*avi_file->index_count);
    if(ret != 1)
    {
        printf("write avi index size failed.\n");
        return -1;
    }
    //printf("add index, all count = %d. video count = %d\n", avi_file->index_count, avi_file->video_count);
    ret = fwrite(avi_file->idx_array, avi_file->index_count*16, 1, avi_file->file);
    if(ret != 1)
    {
        printf("write avi index array failed.\n");
        return -1;
    }

    //updata avi header
    ret = update_avi_header(avi_file);
    if(ret < 0)
    {
        printf("call update_avi_header failed.\n");
        return -1;
    }

    return 0;
}


void AR_AVI_ClearWriteBuff(AviFile * avi_file)
{
    if(avi_file && avi_file->buffer)
    {
        memset(avi_file->buffer, 0, REC_BUF_SIZE);
        avi_file->buffer_frame_cnt = 0;
        avi_file->buffer_pos = 0;
    }
}

AR_S32 AR_AVI_WriteClose(AviFile *avi_file)
{
    if (!avi_file)
    {
        printf("parameter error.\n");
        return -1;
    }

    //close & free
    if(avi_file->file)
    {
        fflush(avi_file->file);
        fsync(fileno(avi_file->file));

        fclose(avi_file->file);
        avi_file->file = NULL;
    }

    if(avi_file->idx_array)
    {
        free(avi_file->idx_array);
        avi_file->idx_array = NULL;
    }

    if(avi_file->buffer)
    {
        free(avi_file->buffer);
    }

    memset(avi_file, 0, sizeof(AviFile));

    return 0;
}

AR_S32 AR_AVI_WriteOpen(AviFile *handle)
{
    if ((!handle) || (strlen(handle->filename) == 0))
    {
        printf("parameter error.\n");
        return -1;
    }

    if(handle->file)
    {
        fclose(handle->file);        handle->file = NULL;    }

    handle->file = fopen(handle->filename, "wb+");
    if(NULL == handle->file)
    {
        printf("open %s failed.\n", handle->filename);
        return -1;
    }

    handle->index_count = 0;
    if (handle->idx_array_count == 0)
        handle->idx_array_count = 3000;

    if(handle->idx_array)
    {
        free(handle->idx_array);
        handle->idx_array = NULL;
    }
    handle->idx_array = (AR_S32 *)malloc(handle->idx_array_count * 16);
    if (handle->idx_array == NULL)
    {
        printf("malloc failed.\n");
        return -1;
    }
    memset(handle->idx_array, 0, handle->idx_array_count * 16);

    if(!handle->buffer)
    {
        handle->buffer = (char *)malloc(REC_BUF_SIZE);
        handle->buffer_frame_cnt = 0;
        handle->buffer_pos = 0;
    }

    return 0;
}

//intra: 1 I, 0 P, 2 A, 3 JPEG
AR_S32 AR_AVI_WriteFrame(char *frame, AR_S32 length, AR_S32 frame_type, AviFile *avi_file, AR_U32 time_ms)
{
    if( (!avi_file) || (!frame))
    {
        printf("parameter error.\n");
        return -1;
    }

    AR_S32 ret = 0;
    AR_S32 is_align = length % 2;

    if((avi_file->buffer_pos + 8 + length + is_align) > REC_BUF_SIZE)
    {
        printf("REC_BUF_SIZE is too small. data_array_pos = %d \n", avi_file->buffer_pos);
        printf("REC_BUF_SIZE:%u, length:%d, is_align:%d\n", REC_BUF_SIZE, length, is_align);
        return -1;
    }

    //*((AR_S32 *)&(data_array[data_array_pos])) = MAKE_FOURCC('0','0','d','c'); //0x63643030
    //*((AR_S32 *)&(data_array[data_array_pos + 4])) = length + is_align;
    if(!avi_file->buffer)
    {
		printf("buffer is null \r\n");
		return -1;
    }

    if(frame_type != AVI_FRAME_TYPE_A)
    {
        AR_AVI_Write32(&(avi_file->buffer[avi_file->buffer_pos]), MAKE_FOURCC('0','0','d','c'));
    }
    else
    {
        AR_AVI_Write32( &(avi_file->buffer[avi_file->buffer_pos]), MAKE_FOURCC('0','1','w','b'));
    }

    AR_AVI_Write32( &(avi_file->buffer[avi_file->buffer_pos + 4]), length + is_align);
    memcpy(avi_file->buffer + avi_file->buffer_pos + 8, frame, length);

    if (is_align)
    {
        avi_file->buffer[avi_file->buffer_pos + 8 + length] = 0;
    }

    avi_file->buffer_pos += 8 + length + is_align;
#if 0
    AR_S32 time32 = 0;
    #if 0
    struct timeval tt = {0};
    gettimeofday(&tt, NULL);
    time32 = tt.tv_sec * 1000 + (tt.tv_usec / 1000);
    #else
    time32 = time_ms;
    #endif
	
    //frame data junk
    AR_AVI_Write32( &(avi_file->buffer[avi_file->buffer_pos]), MAKE_FOURCC('J','U','N','K'));
    AR_AVI_Write32( &(avi_file->buffer[avi_file->buffer_pos + 4]), 4);
    AR_AVI_Write32( &(avi_file->buffer[avi_file->buffer_pos + 8]), time32);
    avi_file->buffer_pos += 12;
#endif
    #if 0
    //intra: 1 I, 0 P, 2 A, 3 JPEG
    //printf("avi write frame, type:%d, time32:%d\n", intra, time32);
    if(intra == 1)
    {
        printf("avi write frame, I, time32:%d\n", time32);
    }
    else if(intra == 0)
    {
        printf("avi write frame, P, time32:%d\n", time32);
    }
    else if(intra == 2)
    {
        //printf("avi write frame, A, time32:%d\n", time32);
    }
    #endif

    avi_file->buffer_frame_cnt++;

    if(avi_file->buffer_frame_cnt == AVI_WRITE_ONCE_NUM)
    {
        //printf("AVI_WRITE_ONCE_NUM: %d\n", AVI_WRITE_ONCE_NUM);
        if (avi_file->file)
        {
            ret = fwrite(avi_file->buffer, 1, avi_file->buffer_pos, avi_file->file);
            if(ret != avi_file->buffer_pos)
            {
                printf("fwrite avi file failed: pos %d, ret %d, err: %s.\n", avi_file->buffer_pos, ret, strerror(errno));
                return -1;
            }
        }
        else
        {
            printf("avi file not exist.\n");
            return -1;
        }

        //memset(data_array, 0, sizeof(data_array));
        avi_file->buffer_pos = 0;
        avi_file->buffer_frame_cnt = 0;
    }

    if(avi_file->index_count >= avi_file->idx_array_count)
    {
        printf("the space of index is too small.\n");
        avi_file->idx_array_count += 3000;
        avi_file->idx_array = (AR_S32 *)realloc(avi_file->idx_array, avi_file->idx_array_count * 16);
        if (avi_file->idx_array == NULL)
        {
            printf("realloc failed.\n");
            return -1;
        }
    }

    if(frame_type != AVI_FRAME_TYPE_A)
    {
        AR_AVI_Write32( &avi_file->idx_array[4*avi_file->index_count+0], MAKE_FOURCC('0','0','d','c'));
        //AR_AVI_Write32( &avi_file->idx_array[4*avi_file->index_count+1], (intra) ? 0x10:0);
        //AR_AVI_Write32( &avi_file->idx_array[4*avi_file->index_count+1], 0x10);
        AR_AVI_Write32( &avi_file->idx_array[4*avi_file->index_count+1], (frame_type == AVI_FRAME_TYPE_P) ? 0x10:0x11);
        avi_file->video_count++;
    }
    else
    {
        AR_AVI_Write32( &avi_file->idx_array[4*avi_file->index_count+0], MAKE_FOURCC('0','1','w','b'));
        AR_AVI_Write32( &avi_file->idx_array[4*avi_file->index_count+1], 0x00);
        avi_file->audio_bytes += length+is_align;
    }

    AR_AVI_Write32( &avi_file->idx_array[4*avi_file->index_count+2], avi_file->data_offset);
    AR_AVI_Write32( &avi_file->idx_array[4*avi_file->index_count+3], length + is_align);


    //avi_file->data_offset += (8+length+is_align+12);
    avi_file->data_offset += (8+length+is_align);
    avi_file->index_count++;

    return 0;
}

static AR_S32 avi_read_check_one_frame(AviFile *avi_file, AR_S32 *frame_id, AR_S32 *frame_size)
{
    AR_S32 ret = 0;
    AR_S32 streamid, size;

    *frame_id = 0;
    *frame_size = 0;

    ret = fseek(avi_file->file, avi_file->data_offset, SEEK_SET);
    if (ret != 0)
    {
        printf("fseek frame_id error\n");
        return -1;
    }

    ret = AR_AVI_ReadFourcc(avi_file->file, &streamid);
    if( ((streamid != MAKE_FOURCC('0','0','d','c'))&&(streamid != MAKE_FOURCC('0','1','w','b'))) || (ret != 1))
    {
        printf("read frame_id error\n");
        return -1;
    }


    ret = AR_AVI_ReadInt32(avi_file->file, &size);;
    if(ret != 1)
    {
        printf("read frame_size error\n");
        return -1;
    }

    ret = fseek(avi_file->file, avi_file->data_offset + 8 + size, SEEK_SET); //8 是 00dc+ size
    if (ret != 0)
    {
        printf("fseek junk error\n");
        return -1;
    }

    AR_S32 junk_id, junk_size, junk_time;
    ret = AR_AVI_ReadFourcc(avi_file->file, &junk_id);
    if( (junk_id != MAKE_FOURCC('J','U','N','K')) || (ret != 1) )
    {
        printf("read junk_id error\n");
        return -1;
    }

    ret = AR_AVI_ReadInt32(avi_file->file, &junk_size);;
    if ((ret != 1) || (junk_size != 4))
    {
        printf("read junk_size error\n");
        return -1;
    }

    ret = AR_AVI_ReadInt32(avi_file->file, &junk_time);;
    if (ret != 1)
    {
        printf("read junk_time error\n");
        return -1;
    }

    *frame_id = streamid;
    *frame_size = size;

    return 0;
}

static AR_S32 avi_fix_read_data(AviFile *avi_file, AR_S32 *p_fps)
{
    AR_S32 i, fps, streamid, size;
    i = 0;
    AR_S32 ret;

    if( (!avi_file))
    {
        printf("parameter error.\n");
        return -1;
    }

    #if 1
    AviHeader avi_header;
    fseek(avi_file->file, 0, SEEK_SET);
    ret = fread(&avi_header, sizeof(AviHeader), 1, avi_file->file);
    if(ret != 1)
    {
        printf("read avi header failed. ret = %d\n", ret);
        return -1;
    }

    if(1000000/25 == avi_header.main_header.dwMicroSecPerFrame)
        fps = 25;
    else
        fps = 1000000 / avi_header.main_header.dwMicroSecPerFrame;

    if (fps <= 0)
    {
        printf("read avi header fps failed. fps = %d\n", fps);
        return -1;
    }

    *p_fps = fps;
    printf("fps = %d \n", fps);

    //update avi_file infomation
    avi_file->video_count = avi_header.main_header.dwTotalFrames;
    avi_file->audio_bytes = avi_header.audio_header.dwLength * avi_header.audio_header.dwScale;
    avi_file->data_offset = sizeof(AviHeader);
    #endif

    //read frame data frame by frame
    avi_file->idx_array_count = 0;
    avi_file->video_count = 0;
    avi_file->audio_bytes = 0;
    avi_file->index_count = 0;
    avi_file->data_offset = sizeof(AviHeader);
    avi_file->idx_array = NULL;

    while(1)
    {
        if( avi_file->idx_array_count <= avi_file->index_count)
        {
            avi_file->idx_array_count += 6000;
            avi_file->idx_array = (AR_S32 *)realloc(avi_file->idx_array, avi_file->idx_array_count * 16 );
            if (avi_file->idx_array == NULL)
            {
                printf("realloc failed.\n");
                break;
            }
        }

        ret = avi_read_check_one_frame(avi_file, &streamid, &size);
        if (ret != 0)
        {
            #if 0
            avi_file->index_count --;
            if(avi_file->index_count < 0)
            {
                avi_file->index_count = 0;
                avi_file->data_offset = sizeof(AviHeader);
                avi_file->video_count = 0;
                avi_file->audio_bytes = 0;
                printf("file no frame data.\n");
                break;
            }

            avi_file->data_offset = avi_file->idx_array[4*avi_file->index_count+2];
            if(frame_type)
                avi_file->video_count--;
            else
                avi_file->audio_bytes = last_audio_bytes;
            #endif

            break;
        }

        AR_AVI_Write32( &avi_file->idx_array[4*i+0], streamid);
        if(streamid == MAKE_FOURCC('0','0','d','c'))
        {
            if(avi_file->video_count%fps == 0)
                AR_AVI_Write32( &avi_file->idx_array[4*i+1], 0x11); //i
            else
                AR_AVI_Write32( &avi_file->idx_array[4*i+1], 0x10); //p

            avi_file->video_count ++;
            //frame_type = 1; //video
        }
        else
        {
            AR_AVI_Write32( &avi_file->idx_array[4*i+1], 0x00);
            //last_audio_bytes = avi_file->audio_bytes;
            avi_file->audio_bytes += size;
            //frame_type = 0; //audio
        }

        AR_AVI_Write32( &avi_file->idx_array[4*i+2], avi_file->data_offset);
        AR_AVI_Write32( &avi_file->idx_array[4*i+3], size);
        i++;

        avi_file->data_offset = avi_file->data_offset + 8 + size + 12; //8 是 00dc+ size, 12 是junk
        avi_file->index_count ++;
    }

    ret = fseek(avi_file->file, avi_file->data_offset, SEEK_SET);
    if(ret < 0)
    {
        printf("fseek to last offset failed. offset = %d\n", avi_file->data_offset);
        return -1;
    }
    return 0;
}

AR_S32 avi_fix_add_index(AviFile *avi_file)
{
    AR_S32 ret;

    if(!avi_file)
    {
        printf("parameter is error.\n");
        return -1;
    }

    //add index
    ret = AR_AVI_WriteFourcc(avi_file->file, MAKE_FOURCC('i','d','x','1'));
    if(ret != 1)
    {
        printf("write avi index fcc failed.\n");
        return -1;
    }
    ret = AR_AVI_WriteInt32(avi_file->file, 16*avi_file->index_count);
    if(ret != 1)
    {
        printf("write avi index size failed.\n");
        return -1;
    }
    printf("add index, all count = %d. video count = %d\n", avi_file->index_count, avi_file->video_count);
    ret = fwrite(avi_file->idx_array, avi_file->index_count*16, 1, avi_file->file);
    if(ret != 1)
    {
        printf("write avi index array failed.\n");
        return -1;
    }

    //updata avi header
    ret = update_avi_header(avi_file);
    if(ret < 0)
    {
        printf("call update_avi_header failed.\n");
        return -1;
    }

    return 0;
}

static AR_S32 fix_single_avi_file(char *file_path)
{
    AviFile avi_file;
    //AviInitParam param;

    //char name_tmp[128];
    char time_stop[30];
    AR_S32 ret;

   //open file
    memset(&avi_file, 0, sizeof(avi_file));
    strcpy(avi_file.filename, file_path);
    printf("fixing file : %s\n", avi_file.filename);
    avi_file.file = fopen(avi_file.filename, "rb+");
    if(NULL == avi_file.file)
    {
        printf("open %s failed.\n", avi_file.filename);
        return -1;
    }

    //read data
    AR_S32 fps;
    ret = avi_fix_read_data(&avi_file, &fps);
    if(ret < 0)
    {
        printf("call avi_read_stream_offset error.\n");
        if(avi_file.file)
        {
            fclose(avi_file.file);
            avi_file.file = NULL;
        }

        if(avi_file.idx_array)
        {
            free(avi_file.idx_array);
            avi_file.idx_array = NULL;
        }
        return -1;
    }

    ret = avi_fix_add_index(&avi_file);
    if(ret < 0)
    {
        printf("call avi_read_stream_offset error.\n");
        if(avi_file.file)
        {
            fclose(avi_file.file);
            avi_file.file = NULL;
        }

        if(avi_file.idx_array)
        {
            free(avi_file.idx_array);
            avi_file.idx_array = NULL;
        }
        return -1;
    }

    if(avi_file.file)
    {
        fclose(avi_file.file);
        avi_file.file = NULL;
    }

    if(avi_file.idx_array)
    {
        free(avi_file.idx_array);
        avi_file.idx_array = NULL;
    }

    //cal stop time
    memset(time_stop, 0 ,sizeof(time_stop));
    AR_AVI_CalcStopTime(time_stop, avi_file.filename, avi_file.video_count, fps);
    printf(" stoptime= %s \n", time_stop);


    char str_tmp[128] = {0};
    char new_name[128] = {0};

    strncpy(str_tmp, avi_file.filename, strlen(avi_file.filename) - 5); //0_001850.part
    sprintf(new_name, "%s_%s.avi", str_tmp, time_stop);

    //rename filename_tmp to filename.avi
    ret = rename(avi_file.filename, new_name);
    if(ret)
    {
        printf("rename %s to %s failed.\n", avi_file.filename, new_name);
        return -1;
    }
    printf("fix %s to %s success.\n", avi_file.filename, new_name);

    return 0;
}

static void fix_avi_file_in_dir(char *dir_path)
{
    DIR *pDir;
    struct dirent *ent;
    char file_path[512];

    printf("fix avi file in %s\n", dir_path);

    pDir = opendir(dir_path);
    while((ent = readdir(pDir)) != NULL)
    {

        if(ent->d_type & DT_REG)
        {
            //printf("%s need to be fixed.\n", ent->d_name);
            memset(file_path, 0, sizeof(file_path));
            sprintf(file_path, "%s/%s", dir_path, ent->d_name);
            fix_single_avi_file(file_path);
        }
    }
	closedir(pDir);
}

void AR_AVI_FixAllFiles(char * dir_name)
{
    DIR *pDir;
    struct dirent *ent;
    char dir_path[512];

    if(access(dir_name, F_OK) != 0 )
    {
        printf("%s not exist, no need to fix rec file\n", dir_name);
        return;
    }

    //printf("fix all avi file.\n");
    pDir = opendir(dir_name);
    while((ent = readdir(pDir)) != NULL)
    {
        if(ent->d_type & DT_DIR)
        {
            if(strcmp(ent->d_name,".") == 0 || strcmp(ent->d_name,"..") == 0)
                continue;

            memset(dir_path, 0, sizeof(dir_path));
            sprintf(dir_path, "%s/%s", dir_name, ent->d_name);
            fix_avi_file_in_dir(dir_path);
        }
    }
	closedir(pDir);
}

