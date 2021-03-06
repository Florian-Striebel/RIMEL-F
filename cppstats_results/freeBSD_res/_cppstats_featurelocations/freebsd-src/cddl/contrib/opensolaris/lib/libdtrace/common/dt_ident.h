

























#if !defined(_DT_IDENT_H)
#define _DT_IDENT_H

#pragma ident "%Z%%M% %I% %E% SMI"

#include <libctf.h>
#include <dtrace.h>

#if defined(__cplusplus)
extern "C" {
#endif

#include <dt_list.h>

struct dt_node;
struct dt_ident;
struct dt_idhash;
struct dt_irlist;
struct dt_regset;

typedef struct dt_idsig {
int dis_varargs;
int dis_optargs;
int dis_argc;
struct dt_node *dis_args;
uint64_t dis_auxinfo;
} dt_idsig_t;

typedef struct dt_idnode {
struct dt_node *din_list;
struct dt_node *din_root;
struct dt_idhash *din_hash;
struct dt_ident **din_argv;
int din_argc;
} dt_idnode_t;

typedef struct dt_idops {
void (*di_cook)(struct dt_node *, struct dt_ident *,
int, struct dt_node *);
void (*di_dtor)(struct dt_ident *);
size_t (*di_size)(struct dt_ident *);
} dt_idops_t;

typedef struct dt_ident {
char *di_name;
ushort_t di_kind;
ushort_t di_flags;
uint_t di_id;
dtrace_attribute_t di_attr;
uint_t di_vers;
const dt_idops_t *di_ops;
void *di_iarg;
void *di_data;
ctf_file_t *di_ctfp;
ctf_id_t di_type;
struct dt_ident *di_next;
ulong_t di_gen;
int di_lineno;
} dt_ident_t;

#define DT_IDENT_ARRAY 0
#define DT_IDENT_SCALAR 1
#define DT_IDENT_PTR 2
#define DT_IDENT_FUNC 3
#define DT_IDENT_AGG 4
#define DT_IDENT_AGGFUNC 5
#define DT_IDENT_ACTFUNC 6
#define DT_IDENT_XLSOU 7
#define DT_IDENT_XLPTR 8
#define DT_IDENT_SYMBOL 9
#define DT_IDENT_ENUM 10
#define DT_IDENT_PRAGAT 11
#define DT_IDENT_PRAGBN 12
#define DT_IDENT_PROBE 13

#define DT_IDFLG_TLS 0x0001
#define DT_IDFLG_LOCAL 0x0002
#define DT_IDFLG_WRITE 0x0004
#define DT_IDFLG_INLINE 0x0008
#define DT_IDFLG_REF 0x0010
#define DT_IDFLG_MOD 0x0020
#define DT_IDFLG_DIFR 0x0040
#define DT_IDFLG_DIFW 0x0080
#define DT_IDFLG_CGREG 0x0100
#define DT_IDFLG_USER 0x0200
#define DT_IDFLG_PRIM 0x0400
#define DT_IDFLG_DECL 0x0800
#define DT_IDFLG_ORPHAN 0x1000

typedef struct dt_idhash {
dt_list_t dh_list;
const char *dh_name;
void (*dh_defer)(struct dt_idhash *, dt_ident_t *);
const dt_ident_t *dh_tmpl;
uint_t dh_nextid;
uint_t dh_minid;
uint_t dh_maxid;
ulong_t dh_nelems;
ulong_t dh_hashsz;
dt_ident_t *dh_hash[1];
} dt_idhash_t;

typedef struct dt_idstack {
dt_list_t dids_list;
} dt_idstack_t;

extern const dt_idops_t dt_idops_assc;
extern const dt_idops_t dt_idops_func;
extern const dt_idops_t dt_idops_args;
extern const dt_idops_t dt_idops_regs;
extern const dt_idops_t dt_idops_type;
extern const dt_idops_t dt_idops_thaw;
extern const dt_idops_t dt_idops_inline;
extern const dt_idops_t dt_idops_probe;

extern dt_idhash_t *dt_idhash_create(const char *, const dt_ident_t *,
uint_t, uint_t);
extern void dt_idhash_destroy(dt_idhash_t *);
extern void dt_idhash_update(dt_idhash_t *);
extern dt_ident_t *dt_idhash_lookup(dt_idhash_t *, const char *);
extern int dt_idhash_nextid(dt_idhash_t *, uint_t *);
extern ulong_t dt_idhash_size(const dt_idhash_t *);
extern const char *dt_idhash_name(const dt_idhash_t *);

extern dt_ident_t *dt_idhash_insert(dt_idhash_t *, const char *, ushort_t,
ushort_t, uint_t, dtrace_attribute_t, uint_t,
const dt_idops_t *, void *, ulong_t);

extern void dt_idhash_xinsert(dt_idhash_t *, dt_ident_t *);
extern void dt_idhash_delete(dt_idhash_t *, dt_ident_t *);

typedef int dt_idhash_f(dt_idhash_t *, dt_ident_t *, void *);
extern int dt_idhash_iter(dt_idhash_t *, dt_idhash_f *, void *);

extern dt_ident_t *dt_idstack_lookup(dt_idstack_t *, const char *);
extern void dt_idstack_push(dt_idstack_t *, dt_idhash_t *);
extern void dt_idstack_pop(dt_idstack_t *, dt_idhash_t *);

extern dt_ident_t *dt_ident_create(const char *, ushort_t, ushort_t, uint_t,
dtrace_attribute_t, uint_t, const dt_idops_t *, void *, ulong_t);
extern void dt_ident_destroy(dt_ident_t *);
extern void dt_ident_morph(dt_ident_t *, ushort_t, const dt_idops_t *, void *);
extern dtrace_attribute_t dt_ident_cook(struct dt_node *,
dt_ident_t *, struct dt_node **);

extern void dt_ident_type_assign(dt_ident_t *, ctf_file_t *, ctf_id_t);
extern dt_ident_t *dt_ident_resolve(dt_ident_t *);
extern size_t dt_ident_size(dt_ident_t *);
extern int dt_ident_unref(const dt_ident_t *);

extern const char *dt_idkind_name(uint_t);

#if defined(__cplusplus)
}
#endif

#endif
