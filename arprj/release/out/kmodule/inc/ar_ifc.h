#ifndef __AR_IFC_H__
#define __AR_IFC_H__

typedef struct
{
    int sum_rh;
    int sum_rl;
    int sum_gh;
    int sum_gl;
    int sum_bh;
    int sum_bl;
    int ssum_rh;
    int ssum_rl;
    int ssum_gh;
    int ssum_gl;
    int ssum_bh;
    int ssum_bl;
    unsigned int frame_index; //for auto mode
} ar_ifc_result_s;

typedef struct
{
    //yuv src
    unsigned int yuv_width;
    unsigned int yuv_height;
    unsigned int y_stride;
    unsigned int u_stride;
    unsigned int v_stride;
    unsigned int y_addr;
    unsigned int u_addr;
    unsigned int v_addr;
    unsigned int yuv_format;
    unsigned int yuv_pixel_bit;
    unsigned int yuv_pixel_byte;

    //rgb dst
    unsigned int r_stride;
    unsigned int g_stride;
    unsigned int b_stride;
    unsigned int r_addr;
    unsigned int g_addr;
    unsigned int b_addr;
    int r_c0;
    int r_c1;
    int r_c2;
    int r_c3;
    int g_c0;
    int g_c1;
    int g_c2;
    int g_c3;
    int b_c0;
    int b_c1;
    int b_c2;
    int b_c3;
    unsigned int rgb_bits_shift;
    int rgb_min;
    int rgb_max;
    int r_avg;
    int g_avg;
    int b_avg;
    unsigned int rgb_format;
} ar_ifc_params_st;

typedef struct
{
    ar_ifc_params_st ifc_param;
    ar_ifc_result_s result_param;
}ar_ifc_ioctl_single_st;

typedef struct
{
    unsigned int params_addr_phy;
    unsigned int result_addr_phy;
    unsigned int frame_num;
} ar_ifc_ioctl_auto_st;


#define IFC_IOC_MAGIC 'F'
#define IFC_IOC_START_SINGLE _IOWR(IFC_IOC_MAGIC, 0, ar_ifc_ioctl_single_st)
#define IFC_IOC_START_AUTO   _IOWR(IFC_IOC_MAGIC, 1, ar_ifc_ioctl_auto_st)
#define IFC_IOC_SET_CACHE_COHERENCY _IOWR(IFC_IOC_MAGIC, 2, unsigned int)
#define IFC_IOC_SET_SECURITY  _IOWR(IFC_IOC_MAGIC, 3, unsigned int)
#define IFC_IOC_SET_FREQUENCY  _IOWR(IFC_IOC_MAGIC, 4, unsigned int)


#endif

