
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stdafx.h"
//define a function in specific condition
//static int find_specific_value(list_mem_st * list_mem, int * value);
#include "common_list.h"
int common_list_init(list_st * list)
{
	list->head.before = NULL;
	list->head.after = &list->rear;
	list->rear.before = &list->head;
	list->rear.after = NULL;
	list->list_count = 0;
//	list->list_lock = spinlock_initialization();

	return 0;
}

//insert head
int common_list_insert(list_mem_st * list_mem, list_st * list)
{
//	spin_lock(list->list_lock);
	list_mem->after = list->head.after;
	list_mem->before = &(list->head);
	list->head.after->before = list_mem;
	list->head.after = list_mem;
	list->list_count++;
//	spin_unlock(list->list_lock);
	
	return 0;
}

int get_common_list_elem_from_head(list_mem_st ** list_mem, list_st * list)
{
	if(list->list_count <= 0)
	{
		return -1;
	}
//	spin_lock(list->list_lock);
	*list_mem = list->head.after;
	list->head.after = list->head.after->after;
	list->head.after->before = &list->head;
	list->list_count--;
//	spin_unlock(list->list_lock);

	return 0;
}

int get_common_list_elem_from_rear(list_mem_st ** list_mem, list_st * list)
{
	if(list->list_count <= 0)
	{
		return -1;
	}
//	spin_lock(list->list_lock);
	*list_mem = list->rear.before;
	list->rear.before = list->rear.before->before;
	list->rear.before->after = &list->rear;	
	list->list_count--;	
//	spin_unlock(list->list_lock);

	return 0;
}

int get_common_list_elem_condition(list_mem_st ** list_mem, list_st * list, find_list_cond_arg_s arg)
{
	int find_times = 0;
	const int list_num = list->list_count;

	if(list_num <= 0)
	{
		return -1;
	}
	*list_mem = list->head.after;
	arg.arg1 = *list_mem;
	while(0 == arg.find_list_cond(arg.arg1, arg.arg2))
	{
		find_times ++;
//if finding times exceeds the list numbers,return -1
		if(find_times > list_num)
		{
			return -1;
		}
		*list_mem = (*list_mem)->after;
		arg.arg1 = *list_mem;
	}
//	spin_lock(list->list_lock);
	((*list_mem)->before)->after = (*list_mem)->after;
	((*list_mem)->after)->before = (*list_mem)->before;
	(*list_mem)->before = NULL;
	(*list_mem)->after = NULL;
	list->list_count--;
//	spin_unlock(list->list_lock);

	return 0;
}

int find_common_list_elem_condition(list_mem_st ** list_mem, list_st * list, find_list_cond_arg_s arg)
{
	int find_times = 0;
	const int list_num = list->list_count;

	if(list_num <= 0)
	{
		return -1;
	}
	*list_mem = list->head.after;
	arg.arg1 = *list_mem;
	while(0 == arg.find_list_cond(arg.arg1, arg.arg2))
	{
		find_times ++;
//if finding times exceeds the list numbers,return -1
		if(find_times > list_num)
		{
			return -1;
		}
		*list_mem = (*list_mem)->after;
		arg.arg1 = *list_mem;
	}

	return 0;
}

int modify_common_list_elem_value_condition(void * mem_value, list_st * list, modify_list_cond_arg_s arg)
{
	list_mem_st * tmp_list_mem = NULL;

	if(find_common_list_elem_condition(&tmp_list_mem, list, arg.find_list_cond_arg) < 0)
	{
		return -1;
	}
//	spin_lock(list->list_lock);
	memcpy(tmp_list_mem->value, mem_value, arg.arg_len);
//	spin_unlock(list->list_lock);

	return 0;
}

int find_and_modify_common_list_elem_value_condition(void * mem_value, void * old_mem_value, list_st * list, modify_list_cond_arg_s arg)
{
	list_mem_st * tmp_list_mem = NULL;

	if(find_common_list_elem_condition(&tmp_list_mem, list, arg.find_list_cond_arg) < 0)
	{
		return -1;
	}
	memcpy(old_mem_value, tmp_list_mem->value, arg.arg_len);
//	spin_lock(list->list_lock);
	memcpy(tmp_list_mem->value, mem_value, arg.arg_len);
//	spin_unlock(list->list_lock);

	return 0;
}


#if 0
static int find_specific_value(list_mem_st * list_mem, int * value)
{
	int ret = 0;

	ret = ((((data_s *)(list_mem->value))->value1) == (*value)) ? (1) : (0);

	return ret;
}


int common_list_test_case()
{
	list_st list;
	list_mem_st *list_mem1 = NULL, *list_mem2 = NULL, *list_mem3 = NULL, *tmp_list_mem = NULL;
	int i = 0;
	int find_conditon = 2;
	find_list_cond_arg_s arg;

	memset(&arg, 0x0, sizeof(find_list_cond_arg_s));
	common_list_init(&list);
	tmp_list_mem = malloc(sizeof(list_mem_st));
	tmp_list_mem->value = malloc(sizeof(data_s));
	list_mem1 = malloc(sizeof(list_mem_st));
	list_mem1->value = malloc(sizeof(data_s));
	((data_s *)(list_mem1->value))->value1 = 1;
	common_list_insert(list_mem1, &list);
	list_mem2 = malloc(sizeof(list_mem_st));
	list_mem2->value = malloc(sizeof(data_s));
	((data_s *)(list_mem2->value))->value1 = 2;
	common_list_insert(list_mem2, &list);
	list_mem3 = malloc(sizeof(list_mem_st));
	list_mem3->value = malloc(sizeof(data_s));
	((data_s *)(list_mem3->value))->value1 = 3;
	common_list_insert(list_mem3, &list);
	arg.find_list_cond = find_specific_value;
	arg.arg2 = &find_conditon;
	get_common_list_elem_condition(&tmp_list_mem, &list, arg);
	printf("get elem from list:%d\n", ((data_s *)(tmp_list_mem->value))->value1);
	get_common_list_elem_from_head(&tmp_list_mem, &list);
	printf("get elem from list:%d\n", ((data_s *)(tmp_list_mem->value))->value1);
	get_common_list_elem_from_head(&tmp_list_mem, &list);
	printf("get elem from list:%d\n", ((data_s *)(tmp_list_mem->value))->value1);
	free(list_mem1->value);
	free(list_mem1);
	free(list_mem2->value);
	free(list_mem2);
	free(list_mem3->value);
	free(list_mem3);
	
	return 0;
}
#endif
