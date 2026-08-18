#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>

#include "mpi_vpss.h"
#include "mpi_vo.h"
#include "mpi_venc.h"

#include "pf_vgs.h"
#include "cfg_vpss.h"
#include "util_timer.h"
#include "util_api.h"

static int vgsRun = 0;
static pthread_t vgsId ;
static int fp=0;
static char *data_virt_addr = NULL;
static unsigned long fb_phy_addr = 0;

static int bmp_image_data_get(char *file_name, char **vrt_addr,
                                        int *width, int *height, int *bitCnt)
{
    bmp_head_t *info = NULL;
    char *bmp_tmp_data = NULL;
    int ret;
    int bitCount, offset;
    ssize_t cnt;
    int stride = 0;

    printf("bmp_image_data_get\r\n");
    int fd = open(file_name, O_RDWR);
    if(fd <= 0) {
        printf("open img file fail\r\n");
        ret = -1;
        goto End;
    }

    size_t len = sizeof(bmp_head_t);
    info = (bmp_head_t *)malloc(len);
    if(NULL == info) {
        printf("dma malloc info fail\r\n");
        ret = -1;
        goto End_Fd;
    }

    cnt = read(fd, (char *)info, len);
    if(cnt != len) {
        printf("read bmp header error (%ld, %ld)\r\n", cnt, len);
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
    printf("bmp info width = %d, height = %d, offset = %d, bitCount = %d\r\n",
           *width, *height, offset, bitCount);

    if((24 != bitCount) && (32 != bitCount)){
        printf("format wrong, only support RGB888 format\r\n");
        ret = -3;
        goto End_Mem;
    }

   bmp_tmp_data = (char *)malloc(offset - sizeof(bmp_head_t));
   if(NULL == bmp_tmp_data) {
       printf("ar_malloc bmp tmp fail\r\n");
       ret = -4;
       goto End_Mem;
   }

   len = offset - sizeof(bmp_head_t);
   cnt = read(fd, bmp_tmp_data, len);
   if (cnt != len)
   {
       printf("read tmp data error (%ld, %ld)\r\n", cnt, len);
       ret = -5;
       goto End_Mem;
   }

   stride = AR_ALIGN4(bitCount/8 * (*width));
   len = stride * (*height);

   printf("bmp data stride: %d, len: %ld\r\n", stride, len);
   *vrt_addr = (char *)malloc(len);
   if(NULL == *vrt_addr) {
       printf("ar_malloc bmp data fail, len = %ld\r\n", len);
       ret = -6;
       goto End_Mem;
   }

   cnt = read(fd, *vrt_addr, len);
   if (cnt != len)
   {
       printf("read image data error (%ld, %ld)\r\n", cnt, len);
       ret = -7;
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

AR_S32 get_yuv_blk(VB_POOL pool_id, AR_U64 size, VB_BLK *y_blk,
                                VB_BLK *u_blk, VB_BLK *v_blk)
{
    if ((*y_blk = AR_MPI_VB_GetBlock(pool_id, size, NULL)) == AR_INVALID_HANDLE)
    {
        printf("Can't get vb!\n");
        return AR_FAILURE;
    }

    if ((*u_blk = AR_MPI_VB_GetBlock(pool_id, size, NULL)) == AR_INVALID_HANDLE)
    {
        printf("Can't get vb!\n");
        AR_MPI_VB_ReleaseBlock(*y_blk);
        return AR_FAILURE;
    }

    if ((*v_blk = AR_MPI_VB_GetBlock(pool_id, size, NULL)) == AR_INVALID_HANDLE)
    {
        printf("Can't get vb!\n");
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

    if ( (ret = get_yuv_blk(VB_INVALID_POOLID, blk_size, &addr->y_blk, &addr->u_blk, &addr->v_blk)) != AR_SUCCESS )
    {
        printf("[%s]%d: get yuv blk failed !!!!!!\n", __FILE__, __LINE__);
        return ret;
    }
    //printf("blk: %u, %u, %u\r\n", addr->y_blk, addr->u_blk, addr->v_blk);
    addr->y_phy = (AR_VOID *)AR_MPI_VB_Handle2PhysAddr(addr->y_blk);
    addr->u_phy = (AR_VOID *)AR_MPI_VB_Handle2PhysAddr(addr->u_blk);
    addr->v_phy = (AR_VOID *)AR_MPI_VB_Handle2PhysAddr(addr->v_blk);
    //printf("phy addr: %p, %p, %p\r\n", addr->y_phy, addr->u_phy, addr->v_phy);
    y_pool = AR_MPI_VB_Handle2PoolId(addr->y_blk);
    u_pool = AR_MPI_VB_Handle2PoolId(addr->u_blk);
    v_pool = AR_MPI_VB_Handle2PoolId(addr->v_blk);
    //printf("pool id: %u, %u, %u\r\n", y_pool, u_pool, v_pool);

    AR_MPI_VB_MmapPool(y_pool);
    AR_MPI_VB_MmapPool(u_pool);
    AR_MPI_VB_MmapPool(v_pool);

    ret = AR_MPI_VB_GetBlockVirAddr(y_pool, (AR_U64)addr->y_phy, &addr->y_vrt);
    if (ret != AR_SUCCESS)
    {
        printf("y_vrt_addr get failed: %d\r\n", ret);
        return ret;
    }

    ret = AR_MPI_VB_GetBlockVirAddr(u_pool, (AR_U64)addr->u_phy, &addr->u_vrt);
    if (ret != AR_SUCCESS)
    {
        printf("u_vrt_addr get failed: %d\r\n", ret);
        return ret;
    }

    ret = AR_MPI_VB_GetBlockVirAddr(v_pool, (AR_U64)addr->v_phy, &addr->v_vrt);
    if (ret != AR_SUCCESS)
    {
        printf("v_vrt_addr get failed: %d\r\n", ret);
        return ret;
    }

//    printf("y_addr(%p %p), u_addr(%p %p), v_addr(%p %p),\r\n",
//                                              addr->y_phy, addr->y_vrt,
//                                              addr->u_phy, addr->u_vrt,
//                                              addr->v_phy, addr->v_vrt);
    return AR_SUCCESS;
}

static void buffer_release(STRU_ADDR *addr)
{
    AR_MPI_VB_ReleaseBlock(addr->y_blk);
    AR_MPI_VB_ReleaseBlock(addr->u_blk);
    AR_MPI_VB_ReleaseBlock(addr->v_blk);
}


AR_S32  src_fill(char* src_data, int src_w, int src_h, ENUM_GE2D_FOMART format, STRU_ADDR  *addr)
{
    uint8_t  *yuv_buffer = NULL;
    AR_S32    stride = 0;
#if 0
    AR_S32    bitcount = 0;
    AR_S32    ret = 0;
    char     *p_src_u=(char *)addr->u_vrt;
    char     *p_src_v=(char *)addr->v_vrt;
#endif
    char     *p_src=(char *)addr->y_vrt;
    if(src_data == NULL || addr == NULL)
        return -1;
     yuv_buffer = (AR_U8 *)src_data;
    addr->w = src_w;
    addr->h = src_h;
    //printf("yuv_buffer: %p!!!!!!!!!!!!!!!!!!!!!!!!!!\n", yuv_buffer);

//if(GE2D_FORMAT_ARGB8888==format)
    {
       addr->y_stride = AR_ALIGN256(src_w*4);
#if 1
       memcpy(p_src, yuv_buffer, stride*src_h);
#else //test data
       int line_buffer_len=AR_ALIGN4(src_w*4);

       for(int i=0;i<src_h;i++)
       {
          int line_offset=line_buffer_len*i;
          int line_offset1=stride*i;
          int k=0;
          int a = 0;
          for(int j=0;j<line_buffer_len;j+=4)
          {
                p_src[line_offset1+k+0]=255;
                p_src[line_offset1+k+1]=0;
                p_src[line_offset1+k+2]=0;
                p_src[line_offset1+k+3]=70;
                k+=4;
          } 
       }
#endif

    }

    return 0;
}


int get_fb_addr (int *fb_w, int *fb_h, long *fb_size) 
{
     struct fb_var_screeninfo  vinfo;
     struct fb_fix_screeninfo  finfo;
     long screensize = 0;
     fp = open ("/dev/fb0",O_RDWR);
 
     if (fp < 0)
     {
          //printf("Error : Can not open framebuffer device!!!!\n");
          return -1;
     } else
        printf(" open framebuffer device success\n");
 
     if (ioctl(fp,FBIOGET_FSCREENINFO,&finfo))
     {
          printf("Error reading fixed information/n");
          close(fp);
          return -1;
     }
     fb_phy_addr = finfo.smem_start;
     if (ioctl(fp,FBIOGET_VSCREENINFO,&vinfo))
     {
          printf("Error reading variable information\n");
          close(fp);
          return -1;
     }

     *fb_w = vinfo.xres;
     *fb_h = vinfo.yres;
     screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
     *fb_size = screensize;
     /*这就是把fp所指的文件中从开始到screensize大小的内容给映射出来，得到一个指向这块空间的指针*/
     data_virt_addr =(char *) mmap (0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fp,0);
     if (data_virt_addr == MAP_FAILED)
     {
        printf ("Error: failed to map framebuffer device to memory.\n");
        close(fp);
        data_virt_addr = NULL;
        return -1;
     }else{
        printf("fb mmap success data_virt_addr: %p!!!!!!!!!!!!!!!!!!!!!!!!!!\n", data_virt_addr);
     }

     return 0;
}

void release_fb(char* fb_addr, long size){
    if(fb_addr != NULL)
        munmap (data_virt_addr, size); /*解除映射*/
    close (fp);    /*关闭文件*/
}

static void *IPC_MAIN_VGS_Thread(void *args)
{
    AR_S32 s32Ret = -1;

    VIDEO_FRAME_INFO_S FrameInfo;
    VGS_HANDLE         job_id = -1;
    VGS_TASK_ATTR_S    task_attr;

    int fb_w = 1920;
    int fb_h = 1080;
    long fb_size = fb_w * fb_h * 4;
    int fb_started = 0;
    
    IPC_CFG_VPSS_S * pVpssCfg = IPC_CFG_VPSS_GetParam();
    VPSS_GRP VpssGrp = 0;
    VPSS_CHN VpssChn = 0;
    AR_U32 u32ScreenW = pVpssCfg->stGrpCfg[VpssGrp].stChnCfg[VpssChn].u32Width;
    AR_U32 u32ScreenH = pVpssCfg->stGrpCfg[VpssGrp].stChnCfg[VpssChn].u32Height;

    
    PIXEL_FORMAT_E     enPixelFmt = PIXEL_FORMAT_ARGB_8888;           /* Pixel format of osd */
#ifdef USE_TEST_DATA
    ENUM_GE2D_FOMART   e_format = GE2D_FORMAT_ARGB8888;
    STRU_ADDR src_addr;
    AR_U32 u32BlkSize = fb_size;
    s32Ret = buffer_get(u32BlkSize, &src_addr);
    if (AR_SUCCESS != s32Ret)
    {
        printf("[%s]%d: src_init failed with %d!!!!!!!!!\n", __FILE__, __LINE__, s32Ret);
        goto EXIT;
    }

    char     *bmp_vrt = NULL;
    int bitcount = 32;
    s32Ret = bmp_image_data_get("./1080p_argb8888.bmp", &bmp_vrt,
                                &u32ScreenW, &u32ScreenH, &bitcount);
    if (s32Ret != AR_SUCCESS)
    {
        printf("bmp_image_data_get failed ret = %d", s32Ret);
        goto EXIT;
    }
    src_fill(bmp_vrt, u32ScreenW, u32ScreenH, e_format, &src_addr);
    data_virt_addr = src_addr.y_vrt;
#endif
#ifdef ENABLE_TIME_MEASURE
    AR_U32 start_time = 0;
    AR_U32 end_start = 0;
    AR_U32 end_end = 0;
#endif
    AR_U32 dump_data_cnt = 0;
    while(vgsRun)
    {
        s32Ret = AR_MPI_VPSS_GetChnFrame(VpssGrp, VpssChn, &FrameInfo, 500000);
        if (AR_SUCCESS != s32Ret)
        {
            printf("[%s]%d: AR_MPI_VPSS_GetChnFrame failed with %d!\n", __FILE__, __LINE__, s32Ret);
            continue;
        }
#ifdef ENABLE_TIME_MEASURE
        start_time = timer_get_ms();
#endif
        if(!fb_started)
        {
            s32Ret = get_fb_addr(&fb_w, &fb_h, &fb_size);
            if(s32Ret == AR_SUCCESS)
            {
                printf("fb_w: %d fb_h: %d fb_size: %ld \n", fb_w, fb_h, fb_size);
                fb_started = 1;
            }
        }
        if(fb_started)
        {
            if(dump_data_cnt == 500)
            {
                char dump_file[128] = {0};
                sprintf(dump_file, "/usrdata/framebuffer_data_%d.bin", dump_data_cnt);
                dump_to_file(dump_file, data_virt_addr, fb_size);
                dump_data_cnt = -1;
            } else {
                if(dump_data_cnt != -1)
                    dump_data_cnt ++;
            }
            
            s32Ret = AR_MPI_VGS_BeginJob(&job_id);
            if (AR_SUCCESS != s32Ret)
            {
                printf("[%s]%d: AR_MPI_VGS_BeginJob failed with %d!!!!!!!!!!!!!\n", __FILE__, __LINE__, s32Ret);
            }
            if(job_id > -1)
            {
                task_attr.stImgIn = FrameInfo;
                task_attr.stImgOut = FrameInfo;
                /* add osd  */
                VGS_ADD_OSD_S  osd;
                osd.stRect.s32X = 0;
                osd.stRect.s32Y = 0;
                osd.stRect.u32Width = u32ScreenW;
                osd.stRect.u32Height = u32ScreenH;
                osd.u32BgColor = 0xffffffff;
                osd.enPixelFmt = enPixelFmt;
                #ifdef USE_TEST_DATA
                osd.u64PhyAddr = src_addr.y_phy;
                osd.u32Stride  = src_addr.y_stride;
                #else
                osd.u64PhyAddr = fb_phy_addr;//src_addr.y_phy;
                osd.u32Stride  = fb_w * 4;//src_addr.y_stride;
                #endif
                osd.u32BgAlpha = 80;
                osd.u32FgAlpha = 80;
                osd.bOsdRevert = 0;
                s32Ret = AR_MPI_VGS_AddOsdTask(job_id, &task_attr, &osd);
                if (AR_SUCCESS != s32Ret)
                {
                    printf("[%s]%d: AR_MPI_VGS_AddOsdTask failed with 0x%x!\n", __FILE__, __LINE__, s32Ret);
                    s32Ret = AR_MPI_VGS_EndJob(job_id);
                    job_id = -1;
                }
#ifdef ENABLE_TIME_MEASURE
                end_start = timer_get_ms();
#endif
                s32Ret = AR_MPI_VGS_EndJob(job_id);
#ifdef ENABLE_TIME_MEASURE
                end_end = timer_get_ms();
#endif
            }
#ifdef ENABLE_TIME_MEASURE
            PRINT_INFO("vgs start: %d, used time: %d, end start: %d, end use: %d!\n", start_time, timer_get_ms() - start_time, end_start, end_end - end_start);
#endif
        }
        AR_MPI_VENC_SendFrame(0, &FrameInfo, -1);

        AR_MPI_VO_SendFrame(0, 0, &FrameInfo, -1);
        
        AR_MPI_VPSS_ReleaseChnFrame(VpssGrp, VpssChn, &FrameInfo);
    }
    if(fb_started)
    {
        release_fb(data_virt_addr, fb_size);
        data_virt_addr = NULL;
    }
    if(job_id > -1)
        s32Ret = AR_MPI_VGS_EndJob(job_id);
    
#ifdef USE_TEST_DATA
EXIT:
    buffer_release(&src_addr);
#endif
    return 0;
}

int  IPC_VGS_Init(void)
{
    if(vgsRun == 0)
    {
        vgsRun = 1;
        pthread_create(&vgsId, NULL, IPC_MAIN_VGS_Thread, NULL);
    }
    return 0;

}
void IPC_VGS_UnInit()
{
    if(vgsRun == 1)
    {
        vgsRun = 0;
        pthread_join(vgsId,NULL);
    }
}


