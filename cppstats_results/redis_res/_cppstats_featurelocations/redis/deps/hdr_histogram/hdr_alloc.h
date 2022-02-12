









#if !defined(HDR_ALLOC_H)
#define HDR_ALLOC_H

#include <stddef.h>
#include <stdint.h>


typedef struct hdrAllocFuncs {
void *(*mallocFn)(size_t);
void *(*callocFn)(size_t, size_t);
void *(*reallocFn)(void *, size_t);
void (*freeFn)(void *);
} hdrAllocFuncs;


extern hdrAllocFuncs hdrAllocFns;

hdrAllocFuncs hdrSetAllocators(hdrAllocFuncs *ha);
void hdrResetAllocators(void);

static inline void *hdr_malloc(size_t size) {
return hdrAllocFns.mallocFn(size);
}

static inline void *hdr_calloc(size_t nmemb, size_t size) {
return hdrAllocFns.callocFn(nmemb, size);
}

static inline void *hdr_realloc(void *ptr, size_t size) {
return hdrAllocFns.reallocFn(ptr, size);
}

static inline void hdr_free(void *ptr) {
hdrAllocFns.freeFn(ptr);
}

#endif
