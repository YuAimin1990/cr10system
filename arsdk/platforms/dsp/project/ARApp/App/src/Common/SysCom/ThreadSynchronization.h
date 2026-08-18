#ifndef THREADSYNCHRONIZATION_H
#define THREADSYNCHRONIZATION_H

#ifdef __cplusplus
extern "C"{
#endif

#define SYNCHRONIZATION_SUCCEED 0
#define SYNCHRONIZATION_FAILED	-1


typedef volatile int lock_t;

extern lock_t * xSpinlockInitialization();
extern void vSpinlockInitializationStatic(lock_t * lock);
extern int xSpinlockDestory(lock_t * lock);

extern void vSpinLock (lock_t * lock_bit);
extern void vSpinUnlock(lock_t * lock_bit);


#ifdef __cplusplus
}
#endif
#endif


