

























#if !defined(_ASSOC_H)
#define _ASSOC_H

#pragma ident "%Z%%M% %I% %E% SMI"






#if defined(__cplusplus)
extern "C" {
#endif

typedef struct alist alist_t;

alist_t *alist_new(void (*)(void *), void (*)(void *));
alist_t *alist_xnew(int, void (*)(void *), void (*)(void *),
int (*)(int, void *), int (*)(void *, void *));
void alist_free(alist_t *);
void alist_add(alist_t *, void *, void *);
int alist_find(alist_t *, void *, void **);
int alist_iter(alist_t *, int (*)(void *, void *, void *), void *);
void alist_stats(alist_t *, int);
int alist_dump(alist_t *, int (*)(void *, void *));

#if defined(__cplusplus)
}
#endif

#endif
