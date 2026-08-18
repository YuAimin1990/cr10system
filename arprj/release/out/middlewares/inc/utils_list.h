#ifndef AR_LIST_H
#define AR_LIST_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

typedef struct ar_list_head ar_list_t;

struct ar_list_head
{
    struct ar_list_head *next, *prev;
};

static inline void ar_list_init_head(struct ar_list_head *list)
{
    list->next = list->prev = list;
}


static inline void list_add(struct ar_list_head *entry,
                struct ar_list_head *prev, struct ar_list_head *next)
{
    next->prev = entry;
    entry->next = next;
    entry->prev = prev;
    prev->next = entry;
}

static inline void ar_list_add(struct ar_list_head *entry, struct ar_list_head *head)
{
    list_add(entry, head, head->next);
}

static inline void ar_list_add_tail(struct ar_list_head *entry, struct ar_list_head *head)
{
    list_add(entry, head->prev, head);
}


static inline void list_del(struct ar_list_head *prev, struct ar_list_head *next)
{
    next->prev = prev;
    prev->next = next;
}

static inline void ar_list_del(struct ar_list_head *entry)
{
    list_del(entry->prev, entry->next);
}

static inline int ar_list_empty(const struct ar_list_head *head)
{
	return head->next == head;
}

static inline void ar_list_move_tail(struct ar_list_head *list,
				  struct ar_list_head *head)
{
    list_del(list->prev, list->next);
	ar_list_add_tail(list, head);
}

#ifndef ar_container_of
#define ar_container_of(ptr, type, member) \
    (type *)((char *)(ptr) - (char *) &((type *)0)->member)
#endif

/**
 * Alias of container_of
 */
#define ar_list_entry(ptr, type, member) \
   ar_container_of(ptr, type, member)

#define ar_list_first_entry(ptr, type, member) \
	ar_list_entry((ptr)->next, type, member)

#define ar_list_next_entry(pos, member) \
	ar_list_entry((pos)->member.next, typeof(*(pos)), member)

#define ar_list_for_each(pos, head) \
	for ((pos)= (head)->next; (pos)!= (head); (pos) = (pos)->next)

#define ar_list_for_each_entry(pos, head, member)				\
	for (pos = ar_list_entry((head)->next, typeof(*pos), member);	\
	     &pos->member != (head);				 	\
	     pos = ar_list_entry(pos->member.next, typeof(*pos), member))

#define ar_list_for_each_entry_safe(pos, n, head, member)			\
            for (pos = ar_list_first_entry(head, typeof(*pos), member),    \
                n = ar_list_next_entry(pos, member);           \
                 &pos->member != (head);                    \
                 pos = n, n = ar_list_next_entry(n, member))

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
