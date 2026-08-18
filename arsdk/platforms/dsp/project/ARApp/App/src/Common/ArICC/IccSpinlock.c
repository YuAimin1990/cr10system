/*
 * Icc spin lock for cortex-A
 */

#include "IccSpinlock.h"
#include "IccPort.h"

void lvSpinlockInitializationStatic(lock_t * lock)
{
	memset((void *)lock, 0x0, sizeof(lock_t));
	return;
}

void vIccShmLockInit(shm_lock *plock)
{
	*plock = ICC_SPINLOCK_UNLOCK;
	lvSpinlockInitializationStatic(plock);
}

void vIccShmAcquireLock(shm_lock *plock)
{
	__asm__("dint");
	__asm__("nop");
	vSpinLock(plock);
}

void vIccShmReleaseLock(shm_lock *plock)
{
	vSpinUnlock(plock);
	__asm__("eint");
	__asm__("nop");
}
