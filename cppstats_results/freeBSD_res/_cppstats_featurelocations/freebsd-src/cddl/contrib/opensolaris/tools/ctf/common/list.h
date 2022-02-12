

























#if !defined(_LIST_H)
#define _LIST_H

#pragma ident "%Z%%M% %I% %E% SMI"





#if defined(__cplusplus)
extern "C" {
#endif

typedef struct list list_t;

void list_add(list_t **, void *);
void slist_add(list_t **, void *, int (*)(void *, void *));
void *list_remove(list_t **, void *, int (*)(void *, void *, void *), void *);
void list_free(list_t *, void (*)(void *, void *), void *);
void *list_find(list_t *, void *, int (*)(void *, void *));
void *list_first(list_t *);
int list_iter(list_t *, int (*)(void *, void *), void *);
int list_count(list_t *);
int list_empty(list_t *);
void list_concat(list_t **, list_t *);
void slist_merge(list_t **, list_t *, int (*)(void *, void *));

#if defined(__cplusplus)
}
#endif

#endif
