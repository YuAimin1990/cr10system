#include "sample_comm_muxer.h"
#include "ar_common.h"
#include "mpi_venc.h"
#include "mpi_audio.h"
#include "ar_comm_venc.h"
#include "ar_comm_aenc.h"

#include <sys/time.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/prctl.h>

static AR_VOID* MUXER_ProcessAudioThread(AR_VOID* arg);
static AR_VOID* MUXER_ProcessVideoThread(AR_VOID* arg);

static AVStream* SAMPLE_COMM_MUXER_AllocStream(MUXER_CONTEXT_T* stContext, AVFormatContext* stFormat, PAYLOAD_TYPE_E enPayload, 
                                            AR_S32 s32VideoWidth, AR_S32 s32VideoHeight, AR_S32 s32VideoBitrate,
                                            AUDIO_SAMPLE_RATE_E enSampleRate, AUDIO_SOUND_MODE_E enSoundMode, AUDIO_BIT_WIDTH_E enBitWidth,
                                            AR_U8* u8ExtraData, AR_S32 s32ExtraSize)
{
    AVStream * stream = avformat_new_stream(stFormat, NULL);
    if (!stream) {
        printf("Could not allocate stream\n");
        return NULL;
    }
    stream->id = stFormat->nb_streams-1;
    
    if( enPayload == PT_AAC ) {
        
        stream->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;
        stream->codecpar->codec_id = AV_CODEC_ID_AAC;
        stream->codecpar->codec_tag = 0;//828601953;
        stream->codecpar->format = 8;
        stream->codecpar->bit_rate = 128000;
        stream->codecpar->bits_per_coded_sample = 16;
        stream->codecpar->bits_per_raw_sample = 0;
        
        stream->codecpar->profile = 1;
        stream->codecpar->level = -99;
        
        stream->codecpar->sample_aspect_ratio.num = 0;
        stream->codecpar->sample_aspect_ratio.den = 1;
        
        stream->codecpar->channel_layout = AV_CH_LAYOUT_STEREO;
        stream->codecpar->channels = 2;
        stream->codecpar->sample_rate = 44100;
        stream->codecpar->frame_size = 1024;
        
        if( stream->codecpar->extradata == NULL ) {
            stream->codecpar->extradata = av_mallocz(2 + AV_INPUT_BUFFER_PADDING_SIZE);
            memset(stream->codecpar->extradata, 0x00, 2 + AV_INPUT_BUFFER_PADDING_SIZE);
        }
        stream->codecpar->extradata[0] = 0x12;
        stream->codecpar->extradata[1] = 0x10;
        stream->codecpar->extradata_size = 2;
    }
    else if( enPayload == PT_H264 ) {
        stream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
        stream->codecpar->codec_id = AV_CODEC_ID_H264;
        stream->codecpar->codec_tag = 0;//828601953;
        stream->codecpar->format = 0;
        stream->codecpar->bit_rate = s32VideoBitrate;
        stream->codecpar->bits_per_coded_sample = 24;
        stream->codecpar->bits_per_raw_sample = 8;
        stream->codecpar->profile = 100;
        stream->codecpar->level = 31;
        stream->codecpar->width = s32VideoWidth;
        stream->codecpar->height = s32VideoHeight;
        stream->codecpar->sample_aspect_ratio.num = 0;
        stream->codecpar->sample_aspect_ratio.den = 1;
        if( u8ExtraData && s32ExtraSize > 0 ) {
            if( stream->codecpar->extradata == NULL ) {
                stream->codecpar->extradata = av_mallocz(s32ExtraSize + AV_INPUT_BUFFER_PADDING_SIZE);
                memset(stream->codecpar->extradata, 0x00, s32ExtraSize + AV_INPUT_BUFFER_PADDING_SIZE);
            }
            memcpy(stream->codecpar->extradata, u8ExtraData, s32ExtraSize);
            stream->codecpar->extradata_size = s32ExtraSize;
        }
    }else if(enPayload == PT_H265){
        return NULL;
    }else{
        return NULL;
    }
    return stream;
}


static AR_S32 SAMPLE_COMM_MUXER_WriteAudioFrame(MUXER_CONTEXT_T* context, MUXER_AVSTREAM_T* pstAvStream, AUDIO_STREAM_S* stAudioFrame)
{
    AVPacket *pkt = av_packet_alloc();

    AVStream* pAvStream = pstAvStream->stStream;

    if(pstAvStream->u64Pts == 0){
        pstAvStream->u64Pts = stAudioFrame->u64TimeStamp;
    }
    int64_t pts = (stAudioFrame->u64TimeStamp - pstAvStream->u64Pts)*0.0441;
    // printf("audio timestamp %lld, pts %lld\n",stAudioFrame->u64TimeStamp,pts);
    
    pkt->buf = NULL;
    pkt->dts = pkt->pts = pts;
    pkt->data = stAudioFrame->pStream;
    pkt->size = stAudioFrame->u32Len;
    pkt->stream_index = pAvStream->index;
    pkt->flags = 0;
    pkt->side_data = NULL;
    pkt->side_data_elems = 0;
    pkt->duration = 0;
    pkt->pos = -1;
    pthread_mutex_lock(&context->muxerMutex);
    int ret = av_interleaved_write_frame(context->stMuxerFormat, pkt);
    pthread_mutex_unlock(&context->muxerMutex);
    av_packet_free(&pkt);
    if( ret ) {
        printf("av_interleaved_write_frame audio failed %d\n", ret);
        return AR_FAILURE;
    }
    return AR_SUCCESS;
}


static AR_S32 SAMPLE_COMM_MUXER_WriteVideoFrame(MUXER_CONTEXT_T* context, MUXER_AVSTREAM_T* pstAvStream, VENC_STREAM_S* stVideoFrame)
{
    if(stVideoFrame->u32PackCount <= 0){
        return AR_FAILURE;
    }

    AVStream* pAvStream = pstAvStream->stStream;

    AVPacket *pkt = av_packet_alloc();
    if(pkt == NULL){
        return AR_FAILURE;
    }

    if(pstAvStream->u64Pts == 0){
        pstAvStream->u64Pts = stVideoFrame->pstPack->u64PTS;
    }

    int64_t pts = (int64_t)((stVideoFrame->pstPack->u64PTS - pstAvStream->u64Pts)*90);
    //printf("U64PTS %lld, pts %lld\n",stVideoFrame->pstPack->u64PTS,pts);

    pkt->buf = NULL;
    pkt->dts = pkt->pts = pts;
    pkt->data = stVideoFrame->pstPack->pu8Addr;
    pkt->size = stVideoFrame->pstPack->u32Len;
    pkt->stream_index = pAvStream->index;
    
    pkt->flags = 0;
    // if( stVideoFrame->pstPack-> ) {
    //     pkt.flags |= AV_PKT_FLAG_KEY;
    // }
    pkt->side_data = NULL;
    pkt->side_data_elems = 0;
    pkt->duration = 0;
    pkt->pos = -1;
    pthread_mutex_lock(&context->muxerMutex);
    int ret = av_interleaved_write_frame(context->stMuxerFormat, pkt);
    pthread_mutex_unlock(&context->muxerMutex);
    av_packet_free(&pkt);
    if( ret ) {
        printf("av_interleaved_write_frame video failed %d\n", ret);
        return AR_FAILURE;
    }
    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_MUXER_InitMuxer(MUXER_CONTEXT_T* context, const AR_CHAR* szFullName)
{
    memset(context, 0, sizeof(*context));

    sprintf(context->szFullName,"%s",szFullName);

    avformat_alloc_output_context2(&(context->stMuxerFormat), NULL, "mp4", szFullName);
    if(NULL == context->stMuxerFormat){
        printf("alloc muxer format context failed\n");
        return AR_FAILURE;
    }
    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_MUXER_AddVideoStream(MUXER_CONTEXT_T* context, AR_S32 s32EncChn, PAYLOAD_TYPE_E enPayload, AR_S32 s32VideoWidth, AR_S32 s32VideoHeight, AR_S32 s32VideoBitrate, AR_U8* u8ExtraData, AR_S32 s32ExtraSize)
{
    if(context->s32AvStreamCount >= MUXER_MAX_AVSTREAM){
        return AR_FAILURE;
    }
    MUXER_AVSTREAM_T* pstAvStream = &context->stAvStreams[context->s32AvStreamCount];
    pstAvStream->s32EncChn = s32EncChn;
    pstAvStream->eMediaType = AVMEDIA_TYPE_VIDEO;
    pstAvStream->stContext = context;
    pstAvStream->stStream = SAMPLE_COMM_MUXER_AllocStream(context,context->stMuxerFormat, enPayload, s32VideoWidth, s32VideoHeight, s32VideoBitrate, 0, 0, 0, u8ExtraData, s32ExtraSize);
    if(!pstAvStream->stStream){
        return AR_FAILURE;
    }
    pstAvStream->s32Valid = 1;
    context->s32AvStreamCount++;
    return AR_SUCCESS;
}


AR_S32 SAMPLE_COMM_MUXER_AddAudioStream(MUXER_CONTEXT_T* context, AR_S32 s32EncChn, PAYLOAD_TYPE_E enPayload, AUDIO_SAMPLE_RATE_E enSampleRate, AUDIO_SOUND_MODE_E enSoundMode, AUDIO_BIT_WIDTH_E enBitWidth, AR_U8* u8ExtraData, AR_S32 s32ExtraSize)
{
    if(context->s32AvStreamCount >= MUXER_MAX_AVSTREAM){
        return AR_FAILURE;
    }
    MUXER_AVSTREAM_T* pstAvStream = &context->stAvStreams[context->s32AvStreamCount];
    pstAvStream->s32EncChn = s32EncChn;
    pstAvStream->eMediaType = AVMEDIA_TYPE_AUDIO;
    pstAvStream->stContext = context;
    pstAvStream->stStream = SAMPLE_COMM_MUXER_AllocStream(context,context->stMuxerFormat, enPayload, 0, 0, 0, enSampleRate, enSoundMode, enBitWidth, u8ExtraData, s32ExtraSize);
    if(!pstAvStream->stStream){
        return AR_FAILURE;
    }
    pstAvStream->s32Valid = 1;
    context->s32AvStreamCount++;
    return AR_SUCCESS;
}


AR_S32 SAMPLE_COMM_MUXER_StartPack(MUXER_CONTEXT_T* context)
{
    if(!context->stMuxerFormat){
        return AR_FAILURE;
    }

    if (!(context->stMuxerFormat->flags & AVFMT_NOFILE)) {
        int ret = avio_open(&context->stMuxerFormat->pb, context->szFullName, AVIO_FLAG_WRITE);
        if (ret < 0) {
            printf("Could not open output file '%s'\n", context->szFullName);
            return AR_FAILURE;
        }
    }
    if(avformat_write_header(context->stMuxerFormat, NULL) < 0){
        return AR_FAILURE;
    }
    av_dump_format(context->stMuxerFormat, 0, context->szFullName, 1);

    context->boolRunFlag = AR_TRUE;

    pthread_mutex_init(&context->muxerMutex, NULL);

    for(int i = 0; i < context->s32AvStreamCount; i++){
        if(context->stAvStreams[i].eMediaType == AVMEDIA_TYPE_VIDEO){
            if(pthread_create(&context->muxerThreadIds[0], 0, MUXER_ProcessVideoThread, context) != 0){
                printf("create muxer thread fail: %d\n",errno);
                return AR_FAILURE;   
            }
            break;
        }
    }

    for(int i = 0; i < context->s32AvStreamCount; i++){
        if(context->stAvStreams[i].eMediaType == AVMEDIA_TYPE_AUDIO){
            if(pthread_create(&context->muxerThreadIds[i+1],0,MUXER_ProcessAudioThread, &context->stAvStreams[i])){
                printf("create muxer audio thread failed: %d\n",errno);
            }
        }
    }
    return AR_SUCCESS;
}


AR_S32 SAMPLE_COMM_MUXER_StopPack(MUXER_CONTEXT_T* context)
{
    if(NULL == context->stMuxerFormat){
        return AR_FAILURE;
    }

    context->boolRunFlag = AR_FALSE;
    for(int i = 0; i < MUXER_MAX_AVSTREAM; i++){
        if(context->muxerThreadIds[i]){
            pthread_join(context->muxerThreadIds[i], NULL);
        }
    }
    pthread_mutex_destroy(&context->muxerMutex);

    av_write_trailer(context->stMuxerFormat);

    if (context->stMuxerFormat && !(context->stMuxerFormat->flags & AVFMT_NOFILE))
        avio_closep(&context->stMuxerFormat->pb);

    avformat_free_context(context->stMuxerFormat);
    context->stMuxerFormat = NULL;
    return AR_SUCCESS;
}

static AR_VOID* MUXER_ProcessAudioThread(AR_VOID* arg)
{
    AR_S32 s32Ret;
    MUXER_AVSTREAM_T* avstream = (MUXER_AVSTREAM_T*)(arg);
    MUXER_CONTEXT_T* context = avstream->stContext;

    printf("Enter Audio %d muxer thread\n",avstream->s32EncChn);

    while (context->boolRunFlag)
    {
        AUDIO_STREAM_S stAencStream;
        memset(&stAencStream,0,sizeof(stAencStream));
        s32Ret = AR_MPI_AENC_GetStream(avstream->s32EncChn, &stAencStream, 1000);
        if(AR_SUCCESS != s32Ret){
            printf("AR_MPI_AENC_GetStream failed with %#x!\n", s32Ret);
            continue;
        }
        s32Ret = SAMPLE_COMM_MUXER_WriteAudioFrame(context, avstream, &stAencStream);
        if(AR_SUCCESS != s32Ret){
            printf("MUXER Write Audio Frame Failed\n");
        }
        s32Ret = AR_MPI_AENC_ReleaseStream(avstream->s32EncChn, &stAencStream);
    }
    printf("Exit Audio %d muxer thread\n",avstream->s32EncChn);
}


static AR_VOID* MUXER_ProcessVideoThread(AR_VOID* arg)
{
    AR_S32 s32Ret;
    AR_S32 s32FdCount = 0;
    AR_S32 s32FdSet[MUXER_MAX_AVSTREAM] = {-1};
    AR_S32 s32FdSlot[MUXER_MAX_AVSTREAM] = {-1};
    struct timeval stTimeout;
    fd_set readFdSets;

    MUXER_CONTEXT_T* context = (MUXER_CONTEXT_T*)(arg);

    printf("Muxer process stream count %d\n",context->s32AvStreamCount);

    for(int i = 0; i < context->s32AvStreamCount; i++){
        if(context->stAvStreams[i].eMediaType == AVMEDIA_TYPE_VIDEO){
            s32FdSet[s32FdCount] = AR_MPI_VENC_GetFd(context->stAvStreams[i].s32EncChn);
            s32FdSlot[s32FdCount] = i;
            s32FdCount++;
        }
    }

    int maxFd = -1;

    while (context->boolRunFlag)
    {
        maxFd = -1;
        FD_ZERO(&readFdSets);
        for(int i = 0; i < s32FdCount; i++){
            FD_SET(s32FdSet[i], &readFdSets);
            if(maxFd < s32FdSet[i]) maxFd = s32FdSet[i];
        }
        
        stTimeout.tv_sec = 0;
        stTimeout.tv_usec = 100*1000;

        s32Ret = select(maxFd+1, &readFdSets, NULL, NULL, &stTimeout);
        if (s32Ret < 0)
        {
            printf("select failed!\n");
            break;
        }
        else if (s32Ret == 0)
        {
            printf("get venc stream time out, exit thread\n");
            continue;
        }
        else
        {
            for(int i = 0; i < s32FdCount; i++){
                if(!FD_ISSET(s32FdSet[i], &readFdSets)){
                    continue;
                }
                MUXER_AVSTREAM_T* pstAvStream = &context->stAvStreams[s32FdSlot[i]];
                if(pstAvStream->eMediaType == AVMEDIA_TYPE_VIDEO){
                    VENC_CHN_STATUS_S stChnStatus;
                    VENC_STREAM_S stVencStream;
                    s32Ret = AR_MPI_VENC_QueryStatus(context->videoEncChn, &stChnStatus);
                    if(0 == stChnStatus.u32CurPacks){
                          printf("NOTE: Current  frame is NULL!\n");
                          continue;
                    }
                    /*******************************************************
                     step 2.3 : malloc corresponding number of pack nodes.
                    *******************************************************/
                    stVencStream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S) * stChnStatus.u32CurPacks);
                    if (NULL == stVencStream.pstPack)
                    {
                        printf("malloc stream pack failed!\n");
                        break;
                    }

                    /*******************************************************
                     step 2.4 : call mpi to get one-frame stream
                    *******************************************************/
                    stVencStream.u32PackCount = stChnStatus.u32CurPacks;
                    s32Ret = AR_MPI_VENC_GetStream(pstAvStream->s32EncChn, &stVencStream, 0);
                    if (AR_SUCCESS != s32Ret)
                    {
                        free(stVencStream.pstPack);
                        stVencStream.pstPack = NULL;
                        printf("AR_MPI_VENC_GetStream failed with %#x!\n", s32Ret);
                        continue;
                    }
                    s32Ret = SAMPLE_COMM_MUXER_WriteVideoFrame(context, pstAvStream, &stVencStream);
                    if(AR_SUCCESS != s32Ret){
                        printf("MUXER WriteVideoFrame failed with %#x!\n", s32Ret);
                    }

                    s32Ret = AR_MPI_VENC_ReleaseStream(pstAvStream->s32EncChn, &stVencStream);
                    free(stVencStream.pstPack);
                }else{
                    continue;
                }
            }
        }
    }
    printf("Muxer thread exit!!!!!\n");
    // Close Fd
    return NULL;
}
