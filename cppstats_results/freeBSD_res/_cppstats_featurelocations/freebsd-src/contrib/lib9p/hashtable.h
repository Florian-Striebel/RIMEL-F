


























#if !defined(LIB9P_HASHTABLE_H)
#define LIB9P_HASHTABLE_H

#include <pthread.h>
#include <sys/queue.h>

struct ht {
struct ht_entry * ht_entries;
ssize_t ht_nentries;
pthread_rwlock_t ht_rwlock;
};

struct ht_entry {
TAILQ_HEAD(, ht_item) hte_items;
};

struct ht_item {
uint32_t hti_hash;
void * hti_data;
TAILQ_ENTRY(ht_item) hti_link;
};

struct ht_iter {
struct ht * htit_parent;
struct ht_item * htit_curr;
struct ht_item * htit_next;
ssize_t htit_slot;
};

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wthread-safety-analysis"
#endif




static inline int
ht_rdlock(struct ht *h)
{

return (pthread_rwlock_rdlock(&h->ht_rwlock));
}




static inline int
ht_wrlock(struct ht *h)
{

return (pthread_rwlock_wrlock(&h->ht_rwlock));
}




static inline int
ht_unlock(struct ht *h)
{

return (pthread_rwlock_unlock(&h->ht_rwlock));
}

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

void ht_init(struct ht *h, ssize_t size);
void ht_destroy(struct ht *h);
void *ht_find(struct ht *h, uint32_t hash);
void *ht_find_locked(struct ht *h, uint32_t hash);
int ht_add(struct ht *h, uint32_t hash, void *value);
int ht_remove(struct ht *h, uint32_t hash);
int ht_remove_locked(struct ht *h, uint32_t hash);
int ht_remove_at_iter(struct ht_iter *iter);
void ht_iter(struct ht *h, struct ht_iter *iter);
void *ht_next(struct ht_iter *iter);

#endif
