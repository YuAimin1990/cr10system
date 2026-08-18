#include "utils_debug.h"
#include <pthread.h>
#include "ar_tunning_service.h"
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include "cJSON.h"
#include <unistd.h>
#include "hal_vin.h"
#include "fifo.h"
#include "hal_vin_log.h"
#include "iq_main_service.h"

#define FPGA_SIMULATION_9301

#define USE_HDR_DDR_MODE
#define VIDIOC_AR_CAM_TUNING_REQ \
    _IOWR('V', BASE_VIDIOC_PRIVATE + 1, uint32_t)

#define VIDIOC_AR_CAM_SET_OPEN_PROPERTY \
    _IOWR('V', BASE_VIDIOC_PRIVATE + 2, open_property_t)

#define VIDIOC_AR_CAM_SET_MAX_FPS \
    _IOWR('V', BASE_VIDIOC_PRIVATE + 3, float)

static tuning_server_t *tuning_obj[AR_DBG_MSG_MAX]={NULL};
static pthread_mutex_t g_lock_tuning_server = PTHREAD_MUTEX_INITIALIZER;
#define ION_HEAP_MASK (1 << 23)
#define CAM_NUM_MAX (8)
#define get_req_pra(msg) ((msg)+sizeof(tuning_request_t))
#define get_ack_pra(ack) ((ack)+sizeof(tuning_ack_t))
static void *send_thread_priview(void *data);
static void *send_thread_video(void *data);
static void *send_thread_raw(void *data);
static void *send_thread_af(void *data);
static void *q_stream_buf_thread_assist(void *data);
static tuning_server_t *get_tuning_server(int msg_id);

#define CAM_ALIGNE_TO(size,num) ( (num)*( ( (size)+(num)-1)/(num) ) )
#define BUF_STRIDE_ALIGN (256)
#define BUF_ALIGN   (512)

static int send_l(void *client,uint8_t *play_load,uint32_t len,uint8_t msg_id, uint8_t ecc_flag)
{
    tuning_server_t *obj=get_tuning_server(msg_id);

    pthread_mutex_lock(&obj->mutex_stop);
    if(obj&&obj->send_server&&obj->client)
    {
       obj->send_server(client,play_load,len,msg_id,ecc_flag);
    }

    pthread_mutex_unlock(&obj->mutex_stop);
    return 0;
}
static tuning_server_t *get_tuning_server(int msg_id)
{
    tuning_server_t *obj=NULL;
    int ret = 0;
    pthread_mutex_lock(&g_lock_tuning_server);
    if(!tuning_obj[msg_id])
    {
        ar_always("creat the tuning obj");
        tuning_obj[msg_id]=(tuning_server_t *)malloc(sizeof(tuning_server_t));
        if(NULL == tuning_obj[msg_id])
        {
            ar_err("malloc tuning server failed");
            return NULL;
        }
        obj=tuning_obj[msg_id];
        memset(obj,0,sizeof(tuning_server_t));
        obj->fd_mem=-1;
        obj->dev_mem_fd = open("/dev/ar_sys", O_RDWR | O_SYNC);
        if(obj->dev_mem_fd<0){
            ar_err("open /dev/ar_sys");
        }

        obj->page_size = getpagesize();
        if(obj->page_size == 0)
        {
            ar_err("fail to get page size");
            obj->page_size = 4096;
        }

        //creat send task mux
        pthread_cond_init(&obj->cond_privew,NULL);
        pthread_cond_init(&obj->cond_video,NULL);
        pthread_cond_init(&obj->cond_raw,NULL);
        pthread_cond_init(&obj->cond_hdr_raw,NULL);
        pthread_cond_init(&obj->qready,NULL);		
        pthread_cond_init(&obj->cond_af,NULL);
        //creat a mutx
        pthread_mutex_init(&obj->mutex,NULL);
        pthread_mutex_init(&obj->mutex_stop,NULL);
        pthread_mutex_init(&obj->qlock, NULL);
        obj->q_dq_event = ar_creat_queue(MAX_MEM_FIFO,"q_dq_event");
        obj->queue_priview=ar_creat_queue(MAX_MEM_FIFO,"prview");
        obj->queue_video=ar_creat_queue(MAX_MEM_FIFO,"video");
        obj->queue_raw=ar_creat_queue(MAX_MEM_FIFO,"raw");
        obj->queue_hdr_raw=ar_creat_queue(MAX_MEM_FIFO,"raw");
        obj->queue_af=ar_creat_queue(MAX_MEM_FIFO,"af");

        //creat a task to send buffer
        pthread_create(&obj->tid_priview,NULL,send_thread_priview,obj);
        pthread_create(&obj->tid_video,NULL,send_thread_video,obj);
        pthread_create(&obj->tid_raw,NULL,send_thread_raw,obj);		
        pthread_create(&obj->tid_af,NULL,send_thread_af,obj);
		
        obj->send=send_l;
    }
    pthread_mutex_unlock(&g_lock_tuning_server);
    return tuning_obj[msg_id];
}

static int get_format_bitdpth(int format)
{
    int bit_depth=0;
    switch(format)
    {
        case STREAM_FORMAT_YUV420_8BIT_Plannar:
        bit_depth=8;
        break;

        case STREAM_FORMAT_RAW_UNPACKED_10BIT_RGGB:
		case STREAM_FORMAT_RAW_UNPACKED_10BIT_BGGR:
		case STREAM_FORMAT_RAW_UNPACKED_10BIT_GRBG:
		case STREAM_FORMAT_RAW_UNPACKED_10BIT_GBRG:
		case STREAM_FORMAT_RAW_UNPACKED_12BIT_RGGB:
        case STREAM_FORMAT_RAW_UNPACKED_12BIT_BGGR:
        case STREAM_FORMAT_RAW_UNPACKED_12BIT_GRBG:
        case STREAM_FORMAT_RAW_UNPACKED_12BIT_GBRG:
        case STREAM_FORMAT_RAW_UNPACKED_14BIT_RGGB:
        case STREAM_FORMAT_RAW_UNPACKED_14BIT_BGGR:
        case STREAM_FORMAT_RAW_UNPACKED_14BIT_GRBG:
        case STREAM_FORMAT_RAW_UNPACKED_14BIT_GBRG:
        case STREAM_FORMAT_RAW_UNPACKED_16BIT_RGGB:
        case STREAM_FORMAT_RAW_UNPACKED_16BIT_BGGR:
        case STREAM_FORMAT_RAW_UNPACKED_16BIT_GRBG:
        case STREAM_FORMAT_RAW_UNPACKED_16BIT_GBRG:
        bit_depth=16;
        break;
        default:
        break;
    }
    return bit_depth;
}

static int process_connect_assist(tuning_server_t *obj, STRU_TUNING_REQ_CONNECT_PRA_T *pra)
{
    ar_always("%s",pra->diag);
    int status=0;
    // int on=0;
    if(!obj->obj)
    {
	    ar_log_init();

        obj->vin_fd=ar_hal_vin_open_vin_dev();
        if(obj->vin_fd < 0)
        {
           ar_err("open vin device err");
		   status=TUNING_ERR_UNKNOW;		   
		   return status;
        }
        //get ddr rw burst len
        STRU_VIN_DRIVER_START_OPT_T *p_opt = NULL;
        p_opt = ( STRU_VIN_DRIVER_START_OPT_T *)malloc(sizeof(STRU_VIN_DRIVER_START_OPT_T));

		if(!p_opt)
		{
		   ar_hal_vin_close_vin_dev(obj->vin_fd);
		   return -1;
		}
		
        int ret = ar_hal_vin_driver_get_start_opt(obj->vin_fd, p_opt);
        if(ret < 0)
        {
            ar_err("failed to get opt, use default ddr rw busrt len %d\n", DEFAULT_DDR_RW_BURST_LEN);
            obj->ddr_rw_burest_len = DEFAULT_DDR_RW_BURST_LEN;
        }
        else
        {

            obj->ddr_rw_burest_len = p_opt->ddr_rw_burst_len;
        }
        ar_always("ddr rw burst len %d", obj->ddr_rw_burest_len);
        free(p_opt);
		
        status=0;
        obj->obj=(cam_obj_t *)malloc(sizeof(cam_obj_t));

        if(!obj->obj)
        {
           ar_hal_vin_close_vin_dev(obj->vin_fd);
		   return -1;
        }
        memset(obj->obj,0,sizeof(cam_obj_t));
        obj->is_first_frame=1;
		obj->fd_assist_mod_bitstram=-1;
		obj->server_status=SERVER_CONNECTED;

    }
    else
    {
        ar_err("can not connect twice");
        status=TUNING_ERR_UNKNOW;
    }
    log_func_exit();
    return status;
}



static int process_connect_ack(tuning_server_t *obj, STRU_TUNING_REQ_CONNECT_PRA_T *pra, int ack_status)
{
    log_func_enter();
    int ack_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_CONNECT_ACK_T);
    tuning_ack_t *ack=(tuning_ack_t *)malloc(ack_len);
	if(!ack)
    {
      return -1;
    }
    STRU_TUNING_REQ_CONNECT_ACK_T *ack_pra=(STRU_TUNING_REQ_CONNECT_ACK_T *)get_ack_pra(((uint8_t *)ack));
    ack->request_type=TUNING_REQ_CONNECT;
    ack->error_code=ack_status;
    ack->len=sizeof(STRU_TUNING_REQ_CONNECT_ACK_T);
    //fill the ack pra by req return
    ack_pra->request_type=pra->request_type;

    STRU_VIN_DRIVER_START_OPT_T *opt=malloc(sizeof(STRU_VIN_DRIVER_START_OPT_T));

	if(!opt)
	{	  
	  free(ack);
	  return -1;
	}

    ar_hal_vin_driver_get_start_opt(obj->vin_fd, opt);
	
    if(ack_status==TUNING_ERR_SUCCESS)
	{
        sprintf(ack_pra->diag,"connected success --> [%s] version:%x",pra->diag,opt->tuning_version);
    }
    else
    {
        sprintf(ack_pra->diag,"connected failed --> [%s] version:%x",pra->diag,opt->tuning_version);
    }
    ar_always("%s",ack_pra->diag);
    obj->send(obj->client,(uint8_t *)ack,ack_len,obj->msg_id,0);
    free(ack);
	free(opt);
    log_func_exit();
    return 0;
}

static AR_S32 process_get_sensor_list_assist(tuning_server_t *obj, STRU_TUNING_REQ_GET_CAM_LSIT_PRA_T *pra)
{
    AR_S32 ret = 0, cam_num = 0;
    STRU_SENSOR_DEV_INFO_T cam_info;

    ret=ar_hal_vin_get_dev_num(obj->vin_fd, &cam_num);

	ar_always("get camera %d",cam_num);


	for(int i = 0; i < cam_num; i++)
	{
	    //each device info
		ar_hal_vin_get_dev_infor(obj->vin_fd, i, &cam_info);
		strcpy(pra->sensor[i], cam_info.sensor_name);
		strcpy(pra->dev_node[i], cam_info.sensor_name);
        strcpy(obj->sensor[i], pra->sensor[i]);
        int opened=0;
        //which device in open
        ret=ar_hal_vin_dev_is_opend(obj->vin_fd, i, &opened);
        if(ret<0 || opened)
        {
            ar_always("the device id = %d have started ret=%d opend=%d",i,ret,opened);
            //if opened then qurey how many stream have created.
            if(opened)
            {
               int pipe_id=0;
               ret=ar_hal_vin_bind_get_pipe_id_by_dev_id(obj->vin_fd, i ,&pipe_id);
               ar_always("dev id =%d pipe_id=%d", i, pipe_id);
               if(ret>=0 && pipe_id>=0)
               {
                  obj->pipe_id[i] = pipe_id;
            	  int fd_pipe=ar_hal_vin_open_pipe_dev(pipe_id);
            	  if(fd_pipe >= 0)
            	  {
            		  int stream_num=0;
            		  int stream_id[MAX_PIPE_NUM];
            		  obj->pipe_fd[i] = fd_pipe;
                      ar_hal_vin_get_all_stream(fd_pipe, pipe_id, stream_id, &stream_num);
                      for (int k = 0; k < stream_num; k++)
                      {
                          obj->stream_id[i][k] = stream_id[k];

                          ret = ar_hal_vin_open_stream_dev(pipe_id,stream_id[k]);
                          if (ret > 0)
                          {
                                obj->stream_fd[i][k] = ret;
                          }
                      }

                  }
               }
             }
         }
         ar_always("cam-%d :%s", i, pra->sensor[i]);
    }
    pra->count = cam_num;
    pra->request_type = TUNING_REQ_GET_CAM_LSIT;
    obj->probe_sensor_num=pra->count;

	return TUNING_ERR_SUCCESS;
}

static int process_get_sensor_list_ack(tuning_server_t *obj, STRU_TUNING_REQ_GET_CAM_LSIT_PRA_T *pra,int ack_status)
{
    log_func_enter();
    int i=0;
    int ack_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_GET_CAM_LSIT_ACK_T);
    tuning_ack_t *ack=(tuning_ack_t *)malloc(ack_len);
	if(!ack)
    {
      return -1;
    }
    STRU_TUNING_REQ_GET_CAM_LSIT_ACK_T *ack_pra=(STRU_TUNING_REQ_GET_CAM_LSIT_ACK_T *)get_ack_pra(((uint8_t *)ack));
    ack->request_type=TUNING_REQ_GET_CAM_LSIT;
    ack->error_code=ack_status;
    ack->len=sizeof(STRU_TUNING_REQ_GET_CAM_LSIT_ACK_T);
    //fill the ack pra by req return
    ack_pra->count=obj->probe_sensor_num;
    for(i=0;i<obj->probe_sensor_num;i++){
        strcpy(ack_pra->sensor[i],pra->sensor[i]);
    }
    obj->send(obj->client,(uint8_t *)ack,ack_len,obj->msg_id,0);
    log_func_exit();
    return 0;
}

static int get_buffer_width(tuning_server_t *obj,uint32_t width,int bits)
{
   if(NULL == obj)
   {
      ar_err("fail to get tuning server");
      return 0;
   }

  uint32_t bit_num_per_burst=DDR_RW_BITS_PER_BURST;
  uint32_t bitdepth=bits;
  uint32_t burst_num=obj->ddr_rw_burest_len*Y_UV_DIV_FACTOR;
  uint32_t tmp_width=CAM_ALIGNE_TO(width,(bit_num_per_burst/bitdepth));
  uint32_t ret_width=CAM_ALIGNE_TO(tmp_width/(bit_num_per_burst/bitdepth),burst_num)*bit_num_per_burst/8;
  return ret_width;
}

static int get_buffer_width_raw( tuning_server_t *obj,uint32_t width,int bits)
{
	 if(NULL == obj)
	 {
		ar_err("fail to get tuning server");
		return 0;
	 }

	uint32_t bit_num_per_burst=DDR_RW_BITS_PER_BURST;
	uint32_t bitdepth=bits;
	uint32_t burst_num=obj->ddr_rw_burest_len;
	uint32_t tmp_width=CAM_ALIGNE_TO(width,(bit_num_per_burst/bitdepth));
	uint32_t ret_width=CAM_ALIGNE_TO(tmp_width/(bit_num_per_burst/bitdepth),burst_num)*bit_num_per_burst/8;
	return ret_width;
}
static int get_buffer_width_div2(tuning_server_t *obj,uint32_t width,int bits)
{
   if(NULL == obj)
   {
      ar_err("fail to get tuning server");
      return 0;
   }

  uint32_t bit_num_per_burst=DDR_RW_BITS_PER_BURST;
  uint32_t bitdepth=bits;
  uint32_t burst_num=obj->ddr_rw_burest_len;
  uint32_t tmp_width=CAM_ALIGNE_TO(width,(bit_num_per_burst/bitdepth));
  uint32_t ret_width=CAM_ALIGNE_TO(tmp_width/(bit_num_per_burst/bitdepth),burst_num)*bit_num_per_burst/8;
  return ret_width;
}

static int get_pix_align_count(uint32_t width,int bits)
{
  uint32_t bit_num_per_burst=DDR_RW_BITS_PER_BURST;
  uint32_t bitdepth=bits;
  uint32_t pix_count_per_burst=bit_num_per_burst/bitdepth;
  return ((width+pix_count_per_burst-1)/pix_count_per_burst)*pix_count_per_burst;
}

static int send_preview_to_pctool(tuning_server_t *obj,send_v4l2_buffer_t *send_buffer)
{
   // log_func_enter();
   unsigned long y_addr, cb_addr, cr_addr;
   tuning_stream_t *stream=&obj->obj->stream[TUNING_STREAM_PREVIEW];
   int ack_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_PREVIEW_FRAME_ACK_T)+stream->width;
   tuning_ack_t *ack=(tuning_ack_t *)malloc(ack_len);
   if(!ack)
   {
      return -1;
   }
   STRU_TUNING_REQ_PREVIEW_FRAME_ACK_T *ack_pra=(STRU_TUNING_REQ_PREVIEW_FRAME_ACK_T *)get_ack_pra(((uint8_t *)ack));
   uint8_t *p_frame_data=(uint8_t *)ack +(sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_PREVIEW_FRAME_ACK_T));
   ack->request_type=TUNING_REQ_PREVIEW_FRAME;
   ack->error_code=0;
   ack->len=sizeof(STRU_TUNING_REQ_PREVIEW_FRAME_ACK_T);
   //fill the ack pra by req return
   ack_pra->request_type=ack->request_type;
   ack_pra->panels=3;
   ack_pra->burst_count[0]=stream->height;
   ack_pra->burst_count[1]=stream->height/2;
   ack_pra->burst_count[2]=stream->height/2;
   ack_pra->data_flag=1;
   ack_pra->width=stream->width;
   ack_pra->height=stream->height;
   ack_pra->format=stream->format;


   y_addr= (unsigned long)send_buffer->mem.va[0];
   cb_addr=(unsigned long)send_buffer->mem.va[1];
   cr_addr=(unsigned long)send_buffer->mem.va[2];

   ar_always("%lx %lx %lx",y_addr,cb_addr,cr_addr);

   //send to client
   //send y

   if(stream->send_data_mod==STREAM_MODE_YUV)
   {
       int ack_real_len=0;
	   int i=0;
       // ar_always("send y");
       int line_len=get_buffer_width(obj,stream->width,stream->bit_depth);
       for(i=0;i<stream->height;i++){
           ack_pra->burst_ref=i;
           ack_pra->yuv_panel=0;
           ack_pra->burst_len=stream->width;
           ack_real_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_PREVIEW_FRAME_ACK_T)+ack_pra->burst_len;
           memcpy(p_frame_data,(void *)(y_addr+line_len*i),ack_pra->burst_len);
           obj->send(obj->client,(uint8_t *)ack,ack_real_len,obj->msg_id,0);

       }
       //send u
       // ar_always("send u");
       line_len=get_buffer_width_div2(obj,stream->width/2,stream->bit_depth);
       for(i=0;i<stream->height/2;i++){
           ack_pra->burst_ref=i;
           ack_pra->yuv_panel=1;
           ack_pra->burst_len=stream->width/2;
           ack_real_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_PREVIEW_FRAME_ACK_T)+ack_pra->burst_len;
           memcpy(p_frame_data,(void *)(cb_addr+line_len*i),ack_pra->burst_len);
           obj->send(obj->client,(uint8_t *)ack,ack_real_len,obj->msg_id,0);
       }
       //send v
       // ar_always("send v");
       line_len=get_buffer_width_div2(obj,stream->width/2,stream->bit_depth);
       for(i=0;i<stream->height/2;i++){
           ack_pra->burst_ref=i;
           ack_pra->yuv_panel=2;
           ack_pra->burst_len=stream->width/2;
           memcpy(p_frame_data,(void *)(cr_addr+line_len*i),ack_pra->burst_len);
           ack_real_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_PREVIEW_FRAME_ACK_T)+ack_pra->burst_len;
           obj->send(obj->client,(uint8_t *)ack,ack_real_len,obj->msg_id,0);
       }
    }
    else //send bitstream
    {

    }
    free(ack);
    // log_func_exit();
    return 0;
}
static int send_video_to_pctool(tuning_server_t *obj,send_v4l2_buffer_t *send_buffer)
{
    unsigned long y_addr, cb_addr, cr_addr;

    tuning_stream_t *stream=&obj->obj->stream[TUNING_STREAM_VIDEO];
    int ack_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_SNAPSHOT_YUV_FRAME_ACK_T)+stream->width;
    tuning_ack_t *ack=(tuning_ack_t *)malloc(ack_len);
	if(!ack)
    {
      return -1;
    }
    STRU_TUNING_REQ_SNAPSHOT_YUV_FRAME_ACK_T *ack_pra=(STRU_TUNING_REQ_SNAPSHOT_YUV_FRAME_ACK_T *)get_ack_pra(((uint8_t *)ack));
    uint8_t *p_frame_data=(uint8_t *)ack +(sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_SNAPSHOT_YUV_FRAME_ACK_T));
    ack->request_type=TUNING_REQ_SNAPSHOT_YUV_FRAME;
    ack->error_code=0;
    ack->len=sizeof(STRU_TUNING_REQ_SNAPSHOT_YUV_FRAME_ACK_T);
    //fill the ack pra by req return
    ack_pra->request_type=ack->request_type;
    ack_pra->panels=3;
    ack_pra->burst_count[0]=stream->height;
    ack_pra->burst_count[1]=stream->height/2;
    ack_pra->burst_count[2]=stream->height/2;
    ack_pra->data_flag=1;
    ack_pra->width=stream->width;
    ack_pra->height=stream->height;
    ack_pra->format=stream->format;

    int i=0;
    //send to client
    int ack_real_len=0;
    int line_len=0;
    if(stream->send_data_mod==STREAM_MODE_YUV)
    {
        y_addr= (unsigned long)send_buffer->mem.va[0];
        cb_addr=(unsigned long)send_buffer->mem.va[1];
        cr_addr=(unsigned long)send_buffer->mem.va[2];
        ar_always("cpy addr %lx %lx %lx",y_addr,cb_addr,cr_addr);
        line_len=get_buffer_width(obj,stream->width,stream->bit_depth);
        ack_pra->frame_id=0;
        ar_always("send y");
        for(i=0;i<stream->height&&y_addr;i++){
            ack_pra->burst_ref=i;
            ack_pra->yuv_panel=0;
            ack_pra->burst_len=stream->width;
            ack_real_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_SNAPSHOT_YUV_FRAME_ACK_T)+ack_pra->burst_len;
            memcpy(p_frame_data,(void *)(y_addr+line_len*i),ack_pra->burst_len);
            obj->send(obj->client,(uint8_t *)ack,ack_real_len,obj->msg_id,0);
        }
        //send u
        ar_always("send u");
        line_len=get_buffer_width_div2(obj,stream->width/2,stream->bit_depth);
        for(i=0;i<stream->height/2&&cb_addr;i++){
            ack_pra->burst_ref=i;
            ack_pra->yuv_panel=1;
            ack_pra->burst_len=stream->width/2;
            ack_real_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_SNAPSHOT_YUV_FRAME_ACK_T)+ack_pra->burst_len;
            memcpy(p_frame_data,(void *)(cb_addr+line_len*i),ack_pra->burst_len);
            obj->send(obj->client,(uint8_t *)ack,ack_real_len,obj->msg_id,0);
        }
        //send v
        ar_always("send v");
        line_len=get_buffer_width_div2(obj,stream->width/2,stream->bit_depth);
        for(i=0;i<stream->height/2&&cr_addr;i++){
            ack_pra->burst_ref=i;
            ack_pra->yuv_panel=2;
            ack_pra->burst_len=stream->width/2;
            memcpy(p_frame_data,(void *)(cr_addr+line_len*i),ack_pra->burst_len);
            ack_real_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_SNAPSHOT_YUV_FRAME_ACK_T)+ack_pra->burst_len;
            obj->send(obj->client,(uint8_t *)ack,ack_real_len,obj->msg_id,0);
        }

    }
    else
    {
       ar_err("err send data mode");
    }
    free(ack);
    return 0;
}

static int convert_line_to_dng_line(uint16_t *dest,uint8_t *src,int bitdepth,int align_pixs)
{
    int i=0;
    uint64_t low_128=0;
    uint64_t high_128=0;
    uint16_t *p_16=(uint16_t *)src;
    int j=0;
    switch(bitdepth)
    {
        case 8:
        {
            for(i=0;i<align_pixs;i++)
            {
                dest[i]=src[i];
            }
        }
        break;
        case 10:
        {
            j=0;
            for(i=0;i<align_pixs;i+=12)
            {
                low_128=*((uint64_t *)(src+j*16));
                high_128=*((uint64_t *)(src+j*16+8));
                dest[i]=low_128&0x3ff;
                dest[i+1]=(low_128>>10)&0x3ff;
                dest[i+2]=(low_128>>20)&0x3ff;
                dest[i+3]=(low_128>>30)&0x3ff;
                dest[i+4]=(low_128>>40)&0x3ff;
                dest[i+5]=(low_128>>50)&0x3ff;
                dest[i+6]=((low_128>>60)&0xf)|((high_128&0x3f)<<4);
                dest[i+7]=(high_128>>6)&0x3ff;
                dest[i+8]=(high_128>>16)&0x3ff;
                dest[i+9]=(high_128>>26)&0x3ff;
                dest[i+10]=(high_128>>36)&0x3ff;
                dest[i+11]=(high_128>>46)&0x3ff;
                j++;
            }
        }
        break;
        case 12:
        {
            j=0;
            for(i=0;i<align_pixs;i+=10)
            {
                low_128=*((uint64_t *)(src+j*16));
                high_128=*((uint64_t *)(src+j*16+8));
                dest[i]=low_128&0xfff;
                dest[i+1]=(low_128>>12)&0xfff;
                dest[i+2]=(low_128>>24)&0xfff;
                dest[i+3]=(low_128>>36)&0xfff;
                dest[i+4]=(low_128>>48)&0xfff;
                dest[i+5]=((low_128>>60)&0xf)|((high_128&0xff)<<4);
                dest[i+6]=(high_128>>8)&0xfff;
                dest[i+7]=(high_128>>20)&0xfff;
                dest[i+8]=(high_128>>32)&0xfff;
                dest[i+9]=(high_128>>44)&0xfff;
                j++;
            }
        }
        break;
        case 14:
        {
            j=0;
            for(i=0;i<align_pixs;i+=9)
            {
                low_128=*((uint64_t *)(src+j*16));
                high_128=*((uint64_t *)(src+j*16+8));
                dest[i]=low_128&0x3fff;
                dest[i+1]=(low_128>>14)&0x3fff;
                dest[i+2]=(low_128>>28)&0x3fff;
                dest[i+3]=(low_128>>42)&0x3fff;
                dest[i+4]=((low_128>>56)&0xff)|((high_128&0x3f)<<8);
                dest[i+5]=(high_128>>6)&0x3fff;
                dest[i+6]=(high_128>>20)&0x3fff;
                dest[i+7]=(high_128>>34)&0x3fff;
                dest[i+8]=(high_128>>48)&0x3fff;
                j++;
            }
        }
        break;
        case 16:
        {
            for(i=0;i<align_pixs;i++)
            {
                dest[i]=p_16[i];
            }
        }
        break;
        default:
            ar_err("not supported bitdepth %d",bitdepth);
            break;
    }
    return 0;
}
static const char *get_raw_format_string(int format)
{
    const char *format_string="unknow format";
    switch(format)
    {
		case STREAM_FORMAT_RAW_UNPACKED_10BIT_RGGB:
		format_string="STREAM_FORMAT_RAW_UNPACKED_10BIT_RGGB";
		break;
        case STREAM_FORMAT_RAW_UNPACKED_10BIT_BGGR:
		format_string="STREAM_FORMAT_RAW_UNPACKED_10BIT_BGGR";
		break;
        case STREAM_FORMAT_RAW_UNPACKED_10BIT_GRBG:
		format_string="STREAM_FORMAT_RAW_UNPACKED_10BIT_GRBG";
		break;
        case STREAM_FORMAT_RAW_UNPACKED_10BIT_GBRG:
		format_string="STREAM_FORMAT_RAW_UNPACKED_10BIT_GBRG";
		break;
		case STREAM_FORMAT_RAW_UNPACKED_12BIT_RGGB:
		format_string="STREAM_FORMAT_RAW_UNPACKED_12BIT_RGGB";
		break;
        case STREAM_FORMAT_RAW_UNPACKED_12BIT_BGGR:
		format_string="STREAM_FORMAT_RAW_UNPACKED_12BIT_BGGR";
		break;
        case STREAM_FORMAT_RAW_UNPACKED_12BIT_GRBG:
		format_string="STREAM_FORMAT_RAW_UNPACKED_12BIT_GRBG";
		break;
        case STREAM_FORMAT_RAW_UNPACKED_12BIT_GBRG:
		format_string="STREAM_FORMAT_RAW_UNPACKED_12BIT_GBRG";
		break;
        case STREAM_FORMAT_RAW_UNPACKED_14BIT_RGGB:
		format_string="STREAM_FORMAT_RAW_UNPACKED_14BIT_RGGB";
		break;
        case STREAM_FORMAT_RAW_UNPACKED_14BIT_BGGR:
		format_string="STREAM_FORMAT_RAW_UNPACKED_14BIT_BGGR";
		break;
        case STREAM_FORMAT_RAW_UNPACKED_14BIT_GRBG:
		format_string="STREAM_FORMAT_RAW_UNPACKED_14BIT_GRBG";
		break;
        case STREAM_FORMAT_RAW_UNPACKED_14BIT_GBRG:
		format_string="STREAM_FORMAT_RAW_UNPACKED_14BIT_GBRG";
		break;
        case STREAM_FORMAT_RAW_UNPACKED_16BIT_RGGB:
		format_string="STREAM_FORMAT_RAW_UNPACKED_16BIT_RGGB";
		break;
        case STREAM_FORMAT_RAW_UNPACKED_16BIT_BGGR:
		format_string="STREAM_FORMAT_RAW_UNPACKED_16BIT_BGGR";
		break;
        case STREAM_FORMAT_RAW_UNPACKED_16BIT_GRBG:
		format_string="STREAM_FORMAT_RAW_UNPACKED_16BIT_GRBG";
		break;
        case STREAM_FORMAT_RAW_UNPACKED_16BIT_GBRG:
		format_string="STREAM_FORMAT_RAW_UNPACKED_16BIT_GBRG";
		break;
	    break;
		default:
		break;
    }

    return format_string;
}

static int send_raw_to_pctool(tuning_server_t *obj,send_v4l2_buffer_t *send_buffer)
{
    log_func_enter();
    unsigned long y_addr, cb_addr, cr_addr;
    tuning_stream_t *stream=&obj->obj->stream[TUNING_STREAM_RAW];
    int line_len=get_buffer_width_raw(obj,stream->width,stream->bit_depth); //to do bitdeth with imx291 12bit, later change it with format
    int pix_count_align=get_pix_align_count(stream->width,stream->bit_depth);
    int line_len_unpacked=pix_count_align*2;
    int real_line_len_unpacked=stream->width*2;
    int ack_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_RAW_FRAME_ACK_T)+line_len_unpacked+sizeof(STRU_AR_RAW_HEADER_T);
    tuning_ack_t *ack=(tuning_ack_t *)malloc(ack_len);
	if(!ack)
    {
      return -1;
    }
    STRU_TUNING_REQ_RAW_FRAME_ACK_T *ack_pra=(STRU_TUNING_REQ_RAW_FRAME_ACK_T *)get_ack_pra(((uint8_t *)ack));
    uint8_t *p_frame_data=(uint8_t *)ack +(sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_RAW_FRAME_ACK_T));
    ack->request_type=TUNING_REQ_RAW_FRAME;
    ack->error_code=0;
    ack->len=sizeof(STRU_TUNING_REQ_RAW_FRAME_ACK_T);
    //fill the ack pra by req return
    ack_pra->request_type=ack->request_type;
    ack_pra->panels=1;
    ack_pra->burst_count[0]=stream->height;
    ack_pra->burst_count[1]=0;
    ack_pra->burst_count[2]=0;
    ack_pra->data_flag=1;
    ack_pra->width=stream->width;
    ack_pra->height=stream->height;
    ack_pra->format=stream->format;

    ar_always("w %d, h %d format 0x%x", ack_pra->width, ack_pra->height, ack_pra->format);


	y_addr= (unsigned long)send_buffer->mem.va[0];
	cb_addr=0;
	cr_addr=0;


    ar_always("%lx %lx %lx",y_addr,cb_addr,cr_addr);

    int ack_real_len=0;
    if(stream->send_data_mod==STREAM_MODE_RAW)
    {
        y_addr=(unsigned long)send_buffer->mem.va[0];
        ack_pra->frame_id=0;
        //send a raw header first
        STRU_AR_RAW_HEADER_T header;
		header.w=stream->width;
		header.h=stream->height;
		strcpy(header.format_sting,get_raw_format_string(stream->format));
		header.data_offset=sizeof(STRU_AR_RAW_HEADER_T);
		header.basic_info=send_buffer->basic_info;

		ack_pra->burst_ref=0;
		ack_pra->yuv_panel=0;
		ack_pra->burst_len=sizeof(STRU_AR_RAW_HEADER_T);
		ack_real_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_RAW_FRAME_ACK_T)+ack_pra->burst_len;
		memcpy(p_frame_data,(void *)&header,ack_pra->burst_len);
        if(obj->obj->with_3a)
        {
            ar_always("send 3a with size %d before raw frame", ack_pra->burst_len);
            obj->send(obj->client, (uint8_t *)ack, ack_real_len, obj->msg_id, 0);
        }

        ar_always("send y %lx id=%d pix_count_align=%d",y_addr,ack_pra->frame_id,pix_count_align);
        for(int i=0;i<stream->height&&y_addr;i++){
            ack_pra->burst_ref=i;
            ack_pra->yuv_panel=0;
            ack_pra->burst_len=real_line_len_unpacked;
            ack_real_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_RAW_FRAME_ACK_T)+ack_pra->burst_len;
            //memcpy(p_frame_data,(void *)(y_addr+line_len*i),ack_pra->burst_len);
            convert_line_to_dng_line((uint16_t *)p_frame_data,(uint8_t *)(y_addr+line_len*i),stream->bit_depth,pix_count_align);
            obj->send(obj->client,(uint8_t *)ack,ack_real_len,obj->msg_id,0);
        }
    }
    else{
        ar_err("err send data mode");
    }
    free(ack);
    log_func_exit();
    return 0;
}


static int send_af_to_pctool(tuning_server_t *obj,send_v4l2_buffer_t *send_buffer)
{
    log_func_enter();
    unsigned long y_addr, cb_addr, cr_addr;
    int line_len=sizeof(STRU_AF_ALGO_LIB_INPUT_T)+sizeof(STRU_AF_ALGO_LIB_OUTPUT_T); //to do bitdeth with imx291 12bit, later change it with format
    int ack_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_AF_FRAME_ACK_T)+line_len;
    tuning_ack_t *ack=(tuning_ack_t *)malloc(ack_len);
	if(!ack)
    {
      return -1;
    }
    STRU_TUNING_REQ_AF_FRAME_ACK_T *ack_pra=(STRU_TUNING_REQ_AF_FRAME_ACK_T *)get_ack_pra(((uint8_t *)ack));
    uint8_t *p_frame_data=(uint8_t *)ack +(sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_AF_FRAME_ACK_T));
    ack->request_type=TUNING_REQ_AF_FRAME;
    ack->error_code=0;
    ack->len=sizeof(STRU_TUNING_REQ_AF_FRAME_ACK_T);
    //fill the ack pra by req return
    ack_pra->request_type=ack->request_type;
    ack_pra->panels=1;
    ack_pra->burst_count[0]=1;
    ack_pra->burst_count[1]=0;
    ack_pra->burst_count[2]=0;
    ack_pra->data_flag=1;
    ack_pra->width=16;
    ack_pra->height=9;
    ack_pra->format=0;

    ar_always("w %d, h %d format 0x%x", ack_pra->width, ack_pra->height, ack_pra->format);

	y_addr= (unsigned long)send_buffer->mem.va[0];
	cb_addr=0;
	cr_addr=0;


    ar_always("%lx %lx %lx",y_addr,cb_addr,cr_addr);

    int ack_real_len=0;
    //if(stream->send_data_mod==STREAM_MODE_RAW)
    {
        y_addr=(unsigned long)send_buffer->mem.va[0];
        ack_pra->frame_id=0;

        ar_always("send y %lx id=%d line_len=%d",y_addr,ack_pra->frame_id,line_len);
        for(int i=0;i<1&&y_addr;i++){
            ack_pra->burst_ref=i;
            ack_pra->yuv_panel=0;
            ack_pra->burst_len=line_len;
            ack_real_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_RAW_FRAME_ACK_T)+ack_pra->burst_len;
            memcpy(p_frame_data,(void *)(y_addr+line_len*i),ack_pra->burst_len);
            obj->send(obj->client,(uint8_t *)ack,ack_real_len,obj->msg_id,0);
        }
    }
   
    free(ack);
    log_func_exit();
    return 0;
}


static int send_hdr_raw_to_pctool(tuning_server_t *obj,send_v4l2_buffer_t *send_buffer)
{
    log_func_enter();
    unsigned long y_addr, cb_addr, cr_addr;
    tuning_stream_t *stream=&obj->obj->stream[TUNING_STREAM_HDR_RAW];
    int line_len=get_buffer_width(obj,stream->width,stream->bit_depth); //to do bitdeth with imx291 12bit, later change it with format
    int pix_count_align=get_pix_align_count(stream->width,stream->bit_depth);
    int line_len_unpacked=pix_count_align*2;
    int real_line_len_unpacked=stream->width*2;
    int ack_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_RAW_FRAME_ACK_T)+line_len_unpacked;
    tuning_ack_t *ack=(tuning_ack_t *)malloc(ack_len);
	if(!ack)
    {
      return -1;
    }
    STRU_TUNING_REQ_RAW_FRAME_ACK_T *ack_pra=(STRU_TUNING_REQ_RAW_FRAME_ACK_T *)get_ack_pra(((uint8_t *)ack));
    uint8_t *p_frame_data=(uint8_t *)ack +(sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_RAW_FRAME_ACK_T));
    ack->request_type=TUNING_REQ_HDR_RAW_FRAME;
    ack->error_code=0;
    ack->len=sizeof(STRU_TUNING_REQ_RAW_FRAME_ACK_T);
    //fill the ack pra by req return
    ack_pra->request_type=ack->request_type;
    ack_pra->panels=1;
    ack_pra->burst_count[0]=stream->height;
    ack_pra->burst_count[1]=0;
    ack_pra->burst_count[2]=0;
    ack_pra->data_flag=1;
    ack_pra->width=stream->width;
    ack_pra->height=stream->height;
    ack_pra->format=stream->format;

    y_addr= (unsigned long)send_buffer->mem.va[0];
    cb_addr=0;
    cr_addr=0;

    ar_always("%lx %lx %lx",y_addr,cb_addr,cr_addr);

    int ack_real_len=0;
    if(stream->send_data_mod==STREAM_MODE_RAW)
    {
        y_addr=(unsigned long)send_buffer->mem.va[0];
        ack_pra->frame_id=0;
        ar_always("send y %lx id=%d pix_count_align=%d",y_addr,ack_pra->frame_id,pix_count_align);
        for(int i=0;i<stream->height&&y_addr;i++){
            ack_pra->burst_ref=i;
            ack_pra->yuv_panel=0;
            ack_pra->burst_len=real_line_len_unpacked;
            ack_real_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_RAW_FRAME_ACK_T)+ack_pra->burst_len;
            //memcpy(p_frame_data,(void *)(y_addr+line_len*i),ack_pra->burst_len);
            convert_line_to_dng_line((uint16_t *)p_frame_data,(uint8_t *)(y_addr+line_len*i),stream->bit_depth,pix_count_align);
            obj->send(obj->client,(uint8_t *)ack,ack_real_len,obj->msg_id,0);
        }
    }else{
        ar_err("err send data mode");
    }
    free(ack);
    log_func_exit();
    return 0;
}


static int write_yuv_to_file(void * y, void * u, void * v, int yl, int ul, int vl,char *name,int index)
{
    FILE *fp_y,*fp_u,*fp_v;
    int len = 0;
    char name_with_dir_y[128];
    char name_with_dir_u[128];
    char name_with_dir_v[128];
    sprintf(name_with_dir_y,"/mnt/img_%d_%s_y.yuv",index,name);
    sprintf(name_with_dir_u,"/mnt/img_%d_%s_u.yuv",index,name);
    sprintf(name_with_dir_v,"/mnt/img_%d_%s_v.yuv",index,name);

    fp_y = fopen(name_with_dir_y, "w+");
    fp_u = fopen(name_with_dir_u, "w+");
    fp_v = fopen(name_with_dir_v, "w+");
    if(!fp_y||!fp_u||!fp_v)
    {
        fprintf(stderr, "Open file error!\n");
        return -1;
    }

    len = fwrite(y, 4, yl>>2, fp_y);
    if(len != (yl>>2))
    {
        fprintf(stderr, "Write Y to file failed, len = %x, yl = %x\n", len, yl);
        fclose(fp_y);
        fclose(fp_u);
        fclose(fp_v);
        return -1;
    }
    len = fwrite(u, 4, ul>>2, fp_u);
    if(len != (ul>>2))
    {
        fprintf(stderr, "Write U to file failed, len = %x, ul = %x\n", len, ul);
        fclose(fp_y);
        fclose(fp_u);
        fclose(fp_v);
        return -1;
    }
    len = fwrite(v, 4, vl>>2, fp_v);
    if(len != (vl>>2))
    {
        fprintf(stderr, "Write V to file failed, len = %x, ul = %x\n", len, vl);
        fclose(fp_y);
        fclose(fp_u);
        fclose(fp_v);
        return -1;
    }

    fclose(fp_y);
    fclose(fp_u);
    fclose(fp_v);
    return 0;
}

static int write_raw_to_file(void * y,int yl,char *name,int index)
{
    FILE *fp_y;
    int len = 0;
    char name_with_dir_y[128];
    sprintf(name_with_dir_y,"/mnt/img_%d_%s.raw",index,name);

    fp_y = fopen(name_with_dir_y, "w+");
    if(!fp_y)
    {
        fprintf(stderr, "Open file error!\n");
        return -1;
    }

    len = fwrite(y, 4, yl>>2, fp_y);
    if(len != (yl>>2))
    {
        fprintf(stderr, "Write Y to file failed, len = %x, yl = %x\n", len, yl);
        fclose(fp_y);
        return -1;
    }
    fclose(fp_y);
    return 0;
}

static int write_to_file(tuning_server_t *obj, void *buffer, int stream_type,char *name)
{
    log_func_enter();
    unsigned long y_addr, cb_addr, cr_addr;
    int y_len,u_len,v_len;
    send_mem_t *send_buf = (send_mem_t *)buffer;
    y_addr=(unsigned long)send_buf->va[0];
    cb_addr=(unsigned long)send_buf->va[1];
    cr_addr=(unsigned long)send_buf->va[2];

    y_len=send_buf->len[0];
    u_len=send_buf->len[1];
    v_len=send_buf->len[2];

    ar_always("%lx %lx %lx %x %x %x",y_addr,cb_addr,cr_addr,y_len,u_len,v_len);
    if(stream_type==TUNING_STREAM_VIDEO||TUNING_STREAM_PREVIEW==stream_type){
        write_yuv_to_file((void *)y_addr,(void *)cb_addr,(void *)cr_addr,y_len,u_len,v_len,name,0);
    }else if(stream_type==TUNING_STREAM_RAW)
    {
        write_raw_to_file((void *)y_addr,y_len,name,0);
    }

    log_func_exit();
    return 0;
}
static int process_video_buffer_assist(tuning_server_t *obj)
{

    int ret = 0;
#if USE_AXI_DMA
    STRU_SYS_DMA_TRAN_INFO transfer = {0};
#endif
    tuning_stream_t *stream=&obj->obj->stream[TUNING_STREAM_VIDEO];
    struct timeval start,end;
    AR_VOID *p_vaddr=NULL;
	AR_U64 phy_addr=0;
    STRU_STREAM_BUFFER_INIFO_T stream_buffer;
    ar_hal_vin_get_stream_buffer(obj->obj->stream_fd[TUNING_STREAM_VIDEO], obj->obj->pipe_id, obj->obj->stream_id[TUNING_STREAM_VIDEO], obj->stream_client[TUNING_STREAM_VIDEO], &stream_buffer, -1);

    //save this buf to prev, wait for next buf coming, then this buf can be returned to FW through Qbuf.

    if((stream->req_frame>0)&&(obj->queue_video->get_queue_size(obj->queue_video)==0))
    {
        if(1)
        {
            int j=0;
			int no_mem_flag=0;
			send_v4l2_buffer_t *send_buffer=malloc(sizeof(send_v4l2_buffer_t));
			if(send_buffer)
			{
			      gettimeofday(&start,NULL);
                  for(j=0;j<3;j++)
                  {
                      //send_buffer->mem.va[j]=malloc(stream->buffer[buf_index].len[j]);
                      //use dma buffer not malloc

                      ret = ar_hal_sys_mmz_alloc_cached(&phy_addr,&p_vaddr,"iq_tool_preview",NULL, stream_buffer.pannel[j].length + 4096);

                      if(p_vaddr==NULL || phy_addr==0 || ret < 0)
                      {
                           ar_err("ar_hal_sys_mmz_alloc_cached err");
                           no_mem_flag = 1;
                           break;
                      }
                      //memset(p_vaddr, 0, stream_buffer.pannel[j].length + 4096);

                      send_buffer->mem.va[j] = p_vaddr;
                      send_buffer->mem.pa[j] = (void *)phy_addr;
                      send_buffer->mem.len[j] =  stream_buffer.pannel[j].used;


                      ar_always("va:%p pa:%p size=%x",send_buffer->mem.va[j],send_buffer->mem.pa[j],send_buffer->mem.len[j]);

					  if(send_buffer->mem.va[j])
					  {
                          //memcpy(send_buffer->mem.va[j],(void *)stream->buffer[buf_index].va[j],stream->buffer[buf_index].len[j]);
                          transfer.s32_tran_cnt_sum = 1;
                          transfer.st_addr_size[0].pv_dst_pa = send_buffer->mem.pa[j];
                          transfer.st_addr_size[0].pv_src_pa = stream_buffer.pannel[j].buffer_phy_addr;
                          transfer.st_addr_size[0].u32_size   = send_buffer->mem.len[j];

#if USE_AXI_DMA
                          ret = ar_hal_sys_mem_transfer_pa(&transfer);
                          if (ret < 0) {
                               ar_err("ar_axi_dma_transfer ret = %d \n", ret);
                               ar_hal_sys_mmz_free(phy_addr, p_vaddr);
                          }
#else
                          memcpy(send_buffer->mem.va[j], stream_buffer.pannel[j].buffer, transfer.addr_size[0].size);

#endif

					  }
					  else
					  {
					     no_mem_flag=1;
					     break;
					  }
                  }
				  gettimeofday(&end,NULL);
				  int time_us=(int)((end.tv_sec-start.tv_sec)*1000000 + (end.tv_usec-start.tv_usec));
				  ar_always("cpy time is %d(us) req_frame=%d",time_us,stream->req_frame);
                  if(!no_mem_flag){
                       send_buffer->stream_type=TUNING_STREAM_VIDEO;
                       send_buffer->write_to_file=0;

                       pthread_mutex_lock(&obj->mutex);
                       obj->queue_video->queue_insert(obj->queue_video,(void *)send_buffer);
                       pthread_cond_signal(&obj->cond_video);
                       pthread_mutex_unlock(&obj->mutex);
                  }else
                  {
                      ar_err("no mem for frame");
                      for(j=0;j<3;j++)
                      {
                           if(send_buffer->mem.va[j])
                           {
                              ar_hal_sys_mmz_free((AR_U64)send_buffer->mem.pa[j], send_buffer->mem.va[j]);
                           }
                      }

                      if (send_buffer)
                      {
                           free(send_buffer);
                           send_buffer = NULL;
                      }
                  }
			}
			else
			{
			      ar_err("no mem for send_buffer");
			}
			stream->req_frame--;
			ar_hal_vin_return_stream_buffer(obj->obj->stream_fd[TUNING_STREAM_VIDEO], obj->obj->pipe_id, obj->obj->stream_id[TUNING_STREAM_VIDEO], &stream_buffer);

        }
    }
    else
    {
        ar_hal_vin_return_stream_buffer(obj->obj->stream_fd[TUNING_STREAM_VIDEO], obj->obj->pipe_id, obj->obj->stream_id[TUNING_STREAM_VIDEO], &stream_buffer);
    }
    return 0;
}
static int process_preview_buffer_assist(tuning_server_t *obj)
{
    int ret = 0;
#if USE_AXI_DMA
    STRU_SYS_DMA_TRAN_INFO transfer = {0};
#endif
    tuning_stream_t *stream=&obj->obj->stream[TUNING_STREAM_PREVIEW];
    struct timeval start,end;
    AR_VOID *p_vaddr=NULL;
    AR_U64 phy_addr=0;
    STRU_STREAM_BUFFER_INIFO_T stream_buffer;
    //ar_always("stream_fd = %d, obj->obj->pipe_id = %d\n",obj->obj->stream_fd[TUNING_STREAM_PREVIEW], obj->obj->pipe_id );
    //ar_always("stream_id = %d, obj->stream_client[TUNING_STREAM_PREVIEW] = %d\n",obj->obj->stream_id[TUNING_STREAM_PREVIEW], obj->stream_client[TUNING_STREAM_PREVIEW] );
    ar_hal_vin_get_stream_buffer(obj->obj->stream_fd[TUNING_STREAM_PREVIEW], obj->obj->pipe_id, obj->obj->stream_id[TUNING_STREAM_PREVIEW], obj->stream_client[TUNING_STREAM_PREVIEW], &stream_buffer, -1);

    //ar_always("stream->req_frame = %d \n", stream->req_frame);
    //save this buf to prev, wait for next buf coming, then this buf can be returned to FW through Qbuf.
    if((stream->req_frame>0))
    {
        //send_preview_to_pctool(obj,&buf);
        if(stream->send_data_mod==STREAM_MODE_YUV)
        {
            int j=0;
			int no_mem_flag=0;
			send_v4l2_buffer_t *send_buffer=malloc(sizeof(send_v4l2_buffer_t));
			if(send_buffer)
			{
			      gettimeofday(&start,NULL);
                  for(j=0;j<3;j++)
                  {
                      //send_buffer->mem.va[j]=malloc(stream->buffer[buf_index].len[j]);
                      //use dma buffer not malloc
                      ar_hal_sys_mmz_alloc_cached(&phy_addr,&p_vaddr,NULL,NULL, stream_buffer.pannel[j].length + 4096);

                      if(p_vaddr==NULL || phy_addr==0)
                      {
                           ar_err("ar_hal_sys_mmz_alloc_cached err");
                           no_mem_flag = 1;
                           break;
                      }

                      send_buffer->mem.va[j] = p_vaddr;
                      send_buffer->mem.pa[j] = (void *)phy_addr;
                      send_buffer->mem.len[j] =  stream_buffer.pannel[j].used;


                      //ar_always("va:%p pa:%p size=%x",send_buffer->mem.va[j],send_buffer->mem.pa[j],send_buffer->mem.len[j]);

					  if(send_buffer->mem.va[j])
					  {
                          transfer.s32_tran_cnt_sum = 1;
                          transfer.st_addr_size[0].pv_dst_pa = send_buffer->mem.pa[j];
                          transfer.st_addr_size[0].pv_src_pa = stream_buffer.pannel[j].buffer_phy_addr;
                          transfer.st_addr_size[0].u32_size   = send_buffer->mem.len[j];

#if USE_AXI_DMA
                          ret = ar_hal_sys_mem_transfer_pa(&transfer);
                          if (ret < 0) {
                               ar_err("ar_axi_dma_transfer ret = %d \n", ret);
                               ar_hal_sys_mmz_free(phy_addr, p_vaddr);
                          }
#else
                          memcpy(send_buffer->mem.va[j], stream_buffer.pannel[j].buffer, transfer.addr_size[0].size);

#endif

					  }
					  else
					  {
					     no_mem_flag=1;
					     break;
					  }
                  }
				  gettimeofday(&end,NULL);
				  int time_us=(int)((end.tv_sec-start.tv_sec)*1000000 + (end.tv_usec-start.tv_usec));
				  ar_always("cpy time is %d(us) req_frame=%d",time_us,stream->req_frame);
                  if(!no_mem_flag){
                      send_buffer->stream_type=TUNING_STREAM_PREVIEW;
                      send_buffer->write_to_file=0;

                      pthread_mutex_lock(&obj->mutex);
                      obj->queue_priview->queue_insert(obj->queue_priview,(void *)send_buffer);
                      pthread_cond_signal(&obj->cond_privew);
                      pthread_mutex_unlock(&obj->mutex);
                  }else
                  {
                      ar_err("no mem for frame");
                      for(j=0;j<3;j++)
                      {
                           if(send_buffer->mem.va[j])
                           {
                              ar_hal_sys_mmz_free((AR_U64)send_buffer->mem.pa[j], send_buffer->mem.va[j]);
                           }
                      }
                      if (send_buffer)
                      {
                           free(send_buffer);
                           send_buffer = NULL;
                      }
                  }
			}
			else
			{
			      ar_err("no mem for send_buffer");
			}

            ar_hal_vin_return_stream_buffer(obj->obj->stream_fd[TUNING_STREAM_PREVIEW], obj->obj->pipe_id, obj->obj->stream_id[TUNING_STREAM_PREVIEW], &stream_buffer);
        }
    }
    else
    {
        ar_hal_vin_return_stream_buffer(obj->obj->stream_fd[TUNING_STREAM_PREVIEW], obj->obj->pipe_id, obj->obj->stream_id[TUNING_STREAM_PREVIEW], &stream_buffer);
    }
    return 0;
}
static int process_raw_buffer_asssit(tuning_server_t *obj)
{
    int ret = 0;
#if USE_AXI_DMA
    STRU_SYS_DMA_TRAN_INFO transfer = {0};
#endif
    tuning_stream_t *stream=&obj->obj->stream[TUNING_STREAM_RAW];
    struct timeval start,end;
    AR_VOID *p_vaddr=NULL;
    AR_U64 phy_addr=0;
    STRU_STREAM_BUFFER_INIFO_T stream_buffer;
    ar_hal_vin_get_stream_buffer(obj->obj->stream_fd[TUNING_STREAM_RAW], obj->obj->pipe_id, obj->obj->stream_id[TUNING_STREAM_RAW], obj->stream_client[TUNING_STREAM_RAW], &stream_buffer, -1);

    //save this buf to prev, wait for next buf coming, then this buf can be returned to FW through Qbuf.
    if((stream->req_frame>0))
    {
        //send_preview_to_pctool(obj,&buf);
        if(stream->send_data_mod==STREAM_MODE_RAW)
        {
            int j=0;
			int no_mem_flag=0;
			send_v4l2_buffer_t *send_buffer=malloc(sizeof(send_v4l2_buffer_t));
			if(send_buffer)
			{
			      gettimeofday(&start,NULL);
				  send_buffer->basic_info=stream_buffer.basic_info;
                  for(j=0;j<1;j++)
                  {
                      //send_buffer->mem.va[j]=malloc(stream->buffer[buf_index].len[j]);
                      //use dma buffer not malloc
                        ret = ar_hal_sys_mmz_alloc_cached(&phy_addr,&p_vaddr,NULL,NULL, stream_buffer.pannel[j].length + 4096);

                        if(p_vaddr==NULL || phy_addr==0 || ret < 0)
                        {
                             ar_err("ar_hal_sys_mmz_alloc_cached err, %d", ret);
                             no_mem_flag = 1;
                             break;
                        }
                        //memset(p_vaddr, 0, stream_buffer.pannel[j].length + 4096);

                        send_buffer->mem.va[j] = p_vaddr;
                        send_buffer->mem.pa[j] = (void *)phy_addr;
                        send_buffer->mem.len[j] =  stream_buffer.pannel[j].used;


                        ar_always("va:%p pa:%p size=%x",send_buffer->mem.va[j],send_buffer->mem.pa[j],send_buffer->mem.len[j]);

                        if(send_buffer->mem.va[j])
                        {
                            transfer.s32_tran_cnt_sum = 1;
                            transfer.st_addr_size[0].pv_dst_pa = send_buffer->mem.pa[j];
                            transfer.st_addr_size[0].pv_src_pa = stream_buffer.pannel[j].buffer_phy_addr;
                            transfer.st_addr_size[0].u32_size = send_buffer->mem.len[j];

#if USE_AXI_DMA
                            ret = ar_hal_sys_mem_transfer_pa(&transfer);
                            if (ret < 0) {
                                 ar_err("ar_axi_dma_transfer ret = %d \n", ret);
                                 ar_hal_sys_mmz_free(phy_addr, p_vaddr);
                            }
#else
                            memcpy(send_buffer->mem.va[j], stream_buffer.pannel[j].buffer, transfer.addr_size[0].size);

#endif

					  }
					  else
					  {
					     no_mem_flag=1;
					     break;
					  }
                  }
				  gettimeofday(&end,NULL);
				  int time_us=(int)((end.tv_sec-start.tv_sec)*1000000 + (end.tv_usec-start.tv_usec));
				  ar_always("cpy time is %d(us) req_frame=%d",time_us,stream->req_frame);
                  if(!no_mem_flag){
                      send_buffer->stream_type=TUNING_STREAM_RAW;
                      send_buffer->write_to_file=0;

                      pthread_mutex_lock(&obj->mutex);
                      obj->queue_raw->queue_insert(obj->queue_raw,(void *)send_buffer);
                      pthread_cond_signal(&obj->cond_raw);
                      pthread_mutex_unlock(&obj->mutex);
                  }else
                  {
                      ar_err("no mem for frame");
                      for(j=0;j<3;j++)
                      {
                           if(send_buffer->mem.va[j])
                           {
                              ar_hal_sys_mmz_free((AR_U64)send_buffer->mem.pa[j], send_buffer->mem.va[j]);
                           }
                      }

                      if (send_buffer)
                      {
                           free(send_buffer);
                           send_buffer = NULL;
                      }
                  }
			}
			else
			{
			      ar_err("no mem for send_buffer");
			}
            stream->req_frame--;
            ar_hal_vin_return_stream_buffer(obj->obj->stream_fd[TUNING_STREAM_RAW], obj->obj->pipe_id, obj->obj->stream_id[TUNING_STREAM_RAW], &stream_buffer);

        }
    }
    else
    {
        ar_hal_vin_return_stream_buffer(obj->obj->stream_fd[TUNING_STREAM_RAW], obj->obj->pipe_id, obj->obj->stream_id[TUNING_STREAM_RAW], &stream_buffer);
    }
    return 0;
}

static int process_af_buffer_asssit(tuning_server_t *obj,int fd_mem,void *event_data)
{
    int ret = 0;
#if USE_AXI_DMA
    STRU_SYS_DMA_TRAN_INFO transfer = {0};
#endif
    struct timeval start,end;
    AR_VOID *p_vaddr=NULL;
    AR_U64 phy_addr=0;


	STRU_META_DATA_TYPE_T *p_meta=(STRU_META_DATA_TYPE_T *)event_data;
	STRU_AF_ALGO_LIB_INPUT_T *p_algo_lib_input_phy=p_meta->meta_data.af_stats_group.p_stats;
	AR_S32 offset=0;
	void *stats_maped=ar_hal_vin_map_phy_addr(fd_mem,p_algo_lib_input_phy,sizeof(STRU_AF_ALGO_LIB_INPUT_T),&offset);
	void *data=stats_maped+offset;

    //map the output 
	STRU_AF_ALGO_LIB_OUTPUT_T *p_algo_lib_out_phy=p_meta->meta_data.af_stats_group.p_out;
	AR_S32 offset_out=0;
	void *stats_maped_out=ar_hal_vin_map_phy_addr(fd_mem,p_algo_lib_out_phy,sizeof(STRU_AF_ALGO_LIB_OUTPUT_T),&offset_out);
	void *data_out=stats_maped_out+offset_out;


    //save this buf to prev, wait for next buf coming, then this buf can be returned to FW through Qbuf.
    if((obj->obj->af_frame_count>0))
    {
        //send_preview_to_pctool(obj,&buf);
        //if(stream->send_data_mod==STREAM_MODE_RAW)
        {
            int j=0;
			int no_mem_flag=0;
			send_v4l2_buffer_t *send_buffer=malloc(sizeof(send_v4l2_buffer_t));
			if(send_buffer)
			{
			      gettimeofday(&start,NULL);
                  for(j=0;j<1;j++)
                  {
                      //send_buffer->mem.va[j]=malloc(stream->buffer[buf_index].len[j]);
                      //use dma buffer not malloc
                        ret = ar_hal_sys_mmz_alloc_cached(&phy_addr,&p_vaddr,NULL,NULL, sizeof(STRU_AF_ALGO_LIB_INPUT_T)+sizeof(STRU_AF_ALGO_LIB_OUTPUT_T) + 4096);

                        if(p_vaddr==NULL || phy_addr==0 || ret < 0)
                        {
                             ar_err("ar_hal_sys_mmz_alloc_cached err, %d", ret);
                             no_mem_flag = 1;
                             break;
                        }
                        //memset(p_vaddr, 0, stream_buffer.pannel[j].length + 4096);

                        send_buffer->mem.va[j] = p_vaddr;
                        send_buffer->mem.pa[j] = (void *)phy_addr;
                        send_buffer->mem.len[j] =  sizeof(STRU_AF_ALGO_LIB_INPUT_T);


                        ar_always("va:%p pa:%p size=%x",send_buffer->mem.va[j],send_buffer->mem.pa[j],send_buffer->mem.len[j]);

                        if(send_buffer->mem.va[j])
                        {
                            transfer.s32_tran_cnt_sum = 1;
                            transfer.st_addr_size[0].pv_dst_pa = send_buffer->mem.pa[j];
                            transfer.st_addr_size[0].pv_src_pa = p_algo_lib_input_phy;
                            transfer.st_addr_size[0].u32_size = send_buffer->mem.len[j];

#if USE_AXI_DMA
                            ret = ar_hal_sys_mem_transfer_pa(&transfer);
                            if (ret < 0) {
                                 ar_err("ar_axi_dma_transfer ret = %d \n", ret);
                                 ar_hal_sys_mmz_free(phy_addr, p_vaddr);
                            }
#else
                            memcpy(send_buffer->mem.va[j]+sizeof(STRU_AF_ALGO_LIB_OUTPUT_T), data_out,sizeof(STRU_AF_ALGO_LIB_OUTPUT_T));

#endif
                            //copy out to mem
                            memcpy(send_buffer->mem.va[j]+sizeof(STRU_AF_ALGO_LIB_INPUT_T), data_out,sizeof(STRU_AF_ALGO_LIB_OUTPUT_T));

                            STRU_AF_ALGO_LIB_OUTPUT_T *p_af_out=(STRU_AF_ALGO_LIB_OUTPUT_T *)data_out;

                            //ar_printf("lens_position=%d \n",p_af_out->lens_position);
                  

					  }
					  else
					  {
					     no_mem_flag=1;
					     break;
					  }
                  }
				  gettimeofday(&end,NULL);
				  int time_us=(int)((end.tv_sec-start.tv_sec)*1000000 + (end.tv_usec-start.tv_usec));
				  ar_always("cpy time is %d(us) req_frame=%d",time_us,obj->obj->af_frame_count);
                  if(!no_mem_flag){
                      send_buffer->stream_type=TUNING_STREAM_META_AF;
                      send_buffer->write_to_file=0;

                      pthread_mutex_lock(&obj->mutex);
                      obj->queue_af->queue_insert(obj->queue_af,(void *)send_buffer);
                      pthread_cond_signal(&obj->cond_af);
                      pthread_mutex_unlock(&obj->mutex);
                  }else
                  {
                      ar_err("no mem for frame");
                      for(j=0;j<3;j++)
                      {
                           if(send_buffer->mem.va[j])
                           {
                              ar_hal_sys_mmz_free((AR_U64)send_buffer->mem.pa[j], send_buffer->mem.va[j]);
                           }
                      }

                      if (send_buffer)
                      {
                           free(send_buffer);
                           send_buffer = NULL;
                      }
                  }
			}
			else
			{
			      ar_err("no mem for send_buffer");
			}
			ar_hal_vin_ummap_vaddr(stats_maped,sizeof(STRU_AF_ALGO_LIB_INPUT_T));			
		    ar_hal_vin_ummap_vaddr(stats_maped_out,sizeof(STRU_AF_ALGO_LIB_OUTPUT_T));

        }
    }
    else
    {
		ar_hal_vin_ummap_vaddr(stats_maped,sizeof(STRU_AF_ALGO_LIB_INPUT_T));		
	    ar_hal_vin_ummap_vaddr(stats_maped_out,sizeof(STRU_AF_ALGO_LIB_OUTPUT_T));
    }
    return 0;
}




static void  *preview_thread_assist(void *data)
{
    tuning_server_t *obj=(tuning_server_t *)data;
    tuning_stream_t *stream=&obj->obj->stream[TUNING_STREAM_PREVIEW];
    ar_always("enter");
    while (!stream->stop_flag)
    {
        process_preview_buffer_assist(obj);
    }
    return NULL;
}


static void  *video_thread_assist(void *data)
{
    tuning_server_t *obj=(tuning_server_t *)data;
    tuning_stream_t *stream=&obj->obj->stream[TUNING_STREAM_VIDEO];
    ar_always("enter");
    while (!stream->stop_flag)
    {
        process_video_buffer_assist(obj);
    }
    return NULL;
}


static void  *raw_thread_assist(void *data)
{
    tuning_server_t *obj=(tuning_server_t *)data;
    tuning_stream_t *stream=&obj->obj->stream[TUNING_STREAM_RAW];
    ar_always("enter");
    while (!stream->stop_flag)
    {
        process_raw_buffer_asssit(obj);
    }
    return NULL;
}

static void *dq_stream_buf_thread_assist(void *data)
{
    tuning_server_t *obj = (tuning_server_t *)data;
    STRU_STREAM_BUFFER_INIFO_T *pstream_buf = NULL;

    ar_always("dq stream thread pipe_fd = %d pipe_id = %d\n", obj->obj->pipe_fd, obj->obj->pipe_id);

    //dqbuf;
    while(1)
    {
        pstream_buf = (STRU_STREAM_BUFFER_INIFO_T *)malloc(sizeof(STRU_STREAM_BUFFER_INIFO_T));
		if(pstream_buf){
            ar_hal_vin_dq_stream_buffer(obj->obj->pipe_fd, obj->obj->pipe_id, TUNING_STREAM_PREVIEW, pstream_buf, -1);
            //ar_always("   dq------\n");
            obj->q_dq_event->queue_insert(obj->q_dq_event, (void *)pstream_buf);
		}
    }

    return NULL;
}

static void *q_stream_buf_thread_assist(void * data)
{
    tuning_server_t *obj=(tuning_server_t *)data;
    int size = 0, ret = 0;
    int i = 0;//, fd = 0;
    void *align_addr = NULL;
    v4ar_mem_t mem;
    STRU_STREAM_BUFFER_INIFO_T *pstream_buf_info = NULL;
    // STRU_STREAM_BUFFER_INIFO_T *tmp_buf_info = NULL;
    // char dev_name[64];

    ar_always("q stream thread pipe_fd = %d pipe_id = %d\n", obj->obj->pipe_fd, obj->obj->pipe_id);

    //request stream buffer;
    ret = ar_hal_vin_requst_stream_buffer(obj->obj->pipe_fd, obj->obj->pipe_id, TUNING_STREAM_PREVIEW, obj->pstream_buf, &size);
    if (ret < 0)
    {
        ar_err("request buffer failed");
	    return NULL;
    }
    //insert stream buffer to event queue
    for (i = 0; i < size; i++)    {
        STRU_STREAM_BUFFER_INIFO_T *tmp_buf_info = (STRU_STREAM_BUFFER_INIFO_T *)malloc(sizeof(STRU_STREAM_BUFFER_INIFO_T));
		if(tmp_buf_info){
             *tmp_buf_info = obj->pstream_buf[i];
             obj->q_dq_event->queue_insert(obj->q_dq_event, (void *)tmp_buf_info);
		}
    }

    //ar_always("obj->server_status = %d \n", obj->server_status);
    while(obj->server_status == SERVER_STREAM_RUNING)
    {
        if (obj->obj->trigger_start)
        {
            while(obj->obj->times-- && obj->obj->trigger_start)
            {
                //for normal path
                //seq_mod, 0: 1-->2-->3

                for (i = obj->obj->start_idx; (i < obj->obj->seq_count + obj->obj->start_idx) && i < obj->obj->nframes;)
                {
                    if ((obj->mem_raw_buf[i].len > 0 )&& (obj->mem_raw_buf[i].pa != NULL))
                    {
                        //1 hdr vc mode: long expore frame and short expore frame with two frames
                        if (obj->obj->hdr_mod == 1)
                        {
                            //qbuf
                            if (obj->q_dq_event->get_queue_size(obj->q_dq_event) > 0)
                            {
                                obj->q_dq_event->queue_pop(obj->q_dq_event, (void *)&pstream_buf_info);

                                for(int k = 0; k < obj->obj->hdr_frames; k++)
                                {
                                    mem = obj->mem_raw_buf[i + k];
                                    //ar_always("hdr normal path mem pa = %p \n", mem.pa);
                                    align_addr = (void *)CAM_ALIGNE_TO((uint64_t)mem.pa, BUF_ALIGN);
                                    pstream_buf_info->pannel[k].buffer_phy_addr = align_addr;
                                    pstream_buf_info->pannel[k].buffer_phy_orign = mem.pa;
                                    //ar_always("each push pannel[%d] = %p \n", k, pstream_buf_info->pannel[k].buffer_phy_orign);

                                }
                                pstream_buf_info->panel_count = obj->obj->hdr_frames;

                                if(obj->obj->with_3a)
                                {
                                    ar_hal_vin_aec_manu_set(obj->obj->pipe_fd, obj->obj->pipe_id, &obj->mem_raw_3a[i].aec_info);
                                    ar_hal_vin_awb_manu_set(obj->obj->pipe_fd, obj->obj->pipe_id, &obj->mem_raw_3a[i].awb_info);
                                }
                                ar_hal_vin_q_stream_buffer(obj->obj->pipe_fd, obj->obj->pipe_id, TUNING_STREAM_PREVIEW, pstream_buf_info);

                                i += obj->obj->hdr_frames;
                                free(pstream_buf_info);
                                pstream_buf_info = NULL;

                            }else
                            {
                                ar_always("hdr trigger dq_event queue no item \n");
                            }

                        }else
                        {
                            mem = obj->mem_raw_buf[i];

                            //qbuf
                            //ar_always("mem va = %p \n", mem.va);
                            if (obj->q_dq_event->get_queue_size(obj->q_dq_event) > 0)
                            {
                                obj->q_dq_event->queue_pop(obj->q_dq_event, (void *)&pstream_buf_info);
                                align_addr = (void *)CAM_ALIGNE_TO((uint64_t)mem.pa, BUF_ALIGN);
                                pstream_buf_info->pannel[0].buffer_phy_addr = align_addr;
                                pstream_buf_info->pannel[0].buffer_phy_orign = mem.pa;

                                //ar_always("time %d, frame %d\n", obj->obj->times, i);
                                //ar_always("    q++++++\n", i);

                                if(obj->obj->with_3a)
                                {
                                    ar_hal_vin_aec_manu_set(obj->obj->pipe_fd, obj->obj->pipe_id, &obj->mem_raw_3a[i].aec_info);
                                    ar_hal_vin_awb_manu_set(obj->obj->pipe_fd, obj->obj->pipe_id, &obj->mem_raw_3a[i].awb_info);
                                }
                                ar_hal_vin_q_stream_buffer(obj->obj->pipe_fd, obj->obj->pipe_id, TUNING_STREAM_PREVIEW, pstream_buf_info);

                                i++;
                                free(pstream_buf_info);
                                pstream_buf_info = NULL;

                            }else
                            {
                                ar_always("normal trigger dq_event queue no item \n");
                            }
                         }
                    }
                    usleep((1000000  / obj->obj->fps) + 15000);

                }
                //for reverse path
                if (obj->obj->seq_mod == 1 && obj->obj->seq_count >=  2) //1: seqence[1-->2-->3-->2-->1]
                {
                    for (i = obj->obj->start_idx + obj->obj->seq_count - obj->obj->hdr_frames - 1; i >= obj->obj->hdr_frames;)
                    {
                        if ((obj->mem_raw_buf[i].len > 0 )&& (obj->mem_raw_buf[i].pa != NULL))
                        {
                            if (obj->obj->hdr_mod == 1)
                            {
                                if (obj->q_dq_event->get_queue_size(obj->q_dq_event) > 0)
                                {
                                    obj->q_dq_event->queue_pop(obj->q_dq_event, (void *)&pstream_buf_info);

                                    for(int k = 0; k < obj->obj->hdr_frames; k++)
                                    {
                                        mem = obj->mem_raw_buf[i - k];
                                        //ar_always("hdr reverse path mem pa = %p \n", mem.pa);
                                        align_addr = (void *)CAM_ALIGNE_TO((uint64_t)mem.pa, BUF_ALIGN);
                                        pstream_buf_info->pannel[obj->obj->hdr_frames - 1 - k].buffer_phy_addr = align_addr;
                                        pstream_buf_info->pannel[obj->obj->hdr_frames - 1 - k].buffer_phy_orign = mem.pa;
                                        //ar_always("each push pannel[%d] = %p \n", k, pstream_buf_info->pannel[k].buffer_phy_orign);
                                    }
                                    pstream_buf_info->panel_count = obj->obj->hdr_frames;

                                    if(obj->obj->with_3a)
                                    {
                                        ar_hal_vin_aec_manu_set(obj->obj->pipe_fd, obj->obj->pipe_id, &obj->mem_raw_3a[i].aec_info);
                                        ar_hal_vin_awb_manu_set(obj->obj->pipe_fd, obj->obj->pipe_id, &obj->mem_raw_3a[i].awb_info);
                                    }
                                    ar_hal_vin_q_stream_buffer(obj->obj->pipe_fd, obj->obj->pipe_id, TUNING_STREAM_PREVIEW, pstream_buf_info);

                                    i -= obj->obj->hdr_frames;
                                    free(pstream_buf_info);
                                    pstream_buf_info = NULL;

                                }else
                                {
                                    ar_always("reverse trigger dq_event queue no item \n");
                                }

                            }else
                            {
                                mem = obj->mem_raw_buf[i];

                                //qbuf
                                //ar_always("mem va = %p \n", mem.va);
                                if (obj->q_dq_event->get_queue_size(obj->q_dq_event) > 0)
                                {
                                    obj->q_dq_event->queue_pop(obj->q_dq_event, (void *)&pstream_buf_info);
                                    align_addr = (void *)CAM_ALIGNE_TO((uint64_t)mem.pa, BUF_ALIGN);
                                    pstream_buf_info->pannel[0].buffer_phy_addr = align_addr;
                                    pstream_buf_info->pannel[0].buffer_phy_orign = mem.pa;

                                    if(obj->obj->with_3a)
                                    {
                                        ar_hal_vin_aec_manu_set(obj->obj->pipe_fd, obj->obj->pipe_id, &obj->mem_raw_3a[i].aec_info);
                                        ar_hal_vin_awb_manu_set(obj->obj->pipe_fd, obj->obj->pipe_id, &obj->mem_raw_3a[i].awb_info);
                                    }
                                    ar_hal_vin_q_stream_buffer(obj->obj->pipe_fd, obj->obj->pipe_id, TUNING_STREAM_PREVIEW, pstream_buf_info);

                                    i--;
                                    free(pstream_buf_info);
                                    pstream_buf_info = NULL;

                                }else
                                {
                                    ar_always("reserve trigger dq_event queue no item \n");
                                }
                             }
                         }

                        usleep((1000000  / obj->obj->fps) + 15000);
                    }
                }
            }
            //time over then stop trigger
            obj->obj->trigger_start = 0;

            if(obj->obj->with_3a)
            {
                ar_hal_vin_aec_set_mode(obj->obj->pipe_fd, obj->obj->pipe_id, 0);
                ar_hal_vin_awb_set_mode(obj->obj->pipe_fd, obj->obj->pipe_id, 10);
            }

        }else
        {
            //stop send raw frame: stop stream
            usleep((1000000  / obj->obj->fps) + 15000);
        }
    }

    return NULL;
}

static int get_trigger_fetch_info(tuning_server_t *obj, STRU_TUNING_TRIGGER_FETCH_RAW_PRA_T *pra)
{
    if (obj != NULL)
    {
        obj->obj->trigger_start = pra->trigger;
        obj->obj->seq_mod = pra->seq_mod;
        obj->obj->seq_count = pra->seq_count;
        obj->obj->times = pra->times;
        obj->obj->start_idx = pra->start_idx;
        obj->obj->fps = pra->fps;
        ar_always("trigger = %d, seq_mod = %d\n", obj->obj->trigger_start,  obj->obj->seq_mod);
        ar_always("seq_count = %d, times = %d\n", obj->obj->seq_count,  obj->obj->times);
        ar_always("start_idx = %d, fps = %d\n", obj->obj->start_idx, obj->obj->fps);
    }

    return 0;
}

static void  process_send_req(tuning_server_t *obj,send_v4l2_buffer_t *send_buffer)
{
    char *name=NULL;
    if(send_buffer->stream_type==TUNING_STREAM_PREVIEW)
    {
        ar_always("send next preview frame to pc !!!!\n");
        send_preview_to_pctool(obj,send_buffer);
        name="preview";
    }else if(send_buffer->stream_type==TUNING_STREAM_VIDEO)
    {
        ar_always("send next video to pctool !!!!\n");
        send_video_to_pctool(obj,send_buffer);
        name="snap";
    }else if(send_buffer->stream_type==TUNING_STREAM_RAW)
    {
        ar_always("send next raw to pctool !!!!\n");
        send_raw_to_pctool(obj,send_buffer);
        name="raw";
    }else if(send_buffer->stream_type==TUNING_STREAM_HDR_RAW)
    {
        ar_always("send next hdr raw to pctool !!!!\n");
        send_hdr_raw_to_pctool(obj,send_buffer);
        name="hdr_raw";
    }else if(send_buffer->stream_type==TUNING_STREAM_META_AF)
    {
    	ar_always("send next af frame to pc !!!!\n");
    	send_af_to_pctool(obj,send_buffer);
    	name="af";
    }
    if(send_buffer->write_to_file)
    {
        ar_always("write frame to file !!!!\n");
        write_to_file(obj,&send_buffer->mem,send_buffer->stream_type,name);
    }

}
static void  *send_thread_priview(void *data)
{
    tuning_server_t *obj=(tuning_server_t *)data;
    send_v4l2_buffer_t *send_buffer=NULL;
    v4ar_mem_t mem;
    ar_always("enter");
	int ret=0;

    while (1)
    {
        //wait the send request
        pthread_mutex_lock(&obj->mutex);

        obj->queue_priview->queue_pop(obj->queue_priview,(void **)&send_buffer);
        while(!send_buffer){
            pthread_cond_wait(&obj->cond_privew,&obj->mutex);
            obj->queue_priview->queue_pop(obj->queue_priview,(void **)&send_buffer);
        }
        pthread_mutex_unlock(&obj->mutex);

        //if send rate is so slower, then discard lasted frames, use latest frames
        if (obj->queue_priview->get_queue_size(obj->queue_priview) <= 1)
        {
            process_send_req(obj,send_buffer);
        }
        //send request have arrived , process it

        //process done, release buffer
		for(int i=0;i<3;i++)
		{
			if(send_buffer->mem.va[i])
			{
				//free(send_buffer->mem.va[i]);
				mem.len = send_buffer->mem.len[i];
				mem.va = send_buffer->mem.va[i];
				mem.pa = send_buffer->mem.pa[i];
				//release the dma buffer
				ret = ar_hal_sys_mmz_free((AR_U64)mem.pa, mem.va);
				if (ret < 0)
				{
				    ar_err("free mem error");
				}
			}
		}
        free(send_buffer);
    }

    return NULL;
}


static void  *send_thread_af(void *data)
{
    tuning_server_t *obj=(tuning_server_t *)data;
    send_v4l2_buffer_t *send_buffer=NULL;
    v4ar_mem_t mem;
    ar_always("enter");
	int ret=0;

    while (1)
    {
        //wait the send request
        pthread_mutex_lock(&obj->mutex);

        obj->queue_af->queue_pop(obj->queue_af,(void **)&send_buffer);
        while(!send_buffer){
            pthread_cond_wait(&obj->cond_af,&obj->mutex);
            obj->queue_af->queue_pop(obj->queue_af,(void **)&send_buffer);
        }
        pthread_mutex_unlock(&obj->mutex);

        //if send rate is so slower, then discard lasted frames, use latest frames
        if (obj->queue_af->get_queue_size(obj->queue_af) <= 1)
        {
            process_send_req(obj,send_buffer);
        }
        //send request have arrived , process it

        //process done, release buffer
		for(int i=0;i<1;i++)
		{
			if(send_buffer->mem.va[i])
			{
				//free(send_buffer->mem.va[i]);
				mem.len = send_buffer->mem.len[i];
				mem.va = send_buffer->mem.va[i];
				mem.pa = send_buffer->mem.pa[i];
				//release the dma buffer
				ret = ar_hal_sys_mmz_free((AR_U64)mem.pa, mem.va);
				if (ret < 0)
				{
				    ar_err("free mem error");
				}
			}
		}
        free(send_buffer);
    }

    return NULL;
}


static void  *send_thread_raw(void *data)
{
    tuning_server_t *obj=(tuning_server_t *)data;
    send_v4l2_buffer_t *send_buffer=NULL;
    v4ar_mem_t mem;
    int ret = 0;
    ar_always("enter");

    while (1)
    {
        //wait the send request
        pthread_mutex_lock(&obj->mutex);
        obj->queue_raw->queue_pop(obj->queue_raw,(void **)&send_buffer);

        while(!send_buffer){
            pthread_cond_wait(&obj->cond_raw,&obj->mutex);
            obj->queue_raw->queue_pop(obj->queue_raw,(void **)&send_buffer);
        }
        pthread_mutex_unlock(&obj->mutex);
        //send request have arrived , process it
        process_send_req(obj,send_buffer);
        //set the send buffer as 0
        if(send_buffer->mem.va[0]){
        	 //free(send_buffer->mem.va[0]);
        	 mem.len = send_buffer->mem.len[0];
        	 mem.va = send_buffer->mem.va[0];
        	 mem.pa = send_buffer->mem.pa[0];
             ret = ar_hal_sys_mmz_free((AR_U64)mem.pa, mem.va);
             if (ret < 0)
             {
                ar_err("free mem error");
             }
        }
        free(send_buffer);
    }

    return NULL;
}

static void  *send_thread_video(void *data)
{
    tuning_server_t *obj=(tuning_server_t *)data;
    send_v4l2_buffer_t *send_buffer=NULL;
    v4ar_mem_t mem;
    int ret = 0;
    ar_always("enter");

    int i=0;
    while (1)
    {
        //wait the send request
        pthread_mutex_lock(&obj->mutex);
        obj->queue_video->queue_pop(obj->queue_video,(void **)&send_buffer);

        while(!send_buffer){
            pthread_cond_wait(&obj->cond_video,&obj->mutex);
            obj->queue_video->queue_pop(obj->queue_video,(void **)&send_buffer);
        }

        pthread_mutex_unlock(&obj->mutex);
        //send request have arrived , process it
        process_send_req(obj,send_buffer);
        //have send completed,return the buffer to v4l2 driver
        if(obj->obj->stream[TUNING_STREAM_VIDEO].send_data_mod==STREAM_MODE_YUV)
        {
            for(i=0;i<3;i++)
            {
                if(send_buffer->mem.va[i])
                {
                    //free(send_buffer->mem.va[i]);
                    mem.len = send_buffer->mem.len[i];
        	        mem.va = send_buffer->mem.va[i];
        	        mem.pa = send_buffer->mem.pa[i];
                    ret = ar_hal_sys_mmz_free((AR_U64)mem.pa, mem.va);
                    if (ret < 0)
				    {
				        ar_err("free mem error");
				    }
                }
            }
        }
        free(send_buffer);
    }
    return NULL;
}

static int close_camera_stream(tuning_server_t *obj, int stream_index)
{
    log_func_enter();

    int  i = 0;
    //release raw frame firmware buffer
    if (obj->mem_raw_frame.len || obj->mem_raw_frame.va)
    {
        ar_always("release raw frame buffer");
        ar_hal_sys_mmz_free((AR_U64)obj->mem_raw_frame.pa, obj->mem_raw_frame.va);
    }

    for (i = 0; i < obj->obj->nframes; i++)
    {
        if (obj->mem_raw_buf[i].va)
        {
             ar_hal_sys_mmz_free((AR_U64)obj->mem_raw_frame.pa, obj->mem_raw_buf[i].va);
        }
    }

    log_func_exit();
    return 0;
}
static int stop_stream_index(tuning_server_t *obj, int stream_index)
{
    ar_always("DO stream off");
    int fd_pipe = 0, pipe_id = 0, stream_id = 0;
    fd_pipe = obj->obj->pipe_fd;
    pipe_id = obj->obj->pipe_id;
    stream_id = obj->obj->stream_id[stream_index];
    ar_hal_vin_stop_stream(fd_pipe, pipe_id, stream_id);

    return 0;
}

static int process_start_video_l_assist(tuning_server_t *obj, STRU_TUNING_REQ_START_PREVIEW_PRA_T *pra, STRU_AR_STREAM_PROPERTY_T *prop)
{
    tuning_stream_t *video_stream=NULL;
    if(obj->obj->first_open)
    {
        obj->obj->first_open=0;
    }
    video_stream=&obj->obj->stream[TUNING_STREAM_VIDEO];
    video_stream->stop_flag = 0;
    video_stream->format=prop->format;
    video_stream->bit_depth=get_format_bitdpth(video_stream->format);

    video_stream->width=prop->width;
    video_stream->height=prop->height;
    video_stream->stream_type = prop->stream_type;
    video_stream->stride =prop->stride;
    pthread_create(&video_stream->tid,NULL,video_thread_assist,obj);
    return 0;
}
static int process_start_raw_l_asssit(tuning_server_t *obj,STRU_TUNING_REQ_START_PREVIEW_PRA_T *pra, STRU_AR_STREAM_PROPERTY_T *prop)
{
    tuning_stream_t *raw_stream=NULL;

    if(obj->obj->first_open)
    {
        obj->obj->first_open=0;
    }
    raw_stream=&obj->obj->stream[TUNING_STREAM_RAW];
    raw_stream->stop_flag = 0;
    raw_stream->format=prop->format;
    raw_stream->bit_depth = get_format_bitdpth(raw_stream->format);

    raw_stream->width=prop->width;
    raw_stream->height=prop->height;
    raw_stream->stream_type = prop->stream_type;
    raw_stream->stride =prop->stride;
    pthread_create(&raw_stream->tid,NULL,raw_thread_assist,obj);
    return 0;
}

static int process_start_preview_l_assist(tuning_server_t *obj,STRU_TUNING_REQ_START_PREVIEW_PRA_T *pra, STRU_AR_STREAM_PROPERTY_T *prop)
{
    tuning_stream_t *preview_stream=NULL;

    if(obj->obj->first_open)
    {
        obj->obj->first_open=0;
    }

    preview_stream=&obj->obj->stream[TUNING_STREAM_PREVIEW];
    preview_stream->stop_flag = 0;
    preview_stream->format=prop->format;
    preview_stream->bit_depth=get_format_bitdpth(preview_stream->format);

    preview_stream->width=prop->width;
    preview_stream->height=prop->height;
    preview_stream->stream_type = prop->stream_type;
    preview_stream->stride =prop->stride;
    pthread_create(&preview_stream->tid,NULL,preview_thread_assist,obj);
    return 0;
}

static int process_req_dis_connect_assit(tuning_server_t *obj, STRU_TUNING_REQ_DIS_CONNECT_PRA_T *pra)
{
    log_func_enter();
    // int off=0;
    if(obj->server_status != SERVER_CONNECTED){
        ar_err("disconnect error");
        return TUNING_ERR_UNKNOW;
    }
    if(obj->obj)
    {
        obj->server_status=SERVER_DISCONECT;
        close(obj->vin_fd);
        free(obj->obj);
        obj->obj = NULL;
    }
    log_func_exit();
    return 0;
}


static int process_req_dis_connect_ack(tuning_server_t *obj,STRU_TUNING_REQ_DIS_CONNECT_PRA_T *pra,int status)
{
    log_func_enter();
    int ack_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_DIS_CONNECT_ACK_T);
    tuning_ack_t *ack=(tuning_ack_t *)malloc(ack_len);
	if(!ack)
    {
      return -1;
    }
    STRU_TUNING_REQ_DIS_CONNECT_ACK_T *ack_pra=(STRU_TUNING_REQ_DIS_CONNECT_ACK_T *)get_ack_pra(((uint8_t *)ack));
    ack->request_type=TUNING_REQ_DIS_CONNECT;
    ack->error_code=status;
    ack->len=sizeof(STRU_TUNING_REQ_DIS_CONNECT_ACK_T);
    //fill the ack pra by req return
    ack_pra->request_type=pra->request_type;
    obj->send(obj->client,(uint8_t *)ack,ack_len,obj->msg_id,0);
    free(ack);
    log_func_exit();
    return 0;
}

static int process_req_stop_preview_assist(tuning_server_t *obj, STRU_TUNING_REQ_STOP_PREVIEW_PRA_T *pra)
{
    log_func_enter();
    if(obj->server_status != SERVER_STREAM_RUNING){
        ar_err("stop preview error");
        return TUNING_ERR_UNKNOW;
    }

#if 1
    obj->obj->stream[TUNING_STREAM_PREVIEW].stop_flag = 1;

    stop_stream_index(obj, TUNING_STREAM_PREVIEW);
    obj->obj->stream[TUNING_STREAM_VIDEO].stop_flag = 1;
    stop_stream_index(obj,TUNING_STREAM_VIDEO);
    obj->obj->stream[TUNING_STREAM_RAW].stop_flag = 1;
    stop_stream_index(obj,TUNING_STREAM_RAW);
    if(obj->obj->sensor_hdr)
    {
        //obj->obj->stream[TUNING_STREAM_HDR_RAW].stop_flag = 1;
        //stop_stream_index(obj,TUNING_STREAM_HDR_RAW);
    }
    // pthread_join(obj->obj->stream[TUNING_STREAM_RAW].tid, NULL);
    // pthread_join(obj->obj->stream[TUNING_STREAM_PREVIEW].tid, NULL);
    // pthread_join(obj->obj->stream[TUNING_STREAM_VIDEO].tid, NULL);

    close_camera_stream(obj, TUNING_STREAM_RAW);
    if(obj->obj->sensor_hdr)
    {
        //close_camera_stream(obj, TUNING_STREAM_HDR_RAW);
    }
    close_camera_stream(obj, TUNING_STREAM_PREVIEW);
    close_camera_stream(obj, TUNING_STREAM_VIDEO);
    obj->server_status=SERVER_CONNECTED;
#endif
    log_func_exit();
    return 0;
}

static int process_req_stop_preview_ack(tuning_server_t *obj,STRU_TUNING_REQ_STOP_PREVIEW_PRA_T *pra,int status)
{
    log_func_enter();
    int ack_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_STOP_PREVIEW_ACK_T);
    tuning_ack_t *ack=(tuning_ack_t *)malloc(ack_len);
	if(!ack)
    {
      return -1;
    }
    STRU_TUNING_REQ_STOP_PREVIEW_ACK_T *ack_pra=(STRU_TUNING_REQ_STOP_PREVIEW_ACK_T *)get_ack_pra(((uint8_t *)ack));
    ack->request_type=TUNING_REQ_STOP_PREVIEW;
    ack->error_code=status;
    ack->len=sizeof(STRU_TUNING_REQ_STOP_PREVIEW_ACK_T);
    //fill the ack pra by req return
    ack_pra->request_type=pra->request_type;
    obj->send(obj->client,(uint8_t *)ack,ack_len,obj->msg_id,0);
    free(ack);
    log_func_exit();
    return 0;
}

static void *event_loop_thread(void* arg)
{

   tuning_server_t *obj=(tuning_server_t *)arg;
   cam_obj_t *p_event_pbj=obj->obj;
   STRU_AR_EVENT_INFO_T * p_event_info=malloc(sizeof(STRU_AR_EVENT_INFO_T));
   if(!p_event_info)
   {
      return NULL;
   }
   STRU_CAM_SERVER_START_OPT_T *p_opt=malloc(sizeof(STRU_CAM_SERVER_START_OPT_T));
   if(!p_opt)
   {  
      free(p_event_info);
      return NULL;
   }
   
   int ret=ar_hal_vin_driver_get_start_opt(obj->vin_fd,p_opt);

   if(ret < 0)
   {     
       ar_err("ar_hal_vin_driver_get_start_opt err");
	   ar_osal_thread_exit();
       return NULL;
   }
   
   int event_data_size=sizeof(STRU_META_DATA_TYPE_T);
   int fd_mem=ar_hal_vin_open_dev_mem();
   while(1)
   {
        ret=ar_hal_vin_get_meta_event(p_event_pbj->pipe_fd,p_event_pbj->pipe_id,p_event_pbj->event_client_id,p_event_info);

        if(ret <0)
        {
           ar_err("get_meta event err");
		   break;
        }
		
        if(p_event_info->event_code==-1)
        {
           ar_hal_vin_release_meta_event(p_event_pbj->pipe_fd,p_event_pbj->pipe_id,p_event_pbj->event_client_id,p_event_info);
		   ar_always("exit event arrvied");
		   break;
        }
        //map the event data
        AR_S32 offset_ret=0;
        void *event_data_maped=ar_hal_vin_map_phy_addr(fd_mem,p_event_info->event_phy,event_data_size,&offset_ret);
        void *event_data=event_data_maped+offset_ret;
		switch(p_event_info->event_code)
		{
			case META_TYPE_AF_GROUP:
			process_af_buffer_asssit(obj,fd_mem,event_data);
			break;
		    default:
		    break;
		}
		ar_hal_vin_release_meta_event(p_event_pbj->pipe_fd,p_event_pbj->pipe_id,p_event_pbj->event_client_id,p_event_info);
		ar_hal_vin_ummap_vaddr(event_data_maped,event_data_size);
   }
   free(p_event_info);
   free(p_opt);
   ar_hal_vin_close_dev_mem(fd_mem);
   ar_osal_thread_exit();
   return NULL;
}



static int process_start_preview_assist(tuning_server_t *obj, STRU_TUNING_REQ_START_PREVIEW_PRA_T *pra)
{
	log_func_enter();
	int ret = 0;
	if(obj->server_status!=SERVER_CONNECTED)
	{
		ar_err("not connected. please conenect tuing server");
		return TUNING_ERR_UNKNOW;
	}

	ar_always("%s",pra->name);

    int cam_id=-1;
	for(int i=0;i<obj->probe_sensor_num;i++)
    {
        ar_always("%s sensor[%d]=%s",pra->name,i,obj->sensor[i]);
        if(!strcmp(pra->name, obj->sensor[i]))
        {
            ar_always("find the sensor=%s cam_id = %d", obj->sensor[i], i);
            cam_id=i;
			break;
        }
    }
    if(cam_id<0)
    {
        ar_err("no camera app with artosync multimedia start");
		return TUNING_ERR_UNKNOW;
    }

	obj->obj->pipe_id= obj->pipe_id[cam_id];
	obj->obj->pipe_fd= obj->pipe_fd[cam_id];
	if(obj->obj->pipe_fd<0)
	{
		ar_err("open %s err, please first start your project app", obj->sensor[cam_id]);
		return TUNING_ERR_UNKNOW;
	}
    ar_always("pipe fd = %d, pipe id = %d\n", obj->obj->pipe_fd, obj->obj->pipe_id);

	int pipe_opend=0;

    ret=ar_hal_vin_pipe_is_opend(obj->vin_fd,obj->obj->pipe_id,&pipe_opend);

	if(ret!=0 || !pipe_opend)
	{	    
         ar_err("pipe fd = %d, pipe id = %d the dev not be enabled, can not used tool\n", obj->obj->pipe_fd, obj->obj->pipe_id);
		 return TUNING_ERR_UNKNOW;
	}
	
	obj->obj->first_open=1;
	obj->obj->sensor_hdr=pra->hdr_stream;
    //get pipe stream property: MAX_PIPE_NUM = MAX_STREAM_MAX
    int k = 0;
	STRU_AR_STREAM_PROPERTY_T prop;
	for (k = 0; k < MAX_PIPE_NUM; k++)
	{
	    ar_always("stream id = %d, stream_fd =  %d \n", obj->stream_id[cam_id][k], obj->stream_fd[cam_id][k]);
	    if ((obj->stream_id[cam_id][k] >= 0) && (obj->stream_fd[cam_id][k] > 0))
	    {
	        ar_hal_vin_get_stream_property(obj->obj->pipe_fd, obj->obj->pipe_id, obj->stream_id[cam_id][k], &prop);
	        if(prop.stream_type==STREAM_TYPE_RAW_BAYER)
	        {
	            int client_id=0;
				ar_always("raw stream have run, start raw thread for tool");
		        ar_hal_vin_create_stream_client(obj->obj->pipe_fd, obj->obj->pipe_id, obj->stream_id[cam_id][k], &client_id);
		        obj->stream_client[TUNING_STREAM_RAW] = client_id;
		        obj->obj->stream_id[TUNING_STREAM_RAW] = obj->stream_id[cam_id][k];
		        obj->obj->stream_fd[TUNING_STREAM_RAW] = obj->stream_fd[cam_id][k];
                ret = process_start_raw_l_asssit(obj, pra, &prop);
            	if(ret != 0)
            	{
            		ar_err("start_video_l failed: %d \n", ret);
            		return ret;
            	}

	        }
	        else if (prop.stream_type == STREAM_TYPE_VIDEO || prop.stream_type == STREAM_TYPE_SIM_VIDEO) //isp scaler2
	        {
	            int client_id = 0;
				ar_always("video stream have run, start video thread for tool");
            	ar_hal_vin_create_stream_client(obj->obj->pipe_fd, obj->obj->pipe_id, obj->stream_id[cam_id][k], &client_id);
		        obj->stream_client[TUNING_STREAM_PREVIEW] = client_id;
                obj->obj->stream_id[TUNING_STREAM_PREVIEW] = obj->stream_id[cam_id][k];
		        obj->obj->stream_fd[TUNING_STREAM_PREVIEW] = obj->stream_fd[cam_id][k];
                ar_always(" tuning preview stream, client_id = %d, stream_id = %d, stream_fd = %d\n ", client_id,obj->stream_id[cam_id][k], obj->stream_fd[cam_id][k] );
                ret = process_start_preview_l_assist(obj, pra, &prop);
            	if(ret != 0)
            	{
            		ar_err("start_video_l failed: %d \n", ret);
            		return ret;
            	}

	        }
	        else if (prop.stream_type == STREAM_TYPE_PREVIEW || prop.stream_type == STREAM_TYPE_SIM_PREVIEW)//isp scaler1
	        {
	            int client_id = 0;
				ar_always("prview stream have run, start prview thread for tool");
            	ar_hal_vin_create_stream_client(obj->obj->pipe_fd, obj->obj->pipe_id, obj->stream_id[cam_id][k], &client_id);
		        obj->stream_client[TUNING_STREAM_VIDEO] = client_id;
		        obj->obj->stream_id[TUNING_STREAM_VIDEO] = obj->stream_id[cam_id][k];
		        obj->obj->stream_fd[TUNING_STREAM_VIDEO] = obj->stream_fd[cam_id][k];

                ret = process_start_video_l_assist(obj, pra, &prop);
            	if(ret != 0)
            	{
            		ar_err("start_video_l failed: %d \n", ret);
            		return ret;
            	}
	        }
	    }
	}
    //here we start a meta event to get af frame
    ret=ar_hal_vin_create_meta_event_client(obj->obj->pipe_fd,obj->obj->pipe_id,&obj->obj->event_client_id);
	if(ret==CAM_ERR_SUCCUESS)
	{
        ar_always("event_client_id=%d",obj->obj->event_client_id);
        ar_os_thread_attr_t ctrl_attr = {0};
        ctrl_attr.name = "event_loop_thread";
        ctrl_attr.stack_size = 16 * 1024;
        obj->obj->event_thread=ar_osal_thread_new(event_loop_thread, obj, &ctrl_attr);
        //after wait on event, we can register all the event, if user use event ,please register your intrest event, not all
        for(int i=META_TYPE_AF_GROUP;i<=META_TYPE_AF_GROUP;i++)
        {
             ar_hal_vin_register_meta_event(obj->obj->pipe_fd,obj->obj->pipe_id,obj->obj->event_client_id,i);
        }
	}else
    {
        ar_err("creat meta event for af stats err");
    }

	obj->server_status=SERVER_STREAM_RUNING;
	STRU_SENSOR_DEV_INFO_T dev_infor={0};

    ar_hal_vin_get_dev_infor(obj->vin_fd, cam_id, &dev_infor);

	ar_always("face=%d cam_mode=%d",dev_infor.face,pra->cam_mode);

    if (dev_infor.face==CAM_FACE_EMULATOT && (pra->cam_mode == SIM_RAW_MODE))
	{
	    //first creat the qbuf thread to get the raw
        pthread_create(&obj->tid_qbuf,NULL,q_stream_buf_thread_assist,obj);
        pthread_create(&obj->tid_dq, NULL, dq_stream_buf_thread_assist, obj);
        obj->obj->fps = 30;
	}
	strcpy(obj->stream_sensor_name, pra->name);
	log_func_exit();
	return 0;
}


static int process_start_preview_ack(tuning_server_t *obj, STRU_TUNING_REQ_START_PREVIEW_PRA_T *pra, int status)
{
    log_func_enter();
    int ack_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_START_PREVIEW_ACK_T);
    tuning_ack_t *ack=(tuning_ack_t *)malloc(ack_len);
	if(!ack)
    {
      return -1;
    }
    STRU_TUNING_REQ_CONNECT_ACK_T *ack_pra=(STRU_TUNING_REQ_CONNECT_ACK_T *)get_ack_pra(((uint8_t *)ack));
    ack->request_type=TUNING_REQ_START_PREVIEW;
    ack->error_code=status;
    ack->len=sizeof(STRU_TUNING_REQ_CONNECT_ACK_T);
    //fill the ack pra by req return
    ack_pra->request_type=pra->request_type;
    obj->send(obj->client,(uint8_t *)ack,ack_len,obj->msg_id,0);
    free(ack);
    log_func_exit();
    return 0;
}

static int process_lock_tuning_assist(tuning_server_t *obj,STRU_TUNING_REQ_LOCK_TUNING_PRA_PRA_T *pra)
{
    log_func_enter();
    if(obj->server_status!=SERVER_STREAM_RUNING)
    {
        ar_err("please open the prview");
        return TUNING_ERR_UNKNOW;
    }

    int fd_mm=ar_hal_vin_open_dev_mem();
	if(fd_mm<0)
	{
	   ar_err("err open dev/mem \n");
	   return -1;
	}
	STRU_PIPE_SET_TUNING_REQUEST_T *req_v=NULL;

	STRU_PIPE_SET_TUNING_REQUEST_T *req=NULL;

	STRU_VIN_MEM_T tuning_req_mem;
	ar_hal_vin_get_tuning_req(obj->obj->pipe_fd, obj->obj->pipe_id, &tuning_req_mem);
	req=tuning_req_mem.remote_pa;

    AR_S32 offset_ret=0;
	void *req_addr=ar_hal_vin_map_phy_addr(fd_mm,(void *)req, sizeof(STRU_PIPE_SET_TUNING_REQUEST_T), &offset_ret);
	req_v=(STRU_PIPE_SET_TUNING_REQUEST_T *)(req_addr+offset_ret);
    req_v->req_type = TUNING_REQ_LOCK_TUNING_PRA;
    req_v->req_pra.lock_tuning_pra = *pra;
	req_v->len=sizeof(STRU_TUNING_REQ_LOCK_TUNING_PRA_PRA_T);

	ar_hal_vin_process_tuning_req(obj->obj->pipe_fd, obj->obj->pipe_id, &tuning_req_mem);
	ar_hal_vin_release_tuning_req(obj->obj->pipe_fd,obj->obj->pipe_id,&tuning_req_mem);
	ar_hal_vin_ummap_vaddr(req_addr,sizeof(STRU_PIPE_SET_TUNING_REQUEST_T));
	ar_hal_vin_close_dev_mem(fd_mm);

    return 0;
    log_func_exit();
}

static int process_lock_tuning_ack(tuning_server_t *obj,STRU_TUNING_REQ_LOCK_TUNING_PRA_PRA_T *pra,int status)
{
    log_func_enter();
    int ack_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_LOCK_TUNING_PRA_ACK_T);
    tuning_ack_t *ack=(tuning_ack_t *)malloc(ack_len);
	if(!ack)
    {
      return -1;
    }
    STRU_TUNING_REQ_LOCK_TUNING_PRA_ACK_T *ack_pra=(STRU_TUNING_REQ_LOCK_TUNING_PRA_ACK_T *)get_ack_pra(((uint8_t *)ack));
    ack->request_type=TUNING_REQ_LOCK_TUNING_PRA;
    ack->error_code=status;
    ack->len=sizeof(STRU_TUNING_REQ_LOCK_TUNING_PRA_ACK_T);
    //fill the ack pra by req return
    ack_pra->request_type=pra->request_type;
    obj->send(obj->client,(uint8_t *)ack,ack_len,obj->msg_id,0);
    free(ack);
    log_func_exit();
    return 0;
}

static int process_unlock_tuning_assist(tuning_server_t *obj,STRU_TUNING_REQ_UNLOCK_TUNING_PRA_PRA_T *pra)
{
    log_func_enter();
    if(obj->server_status!=SERVER_STREAM_RUNING)
    {
        ar_err("please open the prview");
        return TUNING_ERR_UNKNOW;
    }
    int fd_mm=ar_hal_vin_open_dev_mem();
	if(fd_mm<0)
	{
	   ar_err("err open dev/mem \n");
	   return -1;
	}
	STRU_PIPE_SET_TUNING_REQUEST_T *req_v=NULL;

	STRU_PIPE_SET_TUNING_REQUEST_T *req=NULL;

	STRU_VIN_MEM_T tuning_req_mem;
	ar_hal_vin_get_tuning_req(obj->obj->pipe_fd, obj->obj->pipe_id, &tuning_req_mem);
	req=tuning_req_mem.remote_pa;

    AR_S32 offset_ret=0;
	void *req_addr=ar_hal_vin_map_phy_addr(fd_mm,(void *)req, sizeof(STRU_PIPE_SET_TUNING_REQUEST_T), &offset_ret);
	req_v=(STRU_PIPE_SET_TUNING_REQUEST_T *)(req_addr+offset_ret);
    req_v->req_type = TUNING_REQ_UNLOCK_TUNING_PRA;
    req_v->req_pra.unlock_tuning_pra = *pra;
	req_v->req_pra.unlock_tuning_pra.force_type=1;
	req_v->len=sizeof(STRU_TUNING_REQ_UNLOCK_TUNING_PRA_PRA_T);

	ar_hal_vin_process_tuning_req(obj->obj->pipe_fd, obj->obj->pipe_id, &tuning_req_mem);
	ar_hal_vin_release_tuning_req(obj->obj->pipe_fd,obj->obj->pipe_id,&tuning_req_mem);
	ar_hal_vin_ummap_vaddr(req_addr,sizeof(STRU_PIPE_SET_TUNING_REQUEST_T));
	ar_hal_vin_close_dev_mem(fd_mm);
    log_func_exit();
    return 0;
}

static int process_unlock_tuning_ack(tuning_server_t *obj,STRU_TUNING_REQ_UNLOCK_TUNING_PRA_PRA_T *pra,int status)
{
    log_func_enter();
    int ack_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_UNLOCK_TUNING_PRA_ACK_T);
    tuning_ack_t *ack=(tuning_ack_t *)malloc(ack_len);
	if(!ack)
    {
      return -1;
    }
    STRU_TUNING_REQ_UNLOCK_TUNING_PRA_ACK_T *ack_pra=(STRU_TUNING_REQ_UNLOCK_TUNING_PRA_ACK_T *)get_ack_pra(((uint8_t *)ack));
    ack->request_type=TUNING_REQ_UNLOCK_TUNING_PRA;
    ack->error_code=status;
    ack->len=sizeof(STRU_TUNING_REQ_UNLOCK_TUNING_PRA_ACK_T);
    //fill the ack pra by req return
    ack_pra->request_type=pra->request_type;
    obj->send(obj->client,(uint8_t *)ack,ack_len,obj->msg_id,0);
    free(ack);
    log_func_exit();
    return 0;
}

static int process_write_tuning_assist(tuning_server_t *obj,STRU_TUNING_REQ_WRITE_TUNING_PRA_PRA_T *pra)
{
    log_func_enter();
    if(obj->server_status!=SERVER_STREAM_RUNING)
    {
        ar_err("please open the prview");
        return TUNING_ERR_UNKNOW;
    }
    int fd_mm=ar_hal_vin_open_dev_mem();
	if(fd_mm<0)
	{
	   ar_err("err open dev/mem \n");
	   return -1;
	}
	STRU_PIPE_SET_TUNING_REQUEST_T *req_v=NULL;

	STRU_PIPE_SET_TUNING_REQUEST_T *req=NULL;

	STRU_VIN_MEM_T tuning_req_mem;
	ar_hal_vin_get_tuning_req(obj->obj->pipe_fd, obj->obj->pipe_id, &tuning_req_mem);
	req=tuning_req_mem.remote_pa;

    AR_S32 offset_ret=0;
	void *req_addr=ar_hal_vin_map_phy_addr(fd_mm,(void *)req, sizeof(STRU_PIPE_SET_TUNING_REQUEST_T), &offset_ret);
	req_v=(STRU_PIPE_SET_TUNING_REQUEST_T *)(req_addr+offset_ret);
    req_v->req_type = TUNING_REQ_WRITE_TUNING_PRA;
    req_v->req_pra.write_tuning_pra = *pra;
	req_v->len=sizeof(STRU_TUNING_REQ_WRITE_TUNING_PRA_PRA_T);

	ar_hal_vin_process_tuning_req(obj->obj->pipe_fd, obj->obj->pipe_id, &tuning_req_mem);
	ar_hal_vin_release_tuning_req(obj->obj->pipe_fd,obj->obj->pipe_id,&tuning_req_mem);
	ar_hal_vin_ummap_vaddr(req_addr,sizeof(STRU_PIPE_SET_TUNING_REQUEST_T));
	ar_hal_vin_close_dev_mem(fd_mm);
    log_func_exit();
    return 0;
}


static int process_write_tuning_pra_ack(tuning_server_t *obj,STRU_TUNING_REQ_WRITE_TUNING_PRA_PRA_T *pra,int status)
{
    log_func_enter();
    int ack_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_WRITE_TUNING_PRA_ACK_T);
    tuning_ack_t *ack=(tuning_ack_t *)malloc(ack_len);
	if(!ack)
    {
      return -1;
    }
    STRU_TUNING_REQ_WRITE_TUNING_PRA_ACK_T *ack_pra=(STRU_TUNING_REQ_WRITE_TUNING_PRA_ACK_T *)get_ack_pra(((uint8_t *)ack));
    ack->request_type=TUNING_REQ_WRITE_TUNING_PRA;
    ack->error_code=status;
    ack->len=sizeof(STRU_TUNING_REQ_WRITE_TUNING_PRA_ACK_T);
    //fill the ack pra by req return
    ack_pra->request_type=pra->request_type;
    obj->send(obj->client,(uint8_t *)ack,ack_len,obj->msg_id,0);
    free(ack);
    log_func_exit();
    return 0;
}

static int process_write_sensor_assist(tuning_server_t *obj,STRU_TUNING_REQ_SENSOR_WRITE_PRA_T *pra)
{
    log_func_enter();
    if(obj->server_status!=SERVER_STREAM_RUNING)
    {
        ar_err("please open the prview");
        return TUNING_ERR_UNKNOW;
    }

    int fd_mm=ar_hal_vin_open_dev_mem();
	if(fd_mm<0)
	{
	   ar_err("err open dev/mem \n");
	   return -1;
	}
	STRU_PIPE_SET_TUNING_REQUEST_T *req_v=NULL;

	STRU_PIPE_SET_TUNING_REQUEST_T *req=NULL;

	STRU_VIN_MEM_T tuning_req_mem;
	ar_hal_vin_get_tuning_req(obj->obj->pipe_fd, obj->obj->pipe_id, &tuning_req_mem);
	req=tuning_req_mem.remote_pa;

    AR_S32 offset_ret=0;
	void *req_addr=ar_hal_vin_map_phy_addr(fd_mm,(void *)req, sizeof(STRU_PIPE_SET_TUNING_REQUEST_T), &offset_ret);
	req_v=(STRU_PIPE_SET_TUNING_REQUEST_T *)(req_addr+offset_ret);
    req_v->req_type = TUNING_REQ_SENSOR_WRITE;
    req_v->req_pra.write_sensor = *pra;
	req_v->len=sizeof(STRU_TUNING_REQ_SENSOR_WRITE_PRA_T);

	ar_hal_vin_process_tuning_req(obj->obj->pipe_fd, obj->obj->pipe_id, &tuning_req_mem);
	ar_hal_vin_release_tuning_req(obj->obj->pipe_fd,obj->obj->pipe_id,&tuning_req_mem);
	ar_hal_vin_ummap_vaddr(req_addr,sizeof(STRU_PIPE_SET_TUNING_REQUEST_T));
	ar_hal_vin_close_dev_mem(fd_mm);
    log_func_exit();
    return 0;
}



static int process_write_sensor_ack(tuning_server_t *obj,STRU_TUNING_REQ_SENSOR_WRITE_PRA_T *pra,int status)
{
    log_func_enter();
    int ack_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_SENSOR_WRITE_PRA_T);
    tuning_ack_t *ack=(tuning_ack_t *)malloc(ack_len);
	if(!ack)
    {
      return -1;
    }
    STRU_TUNING_REQ_SENSOR_WRITE_PRA_T *ack_pra=(STRU_TUNING_REQ_SENSOR_WRITE_PRA_T *)get_ack_pra(((uint8_t *)ack));
    ack->request_type=TUNING_REQ_SENSOR_WRITE;
    ack->error_code=status;
    ack->len=sizeof(STRU_TUNING_REQ_SENSOR_WRITE_PRA_T);
    //fill the ack pra by req return
    ack_pra->request_type=pra->request_type;
    obj->send(obj->client,(uint8_t *)ack,ack_len,obj->msg_id,0);
    free(ack);
    log_func_exit();
    return 0;
}

static int process_read_sensor(tuning_server_t *obj, STRU_TUNING_REQ_SENSOR_READ_PRA_T *pra)
{
    log_func_enter();
    log_func_exit();
    return 0;
}

static int process_read_sensor_ack_asssit(tuning_server_t *obj,STRU_TUNING_REQ_SENSOR_READ_PRA_T *pra,int status)
{
    log_func_enter();
    int i=0;
    int ack_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_SENSOR_READ_ACK_T);
    tuning_ack_t *ack=(tuning_ack_t *)malloc(ack_len);
	if(!ack)
    {
      return -1;
    }
    STRU_TUNING_REQ_SENSOR_READ_ACK_T *ack_pra=(STRU_TUNING_REQ_SENSOR_READ_ACK_T *)get_ack_pra(((uint8_t *)ack));
    ack->request_type=TUNING_REQ_SENSOR_READ;
    ack->error_code=status;
    ack->len=sizeof(STRU_TUNING_REQ_SENSOR_READ_ACK_T);

    //fill the ack pra by req return
    //fill the read pra according the req
    if(obj->server_status==SERVER_STREAM_RUNING)
    {
        ack_pra->reg_num=pra->reg_num;
        for(i=0;i<ack_pra->reg_num;i++){
           ack_pra->address[i]=pra->address[i];
        }

        int fd_mm=ar_hal_vin_open_dev_mem();
    	if(fd_mm<0)
    	{
    	   ar_err("err open dev/mem \n");
    	   return -1;
    	}
    	STRU_PIPE_SET_TUNING_REQUEST_T *req_v=NULL;

    	STRU_PIPE_SET_TUNING_REQUEST_T *req=NULL;

    	STRU_VIN_MEM_T tuning_req_mem;
    	ar_hal_vin_get_tuning_req(obj->obj->pipe_fd, obj->obj->pipe_id, &tuning_req_mem);
    	req=tuning_req_mem.remote_pa;

        AR_S32 offset_ret=0;
    	void *req_addr=ar_hal_vin_map_phy_addr(fd_mm,(void *)req, sizeof(STRU_PIPE_SET_TUNING_REQUEST_T), &offset_ret);
    	req_v=(STRU_PIPE_SET_TUNING_REQUEST_T *)(req_addr+offset_ret);
        req_v->req_type = TUNING_REQ_SENSOR_READ;
        req_v->req_pra.read_sensor = *ack_pra;
    	req_v->len=sizeof(STRU_TUNING_REQ_SENSOR_WRITE_PRA_T);

    	ar_hal_vin_process_tuning_req(obj->obj->pipe_fd, obj->obj->pipe_id, &tuning_req_mem);
        *ack_pra=req_v->req_pra.read_sensor;		
    	ar_hal_vin_release_tuning_req(obj->obj->pipe_fd,obj->obj->pipe_id,&tuning_req_mem);
    	ar_hal_vin_ummap_vaddr(req_addr,sizeof(STRU_PIPE_SET_TUNING_REQUEST_T));
    	ar_hal_vin_close_dev_mem(fd_mm);
        log_func_exit();

    }
    else
    {
        ar_err("please open the prview");
        ack->error_code=TUNING_ERR_UNKNOW;
    }
    obj->send(obj->client,(uint8_t *)ack,ack_len,obj->msg_id,0);
    free(ack);
    log_func_exit();
    return 0;
}

static int process_endis_isp_module_assist(tuning_server_t *obj,STRU_TUNING_REQ_ENDEN_ISP_MODULE_PRA_T *pra)
{
    log_func_enter();
    if(obj->server_status!=SERVER_STREAM_RUNING)
    {
        ar_err("please open the prview");
        return TUNING_ERR_UNKNOW;
    }

    int fd_mm=ar_hal_vin_open_dev_mem();
	if(fd_mm<0)
	{
	   ar_err("err open dev/mem \n");
	   return -1;
	}
	STRU_PIPE_SET_TUNING_REQUEST_T *req_v=NULL;

	STRU_PIPE_SET_TUNING_REQUEST_T *req=NULL;

	STRU_VIN_MEM_T tuning_req_mem;
	ar_hal_vin_get_tuning_req(obj->obj->pipe_fd, obj->obj->pipe_id, &tuning_req_mem);
	req=tuning_req_mem.remote_pa;

    AR_S32 offset_ret=0;
	void *req_addr=ar_hal_vin_map_phy_addr(fd_mm,(void *)req, sizeof(STRU_PIPE_SET_TUNING_REQUEST_T), &offset_ret);
	req_v=(STRU_PIPE_SET_TUNING_REQUEST_T *)(req_addr+offset_ret);
    req_v->req_type = TUNING_REQ_ENDEN_ISP_MODULE;
    req_v->req_pra.enden_isp_module_pra = *pra;
	req_v->len=sizeof(STRU_TUNING_REQ_ENDEN_ISP_MODULE_PRA_T);

	ar_hal_vin_process_tuning_req(obj->obj->pipe_fd, obj->obj->pipe_id, &tuning_req_mem);
	ar_hal_vin_release_tuning_req(obj->obj->pipe_fd,obj->obj->pipe_id,&tuning_req_mem);
	ar_hal_vin_ummap_vaddr(req_addr,sizeof(STRU_PIPE_SET_TUNING_REQUEST_T));
	ar_hal_vin_close_dev_mem(fd_mm);

    log_func_exit();
    return 0;
}

static int process_endis_isp_module_ack(tuning_server_t *obj,STRU_TUNING_REQ_ENDEN_ISP_MODULE_PRA_T *pra,int status)
{
    log_func_enter();
    int ack_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_ENDEN_ISP_MODULE_ACK_T);
    tuning_ack_t *ack=(tuning_ack_t *)malloc(ack_len);
	if(!ack)
    {
      return -1;
    }
    STRU_TUNING_REQ_ENDEN_ISP_MODULE_ACK_T *ack_pra=(STRU_TUNING_REQ_ENDEN_ISP_MODULE_ACK_T *)get_ack_pra(((uint8_t *)ack));
    ack->request_type=TUNING_REQ_ENDEN_ISP_MODULE;
    ack->error_code=status;
    ack->len=sizeof(STRU_TUNING_REQ_ENDEN_ISP_MODULE_ACK_T);
    //fill the ack pra by req return
    ack_pra->request_type=pra->request_type;
    obj->send(obj->client,(uint8_t *)ack,ack_len,obj->msg_id,0);
    free(ack);
    log_func_exit();
    return 0;
}

static int process_read_tuning_pra(tuning_server_t *obj,STRU_TUNING_REQ_READ_TUNING_PRA_T *pra)
{
    log_func_enter();
    log_func_exit();
    return 0;
}

void write_file(const char *filename, char* context, int len)
{
    FILE *file = NULL;
    file = fopen(filename, "w+");
    long length;
    if(file == NULL)
    {
        ar_always("open file %s error\n", filename);
        return;
    }

    length = fwrite(context, 1, len, file);
    if(length != len){
        ar_always("write file %s error\n", filename);
    }

    fclose(file);
}

char* read_file(const char *filename, int *filelen) {
    FILE *file = NULL;
    long length = 0;
    char *content = NULL;
    size_t read_chars = 0;

    /* open in read binary mode */
    file = fopen(filename, "rb");
    if (file == NULL)
    {
        goto cleanup;
    }

    /* get the length */
    if (fseek(file, 0, SEEK_END) != 0)
    {
        goto cleanup;
    }
    length = ftell(file);
    if (length < 0)
    {
        goto cleanup;
    }
    if (fseek(file, 0, SEEK_SET) != 0)
    {
        goto cleanup;
    }

    /* allocate content buffer */
    content = (char*)malloc((size_t)length + sizeof(""));
    *filelen = length + sizeof("");
    if (content == NULL)
    {
        goto cleanup;
    }

    /* read the file into memory */
    read_chars = fread(content, sizeof(char), (size_t)length, file);
    if ((long)read_chars != length)
    {
        free(content);
        content = NULL;
        goto cleanup;
    }
    content[read_chars] = '\0';


cleanup:
    if (file != NULL)
    {
        fclose(file);
    }

    return content;
}
static int process_get_tuning_bin_ack_assist(tuning_server_t *obj, STRU_TUNING_REQ_GET_TUNING_BIN_PRA_T *pra, int status)
{
    FILE *fp = NULL;
    STRU_SENSE_INFO_T snse_pra={0};
    ar_hal_vin_get_sensce(obj->obj->pipe_fd, obj->obj->pipe_id, &snse_pra);
    ar_always("the tuning_name is %s\n", snse_pra.tuning_name);
    fp = fopen(snse_pra.tuning_name, "r");
    if(fp == NULL){
        ar_err("open tuning bin failed");
        return -1;
    }
    fseek(fp, 0L, SEEK_END);
    int file_len = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    int ack_len = sizeof(tuning_ack_t) + sizeof(STRU_TUNING_REQ_GET_TUNING_BIN_ACK_T) + file_len;
    tuning_ack_t *ack = (tuning_ack_t*)malloc(ack_len);
	if(!ack)
    {      
	  fclose(fp);
      return -1;
    }
    STRU_TUNING_REQ_GET_TUNING_BIN_ACK_T  *ack_pra = (STRU_TUNING_REQ_GET_TUNING_BIN_ACK_T*)get_ack_pra((uint8_t*)ack);
    char* file_data = (char *)ack + sizeof(tuning_ack_t) + sizeof(STRU_TUNING_REQ_GET_TUNING_BIN_ACK_T);
    fread(file_data, file_len, 1, fp);
    fclose(fp);

    ack->request_type = TUNING_REQ_GET_TUNING_BIN;
    ack->error_code = status;
    ack->len = sizeof(STRU_TUNING_REQ_GET_TUNING_BIN_ACK_T);
    ack_pra->request_type = ack->request_type;
    ack_pra->file_len = file_len;
	strcpy(ack_pra->name,snse_pra.tuning_name);
    obj->send(obj->client, (uint8_t*)ack, ack_len, obj->msg_id, 0);
    free(ack);
    log_func_exit();
    return 0;
}

static int process_set_tuning_bin_pra_assist(tuning_server_t *obj, STRU_TUNING_REQ_SET_TUNING_BIN_PRA_T *pra)
{
    long length;
    //write tuning bin to the file
    FILE *fp = NULL;
    ar_always("the file_name is %s\n", pra->file_name);
    fp = fopen(pra->file_name, "w+");
    if(fp == NULL){
       ar_err("open tuning bin failed");
        return -1;
    }
    length = fwrite(&pra->tuning_data, 1, sizeof(STRU_ALGO_ISP_TUNING_T), fp);
    if(length != sizeof(STRU_ALGO_ISP_TUNING_T)){
        ar_always("write file %s error\n", pra->file_name);
        fclose(fp);
        return -1;
    }
    fclose(fp);
    //the file have been write to disk
    //switch the tuing to this file
    STRU_SENSE_INFO_T snse_pra={0};
	strcpy(snse_pra.tuning_name,pra->file_name);
    ar_hal_vin_set_sensce(obj->obj->pipe_fd, obj->obj->pipe_id, &snse_pra);
    return 0;
}

static int process_set_tuning_bin_ack(tuning_server_t *obj,STRU_TUNING_REQ_SET_TUNING_BIN_PRA_T *pra,int status)
{
    log_func_enter();
    int ack_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_SET_TUNING_BIN_ACK_T);
    tuning_ack_t *ack=(tuning_ack_t *)malloc(ack_len);
	if(!ack)
    {
      return -1;
    }
    STRU_TUNING_REQ_SET_TUNING_BIN_ACK_T *ack_pra=(STRU_TUNING_REQ_SET_TUNING_BIN_ACK_T *)get_ack_pra(((uint8_t *)ack));
    ack->request_type=TUNING_REQ_SET_TUNING_BIN;
    ack->error_code=status;
    ack->len=sizeof(STRU_TUNING_REQ_SET_TUNING_BIN_ACK_T);
    //fill the ack pra by req return
    ack_pra->request_type=pra->request_type;
    obj->send(obj->client,(uint8_t *)ack,ack_len,obj->msg_id,0);
    free(ack);
    log_func_exit();
    return 0;
}


static int process_save_tuning_bin_pra_assist(tuning_server_t *obj, STRU_TUNING_REQ_SAVE_TUNING_PRA_TO_BIN_T *pra)
{
    if(obj->server_status==SERVER_STREAM_RUNING)
    {
        int fd_mm=ar_hal_vin_open_dev_mem();
    	if(fd_mm<0)
    	{
    	   ar_err("err open dev/mem \n");
    	   return -1;
    	}
    	STRU_PIPE_SET_TUNING_REQUEST_T *req_v=NULL;

    	STRU_PIPE_SET_TUNING_REQUEST_T *req=NULL;

    	STRU_VIN_MEM_T tuning_req_mem;
    	ar_hal_vin_get_tuning_req(obj->obj->pipe_fd, obj->obj->pipe_id, &tuning_req_mem);
    	req=tuning_req_mem.remote_pa;

        AR_S32 offset_ret=0;
    	void *req_addr=ar_hal_vin_map_phy_addr(fd_mm,(void *)req, sizeof(STRU_PIPE_SET_TUNING_REQUEST_T), &offset_ret);
    	req_v=(STRU_PIPE_SET_TUNING_REQUEST_T *)(req_addr+offset_ret);
        req_v->req_type = TUNING_REQ_SAVE_TUNING_PRA_TO_BIN;
        req_v->req_pra.save=*pra;
    	req_v->len=sizeof(STRU_TUNING_REQ_SAVE_TUNING_PRA_TO_BIN_T);
    	ar_hal_vin_process_tuning_req(obj->obj->pipe_fd, obj->obj->pipe_id, &tuning_req_mem);
    	//fill the ack pra by req return
    	ar_hal_vin_release_tuning_req(obj->obj->pipe_fd,obj->obj->pipe_id,&tuning_req_mem);
    	ar_hal_vin_ummap_vaddr(req_addr,sizeof(STRU_PIPE_SET_TUNING_REQUEST_T));
    	ar_hal_vin_close_dev_mem(fd_mm);

    }
    else
    {
        ar_err("please starting preview");
    }
	return 0;
}

static int process_save_tuning_bin_ack(tuning_server_t *obj,STRU_TUNING_REQ_SAVE_TUNING_PRA_TO_BIN_T *pra,int status)
{
    log_func_enter();
    int ack_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_SAVE_BIN_ACK_T);
    tuning_ack_t *ack=(tuning_ack_t *)malloc(ack_len);
	if(!ack)
    {
      return -1;
    }
    STRU_TUNING_REQ_SAVE_BIN_ACK_T *ack_pra=(STRU_TUNING_REQ_SAVE_BIN_ACK_T *)get_ack_pra(((uint8_t *)ack));
    ack->request_type=TUNING_REQ_SAVE_TUNING_PRA_TO_BIN;
    ack->error_code=status;
    ack->len=sizeof(STRU_TUNING_REQ_SAVE_BIN_ACK_T);
    //fill the ack pra by req return
    ack_pra->request_type=pra->request_type;
    obj->send(obj->client,(uint8_t *)ack,ack_len,obj->msg_id,0);
    free(ack);
    log_func_exit();
    return 0;
}

static int process_read_tuning_pra_assist_ack(tuning_server_t *obj,STRU_TUNING_REQ_READ_TUNING_PRA_T *pra,int status)
{
    log_func_enter();
    int ack_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_READ_TUNING_ACK_T);
    tuning_ack_t *ack=(tuning_ack_t *)malloc(ack_len);

    if(!ack)
    { 
       ar_err("lowmem");
       return -1;
    }
	
    STRU_TUNING_REQ_READ_TUNING_ACK_T *ack_pra=(STRU_TUNING_REQ_READ_TUNING_ACK_T *)get_ack_pra(((uint8_t *)ack));
    ack->request_type=TUNING_REQ_READ_TUNING_PRA;
    ack->error_code=status;
    ack->len=sizeof(STRU_TUNING_REQ_READ_TUNING_ACK_T);

    //read the tuning pra
    if(obj->server_status==SERVER_STREAM_RUNING)
    {
        int fd_mm=ar_hal_vin_open_dev_mem();
    	if(fd_mm<0)
    	{
    	   ar_err("err open dev/mem \n");
    	   return -1;
    	}
    	STRU_PIPE_SET_TUNING_REQUEST_T *req_v=NULL;

    	STRU_PIPE_SET_TUNING_REQUEST_T *req=NULL;

    	STRU_VIN_MEM_T tuning_req_mem;
    	ar_hal_vin_get_tuning_req(obj->obj->pipe_fd, obj->obj->pipe_id, &tuning_req_mem);
    	req=tuning_req_mem.remote_pa;

        AR_S32 offset_ret=0;
    	void *req_addr=ar_hal_vin_map_phy_addr(fd_mm,(void *)req, sizeof(STRU_PIPE_SET_TUNING_REQUEST_T), &offset_ret);
    	req_v=(STRU_PIPE_SET_TUNING_REQUEST_T *)(req_addr+offset_ret);
        req_v->req_type = TUNING_REQ_READ_TUNING_PRA;
        req_v->req_pra.read_tuning_pra.tuning_type = pra->tuning_type;
    	req_v->len=sizeof(STRU_TUNING_REQ_READ_TUNING_ACK_T);
    	ar_hal_vin_process_tuning_req(obj->obj->pipe_fd, obj->obj->pipe_id, &tuning_req_mem);
    	//fill the ack pra by req return
    	*ack_pra = req_v->req_pra.read_tuning_pra;
    	ar_hal_vin_release_tuning_req(obj->obj->pipe_fd,obj->obj->pipe_id,&tuning_req_mem);
    	ar_hal_vin_ummap_vaddr(req_addr,sizeof(STRU_PIPE_SET_TUNING_REQUEST_T));
    	ar_hal_vin_close_dev_mem(fd_mm);

    }
    else
    {
        ar_err("please starting preview");
        ack->error_code=TUNING_ERR_UNKNOW;
    }

    obj->send(obj->client,(uint8_t *)ack,ack_len,obj->msg_id,0);
    free(ack);
    log_func_exit();
    return 0;
}


static int process_read_cur_isp_pra(tuning_server_t *obj,STRU_TUNING_REQ_READ_CUR_ISP_PRA_T *pra)
{
    log_func_enter();
    log_func_exit();
    return 0;
}

static int process_read_cur_isp_pra_ack_assist(tuning_server_t *obj,STRU_TUNING_REQ_READ_CUR_ISP_PRA_T *pra,int status)
{
    log_func_enter();
    int ack_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_READ_CUR_ISP_ACK_T);
    tuning_ack_t *ack=(tuning_ack_t *)malloc(ack_len);

    if(!ack)
    {
       ar_err("no mem");
       return -1;
    }
    STRU_TUNING_REQ_READ_CUR_ISP_ACK_T *ack_pra=(STRU_TUNING_REQ_READ_CUR_ISP_ACK_T *)get_ack_pra(((uint8_t *)ack));
    ack->request_type=TUNING_REQ_READ_CUR_ISP_PRA;
    ack->error_code=status;
    ack->len=sizeof(STRU_TUNING_REQ_READ_CUR_ISP_ACK_T);
    //read the tuning pra
    if(obj->server_status==SERVER_STREAM_RUNING)
    {
        int fd_mm=ar_hal_vin_open_dev_mem();
    	if(fd_mm<0)
    	{
    	   ar_err("err open dev/mem \n");
    	   return -1;
    	}
    	STRU_PIPE_SET_TUNING_REQUEST_T *req_v=NULL;

    	STRU_PIPE_SET_TUNING_REQUEST_T *req=NULL;

    	STRU_VIN_MEM_T tuning_req_mem;
    	ar_hal_vin_get_tuning_req(obj->obj->pipe_fd, obj->obj->pipe_id, &tuning_req_mem);
    	req=tuning_req_mem.remote_pa;

        AR_S32 offset_ret=0;
    	void *req_addr=ar_hal_vin_map_phy_addr(fd_mm,(void *)req, sizeof(STRU_PIPE_SET_TUNING_REQUEST_T), &offset_ret);
    	req_v=(STRU_PIPE_SET_TUNING_REQUEST_T *)(req_addr+offset_ret);
        req_v->req_type = TUNING_REQ_READ_CUR_ISP_PRA;
        req_v->req_pra.read_tuning_pra.tuning_type = pra->tuning_type; //TUNING_REQ_READ_CUR_ISP_PRA
    	req_v->len=sizeof(STRU_TUNING_REQ_READ_TUNING_ACK_T);
    	ar_hal_vin_process_tuning_req(obj->obj->pipe_fd, obj->obj->pipe_id, &tuning_req_mem);
    	//fill the ack pra by req return
    	*ack_pra = req_v->req_pra.read_cur_isp_pra;
    	ar_hal_vin_release_tuning_req(obj->obj->pipe_fd,obj->obj->pipe_id,&tuning_req_mem);
    	ar_hal_vin_ummap_vaddr(req_addr,sizeof(STRU_PIPE_SET_TUNING_REQUEST_T));
    	ar_hal_vin_close_dev_mem(fd_mm);

    }else
    {
        ar_err("please starting preview");
        ack->error_code=TUNING_ERR_UNKNOW;
    }

    obj->send(obj->client,(uint8_t *)ack,ack_len,obj->msg_id,0);

    free(ack);
    log_func_exit();
    return 0;
}



static int process_req_preview_frame(tuning_server_t *obj,STRU_TUNING_REQ_PREVIEW_FRAME_PRA_T *pra)
{
    log_func_enter();
    if(obj->server_status!=SERVER_STREAM_RUNING)
    {
        ar_err("please open the prview");
        return TUNING_ERR_UNKNOW;
    }
    tuning_stream_t *stream=&obj->obj->stream[TUNING_STREAM_PREVIEW];

    stream->req_frame=pra->expected_frame_count;
    stream->send_data_mod=STREAM_MODE_YUV;
    log_func_exit();
    return 0;
}
static int process_req_preview_frame_ack(tuning_server_t *obj,STRU_TUNING_REQ_PREVIEW_FRAME_PRA_T *pra,int status)
{
    log_func_enter();
    int ack_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_PREVIEW_FRAME_ACK_T);
    tuning_ack_t *ack=(tuning_ack_t *)malloc(ack_len);
	if(!ack)
    {
      return -1;
    }
    STRU_TUNING_REQ_PREVIEW_FRAME_ACK_T *ack_pra=(STRU_TUNING_REQ_PREVIEW_FRAME_ACK_T *)get_ack_pra(((uint8_t *)ack));
    ack->request_type=TUNING_REQ_PREVIEW_FRAME;
    ack->error_code=status;
    ack->len=sizeof(STRU_TUNING_REQ_PREVIEW_FRAME_ACK_T);
    //fill the ack pra by req return

    if(obj->server_status==SERVER_STREAM_RUNING){
        tuning_stream_t *stream=&obj->obj->stream[TUNING_STREAM_PREVIEW];
        ack_pra->request_type=pra->request_type;
        ack_pra->panels=3;
        ack_pra->burst_count[0]=stream->height;
        ack_pra->burst_count[1]=stream->height/2;
        ack_pra->burst_count[2]=stream->height/2;
        ack_pra->data_flag=0;
        ack_pra->width=stream->width;
        ack_pra->height=stream->height;
        ack_pra->format=stream->format;
    }else
    {
        ar_err("please open the camera");
        ack->error_code=TUNING_ERR_UNKNOW;
    }

    obj->send(obj->client,(uint8_t *)ack,ack_len,obj->msg_id,0);
    free(ack);
    log_func_exit();
    return 0;
}


static int process_req_af_frame(tuning_server_t *obj,STRU_TUNING_REQ_AF_FRAME_PRA_T *pra)
{
    log_func_enter();
    if(obj->server_status!=SERVER_STREAM_RUNING)
    {
        ar_err("please open the prview");
        return TUNING_ERR_UNKNOW;
    }
    obj->obj->af_frame_count=pra->expected_frame_count;
    log_func_exit();
    return 0;
}
static int process_req_af_frame_ack(tuning_server_t *obj,STRU_TUNING_REQ_AF_FRAME_PRA_T *pra,int status)
{
    log_func_enter();
    int ack_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_AF_FRAME_ACK_T);
    tuning_ack_t *ack=(tuning_ack_t *)malloc(ack_len);
	if(!ack)
    {
      return -1;
    }
    STRU_TUNING_REQ_AF_FRAME_ACK_T *ack_pra=(STRU_TUNING_REQ_AF_FRAME_ACK_T *)get_ack_pra(((uint8_t *)ack));
    ack->request_type=TUNING_REQ_AF_FRAME;
    ack->error_code=status;
    ack->len=sizeof(STRU_TUNING_REQ_AF_FRAME_ACK_T);
    //fill the ack pra by req return

    if(obj->server_status==SERVER_STREAM_RUNING){
        ack_pra->request_type=pra->request_type;
        ack_pra->panels=1;
        ack_pra->burst_count[0]=1;
        ack_pra->burst_count[1]=0;
        ack_pra->burst_count[2]=0;
        ack_pra->data_flag=0;
        ack_pra->width=16;
        ack_pra->height=9;
        ack_pra->format=0;
    }else
    {
        ar_err("please open the camera");
        ack->error_code=TUNING_ERR_UNKNOW;
    }

    obj->send(obj->client,(uint8_t *)ack,ack_len,obj->msg_id,0);
    free(ack);
    log_func_exit();
    return 0;
}


static int process_req_snap_yuv_frame(tuning_server_t *obj,STRU_TUNING_REQ_SNAPSHOT_YUV_FRAME_PRA_T *pra)
{
    log_func_enter();
    if(obj->server_status!=SERVER_STREAM_RUNING)
    {
        ar_err("please open the prview");
        return TUNING_ERR_UNKNOW;
    }
    if(obj->recording_bitstream)
    {
        ar_err("can not take yuv picture when recording,please stop recording");
        return TUNING_ERR_UNKNOW;
    }
    tuning_stream_t *stream=&obj->obj->stream[TUNING_STREAM_VIDEO];
    stream->req_frame=pra->expected_frame_count;
    ar_always("get yuv fream: req_frame = %d \n", pra->expected_frame_count);
    stream->send_data_mod=STREAM_MODE_YUV;
    log_func_exit();
    return 0;
}
static int process_req_snap_yuv_frame_ack(tuning_server_t *obj,STRU_TUNING_REQ_SNAPSHOT_YUV_FRAME_PRA_T *pra,int status)
{
    log_func_enter();
    int ack_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_SNAPSHOT_YUV_FRAME_ACK_T);
    tuning_ack_t *ack=(tuning_ack_t *)malloc(ack_len);

    if(!ack)
    {
      return -1;
    }
	
    STRU_TUNING_REQ_SNAPSHOT_YUV_FRAME_ACK_T *ack_pra=(STRU_TUNING_REQ_SNAPSHOT_YUV_FRAME_ACK_T*)get_ack_pra(((uint8_t *)ack));
    ack->request_type=TUNING_REQ_SNAPSHOT_YUV_FRAME;
    ack->error_code=status;
    ack->len=sizeof(STRU_TUNING_REQ_SNAPSHOT_YUV_FRAME_ACK_T);
    //fill the ack pra by req return
    if(obj->server_status==SERVER_STREAM_RUNING)
    {
        tuning_stream_t *stream=&obj->obj->stream[TUNING_STREAM_VIDEO];
        ack_pra->request_type=pra->request_type;
        ack_pra->panels=3;
        ack_pra->burst_count[0]=stream->height;
        ack_pra->burst_count[1]=stream->height/2;
        ack_pra->burst_count[2]=stream->height/2;
        ack_pra->data_flag=0;
        ack_pra->width=stream->width;
        ack_pra->height=stream->height;
        ack_pra->format=stream->format;
    }else
    {
        ar_err("please open the preview");
        ack->error_code=TUNING_ERR_UNKNOW;
    }

    obj->send(obj->client,(uint8_t *)ack,ack_len,obj->msg_id,0);
    free(ack);
    log_func_exit();
    return 0;
}
static int process_req_raw_frame(tuning_server_t *obj,STRU_TUNING_REQ_RAW_FRAME_PRA_T *pra)
{
    log_func_enter();
    if(obj->server_status!=SERVER_STREAM_RUNING)
    {
        ar_err("please open the prview");
        return TUNING_ERR_UNKNOW;
    }
    tuning_stream_t *stream=&obj->obj->stream[TUNING_STREAM_RAW];
    stream->req_frame=pra->expected_frame_count;
    stream->send_data_mod=STREAM_MODE_RAW;
    obj->obj->with_3a = pra->with_3a;
    ar_always("req raw frame with 3a: %d",  obj->obj->with_3a);
    log_func_exit();
    return 0;
}

static int process_req_hdr_raw_frame(tuning_server_t *obj,STRU_TUNING_REQ_RAW_FRAME_PRA_T *pra)
{
    log_func_enter();
    if(obj->server_status!=SERVER_STREAM_RUNING)
    {
        ar_err("please open the prview");
        return TUNING_ERR_UNKNOW;
    }
    tuning_stream_t *stream=&obj->obj->stream[TUNING_STREAM_HDR_RAW];
    stream->req_frame=pra->expected_frame_count;
    stream->send_data_mod=STREAM_MODE_RAW;
    return 0;
    log_func_exit();
}

static int process_req_raw_frame_ack(tuning_server_t *obj,STRU_TUNING_REQ_RAW_FRAME_PRA_T *pra,int status)
{
    log_func_enter();
    int ack_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_RAW_FRAME_ACK_T);
    tuning_ack_t *ack=(tuning_ack_t *)malloc(ack_len);
	if(!ack)
    {
      return -1;
    }
    STRU_TUNING_REQ_RAW_FRAME_ACK_T *ack_pra=(STRU_TUNING_REQ_RAW_FRAME_ACK_T *)get_ack_pra(((uint8_t *)ack));
    ack->request_type=TUNING_REQ_RAW_FRAME;
    ack->error_code=status;
    ack->len=sizeof(STRU_TUNING_REQ_RAW_FRAME_ACK_T);
    //fill the ack pra by req return

    if(obj->server_status==SERVER_STREAM_RUNING){
        tuning_stream_t *stream=&obj->obj->stream[TUNING_STREAM_RAW];
        ack_pra->request_type=pra->request_type;
        ack_pra->panels=1;
        ack_pra->burst_count[0]=stream->height;
        ack_pra->burst_count[1]=0;
        ack_pra->burst_count[2]=0;
        ack_pra->data_flag=0;
        ack_pra->width=stream->width;
        ack_pra->height=stream->height;
        ack_pra->format=stream->format;
    }
    else
    {
        ar_err("plese open the preview");
        ack->error_code=TUNING_ERR_UNKNOW;
    }
    obj->send(obj->client,(uint8_t *)ack,ack_len,obj->msg_id,0);
    free(ack);
    log_func_exit();
    return 0;
}

static int process_req_hdr_raw_frame_ack(tuning_server_t *obj,STRU_TUNING_REQ_RAW_FRAME_PRA_T *pra,int status)
{
    log_func_enter();
    int ack_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_RAW_FRAME_ACK_T);
    tuning_ack_t *ack=(tuning_ack_t *)malloc(ack_len);
	if(!ack)
    {
      return -1;
    }
    STRU_TUNING_REQ_RAW_FRAME_ACK_T *ack_pra=(STRU_TUNING_REQ_RAW_FRAME_ACK_T *)get_ack_pra(((uint8_t *)ack));
    ack->request_type=TUNING_REQ_HDR_RAW_FRAME;
    ack->error_code=status;
    ack->len=sizeof(STRU_TUNING_REQ_RAW_FRAME_ACK_T);
    //fill the ack pra by req return

    if(obj->server_status==SERVER_STREAM_RUNING){
        tuning_stream_t *stream=&obj->obj->stream[TUNING_STREAM_HDR_RAW];
        ack_pra->request_type=pra->request_type;
        ack_pra->panels=1;
        ack_pra->burst_count[0]=stream->height;
        ack_pra->burst_count[1]=0;
        ack_pra->burst_count[2]=0;
        ack_pra->data_flag=0;
        ack_pra->width=stream->width;
        ack_pra->height=stream->height;
        ack_pra->format=stream->format;
    }else
    {
        ar_err("plese open the preview");
        ack->error_code=TUNING_ERR_UNKNOW;
    }

    obj->send(obj->client,(uint8_t *)ack,ack_len,obj->msg_id,0);
    free(ack);
    log_func_exit();
    return 0;
}

static int process_get_3a_info(tuning_server_t *obj,STRU_TUNING_REQ_GET_3A_INFO_PRA_T *pra)
{
    log_func_enter();
    log_func_exit();
    return 0;
}

static int process_get_3a_info_ack_assist(tuning_server_t *obj,STRU_TUNING_REQ_GET_3A_INFO_PRA_T *pra,int status)
{
    log_func_enter();
    int ack_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_GET_3A_INFO_ACK_T);
    tuning_ack_t *ack=(tuning_ack_t *)malloc(ack_len);
	if(!ack)
    {
      return -1;
    }
    STRU_TUNING_REQ_GET_3A_INFO_ACK_T *ack_pra=(STRU_TUNING_REQ_GET_3A_INFO_ACK_T *)get_ack_pra(((uint8_t *)ack));
    ack->request_type=TUNING_REQ_GET_3A_INFO;
    ack->error_code=status;
    ack->len=sizeof(STRU_TUNING_REQ_GET_3A_INFO_ACK_T);

    STRU_ALGO_3A_GET_INFO_T *p_info = malloc(sizeof(STRU_ALGO_3A_GET_INFO_T));

    if(!p_info)
    {
        free(ack);
		return -1;
    }
	
    //fill the ack pra by req return
    if(obj->server_status==SERVER_STREAM_RUNING){
        ack_pra->request_type=pra->request_type;

        if(ar_hal_vin_get_3a_info_ext(obj->obj->pipe_fd, obj->obj->pipe_id, p_info) < 0)
        {
            ar_err("get 3a info error");
            free(p_info);
            free(ack);
            return TUNING_ERR_UNKNOW;
        }
        ack_pra->get_3a_info=*p_info;
    }else
    {
        ar_err("please open the preview");
        ack->error_code=TUNING_ERR_UNKNOW;
    }
    //send to tool
    obj->send(obj->client,(uint8_t *)ack,ack_len,obj->msg_id,0);
    free(p_info);
    free(ack);
    log_func_exit();
    return 0;
}

static int process_lock_ae_assist(tuning_server_t *obj,STRU_TUNING_REQ_LOCK_AE_PRA_T *pra)
{
    log_func_enter();
    if(obj->server_status!=SERVER_STREAM_RUNING)
    {
        ar_err("please open the prview");
        return TUNING_ERR_UNKNOW;
    }

    int fd_mm=ar_hal_vin_open_dev_mem();
	if(fd_mm<0)
	{
	   ar_err("err open dev/mem \n");
	   return -1;
	}
	STRU_PIPE_SET_TUNING_REQUEST_T *req_v=NULL;

	STRU_PIPE_SET_TUNING_REQUEST_T *req=NULL;

	STRU_VIN_MEM_T tuning_req_mem;
	ar_hal_vin_get_tuning_req(obj->obj->pipe_fd, obj->obj->pipe_id, &tuning_req_mem);
	req=tuning_req_mem.remote_pa;

    AR_S32 offset_ret=0;
	void *req_addr=ar_hal_vin_map_phy_addr(fd_mm,(void *)req, sizeof(STRU_PIPE_SET_TUNING_REQUEST_T), &offset_ret);
	req_v=(STRU_PIPE_SET_TUNING_REQUEST_T *)(req_addr+offset_ret);
    req_v->req_type = TUNING_REQ_LOCK_AE;
	req_v->len=sizeof(STRU_TUNING_REQ_LOCK_AE_PRA_T);
	ar_hal_vin_process_tuning_req(obj->obj->pipe_fd, obj->obj->pipe_id, &tuning_req_mem);
	ar_hal_vin_release_tuning_req(obj->obj->pipe_fd,obj->obj->pipe_id,&tuning_req_mem);
	ar_hal_vin_ummap_vaddr(req_addr,sizeof(STRU_PIPE_SET_TUNING_REQUEST_T));
	ar_hal_vin_close_dev_mem(fd_mm);
    log_func_exit();

    return 0;
}

static int process_lock_ae_ack(tuning_server_t *obj,STRU_TUNING_REQ_LOCK_AE_PRA_T *pra,int status)
{
    log_func_enter();
    int ack_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_LOCK_AE_ACK_T);
    tuning_ack_t *ack=(tuning_ack_t *)malloc(ack_len);
	if(!ack)
    {
      return -1;
    }
    STRU_TUNING_REQ_LOCK_AE_ACK_T *ack_pra=(STRU_TUNING_REQ_LOCK_AE_ACK_T *)get_ack_pra(((uint8_t *)ack));
    ack->request_type=TUNING_REQ_LOCK_AE;
    ack->error_code=status;
    ack->len=sizeof(STRU_TUNING_REQ_LOCK_AE_ACK_T);
    //fill the ack pra by req return
    ack_pra->request_type=pra->request_type;
    //send to tool
    obj->send(obj->client,(uint8_t *)ack,ack_len,obj->msg_id,0);
    free(ack);
    log_func_exit();
    return 0;
}

static int process_unlock_ae_assist(tuning_server_t *obj,STRU_TUNING_REQ_UNLOCK_AE_PRA_T *pra)
{
    log_func_enter();
    if(obj->server_status!=SERVER_STREAM_RUNING)
    {
        ar_err("please open the prview");
        return TUNING_ERR_UNKNOW;
    }

    int fd_mm=ar_hal_vin_open_dev_mem();
	if(fd_mm<0)
	{
	   ar_err("err open dev/mem \n");
	   return -1;
	}
	STRU_PIPE_SET_TUNING_REQUEST_T *req_v=NULL;

	STRU_PIPE_SET_TUNING_REQUEST_T *req=NULL;

	STRU_VIN_MEM_T tuning_req_mem;
	ar_hal_vin_get_tuning_req(obj->obj->pipe_fd, obj->obj->pipe_id, &tuning_req_mem);
	req=tuning_req_mem.remote_pa;

    AR_S32 offset_ret=0;
	void *req_addr=ar_hal_vin_map_phy_addr(fd_mm,(void *)req, sizeof(STRU_PIPE_SET_TUNING_REQUEST_T), &offset_ret);
	req_v=(STRU_PIPE_SET_TUNING_REQUEST_T *)(req_addr+offset_ret);
    req_v->req_type = TUNING_REQ_UNLOCK_AE;
	req_v->len=sizeof(STRU_TUNING_REQ_UNLOCK_AE_PRA_T);
	ar_hal_vin_process_tuning_req(obj->obj->pipe_fd, obj->obj->pipe_id, &tuning_req_mem);
	ar_hal_vin_release_tuning_req(obj->obj->pipe_fd,obj->obj->pipe_id,&tuning_req_mem);
	ar_hal_vin_ummap_vaddr(req_addr,sizeof(STRU_PIPE_SET_TUNING_REQUEST_T));
	ar_hal_vin_close_dev_mem(fd_mm);
    log_func_exit();

    return 0;
}

static int process_unlock_ae_ack(tuning_server_t *obj, STRU_TUNING_REQ_UNLOCK_AE_PRA_T *pra, int status)
{
    log_func_enter();
    int ack_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_UNLOCK_AE_ACK_T);
    tuning_ack_t *ack=(tuning_ack_t *)malloc(ack_len);
	if(!ack)
    {
      return -1;
    }
    STRU_TUNING_REQ_UNLOCK_AE_ACK_T *ack_pra=(STRU_TUNING_REQ_UNLOCK_AE_ACK_T *)get_ack_pra(((uint8_t *)ack));
    ack->request_type=TUNING_REQ_UNLOCK_AE;
    ack->error_code=status;
    ack->len=sizeof(STRU_TUNING_REQ_UNLOCK_AE_ACK_T);
    //fill the ack pra by req return
    ack_pra->request_type=pra->request_type;
    //send to tool
    obj->send(obj->client,(uint8_t *)ack,ack_len,obj->msg_id,0);
    free(ack);
    log_func_exit();
    return 0;
}

static int process_set_man_ae_assist(tuning_server_t *obj,STRU_TUNING_REQ_SET_MANUAL_AE_PRA_T *pra)
{
    log_func_enter();
    if(obj->server_status!=SERVER_STREAM_RUNING)
    {
        ar_err("please open the prview");
        return TUNING_ERR_UNKNOW;
    }

    int fd_mm=ar_hal_vin_open_dev_mem();
    if(fd_mm<0)
    {
       ar_err("err open dev/mem \n");
       return -1;
    }
    STRU_PIPE_SET_TUNING_REQUEST_T *req_v=NULL;

    STRU_PIPE_SET_TUNING_REQUEST_T *req=NULL;

    STRU_VIN_MEM_T tuning_req_mem;
    ar_hal_vin_get_tuning_req(obj->obj->pipe_fd, obj->obj->pipe_id, &tuning_req_mem);
    req=tuning_req_mem.remote_pa;

    AR_S32 offset_ret=0;
    void *req_addr=ar_hal_vin_map_phy_addr(fd_mm,(void *)req, sizeof(STRU_PIPE_SET_TUNING_REQUEST_T), &offset_ret);
    req_v=(STRU_PIPE_SET_TUNING_REQUEST_T *)(req_addr+offset_ret);
    req_v->req_type = TUNING_REQ_SET_MANUAL_AE;
    req_v->req_pra.set_man_ae_pra=*pra;
    req_v->len=sizeof(STRU_TUNING_REQ_SET_MANUAL_AE_PRA_T);
    ar_hal_vin_process_tuning_req(obj->obj->pipe_fd, obj->obj->pipe_id, &tuning_req_mem);
    ar_hal_vin_release_tuning_req(obj->obj->pipe_fd,obj->obj->pipe_id,&tuning_req_mem);
    ar_hal_vin_ummap_vaddr(req_addr,sizeof(STRU_PIPE_SET_TUNING_REQUEST_T));
    ar_hal_vin_close_dev_mem(fd_mm);
    log_func_exit();

    return 0;
}


static int process_set_man_ae_ack(tuning_server_t *obj,STRU_TUNING_REQ_SET_MANUAL_AE_PRA_T *pra,int status)
{
    log_func_enter();
    int ack_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_SET_MANUAL_AE_ACK_T);
    tuning_ack_t *ack=(tuning_ack_t *)malloc(ack_len);
	if(!ack)
    {
      return -1;
    }
    STRU_TUNING_REQ_SET_MANUAL_AE_ACK_T *ack_pra=(STRU_TUNING_REQ_SET_MANUAL_AE_ACK_T *)get_ack_pra(((uint8_t *)ack));
    ack->request_type=TUNING_REQ_SET_MANUAL_AE;
    ack->error_code=status;
    ack->len=sizeof(STRU_TUNING_REQ_SET_MANUAL_AE_ACK_T);
    //fill the ack pra by req return
    ack_pra->request_type=pra->request_type;
    //send to tool
    obj->send(obj->client,(uint8_t *)ack,ack_len,obj->msg_id,0);
    free(ack);
    log_func_exit();
    return 0;
}

static int process_lock_awb_assist(tuning_server_t *obj,STRU_TUNING_REQ_LOCK_AWB_PRA_T *pra)
{
    log_func_enter();
    if(obj->server_status!=SERVER_STREAM_RUNING)
    {
        ar_err("please open the prview");
        return TUNING_ERR_UNKNOW;
    }

    int fd_mm=ar_hal_vin_open_dev_mem();
	if(fd_mm<0)
	{
	   ar_err("err open dev/mem \n");
	   return -1;
	}
	STRU_PIPE_SET_TUNING_REQUEST_T *req_v=NULL;

	STRU_PIPE_SET_TUNING_REQUEST_T *req=NULL;

	STRU_VIN_MEM_T tuning_req_mem;
	ar_hal_vin_get_tuning_req(obj->obj->pipe_fd, obj->obj->pipe_id, &tuning_req_mem);
	req=tuning_req_mem.remote_pa;

    AR_S32 offset_ret=0;
	void *req_addr=ar_hal_vin_map_phy_addr(fd_mm,(void *)req, sizeof(STRU_PIPE_SET_TUNING_REQUEST_T), &offset_ret);
	req_v=(STRU_PIPE_SET_TUNING_REQUEST_T *)(req_addr+offset_ret);
    req_v->req_type = TUNING_REQ_LOCK_AWB;
	req_v->len=sizeof(STRU_TUNING_REQ_LOCK_AWB_PRA_T);
	ar_hal_vin_process_tuning_req(obj->obj->pipe_fd, obj->obj->pipe_id, &tuning_req_mem);
	ar_hal_vin_release_tuning_req(obj->obj->pipe_fd,obj->obj->pipe_id,&tuning_req_mem);
	ar_hal_vin_ummap_vaddr(req_addr,sizeof(STRU_PIPE_SET_TUNING_REQUEST_T));
	ar_hal_vin_close_dev_mem(fd_mm);
    log_func_exit();
    return 0;
}


static int process_lock_awb_ack(tuning_server_t *obj,STRU_TUNING_REQ_LOCK_AWB_PRA_T *pra,int status)
{
    log_func_enter();
    int ack_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_LOCK_AWB_ACK_T);
    tuning_ack_t *ack=(tuning_ack_t *)malloc(ack_len);
	if(!ack)
    {
      return -1;
    }
    STRU_TUNING_REQ_LOCK_AWB_ACK_T *ack_pra=(STRU_TUNING_REQ_LOCK_AWB_ACK_T *)get_ack_pra(((uint8_t *)ack));
    ack->request_type=TUNING_REQ_LOCK_AWB;
    ack->error_code=status;
    ack->len=sizeof(STRU_TUNING_REQ_LOCK_AWB_ACK_T);
    //fill the ack pra by req return
    ack_pra->request_type=pra->request_type;
    //send to tool
    obj->send(obj->client,(uint8_t *)ack,ack_len,obj->msg_id,0);
    free(ack);
    log_func_exit();
    return 0;
}

static int process_unlock_awb_assist(tuning_server_t *obj,STRU_TUNING_REQ_UNLOCK_AWB_PRA_T *pra)
{
    log_func_enter();
    if(obj->server_status!=SERVER_STREAM_RUNING)
    {
        ar_err("please open the prview");
        return TUNING_ERR_UNKNOW;
    }

    int fd_mm=ar_hal_vin_open_dev_mem();
	if(fd_mm<0)
	{
	   ar_err("err open dev/mem \n");
	   return -1;
	}
	STRU_PIPE_SET_TUNING_REQUEST_T *req_v=NULL;

	STRU_PIPE_SET_TUNING_REQUEST_T *req=NULL;

	STRU_VIN_MEM_T tuning_req_mem;
	ar_hal_vin_get_tuning_req(obj->obj->pipe_fd, obj->obj->pipe_id, &tuning_req_mem);
	req=tuning_req_mem.remote_pa;

    AR_S32 offset_ret=0;
	void *req_addr=ar_hal_vin_map_phy_addr(fd_mm,(void *)req, sizeof(STRU_PIPE_SET_TUNING_REQUEST_T), &offset_ret);
	req_v=(STRU_PIPE_SET_TUNING_REQUEST_T *)(req_addr+offset_ret);
    req_v->req_type = TUNING_REQ_UNLOCK_AWB;
	req_v->len=sizeof(STRU_TUNING_REQ_UNLOCK_AWB_PRA_T);
	ar_hal_vin_process_tuning_req(obj->obj->pipe_fd, obj->obj->pipe_id, &tuning_req_mem);
	ar_hal_vin_release_tuning_req(obj->obj->pipe_fd,obj->obj->pipe_id,&tuning_req_mem);
	ar_hal_vin_ummap_vaddr(req_addr,sizeof(STRU_PIPE_SET_TUNING_REQUEST_T));
	ar_hal_vin_close_dev_mem(fd_mm);
    log_func_exit();
    return 0;
}


static int process_unlock_awb_ack(tuning_server_t *obj, STRU_TUNING_REQ_UNLOCK_AWB_PRA_T *pra, int status)
{
    log_func_enter();
    int ack_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_UNLOCK_AWB_ACK_T);
    tuning_ack_t *ack=(tuning_ack_t *)malloc(ack_len);
	if(!ack)
    {
      return -1;
    }
    STRU_TUNING_REQ_UNLOCK_AWB_ACK_T *ack_pra=(STRU_TUNING_REQ_UNLOCK_AWB_ACK_T *)get_ack_pra(((uint8_t *)ack));
    ack->request_type=TUNING_REQ_UNLOCK_AWB;
    ack->error_code=status;
    ack->len=sizeof(STRU_TUNING_REQ_UNLOCK_AWB_ACK_T);
    //fill the ack pra by req return
    ack_pra->request_type=pra->request_type;
    //send to tool
    obj->send(obj->client,(uint8_t *)ack,ack_len,obj->msg_id,0);
    free(ack);
    log_func_exit();
    return 0;
}

static int process_set_man_awb_assist(tuning_server_t *obj,STRU_TUNING_REQ_SET_MANUAL_AWB_PRA_T *pra)
{
    log_func_enter();
    if(obj->server_status!=SERVER_STREAM_RUNING)
    {
        ar_err("please open the prview");
        return TUNING_ERR_UNKNOW;
    }

    int fd_mm=ar_hal_vin_open_dev_mem();
    if(fd_mm<0)
    {
       ar_err("err open dev/mem \n");
       return -1;
    }
    STRU_PIPE_SET_TUNING_REQUEST_T *req_v=NULL;

    STRU_PIPE_SET_TUNING_REQUEST_T *req=NULL;

    STRU_VIN_MEM_T tuning_req_mem;
    ar_hal_vin_get_tuning_req(obj->obj->pipe_fd, obj->obj->pipe_id, &tuning_req_mem);
    req=tuning_req_mem.remote_pa;

    AR_S32 offset_ret=0;
    void *req_addr=ar_hal_vin_map_phy_addr(fd_mm,(void *)req, sizeof(STRU_PIPE_SET_TUNING_REQUEST_T), &offset_ret);
    req_v=(STRU_PIPE_SET_TUNING_REQUEST_T *)(req_addr+offset_ret);
    req_v->req_type = TUNING_REQ_SET_MANUAL_AWB;
    req_v->req_pra.set_awb_pra=*pra;
    req_v->len=sizeof(STRU_TUNING_REQ_SET_MANUAL_AWB_PRA_T);
    ar_hal_vin_process_tuning_req(obj->obj->pipe_fd, obj->obj->pipe_id, &tuning_req_mem);
    ar_hal_vin_release_tuning_req(obj->obj->pipe_fd,obj->obj->pipe_id,&tuning_req_mem);
    ar_hal_vin_ummap_vaddr(req_addr,sizeof(STRU_PIPE_SET_TUNING_REQUEST_T));
    ar_hal_vin_close_dev_mem(fd_mm);
    log_func_exit();
    return 0;
}

static int process_set_man_awb_ack(tuning_server_t *obj,STRU_TUNING_REQ_SET_MANUAL_AWB_PRA_T *pra,int status)
{
    log_func_enter();
    int ack_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_SET_MANUAL_AWB_ACK_T);
    tuning_ack_t *ack=(tuning_ack_t *)malloc(ack_len);
	if(!ack)
    {
      return -1;
    }
    STRU_TUNING_REQ_SET_MANUAL_AWB_ACK_T *ack_pra=(STRU_TUNING_REQ_SET_MANUAL_AWB_ACK_T *)get_ack_pra(((uint8_t *)ack));
    ack->request_type=TUNING_REQ_SET_MANUAL_AWB;
    ack->error_code=status;
    ack->len=sizeof(STRU_TUNING_REQ_SET_MANUAL_AWB_ACK_T);
    //fill the ack pra by req return
    ack_pra->request_type=pra->request_type;
    //send to tool
    obj->send(obj->client,(uint8_t *)ack,ack_len,obj->msg_id,0);
    free(ack);
    log_func_exit();
    return 0;
}

static int process_set_af_lock_assist(tuning_server_t *obj,STRU_TUNING_REQ_LOCK_AF_PRA_T *pra)
{
    log_func_enter();
    if(obj->server_status!=SERVER_STREAM_RUNING)
    {
        ar_err("please open the prview");
        return TUNING_ERR_UNKNOW;
    }

    int fd_mm=ar_hal_vin_open_dev_mem();
	if(fd_mm<0)
	{
	   ar_err("err open dev/mem \n");
	   return -1;
	}
	STRU_PIPE_SET_TUNING_REQUEST_T *req_v=NULL;

	STRU_PIPE_SET_TUNING_REQUEST_T *req=NULL;

	STRU_VIN_MEM_T tuning_req_mem;
	ar_hal_vin_get_tuning_req(obj->obj->pipe_fd, obj->obj->pipe_id, &tuning_req_mem);
	req=tuning_req_mem.remote_pa;

    AR_S32 offset_ret=0;
	void *req_addr=ar_hal_vin_map_phy_addr(fd_mm,(void *)req, sizeof(STRU_PIPE_SET_TUNING_REQUEST_T), &offset_ret);
	req_v=(STRU_PIPE_SET_TUNING_REQUEST_T *)(req_addr+offset_ret);
    req_v->req_type = TUNING_REQ_LOCK_AF;
	req_v->len=sizeof(STRU_TUNING_REQ_LOCK_AF_PRA_T);
	ar_hal_vin_process_tuning_req(obj->obj->pipe_fd, obj->obj->pipe_id, &tuning_req_mem);
	ar_hal_vin_release_tuning_req(obj->obj->pipe_fd,obj->obj->pipe_id,&tuning_req_mem);
	ar_hal_vin_ummap_vaddr(req_addr,sizeof(STRU_PIPE_SET_TUNING_REQUEST_T));
	ar_hal_vin_close_dev_mem(fd_mm);
    log_func_exit();
    return 0;
}

static int process_set_af_lock_ack(tuning_server_t *obj,STRU_TUNING_REQ_LOCK_AF_PRA_T *pra,int status)
{
    log_func_enter();
    int ack_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_LOCK_AF_ACK_T);
    tuning_ack_t *ack=(tuning_ack_t *)malloc(ack_len);
	if(!ack)
    {
      return -1;
    }
    STRU_TUNING_REQ_LOCK_AF_ACK_T *ack_pra=(STRU_TUNING_REQ_LOCK_AF_ACK_T *)get_ack_pra(((uint8_t *)ack));
    ack->request_type=TUNING_REQ_LOCK_AF;
    ack->error_code=status;
    ack->len=sizeof(STRU_TUNING_REQ_LOCK_AF_ACK_T);
    //fill the ack pra by req return
    ack_pra->request_type=pra->request_type;
    //send to tool
    obj->send(obj->client,(uint8_t *)ack,ack_len,obj->msg_id,0);
    free(ack);
    log_func_exit();
    return 0;
}

static int process_set_af_unlock_assist(tuning_server_t *obj,STRU_TUNING_REQ_UNLOCK_AF_PRA_T *pra)
{
    log_func_enter();
    if(obj->server_status!=SERVER_STREAM_RUNING)
    {
        ar_err("please open the prview");
        return TUNING_ERR_UNKNOW;
    }

    int fd_mm=ar_hal_vin_open_dev_mem();
	if(fd_mm<0)
	{
	   ar_err("err open dev/mem \n");
	   return -1;
	}
	STRU_PIPE_SET_TUNING_REQUEST_T *req_v=NULL;

	STRU_PIPE_SET_TUNING_REQUEST_T *req=NULL;

	STRU_VIN_MEM_T tuning_req_mem;
	ar_hal_vin_get_tuning_req(obj->obj->pipe_fd, obj->obj->pipe_id, &tuning_req_mem);
	req=tuning_req_mem.remote_pa;

    AR_S32 offset_ret=0;
	void *req_addr=ar_hal_vin_map_phy_addr(fd_mm,(void *)req, sizeof(STRU_PIPE_SET_TUNING_REQUEST_T), &offset_ret);
	req_v=(STRU_PIPE_SET_TUNING_REQUEST_T *)(req_addr+offset_ret);
    req_v->req_type = TUNING_REQ_UNLOCK_AF;
	req_v->len=sizeof(STRU_TUNING_REQ_UNLOCK_AF_PRA_T);
	ar_hal_vin_process_tuning_req(obj->obj->pipe_fd, obj->obj->pipe_id, &tuning_req_mem);
	ar_hal_vin_release_tuning_req(obj->obj->pipe_fd,obj->obj->pipe_id,&tuning_req_mem);
	ar_hal_vin_ummap_vaddr(req_addr,sizeof(STRU_PIPE_SET_TUNING_REQUEST_T));
	ar_hal_vin_close_dev_mem(fd_mm);
    log_func_exit();
    return 0;
}

static int process_set_af_unlock_ack(tuning_server_t *obj,STRU_TUNING_REQ_UNLOCK_AF_PRA_T *pra,int status)
{
    log_func_enter();
    int ack_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_UNLOCK_AF_ACK_T);
    tuning_ack_t *ack=(tuning_ack_t *)malloc(ack_len);
	if(!ack)
    {
      return -1;
    }
    STRU_TUNING_REQ_UNLOCK_AF_ACK_T *ack_pra=(STRU_TUNING_REQ_UNLOCK_AF_ACK_T *)get_ack_pra(((uint8_t *)ack));
    ack->request_type=TUNING_REQ_UNLOCK_AF;
    ack->error_code=status;
    ack->len=sizeof(STRU_TUNING_REQ_UNLOCK_AF_ACK_T);
    //fill the ack pra by req return
    ack_pra->request_type=pra->request_type;
    //send to tool
    obj->send(obj->client,(uint8_t *)ack,ack_len,obj->msg_id,0);
    free(ack);
    log_func_exit();
    return 0;
}

static int process_set_manual_af_assist(tuning_server_t *obj,STRU_TUNING_REQ_SET_MANUAL_AF_PRA_T *pra)
{
    log_func_enter();
    if(obj->server_status!=SERVER_STREAM_RUNING)
    {
        ar_err("please open the prview");
        return TUNING_ERR_UNKNOW;
    }

    int fd_mm=ar_hal_vin_open_dev_mem();
    if(fd_mm<0)
    {
       ar_err("err open dev/mem \n");
       return -1;
    }
    STRU_PIPE_SET_TUNING_REQUEST_T *req_v=NULL;

    STRU_PIPE_SET_TUNING_REQUEST_T *req=NULL;

    STRU_VIN_MEM_T tuning_req_mem;
    ar_hal_vin_get_tuning_req(obj->obj->pipe_fd, obj->obj->pipe_id, &tuning_req_mem);
    req=tuning_req_mem.remote_pa;

    AR_S32 offset_ret=0;
    void *req_addr=ar_hal_vin_map_phy_addr(fd_mm,(void *)req, sizeof(STRU_PIPE_SET_TUNING_REQUEST_T), &offset_ret);
    req_v=(STRU_PIPE_SET_TUNING_REQUEST_T *)(req_addr+offset_ret);
    req_v->req_type = TUNING_REQ_SET_MANUAL_AF;
    req_v->req_pra.set_man_af_pra=*pra;
    req_v->len=sizeof(STRU_TUNING_REQ_SET_MANUAL_AF_PRA_T);
    ar_hal_vin_process_tuning_req(obj->obj->pipe_fd, obj->obj->pipe_id, &tuning_req_mem);
    ar_hal_vin_release_tuning_req(obj->obj->pipe_fd,obj->obj->pipe_id,&tuning_req_mem);
    ar_hal_vin_ummap_vaddr(req_addr,sizeof(STRU_PIPE_SET_TUNING_REQUEST_T));
    ar_hal_vin_close_dev_mem(fd_mm);
    log_func_exit();
    return 0;
}

static int process_set_manual_af_ack(tuning_server_t *obj,STRU_TUNING_REQ_SET_MANUAL_AF_PRA_T *pra,int status)
{
    log_func_enter();
    int ack_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_SET_MANUAL_AF_ACK_T);
    tuning_ack_t *ack=(tuning_ack_t *)malloc(ack_len);
	if(!ack)
    {
      return -1;
    }
    STRU_TUNING_REQ_SET_MANUAL_AF_ACK_T *ack_pra=(STRU_TUNING_REQ_SET_MANUAL_AF_ACK_T *)get_ack_pra(((uint8_t *)ack));
    ack->request_type=TUNING_REQ_SET_MANUAL_AF;
    ack->error_code=status;
    ack->len=sizeof(STRU_TUNING_REQ_SET_MANUAL_AF_ACK_T);
    //fill the ack pra by req return
    ack_pra->request_type=pra->request_type;
    //send to tool
    obj->send(obj->client,(uint8_t *)ack,ack_len,obj->msg_id,0);
    free(ack);
    log_func_exit();
    return 0;
}

static int process_set_af_mod_assist(tuning_server_t *obj,STRU_TUNING_REQ_SET_AF_MOD_PRA_T *pra)
{
    log_func_enter();
    if(obj->server_status!=SERVER_STREAM_RUNING)
    {
        ar_err("please open the prview");
        return TUNING_ERR_UNKNOW;
    }

    int fd_mm=ar_hal_vin_open_dev_mem();
    if(fd_mm<0)
    {
       ar_err("err open dev/mem \n");
       return -1;
    }
    STRU_PIPE_SET_TUNING_REQUEST_T *req_v=NULL;

    STRU_PIPE_SET_TUNING_REQUEST_T *req=NULL;

    STRU_VIN_MEM_T tuning_req_mem;
    ar_hal_vin_get_tuning_req(obj->obj->pipe_fd, obj->obj->pipe_id, &tuning_req_mem);
    req=tuning_req_mem.remote_pa;

    AR_S32 offset_ret=0;
    void *req_addr=ar_hal_vin_map_phy_addr(fd_mm,(void *)req, sizeof(STRU_PIPE_SET_TUNING_REQUEST_T), &offset_ret);
    req_v=(STRU_PIPE_SET_TUNING_REQUEST_T *)(req_addr+offset_ret);
    req_v->req_type = TUNING_REQ_SET_AF_MOD;
    req_v->req_pra.set_af_mod_pra=*pra;
    req_v->len=sizeof(STRU_TUNING_REQ_SET_AF_MOD_PRA_T);
    ar_hal_vin_process_tuning_req(obj->obj->pipe_fd, obj->obj->pipe_id, &tuning_req_mem);
    ar_hal_vin_release_tuning_req(obj->obj->pipe_fd,obj->obj->pipe_id,&tuning_req_mem);
    ar_hal_vin_ummap_vaddr(req_addr,sizeof(STRU_PIPE_SET_TUNING_REQUEST_T));
    ar_hal_vin_close_dev_mem(fd_mm);
    log_func_exit();
    return 0;
}

static int process_set_af_mod_ack(tuning_server_t *obj,STRU_TUNING_REQ_SET_AF_MOD_PRA_T *pra,int status)
{
    log_func_enter();
    int ack_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_SET_AF_MOD_ACK_T);
    tuning_ack_t *ack=(tuning_ack_t *)malloc(ack_len);
	if(!ack)
    {
      return -1;
    }
    STRU_TUNING_REQ_SET_AF_MOD_ACK_T *ack_pra=(STRU_TUNING_REQ_SET_AF_MOD_ACK_T *)get_ack_pra(((uint8_t *)ack));
    ack->request_type=TUNING_REQ_SET_AF_MOD;
    ack->error_code=status;
    ack->len=sizeof(STRU_TUNING_REQ_SET_AF_MOD_ACK_T);
    //fill the ack pra by req return
    ack_pra->request_type=pra->request_type;
    //send to tool
    obj->send(obj->client,(uint8_t *)ack,ack_len,obj->msg_id,0);
    free(ack);
    log_func_exit();
    return 0;
}

static int process_set_trigger_af_assist(tuning_server_t *obj,STRU_TUNING_REQ_TRIGGER_AF_PRA_T *pra)
{
    log_func_enter();
    if(obj->server_status!=SERVER_STREAM_RUNING)
    {
        ar_err("please open the prview");
        return TUNING_ERR_UNKNOW;
    }

    int fd_mm=ar_hal_vin_open_dev_mem();
    if(fd_mm<0)
    {
       ar_err("err open dev/mem \n");
       return -1;
    }
	ar_hal_vin_trigger_focus(obj->obj->pipe_fd,obj->obj->pipe_id);

    log_func_exit();
    return 0;
}

static int process_set_trigger_af_ack(tuning_server_t *obj,STRU_TUNING_REQ_TRIGGER_AF_PRA_T *pra,int status)
{
    log_func_enter();
    int ack_len=sizeof(tuning_ack_t)+sizeof(STRU_TUNING_REQ_TRIGGER_AF_ACK_T);
    tuning_ack_t *ack=(tuning_ack_t *)malloc(ack_len);
	if(!ack)
    {
      return -1;
    }
    STRU_TUNING_REQ_TRIGGER_AF_ACK_T *ack_pra=(STRU_TUNING_REQ_TRIGGER_AF_ACK_T *)get_ack_pra(((uint8_t *)ack));
    ack->request_type=TUNING_REQ_SET_TRIGGER_AF;
    ack->error_code=status;
    ack->len=sizeof(STRU_TUNING_REQ_TRIGGER_AF_ACK_T);
    //fill the ack pra by req return
    ack_pra->request_type=pra->request_type;
    //send to tool
    obj->send(obj->client,(uint8_t *)ack,ack_len,obj->msg_id,0);
    free(ack);
    log_func_exit();
    return 0;
}
#define AR_SYSTEM_HEAP_ID (27)

static int process_tunning_send_raw_frame(tuning_server_t *obj, STU_CAM_SET_RAW_TO_SENSOR_SIM_PRA_T *pra, void *raw_data)
{
    log_func_enter();
    if(obj->server_status != SERVER_STREAM_RUNING)
    {
        ar_err("please open the prview");
        return TUNING_ERR_UNKNOW;
    }

    STU_CAM_SET_RAW_TO_SENSOR_SIM_PRA_T *sim_pra = pra;

    if (sim_pra != NULL && obj != NULL&& sim_pra->len > 0)
    {
        int /*i = 0,*/ row = 0, width = 1920, height = 1080, tmp_width = 0;
        void *align_addr = NULL;
        v4ar_mem_t mem;
        obj->obj->nframes = sim_pra->nframes;
        obj->obj->p_raw = raw_data;
        obj->obj->len = sim_pra->len;
        obj->obj->fps = sim_pra->fps;
        obj->obj->hdr_mod = sim_pra->hdr_mode;
        obj->obj->hdr_frames = sim_pra->hdr_frames;
        obj->obj->with_3a = sim_pra->with_3a;
        if(obj->obj->with_3a)
        {
            ar_always("send raw frame with 3a: %d",  obj->obj->with_3a);
            STRU_AR_RAW_HEADER_T* p_raw_header = (STRU_AR_RAW_HEADER_T*)obj->obj->p_raw;
            obj->mem_raw_3a[obj->obj->frame_id] = p_raw_header->basic_info;
            obj->mem_raw_3a[obj->obj->frame_id].aec_info.aec_mode = 5;
            obj->mem_raw_3a[obj->obj->frame_id].awb_info.awb_mode = 11;
            obj->obj->p_raw += sizeof(STRU_AR_RAW_HEADER_T);
        }
        width = sim_pra->width;
        height = sim_pra->height;
        if(obj->obj->hdr_frames == 0)
        {
            ar_always("get hdr_frames = %d then need to set it with 1\n", obj->obj->hdr_frames);
            obj->obj->hdr_frames = 1;
        }
        ar_always("(all) frame(s) len = %d nframes = %d\n", obj->obj->len, obj->obj->nframes);
        ar_always("w = %d, h = %d, fps = %d\n", width, height, obj->obj->fps);
        ar_always("hdr_mod = %d , hdr_frames = %d\n", obj->obj->hdr_mod, obj->obj->hdr_frames);

        //v4ar_mem_t mem;
        // int ret = 0;
        memset(&mem, 0, sizeof(v4ar_mem_t));
        tmp_width = CAM_ALIGNE_TO(width * 2, BUF_STRIDE_ALIGN);
        mem.len = tmp_width*height + 4096;

        //if mem_raw_buf no buffer then malloc buffer, else use mem_raw_buf old buffer
        if (obj->mem_raw_buf[obj->obj->frame_id].va == NULL)
        {
            AR_VOID *p_vaddr = NULL;
            AR_U64 phy_addr = 0;
            ar_hal_sys_mmz_alloc_cached(&phy_addr, &p_vaddr, NULL, NULL, mem.len);
            if(p_vaddr == NULL || phy_addr == 0)
            {
                ar_err("ar_hal_sys_mmz_alloc_cached err raw_size = %d(0x%x)", mem.len, mem.len);
                return TUNING_ERR_UNKNOW;
            }
            mem.va = p_vaddr;
            mem.pa = (void *)phy_addr;
            ar_always("malloc a dma buffer addr = %p \n", mem.pa);

            //save the mem to mem_raw_buf array, for burst_len = 16 128bits(16bytes) burst stride aligned
            align_addr = (void *)CAM_ALIGNE_TO((uint64_t)mem.va, BUF_ALIGN);

            for (row = 0; row < height; row++)
            {
                memcpy(align_addr + row * tmp_width, \
                       obj->obj->p_raw + row * (width * 2) , width * 2);
            }
            obj->mem_raw_buf[obj->obj->frame_id] = mem;

        }else
        {
            //save the mem to mem_raw_buf array
            mem = obj->mem_raw_buf[obj->obj->frame_id];
            align_addr = (void *)CAM_ALIGNE_TO((uint64_t)mem.va, BUF_ALIGN);
            for (row = 0; row < height; row++)
            {
                memcpy(align_addr + row * tmp_width, \
                       obj->obj->p_raw + row * (width * 2) , width * 2);

            }
        }
        ar_always("get a raw frame id = %d\n", obj->obj->frame_id);
        obj->obj->frame_id++;
        if (obj->obj->frame_id == obj->obj->nframes)
        {
            obj->obj->frame_id = 0;
        }
    }

    return 0;
}

int dbg_handle_cam_tuning(void *client,uint8_t *msg,uint32_t len,int msg_id)
{
    tuning_server_t *obj=get_tuning_server(msg_id);
    tuning_request_t *req=(tuning_request_t *)msg;
    int status=0;
    log_func_enter();

    log_conditon("%d %d %d",req->request_type,req->len,len);
    switch(req->request_type)
    {
        case TUNING_REQ_CONNECT:
        {
            ar_always("TUNING_REQ_CONNECT");
            STRU_TUNING_REQ_CONNECT_PRA_T  *connect_pra=(STRU_TUNING_REQ_CONNECT_PRA_T *)get_req_pra(msg);
            obj->tool_mode=connect_pra->connect_mod=CONNECT_ASSISTENT;
            status=process_connect_assist(obj,connect_pra);
            status=process_connect_ack(obj,connect_pra,status);
        }
        break;
        case TUNING_REQ_GET_CAM_LSIT:
        {
            ar_always("TUNING_REQ_GET_CAM_LSIT");
            STRU_TUNING_REQ_GET_CAM_LSIT_PRA_T *get_list_pra=malloc(sizeof(STRU_TUNING_REQ_GET_CAM_LSIT_PRA_T));
			if(get_list_pra)
			{
    			status=process_get_sensor_list_assist(obj,get_list_pra);
                status=process_get_sensor_list_ack(obj,get_list_pra,status);
			}
        }
        break;
        case TUNING_REQ_START_PREVIEW:
        {
            ar_always("TUNING_REQ_START_PREVIEW");
            STRU_TUNING_REQ_START_PREVIEW_PRA_T *start_preview_pra=(STRU_TUNING_REQ_START_PREVIEW_PRA_T *)get_req_pra(msg);
            status=process_start_preview_assist(obj,start_preview_pra);
            if(status != 0)
            {
                ar_err("process_start_preview failed: %d", status);
                return status;
            }
            process_start_preview_ack(obj,start_preview_pra,status);
        }
        break;
        case  TUNING_REQ_WRITE_TUNING_PRA:
        {
            ar_always("TUNING_REQ_WRITE_TUNING_PRA");
            STRU_TUNING_REQ_WRITE_TUNING_PRA_PRA_T *write_tuning_pra=(STRU_TUNING_REQ_WRITE_TUNING_PRA_PRA_T *)get_req_pra(msg);

            {
                status=process_write_tuning_assist(obj,write_tuning_pra);
                process_write_tuning_pra_ack(obj,write_tuning_pra,status);
            }
        }
        break;
        case  TUNING_REQ_READ_TUNING_PRA:
        {
            ar_always("TUNING_REQ_READ_TUNING_PRA");
            STRU_TUNING_REQ_READ_TUNING_PRA_T*read_tuning_pra=(STRU_TUNING_REQ_READ_TUNING_PRA_T *)get_req_pra(msg);

            {
                status=process_read_tuning_pra(obj,read_tuning_pra);
                process_read_tuning_pra_assist_ack(obj,read_tuning_pra,status);
            }
        }
        break;
        case TUNING_REQ_GET_TUNING_BIN:
        {
            ar_always("TUNING_REQ_GET_TUNING_BIN");
            STRU_TUNING_REQ_GET_TUNING_BIN_PRA_T *get_bin_pra = (STRU_TUNING_REQ_GET_TUNING_BIN_PRA_T*)get_req_pra(msg);
			process_get_tuning_bin_ack_assist(obj, get_bin_pra, 0);

        }
        break;
        case TUNING_REQ_SET_TUNING_BIN:
        {
            ar_always("TUNING_REQ_SET_TUNING_BIN");
            STRU_TUNING_REQ_SET_TUNING_BIN_PRA_T *set_bin_pra = (STRU_TUNING_REQ_SET_TUNING_BIN_PRA_T*)get_req_pra(msg);
            status = process_set_tuning_bin_pra_assist(obj, set_bin_pra);
            process_set_tuning_bin_ack(obj, set_bin_pra, 0);

        }
        break;
		case TUNING_REQ_SAVE_TUNING_PRA_TO_BIN:
		{
		    ar_always("TUNING_REQ_SET_TUNING_BIN");
		    STRU_TUNING_REQ_SAVE_TUNING_PRA_TO_BIN_T *save_bin=(STRU_TUNING_REQ_SAVE_TUNING_PRA_TO_BIN_T*)get_req_pra(msg);
		    status=process_save_tuning_bin_pra_assist(obj,save_bin);
			process_save_tuning_bin_ack(obj,save_bin,status);
			
		}
		break;
        case TUNING_REQ_READ_CUR_ISP_PRA:
        {
            ar_always("TUNING_REQ_READ_CUR_ISP_PRA");
            STRU_TUNING_REQ_READ_CUR_ISP_PRA_T *read_tuning_pra=(STRU_TUNING_REQ_READ_CUR_ISP_PRA_T *)get_req_pra(msg);

            {
                status=process_read_cur_isp_pra(obj,read_tuning_pra);
                process_read_cur_isp_pra_ack_assist(obj,read_tuning_pra,status);
            }
        }
        break;
        case TUNING_REQ_ENDEN_ISP_MODULE:
        {
            ar_always("TUNING_REQ_ENDEN_ISP_MODULE");
            STRU_TUNING_REQ_ENDEN_ISP_MODULE_PRA_T *en_dis_ispmod=(STRU_TUNING_REQ_ENDEN_ISP_MODULE_PRA_T *)get_req_pra(msg);

            {
                status=process_endis_isp_module_assist(obj,en_dis_ispmod);
                process_endis_isp_module_ack(obj,en_dis_ispmod,status);
            }
        }
        break;
        case TUNING_REQ_SENSOR_WRITE:
        {
            ar_always("TUNING_REQ_SENSOR_WRITE");
            STRU_TUNING_REQ_SENSOR_WRITE_PRA_T *write_sensor=(STRU_TUNING_REQ_SENSOR_WRITE_PRA_T*)get_req_pra(msg);
            {
                status=process_write_sensor_assist(obj,write_sensor);
                process_write_sensor_ack(obj,write_sensor,status);
            }
        }
        break;
        case TUNING_REQ_SENSOR_READ:
        {
            ar_always("TUNING_REQ_SENSOR_READ");
            STRU_TUNING_REQ_SENSOR_READ_PRA_T *read_sensor=(STRU_TUNING_REQ_SENSOR_READ_PRA_T *)get_req_pra(msg);

            {
                status=process_read_sensor(obj,read_sensor);
                process_read_sensor_ack_asssit(obj,read_sensor,status);
            }
        }
        break;
        case TUNING_REQ_LOCK_TUNING_PRA:
        {
            ar_always("TUNING_REQ_LOCK_TUNING_PRA");
            STRU_TUNING_REQ_LOCK_TUNING_PRA_PRA_T *lock_tuning_pra=(STRU_TUNING_REQ_LOCK_TUNING_PRA_PRA_T *)get_req_pra(msg);

            {
                status=process_lock_tuning_assist(obj,lock_tuning_pra);
                process_lock_tuning_ack(obj,lock_tuning_pra,status);
            }
        }
         break;
        case  TUNING_REQ_UNLOCK_TUNING_PRA:
        {
            ar_always("TUNING_REQ_UNLOCK_TUNING_PRA");
            STRU_TUNING_REQ_UNLOCK_TUNING_PRA_PRA_T *unlock_tuning_pra=(STRU_TUNING_REQ_UNLOCK_TUNING_PRA_PRA_T *)get_req_pra(msg);

            {
                status=process_unlock_tuning_assist(obj,unlock_tuning_pra);
                process_unlock_tuning_ack(obj,unlock_tuning_pra,status);
            }
        }
        break;
        case TUNING_REQ_PREVIEW_FRAME:
        {
            ar_always("TUNING_REQ_PREVIEW_FRAME");
            STRU_TUNING_REQ_PREVIEW_FRAME_PRA_T *req_preview_frame=(STRU_TUNING_REQ_PREVIEW_FRAME_PRA_T *)get_req_pra(msg);

            status=process_req_preview_frame(obj,req_preview_frame);
            process_req_preview_frame_ack(obj,req_preview_frame,status);
        }
        break;

        case TUNING_REQ_AF_FRAME:
        {      
     		ar_always("TUNING_REQ_AF_FRAME");
     		STRU_TUNING_REQ_AF_FRAME_PRA_T *req_af_frame=(STRU_TUNING_REQ_AF_FRAME_PRA_T *)get_req_pra(msg);

     		status=process_req_af_frame(obj,req_af_frame);
     		process_req_af_frame_ack(obj,req_af_frame,status);
        }		
        case TUNING_REQ_VIDEO_BITSTREAM:
        {
            ar_always("TUNING_REQ_VIDEO_BITSTREAM");
            //STRU_TUNING_REQ_VIDEO_BITSTREAM_PRA_T *req_preview_frame=(STRU_TUNING_REQ_VIDEO_BITSTREAM_PRA_T *)get_req_pra(msg);

            if(obj->tool_mode==CONNECT_MASTER){

            }else
            {

            }
        }
        break;
        case TUNING_REQ_SNAPSHOT_JPG_FRAME:
        {
            ar_always("TUNING_REQ_SNAPSHOT_JPG_FRAME");
            //STRU_TUNING_REQ_SNAPSHOT_JPG_FRAME_PRA_T *req_preview_frame=(STRU_TUNING_REQ_SNAPSHOT_JPG_FRAME_PRA_T *)get_req_pra(msg);
            if(obj->tool_mode==CONNECT_MASTER){

            }
            else
            {
                ar_always("assist mode, can not TUNING_REQ_SNAPSHOT_JPG_FRAME");
                status=TUNING_ERR_SUCCESS;
            }
        }
        break;
        case TUNING_REQ_SNAPSHOT_YUV_FRAME:
        {
            ar_always("TUNING_REQ_SNAPSHOT_YUV_FRAME");
            STRU_TUNING_REQ_SNAPSHOT_YUV_FRAME_PRA_T *req_snap_yuv_frame=(STRU_TUNING_REQ_SNAPSHOT_YUV_FRAME_PRA_T *)get_req_pra(msg);

            status=process_req_snap_yuv_frame(obj,req_snap_yuv_frame);
            process_req_snap_yuv_frame_ack(obj,req_snap_yuv_frame,status);
        }
        break;
        case TUNING_TRIGGER_FETCH_RAW:
        {
            ar_always("TUNING_TRIGGER_FETCH_RAW");
            STRU_TUNING_TRIGGER_FETCH_RAW_PRA_T *trigger_pra = (STRU_TUNING_TRIGGER_FETCH_RAW_PRA_T *)get_req_pra(msg);
            get_trigger_fetch_info(obj, trigger_pra);
            status = TUNING_ERR_SUCCESS;
        }
        break;
        case TUNING_SEND_RAW_FRAME:
        {
            ar_always("TUNING_SEND_RAW_FRAME");
            STU_CAM_SET_RAW_TO_SENSOR_SIM_PRA_T *sim_pra = (STU_CAM_SET_RAW_TO_SENSOR_SIM_PRA_T *)get_req_pra(msg);
            ar_always("sim_pra.len = %d", sim_pra->len);
            {
                ar_always("assist mode, TUNING_SEND_RAW_FRAME");
                if(0 != sim_pra->len)
                {
                    void *raw_data = (void *)(msg + sizeof(tuning_request_t) + sizeof(STU_CAM_SET_RAW_TO_SENSOR_SIM_PRA_T));
                    if(obj->obj->pipe_fd > 0)
                    {
                        status = process_tunning_send_raw_frame(obj, sim_pra, raw_data);
                    }
                }

                status = TUNING_ERR_SUCCESS;
            }
        }
        break;
        case TUNING_REQ_RAW_FRAME:
        {
            ar_always("TUNING_REQ_RAW_FRAME");
            STRU_TUNING_REQ_RAW_FRAME_PRA_T *req_raw_frame=(STRU_TUNING_REQ_RAW_FRAME_PRA_T*)get_req_pra(msg);
            status=process_req_raw_frame(obj,req_raw_frame);
            process_req_raw_frame_ack(obj,req_raw_frame,status);
        }
        break;
        case TUNING_REQ_HDR_RAW_FRAME:
        {
            ar_always("TUNING_REQ_RAW_FRAME");
            STRU_TUNING_REQ_RAW_FRAME_PRA_T *req_raw_frame=(STRU_TUNING_REQ_RAW_FRAME_PRA_T*)get_req_pra(msg);
            ar_always("assist mode, can not TUNING_REQ_RAW_FRAME");
            status=process_req_hdr_raw_frame(obj,req_raw_frame);
            process_req_hdr_raw_frame_ack(obj,req_raw_frame,status);
            status=TUNING_ERR_SUCCESS;

        }
        break;
        case TUNING_REQ_GET_3A_INFO:
        {
            ar_always("TUNING_REQ_GET_3A_INFO");
            STRU_TUNING_REQ_GET_3A_INFO_PRA_T *get_3a_pra=(STRU_TUNING_REQ_GET_3A_INFO_PRA_T *)get_req_pra(msg);

            status=process_get_3a_info(obj,get_3a_pra);
            process_get_3a_info_ack_assist(obj,get_3a_pra,status);

        }
        break;
        case TUNING_REQ_LOCK_AE:
        {
            ar_always("TUNING_REQ_LOCK_AE");
            STRU_TUNING_REQ_LOCK_AE_PRA_T *lock_ae_pra=(STRU_TUNING_REQ_LOCK_AE_PRA_T *)get_req_pra(msg);
            status=process_lock_ae_assist(obj,lock_ae_pra);
            process_lock_ae_ack(obj,lock_ae_pra,status);

        }
        break;
        case TUNING_REQ_UNLOCK_AE:
        {
            ar_always("TUNING_REQ_UNLOCK_AE");
            STRU_TUNING_REQ_UNLOCK_AE_PRA_T *unlock_ae_pra=(STRU_TUNING_REQ_UNLOCK_AE_PRA_T *)get_req_pra(msg);

            {
                status=process_unlock_ae_assist(obj,unlock_ae_pra);
                process_unlock_ae_ack(obj,unlock_ae_pra,status);
            }
        }
        break;
        case TUNING_REQ_SET_MANUAL_AE:
        {
            ar_always("TUNING_REQ_SET_MANUAL_AE");
            STRU_TUNING_REQ_SET_MANUAL_AE_PRA_T *set_man_ae_pra=(STRU_TUNING_REQ_SET_MANUAL_AE_PRA_T *)get_req_pra(msg);

            {
                status=process_set_man_ae_assist(obj,set_man_ae_pra);
                process_set_man_ae_ack(obj,set_man_ae_pra,status);
            }
        }
        break;
        case TUNING_REQ_LOCK_AWB:
        {
            ar_always("TUNING_REQ_LOCK_AWB");
            STRU_TUNING_REQ_LOCK_AWB_PRA_T *lock_awb_pra=(STRU_TUNING_REQ_LOCK_AWB_PRA_T *)get_req_pra(msg);

            {
               status=process_lock_awb_assist(obj,lock_awb_pra);
               process_lock_awb_ack(obj,lock_awb_pra,status);
            }
        }
        break;
        case TUNING_REQ_UNLOCK_AWB:
        {
            ar_always("TUNING_REQ_UNLOCK_AWB");
            STRU_TUNING_REQ_UNLOCK_AWB_PRA_T *unlock_awb_pra=(STRU_TUNING_REQ_UNLOCK_AWB_PRA_T *)get_req_pra(msg);

            {
               status=process_unlock_awb_assist(obj,unlock_awb_pra);
               process_unlock_awb_ack(obj,unlock_awb_pra,status);
            }
        }
        break;
        case TUNING_REQ_SET_MANUAL_AWB:
        {
            ar_always("TUNING_REQ_SET_MANUAL_AWB");
            STRU_TUNING_REQ_SET_MANUAL_AWB_PRA_T *set_man_awb_pra=(STRU_TUNING_REQ_SET_MANUAL_AWB_PRA_T *)get_req_pra(msg);

            {
                status=process_set_man_awb_assist(obj,set_man_awb_pra);
                process_set_man_awb_ack(obj,set_man_awb_pra,status);
            }
        }
        break;
        case TUNING_REQ_STOP_PREVIEW:
        {
            ar_always("TUNING_REQ_STOP_PREVIEW");
            STRU_TUNING_REQ_STOP_PREVIEW_PRA_T *req_stop_preview=(STRU_TUNING_REQ_STOP_PREVIEW_PRA_T*)get_req_pra(msg);
            status=process_req_stop_preview_assist(obj,req_stop_preview);
            process_req_stop_preview_ack(obj,req_stop_preview,status);

            ar_always("assist mode not run TUNING_REQ_STOP_PREVIEW.switch to SERVER_CONNECTED");
            obj->server_status=SERVER_CONNECTED;
            return 0;

        }
        break;
        case TUNING_REQ_DIS_CONNECT:
            ar_always("TUNING_REQ_DIS_CONNECT");
            STRU_TUNING_REQ_DIS_CONNECT_PRA_T *req_dis_connect=(STRU_TUNING_REQ_DIS_CONNECT_PRA_T*)get_req_pra(msg);
            if(req_dis_connect->mode!=obj->tool_mode){
                ar_err("dis connected err, tool mode not req mode");
                return TUNING_ERR_UNKNOW;
            }

            {
                process_req_dis_connect_assit(obj,req_dis_connect);
                process_req_dis_connect_ack(obj,req_dis_connect,status);
            }
            break;
        case TUNING_REQ_LOCK_AF :
        {
            ar_always("TUNING_REQ_LOCK_AF");
            STRU_TUNING_REQ_LOCK_AF_PRA_T *set_lock_af_pra=(STRU_TUNING_REQ_LOCK_AF_PRA_T *)get_req_pra(msg);

            {
                status=process_set_af_lock_assist(obj,set_lock_af_pra);
                process_set_af_lock_ack(obj,set_lock_af_pra,status);
            }
        }
        break;
        case TUNING_REQ_UNLOCK_AF :
        {
            ar_always("TUNING_REQ_UNLOCK_AF");
            STRU_TUNING_REQ_UNLOCK_AF_PRA_T *set_unlock_af_pra=(STRU_TUNING_REQ_UNLOCK_AF_PRA_T *)get_req_pra(msg);

            {
                status=process_set_af_unlock_assist(obj,set_unlock_af_pra);
                process_set_af_unlock_ack(obj,set_unlock_af_pra,status);
            }
        }
        break;
        case TUNING_REQ_SET_MANUAL_AF :
        {
            ar_always("TUNING_REQ_SET_MANUAL_AF");
            STRU_TUNING_REQ_SET_MANUAL_AF_PRA_T *set_manual_af_pra=(STRU_TUNING_REQ_SET_MANUAL_AF_PRA_T *)get_req_pra(msg);

            {
                status=process_set_manual_af_assist(obj,set_manual_af_pra);
                process_set_manual_af_ack(obj,set_manual_af_pra,status);
            }
        }
        break;
        case TUNING_REQ_SET_AF_MOD :
        {
            ar_always("TUNING_REQ_SET_MANUAL_AF");
            STRU_TUNING_REQ_SET_AF_MOD_PRA_T *set_af_mod_pra=(STRU_TUNING_REQ_SET_AF_MOD_PRA_T *)get_req_pra(msg);

            {
                status=process_set_af_mod_assist(obj,set_af_mod_pra);
                process_set_af_mod_ack(obj,set_af_mod_pra,status);
            }
        }
        break;
        case TUNING_REQ_SET_TRIGGER_AF :
        {
            ar_always("TUNING_REQ_SET_AF_TRIGGER");
            STRU_TUNING_REQ_TRIGGER_AF_PRA_T *set_trigger_af_pra=(STRU_TUNING_REQ_TRIGGER_AF_PRA_T *)get_req_pra(msg);

            {
                status=process_set_trigger_af_assist(obj,set_trigger_af_pra);
                process_set_trigger_af_ack(obj,set_trigger_af_pra,status);
            }
        }
        break;
        default:
            ar_always("not suppported ctl");
            break;
    }
    log_func_exit();
    return 0;
}
int register_cam_tuning_send_ops(pfn_send send, void * client,uint8_t msg_id)
{
    log_func_enter();
    tuning_server_t *obj=get_tuning_server(msg_id);
    pthread_mutex_lock(&obj->mutex_stop);
    if(!obj->send_server)
    {
        obj->send_server=send;
        obj->client=client;
        obj->msg_id=msg_id;
    }
    else
    {
        ar_err("the send function have registered");
    }
    pthread_mutex_unlock(&obj->mutex_stop);
    log_func_exit();
    return 0;
}

int unregister_cam_tuning_client(void * client,int msg_id)
{
    log_func_enter();
    tuning_server_t *obj=get_tuning_server(msg_id);
    pthread_mutex_lock(&obj->mutex_stop);
    if(obj->send_server)
    {
        obj->send_server=NULL;
        obj->client=NULL;
        obj->msg_id=-1;
    }else
    {
        ar_err("the send function have registered");
    }
    pthread_mutex_unlock(&obj->mutex_stop);
    log_func_exit();
    return 0;
}

