#include "utils_debug.h"
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//#include "hal_vin_log.h"
#include "sys/stat.h"
#include "sys/types.h"
#include <dirent.h>
#include <unistd.h>
#include "osal.h"

#define MAX_THERAD_NUM 1024
#define BUFFER_SIZE (4096)

typedef struct 
{
   int tid;
   char name[128];
   uint64_t run_time_start;
   uint64_t run_time_end;
   int sch_voluntary;
   int sch_nonvoluntary;   
   int sch_voluntary_end;
   int sch_nonvoluntary_end;
   uint64_t us_read_start;
   uint64_t us_read_end;
   int run_time_us;
}ar_thread_info_t;

typedef struct
{
 int args_num;
 char args[128][128];
}split_args_t;

static int find_pra_by_name(char *name,int argc, char *argv[])
{
    int i=0;
	for(i=0;i<argc;i++)
	{
         if(!strcmp(name,argv[i]))
         {
            return i;
         }
	}
	return 0;
}

static int show_help()
{  
	printf("--help: show this help\n");		
	printf("--time [ms]: the time to flush\n");	
	printf("--sch_time [us]: the time to flush\n");	
	printf("--pid [id]: the process to stats\n");	
	return 0;
}



split_args_t *get_args(const char *pcCommandString,char *split_str)
{
  split_args_t * args=(split_args_t *)malloc(sizeof(split_args_t));
  if(!args){
	 printf("malloc failed! line");
	 return NULL;
  }
  char *p_cmd=(char *)malloc(BUFFER_SIZE);
  if(!p_cmd){
	 printf("malloc failed! line");
	 return NULL;
  }
  char *p_str=NULL;
  char	*split_char=split_str;
  char *p_free_p_cmd=p_cmd;
  memset(args,0,sizeof(split_args_t));
  strcpy(p_cmd,pcCommandString);


  p_str=strtok(p_cmd,split_char);
  if(p_str)
  {
	strcpy(&args->args[args->args_num][0],p_str);
	args->args_num++;
  }
  while(p_str)
  {
	p_str=strtok(NULL,split_char);
	if(p_str)
	{
	  strcpy(&args->args[args->args_num][0],p_str);
	  args->args_num++;
	}
  }
  free(p_free_p_cmd);
  return args;
}

static int is_dec_number(char * pstr) 
{
   for(int i=0;pstr[i]!=0;i++)
   {
       if(!(pstr[i]>='0' && pstr[i]<='9'))
       {
           return 0;
       }	   	
   }
   return 1;
}


static int cmp_run_time(const void *a,const void *b)
{
    ar_thread_info_t *p_a=(ar_thread_info_t *)a;	
    ar_thread_info_t *p_b=(ar_thread_info_t *)b;
	return p_b->run_time_us-p_a->run_time_us;
}

int main(int argc, char *argv[])
{   
   ar_log_init();
   int index=find_pra_by_name("--help",argc,argv);
   if(index>0 || argc==1)
   {
       show_help();
	   return 0;
   }
   int time_deata_ms=1000; //1s=1000ms
   index=find_pra_by_name("--time",argc,argv);
   if(index>0)
   {
       time_deata_ms=atoi(argv[index+1]);
   }
   int sch_time=0; //1s=1000ms
   index=find_pra_by_name("--sch_time",argc,argv);
   if(index>0)
   {
       sch_time=atoi(argv[index+1]);
   }

   int pid=0;
   index=find_pra_by_name("--pid",argc,argv);
   if(index>0)
   {
       pid=atoi(argv[index+1]);
   }else
   {
	   show_help();
	   return 0;
   }

   ar_thread_info_t *info=malloc(MAX_THERAD_NUM*sizeof(ar_thread_info_t));

   if(!info)
   {
      printf("low mem \n");
	  return -1;
   }   
   struct dirent *sub_dir=NULL;
   int sub_dir_index=0;
   char *sub_dir_name=malloc(512);
   char *p_buffer=malloc(16*4096);


   //open the dir
   char dir_name[128];
   sprintf(dir_name,"/proc/%d/task",pid);
   DIR *dir=opendir(dir_name);
   if(!dir)
   {
      printf("not exit dir %s \n",dir_name);
	  free(info);
	  return -1;
   }
   while(1)
   {
       sub_dir=readdir(dir);
	   if(!sub_dir){
	   	 //printf("read dir %s err \n",dir_name);
	   	 break;
	   }
	   if(!is_dec_number(sub_dir->d_name))
	   {
	     continue;
	   }
	   sprintf(sub_dir_name,"/proc/%s/status",sub_dir->d_name);
	   int fd=open(sub_dir_name,O_RDWR | O_SYNC);
	   if(fd>0)
	   {
	       int read_count=read(fd,p_buffer,BUFFER_SIZE);
		   if(read_count>0)
		   {
		       info[sub_dir_index].us_read_start=ar_get_timestamp_us();
		       p_buffer[read_count]=0;
			   info[sub_dir_index].tid=atoi(sub_dir->d_name);
		       split_args_t *split=get_args(p_buffer,"\n");	

			   #if 0
		       printf("%s \n",split->args[0]);
			   printf("%s \n",split->args[46]);
		       printf("%s \n",split->args[47]);
		       printf("%s \n",split->args[48]);
               #endif
			   
		       split_args_t *split_line=get_args(split->args[0],":");
			   split_args_t *split_name=get_args(split_line->args[1],"\t");
			   strcpy(info[sub_dir_index].name,split_name->args[0]); 
			   free(split_name);
			   free(split_line);               
		       split_line=get_args(split->args[46],":");
			   info[sub_dir_index].sch_voluntary=atoi(split_line->args[1]);
			   free(split_line);
		       split_line=get_args(split->args[47],":");
			   info[sub_dir_index].sch_nonvoluntary=atoi(split_line->args[1]);
			   free(split_line);
			   split_line=get_args(split->args[48],":");
			   info[sub_dir_index].run_time_start=atoi(split_line->args[1]);
			   free(split_line);
			   #if 0
			   printf("%d \n",info[sub_dir_index].sch_voluntary);
			   printf("%d \n",info[sub_dir_index].sch_nonvoluntary);
			   printf("%d \n",info[sub_dir_index].run_time_start);
			   #endif

			   free(split);
			   sub_dir_index++;
			   			   
		   }else
		   {
		       printf("read file %s failed \n",sub_dir_name);
		   }
		   close(fd);
	  }else
	  {
	       printf("open file %s err \n",sub_dir_name);
	  }
  }    
  closedir(dir);
  usleep(time_deata_ms*1000);	
  //re open the dir
  dir=opendir(dir_name);
  if(!dir)
  {
      printf("not exit dir %s \n",dir_name);
	  free(info);
	  return -1;
  }
  while(1)
  {
       sub_dir=readdir(dir);
	   if(!sub_dir){
	   	 //printf("read dir %s err \n",dir_name);
	   	 break;
	   }
	   if(!is_dec_number(sub_dir->d_name))
	   {
	     continue;
	   }
	  sprintf(sub_dir_name,"/proc/%s/status",sub_dir->d_name);
	  int fd=open(sub_dir_name,O_RDWR | O_SYNC);
	  if(fd>0)
      {
		   int read_count=read(fd,p_buffer,BUFFER_SIZE);
		   if(read_count>0)
		   {
		       uint64_t read_end_time=ar_get_timestamp_us();
               int tid_new=atoi(sub_dir->d_name);

			   for(int i=0;i<sub_dir_index;i++)
			   {
			       if(info[i].tid==tid_new)
			       {
			           info[i].us_read_end=read_end_time;
        		       p_buffer[read_count]=0;
        			   info[i].tid=tid_new;
        		       split_args_t *split=get_args(p_buffer,"\n");		
					   #if 0
            		   printf("%s \n",split->args[0]);
            		   printf("%s \n",split->args[46]);
            		   printf("%s \n",split->args[47]);
            		   printf("%s \n",split->args[48]);
					   #endif
            		       		   
            		   split_args_t *split_line=get_args(split->args[46],":");
            		   info[i].sch_voluntary_end=atoi(split_line->args[1]);
            		   free(split_line);
            		   split_line=get_args(split->args[47],":");
            		   info[i].sch_nonvoluntary_end=atoi(split_line->args[1]);
            		   free(split_line);
            		   split_line=get_args(split->args[48],":");
            		   info[i].run_time_end=atoi(split_line->args[1]);
            		   free(split_line);

					   #if 0
            		   printf("%d \n",info[i].sch_voluntary_end);
            		   printf("%d \n",info[i].sch_nonvoluntary_end);
            		   printf("%d \n",info[i].run_time_end);
					   #endif
            		   
            		   free(split);
					   break;
			       }
			   }    		   
		   }
		   close(fd);
	 }else
	 {
	       printf("open file %s err \n",sub_dir_name);
	 }
 }		  
 closedir(dir);

 printf("\n");
 
 printf("%-8s %-32s %-16s %-10s %-10s %-10s %-10s %-10s\n\n\n",\
  		   "pid","thread_name","run_time_us","sch","sch_n","cpu_%","sch_t","sch_%");


 float cpu_per_sum=0;
 int   sch_sum=0;
 float sch_per_sum=0;
 for(int i=0;i<sub_dir_index;i++)
 {
     uint64_t run_time_us=(info[i].run_time_end-info[i].run_time_start)/1000;
	 info[i].run_time_us=run_time_us;

  }
  qsort(info,sub_dir_index,sizeof(info[0]),cmp_run_time);
 
  for(int i=0;i<sub_dir_index;i++)
  {   

	  int sch_num=info[i].sch_voluntary_end-info[i].sch_voluntary;
	  int sch_n_num=info[i].sch_nonvoluntary_end-info[i].sch_nonvoluntary;
	  int sch_total_num=sch_num+sch_n_num;
	  uint64_t read_time=(info[i].us_read_end-info[i].us_read_start);
	  float cpu_per=(float)info[i].run_time_us/read_time*100;
	  int sch_time_l=sch_total_num*sch_time;
	  float sch_per=(float)sch_time_l/read_time*100;
	  
	  cpu_per_sum+=cpu_per;
	  sch_sum+=sch_total_num;
	  sch_per_sum+=sch_per;
	  
      printf("%-8d %-32s %-16d %-10d %-10d %-10f %-10d %-10f\n",\
    		   info[i].tid,\
    		   info[i].name,\
    		   info[i].run_time_us,\
    		   sch_num,\
    		   sch_n_num,\
    		   cpu_per,\
    		   sch_time_l,
    		   sch_per);
  }
 
  printf("cpu_per_sum:%f sch_sum=%d sch_per_sum=%f\n",cpu_per_sum,sch_sum,sch_per_sum);
  return 0;
}

