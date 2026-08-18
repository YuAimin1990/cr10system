
#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

typedef struct JThreadPool JThreadPool;

JThreadPool *j_thread_pool_new(int num_workers, int max_jobs);
int j_thread_pool_destroy(JThreadPool *tp);

int j_thread_pool_add_job(JThreadPool *tp, int (*func)(void *ctx), void *ctx);

int j_thread_pool_stat(JThreadPool *tp, int *nb_workers, int *nb_jobs);

#endif /* __THREADPOOL_H__ */
