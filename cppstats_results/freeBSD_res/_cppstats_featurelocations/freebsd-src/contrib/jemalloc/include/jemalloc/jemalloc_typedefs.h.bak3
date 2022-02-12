typedef struct extent_hooks_s extent_hooks_t;






typedef void *(extent_alloc_t)(extent_hooks_t *, void *, size_t, size_t, bool *,
bool *, unsigned);






typedef bool (extent_dalloc_t)(extent_hooks_t *, void *, size_t, bool,
unsigned);






typedef void (extent_destroy_t)(extent_hooks_t *, void *, size_t, bool,
unsigned);






typedef bool (extent_commit_t)(extent_hooks_t *, void *, size_t, size_t, size_t,
unsigned);






typedef bool (extent_decommit_t)(extent_hooks_t *, void *, size_t, size_t,
size_t, unsigned);






typedef bool (extent_purge_t)(extent_hooks_t *, void *, size_t, size_t, size_t,
unsigned);






typedef bool (extent_split_t)(extent_hooks_t *, void *, size_t, size_t, size_t,
bool, unsigned);






typedef bool (extent_merge_t)(extent_hooks_t *, void *, size_t, void *, size_t,
bool, unsigned);

struct extent_hooks_s {
extent_alloc_t *alloc;
extent_dalloc_t *dalloc;
extent_destroy_t *destroy;
extent_commit_t *commit;
extent_decommit_t *decommit;
extent_purge_t *purge_lazy;
extent_purge_t *purge_forced;
extent_split_t *split;
extent_merge_t *merge;
};
