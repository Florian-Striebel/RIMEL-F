

























#if !defined(_HASH_H)
#define _HASH_H

#pragma ident "%Z%%M% %I% %E% SMI"





#if defined(__cplusplus)
extern "C" {
#endif

typedef struct hash hash_t;

hash_t *hash_new(int, int (*)(int, void *), int (*)(void *, void *));
void hash_add(hash_t *, void *);
void hash_merge(hash_t *, hash_t *);
void hash_remove(hash_t *, void *);
int hash_find(hash_t *, void *, void **);
int hash_find_iter(hash_t *, void *, int (*)(void *, void *), void *);
int hash_iter(hash_t *, int (*)(void *, void *), void *);
int hash_match(hash_t *, void *, int (*)(void *, void *), void *);
int hash_count(hash_t *);
int hash_name(int, const char *);
void hash_stats(hash_t *, int);
void hash_free(hash_t *, void (*)(void *, void *), void *);

#if defined(__cplusplus)
}
#endif

#endif
