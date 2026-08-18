#include "camera.h"
#include "hal_sys.h"

#define USE_HW_DMA_CPY
#define DUMP_ALIGNE_TO(size,num) (void *)(((unsigned long)(size)+((num)-1))&~((num)-1))
typedef struct
{
  void *buffer;
  void *buffer_orign;
  int   len;
  void *buffer_phy;
  void *buffer_phy_orign;
}dump_pannel_t;

typedef struct
{
    int panel_count;
	dump_pannel_t pannel[6];
}dump_buffer_t;

static int dump_to_remote_file(STRU_DUMP_SERVER_T * server_l,dump_buffer_t *stream_buffer)
{
	int i=0;
	int len=0;
    for(i=0;i<stream_buffer->panel_count;i++)
	{
			 len=stream_buffer->pannel[i].len;
     	     write(server_l->remote_fd,stream_buffer->pannel[i].buffer,len);

    }
	return 0;
}

static void *dum_file_task(void* arg)
{
	dump_buffer_t *buffer=NULL;
	STRU_DUMP_SERVER_T * server_l=(STRU_DUMP_SERVER_T *)arg;
	STRU_AR_QUEUE_T *dump_queue=(STRU_AR_QUEUE_T *)server_l->dump_queue;
	int j=0;
	ar_lock(server_l->dump_lock);	
    while(server_l->current_index>0){

         dump_queue->queue_pop(dump_queue,(void **)&buffer);
		 if(buffer)
		 {
			dump_to_remote_file(server_l,buffer);
			for(j=0;j<buffer->panel_count;j++){
                ar_hal_sys_mmz_free((AR_U64)buffer->pannel[j].buffer_phy_orign,buffer->pannel[j].buffer_orign);
			}
		    ar_free(buffer);
			ar_printf("current_index=%d \n",server_l->current_index);
			server_l->current_index--;
		 }else
		 {
		    ar_signal_wait_binary(server_l->dump_queue_signal);
		 }
    }
	close(server_l->remote_fd);
	server_l->remote_fd=-1;
	server_l->current_index=0;
	ar_unlock(server_l->dump_lock);
	ar_printf("dump completed \n");
	ar_osal_thread_exit();
	return NULL;
 }


static int dma_cpy(int fd,void *dst,void *src,int size)
{
#ifdef USE_HW_DMA_CPY
	ar_hal_sys_memcpy_pa((AR_U64)dst,(AR_U64)src,size);
#else
	memcpy(dst,src,size);
#endif
    return 0;
}


static int dma_cpy_segment(int fd,void *dst,void *src,int size)
{
	dma_cpy(fd,dst,src,size);
    return 0;
}
static void dump_segment(STRU_DUMP_SERVER_T *server_l,void *y,void *u,void *v,void *meta,int y_len,int u_len,int v_len,int meta_len)
{

	ar_lock(server_l->req_lock);
	if(server_l->req_count>0)
	{    ar_always("server_l->req_count  %d", server_l->req_count);
		dump_buffer_t *buffer=ar_malloc(sizeof(dump_buffer_t));
		if(buffer){
		   memset(buffer,0,sizeof(dump_buffer_t));
		   if(y&&y_len){


			   //malloc later to instead with vb policy
			   AR_VOID *p_vaddr=NULL;
			   AR_U64 phy_addr=0;
			   ar_hal_sys_mmz_alloc_cached(&phy_addr,&p_vaddr,NULL,NULL,y_len+512);

			   if(p_vaddr==NULL || phy_addr==0)
			   {
				 ar_err("ar_hal_sys_mmz_alloc_cached err");
				 return;
			   }


			   buffer->pannel[buffer->panel_count].buffer_orign=p_vaddr;
			   buffer->pannel[buffer->panel_count].buffer_phy_orign = (void *)phy_addr;

			   if(buffer->pannel[buffer->panel_count].buffer_orign)
			   {
				   buffer->pannel[buffer->panel_count].buffer=DUMP_ALIGNE_TO(buffer->pannel[buffer->panel_count].buffer_orign,512);
				   buffer->pannel[buffer->panel_count].buffer_phy = DUMP_ALIGNE_TO(buffer->pannel[buffer->panel_count].buffer_phy_orign, 512);
				   buffer->pannel[buffer->panel_count].len=y_len;

				   dma_cpy_segment(server_l->dma_fd,buffer->pannel[ buffer->panel_count].buffer_phy,y,y_len);
				   buffer->panel_count++;
			   }
		   }
		   if(u&&u_len){


			   //malloc later to instead with vb policy
			   AR_VOID *p_vaddr=NULL;
			   AR_U64 phy_addr=0;
			   ar_hal_sys_mmz_alloc_cached(&phy_addr,&p_vaddr,NULL,NULL,u_len+512);

			   if(p_vaddr==NULL || phy_addr==0)
			   {
				 ar_err("ar_hal_sys_mmz_alloc_cached err");
				 return;
			   }


			   buffer->pannel[buffer->panel_count].buffer_orign=p_vaddr;
			   buffer->pannel[buffer->panel_count].buffer_phy_orign =  (void *)phy_addr;
			   if(buffer->pannel[buffer->panel_count].buffer_orign)
			   {
				   buffer->pannel[buffer->panel_count].buffer=DUMP_ALIGNE_TO(buffer->pannel[buffer->panel_count].buffer_orign,512);
				   buffer->pannel[buffer->panel_count].buffer_phy = DUMP_ALIGNE_TO(buffer->pannel[buffer->panel_count].buffer_phy_orign, 512);
				   dma_cpy_segment(server_l->dma_fd,buffer->pannel[buffer->panel_count].buffer_phy,u,u_len);
				   buffer->pannel[buffer->panel_count].len=u_len;
				   buffer->panel_count++;
			   }
		   }
		   if(v&&v_len){

			   //malloc later to instead with vb policy
			   AR_VOID *p_vaddr=NULL;
			   AR_U64 phy_addr=0;
			   ar_hal_sys_mmz_alloc_cached(&phy_addr,&p_vaddr,NULL,NULL,v_len+512);

			   if(p_vaddr==NULL || phy_addr==0)
			   {
				 ar_err("ar_hal_sys_mmz_alloc_cached err");
				 return;
			   }


			   buffer->pannel[buffer->panel_count].buffer_orign=p_vaddr;
			   buffer->pannel[buffer->panel_count].buffer_phy_orign =  (void *)phy_addr;
			   if(buffer->pannel[buffer->panel_count].buffer_orign)
			   {
				   buffer->pannel[buffer->panel_count].buffer=DUMP_ALIGNE_TO(buffer->pannel[buffer->panel_count].buffer_orign,512);
				   buffer->pannel[buffer->panel_count].buffer_phy = DUMP_ALIGNE_TO(buffer->pannel[buffer->panel_count].buffer_phy_orign, 512);
				   dma_cpy_segment(server_l->dma_fd,buffer->pannel[buffer->panel_count].buffer_phy,v,v_len);
				   buffer->pannel[buffer->panel_count].len=v_len;
				   buffer->panel_count++;
			   }
		   }
		   if(meta&&meta_len){

			   //malloc later to instead with vb policy
			   AR_VOID *p_vaddr=NULL;
			   AR_U64 phy_addr=0;
			   ar_hal_sys_mmz_alloc_cached(&phy_addr,&p_vaddr,NULL,NULL,meta_len+512);

			   if(p_vaddr==NULL || phy_addr==0)
			   {
				 ar_err("ar_hal_sys_mmz_alloc_cached err");
				 return;
			   }

			   buffer->pannel[buffer->panel_count].buffer_orign=p_vaddr;
			   buffer->pannel[buffer->panel_count].buffer_phy_orign =  (void *)phy_addr;
			   if(buffer->pannel[buffer->panel_count].buffer_orign)
			   {
				   buffer->pannel[buffer->panel_count].buffer=DUMP_ALIGNE_TO(buffer->pannel[buffer->panel_count].buffer_orign,512);
				   buffer->pannel[buffer->panel_count].buffer_phy = DUMP_ALIGNE_TO(buffer->pannel[buffer->panel_count].buffer_phy_orign, 512);
				   dma_cpy_segment(server_l->dma_fd,buffer->pannel[buffer->panel_count].buffer_phy,meta,meta_len);
				   buffer->pannel[buffer->panel_count].len=meta_len;
				   buffer->panel_count++;
			   }
		   }
		   if(buffer->panel_count>0)
		   {
			   STRU_AR_QUEUE_T *dump_queue=(STRU_AR_QUEUE_T *)server_l->dump_queue;
			   dump_queue->queue_insert(dump_queue,(void *)buffer);
			   ar_signal_binary(server_l->dump_queue_signal);
		   }
		}
		server_l->req_count--;
	}
	ar_unlock(server_l->req_lock);
}


static void dump(STRU_DUMP_SERVER_T *server_l,void *y,void *u,void *v,void *meta,int y_len,int u_len,int v_len,int meta_len)
{
	ar_lock(server_l->req_lock);
    if(server_l->req_count>0)
    {
        dump_buffer_t *buffer=ar_malloc(sizeof(dump_buffer_t));
		if(buffer){
		   memset(buffer,0,sizeof(dump_buffer_t));
		   if(y&&y_len){

			   //malloc later to instead with vb policy
			   AR_VOID *p_vaddr=NULL;
			   AR_U64 phy_addr=0;
			   ar_hal_sys_mmz_alloc_cached(&phy_addr,&p_vaddr,NULL,NULL,y_len+512);

			   if(p_vaddr==NULL || phy_addr==0)
			   {
				 ar_err("ar_hal_sys_mmz_alloc_cached err");
				 return ;
			   }

			   buffer->pannel[buffer->panel_count].buffer_orign=p_vaddr;
			   buffer->pannel[buffer->panel_count].buffer_phy_orign =  (void *)phy_addr;
               if(buffer->pannel[buffer->panel_count].buffer_orign)
               {
				   buffer->pannel[buffer->panel_count].buffer=DUMP_ALIGNE_TO(buffer->pannel[buffer->panel_count].buffer_orign,512);
				   buffer->pannel[buffer->panel_count].buffer_phy = DUMP_ALIGNE_TO(buffer->pannel[buffer->panel_count].buffer_phy_orign,512);
				   buffer->pannel[buffer->panel_count].len=y_len;
				   dma_cpy(server_l->dma_fd,buffer->pannel[ buffer->panel_count].buffer_phy,y,y_len);
		           buffer->panel_count++;
               }
		   }
		   if(u&&u_len){

			   //malloc later to instead with vb policy
			   AR_VOID *p_vaddr=NULL;
			   AR_U64 phy_addr=0;
			   ar_hal_sys_mmz_alloc_cached(&phy_addr,&p_vaddr,NULL,NULL,u_len+512);

			   if(p_vaddr==NULL || phy_addr==0)
			   {
				 ar_err("ar_hal_sys_mmz_alloc_cached err");
				 return;
			   }


			   buffer->pannel[buffer->panel_count].buffer_orign=p_vaddr;
			   buffer->pannel[buffer->panel_count].buffer_phy_orign = (void *)phy_addr;
               if(buffer->pannel[buffer->panel_count].buffer_orign)
               {
				   buffer->pannel[buffer->panel_count].buffer=DUMP_ALIGNE_TO(buffer->pannel[buffer->panel_count].buffer_orign,512);
				   buffer->pannel[buffer->panel_count].buffer_phy = DUMP_ALIGNE_TO(buffer->pannel[buffer->panel_count].buffer_phy_orign,512);
				   dma_cpy(server_l->dma_fd,buffer->pannel[buffer->panel_count].buffer_phy,u,u_len);
				   buffer->pannel[buffer->panel_count].len=u_len;
		           buffer->panel_count++;
               }
		   }
		   if(v&&v_len){

			   //malloc later to instead with vb policy
			   AR_VOID *p_vaddr=NULL;
			   AR_U64 phy_addr=0;
			   ar_hal_sys_mmz_alloc_cached(&phy_addr,&p_vaddr,NULL,NULL,v_len+512);

			   if(p_vaddr==NULL || phy_addr==0)
			   {
				 ar_err("ar_hal_sys_mmz_alloc_cached err");
				 return;
			   }

			   buffer->pannel[buffer->panel_count].buffer_orign=p_vaddr;
			   buffer->pannel[buffer->panel_count].buffer_phy_orign = (void *)phy_addr;
               if(buffer->pannel[buffer->panel_count].buffer_orign)
               {
				   buffer->pannel[buffer->panel_count].buffer=DUMP_ALIGNE_TO(buffer->pannel[buffer->panel_count].buffer_orign,512);
				   buffer->pannel[buffer->panel_count].buffer_phy = DUMP_ALIGNE_TO(buffer->pannel[buffer->panel_count].buffer_phy_orign,512);
				   dma_cpy(server_l->dma_fd,buffer->pannel[buffer->panel_count].buffer_phy,v,v_len);
				   buffer->pannel[buffer->panel_count].len=v_len;
		           buffer->panel_count++;
               }
		   }
		   if(meta&&meta_len){

						   //malloc later to instead with vb policy
			   AR_VOID *p_vaddr=NULL;
			   AR_U64 phy_addr=0;
			   ar_hal_sys_mmz_alloc_cached(&phy_addr,&p_vaddr,NULL,NULL,v_len+512);

			   if(p_vaddr==NULL || phy_addr==0)
			   {
				 ar_err("ar_hal_sys_mmz_alloc_cached err");
				 return;
			   }

			   buffer->pannel[buffer->panel_count].buffer_orign=p_vaddr;
			   buffer->pannel[buffer->panel_count].buffer_phy_orign =  (void *)phy_addr;
               if(buffer->pannel[buffer->panel_count].buffer_orign)
               {
				   buffer->pannel[buffer->panel_count].buffer=DUMP_ALIGNE_TO(buffer->pannel[buffer->panel_count].buffer_orign,512);
				   buffer->pannel[buffer->panel_count].buffer_phy = DUMP_ALIGNE_TO(buffer->pannel[buffer->panel_count].buffer_phy_orign,512);
				   dma_cpy(server_l->dma_fd,buffer->pannel[buffer->panel_count].buffer_phy,meta,meta_len);
				   buffer->pannel[buffer->panel_count].len=meta_len;
		           buffer->panel_count++;
               }
		   }
           if(buffer->panel_count>0)
           {
		       STRU_AR_QUEUE_T *dump_queue=(STRU_AR_QUEUE_T *)server_l->dump_queue;
		       dump_queue->queue_insert(dump_queue,(void *)buffer);
			   ar_signal_binary(server_l->dump_queue_signal);
           }
		}
		server_l->req_count--;
    }
	ar_unlock(server_l->req_lock);
}
static void dump_req(STRU_DUMP_SERVER_T *server_l,char *name,int num)
{
    char name_file[128];
	sprintf(name_file,"%s",name);
	ar_lock(server_l->dump_lock);
    server_l->current_index=num;
	server_l->req_count=num;
    server_l->remote_fd=open(name_file,O_CREAT|O_TRUNC|O_RDWR);
	ar_os_thread_attr_t ctrl_attr = {0};
	ctrl_attr.name = "dump_server";
	ctrl_attr.stack_size = 16 * 1024;	
	ar_unlock(server_l->dump_lock);
	server_l->dump_task=ar_osal_thread_new(dum_file_task, (void*)server_l, &ctrl_attr);
	ar_always("dump_task=%p remote_fd=%d",server_l->dump_task,server_l->remote_fd);	
	ar_unlock(server_l->dump_lock);
}
STRU_DUMP_SERVER_T *creat_dump_server()
{
   STRU_DUMP_SERVER_T *server_l=ar_malloc(sizeof(STRU_DUMP_SERVER_T));
   if(server_l)
   {
      memset(server_l,0,sizeof(STRU_DUMP_SERVER_T));
	  server_l->current_index=0;
	  server_l->dump_lock=ar_creat_lock();
	  server_l->req_lock=ar_creat_lock();
	  server_l->dump_queue=ar_creat_queue(8*1024,"dum_queue");
	  server_l->remote_fd=-1;
	  server_l->dump_queue_signal=ar_create_binary_semaphore();
	  server_l->dump=dump;
	  server_l->dump_segment=dump_segment;
	  server_l->dump_req=dump_req;
	  server_l->dma_fd=0;
   }else
   {
      ar_err("low mem");
   }
   return server_l;
}
void delete_dump_server(STRU_DUMP_SERVER_T *server_l)
{
   if(server_l)
   {
       ar_always("wait write file task exit");
	   if(server_l->dump_task){
           ar_osal_thread_join(server_l->dump_task);
	   }
       ar_delete_lock(server_l->dump_lock);
	   ar_delete_lock(server_l->req_lock);
	   ar_delete_queue((STRU_AR_QUEUE_T **)&server_l->dump_queue);
	   ar_delete_signal_binary(server_l->dump_queue_signal);
	   ar_free(server_l);
	   ar_always("exit");
   }
}

