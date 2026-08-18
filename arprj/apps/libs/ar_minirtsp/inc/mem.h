
#ifndef __MEM_H__
#define __MEM_H__

void *j_malloc(size_t size);
void *j_mallocz(size_t size);
void j_free(void *ptr);

void *j_calloc(size_t nmemb, size_t size);
void *j_memdup(void *src, size_t size);

#endif /* __MEM_H__ */
