#include "fmacros.h"
#include "alloc.h"
#include <string.h>
#include <stdlib.h>
hiredisAllocFuncs hiredisAllocFns = {
.mallocFn = malloc,
.callocFn = calloc,
.reallocFn = realloc,
.strdupFn = strdup,
.freeFn = free,
};
hiredisAllocFuncs hiredisSetAllocators(hiredisAllocFuncs *override) {
hiredisAllocFuncs orig = hiredisAllocFns;
hiredisAllocFns = *override;
return orig;
}
void hiredisResetAllocators(void) {
hiredisAllocFns = (hiredisAllocFuncs) {
.mallocFn = malloc,
.callocFn = calloc,
.reallocFn = realloc,
.strdupFn = strdup,
.freeFn = free,
};
}
#if defined(_WIN32)
void *hi_malloc(size_t size) {
return hiredisAllocFns.mallocFn(size);
}
void *hi_calloc(size_t nmemb, size_t size) {
if (SIZE_MAX / size < nmemb)
return NULL;
return hiredisAllocFns.callocFn(nmemb, size);
}
void *hi_realloc(void *ptr, size_t size) {
return hiredisAllocFns.reallocFn(ptr, size);
}
char *hi_strdup(const char *str) {
return hiredisAllocFns.strdupFn(str);
}
void hi_free(void *ptr) {
hiredisAllocFns.freeFn(ptr);
}
#endif
