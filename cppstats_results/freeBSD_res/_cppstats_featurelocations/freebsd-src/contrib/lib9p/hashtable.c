


























#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/queue.h>
#include "lib9p_impl.h"
#include "hashtable.h"

static struct ht_item *ht_iter_advance(struct ht_iter *, struct ht_item *);

void
ht_init(struct ht *h, ssize_t size)
{
ssize_t i;

memset(h, 0, sizeof(struct ht));
h->ht_nentries = size;
h->ht_entries = l9p_calloc((size_t)size, sizeof(struct ht_entry));
pthread_rwlock_init(&h->ht_rwlock, NULL);

for (i = 0; i < size; i++)
TAILQ_INIT(&h->ht_entries[i].hte_items);
}

void
ht_destroy(struct ht *h)
{
struct ht_entry *he;
struct ht_item *item, *tmp;
ssize_t i;

for (i = 0; i < h->ht_nentries; i++) {
he = &h->ht_entries[i];
TAILQ_FOREACH_SAFE(item, &he->hte_items, hti_link, tmp) {
free(item);
}
}

pthread_rwlock_destroy(&h->ht_rwlock);
free(h->ht_entries);
h->ht_entries = NULL;
}

void *
ht_find(struct ht *h, uint32_t hash)
{
void *result;

ht_rdlock(h);
result = ht_find_locked(h, hash);
ht_unlock(h);
return (result);
}

void *
ht_find_locked(struct ht *h, uint32_t hash)
{
struct ht_entry *entry;
struct ht_item *item;

entry = &h->ht_entries[hash % h->ht_nentries];

TAILQ_FOREACH(item, &entry->hte_items, hti_link) {
if (item->hti_hash == hash)
return (item->hti_data);
}

return (NULL);
}

int
ht_add(struct ht *h, uint32_t hash, void *value)
{
struct ht_entry *entry;
struct ht_item *item;

ht_wrlock(h);
entry = &h->ht_entries[hash % h->ht_nentries];

TAILQ_FOREACH(item, &entry->hte_items, hti_link) {
if (item->hti_hash == hash) {
errno = EEXIST;
ht_unlock(h);
return (-1);
}
}

item = l9p_calloc(1, sizeof(struct ht_item));
item->hti_hash = hash;
item->hti_data = value;
TAILQ_INSERT_TAIL(&entry->hte_items, item, hti_link);
ht_unlock(h);

return (0);
}

int
ht_remove(struct ht *h, uint32_t hash)
{
int result;

ht_wrlock(h);
result = ht_remove_locked(h, hash);
ht_unlock(h);
return (result);
}

int
ht_remove_locked(struct ht *h, uint32_t hash)
{
struct ht_entry *entry;
struct ht_item *item, *tmp;
ssize_t slot = hash % h->ht_nentries;

entry = &h->ht_entries[slot];

TAILQ_FOREACH_SAFE(item, &entry->hte_items, hti_link, tmp) {
if (item->hti_hash == hash) {
TAILQ_REMOVE(&entry->hte_items, item, hti_link);
free(item);
return (0);
}
}

errno = ENOENT;
return (-1);
}














static struct ht_item *
ht_iter_advance(struct ht_iter *iter, struct ht_item *cur)
{
struct ht_item *next;
struct ht *h;
ssize_t slot;

h = iter->htit_parent;

if (cur == NULL)
next = NULL;
else
next = TAILQ_NEXT(cur, hti_link);

if (next == NULL) {
slot = iter->htit_slot;
while (++slot < h->ht_nentries) {
next = TAILQ_FIRST(&h->ht_entries[slot].hte_items);
if (next != NULL)
break;
}
iter->htit_slot = slot;
}
return (next);
}






int
ht_remove_at_iter(struct ht_iter *iter)
{
struct ht_item *item;
struct ht *h;
ssize_t slot;

assert(iter != NULL);

if ((item = iter->htit_curr) == NULL) {
errno = EINVAL;
return (-1);
}


h = iter->htit_parent;
ht_wrlock(h);
slot = iter->htit_slot;
iter->htit_next = ht_iter_advance(iter, item);
TAILQ_REMOVE(&h->ht_entries[slot].hte_items, item, hti_link);
ht_unlock(h);


iter->htit_curr = NULL;
free(item);

return (0);
}







void
ht_iter(struct ht *h, struct ht_iter *iter)
{

iter->htit_parent = h;
iter->htit_curr = NULL;
iter->htit_next = NULL;
iter->htit_slot = -1;
}





void *
ht_next(struct ht_iter *iter)
{
struct ht_item *item;
struct ht *h;

if ((item = iter->htit_next) == NULL) {

h = iter->htit_parent;
ht_rdlock(h);
item = ht_iter_advance(iter, iter->htit_curr);
ht_unlock(h);
} else
iter->htit_next = NULL;
iter->htit_curr = item;
return (item == NULL ? NULL : item->hti_data);
}
