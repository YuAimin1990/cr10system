
#ifndef __SHMEM_H__
#define __SHMEM_H__

#include <stdint.h>
#include <semaphore.h>

typedef struct JShmem JShmem;

typedef int (*ShmemFunc)(void *buf, size_t size, void *ctx);

JShmem *j_shmem_create (const char *name, int size);
JShmem *j_shmem_connect(const char *name, int size);

int j_shmem_destroy(JShmem *f);

ssize_t j_shmem_read (JShmem *f, void *buf, size_t size, ShmemFunc func, void *ctx, int milliseconds);
ssize_t j_shmem_write(JShmem *f, void *buf, size_t size, ShmemFunc func, void *ctx, int milliseconds);

#endif /* __SHMEM_H__ */
