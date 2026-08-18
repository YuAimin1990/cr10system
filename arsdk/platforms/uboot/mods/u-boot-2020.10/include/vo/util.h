#ifndef __AR_UTIL_H__
#define __AR_UTIL_H__
//#include <common.h>
#include <stdlib.h>
#include <stdint.h>
#include <config.h>
#include <errno.h>
#include <malloc.h>
#include <linux/delay.h>

#define  DISPLAY_UBOOT

#define MOD_DISPLAY "DISPLAY"

enum{
    AR_SYSTEM_INTERLACE_MOD_1080I,
    AR_SYSTEM_INTERLACE_MOD_720_576I_PAL,
    AR_SYSTEM_INTERLACE_MOD_720_480I_NTSC,
    AR_SYSTEM_INTERLACE_MOD_NULL,
};

typedef enum {
    AR_VIDEO_FORMAT_UNKNOWN = 0,  //
    /* YUV 8 bit*/
    AR_VIDEO_FORMAT_I400,
    AR_VIDEO_FORMAT_I420,  // Planar 4:2:0 YUV
    AR_VIDEO_FORMAT_YV12,  // Planar 4:2:0 YVU (like I420 but UV swapped)
    AR_VIDEO_FORMAT_NV12,  // Planar 4:2:0 YUV with interleaved UV plane
    AR_VIDEO_FORMAT_NV21,  // Planar 4:2:0 YUV with interleaved VU plane
    AR_VIDEO_FORMAT_Y42B,  // Planar 4:2:2 YUV
    AR_VIDEO_FORMAT_YUYV,  // Packed 4:2:2 YUV(Y0-U0-Y1-V0 Y2-U1-Y3-V1 ....)
    AR_VIDEO_FORMAT_YVYU,  // Packed 4:2:2 YUV(Y0-V0-Y1-U0 Y2-V1-Y3-U1 ....)
    AR_VIDEO_FORMAT_Y444,  // Planar 4:4:4 YUV

    /* YUV 10 bit, save as 16bit for per component, little endian */
    AR_VIDEO_FORMAT_I420_P10_16BIT_LE,  // Planar 4:2:0 YUV
    AR_VIDEO_FORMAT_YV12_P10_16BIT_LE,  // Planar 4:2:0 YVU (like I420 but UV swapped)
    AR_VIDEO_FORMAT_NV12_P10_16BIT_LE,  // Planar 4:2:0 YUV with interleaved UV plane
    AR_VIDEO_FORMAT_NV21_P10_16BIT_LE,  // Planar 4:2:0 YUV with interleaved VU plane
    AR_VIDEO_FORMAT_Y42B_P10_16BIT_LE,  // Planar 4:2:2 YUV
    AR_VIDEO_FORMAT_YUYV_P10_16BIT_LE,  // Packed 4:2:2 YUV(Y0-U0-Y1-V0 Y2-U1-Y3-V1 ....)
    AR_VIDEO_FORMAT_YVYU_P10_16BIT_LE,  // Packed 4:2:2 YUV(Y0-V0-Y1-U0 Y2-V1-Y3-U1 ....)
    AR_VIDEO_FORMAT_Y444_P10_16BIT_LE,  // Planar 4:4:4 YUV
    /* YUV 10 bit, save as 16bit for per component, big endian */
    AR_VIDEO_FORMAT_I420_P10_16BIT_BE,  // Planar 4:2:0 YUV
    AR_VIDEO_FORMAT_YV12_P10_16BIT_BE,  // Planar 4:2:0 YVU (like I420 but UV swapped)
    AR_VIDEO_FORMAT_NV12_P10_16BIT_BE,  // Planar 4:2:0 YUV with interleaved UV plane
    AR_VIDEO_FORMAT_NV21_P10_16BIT_BE,  // Planar 4:2:0 YUV with interleaved VU plane
    AR_VIDEO_FORMAT_Y42B_P10_16BIT_BE,  // Planar 4:2:2 YUV
    AR_VIDEO_FORMAT_YUYV_P10_16BIT_BE,  // Packed 4:2:2 YUV(Y0-U0-Y1-V0 Y2-U1-Y3-V1 ....)
    AR_VIDEO_FORMAT_YVYU_P10_16BIT_BE,  // Packed 4:2:2 YUV(Y0-V0-Y1-U0 Y2-V1-Y3-U1 ....)
    AR_VIDEO_FORMAT_Y444_P10_16BIT_BE,  // Planar 4:4:4 YUV

    /* YUV 10 bit, save as 32bit for 3 components, little endian */
    AR_VIDEO_FORMAT_I420_P10_32BIT_LE,  // Planar 4:2:0 YUV
    AR_VIDEO_FORMAT_YV12_P10_32BIT_LE,  // Planar 4:2:0 YVU (like I420 but UV swapped)
    AR_VIDEO_FORMAT_NV12_P10_32BIT_LE,  // Planar 4:2:0 YUV with interleaved UV plane
    AR_VIDEO_FORMAT_NV21_P10_32BIT_LE,  // Planar 4:2:0 YUV with interleaved VU plane
    AR_VIDEO_FORMAT_Y42B_P10_32BIT_LE,  // Planar 4:2:2 YUV
    AR_VIDEO_FORMAT_YUYV_P10_32BIT_LE,  // Packed 4:2:2 YUV(Y0-U0-Y1-V0 Y2-U1-Y3-V1 ....)
    AR_VIDEO_FORMAT_YVYU_P10_32BIT_LE,  // Packed 4:2:2 YUV(Y0-V0-Y1-U0 Y2-V1-Y3-U1 ....)
    AR_VIDEO_FORMAT_Y444_P10_32BIT_LE,  // Planar 4:4:4 YUV
    /* YUV 10 bit, save as 32bit for 3 components, big endian */
    AR_VIDEO_FORMAT_I420_P10_32BIT_BE,  // Planar 4:2:0 YUV
    AR_VIDEO_FORMAT_YV12_P10_32BIT_BE,  // Planar 4:2:0 YVU (like I420 but UV swapped)
    AR_VIDEO_FORMAT_NV12_P10_32BIT_BE,  // Planar 4:2:0 YUV with interleaved UV plane
    AR_VIDEO_FORMAT_NV21_P10_32BIT_BE,  // Planar 4:2:0 YUV with interleaved VU plane
    AR_VIDEO_FORMAT_Y42B_P10_32BIT_BE,  // Planar 4:2:2 YUV
    AR_VIDEO_FORMAT_YUYV_P10_32BIT_BE,  // Packed 4:2:2 YUV(Y0-U0-Y1-V0 Y2-U1-Y3-V1 ....)
    AR_VIDEO_FORMAT_YVYU_P10_32BIT_BE,  // Packed 4:2:2 YUV(Y0-V0-Y1-U0 Y2-V1-Y3-U1 ....)
    AR_VIDEO_FORMAT_Y444_P10_32BIT_BE,  // Planar 4:4:4 YUV

    /* RGB */
    AR_VIDEO_FORMAT_RGB, //
    /* RGB 10 bit, save as 16bit for per component */
    AR_VIDEO_FORMAT_RGB_P10_16BIT_LE,
    AR_VIDEO_FORMAT_RGB_P10_16BIT_BE,
    /* RGB 10 bit, save as 32bit for 3 components */
    AR_VIDEO_FORMAT_RGB_P10_32BIT_LE,
    AR_VIDEO_FORMAT_RGB_P10_32BIT_BE,

    /* RAW */
    AR_VIDEO_FORMAT_RAW_RGGB_P8,
    AR_VIDEO_FORMAT_RAW_BGGR_P8,
    AR_VIDEO_FORMAT_RAW_GRBG_P8,
    AR_VIDEO_FORMAT_RAW_GBRG_P8,


    AR_VIDEO_FORMAT_RAW_RGGB_P10,
    AR_VIDEO_FORMAT_RAW_BGGR_P10,
    AR_VIDEO_FORMAT_RAW_GRBG_P10,
    AR_VIDEO_FORMAT_RAW_GBRG_P10,

    AR_VIDEO_FORMAT_RAW_RGGB_P12,
    AR_VIDEO_FORMAT_RAW_BGGR_P12,
    AR_VIDEO_FORMAT_RAW_GRBG_P12,
    AR_VIDEO_FORMAT_RAW_GBRG_P12,

    AR_VIDEO_FORMAT_RAW_RGGB_P14,
    AR_VIDEO_FORMAT_RAW_BGGR_P14,
    AR_VIDEO_FORMAT_RAW_GRBG_P14,
    AR_VIDEO_FORMAT_RAW_GBRG_P14,

    /* RAW 10 bit, save as 16bit for per component, little endian */
    AR_VIDEO_FORMAT_RAW_RGGB_P10_16BIT_LE,
    AR_VIDEO_FORMAT_RAW_BGGR_P10_16BIT_LE,
    AR_VIDEO_FORMAT_RAW_GRBG_P10_16BIT_LE,
    AR_VIDEO_FORMAT_RAW_GBRG_P10_16BIT_LE,
    /* RAW 10 bit, save as 32 bit for 3 components, little endian */
    AR_VIDEO_FORMAT_RAW_RGGB_P10_32BIT_LE,
    AR_VIDEO_FORMAT_RAW_BGGR_P10_32BIT_LE,
    AR_VIDEO_FORMAT_RAW_GRBG_P10_32BIT_LE,
    AR_VIDEO_FORMAT_RAW_GBRG_P10_32BIT_LE,

    AR_VIDEO_FORMAT_YUV422_SEMI_PLANNAR,  // Planar 4:2:2 YUV with interleaved UV plane
    AR_VIDEO_FORMAT_YUV422_SEMI_PLANNAR_P10_16BIT_LE,  // Planar 4:2:2 YUV with interleaved UV plane
    AR_VIDEO_FORMAT_YUV444_SEMI_PLANNAR_P10_16BIT_LE,  // Planar 4:4:4 YUV with interleaved UV plane
    AR_VIDEO_FORMAT_YUV422_SEMI_PLANNAR_P10_32BIT_LE,  // Planar 4:2:2 YUV with interleaved UV plane

    AR_VIDEO_FORMAT_Y420_CF50_P8_PLANNAR,
    AR_VIDEO_FORMAT_Y420_CF50_P10_PLANNAR,

    AR_VIDEO_FORMAT_MAX
} ar_video_format_t;

typedef enum {
    AR_VIDEO_MATRIX_BT601_FULL,
    AR_VIDEO_MATRIX_BT601_LIMIT,
    AR_VIDEO_MATRIX_BT709_FULL,
    AR_VIDEO_MATRIX_BT709_LIMIT,
    AR_VIDEO_MATRIX_NODATA_FULL,
    AR_VIDEO_MATRIX_NODATA_LIMIT,
    AR_VIDEO_MATRIX_MAX,
} ar_video_matrix_t;

unsigned int read_reg32(unsigned long addr);
void write_reg32(unsigned long addr, unsigned int data);
void write_reg32_Mask(uint32_t regAddr, uint32_t regData, uint32_t regDataMask);

#define CGU_REG_BASE     0x01070000
#define SYSCTRL_REG_BASE 0x0A100000

#define PS2MS(value)     ((value)/1000000000)

#define GPIO_DATA_HIGH			1
#define GPIO_DATA_LOW			0

#define GPIO_DIR_OUTPUT			1
#define GPIO_DIR_INPUT			0

/**********************   gpio ****************************/
#ifdef CONFIG_ARTOSYN_AR9301
#define GPIO_BASE               0x8400000
#define GPIO_DIR_BASE           (GPIO_BASE + 0x04)
#define GPIO_PORT_DIR_STEP      0x0C
#define GPIO_OUT_BASE           GPIO_BASE
#define GPIO_PORT_OUT_STEP      0x0C
#define GPIO_IN_BASE            (GPIO_BASE + 0x50)
#define GPIO_PORT_IN_STEP       0x04
#define GPIO_GROUP_STEP         0x20000

#define MAX_GPIO_GROUP          4
#define MAX_GPIO_PORT           4
#define MAX_GPIO_PIN            8
#endif

#ifdef CONFIG_ARTOSYN_AR9311
#define GPIO_BASE               0x0A10A000
#define GPIO_DIR_BASE           (GPIO_BASE + 0xC0)
#define GPIO_PORT_DIR_STEP      0x0C
#define GPIO_OUT_BASE           (GPIO_BASE + 0xBC)
#define GPIO_PORT_OUT_STEP      0x0C
#define GPIO_IN_BASE            (GPIO_BASE + 0xC4)
#define GPIO_PORT_IN_STEP       0x0C

#define MAX_GPIO_GROUP          1
#define MAX_GPIO_PORT           7
#endif

/**********************   gpio ****************************/

#define AR_SET_REG_BITS(reg,val,start_bit,end_bit) (reg)=((val)<<(start_bit))| ((reg)&~((0xffffffff>>(32-((end_bit)-(start_bit)+1)))<<(start_bit)))
#define AR_GET_REG_BITS(reg,start_bit,end_bit) (((reg)&((0xffffffff>>(32-((end_bit)-(start_bit)+1)))<<(start_bit)))>>(start_bit))

#define AR_ALIGN4(_x)              (((_x)+0x03)&~0x03)
#define AR_ALIGN8(_x)              (((_x)+0x07)&~0x07)
#define AR_ALIGN16(_x)             (((_x)+0x0f)&~0x0f)
#define AR_ALIGN32(_x)             (((_x)+0x1f)&~0x1f)
#define AR_ALIGN64(_x)             (((_x)+0x3f)&~0x3f)
#define AR_ALIGN128(_x)            (((_x)+0x7f)&~0x7f)
#define AR_ALIGN256(_x)            (((_x)+0xff)&~0xff)
#define AR_ALIGN512(_x)            (((_x)+0x1ff)&~0x1ff)
#define AR_ALIGN16384(_x)          (((_x)+0x3fff)&~0x3fff)

#define __REG32__(addr) (*(volatile unsigned int *)(addr))
#define __REG64__(addr) (*(volatile uint64_t *)(addr))
#define __REG8__(addr) (*(volatile unsigned char *)(addr))

//log interface
typedef void*  ar_handle_t;

#define ar_get_task_id()     ((uint32_t)0)
#define dbg_printf printf
//#define ENABALE_ALL_LOG

#ifndef ENABALE_ALL_LOG
#define DISABLE_CND_LOG
#define DISABLE_FUN_LOG
#define DISABLE_INFO_LOG
#define DISABLE_PRT_LOG
//#define DISABLE_ERR_LOG
#define DISABLE_ALWAYS_LOG
#define DISABLE_CTL_LOG
#define DISABLE_FORMAT_LOG
#define DISABLE_DBG_LOG
#define DISABLE_WARNING_LOG
#endif

#define float2int(value) ((int)((value)*10000))
#define USE_BUF_LOG
#ifdef USE_BUF_LOG
#define AR_LOG_LEVEL PRN_BUFF
#else
#define AR_LOG_LEVEL PRN_UART
#endif

#define MAX(a,b)  (((a) > (b)) ? (a):(b))
#define MIN(a,b)  (((a) > (b)) ? (b):(a))

#define ar_delay(ms) udelay(1000*ms)
#define ar_get_timestamp() 0

#ifndef DISABLE_ERR_LOG
#define ar_err(fmt,...) do{\
   printf("[%d][%x][ERR] %s "fmt"\n",ar_get_timestamp(),ar_get_task_id(),__FUNCTION__,##__VA_ARGS__);\
   }while(0)
#else
#define ar_err(fmt,...) do{}while(0)
#endif
#ifndef DISABLE_CND_LOG
#define ar_conditon(fmt, ...) do{\
   printf("[%d][%x][CND] %s "fmt"\n",ar_get_timestamp(),ar_get_task_id(),__FUNCTION__,##__VA_ARGS__);\
   }while(0)
#else
#define ar_conditon(fmt, ...) do{}while(0)
#endif
#ifndef DISABLE_FUN_LOG
#define ar_func_enter() do{\
   printf("[%d][%x][FUNC] %s enter %d\n",ar_get_timestamp(),ar_get_task_id(),__FUNCTION__,__LINE__);\
   }while(0)
#define ar_func_exit() do{\
   printf("[%d][%x][FUNC] %s exit\n",ar_get_timestamp(),ar_get_task_id(),__FUNCTION__);\
   }while(0)
#else
#define ar_func_enter() do{}while(0)
#define ar_func_exit() do{}while(0)
#endif
#ifndef DISABLE_INFO_LOG
#define ar_info(fmt, ...) do{\
   printf("[%d][%x][INFO] %s "fmt"\n",ar_get_timestamp(),ar_get_task_id(),__FUNCTION__,##__VA_ARGS__);\
   }while(0)
#else
#define ar_info(fmt, ...) do{}while(0)
#endif
#ifndef DISABLE_DBG_LOG
#define ar_debug(fmt, ...) do{\
   printf("[%d][%x][DEBUG] %s "fmt"\n",ar_get_timestamp(),ar_get_task_id(),__FUNCTION__,##__VA_ARGS__);\
   }while(0)
#else
#define ar_debug(fmt, ...)  do{}while(0)
#endif
#ifndef DISABLE_PRT_LOG
#define ar_printf(fmt, ...) do{\
   printf(fmt,##__VA_ARGS__);\
   }while(0)
#else
#define ar_printf(fmt, ...) do{}while(0)
#endif
#ifndef DISABLE_ALWAYS_LOG
#define ar_always(fmt, ...)    do{\
        printf("[%d][%x][ALWAYS] %s "fmt"\n",ar_get_timestamp(),ar_get_task_id(),__FUNCTION__,##__VA_ARGS__);\
        }while(0)
#else
#define ar_always(fmt, ...) do{}while(0)
#endif

#define ar_bug_on()   do{printf("%s %d",__FUNCTION__,__LINE__);while(1);}while(0)


#define ar_malloc malloc
#define ar_free   free

#define get_timer() 0L

////queue inerface ------------------------
typedef  struct __ar_queue_t ar_queue_t;

struct __ar_queue_t
{
 char * name;
 uint32_t queue_size;
 uint32_t valid_data_num;
 //queue data
 void **data;
 uint32_t header;
 uint32_t tail;
 //lock
 int (*queue_pop)(ar_queue_t * queue, void **item);
 int (*queue_insert)(ar_queue_t * queue,void *item);
 int (*get_queue_size)(ar_queue_t * queue);
 int (*look_up_head)(ar_queue_t * queue, void **item);
};
ar_queue_t *ar_creat_queue(uint32_t queue_size,char *name);
int ar_delete_queue(ar_queue_t **queue);

/* ar_lock stub */
typedef unsigned long ar_lock_t;
typedef unsigned long ar_signal_t;
void ar_lock(ar_lock_t lock_id);
void ar_unlock(ar_lock_t lock_id);
ar_lock_t ar_creat_lock(void);
uint64_t ar_get_timestamp_us(void);
ar_signal_t ar_create_signal(void);
void ar_enter_critical(void);
void ar_exit_critical(void);
void ar_trace_line(void);
void read_write_reg32_mask(uint32_t reg_addr, uint32_t reg_data, uint32_t mask);
int  ar_hal_set_pix_clk(float clk);

#ifdef CONFIG_ARTOSYN_AR9311
int ar9311_abb_set_mipi_div2(unsigned int freq);
#endif

int gpio_set_direct(uint32_t group, uint32_t port, uint32_t inner_gpio, uint32_t dir);
int gpio_set_val(uint32_t group, uint32_t port, uint32_t inner_gpio, uint32_t val);


#endif
