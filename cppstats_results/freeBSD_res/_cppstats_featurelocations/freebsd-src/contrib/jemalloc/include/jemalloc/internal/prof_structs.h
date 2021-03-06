#if !defined(JEMALLOC_INTERNAL_PROF_STRUCTS_H)
#define JEMALLOC_INTERNAL_PROF_STRUCTS_H

#include "jemalloc/internal/ckh.h"
#include "jemalloc/internal/mutex.h"
#include "jemalloc/internal/prng.h"
#include "jemalloc/internal/rb.h"

struct prof_bt_s {

void **vec;
unsigned len;
};

#if defined(JEMALLOC_PROF_LIBGCC)

typedef struct {
prof_bt_t *bt;
unsigned max;
} prof_unwind_data_t;
#endif

struct prof_accum_s {
#if !defined(JEMALLOC_ATOMIC_U64)
malloc_mutex_t mtx;
uint64_t accumbytes;
#else
atomic_u64_t accumbytes;
#endif
};

struct prof_cnt_s {

uint64_t curobjs;
uint64_t curbytes;
uint64_t accumobjs;
uint64_t accumbytes;
};

typedef enum {
prof_tctx_state_initializing,
prof_tctx_state_nominal,
prof_tctx_state_dumping,
prof_tctx_state_purgatory
} prof_tctx_state_t;

struct prof_tctx_s {

prof_tdata_t *tdata;





uint64_t thr_uid;
uint64_t thr_discrim;


prof_cnt_t cnts;


prof_gctx_t *gctx;














uint64_t tctx_uid;


rb_node(prof_tctx_t) tctx_link;





bool prepared;


prof_tctx_state_t state;





prof_cnt_t dump_cnts;
};
typedef rb_tree(prof_tctx_t) prof_tctx_tree_t;

struct prof_gctx_s {

malloc_mutex_t *lock;











unsigned nlimbo;





prof_tctx_tree_t tctxs;


rb_node(prof_gctx_t) dump_link;


prof_cnt_t cnt_summed;


prof_bt_t bt;


void *vec[1];
};
typedef rb_tree(prof_gctx_t) prof_gctx_tree_t;

struct prof_tdata_s {
malloc_mutex_t *lock;


uint64_t thr_uid;





uint64_t thr_discrim;


char *thread_name;

bool attached;
bool expired;

rb_node(prof_tdata_t) tdata_link;






uint64_t tctx_uid_next;







ckh_t bt2tctx;


uint64_t prng_state;


bool enq;
bool enq_idump;
bool enq_gdump;







bool dumping;





bool active;


prof_cnt_t cnt_summed;


void *vec[PROF_BT_MAX];
};
typedef rb_tree(prof_tdata_t) prof_tdata_tree_t;

#endif
