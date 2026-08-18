#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "sample_comm.h"
#include "hal_vo.h"
#include "hal_vin_log.h"
#include "hal_region.h"
#include "hal_dbglog.h"
#include "mpi_vb.h"
#include "mpi_vgs.h"
#include "osal.h"
#include "sample_vgs.h"

//#define SAMPLE_PRT   printf
#define AR_ALIGN4(_x)              (((_x)+0x03)&~0x03)
#define AR_ALIGN256(_x)            (((_x)+0xff)&~0xff)
#define ALIGN_TO(size,num)         ( (num)*( ( (size)+(num)-1)/(num) ) )
#define SET_REG_BITS(reg, pos)     ((reg) |= (1 << (pos)))
#define CLR_REG_BITS(reg, pos)     ((reg) &= (~(1 << (pos))))

typedef struct
{
    unsigned char type[2];
    unsigned char size[4];
    unsigned char retain[4];
    unsigned char offset[4];
    unsigned char head_len[4];
    unsigned char width[4];
    unsigned char height[4];
    unsigned char planes[2];
    unsigned char bitCount[2];
}bmp_head_t;

typedef struct
{
    AR_VOID  *y_vrt;
    AR_VOID  *u_vrt;
    AR_VOID  *v_vrt;

    AR_VOID  *y_phy;
    AR_VOID  *u_phy;
    AR_VOID  *v_phy;

    VB_BLK    y_blk;
    VB_BLK    u_blk;
    VB_BLK    v_blk;

    AR_S32    y_stride;
    AR_S32    uv_stride;
    AR_S32    w;
    AR_S32    h;
}STRU_ADDR;

typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
}rgb_t;

typedef struct
{
    uint8_t y;
    uint8_t u;
    uint8_t v;
}yuv_t;

typedef struct
{
    ENUM_AR_HAL_VO_DEV_ID    e_dev_id;
    ENUM_AR_HAL_VO_LAYER_ID  e_layer_id;
    AR_S32                   dev_fd;
    AR_S32                   layer_fd;

    STRU_ADDR                src_addr;
    STRU_ADDR                tmp_addr;
    STRU_ADDR                yuv_addr;
    STRU_ADDR                gray1_addr;
}STRU_DISPLAY_OBJ_T;


STRU_DISPLAY_OBJ_T  g_vo_obj={0};
AR_BOOL             is_running = AR_TRUE;
AR_BOOL             is_suspend = AR_FALSE;
ar_lock_t           vgs_lock;
ar_signal_t         vgs_signal;


/* source prepare */
static yuv_t rgb2yuv(rgb_t rgb)
{
    int yy,uu,vv;
    yuv_t yuv;

    yy=0.299*rgb.r+0.587*rgb.g+0.114*rgb.b;
    if(yy>255)
        yy=255;
    if(yy<0)
        yy=0;

    uu=128-0.168736*rgb.r-0.331264*rgb.g+0.5*rgb.b;
    if(uu>255)
        uu=255;
    if(uu<0)
        uu=0;

    vv=128+0.5*rgb.r-0.418688*rgb.g-0.081312*rgb.b;
    if(vv>255)
        vv=255;
    if(vv<0)
        vv=0;

    yuv.y=yy;
    yuv.u=uu;
    yuv.v=vv;
    return yuv;
}

AR_S32 vo_get_yuv_blk(VB_POOL pool_id, AR_U64 size, VB_BLK *y_blk,
                                VB_BLK *u_blk, VB_BLK *v_blk)
{
    if ((*y_blk = AR_MPI_VB_GetBlock(pool_id, size, NULL)) == AR_INVALID_HANDLE)
    {
        SAMPLE_ERR("Can't get vb!\n");
        return AR_FAILURE;
    }

    if ((*u_blk = AR_MPI_VB_GetBlock(pool_id, size, NULL)) == AR_INVALID_HANDLE)
    {
        SAMPLE_ERR("Can't get vb!\n");
        AR_MPI_VB_ReleaseBlock(*y_blk);
        return AR_FAILURE;
    }

    if ((*v_blk = AR_MPI_VB_GetBlock(pool_id, size, NULL)) == AR_INVALID_HANDLE)
    {
        SAMPLE_ERR("Can't get vb!\n");
        AR_MPI_VB_ReleaseBlock(*y_blk);
        AR_MPI_VB_ReleaseBlock(*u_blk);
        return AR_FAILURE;
    }

    return AR_SUCCESS;
}

static AR_S32 buffer_get(int blk_size, STRU_ADDR *addr)
{
    AR_S32    ret;
    VB_POOL   y_pool, u_pool, v_pool;

    if ( (ret = vo_get_yuv_blk(VB_INVALID_POOLID, blk_size, &addr->y_blk, &addr->u_blk, &addr->v_blk)) != AR_SUCCESS )
        return ret;

    SAMPLE_PRT("blk: %u, %u, %u\r\n", addr->y_blk, addr->u_blk, addr->v_blk);
    addr->y_phy = (AR_VOID *)AR_MPI_VB_Handle2PhysAddr(addr->y_blk);
    addr->u_phy = (AR_VOID *)AR_MPI_VB_Handle2PhysAddr(addr->u_blk);
    addr->v_phy = (AR_VOID *)AR_MPI_VB_Handle2PhysAddr(addr->v_blk);
    SAMPLE_PRT("phy addr: %p, %p, %p\r\n", addr->y_phy, addr->u_phy, addr->v_phy);
    y_pool = AR_MPI_VB_Handle2PoolId(addr->y_blk);
    u_pool = AR_MPI_VB_Handle2PoolId(addr->u_blk);
    v_pool = AR_MPI_VB_Handle2PoolId(addr->v_blk);
    SAMPLE_PRT("pool id: %u, %u, %u\r\n", y_pool, u_pool, v_pool);

    AR_MPI_VB_MmapPool(y_pool);
    AR_MPI_VB_MmapPool(u_pool);
    AR_MPI_VB_MmapPool(v_pool);

    ret = AR_MPI_VB_GetBlockVirAddr(y_pool, (AR_U64)addr->y_phy, &addr->y_vrt);
    if (ret != AR_HAL_VO_SUCCESS)
    {
        SAMPLE_ERR("y_vrt_addr get failed: %d\r\n", ret);
        return ret;
    }

    ret = AR_MPI_VB_GetBlockVirAddr(u_pool, (AR_U64)addr->u_phy, &addr->u_vrt);
    if (ret != AR_HAL_VO_SUCCESS)
    {
        SAMPLE_ERR("u_vrt_addr get failed: %d\r\n", ret);
        return ret;
    }

    ret = AR_MPI_VB_GetBlockVirAddr(v_pool, (AR_U64)addr->v_phy, &addr->v_vrt);
    if (ret != AR_HAL_VO_SUCCESS)
    {
        SAMPLE_ERR("v_vrt_addr get failed: %d\r\n", ret);
        return ret;
    }

    SAMPLE_PRT("y_addr(%p %p), u_addr(%p %p), v_addr(%p %p),\r\n",
                                              addr->y_phy, addr->y_vrt,
                                              addr->u_phy, addr->u_vrt,
                                              addr->v_phy, addr->v_vrt);
    return AR_HAL_VO_SUCCESS;
}

static void buffer_release(STRU_ADDR *addr)
{
    AR_MPI_VB_ReleaseBlock(addr->y_blk);
    AR_MPI_VB_ReleaseBlock(addr->u_blk);
    AR_MPI_VB_ReleaseBlock(addr->v_blk);
}

#if 0
static int file_data_get(char *file_name, char **vrt_addr,
                                        int *width, int *height, int *bitCnt)
{
    int ret;
    int stride = 0;
    size_t len = sizeof(bmp_head_t);
    ssize_t cnt;

    *width  = 1024;
    *height = 768;
    *bitCnt = 4;

    int fd = open(file_name, O_RDWR);
    if(fd < 0) {
        SAMPLE_ERR("open img file %s fail\r\n", file_name);
        ret = -1;
        goto End;
    }

   stride = AR_ALIGN4((*bitCnt) * (*width));
   len = stride * (*height);

   SAMPLE_PRT("bmp data stride: %d, len: %ld\r\n", stride, len);
   *vrt_addr = (char *)malloc(len);
   if(NULL == *vrt_addr) {
       SAMPLE_ERR("ar_malloc bmp data fail, len = %ld\r\n", len);
       ret = -7;
       goto End_Mem;
   }

   cnt = read(fd, *vrt_addr, len);
   if (cnt != len)
   {
       SAMPLE_ERR("read image data error (%ld, %ld)\r\n", cnt, len);
       ret = -8;
       free((unsigned char *)*vrt_addr);
       goto End_Mem;
   }

   ret = 0;

   End_Mem:

    close(fd);

   End:
    return ret;
}
#endif

static int bmp_image_data_get(char *file_name, char **vrt_addr,
                                        int *width, int *height, int *bitCnt)
{
    bmp_head_t *info = NULL;
    char *bmp_tmp_data = NULL;
    int ret;
    int bitCount, offset;
    ssize_t cnt;
    int stride = 0;
    size_t len = sizeof(bmp_head_t);

    SAMPLE_PRT("bmp_image_data_get\r\n");
    int fd = open(file_name, O_RDWR);
    if(fd < 0) {
        SAMPLE_ERR("open img file %s fail\r\n", file_name);
        ret = -1;
        goto End;
    }

    info = (bmp_head_t *)malloc(len);
    if(NULL == info) {
        SAMPLE_ERR("dma malloc info fail\r\n");
        ret = -1;
        goto End_Fd;
    }

    cnt = read(fd, (char *)info, len);
    if(cnt != len) {
        SAMPLE_ERR("read bmp header error (%ld, %ld)\r\n", cnt, len);
        ret = -2;
        goto End_Mem;
    }

    *width = info->width[0] + (info->width[1] << 8) \
        + (info->width[2] << 16) + (info->width[3] << 24);
    *height = info->height[0] +  (info->height[1] << 8)\
         + (info->height[2] << 16) + (info->height[3] << 24);
    offset = info->offset[0] + (info->offset[1] << 8)\
         + (info->offset[2] << 16) + (info->offset[3] << 24);
    bitCount = info->bitCount[0] + (info->bitCount[1] << 8) ;
    *bitCnt = bitCount;
    SAMPLE_PRT("bmp info width = %d, height = %d, offset = %d, bitCount = %d\r\n",
           *width, *height, offset, bitCount);

    if((24 != bitCount) && (32 != bitCount)){
        SAMPLE_ERR("format wrong, only support RGB888 format\r\n");
        ret = -3;
        goto End_Mem;
    }

   bmp_tmp_data = (char *)malloc(offset - sizeof(bmp_head_t));
   if(NULL == bmp_tmp_data) {
       SAMPLE_ERR("ar_malloc bmp tmp fail\r\n");
       ret = -4;
       goto End_Mem;
   }

   if (offset < sizeof(bmp_head_t))
   {
       SAMPLE_ERR("wrong bmp header");
       ret = -5;
       goto End_Mem;
   }

   len = offset - sizeof(bmp_head_t);
   cnt = read(fd, bmp_tmp_data, len);
   if (cnt != len)
   {
       SAMPLE_ERR("read tmp data error (%ld, %ld)\r\n", cnt, len);
       ret = -6;
       goto End_Mem;
   }

   stride = AR_ALIGN4(bitCount/8 * (*width));
   len = stride * (*height);

   SAMPLE_PRT("bmp data stride: %d, len: %ld\r\n", stride, len);
   *vrt_addr = (char *)malloc(len);
   if(NULL == *vrt_addr) {
       SAMPLE_ERR("ar_malloc bmp data fail, len = %ld\r\n", len);
       ret = -7;
       goto End_Mem;
   }

   cnt = read(fd, *vrt_addr, len);
   if (cnt != len)
   {
       SAMPLE_ERR("read image data error (%ld, %ld)\r\n", cnt, len);
       ret = -8;
       free((unsigned char *)*vrt_addr);
       goto End_Mem;
   }

   ret = 0;

   End_Mem:
    if (info)
        free((unsigned char *)info);

    if (bmp_tmp_data)
        free((unsigned char *)bmp_tmp_data);

   End_Fd:
    close(fd);

   End:
    return ret;
}

AR_S32  src_fill(char* file_name,   ENUM_GE2D_FOMART format, STRU_ADDR  *addr)
{
    AR_S32    src_w = 0;
    AR_S32    src_h = 0;
    AR_S32    stride = 0;
    AR_S32    bitcount = 0;
    char     *bmp_vrt = NULL;
    AR_S32    ret = 0;
    uint8_t  *yuv_buffer = NULL;
    char     *p_src=(char *)addr->y_vrt;
    char     *p_src_u=(char *)addr->u_vrt;
    char     *p_src_v=(char *)addr->v_vrt;

    SAMPLE_PRT("format=%d", format);
    ret = bmp_image_data_get(file_name, &bmp_vrt,
                                &src_w, &src_h, &bitcount);
    if (ret != HAL_NO_ERROR)
    {
        SAMPLE_ERR("bmp_image_data_get failed ret = %d", ret);
        goto End;
    }

    if (src_h > 2160)
    {
        SAMPLE_ERR("bmp heigth is reach limit", ret);
        goto End;
    }

    yuv_buffer = (AR_U8 *)bmp_vrt;
    addr->w = src_w;
    addr->h = src_h;

    if(GE2D_FORMAT_ARGB4444==format)
    {
       addr->y_stride=stride=AR_ALIGN256(src_w*2);
       int line_buffer_len=AR_ALIGN4(src_w*3);
       for(int i=0;i<src_h;i++)
       {
          int k=0;
          for(int j=0;j<line_buffer_len;j+=3)
          {
              int r=yuv_buffer[line_buffer_len*i+j+0]>>4;
              int g=yuv_buffer[line_buffer_len*i+j+1]>>4;
              int b=yuv_buffer[line_buffer_len*i+j+2]>>4;
              int val=0xf<<12|b<<8|g<<4|r;
              uint16_t *p_src_l=(uint16_t *)&p_src[stride*i+k];
              *p_src_l=val;
              k+=2;
          }
       }
    }else if(GE2D_FORMAT_ARGB1555==format)
    {
       addr->y_stride=stride=AR_ALIGN256(src_w*2);
       int line_buffer_len=AR_ALIGN4(src_w*3);
       for(int i=0;i<src_h;i++)
       {
          int k=0;
          for(int j=0;j<line_buffer_len;j+=3)
          {
              int r=yuv_buffer[line_buffer_len*i+j+0]>>3;
              int g=yuv_buffer[line_buffer_len*i+j+1]>>3;
              int b=yuv_buffer[line_buffer_len*i+j+2]>>3;
              int val=0x1<<15|b<<10|g<<5|r;
              uint16_t *p_src_l=(uint16_t *)&p_src[stride*i+k];
              *p_src_l=val;
              k+=2;
          }
       }
    }else if(GE2D_FORMAT_NV12==format)
    {
       addr->y_stride=stride=AR_ALIGN256(src_w);
       addr->uv_stride=addr->y_stride;
       int line_buffer_len=AR_ALIGN4(src_w*3);
       rgb_t rgb;
       yuv_t yuv;
       for(int i=0;i<src_h;i++)
       {
          int k=0;
          for(int j=0;j<line_buffer_len;j+=3)
          {
              rgb.b=yuv_buffer[line_buffer_len*i+j+0];
              rgb.g=yuv_buffer[line_buffer_len*i+j+1];
              rgb.r=yuv_buffer[line_buffer_len*i+j+2];
              yuv=rgb2yuv(rgb);
              p_src[addr->y_stride*i+k]=yuv.y;
              k++;
          }
       }
       //uv
       int src_line=0;
       for(int i=0;i<src_h;i+=2)
       {
          int k=0;
          for(int j=0;j<line_buffer_len;j+=6)
          {
              rgb.b=yuv_buffer[line_buffer_len*i+j+0];
              rgb.g=yuv_buffer[line_buffer_len*i+j+1];
              rgb.r=yuv_buffer[line_buffer_len*i+j+2];
              yuv=rgb2yuv(rgb);
              p_src_u[addr->uv_stride*src_line+k]=yuv.u;
              p_src_u[addr->uv_stride*src_line+k+1]=yuv.v;
              k+=2;
          }
          src_line++;
       }

    }else if(GE2D_FORMAT_ARGB8888==format)
    {
       addr->y_stride=stride=AR_ALIGN256(src_w*4);
       int line_buffer_len=AR_ALIGN4(src_w*3);
       for(int i=0;i<src_h;i++)
       {
          int line_offset=line_buffer_len*i;
          int line_offset1=stride*i;
          int k=0;
          for(int j=0;j<line_buffer_len;j+=3)
          {
              p_src[line_offset1+k+0]=yuv_buffer[line_offset+j+0];
              p_src[line_offset1+k+1]=yuv_buffer[line_offset+j+1];
              p_src[line_offset1+k+2]=yuv_buffer[line_offset+j+2];
              p_src[line_offset1+k+3]=128;
              k+=4;
          }
       }
    }else if(GE2D_FORMAT_YV12==format)
    {
       addr->y_stride=stride=AR_ALIGN256(src_w);
       addr->uv_stride=addr->y_stride/2;
       int line_buffer_len=AR_ALIGN4(src_w*3);
       rgb_t rgb;
       yuv_t yuv;
       for(int i=0;i<src_h;i++)
       {
          int k=0;
          for(int j=0;j<line_buffer_len;j+=3)
          {
              rgb.b=yuv_buffer[line_buffer_len*i+j+0];
              rgb.g=yuv_buffer[line_buffer_len*i+j+1];
              rgb.r=yuv_buffer[line_buffer_len*i+j+2];
              yuv=rgb2yuv(rgb);
              p_src[addr->y_stride*i+k]=yuv.y;
              k++;
          }
       }
       //uv
       int src_line=0;
       for(int i=0;i<src_h;i+=2)
       {
          int k=0;
          for(int j=0;j<line_buffer_len;j+=6)
          {
              rgb.b=yuv_buffer[line_buffer_len*i+j+0];
              rgb.g=yuv_buffer[line_buffer_len*i+j+1];
              rgb.r=yuv_buffer[line_buffer_len*i+j+2];
              yuv=rgb2yuv(rgb);
              p_src_u[addr->uv_stride*src_line+k]=yuv.u;
              p_src_v[addr->uv_stride*src_line+k]=yuv.v;
              k++;
          }
          src_line++;
       }

    }else if(GE2D_FORMAT_MONOCHROMA==format)
    {
       addr->y_stride=stride=AR_ALIGN256(src_w);
       int line_buffer_len=AR_ALIGN4(src_w*3);
       rgb_t rgb;
       yuv_t yuv;
       for(int i=0;i<src_h;i++)
       {
          int k=0;
          for(int j=0;j<line_buffer_len;j+=3)
          {
              rgb.b=yuv_buffer[line_buffer_len*i+j+0];
              rgb.g=yuv_buffer[line_buffer_len*i+j+1];
              rgb.r=yuv_buffer[line_buffer_len*i+j+2];
              yuv=rgb2yuv(rgb);
              p_src[addr->y_stride*i+k]=yuv.y;
              k++;
          }
       }
    }
    else if(GE2D_FORMAT_GRAY1==format)
        {
        addr->y_stride=stride=AR_ALIGN256(src_w);
        addr->uv_stride=addr->y_stride/2;
        int line_buffer_len=AR_ALIGN4(src_w*3);
        rgb_t rgb;
        for(int i=0;i<src_h;i++)
        {
          int k=0;
          for(int j=0;j<line_buffer_len;j+=3)
          {
              rgb.b=yuv_buffer[line_buffer_len*i+j+0];
              rgb.g=yuv_buffer[line_buffer_len*i+j+1];
              rgb.r=yuv_buffer[line_buffer_len*i+j+2];

              if ((rgb.r == 0xff) && (rgb.g == 0xff) && (rgb.b == 0xff))
              {
                  CLR_REG_BITS(p_src[(addr->y_stride*i)+k/8],  7-(k%8));
              }
              else
              {
                  SET_REG_BITS(p_src[(addr->y_stride*i)+k/8],  7-(k%8));
              }
              k++;
          }
        }
    }
    else
    {
      SAMPLE_ERR("not supported format now");
    }

   End:
    if (bmp_vrt)
        free((unsigned char*)bmp_vrt);

    return ret;
}

AR_VOID SAMPLE_AR_MPI_VIN_OpenDev(AR_S32 mode)
{
   VI_DEV_PROP_S Prop={0};
   if(mode==0){
      Prop.cam_mode=VIN_CAMERA_NORMAL;
   }else if(mode==1)
   {
      Prop.cam_mode=VIN_CAMERA_OFFLINE;
   }else if(mode==2)
   {
      Prop.cam_mode=VIN_CMAERA_MULTI_MODE;
   }else
   {
      Prop.cam_mode=VIN_CAMERA_OFFLINE;
   }
   //cfg the fre
   Prop.hdr_fre_mod=1;
   Prop.hdr_fre_hz=100000000;
   Prop.vif_fre_mod=1;
   Prop.vif_fre_hz=100000000;
   Prop.isp_fre_mod=1;
   Prop.isp_fre_hz=100000000;
   Prop.mipi_fre_mod=1;
   Prop.mipi_fre_hz=100000000;

   AR_MPI_VIN_OpenDev(&Prop);
   return;
}

void SAMPLE_VGS_HandleSig(AR_S32 signo)
{
    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);

    if (SIGINT == signo || SIGTERM == signo)
    {
        is_running = AR_FALSE;
        SAMPLE_ERR("\033[0;31mprogram termination abnormally!\033[0;39m\n");
    }
}

void region_video_print(VIDEO_FRAME_INFO_S *p_frame_info)
{
    printf("width=%d height=%d stride_y=%d stride_u=%d stride_v=%d\r\n",
                p_frame_info->stVFrame.u32Width, p_frame_info->stVFrame.u32Height,
                p_frame_info->stVFrame.u32Stride[0],
                p_frame_info->stVFrame.u32Stride[1],
                p_frame_info->stVFrame.u32Stride[2]);
    printf("phy_y=0x%llx phy_u=0x%llx phy_v=0x%llx vrt_y=0x%llx vrt_u=0x%llx vrt_v=0x%llx\r\n",
                p_frame_info->stVFrame.u64PhyAddr[0],
                p_frame_info->stVFrame.u64PhyAddr[1],
                p_frame_info->stVFrame.u64PhyAddr[2],
                p_frame_info->stVFrame.u64VirAddr[0],
                p_frame_info->stVFrame.u64VirAddr[1],
                p_frame_info->stVFrame.u64VirAddr[2]);
}

int SAMPLE_VGS_Env_Init(void)
{
    vgs_lock = ar_creat_lock();
    if (!vgs_lock)
    {
        printf("create vgs lock failed\r\n");
        return -1;
    }

    vgs_signal = ar_create_signal_binary();
    if (!vgs_signal)
    {
        printf("create vgs signal failed\r\n");
        return -1;
    }

    return 0;
}

int SAMPLE_VGS_Env_DeInit(void)
{
    if (!vgs_lock)
    {
        printf("error vgs lock\r\n");
        return -1;
    }

    if (!vgs_signal)
    {
        printf("error vgs signal\r\n");
        return -1;
    }

    ar_delete_lock(vgs_lock);
    ar_delete_signal_binary(vgs_signal);
    return 0;
}

void SAMPLE_VGS_RetentionProc(void)
{
    static AR_BOOL LastState = FALSE;

    ar_lock(vgs_lock);
    if (LastState != is_suspend)
    {
        printf("flag change %d to %d\r\n", LastState, is_suspend);
        ar_signal_binary(vgs_signal);
        LastState = is_suspend;
    }
    ar_unlock(vgs_lock);

    if (LastState)
    {
        sleep(1);
    }
}


AR_S32 SAMPLE_VGS_Start_Draw_Test(int test_case, AR_CHAR *spFilename)
{
    AR_S32             s32Ret;

    AR_S32             s32ViCnt       = 1;
    VI_DEV             ViDev          = 0;
    VI_PIPE            ViPipe         = 0;
    VI_CHN             ViChn          = 0;
    AR_S32             s32WorkSnsId   = 0;
    SAMPLE_VI_CONFIG_S stViConfig;

    SIZE_S             stSize;
    VB_CONFIG_S        stVbConf;
    PIC_SIZE_E         enPicSize;
    AR_U32             u32BlkSize;

//    VO_CHN             VoChn          = 0;
    SAMPLE_VO_CONFIG_S stVoConfig;

    WDR_MODE_E         enWDRMode      = WDR_MODE_NONE;
    DYNAMIC_RANGE_E    enDynamicRange = DYNAMIC_RANGE_SDR8;
    PIXEL_FORMAT_E     enPixFormat    = PIXEL_FORMAT_YVU_PLANAR_420;
    VIDEO_FORMAT_E     enVideoFormat  = VIDEO_FORMAT_LINEAR;
    COMPRESS_MODE_E    enCompressMode = COMPRESS_MODE_NONE;

    VGS_HANDLE         job_id;
    VGS_TASK_ATTR_S    task_attr;

//    PIXEL_FORMAT_E     enPixelFmt = PIXEL_FORMAT_YVU_PLANAR_420;           /* Pixel format of osd */
    ENUM_GE2D_FOMART   e_format = GE2D_FORMAT_YV12;


    VIDEO_FRAME_INFO_S FrameInfo;
    AR_U32             u32BlkSize_org = 0;

    SAMPLE_VGS_Env_Init();


    /*config vi*/
   #if 1
    SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

    stViConfig.s32WorkingViNum                                   = s32ViCnt;
    stViConfig.as32WorkingViId[0]                                = 0;
    stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev         = ViDev;
    stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev           = ViDev;
    stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode       = enWDRMode;
    stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
    stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0]       = ViPipe;
    stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1]       = -1;
    stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2]       = -1;
    stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3]       = -1;
    stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn           = ViChn;
    stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat     = enPixFormat;
    stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange  = enDynamicRange;
    stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat   = enVideoFormat;
    stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode  = enCompressMode;

    /*get picture size*/
    s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_ERR("get picture size by sensor failed!\n");
        return s32Ret;
    }

    s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_ERR("get picture size failed!\n");
        return s32Ret;
    }
   #else
    stSize.u32Width = 3840;
    stSize.u32Height = 2160;
   #endif

    /*config vb*/
    ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
    stVbConf.u32MaxPoolCnt              = 2;

    SAMPLE_PRT("u32Width %d\n u32Height %d\n", stSize.u32Width, stSize.u32Height);

    u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 256);
    stVbConf.astCommPool[0].u64BlkSize  = u32BlkSize;
    stVbConf.astCommPool[0].u32BlkCnt   = 9;

    u32BlkSize_org = u32BlkSize;

    u32BlkSize *= 3;
    stVbConf.astCommPool[1].u64BlkSize  = u32BlkSize;
    stVbConf.astCommPool[1].u32BlkCnt   = 3;

    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_ERR("system init failed with %d!\n", s32Ret);
        return s32Ret;
    }

    SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
    stVoConfig.VoDev									= SAMPLE_VO_DEV_DHD0;
    stVoConfig.enVoIntfType 							= VO_INTF_HDMI;
    stVoConfig.enIntfSync								= VO_OUTPUT_1080P60;
    stVoConfig.enPicSize								= enPicSize;
    stVoConfig.u32DisBufLen 							= 3;
    stVoConfig.enDstDynamicRange						= enDynamicRange;
    stVoConfig.enVoMode 								= VO_MODE_1MUX;

    s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);
    if (AR_SUCCESS != s32Ret)
    {
    	SAMPLE_ERR("SAMPLE_COMM_VO_StartVO failed with %d!\n", s32Ret);
    	goto EXIT;
    }

    /* prepare buffer */
    SAMPLE_PRT("prepare buffer! %d\n", u32BlkSize);
    s32Ret = buffer_get(u32BlkSize, &g_vo_obj.src_addr);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_ERR("src_init failed with %d!\n", s32Ret);
        while(1)
        {
            sleep(1);
        }
        goto EXIT_1;
    }

    s32Ret = src_fill(spFilename, e_format, &g_vo_obj.src_addr);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_ERR("src_fill failed with %d!\n", s32Ret);
        goto EXIT_1;
    }

    while(is_running)
    {
        SAMPLE_VGS_RetentionProc();

        //get frame buffer
        VB_BLK hBlkHdl;

        memset(&FrameInfo,0,sizeof(VIDEO_FRAME_INFO_S));

        FrameInfo.stVFrame.enCompressMode = COMPRESS_MODE_NONE;
        FrameInfo.stVFrame.enPixelFormat = PIXEL_FORMAT_YVU_PLANAR_420;
        FrameInfo.stVFrame.u32Width = stSize.u32Width;
        FrameInfo.stVFrame.u32Height = stSize.u32Height;
        FrameInfo.stVFrame.u32Stride[0] = AR_ALIGN256(stSize.u32Width);
        FrameInfo.stVFrame.u32Stride[1] = FrameInfo.stVFrame.u32Stride[0] >> 1;
        FrameInfo.stVFrame.u32Stride[2] = FrameInfo.stVFrame.u32Stride[0] >> 1;
        FrameInfo.stVFrame.u32TimeRef = 0;
        FrameInfo.stVFrame.u64PTS = 0;


        AR_U32 u32LumaSize = FrameInfo.stVFrame.u32Stride[0] * stSize.u32Height;
        AR_U32 u32ChromaSize =  u32LumaSize / 4;

       // printf("u32LumaSize %d,u32ChromaSize %d, u32BlkSize_org %d\r\n",  u32LumaSize,  u32ChromaSize, u32BlkSize_org);

        hBlkHdl = AR_MPI_VB_GetBlock(VB_INVALID_POOLID, u32BlkSize_org, NULL);
        if (hBlkHdl == VB_INVALID_HANDLE) {
            printf("get vb fail!!!\n");
            sleep(1);
            goto EXIT_1;
        }

        FrameInfo.u32PoolId = AR_MPI_VB_Handle2PoolId(hBlkHdl);
        FrameInfo.stVFrame.u64PhyAddr[0] = AR_MPI_VB_Handle2PhysAddr( hBlkHdl );
        FrameInfo.stVFrame.u64PhyAddr[1] = FrameInfo.stVFrame.u64PhyAddr[0] + u32LumaSize;
        FrameInfo.stVFrame.u64PhyAddr[2] = FrameInfo.stVFrame.u64PhyAddr[1] + u32ChromaSize;

        FrameInfo.stVFrame.u64VirAddr[0] = (AR_U64)AR_MPI_SYS_Mmap(FrameInfo.stVFrame.u64PhyAddr[0], u32BlkSize_org);
        FrameInfo.stVFrame.u64VirAddr[1] = (AR_U64)(FrameInfo.stVFrame.u64VirAddr[0]) + u32LumaSize;
        FrameInfo.stVFrame.u64VirAddr[2] = (AR_U64)(FrameInfo.stVFrame.u64VirAddr[1]) + u32ChromaSize;

        memset((void *)FrameInfo.stVFrame.u64VirAddr[0], 0, u32LumaSize);
        memset((void *)FrameInfo.stVFrame.u64VirAddr[1], 0, u32ChromaSize);
        memset((void *)FrameInfo.stVFrame.u64VirAddr[2], 0, u32ChromaSize);

       // region_video_print(&FrameInfo);

        AR_MPI_SYS_Munmap((void *)FrameInfo.stVFrame.u64VirAddr[0], u32BlkSize_org);

        /* VGS */
        s32Ret = AR_MPI_VGS_BeginJob(&job_id);
        if (AR_SUCCESS != s32Ret)
        {
            SAMPLE_ERR("AR_MPI_VGS_BeginJob failed with %d!\n", s32Ret);
            goto EXIT;
        }

        task_attr.stImgIn = FrameInfo;
        task_attr.stImgOut = FrameInfo;

        if (test_case == 0)
        {
            VGS_DRAW_LINE_S line_array[4];
            int             side_length = 128;
            int             x = 100;
            int             y = 100;

            for (int i=0; i<4; i++)
            {
                if (i%3)
                    line_array[i].stStartPoint.s32X = x + side_length;
                else
                    line_array[i].stStartPoint.s32X = x;

                line_array[i].stStartPoint.s32Y = y + i/2*side_length;
                line_array[i].u32Color = 0xff0000bb;
                line_array[i].u32Thick = 1;
            }

            for (int i=0; i<4; i++)
            {
                line_array[i].stEndPoint.s32X = line_array[(i+1)%4].stStartPoint.s32X;
                line_array[i].stEndPoint.s32Y = line_array[(i+1)%4].stStartPoint.s32Y;
            }

            s32Ret = AR_MPI_VGS_AddDrawLineTaskArray(job_id, &task_attr, line_array, 4);
            if (AR_SUCCESS != s32Ret)
            {
                SAMPLE_ERR("AR_MPI_VGS_AddDrawLineTask failed with %d!\n", s32Ret);
                goto EXIT;
            }
        }
        else if (test_case == 1)
        {
            /* add rotate  */
            task_attr.stImgIn.stVFrame.u32Width = g_vo_obj.src_addr.w;
            task_attr.stImgIn.stVFrame.u32Height = g_vo_obj.src_addr.h;
            task_attr.stImgIn.stVFrame.u32Stride[0] = g_vo_obj.src_addr.y_stride;
            task_attr.stImgIn.stVFrame.u64PhyAddr[0] = (AR_U64)g_vo_obj.src_addr.y_phy;
            task_attr.stImgIn.stVFrame.u32Stride[1] = g_vo_obj.src_addr.uv_stride;
            task_attr.stImgIn.stVFrame.u64PhyAddr[1] = (AR_U64)g_vo_obj.src_addr.u_phy;
            task_attr.stImgIn.stVFrame.u32Stride[2] = g_vo_obj.src_addr.uv_stride;
            task_attr.stImgIn.stVFrame.u64PhyAddr[2] = (AR_U64)g_vo_obj.src_addr.v_phy;
            task_attr.stImgIn.stVFrame.enPixelFormat=PIXEL_FORMAT_YVU_PLANAR_420;
            task_attr.stImgOut = FrameInfo;
            s32Ret = AR_MPI_VGS_AddRotationTask(job_id, &task_attr, ROTATION_90);
            if (AR_SUCCESS != s32Ret)
            {
                SAMPLE_ERR("AR_MPI_VGS_AddOsdTask failed with 0x%x!\n", s32Ret);
                goto EXIT;
            }
        }
        else if (test_case == 2)
        {
            /* add rotate  */
            task_attr.stImgIn.stVFrame.u32Width = g_vo_obj.src_addr.w;
            task_attr.stImgIn.stVFrame.u32Height = g_vo_obj.src_addr.h;
            task_attr.stImgIn.stVFrame.u32Stride[0] = g_vo_obj.src_addr.y_stride;
            task_attr.stImgIn.stVFrame.u64PhyAddr[0] = (AR_U64)g_vo_obj.src_addr.y_phy;
            task_attr.stImgIn.stVFrame.u32Stride[1] = g_vo_obj.src_addr.uv_stride;
            task_attr.stImgIn.stVFrame.u64PhyAddr[1] = (AR_U64)g_vo_obj.src_addr.u_phy;
            task_attr.stImgIn.stVFrame.u32Stride[2] = g_vo_obj.src_addr.uv_stride;
            task_attr.stImgIn.stVFrame.u64PhyAddr[2] = (AR_U64)g_vo_obj.src_addr.v_phy;
            task_attr.stImgIn.stVFrame.enPixelFormat=PIXEL_FORMAT_YVU_PLANAR_420;
            task_attr.stImgOut = FrameInfo;
            s32Ret = AR_MPI_VGS_AddRotationTask(job_id, &task_attr, ROTATION_180);
            if (AR_SUCCESS != s32Ret)
            {
                SAMPLE_ERR("AR_MPI_VGS_AddOsdTask failed with 0x%x!\n", s32Ret);
                goto EXIT;
            }
        }
        else if (test_case == 3)
        {
            /* add scale  */
            task_attr.stImgIn.stVFrame.u32Width = g_vo_obj.src_addr.w;
            task_attr.stImgIn.stVFrame.u32Height = g_vo_obj.src_addr.h;
            task_attr.stImgIn.stVFrame.u32Stride[0] = g_vo_obj.src_addr.y_stride;
            task_attr.stImgIn.stVFrame.u64PhyAddr[0] = (AR_U64)g_vo_obj.src_addr.y_phy;
            task_attr.stImgIn.stVFrame.u32Stride[1] = g_vo_obj.src_addr.uv_stride;
            task_attr.stImgIn.stVFrame.u64PhyAddr[1] = (AR_U64)g_vo_obj.src_addr.u_phy;
            task_attr.stImgIn.stVFrame.u32Stride[2] = g_vo_obj.src_addr.uv_stride;
            task_attr.stImgIn.stVFrame.u64PhyAddr[2] = (AR_U64)g_vo_obj.src_addr.v_phy;
            task_attr.stImgIn.stVFrame.enPixelFormat=PIXEL_FORMAT_YVU_PLANAR_420;

            task_attr.stImgOut = FrameInfo;
            task_attr.stImgOut.stVFrame.u32Width = 1920;
            task_attr.stImgOut.stVFrame.u32Height = 1080;

            s32Ret = AR_MPI_VGS_AddScaleTask(job_id, &task_attr, VGS_SCLCOEF_NORMAL);
            if (AR_SUCCESS != s32Ret)
            {
                SAMPLE_ERR("AR_MPI_VGS_AddOsdTask failed with 0x%x!\n", s32Ret);
                goto EXIT;
            }
        }

        s32Ret = AR_MPI_VGS_EndJob(job_id);
        if (AR_SUCCESS != s32Ret)
        {
            SAMPLE_ERR("AR_MPI_VGS_EndJob failed with %d!\n", s32Ret);
            goto EXIT;
        }

        AR_MPI_VO_SendFrame(stVoConfig.VoDev, 0, &FrameInfo, -1);
        AR_MPI_VB_ReleaseBlock(hBlkHdl);

    }

EXIT_1:
    if (g_vo_obj.src_addr.y_vrt)
        buffer_release(&g_vo_obj.src_addr);

    SAMPLE_COMM_VO_StopVO(&stVoConfig);

EXIT:
    SAMPLE_COMM_SYS_Exit();
    SAMPLE_VGS_Env_DeInit();
    return s32Ret;
}

AR_S32 SAMPLE_VGS_Start(AR_CHAR *spFilename)
{
    AR_S32             s32Ret;

    AR_S32             s32ViCnt       = 1;
    VI_DEV             ViDev          = 0;
    VI_PIPE            ViPipe         = 0;
    VI_CHN             ViChn          = 0;
    AR_S32             s32WorkSnsId   = 0;
    SAMPLE_VI_CONFIG_S stViConfig;

    SIZE_S             stSize;
    VB_CONFIG_S        stVbConf;
    PIC_SIZE_E         enPicSize;
    AR_U32             u32BlkSize;

//    VO_CHN             VoChn          = 0;
    SAMPLE_VO_CONFIG_S stVoConfig;

    WDR_MODE_E         enWDRMode      = WDR_MODE_NONE;
    DYNAMIC_RANGE_E    enDynamicRange = DYNAMIC_RANGE_SDR8;
    PIXEL_FORMAT_E     enPixFormat    = PIXEL_FORMAT_YVU_PLANAR_420;
    VIDEO_FORMAT_E     enVideoFormat  = VIDEO_FORMAT_LINEAR;
    COMPRESS_MODE_E    enCompressMode = COMPRESS_MODE_NONE;

    VGS_HANDLE         job_id;
    VGS_TASK_ATTR_S    task_attr;

    PIXEL_FORMAT_E     enPixelFmt = PIXEL_FORMAT_ARGB_8888;           /* Pixel format of osd */
    ENUM_GE2D_FOMART   e_format = GE2D_FORMAT_ARGB8888;


    VIDEO_FRAME_INFO_S FrameInfo;
    AR_U32             u32BlkSize_org = 0;

    uint64_t cnt = 0;

    SAMPLE_VGS_Env_Init();

    /*config vi*/
    SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

    stViConfig.s32WorkingViNum                                   = s32ViCnt;
    stViConfig.as32WorkingViId[0]                                = 0;
    stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev         = ViDev;
    stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev           = ViDev;
    stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode       = enWDRMode;
    stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
    stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0]       = ViPipe;
    stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1]       = -1;
    stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2]       = -1;
    stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3]       = -1;
    stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn           = ViChn;
    stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat     = enPixFormat;
    stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange  = enDynamicRange;
    stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat   = enVideoFormat;
    stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode  = enCompressMode;

    /*get picture size*/
    s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_ERR("get picture size by sensor failed!\n");
        return s32Ret;
    }

    s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_ERR("get picture size failed!\n");
        return s32Ret;
    }

    /*config vb*/
    ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
    stVbConf.u32MaxPoolCnt              = 2;

    SAMPLE_PRT("u32Width %d\n u32Height %d\n", stSize.u32Width, stSize.u32Height);

    u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 256);
    stVbConf.astCommPool[0].u64BlkSize  = u32BlkSize;
    stVbConf.astCommPool[0].u32BlkCnt   = 10;

    u32BlkSize_org = u32BlkSize;

    u32BlkSize *= 3;
    stVbConf.astCommPool[1].u64BlkSize  = u32BlkSize;
    stVbConf.astCommPool[1].u32BlkCnt   = 12;

    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_ERR("system init failed with %d!\n", s32Ret);
        return s32Ret;
    }

    SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
    stVoConfig.VoDev									= SAMPLE_VO_DEV_DHD0;
    stVoConfig.enVoIntfType 							= VO_INTF_HDMI;
    stVoConfig.enIntfSync								= VO_OUTPUT_1080P60;
    stVoConfig.enPicSize								= enPicSize;
    stVoConfig.u32DisBufLen 							= 3;
    stVoConfig.enDstDynamicRange						= enDynamicRange;
    stVoConfig.enVoMode 								= VO_MODE_1MUX;

    s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);
    if (AR_SUCCESS != s32Ret)
    {
    	SAMPLE_ERR("SAMPLE_COMM_VO_StartVO failed with %d!\n", s32Ret);
    	goto EXIT;
    }

    /* prepare buffer */
    SAMPLE_PRT("prepare buffer! %d\n", u32BlkSize);
    s32Ret = buffer_get(u32BlkSize, &g_vo_obj.src_addr);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_ERR("src_init failed with %d!\n", s32Ret);
        goto EXIT_1;
    }

    s32Ret = src_fill(spFilename, e_format, &g_vo_obj.src_addr);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_ERR("src_fill failed with %d!\n", s32Ret);
        goto EXIT_1;
    }

    while(is_running)
    {

        //get frame buffer
        VB_BLK hBlkHdl;

        memset(&FrameInfo,0,sizeof(VIDEO_FRAME_INFO_S));

        FrameInfo.stVFrame.enCompressMode = COMPRESS_MODE_NONE;
        FrameInfo.stVFrame.enPixelFormat = PIXEL_FORMAT_YVU_PLANAR_420;
        FrameInfo.stVFrame.u32Width = stSize.u32Width;
        FrameInfo.stVFrame.u32Height = stSize.u32Height;
        FrameInfo.stVFrame.u32Stride[0] = AR_ALIGN256(stSize.u32Width);
        FrameInfo.stVFrame.u32Stride[1] = FrameInfo.stVFrame.u32Stride[0] >> 1;
        FrameInfo.stVFrame.u32Stride[2] = FrameInfo.stVFrame.u32Stride[0] >> 1;
        FrameInfo.stVFrame.u32TimeRef = 0;
        FrameInfo.stVFrame.u64PTS = 0;


        AR_U32 u32LumaSize = FrameInfo.stVFrame.u32Stride[0] * stSize.u32Height;
        AR_U32 u32ChromaSize =  u32LumaSize / 4;

       // printf("u32LumaSize %d,u32ChromaSize %d, u32BlkSize_org %d\r\n",  u32LumaSize,  u32ChromaSize, u32BlkSize_org);

        hBlkHdl = AR_MPI_VB_GetBlock(VB_INVALID_POOLID, u32BlkSize_org, NULL);
        if (hBlkHdl == VB_INVALID_HANDLE) {
            printf("get vb fail!!!\n");
            sleep(1);
            goto EXIT_1;
        }

        FrameInfo.u32PoolId = AR_MPI_VB_Handle2PoolId(hBlkHdl);
        FrameInfo.stVFrame.u64PhyAddr[0] = AR_MPI_VB_Handle2PhysAddr( hBlkHdl );
        FrameInfo.stVFrame.u64PhyAddr[1] = FrameInfo.stVFrame.u64PhyAddr[0] + u32LumaSize;
        FrameInfo.stVFrame.u64PhyAddr[2] = FrameInfo.stVFrame.u64PhyAddr[1] + u32ChromaSize;

        FrameInfo.stVFrame.u64VirAddr[0] = (AR_U64)AR_MPI_SYS_Mmap(FrameInfo.stVFrame.u64PhyAddr[0], u32BlkSize_org);
        FrameInfo.stVFrame.u64VirAddr[1] = (AR_U64)(FrameInfo.stVFrame.u64VirAddr[0]) + u32LumaSize;
        FrameInfo.stVFrame.u64VirAddr[2] = (AR_U64)(FrameInfo.stVFrame.u64VirAddr[1]) + u32ChromaSize;

        memset((void *)FrameInfo.stVFrame.u64VirAddr[0], 0, u32LumaSize);
        memset((void *)FrameInfo.stVFrame.u64VirAddr[1], 0, u32ChromaSize);
        memset((void *)FrameInfo.stVFrame.u64VirAddr[2], 0, u32ChromaSize);

       // region_video_print(&FrameInfo);

        AR_MPI_SYS_Munmap((void *)FrameInfo.stVFrame.u64VirAddr[0], u32BlkSize_org);

        /* VGS */
        s32Ret = AR_MPI_VGS_BeginJob(&job_id);
        if (AR_SUCCESS != s32Ret)
        {
            SAMPLE_ERR("AR_MPI_VGS_BeginJob failed with %d!\n", s32Ret);
            goto EXIT;
        }

        task_attr.stImgIn = FrameInfo;
        task_attr.stImgOut = FrameInfo;

        /* add osd  */
        VGS_ADD_OSD_S  osd;
        osd.stRect.s32X = 300;
        osd.stRect.s32Y = 300;
        osd.stRect.u32Width = g_vo_obj.src_addr.w;
        osd.stRect.u32Height = g_vo_obj.src_addr.h;
        osd.u32BgColor = 0xffffffff;
        osd.enPixelFmt = enPixelFmt;
        osd.u64PhyAddr = (AR_U64)g_vo_obj.src_addr.y_phy;
        osd.u32Stride  = g_vo_obj.src_addr.y_stride;
        osd.u32BgAlpha = 80;
        osd.u32FgAlpha = 80;
        osd.bOsdRevert = 0;
        s32Ret = AR_MPI_VGS_AddOsdTask(job_id, &task_attr, &osd);
        if (AR_SUCCESS != s32Ret)
        {
            SAMPLE_ERR("AR_MPI_VGS_AddOsdTask failed with 0x%x!\n", s32Ret);
            goto EXIT;
        }

        s32Ret = AR_MPI_VGS_EndJob(job_id);

        cnt++;
        if (AR_SUCCESS != s32Ret)
        {
         SAMPLE_ERR("AR_MPI_VGS_EndJob failed with %d!\n", s32Ret);
         goto EXIT;
        }
#if 1
        AR_MPI_VO_SendFrame(stVoConfig.VoDev, 0, &FrameInfo, -1);
#endif
        AR_MPI_VB_ReleaseBlock(hBlkHdl);

    }

EXIT_1:
    if (g_vo_obj.src_addr.y_vrt)
        buffer_release(&g_vo_obj.src_addr);

    SAMPLE_COMM_VO_StopVO(&stVoConfig);

EXIT:
    SAMPLE_COMM_SYS_Exit();
    SAMPLE_VGS_Env_DeInit();

    return s32Ret;
}

AR_S32 SAMPLE_VGS_Start_With_VI(AR_CHAR *spFilename)
{
    AR_S32             s32Ret;

    AR_S32             s32ViCnt       = 1;
    VI_DEV             ViDev          = 0;
    VI_PIPE            ViPipe         = 0;
    VI_CHN             ViChn          = 0;
    AR_S32             s32WorkSnsId   = 0;
    SAMPLE_VI_CONFIG_S stViConfig;

    SIZE_S             stSize;
    VB_CONFIG_S        stVbConf;
    PIC_SIZE_E         enPicSize;
    AR_U32             u32BlkSize;

//    VO_CHN             VoChn          = 0;
    SAMPLE_VO_CONFIG_S stVoConfig;

    WDR_MODE_E         enWDRMode      = WDR_MODE_NONE;
    DYNAMIC_RANGE_E    enDynamicRange = DYNAMIC_RANGE_SDR8;
    PIXEL_FORMAT_E     enPixFormat    = PIXEL_FORMAT_YVU_PLANAR_420;
    VIDEO_FORMAT_E     enVideoFormat  = VIDEO_FORMAT_LINEAR;
    COMPRESS_MODE_E    enCompressMode = COMPRESS_MODE_NONE;

    VGS_HANDLE         job_id;
    VGS_TASK_ATTR_S    task_attr = {{{0}}};
    VGS_DRAW_LINE_S    draw_line={{0}};

    PIXEL_FORMAT_E     enPixelFmt = PIXEL_FORMAT_ARGB_8888;           /* Pixel format of osd */
    ENUM_GE2D_FOMART   e_format = GE2D_FORMAT_ARGB8888;


    VIDEO_FRAME_INFO_S FrameInfo;

    SAMPLE_VGS_Env_Init();

    /*config vi*/
    SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

    stViConfig.s32WorkingViNum                                   = s32ViCnt;
    stViConfig.as32WorkingViId[0]                                = 0;
    stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev         = ViDev;
    stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev           = ViDev;
    stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode       = enWDRMode;
    stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
    stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0]       = ViPipe;
    stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1]       = -1;
    stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2]       = -1;
    stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3]       = -1;
    stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn           = ViChn;
    stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat     = enPixFormat;
    stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange  = enDynamicRange;
    stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat   = enVideoFormat;
    stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode  = enCompressMode;

    /*get picture size*/
    s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_ERR("get picture size by sensor failed!\n");
        return s32Ret;
    }

    s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_ERR("get picture size failed!\n");
        return s32Ret;
    }

    /*config vb*/
    ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
    stVbConf.u32MaxPoolCnt              = 2;

    SAMPLE_PRT("u32Width %d\n u32Height %d\n", stSize.u32Width, stSize.u32Height);

    u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 256);
    stVbConf.astCommPool[0].u64BlkSize  = u32BlkSize;
    stVbConf.astCommPool[0].u32BlkCnt   = 10;

    u32BlkSize *= 3;
    stVbConf.astCommPool[1].u64BlkSize  = u32BlkSize;
    stVbConf.astCommPool[1].u32BlkCnt   = 12;

    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_ERR("system init failed with %d!\n", s32Ret);
        return s32Ret;
    }

    SAMPLE_AR_MPI_VIN_OpenDev(0);

    /*start vi*/
    s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_ERR("start vi failed.s32Ret:0x%x !\n", s32Ret);
        goto EXIT;
    }

    SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
    stVoConfig.VoDev									= SAMPLE_VO_DEV_DHD0;
    stVoConfig.enVoIntfType 							= VO_INTF_HDMI;
    stVoConfig.enIntfSync								= VO_OUTPUT_1080P60;
    stVoConfig.enPicSize								= enPicSize;
    stVoConfig.u32DisBufLen 							= 3;
    stVoConfig.enDstDynamicRange						= enDynamicRange;
    stVoConfig.enVoMode 								= VO_MODE_1MUX;

    s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);
    if (AR_SUCCESS != s32Ret)
    {
    	SAMPLE_ERR("SAMPLE_COMM_VO_StartVO failed with %d!\n", s32Ret);
    	goto EXIT;
    }

    /* prepare buffer */
    SAMPLE_PRT("prepare buffer! %d\n", u32BlkSize);
    s32Ret = buffer_get(u32BlkSize, &g_vo_obj.src_addr);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_ERR("src_init failed with %d!\n", s32Ret);
        goto EXIT_1;
    }

    s32Ret = src_fill(spFilename, e_format, &g_vo_obj.src_addr);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_ERR("src_fill failed with %d!\n", s32Ret);
        goto EXIT_1;
    }

    s32Ret = buffer_get(u32BlkSize, &g_vo_obj.yuv_addr);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_ERR("yuv_init failed with %d!\n", s32Ret);
        goto EXIT_1;
    }

    s32Ret = src_fill(spFilename, GE2D_FORMAT_YV12, &g_vo_obj.yuv_addr);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_ERR("src_fill failed with %d!\n", s32Ret);
        goto EXIT_1;
    }

    s32Ret = buffer_get(u32BlkSize, &g_vo_obj.gray1_addr);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_ERR("yuv_init failed with %d!\n", s32Ret);
        goto EXIT_1;
    }

    s32Ret = src_fill(spFilename, GE2D_FORMAT_GRAY1, &g_vo_obj.gray1_addr);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_ERR("src_fill failed with %d!\n", s32Ret);
        goto EXIT_1;
    }

    s32Ret = buffer_get(u32BlkSize, &g_vo_obj.tmp_addr);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_ERR("tmp buffer init failed with %d!\n", s32Ret);
        goto EXIT_1;
    }

    g_vo_obj.tmp_addr.w = 512;
    g_vo_obj.tmp_addr.h = 320 ;
    g_vo_obj.tmp_addr.y_stride = 512;
    g_vo_obj.tmp_addr.uv_stride = 256;

    while(is_running)
    {
        //get frame buffer
        s32Ret = AR_MPI_VI_GetChnFrame(ViPipe,ViChn,&FrameInfo,500000);
        if (AR_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("AR_MPI_VI_GetChnFrame failed with %d!\n", s32Ret);
            continue;
        }

       // region_video_print(&FrameInfo);

#if 1
        /* VGS */
        s32Ret = AR_MPI_VGS_BeginJob(&job_id);
        if (AR_SUCCESS != s32Ret)
        {
            SAMPLE_ERR("AR_MPI_VGS_BeginJob failed with %d!\n", s32Ret);
            goto EXIT;
        }

        task_attr.stImgIn = FrameInfo;
        task_attr.stImgOut = FrameInfo;

        /* draw line */
        draw_line.stStartPoint.s32X = 30;
        draw_line.stStartPoint.s32Y = 30;
        draw_line.stEndPoint.s32X = 1024;
        draw_line.stEndPoint.s32Y = 30;
        draw_line.u32Color = 0xffff0000;
        draw_line.u32Thick = 10;
        s32Ret = AR_MPI_VGS_AddDrawLineTask(job_id, &task_attr, &draw_line);
        if (AR_SUCCESS != s32Ret)
        {
            SAMPLE_ERR("AR_MPI_VGS_AddDrawLineTask failed with %d!\n", s32Ret);
            goto EXIT;
        }

        VGS_DRAW_LINE_S   line_array[2];

        memset(line_array, 0, sizeof(VGS_DRAW_LINE_S));
        line_array[0].stStartPoint.s32X = 30;
        line_array[0].stStartPoint.s32Y = 100;
        line_array[0].stEndPoint.s32X = 1024;
        line_array[0].stEndPoint.s32Y = 100;
        line_array[0].u32Color = 0xff00ff00;
        line_array[0].u32Thick = 10;

        line_array[1].stStartPoint.s32X = 100;
        line_array[1].stStartPoint.s32Y = 100;
        line_array[1].stEndPoint.s32X = 1024;
        line_array[1].stEndPoint.s32Y = 1024;
        line_array[1].u32Color = 0xff0000ff;
        line_array[1].u32Thick = 10;

        s32Ret = AR_MPI_VGS_AddDrawLineTaskArray(job_id, &task_attr, line_array, 2);
        if (AR_SUCCESS != s32Ret)
        {
            SAMPLE_ERR("AR_MPI_VGS_AddDrawLineTask failed with %d!\n", s32Ret);
            goto EXIT;
        }

        /* draw cover */
        VGS_ADD_COVER_S  cover;

        memset(&cover, 0, sizeof(VGS_ADD_COVER_S));
        cover.enCoverType = COVER_RECT;
        cover.stDstRect.s32X = 0;
        cover.stDstRect.s32Y = 0;
        cover.stDstRect.u32Width=256;
        cover.stDstRect.u32Height=256;
        cover.u32Color = 0xffff0000;
        s32Ret = AR_MPI_VGS_AddCoverTask(job_id, &task_attr, &cover);
        if (AR_SUCCESS != s32Ret)
        {
            SAMPLE_ERR("AR_MPI_VGS_AddDrawLineTask failed with %d!\n", s32Ret);
            goto EXIT;
        }

        VGS_ADD_COVER_S  cover_array[2];

        memset(cover_array, 0, sizeof(VGS_ADD_COVER_S)*2);
        cover_array[0].enCoverType = COVER_RECT;
        cover_array[0].stDstRect.s32X = 800;
        cover_array[0].stDstRect.s32Y = 0;
        cover_array[0].stDstRect.u32Width=256;
        cover_array[0].stDstRect.u32Height=256;
        cover_array[0].u32Color = 0x00000000;

        cover_array[1].enCoverType = COVER_QUAD_RANGLE;
        cover_array[1].stQuadRangle.bSolid = AR_TRUE;
        cover_array[1].stQuadRangle.u32Thick = 3;
        cover_array[1].stQuadRangle.stPoint[0].s32X=800;
        cover_array[1].stQuadRangle.stPoint[0].s32Y=800;
        cover_array[1].stQuadRangle.stPoint[1].s32X=800+256;
        cover_array[1].stQuadRangle.stPoint[1].s32Y=800;
        cover_array[1].stQuadRangle.stPoint[2].s32X=800+256;
        cover_array[1].stQuadRangle.stPoint[2].s32Y=800+256;
        cover_array[1].stQuadRangle.stPoint[3].s32X=800;
        cover_array[1].stQuadRangle.stPoint[3].s32Y=800+256;
        cover_array[1].u32Color = 0x3f3f3f;

        s32Ret = AR_MPI_VGS_AddCoverTaskArray(job_id, &task_attr, cover_array, 2);
        if (AR_SUCCESS != s32Ret)
        {
            SAMPLE_ERR("AR_MPI_VGS_AddDrawLineTask failed with %d!\n", s32Ret);
            goto EXIT;
        }

        /* add osd  */
        VGS_ADD_OSD_S  osd;

        memset(&osd, 0, sizeof(VGS_ADD_OSD_S));
        osd.stRect.s32X = 300;
        osd.stRect.s32Y = 300;
        osd.stRect.u32Width = g_vo_obj.src_addr.w;
        osd.stRect.u32Height = g_vo_obj.src_addr.h;
        osd.u32BgColor = 0xffffffff;
        osd.enPixelFmt = enPixelFmt;
        osd.u64PhyAddr = (AR_U64)g_vo_obj.src_addr.y_phy;
        osd.u32Stride  = g_vo_obj.src_addr.y_stride;
        osd.u32BgAlpha = 80;
        osd.u32FgAlpha = 80;
        osd.bOsdRevert = 0;
        osd.u32CloseAlpha = 0;

        s32Ret = AR_MPI_VGS_AddOsdTask(job_id, &task_attr, &osd);
        if (AR_SUCCESS != s32Ret)
        {
            SAMPLE_ERR("AR_MPI_VGS_AddOsdTask failed with 0x%x!\n", s32Ret);
            goto EXIT;
        }

        VGS_ADD_OSD_S  osd_array[2];

        memset(osd_array, 0, sizeof(VGS_ADD_OSD_S)*2);
        osd_array[0].stRect.s32X = 700;
        osd_array[0].stRect.s32Y = 700;
        osd_array[0].stRect.u32Width = g_vo_obj.src_addr.w;
        osd_array[0].stRect.u32Height = g_vo_obj.src_addr.h;
        osd_array[0].u32BgColor = 0xffffffff;
        osd_array[0].enPixelFmt = enPixelFmt;
        osd_array[0].u64PhyAddr = (AR_U64)g_vo_obj.src_addr.y_phy;
        osd_array[0].u32Stride  = g_vo_obj.src_addr.y_stride;
        osd_array[0].u32BgAlpha = 80;
        osd_array[0].u32FgAlpha = 255;
        osd_array[0].bOsdRevert = 0;
        osd_array[0].u32CloseAlpha = 0;

        osd_array[1].stRect.s32X = 1000;
        osd_array[1].stRect.s32Y = 1000;
        osd_array[1].stRect.u32Width = g_vo_obj.gray1_addr.w;
        osd_array[1].stRect.u32Height = g_vo_obj.gray1_addr.h;
        osd_array[1].u32BgColor = 0xffffffff;
        osd_array[1].enPixelFmt = PIXEL_FORMAT_GRAY1;
        osd_array[1].u64PhyAddr = (AR_U64)g_vo_obj.gray1_addr.y_phy;
        osd_array[1].u32Stride  = g_vo_obj.gray1_addr.y_stride;
        osd_array[1].u32BgAlpha = 80;
        osd_array[1].u32FgAlpha = 255;
        osd_array[1].bOsdRevert = 1;
        osd_array[1].stOsdRevert.enColorRevertMode = VGS_COLOR_REVERT_ALPHA;
        osd_array[1].u16ColorLUT[0] = 0x0000;
        osd_array[1].u16ColorLUT[1] = 0xffff;
        osd_array[1].u32CloseAlpha = 0;

        s32Ret = AR_MPI_VGS_AddOsdTaskArray(job_id, &task_attr, osd_array, 2);
        if (AR_SUCCESS != s32Ret)
        {
            SAMPLE_ERR("AR_MPI_VGS_AddOsdTask failed with 0x%x!\n", s32Ret);
            goto EXIT;
        }

        /* add scale  */
        task_attr.stImgIn.stVFrame.u32Width = g_vo_obj.yuv_addr.w;
        task_attr.stImgIn.stVFrame.u32Height = g_vo_obj.yuv_addr.h;
        task_attr.stImgIn.stVFrame.u32Stride[0] = g_vo_obj.yuv_addr.y_stride;
        task_attr.stImgIn.stVFrame.u64PhyAddr[0] = (AR_U64)g_vo_obj.yuv_addr.y_phy;
        task_attr.stImgIn.stVFrame.u32Stride[1] = g_vo_obj.yuv_addr.uv_stride;
        task_attr.stImgIn.stVFrame.u64PhyAddr[1] = (AR_U64)g_vo_obj.yuv_addr.u_phy;
        task_attr.stImgIn.stVFrame.u32Stride[2] = g_vo_obj.yuv_addr.uv_stride;
        task_attr.stImgIn.stVFrame.u64PhyAddr[2] = (AR_U64)g_vo_obj.yuv_addr.v_phy;
        task_attr.stImgIn.stVFrame.enPixelFormat=PIXEL_FORMAT_YVU_PLANAR_420;

        task_attr.stImgOut.stVFrame.u32Width = g_vo_obj.tmp_addr.w;
        task_attr.stImgOut.stVFrame.u32Height = g_vo_obj.tmp_addr.h;
        task_attr.stImgOut.stVFrame.u32Stride[0] = g_vo_obj.tmp_addr.y_stride;
        task_attr.stImgOut.stVFrame.u64PhyAddr[0] = (AR_U64)g_vo_obj.tmp_addr.y_phy;
        task_attr.stImgOut.stVFrame.u32Stride[1] = g_vo_obj.tmp_addr.uv_stride;
        task_attr.stImgOut.stVFrame.u64PhyAddr[1] = (AR_U64)g_vo_obj.tmp_addr.u_phy;
        task_attr.stImgOut.stVFrame.u32Stride[2] = g_vo_obj.tmp_addr.uv_stride;
        task_attr.stImgOut.stVFrame.u64PhyAddr[2] = (AR_U64)g_vo_obj.tmp_addr.v_phy;
        task_attr.stImgOut.stVFrame.enPixelFormat=PIXEL_FORMAT_YVU_PLANAR_420;
        s32Ret = AR_MPI_VGS_AddScaleTask(job_id, &task_attr, VGS_SCLCOEF_NORMAL);
        if (AR_SUCCESS != s32Ret)
        {
            SAMPLE_ERR("AR_MPI_VGS_AddOsdTask failed with 0x%x!\n", s32Ret);
            goto EXIT;
        }

        /* add rotate  */
        VGS_ADD_ROTATION_S rot = {0};

        task_attr.stImgIn.stVFrame.u32Width = g_vo_obj.tmp_addr.w;
        task_attr.stImgIn.stVFrame.u32Height = g_vo_obj.tmp_addr.h;
        task_attr.stImgIn.stVFrame.u32Stride[0] = g_vo_obj.tmp_addr.y_stride;
        task_attr.stImgIn.stVFrame.u64PhyAddr[0] = (AR_U64)g_vo_obj.tmp_addr.y_phy;
        task_attr.stImgIn.stVFrame.u32Stride[1] = g_vo_obj.tmp_addr.uv_stride;
        task_attr.stImgIn.stVFrame.u64PhyAddr[1] = (AR_U64)g_vo_obj.tmp_addr.u_phy;
        task_attr.stImgIn.stVFrame.u32Stride[2] = g_vo_obj.tmp_addr.uv_stride;
        task_attr.stImgIn.stVFrame.u64PhyAddr[2] = (AR_U64)g_vo_obj.tmp_addr.v_phy;
        task_attr.stImgIn.stVFrame.enPixelFormat=PIXEL_FORMAT_YVU_PLANAR_420;
        task_attr.stImgOut = FrameInfo;

        rot.enAngle = ROTATION_90;
        rot.u32CloseAlpha = 0;

        s32Ret = AR_MPI_VGS_AddRotationTaskExt(job_id, &task_attr, &rot);
        if (AR_SUCCESS != s32Ret)
        {
            SAMPLE_ERR("AR_MPI_VGS_AddOsdTask failed with 0x%x!\n", s32Ret);
            goto EXIT;
        }

        s32Ret = AR_MPI_VGS_EndJob(job_id);

        if (AR_SUCCESS != s32Ret)
        {
         SAMPLE_ERR("AR_MPI_VGS_EndJob failed with %d!\n", s32Ret);
         goto EXIT;
        }
        #endif
        AR_MPI_VO_SendFrame(stVoConfig.VoDev, 0, &FrameInfo, -1);

        AR_MPI_VI_ReleaseChnFrame(ViPipe,ViChn,&FrameInfo);
    }

EXIT_1:
    if (g_vo_obj.src_addr.y_vrt)
        buffer_release(&g_vo_obj.src_addr);

    if (g_vo_obj.tmp_addr.y_vrt)
        buffer_release(&g_vo_obj.tmp_addr);

    if (g_vo_obj.yuv_addr.y_vrt)
        buffer_release(&g_vo_obj.yuv_addr);

    if (g_vo_obj.gray1_addr.y_vrt)
        buffer_release(&g_vo_obj.gray1_addr);

    SAMPLE_COMM_VO_StopVO(&stVoConfig);

    SAMPLE_COMM_VI_StopVi(&stViConfig);

EXIT:
    SAMPLE_COMM_SYS_Exit();
    SAMPLE_VGS_Env_DeInit();
    return s32Ret;
}

AR_S32 SAMPLE_VGS_Vo_Init(void)
{
    SAMPLE_VO_CONFIG_S stVoConfig;
    AR_S32             s32Ret;
    DYNAMIC_RANGE_E    enDynamicRange = DYNAMIC_RANGE_SDR8;
    PIC_SIZE_E         enPicSize=2048*1080*3/2;

    SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
    stVoConfig.VoDev									= SAMPLE_VO_DEV_DHD0;
    stVoConfig.enVoIntfType 							= VO_INTF_HDMI;
    stVoConfig.enIntfSync								= VO_OUTPUT_1080P60;
    stVoConfig.enPicSize								= enPicSize;
    stVoConfig.u32DisBufLen 							= 3;
    stVoConfig.enDstDynamicRange						= enDynamicRange;
    stVoConfig.enVoMode 								= VO_MODE_1MUX;

    s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);
    if (AR_SUCCESS != s32Ret)
    {
    	SAMPLE_ERR("SAMPLE_COMM_VO_StartVO failed with %d!\n", s32Ret);
    	return s32Ret;
    }

    return s32Ret;
}

AR_S32 SAMPLE_VGS_Vo_DeInit(void)
{
    SAMPLE_VO_CONFIG_S stVoConfig;
    AR_S32             s32Ret;
    DYNAMIC_RANGE_E    enDynamicRange = DYNAMIC_RANGE_SDR8;
    PIC_SIZE_E         enPicSize=2048*1080*3/2;

    SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
    stVoConfig.VoDev									= SAMPLE_VO_DEV_DHD0;
    stVoConfig.enVoIntfType 							= VO_INTF_HDMI;
    stVoConfig.enIntfSync								= VO_OUTPUT_1080P60;
    stVoConfig.enPicSize								= enPicSize;
    stVoConfig.u32DisBufLen 							= 3;
    stVoConfig.enDstDynamicRange						= enDynamicRange;
    stVoConfig.enVoMode 								= VO_MODE_1MUX;

    SAMPLE_COMM_VO_StopVO(&stVoConfig);

    return AR_SUCCESS;
}

void SAMPLE_VGS_Suspend(void)
{
    ar_lock(vgs_lock);
    is_suspend = TRUE;
    ar_unlock(vgs_lock);
    ar_signal_wait_binary(vgs_signal);
}

void SAMPLE_VGS_Rusume(void)
{
    ar_lock(vgs_lock);
    is_suspend = false;
    ar_unlock(vgs_lock);
    ar_signal_wait_binary(vgs_signal);
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
