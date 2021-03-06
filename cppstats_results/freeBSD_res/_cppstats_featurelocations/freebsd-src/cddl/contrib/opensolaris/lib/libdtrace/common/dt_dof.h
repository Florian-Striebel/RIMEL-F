

























#if !defined(_DT_DOF_H)
#define _DT_DOF_H

#pragma ident "%Z%%M% %I% %E% SMI"

#include <dtrace.h>

#if defined(__cplusplus)
extern "C" {
#endif

#include <dt_buf.h>

typedef struct dt_dof {
dtrace_hdl_t *ddo_hdl;
dtrace_prog_t *ddo_pgp;
uint_t ddo_nsecs;
dof_secidx_t ddo_strsec;
dof_secidx_t *ddo_xlimport;
dof_secidx_t *ddo_xlexport;
dt_buf_t ddo_secs;
dt_buf_t ddo_strs;
dt_buf_t ddo_ldata;
dt_buf_t ddo_udata;
dt_buf_t ddo_probes;
dt_buf_t ddo_args;
dt_buf_t ddo_offs;
dt_buf_t ddo_enoffs;
dt_buf_t ddo_rels;
dt_buf_t ddo_xlms;
} dt_dof_t;

extern void dt_dof_init(dtrace_hdl_t *);
extern void dt_dof_fini(dtrace_hdl_t *);

#if defined(__cplusplus)
}
#endif

#endif
