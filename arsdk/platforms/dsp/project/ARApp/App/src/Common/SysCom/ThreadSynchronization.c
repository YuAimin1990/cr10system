
#include "ThreadSynchronization.h"

#include <string.h>
#include <vec-c.h>


lock_t * xSpinlockInitialization()
{
	lock_t * lock = NULL;

	lock = (lock_t * )malloc(sizeof(lock_t));
	//debug_print("lock:%x\r\n", lock);
	memset((void *)lock, 0x0, sizeof(lock_t));

	return lock;
}

void vSpinlockInitializationStatic(lock_t * lock)
{
	memset((void *)lock, 0x0, sizeof(lock_t));
	return;
}

int xSpinlockDestory(lock_t * lock)
{
	free(lock);
	return SYNCHRONIZATION_SUCCEED;
}
/*
void vSpinLock (lock_t * lock_bit)
{}
void vSpinUnlock(lock_t * lock_bit)
{}
*/
