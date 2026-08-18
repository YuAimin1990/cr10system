/*****************************************************************************
Copyright: 2016-2020, Artosyn. Co., Ltd.
File name: ar_meta_list.h
Description: meta list apis define
Author: ycqiu@artosyn.cn
Version: v1.0
Date:2018-05-22
History:2018-05-22 : first release sdk
*****************************************************************************/
#ifndef __UTILS_DATA_LIST_H__
#define __UTILS_DATA_LIST_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


#include <stdbool.h>
#include "utils_list.h"
typedef void* ar_data_t;
typedef const void* ar_const_data_t;

/* context define */
typedef struct {
    ar_list_t list;
    ar_data_t element;
} ar_data_list_t;

typedef bool (*arCompareFunc)(ar_data_t, ar_const_data_t);

bool ar_data_list_init(ar_list_t* list_head);
bool ar_data_list_add(ar_list_t* list_head, ar_data_t element);
void ar_data_list_remove(ar_list_t* list_head, ar_data_t element);
void ar_data_list_remove2(ar_list_t* list_head, ar_data_t element, unsigned int size);
void ar_data_list_remove_all(ar_list_t* list_head);
ar_data_t ar_data_list_pop_front(ar_list_t* list_head);
ar_data_t ar_data_list_find(ar_list_t* list_head, ar_data_t data, unsigned int size);
ar_data_t ar_data_list_find_custom(ar_list_t* list_head, ar_const_data_t data, arCompareFunc func);

#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif /* End of #ifdef __cplusplus */

#endif // __UTILS_DATA_LIST_H__
