#if !defined(JEMALLOC_INTERNAL_HOOK_H)
#define JEMALLOC_INTERNAL_HOOK_H

#include "jemalloc/internal/tsd.h"












































#define HOOK_MAX 4

enum hook_alloc_e {
hook_alloc_malloc,
hook_alloc_posix_memalign,
hook_alloc_aligned_alloc,
hook_alloc_calloc,
hook_alloc_memalign,
hook_alloc_valloc,
hook_alloc_mallocx,


hook_alloc_realloc,
hook_alloc_rallocx,
};




typedef enum hook_alloc_e hook_alloc_t;

enum hook_dalloc_e {
hook_dalloc_free,
hook_dalloc_dallocx,
hook_dalloc_sdallocx,





hook_dalloc_realloc,
hook_dalloc_rallocx,
};
typedef enum hook_dalloc_e hook_dalloc_t;


enum hook_expand_e {
hook_expand_realloc,
hook_expand_rallocx,
hook_expand_xallocx,
};
typedef enum hook_expand_e hook_expand_t;

typedef void (*hook_alloc)(
void *extra, hook_alloc_t type, void *result, uintptr_t result_raw,
uintptr_t args_raw[3]);

typedef void (*hook_dalloc)(
void *extra, hook_dalloc_t type, void *address, uintptr_t args_raw[3]);

typedef void (*hook_expand)(
void *extra, hook_expand_t type, void *address, size_t old_usize,
size_t new_usize, uintptr_t result_raw, uintptr_t args_raw[4]);

typedef struct hooks_s hooks_t;
struct hooks_s {
hook_alloc alloc_hook;
hook_dalloc dalloc_hook;
hook_expand expand_hook;
void *extra;
};




















typedef struct hook_ralloc_args_s hook_ralloc_args_t;
struct hook_ralloc_args_s {

bool is_realloc;





uintptr_t args[4];
};





bool hook_boot();

void *hook_install(tsdn_t *tsdn, hooks_t *hooks);

void hook_remove(tsdn_t *tsdn, void *opaque);



void hook_invoke_alloc(hook_alloc_t type, void *result, uintptr_t result_raw,
uintptr_t args_raw[3]);

void hook_invoke_dalloc(hook_dalloc_t type, void *address,
uintptr_t args_raw[3]);

void hook_invoke_expand(hook_expand_t type, void *address, size_t old_usize,
size_t new_usize, uintptr_t result_raw, uintptr_t args_raw[4]);

#endif
