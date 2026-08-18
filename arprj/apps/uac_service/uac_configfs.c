#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#include "uac_configfs.h"

static int readConfigfsValue(char * path)
{
    char    buf[32] = {""};
    int     fd      = -1;
    int     ret     = -1;

    fd = open(path, O_RDONLY);
    if (fd < 0)
    {
        printf("open %s failed!\n", path);
        return -1;
    }

    ret = read(fd, buf, sizeof(buf));
    if(ret < 0)
    {
        printf("read failed!\n");
        close(fd);
        return ret;
    }

    ret = atoi(buf);
    close(fd);

    return ret;
}
#if 0
static int readConfigfsHexValue(char * path)
{
    char    buf[32] = {""};
    int     fd      = -1;
    int     ret     = -1;

    fd = open(path, O_RDONLY);
    if (fd < 0)
    {
        printf("open %s failed!\n", path);
        return -1;
    }

    ret = read(fd, buf, sizeof(buf));
    if(ret < 0)
    {
        printf("read failed!\n");
        close(fd);
        return ret;
    }

    ret = strtol(buf, NULL, 16);
    close(fd);

    return ret;
}


static int readConfigfsArray(char * path, unsigned int * array, int max_num)
{
    char    buf[256] = {""};
    int     fd      = -1;
    int     ret     = -1;
    int     i       = 0;
    char   *p       = buf;

    fd = open(path, O_RDONLY);
    if (fd < 0)
    {
        printf("open %s failed!\n", path);
        return -1;
    }

    ret = read(fd, buf, sizeof(buf));
    if(ret < 0)
    {
        printf("read failed!\n");
        close(fd);
        return ret;
    }

    for(i = 0; i < max_num; ++i)
    {
        int count;
        ret = sscanf(p, "%u\n%n", &(array[i]), &count);
        if(ret <= 0)
        {
            break;
        }
        p += count;
    }

    close(fd);

    return i;
}
#endif
int getPlayMask(int index)
{
    int ret = -1;
    char path[256] = {""};

    sprintf(path, AR_UAC_PLAYBACK_MASK_PATH, index);

    ret = readConfigfsValue(path);
    if(ret < 0)
    {
        printf("read failed!\n");
    }
    printf("get playback mask: %x\n", ret);
    return ret;
}

int getCaptureMask(int index)
{
    int ret = -1;
    char path[256] = {""};

    sprintf(path, AR_UAC_CAPTURE_MASK_PATH, index);

    ret = readConfigfsValue(path);
    if(ret < 0)
    {
        printf("read failed!\n");
    }
    printf("get capture mask: %x\n", ret);
    return ret;
}
