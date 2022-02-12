






























#if !defined(_DT_REGSET_H)
#define _DT_REGSET_H

#include <sys/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct dt_regset {
ulong_t dr_size;
ulong_t *dr_bitmap;
} dt_regset_t;

extern dt_regset_t *dt_regset_create(ulong_t);
extern void dt_regset_destroy(dt_regset_t *);
extern void dt_regset_reset(dt_regset_t *);
extern int dt_regset_alloc(dt_regset_t *);
extern void dt_regset_free(dt_regset_t *, int);
extern void dt_regset_assert_free(dt_regset_t *);

#if defined(__cplusplus)
}
#endif

#endif
