
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mem.h"

void *j_malloc(size_t size)
{
	return malloc(size);
}

void *j_mallocz(size_t size)
{
	void *ptr = j_malloc(size);
	if (ptr)
		memset(ptr, 0, size);
	return ptr;
}

void *j_calloc(size_t nmemb, size_t size)
{
	return calloc(nmemb, size);
}

void j_free(void *ptr)
{
	if (ptr)
		free(ptr);
}

void *j_memdup(void *src, size_t size)
{
	if (!src || !size)
		return NULL;

	void *ptr = j_malloc(size);
	if (!ptr)
		return NULL;

	memcpy(ptr, src, size);

	return ptr;
}
