#ifndef __AR_GDC_H__
#define __AR_GDC_H__

#define GDC_MAX_PANNEL_COUNT 4

#define AR_GDC_LUT_SPACE_RANGE 0x2800


typedef struct
{
  int width;
  int height;
  int luma_stride;
  int chroma_stride;
}ar_gdc_buffer_format_t;

typedef struct
{
   unsigned long long addr_virt;
   unsigned long long addr_phy;
}ar_gdc_buffer_pannel_t;

typedef struct
{
  ar_gdc_buffer_pannel_t pannel[GDC_MAX_PANNEL_COUNT]; //GDC DATA 地址
  ar_gdc_buffer_format_t format;  //GDC 图像格式
}AR_GDC_BUFFER_S;


typedef struct
{
	AR_GDC_BUFFER_S in_buffer;   //GDC 图像输入
	AR_GDC_BUFFER_S out_buffer;  //GDC 图像输出
	uint32_t rotate_angle;       //0: 0, 1: 90, 2:180, 3:270  counter-clockwise
	uint32_t transform_ex;       //0: Standard Rotate(0/90/180/270) Non-0:Other GDC Operations
	uint32_t lut_pa;             //LUT 物理地址
	uint32_t lut_len;            //LUT 数据长度
} AR_GDC_PARAMS_S;


#define AR_GDC_IOC_MAGIC 'Z'
#define AR_GDC_IOC_START          _IOWR(AR_GDC_IOC_MAGIC, 0, AR_GDC_PARAMS_S)
#define AR_GDC_IOC_SET_FREQUENCY  _IOWR(AR_GDC_IOC_MAGIC, 1, unsigned int)
#define AR_GDC_IOC_GET_TIME       _IOWR(AR_GDC_IOC_MAGIC, 2, unsigned long long)

#endif

