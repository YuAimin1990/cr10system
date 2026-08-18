#ifndef __OSAL_TIMER_H__
#define __OSAL_TIMER_H__
#include <time.h>
#include <signal.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

typedef void* ar_timer_id_t;
typedef void(*time_func)(sigval_t);

typedef struct {
    timer_t timer;
} ar_timer_t;

//typedef struct {
//    // TODO
//} ar_timer_attr_t;

// Create timer with callback func
ar_timer_id_t ar_timer_create(time_func func, void* arg, void* attr);

// Set timer interval(ms) and timeout(ms), then start timer.
// If interval is 0, callback func will only run once, otherwise
// func will be executed in each interval.
int32_t ar_timer_start(ar_timer_id_t id, uint32_t interval, uint32_t timeout);

// Stop timer
int32_t ar_timer_stop(ar_timer_id_t id);

// delete timer
int32_t ar_timer_delete(ar_timer_id_t id);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif //_AR_TIMER_H_
