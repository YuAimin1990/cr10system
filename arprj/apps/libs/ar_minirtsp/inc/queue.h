
#ifndef __QUEUE_H__
#define __QUEUE_H__

typedef struct JQueue JQueue;

JQueue *j_queue_new(void);
int j_queue_destroy(JQueue *q);
int j_queue_abort(JQueue *q);

int j_queue_put(JQueue *q, void *data);
void *j_queue_get(JQueue *q, int block);
int j_queue_length(JQueue *q);

#endif /* __QUEUE_H__ */
