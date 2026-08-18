#define _GNU_SOURCE
#define ENABALE_ALL_LOG
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <sched.h>
#include <linux/usb/ch9.h>
#include <linux/usb/video.h>
#include <linux/videodev2.h>
#include <linux/usb/uvc.h>
#include <pthread.h>

#include <ar_comm_vb.h>
#include <ar_comm_video.h>

#include <mpi_sys.h>
#include <mpi_vb.h>
#include <mpi_vi.h>

#include <hal_vb.h>
#include <rpc_proxy.h>

#include "uvc_service2.h"
#include "pipeline_service.h"
#include "uvc_configfs.h"
#include "uvc_function.h"

#include "ar_buffer.h"
//#include "ar_time.h"

#define round_boundary(value, boundary)         \
        ((__typeof__(value))((boundary) - 1))

#define round_down(value, boundary)             \
        ((value) & ~round_boundary(value, boundary))

struct uvc_format_info *g_uvc_formats = NULL;
int g_uvc_formats_num = 0;

uvc_server_t *get_uvc_server()
{
    static uvc_server_t *p_uvc_server = NULL;
    if(!p_uvc_server)
    {
        p_uvc_server=malloc(sizeof(uvc_server_t));
        memset(p_uvc_server,0,sizeof(uvc_server_t));
        p_uvc_server->run_flag=1;
    }
    return p_uvc_server;
}

//capture exit signal
static void sighandler(int signo)
{
    uvc_server_t *server_l = get_uvc_server();
    if (signo == SIGINT || signo == SIGQUIT || signo == SIGKILL || signo == SIGTERM)
    {
        server_l->run_flag = 0;
        log_err("uvc gadget exit\n");
    }
}

//help function
static void usage(const char *argv0)
{
    fprintf(stderr, "Usage: %s [options]\n", argv0);
    fprintf(stderr, "Available options are\n");
    fprintf(stderr, " -b            Use bulk mode\n");
    fprintf(stderr, " -d device     Video device\n");
    fprintf(stderr, " -i image      MJPEG image\n");
}

//find output device
static int uvc_dev_recognition(char* buf, int len, int mode)
{
    int i = 0, j = 0;
    char* uvc_flag = __OUTPUT_DEVICE_NAME__;

    if(1 == mode)
    {
        uvc_flag = __OUTPUT_DEVICE_NAME2__;
    }

    for(i = 0; i < strlen(__OUTPUT_DEVICE_NAME__) - 1; i++)
    {
        while(j < len && uvc_flag[i] != buf[j])
            j++;
        if(j == len)
            return -1;
    }
    return 0;
}

//open device
static struct uvc_device * uvc_open(const char *devname, int fps)
{
    struct uvc_device *dev;
    struct v4l2_capability cap;
    int ret;
    int fd = -1;
    uvc_server_t       *server_l    = get_uvc_server();

    fd = open(devname, O_RDWR | O_NONBLOCK);
    if (fd < 0) {
        log_err("v4l2 open failed: %s (%d)\n", strerror(errno), errno);
        return NULL;
    }

    log_always("open %s succeeded, file descriptor = %d\n", devname, fd);

    ret = ioctl(fd, VIDIOC_QUERYCAP, &cap);
    if (ret < 0) {
        log_err("unable to query device: %s (%d)\n", strerror(errno),
            errno);
        close(fd);
        return NULL;
    }

    log_printf("***********************Device info*********************\n");
    log_printf("device is %s on bus %s\n", cap.card, cap.bus_info);
    log_printf("device driver      : \t%s\n", cap.driver);
    log_printf("device card        : \t%s\n", cap.card);
    log_printf("device capabilities: \t0x%08x\n", cap.capabilities);
    log_printf("device device_caps : \t0x%08x\n", cap.device_caps);
    log_printf("*******************************************************\n");

    dev = malloc(sizeof *dev);
    if (dev == NULL) {
        close(fd);
        return NULL;
    }

    memset(dev, 0, sizeof *dev);
    dev->fd = fd;
    dev->width = server_l->width[0];
    dev->height = server_l->height[0];
    dev->frame_interval = (10000000 / fps);
    dev->fcc = g_uvc_formats[0].fcc;
    dev->fcc_index = 0;

    return dev;
}

static void uvc_close(struct uvc_device *dev)
{
    if(dev)
    {
       close(dev->fd);
       dev->fd = -1;
       //free(dev->imgdata);
       free(dev->mem);
       free(dev);
    }
}

//load still image
static void image_load(struct uvc_device *dev, const char *img)
{
    int fd = -1;
    uvc_server_t *server_l=get_uvc_server();

    if (img == NULL)
        return;

    fd = open(img, O_RDONLY);
    if (fd == -1) {
        log_err("Unable to open MJPEG image '%s'\n", img);
        return;
    }

    dev->imgsize = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    dev->imgdata = malloc(server_l->g_max_buffer_size);
    if (dev->imgdata == NULL) {
        log_err("Unable to allocate memory for MJPEG image\n");
        dev->imgsize = 0;
        return;
    }

    read(fd, dev->imgdata, dev->imgsize);
    server_l->first_image_data = dev->imgdata;
    server_l->first_image_size = dev->imgsize;
    close(fd);
}

/****************************************WORKER FUNCTION**************************************/
void *get_uvc_video_form_uvc_sink(void * para)
{
    uvc_server_t                    *server_l               = get_uvc_server();
    struct uvc_frame_buf_info       *buf_info[BUFFER_COUNT] = {0};
    //int                             count                   = 500;
    //char                            path[128]               = {""};
    //basic_3a_info_t                 info;
    fd_set                          fds;
    struct timeval                  tv;
    unsigned int                    tmp_index, fcc_index;
    struct uvc_format_opt           *format_opt;
    int                             ret = -1;
    int                             get_frame_succ = 0;
    ISP_AEC_MANU_TIDY_ATTR_S        aec_attr = {0};

    while(server_l->run_flag)
    {
        switch(server_l->status)
        {
            case UVC_STATUS_IDLE:
                server_l->status = UVC_STATUS_INIT;
                break;
            case UVC_STATUS_INIT:

                if(NULL == g_uvc_formats[server_l->dev->fcc_index].data || NULL == g_uvc_formats[server_l->dev->fcc_index].data->format_opt)
                {
                    log_err("g_uvc_formats get error!\n");
                    server_l->status = UVC_STATUS_EXIT;
                    break;
                }

                if(g_uvc_formats[server_l->dev->fcc_index].data->format_opt->format_init(
                                                            g_uvc_formats[server_l->dev->fcc_index].data,
                                                            server_l->handle,
                                                            server_l->argc,
                                                            server_l->argv))
                {
                    log_err("g_uvc_formats init error!\n");
                    server_l->status = UVC_STATUS_EXIT;
                    break;
                }

                log_always("cam_index=%d\n",server_l->cam_index);

                FD_ZERO(&fds);
                FD_SET(server_l->dev->fd, &fds);

                server_l->status = UVC_STATUS_INIT_COMPLETE;
                break;

            case UVC_STATUS_INIT_COMPLETE:
            
                AR_MPI_ISP_GetAecManuTidyAttr(server_l->cam_index, &aec_attr);
                
                server_l->dev->camera_terminal.exposure_time_absolute_val       = aec_attr.stAecManu.exp_time_us * 10000;//server_l->CT_BRIGHTNESS_DEFAULT_VAL;
                server_l->dev->camera_terminal.auto_exposure_aec_info           = aec_attr.stAecManu;
                server_l->status = UVC_STATUS_RELEASE_FRAME;

                break;

            case UVC_STATUS_RELEASE_FRAME:

                tmp_index   = server_l->idx % BUFFER_COUNT;
                fcc_index   = server_l->dev->fcc_index;
                format_opt  = g_uvc_formats[fcc_index].data->format_opt;

                if(NULL != buf_info[tmp_index])
                {
                    if(format_opt->release_frame(g_uvc_formats[fcc_index].data, buf_info[tmp_index]))
                    {
                        log_err("release buffer fail!! %s line:%d\n", __func__, __LINE__);
                        server_l->status = UVC_STATUS_EXIT;
                        break;
                    }

                    buf_info[tmp_index] = NULL;
                }

                if(1 == server_l->g_chg_fmt_flag)
                {
                    server_l->status = UVC_STATUS_CHANGE_FORMAT;
                }
                else
                {
                    server_l->status = UVC_STATUS_GET_FRAME;
                }

                break;

            case UVC_STATUS_GET_FRAME:

                ret = -1;
                tv.tv_sec   = 0;
                tv.tv_usec  = 10000;

                tmp_index   = server_l->idx % BUFFER_COUNT;
                fcc_index   = server_l->dev->fcc_index;
                format_opt  = g_uvc_formats[fcc_index].data->format_opt;

                buf_info[tmp_index] = format_opt->get_frame(g_uvc_formats[fcc_index].data);
                if(!buf_info[tmp_index])
                {
                    log_err("get buffer fail!! %s line:%d\n", __func__, __LINE__);
                }
                else
                {
                    if (server_l->idx == 0 && get_frame_succ == 0)
                    {
                        log_printf("UVC get frame success\n");
                        get_frame_succ = 1;
                    }
                    // struct timeval tv0, tv1;
                    // gettimeofday(&tv0, NULL);
                    if (buf_info[tmp_index]->len > server_l->g_max_buffer_size)
                    {
                        log_info("error package length over max buffer size %u bytes(%u bytes)\n", server_l->g_max_buffer_size, buf_info[tmp_index]->len);
                        ret = uvc_video_send(server_l->dev, (void *)buf_info[tmp_index]->addr, server_l->g_max_buffer_size, buf_info[tmp_index]->pts, fds, tv);
                    }
                    else
                    {
                        //log_info("package length max buffer %p %u bytes)\n", (void *)buf_info[tmp_index]->addr, buf_info[tmp_index]->len);
                        ret = uvc_video_send(server_l->dev, (void *)buf_info[tmp_index]->addr, buf_info[tmp_index]->len, buf_info[tmp_index]->pts, fds, tv);
                    }
                    // gettimeofday(&tv1, NULL);
                    // if(access("/tmp/cam0", F_OK) == 0 && server_l->cam_index == 0)
                    //     printf("cam %d send frame cost %ld ms\n", server_l->cam_index, (tv1.tv_sec*1000+tv1.tv_usec/1000) - (tv0.tv_sec*1000+tv0.tv_usec/1000));
                    // if(access("/tmp/cam1", F_OK) == 0 && server_l->cam_index == 1)
                    //     printf("cam %d send frame cost %ld ms\n", server_l->cam_index, (tv1.tv_sec*1000+tv1.tv_usec/1000) - (tv0.tv_sec*1000+tv0.tv_usec/1000));
                }

                if(ret < 0)
                {
                    //log_err("uvc_video_send failed!\n");
                }
                else
                {
                    server_l->idx++;
                }

                if(1 == server_l->g_chg_fmt_flag)
                {
                    server_l->status = UVC_STATUS_CHANGE_FORMAT;
                }
                else
                {
                    server_l->status = UVC_STATUS_RELEASE_FRAME;
                }

                break;

            case UVC_STATUS_CHANGE_FORMAT:

                //if(server_l->dev->new_fcc != server_l->dev->fcc)
                {
                    int i;

                    printf("release buffer!\n");

                    //release frame
                    fcc_index   = server_l->dev->fcc_index;
                    format_opt  = g_uvc_formats[fcc_index].data->format_opt;
                    for(i = 0; i < BUFFER_COUNT; ++i)
                    {
                        if(NULL != buf_info[i])
                        {
                            if(format_opt->release_frame(g_uvc_formats[fcc_index].data, buf_info[i]))
                            {
                                log_err("release buffer fail!! %s line:%d\n", __func__, __LINE__);
                            }
                            buf_info[i] = NULL;
                        }
                    }
                }

                uvc_pipeline_set_format(server_l->dev->width, server_l->dev->height, 10000000/server_l->dev->frame_interval, server_l->dev->new_fcc);

                uvc_video_set_format(server_l->dev);

                server_l->g_chg_fmt_flag = 0;
                sem_post(&server_l->g_chg_fmt_sem);//release lock

                server_l->status = UVC_STATUS_INIT_COMPLETE;
                break;

            case UVC_STATUS_EXIT:
            case UVC_STATUS_MAX:
            default:
                if(1 == server_l->run_flag)
                    server_l->run_flag = 0;

                break;
        }
    }

    fcc_index   = server_l->dev->fcc_index;
    format_opt  = g_uvc_formats[fcc_index].data->format_opt;

    while(NULL != buf_info[server_l->idx % BUFFER_COUNT])
    {
        if(format_opt->release_frame(g_uvc_formats[fcc_index].data, buf_info[server_l->idx % BUFFER_COUNT]))
        {
            log_err("release buffer fail!! %s line:%d\n", __func__, __LINE__);
            break;
        }

        buf_info[server_l->idx % BUFFER_COUNT] = NULL;

        server_l->idx++;
    }
    
    format_opt->format_deinit(g_uvc_formats[fcc_index].data, server_l->handle);

    if(1 == server_l->g_chg_fmt_flag)
    {
        server_l->g_chg_fmt_flag = 0;
        sem_post(&server_l->g_chg_fmt_sem);//release lock
    }

    log_err("%s exit!\n", __func__);

    return NULL;
}
#if 0
static void *uvc_mmap(unsigned long pa, size_t length, int fd)
{
    unsigned int    page_size       = sysconf(_SC_PAGE_SIZE);
    off_t           offset          = round_down(pa, page_size);
    void            *real_start_va  = NULL;

    length += (pa & (page_size - 1));
    real_start_va = mmap(NULL, length, PROT_READ|PROT_WRITE, MAP_SHARED, fd, offset);
    return (real_start_va + (pa & (page_size - 1)));
}

static int uvc_munmap(void *real_start_va, unsigned long pa, size_t length)
{
    int ret;
    unsigned int    page_size       = sysconf(_SC_PAGE_SIZE);

    if(NULL == real_start_va)
        return 0;

    real_start_va -= (pa & (page_size - 1));
    length += (pa & (page_size - 1));
    ret = munmap(real_start_va, length);

    return ret;
}
#endif
static int start_uvc_trans(int argc, char *argv[])
{
    AR_S32                  s32Index        = 0;
    AR_S32                  s32Len          = 0;
    AR_S32                  s32Ret          = 0;
    AR_CHAR                *strDevice       = NULL;
    AR_S32                  s32BulkMode     = 0;
    AR_CHAR                *strMjpegImg     = NULL;
    AR_S32                  s32FindMode     = 0;
    AR_CHAR                 strNameBuf[32]  = {""};
    AR_CHAR                 strPath[128]    = {""};
    AR_S32                  s32Fps          = 30;
    AR_S32                  i, j;
    AR_S32                  fd              = -1;
    pthread_attr_t          attr;
    fd_set                  fds;
    struct timeval          tv;
    pthread_t               thread_getbuf;
    //basic_3a_info_t       info;
    
    uvc_server_t           *server_l        = get_uvc_server();

    pthread_mutex_init(&server_l->g_wrt_com_mutex, NULL);
    sem_init(&server_l->g_chg_fmt_sem, 0, 1);
    pthread_attr_init(&attr);

    s32Index = uvc_find_pra_index_by_name("-d",argc,argv);
    if(s32Index > 0)
    {
        strDevice = argv[s32Index + 1];
    }

    s32Index = uvc_find_pra_index_by_name("-b",argc,argv);
    if(s32Index > 0)
    {
        s32BulkMode = 1;
    }

    s32Index = uvc_find_pra_index_by_name("-i",argc,argv);
    if(s32Index > 0)
    {
        strMjpegImg = argv[s32Index + 1];
        server_l->is_test = 1;
    }

    s32Index = uvc_find_pra_index_by_name("--fps",argc,argv);
    if(s32Index > 0)
    {
        s32Fps = atoi(argv[s32Index + 1]);
    }

    g_uvc_formats_num = getConfigfsFormat(&g_uvc_formats, server_l->dev_index);
    if(g_uvc_formats_num < 0)
    {
        printf("UVC configfs info get failed!\n");
        return -1;
    }

    printf("***********************g_uvc_formats_num: %d***********************\n", g_uvc_formats_num);
    for(i = 0; i < g_uvc_formats_num; ++i)
    {
        char * p = (char *)&(g_uvc_formats[i].fcc);
        printf("format: %c%c%c%c\n", p[0], p[1], p[2], p[3]);
        j = 0;
        for(j = 0; 0 != g_uvc_formats[i].frames[j].height; ++j)
        {
            int k;
            printf("    %d. %d * %d:\n", j, g_uvc_formats[i].frames[j].width, g_uvc_formats[i].frames[j].height);
            printf("        interval: ");
            for(k = 0; k < 8; ++k)
            {
                printf("%d ", g_uvc_formats[i].frames[j].intervals[k]);
            }
            printf("\n");
        }
    }
    printf("*****************************************************************\n");

    if(getConfigfsUDCName(strNameBuf, 32) < 0)
    {
        printf("USB gadget UDC get failed!\n");
        return -1;
    }
    else
    {
        if(0 == strncmp(strNameBuf, "60300000", strlen("60300000")))
        {
            s32FindMode = 1;
        }
    }

    //if no appointed device auto find a usb gadget webcamera(output)
    if(NULL == strDevice)
    {
        for (i = 0; i < CAM_NUM_MAX; i++)
        {
            memset(strNameBuf, 0, sizeof(strNameBuf));
            sprintf(strPath, "/sys/class/video4linux/video%d/name", i);
            fd = open(strPath, O_RDONLY);
            if (fd < 0)
            {
                log_err("open %s failed!\n", strPath);
                continue;
            }
            s32Len = read(fd, strNameBuf, sizeof(strNameBuf));
            s32Ret = uvc_dev_recognition(strNameBuf, s32Len, s32FindMode);
            if (!s32Ret)
            {
                 strDevice = (char *)malloc(32);
                 sprintf(strDevice, "/dev/video%d", i + server_l->dev_index);
                 break;
            }
        }
    }

    //find device open it or exit
    if(NULL != strDevice)
    {
        server_l->dev = uvc_open(strDevice, s32Fps);
        if (server_l->dev == NULL)
            return -1;
    }
    else
    {
        log_err("auto find video device failed!\n");
        goto auto_find_fail;
    }

    server_l->dev->camera_terminal.auto_exposure_mode_control       = CT_AUTO_EXPOSURE_MODE_DEFAULF_VAL;
    server_l->dev->camera_terminal.zoom_absolute_val                = CT_ZOOM_ABSOLUTE_DEFAULT_VAL;
    server_l->dev->camera_terminal.roll_absolute_val                = CT_ROLL_ABSOLUTE_DEFAULT_VAL;
    server_l->dev->processing_unit.brightness_val                   = PU_BRIGHTNESS_DEFAULT_VAL;
    server_l->dev->processing_unit.contrast_val                     = PU_CONTRAST_DEFAULT_VAL;
    server_l->dev->processing_unit.hue_val                          = PU_HUE_DEFAULT_VAL;
    server_l->dev->processing_unit.saturation_val                   = PU_SATURATION_DEFAULT_VAL;
    server_l->dev->processing_unit.sharpness_val                    = PU_SHARPNESS_DEFAULT_VAL;
    server_l->dev->processing_unit.gamma_val                        = PU_GAMMA_DEFAULT_VAL;
    server_l->dev->processing_unit.white_balance_temperature_val    = PU_WHITE_BALANCE_TEMPERATURE_DEFAULT_VAL;
    server_l->dev->processing_unit.white_balance_component_val      = 0;
    server_l->dev->processing_unit.backlight_compensation_val       = PU_BACKLIGHT_COMPENSATION_DEFAULT_VAL;
    server_l->dev->processing_unit.gain_val                         = PU_GAIN_DEFAULT_VAL;
    server_l->dev->processing_unit.power_line_frequency_val         = PU_POWER_LINE_FREQUENCY_DEFAULT_VAL;
    server_l->dev->processing_unit.hue_auto_val                     = PU_HUE_AUTO_DEFAULT_VAL;
    server_l->dev->processing_unit.white_balance_temperature_auto_val = WHITE_BALANCE_TEMPERATURE_AUTO_DEFAULT_VAL;

    server_l->dev->bulk = s32BulkMode;
    server_l->dev->control_interface = 0;
    server_l->dev->streaming_interface = 1;

    if((server_l->dev->control_interface = getCtrlInterfaceNum(server_l->dev_index)) < 0)
    {
        log_err("auto get control interface failed!\n");
    }
    else
    {
        log_info("pid %d get control bInterfaceNumber success(%d)!\n", getpid(), server_l->dev->control_interface);
    }

    if((server_l->dev->streaming_interface = getStreamInterfaceNum(server_l->dev_index)) < 0)
    {
        log_err("auto get stream interface failed!\n");
    }
    else
    {
        log_info("pid %d get streaming bInterfaceNumber success(%d)!\n", getpid(), server_l->dev->streaming_interface);
    }

    //init uvc
    uvc_events_init(server_l->dev);
    uvc_video_init(server_l->dev);

    //if want output mjpeg image, load image
    if(NULL != strMjpegImg)
    {
        image_load(server_l->dev, strMjpegImg);
    }
    else//get video data form uvc sink
    {
        /* avoid broken frame in plugin */
        server_l->first_image_data = (void *)malloc(server_l->g_max_buffer_size * sizeof(char));
        memset(server_l->first_image_data, 0x0, server_l->g_max_buffer_size);
        server_l->first_image_size = server_l->g_max_buffer_size / (1024 * 16);

        server_l->cache_image_align = 256;
        server_l->cache_image_size = server_l->g_max_buffer_size * 2;

        server_l->cache_image_data_org = malloc(server_l->cache_image_size + server_l->cache_image_align);
        if(NULL == server_l->cache_image_data_org)
        {
            log_err("malloc cache_image_data failed!\n");
            goto init_failed;
        }

        server_l->cache_image_data =
            (void *)((((unsigned long)server_l->cache_image_data_org) + server_l->cache_image_align - 1) & (~(server_l->cache_image_align - 1)));
        server_l->cache_image_point = 0;

        server_l->status = UVC_STATUS_IDLE;

        pthread_create(&thread_getbuf, &attr, (void *)get_uvc_video_form_uvc_sink, (void *)server_l->dev);
    }

    //uvc contorl loop
    FD_ZERO(&fds);
    FD_SET(server_l->dev->fd, &fds);

    while (server_l->run_flag)
    {
        fd_set efds = fds;
        fd_set wfds = fds;

        /* Timeout. */
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        if(NULL != strMjpegImg)//if output still image, listen wfds and efds
            s32Ret = select(server_l->dev->fd + 1, NULL, &wfds, &efds, &tv);
        else
            s32Ret = select(server_l->dev->fd + 1, NULL, NULL, &efds, &tv);

        if(s32Ret > 0)
        {
            if (FD_ISSET(server_l->dev->fd, &efds))
                uvc_events_process(server_l->dev);
            if (NULL != strMjpegImg && 1 == server_l->g_stream_on && FD_ISSET(server_l->dev->fd, &wfds))
            {
                //usleep(30000);
                uvc_video_process(server_l->dev);
            }
        }
    }
    
init_failed:

    //wait thread stop
    if(NULL == strMjpegImg)
    {
        pthread_join(thread_getbuf, NULL);
        log_err("%s thread_getbuf join!\n", __func__);
    }

    //clean up
    if(NULL == strMjpegImg && NULL != server_l->cache_image_data_org)
    {
        free(server_l->cache_image_data_org);
        server_l->cache_image_data_org = NULL;
        server_l->cache_image_data = NULL;
    }
    
    if(NULL == strMjpegImg && NULL != server_l->first_image_data)
    {
        free(server_l->first_image_data);
    }

    if(NULL != strMjpegImg && NULL != server_l->dev->imgdata)
    {
        free(server_l->dev->imgdata);
        server_l->dev->imgdata = NULL;
    }

    uvc_close(server_l->dev);
    log_err("%s uvc_close!\n", __func__);

auto_find_fail:

    //ar_video_stream_close(g_uvc_formats[server_l->dev->fcc_index].data->vs_handle);
    log_err("%s ar_video_stream_close!\n", __func__);

    freeConfigfsFormat(&g_uvc_formats, g_uvc_formats_num);
    log_err("%s freeConfigfsFormat!\n", __func__);

    free(strDevice);
    log_err("%s exit!\n", __func__);
    return 0;
}


AR_S32 SAMPLE_COMM_SYS_Init(VB_CONFIG_S * st_vb_config)
{
    AR_S32 s32_ret = AR_FAILURE;

    AR_MPI_SYS_Exit();
    AR_MPI_VB_Exit();

    if (NULL == st_vb_config)
    {
        printf("input parameter is null, it is invaild!\n");
        return AR_FAILURE;
    }

    s32_ret = AR_MPI_VB_SetConfig(st_vb_config);
    if (AR_SUCCESS != s32_ret)
    {
        printf("AR_MPI_VB_SetConf failed!\n");
        //return AR_FAILURE;
    }

    s32_ret = AR_MPI_VB_Init();
    if (AR_SUCCESS != s32_ret)
    {
        printf("AR_MPI_VB_Init failed!\n");
        //return AR_FAILURE;
    }

    s32_ret = AR_MPI_SYS_Init();
    if (AR_SUCCESS != s32_ret)
    {
        printf("AR_MPI_SYS_Init failed, ret = %d!\n", s32_ret);
        AR_MPI_VB_Exit();
        return AR_FAILURE;
    }

    if(ar_hal_sys_mpp_service_is_lib())
    {
        s32_ret = AR_MPI_VIN_load_vin_driver();
        if (AR_SUCCESS != s32_ret)
        {
            printf("AR_MPI_VIN_load_vin_driver failed! s32Ret:0x%x !\n", s32_ret);
            return s32_ret;
        }
    }

    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_SYS_UnInit()
{
    AR_S32 s32_ret = AR_FAILURE;
    int i = 0;

    //AR_MPI_VIN_unload_vin_driver();
    
    s32_ret = AR_MPI_SYS_Exit();
    if (AR_SUCCESS != s32_ret)
    {
        printf("AR_MPI_SYS_Init failed, ret = 0x%x!\n", s32_ret);
        //return AR_FAILURE;
    }

    for(i = 0; i < AR_VB_MAX_POOLS; ++i)
    {
        AR_MPI_VB_MunmapPool(i);
    }
    
    s32_ret = AR_MPI_VB_Exit();
    if (AR_SUCCESS != s32_ret)
    {
        printf("AR_MPI_VB_Exit failed, ret = 0x%x!\n", s32_ret);
        AR_MPI_VB_Exit();
        //return AR_FAILURE;
    }

    return AR_SUCCESS;
}

static AR_S32 UVC_SYS_Init()
{
    AR_S32              s32Ret;
    //AR_U64              u64BlkSize;
    VB_CONFIG_S         stVbConf;
    // uvc_server_t       *server_l    = get_uvc_server();
    AR_S32              i = 0;

    memset(&stVbConf, 0, sizeof(VB_CONFIG_S));

/*
    for(i = 0; i < server_l->g_pool_count; ++i)
    {
        stVbConf.astCommPool[i].u64BlkSize = server_l->g_pool_size[i];
        stVbConf.astCommPool[i].u32BlkCnt = BUFFER_COUNT + 4;
    }

    stVbConf.u32MaxPoolCnt = server_l->g_pool_count;
*/

    // 1920*1080 raw
    stVbConf.astCommPool[i].u64BlkSize = ALIGN_UP(1920, 256) * 1080 * 2 + 8192 ;
    stVbConf.astCommPool[i].u32BlkCnt = 5;
    i++;

    // 720*1280 raw
    stVbConf.astCommPool[i].u64BlkSize = ALIGN_UP(720, 256) * 1280 * 2 + 8192 ;
    stVbConf.astCommPool[i].u32BlkCnt = 5;
    i++;

    // 1920*1080 yuv
    stVbConf.astCommPool[i].u64BlkSize = ALIGN_UP(1920, 512) * ALIGN_UP(1080, 64) * 1.5 + 8192 * 3;
    stVbConf.astCommPool[i].u32BlkCnt = 10;
    i++;

    // 720*1280 yuv
    stVbConf.astCommPool[i].u64BlkSize = ALIGN_UP(720, 512) * ALIGN_UP(1280, 64) * 1.5 + 8192 * 3;
    stVbConf.astCommPool[i].u32BlkCnt = 10;
    i++;

    stVbConf.u32MaxPoolCnt = i;


    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);

    if (AR_SUCCESS != s32Ret)
    {
        printf("SAMPLE_COMM_SYS_GetPicSize failed!\n");
        return s32Ret;
    }

    return AR_SUCCESS;
}

static AR_S32 UVC_SYS_UnInit()
{
    return SAMPLE_COMM_SYS_UnInit();
}


int main(int argc, char *argv[])
{
    AR_S32                  s32Index       = 0;
    uvc_server_t           *server_l    = get_uvc_server();
    struct sigaction        action;

    action.sa_flags                     = 0;
    action.sa_handler                   = sighandler;

    sigaction(SIGINT, &action, NULL);
    sigaction(SIGKILL, &action, NULL);
    sigaction(SIGTERM, &action, NULL);

    s32Index = uvc_find_pra_index_by_name("--help", argc, argv);
    if(s32Index > 0)
    {
        log_printf("--help:show this help");
        log_printf("--name [sensor name]: set sensor,eg --name imx307 \n");
        log_printf("--power_gpio [gpio]: set sensor power gpio,eg --power_gpio C1_0 \n");
        log_printf("--reset_gpio [gpio]: set sensor reset gpio,eg --reset_gpio C1_1 \n");
        log_printf("--i2c [i2c_bus]: set sensor i2c,eg --i2c 1 \n");
        log_printf("--fps [fps]: set start fps of src,eg --fps 25 \n");
        log_printf("--hdr : start with hdr mode \n");
        log_printf("--res0 [w] [h] \n");
        log_printf("--res1 [w] [h] \n");
        log_printf("--display -w [1920] -h [1080] -disp_fps \n");
        log_printf("--cam_index: open the index of Camera\n");
        log_printf("--dev_index: open the index of UVC gadget\n");
        log_printf("--raw: get raw picture from camera(Z16 or yuy2)\n");
        log_printf("--multi: set sensor in multi mode for multi sensor\n");
        log_printf("--hdr_fre: set hdr frequency\n");
        log_printf("--vif_fre: set vif frequency\n");
        log_printf("--isp_fre: set isp frequency\n");
        log_printf("--h26x_c_fre: set h26x core frequency\n");
        log_printf("--h26x_b_fre: set h26x bpu frequency\n");
        log_printf("--mjpeg_fre: set jpeg frequency\n");
        log_printf("--max_buf_size: set max buffer size\n");
        log_printf("--pool_cnt_size: set vb pool count and block size.ex:--pool_cnt_size 2 4194304 16777216\n");
        usage(argv[0]);
        return 0;
    }

    s32Index = uvc_find_pra_index_by_name("--cam_index",argc,argv);
    if(s32Index > 0)
    {
        server_l->cam_index = atoi(argv[s32Index + 1]);
    }

    s32Index = uvc_find_pra_index_by_name("--dev_index",argc,argv);
    if(s32Index > 0)
    {
        server_l->dev_index = atoi(argv[s32Index + 1]);
    }
    else
    {
        server_l->dev_index =server_l->cam_index;
    }

    server_l->g_max_buffer_size = MAX_BUFFER_SIZE;
    s32Index = uvc_find_pra_index_by_name("--max_buf_size",argc,argv);
    if(s32Index > 0)
    {
        server_l->g_max_buffer_size = atoi(argv[s32Index + 1]);
    }

    server_l->g_pool_count = POOL_COUNT;
    s32Index = uvc_find_pra_index_by_name("--pool_cnt_size",argc,argv);
    if(s32Index > 0)
    {
        int i = 0;
        server_l->g_pool_count = atoi(argv[s32Index + 1]);
        server_l->g_pool_size = (unsigned int *)malloc(server_l->g_pool_count * sizeof(unsigned int));
        printf("pool cnt: %d\n", server_l->g_pool_count);
        for(i = 0; i < server_l->g_pool_count; ++i)
        {
            server_l->g_pool_size[i] = atoi(argv[s32Index + 2 + i]);
            printf("pool size: %d\n", server_l->g_pool_size[i]);
        }
    }
    else
    {
        int i = 0;
        server_l->g_pool_size = (unsigned int *)malloc(server_l->g_pool_count * sizeof(unsigned int));
        for(i = 0; i < server_l->g_pool_count; ++i)
        {
            server_l->g_pool_size[i] = server_l->g_max_buffer_size;
        }
    }

    s32Index = uvc_find_pra_index_by_name("-i",argc,argv);
    if(s32Index > 0)
    {
        start_uvc_trans(argc,argv);
    }
    else
    {
        server_l->argc = argc;
        server_l->argv = argv;

        s32Index = uvc_find_pra_index_by_name("--res0",argc,argv);
        server_l->width[0]  = 720;
        server_l->height[0] = 1280;
        if(s32Index > 0)
        {
            server_l->width[0]= atoi(argv[s32Index + 1]);
            server_l->height[0]= atoi(argv[s32Index + 2]);
        }

        s32Index = uvc_find_pra_index_by_name("--res1",argc,argv);
        server_l->width[1]  = 720;
        server_l->height[1] = 1280;
        if(s32Index > 0)
        {
            server_l->width[1]= atoi(argv[s32Index + 1]);
            server_l->height[1]= atoi(argv[s32Index + 2]);
        }

        s32Index = uvc_find_pra_index_by_name("--display",argc,argv);
        server_l->disp_w=1920;
        server_l->disp_h=1080;
        server_l->disp_fps=60;
        server_l->disp_squ=1;
        if(s32Index > 0)
        {
            server_l->use_display = 1;
            s32Index = uvc_find_pra_index_by_name("-w",argc,argv);
            if(s32Index > 0)
            {
                server_l->disp_w= atoi(argv[s32Index + 1]);
            }
            
            s32Index = uvc_find_pra_index_by_name("-h",argc,argv);
            if(s32Index > 0)
            {
                server_l->disp_h= atoi(argv[s32Index + 1]);
            }

            s32Index = uvc_find_pra_index_by_name("-s",argc,argv);
            if(s32Index > 0)
            {
                server_l->disp_squ= atoi(argv[s32Index + 1]);
            }
            
            s32Index = uvc_find_pra_index_by_name("-disp_fps",argc,argv);
            if(s32Index > 0)
            {
                server_l->disp_fps= atoi(argv[s32Index + 1]);
            }
        }

        s32Index = uvc_find_pra_index_by_name("--name",argc,argv);
        if(s32Index > 0)
        {
            strncpy(server_l->sensor_name, argv[s32Index + 1], 128);
        }
        else
        {
            printf("failed to find sensor name!\n");
            return -1;
        }

        s32Index = uvc_find_pra_index_by_name("--power_gpio",argc,argv);
        if(s32Index > 0)
        {
            strncpy(server_l->power_gpio, argv[s32Index + 1], 8);
        }
        else
        {
            printf("failed to find power gpio!\n");
            return -1;
        }

        s32Index = uvc_find_pra_index_by_name("--reset_gpio",argc,argv);
        if(s32Index > 0)
        {
            strncpy(server_l->reset_gpio, argv[s32Index + 1], 8);
        }
        else
        {
            printf("failed to find power gpio!\n");
            return -1;
        }

        s32Index = uvc_find_pra_index_by_name("--i2c",argc,argv);
        if(s32Index > 0)
        {
            server_l->i2c_bus = atoi(argv[s32Index + 1]);
        }
        else
        {
            printf("failed to find i2c bus!\n");
            return -1;
        }

        server_l->hdr_fre = 75000000;
        s32Index = uvc_find_pra_index_by_name("--hdr_fre",argc,argv);
        if(s32Index > 0)
        {
            server_l->hdr_fre = atoi(argv[s32Index + 1]);
        }

        server_l->vif_fre = 250000000;
        s32Index = uvc_find_pra_index_by_name("--vif_fre",argc,argv);
        if(s32Index > 0)
        {
            server_l->vif_fre = atoi(argv[s32Index + 1]);
        }

        server_l->isp_fre = 250000000;
        s32Index = uvc_find_pra_index_by_name("--isp_fre",argc,argv);
        if(s32Index > 0)
        {
            server_l->isp_fre = atoi(argv[s32Index + 1]);
        }
#ifdef PROXIMA
        server_l->mipi_fre = 100000000;
#else
        server_l->mipi_fre = 200000000;
#endif
        s32Index = uvc_find_pra_index_by_name("--mipi_fre",argc,argv);
        if(s32Index > 0)
        {
            server_l->mipi_fre = atoi(argv[s32Index + 1]);
        }

        server_l->h26x_core_fre = 600000000;
        s32Index = uvc_find_pra_index_by_name("--h26x_c_fre",argc,argv);
        if(s32Index > 0)
        {
            server_l->h26x_core_fre = atoi(argv[s32Index + 1]);
        }

        server_l->h26x_bpu_fre = 500000000;
        s32Index = uvc_find_pra_index_by_name("--h26x_b_fre",argc,argv);
        if(s32Index > 0)
        {
            server_l->h26x_bpu_fre = atoi(argv[s32Index + 1]);
        }

        server_l->jpeg_core_fre = 150000000;
        s32Index = uvc_find_pra_index_by_name("--mjpeg_fre",argc,argv);
        if(s32Index > 0)
        {
            server_l->jpeg_core_fre = atoi(argv[s32Index + 1]);
        }

        server_l->fps = 30;
        s32Index = uvc_find_pra_index_by_name("--fps",argc,argv);
        if(s32Index > 0)
        {
            server_l->fps = atoi(argv[s32Index + 1]);
        }

        server_l->multi = 0;
        s32Index = uvc_find_pra_index_by_name("--multi",argc,argv);
        if(s32Index > 0)
        {
            server_l->multi = 1;
        }

        server_l->vfe_mode = VFE_MODE_NULL;
        s32Index = uvc_find_pra_index_by_name("--hdr",argc,argv);
        if(s32Index > 0)
        {
            server_l->is_hdr    = 1;
            server_l->multi     = 1;
            server_l->vfe_mode  = VFE_MODE_CEVA_HDR;
            server_l->wdr_mode  = WDR_MODE_2To1_LINE;
        }

        s32Index = uvc_find_pra_index_by_name("--raw",argc,argv);
        if(s32Index > 0)
        {
            server_l->is_raw = 1;
        }

        server_l->vi_comp = 0;
        s32Index = uvc_find_pra_index_by_name("--vi_comp",argc,argv);
        if(s32Index > 0)
        {
            server_l->vi_comp = 1;
        }

        if(AR_SUCCESS != UVC_SYS_Init())
        {
            printf("UVC_SYS_Init failed!\n");
            //return -1;
        }
        
        start_uvc_trans(argc,argv);

        UVC_SYS_UnInit();

        log_err("%s exit!\n", __func__);

    }
    return 0;
}


