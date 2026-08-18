#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/prctl.h>

#include "util_api.h"
#include "util_timer.h"
#include "media_fifo.h"
#include "cfg_all.h"
#include "pf_video.h"

#include "ar_g711.h"
#include "ar_mov_api.h"
#include "ar_avi_api.h"

#include "recmng_sd.h"
#include "recmng_rec.h"

#ifndef INDEX_NODE
typedef struct tagINDEX_NODE
{
    AR_U32 ch_num;
    AR_U64 start;
    AR_U64 stop;
}INDEX_NODE;
typedef struct
{
    AR_S32 type;
    AR_S32 num;
    INDEX_NODE *h;
} MANAGER_INFO;

#endif

static int g_sd_rec_limit_size = 10;

pthread_mutex_t g_manager_mutex = PTHREAD_MUTEX_INITIALIZER;

static IPC_CHANNEL_REC_CTRL_S g_stChannelRecCtrl[IPC_TOTAL_STREAM_NUM] = {0};

#define	 WRITE_LOG		0
#if	WRITE_LOG
static void log_save_file(unsigned char *data, unsigned int len)
{
#define SAVE_FILE		"/mnt/sd_card/log.txt"
    static int fd = -1;
    int ret = 0;

	if(fd <= 0) {
		fd = open(SAVE_FILE, O_RDWR | O_CREAT | O_TRUNC);
		if(fd < 0) {
			printf("open %s", SAVE_FILE);
			return ;
		}
		printf("create %s success\n", SAVE_FILE);
	}

    if(len != (ret = write(fd, data, len)))
    {
        printf("write fd=%d error wlen:%d, len:%d", fd, ret, len);
        return ;
    }

    IPC_MID_UTIL_SysCall("sync", NULL, 0);
}
#endif

static AR_U64 Time_GetU64(void)
{
    AR_CHAR time_now[30];
    struct tm *ptm;
    long ts;

    memset(time_now, 0, sizeof(time_now));

    ts = time(NULL);
    struct tm tt = {0};
    ptm = localtime_r(&ts, &tt);

	sprintf(time_now, "%04d%02d%02d%02d%02d%02d", ptm->tm_year+1900,
											 ptm->tm_mon+1,
											 ptm->tm_mday,
											 ptm->tm_hour,
											 ptm->tm_min,
											 ptm->tm_sec);
    AR_U64 time_64 = atoll(time_now);

    return time_64;
}

static void Time_U64ToArTime(IPC_SYS_TIME_INFO_S *ptime, AR_U64 tmp64)
{
    AR_CHAR tmp_str[20];
    AR_CHAR tmp[20];

    memset(tmp_str, 0, sizeof(tmp_str));
    sprintf(tmp_str, "%lld", tmp64);

    memset(tmp, 0, sizeof(tmp));
    IPC_MID_UTIL_ExtractSubstr(tmp, tmp_str, 0, 0+4);
    ptime->Year = atoi(tmp);

    memset(tmp, 0, sizeof(tmp));
    IPC_MID_UTIL_ExtractSubstr(tmp, tmp_str, 4, 4+2);
    ptime->Mon = atoi(tmp);

    memset(tmp, 0, sizeof(tmp));
    IPC_MID_UTIL_ExtractSubstr(tmp, tmp_str, 6, 6+2);
    ptime->Day = atoi(tmp);

    memset(tmp, 0, sizeof(tmp));
    IPC_MID_UTIL_ExtractSubstr(tmp, tmp_str, 8, 8+2);
    ptime->Hour = atoi(tmp);

    memset(tmp, 0, sizeof(tmp));
    IPC_MID_UTIL_ExtractSubstr(tmp, tmp_str, 10, 10+2);
    ptime->Min = atoi(tmp);

    memset(tmp, 0, sizeof(tmp));
    IPC_MID_UTIL_ExtractSubstr(tmp, tmp_str, 12, 12+2);
    ptime->Sec = atoi(tmp);

}

#if 0
static AR_U64 Time_ArTimeToU64(IPC_SYS_TIME_INFO_S *p)
{
    AR_CHAR tmp[20];
    AR_U64 tmp64 = 0;
    memset(tmp, 0, sizeof(tmp));
    sprintf(tmp, "%04u%02u%02u%02u%02u%02u", p->Year, p->Mon, p->Day,
                                            p->Hour, p->Min, p->Sec);
    tmp64 = atoll(tmp);

    return tmp64;
}

static void Time_U64AddSeconds(AR_U64 opr1, int seconds, AR_U64 *opr2)
{
    IPC_SYS_TIME_INFO_S src;
    Time_U64ToArTime(&src, opr1);

    time_t timep;
    struct tm *p;

    time(&timep);

    struct tm t1 = {0};
    p = localtime_r(&timep, &t1);

    p->tm_year = src.Year - 1900;
    p->tm_mon = src.Mon - 1;
    p->tm_mday = src.Day;
    p->tm_hour = src.Hour;
    p->tm_min = src.Min;
    p->tm_sec = src.Sec;

    timep = mktime(p);
    timep += seconds;
    memset(&t1, 0, sizeof(struct tm));
    p = localtime_r(&timep, &t1);

    IPC_SYS_TIME_INFO_S dst;
    dst.Year = p->tm_year + 1900;
    dst.Mon = p->tm_mon + 1;
    dst.Day =  p->tm_mday;
    dst.Hour = p->tm_hour;
    dst.Min = p->tm_min;
    dst.Sec = p->tm_sec;

    *opr2 = Time_ArTimeToU64(&dst);
}

static int write_manage_info(AR_U32 type, AR_U32 channel, AR_U64 start, AR_U64 stop)
{
    char index_file[128];
    FILE *fp;
    int count, ret;
    INDEX_NODE index_node;
    memset(index_file, 0 ,sizeof(index_file));
    switch(type)
    {
        case RECORD_TYPE_SCHED:
            sprintf(index_file, "%s/.sched", IPC_SD_MOUNT_POINT);
            break;
        case RECORD_TYPE_MOTION:
            sprintf(index_file, "%s/.motion", IPC_SD_MOUNT_POINT);
            break;
        case RECORD_TYPE_ALARM:
            sprintf(index_file, "%s/.alarm", IPC_SD_MOUNT_POINT);
            break;
        case RECORD_TYPE_CMD:
            sprintf(index_file, "%s/.cmd", IPC_SD_MOUNT_POINT);
            break;
        case RECORD_TYPE_MANU:
            sprintf(index_file, "%s/.manu", IPC_SD_MOUNT_POINT);
            break;
        case RECORD_TYPE_SCHED_2:
            sprintf(index_file, "%s/.sched2", IPC_SD_MOUNT_POINT);
            break;
        default:
            PRINT_ERR();
            return -1;
    }

    pthread_mutex_lock(&g_manager_mutex);
    if(access(index_file, F_OK) != 0)
    {
        fp = fopen(index_file, "wb+");
        if(NULL == fp)
        {
            PRINT_ERR();
            pthread_mutex_unlock(&g_manager_mutex);
            return -1;
        }
        count = 0;
        fseek(fp, 0, SEEK_SET);
        ret = fwrite(&count, sizeof(int), 1, fp);
        if(ret != 1)
        {
            PRINT_ERR();
            fclose(fp);
            pthread_mutex_unlock(&g_manager_mutex);
            return -1;
        }
    }
    else
    {
        fp = fopen(index_file, "rb+");
        if(NULL == fp)
        {
            PRINT_ERR();
            pthread_mutex_unlock(&g_manager_mutex);
            return -1;
        }
        fseek(fp, 0, SEEK_SET);
        ret = fread(&count, sizeof(int), 1, fp);
        if(ret != 1)
        {
            PRINT_ERR();
            fclose(fp);
            pthread_mutex_unlock(&g_manager_mutex);
            return -1;
        }
    }

    memset(&index_node, 0 ,sizeof(INDEX_NODE));
    index_node.ch_num = channel;
    index_node.start = start;
    index_node.stop = stop;


    fseek(fp, 0, SEEK_END);
    ret = fwrite(&index_node, sizeof(INDEX_NODE), 1, fp);
    if(ret != 1)
    {
        PRINT_ERR();
        fclose(fp);
        pthread_mutex_unlock(&g_manager_mutex);
        return -1;
    }

    PRINT_INFO("write to %s : ch_num = %u, start = %llu, stop = %llu.\n",
        index_file, index_node.ch_num, index_node.start, index_node.stop);

    count++;
    fseek(fp, 0, SEEK_SET);
    ret = fwrite(&count, sizeof(int), 1, fp);
    if(ret != 1)
    {
        PRINT_ERR();
        fclose(fp);
        pthread_mutex_unlock(&g_manager_mutex);
        return -1;
    }

    fseek(fp, 0, SEEK_SET);

    fflush(fp);
    fsync(fileno(fp));
    fclose(fp);

    pthread_mutex_unlock(&g_manager_mutex);
    return 0;
}

static MANAGER_INFO *read_manage_info(RECORD_TYPE_E type)
{
    FILE *fp;
    int count, ret;

    char index_file[128];
    memset(index_file, 0, sizeof(index_file));
    switch(type)
    {
        case RECORD_TYPE_SCHED:
            sprintf(index_file, "%s/.sched", IPC_SD_MOUNT_POINT);
            break;
        case RECORD_TYPE_MOTION:
            sprintf(index_file, "%s/.motion", IPC_SD_MOUNT_POINT);
            break;
        case RECORD_TYPE_ALARM:
            sprintf(index_file, "%s/.alarm", IPC_SD_MOUNT_POINT);
            break;
        case RECORD_TYPE_CMD:
            sprintf(index_file, "%s/.cmd", IPC_SD_MOUNT_POINT);
            break;
        case RECORD_TYPE_MANU:
            sprintf(index_file, "%s/.manu", IPC_SD_MOUNT_POINT);
            break;
        case RECORD_TYPE_SCHED_2:
            sprintf(index_file, "%s/.sched2", IPC_SD_MOUNT_POINT);
            break;
        default:
            PRINT_INFO("not use manager file\n");
            return NULL;
    }

    pthread_mutex_lock(&g_manager_mutex);
    fp = fopen(index_file, "rb");
    if(fp == NULL)
    {
        PRINT_ERR();
        goto err4;
    }
    fseek(fp, 0, SEEK_SET);
    ret = fread(&count, sizeof(int), 1, fp);
    if ( (ret != 1) || (count <= 0) )
    {
        PRINT_ERR();
        goto err3;
    }

    MANAGER_INFO *info = (MANAGER_INFO *)malloc(sizeof(MANAGER_INFO));
    if(info == NULL)
    {
        PRINT_ERR();
        goto err3;
    }
    info->type = type;
    info->num = count;
    info->h = (INDEX_NODE *)malloc(sizeof(INDEX_NODE) * count);
    if(info->h == NULL)
    {
        PRINT_ERR();
        goto err2;
    }

    ret = fread(info->h, sizeof(INDEX_NODE) * count, 1, fp);
    if (ret != 1)
    {
        PRINT_ERR();
        goto err1;
    }

    fclose(fp);
    pthread_mutex_unlock(&g_manager_mutex);
    return info;

err1:
    free(info->h);
err2:
    free(info);
err3:
    fclose(fp);
err4:
    pthread_mutex_unlock(&g_manager_mutex);
    return NULL;
}

static void free_manage_info(MANAGER_INFO *info)
{
    pthread_mutex_lock(&g_manager_mutex);

    if (info == NULL) {
        pthread_mutex_unlock(&g_manager_mutex);
        return;
    }

    if (info->h)
        free(info->h);

    if (info)
        free(info);

    pthread_mutex_unlock(&g_manager_mutex);
}
#endif

//#ifdef MODULE_SUPPORT_REC_MOV
static int sd_init_mov_handle(IPC_REC_PARAM_S *param)
{
    if(!param)
    {
        PRINT_ERR("parameter error.\n");
        return -1;
    }

    param->mov_w_handle.fps = param->fps;
    param->mov_w_handle.gop = param->gop;
    //handle->time_scale = 300;
    param->mov_w_handle.time_scale = 1000;
    param->mov_w_handle.rate = 0x00010000;
    param->mov_w_handle.volume = 0x0100;
    param->mov_w_handle.width = param->width;
    param->mov_w_handle.height  = param->height;

    param->mov_w_handle.audio_enable = param->audio_enable;
    param->mov_w_handle.a_enc_type = param->a_enc_type;
    param->mov_w_handle.a_rate = param->a_rate;
    param->mov_w_handle.a_chans = param->a_chans;
    param->mov_w_handle.a_bits = param->a_bits;
    if(param->codec_type == ENC_TYPE_H264)
    {
        param->mov_w_handle.codec_type = MOV_H264_TYPE;
    }
    else
    {
        param->mov_w_handle.codec_type = MOV_H265_TYPE;
    }
    //How about MJPEG ? to be added.

    return 0;
}

static int sd_rec_open_mov(IPC_REC_PARAM_S *param, int type)
{
    int ret = 0;
    AR_U64 all_size;

    if(!param)
    {
        PRINT_ERR("parameter error.\n");
        return -1;
    }

    AR_MOV_WriteClose(&param->mov_w_handle);

    sd_init_mov_handle(param);

    int frame_counts = 0;
    if(RECORD_IS_FIXED_SIZE(param->mode))
    {
        all_size = param->size_m * 1024 * 1024; // M to Byte

        frame_counts = all_size  * (param->fps) / (param->bps / 8) * 4;
        PRINT_INFO("fps = %d, rec size = %d Mb, idx_array_count = %d\n",
               param->fps, param->size_m, frame_counts);
    }
    else if(RECORD_IS_FIXED_DURATION(param->mode))
    {
        PRINT_INFO("bps = %d, duration = %d minutes \n", param->bps, param->duration);
        all_size = (param->bps / 8) * (param->duration) * 6 * 13 ;  //add space to 130%
        param->size_m = (int) (all_size/1024/1024);

        //int tmp = param->a_rate * (param->a_bits / 8) / 2048 + 1 + param->fps;
        int tmp = 50 + 25;
        frame_counts = (param->duration) * 70 * (tmp);
        PRINT_INFO("bps = %d, fps = %d, rec duration = %d minutes, idx_array_count = %d\n",
                    param->bps, param->fps, param->duration, frame_counts);
    }
    else
    {
        PRINT_ERR("record mode is error. mode = %d\n", param->mode);
        return -1;
    }

    //check the sd card is mounted.
    if(IPC_MID_RECMNG_IsSDMount() < 0)
    {
        ret = IPC_MID_RECMNG_MountSD();
        if(ret < 0)
        {
        	PRINT_ERR("%s mount sdcard failed %d", __func__, __LINE__);
			return -1;
        }
    }

    while (IPC_MID_RECMNG_GetSDFreeSize() < 10)
    {
        while(IPC_MID_RECMNG_GetSDFreeSize() < (param->size_m + 10))
        {
            if(IPC_MID_RECMNG_GetSDFreeSize() == 0)
            {
                PRINT_ERR("sd card is not mounted, so free size is 0.\n");
                return -1;
            }
            PRINT_INFO("sd free size = %d MB, avi file size = %d MB\n", IPC_MID_RECMNG_GetSDFreeSize() ,  param->size_m);

            if((!RECORD_IS_DELETE(param->mode)))
            {
                PRINT_ERR("sd card have not enough space and the record flag is not delete, so return.\n");
                return -1;
            }
            else
            {
                //mov todo
                PRINT_INFO("delete old files ...\n");
            }
        }
    }

    //avi_file->idx_array_count = (param->duration) * 60 * (param->fps) *2;
    param->mov_w_handle.idx_array_count = frame_counts;

    if(access(IPC_SD_VIDEO_PATH, F_OK) != 0 )
    {
        ret = mkdir(IPC_SD_VIDEO_PATH, 0777);
        if(ret < 0)
        {
            PRINT_ERR("mkdir %s failed\n", IPC_SD_VIDEO_PATH);
            return -1;
        }
    }

    char date[10] = {0};
    char str_time[10] = {0};
    char date_path[128] = {0};
    char sub_dir[32] = {0};

    if(type == RECORD_TYPE_ALARM)
    {
        strcpy(sub_dir, "alarm");
        IPC_MID_UTIL_GetDateAndTimeStr(date, str_time, param->pre_rec_sec);
    }
    else
    {
        strcpy(sub_dir, "record");
        IPC_MID_UTIL_GetDateAndTimeStr(date, str_time, 0);
    }

    sprintf(date_path, "%s/%s", IPC_SD_VIDEO_PATH, sub_dir);
    if(access(date_path, F_OK) != 0 )
    {
        ret = mkdir(date_path, 0777);
        if(ret < 0)
        {
            PRINT_ERR("mkdir %s failed\n", date_path);
            return -1;
        }
    }

    sprintf(date_path, "%s/%s/%s", IPC_SD_VIDEO_PATH, sub_dir, date); // str = /mnt/sd_card/VIDEO/20120914
    if(access(date_path, F_OK) != 0 )
    {
        ret = mkdir(date_path, 0777);
        if(ret < 0)
        {
            PRINT_ERR_MSG("mkdir %s failed\n", date_path);
            return -1;
        }
    }

    //create avi file
    memset(param->mov_w_handle.file_path, 0, sizeof(param->mov_w_handle.file_path));
    sprintf(param->mov_w_handle.file_path, "%s/ch%d_%d_%s.part", date_path, param->channel,param->stream, str_time);  // filename: 20190101/0_101010.part
    PRINT_INFO("1.Rec filename = %s \n", param->mov_w_handle.file_path);

    //write file
    ret = AR_MOV_WriteOpen(&param->mov_w_handle);
    if(ret < 0)
    {
        PRINT_ERR("AR_MOV_WriteOpen failed.\n");
        return -1;
    }
    PRINT_INFO("2. open file success.\n");

    //write header
    ret = AR_MOV_WriteHead(&param->mov_w_handle);
    if(ret < 0)
    {
        PRINT_ERR("AR_MOV_WriteHead failed.\n");
        return -1;
    }
    PRINT_INFO("3. write file header success.\n");

    AR_MOV_ClearWriteBuff(&param->mov_w_handle);

    return 0;
}


static int sd_rec_close_mov(MovWriteHandle *handle)
{
    int ret;

    if(!handle)
    {
        PRINT_ERR("parameter is error.\n");
        return -1;
    }

    AR_MOV_WriteTail(handle);
    PRINT_INFO("6. write file tailer success, record file is %s\n", handle->file_path);

    char oldname[128] = {0};
    strcpy(oldname, handle->file_path);

    char str_tmp[128] = {0};
    char new_name[128] = {0};
    char str_time[10] = {0};
    IPC_MID_UTIL_GetDateAndTimeStr(NULL, str_time, 0);
    strncpy(str_tmp, oldname, strlen(oldname) - 5);
    sprintf(new_name, "%s_%s.mov", str_tmp, str_time);

    ret = rename(oldname, new_name);
    if(ret)
    {
        PRINT_ERR("rename %s to %s failed.\n", oldname, new_name);
        return -1;
    }
    PRINT_INFO("7. rename file %s to %s success.\n", oldname, new_name);
    AR_MOV_WriteClose(handle);
    PRINT_INFO("8. close file success.\n");
    IPC_MID_UTIL_SysCall("sync", NULL, 0);

    return 0;
}

static int sd_rec_write_mov(char *data, int length, int frame_type, unsigned int time_ms, IPC_REC_PARAM_S * pstParams)
{
    int ret = 0;

    //check the sd card is mounted.
    if(IPC_MID_RECMNG_IsSDMount() < 0)
    {
        ret = IPC_MID_RECMNG_MountSD();
        if(ret < 0)
        {
            PRINT_ERR("%s mount sdcard failed %d", __func__, __LINE__);
            return -1;
        }
    }

    ret = AR_MOV_WriteFrame(data, length, frame_type, &pstParams->mov_w_handle, time_ms);

    return ret;
}

//#endif

//#ifdef MODULE_SUPPORT_REC_AVI
static int sd_init_avi_handle(IPC_REC_PARAM_S *param)
{
    if(!param)
    {
        PRINT_ERR("parameter error.\n");
        return -1;
    }

    param->avi_w_handle.bps = param->bps;
    param->avi_w_handle.fps = param->fps;
    param->avi_w_handle.gop = param->gop;
    param->avi_w_handle.width = param->width;
    param->avi_w_handle.height  = param->height;
    param->avi_w_handle.audio_enable = param->audio_enable;
    param->avi_w_handle.a_enc_type = param->a_enc_type;
    param->avi_w_handle.a_chans = param->a_chans;
    param->avi_w_handle.a_rate = param->a_rate;
    param->avi_w_handle.a_bits = param->a_bits;

    if(param->codec_type == ENC_TYPE_H264)
    {
        param->avi_w_handle.codec_type  = AVI_TYPE_H264;
    }
    else if(param->codec_type == ENC_TYPE_MJPEG)
    {
        param->avi_w_handle.codec_type = AVI_TYPE_MJPEG;
    }
    else
    {
        param->avi_w_handle.codec_type = AVI_TYPE_H265;
    }

    printf("%s: codec type: %d\n", __FUNCTION__, param->avi_w_handle.codec_type );
    return 0;
}

int sd_rec_open_avi(IPC_REC_PARAM_S *param, int type)
{
    int ret = 0;
    AR_U64 all_size;

    if(!param)
    {
        PRINT_ERR("parameter error.\n");
        return -1;
    }

    AR_AVI_WriteClose(&param->avi_w_handle);
    sd_init_avi_handle(param);

    int frame_counts = 0;
    if(RECORD_IS_FIXED_SIZE(param->mode))
    {
        all_size = param->size_m * 1024 * 1024; // M to Byte

        frame_counts = all_size  * (param->fps) / (param->bps / 8) * 4;
        PRINT_INFO("fps = %d, rec size = %d Mb, frame_counts = %d\n",
               param->fps, param->size_m, frame_counts);
    }
    else if(RECORD_IS_FIXED_DURATION(param->mode))
    {
        PRINT_INFO("bps = %d, duration = %d minutes \n", param->bps, param->duration);
        all_size = (param->bps / 8) * (param->duration) * 6 * 13 ;  //add space to 130%
        param->size_m = (int) (all_size/1024/1024);

        //int tmp = param->a_rate * (param->a_bits / 8) / 2048 + 1 + param->fps;
        int tmp = 50 + 25;
        frame_counts = (param->duration) * 70 * (tmp); //hbbai: why it's 70 not 60 ??
        PRINT_INFO("bps = %d, fps = %d, rec duration = %d minutes, frame_counts = %d\n",
                    param->bps, param->fps, param->duration, frame_counts);
    }
    else
    {
        PRINT_ERR("record mode is error. mode = %d\n", param->mode);
        return -1;
    }

    //check the sd card is mounted.
    if(IPC_MID_RECMNG_IsSDMount() < 0)
    {
        ret = IPC_MID_RECMNG_MountSD();
        if(ret < 0)
        {
            PRINT_ERR("%s mount sdcard failed %d", __func__, __LINE__);
            return -1;
        }
    }

    while (IPC_MID_RECMNG_GetSDFreeSize() < 10)
    {
        while(IPC_MID_RECMNG_GetSDFreeSize() < (param->size_m + 10))
        {
            if(IPC_MID_RECMNG_GetSDFreeSize() == 0)
            {
                PRINT_ERR("sd card is not mounted, so free size is 0.\n");
                return -1;
            }
            PRINT_INFO("sd free size = %d MB, avi file size = %d MB\n", IPC_MID_RECMNG_GetSDFreeSize() ,  param->size_m);

            if((!RECORD_IS_DELETE(param->mode)))
            {
                PRINT_ERR("sd card have not enough space and the record flag is not delete, so return.\n");
                return -1;
            }
            else
            {
                PRINT_INFO("delete old files ...\n");
                ret = IPC_MID_RECMNG_DelOldestDir();
                if(ret == 0)
                {
                    PRINT_ERR("no dir to del, dir error\n");
                    return -1;
                }
            }
        }
    }

    //avi_file->idx_array_count = (param->duration) * 60 * (param->fps) *2;
    param->avi_w_handle.idx_array_count = frame_counts;
    param->avi_w_handle.audio_enable = param->audio_enable;
    param->avi_w_handle.a_enc_type = param->a_enc_type;

    if(access(IPC_SD_VIDEO_PATH, F_OK) != 0 )
    {
        ret = mkdir(IPC_SD_VIDEO_PATH, 0777);
        if(ret < 0)
        {
            PRINT_ERR("mkdir %s failed\n", IPC_SD_VIDEO_PATH);
            return -1;
        }
    }

    char date[10] = {0};
    char str_time[10] = {0};
    char date_path[128] = {0};
    char sub_dir[32] = {0};

    if(type == RECORD_TYPE_ALARM)
    {
        strcpy(sub_dir, "alarm");
        IPC_MID_UTIL_GetDateAndTimeStr(date, str_time, param->pre_rec_sec);
    }
    else
    {
        strcpy(sub_dir, "record");
        IPC_MID_UTIL_GetDateAndTimeStr(date, str_time, 0);
    }

    sprintf(date_path, "%s/%s", IPC_SD_VIDEO_PATH, sub_dir);
    if(access(date_path, F_OK) != 0 )
    {
        ret = mkdir(date_path, 0777);
        if(ret < 0)
        {
            PRINT_ERR("mkdir %s failed\n", date_path);
            return -1;
        }
    }

    sprintf(date_path, "%s/%s/%s", IPC_SD_VIDEO_PATH, sub_dir, date); // str = /mnt/sd_card/VIDEO/20120914
    if(access(date_path, F_OK) != 0 )
    {
        ret = mkdir(date_path, 0777);
        if(ret < 0)
        {
            PRINT_ERR_MSG("mkdir %s failed\n", date_path);
            return -1;
        }
    }

    //create avi file
    memset(param->avi_w_handle.filename, 0, sizeof(param->avi_w_handle.filename));
    sprintf(param->avi_w_handle.filename, "%s/ch%d_%d_%s.part", date_path, param->channel, param->stream, str_time);  // filename: 20190101/0_101010.part

    PRINT_INFO("1.Rec filename = %s \n", param->avi_w_handle.filename);

    ret = AR_AVI_WriteOpen(&param->avi_w_handle);
    if(ret < 0)
    {
        PRINT_ERR("AR_AVI_WriteOpen failed.\n");
        return -1;
    }
    PRINT_INFO("2. open file success.\n");

    printf("%s: codec type: %d\n", __FUNCTION__, param->avi_w_handle.codec_type );

    //write avi header
    ret = AR_AVI_WriteHeader(&param->avi_w_handle);
    if(ret < 0)
    {
        PRINT_ERR("AR_AVI_WriteHeader failed.\n");
        return -1;
    }
    PRINT_INFO("3. write file header success.\n");

    AR_AVI_ClearWriteBuff(&param->avi_w_handle);

    return 0;
}



int sd_rec_close_avi(AviFile *avi_file)
{
    int ret;

    if(!avi_file)
    {
        PRINT_ERR("parameter is error.\n");
        return -1;
    }

    AR_AVI_AddIndex(avi_file);
    PRINT_INFO("6. write file tailer success, record file is %s\n", avi_file->filename);

    char str_tmp[128] = {0};
    char new_name[128] = {0};
    char oldname[128] = {0};
    char time_stop[10] = {0};

    strcpy(oldname, avi_file->filename);
    IPC_MID_UTIL_GetDateAndTimeStr(NULL, time_stop, 0);

    PRINT_INFO("stoptime= %s \n", time_stop);

    strncpy(str_tmp, oldname, strlen(oldname) - 5); //0_001850.part
    sprintf(new_name, "%s_%s.avi", str_tmp, time_stop);

    ret = rename(oldname, new_name);
    if(ret)
    {
        PRINT_ERR("rename %s to %s failed.\n", oldname, new_name);
        return -1;
    }

    PRINT_INFO("7. rename file %s to %s success.\n", oldname, new_name);

    AR_AVI_WriteClose(avi_file);
    PRINT_INFO("8. close file.\n");
    //IPC_MID_UTIL_SysCall("sync", NULL, 0);

    return 0;
}

//Here we don't care about the frame type, the caller should make sure send IDR first when starting a new file record.
static int sd_rec_write_avi(char *data, int length, int avi_frame_type, unsigned int time_ms, IPC_REC_PARAM_S * pstParams)
{
    int ret = 0;

    //check the sd card is mounted.
    if(IPC_MID_RECMNG_IsSDMount() < 0)
    {
        PRINT_ERR("sd card is not mounted.\n");
        return -1;
    }

    ret = AR_AVI_WriteFrame(data, length, avi_frame_type, &pstParams->avi_w_handle, time_ms);

    return ret;
}
//#endif

/***************************************************************/
//channel: ch * 4 + stream
static int sd_init_rec_param(IPC_REC_PARAM_S *rec_init_param, int id)
{
    IPC_CFG_VENC_S * pstVencCfg = NULL;
    IPC_CFG_RECORD_S * pstRecordCfg = NULL;
    AR_U32 u32RecMode = RECORD_FIXED_DURATION;
    AR_U32 u32Chn = 0, u32Stream = 0;

    if (!rec_init_param)
    {
        PRINT_ERR("parameter error.\n");
        return -1;
    }

    u32Chn = id / IPC_MAX_VENC_STREAM_PER_CHN;
    u32Stream = id % IPC_MAX_VENC_STREAM_PER_CHN;

    pstVencCfg = IPC_CFG_VENC_GetParam();
    if (pstVencCfg->vencStream[u32Chn][u32Stream].enable == 0)
    {
        PRINT_ERR("channel %d stream %d is not encode, so can't be record.\n", u32Chn, u32Stream);
        return -1;
    }

    pstRecordCfg = IPC_CFG_RECORD_GetParam();

    memset((void *)rec_init_param, 0, sizeof(IPC_REC_PARAM_S));
    rec_init_param->pre_rec_sec = pstRecordCfg->stRecord[u32Chn][u32Stream].preRecordTime;
    rec_init_param->channel = u32Chn;
    rec_init_param->stream = u32Stream;

    /* video parameter */
    if (pstVencCfg->vencStream[u32Chn][u32Stream].cfg.brcMode == 0)
    {
        rec_init_param->bps = pstVencCfg->vencStream[u32Chn][u32Stream].cfg.cbrAvgBps * 1000;
    }
    else
    {
        rec_init_param->bps = pstVencCfg->vencStream[u32Chn][u32Stream].cfg.vbrMaxBps * 1000;
    }

#ifdef SUPPORT_EMPTY_FRAME_INSERT
	rec_init_param->fps = FPS_DUMMY;
#else
    rec_init_param->fps = pstVencCfg->vencStream[u32Chn][u32Stream].cfg.fps;
#endif

    rec_init_param->gop = pstVencCfg->vencStream[u32Chn][u32Stream].cfg.gop;
    rec_init_param->width = pstVencCfg->vencStream[u32Chn][u32Stream].cfg.width;
    rec_init_param->height = pstVencCfg->vencStream[u32Chn][u32Stream].cfg.height;

    if (pstRecordCfg->stRecord[u32Chn][u32Stream].audioRecEnable == 1)
    {
        rec_init_param->audio_enable = 1;
    }
    else
    {
        rec_init_param->audio_enable = 0;
    }

    int enc_type = pstVencCfg->vencStream[u32Chn][u32Stream].cfg.encodeType;
    switch (enc_type)
    {
        case VENC_ENCODE_TYPE_H264:
        {
			rec_init_param->codec_type = ENC_TYPE_H264;
			PRINT_INFO("compression type is h264.\n");
			break;
        }
		case VENC_ENCODE_TYPE_H265:
        {
			rec_init_param->codec_type = ENC_TYPE_H265;
			PRINT_INFO("compression type is H265.\n");
			break;
        }
        case VENC_ENCODE_TYPE_MJPEG:
        {
			rec_init_param->codec_type = ENC_TYPE_MJPEG;
			PRINT_INFO("compression type is MJPEG.\n");
			break;
        }

        default:
            PRINT_ERR("encode type error, encode type = %d.\n", enc_type);
            break;
    }

    /* audio parameter */
    rec_init_param->a_enc_type = pstRecordCfg->stRecord[u32Chn][u32Stream].recAudioType; // 0 a-law; 1 u-law; 2 pcm; 3-adpcm
    rec_init_param->a_chans = 0;
    rec_init_param->a_rate = 0;
    rec_init_param->a_bits = 0;

    /* record parameter */
    if(pstRecordCfg->stRecord[u32Chn][u32Stream].recycleRecord == 1)
    {
        u32RecMode = RECORD_FIXED_DURATION |RECORD_DELETE_OLD_FILES;
    }
    else
    {
        u32RecMode = RECORD_FIXED_DURATION;
    }

    rec_init_param->mode = u32RecMode;
					       //mode = RECORD_FIXED_SIZE;
					       //mode = RECORD_FIXED_SIZE | RECORD_DELETE_OLD_FILES;
					       //mode = RECORD_FIXED_DURATION | RECORD_DELETE_OLD_FILES;

    rec_init_param->size_m = RECORD_SIZE_MB;
    rec_init_param->duration = pstRecordCfg->stRecord[u32Chn][u32Stream].recordDuration;
    rec_init_param->record_type = pstRecordCfg->stRecord[u32Chn][u32Stream].recordType;

    return 0; //init param success
}


static int sd_rec_init(int id, IPC_REC_PARAM_S * pstParams, int type) //alarm/manual/sched
{
    int ret = 0;

    ret = IPC_MID_RECMNG_InitSD();
    if (ret < 0)
    {
        PRINT_ERR("init sd card failed\n");
        return -1;
    }

    //check the sd card is mounted.
    if(IPC_MID_RECMNG_IsSDMount() < 0)
    {
        ret = IPC_MID_RECMNG_MountSD();
        if(ret < 0)
        {
            PRINT_ERR("%s mount sdcard failed %d", __func__, __LINE__);
            return -1;
        }
    }

    sd_init_rec_param(pstParams, id);

    PRINT_INFO("0. get rec init param, rec_type:%d\n", pstParams->record_type);

    if (pstParams->record_type == MMC_REC_AVI)
    {
        ret = sd_rec_open_avi(pstParams, type);
    }
    else if (pstParams->record_type == MMC_REC_MOV)
    {
        ret = sd_rec_open_mov(pstParams, type);
        //AR_MOV_PrintHandle(&mov_w_handle);
    }
    else
    {
        PRINT_ERR("rec type error:%d\n", pstParams->record_type);
        ret = -1;
    }

    return ret;
}


static int sd_rec_stop(IPC_REC_PARAM_S * pstParams)
{
    int ret = 0;

    if (pstParams->record_type == MMC_REC_AVI)
    {
        ret = sd_rec_close_avi(&pstParams->avi_w_handle);
    }
    else if (pstParams->record_type == MMC_REC_MOV)
    {
        ret = sd_rec_close_mov(&pstParams->mov_w_handle);
    }
    else
    {
        PRINT_ERR("rec type error:%d\n", pstParams->record_type);
        ret = -1;
    }

    return ret;
}

static int sd_rec_write(char *data, int length, int frame_type, unsigned int time_ms, IPC_REC_PARAM_S * pstParams)
{
    int ret = 0;

    if (pstParams->record_type == MMC_REC_AVI)
    {
        int avi_frame_type;
        avi_frame_type = (frame_type == IPC_FRAME_TYPE_I ? AVI_FRAME_TYPE_I :
                         (frame_type == IPC_FRAME_TYPE_IDR ? AVI_FRAME_TYPE_IDR :
                           (frame_type == IPC_FRAME_TYPE_P ? AVI_FRAME_TYPE_P : IPC_FRAME_TYPE_A)));
        ret = sd_rec_write_avi(data, length, avi_frame_type, time_ms, pstParams);
    }
    else if (pstParams->record_type == MMC_REC_MOV)
    {
        int mov_frame_type;
        mov_frame_type = (frame_type == IPC_FRAME_TYPE_I ? MOV_FRAME_TYPE_I :
                         (frame_type == IPC_FRAME_TYPE_IDR ? MOV_FRAME_TYPE_IDR :
                           (frame_type == IPC_FRAME_TYPE_P ? MOV_FRAME_TYPE_P : MOV_FRAME_TYPE_A)));

        ret = sd_rec_write_mov(data, length, mov_frame_type, time_ms, pstParams);
    }
    else
    {
        PRINT_ERR("rec type error:%d\n", pstParams->record_type);
    }

	if(ret < 0)
	{
	    PRINT_ERR("sd write data error.\n");
        IPC_MID_RECMNG_UmountSD();
        sleep(3);
        IPC_MID_RECMNG_MountSD();

        if (pstParams->record_type == MMC_REC_AVI)
        {
            AR_AVI_ClearWriteBuff(&pstParams->avi_w_handle);
        }
        else if (pstParams->record_type == MMC_REC_MOV)
        {
            AR_MOV_ClearWriteBuff(&pstParams->mov_w_handle);
        }
        else
        {
            PRINT_ERR("rec type error:%d\n", pstParams->record_type);
        }
    }

    return ret;
}

static AR_S32 IPC_MID_RECMNG_NeedRecSchedule(IPC_CFG_RECORD_S * pstRecordCfg, AR_U32 u32Id)
{
    //check current time if in the time range
    int i = 0, j = 0;
    AR_U64 u64Curr;
    //AR_U64 u64Start, u64Stop;
    IPC_SYS_TIME_INFO_S stSysTime;
    AR_U32 u32Chn, u32Stream;

    u64Curr = Time_GetU64();
    Time_U64ToArTime(&stSysTime, u64Curr);

    u32Chn = u32Id / IPC_MAX_VENC_STREAM_PER_CHN;
    u32Stream = u32Id % IPC_MAX_VENC_STREAM_PER_CHN;

    for(i = 0; i < 7; i++)
    {
        for(j = 0; j < 4; j++)
        {
            if(pstRecordCfg->stRecord[u32Chn][u32Stream].scheduleTime[i][j].startHour <= stSysTime.Hour
                && stSysTime.Hour <= pstRecordCfg->stRecord[u32Chn][u32Stream].scheduleTime[i][j].stopHour
                && pstRecordCfg->stRecord[u32Chn][u32Stream].scheduleTime[i][j].startMin <= stSysTime.Min
                && stSysTime.Min <= pstRecordCfg->stRecord[u32Chn][u32Stream].scheduleTime[i][j].stopMin)
            {
                return 1;
            }
        }
    }

    //never match
    return 0;
}

static AR_U32 IPC_MID_RECMNG_IsAlarmThreadCreated(AR_U32 u32Chn, AR_U32 u32Stream)
{
    AR_U32 u32IsRecording = 0;
    AR_U32 u32Id = IPC_GET_VENC_CHN_ID(u32Chn, u32Stream);

    pthread_mutex_lock(&g_stChannelRecCtrl[u32Id].lock);
    u32IsRecording = g_stChannelRecCtrl[u32Id].u32AlarmThreadCreated;
    pthread_mutex_unlock(&g_stChannelRecCtrl[u32Id].lock);

    return u32IsRecording;
}

static AR_U32 IPC_MID_RECMNG_IsManualSchedThreadCreated(AR_U32 u32Chn, AR_U32 u32Stream)
{
    AR_U32 u32IsRecording = 0;
    AR_U32 u32Id = IPC_GET_VENC_CHN_ID(u32Chn, u32Stream);

    pthread_mutex_lock(&g_stChannelRecCtrl[u32Id].lock);
    u32IsRecording = g_stChannelRecCtrl[u32Id].u32ManualSchedThreadCreated;
    pthread_mutex_unlock(&g_stChannelRecCtrl[u32Id].lock);

    return u32IsRecording;
}

static AR_U32 IPC_MID_RECMNG_SetAlarmThreadCreated(AR_U32 u32Chn, AR_U32 u32Stream)
{
    AR_U32 u32Id = IPC_GET_VENC_CHN_ID(u32Chn, u32Stream);

    pthread_mutex_lock(&g_stChannelRecCtrl[u32Id].lock);
    g_stChannelRecCtrl[u32Id].u32AlarmThreadCreated = 1;
    g_stChannelRecCtrl[u32Id].u32AlarmThreadExit = 0;
    pthread_mutex_unlock(&g_stChannelRecCtrl[u32Id].lock);

    return 0;
}

static AR_U32 IPC_MID_RECMNG_SetManualSchedThreadCreated(AR_U32 u32Chn, AR_U32 u32Stream)
{
    AR_U32 u32Id = IPC_GET_VENC_CHN_ID(u32Chn, u32Stream);

    pthread_mutex_lock(&g_stChannelRecCtrl[u32Id].lock);
    g_stChannelRecCtrl[u32Id].u32ManualSchedThreadCreated = 1;
    g_stChannelRecCtrl[u32Id].u32ManualSchedThreadExit = 0;
    pthread_mutex_unlock(&g_stChannelRecCtrl[u32Id].lock);

    return 0;
}

static AR_U32 IPC_MID_RECMNG_IsManualSchedThreadDeleted(AR_U32 u32Chn, AR_U32 u32Stream)
{
    AR_U32 u32Ret;
    AR_U32 u32Id = IPC_GET_VENC_CHN_ID(u32Chn, u32Stream);

    pthread_mutex_lock(&g_stChannelRecCtrl[u32Id].lock);
    u32Ret = g_stChannelRecCtrl[u32Id].u32ManualSchedThreadExit;
    pthread_mutex_unlock(&g_stChannelRecCtrl[u32Id].lock);

    return u32Ret;
}

static AR_U32 IPC_MID_RECMNG_IsAlarmThreadDeleted(AR_U32 u32Chn, AR_U32 u32Stream)
{
    AR_U32 u32Ret;
    AR_U32 u32Id = IPC_GET_VENC_CHN_ID(u32Chn, u32Stream);

    pthread_mutex_lock(&g_stChannelRecCtrl[u32Id].lock);
    u32Ret = g_stChannelRecCtrl[u32Id].u32AlarmThreadExit;
    pthread_mutex_unlock(&g_stChannelRecCtrl[u32Id].lock);

    return u32Ret;
}

static AR_U32 IPC_MID_RECMNG_IsAlarmRecStopped(AR_U32 u32Chn, AR_U32 u32Stream)
{
    AR_U32 u32Ret;
    AR_U32 u32Id = IPC_GET_VENC_CHN_ID(u32Chn, u32Stream);

    pthread_mutex_lock(&g_stChannelRecCtrl[u32Id].lock);
    u32Ret = g_stChannelRecCtrl[u32Id].u32AlarmRecStop;
    pthread_mutex_unlock(&g_stChannelRecCtrl[u32Id].lock);

    return u32Ret;
}

static AR_U32 IPC_MID_RECMNG_IsManualRecStopped(AR_U32 u32Chn, AR_U32 u32Stream)
{
    AR_U32 u32Ret;
    AR_U32 u32Id = IPC_GET_VENC_CHN_ID(u32Chn, u32Stream);

    pthread_mutex_lock(&g_stChannelRecCtrl[u32Id].lock);
    u32Ret = g_stChannelRecCtrl[u32Id].u32ManualRecStop;
    pthread_mutex_unlock(&g_stChannelRecCtrl[u32Id].lock);

    return u32Ret;
}

static AR_U32 IPC_MID_RECMNG_IsSchedRecStopped(AR_U32 u32Chn, AR_U32 u32Stream)
{
    AR_U32 u32Ret;
    AR_U32 u32Id = IPC_GET_VENC_CHN_ID(u32Chn, u32Stream);

    pthread_mutex_lock(&g_stChannelRecCtrl[u32Id].lock);
    u32Ret = g_stChannelRecCtrl[u32Id].u32ScheduleRecStop;
    pthread_mutex_unlock(&g_stChannelRecCtrl[u32Id].lock);

    return u32Ret;
}

static AR_U32 IPC_MID_RECMNG_IsAlarmTriggered(AR_U32 u32Chn, AR_U32 u32Stream)
{
    AR_U32 u32Ret;
    AR_U32 u32Id = IPC_GET_VENC_CHN_ID(u32Chn, u32Stream);

    pthread_mutex_lock(&g_stChannelRecCtrl[u32Id].lock);
    u32Ret = g_stChannelRecCtrl[u32Id].u32AlarmRecTrigger;
    pthread_mutex_unlock(&g_stChannelRecCtrl[u32Id].lock);

    return u32Ret;
}
static AR_U32 IPC_MID_RECMNG_ClearAlarmTrigger(AR_U32 u32Chn, AR_U32 u32Stream)
{
    AR_U32 u32Id = IPC_GET_VENC_CHN_ID(u32Chn, u32Stream);

    pthread_mutex_lock(&g_stChannelRecCtrl[u32Id].lock);
    g_stChannelRecCtrl[u32Id].u32AlarmRecTrigger = 0;
    pthread_mutex_unlock(&g_stChannelRecCtrl[u32Id].lock);

    return 0;
}
AR_U32 IPC_MID_RECMNG_TriggerAlarmRec(AR_U32 u32Chn, AR_U32 u32Stream)
{
    AR_U32 u32Id = IPC_GET_VENC_CHN_ID(u32Chn, u32Stream);

    pthread_mutex_lock(&g_stChannelRecCtrl[u32Id].lock);
    g_stChannelRecCtrl[u32Id].u32AlarmRecTrigger = 1;
    pthread_mutex_unlock(&g_stChannelRecCtrl[u32Id].lock);

    return 0;
}

static AR_S32 IPC_MID_RECMNG_RecOneFile(AR_U32 u32Id, MEDIABUF_HANDLE hMbufReader, AR_U32 u32Type)
{
    int ret = 0;
    int avi_size = 0;
    //int tmp32_video_index = 0;
    //int tmp32_audio_index = 0;
    AR_U32 u32Chn = u32Id / IPC_MAX_VENC_STREAM_PER_CHN;//IPC_GET_CHN_FROM_ID(u32Id);
    AR_U32 u32Stream = u32Id % IPC_MAX_VENC_STREAM_PER_CHN;//IPC_GET_STREAM_FROM_ID(u32Id);

    struct timeval t1, t2;

    IPC_FRAME_HEADER_S header = {0};
    IPC_REC_PARAM_S stRecParams = {0};

    //int have_audio = 1;

    //int frameType;
    char *recv_buf = NULL;
    int data_length = 0;

	ret = sd_rec_init(u32Id, &stRecParams, u32Type);
    if (ret < 0)
    {
        PRINT_ERR("call sd_rec_start failed\n");
        return -1;
    }

    /* For alarm record, we search for pre_rec_sec backforward.
    *  For normal record, we start from the latest frame.
    */
    AR_S32 s32Duration = 0;
    if(u32Type == RECORD_TYPE_ALARM)
    {
        s32Duration = RECORD_DEFAULT_ALARM_DURATION_SEC;
        int cur_sec = 0;
        if ((stRecParams.pre_rec_sec > 0))
        {
            IPC_MID_MBUF_SetNewestFrame(hMbufReader);
            ret = IPC_MID_MBUF_ReadFrame(hMbufReader, (void **)&recv_buf, &data_length, &header);
            if (ret < 0)
            {
    			PRINT_ERR("stream%d : IPC_MID_MBUF_ReadFrame fail.\n", u32Id);
        		return -1;
            }
            cur_sec = header.sec;

            //search for where we start from:  start_point |<--pre_sec---now.
            IPC_MID_MBUF_SetOldestFrame(hMbufReader);
            while(1)
            {
                recv_buf = NULL;
                ret = IPC_MID_MBUF_ReadFrame(hMbufReader, (void **)&recv_buf, &data_length, &header);
                if (ret < 0)
                {
        			PRINT_ERR("stream%d : IPC_MID_MBUF_ReadFrame fail.\n", u32Id);
            		return -1;
                }
                if(cur_sec - header.sec <= stRecParams.pre_rec_sec)
                {
                    break;
                }
            }
        }
    }
    else
    {
        s32Duration = stRecParams.duration;
        stRecParams.pre_rec_sec = 0; //regular record does not need pre rec.
    }

    //Request an IDR to start the record
    IPC_PF_VENC_RequestIDR(u32Id);

    gettimeofday(&t1, NULL);

	while (1)
    {
        if (IPC_MID_RECMNG_GetSDFreeSize() < g_sd_rec_limit_size)
        {
            PRINT_INFO("sd free size = %d MB, so to end writing file.\n", IPC_MID_RECMNG_GetSDFreeSize());
            break;
        }

        recv_buf = NULL;
        ret = IPC_MID_MBUF_ReadFrame(hMbufReader, (void **)&recv_buf, &data_length, &header);
        if (ret < 0)
        {
			PRINT_ERR("stream%d : IPC_MID_MBUF_ReadFrame fail.\n", u32Id);
    		break;
        }

        if (ret == 0)
        {
            PRINT_INFO("read no data.\n");
            usleep(3000);
            continue;
        }

        //printf("Rec read frame %ld %d from mbuf...\n", header.pts, header.frame_no);

        AR_U32 time_ms = (AR_U32)((AR_U64)header.sec * 1000 + (AR_U64)header.usec / 1000);

        if (header.frame_type == IPC_FRAME_TYPE_A)
        {
            if (stRecParams.a_enc_type == AUDIO_REC_PCM && stRecParams.audio_enable)
            { //pcm
                ret = sd_rec_write(recv_buf, data_length, header.frame_type, time_ms, &stRecParams);
            }
            else
            {
                PRINT_ERR("get audio type error:%d.\n", stRecParams.a_enc_type);
            }
        }
        else
        {
            ret = sd_rec_write(recv_buf, data_length, header.frame_type, time_ms, &stRecParams);
        }

        if (ret < 0)
		{
		    PRINT_ERR("avi_record_write failed.\n");
			break;
		}

        if (RECORD_IS_FIXED_SIZE(stRecParams.mode))
        {
            //PRINT_INFO("fix size \n");

            if (stRecParams.record_type == MMC_REC_AVI)
            {
                avi_size = stRecParams.avi_w_handle.data_offset + 8 + stRecParams.avi_w_handle.index_count* 16 + 100; //leave 100 BYTE space
            }
            else if (stRecParams.record_type == MMC_REC_MOV)
            {
                avi_size = stRecParams.mov_w_handle.frame_offset + stRecParams.mov_w_handle.index_count * 16;
            }
            else
            {
                PRINT_ERR("rec type error:%d\n", stRecParams.record_type);
            }

            if(avi_size > stRecParams.size_m * 1024 * 1024)
            {
                PRINT_INFO("rec size of one file is %d MB, time to stop.\n", stRecParams.duration);
                break;
            }
        }
        else if(RECORD_IS_FIXED_DURATION(stRecParams.mode))
        {
            //PRINT_INFO("fix duration \n");
            gettimeofday(&t2, NULL);
            if ((t2.tv_sec - t1.tv_sec) >= (s32Duration * 60))
            {
                PRINT_INFO("End record: tv_sec:%llu\n", (AR_U64)(t2.tv_sec));
                PRINT_INFO("4. Rec time %d minutes, time to stop.\n", stRecParams.duration);
                break;
            }
        }
        else
        {
            PRINT_ERR("rec mode error.");
            break;
        }

        if (u32Type == RECORD_TYPE_ALARM)
        {
            if(IPC_MID_RECMNG_IsAlarmRecStopped(u32Chn, u32Stream) || IPC_MID_RECMNG_IsAlarmThreadDeleted(u32Chn, u32Stream))
            {
                PRINT_INFO("IPC_MID_RECMNG_RecOneFile, stop alarm record.\n");
                break;
            }
        }
        else if(u32Type == RECORD_TYPE_MANU)
        {
            if(IPC_MID_RECMNG_IsManualRecStopped(u32Chn, u32Stream) || IPC_MID_RECMNG_IsManualSchedThreadDeleted(u32Chn, u32Stream))
            {
                PRINT_INFO("IPC_MID_RECMNG_RecOneFile, stop manual record.\n");
                break;
            }
        }
        else
        {
            if(IPC_MID_RECMNG_IsSchedRecStopped(u32Chn, u32Stream) || IPC_MID_RECMNG_IsManualSchedThreadDeleted(u32Chn, u32Stream))
            {
                PRINT_INFO("IPC_MID_RECMNG_RecOneFile, stop schedule record.\n");
                break;
            }
        }

	}

    PRINT_INFO("5. write stream data end.");
    ret = sd_rec_stop(&stRecParams);
    if(ret < 0)
    {
        PRINT_ERR("sd_rec_stop failed.\n");
        return ret;
    }

    PRINT_INFO("9. end rec file ok.");
    return 0;
}

static void * IPC_MID_RECMNG_AlarmRecThread(void *args)
{
    AR_U32 u32Id = *(int *)args;
    AR_CHAR name[16] = {0};
    //IPC_CFG_RECORD_S * pstRecordCfg = IPC_CFG_RECORD_GetParam();
    //AR_S32 u32Ret;
    MEDIABUF_HANDLE hMbufReader = NULL;

    AR_U32 u32Chn = IPC_GET_VENC_CHN_FROM_ID(u32Id);
    AR_U32 u32Stream = IPC_GET_VENC_STREAM_FROM_ID(u32Id);

    sprintf(name, "alarm_rec_%d_%d", u32Id/IPC_MAX_VENC_STREAM_PER_CHN, u32Id % IPC_MAX_VENC_STREAM_PER_CHN);
    prctl(PR_SET_NAME, name, 0, 0, 0);

    hMbufReader = IPC_MID_MBUF_AddReader(u32Id);
    if(hMbufReader == NULL)
    {
        PRINT_ERR("add_recoder_user error ");
        return NULL;
    }

    //in some case, we need to record pre_seconds to keep the alarm context
    //int pre_rec_sec = 0;
#if 0
    if ((pstRecordCfg->stRecord[u32Id].preRecordTime > 0))
    {
        PRINT_INFO("use pre record.\n");
        //pre_rec_sec = 1;
        u32Ret = IPC_MID_MBUF_GetPreSeconds(hMbufReader, &pre_rec_sec);
        if (u32Ret < 0)
        {
            PRINT_ERR("IPC_MID_MBUF_GetPreSeconds error\n");
            pre_rec_sec = 0;
        }
    }
#endif
    while(!IPC_MID_RECMNG_IsAlarmThreadDeleted(u32Chn, u32Stream))
    {
        if(IPC_MID_RECMNG_IsAlarmRecStopped(u32Chn, u32Stream))
        {
            usleep(500 * 1000);
            continue;
        }

        if(IPC_MID_RECMNG_IsAlarmTriggered(u32Chn, u32Stream))
        {
            //record one file.
            IPC_MID_RECMNG_RecOneFile(u32Id, hMbufReader, RECORD_TYPE_ALARM);
            IPC_MID_RECMNG_ClearAlarmTrigger(u32Chn, u32Stream);
        }
        else
        {
            usleep(500 * 1000);
            continue;
        }
    }

    if(hMbufReader)
    {
        IPC_MID_MBUF_DelReader(hMbufReader);
    }

    return NULL;

}

//manual and schedule common function ?
//if manual enabled or in the scheduled time slice, then record files with each file records recordDuration time.
static void * IPC_MID_RECMNG_ManualSchedRecThread(void *args)
{
    AR_U32 u32Id = *(int *)args;
    AR_CHAR name[16] = {0};
    IPC_CFG_RECORD_S * pstRecordCfg = IPC_CFG_RECORD_GetParam();
    //AR_S32 u32Ret;
    MEDIABUF_HANDLE hMbufReader = NULL;

    AR_U32 u32Chn = IPC_GET_VENC_CHN_FROM_ID(u32Id);
    AR_U32 u32Stream = IPC_GET_VENC_STREAM_FROM_ID(u32Id);

    sprintf(name, "manu_rec_%d_%d", u32Id/IPC_MAX_VENC_STREAM_PER_CHN, u32Id % IPC_MAX_VENC_STREAM_PER_CHN);
    prctl(PR_SET_NAME, name, 0, 0, 0);

    hMbufReader = IPC_MID_MBUF_AddReader(u32Id);
    if(hMbufReader == NULL)
    {
        PRINT_ERR("add_recoder_user error ");
        return NULL;
    }

    while(!IPC_MID_RECMNG_IsManualSchedThreadDeleted(u32Chn, u32Stream))
    {
        if(IPC_MID_RECMNG_IsManualRecStopped(u32Chn, u32Stream) && IPC_MID_RECMNG_IsSchedRecStopped(u32Chn, u32Stream))
        {
            usleep(500*1000);
            continue;
        }

        //manual has higher priority
        if(!IPC_MID_RECMNG_IsManualRecStopped(u32Chn, u32Stream))
        {
            IPC_MID_RECMNG_RecOneFile(u32Id, hMbufReader, RECORD_TYPE_MANU);
            //then continue;
            continue;
        }

        //then schedule record.
        if(!IPC_MID_RECMNG_IsSchedRecStopped(u32Chn, u32Stream))
        {
            //if time is in the range, then record one file.
            if(IPC_MID_RECMNG_NeedRecSchedule(pstRecordCfg, u32Id))
            {
                IPC_MID_RECMNG_RecOneFile(u32Id, hMbufReader, RECORD_TYPE_SCHED);
            }
        }

    }

    if(hMbufReader)
    {
        IPC_MID_MBUF_DelReader(hMbufReader);
    }

    return NULL;

}


/**********************************************************************/

void IPC_MID_RECMNG_CreateAlarmThread(AR_U32 u32Chn, AR_U32 u32Stream)
{
    AR_S32 ret = 0;
    AR_U32 u32Id = IPC_GET_VENC_CHN_ID(u32Chn, u32Stream);

    PRINT_INFO("jump into IPC_MID_RECMNG_StartAlarmRec.\n");

    if(IPC_MID_RECMNG_IsAlarmThreadCreated(u32Chn, u32Stream))
        return;

#ifdef IPC_SEARCH_SUPPORT
    AR_U64 u64Start = Time_GetU64();
    AR_U64 u64Stop;

    Time_U64AddSeconds(u64Start, RECORD_DEFAULT_ALARM_DURATION_SEC, &u64Stop);

    write_manage_info(RECORD_TYPE_ALARM, u32Id, u64Start, u64Stop);
#endif
    //IPC_MID_RECMNG_CreateThread(channel);

    ret = IPC_MID_RECMNG_InitSD();
    if (ret < 0)
    {
        PRINT_ERR("IPC_MID_RECMNG_InitSD failed.\n");
        sleep(3);
        return;
    }

    //check the sd card is mounted.
    if (IPC_MID_RECMNG_IsSDMount() < 0)
    {
       ret = IPC_MID_RECMNG_MountSD();
       if(ret < 0)
       {
           PRINT_ERR("%s mount sdcard failed %d", __func__, __LINE__);
           return;
       }
    }

    pthread_create(&g_stChannelRecCtrl[u32Id].tidAlarm, NULL, IPC_MID_RECMNG_AlarmRecThread, &g_stChannelRecCtrl[u32Id].u32Id);

    IPC_MID_RECMNG_SetAlarmThreadCreated(u32Chn, u32Stream);

}
void IPC_MID_RECMNG_DeleteAlarmThread(AR_U32 u32Chn, AR_U32 u32Stream)
{
    AR_U32 u32Id = IPC_GET_VENC_CHN_ID(u32Chn, u32Stream);

    pthread_mutex_lock(&g_stChannelRecCtrl[u32Id].lock);
    g_stChannelRecCtrl[u32Id].u32AlarmThreadExit = 1;
    g_stChannelRecCtrl[u32Id].u32AlarmThreadCreated = 0;
    pthread_mutex_unlock(&g_stChannelRecCtrl[u32Id].lock);
}

void IPC_MID_RECMNG_StartAlarmRec(AR_U32 u32Chn, AR_U32 u32Stream)
{
    AR_U32 u32Id = IPC_GET_VENC_CHN_ID(u32Chn, u32Stream);
    pthread_mutex_lock(&g_stChannelRecCtrl[u32Id].lock);
    g_stChannelRecCtrl[u32Id].u32AlarmRecStop = 0;
    pthread_mutex_unlock(&g_stChannelRecCtrl[u32Id].lock);
}

void IPC_MID_RECMNG_StopAlarmRec(AR_U32 u32Chn, AR_U32 u32Stream)
{
    AR_U32 u32Id = IPC_GET_VENC_CHN_ID(u32Chn, u32Stream);

    pthread_mutex_lock(&g_stChannelRecCtrl[u32Id].lock);
    g_stChannelRecCtrl[u32Id].u32AlarmRecStop = 1;
    pthread_mutex_unlock(&g_stChannelRecCtrl[u32Id].lock);
}

void IPC_MID_RECMNG_CreateManualSchedThread(AR_U32 u32Chn, AR_U32 u32Stream)
{
    AR_S32 ret = 0;
    AR_U32 u32Id = IPC_GET_VENC_CHN_ID(u32Chn, u32Stream);

    PRINT_INFO("jump into IPC_MID_RECMNG_StartManuRec.\n");

    if(IPC_MID_RECMNG_IsManualSchedThreadCreated(u32Chn, u32Stream))
        return;

#ifdef IPC_SEARCH_SUPPORT
    AR_U64 u64Start = Time_GetU64();
    AR_U64 u64Stop;
    IPC_CFG_RECORD_S * pstRecord = IPC_CFG_RECORD_GetParam();

    Time_U64AddSeconds(u64Start, pstRecord->stRecord[u32Chn].recordDuration, &u64Stop);

    write_manage_info(RECORD_TYPE_MANU, u32Id, u64Start, u64Stop);
#endif

    pthread_create(&g_stChannelRecCtrl[u32Id].tidManualSched, NULL, IPC_MID_RECMNG_ManualSchedRecThread, &g_stChannelRecCtrl[u32Id].u32Id);

    IPC_MID_RECMNG_SetManualSchedThreadCreated(u32Chn, u32Stream);

}

void IPC_MID_RECMNG_DeleteManualSchedThread(AR_U32 u32Chn, AR_U32 u32Stream)
{
    AR_U32 u32Id = IPC_GET_VENC_CHN_ID(u32Chn, u32Stream);

    pthread_mutex_lock(&g_stChannelRecCtrl[u32Id].lock);
    g_stChannelRecCtrl[u32Id].u32ManualSchedThreadExit = 1;
    g_stChannelRecCtrl[u32Id].u32ManualSchedThreadCreated = 0;
    pthread_mutex_unlock(&g_stChannelRecCtrl[u32Id].lock);

}

void IPC_MID_RECMNG_StartManuRec(AR_U32 u32Chn, AR_U32 u32Stream)
{
    AR_U32 u32Id = IPC_GET_VENC_CHN_ID(u32Chn, u32Stream);

    PRINT_INFO("jump into manu_rec_start.\n");
    pthread_mutex_lock(&g_stChannelRecCtrl[u32Id].lock);
    if(g_stChannelRecCtrl[u32Id].u32ScheduleRecStop == 0)
    {
        PRINT_INFO("Schedule record has been enabled. will disable it and enable manual record.\n");
        g_stChannelRecCtrl[u32Id].u32ScheduleRecStop = 1;
    }
    g_stChannelRecCtrl[u32Id].u32ManualRecStop = 0;
    pthread_mutex_unlock(&g_stChannelRecCtrl[u32Id].lock);

}

void IPC_MID_RECMNG_StopManuRec(AR_U32 u32Chn, AR_U32 u32Stream)
{
    AR_U32 u32Id = IPC_GET_VENC_CHN_ID(u32Chn, u32Stream);

    PRINT_INFO("jump into manu_rec_stop.\n");
    pthread_mutex_lock(&g_stChannelRecCtrl[u32Id].lock);
    g_stChannelRecCtrl[u32Id].u32ManualRecStop = 1;
    pthread_mutex_unlock(&g_stChannelRecCtrl[u32Id].lock);

}

void IPC_MID_RECMNG_StartSchedRec(AR_U32 u32Chn, AR_U32 u32Stream)
{
    AR_U32 u32Id = IPC_GET_VENC_CHN_ID(u32Chn, u32Stream);

    PRINT_INFO("jump into sched_rec_start.\n");
    pthread_mutex_lock(&g_stChannelRecCtrl[u32Id].lock);
    if(g_stChannelRecCtrl[u32Id].u32ManualRecStop == 0)
    {
        PRINT_INFO("Manual record is enabled, can't set schedule record..\n");
        pthread_mutex_unlock(&g_stChannelRecCtrl[u32Id].lock);
        return;
    }

    g_stChannelRecCtrl[u32Id].u32ScheduleRecStop = 0;
    pthread_mutex_unlock(&g_stChannelRecCtrl[u32Id].lock);
}

void IPC_MID_RECMNG_StopSchedRec(AR_U32 u32Chn, AR_U32 u32Stream)
{
    AR_U32 u32Id = IPC_GET_VENC_CHN_ID(u32Chn, u32Stream);

    PRINT_INFO("jump into sched_rec_stop.\n");
    pthread_mutex_lock(&g_stChannelRecCtrl[u32Id].lock);
    g_stChannelRecCtrl[u32Id].u32ScheduleRecStop = 1;
    pthread_mutex_unlock(&g_stChannelRecCtrl[u32Id].lock);
}

//start all threads
void IPC_MID_RECMNG_Init()
{
    AR_U32 i = 0, j = 0, u32Id = 0;
	AR_S32 ret = 0;
    IPC_CFG_RECORD_S * pstRecord = IPC_CFG_RECORD_GetParam();

    memset(g_stChannelRecCtrl, 0, sizeof(g_stChannelRecCtrl));

	ret = IPC_MID_RECMNG_InitSD();
    if (ret < 0)
    {
        PRINT_ERR("%s check mmcblk1 failed, no sdcard insert %d\n", __func__, __LINE__);
    }

	if(ret == 0)
	{
    	ret = IPC_MID_RECMNG_MountSD();
    	if(ret < 0)
    	{
    		PRINT_ERR("%s mount error %d\n", __func__, __LINE__);
    	}
	}

    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        for(j = 0; j < IPC_MAX_VENC_STREAM_PER_CHN; j++)
        {
            u32Id = IPC_GET_VENC_CHN_ID(i, j);
            g_stChannelRecCtrl[u32Id].u32Id = u32Id;
            if(!pstRecord->stRecord[i][j].enable)
            {
                continue;
            }
            pthread_mutex_init(&g_stChannelRecCtrl[u32Id].lock, NULL);
			
            IPC_MID_RECMNG_StopManuRec(i, j);
            IPC_MID_RECMNG_StopSchedRec(i, j);

            if(pstRecord->stRecord[i][j].recordMode == IPC_CFG_REC_MODE_MANUAL
                || pstRecord->stRecord[i][j].recordMode == IPC_CFG_REC_MODE_SCHEDULE)
            {
                IPC_MID_RECMNG_CreateManualSchedThread(i, j);
            }
            else
            {
                printf("Not enable any record mode.\n");
            }

        }
    }

    IPC_MID_UTIL_TIMER_AddTask(IPC_MID_RECMNG_UpdateSDStatus, 4, TIMER_TRUE, TIMER_FALSE);

    return;
}

//exit all threads
void IPC_MID_RECMNG_UnInit()
{
    int i = 0, j = 0;
	AR_U32 u32Id = 0;
    IPC_CFG_RECORD_S * pstRecord = IPC_CFG_RECORD_GetParam();

    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        for(j = 0; j < IPC_MAX_VENC_STREAM_PER_CHN; j++)
        {

			u32Id = IPC_GET_VENC_CHN_ID(i, j);
            if(!pstRecord->stRecord[i][j].enable)
            {
                continue;
            }
			
            IPC_MID_RECMNG_StopAlarmRec(i, j);
            IPC_MID_RECMNG_StopManuRec(i, j);
            IPC_MID_RECMNG_StopSchedRec(i, j);

            IPC_MID_RECMNG_DeleteAlarmThread(i, j);
            IPC_MID_RECMNG_DeleteManualSchedThread(i, j);
            usleep(1000*1000);
            
            pthread_mutex_destroy(&g_stChannelRecCtrl[u32Id].lock);
        }
    }

	IPC_MID_UTIL_TIMER_DelTask(IPC_MID_RECMNG_UpdateSDStatus);

	IPC_MID_RECMNG_UmountSD();

      //memset(g_stChannelRecCtrl, 0, sizeof(g_stChannelRecCtrl));
}

void IPC_MID_RECMNG_StartRec(AR_U32 u32Chn, AR_U32 u32Stream)
{
    IPC_MID_RECMNG_StartManuRec(u32Chn, u32Stream);
    IPC_MID_RECMNG_StartSchedRec(u32Chn, u32Stream);
    IPC_MID_RECMNG_StartAlarmRec(u32Chn, u32Stream);
}

void IPC_MID_RECMNG_StopRec(AR_U32 u32Chn, AR_U32 u32Stream)
{
    IPC_MID_RECMNG_StopManuRec(u32Chn, u32Stream);
    IPC_MID_RECMNG_StopSchedRec(u32Chn, u32Stream);
    IPC_MID_RECMNG_StopAlarmRec(u32Chn, u32Stream);
}

void IPC_MID_RECMNG_StartRecAll()
{
    int i = 0, j = 0;

    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        for(j = 0; j < IPC_MAX_VENC_STREAM_PER_CHN; j++)
        {
            IPC_MID_RECMNG_StartRec(i, j);
        }
    }
}

void IPC_MID_RECMNG_StopRecAll()
{
    int i = 0, j = 0;

    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        for(j = 0; j < IPC_MAX_VENC_STREAM_PER_CHN; j++)
        {
            IPC_MID_RECMNG_StopRec(i, j);
        }
    }

}

void IPC_MID_RECMNG_VedioRecord(AR_U32 u32ChannelId, AR_U32 u32Stream, AR_S32 s32Mode, AR_S32 s32Format, AR_S32 s32Status)
{
	IPC_CFG_RECORD_S *pstVideoRecord =  IPC_CFG_RECORD_GetParam();

	pstVideoRecord->stRecord[u32ChannelId][u32Stream].recordType = s32Format;
	pstVideoRecord->stRecord[u32ChannelId][u32Stream].recordMode = s32Mode;

    if(s32Status)
    {
        if(s32Mode == 1)
        {
            IPC_MID_RECMNG_StartManuRec(u32ChannelId, u32Stream);
        }
		else if(s32Mode == 2)
		{
			IPC_MID_RECMNG_StartSchedRec(u32ChannelId, u32Stream);
		}
		
    }
    else
    {
        if(s32Mode == 1)
        {
            IPC_MID_RECMNG_StopManuRec(u32ChannelId, u32Stream);
        }
        else if(s32Mode == 2)
        {
            IPC_MID_RECMNG_StopSchedRec(u32ChannelId, u32Stream);
        }
    }
}


