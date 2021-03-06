#if !defined(JEMALLOC_PREAMBLE_H)
#define JEMALLOC_PREAMBLE_H

#include "jemalloc_internal_defs.h"
#include "jemalloc/internal/jemalloc_internal_decls.h"

#if defined(JEMALLOC_UTRACE)
#include <sys/ktrace.h>
#endif

#include "un-namespace.h"
#include "libc_private.h"

#define JEMALLOC_NO_DEMANGLE
#if defined(JEMALLOC_JET)
#undef JEMALLOC_IS_MALLOC
#define JEMALLOC_N(n) jet_##n
#include "jemalloc/internal/public_namespace.h"
#define JEMALLOC_NO_RENAME
#include "../jemalloc.h"
#undef JEMALLOC_NO_RENAME
#else
#define JEMALLOC_N(n) __je_##n
#include "../jemalloc.h"
#endif

#if defined(JEMALLOC_OSATOMIC)
#include <libkern/OSAtomic.h>
#endif

#if defined(JEMALLOC_ZONE)
#include <mach/mach_error.h>
#include <mach/mach_init.h>
#include <mach/vm_map.h>
#endif

#include "jemalloc/internal/jemalloc_internal_macros.h"






#if !defined(JEMALLOC_NO_PRIVATE_NAMESPACE)
#if !defined(JEMALLOC_JET)
#include "jemalloc/internal/private_namespace.h"
#else
#include "jemalloc/internal/private_namespace_jet.h"
#endif
#endif
#include "jemalloc/internal/test_hooks.h"

#if defined(JEMALLOC_DEFINE_MADVISE_FREE)
#define JEMALLOC_MADV_FREE 8
#endif

static const bool config_debug =
#if defined(JEMALLOC_DEBUG)
true
#else
false
#endif
;
static const bool have_dss =
#if defined(JEMALLOC_DSS)
true
#else
false
#endif
;
static const bool have_madvise_huge =
#if defined(JEMALLOC_HAVE_MADVISE_HUGE)
true
#else
false
#endif
;
static const bool config_fill =
#if defined(JEMALLOC_FILL)
true
#else
false
#endif
;
static const bool config_lazy_lock = true;
static const char * const config_malloc_conf = JEMALLOC_CONFIG_MALLOC_CONF;
static const bool config_prof =
#if defined(JEMALLOC_PROF)
true
#else
false
#endif
;
static const bool config_prof_libgcc =
#if defined(JEMALLOC_PROF_LIBGCC)
true
#else
false
#endif
;
static const bool config_prof_libunwind =
#if defined(JEMALLOC_PROF_LIBUNWIND)
true
#else
false
#endif
;
static const bool maps_coalesce =
#if defined(JEMALLOC_MAPS_COALESCE)
true
#else
false
#endif
;
static const bool config_stats =
#if defined(JEMALLOC_STATS)
true
#else
false
#endif
;
static const bool config_tls =
#if defined(JEMALLOC_TLS)
true
#else
false
#endif
;
static const bool config_utrace =
#if defined(JEMALLOC_UTRACE)
true
#else
false
#endif
;
static const bool config_xmalloc =
#if defined(JEMALLOC_XMALLOC)
true
#else
false
#endif
;
static const bool config_cache_oblivious =
#if defined(JEMALLOC_CACHE_OBLIVIOUS)
true
#else
false
#endif
;




static const bool config_log =
#if defined(JEMALLOC_LOG)
true
#else
false
#endif
;




static const bool config_opt_safety_checks =
#if defined(JEMALLOC_OPT_SAFETY_CHECKS)
true
#elif defined(JEMALLOC_DEBUG)





true
#else
false
#endif
;

#if defined(_WIN32) || defined(JEMALLOC_HAVE_SCHED_GETCPU)

#define JEMALLOC_PERCPU_ARENA
#endif
static const bool have_percpu_arena =
#if defined(JEMALLOC_PERCPU_ARENA)
true
#else
false
#endif
;




static const bool force_ivsalloc =
#if defined(JEMALLOC_FORCE_IVSALLOC)
true
#else
false
#endif
;
static const bool have_background_thread =
#if defined(JEMALLOC_BACKGROUND_THREAD)
true
#else
false
#endif
;

#endif
