#include <stddef.h>
#include <stdint.h>
#if defined(__cplusplus)
extern "C" {
#endif
typedef struct hiredisAllocFuncs {
void *(*mallocFn)(size_t);
void *(*callocFn)(size_t,size_t);
void *(*reallocFn)(void*,size_t);
char *(*strdupFn)(const char*);
void (*freeFn)(void*);
} hiredisAllocFuncs;
hiredisAllocFuncs hiredisSetAllocators(hiredisAllocFuncs *ha);
void hiredisResetAllocators(void);
#if !defined(_WIN32)
extern hiredisAllocFuncs hiredisAllocFns;
static inline void *hi_malloc(size_t size) {
return hiredisAllocFns.mallocFn(size);
}
static inline void *hi_calloc(size_t nmemb, size_t size) {
if (SIZE_MAX / size < nmemb)
return NULL;
return hiredisAllocFns.callocFn(nmemb, size);
}
static inline void *hi_realloc(void *ptr, size_t size) {
return hiredisAllocFns.reallocFn(ptr, size);
}
static inline char *hi_strdup(const char *str) {
return hiredisAllocFns.strdupFn(str);
}
static inline void hi_free(void *ptr) {
hiredisAllocFns.freeFn(ptr);
}
#else
void *hi_malloc(size_t size);
void *hi_calloc(size_t nmemb, size_t size);
void *hi_realloc(void *ptr, size_t size);
char *hi_strdup(const char *str);
void hi_free(void *ptr);
#endif
#if defined(__cplusplus)
}
#endif
