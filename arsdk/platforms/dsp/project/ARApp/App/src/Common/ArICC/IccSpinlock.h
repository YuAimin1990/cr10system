#ifndef __ICCSPINLOCK_H__
#define __ICCSPINLOCK_H__
#include <stdint.h>

#define ICC_SPINLOCK_LOCK       1
#define ICC_SPINLOCK_UNLOCK     0

typedef uint32_t shm_lock;

void vIccShmLockInit(shm_lock *plock);
void vIccShmAcquireLock(shm_lock *plock);
void vIccShmReleaseLock(shm_lock *plock);

#endif
