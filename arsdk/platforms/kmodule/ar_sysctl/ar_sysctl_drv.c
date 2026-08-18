#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/sched_clock.h>
#include <linux/wait.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/kthread.h>
#include <linux/atomic.h>
#include <linux/freezer.h>

#include "hi_osal.h"
#include "ar_sysctl_drv.h"

typedef struct
{
    STRU_REGISTER_INFO    info;
    int                   is_active;
    struct file           *file;
    atomic_t              trigger_cond;
    struct osal_list_head node;
} ar_sysctl_node;

typedef struct
{
    wait_queue_head_t      trigger_waitq;
    atomic_t               done_cond;
    wait_queue_head_t      done_waitq;
    int                    active_count;
    int                    inactive_count;
    struct osal_list_head  list_head;
} ar_sysctl_priorty_element;

typedef struct
{
    struct mutex              mutex_lock;
    ENUM_AR_SYSCTL_STATUS     stats;
    struct proc_dir_entry     *proc_dir;
    struct task_struct        *task_thread;
    struct semaphore          sem;
    struct semaphore          sem_suspend;
    ENUM_AR_SYSCTL_EVENT      event;
    ar_sysctl_priorty_element *priority_element[SYSCTL_MAX_PRIORITY];
} ar_sysctl_server;

ar_sysctl_server g_sysctl_server = {0};
static long g_time_out           = -1;
static int g_timeout_handle      = 0;

static ar_sysctl_priorty_element* ar_create_priority_element(unsigned int priority)
{
    ar_sysctl_priorty_element *new_element = NULL;

    if(priority > SYSCTL_MAX_PRIORITY-1)
        return NULL;

    new_element = (ar_sysctl_priorty_element*)kmalloc(sizeof(ar_sysctl_priorty_element), GFP_KERNEL);
    if(!new_element){
        pr_err("%s %d: kmalloc failed.\n", __func__, __LINE__);
    }else{
        /***init node.***/
        memset(new_element, 0, sizeof(ar_sysctl_priorty_element));
        OSAL_INIT_LIST_HEAD(&new_element->list_head);
        atomic_set(&new_element->done_cond, 0);
        init_waitqueue_head(&new_element->trigger_waitq);
        init_waitqueue_head(&new_element->done_waitq);
    }

    return new_element;
}

static ar_sysctl_priorty_element* ar_sysctl_get_priority_element(unsigned int priority)
{
    if(priority > SYSCTL_MAX_PRIORITY-1)
        return NULL;

   return g_sysctl_server.priority_element[priority];
}

static int ar_sysctl_get_node_count(void)
{
    ar_sysctl_priorty_element *priority_element = NULL;
    int count = 0;
    int i;
    for(i=0; i<SYSCTL_MAX_PRIORITY; ++i){

        priority_element = ar_sysctl_get_priority_element(i);
        if(priority_element){
            count += priority_element->active_count;
        }
    }

    return count;
}

static int ar_sysctl_proc_state_show(struct seq_file *s, void *p)
{
    struct osal_list_head *this                 = NULL;
    ar_sysctl_node *sc_node                     = NULL;
    ar_sysctl_priorty_element *priority_element = NULL;
    int i = 0;

    seq_printf(s, "-------------------sysctl state--------------------\n");
    seq_printf(s, "stats               :  %s\n", g_sysctl_server.stats == SYSCTL_STATUS_RUNNING  ? "RUNNING" :
                                            g_sysctl_server.stats == SYSCTL_STATUS_SUSPEND ? "SUSPEND" :
                                            g_sysctl_server.stats == SYSCTL_STATUS_BUSY ? "BUSY" : "ERROR");

    seq_printf(s, "total_active_count  :  %d\n", ar_sysctl_get_node_count());
    seq_printf(s, "\n");

    seq_printf(s, "-------------------apps info------------------------\n");
    mutex_lock(&g_sysctl_server.mutex_lock);
    for(i=0; i<SYSCTL_MAX_PRIORITY; ++i){

        priority_element = ar_sysctl_get_priority_element(i);
        if(!priority_element)
            continue;

        if(!priority_element->active_count && !priority_element->inactive_count)
            continue;

        seq_printf(s, "priority            :  %d\n", i);
        seq_printf(s, "active_count        :  %d\n", priority_element->active_count);
        seq_printf(s, "inactive_count      :  %d\n", priority_element->inactive_count);
        seq_printf(s, "\n");

        osal_list_for_each(this, &priority_element->list_head) {
            sc_node = osal_list_entry(this, ar_sysctl_node, node);
            if(!sc_node)
                continue;

            seq_printf(s, "  name              :  %s\n", sc_node->info.name);
            seq_printf(s, "  is_active         :  %s\n", sc_node->is_active ? "Y" : "N");
            seq_printf(s, "\n");
        }

    }

    mutex_unlock(&g_sysctl_server.mutex_lock);

    return 0;

}

static int ar_sysctl_proc_state_open(struct inode *inode, struct file *file)
{
    return single_open(file, ar_sysctl_proc_state_show, inode->i_private);
}

static struct file_operations ar_sysctl_proc_state_fops={
    .owner      = THIS_MODULE,
    .open       = ar_sysctl_proc_state_open,
    .release    = single_release,
    .read       = seq_read,
};

static int ar_sysctl_proc_create(void)
{
    struct proc_dir_entry * entry;

    g_sysctl_server.proc_dir = proc_mkdir("arsysctl", NULL);
    if(!g_sysctl_server.proc_dir)
    {
        printk(KERN_ERR"Create proc dir failed.\n");
        return -ENOMEM;
    }

    entry = proc_create("state", 0, g_sysctl_server.proc_dir, &ar_sysctl_proc_state_fops);
    if(!entry)
    {
        printk(KERN_ERR"Create proc state failed.\n");
        return -ENOMEM;
    }

    return 0;
}

static int ar_sysctl_proc_destroy(void)
{
    if(g_sysctl_server.proc_dir)
    {
        proc_remove(g_sysctl_server.proc_dir);
    }

    return 0;
}

static ar_sysctl_node* ar_list_get_node_by_name(struct osal_list_head* list_head, char* name)
{
    int flag = 0;
    struct osal_list_head *this = NULL;
    ar_sysctl_node        *sc_node = NULL;

    osal_list_for_each(this, list_head) {
        sc_node = osal_list_entry(this, ar_sysctl_node, node);
        if(!sc_node)
            continue;

        if(!strcmp(name, sc_node->info.name)){
            flag = 1;
            break;
        }
    }

    if(!flag)
        sc_node = NULL;

    return sc_node;
}

static int ar_list_del_node_by_name(struct osal_list_head* list_head, char* name)
{
    struct osal_list_head *this = NULL;
    ar_sysctl_node        *sc_node = NULL;

    osal_list_for_each(this, list_head) {
        sc_node = osal_list_entry(this, ar_sysctl_node, node);
        if(!sc_node)
            continue;

        if(!strcmp(name, sc_node->info.name)){
            osal_list_del(this);
            kfree(sc_node);
            sc_node = NULL;
            return 0;
        }
    }

    return -1;
}

int ar_sysctl_task(int is_suspend)
{
    int ret = 0;
    int i = 0;
    ar_sysctl_priorty_element *priority_element = NULL;
    struct osal_list_head     *this = NULL;
    ar_sysctl_node            *sc_node = NULL;
    if(is_suspend)
        i = SYSCTL_MAX_PRIORITY-1;

    for( i=(is_suspend ? SYSCTL_MAX_PRIORITY-1 : 0); is_suspend ? i>=0 : i<SYSCTL_MAX_PRIORITY; is_suspend ? --i : ++i){

        /*** priority_element can not be changed in busy state. ***/
        priority_element = ar_sysctl_get_priority_element(i);
        if(!priority_element)
            continue;

        if(!priority_element->active_count)
            continue;

        /*** the node active state can be set when close, so to protect it. ***/
        mutex_lock(&g_sysctl_server.mutex_lock);
        osal_list_for_each(this, &priority_element->list_head) {
            sc_node = osal_list_entry(this, ar_sysctl_node, node);
            if(!sc_node)
                continue;

            if(sc_node->is_active){
                sc_node->info.event = g_sysctl_server.event;
                atomic_set(&sc_node->trigger_cond, 1);
            }
        }
        mutex_unlock(&g_sysctl_server.mutex_lock);

        mb();

        wake_up_all(&priority_element->trigger_waitq);

        if(-1 == g_time_out){
            /*** wake up when all apps return done event. ***/
            wait_event(priority_element->done_waitq,
                       priority_element->active_count == atomic_read(&priority_element->done_cond));
            atomic_set(&priority_element->done_cond, 0);
            pr_debug("%s %d wait event done_waitq success.\n", __func__, __LINE__);
            mb();
        }else{
            ret = wait_event_timeout(priority_element->done_waitq,
                                     priority_element->active_count == atomic_read(&priority_element->done_cond),
                                     g_time_out);
            atomic_set(&priority_element->done_cond, 0);
            mb();
            if(0 == ret){
                //0 if the @condition evaluated to %false after the @timeout elapsed,
                pr_err("%s %d: %s wait done_waitq timeout, time:%ld!!!.\n", __func__, __LINE__, sc_node->info.name, g_time_out);
                ret = -1;
                if(g_timeout_handle)
                    continue;
                else
                    break;
            }else{
                ret = 0;
                pr_debug("%s %d wait event done_waitq success.\n", __func__, __LINE__);
            }
        }
    }

    return ret;
}

static void ar_sysctl_clean_inactive_node(void)
{
    int i = 0;
    ar_sysctl_priorty_element *priority_element = NULL;
    struct osal_list_head *this = NULL;
    ar_sysctl_node *sc_node = NULL;
    struct osal_list_head tmp = {0};

    for(i=0; i<SYSCTL_MAX_PRIORITY; ++i){

        priority_element = ar_sysctl_get_priority_element(i);
        if(!priority_element)
            continue;

        if(!priority_element->inactive_count)
            continue;

        osal_list_for_each(this, &priority_element->list_head) {
            sc_node = osal_list_entry(this, ar_sysctl_node, node);
            if(!sc_node)
                continue;

            if(!sc_node->is_active){
                tmp.next = this->next;
                pr_info("%s %d: clean %s.\n", __func__, __LINE__, sc_node->info.name);
                osal_list_del(this);
                kfree(sc_node);
                sc_node = NULL;
                this = &tmp;
                priority_element->inactive_count--;
            }
        }

    }

}

static long ar_sysctl_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int ret = 0;

    switch(cmd) {
        case IOC_SYSCTL_REGISTER:{
            STRU_REGISTER_INFO *reg_info = (STRU_REGISTER_INFO *)arg;

            ar_sysctl_priorty_element *priority_element = NULL;
            ar_sysctl_node            *new_node = NULL;

            mutex_lock(&g_sysctl_server.mutex_lock);
            if(SYSCTL_STATUS_RUNNING != g_sysctl_server.stats){
                pr_err("%s %d: %s register, but is not in RUNNING status.\n", __func__, __LINE__, reg_info->name);
                ret = -EFAULT;
                goto end_register;
            }

            priority_element = ar_sysctl_get_priority_element(reg_info->priority);
            if(!priority_element){
                /*** It will be created in first time, then directly used. ***/
                priority_element = ar_create_priority_element(reg_info->priority);
                if(!priority_element){
                    pr_err("%s %d: %s create priority element failed, please check para(%d<SYSCTL_MAX_PRIORITY(%d)).\n",
                                                               __func__, __LINE__, reg_info->name, reg_info->priority, SYSCTL_MAX_PRIORITY);
                    ret = -EFAULT;
                    goto end_register;
                }
                g_sysctl_server.priority_element[reg_info->priority] = priority_element;
            }

            new_node = ar_list_get_node_by_name(&priority_element->list_head, reg_info->name);
            if(new_node){
                if(new_node->is_active){
                    pr_err("%s %d: %s has already register.\n", __func__, __LINE__, reg_info->name);
                    ret = -EFAULT;
                    goto end_register;
                }
                /*** If the node is in_active, then no need to create a new node, just active it. ***/
                priority_element->inactive_count--;
            }else{
                /*** create a new node. ***/
                new_node = (ar_sysctl_node*)kmalloc(sizeof(ar_sysctl_node), GFP_KERNEL);
                if(!new_node){
                    pr_err("%s %d: %s kmalloc failed.\n", __func__, __LINE__, reg_info->name);
                    ret = -EFAULT;
                    goto end_register;
                }
                memset(new_node, 0, sizeof(ar_sysctl_node));
                osal_list_add_tail(&(new_node->node), &priority_element->list_head);
            }

            new_node->info = *reg_info;
            new_node->file = file;
            new_node->is_active = 1;
            atomic_set(&new_node->trigger_cond, 0);

            priority_element->active_count++;

            /*** clean all in_active node which is caused by an abnormal exit. ***/
            ar_sysctl_clean_inactive_node();

            pr_info("%s %d: %s register success.\n", __func__, __LINE__, reg_info->name);

    end_register:
            mutex_unlock(&g_sysctl_server.mutex_lock);

            break;
        }

        case IOC_SYSCTL_UNREGISTER:{
            STRU_REGISTER_INFO *reg_info = (STRU_REGISTER_INFO *)arg;

            ar_sysctl_priorty_element *priority_element = NULL;

            mutex_lock(&g_sysctl_server.mutex_lock);

            if(SYSCTL_STATUS_RUNNING != g_sysctl_server.stats){
                pr_err("%s %d: %s unregister, but is not in RUNNING status.\n", __func__, __LINE__, reg_info->name);
                ret = -EFAULT;
                goto end_unregister;
            }

            priority_element = ar_sysctl_get_priority_element(reg_info->priority);
            if(!priority_element){
                pr_err("%s %d: %s get priority node failed, please check para(%d<SYSCTL_MAX_PRIORITY(%d)).\n",
                                                           __func__, __LINE__, reg_info->name, reg_info->priority, SYSCTL_MAX_PRIORITY);
                ret = -EFAULT;
                goto end_unregister;
            }

            if(ar_list_del_node_by_name(&priority_element->list_head, reg_info->name)){
                pr_err("%s %d : %s has not register.\n", __func__, __LINE__, reg_info->name);
                ret = -EFAULT;
            }else{
                pr_info("%s %d: %s unregister success.\n", __func__, __LINE__, reg_info->name);
                priority_element->active_count--;
            }

    end_unregister:

            mutex_unlock(&g_sysctl_server.mutex_lock);

            break;
        }

        case IOC_SYSCTL_WAIT_EVENT:{
            STRU_REGISTER_INFO * __user reg_info = (STRU_REGISTER_INFO *)arg;

            ar_sysctl_priorty_element *priority_element = NULL;
            ar_sysctl_node            *node = NULL;

            /*** lock to protect list when other app register in same priority***/
            mutex_lock(&g_sysctl_server.mutex_lock);
            priority_element = ar_sysctl_get_priority_element(reg_info->priority);
            if(!priority_element){
                pr_err("%s %d: %s get priority element failed, please check para(%d<SYSCTL_MAX_PRIORITY(%d)).\n",
                                                           __func__, __LINE__, reg_info->name, reg_info->priority, SYSCTL_MAX_PRIORITY);
                ret = -EFAULT;
                mutex_unlock(&g_sysctl_server.mutex_lock);
                break;
            }

            node = ar_list_get_node_by_name(&priority_element->list_head, reg_info->name);
            if(!node){
                pr_err("%s %d: %s can find node.\n", __func__, __LINE__, reg_info->name);
                ret = -EFAULT;
                mutex_unlock(&g_sysctl_server.mutex_lock);
                break;
            }
            mutex_unlock(&g_sysctl_server.mutex_lock);

            pr_debug("%s %d: %s wait_event trigger_waitq.\n", __func__, __LINE__, reg_info->name);

            /*** not need to protect node:***/
            /*** 1. Hal layer ensures correct unregister process(wake up, then delete.); ***/
            /*** 2. It will be wake_up and set in_active in close when abnormal exit; ***/
            /*** 3. close and ar_sysctl_clean_inactive_node mutual exclusion. ***/
            ret = wait_event_freezable(priority_element->trigger_waitq, atomic_read(&node->trigger_cond));
            if (ret){
                pr_err("%s %d: %s wait trigger_waitq was interrupted!!!, ret:%d.\n", __func__, __LINE__, reg_info->name, ret);
                break;
            }
            atomic_set(&node->trigger_cond, 0);
            mb();

            ret = copy_to_user(reg_info, &node->info, sizeof(STRU_REGISTER_INFO));
            pr_debug("%s %d: %s wait trigger_waitq success.\n", __func__, __LINE__, reg_info->name);

            break;
        }

        case IOC_SYSCTL_EVENT_DONE:{
            STRU_REGISTER_INFO * __user reg_info = (STRU_REGISTER_INFO *)arg;
            ar_sysctl_priorty_element *priority_element = NULL;
            ar_sysctl_node            *node = NULL;

            mutex_lock(&g_sysctl_server.mutex_lock);
            priority_element = ar_sysctl_get_priority_element(reg_info->priority);
            if(!priority_element){
                pr_err("%s %d: %s get priority node failed, please check para(%d<SYSCTL_MAX_PRIORITY(%d)).\n",
                                                           __func__, __LINE__, reg_info->name, reg_info->priority, SYSCTL_MAX_PRIORITY);
                ret = -EFAULT;
                mutex_unlock(&g_sysctl_server.mutex_lock);
                break;
            }

            node = ar_list_get_node_by_name(&priority_element->list_head, reg_info->name);
            if(!node){
                pr_err("%s %d: %s can find node.\n", __func__, __LINE__, reg_info->name);
                ret = -EFAULT;
                mutex_unlock(&g_sysctl_server.mutex_lock);
                break;
            }
            mutex_unlock(&g_sysctl_server.mutex_lock);

            /*** not need to protect node, because it just work in unregister opareation or busy state.***/
            if(AR_SYSCTL_EVENT_EXIT == reg_info->event){
                node->info.event = AR_SYSCTL_EVENT_EXIT;
                atomic_set(&node->trigger_cond, 1);
                mb();
                pr_debug("%s %d: %s wake_up trigger_waitq.\n", __func__, __LINE__, reg_info->name);
                wake_up_all(&priority_element->trigger_waitq);
            }else{
                atomic_inc(&priority_element->done_cond);
                mb();
                pr_debug("%s %d: %s wake_up done_waitq.\n", __func__, __LINE__, reg_info->name);
                wake_up(&priority_element->done_waitq);
            }

            ret = copy_to_user(reg_info, &node->info, sizeof(STRU_REGISTER_INFO));
            break;
        }

        case IOC_SYSCTL_SUSPEND:{
            STRU_REGISTER_INFO * __user reg_info = (STRU_REGISTER_INFO *)arg;

            mutex_lock(&g_sysctl_server.mutex_lock);
            if(SYSCTL_STATUS_RUNNING != g_sysctl_server.stats){
                pr_err("%s %d: suspend failed, not in RUNNING status.\n", __func__, __LINE__);
                ret = -EFAULT;
                goto end_suspend;
            }

            if( 0 == ar_sysctl_get_node_count()){
                pr_err("%s %d: suspend failed, app count is 0.\n", __func__, __LINE__);
                ret = -EFAULT;
                goto end_suspend;
            }

            g_sysctl_server.event = reg_info->event;
            g_sysctl_server.stats = SYSCTL_STATUS_BUSY;
            up(&g_sysctl_server.sem);

    end_suspend:
            mutex_unlock(&g_sysctl_server.mutex_lock);

            if(AR_SYSCTL_EVENT_SUSPEND == g_sysctl_server.event){
                ret = down_interruptible(&g_sysctl_server.sem_suspend);
                if (ret == -EINTR)
                    pr_err("%s %d: even %d suspend interrupted!\n", __func__, __LINE__, g_sysctl_server.event);
            }

            break;
        }

        case IOC_SYSCTL_RESUME:{
            mutex_lock(&g_sysctl_server.mutex_lock);
            if(SYSCTL_STATUS_SUSPEND != g_sysctl_server.stats){
                pr_err("%s %d: resume failed, not in SUSPEND status\n", __func__, __LINE__);
                ret = -EFAULT;
                goto end_resume;
            }

            if(0 == ar_sysctl_get_node_count()){
                pr_err("%s %d: resume failed, app count is 0..\n", __func__, __LINE__);
                ret = -EFAULT;
            }
            else{
                g_sysctl_server.event = AR_SYSCTL_EVENT_FAST_RESUME;
                g_sysctl_server.stats = SYSCTL_STATUS_BUSY;
                up(&g_sysctl_server.sem);
            }
    end_resume:
            mutex_unlock(&g_sysctl_server.mutex_lock);

            break;
        }

        case IOC_SYSCTL_QUERY_STATUS:{
            ENUM_AR_SYSCTL_STATUS * __user status = (ENUM_AR_SYSCTL_STATUS *)arg;
            mutex_lock(&g_sysctl_server.mutex_lock);
            ret = copy_to_user(status, &g_sysctl_server.stats, sizeof(int));
            mutex_unlock(&g_sysctl_server.mutex_lock);
            break;
        }

        default:
            ret = -1;
            break;

    }

    return ret;
}

static int ar_sysctl_close(struct inode *inode, struct file *file)
{
    int i = 0;
    ar_sysctl_priorty_element *priority_element = NULL;
    struct osal_list_head *this = NULL;
    ar_sysctl_node *sc_node = NULL;

    pr_debug("%s %d\n", __func__, __LINE__);

    mutex_lock(&g_sysctl_server.mutex_lock);

    for(i=0; i<SYSCTL_MAX_PRIORITY; ++i){

        priority_element = ar_sysctl_get_priority_element(i);
        if(!priority_element)
            continue;

        if(!priority_element->active_count)
            continue;

        osal_list_for_each(this, &priority_element->list_head) {
            sc_node = osal_list_entry(this, ar_sysctl_node, node);
            if(!sc_node)
                continue;

            if(sc_node->file == file){
                sc_node->is_active = 0;
                priority_element->active_count--;
                priority_element->inactive_count++;
                pr_debug("%s %d %s is set to inactive\n", __func__, __LINE__, sc_node->info.name);
            }
        }

    }

    mutex_unlock(&g_sysctl_server.mutex_lock);

    return 0;
}

int ar_sysctl_thread_func(void * args)
{
    int ret = 0;
    while(0 == down_interruptible(&g_sysctl_server.sem)){

        pr_debug("%s %d down\n", __func__, __LINE__);
        if(kthread_should_stop()){
            do_exit(0);
        }

        if(AR_SYSCTL_EVENT_FAST_SUSPEND == g_sysctl_server.event ||
            AR_SYSCTL_EVENT_SUSPEND == g_sysctl_server.event){

            ret = ar_sysctl_task(1);

            mutex_lock(&g_sysctl_server.mutex_lock);

            if(ret){
                g_sysctl_server.stats = SYSCTL_STATUS_ERROR;
                pr_err("%s %d: %s failed\n", __func__, __LINE__,
                    AR_SYSCTL_EVENT_FAST_SUSPEND == g_sysctl_server.event ? "FAST_SUSPEND" : "SUSPEND");
            }else{
                g_sysctl_server.stats = SYSCTL_STATUS_SUSPEND;
                pr_info("%s %d: %s success\n", __func__, __LINE__,
                    AR_SYSCTL_EVENT_FAST_SUSPEND == g_sysctl_server.event ? "FAST_SUSPEND" : "SUSPEND");
            }

            mutex_unlock(&g_sysctl_server.mutex_lock);

            if(AR_SYSCTL_EVENT_SUSPEND == g_sysctl_server.event)
                up(&g_sysctl_server.sem_suspend);

        }else if(AR_SYSCTL_EVENT_FAST_RESUME == g_sysctl_server.event){

            ret = ar_sysctl_task(0);

            mutex_lock(&g_sysctl_server.mutex_lock);

            if(ret){
                 g_sysctl_server.stats = SYSCTL_STATUS_ERROR;
                 pr_err("%s %d: FAST_RESUME failed\n", __func__, __LINE__);
            }else{
                 g_sysctl_server.stats = SYSCTL_STATUS_RUNNING;
                 pr_info("%s %d: FAST_RESUME success\n", __func__, __LINE__);
            }

            mutex_unlock(&g_sysctl_server.mutex_lock);
        }

    }
    return 0;
}

static struct file_operations ar_sysctl_fops =
{
    .owner		        = THIS_MODULE,
    .unlocked_ioctl     = ar_sysctl_ioctl,
    .release            = ar_sysctl_close,
};

static struct miscdevice ar_sysctl =
{
    .minor = MISC_DYNAMIC_MINOR,
    .name = "ar_sysctl",
    .fops = &ar_sysctl_fops,
};

static void ar_sysctl_remove(void)
{
    int i = 0;
    ar_sysctl_priorty_element *priority_element = NULL;
    struct osal_list_head *this = NULL;
    ar_sysctl_node *sc_node = NULL;
    struct osal_list_head tmp = {0};

    for(i=0; i<SYSCTL_MAX_PRIORITY; ++i){

        priority_element = ar_sysctl_get_priority_element(i);
        if(!priority_element)
            continue;

        osal_list_for_each(this, &priority_element->list_head) {
            sc_node = osal_list_entry(this, ar_sysctl_node, node);
            if(!sc_node)
                continue;

            tmp.next = this->next;
            pr_info("%s %d: rm %s.\n", __func__, __LINE__, sc_node->info.name);
            osal_list_del(this);
            kfree(sc_node);
            sc_node = NULL;
            this = &tmp;
        }

        kfree(priority_element);
        priority_element = NULL;
    }

}

static int __init ar_sysctl_init(void)
{
    int ret = 0;

    ret = misc_register(&ar_sysctl);
    if(ret < 0) {
        goto out;
    }

    g_sysctl_server.stats = SYSCTL_STATUS_RUNNING;
    mutex_init(&g_sysctl_server.mutex_lock);
    ret = ar_sysctl_proc_create();
    sema_init(&g_sysctl_server.sem, 0);
    sema_init(&g_sysctl_server.sem_suspend, 0);

    g_sysctl_server.task_thread = kthread_run(ar_sysctl_thread_func, NULL, "sysctl_task_thread");
    if(!g_sysctl_server.task_thread)
        ret = -EINVAL;

out:
    pr_info("ar_sysctl_init %d\n", ret);

    return ret;
}

static void __exit ar_sysctl_exit(void)
{
    misc_deregister(&ar_sysctl);
    pr_info("ar_sysctl_exit\n");
    ar_sysctl_proc_destroy();
    up(&g_sysctl_server.sem);
    if(g_sysctl_server.task_thread){
        kthread_stop(g_sysctl_server.task_thread);
    }

    ar_sysctl_remove();
}

module_param_named(timeout, g_time_out, long, 0);
module_param_named(handle, g_timeout_handle, int, 0); // 0 default, stop handle; 1 continue handle other app.

module_init(ar_sysctl_init);
module_exit(ar_sysctl_exit);

MODULE_AUTHOR("Artosyn");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");

