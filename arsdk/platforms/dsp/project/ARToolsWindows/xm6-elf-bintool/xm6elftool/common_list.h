#ifndef COMMON_LIST_H
#define COMMON_LIST_H

//#include "thread_synchronization.h"
#ifdef WIN32
//hbbai: this lock is not needed for single thread, we just ignore them in this tools.
typedef int lock_t;
extern "C" {
#if defined(spinlock_initialization)
#undef spinlock_initialization
lock_t * spinlock_initialization() { return (lock_t *)0; }
#endif
#if defined(spin_lock)
#undef spin_lock
void spin_lock(lock_t * lock) {}
#endif
#if defined(spin_unlock)
#undef spin_unlock
void spin_unlock(lock_t * lock) {}
#endif
}

#endif

typedef struct list_mem_t
{
	void * value;
	struct list_mem_t * before;
	struct list_mem_t * after;
}list_mem_st;
typedef struct list_t
{
	list_mem_st head;
	list_mem_st rear;
	int list_count;
	lock_t * list_lock;
}list_st;
typedef int(*find_list_cond_t)(void* arg1, void* arg2);
typedef struct
{
	find_list_cond_t find_list_cond;
	void* arg1;
	void* arg2;
}find_list_cond_arg_s;

typedef struct
{
	find_list_cond_arg_s find_list_cond_arg;
	int arg_len;
}modify_list_cond_arg_s;

extern int common_list_init(list_st * list);
extern int common_list_insert(list_mem_st * list_mem, list_st * list);
extern int get_common_list_elem_from_head(list_mem_st ** list_mem, list_st * list);
extern int get_common_list_elem_from_rear(list_mem_st ** list_mem, list_st * list);
extern int get_common_list_elem_condition(list_mem_st ** list_mem, list_st * list, find_list_cond_arg_s arg);
extern int find_common_list_elem_condition(list_mem_st ** list_mem, list_st * list, find_list_cond_arg_s arg);
extern int modify_common_list_elem_value_condition(void * mem_value, list_st * list, modify_list_cond_arg_s arg);
extern int find_and_modify_common_list_elem_value_condition(void * mem_value, void * old_mem_value, list_st * list, modify_list_cond_arg_s arg);
//extern int common_list_test_case();

#endif
