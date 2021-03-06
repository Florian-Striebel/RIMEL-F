

























#if !defined(_DT_XLATOR_H)
#define _DT_XLATOR_H

#pragma ident "%Z%%M% %I% %E% SMI"

#include <libctf.h>
#include <dtrace.h>
#include <dt_ident.h>
#include <dt_list.h>

#if defined(__cplusplus)
extern "C" {
#endif

struct dt_node;

typedef struct dt_xlator {
dt_list_t dx_list;
dt_idhash_t *dx_locals;
dt_ident_t *dx_ident;
dt_ident_t dx_souid;
dt_ident_t dx_ptrid;
ctf_file_t *dx_src_ctfp;
ctf_id_t dx_src_type;
ctf_id_t dx_src_base;
ctf_file_t *dx_dst_ctfp;
ctf_id_t dx_dst_type;
ctf_id_t dx_dst_base;
struct dt_node *dx_members;
uint_t dx_nmembers;
dtrace_difo_t **dx_membdif;
struct dt_node *dx_nodes;
dtrace_hdl_t *dx_hdl;
ulong_t dx_gen;
id_t dx_id;
int dx_arg;
} dt_xlator_t;

extern dt_xlator_t *dt_xlator_create(dtrace_hdl_t *,
const dtrace_typeinfo_t *, const dtrace_typeinfo_t *,
const char *, struct dt_node *, struct dt_node *);

extern void dt_xlator_destroy(dtrace_hdl_t *, dt_xlator_t *);

#define DT_XLATE_FUZZY 0x0
#define DT_XLATE_EXACT 0x1
#define DT_XLATE_EXTERN 0x2

extern dt_xlator_t *dt_xlator_lookup(dtrace_hdl_t *,
struct dt_node *, struct dt_node *, int);

extern dt_xlator_t *dt_xlator_lookup_id(dtrace_hdl_t *, id_t);
extern dt_ident_t *dt_xlator_ident(dt_xlator_t *, ctf_file_t *, ctf_id_t);
extern struct dt_node *dt_xlator_member(dt_xlator_t *, const char *);
extern int dt_xlator_dynamic(const dt_xlator_t *);

#if defined(__cplusplus)
}
#endif

#endif
