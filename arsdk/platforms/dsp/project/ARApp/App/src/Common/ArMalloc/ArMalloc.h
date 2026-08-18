
#ifndef ARMALLOC_H_
#define ARMALLOC_H_

#ifdef __cplusplus
extern "C"{
#endif


 #ifdef __xstormy16__
 #define MALLOC_DIRECTION -1
 #endif


 #ifndef MALLOC_DIRECTION
 #define MALLOC_DIRECTION 1
 #endif

 #include <stddef.h>

 typedef struct freelist_entry {
   size_t size;
   struct freelist_entry *next;
 } *fle;


 struct mallinfo {
	int arena;     /* Non-mmapped space allocated (bytes) */
	int ordblks;   /* Number of free chunks */
	int smblks;    /* Number of free fastbin blocks */
	int hblks;     /* Number of mmapped regions */
	int hblkhd;    /* Space allocated in mmapped regions (bytes) */
	int usmblks;   /* Maximum total allocated space (bytes) */
	int fsmblks;   /* Space in freed fastbin blocks (bytes) */
	int uordblks;  /* Total allocated space (bytes) */
	int fordblks;  /* Total free space (bytes) */
	int keepcost;  /* Top-most, releasable space (bytes) */
};

 /* Return the number of bytes that need to be added to X to make it
    aligned to an ALIGN boundary.  ALIGN must be a power of 2.  */
 #define M_ALIGN(x, align) (-(size_t)(x) & ((align) - 1))


 /* Return the number of bytes that need to be subtracted from X to make it
    aligned to an ALIGN boundary.  ALIGN must be a power of 2.  */
 #define M_ALIGN_SUB(x, align) ((size_t)(x) & ((align) - 1))


 /* This is the minimum gap allowed between __malloc_end and the top of
    the stack.  This is only checked for when __malloc_end is
    decreased; if instead the stack grows into the heap, silent data
    corruption will result.  */
 #define MALLOC_MINIMUM_GAP 32


 #ifdef __xstormy16__
 // register void * stack_pointer asm ("r15");
 // #define MALLOC_LIMIT stack_pointer
 #else
// #define MALLOC_LIMIT __builtin_frame_address (0)
 #endif

  /* real_size is the size we actually have to allocate, allowing for
     overhead and alignment.  */
  #define REAL_SIZE(sz)						\
    ((sz) < sizeof (struct freelist_entry) - sizeof (size_t)	\
     ? sizeof (struct freelist_entry)				\
     : sz + sizeof (size_t) + M_ALIGN(sz, sizeof (size_t)))


extern void ar_malloc_init(size_t,size_t);
extern void* ar_malloc(size_t);
extern void  ar_free(void*);
extern void* ar_realloc(void*, size_t);
extern void* ar_memalign(size_t, size_t);
extern void* ar_valloc(size_t);
extern void* ar_pvalloc(size_t);
extern void* ar_calloc(size_t, size_t);
extern void  ar_cfree(void*);
extern int  ar_malloc_trim(size_t);
extern size_t  ar_malloc_usable_size(void*);
extern void    ar_malloc_stats(void);
extern int     ar_mallopt(int, int);
extern struct mallinfo ar_mallinfo(void);

#define vArMallocInit ar_malloc_init
#define pvArMalloc ar_malloc
#define vArFree ar_free
#define pvArRealloc ar_realloc
#define pvArMemAlign  ar_memalign
#define pvArValloc ar_valloc
#define pvArPvalloc ar_pvalloc
#define pvArCalloc ar_calloc
#define vArCfree ar_cfree
#define xArCallocTrim ar_malloc_trim
#define xArMallocUsableSize ar_malloc_usable_size
#define vArMallocStats ar_malloc_stats
#define xArMallopt ar_mallopt
#define xArMallinfo ar_mallinfo


#ifdef __cplusplus
}
#endif
#endif /* ARMALLOC_H_ */


