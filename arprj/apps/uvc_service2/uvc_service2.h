#ifndef __UVC_SERVICE2_H__
#define __UVC_SERVICE2_H__

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <semaphore.h>

#include <linux/usb/video.h>
#include <linux/videodev2.h>
#include <linux/usb/uvc.h>

#include <mpi_type.h>
#include <mpi_isp.h>
#include <ar_comm_video.h>
//#include <cam_type_def.h>
#include "uvc_sensor.h"

//#include "ar_cam_src.h"
//#include "ar_meta_define.h"
//#include "ar_multimedia_base.h"
//#include "ar_video_stream.h"
//#include "ar_picture_common_api.h"

#include "pipe_transfer_framework.h"

#define log_info 		printf
#define log_always		printf
#define log_err			printf
#define log_printf		printf

#define clamp(val, min, max) ({                 \
        typeof(val) __val = (val);              \
        typeof(min) __min = (min);              \
        typeof(max) __max = (max);              \
        (void) (&__val == &__min);              \
        (void) (&__val == &__max);              \
        __val = __val < __min ? __min: __val;   \
        __val > __max ? __max: __val; })

#define ARRAY_SIZE(a)                           ((sizeof(a) / sizeof(a[0])))
#define MAX_BUFFER_SIZE                         (4 * 1024 * 1024)
#define BUFFER_COUNT                            (4)
#define POOL_COUNT                              (2)
#define CAM_NUM_MAX                             (6)
#define USB_30_VALUE                            (0x0300)
#define USB_20_VALUE                            (0x0200)

#define __RESOLUTION_NUM__                      256

#define __IPC_CEVA_KEY__                        (0x1234)
#define __IPC_CEVA_BUF_NUM__                    BUFFER_COUNT       // >= BUFFER_COUNT

#define __IPC_LOG_KEY__                         (0x4321)
#define __IPC_LOG_BUF_NUM__                     BUFFER_COUNT       // >= BUFFER_COUNT

#define __OUTPUT_DEVICE_NAME__                  "dwc2"
#define __OUTPUT_DEVICE_NAME2__                 "usb-"

#define __DEFAULT_HEVC_BITRATE__                (4 * 1024)
#define __DEFAULT_H264_BITRATE__                (4 * 1024)
#define __DEFAULT_MJPG_BITRATE__                (40 * 1024)

#define __UVC_SINK_PATH__                       "/dev/uvc_sink-%d"

#define __MAX_I_FRAME_FOR_H26x_START__          (2)

#define CT_AUTO_EXPOSURE_MODE_DEFAULF_VAL       2
#define CT_AUTO_EXPOSURE_MODE_AUTO_MODE         (1 << 1)
#define CT_AUTO_EXPOSURE_MODE_SHUTTER_MODE      (1 << 2)
#define CT_AUTO_EXPOSURE_MODE_RES               (CT_AUTO_EXPOSURE_MODE_AUTO_MODE | CT_AUTO_EXPOSURE_MODE_SHUTTER_MODE)

#define CT_EXPOSURE_TIME_ABSOLUTE_MIN_VAL       1
#define CT_EXPOSURE_TIME_ABSOLUTE_MAX_VAL       300
#define CT_EXPOSURE_TIME_ABSOLUTE_STEP_SIZE     1

#define CT_ZOOM_ABSOLUTE_MIN_VAL                100
#define CT_ZOOM_ABSOLUTE_MAX_VAL                200
#define CT_ZOOM_ABSOLUTE_STEP_SIZE              1
#define CT_ZOOM_ABSOLUTE_DEFAULT_VAL            100

#define CT_ROLL_ABSOLUTE_MIN_VAL                0
#define CT_ROLL_ABSOLUTE_MAX_VAL                3
#define CT_ROLL_ABSOLUTE_STEP_SIZE              1
#define CT_ROLL_ABSOLUTE_DEFAULT_VAL            0
#define CT_ROLL_ABSOLUTE_MIRROR                 (1 << 0)
#define CT_ROLL_ABSOLUTE_FLIP                   (1 << 1)

#define PU_BRIGHTNESS_MIN_VAL                   0
#define PU_BRIGHTNESS_MAX_VAL                   100
#define PU_BRIGHTNESS_STEP_SIZE                 1
#define PU_BRIGHTNESS_DEFAULT_VAL               50

#define PU_CONTRAST_MIN_VAL                     0
#define PU_CONTRAST_MAX_VAL                     100
#define PU_CONTRAST_STEP_SIZE                   1
#define PU_CONTRAST_DEFAULT_VAL                 50

#define PU_HUE_MIN_VAL                          0
#define PU_HUE_MAX_VAL                          100
#define PU_HUE_STEP_SIZE                        1
#define PU_HUE_DEFAULT_VAL                      50

#define PU_SATURATION_MIN_VAL                   0
#define PU_SATURATION_MAX_VAL                   100
#define PU_SATURATION_STEP_SIZE                 1
#define PU_SATURATION_DEFAULT_VAL               50

#define PU_SHARPNESS_MIN_VAL                    0
#define PU_SHARPNESS_MAX_VAL                    100
#define PU_SHARPNESS_STEP_SIZE                  1
#define PU_SHARPNESS_DEFAULT_VAL                50

#define PU_GAMMA_MIN_VAL                        0
#define PU_GAMMA_MAX_VAL                        100
#define PU_GAMMA_STEP_SIZE                      1
#define PU_GAMMA_DEFAULT_VAL                    50

#define PU_WHITE_BALANCE_TEMPERATURE_MIN_VAL        2800
#define PU_WHITE_BALANCE_TEMPERATURE_MAX_VAL        6800
#define PU_WHITE_BALANCE_TEMPERATURE_STEP_SIZE      1
#define PU_WHITE_BALANCE_TEMPERATURE_DEFAULT_VAL    5000
#define PU_WHITE_BALANCE_TEMPERATURE_AUTO_VAL       10

#define PU_BACKLIGHT_COMPENSATION_MIN_VAL           0
#define PU_BACKLIGHT_COMPENSATION_MAX_VAL           100
#define PU_BACKLIGHT_COMPENSATION_STEP_SIZE         1
#define PU_BACKLIGHT_COMPENSATION_DEFAULT_VAL       50

#define PU_GAIN_MIN_VAL                         0
#define PU_GAIN_MAX_VAL                         255
#define PU_GAIN_STEP_SIZE                       1
#define PU_GAIN_DEFAULT_VAL                     127

#define PU_POWER_LINE_FREQUENCY_MIN_VAL         0
#define PU_POWER_LINE_FREQUENCY_MAX_VAL         3
#define PU_POWER_LINE_FREQUENCY_DEFAULT_VAL     3

#define PU_HUE_AUTO_MIN_VAL                     0
#define PU_HUE_AUTO_MAX_VAL                     255
#define PU_HUE_AUTO_STEP_SIZE                   1
#define PU_HUE_AUTO_DEFAULT_VAL                 1


#define WHITE_BALANCE_TEMPERATURE_AUTO_MIN_VAL      0
#define WHITE_BALANCE_TEMPERATURE_AUTO_MAX_VAL      1
#define WHITE_BALANCE_TEMPERATURE_AUTO_STEP_SIZE    1
#define WHITE_BALANCE_TEMPERATURE_AUTO_DEFAULT_VAL  1

struct uvc_frame_info
{
    unsigned int width;
    unsigned int height;
    unsigned int intervals[8];
};

struct uvc_frame_buf_info
{
    unsigned long addr;
    unsigned int len;
    uint64_t pts;    /**< timestamps*/
    union
    {
        unsigned int index;
        void * ptr;
    };
};

struct raw_data_buf_info
{
    void * ptr;
    unsigned int len;
    uint64_t pts;    /**< timestamps*/
};


typedef struct uvc_format_data  uvc_format_data_t;

struct uvc_format_opt
{
    int (*format_init)(uvc_format_data_t *data, void * handle, int argc, char *argv[]);
    int (*format_deinit)(uvc_format_data_t *data, void * handle);
    struct uvc_frame_buf_info * (*get_frame)(uvc_format_data_t *data);
    int (*release_frame)(uvc_format_data_t *data, struct uvc_frame_buf_info * buf_info);
};

struct uvc_format_data
{
    void * vs_handle;
    struct uvc_format_opt * format_opt;
    void * priv;
};

struct uvc_format_info
{
    unsigned int fcc;
    struct uvc_frame_info *frames;
    uvc_format_data_t *data;
};

typedef struct
{
    unsigned char           auto_exposure_mode_control;
    STRU_AEC_SET_INFO_T     auto_exposure_aec_info;
    //aec_set_info_t          auto_exposure_aec_info;
    unsigned int            exposure_time_absolute_val;
    short                   zoom_absolute_val;
    short                   roll_absolute_val;
}uvc_camera_terminal;

typedef struct
{
    unsigned int            brightness_val;
    unsigned short          contrast_val;
    unsigned int            hue_val;
    unsigned int            saturation_val;
    unsigned int            sharpness_val;
    unsigned int            gamma_val;
    unsigned int            white_balance_temperature_val;
    unsigned int            white_balance_component_val;
    unsigned int            backlight_compensation_val;
    unsigned int            gain_val;
    unsigned char           power_line_frequency_val;
    unsigned int            hue_auto_val;
    unsigned int            white_balance_temperature_auto_val;
}uvc_processing_unit;

struct uvc_device
{
    int                     fd;
    //int                     camera_fd;

    struct uvc_streaming_control probe;
    struct uvc_streaming_control commit;
    unsigned int            control_interface;
    unsigned int            streaming_interface;

    int                     control;

    struct uvc_request_data request_error_code;
    uvc_camera_terminal     camera_terminal;
    uvc_processing_unit     processing_unit;
    unsigned char           cs;
    unsigned char           entity_id;

    unsigned int            fcc;
    unsigned int            new_fcc;
    unsigned int            fcc_index;
    unsigned int            width;
    unsigned int            height;
    unsigned int            frame_interval;

    void                  **mem;
    unsigned int            nbufs;
    unsigned int            bufsize;

    unsigned int            bulk;
    uint8_t                 color;

    unsigned int            imgsize;
    void                   *imgdata;

    unsigned int            pts; /*unit: ms*/
};

typedef enum
{
    UVC_STATUS_IDLE = 0,
    UVC_STATUS_INIT,
    UVC_STATUS_INIT_COMPLETE,
    UVC_STATUS_RELEASE_FRAME,
    UVC_STATUS_GET_FRAME,
    UVC_STATUS_CHANGE_FORMAT,
    UVC_STATUS_EXIT,
    UVC_STATUS_MAX
}uvc_status;


typedef struct
{
    pthread_mutex_t        g_wrt_com_mutex;
    sem_t                  g_chg_fmt_sem;
    volatile int           g_chg_fmt_flag;
    volatile int           g_stream_on;
    unsigned long long     g_uvc_timestamp;
    unsigned int           g_max_buffer_size;
    unsigned int           g_pool_count;
    unsigned int          *g_pool_size;
    volatile unsigned int  idx;
    void                  *first_image_data;
    unsigned int           first_image_size;
    void                  *cache_image_data;
    void                  *cache_image_data_org;
    unsigned long          cache_image_align;
    unsigned int           cache_image_size;
    unsigned int           cache_image_point;
    
    struct uvc_device     *dev;
    
    void *					handle;
    int                    run_flag;
    int                    dev_index;
    int                    cam_index;
    
    int                    width[2];
    int                    height[2];
    int                    multi;
    int                    osd;
    int                    is_hdr;
    int                    is_raw;
    int                    is_test;
    int                    use_display;
    int                    disp_squ;
    int                    disp_w;
    int                    disp_h;
    int                    disp_fps;
    
    struct uvc_frame_buf_info  **buf_info;
    unsigned int           now_format;
    int                    argc;
    char                 **argv;
    int                    first_inited;
    uvc_status             status;

    AR_CHAR                 sensor_name[128];
    AR_CHAR                 power_gpio[8];
    AR_CHAR                 reset_gpio[8];
    int                     i2c_bus;
    int                     fps;
    int                     hdr_fre;
    int                     vif_fre;
    int                     isp_fre;
    int                     mipi_fre;
    int                     h26x_core_fre;
    int                     h26x_bpu_fre;
    int                     jpeg_core_fre;
    
    UVC_SNS_TYPE_E          sns_type;
    WDR_MODE_E              wdr_mode;
    ENUM_VFE_MODE           vfe_mode;
    int                     vi_comp;
}uvc_server_t;

struct framebased_priv_data
{
    unsigned int    i_frame_for_start;
};

extern uvc_format_data_t uvc_h265_data;
extern uvc_format_data_t uvc_h264_data;
extern uvc_format_data_t uvc_mjpeg_data;
extern uvc_format_data_t uvc_nv12_data;
extern uvc_format_data_t uvc_yuy2_data;
extern uvc_format_data_t uvc_z16_data;
extern struct uvc_format_info *g_uvc_formats;
extern int g_uvc_formats_num;

uvc_server_t * get_uvc_server();

#endif
