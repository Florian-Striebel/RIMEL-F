

























#if !defined(_DT_PROGRAM_H)
#define _DT_PROGRAM_H

#pragma ident "%Z%%M% %I% %E% SMI"

#if defined(__cplusplus)
extern "C" {
#endif

#include <dtrace.h>
#include <dt_list.h>

typedef struct dt_stmt {
dt_list_t ds_list;
dtrace_stmtdesc_t *ds_desc;
} dt_stmt_t;

struct dtrace_prog {
dt_list_t dp_list;
dt_list_t dp_stmts;
ulong_t **dp_xrefs;
uint_t dp_xrefslen;
uint8_t dp_dofversion;
};

extern dtrace_prog_t *dt_program_create(dtrace_hdl_t *);
extern void dt_program_destroy(dtrace_hdl_t *, dtrace_prog_t *);

extern dtrace_ecbdesc_t *dt_ecbdesc_create(dtrace_hdl_t *,
const dtrace_probedesc_t *);
extern void dt_ecbdesc_release(dtrace_hdl_t *, dtrace_ecbdesc_t *);

#if defined(__cplusplus)
}
#endif

#endif
