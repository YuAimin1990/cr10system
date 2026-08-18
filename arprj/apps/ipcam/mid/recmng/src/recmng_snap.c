#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/statfs.h>

#include "util_api.h"

#include "recmng_sd.h"
#include "recmng_snap.h"

int snap_check_sd_space(AR_U64 len)
{
    struct statfs statFS;
    AR_U64 freeBytes = 0;

    int ret;
    ret = statfs(IPC_SD_MOUNT_POINT, &statFS);
    if (ret == -1)
    {
        PRINT_ERR("statfs failed, path -> [%s]\n", IPC_SD_MOUNT_POINT);
        return -1;
    }

    if (statFS.f_type != 0x00004d44)
    {
        PRINT_ERR("statFS.f_type failed, f_type=0x%lx\n", statFS.f_type);
        return -1;
    }

    freeBytes = (AR_U64)statFS.f_bfree * (AR_U64)statFS.f_frsize;

    if (freeBytes < (len + 100))
    {

        PRINT_ERR("snap, no space, free:%llu Bytes, datalen:%llu Bytes\n", freeBytes, len + 100);
        return -1;
    }

    return 0;
}

int snap_get_path(int stream_type, char *jpg_path)
{
    //check the sd card is mounted.
    if(IPC_MID_RECMNG_IsSDMount() < 0)
    {
        PRINT_ERR("sd card is not mounted.\n");
        return IPC_SD_STATUS_NOTEXIST;
    }

    #if 0
    if (IPC_MID_RECMNG_GetSDFreeSize() < ((len >> 10) + 1))
    {
        PRINT_ERR("snap, no space, free:%d datalen:%d.\n",
                       IPC_MID_RECMNG_GetSDFreeSize(), (len >> 10) + 1);
        return IPC_SD_STATUS_NO_SPACE;
    }
    #endif

    int ret = 0;
    if(access(IPC_SD_SNAP_PATH, F_OK) != 0 )
    {
        ret = mkdir(IPC_SD_SNAP_PATH, 0777);
        if(ret < 0)
        {
            PRINT_ERR("mkdir %s failed\n", IPC_SD_SNAP_PATH);
            return IPC_SD_STATUS_ERROR;
        }
    }

    char date[10] = {0};
    char str_time[10] = {0};
    IPC_MID_UTIL_GetDateAndTimeStr(date, str_time, 0);

    char date_path[128] = {0};
    sprintf(date_path, "%s/%s", IPC_SD_SNAP_PATH, date); // str = /mnt/sd_card/img/20120914
    if(access(date_path, F_OK) != 0 )
    {
        ret = mkdir(date_path, 0777);
        if(ret < 0)
        {
            PRINT_ERR("mkdir %s failed\n", date_path);
            return IPC_SD_STATUS_ERROR;
        }
    }

    sprintf(jpg_path, "%s/%d_%s.jpg", date_path, stream_type, str_time);

    return 0;
}

FILE *snap_open(int stream_type)
{
    int ret;

    char jpg_path[128] = {0};
    memset(jpg_path, 0, sizeof(jpg_path));
    ret = snap_get_path(stream_type, jpg_path);
    if (ret < 0) {
        PRINT_ERR("get snap file path error\n");
        return NULL;
    }

    FILE *pFile = NULL;
	pFile = fopen(jpg_path, "wb+");
	if (pFile == NULL) {
		PRINT_ERR("IPC_PF_VENC_CaptureJpeg open file=%s err\n", jpg_path);
		return NULL;
	}

    return pFile;
}

void snap_close(FILE *pFile)
{
    fflush(pFile);
	fclose(pFile);
}

int snap_write(FILE *pFile, char *data, int len)
{
    int ret;

    #if 0
    ret = snap_check_sd_space(len);
    if (ret < 0) {
        PRINT_ERR("sd no space!\n");
        return -1;
    }
    #endif

    ret = fwrite(data, len, 1, pFile);
    if (ret < 0) {
        PRINT_ERR("snap write error!\n");
        return -1;
    }

    return 0;
}

int snap_mv_to_sd(char *src, int stream_type, int len)
{
    //check the sd card is mounted.
    if(IPC_MID_RECMNG_IsSDMount() < 0)
    {
        PRINT_ERR("sd card is not mounted.\n");
        return IPC_SD_STATUS_NOTEXIST;
    }

    if (IPC_MID_RECMNG_GetSDFreeSize() < ((len >> 10) + 1))
    {
        PRINT_ERR("snap, no space, free:%d datalen:%d.\n",
                       IPC_MID_RECMNG_GetSDFreeSize(), (len >> 10) + 1);
        return IPC_SD_STATUS_NO_SPACE;
    }

    int ret = 0;
    if(access(IPC_SD_SNAP_PATH, F_OK) != 0 )
    {
        ret = mkdir(IPC_SD_SNAP_PATH, 0777);
        if(ret < 0)
        {
            PRINT_ERR("mkdir %s failed\n", IPC_SD_SNAP_PATH);
            return IPC_SD_STATUS_ERROR;
        }
    }

    char jpg_path[128] = {0};
    snap_get_path(stream_type, jpg_path);

    char cmd[128];
    memset(cmd, 0, sizeof(cmd));
    sprintf(cmd, "mv  %s  %s", src, jpg_path);
    PRINT_INFO("cmd:%s\n", cmd);
    IPC_MID_UTIL_SysCall(cmd, NULL, 0);

    return 0;
}

AR_S32 IPC_MID_RECMNG_Snapshot(AR_U32 u32StreamId)
{
    //capture a snap and save to its location
    return 0;
}


