#include <dtrace.h>
#if defined(__cplusplus)
extern "C" {
#endif
typedef uint64_t (*dt_pq_value_f)(void *, void *);
typedef struct dt_pq {
dtrace_hdl_t *dtpq_hdl;
void **dtpq_items;
uint_t dtpq_size;
uint_t dtpq_last;
dt_pq_value_f dtpq_value;
void *dtpq_arg;
} dt_pq_t;
extern dt_pq_t *dt_pq_init(dtrace_hdl_t *, uint_t size, dt_pq_value_f, void *);
extern void dt_pq_fini(dt_pq_t *);
extern void dt_pq_insert(dt_pq_t *, void *);
extern void *dt_pq_pop(dt_pq_t *);
extern void *dt_pq_walk(dt_pq_t *, uint_t *);
#if defined(__cplusplus)
}
#endif
