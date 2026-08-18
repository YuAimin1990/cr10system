#if 0
//#ifndef AR_FEAT_MPP_SERVICE_LIB
#include"sample_vio_big_pic.h"
static int queue_pop(STRU_AR_QUEUE_T * queue, void **item)
{
    ar_lock(queue->lock);
    if(!queue||!item)
    {
      ar_unlock(queue->lock);
      return -1;
    }

    if(queue->valid_data_num==0)
    {
      //set to NULL init
      *item=NULL;
      ar_unlock(queue->lock);
      return -1;
    }
    *item=queue->data[queue->header];
    queue->header=(queue->header+1)%queue->queue_size;
    queue->valid_data_num--;
    ar_unlock(queue->lock);
    return 0;
}

static int get_queue_size(STRU_AR_QUEUE_T * queue)
{
    int num=0;
    ar_lock(queue->lock);
    if(!queue)
    {
      ar_unlock(queue->lock);
      return -1;
    }
    num=queue->valid_data_num;
    ar_unlock(queue->lock);
    return num;
}

static int look_up_head_item(STRU_AR_QUEUE_T * queue, void **item)
{
    int num=0;
    ar_lock(queue->lock);
    if(!queue)
    {
      ar_unlock(queue->lock);
      return -1;
    }

    if(queue->valid_data_num==0)
    {
      //set to NULL init
      *item=NULL;
      ar_unlock(queue->lock);
      return -1;
    }
    *item=queue->data[queue->header];
    ar_unlock(queue->lock);
    return num;
}

static int queue_insert(STRU_AR_QUEUE_T *queue,void *item)
{
   ar_lock(queue->lock);
   if(!queue||!item)
   {
      ar_unlock(queue->lock);
      return -1;
   }
   if(queue->valid_data_num==queue->queue_size)
   {
      ar_unlock(queue->lock);
      ar_err("queue %s size=%d is full",queue->name,queue->queue_size);
      return -1;
   }
   queue->data[queue->tail]=item;
   queue->tail=(queue->tail+1)%queue->queue_size;
   queue->valid_data_num++;
   ar_unlock(queue->lock);
   return 0;
}

STRU_AR_QUEUE_T *ar_creat_queue(uint32_t queue_size,char *name)
{
  STRU_AR_QUEUE_T *queue=NULL;
  queue=(STRU_AR_QUEUE_T*)ar_malloc(sizeof(STRU_AR_QUEUE_T));
  if(queue&&queue_size>=2&&name)
  {
     void **data=NULL;
     data=(void **)ar_malloc(sizeof(void *)*queue_size);
     if(data)
     {
       queue->name=name;
       queue->data=data;
       queue->queue_size=queue_size;
       queue->header=0;
       queue->tail=queue->header;
       queue->valid_data_num=0;
       queue->queue_insert=queue_insert;
       queue->queue_pop=queue_pop;
       queue->get_queue_size=get_queue_size;
       queue->look_up_head=look_up_head_item;
       queue->lock=ar_creat_lock();
       return queue;
     }else
     {
       if(queue)
           ar_free(queue);
     }
  }else
  {
    if(queue)
        ar_free(queue);
  }
  ar_err("creat queue failed");
  return NULL;
}

int ar_delete_queue(STRU_AR_QUEUE_T **queue)
{
   STRU_AR_QUEUE_T *p_queue=*queue;

   if(p_queue)
    {
       if(p_queue->data)
       {
         ar_delete_lock(p_queue->lock);
         ar_free(p_queue->data);
         p_queue->data=NULL;
       }
      ar_free(p_queue);
      *queue=NULL;
    }
   return 0;
}
#endif
