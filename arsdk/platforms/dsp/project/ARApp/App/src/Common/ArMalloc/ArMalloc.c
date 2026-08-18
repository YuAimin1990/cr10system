
#include "ArMalloc.h"

#include "string.h"
#include <stdio.h>


#define DEFINE_MALLOC
#define DEFINE_FREE
#define DEFINE_REALLOC
#define DEFINE_CALLOC
#define DEFINE_CFREE
#define DEFINE_MEMALIGN
#define DEFINE_VALLOC
#define DEFINE_PVALLOC
#define DEFINE_MALLINFO
#define DEFINE_MALLOC_USABLE_SIZE
#define DEFINE_MALLOC_STATS
#define DEFINE_MALLOPT



#if MALLOC_DIRECTION < 0
#define CAN_ALLOC_P(required)				\
  (((size_t) __ar_malloc_end - (size_t)MALLOC_LIMIT	\
	- MALLOC_MINIMUM_GAP) >= (required))
#else
#define CAN_ALLOC_P(required)				\
  (((size_t)__ar_malloc_limit - (size_t) __ar_malloc_end	\
	- MALLOC_MINIMUM_GAP) >= (required))
#endif


fle __ar_malloc_freelist;
void * __ar_malloc_start = NULL;
void * __ar_malloc_end = NULL;
size_t __ar_malloc_limit;


void ar_malloc_init(size_t start_addr,size_t heap_len)
{
	__ar_malloc_start = (void*)start_addr;
	__ar_malloc_end = (void*)start_addr;
	 __ar_malloc_limit = start_addr + heap_len;
}

#ifdef DEFINE_MALLOC

void *ar_malloc(size_t sz) {
	fle *nextfree;
	fle block;

	/* real_size is the size we actually have to allocate, allowing for
	 overhead and alignment.  */
	size_t real_size = REAL_SIZE(sz);

	/* Look for the first block on the freelist that is large enough.  */
	for (nextfree = &__ar_malloc_freelist; *nextfree; nextfree = &(*nextfree)->next)
	{
		block = *nextfree;
		if (block->size >= real_size)
		{
			/* If the block found is just the right size, remove it from
			 the free list.  Otherwise, split it.  */
			if (block->size < real_size + sizeof(struct freelist_entry))
			{
				*nextfree = block->next;
				return (void *) &block->next;
			}
			else
			{
				size_t newsize = block->size - real_size;
				fle newnext = block->next;
				*nextfree = (fle) ((size_t) block + real_size);
				(*nextfree)->size = newsize;
				(*nextfree)->next = newnext;
				goto done;
			}
		}

		/* If this is the last block on the freelist, and it was too small,
		 enlarge it.  */
		if (!block->next && __ar_malloc_end == (void *) ((size_t) block + block->size))
		{
			size_t moresize = real_size - block->size;
			if (!CAN_ALLOC_P(moresize))
				return NULL;

			*nextfree = NULL;
			if (MALLOC_DIRECTION < 0)
			{
				block = __ar_malloc_end = (void *) ((size_t) block - moresize);
			}
			else
			{
				__ar_malloc_end = (void *) ((size_t) block + real_size);
			}

			goto done;
		}
	}

	/* No free space at the end of the free list.  Allocate new space
	 and use that.  */

	if (!CAN_ALLOC_P(real_size))
		return NULL;

	if (MALLOC_DIRECTION > 0) {
		block = __ar_malloc_end;
		__ar_malloc_end = (void *) ((size_t) __ar_malloc_end + real_size);
	} else {
		block = __ar_malloc_end = (void *) ((size_t) __ar_malloc_end - real_size);
	}
	done: block->size = real_size;
	return (void *) &block->next;
}
#endif


#ifdef DEFINE_FREE

void ar_free(void *block_p) {
	fle *nextfree;
	fle block = (fle) ((size_t) block_p - offsetof(struct freelist_entry, next));

	if (block_p == NULL)
		return;

	/* Look on the freelist to see if there's a free block just before
	 or just after this block.  */
	for (nextfree = &__ar_malloc_freelist; *nextfree; nextfree =&(*nextfree)->next)
	{
		fle thisblock = *nextfree;
		if ((size_t) thisblock + thisblock->size == (size_t) block)
		{
			thisblock->size += block->size;
			if (MALLOC_DIRECTION > 0 && thisblock->next && (size_t) block + block->size == (size_t) thisblock->next)
			{
				thisblock->size += thisblock->next->size;
				thisblock->next = thisblock->next->next;
			}
			return;
		}
		else if ((size_t) thisblock == (size_t) block + block->size)
		{
			if (MALLOC_DIRECTION < 0 && thisblock->next && (size_t) block == ((size_t) thisblock->next + thisblock->next->size))
			{
				*nextfree = thisblock->next;
				thisblock->next->size += block->size + thisblock->size;
			}
			else
			{
				block->size += thisblock->size;
				block->next = thisblock->next;
				*nextfree = block;
			}
			return;
		}
		else if ((MALLOC_DIRECTION > 0 && (size_t) thisblock > (size_t) block) || (MALLOC_DIRECTION < 0 && (size_t) thisblock < (size_t) block))
		{
			break;
		}
	}

	block->next = *nextfree;
	*nextfree = block;
	return;
}
#endif



#ifdef DEFINE_REALLOC

void * ar_realloc(void *block_p, size_t sz) {
	fle block = (fle) ((size_t) block_p - offsetof(struct freelist_entry, next));
	size_t real_size = REAL_SIZE(sz);
	size_t old_real_size;

	if (block_p == NULL)
		return ar_malloc(sz);

	old_real_size = block->size;

	/* Perhaps we need to allocate more space.  */
	if (old_real_size < real_size) {
		void *result;
		size_t old_size = old_real_size - sizeof(size_t);

		/* Need to allocate, copy, and free.  */
		result = ar_malloc(sz);
		if (result == NULL)
			return NULL;
		memcpy(result, block_p, old_size < sz ? old_size : sz);
		ar_free(block_p);
		return result;
	}
	/* Perhaps we can free some space.  */
	if (old_real_size - real_size >= sizeof(struct freelist_entry)) {
		fle newblock = (fle) ((size_t) block + real_size);
		block->size = real_size;
		newblock->size = old_real_size - real_size;
		ar_free(&newblock->next);
	}
	return block_p;
}
#endif


#ifdef DEFINE_CALLOC
void * ar_calloc(size_t n, size_t elem_size) {
	void *result;
	size_t sz = n * elem_size;
	result = ar_malloc(sz);
	if (result != NULL)
		memset(result, 0, sz);
	return result;
}
#endif

#ifdef DEFINE_CFREE
void ar_cfree(void *p) {
	ar_free(p);
}
#endif

#ifdef DEFINE_MEMALIGN
void * ar_memalign(size_t align, size_t sz) {
	fle *nextfree;
	fle block;

	/* real_size is the size we actually have to allocate, allowing for
	 overhead and alignment.  */
	size_t real_size = REAL_SIZE(sz);

	/* Some sanity checking on 'align'. */
	if ((align & (align - 1)) != 0 || align <= 0)
		return NULL;

	/* Look for the first block on the freelist that is large enough.  */
	/* One tricky part is this: We want the result to be a valid pointer
	 to free.  That means that there has to be room for a size_t
	 before the block.  If there's additional space before the block,
	 it should go on the freelist, or it'll be lost---we could add it
	 to the size of the block before it in memory, but finding the
	 previous block is expensive.  */
	for (nextfree = &__ar_malloc_freelist;; nextfree = &(*nextfree)->next) {
		size_t before_size;
		size_t old_size;

		/* If we've run out of free blocks, allocate more space.  */
		if (!*nextfree) {
			old_size = real_size;
			if (MALLOC_DIRECTION < 0) {
				old_size += M_ALIGN_SUB(
						((size_t )__ar_malloc_end - old_size + sizeof(size_t)),
						align);
				if (!CAN_ALLOC_P(old_size))
					return NULL;
				block = __ar_malloc_end = (void *) ((size_t) __ar_malloc_end
						- old_size);
			} else {
				block = __ar_malloc_end;
				old_size += M_ALIGN((size_t )__ar_malloc_end + sizeof(size_t),
						align);
				if (!CAN_ALLOC_P(old_size))
					return NULL;
				__ar_malloc_end = (void *) ((size_t) __ar_malloc_end + old_size);
			}
			*nextfree = block;
			block->size = old_size;
			block->next = NULL;
		} else {
			block = *nextfree;
			old_size = block->size;
		}

		before_size = M_ALIGN(&block->next, align);
		if (before_size != 0)
			before_size = sizeof(*block) + M_ALIGN(&(block + 1)->next, align);

		/* If this is the last block on the freelist, and it is too small,
		 enlarge it.  */
		if (!block->next && old_size < real_size + before_size
				&& __ar_malloc_end == (void *) ((size_t) block + block->size)) {
			if (MALLOC_DIRECTION < 0) {
				size_t moresize = real_size - block->size;
				moresize += M_ALIGN_SUB((size_t )&block->next - moresize,
						align);
				if (!CAN_ALLOC_P(moresize))
					return NULL;
				block = __ar_malloc_end = (void *) ((size_t) block - moresize);
				block->next = NULL;
				block->size = old_size = old_size + moresize;
				before_size = 0;
			} else {
				if (!CAN_ALLOC_P(before_size + real_size - block->size))
					return NULL;
				__ar_malloc_end = (void *) ((size_t) block + before_size
						+ real_size);
				block->size = old_size = before_size + real_size;
			}

			/* Two out of the four cases below will now be possible; which
			 two depends on MALLOC_DIRECTION.  */
		}

		if (old_size >= real_size + before_size) {
			/* This block will do.  If there needs to be space before it,
			 split the block.  */
			if (before_size != 0) {
				fle old_block = block;

				old_block->size = before_size;
				block = (fle) ((size_t) block + before_size);

				/* If there's no space after the block, we're now nearly
				 done; just make a note of the size required.
				 Otherwise, we need to create a new free space block.  */
				if (old_size - before_size
						<= real_size + sizeof(struct freelist_entry)) {
					block->size = old_size - before_size;
					return (void *) &block->next;
				} else {
					fle new_block;
					new_block = (fle) ((size_t) block + real_size);
					new_block->size = old_size - before_size - real_size;
					if (MALLOC_DIRECTION > 0) {
						new_block->next = old_block->next;
						old_block->next = new_block;
					} else {
						new_block->next = old_block;
						*nextfree = new_block;
					}
					goto done;
				}
			} else {
				/* If the block found is just the right size, remove it from
				 the free list.  Otherwise, split it.  */
				if (old_size <= real_size + sizeof(struct freelist_entry)) {
					*nextfree = block->next;
					return (void *) &block->next;
				} else {
					size_t newsize = old_size - real_size;
					fle newnext = block->next;
					*nextfree = (fle) ((size_t) block + real_size);
					(*nextfree)->size = newsize;
					(*nextfree)->next = newnext;
					goto done;
				}
			}
		}
	}

	done: block->size = real_size;
	return (void *) &block->next;
}
#endif


#ifdef DEFINE_VALLOC

void * ar_valloc (size_t sz)
{
  return ar_memalign (128, sz);
}
#endif

#ifdef DEFINE_PVALLOC

void * ar_pvalloc (size_t sz)
{
  return ar_memalign (128, sz + M_ALIGN (sz, 128));
}
#endif


#ifdef DEFINE_MALLINFO

struct mallinfo mallinfo (void)
{
  struct mallinfo r;
  fle fr;
  size_t free_size;
  size_t total_size;
  size_t free_blocks;


  memset (&r, 0, sizeof (r));


  free_size = 0;
  free_blocks = 0;
  for (fr = __ar_malloc_freelist; fr; fr = fr->next)
    {
      free_size += fr->size;
      free_blocks++;
      if (! fr->next)
	{
	  int atend;
	  if (MALLOC_DIRECTION > 0)
	    atend = (void *)((size_t)fr + fr->size) == __ar_malloc_end;
	  else
	    atend = (void *)fr == __ar_malloc_end;
	  if (atend)
	    r.keepcost = fr->size;
	}
    }


  if (MALLOC_DIRECTION > 0)
    total_size = (char *)__ar_malloc_end - (char *)&__ar_malloc_start;
  else
    total_size = (char *)&__ar_malloc_start - (char *)__ar_malloc_end;

#ifdef DEBUG
  /* Fixme: should walk through all the in-use blocks and see if
     they're valid.  */
#endif


  r.arena = total_size;
  r.fordblks = free_size;
  r.uordblks = total_size - free_size;
  r.ordblks = free_blocks;
  return r;
}
#endif


#ifdef DEFINE_MALLOC_STATS

void malloc_stats(void)
{
  struct mallinfo i;
  FILE *fp;

  fp = stderr;
  i = mallinfo();
  fprintf (fp, "malloc has reserved %u bytes between %p and %p\n",
	   i.arena, &__ar_malloc_start, __ar_malloc_end);
  fprintf (fp, "there are %u bytes free in %u chunks\n",
	   i.fordblks, i.ordblks);
  fprintf (fp, "of which %u bytes are at the end of the reserved space\n",
	   i.keepcost);
  fprintf (fp, "and %u bytes are in use.\n", i.uordblks);
}
#endif


#ifdef DEFINE_MALLOC_USABLE_SIZE

size_t malloc_usable_size (void *block_p)
{
  fle block = (fle)((size_t) block_p - offsetof (struct freelist_entry, next));
  return block->size - sizeof (size_t);
}
#endif


#ifdef DEFINE_MALLOPT
int mallopt (int n, int v)
{
  (void)n; (void)v;
  return 0;
}
#endif






