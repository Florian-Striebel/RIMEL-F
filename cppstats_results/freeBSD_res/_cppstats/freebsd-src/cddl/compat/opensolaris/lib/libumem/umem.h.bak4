



























#if !defined(_UMEM_H)
#define _UMEM_H



#include <sys/types.h>
#include <stdlib.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define UMEM_DEFAULT 0x0000
#define UMEM_NOFAIL 0x0100

#define UMEM_FLAGS 0xffff

extern void *umem_alloc(size_t, int);
extern void *umem_alloc_align(size_t, size_t, int);
extern void *umem_zalloc(size_t, int);
extern void umem_free(void *, size_t);
extern void umem_free_align(void *, size_t);




#define UMC_NOTOUCH 0x00010000
#define UMC_NODEBUG 0x00020000
#define UMC_NOMAGAZINE 0x00040000
#define UMC_NOHASH 0x00080000

struct umem_cache;

typedef struct umem_cache umem_cache_t;
typedef int umem_constructor_t(void *, void *, int);
typedef void umem_destructor_t(void *, void *);
typedef void umem_reclaim_t(void *);

typedef int umem_nofail_callback_t(void);
#define UMEM_CALLBACK_RETRY 0
#define UMEM_CALLBACK_EXIT(status) (0x100 | ((status) & 0xFF))

extern void umem_nofail_callback(umem_nofail_callback_t *);

extern umem_cache_t *umem_cache_create(char *, size_t,
size_t, umem_constructor_t *, umem_destructor_t *, umem_reclaim_t *,
void *, void *, int);
extern void umem_cache_destroy(umem_cache_t *);

extern void *umem_cache_alloc(umem_cache_t *, int);
extern void umem_cache_free(umem_cache_t *, void *);

extern void umem_reap(void);

#if defined(__cplusplus)
}
#endif

#endif
