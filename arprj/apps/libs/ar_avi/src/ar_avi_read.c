#include "ar_avi_api.h"
#include "ar_avi_utility.h"

int avi_flag_pause = 0;
//pthread_mutex_t avi_pb_speed_x;
int avi_pb_speed = 1;

#define PB_N_LOOP 10
#define PB_DEBUG 0

/**********************************************************************
函数描述：返回 n1 - n2 的值
入口参数：AviPBHandle *pPBHandle,AVI_DMS_TIME *ptime
返回值：  计算指定时间到文件开始时间的长度
**********************************************************************/
int cal_pb_seek_time(AviPBHandle *pPBHandle,AVI_DMS_TIME *ptime);
int avi_pb_close(AviPBHandle *pPBHandle);
int avi_pb_slide_to_next_file(AviPBHandle *pPBHandle);

static int is_empty_list(FILE_LIST *list)
{
    return NULL == list->head->next;
}

static int get_len_list(FILE_LIST *list)
{
    if (list == NULL)
        return 0;

    int i = 0;
    FILE_NODE *p;
    p = list->head;
    while (p->next != NULL)
    {
        i++;
        p = p->next;
    }
    return i;
}

static void delete_node(FILE_LIST *list, int index)
{
    if(is_empty_list(list))
    {
        return;
    }

    FILE_NODE *p, *q;

    p = list->head;

    if(index >= get_len_list(list) || index < 0)
    {
        return;
    }


    while(index--)
        p = p->next;

    q = p->next;
    p->next = q->next;

#if _DEBUG_MSG_ON_
    print_node(q);
    PRINT_INFO("len = %lu\n", list->len);
#endif

    q->next = NULL;
    free(q);

    list->len --;
}

/**********************************************************************
函数描述：查找文件结束
入口参数：FILE_LIST *: 查找文件的控制句柄
返回值：  无
**********************************************************************/
void search_close(FILE_LIST *list)
{
    if (list == NULL)
        return;

    while(list->len)
        delete_node(list, 0);

    list->head->next = NULL;
    free(list->head);

    list->head = NULL;
    free(list);
}

static AR_U32 avi_get_u32(void *pp)
{
    char *p = pp;
    AR_U32 tmp32 = (AR_U32)((p[0])|(p[1])<<8|(p[2])<<16|(p[3])<<24);
    return tmp32;
}

static int check_avi_header_fcc(AviHeader *avi_header)
{
   if(avi_header->fcc_riff != MAKE_FOURCC('R','I','F','F'))
        return -1;

    if(avi_header->fcc_avi != MAKE_FOURCC('A','V','I',' '))
        return -1;

    if(avi_header->fcc_list_hdrl != MAKE_FOURCC('L','I','S','T'))
        return -1;

    if(avi_header->fcc_hdrl != MAKE_FOURCC('h','d','r','l'))
        return -1;

    if(avi_header->fcc_list_strl_video != MAKE_FOURCC('L','I','S','T'))
        return -1;
    if(avi_header->fcc_strl_video != MAKE_FOURCC('s','t','r','l'))
        return -1;
    if(avi_header->fcc_strf_video != MAKE_FOURCC('s','t','r','f'))
        return -1;

    if(avi_header->fcc_list_strl_audio != MAKE_FOURCC('L','I','S','T'))
        return -1;
    if(avi_header->fcc_strl_audio != MAKE_FOURCC('s','t','r','l'))
        return -1;
    if(avi_header->fcc_strf_audio != MAKE_FOURCC('s','t','r','f'))
        return -1;

    if(avi_header->fcc_list_movi != MAKE_FOURCC('L','I','S','T'))
        return -1;
    if(avi_header->fcc_movi != MAKE_FOURCC('m','o','v','i'))
        return -1;

    return 0;
}

/**********************************************************************
函数描述：读取avi文件的头信息
入口参数：AviFile *avi_file: avi文件的控制句柄
          AviInitParam *param: avi文件的设置参数
返回值：  <0: 错误
           0: 正常
**********************************************************************/
static int avi_read_header_and_index(AviPBHandle *pPBHandle)
{
    int tmp32;
    int ret;

    if(!pPBHandle)
    {
        printf("parameter error.\n");
        return -1;
    }

    AviHeader avi_header;
    ret = fread(&avi_header, sizeof(AviHeader), 1, pPBHandle->file);
    if(ret != 1)
    {
        printf("read avi header failed. ret = %d\n", ret);
        return -1;
    }

    ret = check_avi_header_fcc(&avi_header);
    if(ret != 0)
    {
        printf("avi header fcc error.\n");
        return -1;
    }

    if (avi_header.main_header.dwMicroSecPerFrame <= 0)
    {
        printf("dwMicroSecPerFrame %d error.\n", avi_header.main_header.dwMicroSecPerFrame);
        return -1;
    }

    //param->bps = avi_header.main_header.dwMaxBytesPerSec * 8;
    if(1000000/25 == avi_header.main_header.dwMicroSecPerFrame)
        pPBHandle->fps = 25;
    else
        pPBHandle->fps = 1000000 / avi_header.main_header.dwMicroSecPerFrame;
    printf("fps = %d \n", pPBHandle->fps);

    pPBHandle->video_width = avi_header.main_header.dwWidth;
    pPBHandle->video_height = avi_header.main_header.dwHeight;
    pPBHandle->v_enc_type = avi_header.video_header.fccHandler;   //H264,H265
    pPBHandle->bps = avi_header.main_header.dwMaxBytesPerSec * 8 /1000;

    //update avi_file infomation
    pPBHandle->video_count = avi_header.main_header.dwTotalFrames;
    //pPBHandle->audio_bytes = avi_header.audio_header.dwLength * avi_header.audio_header.dwScale;
    //pPBHandle->data_offset = sizeof(AviHeader);

    //rec: int movi_len = avi_file->data_offset - sizeof(AviHeader) + 4;
    ret = fseek(pPBHandle->file, avi_header.movi_len - 4 + sizeof(AviHeader), SEEK_SET);
    if(ret != 0)
    {
        printf("fseek failed.\n");
        return -1;
    }

    //printf("movi_len = %d.\n", avi_header.movi_len);
    //printf("index offset = %d, data start = %d \n", avi_header.movi_len - 4 + sizeof(AviHeader), sizeof(AviHeader));
    //ret = fread(&tmp32, sizeof(int), 1, avi_file->file);
    int fcc;
    ret = AR_AVI_ReadFourcc(pPBHandle->file, &fcc);
    if(ret != 1)
    {
        printf("read fcc idx1 failed. ret = %d\n", ret);
        return -1;
    }
    if(fcc != MAKE_FOURCC('i','d','x','1'))
    {
        printf("fcc %08x is not idx1.\n", fcc);
        return -1;
    }


    ret = fread(&tmp32, sizeof(int), 1, pPBHandle->file);
    if(ret != 1)
    {
        printf("read index size failed.\n");
        return -1;
    }
    pPBHandle->index_count = tmp32 / 16;
    printf("index count = %d\n", pPBHandle->index_count);

    //read index to mem
    pPBHandle->idx_array_count = pPBHandle->index_count;
    pPBHandle->idx_array = (int *)malloc((pPBHandle->idx_array_count) * 16);
    if(pPBHandle->idx_array == NULL)
    {
        printf("avi_read_header, malloc index failed.\n");
        return -1;
    }

    ret = fread(pPBHandle->idx_array, pPBHandle->index_count*16, 1, pPBHandle->file);
    if(ret != 1)
    {
        printf("avi_read_header, fread error.\n");
        return -1;
    }

    printf("avi_read_header_and_index ok.\n");
    return 0;
}


int AR_AVI_LoadFile(char *file_name, AviPBHandle *pPBHandle)
{
    int ret;

    if(!pPBHandle || !file_name)
    {
        printf("parameter is error.\n");
        return -1;
    }

    if(pPBHandle->file)
    {
        fclose(pPBHandle->file);
        pPBHandle->file = NULL;
    }
    if(pPBHandle->idx_array)
    {
        free(pPBHandle->idx_array);
        pPBHandle->idx_array = NULL;
    }

    //open file
    pPBHandle->file = fopen(file_name, "rb+");
    if(NULL == pPBHandle->file)
    {
        printf("open %s failed.\n", file_name);
        return -1;
    }

    //read avi header
    ret = avi_read_header_and_index(pPBHandle);
    if(ret < 0)
    {
        printf("call avi_read_header failed.\n");
        return -1;
    }

    pPBHandle->pb_buf_index = 0;
    pPBHandle->pb_buf_pos = 0;
    pPBHandle->no = 0;

    printf("avi_pb_open ok.\n");
    return 0;
}



#if 0
/**********************************************************************
函数描述：通过AVI文件的控制句柄和参数信息，来设置帧数据的控制句柄
入口参数：AviFile *avi_file: avi文件的控制句柄
          AviInitParam *param: avi文件中的参数信息
          AviFrameInfo *p_frame: avi文件中帧数据的控制句柄
返回值：  无
**********************************************************************/
void set_frame_info(AviFile *avi_file, AviInitParam *param, AviFrameInfo *p_frame)
{
    memset(p_frame->file_path, 0, sizeof(p_frame->file_path));
    strcpy(p_frame->file_path, avi_file->filename);
    p_frame->video_count = avi_file->video_count;
    p_frame->index_count = avi_file->index_count;
    p_frame->fps = param->fps;
    p_frame->video_width = param->width;
    p_frame->video_height = param->height;
}
#endif

/**********************************************************************
函数描述：打印出avi文件中帧数据所在的avi文件中的信息
入口参数：AviPBHandle*pPBHandle: avi文件中帧数据的控制句柄
返回值：  无
**********************************************************************/
void print_pb_handle(AviPBHandle*pPBHandle)
{
    printf("******** PRINT PB HANDLE *******\n");
    printf("file_path = %s\n", pPBHandle->file_path);
    printf("video count = %d\n", pPBHandle->video_count);
    printf("index count = %d\n", pPBHandle->index_count);
    printf("no = %d\n", pPBHandle->no);
    printf("fps = %d\n", pPBHandle->fps);
    printf("video width  = %d\n", pPBHandle->video_width);
    printf("video height = %d\n", pPBHandle->video_height);
    if(pPBHandle->list == NULL)
        printf("just signal file, not use file list\n");
    else
        printf("more than one file, use file list\n");

    printf("******** PRINT PB HANDLE *******\n\n");
}

/**********************************************************************
函数描述：打印出avi文件中帧数据的信息
入口参数：AviFrameInfo *p: avi文件中帧数据的控制句柄
返回值：  无
**********************************************************************/
void print_frame_node(AviPBHandle *p)
{
    printf("\n******** PRINT FRAME NODE *******\n");
    printf("frame_type = %d\n", p->node.frame_type);
    printf("offset = %d\n", p->node.offset);
    printf("size = %d\n", p->node.size);
    printf("timetick = %u\n", p->node.timetick);

    printf("******** PRINT FRAME NODE *******\n\n");
}

/**********************************************************************
函数描述：通过AVI文件的控制句柄和参数信息, 计算出该百分比位置是第几帧
          数据，并更新到 AviFrameInfo 的 no 参数中
入口参数：AviFile *avi_file: avi文件的控制句柄
          AviInitParam *param: avi文件中的参数信息
          int percent: 百分比，如25%，percent = 25
          AviFrameInfo *p_frame: avi文件中帧数据的控制句柄
返回值：  <0: 错误
           0: 正常
**********************************************************************/
int avi_pb_seek_by_percent(AviFile *avi_file, AviInitParam *param, int percent, AviPBHandle *pPBHandle)
{
    int tmp32 = 0;

    if( (!avi_file) || (!param))
    {
        printf("parameter error.\n");
        return -1;
    }

    // if(0 == is_avi_file_path_format(avi_file->filename))
    // {
    //     printf("%s is not avi file path format.\n", avi_file->filename);
    //     return -1;
    // }

    tmp32 = (int)(avi_file->video_count * percent / (100 * param->fps) );
    pPBHandle->no = tmp32 * param->fps;

    return 0;
}

/**********************************************************************
函数描述：通过AVI文件的控制句柄和参数信息, 计算出该定位时间是第几帧
          数据，并更新到 AviFrameInfo 的 no 参数中
入口参数：AviFile *avi_file: avi文件的控制句柄
          AviInitParam *param: avi文件中的参数信息
          char *seek_time: 通过时间在进度条中定位
          AviFrameInfo *p_frame: avi文件中帧数据的控制句柄
返回值：  <0: 错误
           0: 正常
**********************************************************************/
int avi_pb_seek_by_time(AviFile *avi_file, AviInitParam *param, char *seek_time, AviPBHandle *pPBHandle)
{

    char str_tmp[10];
    char avi_name[128];
    time_t timep1, timep2;
    struct tm *p1;
    struct tm *p2;
    int duration = 0;
    int i;


    time(&timep1);
    time(&timep2);
    struct tm tt1 = {0};
    p1 = localtime_r(&timep1, &tt1);
    struct tm tt2 = {0};
    p2 = localtime_r(&timep2, &tt2);

    //int fps = param->fps;

    if(!avi_file || !param)
    {
        printf("parameter error.\n");
        return -1;
    }

    memset(avi_name, 0 ,sizeof(avi_name));
    i = AR_AVI_FindLastIndex(avi_file->filename, '/');
    strcpy(avi_name, avi_file->filename + i + 1);

    printf("avi_pb_seek_by_time avi_name=%s\n",avi_name);
    AR_AVI_ExtractSubstr(str_tmp, avi_name, 4, 4+4);
    p1->tm_year = atoi(str_tmp) - 1900;
    AR_AVI_ExtractSubstr(str_tmp, avi_name, 8, 8+2);
    p1->tm_mon = atoi(str_tmp) - 1;
    AR_AVI_ExtractSubstr(str_tmp, avi_name, 10, 10+2);
    p1->tm_mday = atoi(str_tmp);
    AR_AVI_ExtractSubstr(str_tmp, avi_name, 12, 12+2);
    p1->tm_hour = atoi(str_tmp);
    AR_AVI_ExtractSubstr(str_tmp, avi_name, 14, 14+2);
    p1->tm_min = atoi(str_tmp);
    AR_AVI_ExtractSubstr(str_tmp, avi_name, 16, 16+2);
    p1->tm_sec = atoi(str_tmp);

    AR_AVI_ExtractSubstr(str_tmp, seek_time, 0, 0+4);
    p2->tm_year = atoi(str_tmp) - 1900;
    AR_AVI_ExtractSubstr(str_tmp, seek_time, 4, 4+2);
    p2->tm_mon = atoi(str_tmp) - 1;
    AR_AVI_ExtractSubstr(str_tmp, seek_time, 6, 6+2);
    p2->tm_mday = atoi(str_tmp);
    AR_AVI_ExtractSubstr(str_tmp, seek_time, 8, 8+2);
    p2->tm_hour = atoi(str_tmp);
    AR_AVI_ExtractSubstr(str_tmp, seek_time, 10, 10+2);
    p2->tm_min = atoi(str_tmp);
    AR_AVI_ExtractSubstr(str_tmp, seek_time, 12, 12+2);
    p2->tm_sec = atoi(str_tmp);

    timep1 = mktime(p1);
    timep2 = mktime(p2);
    duration = timep2 - timep1;
    pPBHandle->video_no = duration * param->fps;

    return 0;
}

static int avi_read_data_to_buf(AviPBHandle *pPBHandle)
{
    //避免定位后计算出来的no超出范围，如果超出范围读取最后一帧
    if (pPBHandle->no >= pPBHandle->index_count)
        pPBHandle->no = pPBHandle->index_count - 1;

    int ret;
    int bytes_loop = 0;
    int index_tmp = pPBHandle->no;
    pPBHandle->pb_buf_index = pPBHandle->no;
    pPBHandle->pb_buf_pos = 0;

    if(index_tmp >= (pPBHandle->index_count-1))
    {
        bytes_loop = pPBHandle->idx_array[4*(pPBHandle->index_count - 1)+3];
        pPBHandle->pb_buf_index = pPBHandle->index_count;
    }
    else
    {
        bytes_loop = pPBHandle->idx_array[4*(index_tmp+1) + 2] - pPBHandle->idx_array[4*index_tmp+2];
        pPBHandle->pb_buf_index = pPBHandle->no + 1;
    }


    if(bytes_loop >= pPBHandle->pb_buf_size)
    {
        if(pPBHandle->pb_buf == NULL)
        {
            pPBHandle->pb_buf = malloc(bytes_loop + 60*1024);
            if(pPBHandle->pb_buf == NULL)
            {
                printf("Malloc read frame buffer(%dKB) failed",(bytes_loop+60*1024)>>10);
                pPBHandle->pb_buf_size = 0;
                return -1;
            }
        }
        else
        {
            pPBHandle->pb_buf = realloc(pPBHandle->pb_buf, bytes_loop + 60*1024);
            if(pPBHandle->pb_buf == NULL)
            {
                printf("Realloc read frame buffer(%dKB) failed",(bytes_loop+60*1024)>>10);
                pPBHandle->pb_buf_size = 0;
                return -1;
            }

            printf("Remalloc read frame buffer(%dKB) ",(bytes_loop+60*1024)>>10);
        }
        pPBHandle->pb_buf_size = (bytes_loop + 60*1024);
    }

    memset(pPBHandle->pb_buf, 0, pPBHandle->pb_buf_size);
    fseek(pPBHandle->file, pPBHandle->idx_array[4*index_tmp + 2], SEEK_SET);
    ret = fread(pPBHandle->pb_buf, 1, bytes_loop, pPBHandle->file);
    if(ret != bytes_loop)
    {
        printf("fread error. read %d bytes, cal %d byets\n", ret, bytes_loop);
        return -1;
    }

    return 0;
}


/**********************************************************************
函数描述：得到一帧数据
入口参数：AviPBHandle *pPBHandle: avi文件中一帧数据的相关信息的指针
返回值：  <0: 错误
           0: 正常
**********************************************************************/
int avi_get_frame_loop(AviPBHandle *pPBHandle)
{
    int ret;

    if(!pPBHandle)
    {
        printf("parameter error.\n");
        return -1;
    }

    if ((pPBHandle->no >= pPBHandle->pb_buf_index))
    {
        ret = avi_read_data_to_buf(pPBHandle);
        if(ret < 0)
        {
            printf("call avi_read_data_to_buf failed");
            return -1;
        }
    }
	else
	{
		printf("current frame index is small to the last one\n");
		return -1;
	}

    //fcc32 00dc | 00wb
    int fcc32 = AR_AVI_Read32(pPBHandle->pb_buf + pPBHandle->pb_buf_pos);
    if(fcc32 != pPBHandle->idx_array[4 * pPBHandle->no])
    {
        printf("fcc is not equal.\n");
        return  -1;
    }
    
    pPBHandle->pb_buf_pos += 4;

    int frame_size = AR_AVI_Read32(pPBHandle->pb_buf + pPBHandle->pb_buf_pos);
    if(frame_size != pPBHandle->idx_array[4*pPBHandle->no+3])
    {
        printf("size is not equal.\n");
        printf("buf_size = %d, index size = %d \n", frame_size, pPBHandle->idx_array[4*pPBHandle->no+3]);
        return -1;
    }
    pPBHandle->pb_buf_pos += 4;
	
    /* get time stamp*/
    int tmp32 = AR_AVI_Read32(pPBHandle->pb_buf + pPBHandle->pb_buf_pos);
    if(tmp32 == MAKE_FOURCC('J','U','N','K'))
    {    
        pPBHandle->pb_buf_pos += 12;
        //printf("JUNK timetick = %u\n", timetick);
    }

	pPBHandle->node.size = frame_size;
    pPBHandle->node.one_frame_buf = pPBHandle->pb_buf + pPBHandle->pb_buf_pos;
    pPBHandle->pb_buf_pos += frame_size;

    if(fcc32 == MAKE_FOURCC('0','0','d','c'))
    {
        pPBHandle->video_no ++;
		pPBHandle->node.frame_type = 1;
    }
    else
    {
        pPBHandle->audio_no ++;
		pPBHandle->node.frame_type = 0;
    }

    pPBHandle->no ++;
    return 0;
}

/**********************************************************************
函数描述：得到一帧数据
入口参数：AviPBHandle *pPBHandle: avi文件中一帧数据的相关信息的指针
返回值：  <0: 错误
           0: 正常
**********************************************************************/
int avi_get_frame_by_no(AviPBHandle *pPBHandle, char *p_buff, int *p_size)
{
    int ret;

    if(!pPBHandle)
    {
        printf("parameter error.\n");
        return -1;
    }

    /* 计算offset 和 size */
    int frame_size = pPBHandle->idx_array[4*pPBHandle->no+3];
    //printf("frame_size : %d\n", frame_size);
    int one_len = 8+frame_size+12;

    /* 读入数据 */

    if(one_len >= pPBHandle->pb_buf_size)
    {
        if(pPBHandle->pb_buf!= NULL)
            free(pPBHandle->pb_buf);
        pPBHandle->pb_buf = malloc(one_len+60*1024);
        if(pPBHandle->pb_buf == NULL)
        {
            printf("Malloc read frame buffer(%dKB) failed",(one_len+60*1024)>>10);
            return -1;
        }
        else
        {
            printf("REmalloc read frame buffer(%dKB) ",(one_len+60*1024)>>10);
        }
        pPBHandle->pb_buf_size = (one_len+60*1024);
    }


    int offset = pPBHandle->idx_array[4*pPBHandle->no+2];
    fseek(pPBHandle->file, offset, SEEK_SET);
    ret = fread(pPBHandle->pb_buf, 1, one_len, pPBHandle->file);
    if(ret != one_len)
    {
        printf("fread error. read %d bytes, cal %d byets\n", ret, one_len);
        return -1;
    }

    // 读fcc
    pPBHandle->pb_buf_pos = 0;
    int fcc32 = AR_AVI_Read32(pPBHandle->pb_buf + pPBHandle->pb_buf_pos);
    if(fcc32 != pPBHandle->idx_array[4*pPBHandle->no])
    {
        printf("fcc is not equal.\n");
        printf("streamid = 0x%08x, index fcc = 0x%08x \n", fcc32, pPBHandle->idx_array[4*pPBHandle->no]);
        return  -1;
    }
	#if 0
    if(fcc32 != MAKE_FOURCC('0','0','d','c'))
    {
        printf("fcc error.\n");
        return  -1;
    }
	#endif
    pPBHandle->pb_buf_pos += 4;

    // 读frame_size
    frame_size = AR_AVI_Read32(pPBHandle->pb_buf + pPBHandle->pb_buf_pos);
    if(frame_size != pPBHandle->idx_array[4*pPBHandle->no+3])
    {
        printf("size is not equal.\n");
        printf("buf_size = %d, index size = %d \n", frame_size, pPBHandle->idx_array[4*pPBHandle->no+3]);
        return -1;
    }
    pPBHandle->pb_buf_pos += 4;
    *p_size = frame_size;

    //p_buff 得到这一帧数据
    if(frame_size > ONE_FRAME_BUF_SIZE)
    {
        printf("ONE_FRAME_BUF_SIZE is too small, frame size = %d.\n", frame_size);
        return -1;
    }
    //memcpy(p_buff, (char *)(pPBHandle->pb_buf + pPBHandle->pb_buf_pos), frame_size);
    pPBHandle->node.one_frame_buf = (char *)(pPBHandle->pb_buf + pPBHandle->pb_buf_pos);
    pPBHandle->pb_buf_pos += frame_size;

    /* get time stamp*/
    int tmp32 = AR_AVI_Read32(pPBHandle->pb_buf + pPBHandle->pb_buf_pos);
    if(tmp32 != MAKE_FOURCC('J','U','N','K'))
    {
        printf("fcc junk error.\n");
        return  -1;
    }
    pPBHandle->pb_buf_pos += 4;
    //printf("JUNK fcc = 0x%08x\n", tmp32);

    tmp32 = AR_AVI_Read32(pPBHandle->pb_buf + pPBHandle->pb_buf_pos);
    if(tmp32 != 4)
    {
        printf("junk size is not 4.\n");
        return -1;
    }
    pPBHandle->pb_buf_pos += 4;
    //printf("JUNK size = %d\n", tmp32);

    AR_U32 timetick = avi_get_u32(pPBHandle->pb_buf + pPBHandle->pb_buf_pos);
    pPBHandle->pb_buf_pos += 4;
    //printf("JUNK timetick = %u\n", timetick);

    pPBHandle->node.frame_type = pPBHandle->idx_array[4*pPBHandle->no+1];
    pPBHandle->node.offset = pPBHandle->idx_array[4*pPBHandle->no+2];
    pPBHandle->node.size = pPBHandle->idx_array[4*pPBHandle->no+3];  //not use

    if(fcc32 == MAKE_FOURCC('0','0','d','c'))
    {
        //cal_timetick_video(p_frame);
        pPBHandle->video_no ++;
        pPBHandle->node.timetick = timetick;
        pPBHandle->pb_timetick_tmp = timetick;
    }
    else
    {
        //cal_timetick_audio(p_frame);
        pPBHandle->audio_no ++;
        pPBHandle->node.timetick = pPBHandle->pb_timetick_tmp;
        //p_frame->node.timetick = timetick;
    }

    //printf("no = %d, frame_size = %d\n", pPBHandle->no, *p_size);
    pPBHandle->no ++;
    pPBHandle->pb_buf_index = pPBHandle->no;
    return 0;
}


/**********************************************************************
函数描述：返回 n1 - n2 的值
入口参数：u64t n1: 操作数1
          AR_U64 n2: 操作数2
返回值：  n1 - n2 的值，可正可负，单位为秒
**********************************************************************/
static int cal_del(AR_U64 n1, AR_U64 n2)
{
    int r;

    time_t timep1, timep2;
    struct tm *p1;
    struct tm *p2;


    AVI_DMS_TIME t1, t2;
    memset(&t1, 0, sizeof(t1));
    memset(&t2, 0, sizeof(t2));
    AR_AVI_U64ToTime(&t1, n1);
    AR_AVI_U64ToTime(&t2, n2);

    time(&timep1);
    struct tm tm1 = {0};
    p1 = localtime_r(&timep1, &tm1);
    time(&timep2);
    struct tm tm2 = {0};
    p2 = localtime_r(&timep2, &tm2);

    p1->tm_year = t1.dwYear- 1900;
    p1->tm_mon = t1.dwMonth- 1;
    p1->tm_mday = t1.dwDay;
    p1->tm_hour = t1.dwHour;
    p1->tm_min = t1.dwMinute;
    p1->tm_sec = t1.dwSecond;

    p2->tm_year = t2.dwYear- 1900;
    p2->tm_mon = t2.dwMonth- 1;
    p2->tm_mday = t2.dwDay;
    p2->tm_hour = t2.dwHour;
    p2->tm_min = t2.dwMinute;
    p2->tm_sec = t2.dwSecond;

    timep1 = mktime(p1);
    timep2 = mktime(p2);

    r = timep1 - timep2;
    return r;
}


/**********************************************************************
函数描述：返回 n1 - n2 的值
入口参数：AviPBHandle *pPBHandle
返回值：  计算文件的总时长，单位为秒
**********************************************************************/
int cal_pb_all_time(AviPBHandle *pPBHandle)
{
    //printf("in cal_pb_all_time, file path: %s\n", pPBHandle->file_path);
    char file_name[128];
    memset(file_name, 0 ,sizeof(file_name));
    int i = AR_AVI_FindLastIndex(pPBHandle->file_path, '/');
    strcpy(file_name, pPBHandle->file_path + i + 1);
    char dir[32] = {0};
    memset(dir, 0 ,sizeof(dir));
    strncpy(dir, pPBHandle->file_path, i);
    //printf("in cal_pb_all_time, file name: %s\n", file_name);


    AR_U64 time_start_64 = AR_AVI_GetStartTimeFromName(dir, file_name);
    AR_U64 time_stop_64 = AR_AVI_GetStopTimeFromName(dir, file_name);
    //printf("in cal_pb_all_time, start time: %llu\n", time_start_64);
    //printf("in cal_pb_all_time, stop  time: %llu\n", time_stop_64);

    int ret = cal_del(time_stop_64, time_start_64);
    //printf("in cal_pb_all_time, all time: %d\n", ret);

    return ret;
}


#if 1
int get_file_node(FILE_LIST *list, FILE_NODE *node)
{

    if (list == NULL)
        return -1;

    if(NULL == list->head->next)
        return -1;

    FILE_NODE *p;
    AR_U32 index;
    p = list->head;
    index = list->index;

    if(index  == list->len)
       return -1;

    while(index--)
        p = p->next;

    strcpy(node->path, p->next->path);
    node->start = p->next->start;
    node->stop = p->next->stop;
    node->size = p->next->size;

    list->index ++;
    return 0;
}

/**********************************************************************
函数描述：返回 n1 - n2 的值
入口参数：AviPBHandle *pPBHandle,AVI_DMS_TIME *ptime
返回值：  计算指定时间到文件开始时间的长度
**********************************************************************/
int cal_pb_seek_time(AviPBHandle *pPBHandle,AVI_DMS_TIME *ptime)
{
    if(NULL == pPBHandle)
    {
        printf("pPBHandle is null!\n");
        return -1;
    }
    //printf("in cal_pb_all_time, file path: %s\n", pPBHandle->file_path);

    int r;
    time_t timep1, timep2 ,timep3;
    struct tm *p1,*p2,*p3;

    int ret = 0;
    char dir[32] = {0};
    char file_name[128] = {0};
    AR_U64 time_start_64 = 0;
    AR_U64 time_stop_64 = 0;
    AR_U64 cur_time_64 = AR_AVI_TimeToU64(ptime);

    if (pPBHandle->list)
        pPBHandle->list->index = 0;
    while (1)
    {
        //printf("pb to check file : %s, cur_time:%llu\n", pPBHandle->file_path, cur_time_64);
        int i = AR_AVI_FindLastIndex(pPBHandle->file_path, '/');
        strcpy(file_name, pPBHandle->file_path + i + 1);
        //printf("in cal_pb_all_time, file name: %s\n", file_name);
        memset(dir, 0 ,sizeof(dir));
        strncpy(dir, pPBHandle->file_path, i);

        time_start_64 = AR_AVI_GetStartTimeFromName(dir, file_name);
        time_stop_64 = AR_AVI_GetStopTimeFromName(dir, file_name);

        //printf("----------->time_start_64:%llu time_stop_64:%llu cur_time_64:%llu\n",
        //    time_start_64, time_stop_64, cur_time_64);

        if ((cur_time_64 >= time_start_64) && (cur_time_64 <= time_stop_64))
        {
            printf("cur_time_64:%llu, find file: %s\n", cur_time_64, pPBHandle->file_path);
            break;
        }

        if (pPBHandle->list == NULL)
        {
            printf("pPBHandle list is null!\n");
            return -1;
        }
        else
        {
            //读下一个文件
            FILE_NODE node;
            memset(&node, 0, sizeof(node));
            ret = get_file_node(pPBHandle->list, &node);
            if (ret)
            {
                printf("get_file_node failed, ret = %d\n", ret);
                return -1;
            }
            //print_node(&node);
            strcpy(pPBHandle->file_path, node.path);
        }
    }
    //printf("find file -- pb open file : %s\n", pPBHandle->file_path);


    AVI_DMS_TIME t1 = {0};
    AR_AVI_U64ToTime(&t1, time_start_64);
    AVI_DMS_TIME t2 = {0};
    AR_AVI_U64ToTime(&t2, time_stop_64);

    #if 0
    printf("----------->start %lu-%lu-%lu %lu:%lu:%lu\n",
        t1.dwYear,t1.dwMonth,t1.dwDay,t1.dwHour,t1.dwMinute,t1.dwSecond);

    printf("----------->stop %lu-%lu-%lu %lu:%lu:%lu\n",
        t2.dwYear,t2.dwMonth,t2.dwDay,t2.dwHour,t2.dwMinute,t2.dwSecond);

    printf("----------->setpos %lu-%lu-%lu %lu:%lu:%lu\n",
        ptime->dwYear,ptime->dwMonth,ptime->dwDay,ptime->dwHour,ptime->dwMinute,ptime->dwSecond);
    #endif

    /*初始化timep1*/
    time(&timep1);
    struct tm tm1 = {0};
    p1 = localtime_r(&timep1, &tm1);

    /*初始化timep2*/
    time(&timep2);
    struct tm tm2 = {0};
    p2 = localtime_r(&timep2, &tm2);

    /*初始化timep3*/
    time(&timep3);
    struct tm tm3 = {0};
    p3 = localtime_r(&timep3, &tm3);

    p1->tm_year = t1.dwYear- 1900;
    p1->tm_mon = t1.dwMonth- 1;
    p1->tm_mday = t1.dwDay;
    p1->tm_hour = t1.dwHour;
    p1->tm_min = t1.dwMinute;
    p1->tm_sec = t1.dwSecond;

    p2->tm_year = ptime->dwYear- 1900;
    p2->tm_mon = ptime->dwMonth- 1;
    p2->tm_mday = ptime->dwDay;
    p2->tm_hour = ptime->dwHour;
    p2->tm_min = ptime->dwMinute;
    p2->tm_sec = ptime->dwSecond;

    p3->tm_year = t2.dwYear- 1900;
    p3->tm_mon = t2.dwMonth- 1;
    p3->tm_mday = t2.dwDay;
    p3->tm_hour = t2.dwHour;
    p3->tm_min = t2.dwMinute;
    p3->tm_sec = t2.dwSecond;

    timep1 = mktime(p1);
    timep2 = mktime(p2);
    timep3 = mktime(p3);
    #if 0
    printf("----------->timep1 %d-%d-%d %d:%d:%d\n",
        p1->tm_year,p1->tm_mon,p1->tm_mday,p1->tm_hour,p1->tm_min,p1->tm_sec);

    printf("timep1:%ld\n",timep1);

    printf("----------->timep2 %d-%d-%d %d:%d:%d\n",
        p2->tm_year,p2->tm_mon,p2->tm_mday,p2->tm_min,p2->tm_min,p2->tm_sec);

    printf("timep2:%ld\n",timep2);

    printf("----------->timep3 %d-%d-%d %d:%d:%d\n",
        p3->tm_year,p3->tm_mon,p3->tm_mday,p3->tm_hour,p3->tm_min,p3->tm_sec);

    printf("timep3:%ld\n",timep3);

    printf("----------->\n");
    #endif

    r = timep2 - timep1;

    printf("----------->SEEK pos r:%d\n", r);
    if((r < 0) || r > timep3 - timep1)
    {
        printf("SEEK pos of file %d err,should between %lld to %lld !\n",r,time_start_64,time_stop_64);
        return -1;
    }

    return r;
}
#endif

/**********************************************************************
函数描述：将时间戳 timetick 转换成 AVI_DMS_TIME 类型的数据结构
入口参数：AVI_DMS_TIME *ptime: DMS_TIME 类型的指针，记录时间
          int timetick: 时间戳，毫秒为单位
返回值：  无
**********************************************************************/
void timetick_to_time(AVI_DMS_TIME *ptime, unsigned int timetick)
{
    struct tm *p;
    long timep = timetick /1000;

    //printf("timep4 = %d \n", timep);

    struct tm t1 = {0};
    p = gmtime_r(&timep, &t1);


    ptime->dwYear = p->tm_year + 1900;
    ptime->dwMonth = p->tm_mon + 1;
    ptime->dwDay = p->tm_mday;
    ptime->dwHour = p->tm_hour;
    ptime->dwMinute = p->tm_min;
    ptime->dwSecond = p->tm_sec;


}


AviPBHandle *AR_AVI_CreatePbHandle(void)
{
    AviPBHandle *pPBHandle = NULL;
    pPBHandle = (AviPBHandle *)malloc(sizeof(AviPBHandle));
    if(NULL == pPBHandle)
    {
        return NULL;
    }
    memset(pPBHandle, 0, sizeof(AviPBHandle));
    pPBHandle->list = NULL;
    pPBHandle->file = NULL;
    pPBHandle->idx_array = NULL;
    pPBHandle->pb_buf = NULL;

    return pPBHandle;
}

int avi_pb_slide_to_next_file(AviPBHandle *pPBHandle)
{
    if(NULL == pPBHandle)
    {
        printf("param error.\n");
        return -1;
    }

    //单文件
    if(pPBHandle->list == NULL) //pb by name
        return -1;
    else //多文件
    {
        int ret = 0;
        FILE_NODE node;
        memset(&node, 0, sizeof(node));
        ret = get_file_node(pPBHandle->list, &node);
        if (ret)
        {
            printf("get_file_node failed, ret = %d\n", ret);
            return -1;
        }
        //print_node(&node);
        //tmp set /mnt as the root dir.
        ret = AR_AVI_LoadFile(node.path, pPBHandle);
        if (ret < 0)
        {
            printf("avi_pb_open %s error.\n", node.path);
            return -1;
        }
    }

    return 0;
}

/**********************************************************************
函数描述：设置avi文件播放的速度，目前只支持快进
入口参数：int speed: 播放速度，如 1,2,4,8,16,32
返回值：  <0: 错误
           0: 正常
**********************************************************************/
int avi_pb_set_speed(int speed)
{
    printf("avi_pb_set_speed = %d\n", speed);
    //pthread_mutex_lock(&avi_pb_speed_x);
    avi_pb_speed = speed;
    //pthread_mutex_unlock(&avi_pb_speed_x);

    return 0;
}

int avi_pb_start(void)
{
    printf("avi playback start.\n");

    avi_pb_set_speed(1);

 /*   int ret = 0;
   // while(ret != GK_NET_NOMOREFILE)
   // {
        while(avi_flag_pause)
            sleep(2);
        printf("in start 2.\n");
        ret = avi_pb_get_frame(p_frame, pBuff, pSize);
        if(ret < 0)
            printf("in start 3.\n");
        else
            printf("in start 4.\n");
        p_frame->no = p_frame->no + speed - 1;
        printf("in start 5. no = %d\n", p_frame->no);
   // }
*/
    return 0;
}

/**********************************************************************
函数描述：从头开始重新播放avi文件
入口参数：AviFrameInfo *p_frame: avi文件中一帧数据的控制句柄
返回值：  <0: 错误
           0: 正常
**********************************************************************/
int avi_pb_restart(AviPBHandle *pPBHandle)
{
    if(pPBHandle->list == NULL)
    {
        pPBHandle->no = 0;
        avi_pb_set_speed(1);
    }

    pPBHandle->list->index = 0;
    //init_frameinfo(p_frame);
    avi_pb_set_speed(1);

    //avi_pb_start();

    return 0;
}

int avi_pb_pause(void)
{
    avi_flag_pause = (avi_flag_pause + 1) % 2;

    return 0;
}

int avi_pb_get_pos(AviPBHandle *pPBHandle)
{
   int pos;
   pos = (pPBHandle->no * 100) / pPBHandle->video_count;
   return pos;
}

int avi_pb_set_pos(AviPBHandle *pPBHandle, int pTimePos)
{
    int ret;
    int time_pos = pTimePos;
    int all_time = cal_pb_all_time(pPBHandle);
    printf("set pos, time_pos = %d, all time = %d\n", time_pos, all_time);

    /*
    int video_no = (int)(pPBHandle->video_count * time_pos / all_time);
    int no = 0;

    printf("set pos, video_no = %d, video_count = %d\n", video_no, pPBHandle->video_count);

    while(video_no)
    {
        int fcc = pPBHandle->idx_array[4*no];
        if(fcc == MAKE_FOURCC('0','0','d','c'))
            video_no --;

        no ++;
    }
    */

    pPBHandle->no = (int)(pPBHandle->index_count * time_pos / all_time);
    //第一帧是i帧
    while(pPBHandle->no < pPBHandle->index_count)
    {
        if ((pPBHandle->idx_array[4*pPBHandle->no] == MAKE_FOURCC('0','0','d','c'))
            && (pPBHandle->idx_array[4*pPBHandle->no+1] == 0x11))
            break;
        else
            pPBHandle->no ++;
    }
    printf("set pos, get no = %d, index count = %d\n", pPBHandle->no, pPBHandle->index_count);

    ret = avi_read_data_to_buf(pPBHandle);
    if(ret < 0)
    {
        printf("call avi_read_data_to_buf failed");
        return -1;
    }

    return 0;
}

int avi_pb_seek(AviPBHandle *pPBHandle, int seek_no)
{
    int ret;

    pPBHandle->no = seek_no;

    //第一帧是i帧
    while(pPBHandle->no < pPBHandle->index_count)
    {
        if ((pPBHandle->idx_array[4*pPBHandle->no] == MAKE_FOURCC('0','0','d','c'))
            && (pPBHandle->idx_array[4*pPBHandle->no+1] == 0x11))
            break;
        else
            pPBHandle->no ++;
    }
    printf("set pos, get no = %d, index count = %d\n", pPBHandle->no, pPBHandle->index_count);

    ret = avi_read_data_to_buf(pPBHandle);
    if(ret < 0)
    {
        printf("call avi_read_data_to_buf failed");
        return -1;
    }

    return 0;
}


int avi_pb_get_time(AviPBHandle *pPBHandle, struct tm *p_nowtime)
{
    char file_name[128];
    int i;
    char str_tmp[20];

    int    y1,m1,d1,h1,n1,s1;  //start
    int duration;
    time_t timep;
    struct tm *p;

    i = AR_AVI_FindLastIndex(pPBHandle->file_path, '/');

    memset(file_name, 0 ,sizeof(file_name));
    strcpy(file_name, pPBHandle->file_path + i + 1);

    AR_AVI_ExtractSubstr(str_tmp, file_name, 4, 4+4);
    y1 = atoi(str_tmp);
    AR_AVI_ExtractSubstr(str_tmp, file_name, 8, 8+2);
    m1 = atoi(str_tmp);
    AR_AVI_ExtractSubstr(str_tmp, file_name, 10, 10+2);
    d1 = atoi(str_tmp);
    AR_AVI_ExtractSubstr(str_tmp, file_name, 12, 12+2);
    h1 = atoi(str_tmp);
    AR_AVI_ExtractSubstr(str_tmp, file_name, 14, 14+2);
    n1 = atoi(str_tmp);
    AR_AVI_ExtractSubstr(str_tmp, file_name, 16, 16+2);
    s1 = atoi(str_tmp);

    duration = (int) pPBHandle->video_no / pPBHandle->fps;

    time(&timep);
    struct tm tm1 = {0};
    p = localtime_r(&timep, &tm1);

    p->tm_year = y1 - 1900;
    p->tm_mon = m1 - 1;
    p->tm_mday = d1;
    p->tm_hour = h1;
    p->tm_min = n1;
    p->tm_sec = s1;

    timep = mktime(p);

    timep += duration;

    localtime_r(&timep, p_nowtime);

    return 0;
}

AviPBHandle *AR_AVI_ReadPbHandleCreate(char *filename)
{
    printf("call avi_pb_open_by_file\n");

    AviPBHandle *pPBHandle = (AviPBHandle *)AR_AVI_CreatePbHandle();
    if (NULL == pPBHandle) {
        return NULL;
    }

    int ret = AR_AVI_LoadFile(filename, pPBHandle);
    if (ret < 0) {
        printf("avi_pb_open %s error.\n", filename);
        return NULL;
    }

    return pPBHandle;
}

AR_S32 AR_AVI_ReadPbHandleDestory(AviPBHandle *pPBHandle)
{
    if(!pPBHandle)
    {
        printf("parameter is error.\n");
        return -1;
    }

    //close & free
    if(pPBHandle->list)
    {
        search_close(pPBHandle->list);
        pPBHandle->list = NULL;
    }
    if(pPBHandle->file)
    {
        fclose(pPBHandle->file);
        pPBHandle->file = NULL;
    }
    if(pPBHandle->idx_array)
    {
        free(pPBHandle->idx_array);
        pPBHandle->idx_array = NULL;
    }
    if(pPBHandle->pb_buf)
    {
        free(pPBHandle->pb_buf);
        pPBHandle->pb_buf = NULL;
    }

    free(pPBHandle);
    
    return 0;
}

AR_S32 AR_AVI_ReadGetFrame(AviPBHandle *pPBHandle)
{
    int ret;

	//read fram no > file frame no
    if(pPBHandle->no >= pPBHandle->index_count)
    {
        printf("avi file has been read to the end, please return to next\n");
        return -1;
    }

    //printf("get frame, no = %d, video count = %d, all count =%d \n", pPBHandle->no, pPBHandle->video_count, pPBHandle->index_count);

    ret = avi_get_frame_loop(pPBHandle);
    if(ret < 0)
    { 
        printf("call avi_get_frame_loop error.");
        return ret;
    }
    
    return 0;
 }


