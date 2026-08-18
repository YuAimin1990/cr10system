//#define ENABALE_ALL_LOG

#include <math.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "ge2d_api.h"
#include "ge2d_hw.h"
#include "ge2d_util.h"
#include "ar_mpp_irq.h"
#include "utils_debug.h"

#if defined LOG_TAG
#undef      LOG_TAG
#define     LOG_TAG  "ge2d"
#endif

//#define USE_SRC_WH
#define ge2d_ceil(a,b) ((a)%(b)>0 ? ((a)/(b)+1):(a)/(b))

static int get_stride_pix_num(int format,int y_sride,int uv_stride,int *pix_y_stride,int *pix_uv_stride);

static void reorder(FLOAT_AXIS map[4], float *min_x, float *max_x, float *min_y, float *max_y)
{
    float x[4];
    float y[4];
    float x_max, x_min;
    float y_max, y_min;

    for (int i = 0; i < 4; i++)
    {
        x[i] = map[i].x;
        y[i] = map[i].y;
    }

    // locate the boundary @ 1 cycle
    int selx = ((x[0] < x[2]) << 1) | (x[0] < x[1]);
    switch (selx)
    {
    case 0:
        x_max = x[0];
        x_min = x[3];
        break;
    case 1:
        x_max = x[1];
        x_min = x[2];
        break;
    case 2:
        x_max = x[2];
        x_min = x[1];
        break;
    case 3:
        x_max = x[3];
        x_min = x[0];
        break;
    }

    *max_x = x_max;
    *min_x = x_min;

    int sely = ((y[0] < y[2]) << 1) | (y[0] < y[1]);
    switch (sely)
    {
    case 0:
        y_max = y[0];
        y_min = y[3];
        break;
    case 1:
        y_max = y[1];
        y_min = y[2];
        break;
    case 2:
        y_max = y[2];
        y_min = y[1];
        break;
    case 3:
        y_max = y[3];
        y_min = y[0];
        break;
    }

    *max_y = y_max;
    *min_y = y_min;
}

static inline int round_ge2d(float x)
{
    int value;
    if (x < 0)
    	value = (int)(x - 0.5);
    else
    	value = (int)(x + 0.5);
    
    return value;
}
// Inv-Rotation Matrix£º rotation axis( x0, y0 )
//
//    |  cos(r)    -sin(r)     x0*(1-cos(r)) + y0*sin(r) |
//    |  sin(r)     cos(r)     y0*(1-cos(r)) - x0*sin(r)  |
//    |    0          0                  1                          |
//
static void calculate_maps(uint32_t *pmap_mem, int hor_vert_num, int ver_vert_num, FLOAT_AXIS rot, FLOAT_AXIS topleft, float degree)
{
	int blk64x64_hor_cnt, blk64x64_ver_cnt;
	FLOAT_AXIS point_axis;
	FLOAT_AXIS inv_axis;
	float cos_theta, sin_theta;
	AXIS inv_axis_int;
	cos_theta = (float)cos(degree*3.1415926535897931f / 180);
	sin_theta = (float)sin(degree*3.1415926535897931f / 180);
	float x_offset;
	float y_offset;
	x_offset = rot.x * (1 - cos_theta) + rot.y * sin_theta;
	y_offset = rot.y * (1 - cos_theta) - rot.x * sin_theta;
	int stride = ( ( (hor_vert_num * 4 + 31) / 32 ) * 32);
	stride/=4;
	uint32_t val=0;
	for (blk64x64_ver_cnt = 0; blk64x64_ver_cnt < ver_vert_num; blk64x64_ver_cnt++)
	{
		for (blk64x64_hor_cnt = 0; blk64x64_hor_cnt < hor_vert_num; blk64x64_hor_cnt++)
		{
			point_axis.x = topleft.x + blk64x64_hor_cnt * 64;
			point_axis.y = topleft.y + blk64x64_ver_cnt * 64;
			inv_axis.x =  point_axis.x * cos_theta - point_axis.y * sin_theta + x_offset;
			inv_axis.y =  point_axis.x * sin_theta + point_axis.y * cos_theta + y_offset;
			inv_axis_int.x = (short)round_ge2d(inv_axis.x * 8); // round
			inv_axis_int.y = (short)round_ge2d(inv_axis.y * 8);
			val= (inv_axis_int.x&0xffff)|((inv_axis_int.y&0xffff)<<16);
			pmap_mem[stride * blk64x64_ver_cnt +blk64x64_hor_cnt]=val;
		}
	}
}
//     axis
//      |
//      |
//------------------------------>
//      |
//      |
//      |
//      |
//      \/
// Rotation Matrix£º rotation axis( x0, y0 )
//    |  cos(r)    sin(r)    x0*(1-cos(r)) - y0*sin(r)  |
//    | -sin(r)    cos(r)    y0*(1-cos(r)) + x0*sin(r)  |
//    |    0          0                  1				|
void calculate_rot_vert(FLOAT_AXIS rot, FLOAT_AXIS dst_vert[4], FLOAT_AXIS src_vert[4], float degree)
{
	float sin_theta, cos_theta;
	int vert_cnt = 0;

	sin_theta = (float)sin(degree*3.1415926535897931f / 180);
	cos_theta = (float)cos(degree*3.1415926535897931f / 180);
	float x_offset = rot.x * (1 - cos_theta) - rot.y * sin_theta;
	float y_offset = rot.y * (1 - cos_theta) + rot.x * sin_theta;
	for (vert_cnt = 0; vert_cnt < 4; vert_cnt++)
	{
		dst_vert[vert_cnt].x = src_vert[vert_cnt].x *cos_theta + src_vert[vert_cnt].y * sin_theta + x_offset;
		dst_vert[vert_cnt].y =-src_vert[vert_cnt].x *sin_theta + src_vert[vert_cnt].y * cos_theta + y_offset;
	}
}

static int gen_rotation_table(ge2d_client_t *client,frame_cmd_t *frame,int w,int h,int *dst_out_x,int *dst_out_y,float degree)
{
	FLOAT_AXIS rot, dst_vert[4], src_vert[4];

	rot.x = (float)(w - 1)*1.0 / 2;
	rot.y = (float)(h - 1)*1.0 / 2;
	src_vert[0].x = 0;
	src_vert[0].y = 0;
	src_vert[1].x = (float)w- 1;
	src_vert[1].y = 0;
	src_vert[2].x = 0;
	src_vert[2].y = (float)h - 1;
	src_vert[3].x = (float)w - 1;
	src_vert[3].y = (float)h - 1;
	float x_max, x_min, y_max, y_min;

	calculate_rot_vert(rot, dst_vert, src_vert, degree);

	reorder(dst_vert, &x_min, &x_max, &y_min, &y_max);

	float width_after_rot_flt;
	float height_after_rot_flt;
	int width_after_rot_flt_int;
	int height_after_rot_flt_int;

	width_after_rot_flt = x_max - x_min;
	height_after_rot_flt = y_max - y_min;
	width_after_rot_flt_int = (int)(width_after_rot_flt * 8);
	height_after_rot_flt_int = (int)(height_after_rot_flt * 8);


	int width_after_rot, height_after_rot;
	width_after_rot = 1 + (width_after_rot_flt_int >> 3) + (!!(width_after_rot_flt_int & 7));
	height_after_rot = 1 + (height_after_rot_flt_int >> 3) + (!!(height_after_rot_flt_int & 7));

	*dst_out_x=width_after_rot;
	*dst_out_y=height_after_rot;

    //log_tag_info("dest wh  %d %d src wh %d %d",*dst_out_x,*dst_out_y,w,h);

	//if (pbmp_param->transform_mode == DEGREE_OTHERS)
	{
		int width = (width_after_rot + 63) / 64 + 1; // need more 1
		int height = (height_after_rot + 63) / 64 + 1; // need more 1

		FLOAT_AXIS topleft;
		topleft.x = x_min;
		topleft.y = y_min;
		// fill data
		calculate_maps(frame->map_table, width, height, rot, topleft, degree);
	}
   return 0;
}

// register irq and wait irq done, sync interface
static void irq_reg(unsigned int irq_id, ge2d_server_t *server)
{
    ar_assert(server);
    ar_assert(server->mpp_dev_fd > 0);

    irq_id = irq_id - IRQ_OFFSET;
    ar_assert(irq_id < MAX_IRQ_CNT);

    struct request_irq_desc reqest;
    memset(&reqest, 0, sizeof(struct request_irq_desc));
    reqest.irq_id = irq_id;

    int rc = 0;
    rc = ioctl(server->mpp_dev_fd, AR_MPP_QUERY_IRQ_ONESHOT, &reqest);
    if (rc < 0) {
        log_tag_err("register irq 166 failed");
    }
}

#if defined(__linux__)
static void *ge2d_server_task(void *argument)
#else
static void ge2d_server_task(void *argument)
#endif
{
   ge2d_server_t *server=(ge2d_server_t *)argument;
   frame_cmd_t *p_frame=NULL;
   int index=0;
   do
   {
       server->ge2d_main_queue->queue_pop(server->ge2d_main_queue,(void **)&p_frame);
       if (p_frame) {
           //process the frame and wait the frame completed
           //log_tag_info("start the frame");
           ge2d_hw_start_frame(p_frame);

           //register the irq 166?
           irq_reg(GE2D_IRQ_SIGNAL, server);

           index=ge2d_hw_get_frame_index();
           if (index >= 0 && index < 256) {
               //log_tag_info("process done index=%d\n",index);
               //p_frame=server->frame[index];
               //ar_assert(p_frame == server->frame[index]);
           } else {
               log_tag_err("err_index");
           }

           ar_signal(p_frame->frame_done);
       } else {
	        //log_tag_info("wait the frame cmd");
	        ar_signal_wait(server->ge2d_main_queue_signal);
       }
   } while(1);

#if defined(__linux__)
	return NULL;
#else
   	return;
#endif
}

#if 0
static BaseType_t ge2d_server_debug(char *p_write_buffer, size_t write_bufferLen, const char *pc_command_string,void *usr_data)
{
	p_write_buffer[0]=0;
	cmd_args_t *p_args=NULL;
	//ge2d_server_t * server=(ge2d_server_t *)usr_data;
	p_args=get_args(pc_command_string);
	int index=find_pra_index_by_name(p_args,"--help");
	if(index>0)
	{
       ar_printf("--help :show this help\n");
       ar_free(p_args);
       return pdTRUE;
	}

    ge2d_hw_info_show();
	ar_free(p_args);
	return pdTRUE;

}
#endif

static int client_set_format(ge2d_client_t *client,int w,int h,int format,int y_stride,int uv_stride)
{

   client->format=format;
   client->w=w;
   client->h=h;
   client->y_stride=y_stride;
   client->uv_stride=uv_stride;

   AR_SET_REG_BITS(client->start_frame.cmd_offset_0,0XA1,0,7);
   AR_SET_REG_BITS(client->start_frame.cmd_offset_0,0XA1,24,31);
   AR_SET_REG_BITS(client->start_frame.cmd_offset_0,format,8,11);
   AR_SET_REG_BITS(client->start_frame.cmd_offset_0,1,12,12);
   //disable the global alfa
   AR_SET_REG_BITS(client->start_frame.cmd_offset_0,0,13,13);
   AR_SET_REG_BITS(client->start_frame.cmd_offset_0,0x7f,17,23);
   AR_SET_REG_BITS(client->start_frame.cmd_offset_0,client->csc_mode,15,16);

   //w ,h
   AR_SET_REG_BITS(client->start_frame.cmd_offset_1,w-1,0,11);
   AR_SET_REG_BITS(client->start_frame.cmd_offset_1,h-1,16,27);

   //w ,h stride
   int pix_y_stride;
   int pix_uv_stride;
   get_stride_pix_num(format,y_stride,uv_stride,&pix_y_stride,&pix_uv_stride);
   AR_SET_REG_BITS(client->start_frame.cmd_offset_2,pix_y_stride,0,9);
   AR_SET_REG_BITS(client->start_frame.cmd_offset_2,pix_uv_stride,16,25);

   if(client->need_stats)
   {
	  AR_SET_REG_BITS(client->start_frame.cmd_offset_0,1,14,14);
	  client->start_frame.stats_addr=(uint32_t)client->stats_buffer;
   }else
   {
	  AR_SET_REG_BITS(client->start_frame.cmd_offset_0,0,14,14);
	  client->start_frame.stats_addr=0;
   }
   return 0;
}

static int client_start(ge2d_client_t *client)
{
   return 0;
}
static int client_stop(ge2d_client_t *client)
{
   return 0;
}
static frame_cmd_t *malloc_frame(ge2d_client_t *client)
{
   frame_cmd_t *frame=NULL;
   ge2d_server_t *server=client->server;
   while(!frame)
   {
       server->ge2d_frame_pool_queue->queue_pop(server->ge2d_frame_pool_queue,(void **)&frame);
	   if(!frame)
	   {
	   	  ar_signal_wait(server->ge2d_frame_pool_queue_signal);
	   }
   }
   frame->frame_len=0;
   frame->line_count=0;
   frame->frame_end_addr=frame->frame_start_addr;
   //memset(frame->frame_start_addr,0,G2ED_FRAME_MAX_SIZE*sizeof(uint32_t));
   //log_tag_info("%x %x",frame->frame_end_addr,frame->frame_start_addr);
   return frame;
}

static int free_frame(ge2d_client_t *client,frame_cmd_t *frame)
{
	ge2d_server_t *server=client->server;
    if(server->ge2d_frame_pool_queue->get_queue_size(server->ge2d_frame_pool_queue)==0)
    {
		ar_signal(server->ge2d_frame_pool_queue_signal);
    }
	server->ge2d_frame_pool_queue->queue_insert(server->ge2d_frame_pool_queue,frame);

	return 0;
}

static int process_frame(ge2d_client_t *client,frame_cmd_t *frame)
{
	ge2d_server_t *server=client->server;
	//for debug only
	//clear_irq_force_for_debug();
	server->ge2d_main_queue->queue_insert(server->ge2d_main_queue,frame);
	ar_signal(server->ge2d_main_queue_signal);
	//log_tag_info("wait done frame->frame_index=%d....",frame->frame_index);
	client->current_frame_index=frame->frame_index;

	ar_signal_wait(frame->frame_done);
	//log_tag_info("wait done completed frame->frame_index=%d....",frame->frame_index);

    return 0;
}

static int frame_add_start(ge2d_client_t *client,frame_cmd_t *frame,void *y,void *u,void *v)
{
    client->start_frame.yuv[0]=(uint32_t)y;
    client->start_frame.yuv[1]=(uint32_t)u;
    client->start_frame.yuv[2]=(uint32_t)v;
	memcpy(frame->frame_end_addr,&client->start_frame,6*4); //28 byte start frame

	//for(int i=0;i<6;i++)
	//{
	//   log_tag_info("%x",frame->frame_end_addr[i]);
	//}
	frame->frame_len+=6;
	frame->frame_end_addr+=6; //point to next item
	if(client->need_stats)
	{
       frame->frame_end_addr[0]=(uint32_t)client->stats_buffer_phy;
	   frame->frame_end_addr++;
	   frame->frame_len++;
	}
    return 0;
}
static int frame_add_end(ge2d_client_t *client,frame_cmd_t *frame) //always have a done int
{
  AR_SET_REG_BITS(frame->frame_end_addr[0],0xa5,24,31);
  AR_SET_REG_BITS(frame->frame_end_addr[0],0xa5,0,7);
  AR_SET_REG_BITS(frame->frame_end_addr[0],1,8,8); //enable the frame buffer done
  //log_tag_info("%x",frame->frame_end_addr[0]);
  frame->frame_len++;
  //for end fame, not ++ the addr , point the frame end item
  //frame->frame_end_addr++;
  return 0;
}
static int frame_add_line(ge2d_client_t *client,frame_cmd_t *frame,line_data_t *data)
{
  AR_SET_REG_BITS(frame->frame_end_addr[0],0xa2,24,31);
  AR_SET_REG_BITS(frame->frame_end_addr[0],0xa2,0,7);
  frame->line_count=0;
  //log_tag_info("frame->frame_end_addr[0]=%x",frame->frame_end_addr[0]);
  frame->line_header_addr=frame->frame_end_addr;
  frame->frame_end_addr++;
  frame->frame_len++;
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->rgb,0,23);

  //log_tag_info("frame->frame_end_addr[0]=%x",frame->frame_end_addr[0]);
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->alfa,24,31);

  //log_tag_info("frame->frame_end_addr[0]=%x",frame->frame_end_addr[0]);
  frame->frame_end_addr++;
  frame->frame_len++;
  return 0;
}

static int frame_append_line(ge2d_client_t *client,frame_cmd_t *frame,line_append_data_t *data)
{
	//log_tag_info("enter");
    int line_w=data->line_w;
	AR_SET_REG_BITS(frame->frame_end_addr[0],data->start_x,0,11);
	AR_SET_REG_BITS(frame->frame_end_addr[0],data->start_y,12,23);
	AR_SET_REG_BITS(frame->frame_end_addr[0],(line_w&0xff),24,31);
	frame->frame_end_addr++;
	frame->frame_len++;
	AR_SET_REG_BITS(frame->frame_end_addr[0],((line_w&0xf00)>>8),24,27);
	AR_SET_REG_BITS(frame->frame_end_addr[0],1,28,28); //atin
	AR_SET_REG_BITS(frame->frame_end_addr[0],data->end_x,0,11);
	AR_SET_REG_BITS(frame->frame_end_addr[0],data->end_y,12,23);
	AR_SET_REG_BITS(frame->frame_end_addr[0],!data->alfa_en,29,29); //alfa en to set the bit as 1

	frame->frame_end_addr++;
	frame->frame_len++;
	frame->line_count++;
	if(data->end_line)
	{
	   AR_SET_REG_BITS(frame->line_header_addr[0],(frame->line_count-1),12,23);
	}

    return 0;
}

static int frame_add_line_end(ge2d_client_t *client,frame_cmd_t *frame)
{
	AR_SET_REG_BITS(frame->line_header_addr[0],(frame->line_count-1),12,23);
    return 0;
}

static int get_stride_pix_num(int format,int y_sride,int uv_stride,int *pix_y_stride,int *pix_uv_stride)
{
    *pix_y_stride=y_sride;
    *pix_uv_stride=uv_stride;
    switch(format)
    {
       case ARGB8888:
	   {
		  *pix_y_stride=ge2d_ceil(y_sride/4,8);
		  *pix_uv_stride=0;
	   }
	   break;
	   case ARGB4444:
	   case ARGB1555:
	   {
		  *pix_y_stride=ge2d_ceil(y_sride/2,8);
		  *pix_uv_stride=0;
	   }
	   break;
	   case YV12:
	   {
		   *pix_y_stride=ge2d_ceil(y_sride,8);
		   *pix_uv_stride=ge2d_ceil(uv_stride,4);
	   }
	   break;
	   case NV12:
	   {
		  *pix_y_stride=ge2d_ceil(y_sride,8);
		  *pix_uv_stride=ge2d_ceil(uv_stride,8);
	   }
	   break;
	   case MONOCHROMA:
	   {
		  *pix_y_stride=ge2d_ceil(y_sride,8);
		  *pix_uv_stride=0;
	   }
	   break;
	   case GRAY1:
	   {
		   *pix_y_stride=ge2d_ceil(y_sride*8,8);
		   *pix_uv_stride=0;
	   }
       break;
	   case GRAY4:
	   {
		   *pix_y_stride=ge2d_ceil(y_sride*2,8);
		   *pix_uv_stride=0;
	   }
       break;
	   case GRAY8:
	   {
		   *pix_y_stride=ge2d_ceil(y_sride,8);
		   *pix_uv_stride=0;
	   }
       break;
	   default:
	   break;
    }

    //log_tag_info("y_sride = %d, uv_stride = %d, pix_y_stride = %d pix_uv_stride = %d",
    //              y_sride,uv_stride,*pix_y_stride,*pix_uv_stride);

	return 0;
};

static int frame_add_bmp(ge2d_client_t *client,frame_cmd_t *frame,bmp_data_t *data)
{
  //log_tag_info("enter");
  AR_SET_REG_BITS(frame->frame_end_addr[0],0xa3,24,31);
  AR_SET_REG_BITS(frame->frame_end_addr[0],0xa3,0,7);
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->format,8,11);
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->alfa/2,17,23);
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,12,12);  //not rotation
  if(data->use_global_alfa){
     AR_SET_REG_BITS(frame->frame_end_addr[0],1,13,13); //global alfa enable
  }else
  {
     AR_SET_REG_BITS(frame->frame_end_addr[0],0,13,13);
  }
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,14,14); //disable color key
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,15,15); //disable mosic
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,16,16); //no pre multi

  frame->frame_end_addr++;
  frame->frame_len++;

  AR_SET_REG_BITS(frame->frame_end_addr[0],data->dest_x,0,12);
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->dest_y,16,28);

  AR_SET_REG_BITS(frame->frame_end_addr[0],1,13,13); //enable crop
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,14,14); //close alfa blending function
  //csc_mode
  AR_SET_REG_BITS(frame->frame_end_addr[0],client->csc_mode,29,30);
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,31,31); //color revert
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,15,15); //enable stats

  frame->frame_end_addr++;
  frame->frame_len++;
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->dest_x+data->w-1,0,12);
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->dest_y+data->h-1,16,28);
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,31,31); //luma stats only
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,13,15); //cci

  frame->frame_end_addr++;
  frame->frame_len++;

  #ifdef USE_SRC_WH
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->src_w-1,0,11);  //src w
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->src_h-1,16,27); //src h
  #else
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->x+data->w-1,0,11);  //src w
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->y+data->h-1,16,27); //src h
  #endif


  frame->frame_end_addr++;
  frame->frame_len++;

  int pix_y_stride;
  int pix_uv_stride;
  get_stride_pix_num(data->format,data->stride,data->uv_stride,&pix_y_stride,&pix_uv_stride);
  AR_SET_REG_BITS(frame->frame_end_addr[0],pix_y_stride,0,9); //y stride
  AR_SET_REG_BITS(frame->frame_end_addr[0],pix_uv_stride,16,25); //uv stride
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,10,15); //low index
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,26,31); //low index

  //always set orig x,y
  frame->frame_end_addr++;
  frame->frame_len++;
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->x,0,12);
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->y,16,28);

  frame->frame_end_addr++;
  frame->frame_len++;
  frame->frame_end_addr[0]=(uint32_t)data->y_addr;

  if(data->format==YV12 || data->format==NV12){
      frame->frame_end_addr++;
      frame->frame_len++;
      frame->frame_end_addr[0]=(uint32_t)data->u_addr;
  }
  if(data->format==YV12){
      frame->frame_end_addr++;
      frame->frame_len++;
      frame->frame_end_addr[0]=(uint32_t)data->v_addr;
  }

  if(0)
  {
      frame->frame_end_addr++;
      frame->frame_len++;
      frame->frame_end_addr[0]=0; //map base

      frame->frame_end_addr++;
      frame->frame_len++;
      frame->frame_end_addr[0]=0; //rtatio background

      frame->frame_end_addr++;
      frame->frame_len++;
      frame->frame_end_addr[0]=0; //text background

      frame->frame_end_addr++;
      frame->frame_len++;
      frame->frame_end_addr[0]=0; //text frontground

      frame->frame_end_addr++;
      frame->frame_len++;
      frame->frame_end_addr[0]=0; //color key high

      frame->frame_end_addr++;
      frame->frame_len++;
      frame->frame_end_addr[0]=0; //color key low

      frame->frame_end_addr++;
      frame->frame_len++;
      frame->frame_end_addr[0]=0; //mosic level
  }
   //point to next
  frame->frame_end_addr++;
  frame->frame_len++;

  return 0;
}
static int frame_add_dot_mat_stats(ge2d_client_t *client,frame_cmd_t *frame,dot_mat_data_t *data)
{
  //log_tag_info("enter");
  frame->th=data->th;
  AR_SET_REG_BITS(frame->frame_end_addr[0],0xa3,24,31);
  AR_SET_REG_BITS(frame->frame_end_addr[0],0xa3,0,7);
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->format,8,11); //format, dot matrix only 1 bit 1 pix
  AR_SET_REG_BITS(frame->frame_end_addr[0],(data->alfa/2),17,23);
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,12,12);  //not rotation
  if(data->use_global_alfa){
	 AR_SET_REG_BITS(frame->frame_end_addr[0],1,13,13); //global alfa enable
  }else
  {
	 AR_SET_REG_BITS(frame->frame_end_addr[0],0,13,13);
  }
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,14,14); //disable color key
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,15,15); //disable mosic
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,16,16); //no pre multi
  frame->frame_end_addr++;
  frame->frame_len++;

  AR_SET_REG_BITS(frame->frame_end_addr[0],data->dest_x,0,12);
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->dest_y,16,28);
  AR_SET_REG_BITS(frame->frame_end_addr[0],1,13,13); //enable crop
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,14,14);
  AR_SET_REG_BITS(frame->frame_end_addr[0],client->csc_mode,29,30);
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,31,31); //invert color, not invert color
  AR_SET_REG_BITS(frame->frame_end_addr[0],1,15,15); //enable stats

  frame->frame_end_addr++;
  frame->frame_len++;
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->dest_x+data->w-1,0,12); //dest  rb_x
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->dest_y+data->h-1,16,28);//dest rb_y
  AR_SET_REG_BITS(frame->frame_end_addr[0],1,31,31); //stats only enable
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,13,15); //cci


  //src x y always is 0,so
  frame->frame_end_addr++;
  frame->frame_len++;
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->w-1,0,11); //src w
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->h-1,16,27);//src h


  frame->frame_end_addr++;
  frame->frame_len++;
  int pix_y_stride;
  int pix_uv_stride;
  get_stride_pix_num(data->format,data->stride,data->stride,&pix_y_stride,&pix_uv_stride);
  AR_SET_REG_BITS(frame->frame_end_addr[0],pix_y_stride,0,9); //y stride
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,16,25);
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,10,15); //index low
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,26,31); //index high

  //always set 0,0 as crop
  frame->frame_end_addr++;
  frame->frame_len++;
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,0,12); //src x
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,16,28);//src y

  frame->frame_end_addr++;
  frame->frame_len++;
  frame->frame_end_addr[0]=(uint32_t)data->addr;  //y

  //dot no uv addr
  if(0)
  {
	  frame->frame_end_addr++;
	  frame->frame_len++;
	  frame->frame_end_addr[0]=(uint32_t)data->addr;//u
	  frame->frame_end_addr++;
	  frame->frame_len++;
	  frame->frame_end_addr[0]=(uint32_t)data->addr;//v
  }


  if(0)
  {
	 frame->frame_end_addr++;
	 frame->frame_len++;
	 frame->frame_end_addr[0]=0; //map base

	 frame->frame_end_addr++;
	 frame->frame_len++;
	 frame->frame_end_addr[0]=0; //rtatio background
  }

  frame->frame_end_addr++;
  frame->frame_len++;
  frame->frame_end_addr[0]=data->front_color; //text frontground

  frame->frame_end_addr++;
  frame->frame_len++;
  frame->frame_end_addr[0]=(1<<16)/data->w/data->h;

  if(0)
  {
	 frame->frame_end_addr++;
	 frame->frame_len++;
	 frame->frame_end_addr[0]=0; //color key high

	 frame->frame_end_addr++;
	 frame->frame_len++;
	 frame->frame_end_addr[0]=0; //color key low

	 frame->frame_end_addr++;
	 frame->frame_len++;
	 frame->frame_end_addr[0]=0; //mosic level
  }
  //point to next
  frame->frame_end_addr++;
  frame->frame_len++;

  return 0;
}

static int frame_add_dot_mat(ge2d_client_t *client,frame_cmd_t *frame,dot_mat_data_t *data)
{
  //log_tag_info("enter");
  if(data->need_revert_color)
  {
      frame_add_dot_mat_stats(client,frame,data);
  }
  AR_SET_REG_BITS(frame->frame_end_addr[0],0xa3,24,31);
  AR_SET_REG_BITS(frame->frame_end_addr[0],0xa3,0,7);
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->format,8,11); //format, dot matrix only 1 bit 1 pix
  AR_SET_REG_BITS(frame->frame_end_addr[0],(data->alfa/2),17,23);
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,12,12);  //not rotation
  if(data->use_global_alfa){
	 AR_SET_REG_BITS(frame->frame_end_addr[0],1,13,13); //global alfa enable
  }else
  {
	 AR_SET_REG_BITS(frame->frame_end_addr[0],0,13,13);
  }
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,14,14); //disable color key
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,15,15); //disable mosic
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,16,16); //no pre multi
  frame->frame_end_addr++;
  frame->frame_len++;

  AR_SET_REG_BITS(frame->frame_end_addr[0],data->dest_x,0,12);
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->dest_y,16,28);
  AR_SET_REG_BITS(frame->frame_end_addr[0],1,13,13); //enable crop
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,14,14);
  AR_SET_REG_BITS(frame->frame_end_addr[0],client->csc_mode,29,30);
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,15,15); //enable stats
  if(data->need_revert_color)
  {
      AR_SET_REG_BITS(frame->frame_end_addr[0],1,31,31); //enable revert color
  }else
  {
      AR_SET_REG_BITS(frame->frame_end_addr[0],0,31,31); //disable revert color
  }

  frame->frame_end_addr++;
  frame->frame_len++;
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->dest_x+data->w-1,0,12); //dest  rb_x
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->dest_y+data->h-1,16,28);//dest rb_y
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,31,31); //close stats only
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,13,15); //cci

  //src x y always is 0,so
  frame->frame_end_addr++;
  frame->frame_len++;
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->w-1,0,11); //src w
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->h-1,16,27);//src h


  frame->frame_end_addr++;
  frame->frame_len++;
  int pix_y_stride;
  int pix_uv_stride;
  get_stride_pix_num(data->format,data->stride,data->stride,&pix_y_stride,&pix_uv_stride);
  AR_SET_REG_BITS(frame->frame_end_addr[0],pix_y_stride,0,9); //y stride
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,16,25);
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,10,15); //index low
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,26,31); //index high

  //always set 0,0 as crop
  frame->frame_end_addr++;
  frame->frame_len++;
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,0,12); //src x
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,16,28);//src y

  frame->frame_end_addr++;
  frame->frame_len++;
  frame->frame_end_addr[0]=(uint32_t)data->addr;  //y

  //dot no uv addr
  if(0)
  {
      frame->frame_end_addr++;
      frame->frame_len++;
      frame->frame_end_addr[0]=(uint32_t)data->addr;//u
      frame->frame_end_addr++;
      frame->frame_len++;
      frame->frame_end_addr[0]=(uint32_t)data->addr;//v
  }


  if(0)
  {
     frame->frame_end_addr++;
     frame->frame_len++;
     frame->frame_end_addr[0]=0; //map base

     frame->frame_end_addr++;
     frame->frame_len++;
     frame->frame_end_addr[0]=0; //rtatio background
  }

  frame->frame_end_addr++;
  frame->frame_len++;
  frame->frame_end_addr[0]=data->front_color; //text frontground

  frame->frame_end_addr++;
  frame->frame_len++;
  frame->frame_end_addr[0]=data->back_color; //text background

  if(0)
  {
     frame->frame_end_addr++;
     frame->frame_len++;
     frame->frame_end_addr[0]=0; //color key high

     frame->frame_end_addr++;
     frame->frame_len++;
     frame->frame_end_addr[0]=0; //color key low

     frame->frame_end_addr++;
     frame->frame_len++;
     frame->frame_end_addr[0]=0; //mosic level
  }
  //point to next
  frame->frame_end_addr++;
  frame->frame_len++;

  return 0;
}

static int frame_add_rotation(ge2d_client_t *client,frame_cmd_t *frame,rotation_data_t *data)
{
  int dst_w=0;
  int dst_h=0;
  gen_rotation_table(client,frame,data->w,data->h,&dst_w,&dst_h,data->degree);
  AR_SET_REG_BITS(frame->frame_end_addr[0],0xa3,24,31);
  AR_SET_REG_BITS(frame->frame_end_addr[0],0xa3,0,7);
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->format,8,11);
  AR_SET_REG_BITS(frame->frame_end_addr[0],(data->alfa/2),17,23);
  AR_SET_REG_BITS(frame->frame_end_addr[0],1,12,12);  //rotation
  if(data->use_global_alfa){
	 AR_SET_REG_BITS(frame->frame_end_addr[0],1,13,13); //global alfa enable
  }else
  {
	 AR_SET_REG_BITS(frame->frame_end_addr[0],0,13,13);
  }
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,14,14); //disable color key
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,15,15); //disable mosic
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,16,16); //no pre multi
  frame->frame_end_addr++;
  frame->frame_len++;

  AR_SET_REG_BITS(frame->frame_end_addr[0],data->dest_x,0,12);
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->dest_y,16,28);
  AR_SET_REG_BITS(frame->frame_end_addr[0],1,13,13); //disable crop
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,14,14);
  AR_SET_REG_BITS(frame->frame_end_addr[0],client->csc_mode,29,30);
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,31,31); //color revert
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,15,15); //enable stats


  frame->frame_end_addr++;
  frame->frame_len++;
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->dest_x+dst_w-1,0,12);
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->dest_y+dst_h-1,16,28);
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,31,31); //luma stats only
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,13,15); //cci


  frame->frame_end_addr++;
  frame->frame_len++;
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->x+data->w-1,0,11);  //src w
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->y+data->h-1,16,27); //src h

  frame->frame_end_addr++;
  frame->frame_len++;
  int pix_y_stride;
  int pix_uv_stride;
  get_stride_pix_num(data->format,data->stride,data->uv_stride,&pix_y_stride,&pix_uv_stride);
  AR_SET_REG_BITS(frame->frame_end_addr[0],pix_y_stride,0,11);
  AR_SET_REG_BITS(frame->frame_end_addr[0],pix_uv_stride,16,25);
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,10,15);
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,26,31);

  frame->frame_end_addr++;
  frame->frame_len++;
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->x,0,12);
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->y,16,28);

  frame->frame_end_addr++;
  frame->frame_len++;
  frame->frame_end_addr[0]=(uint32_t)data->y_addr;


  if(data->format==YV12 || data->format==NV12)
  {
      frame->frame_end_addr++;
      frame->frame_len++;
      frame->frame_end_addr[0]=(uint32_t)data->u_addr;
  }
  if(data->format==YV12){
      frame->frame_end_addr++;
      frame->frame_len++;
      frame->frame_end_addr[0]=(uint32_t)data->v_addr;
  }

  frame->frame_end_addr++;
  frame->frame_len++;
  frame->frame_end_addr[0]=(uint32_t)frame->map_table_phy; //map base

  frame->frame_end_addr++;
  frame->frame_len++;
  frame->frame_end_addr[0]=data->back_color; //rtatio background

  if(0)
  {
     frame->frame_end_addr++;
     frame->frame_len++;
     frame->frame_end_addr[0]=0; //text background

     frame->frame_end_addr++;
     frame->frame_len++;
     frame->frame_end_addr[0]=0; //text frontground

     frame->frame_end_addr++;
     frame->frame_len++;
     frame->frame_end_addr[0]=0; //color key high

     frame->frame_end_addr++;
     frame->frame_len++;
     frame->frame_end_addr[0]=0; //color key low

     frame->frame_end_addr++;
     frame->frame_len++;
     frame->frame_end_addr[0]=0; //mosic level
  }
  frame->frame_end_addr++;
  frame->frame_len++;
  return 0;
}

static int frame_add_mosaic(ge2d_client_t *client,frame_cmd_t *frame,mosaic_data_t *data)
{
  AR_SET_REG_BITS(frame->frame_end_addr[0],0xa3,24,31);
  AR_SET_REG_BITS(frame->frame_end_addr[0],0xa3,0,7);
  AR_SET_REG_BITS(frame->frame_end_addr[0],client->format,8,11); //format
  AR_SET_REG_BITS(frame->frame_end_addr[0],(data->alfa/2),17,23);
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,12,12);  //rotation
  if(data->use_global_alfa){
	 AR_SET_REG_BITS(frame->frame_end_addr[0],1,13,13); //global alfa enable
  }else
  {
	 AR_SET_REG_BITS(frame->frame_end_addr[0],0,13,13);
  }
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,14,14); //disable color key
  AR_SET_REG_BITS(frame->frame_end_addr[0],1,15,15); //enable mosic
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,16,16); //no pre multi
  frame->frame_end_addr++;
  frame->frame_len++;

  AR_SET_REG_BITS(frame->frame_end_addr[0],data->x,0,12); //dest x
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->y,16,28);//dest y
  AR_SET_REG_BITS(frame->frame_end_addr[0],1,13,13); //disable crop /om
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,14,14);//res
  AR_SET_REG_BITS(frame->frame_end_addr[0],client->csc_mode,29,30);
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,31,31); //color revert
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,15,15); //enable stats


  frame->frame_end_addr++;
  frame->frame_len++;
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->x+data->w-1,0,12); //dest bR_x
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->y+data->h-1,16,28);//dest br_y
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,31,31); //luma stats only
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,13,15); //cci

  //mosic ,crop x,y always is 0
  frame->frame_end_addr++;
  frame->frame_len++;

  AR_SET_REG_BITS(frame->frame_end_addr[0],client->w-1,0,11); //src w
  AR_SET_REG_BITS(frame->frame_end_addr[0],client->h-1,16,27); //src h

  frame->frame_end_addr++;
  frame->frame_len++;
  int pix_y_stride;
  int pix_uv_stride;
  get_stride_pix_num(client->format,client->y_stride,client->uv_stride,&pix_y_stride,&pix_uv_stride);
  AR_SET_REG_BITS(frame->frame_end_addr[0],pix_y_stride,0,11); //stride u
  AR_SET_REG_BITS(frame->frame_end_addr[0],pix_uv_stride,16,25); //stride v
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,10,15); //lowindex
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,26,31); //high index

  frame->frame_end_addr++;
  frame->frame_len++;
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->x,0,12); //src x
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->y,16,28);//src y


  frame->frame_end_addr++;
  frame->frame_len++;
  frame->frame_end_addr[0]=client->start_frame.yuv[0];

  if(client->format==YV12 || client->format==NV12)
  {
     frame->frame_end_addr++;
     frame->frame_len++;
     frame->frame_end_addr[0]=client->start_frame.yuv[1];
  }
  if(client->format==YV12)
  {
      frame->frame_end_addr++;
      frame->frame_len++;
      frame->frame_end_addr[0]=client->start_frame.yuv[2];
  }

  if(0)
  {
      frame->frame_end_addr++;
      frame->frame_len++;
      frame->frame_end_addr[0]=0; //map base

      frame->frame_end_addr++;
      frame->frame_len++;
      frame->frame_end_addr[0]=0; //rtatio background

      frame->frame_end_addr++;
      frame->frame_len++;
      frame->frame_end_addr[0]=0; //text background

      frame->frame_end_addr++;
      frame->frame_len++;
      frame->frame_end_addr[0]=0; //text frontground

      frame->frame_end_addr++;
      frame->frame_len++;
      frame->frame_end_addr[0]=0; //color key high

      frame->frame_end_addr++;
      frame->frame_len++;
      frame->frame_end_addr[0]=0; //color key low
  }
  frame->frame_end_addr++;
  frame->frame_len++;
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->level,0,1);//mosic level give max
  //point to next
  frame->frame_end_addr++;
  frame->frame_len++;
  return 0;
}

static int frame_add_color_key(ge2d_client_t *client,frame_cmd_t *frame,color_key_data_t *data)
{
  AR_SET_REG_BITS(frame->frame_end_addr[0],0xa3,24,31); //header
  AR_SET_REG_BITS(frame->frame_end_addr[0],0xa3,0,7); //header
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->format,8,11); //format
  AR_SET_REG_BITS(frame->frame_end_addr[0],(data->alfa/2),17,23); //alfa
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,12,12);  //rotation
  if(data->use_global_alfa){
	 AR_SET_REG_BITS(frame->frame_end_addr[0],1,13,13); //global alfa enable
  }else
  {
	 AR_SET_REG_BITS(frame->frame_end_addr[0],0,13,13);
  }
  AR_SET_REG_BITS(frame->frame_end_addr[0],1,14,14); //disable color key
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,15,15); //enable mosic
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,16,16); //pre multi
  frame->frame_end_addr++;
  frame->frame_len++;

  AR_SET_REG_BITS(frame->frame_end_addr[0],data->dest_x,0,12); //dest x
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->dest_y,16,28);//dest y
  AR_SET_REG_BITS(frame->frame_end_addr[0],1,13,13); //disable crop
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,14,14); //res
  AR_SET_REG_BITS(frame->frame_end_addr[0],client->csc_mode,29,30);
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,31,31); //color revert
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,15,15); //enable stats


  frame->frame_end_addr++;
  frame->frame_len++;
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->dest_x+data->w-1,0,12); //dest bR_x
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->dest_y+data->h-1,16,28);//dest br_y
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,31,31); //luma stats only
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,13,15); //cci

  frame->frame_end_addr++;
  frame->frame_len++;
  #ifdef USE_SRC_WH
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->src_w-1,0,11); //src w
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->src_h-1,16,27); //src h
  #else
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->x+data->w-1,0,11);  //src w
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->y+data->h-1,16,27); //src h
  #endif

  frame->frame_end_addr++;
  frame->frame_len++;
  int pix_y_stride;
  int pix_uv_stride;
  get_stride_pix_num(data->format,data->stride,data->uv_stride,&pix_y_stride,&pix_uv_stride);
  AR_SET_REG_BITS(frame->frame_end_addr[0],pix_y_stride,0,11);
  AR_SET_REG_BITS(frame->frame_end_addr[0],pix_uv_stride,16,25);
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,10,15); //lowindex
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,26,31); //high index

  frame->frame_end_addr++;
  frame->frame_len++;
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->x,0,12); //src x
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->y,16,28);//src y


  frame->frame_end_addr++;
  frame->frame_len++;
  frame->frame_end_addr[0]=(uint32_t)data->y_addr;

  if(data->format==YV12 || data->format==NV12)
  {
     frame->frame_end_addr++;
     frame->frame_len++;
     frame->frame_end_addr[0]=(uint32_t)data->u_addr;
  }
  if(data->format==YV12){
     frame->frame_end_addr++;
     frame->frame_len++;
     frame->frame_end_addr[0]=(uint32_t)data->v_addr;
  }

  if(0)
  {
     frame->frame_end_addr++;
     frame->frame_len++;
     frame->frame_end_addr[0]=0; //map base

     frame->frame_end_addr++;
     frame->frame_len++;
     frame->frame_end_addr[0]=0; //rtatio background

     frame->frame_end_addr++;
     frame->frame_len++;
     frame->frame_end_addr[0]=0; //text background

     frame->frame_end_addr++;
     frame->frame_len++;
     frame->frame_end_addr[0]=0; //text frontground
  }
  frame->frame_end_addr++;
  frame->frame_len++;
  frame->frame_end_addr[0]=data->color_key_high; //color key high

  frame->frame_end_addr++;
  frame->frame_len++;
  frame->frame_end_addr[0]=data->color_key_low; //color key low
  if(0)
  {
     frame->frame_end_addr++;
     frame->frame_len++;
     AR_SET_REG_BITS(frame->frame_end_addr[0],0,0,1);//mosic level give min
  }
  frame->frame_end_addr++;
  frame->frame_len++;

  return 0;
}
static int gen_strecth_blit_table(ge2d_client_t *client,frame_cmd_t *frame,int w,int h,int dst_w,int dst_h)
{
   float ration_x=(float)w/(float)dst_w;
   int point_num_x=AR_ALIGN64(dst_w)/64+1;
   float ration_y=(float)h/(float)dst_h;
   int point_num_y=AR_ALIGN64(dst_h)/64+1;
   uint32_t *p_table=(uint32_t *)frame->map_table;
   int x=0;
   int y=0;
   int line_len=point_num_x*sizeof(uint32_t);
   line_len=AR_ALIGN32(line_len);
   line_len/=sizeof(uint32_t);
   uint32_t val=0;
   //log_tag_info("line_len=%d ration_x=%d ration_y=%d point_num_x=%d point_num_y=%d",line_len,float2int(ration_x),float2int(ration_y),point_num_x,point_num_y);
   for(int i=0;i<point_num_y;i++)
   {
       for(int j=0;j<point_num_x;j++)
       {
    	   x=j*64*ration_x*8;
		   y=i*64*ration_y*8;
           val= x|(y<<16);
		   p_table[line_len * i +j]=val;
       }
   }
   return 0;
}
static int frame_add_stretch_blit(ge2d_client_t *client,frame_cmd_t *frame,stretch_blit_data_t *data)
{
  AR_SET_REG_BITS(frame->frame_end_addr[0],0xa3,24,31);
  AR_SET_REG_BITS(frame->frame_end_addr[0],0xa3,0,7);
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->format,8,11);
  AR_SET_REG_BITS(frame->frame_end_addr[0],(data->alfa/2),17,23);
  AR_SET_REG_BITS(frame->frame_end_addr[0],1,12,12);  //rotation
  if(data->use_global_alfa){
	 AR_SET_REG_BITS(frame->frame_end_addr[0],1,13,13); //global alfa enable
  }else
  {
	 AR_SET_REG_BITS(frame->frame_end_addr[0],0,13,13);
  }
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,14,14); //disable color key
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,15,15); //disable mosic
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,16,16); //no pre multi
  frame->frame_end_addr++;
  frame->frame_len++;

  AR_SET_REG_BITS(frame->frame_end_addr[0],data->dest_x,0,12);
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->dest_y,16,28);
  AR_SET_REG_BITS(frame->frame_end_addr[0],1,13,13); //disable crop
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,14,14);
  AR_SET_REG_BITS(frame->frame_end_addr[0],client->csc_mode,29,30);
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,31,31); //color revert
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,15,15); //enable stats


  frame->frame_end_addr++;
  frame->frame_len++;
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->dest_x+data->dest_w-1,0,12);
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->dest_y+data->dest_h-1,16,28);
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,31,31); //luma stats only
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,13,15); //luma stats only

  frame->frame_end_addr++;
  frame->frame_len++;
  #ifdef USE_SRC_WH
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->src_w-1,0,11);  //src w
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->src_h-1,16,27); //src h
  #else
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->x+data->w-1,0,11);  //src w
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->y+data->h-1,16,27); //src h
  #endif

  frame->frame_end_addr++;
  frame->frame_len++;
  int pix_y_stride;
  int pix_uv_stride;
  get_stride_pix_num(data->format,data->stride,data->uv_stride,&pix_y_stride,&pix_uv_stride);
  AR_SET_REG_BITS(frame->frame_end_addr[0],pix_y_stride,0,11);
  AR_SET_REG_BITS(frame->frame_end_addr[0],pix_uv_stride,16,25);
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,10,15);
  AR_SET_REG_BITS(frame->frame_end_addr[0],0,26,31);

  frame->frame_end_addr++;
  frame->frame_len++;
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->x,0,12);
  AR_SET_REG_BITS(frame->frame_end_addr[0],data->y,16,28);

  frame->frame_end_addr++;
  frame->frame_len++;
  frame->frame_end_addr[0]=(uint32_t)data->y_addr;


  if(data->format==YV12 || data->format==NV12)
  {
	  frame->frame_end_addr++;
	  frame->frame_len++;
	  frame->frame_end_addr[0]=(uint32_t)data->u_addr;
  }
  if(data->format==YV12){
	  frame->frame_end_addr++;
	  frame->frame_len++;
	  frame->frame_end_addr[0]=(uint32_t)data->v_addr;
  }

  frame->frame_end_addr++;
  frame->frame_len++;
  //gen map table
  gen_strecth_blit_table(client,frame,data->w,data->h,data->dest_w,data->dest_h);
  frame->frame_end_addr[0]=(uint32_t)frame->map_table_phy; //map base

  frame->frame_end_addr++;
  frame->frame_len++;
  frame->frame_end_addr[0]=0x00000000; //rtatio background

  if(0)
  {
	 frame->frame_end_addr++;
	 frame->frame_len++;
	 frame->frame_end_addr[0]=0; //text background

	 frame->frame_end_addr++;
	 frame->frame_len++;
	 frame->frame_end_addr[0]=0; //text frontground

	 frame->frame_end_addr++;
	 frame->frame_len++;
	 frame->frame_end_addr[0]=0; //color key high

	 frame->frame_end_addr++;
	 frame->frame_len++;
	 frame->frame_end_addr[0]=0; //color key low

	 frame->frame_end_addr++;
	 frame->frame_len++;
	 frame->frame_end_addr[0]=0; //mosic level
  }
  frame->frame_end_addr++;
  frame->frame_len++;
  return 0;
}

static int client_set_csc_mode(ge2d_client_t *client,int csc_mode)
{
    client->csc_mode=csc_mode;
	return 0;
}

static ge2d_client_t* creat_client(ge2d_server_t *server,char *name)
{
   ar_lock(server->lock);

   ge2d_client_t* p_client=ar_malloc(sizeof(ge2d_client_t));
   memset(p_client,0,sizeof(ge2d_client_t));
   if(p_client)
   {
        //register the client to server
        //find a empty slot to register the client
        for(int i=0;i<G2ED_MAX_CLIENT;i++)
        {
            if(!server->client_list[i])
            {
			    server->client_list[i]=p_client;
			    p_client->frame_count=0;
			    p_client->csc_mode=GE2D_CSC_MODE_BT709_PC;
			    p_client->server=server;
			    p_client->signal=ar_create_signal();
			    strcpy(p_client->name,name);
			    p_client->need_stats=1;
			    {
   			        //max 1024 dotmatrix
 			        p_client->stats_buffer=p_client->stats_buffer_orgin=ar_dma_malloc(MAX_DOT_MAT_NUM*64+64);
     		    	p_client->stats_buffer=(void *)AR_ALIGN64((unsigned long)p_client->stats_buffer);
			        p_client->stats_buffer_phy = ar_dma_va_to_pa(p_client->stats_buffer_orgin);
 			        p_client->stats_buffer_phy=(void *)AR_ALIGN64((unsigned long)p_client->stats_buffer_phy);
			    }
			    p_client->client_set_format=client_set_format;
			    p_client->client_set_csc_mode=client_set_csc_mode;
			    p_client->client_start=client_start;
			    p_client->client_stop=client_stop;
			    p_client->malloc_frame=malloc_frame;
			    p_client->free_frame=free_frame;
			    p_client->process_frame=process_frame;
			    p_client->frame_add_start=frame_add_start;
			    p_client->frame_add_end=frame_add_end;
			    p_client->frame_add_line=frame_add_line;
			    p_client->frame_append_line=frame_append_line;
			    p_client->frame_add_line_end=frame_add_line_end;
			    p_client->frame_add_bmp=frame_add_bmp;
			    p_client->frame_add_dot_mat=frame_add_dot_mat;
			    p_client->frame_add_rotation=frame_add_rotation;
			    p_client->frame_add_mosaic=frame_add_mosaic;
			    p_client->frame_add_color_key=frame_add_color_key;
			    p_client->frame_add_stretch_blit=frame_add_stretch_blit;
			    break;
            }
        }
   }else
   {
      log_tag_err("no mem to creat client");
   }
   ar_unlock(server->lock);
   return p_client;
}

static int delete_client(ge2d_server_t *server,ge2d_client_t *client)
{
   ar_lock(server->lock);
   ge2d_client_t* p_client=client;
   if(p_client)
   {
        //register the client to server
        //find a empty slot to register the client
        for(int i=0;i<G2ED_MAX_CLIENT;i++)
        {
            if(server->client_list[i]==client)
            {
                server->client_list[i]=NULL;
                ar_delete_signal(p_client->signal);
                if(p_client->need_stats && p_client->stats_buffer)
                {
                    ar_dma_free(p_client->stats_buffer_orgin);
                }
                ar_free(p_client);
                break;
            }
        }
   } else {
        log_tag_err("no mem to creat client");
   }
   ar_unlock(server->lock);
   return 0;
}

static int init_irq()
{
    char mpp_dev[] = "/dev/ar_mpp_ctl";
    int fd = -1;
    if ((fd = open(mpp_dev, O_RDWR)) < 0) {
        log_tag_err("open %s failed, fd = %d", mpp_dev, fd);
    }

    return fd;
}

ge2d_server_t *get_ge2d_server()
{
    static ge2d_server_t *server=NULL;
    uint8_t *p=NULL;
    void *map_table=NULL;
    frame_cmd_t *p_frame=NULL;
    int mpp_dev_fd = init_irq();
    if (mpp_dev_fd < 0) {
        return NULL;
    }

    if (server==NULL)
    {
        log_tag_always("creat ge2d server");
        server=ar_malloc(sizeof(ge2d_server_t));
        memset(server,0,sizeof(ge2d_server_t));
        server->lock=ar_creat_lock();
        server->creat_client=creat_client;
        server->delete_client=delete_client;
        //frame pool
        server->ge2d_frame_pool_queue=ar_creat_queue(G2ED_FRAME_POOL_SIZE+2,"ge2d_frame_pool_queue");
        server->ge2d_frame_pool_queue_signal=ar_create_signal();
        //malloc frame buffer
        for(int i=0;i<G2ED_FRAME_POOL_SIZE;i++)
        {
            p=ar_dma_malloc(G2ED_FRAME_MAX_SIZE*sizeof(uint32_t)+sizeof(frame_cmd_t)+64);
            //p=ar_dma_malloc(G2ED_FRAME_MAX_SIZE*sizeof(uint32_t)+sizeof(frame_cmd_t));
            if(p)
            {
                memset(p,0,G2ED_FRAME_MAX_SIZE*sizeof(uint32_t)+sizeof(frame_cmd_t));
                p_frame=(frame_cmd_t *)p;
                server->frame[i]=p_frame;
                p_frame->phy_orgin_addr = ar_dma_va_to_pa(p);
                p_frame->frame_index=i;
                p_frame->frame_len=0;
                p_frame->frame_done=ar_create_signal();
                p_frame->frame_start_addr=(void*)((unsigned long)p+sizeof(frame_cmd_t));
                p_frame->frame_start_addr=(uint32_t *)AR_ALIGN64((unsigned long)p_frame->frame_start_addr);
                log_tag_always("p_frame %p, frame_start_addr %p, phy_orgin_addr %p",
                                p_frame, p_frame->frame_start_addr, p_frame->phy_orgin_addr);
                //p_frame->frame_end_addr=p_frame->frame_start_addr;
                        //malloc map table
                map_table=ar_dma_malloc(MAX_MAP_TABLE_SIZE+128);
                if(map_table){
                    p_frame->map_table_phy	= ar_dma_va_to_pa(map_table);
                    p_frame->map_table=(uint32_t *)AR_ALIGN128((unsigned long)map_table);
                    p_frame->map_table_phy=(uint32_t *)AR_ALIGN128((unsigned long)p_frame->map_table_phy);
                }else
                {
                    log_tag_err("mo map table space mem");
                }
                server->ge2d_frame_pool_queue->queue_insert(server->ge2d_frame_pool_queue,p);
            }
        }
        server->ge2d_main_queue=ar_creat_queue(G2ED_MAIN_QUEUE_SIZE+2,"ge2d_main_queue");
        server->ge2d_main_queue_signal=ar_create_signal();
        //init ge2d hw
        ge2d_hw_init();

        //creat a task to process task
        ar_os_thread_attr_t attr={0};
        attr.name="ge2d_server";
        attr.stack_size = 128*1024;
        ar_osal_thread_new(ge2d_server_task,server,&attr);

        server->mpp_dev_fd = mpp_dev_fd;
#if 0
        //register a rtcmd to debug the server
        //regsiter a cmd to set awb_debug
        CLI_Command_Definition_t *cmd_def=ar_malloc(sizeof(CLI_Command_Definition_t));
        cmd_def->pcCommand = "ge2d_server";
        cmd_def->pcHelpString = "ge2d_server: --help to show detail usage";
        cmd_def->pxCommandInterpreter =NULL;
        cmd_def->pxCommandInterpreter_ext=ge2d_server_debug;
        cmd_def->usr_data=server;
        cmd_def->cExpectedNumberOfParameters = -1;
        FreeRTOS_CLIRegisterCommand(cmd_def);
#endif

        return server;
    }

    return server;
}


