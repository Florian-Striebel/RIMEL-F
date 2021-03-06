#pragma ident "%Z%%M% %I% %E% SMI"
#include <sys/types.h>
#include <sys/dtrace.h>
#if defined(__cplusplus)
extern "C" {
#endif
typedef struct dt_irnode {
uint_t di_label;
dif_instr_t di_instr;
void *di_extern;
struct dt_irnode *di_next;
} dt_irnode_t;
#define DT_LBL_NONE 0
typedef struct dt_irlist {
dt_irnode_t *dl_list;
dt_irnode_t *dl_last;
uint_t dl_len;
uint_t dl_label;
} dt_irlist_t;
extern void dt_irlist_create(dt_irlist_t *);
extern void dt_irlist_destroy(dt_irlist_t *);
extern void dt_irlist_append(dt_irlist_t *, dt_irnode_t *);
extern uint_t dt_irlist_label(dt_irlist_t *);
#if defined(__cplusplus)
}
#endif
