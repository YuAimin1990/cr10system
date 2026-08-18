#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#include "uvc_configfs.h"

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

int getCtrlInterfaceNum(int index)
{
    int ret = -1;
    char path[256] = {""};

    sprintf(path, PATH_OF_CONTROL_INTERFACE, index);

    ret = readConfigfsValue(path);
    if(ret < 0)
    {
        printf("read failed!\n");
    }
    printf("get control bInterfaceNumber: %d\n", ret);
    return ret;
}

int getStreamInterfaceNum(int index)
{
    int ret = -1;
    char path[256] = {""};

    sprintf(path, PATH_OF_STREAM_INTERFACE, index);

    ret = readConfigfsValue(path);
    if(ret < 0)
    {
        printf("read failed!\n");
    }
    printf("get streaming bInterfaceNumber: %d\n", ret);
    return ret;
}


int getMaxPayloadTransferSize(int index)
{
    int ret = 3072;
    char path[256] = {""};

    sprintf(path, PATH_OF_STREAM_MAXPACKET, index);

    ret = readConfigfsValue(path);
    if(ret < 0)
    {
        printf("read failed!\n");
    }
    printf("get streaming_maxpacket: %d\n", ret);
    return ret;
}

int getMaxPayloadTransferSizeBulk(int index)
{
    int ret = 1024;
    char path[256] = {""};

    if(readConfigfsHexValue(PATH_OF_USBGADGET_BCDUSB) >= USB_30_VALUE)
    {
        ret = (getTransferMaxburst(index) + 1) *
              (getMaxPayloadTransferSize(index) >= 1024 ? 1024 :
               getMaxPayloadTransferSize(index));
    }
    else if(readConfigfsHexValue(PATH_OF_USBGADGET_BCDUSB) >= USB_20_VALUE)
    {
        ret = (getMaxPayloadTransferSize(index) >= 512 ? 512 :
               getMaxPayloadTransferSize(index));
    }
    else
    {
        ret = (getMaxPayloadTransferSize(index) >= 64 ? 64 :
               getMaxPayloadTransferSize(index));
    }

    sprintf(path, PATH_OF_STREAM_MAXPAYLOAD, index);

    if(readConfigfsValue(path) > ret)
    {
        ret = readConfigfsValue(path);
        if(ret < 0)
        {
            printf("read failed!\n");
        }
    }

    printf("get streaming_maxpayload: %d\n", ret);
    return ret;
}


int getTransferInterval(int index)
{
    int ret = 1;
    char path[256] = {""};

    sprintf(path, PATH_OF_STREAM_INTERVAL, index);

    ret = readConfigfsValue(path);
    if(ret < 0)
    {
        printf("read failed!\n");
    }
    printf("get streaming_interval: %d\n", ret);
    return ret;
}

int getTransferMaxburst(int index)
{
    int ret = 0;
    char path[256] = {""};

    sprintf(path, PATH_OF_STREAM_MAXBURST, index);

    ret = readConfigfsValue(path);
    if(ret < 0)
    {
        printf("read failed!\n");
    }
    printf("get streaming_maxburst: %d\n", ret);
    return ret;
}


int getConfigfsUDCName(char * buf, int buf_size)
{
    char path[64] = {""};
    int fd = -1;
    int ret = -1;

    int len = 0;
    fd = open(PATH_OF_USBGADGET_UDC, O_RDONLY);
    if (fd < 0)
    {
        printf("open %s failed!\n", path);
        close(fd);
        return ret;
    }

    len = read(fd, buf, buf_size);
    if(len < 0)
    {
        printf("read failed!\n");
    }
    else
    {
        ret = 0;
    }

    close(fd);
    return ret;
}


int getConfigfsbcdUSB(void)
{
    return readConfigfsHexValue(PATH_OF_USBGADGET_BCDUSB);
}


static int getConfigfsResolution(char * path, struct uvc_frame_info ** frame)
{
    int resolution;
    int resolution_num = 0;
    struct uvc_frame_info resolutions[__RESOLUTION_NUM__];
    char path_name[256] = {""};

    memset(resolutions, 0, sizeof(resolutions));

    for(resolution = 1; resolution < __RESOLUTION_NUM__; ++resolution)
    {
        sprintf(path_name, "%s/%d", path, resolution);
        if(opendir(path_name))
        {
            char tmp_path[256] = {""};
            int bFrameIndex = -1;
            int wHeight = -1;
            int wWidth = -1;

            sprintf(tmp_path, "%s/bFrameIndex", path_name);
            bFrameIndex = readConfigfsValue(tmp_path);
            if(bFrameIndex < 0)
            {
                continue;
            }

            sprintf(tmp_path, "%s/wHeight", path_name);
            wHeight = readConfigfsValue(tmp_path);
            if(wHeight < 0)
            {
                continue;
            }

            sprintf(tmp_path, "%s/wWidth", path_name);
            wWidth = readConfigfsValue(tmp_path);
            if(wWidth < 0)
            {
                continue;
            }

            resolutions[bFrameIndex - 1].height = wHeight;
            resolutions[bFrameIndex - 1].width = wWidth;

            sprintf(tmp_path, "%s/dwFrameInterval", path_name);
            readConfigfsArray(tmp_path, resolutions[bFrameIndex - 1].intervals, sizeof(resolutions[bFrameIndex - 1].intervals));

            ++resolution_num;
        }
        else
        {
            break;
        }
    }

    *frame = (struct uvc_frame_info *)calloc(resolution_num + 1, sizeof(struct uvc_frame_info));//alloc n+1 unit
    memcpy(*frame, resolutions, resolution_num * sizeof(struct uvc_frame_info));

    return resolution_num;
}

static int getConfigfsFrame(struct uvc_format_info * uvc_format, int index)
{
    int ret = -1;
    char path[256] = {""};

    switch(uvc_format->fcc)
    {
        case V4L2_PIX_FMT_HEVC:
            sprintf(path, PATH_OF_STREAM_FORMAT_H265, index);
            ret = getConfigfsResolution(path, &(uvc_format->frames));
            break;

        case V4L2_PIX_FMT_H264:
            sprintf(path, PATH_OF_STREAM_FORMAT_H264, index);
            ret = getConfigfsResolution(path, &(uvc_format->frames));
            break;

        case V4L2_PIX_FMT_MJPEG:
            sprintf(path, PATH_OF_STREAM_FORMAT_MJPEG, index);
            ret = getConfigfsResolution(path, &(uvc_format->frames));
            break;

        case V4L2_PIX_FMT_NV12:
            sprintf(path, PATH_OF_STREAM_FORMAT_NV12, index);
            ret = getConfigfsResolution(path, &(uvc_format->frames));
            break;

        case V4L2_PIX_FMT_YUYV:
            sprintf(path, PATH_OF_STREAM_FORMAT_YUY2, index);
            ret = getConfigfsResolution(path, &(uvc_format->frames));
            break;

        case V4L2_PIX_FMT_Z16:
            sprintf(path, PATH_OF_STREAM_FORMAT_Z16, index);
            ret = getConfigfsResolution(path, &(uvc_format->frames));
            break;

        default:
            break;
    }

    return ret;
}

int getConfigfsFormat(struct uvc_format_info ** uvc_formats, int index)
{
    int ret = -1;
    int format_h265 = 0;
    int format_h264 = 0;
    int format_mjpeg = 0;
    int format_nv12 = 0;
    int format_yuy2 = 0;
    int format_z16 = 0;
    int format_num = 0;
    char path[256] = {""};
    uvc_server_t * server_l = get_uvc_server();

    sprintf(path, PATH_OF_STREAM_FORMAT_H265, index);
    if(NULL != opendir(path))
    {
        format_h265 = 1;
    }
    else
    {
        //printf("can not open %s\n", path);
    }

    sprintf(path, PATH_OF_STREAM_FORMAT_H264, index);
    if(NULL != opendir(path))
    {
        format_h264 = 1;
    }
    else
    {
        //printf("can not open %s\n", path);
    }

    sprintf(path, PATH_OF_STREAM_FORMAT_MJPEG, index);
    if(NULL != opendir(path))
    {
        format_mjpeg = 1;
    }
    else
    {
        //printf("can not open %s\n", path);
    }

    sprintf(path, PATH_OF_STREAM_FORMAT_NV12, index);
    if(NULL != opendir(path))
    {
        format_nv12 = 1;
    }
    else
    {
        //printf("can not open %s\n", path);
    }

    sprintf(path, PATH_OF_STREAM_FORMAT_YUY2, index);
    if(NULL != opendir(path))
    {
        format_yuy2 = 1;
    }
    else
    {
        //printf("can not open %s\n", path);
    }

    sprintf(path, PATH_OF_STREAM_FORMAT_Z16, index);
    if(NULL != opendir(path))
    {
        format_z16 = 1;
    }
    else
    {
        //printf("can not open %s\n", path);
    }

    if(1 == server_l->is_raw && format_h265 + format_h264 + format_mjpeg + format_nv12 > 0)
    {
        printf("raw mode can not support h265, h264, mjpeg and nv12!!!\n");
        printf("raw mode can not support h265, h264, mjpeg and nv12!!!\n");
        printf("raw mode can not support h265, h264, mjpeg and nv12!!!\n");
        return -1;
    }

    if(format_h265 + format_h264 + format_mjpeg + format_nv12 + format_yuy2 + format_z16 > 0)
    {
        int ret0 = -1, ret1 = -1, ret2 = -1, ret3 = -1, ret4 = -1, ret5 = -1;
        *uvc_formats = (struct uvc_format_info *)calloc((format_h265 + format_h264 + format_mjpeg + format_nv12 + format_yuy2 + format_z16),
                                                        sizeof(struct uvc_format_info));
        if(NULL != *uvc_formats)
        {
            if(1 == format_h265)
            {
                (*uvc_formats)[format_num].fcc = V4L2_PIX_FMT_HEVC;
                (*uvc_formats)[format_num].data = &uvc_h265_data;
                ret0 = getConfigfsFrame(&((*uvc_formats)[format_num]), index);
                ++format_num;
            }

            if(1 == format_h264)
            {
                (*uvc_formats)[format_num].fcc = V4L2_PIX_FMT_H264;
                (*uvc_formats)[format_num].data = &uvc_h264_data;
                ret1 = getConfigfsFrame(&((*uvc_formats)[format_num]), index);
                ++format_num;
            }

            if(1 == format_mjpeg)
            {
                (*uvc_formats)[format_num].fcc = V4L2_PIX_FMT_MJPEG;
                (*uvc_formats)[format_num].data = &uvc_mjpeg_data;
                ret2 = getConfigfsFrame(&((*uvc_formats)[format_num]), index);
                ++format_num;
            }

            if(1 == format_nv12)
            {
                (*uvc_formats)[format_num].fcc = V4L2_PIX_FMT_NV12;
                (*uvc_formats)[format_num].data = &uvc_nv12_data;
                ret3 = getConfigfsFrame(&((*uvc_formats)[format_num]), index);
                ++format_num;
            }

            if(1 == format_yuy2)
            {
                (*uvc_formats)[format_num].fcc = V4L2_PIX_FMT_YUYV;
                (*uvc_formats)[format_num].data = &uvc_yuy2_data;
                ret4 = getConfigfsFrame(&((*uvc_formats)[format_num]), index);
                ++format_num;
            }

            if(1 == format_z16)
            {
                (*uvc_formats)[format_num].fcc = V4L2_PIX_FMT_Z16;
                (*uvc_formats)[format_num].data = &uvc_z16_data;
                ret5 = getConfigfsFrame(&((*uvc_formats)[format_num]), index);
                ++format_num;
            }

            if(ret0 < 0 && ret1 < 0 && ret2 < 0 && ret3 < 0 && ret4 < 0 && ret5 < 0)
            {
                printf("Frame info get failed!\n");
                return -1;
            }
            return format_num;
        }
    }
    else
    {
        printf("NO uvc format register!\n");
    }

    return ret;
}

void freeConfigfsFormat(struct uvc_format_info ** uvc_formats, int format_num)
{
    int i;
    if(NULL == uvc_formats)
        return;

    for(i = 0; i < format_num; ++i)
    {
        free((*uvc_formats)->frames);
        (*uvc_formats)->frames = NULL;
    }

    free(*uvc_formats);
    *uvc_formats = NULL;
}

