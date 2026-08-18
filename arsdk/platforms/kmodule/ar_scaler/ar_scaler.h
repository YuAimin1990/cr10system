#ifndef __AR_SCALER_H__
#define __AR_SCALER_H__

//#include <linux/miscdevice.h>

/* Only one single memory region can be handled each time.
*   src_phy_addr -----------------------
*                |                     |
*                |                     |
*                |    crop:[x,y]       |
*                |     ----w---        |
*                |     |      |        |         dst_phy_addr
*                |    h|      |----------------> ---------------
*                |     |      |        |         |             |
*                |     --------        |         |             |
*                |                     |         ---------------
*                |                     |
*                |---------------------|
*
*/

#define AR_SCALER_MAX_BATCH    255 //TMP
#define SCALER_MODE_BICUBIC     0
#define SCALER_MODE_BILINEAR    1

enum{
   SCALER_BIT_DEPTH_8=0,
   SCALER_BIT_DEPTH_10=1,
};

enum{
   SCALER_1_BYTES_PER_PIX=0,
   SCALER_2_BYTES_PER_PIX=1,
};

typedef struct
{
    uint32_t src_phy_addr;
    uint32_t src_w;
    uint32_t src_h;
    uint32_t src_stride;

    uint32_t crop_x;
    uint32_t crop_y;
    uint32_t crop_w;
    uint32_t crop_h;

    uint32_t dst_phy_addr;
    uint32_t dst_w;
    uint32_t dst_h;
    uint32_t dst_stride;

    uint32_t channels;
    uint32_t mode; //bilinear, bicubic
    uint32_t byte_per_pixel; //0: 1byte, 1: 2bytes
    uint32_t bit_depth; //0: 8bits, 1: 10bits
} ar_scaler_params_st;

typedef struct
{
    uint32_t batch_num;
    ar_scaler_params_st batch_params[AR_SCALER_MAX_BATCH];
} ar_scaler_auto_st;

#define AR_SCALER_IOC_MAGIC 'Z'
#define AR_SCALER_IOC_START   _IOWR(AR_SCALER_IOC_MAGIC, 0, ar_scaler_auto_st)
#define AR_SCALER_IOC_START_SINGLE _IOWR(AR_SCALER_IOC_MAGIC, 1, ar_scaler_params_st)
#define AR_SCALER_IOC_SET_FREQUENCY  _IOWR(AR_SCALER_IOC_MAGIC, 2, unsigned int)

#endif

