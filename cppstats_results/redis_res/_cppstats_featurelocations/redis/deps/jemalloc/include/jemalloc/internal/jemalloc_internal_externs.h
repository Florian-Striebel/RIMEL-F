#if !defined(JEMALLOC_INTERNAL_EXTERNS_H)
#define JEMALLOC_INTERNAL_EXTERNS_H

#include "jemalloc/internal/atomic.h"
#include "jemalloc/internal/tsd_types.h"


extern bool malloc_slow;


extern bool opt_abort;
extern bool opt_abort_conf;
extern bool opt_confirm_conf;
extern const char *opt_junk;
extern bool opt_junk_alloc;
extern bool opt_junk_free;
extern bool opt_utrace;
extern bool opt_xmalloc;
extern bool opt_zero;
extern unsigned opt_narenas;


extern unsigned ncpus;


extern unsigned narenas_auto;


extern unsigned manual_arena_base;





extern atomic_p_t arenas[];

void *a0malloc(size_t size);
void a0dalloc(void *ptr);
void *bootstrap_malloc(size_t size);
void *bootstrap_calloc(size_t num, size_t size);
void bootstrap_free(void *ptr);
void arena_set(unsigned ind, arena_t *arena);
unsigned narenas_total_get(void);
arena_t *arena_init(tsdn_t *tsdn, unsigned ind, extent_hooks_t *extent_hooks);
arena_tdata_t *arena_tdata_get_hard(tsd_t *tsd, unsigned ind);
arena_t *arena_choose_hard(tsd_t *tsd, bool internal);
void arena_migrate(tsd_t *tsd, unsigned oldind, unsigned newind);
void iarena_cleanup(tsd_t *tsd);
void arena_cleanup(tsd_t *tsd);
void arenas_tdata_cleanup(tsd_t *tsd);
void jemalloc_prefork(void);
void jemalloc_postfork_parent(void);
void jemalloc_postfork_child(void);
bool malloc_initialized(void);
void je_sdallocx_noflags(void *ptr, size_t size);

#endif
