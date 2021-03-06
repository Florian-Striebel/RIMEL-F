#pragma ident "%Z%%M% %I% %E% SMI"
#include <dt_impl.h>
#include <dt_ident.h>
#include <dt_list.h>
#if defined(__cplusplus)
extern "C" {
#endif
typedef struct dt_provider {
dt_list_t pv_list;
struct dt_provider *pv_next;
dtrace_providerdesc_t pv_desc;
dt_idhash_t *pv_probes;
dt_node_t *pv_nodes;
ulong_t *pv_xrefs;
ulong_t pv_xrmax;
ulong_t pv_gen;
dtrace_hdl_t *pv_hdl;
uint_t pv_flags;
} dt_provider_t;
#define DT_PROVIDER_INTF 0x1
#define DT_PROVIDER_IMPL 0x2
typedef struct dt_probe_iter {
dtrace_probedesc_t pit_desc;
dtrace_hdl_t *pit_hdl;
dt_provider_t *pit_pvp;
const char *pit_pat;
dtrace_probe_f *pit_func;
void *pit_arg;
uint_t pit_matches;
} dt_probe_iter_t;
typedef struct dt_probe_instance {
char *pi_fname;
char *pi_rname;
uint32_t *pi_offs;
uint32_t *pi_enoffs;
uint_t pi_noffs;
uint_t pi_maxoffs;
uint_t pi_nenoffs;
uint_t pi_maxenoffs;
struct dt_probe_instance *pi_next;
} dt_probe_instance_t;
typedef struct dt_probe {
dt_provider_t *pr_pvp;
dt_ident_t *pr_ident;
const char *pr_name;
dt_node_t *pr_nargs;
dt_node_t **pr_nargv;
uint_t pr_nargc;
dt_node_t *pr_xargs;
dt_node_t **pr_xargv;
uint_t pr_xargc;
uint8_t *pr_mapping;
dt_probe_instance_t *pr_inst;
dtrace_typeinfo_t *pr_argv;
int pr_argc;
} dt_probe_t;
extern dt_provider_t *dt_provider_lookup(dtrace_hdl_t *, const char *);
extern dt_provider_t *dt_provider_create(dtrace_hdl_t *, const char *);
extern void dt_provider_destroy(dtrace_hdl_t *, dt_provider_t *);
extern int dt_provider_xref(dtrace_hdl_t *, dt_provider_t *, id_t);
extern dt_probe_t *dt_probe_create(dtrace_hdl_t *, dt_ident_t *, int,
dt_node_t *, uint_t, dt_node_t *, uint_t);
extern dt_probe_t *dt_probe_info(dtrace_hdl_t *,
const dtrace_probedesc_t *, dtrace_probeinfo_t *);
extern dt_probe_t *dt_probe_lookup(dt_provider_t *, const char *);
extern void dt_probe_declare(dt_provider_t *, dt_probe_t *);
extern void dt_probe_destroy(dt_probe_t *);
extern int dt_probe_define(dt_provider_t *, dt_probe_t *,
const char *, const char *, uint32_t, int);
extern dt_node_t *dt_probe_tag(dt_probe_t *, uint_t, dt_node_t *);
#if defined(__cplusplus)
}
#endif
