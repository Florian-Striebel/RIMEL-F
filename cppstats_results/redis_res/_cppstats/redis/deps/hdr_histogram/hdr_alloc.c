#include "hdr_alloc.h"
#include <stdlib.h>
hdrAllocFuncs hdrAllocFns = {
.mallocFn = malloc,
.callocFn = calloc,
.reallocFn = realloc,
.freeFn = free,
};
hdrAllocFuncs hdrSetAllocators(hdrAllocFuncs *override) {
hdrAllocFuncs orig = hdrAllocFns;
hdrAllocFns = *override;
return orig;
}
void hdrResetAllocators(void) {
hdrAllocFns = (hdrAllocFuncs){
.mallocFn = malloc,
.callocFn = calloc,
.reallocFn = realloc,
.freeFn = free,
};
}
