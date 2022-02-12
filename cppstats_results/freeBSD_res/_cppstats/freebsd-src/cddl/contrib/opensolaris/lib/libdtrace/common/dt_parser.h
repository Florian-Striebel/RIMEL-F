#include <sys/types.h>
#include <sys/dtrace.h>
#include <libctf.h>
#include <stdarg.h>
#include <stdio.h>
#if defined(__cplusplus)
extern "C" {
#endif
#include <dt_errtags.h>
#include <dt_ident.h>
#include <dt_decl.h>
#include <dt_xlator.h>
#include <dt_list.h>
typedef struct dt_node {
ctf_file_t *dn_ctfp;
ctf_id_t dn_type;
uchar_t dn_kind;
uchar_t dn_flags;
ushort_t dn_op;
int dn_line;
int dn_reg;
dtrace_attribute_t dn_attr;
union {
struct {
uintmax_t _value;
char *_string;
} _const;
struct {
dt_ident_t *_ident;
struct dt_node *_links[3];
} _nodes;
struct {
struct dt_node *_descs;
struct dt_node *_pred;
struct dt_node *_acts;
dt_idhash_t *_locals;
dtrace_attribute_t _attr;
} _clause;
struct {
char *_spec;
dtrace_probedesc_t *_desc;
} _pdesc;
struct {
char *_name;
struct dt_node *_expr;
dt_xlator_t *_xlator;
uint_t _id;
} _member;
struct {
dt_xlator_t *_xlator;
struct dt_node *_xmemb;
struct dt_node *_membs;
} _xlator;
struct {
char *_name;
struct dt_provider *_pvp;
struct dt_node *_probes;
int _redecl;
} _provider;
struct {
struct dt_node *_conditional;
struct dt_node *_body;
struct dt_node *_alternate_body;
} _conditional;
} dn_u;
struct dt_node *dn_list;
struct dt_node *dn_link;
} dt_node_t;
#define dn_value dn_u._const._value
#define dn_string dn_u._const._string
#define dn_ident dn_u._nodes._ident
#define dn_args dn_u._nodes._links[0]
#define dn_child dn_u._nodes._links[0]
#define dn_left dn_u._nodes._links[0]
#define dn_right dn_u._nodes._links[1]
#define dn_expr dn_u._nodes._links[2]
#define dn_aggfun dn_u._nodes._links[0]
#define dn_aggtup dn_u._nodes._links[1]
#define dn_pdescs dn_u._clause._descs
#define dn_pred dn_u._clause._pred
#define dn_acts dn_u._clause._acts
#define dn_locals dn_u._clause._locals
#define dn_ctxattr dn_u._clause._attr
#define dn_spec dn_u._pdesc._spec
#define dn_desc dn_u._pdesc._desc
#define dn_membname dn_u._member._name
#define dn_membexpr dn_u._member._expr
#define dn_membxlator dn_u._member._xlator
#define dn_membid dn_u._member._id
#define dn_xlator dn_u._xlator._xlator
#define dn_xmember dn_u._xlator._xmemb
#define dn_members dn_u._xlator._membs
#define dn_provname dn_u._provider._name
#define dn_provider dn_u._provider._pvp
#define dn_provred dn_u._provider._redecl
#define dn_probes dn_u._provider._probes
#define dn_conditional dn_u._conditional._conditional
#define dn_body dn_u._conditional._body
#define dn_alternate_body dn_u._conditional._alternate_body
#define DT_NODE_FREE 0
#define DT_NODE_INT 1
#define DT_NODE_STRING 2
#define DT_NODE_IDENT 3
#define DT_NODE_VAR 4
#define DT_NODE_SYM 5
#define DT_NODE_TYPE 6
#define DT_NODE_FUNC 7
#define DT_NODE_OP1 8
#define DT_NODE_OP2 9
#define DT_NODE_OP3 10
#define DT_NODE_DEXPR 11
#define DT_NODE_DFUNC 12
#define DT_NODE_AGG 13
#define DT_NODE_PDESC 14
#define DT_NODE_CLAUSE 15
#define DT_NODE_INLINE 16
#define DT_NODE_MEMBER 17
#define DT_NODE_XLATOR 18
#define DT_NODE_PROBE 19
#define DT_NODE_PROVIDER 20
#define DT_NODE_PROG 21
#define DT_NODE_IF 22
#define DT_NF_SIGNED 0x01
#define DT_NF_COOKED 0x02
#define DT_NF_REF 0x04
#define DT_NF_LVALUE 0x08
#define DT_NF_WRITABLE 0x10
#define DT_NF_BITFIELD 0x20
#define DT_NF_USERLAND 0x40
#define DT_TYPE_NAMELEN 128
extern int dt_node_is_integer(const dt_node_t *);
extern int dt_node_is_float(const dt_node_t *);
extern int dt_node_is_scalar(const dt_node_t *);
extern int dt_node_is_arith(const dt_node_t *);
extern int dt_node_is_vfptr(const dt_node_t *);
extern int dt_node_is_dynamic(const dt_node_t *);
extern int dt_node_is_stack(const dt_node_t *);
extern int dt_node_is_symaddr(const dt_node_t *);
extern int dt_node_is_usymaddr(const dt_node_t *);
extern int dt_node_is_string(const dt_node_t *);
extern int dt_node_is_strcompat(const dt_node_t *);
extern int dt_node_is_pointer(const dt_node_t *);
extern int dt_node_is_void(const dt_node_t *);
extern int dt_node_is_ptrcompat(const dt_node_t *, const dt_node_t *,
ctf_file_t **, ctf_id_t *);
extern int dt_node_is_argcompat(const dt_node_t *, const dt_node_t *);
extern int dt_node_is_posconst(const dt_node_t *);
extern int dt_node_is_actfunc(const dt_node_t *);
extern dt_node_t *dt_node_int(uintmax_t);
extern dt_node_t *dt_node_string(char *);
extern dt_node_t *dt_node_ident(char *);
extern dt_node_t *dt_node_type(dt_decl_t *);
extern dt_node_t *dt_node_vatype(void);
extern dt_node_t *dt_node_decl(void);
extern dt_node_t *dt_node_func(dt_node_t *, dt_node_t *);
extern dt_node_t *dt_node_offsetof(dt_decl_t *, char *);
extern dt_node_t *dt_node_op1(int, dt_node_t *);
extern dt_node_t *dt_node_op2(int, dt_node_t *, dt_node_t *);
extern dt_node_t *dt_node_op3(dt_node_t *, dt_node_t *, dt_node_t *);
extern dt_node_t *dt_node_statement(dt_node_t *);
extern dt_node_t *dt_node_pdesc_by_name(char *);
extern dt_node_t *dt_node_pdesc_by_id(uintmax_t);
extern dt_node_t *dt_node_clause(dt_node_t *, dt_node_t *, dt_node_t *);
extern dt_node_t *dt_node_inline(dt_node_t *);
extern dt_node_t *dt_node_member(dt_decl_t *, char *, dt_node_t *);
extern dt_node_t *dt_node_xlator(dt_decl_t *, dt_decl_t *, char *, dt_node_t *);
extern dt_node_t *dt_node_probe(char *, int, dt_node_t *, dt_node_t *);
extern dt_node_t *dt_node_provider(char *, dt_node_t *);
extern dt_node_t *dt_node_program(dt_node_t *);
extern dt_node_t *dt_node_if(dt_node_t *, dt_node_t *, dt_node_t *);
extern dt_node_t *dt_node_link(dt_node_t *, dt_node_t *);
extern dt_node_t *dt_node_cook(dt_node_t *, uint_t);
extern dt_node_t *dt_node_xalloc(dtrace_hdl_t *, int);
extern void dt_node_free(dt_node_t *);
extern dtrace_attribute_t dt_node_list_cook(dt_node_t **, uint_t);
extern void dt_node_list_free(dt_node_t **);
extern void dt_node_link_free(dt_node_t **);
extern void dt_node_attr_assign(dt_node_t *, dtrace_attribute_t);
extern void dt_node_type_assign(dt_node_t *, ctf_file_t *, ctf_id_t, boolean_t);
extern void dt_node_type_propagate(const dt_node_t *, dt_node_t *);
extern const char *dt_node_type_name(const dt_node_t *, char *, size_t);
extern size_t dt_node_type_size(const dt_node_t *);
extern dt_ident_t *dt_node_resolve(const dt_node_t *, uint_t);
extern size_t dt_node_sizeof(const dt_node_t *);
extern void dt_node_promote(dt_node_t *, dt_node_t *, dt_node_t *);
extern void dt_node_diftype(dtrace_hdl_t *,
const dt_node_t *, dtrace_diftype_t *);
extern void dt_node_printr(dt_node_t *, FILE *, int);
extern void dt_printd(dt_node_t *, FILE *, int);
extern const char *dt_node_name(const dt_node_t *, char *, size_t);
extern int dt_node_root(dt_node_t *);
struct dtrace_typeinfo;
struct dt_pcb;
#define IS_CHAR(e) (((e).cte_format & (CTF_INT_CHAR | CTF_INT_SIGNED)) == (CTF_INT_CHAR | CTF_INT_SIGNED) && (e).cte_bits == NBBY)
#define IS_VOID(e) ((e).cte_offset == 0 && (e).cte_bits == 0)
extern int dt_type_lookup(const char *, struct dtrace_typeinfo *);
extern int dt_type_pointer(struct dtrace_typeinfo *);
extern const char *dt_type_name(ctf_file_t *, ctf_id_t, char *, size_t);
typedef enum {
YYS_CLAUSE,
YYS_DEFINE,
YYS_EXPR,
YYS_DONE,
YYS_CONTROL
} yystate_t;
extern void dnerror(const dt_node_t *, dt_errtag_t, const char *, ...);
extern void dnwarn(const dt_node_t *, dt_errtag_t, const char *, ...);
extern void xyerror(dt_errtag_t, const char *, ...);
extern void xywarn(dt_errtag_t, const char *, ...);
extern void xyvwarn(dt_errtag_t, const char *, va_list);
extern void yyerror(const char *, ...);
extern void yywarn(const char *, ...);
extern void yyvwarn(const char *, va_list);
extern void yylabel(const char *);
extern void yybegin(yystate_t);
extern void yyinit(struct dt_pcb *);
extern int yyparse(void);
extern int yyinput(void);
#if defined(__cplusplus)
}
#endif
