#pragma ident "%Z%%M% %I% %E% SMI"
#include <dtrace.h>
#if defined(__cplusplus)
extern "C" {
#endif
typedef struct dt_inthash {
struct dt_inthash *inh_hash;
struct dt_inthash *inh_next;
uint64_t inh_value;
uint_t inh_index;
uint_t inh_flags;
} dt_inthash_t;
typedef struct dt_inttab {
dtrace_hdl_t *int_hdl;
dt_inthash_t **int_hash;
uint_t int_hashlen;
uint_t int_nelems;
dt_inthash_t *int_head;
dt_inthash_t *int_tail;
uint_t int_index;
} dt_inttab_t;
#define DT_INT_PRIVATE 0
#define DT_INT_SHARED 1
extern dt_inttab_t *dt_inttab_create(dtrace_hdl_t *);
extern void dt_inttab_destroy(dt_inttab_t *);
extern int dt_inttab_insert(dt_inttab_t *, uint64_t, uint_t);
extern uint_t dt_inttab_size(const dt_inttab_t *);
extern void dt_inttab_write(const dt_inttab_t *, uint64_t *);
#if defined(__cplusplus)
}
#endif
