#include <linux/module.h>
#include <linux/errno.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/fb.h>
#include <linux/videodev2.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/of.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <asm/uaccess.h>
#include <linux/ioport.h>
#include <linux/dma-mapping.h>
#include <linux/moduleparam.h>
#include <linux/kthread.h>

#include "ar_framebuffer_def.h"
#include <linux/ar_framebuffer_def.h>
// #include <asm/fbio.h>
// #include <sbusilb.h>

#include "viv_dc_type.h"
#include "rpc_common.h"
#include "v4ar_dev.h"

#if defined(CONFIG_ION)
#include <linux/artosyn_ion.h>
#else
#include <linux/mman.h>
#include "osal_mmz.h"
#endif

///////////////////////////////////////////////////////////////////////////////////////////

// #define AR_FRAMEBUFFER_ENABLE_CACHE

#if defined(CONFIG_ION)
#define AR_ION_SYSTEM_HEAP_ID   27
#else
#define MMZ_PATH "/dev/mmz_userdev"
#endif

#define AR_DISPLAY_BUFFER_COUNT 3
#define AR_FRAMEBUFFER_REMOTE_DEVICE_NAME "ar_overlay"
#define AR_FRAMEBUFFER_NAME_SYS_INFO "sys_info"

#define AR_FRAMEBUFFER_PAGE_STRIDE           (1<<PAGE_SHIFT)

// #define AR_FRAMEBUFFER_STRIDE           (512)
// #define AR_FRAMEBUFFER_WIDTH_STRIDE     (128) // AR_FRAMEBUFFER_STRIDE>>2

// #define AR_FRAMEBUFFER_YUV_FORMAT_ENABLE

// #define VIDEOMEMSIZE    (2*1024*1024)   /* 2 MB */

static int frame_buffer_width = 1920;
static int frame_buffer_height = 1080;
static int frame_buffer_count = 3;
//0 default argb888, 1 rgb 888 2 rgb565 3 rgb1555
static int frame_buffer_format = 0;
static int frame_stream_enable = 1;

static struct task_struct *remote_check_thread = NULL;
static int remote_is_ready = 0;

/*display cursor ops*/
enum {
    AR_DISPLAY_CURSOR_CFG = 0X1024,
    AR_DISPLAY_CURSOR_DIS,
    AR_DISPLAY_CURSOR_EN,
    AR_DISPLAY_CURSOR_GET,
    AR_DISPLAY_CURSOR_SET,
    AR_DISPLAY_OVERLAY_SET,
};

extern int cuse_dev_open(char *devname, bool compat);
extern int cuse_dev_ioctl(int fd, unsigned int cmd, unsigned long args);
extern int cuse_dev_close(int fd);

typedef enum {
    SYS_INFO_EVENT_EXT_NONE = 0,
    SYS_INFO_EVENT_EXT_GET_SYS_INFO,
    SYS_INFO_EVENT_EXT_MAX,
} sys_info_get_event_ext_t;

typedef struct {
    unsigned int ddr_rw_burst_len;
} RTOS_SYS_DDR_INFO_CONTEXT_T;

typedef struct {
    RTOS_SYS_DDR_INFO_CONTEXT_T ddr_info;
} RTOS_SYS_INFO_CONTEXT_t;

// stride = 2 * ddr_rw_burst_len * DDR_SINGLE_BURST_LEN
// #define DDR_SINGLE_BURST_LEN    (16)    //16byte - 128bit
#define AR_FRAMEBUFFER_DDR_BURST_LEN_SHIFT (5)
#define AR_FRAMEBUFFER_DEFAULT_DDR_RW_BURST_LEN (16)

typedef struct {
    int fd;
    int current_width;
    int current_height;
    int current_width_aligned;
    int current_format;
} ar_fb_device_context;

typedef struct {
    long mem_start;
    int ddr_rw_burst_len;
    int buffer_count;
    void *p_addr;
    int offset[AR_DISPLAY_BUFFER_COUNT];
} ar_fb_data_context;

typedef struct {
    ar_fb_device_context dev;
    ar_fb_data_context data;
#if defined(CONFIG_ION)
    int ion_fd;
#else
    hil_mmb_t *mmb;
#endif
} ar_fb_context_t;

static char *mode_option = NULL;

static struct fb_fix_screeninfo ar_fb_fix = {
    .id =       "arfb",
    .smem_start = 0,  /* (physical address) */
#ifdef AR_FRAMEBUFFER_YUV_FORMAT_ENABLE
    .type =     FB_TYPE_FOURCC,
    .visual =   FB_VISUAL_FOURCC,
    .capabilities = FB_CAP_FOURCC,
#else
    .type =     FB_TYPE_PACKED_PIXELS,
    .visual =   FB_VISUAL_TRUECOLOR,
    .capabilities = 0,
#endif
    .type_aux = 0,

    .xpanstep = 0, //1,
    .ypanstep = 1,
    .ywrapstep = 0, // 1,

    .line_length = 0,
    .mmio_start = 0,  /* (physical address) */
    .mmio_len = 0,

    .accel = FB_ACCEL_NONE,
    .reserved = {0},
};

static struct fb_var_screeninfo ar_fb_var = {
    .xres         = 1920,
    .yres         = 1080,
    .xres_virtual = 1920, //align
    .yres_virtual = 1080 * 3,
    .xoffset = 0,
    .yoffset = 0,

#ifdef AR_FRAMEBUFFER_YUV_FORMAT_ENABLE
    .grayscale = V4L2_PIX_FMT_YUV420,//apllication set YU12 - three planes - Y Cb, Cr
    .bits_per_pixel = 12,
    .blue = {
        .offset = 0,
        .length = 0,
        .msb_right = 0,
    },
    .green = {
        .offset = 0,
        .length = 0,
        .msb_right = 0,
    },
    .red = {
        .offset = 0,
        .length = 0,
        .msb_right = 0,
    },
    .transp = {
        .offset = 0,
        .length = 0,
        .msb_right = 0,
    },
#else  // AR_FRAMEBUFFER_YUV_FORMAT_ENABLE
    .grayscale=0, //0 color 1 grayscale
    .bits_per_pixel = 32,
    .blue = {
        .offset = 0,
        .length = 8,
        .msb_right = 0,
    },
    .green = {
        .offset = 8,
        .length = 8,
        .msb_right = 0,
    },
    .red = {
        .offset = 16,
        .length = 8,
        .msb_right = 0,
    },
    .transp = {
        .offset = 24,
        .length = 8,
        .msb_right = 0,
    },
#endif // AR_FRAMEBUFFER_YUV_FORMAT_ENABLE
    .nonstd = 0,
    .colorspace = V4L2_COLORSPACE_DEFAULT,
};

// static int ar_fb_check_var(struct fb_var_screeninfo *var, struct fb_info *info);
// static int ar_fb_set_par(struct fb_info *info);
// static int ar_fb_cursor(struct fb_info *info, struct fb_cursor *cursor);
static int ar_fb_ioctl(struct fb_info *info, unsigned int cmd, unsigned long arg);
static int ar_fb_pan_display(struct fb_var_screeninfo *var, struct fb_info *info);
static int ar_fb_mmap(struct fb_info *info, struct vm_area_struct *vma);

static struct fb_ops ar_fb_ops = {
    .owner              = THIS_MODULE,
  //.fb_check_var       = ar_fb_check_var,
  //.fb_set_par         = ar_fb_set_par,
    .fb_cursor          = NULL, //ar_fb_cursor,
    .fb_ioctl           = ar_fb_ioctl,
    .fb_compat_ioctl    = ar_fb_ioctl,
    .fb_pan_display     = ar_fb_pan_display,
    .fb_fillrect        = cfb_fillrect,
    .fb_copyarea        = cfb_copyarea,
    .fb_imageblit       = cfb_imageblit,
    .fb_mmap            = ar_fb_mmap,
};

///////////////////////////////////////////////////////////////////////////////////////////
/// Internal func
///////////////////////////////////////////////////////////////////////////////////////////

static u_long get_line_length(struct fb_info *info, int xres_virtual, int bpp)
{
    u_long length;

    ar_fb_context_t *ar_fb_context;
    unsigned int stride;

    if (NULL == info) {
        ar_err("empty input");
        return -1;
    }

    ar_fb_context = (ar_fb_context_t *)info->par;
    stride = (ar_fb_context->data.ddr_rw_burst_len << AR_FRAMEBUFFER_DDR_BURST_LEN_SHIFT) - 1;

    if (FB_TYPE_FOURCC == ar_fb_fix.type) {
        if (V4L2_PIX_FMT_YUV420 == ar_fb_var.grayscale) {
            length = xres_virtual;
        } else {
            ar_err("unsupported format 0x%x", ar_fb_var.grayscale);
            return 0;
        }
    } else {
        length = xres_virtual * bpp;
        length = (length + 31) & ~31;
        length >>= 3;
    }

    length = ( length + (stride) ) & (~stride);

    return (length);
}

static u_long get_buffer_size(void)
{
    u_long ret_size = 0;
    if (FB_TYPE_FOURCC == ar_fb_fix.type) {
        if (V4L2_PIX_FMT_YUV420 == ar_fb_var.grayscale)
        {
            ret_size = ar_fb_fix.line_length * ar_fb_var.yres_virtual;
            ret_size += (ret_size>>1);
            ar_always("%d-%d-%ld", ar_fb_fix.line_length, ar_fb_var.yres_virtual, ret_size);
        } else {
            ar_err("unsupported format 0x%x", ar_fb_var.grayscale);
            return 0;
        }
    } else {
        ret_size = ar_fb_fix.line_length * ar_fb_var.yres_virtual;
    }

    return ret_size;
}

static int ar_fb_parse_init(char *param)
{
    if (NULL == param) {
        //use default value
        ar_err("empty param");
        // return 0;
    }

    //TODO: parse mode_option and init structure

    // ar_fb_var.xres_virtual = ( ar_fb_var.xres + (AR_FRAMEBUFFER_WIDTH_STRIDE-1) )& (~(AR_FRAMEBUFFER_WIDTH_STRIDE-1));
    // ar_fb_fix.line_length = get_line_length(ar_fb_var.xres_virtual, ar_fb_var.bits_per_pixel);

    return 0;
}

static int ar_fb_priv_context_init(struct fb_info *info, const u_long mem_Start)
{
    ar_fb_context_t *ar_fb_context;

    if (NULL == info) {
        ar_err("empty input");
        return -1;
    }

    ar_fb_context = (ar_fb_context_t *)info->par;
    ar_fb_context->data.mem_start = mem_Start;

#if defined(_32BIT_SDK)
    bool compat = true;
#else
    bool compat = false;
#endif
    ar_fb_context->dev.fd = cuse_dev_open(AR_FRAMEBUFFER_REMOTE_DEVICE_NAME, compat);
    if (ar_fb_context->dev.fd < 0) {
        ar_err("open %s failed, fd = %d\n", AR_FRAMEBUFFER_REMOTE_DEVICE_NAME, ar_fb_context->dev.fd);
        return -1;
    }

    ar_always("open %s success, fd = %d\n", AR_FRAMEBUFFER_REMOTE_DEVICE_NAME, ar_fb_context->dev.fd);
    ar_fb_context->dev.current_width = info->var.xres;
    ar_fb_context->dev.current_height = info->var.yres;
    ar_fb_context->dev.current_width_aligned = info->var.xres_virtual;

    if (FB_TYPE_FOURCC == info->fix.type) {
        ar_fb_context->dev.current_format = info->var.grayscale;
    } else {
       if (frame_buffer_format == 0) {
          ar_fb_context->dev.current_format = V4L2_PIX_FMT_ABGR32;
       } else if(frame_buffer_format == 1) {
          //rgb888
          ar_fb_context->dev.current_format = V4L2_PIX_FMT_BGR24;
       } else if(frame_buffer_format == 2) {
           //rgb 565
           ar_fb_context->dev.current_format = V4L2_PIX_FMT_RGB565;
       } else if(frame_buffer_format == 3) {
           //rgb 1555
           ar_fb_context->dev.current_format = V4L2_PIX_FMT_ARGB555;
       }
    }

    // ar_fb_context->data.ddr_rw_burst_len = ddr_burst_len;

    return 0;
}

static int ar_fb_remote_init(struct fb_info *info)
{
    int ret;
    ar_fb_context_t *ar_fb_context;
    v4ar_dev_format_t format_info;
    v4ar_dev_size_t resolution_info;
    v4ar_common_ctl_ext_t ctl;
    int stream_on = 1;

    v4ar_buffer_t buffer_info;

    ar_always("frame_stream_enable: %d", frame_stream_enable);
    if(!frame_stream_enable){
        return 0;
    }

    if (NULL == info) {
        ar_err("empty input");
        return -1;
    }

    ar_fb_context = (ar_fb_context_t *)info->par;

    //Set Resolution
    resolution_info.width = info->var.xres;
    resolution_info.height = info->var.yres;
    resolution_info.y_stride = info->fix.line_length;
    resolution_info.chroma_stride = (info->fix.line_length>>1);

    ar_always("set resolution w %d : h %d : y %d : chroma : %d",
               resolution_info.width, resolution_info.height,
               resolution_info.y_stride, resolution_info.chroma_stride);

    ctl.len  = sizeof(v4ar_dev_size_t);
    ctl.data = (void*)&resolution_info;
    ctl.ctl_code = VIDIOC_V4AR_DEV_SET_SIZE;
    ret = cuse_dev_ioctl(ar_fb_context->dev.fd, VIDIOC_V4AR_DEV_COMMON_CTL_EXT, &ctl);
    if (ret < 0) {
        ar_err(" set resolution w %d : h %d : y %d : "
               "chroma : %d - ret : %d", resolution_info.width,
               resolution_info.height, resolution_info.y_stride,
               resolution_info.chroma_stride, ret);
        return ret;
    }

    //Set Format
    if (V4L2_PIX_FMT_ABGR32 == ar_fb_context->dev.current_format) {
        format_info.format = Input_ARGB8888;
    } else if (V4L2_PIX_FMT_YUV420 == ar_fb_context->dev.current_format) {
        format_info.format = Input_YV12;
        // ar_always("set 0x80");
        // memset(info->fix.smem_start, 0x80, info->fix.smem_len);
    } else if (V4L2_PIX_FMT_BGR24==ar_fb_context->dev.current_format) {
       ar_err("unsupported format 0x%x", ar_fb_context->dev.current_format);
	   return -EINVAL;
    } else if (V4L2_PIX_FMT_RGB565==ar_fb_context->dev.current_format) {
       format_info.format = Input_RGB565;
    } else if (V4L2_PIX_FMT_ARGB555==ar_fb_context->dev.current_format) {
       format_info.format = Input_ARGB1555;
    } else {
        ar_err("unsupported format 0x%x", ar_fb_context->dev.current_format);
        return -EINVAL;
    }

    ar_always("set format 0x%x", format_info.format);
    ctl.len  = sizeof(v4ar_dev_format_t);
    ctl.data = (void*)&format_info;
    ctl.ctl_code = VIDIOC_V4AR_DEV_SET_FOAMAT;
    ret = cuse_dev_ioctl(ar_fb_context->dev.fd, VIDIOC_V4AR_DEV_COMMON_CTL_EXT, &ctl);
    if (ret < 0) {
        ar_err("set format %d - ret : %d", format_info.format, ret);
        return ret;
    }

    //Set StreamOn
    ar_always("stream on");
    ctl.len  = sizeof(stream_on);
    ctl.data = (void*)&stream_on;
    ctl.ctl_code = VIDIOC_V4AR_DEV_STREAM_ON;
    ret = cuse_dev_ioctl(ar_fb_context->dev.fd, VIDIOC_V4AR_DEV_COMMON_CTL_EXT, &ctl);
    if (ret < 0) {
        ar_err("set stream_on %d - ret : %d", stream_on, ret);
        return ret;
    }

    // debug test
    // memset(info->fix.smem_start,0x80, ar_fb_fix.line_length*(info->var.yres>>2) );

    //QBuf
    buffer_info.frame_id = 0;
    buffer_info.pts = 0;
    if (V4L2_PIX_FMT_YUV420 == ar_fb_context->dev.current_format) {
        buffer_info.pannle_num = 3;
        buffer_info.mem.pannel[0].pa = (void *)info->fix.smem_start; //need align
        buffer_info.mem.pannel[1].pa = buffer_info.mem.pannel[0].pa + info->fix.line_length*info->var.yres;
        buffer_info.mem.pannel[2].pa = buffer_info.mem.pannel[1].pa + (info->fix.line_length>>1)*(info->var.yres>>1);
    } else { //ARGB8888
        buffer_info.pannle_num = 1;
        buffer_info.mem.pannel[0].pa = (void *)info->fix.smem_start; //need align
        buffer_info.mem.pannel[1].pa = NULL;
        buffer_info.mem.pannel[2].pa = NULL;
    }

    ctl.len  = sizeof(v4ar_buffer_t);
    ctl.data = (void*)&buffer_info;
    ctl.ctl_code = VIDIOC_V4AR_DEV_QBUF;
    ret = cuse_dev_ioctl(ar_fb_context->dev.fd, VIDIOC_V4AR_DEV_COMMON_CTL_EXT, &ctl);
    if (ret < 0) {
        ar_err("QBuf ret : %d", ret);
        return -EIO;
        // return -EAGAIN; //??
    }

    return 0;
}

static int ar_fb_remote_release(int fd)
{
    int ret;
    int stream_off = 1;
    v4ar_common_ctl_ext_t ctl;

    if (fd < 0) {
        ar_err("invalid fd %d", fd);
        return -1;
    }

    ctl.len  = sizeof(stream_off);
    ctl.data = (void*)&stream_off;
    ctl.ctl_code = VIDIOC_V4AR_DEV_STREAM_OFF;
    ret = cuse_dev_ioctl(fd, VIDIOC_V4AR_DEV_COMMON_CTL_EXT, (unsigned long)&ctl);
    if (ret < 0) {
        ar_err("streamOff - ret : %d", ret);
        return ret;
    }

    return 0;
}

// static int ar_fb_remote_changeReolution(struct fb_info *info)
// {
//     if (NULL == info) {
//         ar_err("empty input");
//         return -1;
//     }

//     ar_fb_context_t *ar_fb_context = (ar_fb_context_t *)info->par;
//     int ret;
//     v4ar_pannel_t set_param={0};
//     v4ar_common_ctl_ext_t ctl;

//     //Set StreamOff
//     set_param.len = 0;
//     set_param.pa = NULL;

//     ctl.len  = sizeof(v4ar_pannel_t);
//     ctl.data = (void*)&set_param;
//     ctl.ctl_code = VIDIOC_V4AR_DEV_STREAM_OFF;
//     ret = cuse_dev_ioctl(fd_mem, VIDIOC_V4AR_DEV_COMMON_CTL_EXT, (unsigned long)&set_param);
//     if (ret) < 0 {
//         ar_err("set streamOff %d - ret : %d", format_info.format, ret);
//         return ret;
//     }

//     //Set Resolution
//     v4ar_dev_size_t resolution_info;
//     resolution_info.width = info->var.xres;
//     resolution_info.height = info->var.yres;

//     set_param.len = sizeof(v4ar_dev_size_t);
//     set_param.pa = (void *)&resolution_info;

//     ctl.len  = sizeof(v4ar_pannel_t);
//     ctl.data = (void*)&set_param;
//     ctl.ctl_code = VIDIOC_V4AR_DEV_SET_SIZE;
//     ret = cuse_dev_ioctl(fd_mem, VIDIOC_V4AR_DEV_COMMON_CTL_EXT, (unsigned long)&set_param);
//     if (ret < 0) {
//         ar_err(" set resolution w %d : h %d - ret : %d", resolution_info.width, resolution_info.height, ret);
//         return ret;
//     }

//     //Set StreamOn
//     set_param.len = 0;
//     set_param.pa = NULL;

//     ctl.len  = sizeof(v4ar_pannel_t);
//     ctl.data = (void*)&set_param;
//     ctl.ctl_code = VIDIOC_V4AR_DEV_STREAM_ON;
//     ret = cuse_dev_ioctl(fd_mem, VIDIOC_V4AR_DEV_COMMON_CTL_EXT, (unsigned long)&set_param);
//     if (ret < 0) {
//         ar_err("set streamOn %d - ret : %d", format_info.format, ret);
//         return ret;
//     }

//     return 0;
// }

static int ar_fb_ioctl(struct fb_info *info, unsigned int cmd, unsigned long arg)
{
    int ret;
    int flag = 0;
    int tmp = 1;

    ar_fb_context_t *ar_fb_context;
    v4ar_buffer_t buffer_info;
    v4ar_common_ctl_ext_t ctl;
    arfb_flush_zone_t* zone = NULL;

    if (NULL == info) {
        ar_err("empty input of info");
        return -EINVAL;
    }

    ar_fb_context = (ar_fb_context_t *)info->par;
    if (NULL == ar_fb_context) {
        ar_err("empyt fb context");
        return -EINVAL;
    }

    ar_always("[%d] cmd 0x%x",__LINE__, _IOC_NR(cmd));
    switch (cmd)
    {
        case AR_FRAMEBUFFER_IO_CURSOR_SET_ICON:
            if (NULL == (char *)arg) {
                ar_err("set cursor icon, empty input");
                return -EINVAL;
            }
            ar_always("set ICON 0x%lx", arg);
            ar_always("set icon %s", (char *)arg);

            ctl.ctl_code = AR_DISPLAY_CURSOR_CFG;
            ctl.len = strlen((char *)arg) + 1;
            ctl.data = (char *)arg;
            break;
        case AR_FRAMEBUFFER_IO_CURSOR_DISPLAY_ENABLE:
            if (arg) {
                ctl.ctl_code = AR_DISPLAY_CURSOR_EN;
                flag = 1;
            } else {
                ctl.ctl_code = AR_DISPLAY_CURSOR_DIS;
                flag = 0;
            }
            ctl.len = sizeof(int);
            ctl.data = (char *)&flag;
            break;
        case AR_FRAMEBUFFER_IO_CMD_CURSOR_SET_POS:
            if (NULL == (arfb_cursor_pos_t *)arg) {
                ar_err("cursor set pos, empty input");
                return -EINVAL;
            }
            ctl.ctl_code = AR_DISPLAY_CURSOR_SET;
            ctl.len = sizeof(arfb_cursor_pos_t);
            ctl.data = arg;
            break;
        case AR_FRAMEBUFFER_IO_CMD_FLUSH_ZONE:
           zone = (arfb_flush_zone_t *)arg;
           if (NULL == zone) {
               ar_err("flush zone, empty input");
               return -EINVAL;
           }

           if ((zone->line_offset+zone->line_num) >= ar_fb_var.yres_virtual) {
               ar_err("flush zone failed, invalid zone->line_offset "
                      "%d zone->line_num %d, large than yres_virtual %d.",
                       zone->line_offset, zone->line_num, ar_fb_var.yres_virtual);
               return -EINVAL;
           }
           //dma_sync_single_range_for_device(info->dev,
           //                 info->fix.smem_start+zone->line_offset*info->fix.line_length,
           //                 0,
           //                 zone->line_num*info->fix.line_length,
           //                 DMA_TO_DEVICE);
           return 0;
        default:
            ar_err("unsupported cmd 0x%x", _IOC_NR(cmd));
            return -ENOTTY;
            // return sbusfb_ioctl_helper(cmd, arg, info, FBTYPE_NOTYPE, 32, info->fix.smem_len);
            break;
    }

    ret = cuse_dev_ioctl(ar_fb_context->dev.fd, VIDIOC_V4AR_DEV_COMMON_CTL_EXT, &ctl);
    if (ret < 0) {
        ar_err("cmd 0x%x, ret %d", ctl.ctl_code, ret);
        return -EIO;
    }

    return 0;
}

/* checks var and eventually tweaks it to something supported,
 * DO NOT MODIFY PAR */
#if 0
static int ar_fb_check_var(struct fb_var_screeninfo *var, struct fb_info *info)
{
    ar_fb_context_t *ar_fb_context;
    u_long line_length;

    ar_always("[%d]",__LINE__);

    if (NULL == var || NULL == info) {
        ar_err("empyt input 0x%x 0x%x", var, info);
        return -EINVAL;
    }

    if (var->bits_per_pixel != 32) {// only support @RGB
        ar_err("only support ARGB8888");
        return -EINVAL;
    }

    if (var->red.offset != 0 || var->red.length != 8 || var->red.msb_right != 0) {
        ar_err("red only support 0,8,0 - %d,%d,%d",
               var->red.offset, var->red.length, var->red.msb_right);
        return -EINVAL;
    }

    if (var->green.offset != 0 || var->green.length != 8 || var->green.msb_right != 0) {
        ar_err("green only support 0,8,0 - %d,%d,%d",
               var->green.offset, var->green.length, var->green.msb_right);
        return -EINVAL;
    }

    if(var->blue.offset != 0 || var->blue.length != 8 || var->blue.msb_right != 0) {
        ar_err("blue only support 0,8,0 - %d,%d,%d",
                var->blue.offset, var->blue.length, var->blue.msb_right);
        return -EINVAL;
    }

    if (var->transp.offset != 0 || var->transp.length != 8 || var->transp.msb_right != 0) {
        ar_err("transp only support 0,8,0 - %d,%d,%d",
                var->transp.offset, var->transp.length, var->transp.msb_right);
        return -EINVAL;
    }

    ar_fb_context = (ar_fb_context_t *)info->par;

    if (var->xres != ar_fb_context->dev.current_width \
        || var->xres_virtual != ar_fb_context->dev.current_width_aligned \
        || var->xoffset != 0 \
        || var->yres != ar_fb_context->dev.current_height )
    {
        ar_err("only support res w-%d(%d), h-%d : input w-%d(%d), h-%d"\
            , ar_fb_context->dev.current_width, ar_fb_context->dev.current_width_aligned\
            , ar_fb_context->dev.current_height\
            , var->xres, var->xres_virtual, var->yres);
        return -EINVAL;
    }

    if (var->yres_virtual < (var->yoffset + var->yres)) {
        ar_err("yoffset(%d) + res(%d) > yres_virtual(%d)",
               var->yoffset, var->yres, var->yres_virtual);
        return -EINVAL;
    }

    line_length = get_line_length(var->xres_virtual, var->bits_per_pixel);

    if ((line_length * var->yres_virtual) > info->fix.smem_len) {
        return -ENOMEM;
    }

    return 0;
}

static int ar_fb_set_par(struct fb_info *info)
{
    ar_always("[%d]",__LINE__);

    // info->fix.line_length = get_line_length(info->var.xres_virtual, info->var.bits_per_pixel);

    //To inform remote

    return 0;
}
#endif

#if 0
struct fb_image {
    __u32 dx;       /* Where to place image */
    __u32 dy;
    __u32 width;        /* Size of image */
    __u32 height;
    __u32 fg_color;     /* Only used when a mono bitmap */
    __u32 bg_color;
    __u8  depth;        /* Depth of the image */
    const char *data;   /* Pointer to image data */
    struct fb_cmap cmap;    /* color map info */
};

struct fbcurpos {
    __u16 x, y;
};
struct fb_cursor {
    __u16 set;      /* what to set */
    __u16 enable;       /* cursor on/off */
    __u16 rop;      /* bitop operation */
    const char *mask;   /* cursor mask bits */
    struct fbcurpos hot;    /* cursor hot spot */
    struct fb_image image;  /* Cursor image */
};
#endif

#if 0
static int ar_fb_cursor(struct fb_info *info, struct fb_cursor *cursor)
{
    ar_always("[%d]",__LINE__);

    if(NULL == info || NULL == cursor)
    {
        ar_err("empyt intut 0x%x 0x%x", info, cursor);
        return -EINVAL;
    }

    return 0;
}
#endif

/*
 *  Pan or Wrap the Display
 *
 *  This call looks only at xoffset, yoffset and the FB_VMODE_YWRAP flag
 */
static int ar_fb_pan_display(struct fb_var_screeninfo *var, struct fb_info *info)
{
    int ret;
    long mem_start;
    ar_fb_context_t *ar_fb_context;
    v4ar_common_ctl_ext_t ctl;

    v4ar_buffer_t buffer_info;

    if(!frame_stream_enable || !remote_is_ready){
        ar_info("frame_stream_enable: %d remote_is_ready:%d", frame_stream_enable, remote_is_ready);
        return 0;
    }

    if (NULL == var || NULL == info) {
        ar_err("empty input %p %p", var, info);
        return -EINVAL;
    }

    // if (var->bits_per_pixel != 32) // only support @RGB
    // {
    //     ar_err("only support ARGB8888");
    //     return -EINVAL;
    // }

    ar_fb_context = (ar_fb_context_t *)info->par;

    if (var->xres != ar_fb_context->dev.current_width \
        || var->xres_virtual != ar_fb_context->dev.current_width_aligned \
        || var->xoffset != 0 \
        || var->yres != ar_fb_context->dev.current_height \
        || ( FB_TYPE_FOURCC == info->fix.type && var->grayscale != ar_fb_context->dev.current_format) \
    )
    {
        ar_err("only support res w-%d(%d), h-%d : input w-%d(%d), h-%d : format 0x%x"\
            , ar_fb_context->dev.current_width, ar_fb_context->dev.current_width_aligned\
            , ar_fb_context->dev.current_height\
            , var->xres, var->xres_virtual, var->yres\
            , ar_fb_context->dev.current_format);
        return -EINVAL;
    }

    if (V4L2_PIX_FMT_ABGR32 == ar_fb_context->dev.current_format \
        && var->yres_virtual < (var->yoffset + var->yres) \
    ) {
        ar_err("yoffset(%d) + res(%d) > yres_virtual(%d)",
        var->yoffset, var->yres, var->yres_virtual);
        return -EINVAL;
    } else if (V4L2_PIX_FMT_YUV420 == ar_fb_context->dev.current_format
            && (var->yres_virtual + (var->yres_virtual>>1)) < (var->yoffset + var->yres + (var->yres>>1))
    ) {
        ar_err("yoffset(%d) + res(%d) > yres_virtual(%d)",
               var->yoffset, var->yres, var->yres_virtual);
        return -EINVAL;
    }

    if (!info->fix.ypanstep || var->yoffset%info->fix.ypanstep) {
        ar_err("yoffset %d : ypan %d", var->yoffset, info->fix.ypanstep);
        return -EINVAL;
    }


    // mem_start = info->fix.smem_start + var->yoffset*info->fix.line_length+var->xoffset*info->var.bits_per_pixel/8;
    mem_start = info->fix.smem_start + var->yoffset*info->fix.line_length;


    //QBuf
    buffer_info.frame_id = 0;
    buffer_info.pts = 0;

    if (V4L2_PIX_FMT_YUV420 == ar_fb_context->dev.current_format) {
        buffer_info.pannle_num = 3;
        buffer_info.mem.pannel[0].pa = (void *)mem_start;
        buffer_info.mem.pannel[1].pa = buffer_info.mem.pannel[0].pa + info->fix.line_length*info->var.yres;
        buffer_info.mem.pannel[2].pa = buffer_info.mem.pannel[1].pa + (info->fix.line_length>>1)*(info->var.yres>>1);
    } else { //ARGB8888 ot 565 ot 1555
        buffer_info.pannle_num = 1;
        buffer_info.mem.pannel[0].pa = (void *)mem_start;
        buffer_info.mem.pannel[1].pa = NULL;
        buffer_info.mem.pannel[2].pa = NULL;
    }

    // cache flush
#ifdef AR_FRAMEBUFFER_ENABLE_CACHE
    dma_sync_single_range_for_device(info->dev,
        mem_start,
        0,
        var->yres*info->fix.line_length,
        DMA_TO_DEVICE);
#endif

    ctl.len  = sizeof(v4ar_buffer_t);
    ctl.data = (void*)&buffer_info;
    ctl.ctl_code = VIDIOC_V4AR_DEV_QBUF;
    ret = cuse_dev_ioctl(ar_fb_context->dev.fd, VIDIOC_V4AR_DEV_COMMON_CTL_EXT, (unsigned long)&ctl);
    if (ret < 0) {
        ar_err("QBuf ret : %d", ret);
        return -EIO;
        // return -EAGAIN; //??
    }

    //?? DQBuf
    do {
        buffer_info.mem.pannel[0].pa = NULL;
        ctl.len  = sizeof(v4ar_buffer_t);
        ctl.data = (void*)&buffer_info;
        ctl.ctl_code = VIDIOC_V4AR_DEV_DQBUF;
        ret = cuse_dev_ioctl(ar_fb_context->dev.fd, VIDIOC_V4AR_DEV_COMMON_CTL_EXT, (unsigned long)&ctl);
        if (ret < 0) {
            // ar_always("empty, DQBuf ret : %d", ret);
            break;
        }
        //ar_always("DQbuf 0x%x-%d:0x%x:0x%x:0x%x"\
        //    , info->fix.smem_start, buffer_info.pannle_num\
        //    , buffer_info.mem.pannel[0].pa\
        //    , buffer_info.mem.pannel[1].pa\
        //    , buffer_info.mem.pannel[2].pa);
    } while (NULL != buffer_info.mem.pannel[0].pa);

    return 0;
}

static int ar_fb_mmap(struct fb_info *info, struct vm_area_struct *vma)
{
    int ret = -EINVAL;
    unsigned long len, off = vma->vm_pgoff << PAGE_SHIFT;
    len = info->fix.smem_len;

    //ar_always("len 0x%x, vmstart 0x%x, vmend 0x%x, pgoff 0x%x, off 0x%x, pgprot 0x%x, flags 0x%x"\
    //    , info->fix.smem_len, vma->vm_start,  vma->vm_end, vma->vm_pgoff, off\
    //    , vma->vm_page_prot, vma->vm_flags);

    //ar_always("phys to phn 0x%x", __phys_to_pfn(info->fix.smem_start));
    if (off <= len && vma->vm_end - vma->vm_start <= len - off &&
                      info->fix.smem_start) {
#ifdef AR_FRAMEBUFFER_ENABLE_CACHE
        vma->vm_flags |= VM_IO | VM_DONTCOPY | VM_DONTEXPAND | VM_DONTDUMP;
        vma->vm_pgoff = 0;
#endif

#if 0
        ret = remap_pfn_range(vma, vma->vm_start,
                __phys_to_pfn(info->fix.smem_start) + vma->vm_pgoff,
                (vma->vm_end - vma->vm_start),
                vma->vm_page_prot | L_PTE_MT_WRITEBACK | L_PTE_MT_BUFFERABLE);
#else
        ret = remap_pfn_range(vma, vma->vm_start,
                __phys_to_pfn(info->fix.smem_start) + vma->vm_pgoff,
                (vma->vm_end - vma->vm_start),
                pgprot_writecombine(vma->vm_page_prot));
#endif
    } else {
        ar_err("invalid parameter offset 0x%lx, len 0x%lx, "
               "vm_start 0x%lx, vm_end 0x%lx, mem 0x%lx", off, len,
               vma->vm_start, vma->vm_end, info->fix.smem_start);
    }

    return ret;
}

/* Initialization */
static int ar_fb_init(struct platform_device *dev)
{
    int retval = -ENOMEM;
    int fd_sys = -1;
    int fd_mem = -1;
    void* pa = NULL;
    void* va = NULL;
    void* pa_aligned = NULL;
    void* va_aligned = NULL;
    u_long videomemorysize;
    unsigned int line_width_stride;
    int ddr_burst_len = AR_FRAMEBUFFER_DEFAULT_DDR_RW_BURST_LEN;

    struct fb_info *info = NULL;
    ar_fb_context_t *ar_fb_context = NULL;
    v4ar_mem_t mem_req = {0};
    v4ar_common_ctl_ext_t event_ext;
    RTOS_SYS_INFO_CONTEXT_t sys_info;

    // here we malloc mem for display
    // unsigned int mem_size_aligned = PAGE_ALIGN(videomemorysize);
    // if (!(videomemory = vmalloc_32_user(mem_size_aligned)))
    //     return retval;

    // if (NULL != mode_option) {
    //     if (ar_fb_parse_init(mode_option)) {
    //         ar_err("err open dma mem");
    //         goto release_exit;
    //     }

    //     if (!fb_find_mode(&info->var, info, mode_option,
    //               NULL, 0, &ar_framebuffer_default, 8)){
    //         ar_err(info, "Unable to find usable video mode.\n");
    //         retval = -EINVAL;
    //         goto release_exit1;
    //     }
    // } else {
    //     //TODO: init from dts
    // }

    if (frame_buffer_width > 0) {
        ar_fb_var.xres = frame_buffer_width;
        ar_fb_var.xres_virtual = frame_buffer_width;
    }

    if (frame_buffer_height > 0) {
        ar_fb_var.yres = frame_buffer_height;
    }

    if (frame_buffer_count > 0) {
        ar_fb_var.yres_virtual = ar_fb_var.yres * frame_buffer_count;
    } else {
        ar_fb_var.yres_virtual = ar_fb_var.yres * 3;
    }

    if (frame_buffer_format == 0) {
       ar_always("use default format");
    } else if (frame_buffer_format == 1) {
       //argb888
       ar_fb_var.bits_per_pixel = 24;
	   ar_fb_var.blue.offset = 0;
	   ar_fb_var.blue.length = 8;
	   ar_fb_var.green.offset = 8;
	   ar_fb_var.green.length = 8;
	   ar_fb_var.red.offset = 16;
	   ar_fb_var.red.length = 8;
	   ar_fb_var.transp.offset = 24;
	   ar_fb_var.transp.length = 0;
    } else if (frame_buffer_format == 2) {
       //rgb565
       ar_fb_var.bits_per_pixel = 16;
	   ar_fb_var.blue.offset = 0;
	   ar_fb_var.blue.length = 5;
	   ar_fb_var.green.offset = 5;
	   ar_fb_var.green.length = 6;
	   ar_fb_var.red.offset = 11;
	   ar_fb_var.red.length = 5;
	   ar_fb_var.transp.offset = 16;
	   ar_fb_var.transp.length = 0;
    } else if (frame_buffer_format == 3) {
       //rgb1555
       ar_fb_var.bits_per_pixel = 16;
	   ar_fb_var.blue.offset = 0;
	   ar_fb_var.blue.length = 5;
	   ar_fb_var.green.offset = 5;
	   ar_fb_var.green.length = 5;
	   ar_fb_var.red.offset = 10;
	   ar_fb_var.red.length = 5;
	   ar_fb_var.transp.offset = 15;
	   ar_fb_var.transp.length = 1;
    }

#if defined(_32BIT_SDK)
    bool compat = true;
#else
    bool compat = false;
#endif
    fd_sys = cuse_dev_open(AR_FRAMEBUFFER_NAME_SYS_INFO, compat);
    if (fd_sys <= 0) {
        ar_err("open %s, use default ddr rw burst len %d\n",
               AR_FRAMEBUFFER_NAME_SYS_INFO, ddr_burst_len);
    } else {
        event_ext.ctl_code = SYS_INFO_EVENT_EXT_GET_SYS_INFO;
        event_ext.data = (char *)&sys_info;
        event_ext.len = sizeof(v4ar_common_ctl_ext_t);

        retval = cuse_dev_ioctl(fd_sys, VIDIOC_V4AR_DEV_COMMON_CTL_EXT, &event_ext);
        if (retval < 0) {
            ar_err("failed to get sys info, use default ddr rw busrt len %d\n", ddr_burst_len);
        } else {
            //ddr_burst_len = sys_info.ddr_info.ddr_rw_burst_len;
            //we always set the display burst len as 4; 64byte algin
            ddr_burst_len = 4;
        }
        cuse_dev_close(fd_sys);
    }

    ar_always("ddr burst %d\n", ddr_burst_len);

    info = framebuffer_alloc(sizeof(ar_fb_context_t), &dev->dev);
    if (!info) {
        ar_err("framebuffer_alloc failed\n");
        goto release_exit;
    }

    ar_fb_context = (ar_fb_context_t *)info->par;
    ar_fb_context->data.ddr_rw_burst_len = ddr_burst_len;

    line_width_stride = (ddr_burst_len << (AR_FRAMEBUFFER_DDR_BURST_LEN_SHIFT-2)) - 1;
    ar_fb_var.xres_virtual = (ar_fb_var.xres + (line_width_stride)) & (~line_width_stride);
    ar_fb_fix.line_length = get_line_length(info, ar_fb_var.xres_virtual, ar_fb_var.bits_per_pixel);

    //do a 4k size align
    videomemorysize = get_buffer_size();
	videomemorysize = (videomemorysize + (AR_FRAMEBUFFER_PAGE_STRIDE-1)) & (~(AR_FRAMEBUFFER_PAGE_STRIDE-1));
    if (0 == videomemorysize) {
        ar_err("fail to calculate memory size");
        goto release_exit1;
    }

#if defined(CONFIG_ION)
    // alloc buffer from ion
    retval = artosyn_client_create("fb_ion");
    if (retval < 0) {
        ar_err("create ion faild");
        goto release_exit1;
    }

    ar_fb_context->ion_fd = artosyn_alloc_buffer("fb_ion", videomemorysize,
                                 0x1000, (1<<AR_ION_SYSTEM_HEAP_ID), 0, &va, &pa);
    if (ar_fb_context->ion_fd < 0 || !va || !pa) {
        ar_err("artosyn_alloc_buffer ion failded");
        goto release_exit2;
    }
#else
    // alloc buffer from mmz
    struct mmb_info mi;
    memset(&mi, 0, sizeof(mi));
    mi.flags = MAP_SHARED;
    mi.size = videomemorysize;
    mi.prot = PROT_READ | PROT_WRITE;
    strncpy(mi.mmb_name, "fb_mmz", HIL_MMB_NAME_LEN - 1);

    ar_fb_context->mmb = hil_mmb_alloc_v2(mi.mmb_name, mi.size, mi.align,
                                          mi.gfp, mi.mmz_name, mi.order);
    if (!ar_fb_context->mmb) {
        ar_err("alloc MMB faild");
        goto release_exit1;
    }

    pa = hil_mmb_phys(ar_fb_context->mmb);
    va = hil_mmb_map2kern(ar_fb_context->mmb);
    if (!va) {
        ar_err("hil_mmb_map2kern failed");
        goto release_exit2;
    }
#endif

    pa_aligned = ((unsigned long)pa + (AR_FRAMEBUFFER_PAGE_STRIDE-1)) & (~(AR_FRAMEBUFFER_PAGE_STRIDE-1));
    va_aligned = ((unsigned long)va + (AR_FRAMEBUFFER_PAGE_STRIDE-1)) & (~(AR_FRAMEBUFFER_PAGE_STRIDE-1));
    //dma_sync_single_range_for_device(NULL, pa_aligned, 0, videomemorysize, DMA_TO_DEVICE);
    memset(va, 0, videomemorysize);

    ar_fb_fix.smem_len = videomemorysize;
    ar_fb_fix.smem_start = pa_aligned;
    info->fix = ar_fb_fix;
    info->var = ar_fb_var;
    info->screen_base = va_aligned;
    info->screen_size = videomemorysize;
    info->fbops = &ar_fb_ops;
    info->flags = FBINFO_FLAG_DEFAULT;

    //init ar_fb_context
    if (ar_fb_priv_context_init(info, pa_aligned)) {
        ar_err("priv context init\n");
        goto release_exit3;
    }

    //init remote device
    if (0 != ar_fb_remote_init(info)) {
        ar_err("fb remote init failed");
        goto release_exit4;
    }

    retval = fb_alloc_cmap(&info->cmap, 256, 0);
    if (retval < 0) {
        goto release_exit5;
    }

    platform_set_drvdata(dev, info);
    retval = register_framebuffer(info);
    if (retval < 0) {
        ar_err("Failed to register framebuffer device: %d\n", retval);
        goto release_exit6;
    }

    return 0;

release_exit6:
    fb_dealloc_cmap(&info->cmap);
release_exit5:
    ar_fb_remote_release(ar_fb_context->dev.fd);
release_exit4:
    cuse_dev_close(ar_fb_context->dev.fd);
release_exit3:
#if defined(CONFIG_ION)
    artosyn_free_buffer("fb_ion", ar_fb_context->ion_fd);
#else
    if (ar_fb_context->mmb)
        hil_mmb_unmap(ar_fb_context->mmb);
#endif
release_exit2:
#if defined(CONFIG_ION)
    artosyn_destroy_client("fb_ion");
#else
    if (ar_fb_context->mmb)
        hil_mmb_free(ar_fb_context->mmb);
#endif
release_exit1:
    if (info != NULL) {
        framebuffer_release(info);
        info = NULL;
    }
release_exit:

    return retval;
}

static int ar_sysctl_thread_func(void * args)
{
    struct platform_device *dev = (struct platform_device *)args;
    int fd = 0;
#if defined(_32BIT_SDK)
    bool compat = true;
#else
    bool compat = false;
#endif

    ar_always("wait fb remote status...");

    while(!remote_is_ready)
    {
        if(kthread_should_stop()){
            do_exit(0);
        }

        fd = cuse_dev_open(AR_FRAMEBUFFER_REMOTE_DEVICE_NAME, compat);
        if (fd < 0) {
            msleep(100);
        }else{
            remote_is_ready = 1;
            cuse_dev_close(fd);
            ar_always("fb remote is ready!");
        }
    }

    if(remote_is_ready)
        ar_fb_init(dev);

    return 0;
}

static int ar_fb_probe(struct platform_device *dev)
{
    remote_check_thread =  kthread_run(ar_sysctl_thread_func, (void*)dev, "remote_check_thread");
    return 0;
}

static int ar_fb_remove(struct platform_device *dev)
{
    ar_fb_context_t *ar_fb_context = NULL;
    struct fb_info *info = platform_get_drvdata(dev);
    v4ar_mem_t mem_req = {0};
    v4ar_common_ctl_ext_t arg;

    if(!remote_is_ready)
        return 0;

    if (info) {
        ar_fb_context = (ar_fb_context_t *)info->par;

        unregister_framebuffer(info);
        platform_set_drvdata(dev, NULL);

        if (ar_fb_context->dev.fd >= 0) {
            ar_fb_remote_release(ar_fb_context->dev.fd);
            cuse_dev_close(ar_fb_context->dev.fd);
        }

        if (ar_fb_context->data.mem_start) {
#if defined(CONFIG_ION)
            artosyn_free_buffer("fb_ion", ar_fb_context->ion_fd);
            artosyn_destroy_client("fb_ion");
#else
            if (ar_fb_context->mmb) {
                hil_mmb_unmap(ar_fb_context->mmb);
                hil_mmb_free(ar_fb_context->mmb);
            }
#endif
        }

        fb_dealloc_cmap(&info->cmap);
        framebuffer_release(info);
    }

    return 0;
}

static const struct of_device_id ar_framebuffer_match[] = {
	{.compatible = "artosyn,ar_framebuffer",},
	{},
};

MODULE_DEVICE_TABLE(of, ar_framebuffer_match);

static struct platform_driver ar_framebuffer_driver = {
    .probe  = ar_fb_probe,
    .remove = ar_fb_remove,
    .driver = {
        .name           = "ar_framebuffer",
        .of_match_table = ar_framebuffer_match,
    },
};

static bool ar_framebuffer_enable __initdata = 0;  /* disabled by default */

static int __init ar_framebuffer_setup(char *options)
{
    char *this_opt;
    ar_framebuffer_enable = 0;

    if (!options || !*options) {
        if (options) {
            ar_always("empty info %x", *options);
        } else {
            ar_err("empty input of options");
        }

        ar_framebuffer_enable = 1;
        mode_option = NULL;
        return 0;
    }

    ar_always("setup options: %s", options);

    ar_framebuffer_enable = 1;

    while ((this_opt = strsep(&options, ",")) != NULL) {
        if (!*this_opt)
            continue;

        if (!strcmp(this_opt, "disable")) {
            ar_framebuffer_enable = 0;
            ar_always("disable ar framebuffer");
        } else {
            mode_option = this_opt;
            ar_always("get options: %s", options);
        }
    }

    return 0;
}

static int __init ar_framebuffer_init(void)
{
    int ret = 0;
    char *option = NULL;

    ar_always("enter...");

    if (fb_get_options("ar_framebuffer", &option)) {
        ar_err("get options for ar framebuffer");
        return -ENODEV;
    }
    ar_framebuffer_setup(option);

    if (!ar_framebuffer_enable) {
        ar_err("ar framebuffer is disabled");
        return -ENXIO;
    }

    ret = platform_driver_register(&ar_framebuffer_driver);
    if (ret) {
        ar_err("register driver: %d\n", ret);
    }

    ar_always("exit...");
    return ret;
}

static void __exit ar_framebuffer_exit(void)
{
    if(remote_check_thread && !remote_is_ready){
        kthread_stop(remote_check_thread);
    }

    platform_driver_unregister(&ar_framebuffer_driver);
    ar_always("exit...\n");
}

module_param_named(width, frame_buffer_width, int, 0);
module_param_named(height, frame_buffer_height, int, 0);
module_param_named(number, frame_buffer_count, int, 0);
module_param_named(format, frame_buffer_format, int, 0);
module_param_named(stream, frame_stream_enable, int, 0);

module_init(ar_framebuffer_init);
module_exit(ar_framebuffer_exit);

MODULE_LICENSE("GPL");
