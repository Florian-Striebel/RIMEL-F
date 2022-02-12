#include "jemalloc/internal/quantum.h"
typedef unsigned pszind_t;
typedef unsigned szind_t;
typedef int malloc_cpuid_t;
#define MALLOCX_ARENA_BITS 12
#define MALLOCX_TCACHE_BITS 12
#define MALLOCX_LG_ALIGN_BITS 6
#define MALLOCX_ARENA_SHIFT 20
#define MALLOCX_TCACHE_SHIFT 8
#define MALLOCX_ARENA_MASK (((1 << MALLOCX_ARENA_BITS) - 1) << MALLOCX_ARENA_SHIFT)
#define MALLOCX_ARENA_LIMIT ((1 << MALLOCX_ARENA_BITS) - 1)
#define MALLOCX_TCACHE_MASK (((1 << MALLOCX_TCACHE_BITS) - 1) << MALLOCX_TCACHE_SHIFT)
#define MALLOCX_TCACHE_MAX ((1 << MALLOCX_TCACHE_BITS) - 3)
#define MALLOCX_LG_ALIGN_MASK ((1 << MALLOCX_LG_ALIGN_BITS) - 1)
#define MALLOCX_ALIGN_GET_SPECIFIED(flags) (ZU(1) << (flags & MALLOCX_LG_ALIGN_MASK))
#define MALLOCX_ALIGN_GET(flags) (MALLOCX_ALIGN_GET_SPECIFIED(flags) & (SIZE_T_MAX-1))
#define MALLOCX_ZERO_GET(flags) ((bool)(flags & MALLOCX_ZERO))
#define MALLOCX_TCACHE_GET(flags) (((unsigned)((flags & MALLOCX_TCACHE_MASK) >> MALLOCX_TCACHE_SHIFT)) - 2)
#define MALLOCX_ARENA_GET(flags) (((unsigned)(((unsigned)flags) >> MALLOCX_ARENA_SHIFT)) - 1)
#define TINY_MIN (1U << LG_TINY_MIN)
#define LONG ((size_t)(1U << LG_SIZEOF_LONG))
#define LONG_MASK (LONG - 1)
#define LONG_CEILING(a) (((a) + LONG_MASK) & ~LONG_MASK)
#define SIZEOF_PTR (1U << LG_SIZEOF_PTR)
#define PTR_MASK (SIZEOF_PTR - 1)
#define PTR_CEILING(a) (((a) + PTR_MASK) & ~PTR_MASK)
#define LG_CACHELINE 6
#define CACHELINE 64
#define CACHELINE_MASK (CACHELINE - 1)
#define CACHELINE_CEILING(s) (((s) + CACHELINE_MASK) & ~CACHELINE_MASK)
#define ALIGNMENT_ADDR2BASE(a, alignment) ((void *)((uintptr_t)(a) & ((~(alignment)) + 1)))
#define ALIGNMENT_ADDR2OFFSET(a, alignment) ((size_t)((uintptr_t)(a) & (alignment - 1)))
#define ALIGNMENT_CEILING(s, alignment) (((s) + (alignment - 1)) & ((~(alignment)) + 1))
#if __STDC_VERSION__ < 199901L
#if defined(_MSC_VER)
#include <malloc.h>
#define alloca _alloca
#else
#if defined(JEMALLOC_HAS_ALLOCA_H)
#include <alloca.h>
#else
#include <stdlib.h>
#endif
#endif
#define VARIABLE_ARRAY(type, name, count) type *name = alloca(sizeof(type) * (count))
#else
#define VARIABLE_ARRAY(type, name, count) type name[(count)]
#endif
