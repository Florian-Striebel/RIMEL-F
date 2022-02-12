#if !defined(JEMALLOC_INTERNAL_PAGES_EXTERNS_H)
#define JEMALLOC_INTERNAL_PAGES_EXTERNS_H


#if defined(PAGE_MASK)
#undef PAGE_MASK
#endif
#define PAGE ((size_t)(1U << LG_PAGE))
#define PAGE_MASK ((size_t)(PAGE - 1))

#define PAGE_ADDR2BASE(a) ((void *)((uintptr_t)(a) & ~PAGE_MASK))


#define PAGE_CEILING(s) (((s) + PAGE_MASK) & ~PAGE_MASK)



#define HUGEPAGE ((size_t)(1U << LG_HUGEPAGE))
#define HUGEPAGE_MASK ((size_t)(HUGEPAGE - 1))

#define HUGEPAGE_ADDR2BASE(a) ((void *)((uintptr_t)(a) & ~HUGEPAGE_MASK))


#define HUGEPAGE_CEILING(s) (((s) + HUGEPAGE_MASK) & ~HUGEPAGE_MASK)



#if defined(_WIN32) || defined(JEMALLOC_PURGE_MADVISE_FREE)
#define PAGES_CAN_PURGE_LAZY
#endif









#if !defined(_WIN32) && ((defined(JEMALLOC_PURGE_MADVISE_DONTNEED) && defined(JEMALLOC_PURGE_MADVISE_DONTNEED_ZEROS)) || defined(JEMALLOC_MAPS_COALESCE))


#define PAGES_CAN_PURGE_FORCED
#endif

static const bool pages_can_purge_lazy =
#if defined(PAGES_CAN_PURGE_LAZY)
true
#else
false
#endif
;
static const bool pages_can_purge_forced =
#if defined(PAGES_CAN_PURGE_FORCED)
true
#else
false
#endif
;

typedef enum {
thp_mode_default = 0,
thp_mode_always = 1,
thp_mode_never = 2,

thp_mode_names_limit = 3,
thp_mode_not_supported = 3
} thp_mode_t;

#define THP_MODE_DEFAULT thp_mode_default
extern thp_mode_t opt_thp;
extern thp_mode_t init_system_thp_mode;
extern const char *thp_mode_names[];

void *pages_map(void *addr, size_t size, size_t alignment, bool *commit);
void pages_unmap(void *addr, size_t size);
bool pages_commit(void *addr, size_t size);
bool pages_decommit(void *addr, size_t size);
bool pages_purge_lazy(void *addr, size_t size);
bool pages_purge_forced(void *addr, size_t size);
bool pages_huge(void *addr, size_t size);
bool pages_nohuge(void *addr, size_t size);
bool pages_dontdump(void *addr, size_t size);
bool pages_dodump(void *addr, size_t size);
bool pages_boot(void);
void pages_set_thp_state (void *ptr, size_t size);

#endif
