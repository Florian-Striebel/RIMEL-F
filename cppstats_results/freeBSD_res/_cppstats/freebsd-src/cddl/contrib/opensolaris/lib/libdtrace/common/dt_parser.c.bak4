



























#pragma ident "%Z%%M% %I% %E% SMI"





































































#include <sys/param.h>
#include <sys/sysmacros.h>
#include <limits.h>
#include <setjmp.h>
#include <strings.h>
#include <assert.h>
#if defined(illumos)
#include <alloca.h>
#endif
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>

#include <dt_impl.h>
#include <dt_grammar.h>
#include <dt_module.h>
#include <dt_provider.h>
#include <dt_string.h>
#include <dt_as.h>

dt_pcb_t *yypcb;
dt_node_t *yypragma;
char yyintprefix;
char yyintsuffix[4];
int yyintdecimal;

static const char *
opstr(int op)
{
switch (op) {
case DT_TOK_COMMA: return (",");
case DT_TOK_ELLIPSIS: return ("...");
case DT_TOK_ASGN: return ("=");
case DT_TOK_ADD_EQ: return ("+=");
case DT_TOK_SUB_EQ: return ("-=");
case DT_TOK_MUL_EQ: return ("*=");
case DT_TOK_DIV_EQ: return ("/=");
case DT_TOK_MOD_EQ: return ("%=");
case DT_TOK_AND_EQ: return ("&=");
case DT_TOK_XOR_EQ: return ("^=");
case DT_TOK_OR_EQ: return ("|=");
case DT_TOK_LSH_EQ: return ("<<=");
case DT_TOK_RSH_EQ: return (">>=");
case DT_TOK_QUESTION: return ("?");
case DT_TOK_COLON: return (":");
case DT_TOK_LOR: return ("||");
case DT_TOK_LXOR: return ("^^");
case DT_TOK_LAND: return ("&&");
case DT_TOK_BOR: return ("|");
case DT_TOK_XOR: return ("^");
case DT_TOK_BAND: return ("&");
case DT_TOK_EQU: return ("==");
case DT_TOK_NEQ: return ("!=");
case DT_TOK_LT: return ("<");
case DT_TOK_LE: return ("<=");
case DT_TOK_GT: return (">");
case DT_TOK_GE: return (">=");
case DT_TOK_LSH: return ("<<");
case DT_TOK_RSH: return (">>");
case DT_TOK_ADD: return ("+");
case DT_TOK_SUB: return ("-");
case DT_TOK_MUL: return ("*");
case DT_TOK_DIV: return ("/");
case DT_TOK_MOD: return ("%");
case DT_TOK_LNEG: return ("!");
case DT_TOK_BNEG: return ("~");
case DT_TOK_ADDADD: return ("++");
case DT_TOK_PREINC: return ("++");
case DT_TOK_POSTINC: return ("++");
case DT_TOK_SUBSUB: return ("--");
case DT_TOK_PREDEC: return ("--");
case DT_TOK_POSTDEC: return ("--");
case DT_TOK_IPOS: return ("+");
case DT_TOK_INEG: return ("-");
case DT_TOK_DEREF: return ("*");
case DT_TOK_ADDROF: return ("&");
case DT_TOK_OFFSETOF: return ("offsetof");
case DT_TOK_SIZEOF: return ("sizeof");
case DT_TOK_STRINGOF: return ("stringof");
case DT_TOK_XLATE: return ("xlate");
case DT_TOK_LPAR: return ("(");
case DT_TOK_RPAR: return (")");
case DT_TOK_LBRAC: return ("[");
case DT_TOK_RBRAC: return ("]");
case DT_TOK_PTR: return ("->");
case DT_TOK_DOT: return (".");
case DT_TOK_STRING: return ("<string>");
case DT_TOK_IDENT: return ("<ident>");
case DT_TOK_TNAME: return ("<type>");
case DT_TOK_INT: return ("<int>");
default: return ("<?>");
}
}

int
dt_type_lookup(const char *s, dtrace_typeinfo_t *tip)
{
static const char delimiters[] = " \t\n\r\v\f*`";
dtrace_hdl_t *dtp = yypcb->pcb_hdl;
const char *p, *q, *r, *end, *obj;

for (p = s, end = s + strlen(s); *p != '\0'; p = q) {
while (isspace(*p))
p++;

if (p == end || (q = strpbrk(p + 1, delimiters)) == NULL)
break;

if (*q == '`') {
char *object = alloca((size_t)(q - p) + 1);
char *type = alloca((size_t)(end - s) + 1);





bcopy(p, object, (size_t)(q - p));
object[(size_t)(q - p)] = '\0';






bcopy(s, type, (size_t)(p - s));
bcopy(q + 1, type + (size_t)(p - s), strlen(q + 1) + 1);









if ((r = strchr(q + 1, '`')) != NULL &&
((r = strchr(r + 1, '`')) != NULL)) {
if (strchr(r + 1, '`') != NULL)
return (dt_set_errno(dtp,
EDT_BADSCOPE));
if (q[1] != 'L' || q[2] != 'M')
return (dt_set_errno(dtp,
EDT_BADSCOPE));
}

return (dtrace_lookup_by_type(dtp, object, type, tip));
}
}

if (yypcb->pcb_idepth != 0)
obj = DTRACE_OBJ_CDEFS;
else
obj = DTRACE_OBJ_EVERY;

return (dtrace_lookup_by_type(dtp, obj, s, tip));
}










int
dt_type_pointer(dtrace_typeinfo_t *tip)
{
dtrace_hdl_t *dtp = yypcb->pcb_hdl;
ctf_file_t *ctfp = tip->dtt_ctfp;
ctf_id_t type = tip->dtt_type;
ctf_id_t base = ctf_type_resolve(ctfp, type);
uint_t bflags = tip->dtt_flags;

dt_module_t *dmp;
ctf_id_t ptr;

if ((ptr = ctf_type_pointer(ctfp, type)) != CTF_ERR ||
(ptr = ctf_type_pointer(ctfp, base)) != CTF_ERR) {
tip->dtt_type = ptr;
return (0);
}

if (yypcb->pcb_idepth != 0)
dmp = dtp->dt_cdefs;
else
dmp = dtp->dt_ddefs;

if (ctfp != dmp->dm_ctfp && ctfp != ctf_parent_file(dmp->dm_ctfp) &&
(type = ctf_add_type(dmp->dm_ctfp, ctfp, type)) == CTF_ERR) {
dtp->dt_ctferr = ctf_errno(dmp->dm_ctfp);
return (dt_set_errno(dtp, EDT_CTF));
}

ptr = ctf_add_pointer(dmp->dm_ctfp, CTF_ADD_ROOT, type);

if (ptr == CTF_ERR || ctf_update(dmp->dm_ctfp) == CTF_ERR) {
dtp->dt_ctferr = ctf_errno(dmp->dm_ctfp);
return (dt_set_errno(dtp, EDT_CTF));
}

tip->dtt_object = dmp->dm_name;
tip->dtt_ctfp = dmp->dm_ctfp;
tip->dtt_type = ptr;
tip->dtt_flags = bflags;

return (0);
}

const char *
dt_type_name(ctf_file_t *ctfp, ctf_id_t type, char *buf, size_t len)
{
dtrace_hdl_t *dtp = yypcb->pcb_hdl;

if (ctfp == DT_FPTR_CTFP(dtp) && type == DT_FPTR_TYPE(dtp))
(void) snprintf(buf, len, "function pointer");
else if (ctfp == DT_FUNC_CTFP(dtp) && type == DT_FUNC_TYPE(dtp))
(void) snprintf(buf, len, "function");
else if (ctfp == DT_DYN_CTFP(dtp) && type == DT_DYN_TYPE(dtp))
(void) snprintf(buf, len, "dynamic variable");
else if (ctfp == NULL)
(void) snprintf(buf, len, "<none>");
else if (ctf_type_name(ctfp, type, buf, len) == NULL)
(void) snprintf(buf, len, "unknown");

return (buf);
}






static void
dt_type_promote(dt_node_t *lp, dt_node_t *rp, ctf_file_t **ofp, ctf_id_t *otype)
{
ctf_file_t *lfp = lp->dn_ctfp;
ctf_id_t ltype = lp->dn_type;

ctf_file_t *rfp = rp->dn_ctfp;
ctf_id_t rtype = rp->dn_type;

ctf_id_t lbase = ctf_type_resolve(lfp, ltype);
uint_t lkind = ctf_type_kind(lfp, lbase);

ctf_id_t rbase = ctf_type_resolve(rfp, rtype);
uint_t rkind = ctf_type_kind(rfp, rbase);

dtrace_hdl_t *dtp = yypcb->pcb_hdl;
ctf_encoding_t le, re;
uint_t lrank, rrank;

assert(lkind == CTF_K_INTEGER || lkind == CTF_K_ENUM);
assert(rkind == CTF_K_INTEGER || rkind == CTF_K_ENUM);

if (lkind == CTF_K_ENUM) {
lfp = DT_INT_CTFP(dtp);
ltype = lbase = DT_INT_TYPE(dtp);
}

if (rkind == CTF_K_ENUM) {
rfp = DT_INT_CTFP(dtp);
rtype = rbase = DT_INT_TYPE(dtp);
}

if (ctf_type_encoding(lfp, lbase, &le) == CTF_ERR) {
yypcb->pcb_hdl->dt_ctferr = ctf_errno(lfp);
longjmp(yypcb->pcb_jmpbuf, EDT_CTF);
}

if (ctf_type_encoding(rfp, rbase, &re) == CTF_ERR) {
yypcb->pcb_hdl->dt_ctferr = ctf_errno(rfp);
longjmp(yypcb->pcb_jmpbuf, EDT_CTF);
}







lrank = le.cte_bits + ((le.cte_format & CTF_INT_SIGNED) == 0);
rrank = re.cte_bits + ((re.cte_format & CTF_INT_SIGNED) == 0);

if (lrank == rrank) {
if (lbase - rbase < 0)
goto return_rtype;
else
goto return_ltype;
} else if (lrank > rrank) {
goto return_ltype;
} else
goto return_rtype;

return_ltype:
*ofp = lfp;
*otype = ltype;
return;

return_rtype:
*ofp = rfp;
*otype = rtype;
}

void
dt_node_promote(dt_node_t *lp, dt_node_t *rp, dt_node_t *dnp)
{
dt_type_promote(lp, rp, &dnp->dn_ctfp, &dnp->dn_type);
dt_node_type_assign(dnp, dnp->dn_ctfp, dnp->dn_type, B_FALSE);
dt_node_attr_assign(dnp, dt_attr_min(lp->dn_attr, rp->dn_attr));
}

const char *
dt_node_name(const dt_node_t *dnp, char *buf, size_t len)
{
char n1[DT_TYPE_NAMELEN];
char n2[DT_TYPE_NAMELEN];

const char *prefix = "", *suffix = "";
const dtrace_syminfo_t *dts;
char *s;

switch (dnp->dn_kind) {
case DT_NODE_INT:
(void) snprintf(buf, len, "integer constant 0x%llx",
(u_longlong_t)dnp->dn_value);
break;
case DT_NODE_STRING:
s = strchr2esc(dnp->dn_string, strlen(dnp->dn_string));
(void) snprintf(buf, len, "string constant \"%s\"",
s != NULL ? s : dnp->dn_string);
free(s);
break;
case DT_NODE_IDENT:
(void) snprintf(buf, len, "identifier %s", dnp->dn_string);
break;
case DT_NODE_VAR:
case DT_NODE_FUNC:
case DT_NODE_AGG:
case DT_NODE_INLINE:
switch (dnp->dn_ident->di_kind) {
case DT_IDENT_FUNC:
case DT_IDENT_AGGFUNC:
case DT_IDENT_ACTFUNC:
suffix = "( )";
break;
case DT_IDENT_AGG:
prefix = "@";
break;
}
(void) snprintf(buf, len, "%s %s%s%s",
dt_idkind_name(dnp->dn_ident->di_kind),
prefix, dnp->dn_ident->di_name, suffix);
break;
case DT_NODE_SYM:
dts = dnp->dn_ident->di_data;
(void) snprintf(buf, len, "symbol %s`%s",
dts->dts_object, dts->dts_name);
break;
case DT_NODE_TYPE:
(void) snprintf(buf, len, "type %s",
dt_node_type_name(dnp, n1, sizeof (n1)));
break;
case DT_NODE_OP1:
case DT_NODE_OP2:
case DT_NODE_OP3:
(void) snprintf(buf, len, "operator %s", opstr(dnp->dn_op));
break;
case DT_NODE_DEXPR:
case DT_NODE_DFUNC:
if (dnp->dn_expr)
return (dt_node_name(dnp->dn_expr, buf, len));
(void) snprintf(buf, len, "%s", "statement");
break;
case DT_NODE_PDESC:
if (dnp->dn_desc->dtpd_id == 0) {
(void) snprintf(buf, len,
"probe description %s:%s:%s:%s",
dnp->dn_desc->dtpd_provider, dnp->dn_desc->dtpd_mod,
dnp->dn_desc->dtpd_func, dnp->dn_desc->dtpd_name);
} else {
(void) snprintf(buf, len, "probe description %u",
dnp->dn_desc->dtpd_id);
}
break;
case DT_NODE_CLAUSE:
(void) snprintf(buf, len, "%s", "clause");
break;
case DT_NODE_MEMBER:
(void) snprintf(buf, len, "member %s", dnp->dn_membname);
break;
case DT_NODE_XLATOR:
(void) snprintf(buf, len, "translator <%s> (%s)",
dt_type_name(dnp->dn_xlator->dx_dst_ctfp,
dnp->dn_xlator->dx_dst_type, n1, sizeof (n1)),
dt_type_name(dnp->dn_xlator->dx_src_ctfp,
dnp->dn_xlator->dx_src_type, n2, sizeof (n2)));
break;
case DT_NODE_PROG:
(void) snprintf(buf, len, "%s", "program");
break;
default:
(void) snprintf(buf, len, "node <%u>", dnp->dn_kind);
break;
}

return (buf);
}





dt_node_t *
dt_node_xalloc(dtrace_hdl_t *dtp, int kind)
{
dt_node_t *dnp = dt_alloc(dtp, sizeof (dt_node_t));

if (dnp == NULL)
return (NULL);

dnp->dn_ctfp = NULL;
dnp->dn_type = CTF_ERR;
dnp->dn_kind = (uchar_t)kind;
dnp->dn_flags = 0;
dnp->dn_op = 0;
dnp->dn_line = -1;
dnp->dn_reg = -1;
dnp->dn_attr = _dtrace_defattr;
dnp->dn_list = NULL;
dnp->dn_link = NULL;
bzero(&dnp->dn_u, sizeof (dnp->dn_u));

return (dnp);
}







static dt_node_t *
dt_node_alloc(int kind)
{
dt_node_t *dnp = dt_node_xalloc(yypcb->pcb_hdl, kind);

if (dnp == NULL)
longjmp(yypcb->pcb_jmpbuf, EDT_NOMEM);

dnp->dn_line = yylineno;
dnp->dn_link = yypcb->pcb_list;
yypcb->pcb_list = dnp;

return (dnp);
}

void
dt_node_free(dt_node_t *dnp)
{
uchar_t kind = dnp->dn_kind;

dnp->dn_kind = DT_NODE_FREE;

switch (kind) {
case DT_NODE_STRING:
case DT_NODE_IDENT:
case DT_NODE_TYPE:
free(dnp->dn_string);
dnp->dn_string = NULL;
break;

case DT_NODE_VAR:
case DT_NODE_FUNC:
case DT_NODE_PROBE:
if (dnp->dn_ident != NULL) {
if (dnp->dn_ident->di_flags & DT_IDFLG_ORPHAN)
dt_ident_destroy(dnp->dn_ident);
dnp->dn_ident = NULL;
}
dt_node_list_free(&dnp->dn_args);
break;

case DT_NODE_OP1:
if (dnp->dn_child != NULL) {
dt_node_free(dnp->dn_child);
dnp->dn_child = NULL;
}
break;

case DT_NODE_OP3:
if (dnp->dn_expr != NULL) {
dt_node_free(dnp->dn_expr);
dnp->dn_expr = NULL;
}

case DT_NODE_OP2:
if (dnp->dn_left != NULL) {
dt_node_free(dnp->dn_left);
dnp->dn_left = NULL;
}
if (dnp->dn_right != NULL) {
dt_node_free(dnp->dn_right);
dnp->dn_right = NULL;
}
break;

case DT_NODE_DEXPR:
case DT_NODE_DFUNC:
if (dnp->dn_expr != NULL) {
dt_node_free(dnp->dn_expr);
dnp->dn_expr = NULL;
}
break;

case DT_NODE_AGG:
if (dnp->dn_aggfun != NULL) {
dt_node_free(dnp->dn_aggfun);
dnp->dn_aggfun = NULL;
}
dt_node_list_free(&dnp->dn_aggtup);
break;

case DT_NODE_PDESC:
free(dnp->dn_spec);
dnp->dn_spec = NULL;
free(dnp->dn_desc);
dnp->dn_desc = NULL;
break;

case DT_NODE_CLAUSE:
if (dnp->dn_pred != NULL)
dt_node_free(dnp->dn_pred);
if (dnp->dn_locals != NULL)
dt_idhash_destroy(dnp->dn_locals);
dt_node_list_free(&dnp->dn_pdescs);
dt_node_list_free(&dnp->dn_acts);
break;

case DT_NODE_MEMBER:
free(dnp->dn_membname);
dnp->dn_membname = NULL;
if (dnp->dn_membexpr != NULL) {
dt_node_free(dnp->dn_membexpr);
dnp->dn_membexpr = NULL;
}
break;

case DT_NODE_PROVIDER:
dt_node_list_free(&dnp->dn_probes);
free(dnp->dn_provname);
dnp->dn_provname = NULL;
break;

case DT_NODE_PROG:
dt_node_list_free(&dnp->dn_list);
break;
}
}

void
dt_node_attr_assign(dt_node_t *dnp, dtrace_attribute_t attr)
{
if ((yypcb->pcb_cflags & DTRACE_C_EATTR) &&
(dt_attr_cmp(attr, yypcb->pcb_amin) < 0)) {
char a[DTRACE_ATTR2STR_MAX];
char s[BUFSIZ];

dnerror(dnp, D_ATTR_MIN, "attributes for %s (%s) are less than "
"predefined minimum\n", dt_node_name(dnp, s, sizeof (s)),
dtrace_attr2str(attr, a, sizeof (a)));
}

dnp->dn_attr = attr;
}

void
dt_node_type_assign(dt_node_t *dnp, ctf_file_t *fp, ctf_id_t type,
boolean_t user)
{
ctf_id_t base = ctf_type_resolve(fp, type);
uint_t kind = ctf_type_kind(fp, base);
ctf_encoding_t e;

dnp->dn_flags &=
~(DT_NF_SIGNED | DT_NF_REF | DT_NF_BITFIELD | DT_NF_USERLAND);

if (kind == CTF_K_INTEGER && ctf_type_encoding(fp, base, &e) == 0) {
size_t size = e.cte_bits / NBBY;

if (size > 8 || (e.cte_bits % NBBY) != 0 || (size & (size - 1)))
dnp->dn_flags |= DT_NF_BITFIELD;

if (e.cte_format & CTF_INT_SIGNED)
dnp->dn_flags |= DT_NF_SIGNED;
}

if (kind == CTF_K_FLOAT && ctf_type_encoding(fp, base, &e) == 0) {
if (e.cte_bits / NBBY > sizeof (uint64_t))
dnp->dn_flags |= DT_NF_REF;
}

if (kind == CTF_K_STRUCT || kind == CTF_K_UNION ||
kind == CTF_K_FORWARD ||
kind == CTF_K_ARRAY || kind == CTF_K_FUNCTION)
dnp->dn_flags |= DT_NF_REF;
else if (yypcb != NULL && fp == DT_DYN_CTFP(yypcb->pcb_hdl) &&
type == DT_DYN_TYPE(yypcb->pcb_hdl))
dnp->dn_flags |= DT_NF_REF;

if (user)
dnp->dn_flags |= DT_NF_USERLAND;

dnp->dn_flags |= DT_NF_COOKED;
dnp->dn_ctfp = fp;
dnp->dn_type = type;
}

void
dt_node_type_propagate(const dt_node_t *src, dt_node_t *dst)
{
assert(src->dn_flags & DT_NF_COOKED);
dst->dn_flags = src->dn_flags & ~DT_NF_LVALUE;
dst->dn_ctfp = src->dn_ctfp;
dst->dn_type = src->dn_type;
}

const char *
dt_node_type_name(const dt_node_t *dnp, char *buf, size_t len)
{
if (dt_node_is_dynamic(dnp) && dnp->dn_ident != NULL) {
(void) snprintf(buf, len, "%s",
dt_idkind_name(dt_ident_resolve(dnp->dn_ident)->di_kind));
return (buf);
}

if (dnp->dn_flags & DT_NF_USERLAND) {
size_t n = snprintf(buf, len, "userland ");
len = len > n ? len - n : 0;
(void) dt_type_name(dnp->dn_ctfp, dnp->dn_type, buf + n, len);
return (buf);
}

return (dt_type_name(dnp->dn_ctfp, dnp->dn_type, buf, len));
}

size_t
dt_node_type_size(const dt_node_t *dnp)
{
ctf_id_t base;
dtrace_hdl_t *dtp = yypcb->pcb_hdl;

if (dnp->dn_kind == DT_NODE_STRING)
return (strlen(dnp->dn_string) + 1);

if (dt_node_is_dynamic(dnp) && dnp->dn_ident != NULL)
return (dt_ident_size(dnp->dn_ident));

base = ctf_type_resolve(dnp->dn_ctfp, dnp->dn_type);

if (ctf_type_kind(dnp->dn_ctfp, base) == CTF_K_FORWARD)
return (0);









if (ctf_type_kind(dnp->dn_ctfp, base) == CTF_K_POINTER &&
ctf_getmodel(dnp->dn_ctfp) == CTF_MODEL_ILP32 &&
!(dnp->dn_flags & DT_NF_USERLAND) &&
dtp->dt_conf.dtc_ctfmodel == CTF_MODEL_LP64)
return (8);

return (ctf_type_size(dnp->dn_ctfp, dnp->dn_type));
}






dt_ident_t *
dt_node_resolve(const dt_node_t *dnp, uint_t idkind)
{
dt_ident_t *idp;

switch (dnp->dn_kind) {
case DT_NODE_VAR:
case DT_NODE_SYM:
case DT_NODE_FUNC:
case DT_NODE_AGG:
case DT_NODE_INLINE:
case DT_NODE_PROBE:
idp = dt_ident_resolve(dnp->dn_ident);
return (idp->di_kind == idkind ? idp : NULL);
}

if (dt_node_is_dynamic(dnp)) {
idp = dt_ident_resolve(dnp->dn_ident);
return (idp->di_kind == idkind ? idp : NULL);
}

return (NULL);
}

size_t
dt_node_sizeof(const dt_node_t *dnp)
{
dtrace_syminfo_t *sip;
GElf_Sym sym;
dtrace_hdl_t *dtp = yypcb->pcb_hdl;










if (dnp->dn_kind != DT_NODE_SYM)
return (dt_node_type_size(dnp));

sip = dnp->dn_ident->di_data;

if (dtrace_lookup_by_name(dtp, sip->dts_object,
sip->dts_name, &sym, NULL) == -1)
return (0);

return (sym.st_size);
}

int
dt_node_is_integer(const dt_node_t *dnp)
{
ctf_file_t *fp = dnp->dn_ctfp;
ctf_encoding_t e;
ctf_id_t type;
uint_t kind;

assert(dnp->dn_flags & DT_NF_COOKED);

type = ctf_type_resolve(fp, dnp->dn_type);
kind = ctf_type_kind(fp, type);

if (kind == CTF_K_INTEGER &&
ctf_type_encoding(fp, type, &e) == 0 && IS_VOID(e))
return (0);

return (kind == CTF_K_INTEGER || kind == CTF_K_ENUM);
}

int
dt_node_is_float(const dt_node_t *dnp)
{
ctf_file_t *fp = dnp->dn_ctfp;
ctf_encoding_t e;
ctf_id_t type;
uint_t kind;

assert(dnp->dn_flags & DT_NF_COOKED);

type = ctf_type_resolve(fp, dnp->dn_type);
kind = ctf_type_kind(fp, type);

return (kind == CTF_K_FLOAT &&
ctf_type_encoding(dnp->dn_ctfp, type, &e) == 0 && (
e.cte_format == CTF_FP_SINGLE || e.cte_format == CTF_FP_DOUBLE ||
e.cte_format == CTF_FP_LDOUBLE));
}

int
dt_node_is_scalar(const dt_node_t *dnp)
{
ctf_file_t *fp = dnp->dn_ctfp;
ctf_encoding_t e;
ctf_id_t type;
uint_t kind;

assert(dnp->dn_flags & DT_NF_COOKED);

type = ctf_type_resolve(fp, dnp->dn_type);
kind = ctf_type_kind(fp, type);

if (kind == CTF_K_INTEGER &&
ctf_type_encoding(fp, type, &e) == 0 && IS_VOID(e))
return (0);

return (kind == CTF_K_INTEGER || kind == CTF_K_ENUM ||
kind == CTF_K_POINTER);
}

int
dt_node_is_arith(const dt_node_t *dnp)
{
ctf_file_t *fp = dnp->dn_ctfp;
ctf_encoding_t e;
ctf_id_t type;
uint_t kind;

assert(dnp->dn_flags & DT_NF_COOKED);

type = ctf_type_resolve(fp, dnp->dn_type);
kind = ctf_type_kind(fp, type);

if (kind == CTF_K_INTEGER)
return (ctf_type_encoding(fp, type, &e) == 0 && !IS_VOID(e));
else
return (kind == CTF_K_ENUM);
}

int
dt_node_is_vfptr(const dt_node_t *dnp)
{
ctf_file_t *fp = dnp->dn_ctfp;
ctf_encoding_t e;
ctf_id_t type;
uint_t kind;

assert(dnp->dn_flags & DT_NF_COOKED);

type = ctf_type_resolve(fp, dnp->dn_type);
if (ctf_type_kind(fp, type) != CTF_K_POINTER)
return (0);

type = ctf_type_resolve(fp, ctf_type_reference(fp, type));
kind = ctf_type_kind(fp, type);

return (kind == CTF_K_FUNCTION || (kind == CTF_K_INTEGER &&
ctf_type_encoding(fp, type, &e) == 0 && IS_VOID(e)));
}

int
dt_node_is_dynamic(const dt_node_t *dnp)
{
if (dnp->dn_kind == DT_NODE_VAR &&
(dnp->dn_ident->di_flags & DT_IDFLG_INLINE)) {
const dt_idnode_t *inp = dnp->dn_ident->di_iarg;
return (inp->din_root ? dt_node_is_dynamic(inp->din_root) : 0);
}

return (dnp->dn_ctfp == DT_DYN_CTFP(yypcb->pcb_hdl) &&
dnp->dn_type == DT_DYN_TYPE(yypcb->pcb_hdl));
}

int
dt_node_is_string(const dt_node_t *dnp)
{
return (dnp->dn_ctfp == DT_STR_CTFP(yypcb->pcb_hdl) &&
dnp->dn_type == DT_STR_TYPE(yypcb->pcb_hdl));
}

int
dt_node_is_stack(const dt_node_t *dnp)
{
return (dnp->dn_ctfp == DT_STACK_CTFP(yypcb->pcb_hdl) &&
dnp->dn_type == DT_STACK_TYPE(yypcb->pcb_hdl));
}

int
dt_node_is_symaddr(const dt_node_t *dnp)
{
return (dnp->dn_ctfp == DT_SYMADDR_CTFP(yypcb->pcb_hdl) &&
dnp->dn_type == DT_SYMADDR_TYPE(yypcb->pcb_hdl));
}

int
dt_node_is_usymaddr(const dt_node_t *dnp)
{
return (dnp->dn_ctfp == DT_USYMADDR_CTFP(yypcb->pcb_hdl) &&
dnp->dn_type == DT_USYMADDR_TYPE(yypcb->pcb_hdl));
}

int
dt_node_is_strcompat(const dt_node_t *dnp)
{
ctf_file_t *fp = dnp->dn_ctfp;
ctf_encoding_t e;
ctf_arinfo_t r;
ctf_id_t base;
uint_t kind;

assert(dnp->dn_flags & DT_NF_COOKED);

base = ctf_type_resolve(fp, dnp->dn_type);
kind = ctf_type_kind(fp, base);

if (kind == CTF_K_POINTER &&
(base = ctf_type_reference(fp, base)) != CTF_ERR &&
(base = ctf_type_resolve(fp, base)) != CTF_ERR &&
ctf_type_encoding(fp, base, &e) == 0 && IS_CHAR(e))
return (1);

if (kind == CTF_K_ARRAY && ctf_array_info(fp, base, &r) == 0 &&
(base = ctf_type_resolve(fp, r.ctr_contents)) != CTF_ERR &&
ctf_type_encoding(fp, base, &e) == 0 && IS_CHAR(e))
return (1);

return (0);
}

int
dt_node_is_pointer(const dt_node_t *dnp)
{
ctf_file_t *fp = dnp->dn_ctfp;
uint_t kind;

assert(dnp->dn_flags & DT_NF_COOKED);

if (dt_node_is_string(dnp))
return (0);

kind = ctf_type_kind(fp, ctf_type_resolve(fp, dnp->dn_type));
return (kind == CTF_K_POINTER || kind == CTF_K_ARRAY);
}

int
dt_node_is_void(const dt_node_t *dnp)
{
ctf_file_t *fp = dnp->dn_ctfp;
ctf_encoding_t e;
ctf_id_t type;

if (dt_node_is_dynamic(dnp))
return (0);

if (dt_node_is_stack(dnp))
return (0);

if (dt_node_is_symaddr(dnp) || dt_node_is_usymaddr(dnp))
return (0);

type = ctf_type_resolve(fp, dnp->dn_type);

return (ctf_type_kind(fp, type) == CTF_K_INTEGER &&
ctf_type_encoding(fp, type, &e) == 0 && IS_VOID(e));
}

int
dt_node_is_ptrcompat(const dt_node_t *lp, const dt_node_t *rp,
ctf_file_t **fpp, ctf_id_t *tp)
{
ctf_file_t *lfp = lp->dn_ctfp;
ctf_file_t *rfp = rp->dn_ctfp;

ctf_id_t lbase = CTF_ERR, rbase = CTF_ERR;
ctf_id_t lref = CTF_ERR, rref = CTF_ERR;

int lp_is_void, rp_is_void, lp_is_int, rp_is_int, compat;
uint_t lkind, rkind;
ctf_encoding_t e;
ctf_arinfo_t r;

assert(lp->dn_flags & DT_NF_COOKED);
assert(rp->dn_flags & DT_NF_COOKED);

if (dt_node_is_dynamic(lp) || dt_node_is_dynamic(rp))
return (0);

lp_is_int = dt_node_is_integer(lp);
rp_is_int = dt_node_is_integer(rp);

if (lp_is_int && rp_is_int)
return (0);

if (lp_is_int && (lp->dn_kind != DT_NODE_INT || lp->dn_value != 0))
return (0);

if (rp_is_int && (rp->dn_kind != DT_NODE_INT || rp->dn_value != 0))
return (0);

if ((lp_is_int == 0 && rp_is_int == 0) && (
(lp->dn_flags & DT_NF_USERLAND) ^ (rp->dn_flags & DT_NF_USERLAND)))
return (0);






if (!lp_is_int) {
lbase = ctf_type_resolve(lfp, lp->dn_type);
lkind = ctf_type_kind(lfp, lbase);

if (lkind == CTF_K_POINTER) {
lref = ctf_type_resolve(lfp,
ctf_type_reference(lfp, lbase));
} else if (lkind == CTF_K_ARRAY &&
ctf_array_info(lfp, lbase, &r) == 0) {
lref = ctf_type_resolve(lfp, r.ctr_contents);
}
}

if (!rp_is_int) {
rbase = ctf_type_resolve(rfp, rp->dn_type);
rkind = ctf_type_kind(rfp, rbase);

if (rkind == CTF_K_POINTER) {
rref = ctf_type_resolve(rfp,
ctf_type_reference(rfp, rbase));
} else if (rkind == CTF_K_ARRAY &&
ctf_array_info(rfp, rbase, &r) == 0) {
rref = ctf_type_resolve(rfp, r.ctr_contents);
}
}






if (lp_is_int) {
lbase = rbase;
lkind = rkind;
lref = rref;
lfp = rfp;
} else if (rp_is_int) {
rbase = lbase;
rkind = lkind;
rref = lref;
rfp = lfp;
}

lp_is_void = ctf_type_encoding(lfp, lref, &e) == 0 && IS_VOID(e);
rp_is_void = ctf_type_encoding(rfp, rref, &e) == 0 && IS_VOID(e);






compat = (lkind == CTF_K_POINTER || lkind == CTF_K_ARRAY) &&
(rkind == CTF_K_POINTER || rkind == CTF_K_ARRAY) &&
(lp_is_void || rp_is_void || ctf_type_compat(lfp, lref, rfp, rref));

if (compat) {
if (fpp != NULL)
*fpp = rp_is_void ? lfp : rfp;
if (tp != NULL)
*tp = rp_is_void ? lbase : rbase;
}

return (compat);
}






int
dt_node_is_argcompat(const dt_node_t *lp, const dt_node_t *rp)
{
ctf_file_t *lfp = lp->dn_ctfp;
ctf_file_t *rfp = rp->dn_ctfp;

assert(lp->dn_flags & DT_NF_COOKED);
assert(rp->dn_flags & DT_NF_COOKED);

if (dt_node_is_integer(lp) && dt_node_is_integer(rp))
return (1);

if (dt_node_is_strcompat(lp) && dt_node_is_strcompat(rp))
return (1);

if (dt_node_is_stack(lp) && dt_node_is_stack(rp))
return (1);

if (dt_node_is_symaddr(lp) && dt_node_is_symaddr(rp))
return (1);

if (dt_node_is_usymaddr(lp) && dt_node_is_usymaddr(rp))
return (1);

switch (ctf_type_kind(lfp, ctf_type_resolve(lfp, lp->dn_type))) {
case CTF_K_FUNCTION:
case CTF_K_STRUCT:
case CTF_K_UNION:
return (ctf_type_compat(lfp, lp->dn_type, rfp, rp->dn_type));
default:
return (dt_node_is_ptrcompat(lp, rp, NULL, NULL));
}
}





int
dt_node_is_posconst(const dt_node_t *dnp)
{
return (dnp->dn_kind == DT_NODE_INT && dnp->dn_value != 0 && (
(dnp->dn_flags & DT_NF_SIGNED) == 0 || (int64_t)dnp->dn_value > 0));
}

int
dt_node_is_actfunc(const dt_node_t *dnp)
{
return (dnp->dn_kind == DT_NODE_FUNC &&
dnp->dn_ident->di_kind == DT_IDENT_ACTFUNC);
}































dt_node_t *
dt_node_int(uintmax_t value)
{
dt_node_t *dnp = dt_node_alloc(DT_NODE_INT);
dtrace_hdl_t *dtp = yypcb->pcb_hdl;

int n = (yyintdecimal | (yyintsuffix[0] == 'u')) + 1;
int i = 0;

const char *p;
char c;

dnp->dn_op = DT_TOK_INT;
dnp->dn_value = value;

for (p = yyintsuffix; (c = *p) != '\0'; p++) {
if (c == 'U' || c == 'u')
i += 1;
else if (c == 'L' || c == 'l')
i += 2;
}

for (; i < sizeof (dtp->dt_ints) / sizeof (dtp->dt_ints[0]); i += n) {
if (value <= dtp->dt_ints[i].did_limit) {
dt_node_type_assign(dnp,
dtp->dt_ints[i].did_ctfp,
dtp->dt_ints[i].did_type, B_FALSE);





switch (yyintprefix) {
case '+':
return (dt_node_op1(DT_TOK_IPOS, dnp));
case '-':
return (dt_node_op1(DT_TOK_INEG, dnp));
default:
return (dnp);
}
}
}

xyerror(D_INT_OFLOW, "integer constant 0x%llx cannot be represented "
"in any built-in integral type\n", (u_longlong_t)value);

return (NULL);
}

dt_node_t *
dt_node_string(char *string)
{
dtrace_hdl_t *dtp = yypcb->pcb_hdl;
dt_node_t *dnp;

if (string == NULL)
longjmp(yypcb->pcb_jmpbuf, EDT_NOMEM);

dnp = dt_node_alloc(DT_NODE_STRING);
dnp->dn_op = DT_TOK_STRING;
dnp->dn_string = string;
dt_node_type_assign(dnp, DT_STR_CTFP(dtp), DT_STR_TYPE(dtp), B_FALSE);

return (dnp);
}

dt_node_t *
dt_node_ident(char *name)
{
dt_ident_t *idp;
dt_node_t *dnp;

if (name == NULL)
longjmp(yypcb->pcb_jmpbuf, EDT_NOMEM);







if ((idp = dt_idstack_lookup(&yypcb->pcb_globals, name)) != NULL &&
(idp->di_flags & DT_IDFLG_INLINE)) {
dt_idnode_t *inp = idp->di_iarg;

if (inp->din_root != NULL &&
inp->din_root->dn_kind == DT_NODE_INT) {
free(name);

dnp = dt_node_alloc(DT_NODE_INT);
dnp->dn_op = DT_TOK_INT;
dnp->dn_value = inp->din_root->dn_value;
dt_node_type_propagate(inp->din_root, dnp);

return (dnp);
}
}

dnp = dt_node_alloc(DT_NODE_IDENT);
dnp->dn_op = name[0] == '@' ? DT_TOK_AGG : DT_TOK_IDENT;
dnp->dn_string = name;

return (dnp);
}






dt_node_t *
dt_node_type(dt_decl_t *ddp)
{
dtrace_hdl_t *dtp = yypcb->pcb_hdl;
dtrace_typeinfo_t dtt;
dt_node_t *dnp;
char *name = NULL;
int err;





if (ddp == NULL)
ddp = dt_decl_pop_param(&name);

err = dt_decl_type(ddp, &dtt);
dt_decl_free(ddp);

if (err != 0) {
free(name);
longjmp(yypcb->pcb_jmpbuf, EDT_COMPILER);
}

dnp = dt_node_alloc(DT_NODE_TYPE);
dnp->dn_op = DT_TOK_IDENT;
dnp->dn_string = name;

dt_node_type_assign(dnp, dtt.dtt_ctfp, dtt.dtt_type, dtt.dtt_flags);

if (dtt.dtt_ctfp == dtp->dt_cdefs->dm_ctfp ||
dtt.dtt_ctfp == dtp->dt_ddefs->dm_ctfp)
dt_node_attr_assign(dnp, _dtrace_defattr);
else
dt_node_attr_assign(dnp, _dtrace_typattr);

return (dnp);
}





dt_node_t *
dt_node_vatype(void)
{
dt_node_t *dnp = dt_node_alloc(DT_NODE_TYPE);

dnp->dn_op = DT_TOK_IDENT;
dnp->dn_ctfp = yypcb->pcb_hdl->dt_cdefs->dm_ctfp;
dnp->dn_type = CTF_ERR;
dnp->dn_attr = _dtrace_defattr;

return (dnp);
}








dt_node_t *
dt_node_decl(void)
{
dtrace_hdl_t *dtp = yypcb->pcb_hdl;
dt_scope_t *dsp = &yypcb->pcb_dstack;
dt_dclass_t class = dsp->ds_class;
dt_decl_t *ddp = dt_decl_top();

dt_module_t *dmp;
dtrace_typeinfo_t dtt;
ctf_id_t type;

char n1[DT_TYPE_NAMELEN];
char n2[DT_TYPE_NAMELEN];

if (dt_decl_type(ddp, &dtt) != 0)
longjmp(yypcb->pcb_jmpbuf, EDT_COMPILER);






if (dsp->ds_ident == NULL) {
if (ddp->dd_kind != CTF_K_STRUCT &&
ddp->dd_kind != CTF_K_UNION && ddp->dd_kind != CTF_K_ENUM)
xyerror(D_DECL_USELESS, "useless declaration\n");

dt_dprintf("type %s added as id %ld\n", dt_type_name(
ddp->dd_ctfp, ddp->dd_type, n1, sizeof (n1)), ddp->dd_type);

return (NULL);
}

if (strchr(dsp->ds_ident, '`') != NULL) {
xyerror(D_DECL_SCOPE, "D scoping operator may not be used in "
"a declaration name (%s)\n", dsp->ds_ident);
}





if (yypcb->pcb_idepth != 0)
dmp = dtp->dt_cdefs;
else
dmp = dtp->dt_ddefs;





if (ctf_type_kind(dtt.dtt_ctfp, dtt.dtt_type) == CTF_K_FUNCTION &&
(class == DT_DC_DEFAULT || class == DT_DC_STATIC))
class = DT_DC_EXTERN;

switch (class) {
case DT_DC_AUTO:
case DT_DC_REGISTER:
case DT_DC_STATIC:
xyerror(D_DECL_BADCLASS, "specified storage class not "
"appropriate in D\n");


case DT_DC_EXTERN: {
dtrace_typeinfo_t ott;
dtrace_syminfo_t dts;
GElf_Sym sym;

int exists = dtrace_lookup_by_name(dtp,
dmp->dm_name, dsp->ds_ident, &sym, &dts) == 0;

if (exists && (dtrace_symbol_type(dtp, &sym, &dts, &ott) != 0 ||
ctf_type_cmp(dtt.dtt_ctfp, dtt.dtt_type,
ott.dtt_ctfp, ott.dtt_type) != 0)) {
xyerror(D_DECL_IDRED, "identifier redeclared: %s`%s\n"
"\t current: %s\n\tprevious: %s\n",
dmp->dm_name, dsp->ds_ident,
dt_type_name(dtt.dtt_ctfp, dtt.dtt_type,
n1, sizeof (n1)),
dt_type_name(ott.dtt_ctfp, ott.dtt_type,
n2, sizeof (n2)));
} else if (!exists && dt_module_extern(dtp, dmp,
dsp->ds_ident, &dtt) == NULL) {
xyerror(D_UNKNOWN,
"failed to extern %s: %s\n", dsp->ds_ident,
dtrace_errmsg(dtp, dtrace_errno(dtp)));
} else {
dt_dprintf("extern %s`%s type=<%s>\n",
dmp->dm_name, dsp->ds_ident,
dt_type_name(dtt.dtt_ctfp, dtt.dtt_type,
n1, sizeof (n1)));
}
break;
}

case DT_DC_TYPEDEF:
if (dt_idstack_lookup(&yypcb->pcb_globals, dsp->ds_ident)) {
xyerror(D_DECL_IDRED, "global variable identifier "
"redeclared: %s\n", dsp->ds_ident);
}

if (ctf_lookup_by_name(dmp->dm_ctfp,
dsp->ds_ident) != CTF_ERR) {
xyerror(D_DECL_IDRED,
"typedef redeclared: %s\n", dsp->ds_ident);
}






if (dtt.dtt_ctfp != dmp->dm_ctfp &&
dtt.dtt_ctfp != ctf_parent_file(dmp->dm_ctfp)) {

dtt.dtt_type = ctf_add_type(dmp->dm_ctfp,
dtt.dtt_ctfp, dtt.dtt_type);
dtt.dtt_ctfp = dmp->dm_ctfp;

if (dtt.dtt_type == CTF_ERR ||
ctf_update(dtt.dtt_ctfp) == CTF_ERR) {
xyerror(D_UNKNOWN, "failed to copy typedef %s "
"source type: %s\n", dsp->ds_ident,
ctf_errmsg(ctf_errno(dtt.dtt_ctfp)));
}
}

type = ctf_add_typedef(dmp->dm_ctfp,
CTF_ADD_ROOT, dsp->ds_ident, dtt.dtt_type);

if (type == CTF_ERR || ctf_update(dmp->dm_ctfp) == CTF_ERR) {
xyerror(D_UNKNOWN, "failed to typedef %s: %s\n",
dsp->ds_ident, ctf_errmsg(ctf_errno(dmp->dm_ctfp)));
}

dt_dprintf("typedef %s added as id %ld\n", dsp->ds_ident, type);
break;

default: {
ctf_encoding_t cte;
dt_idhash_t *dhp;
dt_ident_t *idp;
dt_node_t idn;
int assc, idkind;
uint_t id, kind;
ushort_t idflags;

switch (class) {
case DT_DC_THIS:
dhp = yypcb->pcb_locals;
idflags = DT_IDFLG_LOCAL;
idp = dt_idhash_lookup(dhp, dsp->ds_ident);
break;
case DT_DC_SELF:
dhp = dtp->dt_tls;
idflags = DT_IDFLG_TLS;
idp = dt_idhash_lookup(dhp, dsp->ds_ident);
break;
default:
dhp = dtp->dt_globals;
idflags = 0;
idp = dt_idstack_lookup(
&yypcb->pcb_globals, dsp->ds_ident);
break;
}

if (ddp->dd_kind == CTF_K_ARRAY && ddp->dd_node == NULL) {
xyerror(D_DECL_ARRNULL,
"array declaration requires array dimension or "
"tuple signature: %s\n", dsp->ds_ident);
}

if (idp != NULL && idp->di_gen == 0) {
xyerror(D_DECL_IDRED, "built-in identifier "
"redeclared: %s\n", idp->di_name);
}

if (dtrace_lookup_by_type(dtp, DTRACE_OBJ_CDEFS,
dsp->ds_ident, NULL) == 0 ||
dtrace_lookup_by_type(dtp, DTRACE_OBJ_DDEFS,
dsp->ds_ident, NULL) == 0) {
xyerror(D_DECL_IDRED, "typedef identifier "
"redeclared: %s\n", dsp->ds_ident);
}








assc = ddp->dd_kind == CTF_K_ARRAY &&
ddp->dd_node->dn_kind == DT_NODE_TYPE;

idkind = assc ? DT_IDENT_ARRAY : DT_IDENT_SCALAR;









bzero(&idn, sizeof (dt_node_t));

if (idp != NULL && idp->di_type != CTF_ERR)
dt_node_type_assign(&idn, idp->di_ctfp, idp->di_type,
B_FALSE);
else if (idp != NULL)
(void) dt_ident_cook(&idn, idp, NULL);

if (assc) {
if (class == DT_DC_THIS) {
xyerror(D_DECL_LOCASSC, "associative arrays "
"may not be declared as local variables:"
" %s\n", dsp->ds_ident);
}

if (dt_decl_type(ddp->dd_next, &dtt) != 0)
longjmp(yypcb->pcb_jmpbuf, EDT_COMPILER);
}

if (idp != NULL && (idp->di_kind != idkind ||
ctf_type_cmp(dtt.dtt_ctfp, dtt.dtt_type,
idn.dn_ctfp, idn.dn_type) != 0)) {
xyerror(D_DECL_IDRED, "identifier redeclared: %s\n"
"\t current: %s %s\n\tprevious: %s %s\n",
dsp->ds_ident, dt_idkind_name(idkind),
dt_type_name(dtt.dtt_ctfp,
dtt.dtt_type, n1, sizeof (n1)),
dt_idkind_name(idp->di_kind),
dt_node_type_name(&idn, n2, sizeof (n2)));

} else if (idp != NULL && assc) {
const dt_idsig_t *isp = idp->di_data;
dt_node_t *dnp = ddp->dd_node;
int argc = 0;

for (; dnp != NULL; dnp = dnp->dn_list, argc++) {
const dt_node_t *pnp = &isp->dis_args[argc];

if (argc >= isp->dis_argc)
continue;

if (ctf_type_cmp(dnp->dn_ctfp, dnp->dn_type,
pnp->dn_ctfp, pnp->dn_type) == 0)
continue;

xyerror(D_DECL_IDRED,
"identifier redeclared: %s\n"
"\t current: %s, key #%d of type %s\n"
"\tprevious: %s, key #%d of type %s\n",
dsp->ds_ident,
dt_idkind_name(idkind), argc + 1,
dt_node_type_name(dnp, n1, sizeof (n1)),
dt_idkind_name(idp->di_kind), argc + 1,
dt_node_type_name(pnp, n2, sizeof (n2)));
}

if (isp->dis_argc != argc) {
xyerror(D_DECL_IDRED,
"identifier redeclared: %s\n"
"\t current: %s of %s, tuple length %d\n"
"\tprevious: %s of %s, tuple length %d\n",
dsp->ds_ident, dt_idkind_name(idkind),
dt_type_name(dtt.dtt_ctfp, dtt.dtt_type,
n1, sizeof (n1)), argc,
dt_idkind_name(idp->di_kind),
dt_node_type_name(&idn, n2, sizeof (n2)),
isp->dis_argc);
}

} else if (idp == NULL) {
type = ctf_type_resolve(dtt.dtt_ctfp, dtt.dtt_type);
kind = ctf_type_kind(dtt.dtt_ctfp, type);

switch (kind) {
case CTF_K_INTEGER:
if (ctf_type_encoding(dtt.dtt_ctfp, type,
&cte) == 0 && IS_VOID(cte)) {
xyerror(D_DECL_VOIDOBJ, "cannot have "
"void object: %s\n", dsp->ds_ident);
}
break;
case CTF_K_STRUCT:
case CTF_K_UNION:
if (ctf_type_size(dtt.dtt_ctfp, type) != 0)
break;

case CTF_K_FORWARD:
xyerror(D_DECL_INCOMPLETE,
"incomplete struct/union/enum %s: %s\n",
dt_type_name(dtt.dtt_ctfp, dtt.dtt_type,
n1, sizeof (n1)), dsp->ds_ident);

}

if (dt_idhash_nextid(dhp, &id) == -1) {
xyerror(D_ID_OFLOW, "cannot create %s: limit "
"on number of %s variables exceeded\n",
dsp->ds_ident, dt_idhash_name(dhp));
}

dt_dprintf("declare %s %s variable %s, id=%u\n",
dt_idhash_name(dhp), dt_idkind_name(idkind),
dsp->ds_ident, id);

idp = dt_idhash_insert(dhp, dsp->ds_ident, idkind,
idflags | DT_IDFLG_WRITE | DT_IDFLG_DECL, id,
_dtrace_defattr, 0, assc ? &dt_idops_assc :
&dt_idops_thaw, NULL, dtp->dt_gen);

if (idp == NULL)
longjmp(yypcb->pcb_jmpbuf, EDT_NOMEM);

dt_ident_type_assign(idp, dtt.dtt_ctfp, dtt.dtt_type);









if (assc) {
idp->di_attr =
dt_ident_cook(&idn, idp, &ddp->dd_node);
}
}
}

}

free(dsp->ds_ident);
dsp->ds_ident = NULL;

return (NULL);
}

dt_node_t *
dt_node_func(dt_node_t *dnp, dt_node_t *args)
{
dt_ident_t *idp;

if (dnp->dn_kind != DT_NODE_IDENT) {
xyerror(D_FUNC_IDENT,
"function designator is not of function type\n");
}

idp = dt_idstack_lookup(&yypcb->pcb_globals, dnp->dn_string);

if (idp == NULL) {
xyerror(D_FUNC_UNDEF,
"undefined function name: %s\n", dnp->dn_string);
}

if (idp->di_kind != DT_IDENT_FUNC &&
idp->di_kind != DT_IDENT_AGGFUNC &&
idp->di_kind != DT_IDENT_ACTFUNC) {
xyerror(D_FUNC_IDKIND, "%s '%s' may not be referenced as a "
"function\n", dt_idkind_name(idp->di_kind), idp->di_name);
}

free(dnp->dn_string);
dnp->dn_string = NULL;

dnp->dn_kind = DT_NODE_FUNC;
dnp->dn_flags &= ~DT_NF_COOKED;
dnp->dn_ident = idp;
dnp->dn_args = args;
dnp->dn_list = NULL;

return (dnp);
}






dt_node_t *
dt_node_offsetof(dt_decl_t *ddp, char *s)
{
dtrace_typeinfo_t dtt;
dt_node_t dn;
char *name;
int err;

ctf_membinfo_t ctm;
ctf_id_t type;
uint_t kind;

name = alloca(strlen(s) + 1);
(void) strcpy(name, s);
free(s);

err = dt_decl_type(ddp, &dtt);
dt_decl_free(ddp);

if (err != 0)
longjmp(yypcb->pcb_jmpbuf, EDT_COMPILER);

type = ctf_type_resolve(dtt.dtt_ctfp, dtt.dtt_type);
kind = ctf_type_kind(dtt.dtt_ctfp, type);

if (kind != CTF_K_STRUCT && kind != CTF_K_UNION) {
xyerror(D_OFFSETOF_TYPE,
"offsetof operand must be a struct or union type\n");
}

if (ctf_member_info(dtt.dtt_ctfp, type, name, &ctm) == CTF_ERR) {
xyerror(D_UNKNOWN, "failed to determine offset of %s: %s\n",
name, ctf_errmsg(ctf_errno(dtt.dtt_ctfp)));
}

bzero(&dn, sizeof (dn));
dt_node_type_assign(&dn, dtt.dtt_ctfp, ctm.ctm_type, B_FALSE);

if (dn.dn_flags & DT_NF_BITFIELD) {
xyerror(D_OFFSETOF_BITFIELD,
"cannot take offset of a bit-field: %s\n", name);
}

return (dt_node_int(ctm.ctm_offset / NBBY));
}

dt_node_t *
dt_node_op1(int op, dt_node_t *cp)
{
dt_node_t *dnp;

if (cp->dn_kind == DT_NODE_INT) {
switch (op) {
case DT_TOK_INEG:





cp->dn_value = -cp->dn_value;
if (!(cp->dn_flags & DT_NF_SIGNED)) {
cp->dn_value &= ~0ULL >>
(64 - dt_node_type_size(cp) * NBBY);
}

case DT_TOK_IPOS:
return (cp);
case DT_TOK_BNEG:
cp->dn_value = ~cp->dn_value;
return (cp);
case DT_TOK_LNEG:
cp->dn_value = !cp->dn_value;
return (cp);
}
}






if (op == DT_TOK_SIZEOF &&
(cp->dn_kind == DT_NODE_STRING || cp->dn_kind == DT_NODE_TYPE)) {
dtrace_hdl_t *dtp = yypcb->pcb_hdl;
size_t size = dt_node_type_size(cp);

if (size == 0) {
xyerror(D_SIZEOF_TYPE, "cannot apply sizeof to an "
"operand of unknown size\n");
}

dt_node_type_assign(cp, dtp->dt_ddefs->dm_ctfp,
ctf_lookup_by_name(dtp->dt_ddefs->dm_ctfp, "size_t"),
B_FALSE);

cp->dn_kind = DT_NODE_INT;
cp->dn_op = DT_TOK_INT;
cp->dn_value = size;

return (cp);
}

dnp = dt_node_alloc(DT_NODE_OP1);
assert(op <= USHRT_MAX);
dnp->dn_op = (ushort_t)op;
dnp->dn_child = cp;

return (dnp);
}









static void
dt_cast(dt_node_t *lp, dt_node_t *rp)
{
size_t srcsize = dt_node_type_size(rp);
size_t dstsize = dt_node_type_size(lp);

if (dstsize < srcsize) {
int n = (sizeof (uint64_t) - dstsize) * NBBY;
rp->dn_value <<= n;
rp->dn_value >>= n;
} else if (dstsize > srcsize) {
int n = (sizeof (uint64_t) - srcsize) * NBBY;
int s = (dstsize - srcsize) * NBBY;

rp->dn_value <<= n;
if (rp->dn_flags & DT_NF_SIGNED) {
rp->dn_value = (intmax_t)rp->dn_value >> s;
rp->dn_value >>= n - s;
} else {
rp->dn_value >>= n;
}
}
}

dt_node_t *
dt_node_op2(int op, dt_node_t *lp, dt_node_t *rp)
{
dtrace_hdl_t *dtp = yypcb->pcb_hdl;
dt_node_t *dnp;





if (rp->dn_kind == DT_NODE_INT && rp->dn_value == 0 &&
(op == DT_TOK_MOD || op == DT_TOK_DIV ||
op == DT_TOK_MOD_EQ || op == DT_TOK_DIV_EQ))
xyerror(D_DIV_ZERO, "expression contains division by zero\n");





if (lp->dn_kind == DT_NODE_INT && rp->dn_kind == DT_NODE_INT) {
uintmax_t l = lp->dn_value;
uintmax_t r = rp->dn_value;

dnp = dt_node_int(0);

switch (op) {
case DT_TOK_LOR:
dnp->dn_value = l || r;
dt_node_type_assign(dnp,
DT_INT_CTFP(dtp), DT_INT_TYPE(dtp), B_FALSE);
break;
case DT_TOK_LXOR:
dnp->dn_value = (l != 0) ^ (r != 0);
dt_node_type_assign(dnp,
DT_INT_CTFP(dtp), DT_INT_TYPE(dtp), B_FALSE);
break;
case DT_TOK_LAND:
dnp->dn_value = l && r;
dt_node_type_assign(dnp,
DT_INT_CTFP(dtp), DT_INT_TYPE(dtp), B_FALSE);
break;
case DT_TOK_BOR:
dnp->dn_value = l | r;
dt_node_promote(lp, rp, dnp);
break;
case DT_TOK_XOR:
dnp->dn_value = l ^ r;
dt_node_promote(lp, rp, dnp);
break;
case DT_TOK_BAND:
dnp->dn_value = l & r;
dt_node_promote(lp, rp, dnp);
break;
case DT_TOK_EQU:
dnp->dn_value = l == r;
dt_node_type_assign(dnp,
DT_INT_CTFP(dtp), DT_INT_TYPE(dtp), B_FALSE);
break;
case DT_TOK_NEQ:
dnp->dn_value = l != r;
dt_node_type_assign(dnp,
DT_INT_CTFP(dtp), DT_INT_TYPE(dtp), B_FALSE);
break;
case DT_TOK_LT:
dt_node_promote(lp, rp, dnp);
if (dnp->dn_flags & DT_NF_SIGNED)
dnp->dn_value = (intmax_t)l < (intmax_t)r;
else
dnp->dn_value = l < r;
dt_node_type_assign(dnp,
DT_INT_CTFP(dtp), DT_INT_TYPE(dtp), B_FALSE);
break;
case DT_TOK_LE:
dt_node_promote(lp, rp, dnp);
if (dnp->dn_flags & DT_NF_SIGNED)
dnp->dn_value = (intmax_t)l <= (intmax_t)r;
else
dnp->dn_value = l <= r;
dt_node_type_assign(dnp,
DT_INT_CTFP(dtp), DT_INT_TYPE(dtp), B_FALSE);
break;
case DT_TOK_GT:
dt_node_promote(lp, rp, dnp);
if (dnp->dn_flags & DT_NF_SIGNED)
dnp->dn_value = (intmax_t)l > (intmax_t)r;
else
dnp->dn_value = l > r;
dt_node_type_assign(dnp,
DT_INT_CTFP(dtp), DT_INT_TYPE(dtp), B_FALSE);
break;
case DT_TOK_GE:
dt_node_promote(lp, rp, dnp);
if (dnp->dn_flags & DT_NF_SIGNED)
dnp->dn_value = (intmax_t)l >= (intmax_t)r;
else
dnp->dn_value = l >= r;
dt_node_type_assign(dnp,
DT_INT_CTFP(dtp), DT_INT_TYPE(dtp), B_FALSE);
break;
case DT_TOK_LSH:
dnp->dn_value = l << r;
dt_node_type_propagate(lp, dnp);
dt_node_attr_assign(rp,
dt_attr_min(lp->dn_attr, rp->dn_attr));
break;
case DT_TOK_RSH:
dnp->dn_value = l >> r;
dt_node_type_propagate(lp, dnp);
dt_node_attr_assign(rp,
dt_attr_min(lp->dn_attr, rp->dn_attr));
break;
case DT_TOK_ADD:
dnp->dn_value = l + r;
dt_node_promote(lp, rp, dnp);
break;
case DT_TOK_SUB:
dnp->dn_value = l - r;
dt_node_promote(lp, rp, dnp);
break;
case DT_TOK_MUL:
dnp->dn_value = l * r;
dt_node_promote(lp, rp, dnp);
break;
case DT_TOK_DIV:
dt_node_promote(lp, rp, dnp);
if (dnp->dn_flags & DT_NF_SIGNED)
dnp->dn_value = (intmax_t)l / (intmax_t)r;
else
dnp->dn_value = l / r;
break;
case DT_TOK_MOD:
dt_node_promote(lp, rp, dnp);
if (dnp->dn_flags & DT_NF_SIGNED)
dnp->dn_value = (intmax_t)l % (intmax_t)r;
else
dnp->dn_value = l % r;
break;
default:
dt_node_free(dnp);
dnp = NULL;
}

if (dnp != NULL) {
dt_node_free(lp);
dt_node_free(rp);
return (dnp);
}
}

if (op == DT_TOK_LPAR && rp->dn_kind == DT_NODE_INT &&
dt_node_is_integer(lp)) {
dt_cast(lp, rp);
dt_node_type_propagate(lp, rp);
dt_node_attr_assign(rp, dt_attr_min(lp->dn_attr, rp->dn_attr));
dt_node_free(lp);

return (rp);
}





dnp = dt_node_alloc(DT_NODE_OP2);
assert(op <= USHRT_MAX);
dnp->dn_op = (ushort_t)op;
dnp->dn_left = lp;
dnp->dn_right = rp;

return (dnp);
}

dt_node_t *
dt_node_op3(dt_node_t *expr, dt_node_t *lp, dt_node_t *rp)
{
dt_node_t *dnp;

if (expr->dn_kind == DT_NODE_INT)
return (expr->dn_value != 0 ? lp : rp);

dnp = dt_node_alloc(DT_NODE_OP3);
dnp->dn_op = DT_TOK_QUESTION;
dnp->dn_expr = expr;
dnp->dn_left = lp;
dnp->dn_right = rp;

return (dnp);
}

dt_node_t *
dt_node_statement(dt_node_t *expr)
{
dt_node_t *dnp;

if (expr->dn_kind == DT_NODE_AGG)
return (expr);

if (expr->dn_kind == DT_NODE_FUNC &&
expr->dn_ident->di_kind == DT_IDENT_ACTFUNC)
dnp = dt_node_alloc(DT_NODE_DFUNC);
else
dnp = dt_node_alloc(DT_NODE_DEXPR);

dnp->dn_expr = expr;
return (dnp);
}

dt_node_t *
dt_node_if(dt_node_t *pred, dt_node_t *acts, dt_node_t *else_acts)
{
dt_node_t *dnp = dt_node_alloc(DT_NODE_IF);
dnp->dn_conditional = pred;
dnp->dn_body = acts;
dnp->dn_alternate_body = else_acts;

return (dnp);
}

dt_node_t *
dt_node_pdesc_by_name(char *spec)
{
dtrace_hdl_t *dtp = yypcb->pcb_hdl;
dt_node_t *dnp;

if (spec == NULL)
longjmp(yypcb->pcb_jmpbuf, EDT_NOMEM);

dnp = dt_node_alloc(DT_NODE_PDESC);
dnp->dn_spec = spec;
dnp->dn_desc = malloc(sizeof (dtrace_probedesc_t));

if (dnp->dn_desc == NULL)
longjmp(yypcb->pcb_jmpbuf, EDT_NOMEM);

if (dtrace_xstr2desc(dtp, yypcb->pcb_pspec, dnp->dn_spec,
yypcb->pcb_sargc, yypcb->pcb_sargv, dnp->dn_desc) != 0) {
xyerror(D_PDESC_INVAL, "invalid probe description \"%s\": %s\n",
dnp->dn_spec, dtrace_errmsg(dtp, dtrace_errno(dtp)));
}

free(dnp->dn_spec);
dnp->dn_spec = NULL;

return (dnp);
}

dt_node_t *
dt_node_pdesc_by_id(uintmax_t id)
{
static const char *const names[] = {
"providers", "modules", "functions"
};

dtrace_hdl_t *dtp = yypcb->pcb_hdl;
dt_node_t *dnp = dt_node_alloc(DT_NODE_PDESC);

if ((dnp->dn_desc = malloc(sizeof (dtrace_probedesc_t))) == NULL)
longjmp(yypcb->pcb_jmpbuf, EDT_NOMEM);

if (id > UINT_MAX) {
xyerror(D_PDESC_INVAL, "identifier %llu exceeds maximum "
"probe id\n", (u_longlong_t)id);
}

if (yypcb->pcb_pspec != DTRACE_PROBESPEC_NAME) {
xyerror(D_PDESC_INVAL, "probe identifier %llu not permitted "
"when specifying %s\n", (u_longlong_t)id,
names[yypcb->pcb_pspec]);
}

if (dtrace_id2desc(dtp, (dtrace_id_t)id, dnp->dn_desc) != 0) {
xyerror(D_PDESC_INVAL, "invalid probe identifier %llu: %s\n",
(u_longlong_t)id, dtrace_errmsg(dtp, dtrace_errno(dtp)));
}

return (dnp);
}

dt_node_t *
dt_node_clause(dt_node_t *pdescs, dt_node_t *pred, dt_node_t *acts)
{
dt_node_t *dnp = dt_node_alloc(DT_NODE_CLAUSE);

dnp->dn_pdescs = pdescs;
dnp->dn_pred = pred;
dnp->dn_acts = acts;

return (dnp);
}

dt_node_t *
dt_node_inline(dt_node_t *expr)
{
dtrace_hdl_t *dtp = yypcb->pcb_hdl;
dt_scope_t *dsp = &yypcb->pcb_dstack;
dt_decl_t *ddp = dt_decl_top();

char n[DT_TYPE_NAMELEN];
dtrace_typeinfo_t dtt;

dt_ident_t *idp, *rdp;
dt_idnode_t *inp;
dt_node_t *dnp;

if (dt_decl_type(ddp, &dtt) != 0)
longjmp(yypcb->pcb_jmpbuf, EDT_COMPILER);

if (dsp->ds_class != DT_DC_DEFAULT) {
xyerror(D_DECL_BADCLASS, "specified storage class not "
"appropriate for inline declaration\n");
}

if (dsp->ds_ident == NULL)
xyerror(D_DECL_USELESS, "inline declaration requires a name\n");

if ((idp = dt_idstack_lookup(
&yypcb->pcb_globals, dsp->ds_ident)) != NULL) {
xyerror(D_DECL_IDRED, "identifier redefined: %s\n\t current: "
"inline definition\n\tprevious: %s %s\n",
idp->di_name, dt_idkind_name(idp->di_kind),
(idp->di_flags & DT_IDFLG_INLINE) ? "inline" : "");
}





if (ddp->dd_kind == CTF_K_ARRAY) {
if (ddp->dd_node == NULL) {
xyerror(D_DECL_ARRNULL, "inline declaration requires "
"array tuple signature: %s\n", dsp->ds_ident);
}

if (ddp->dd_node->dn_kind != DT_NODE_TYPE) {
xyerror(D_DECL_ARRNULL, "inline declaration cannot be "
"of scalar array type: %s\n", dsp->ds_ident);
}

if (dt_decl_type(ddp->dd_next, &dtt) != 0)
longjmp(yypcb->pcb_jmpbuf, EDT_COMPILER);
}






dnp = dt_node_alloc(DT_NODE_INLINE);
dt_node_type_assign(dnp, dtt.dtt_ctfp, dtt.dtt_type, B_FALSE);
dt_node_attr_assign(dnp, _dtrace_defattr);

if (dt_node_is_void(dnp)) {
xyerror(D_DECL_VOIDOBJ,
"cannot declare void inline: %s\n", dsp->ds_ident);
}

if (ctf_type_kind(dnp->dn_ctfp, ctf_type_resolve(
dnp->dn_ctfp, dnp->dn_type)) == CTF_K_FORWARD) {
xyerror(D_DECL_INCOMPLETE,
"incomplete struct/union/enum %s: %s\n",
dt_node_type_name(dnp, n, sizeof (n)), dsp->ds_ident);
}

if ((inp = malloc(sizeof (dt_idnode_t))) == NULL)
longjmp(yypcb->pcb_jmpbuf, EDT_NOMEM);

bzero(inp, sizeof (dt_idnode_t));

idp = dnp->dn_ident = dt_ident_create(dsp->ds_ident,
ddp->dd_kind == CTF_K_ARRAY ? DT_IDENT_ARRAY : DT_IDENT_SCALAR,
DT_IDFLG_INLINE | DT_IDFLG_REF | DT_IDFLG_DECL | DT_IDFLG_ORPHAN, 0,
_dtrace_defattr, 0, &dt_idops_inline, inp, dtp->dt_gen);

if (idp == NULL) {
free(inp);
longjmp(yypcb->pcb_jmpbuf, EDT_NOMEM);
}






if (ddp->dd_kind == CTF_K_ARRAY) {
dt_idnode_t *pinp;
dt_ident_t *pidp;
dt_node_t *pnp;
uint_t i = 0;

for (pnp = ddp->dd_node; pnp != NULL; pnp = pnp->dn_list)
i++;

inp->din_hash = dt_idhash_create("inline args", NULL, 0, 0);
inp->din_argv = calloc(i, sizeof (dt_ident_t *));

if (inp->din_hash == NULL || inp->din_argv == NULL)
longjmp(yypcb->pcb_jmpbuf, EDT_NOMEM);








for (i = 0, pnp = ddp->dd_node;
pnp != NULL; pnp = pnp->dn_list, i++) {

if (pnp->dn_string == NULL)
continue;

if ((pinp = malloc(sizeof (dt_idnode_t))) == NULL)
longjmp(yypcb->pcb_jmpbuf, EDT_NOMEM);

pidp = dt_idhash_insert(inp->din_hash, pnp->dn_string,
DT_IDENT_SCALAR, DT_IDFLG_DECL | DT_IDFLG_INLINE, 0,
_dtrace_defattr, 0, &dt_idops_inline,
pinp, dtp->dt_gen);

if (pidp == NULL) {
free(pinp);
longjmp(yypcb->pcb_jmpbuf, EDT_NOMEM);
}

inp->din_argv[i] = pidp;
bzero(pinp, sizeof (dt_idnode_t));
dt_ident_type_assign(pidp, pnp->dn_ctfp, pnp->dn_type);
}

dt_idstack_push(&yypcb->pcb_globals, inp->din_hash);
}






expr = dt_node_cook(expr, DT_IDFLG_REF);

if (ddp->dd_kind == CTF_K_ARRAY)
dt_idstack_pop(&yypcb->pcb_globals, inp->din_hash);







if (dt_node_is_dynamic(expr))
rdp = dt_ident_resolve(expr->dn_ident);
else if (expr->dn_kind == DT_NODE_VAR || expr->dn_kind == DT_NODE_SYM)
rdp = expr->dn_ident;
else
rdp = NULL;

if (rdp != NULL) {
idp->di_flags |= (rdp->di_flags &
(DT_IDFLG_WRITE | DT_IDFLG_USER | DT_IDFLG_PRIM));
}

idp->di_attr = dt_attr_min(_dtrace_defattr, expr->dn_attr);
dt_ident_type_assign(idp, dtt.dtt_ctfp, dtt.dtt_type);
(void) dt_ident_cook(dnp, idp, &ddp->dd_node);






inp->din_list = yypcb->pcb_list;
inp->din_root = expr;

dt_decl_free(dt_decl_pop());
yybegin(YYS_CLAUSE);





dt_idhash_xinsert(dtp->dt_globals, idp);
return (dt_node_cook(dnp, DT_IDFLG_REF));
}

dt_node_t *
dt_node_member(dt_decl_t *ddp, char *name, dt_node_t *expr)
{
dtrace_typeinfo_t dtt;
dt_node_t *dnp;
int err;

if (ddp != NULL) {
err = dt_decl_type(ddp, &dtt);
dt_decl_free(ddp);

if (err != 0)
longjmp(yypcb->pcb_jmpbuf, EDT_COMPILER);
}

dnp = dt_node_alloc(DT_NODE_MEMBER);
dnp->dn_membname = name;
dnp->dn_membexpr = expr;

if (ddp != NULL)
dt_node_type_assign(dnp, dtt.dtt_ctfp, dtt.dtt_type,
dtt.dtt_flags);

return (dnp);
}

dt_node_t *
dt_node_xlator(dt_decl_t *ddp, dt_decl_t *sdp, char *name, dt_node_t *members)
{
dtrace_hdl_t *dtp = yypcb->pcb_hdl;
dtrace_typeinfo_t src, dst;
dt_node_t sn, dn;
dt_xlator_t *dxp;
dt_node_t *dnp;
int edst, esrc;
uint_t kind;

char n1[DT_TYPE_NAMELEN];
char n2[DT_TYPE_NAMELEN];

edst = dt_decl_type(ddp, &dst);
dt_decl_free(ddp);

esrc = dt_decl_type(sdp, &src);
dt_decl_free(sdp);

if (edst != 0 || esrc != 0) {
free(name);
longjmp(yypcb->pcb_jmpbuf, EDT_COMPILER);
}

bzero(&sn, sizeof (sn));
dt_node_type_assign(&sn, src.dtt_ctfp, src.dtt_type, B_FALSE);

bzero(&dn, sizeof (dn));
dt_node_type_assign(&dn, dst.dtt_ctfp, dst.dtt_type, B_FALSE);

if (dt_xlator_lookup(dtp, &sn, &dn, DT_XLATE_EXACT) != NULL) {
xyerror(D_XLATE_REDECL,
"translator from %s to %s has already been declared\n",
dt_node_type_name(&sn, n1, sizeof (n1)),
dt_node_type_name(&dn, n2, sizeof (n2)));
}

kind = ctf_type_kind(dst.dtt_ctfp,
ctf_type_resolve(dst.dtt_ctfp, dst.dtt_type));

if (kind == CTF_K_FORWARD) {
xyerror(D_XLATE_SOU, "incomplete struct/union/enum %s\n",
dt_type_name(dst.dtt_ctfp, dst.dtt_type, n1, sizeof (n1)));
}

if (kind != CTF_K_STRUCT && kind != CTF_K_UNION) {
xyerror(D_XLATE_SOU,
"translator output type must be a struct or union\n");
}

dxp = dt_xlator_create(dtp, &src, &dst, name, members, yypcb->pcb_list);
yybegin(YYS_CLAUSE);
free(name);

if (dxp == NULL)
longjmp(yypcb->pcb_jmpbuf, EDT_NOMEM);

dnp = dt_node_alloc(DT_NODE_XLATOR);
dnp->dn_xlator = dxp;
dnp->dn_members = members;

return (dt_node_cook(dnp, DT_IDFLG_REF));
}

dt_node_t *
dt_node_probe(char *s, int protoc, dt_node_t *nargs, dt_node_t *xargs)
{
dtrace_hdl_t *dtp = yypcb->pcb_hdl;
int nargc, xargc;
dt_node_t *dnp;

size_t len = strlen(s) + 3;
char *name = alloca(len);

(void) snprintf(name, len, "::%s", s);
(void) strhyphenate(name);
free(s);

if (strchr(name, '`') != NULL) {
xyerror(D_PROV_BADNAME, "probe name may not "
"contain scoping operator: %s\n", name);
}

if (strlen(name) - 2 >= DTRACE_NAMELEN) {
xyerror(D_PROV_BADNAME, "probe name may not exceed %d "
"characters: %s\n", DTRACE_NAMELEN - 1, name);
}

dnp = dt_node_alloc(DT_NODE_PROBE);

dnp->dn_ident = dt_ident_create(name, DT_IDENT_PROBE,
DT_IDFLG_ORPHAN, DTRACE_IDNONE, _dtrace_defattr, 0,
&dt_idops_probe, NULL, dtp->dt_gen);

nargc = dt_decl_prototype(nargs, nargs,
"probe input", DT_DP_VOID | DT_DP_ANON);

xargc = dt_decl_prototype(xargs, nargs,
"probe output", DT_DP_VOID);

if (nargc > UINT8_MAX) {
xyerror(D_PROV_PRARGLEN, "probe %s input prototype exceeds %u "
"parameters: %d params used\n", name, UINT8_MAX, nargc);
}

if (xargc > UINT8_MAX) {
xyerror(D_PROV_PRARGLEN, "probe %s output prototype exceeds %u "
"parameters: %d params used\n", name, UINT8_MAX, xargc);
}

if (dnp->dn_ident == NULL || dt_probe_create(dtp,
dnp->dn_ident, protoc, nargs, nargc, xargs, xargc) == NULL)
longjmp(yypcb->pcb_jmpbuf, EDT_NOMEM);

return (dnp);
}

dt_node_t *
dt_node_provider(char *name, dt_node_t *probes)
{
dtrace_hdl_t *dtp = yypcb->pcb_hdl;
dt_node_t *dnp = dt_node_alloc(DT_NODE_PROVIDER);
dt_node_t *lnp;
size_t len;

dnp->dn_provname = name;
dnp->dn_probes = probes;

if (strchr(name, '`') != NULL) {
dnerror(dnp, D_PROV_BADNAME, "provider name may not "
"contain scoping operator: %s\n", name);
}

if ((len = strlen(name)) >= DTRACE_PROVNAMELEN) {
dnerror(dnp, D_PROV_BADNAME, "provider name may not exceed %d "
"characters: %s\n", DTRACE_PROVNAMELEN - 1, name);
}

if (isdigit(name[len - 1])) {
dnerror(dnp, D_PROV_BADNAME, "provider name may not "
"end with a digit: %s\n", name);
}







if ((dnp->dn_provider = dt_provider_lookup(dtp, name)) != NULL)
dnp->dn_provred = B_TRUE;
else if ((dnp->dn_provider = dt_provider_create(dtp, name)) == NULL)
longjmp(yypcb->pcb_jmpbuf, EDT_NOMEM);
else
dnp->dn_provider->pv_flags |= DT_PROVIDER_INTF;









for (lnp = yypcb->pcb_list; lnp->dn_link != NULL; lnp = lnp->dn_link)
continue;

lnp->dn_link = dnp->dn_provider->pv_nodes;
dnp->dn_provider->pv_nodes = yypcb->pcb_list;

yybegin(YYS_CLAUSE);
return (dnp);
}

dt_node_t *
dt_node_program(dt_node_t *lnp)
{
dt_node_t *dnp = dt_node_alloc(DT_NODE_PROG);
dnp->dn_list = lnp;
return (dnp);
}









static void
dt_xcook_ident(dt_node_t *dnp, dt_idhash_t *dhp, uint_t idkind, int create)
{
dtrace_hdl_t *dtp = yypcb->pcb_hdl;
const char *sname = dt_idhash_name(dhp);
int uref = 0;

dtrace_attribute_t attr = _dtrace_defattr;
dt_ident_t *idp;
dtrace_syminfo_t dts;
GElf_Sym sym;

const char *scope, *mark;
uchar_t dnkind;
char *name;









if ((name = strrchr(dnp->dn_string, '`')) != NULL) {
if (name > dnp->dn_string && name[-1] == '`') {
uref++;
name[-1] = '\0';
}

if (name == dnp->dn_string + uref)
scope = uref ? DTRACE_OBJ_UMODS : DTRACE_OBJ_KMODS;
else
scope = dnp->dn_string;

*name++ = '\0';
dnkind = DT_NODE_VAR;

} else if (idkind == DT_IDENT_AGG) {
scope = DTRACE_OBJ_EXEC;
name = dnp->dn_string + 1;
dnkind = DT_NODE_AGG;
} else {
scope = DTRACE_OBJ_EXEC;
name = dnp->dn_string;
dnkind = DT_NODE_VAR;
}







(void) dt_set_errno(dtp, EDT_NOVAR);
mark = uref ? "``" : "`";

if (scope == DTRACE_OBJ_EXEC && (
(dhp != dtp->dt_globals &&
(idp = dt_idhash_lookup(dhp, name)) != NULL) ||
(dhp == dtp->dt_globals &&
(idp = dt_idstack_lookup(&yypcb->pcb_globals, name)) != NULL))) {






if (idp->di_kind != idkind && dhp == dtp->dt_globals) {
xyerror(D_IDENT_BADREF, "%s '%s' may not be referenced "
"as %s\n", dt_idkind_name(idp->di_kind),
idp->di_name, dt_idkind_name(idkind));
}






if (idp->di_kind != DT_IDENT_ARRAY &&
idp->di_kind != DT_IDENT_AGG)
attr = dt_ident_cook(dnp, idp, NULL);
else {
dt_node_type_assign(dnp,
DT_DYN_CTFP(dtp), DT_DYN_TYPE(dtp), B_FALSE);
attr = idp->di_attr;
}

free(dnp->dn_string);
dnp->dn_string = NULL;
dnp->dn_kind = dnkind;
dnp->dn_ident = idp;
dnp->dn_flags |= DT_NF_LVALUE;

if (idp->di_flags & DT_IDFLG_WRITE)
dnp->dn_flags |= DT_NF_WRITABLE;

dt_node_attr_assign(dnp, attr);

} else if (dhp == dtp->dt_globals && scope != DTRACE_OBJ_EXEC &&
dtrace_lookup_by_name(dtp, scope, name, &sym, &dts) == 0) {

dt_module_t *mp = dt_module_lookup_by_name(dtp, dts.dts_object);
int umod = (mp->dm_flags & DT_DM_KERNEL) == 0;
static const char *const kunames[] = { "kernel", "user" };

dtrace_typeinfo_t dtt;
dtrace_syminfo_t *sip;

if (uref ^ umod) {
xyerror(D_SYM_BADREF, "%s module '%s' symbol '%s' may "
"not be referenced as a %s symbol\n", kunames[umod],
dts.dts_object, dts.dts_name, kunames[uref]);
}

if (dtrace_symbol_type(dtp, &sym, &dts, &dtt) != 0) {




if (dtp->dt_errno == EDT_DATAMODEL) {
xyerror(D_SYM_MODEL, "cannot use %s symbol "
"%s%s%s in a %s D program\n",
dt_module_modelname(mp),
dts.dts_object, mark, dts.dts_name,
dt_module_modelname(dtp->dt_ddefs));
}

xyerror(D_SYM_NOTYPES,
"no symbolic type information is available for "
"%s%s%s: %s\n", dts.dts_object, mark, dts.dts_name,
dtrace_errmsg(dtp, dtrace_errno(dtp)));
}

idp = dt_ident_create(name, DT_IDENT_SYMBOL, 0, 0,
_dtrace_symattr, 0, &dt_idops_thaw, NULL, dtp->dt_gen);

if (idp == NULL)
longjmp(yypcb->pcb_jmpbuf, EDT_NOMEM);

if (mp->dm_flags & DT_DM_PRIMARY)
idp->di_flags |= DT_IDFLG_PRIM;

idp->di_next = dtp->dt_externs;
dtp->dt_externs = idp;

if ((sip = malloc(sizeof (dtrace_syminfo_t))) == NULL)
longjmp(yypcb->pcb_jmpbuf, EDT_NOMEM);

bcopy(&dts, sip, sizeof (dtrace_syminfo_t));
idp->di_data = sip;
idp->di_ctfp = dtt.dtt_ctfp;
idp->di_type = dtt.dtt_type;

free(dnp->dn_string);
dnp->dn_string = NULL;
dnp->dn_kind = DT_NODE_SYM;
dnp->dn_ident = idp;
dnp->dn_flags |= DT_NF_LVALUE;

dt_node_type_assign(dnp, dtt.dtt_ctfp, dtt.dtt_type,
dtt.dtt_flags);
dt_node_attr_assign(dnp, _dtrace_symattr);

if (uref) {
idp->di_flags |= DT_IDFLG_USER;
dnp->dn_flags |= DT_NF_USERLAND;
}

} else if (scope == DTRACE_OBJ_EXEC && create == B_TRUE) {
uint_t flags = DT_IDFLG_WRITE;
uint_t id;

if (dt_idhash_nextid(dhp, &id) == -1) {
xyerror(D_ID_OFLOW, "cannot create %s: limit on number "
"of %s variables exceeded\n", name, sname);
}

if (dhp == yypcb->pcb_locals)
flags |= DT_IDFLG_LOCAL;
else if (dhp == dtp->dt_tls)
flags |= DT_IDFLG_TLS;

dt_dprintf("create %s %s variable %s, id=%u\n",
sname, dt_idkind_name(idkind), name, id);

if (idkind == DT_IDENT_ARRAY || idkind == DT_IDENT_AGG) {
idp = dt_idhash_insert(dhp, name,
idkind, flags, id, _dtrace_defattr, 0,
&dt_idops_assc, NULL, dtp->dt_gen);
} else {
idp = dt_idhash_insert(dhp, name,
idkind, flags, id, _dtrace_defattr, 0,
&dt_idops_thaw, NULL, dtp->dt_gen);
}

if (idp == NULL)
longjmp(yypcb->pcb_jmpbuf, EDT_NOMEM);






if (idp->di_kind != DT_IDENT_ARRAY &&
idp->di_kind != DT_IDENT_AGG)
attr = dt_ident_cook(dnp, idp, NULL);
else {
dt_node_type_assign(dnp,
DT_DYN_CTFP(dtp), DT_DYN_TYPE(dtp), B_FALSE);
attr = idp->di_attr;
}

free(dnp->dn_string);
dnp->dn_string = NULL;
dnp->dn_kind = dnkind;
dnp->dn_ident = idp;
dnp->dn_flags |= DT_NF_LVALUE | DT_NF_WRITABLE;

dt_node_attr_assign(dnp, attr);

} else if (scope != DTRACE_OBJ_EXEC) {
xyerror(D_IDENT_UNDEF, "failed to resolve %s%s%s: %s\n",
dnp->dn_string, mark, name,
dtrace_errmsg(dtp, dtrace_errno(dtp)));
} else {
xyerror(D_IDENT_UNDEF, "failed to resolve %s: %s\n",
dnp->dn_string, dtrace_errmsg(dtp, dtrace_errno(dtp)));
}
}

static dt_node_t *
dt_cook_ident(dt_node_t *dnp, uint_t idflags)
{
dtrace_hdl_t *dtp = yypcb->pcb_hdl;

if (dnp->dn_op == DT_TOK_AGG)
dt_xcook_ident(dnp, dtp->dt_aggs, DT_IDENT_AGG, B_FALSE);
else
dt_xcook_ident(dnp, dtp->dt_globals, DT_IDENT_SCALAR, B_FALSE);

return (dt_node_cook(dnp, idflags));
}







static dt_node_t *
dt_cook_var(dt_node_t *dnp, uint_t idflags)
{
dt_ident_t *idp = dnp->dn_ident;

if ((idflags & DT_IDFLG_REF) && dt_ident_unref(idp)) {
dnerror(dnp, D_VAR_UNDEF,
"%s%s has not yet been declared or assigned\n",
(idp->di_flags & DT_IDFLG_LOCAL) ? "this->" :
(idp->di_flags & DT_IDFLG_TLS) ? "self->" : "",
idp->di_name);
}

dt_node_attr_assign(dnp, dt_ident_cook(dnp, idp, &dnp->dn_args));
return (dnp);
}


static dt_node_t *
dt_cook_func(dt_node_t *dnp, uint_t idflags)
{
dt_node_attr_assign(dnp,
dt_ident_cook(dnp, dnp->dn_ident, &dnp->dn_args));

return (dnp);
}

static dt_node_t *
dt_cook_op1(dt_node_t *dnp, uint_t idflags)
{
dtrace_hdl_t *dtp = yypcb->pcb_hdl;
dt_node_t *cp = dnp->dn_child;

char n[DT_TYPE_NAMELEN];
dtrace_typeinfo_t dtt;
dt_ident_t *idp;

ctf_encoding_t e;
ctf_arinfo_t r;
ctf_id_t type, base;
uint_t kind;

if (dnp->dn_op == DT_TOK_PREINC || dnp->dn_op == DT_TOK_POSTINC ||
dnp->dn_op == DT_TOK_PREDEC || dnp->dn_op == DT_TOK_POSTDEC)
idflags = DT_IDFLG_REF | DT_IDFLG_MOD;
else
idflags = DT_IDFLG_REF;





if (cp->dn_kind == DT_NODE_IDENT && (idflags & DT_IDFLG_MOD))
dt_xcook_ident(cp, dtp->dt_globals, DT_IDENT_SCALAR, B_TRUE);

cp = dnp->dn_child = dt_node_cook(cp, 0);

if (cp->dn_kind == DT_NODE_VAR && dt_ident_unref(cp->dn_ident)) {
if (dt_type_lookup("int64_t", &dtt) != 0)
xyerror(D_TYPE_ERR, "failed to lookup int64_t\n");

dt_ident_type_assign(cp->dn_ident, dtt.dtt_ctfp, dtt.dtt_type);
dt_node_type_assign(cp, dtt.dtt_ctfp, dtt.dtt_type,
dtt.dtt_flags);
}

if (cp->dn_kind == DT_NODE_VAR)
cp->dn_ident->di_flags |= idflags;

switch (dnp->dn_op) {
case DT_TOK_DEREF:




if ((idp = dt_node_resolve(cp, DT_IDENT_XLPTR)) != NULL) {
dt_xlator_t *dxp = idp->di_data;

dnp->dn_ident = &dxp->dx_souid;
dt_node_type_assign(dnp,
dnp->dn_ident->di_ctfp, dnp->dn_ident->di_type,
cp->dn_flags & DT_NF_USERLAND);
break;
}

type = ctf_type_resolve(cp->dn_ctfp, cp->dn_type);
kind = ctf_type_kind(cp->dn_ctfp, type);

if (kind == CTF_K_ARRAY) {
if (ctf_array_info(cp->dn_ctfp, type, &r) != 0) {
dtp->dt_ctferr = ctf_errno(cp->dn_ctfp);
longjmp(yypcb->pcb_jmpbuf, EDT_CTF);
} else
type = r.ctr_contents;
} else if (kind == CTF_K_POINTER) {
type = ctf_type_reference(cp->dn_ctfp, type);
} else {
xyerror(D_DEREF_NONPTR,
"cannot dereference non-pointer type\n");
}

dt_node_type_assign(dnp, cp->dn_ctfp, type,
cp->dn_flags & DT_NF_USERLAND);
base = ctf_type_resolve(cp->dn_ctfp, type);
kind = ctf_type_kind(cp->dn_ctfp, base);

if (kind == CTF_K_INTEGER && ctf_type_encoding(cp->dn_ctfp,
base, &e) == 0 && IS_VOID(e)) {
xyerror(D_DEREF_VOID,
"cannot dereference pointer to void\n");
}

if (kind == CTF_K_FUNCTION) {
xyerror(D_DEREF_FUNC,
"cannot dereference pointer to function\n");
}

if (kind != CTF_K_ARRAY || dt_node_is_string(dnp))
dnp->dn_flags |= DT_NF_LVALUE;








if ((cp->dn_flags & DT_NF_WRITABLE) ||
(cp->dn_kind == DT_NODE_OP2 && cp->dn_op == DT_TOK_ADD &&
(cp->dn_left->dn_flags & DT_NF_WRITABLE)))
dnp->dn_flags |= DT_NF_WRITABLE;

if ((cp->dn_flags & DT_NF_USERLAND) &&
(kind == CTF_K_POINTER || (dnp->dn_flags & DT_NF_REF)))
dnp->dn_flags |= DT_NF_USERLAND;
break;

case DT_TOK_IPOS:
case DT_TOK_INEG:
if (!dt_node_is_arith(cp)) {
xyerror(D_OP_ARITH, "operator %s requires an operand "
"of arithmetic type\n", opstr(dnp->dn_op));
}
dt_node_type_propagate(cp, dnp);
break;

case DT_TOK_BNEG:
if (!dt_node_is_integer(cp)) {
xyerror(D_OP_INT, "operator %s requires an operand of "
"integral type\n", opstr(dnp->dn_op));
}
dt_node_type_propagate(cp, dnp);
break;

case DT_TOK_LNEG:
if (!dt_node_is_scalar(cp)) {
xyerror(D_OP_SCALAR, "operator %s requires an operand "
"of scalar type\n", opstr(dnp->dn_op));
}
dt_node_type_assign(dnp, DT_INT_CTFP(dtp), DT_INT_TYPE(dtp),
B_FALSE);
break;

case DT_TOK_ADDROF:
if (cp->dn_kind == DT_NODE_VAR || cp->dn_kind == DT_NODE_AGG) {
xyerror(D_ADDROF_VAR,
"cannot take address of dynamic variable\n");
}

if (dt_node_is_dynamic(cp)) {
xyerror(D_ADDROF_VAR,
"cannot take address of dynamic object\n");
}

if (!(cp->dn_flags & DT_NF_LVALUE)) {
xyerror(D_ADDROF_LVAL,
"unacceptable operand for unary & operator\n");
}

if (cp->dn_flags & DT_NF_BITFIELD) {
xyerror(D_ADDROF_BITFIELD,
"cannot take address of bit-field\n");
}

dtt = (dtrace_typeinfo_t){
.dtt_ctfp = cp->dn_ctfp,
.dtt_type = cp->dn_type,
};

if (dt_type_pointer(&dtt) == -1) {
xyerror(D_TYPE_ERR, "cannot find type for \"&\": %s*\n",
dt_node_type_name(cp, n, sizeof (n)));
}

dt_node_type_assign(dnp, dtt.dtt_ctfp, dtt.dtt_type,
cp->dn_flags & DT_NF_USERLAND);
break;

case DT_TOK_SIZEOF:
if (cp->dn_flags & DT_NF_BITFIELD) {
xyerror(D_SIZEOF_BITFIELD,
"cannot apply sizeof to a bit-field\n");
}

if (dt_node_sizeof(cp) == 0) {
xyerror(D_SIZEOF_TYPE, "cannot apply sizeof to an "
"operand of unknown size\n");
}

dt_node_type_assign(dnp, dtp->dt_ddefs->dm_ctfp,
ctf_lookup_by_name(dtp->dt_ddefs->dm_ctfp, "size_t"),
B_FALSE);
break;

case DT_TOK_STRINGOF:
if (!dt_node_is_scalar(cp) && !dt_node_is_pointer(cp) &&
!dt_node_is_strcompat(cp)) {
xyerror(D_STRINGOF_TYPE,
"cannot apply stringof to a value of type %s\n",
dt_node_type_name(cp, n, sizeof (n)));
}
dt_node_type_assign(dnp, DT_STR_CTFP(dtp), DT_STR_TYPE(dtp),
cp->dn_flags & DT_NF_USERLAND);
break;

case DT_TOK_PREINC:
case DT_TOK_POSTINC:
case DT_TOK_PREDEC:
case DT_TOK_POSTDEC:
if (dt_node_is_scalar(cp) == 0) {
xyerror(D_OP_SCALAR, "operator %s requires operand of "
"scalar type\n", opstr(dnp->dn_op));
}

if (dt_node_is_vfptr(cp)) {
xyerror(D_OP_VFPTR, "operator %s requires an operand "
"of known size\n", opstr(dnp->dn_op));
}

if (!(cp->dn_flags & DT_NF_LVALUE)) {
xyerror(D_OP_LVAL, "operator %s requires modifiable "
"lvalue as an operand\n", opstr(dnp->dn_op));
}

if (!(cp->dn_flags & DT_NF_WRITABLE)) {
xyerror(D_OP_WRITE, "operator %s can only be applied "
"to a writable variable\n", opstr(dnp->dn_op));
}

dt_node_type_propagate(cp, dnp);
break;

default:
xyerror(D_UNKNOWN, "invalid unary op %s\n", opstr(dnp->dn_op));
}

dt_node_attr_assign(dnp, cp->dn_attr);
return (dnp);
}

static void
dt_assign_common(dt_node_t *dnp)
{
dt_node_t *lp = dnp->dn_left;
dt_node_t *rp = dnp->dn_right;
int op = dnp->dn_op;

if (rp->dn_kind == DT_NODE_INT)
dt_cast(lp, rp);

if (!(lp->dn_flags & DT_NF_LVALUE)) {
xyerror(D_OP_LVAL, "operator %s requires modifiable "
"lvalue as an operand\n", opstr(op));

}

if (!(lp->dn_flags & DT_NF_WRITABLE)) {
xyerror(D_OP_WRITE, "operator %s can only be applied "
"to a writable variable\n", opstr(op));
}

dt_node_type_propagate(lp, dnp);
dt_node_attr_assign(dnp, dt_attr_min(lp->dn_attr, rp->dn_attr));
}

static dt_node_t *
dt_cook_op2(dt_node_t *dnp, uint_t idflags)
{
dtrace_hdl_t *dtp = yypcb->pcb_hdl;
dt_node_t *lp = dnp->dn_left;
dt_node_t *rp = dnp->dn_right;
int op = dnp->dn_op;

ctf_membinfo_t m;
ctf_file_t *ctfp;
ctf_id_t type;
int kind, val, uref;
dt_ident_t *idp;

char n1[DT_TYPE_NAMELEN];
char n2[DT_TYPE_NAMELEN];








if (op == DT_TOK_LBRAC) {
if (lp->dn_kind == DT_NODE_IDENT) {
dt_idhash_t *dhp;
uint_t idkind;

if (lp->dn_op == DT_TOK_AGG) {
dhp = dtp->dt_aggs;
idp = dt_idhash_lookup(dhp, lp->dn_string + 1);
idkind = DT_IDENT_AGG;
} else {
dhp = dtp->dt_globals;
idp = dt_idstack_lookup(
&yypcb->pcb_globals, lp->dn_string);
idkind = DT_IDENT_ARRAY;
}

if (idp == NULL || dt_ident_unref(idp))
dt_xcook_ident(lp, dhp, idkind, B_TRUE);
else
dt_xcook_ident(lp, dhp, idp->di_kind, B_FALSE);
} else {
lp = dnp->dn_left = dt_node_cook(lp, 0);
}









if (lp->dn_kind == DT_NODE_VAR) {
if (lp->dn_ident->di_kind == DT_IDENT_ARRAY) {
if (lp->dn_args != NULL)
op = DT_TOK_ADD;
} else if (!dt_ident_unref(lp->dn_ident)) {
op = DT_TOK_ADD;
}
} else if (lp->dn_kind != DT_NODE_AGG) {
op = DT_TOK_ADD;
}
}

switch (op) {
case DT_TOK_BAND:
case DT_TOK_XOR:
case DT_TOK_BOR:
lp = dnp->dn_left = dt_node_cook(lp, DT_IDFLG_REF);
rp = dnp->dn_right = dt_node_cook(rp, DT_IDFLG_REF);

if (!dt_node_is_integer(lp) || !dt_node_is_integer(rp)) {
xyerror(D_OP_INT, "operator %s requires operands of "
"integral type\n", opstr(op));
}

dt_node_promote(lp, rp, dnp);
break;

case DT_TOK_LSH:
case DT_TOK_RSH:
lp = dnp->dn_left = dt_node_cook(lp, DT_IDFLG_REF);
rp = dnp->dn_right = dt_node_cook(rp, DT_IDFLG_REF);

if (!dt_node_is_integer(lp) || !dt_node_is_integer(rp)) {
xyerror(D_OP_INT, "operator %s requires operands of "
"integral type\n", opstr(op));
}

dt_node_type_propagate(lp, dnp);
dt_node_attr_assign(dnp, dt_attr_min(lp->dn_attr, rp->dn_attr));
break;

case DT_TOK_MOD:
lp = dnp->dn_left = dt_node_cook(lp, DT_IDFLG_REF);
rp = dnp->dn_right = dt_node_cook(rp, DT_IDFLG_REF);

if (!dt_node_is_integer(lp) || !dt_node_is_integer(rp)) {
xyerror(D_OP_INT, "operator %s requires operands of "
"integral type\n", opstr(op));
}

dt_node_promote(lp, rp, dnp);
break;

case DT_TOK_MUL:
case DT_TOK_DIV:
lp = dnp->dn_left = dt_node_cook(lp, DT_IDFLG_REF);
rp = dnp->dn_right = dt_node_cook(rp, DT_IDFLG_REF);

if (!dt_node_is_arith(lp) || !dt_node_is_arith(rp)) {
xyerror(D_OP_ARITH, "operator %s requires operands of "
"arithmetic type\n", opstr(op));
}

dt_node_promote(lp, rp, dnp);
break;

case DT_TOK_LAND:
case DT_TOK_LXOR:
case DT_TOK_LOR:
lp = dnp->dn_left = dt_node_cook(lp, DT_IDFLG_REF);
rp = dnp->dn_right = dt_node_cook(rp, DT_IDFLG_REF);

if (!dt_node_is_scalar(lp) || !dt_node_is_scalar(rp)) {
xyerror(D_OP_SCALAR, "operator %s requires operands "
"of scalar type\n", opstr(op));
}

dt_node_type_assign(dnp, DT_INT_CTFP(dtp), DT_INT_TYPE(dtp),
B_FALSE);
dt_node_attr_assign(dnp, dt_attr_min(lp->dn_attr, rp->dn_attr));
break;

case DT_TOK_LT:
case DT_TOK_LE:
case DT_TOK_GT:
case DT_TOK_GE:
case DT_TOK_EQU:
case DT_TOK_NEQ:








lp = dnp->dn_left = dt_node_cook(lp, DT_IDFLG_REF);

kind = ctf_type_kind(lp->dn_ctfp,
ctf_type_resolve(lp->dn_ctfp, lp->dn_type));

if (kind == CTF_K_ENUM && rp->dn_kind == DT_NODE_IDENT &&
strchr(rp->dn_string, '`') == NULL && ctf_enum_value(
lp->dn_ctfp, lp->dn_type, rp->dn_string, &val) == 0) {

if ((idp = dt_idstack_lookup(&yypcb->pcb_globals,
rp->dn_string)) != NULL) {
xyerror(D_IDENT_AMBIG,
"ambiguous use of operator %s: %s is "
"both a %s enum tag and a global %s\n",
opstr(op), rp->dn_string,
dt_node_type_name(lp, n1, sizeof (n1)),
dt_idkind_name(idp->di_kind));
}

free(rp->dn_string);
rp->dn_string = NULL;
rp->dn_kind = DT_NODE_INT;
rp->dn_flags |= DT_NF_COOKED;
rp->dn_op = DT_TOK_INT;
rp->dn_value = (intmax_t)val;

dt_node_type_assign(rp, lp->dn_ctfp, lp->dn_type,
B_FALSE);
dt_node_attr_assign(rp, _dtrace_symattr);
}

rp = dnp->dn_right = dt_node_cook(rp, DT_IDFLG_REF);












if (ctf_type_compat(lp->dn_ctfp, lp->dn_type,
rp->dn_ctfp, rp->dn_type))
;
else if (dt_node_is_integer(lp) && dt_node_is_integer(rp))
;
else if (dt_node_is_strcompat(lp) && dt_node_is_strcompat(rp) &&
(dt_node_is_string(lp) || dt_node_is_string(rp)))
;
else if (dt_node_is_ptrcompat(lp, rp, NULL, NULL) == 0) {
xyerror(D_OP_INCOMPAT, "operands have "
"incompatible types: \"%s\" %s \"%s\"\n",
dt_node_type_name(lp, n1, sizeof (n1)), opstr(op),
dt_node_type_name(rp, n2, sizeof (n2)));
}

dt_node_type_assign(dnp, DT_INT_CTFP(dtp), DT_INT_TYPE(dtp),
B_FALSE);
dt_node_attr_assign(dnp, dt_attr_min(lp->dn_attr, rp->dn_attr));
break;

case DT_TOK_ADD:
case DT_TOK_SUB: {






int lp_is_ptr, lp_is_int, rp_is_ptr, rp_is_int;

lp = dnp->dn_left = dt_node_cook(lp, DT_IDFLG_REF);
rp = dnp->dn_right = dt_node_cook(rp, DT_IDFLG_REF);

lp_is_ptr = dt_node_is_string(lp) ||
(dt_node_is_pointer(lp) && !dt_node_is_vfptr(lp));
lp_is_int = dt_node_is_integer(lp);

rp_is_ptr = dt_node_is_string(rp) ||
(dt_node_is_pointer(rp) && !dt_node_is_vfptr(rp));
rp_is_int = dt_node_is_integer(rp);

if (lp_is_int && rp_is_int) {
dt_type_promote(lp, rp, &ctfp, &type);
uref = 0;
} else if (lp_is_ptr && rp_is_int) {
ctfp = lp->dn_ctfp;
type = lp->dn_type;
uref = lp->dn_flags & DT_NF_USERLAND;
} else if (lp_is_int && rp_is_ptr && op == DT_TOK_ADD) {
ctfp = rp->dn_ctfp;
type = rp->dn_type;
uref = rp->dn_flags & DT_NF_USERLAND;
} else if (lp_is_ptr && rp_is_ptr && op == DT_TOK_SUB &&
dt_node_is_ptrcompat(lp, rp, NULL, NULL)) {
ctfp = dtp->dt_ddefs->dm_ctfp;
type = ctf_lookup_by_name(ctfp, "ptrdiff_t");
uref = 0;
} else {
xyerror(D_OP_INCOMPAT, "operands have incompatible "
"types: \"%s\" %s \"%s\"\n",
dt_node_type_name(lp, n1, sizeof (n1)), opstr(op),
dt_node_type_name(rp, n2, sizeof (n2)));
}

dt_node_type_assign(dnp, ctfp, type, B_FALSE);
dt_node_attr_assign(dnp, dt_attr_min(lp->dn_attr, rp->dn_attr));

if (uref)
dnp->dn_flags |= DT_NF_USERLAND;
break;
}

case DT_TOK_OR_EQ:
case DT_TOK_XOR_EQ:
case DT_TOK_AND_EQ:
case DT_TOK_LSH_EQ:
case DT_TOK_RSH_EQ:
case DT_TOK_MOD_EQ:
if (lp->dn_kind == DT_NODE_IDENT) {
dt_xcook_ident(lp, dtp->dt_globals,
DT_IDENT_SCALAR, B_TRUE);
}

lp = dnp->dn_left =
dt_node_cook(lp, DT_IDFLG_REF | DT_IDFLG_MOD);

rp = dnp->dn_right =
dt_node_cook(rp, DT_IDFLG_REF | DT_IDFLG_MOD);

if (!dt_node_is_integer(lp) || !dt_node_is_integer(rp)) {
xyerror(D_OP_INT, "operator %s requires operands of "
"integral type\n", opstr(op));
}
goto asgn_common;

case DT_TOK_MUL_EQ:
case DT_TOK_DIV_EQ:
if (lp->dn_kind == DT_NODE_IDENT) {
dt_xcook_ident(lp, dtp->dt_globals,
DT_IDENT_SCALAR, B_TRUE);
}

lp = dnp->dn_left =
dt_node_cook(lp, DT_IDFLG_REF | DT_IDFLG_MOD);

rp = dnp->dn_right =
dt_node_cook(rp, DT_IDFLG_REF | DT_IDFLG_MOD);

if (!dt_node_is_arith(lp) || !dt_node_is_arith(rp)) {
xyerror(D_OP_ARITH, "operator %s requires operands of "
"arithmetic type\n", opstr(op));
}
goto asgn_common;

case DT_TOK_ASGN:






if (lp->dn_kind == DT_NODE_IDENT) {
if (lp->dn_op == DT_TOK_AGG) {
dt_xcook_ident(lp, dtp->dt_aggs,
DT_IDENT_AGG, B_TRUE);
} else {
dt_xcook_ident(lp, dtp->dt_globals,
DT_IDENT_SCALAR, B_TRUE);
}
}

lp = dnp->dn_left = dt_node_cook(lp, 0);
rp = dnp->dn_right = dt_node_cook(rp, DT_IDFLG_REF);










if (lp->dn_kind == DT_NODE_AGG) {
const char *aname = lp->dn_ident->di_name;
dt_ident_t *oid = lp->dn_ident->di_iarg;

if (rp->dn_kind != DT_NODE_FUNC ||
rp->dn_ident->di_kind != DT_IDENT_AGGFUNC) {
xyerror(D_AGG_FUNC,
"@%s must be assigned the result of "
"an aggregating function\n", aname);
}

if (oid != NULL && oid != rp->dn_ident) {
xyerror(D_AGG_REDEF,
"aggregation redefined: @%s\n\t "
"current: @%s = %s( )\n\tprevious: @%s = "
"%s( ) : line %d\n", aname, aname,
rp->dn_ident->di_name, aname, oid->di_name,
lp->dn_ident->di_lineno);
} else if (oid == NULL)
lp->dn_ident->di_iarg = rp->dn_ident;







if (lp->dn_aggfun != NULL) {
xyerror(D_OP_LVAL, "operator = requires "
"modifiable lvalue as an operand\n");
}

lp->dn_aggfun = rp;
lp = dt_node_cook(lp, DT_IDFLG_MOD);

dnp->dn_left = dnp->dn_right = NULL;
dt_node_free(dnp);

return (lp);
}





if ((idp = dt_node_resolve(rp, DT_IDENT_XLSOU)) != NULL) {
ctfp = idp->di_ctfp;
type = idp->di_type;
uref = idp->di_flags & DT_IDFLG_USER;
} else {
ctfp = rp->dn_ctfp;
type = rp->dn_type;
uref = rp->dn_flags & DT_NF_USERLAND;
}






if (lp->dn_kind == DT_NODE_VAR &&
dt_ident_unref(lp->dn_ident)) {
dt_node_type_assign(lp, ctfp, type, B_FALSE);
dt_ident_type_assign(lp->dn_ident, ctfp, type);

if (uref) {
lp->dn_flags |= DT_NF_USERLAND;
lp->dn_ident->di_flags |= DT_IDFLG_USER;
}
}

if (lp->dn_kind == DT_NODE_VAR)
lp->dn_ident->di_flags |= DT_IDFLG_MOD;






if (!dt_node_is_string(lp)) {
kind = ctf_type_kind(lp->dn_ctfp,
ctf_type_resolve(lp->dn_ctfp, lp->dn_type));

if (kind == CTF_K_ARRAY || kind == CTF_K_FUNCTION) {
xyerror(D_OP_ARRFUN, "operator %s may not be "
"applied to operand of type \"%s\"\n",
opstr(op),
dt_node_type_name(lp, n1, sizeof (n1)));
}
}

if (idp != NULL && idp->di_kind == DT_IDENT_XLSOU &&
ctf_type_compat(lp->dn_ctfp, lp->dn_type, ctfp, type))
goto asgn_common;

if (dt_node_is_argcompat(lp, rp))
goto asgn_common;

xyerror(D_OP_INCOMPAT,
"operands have incompatible types: \"%s\" %s \"%s\"\n",
dt_node_type_name(lp, n1, sizeof (n1)), opstr(op),
dt_node_type_name(rp, n2, sizeof (n2)));


case DT_TOK_ADD_EQ:
case DT_TOK_SUB_EQ:
if (lp->dn_kind == DT_NODE_IDENT) {
dt_xcook_ident(lp, dtp->dt_globals,
DT_IDENT_SCALAR, B_TRUE);
}

lp = dnp->dn_left =
dt_node_cook(lp, DT_IDFLG_REF | DT_IDFLG_MOD);

rp = dnp->dn_right =
dt_node_cook(rp, DT_IDFLG_REF | DT_IDFLG_MOD);

if (dt_node_is_string(lp) || dt_node_is_string(rp)) {
xyerror(D_OP_INCOMPAT, "operands have "
"incompatible types: \"%s\" %s \"%s\"\n",
dt_node_type_name(lp, n1, sizeof (n1)), opstr(op),
dt_node_type_name(rp, n2, sizeof (n2)));
}






if (dt_node_is_integer(lp) == 0 ||
dt_node_is_integer(rp) == 0) {
if (!dt_node_is_pointer(lp) || dt_node_is_vfptr(lp)) {
xyerror(D_OP_VFPTR,
"operator %s requires left-hand scalar "
"operand of known size\n", opstr(op));
} else if (dt_node_is_integer(rp) == 0 &&
dt_node_is_ptrcompat(lp, rp, NULL, NULL) == 0) {
xyerror(D_OP_INCOMPAT, "operands have "
"incompatible types: \"%s\" %s \"%s\"\n",
dt_node_type_name(lp, n1, sizeof (n1)),
opstr(op),
dt_node_type_name(rp, n2, sizeof (n2)));
}
}
asgn_common:
dt_assign_common(dnp);
break;

case DT_TOK_PTR:





if (lp->dn_kind == DT_NODE_IDENT) {
dt_idhash_t *dhp = NULL;

if (strcmp(lp->dn_string, "self") == 0) {
dhp = dtp->dt_tls;
} else if (strcmp(lp->dn_string, "this") == 0) {
dhp = yypcb->pcb_locals;
}
if (dhp != NULL) {
if (rp->dn_kind != DT_NODE_VAR) {
dt_xcook_ident(rp, dhp,
DT_IDENT_SCALAR, B_TRUE);
}

if (idflags != 0)
rp = dt_node_cook(rp, idflags);


dnp->dn_right = dnp->dn_left;
dt_node_free(dnp);
return (rp);
}
}

case DT_TOK_DOT:
lp = dnp->dn_left = dt_node_cook(lp, DT_IDFLG_REF);

if (rp->dn_kind != DT_NODE_IDENT) {
xyerror(D_OP_IDENT, "operator %s must be followed by "
"an identifier\n", opstr(op));
}

if ((idp = dt_node_resolve(lp, DT_IDENT_XLSOU)) != NULL ||
(idp = dt_node_resolve(lp, DT_IDENT_XLPTR)) != NULL) {




dt_xlator_t *dxp = idp->di_data;

if (dt_xlator_member(dxp, rp->dn_string) == NULL) {
xyerror(D_XLATE_NOCONV,
"translator does not define conversion "
"for member: %s\n", rp->dn_string);
}

ctfp = idp->di_ctfp;
type = ctf_type_resolve(ctfp, idp->di_type);
uref = idp->di_flags & DT_IDFLG_USER;
} else {
ctfp = lp->dn_ctfp;
type = ctf_type_resolve(ctfp, lp->dn_type);
uref = lp->dn_flags & DT_NF_USERLAND;
}

kind = ctf_type_kind(ctfp, type);

if (op == DT_TOK_PTR) {
if (kind != CTF_K_POINTER) {
xyerror(D_OP_PTR, "operator %s must be "
"applied to a pointer\n", opstr(op));
}
type = ctf_type_reference(ctfp, type);
type = ctf_type_resolve(ctfp, type);
kind = ctf_type_kind(ctfp, type);
}





while (kind == CTF_K_FORWARD) {
char *tag = ctf_type_name(ctfp, type, n1, sizeof (n1));
dtrace_typeinfo_t dtt;

if (tag != NULL && dt_type_lookup(tag, &dtt) == 0 &&
(dtt.dtt_ctfp != ctfp || dtt.dtt_type != type)) {
ctfp = dtt.dtt_ctfp;
type = ctf_type_resolve(ctfp, dtt.dtt_type);
kind = ctf_type_kind(ctfp, type);
} else {
xyerror(D_OP_INCOMPLETE,
"operator %s cannot be applied to a "
"forward declaration: no %s definition "
"is available\n", opstr(op), tag);
}
}

if (kind != CTF_K_STRUCT && kind != CTF_K_UNION) {
if (op == DT_TOK_PTR) {
xyerror(D_OP_SOU, "operator -> cannot be "
"applied to pointer to type \"%s\"; must "
"be applied to a struct or union pointer\n",
ctf_type_name(ctfp, type, n1, sizeof (n1)));
} else {
xyerror(D_OP_SOU, "operator %s cannot be "
"applied to type \"%s\"; must be applied "
"to a struct or union\n", opstr(op),
ctf_type_name(ctfp, type, n1, sizeof (n1)));
}
}

if (ctf_member_info(ctfp, type, rp->dn_string, &m) == CTF_ERR) {
xyerror(D_TYPE_MEMBER,
"%s is not a member of %s\n", rp->dn_string,
ctf_type_name(ctfp, type, n1, sizeof (n1)));
}

type = ctf_type_resolve(ctfp, m.ctm_type);
kind = ctf_type_kind(ctfp, type);

dt_node_type_assign(dnp, ctfp, m.ctm_type, B_FALSE);
dt_node_attr_assign(dnp, lp->dn_attr);

if (op == DT_TOK_PTR && (kind != CTF_K_ARRAY ||
dt_node_is_string(dnp)))
dnp->dn_flags |= DT_NF_LVALUE;

if (op == DT_TOK_DOT && (lp->dn_flags & DT_NF_LVALUE) &&
(kind != CTF_K_ARRAY || dt_node_is_string(dnp)))
dnp->dn_flags |= DT_NF_LVALUE;

if (lp->dn_flags & DT_NF_WRITABLE)
dnp->dn_flags |= DT_NF_WRITABLE;

if (uref && (kind == CTF_K_POINTER ||
(dnp->dn_flags & DT_NF_REF)))
dnp->dn_flags |= DT_NF_USERLAND;
break;

case DT_TOK_LBRAC: {




dt_node_t *lnp;








if (lp->dn_kind == DT_NODE_AGG) {
if (lp->dn_aggtup != NULL) {
xyerror(D_AGG_MDIM, "improper attempt to "
"reference @%s as a multi-dimensional "
"array\n", lp->dn_ident->di_name);
}

lp->dn_aggtup = rp;
lp = dt_node_cook(lp, 0);

dnp->dn_left = dnp->dn_right = NULL;
dt_node_free(dnp);

return (lp);
}

assert(lp->dn_kind == DT_NODE_VAR);
idp = lp->dn_ident;






if (idp->di_kind == DT_IDENT_SCALAR && dt_ident_unref(idp) &&
(idp->di_flags & (DT_IDFLG_LOCAL | DT_IDFLG_TLS)) != 0) {

if (idp->di_flags & DT_IDFLG_LOCAL) {
xyerror(D_ARR_LOCAL,
"local variables may not be used as "
"associative arrays: %s\n", idp->di_name);
}

dt_dprintf("morph variable %s (id %u) from scalar to "
"array\n", idp->di_name, idp->di_id);

dt_ident_morph(idp, DT_IDENT_ARRAY,
&dt_idops_assc, NULL);
}

if (idp->di_kind != DT_IDENT_ARRAY) {
xyerror(D_IDENT_BADREF, "%s '%s' may not be referenced "
"as %s\n", dt_idkind_name(idp->di_kind),
idp->di_name, dt_idkind_name(DT_IDENT_ARRAY));
}



















assert(lp->dn_kind == DT_NODE_VAR);
assert(lp->dn_args == NULL);

lnp = dnp->dn_link;
bcopy(lp, dnp, sizeof (dt_node_t));
dnp->dn_link = lnp;

dnp->dn_args = rp;
dnp->dn_list = NULL;

dt_node_free(lp);
return (dt_node_cook(dnp, idflags));
}

case DT_TOK_XLATE: {
dt_xlator_t *dxp;

assert(lp->dn_kind == DT_NODE_TYPE);
rp = dnp->dn_right = dt_node_cook(rp, DT_IDFLG_REF);
dxp = dt_xlator_lookup(dtp, rp, lp, DT_XLATE_FUZZY);

if (dxp == NULL) {
xyerror(D_XLATE_NONE,
"cannot translate from \"%s\" to \"%s\"\n",
dt_node_type_name(rp, n1, sizeof (n1)),
dt_node_type_name(lp, n2, sizeof (n2)));
}

dnp->dn_ident = dt_xlator_ident(dxp, lp->dn_ctfp, lp->dn_type);
dt_node_type_assign(dnp, DT_DYN_CTFP(dtp), DT_DYN_TYPE(dtp),
B_FALSE);
dt_node_attr_assign(dnp,
dt_attr_min(rp->dn_attr, dnp->dn_ident->di_attr));
break;
}

case DT_TOK_LPAR: {
ctf_id_t ltype, rtype;
uint_t lkind, rkind;

assert(lp->dn_kind == DT_NODE_TYPE);
rp = dnp->dn_right = dt_node_cook(rp, DT_IDFLG_REF);

ltype = ctf_type_resolve(lp->dn_ctfp, lp->dn_type);
lkind = ctf_type_kind(lp->dn_ctfp, ltype);

rtype = ctf_type_resolve(rp->dn_ctfp, rp->dn_type);
rkind = ctf_type_kind(rp->dn_ctfp, rtype);








if (ctf_type_compat(lp->dn_ctfp, lp->dn_type,
rp->dn_ctfp, rp->dn_type))
;
else if (dt_node_is_scalar(lp) &&
(dt_node_is_scalar(rp) || rkind == CTF_K_FUNCTION))
;
else if (dt_node_is_void(lp))
;
else if (lkind == CTF_K_POINTER && dt_node_is_pointer(rp))
;
else if (dt_node_is_string(lp) && (dt_node_is_scalar(rp) ||
dt_node_is_pointer(rp) || dt_node_is_strcompat(rp)))
;
else {
xyerror(D_CAST_INVAL,
"invalid cast expression: \"%s\" to \"%s\"\n",
dt_node_type_name(rp, n1, sizeof (n1)),
dt_node_type_name(lp, n2, sizeof (n2)));
}

dt_node_type_propagate(lp, dnp);
dt_node_attr_assign(dnp, dt_attr_min(lp->dn_attr, rp->dn_attr));





if (lkind == CTF_K_POINTER)
dnp->dn_flags |= DT_NF_WRITABLE;

break;
}

case DT_TOK_COMMA:
lp = dnp->dn_left = dt_node_cook(lp, DT_IDFLG_REF);
rp = dnp->dn_right = dt_node_cook(rp, DT_IDFLG_REF);

if (dt_node_is_dynamic(lp) || dt_node_is_dynamic(rp)) {
xyerror(D_OP_DYN, "operator %s operands "
"cannot be of dynamic type\n", opstr(op));
}

if (dt_node_is_actfunc(lp) || dt_node_is_actfunc(rp)) {
xyerror(D_OP_ACT, "operator %s operands "
"cannot be actions\n", opstr(op));
}

dt_node_type_propagate(rp, dnp);
dt_node_attr_assign(dnp, dt_attr_min(lp->dn_attr, rp->dn_attr));
break;

default:
xyerror(D_UNKNOWN, "invalid binary op %s\n", opstr(op));
}








if (dnp->dn_op == DT_TOK_LBRAC && op == DT_TOK_ADD) {
dt_node_t *pnp;

if (rp->dn_list != NULL) {
xyerror(D_ARR_BADREF,
"cannot access %s as an associative array\n",
dt_node_name(lp, n1, sizeof (n1)));
}

dnp->dn_op = DT_TOK_ADD;
pnp = dt_node_op1(DT_TOK_DEREF, dnp);







assert(yypcb->pcb_list == pnp);
yypcb->pcb_list = pnp->dn_link;

pnp->dn_link = dnp->dn_link;
dnp->dn_link = pnp;

return (dt_node_cook(pnp, DT_IDFLG_REF));
}

return (dnp);
}


static dt_node_t *
dt_cook_op3(dt_node_t *dnp, uint_t idflags)
{
dt_node_t *lp, *rp;
ctf_file_t *ctfp;
ctf_id_t type;

dnp->dn_expr = dt_node_cook(dnp->dn_expr, DT_IDFLG_REF);
lp = dnp->dn_left = dt_node_cook(dnp->dn_left, DT_IDFLG_REF);
rp = dnp->dn_right = dt_node_cook(dnp->dn_right, DT_IDFLG_REF);

if (!dt_node_is_scalar(dnp->dn_expr)) {
xyerror(D_OP_SCALAR,
"operator ?: expression must be of scalar type\n");
}

if (dt_node_is_dynamic(lp) || dt_node_is_dynamic(rp)) {
xyerror(D_OP_DYN,
"operator ?: operands cannot be of dynamic type\n");
}






if (ctf_type_compat(lp->dn_ctfp, lp->dn_type,
rp->dn_ctfp, rp->dn_type)) {
ctfp = lp->dn_ctfp;
type = lp->dn_type;
} else if (dt_node_is_integer(lp) && dt_node_is_integer(rp)) {
dt_type_promote(lp, rp, &ctfp, &type);
} else if (dt_node_is_strcompat(lp) && dt_node_is_strcompat(rp) &&
(dt_node_is_string(lp) || dt_node_is_string(rp))) {
ctfp = DT_STR_CTFP(yypcb->pcb_hdl);
type = DT_STR_TYPE(yypcb->pcb_hdl);
} else if (dt_node_is_ptrcompat(lp, rp, &ctfp, &type) == 0) {
xyerror(D_OP_INCOMPAT,
"operator ?: operands must have compatible types\n");
}

if (dt_node_is_actfunc(lp) || dt_node_is_actfunc(rp)) {
xyerror(D_OP_ACT, "action cannot be "
"used in a conditional context\n");
}

dt_node_type_assign(dnp, ctfp, type, B_FALSE);
dt_node_attr_assign(dnp, dt_attr_min(dnp->dn_expr->dn_attr,
dt_attr_min(lp->dn_attr, rp->dn_attr)));

return (dnp);
}

static dt_node_t *
dt_cook_statement(dt_node_t *dnp, uint_t idflags)
{
dnp->dn_expr = dt_node_cook(dnp->dn_expr, idflags);
dt_node_attr_assign(dnp, dnp->dn_expr->dn_attr);

return (dnp);
}








static dt_node_t *
dt_cook_aggregation(dt_node_t *dnp, uint_t idflags)
{
dtrace_hdl_t *dtp = yypcb->pcb_hdl;

if (dnp->dn_aggfun != NULL) {
dnp->dn_aggfun = dt_node_cook(dnp->dn_aggfun, DT_IDFLG_REF);
dt_node_attr_assign(dnp, dt_ident_cook(dnp,
dnp->dn_ident, &dnp->dn_aggtup));
} else {
dt_node_type_assign(dnp, DT_DYN_CTFP(dtp), DT_DYN_TYPE(dtp),
B_FALSE);
dt_node_attr_assign(dnp, dnp->dn_ident->di_attr);
}

return (dnp);
}












































static dt_node_t *
dt_cook_clause(dt_node_t *dnp, uint_t idflags)
{
volatile int err, tries;
jmp_buf ojb;





dt_node_attr_assign(dnp, yypcb->pcb_pinfo.dtp_attr);
dnp->dn_ctxattr = yypcb->pcb_pinfo.dtp_attr;

bcopy(yypcb->pcb_jmpbuf, ojb, sizeof (jmp_buf));
tries = 0;

if (dnp->dn_pred != NULL && (err = setjmp(yypcb->pcb_jmpbuf)) != 0) {
bcopy(ojb, yypcb->pcb_jmpbuf, sizeof (jmp_buf));
if (tries++ != 0 || err != EDT_COMPILER || (
yypcb->pcb_hdl->dt_errtag != dt_errtag(D_IDENT_UNDEF) &&
yypcb->pcb_hdl->dt_errtag != dt_errtag(D_VAR_UNDEF)))
longjmp(yypcb->pcb_jmpbuf, err);
}

if (tries == 0) {
yylabel("action list");

dt_node_attr_assign(dnp,
dt_node_list_cook(&dnp->dn_acts, idflags));

bcopy(ojb, yypcb->pcb_jmpbuf, sizeof (jmp_buf));
yylabel(NULL);
}

if (dnp->dn_pred != NULL) {
yylabel("predicate");

dnp->dn_pred = dt_node_cook(dnp->dn_pred, idflags);
dt_node_attr_assign(dnp,
dt_attr_min(dnp->dn_attr, dnp->dn_pred->dn_attr));

if (!dt_node_is_scalar(dnp->dn_pred)) {
xyerror(D_PRED_SCALAR,
"predicate result must be of scalar type\n");
}

yylabel(NULL);
}

if (tries != 0) {
yylabel("action list");

dt_node_attr_assign(dnp,
dt_node_list_cook(&dnp->dn_acts, idflags));

yylabel(NULL);
}

return (dnp);
}


static dt_node_t *
dt_cook_inline(dt_node_t *dnp, uint_t idflags)
{
dt_idnode_t *inp = dnp->dn_ident->di_iarg;
dt_ident_t *rdp;

char n1[DT_TYPE_NAMELEN];
char n2[DT_TYPE_NAMELEN];

assert(dnp->dn_ident->di_flags & DT_IDFLG_INLINE);
assert(inp->din_root->dn_flags & DT_NF_COOKED);






if ((rdp = dt_node_resolve(inp->din_root, DT_IDENT_XLSOU)) != NULL ||
(rdp = dt_node_resolve(inp->din_root, DT_IDENT_XLPTR)) != NULL) {

ctf_file_t *lctfp = dnp->dn_ctfp;
ctf_id_t ltype = ctf_type_resolve(lctfp, dnp->dn_type);

dt_xlator_t *dxp = rdp->di_data;
ctf_file_t *rctfp = dxp->dx_dst_ctfp;
ctf_id_t rtype = dxp->dx_dst_base;

if (ctf_type_kind(lctfp, ltype) == CTF_K_POINTER) {
ltype = ctf_type_reference(lctfp, ltype);
ltype = ctf_type_resolve(lctfp, ltype);
}

if (ctf_type_compat(lctfp, ltype, rctfp, rtype) == 0) {
dnerror(dnp, D_OP_INCOMPAT,
"inline %s definition uses incompatible types: "
"\"%s\" = \"%s\"\n", dnp->dn_ident->di_name,
dt_type_name(lctfp, ltype, n1, sizeof (n1)),
dt_type_name(rctfp, rtype, n2, sizeof (n2)));
}

} else if (dt_node_is_argcompat(dnp, inp->din_root) == 0) {
dnerror(dnp, D_OP_INCOMPAT,
"inline %s definition uses incompatible types: "
"\"%s\" = \"%s\"\n", dnp->dn_ident->di_name,
dt_node_type_name(dnp, n1, sizeof (n1)),
dt_node_type_name(inp->din_root, n2, sizeof (n2)));
}

return (dnp);
}

static dt_node_t *
dt_cook_member(dt_node_t *dnp, uint_t idflags)
{
dnp->dn_membexpr = dt_node_cook(dnp->dn_membexpr, idflags);
dt_node_attr_assign(dnp, dnp->dn_membexpr->dn_attr);
return (dnp);
}


static dt_node_t *
dt_cook_xlator(dt_node_t *dnp, uint_t idflags)
{
dtrace_hdl_t *dtp = yypcb->pcb_hdl;
dt_xlator_t *dxp = dnp->dn_xlator;
dt_node_t *mnp;

char n1[DT_TYPE_NAMELEN];
char n2[DT_TYPE_NAMELEN];

dtrace_attribute_t attr = _dtrace_maxattr;
ctf_membinfo_t ctm;






dt_idstack_push(&yypcb->pcb_globals, dxp->dx_locals);

for (mnp = dnp->dn_members; mnp != NULL; mnp = mnp->dn_list) {
if (ctf_member_info(dxp->dx_dst_ctfp, dxp->dx_dst_type,
mnp->dn_membname, &ctm) == CTF_ERR) {
xyerror(D_XLATE_MEMB,
"translator member %s is not a member of %s\n",
mnp->dn_membname, ctf_type_name(dxp->dx_dst_ctfp,
dxp->dx_dst_type, n1, sizeof (n1)));
}

(void) dt_node_cook(mnp, DT_IDFLG_REF);
dt_node_type_assign(mnp, dxp->dx_dst_ctfp, ctm.ctm_type,
B_FALSE);
attr = dt_attr_min(attr, mnp->dn_attr);

if (dt_node_is_argcompat(mnp, mnp->dn_membexpr) == 0) {
xyerror(D_XLATE_INCOMPAT,
"translator member %s definition uses "
"incompatible types: \"%s\" = \"%s\"\n",
mnp->dn_membname,
dt_node_type_name(mnp, n1, sizeof (n1)),
dt_node_type_name(mnp->dn_membexpr,
n2, sizeof (n2)));
}
}

dt_idstack_pop(&yypcb->pcb_globals, dxp->dx_locals);

dxp->dx_souid.di_attr = attr;
dxp->dx_ptrid.di_attr = attr;

dt_node_type_assign(dnp, DT_DYN_CTFP(dtp), DT_DYN_TYPE(dtp), B_FALSE);
dt_node_attr_assign(dnp, _dtrace_defattr);

return (dnp);
}

static void
dt_node_provider_cmp_argv(dt_provider_t *pvp, dt_node_t *pnp, const char *kind,
uint_t old_argc, dt_node_t *old_argv, uint_t new_argc, dt_node_t *new_argv)
{
dt_probe_t *prp = pnp->dn_ident->di_data;
uint_t i;

char n1[DT_TYPE_NAMELEN];
char n2[DT_TYPE_NAMELEN];

if (old_argc != new_argc) {
dnerror(pnp, D_PROV_INCOMPAT,
"probe %s:%s %s prototype mismatch:\n"
"\t current: %u arg%s\n\tprevious: %u arg%s\n",
pvp->pv_desc.dtvd_name, prp->pr_ident->di_name, kind,
new_argc, new_argc != 1 ? "s" : "",
old_argc, old_argc != 1 ? "s" : "");
}

for (i = 0; i < old_argc; i++,
old_argv = old_argv->dn_list, new_argv = new_argv->dn_list) {
if (ctf_type_cmp(old_argv->dn_ctfp, old_argv->dn_type,
new_argv->dn_ctfp, new_argv->dn_type) == 0)
continue;

dnerror(pnp, D_PROV_INCOMPAT,
"probe %s:%s %s prototype argument #%u mismatch:\n"
"\t current: %s\n\tprevious: %s\n",
pvp->pv_desc.dtvd_name, prp->pr_ident->di_name, kind, i + 1,
dt_node_type_name(new_argv, n1, sizeof (n1)),
dt_node_type_name(old_argv, n2, sizeof (n2)));
}
}







static void
dt_node_provider_cmp(dt_provider_t *pvp, dt_node_t *pnp,
dt_probe_t *old, dt_probe_t *new)
{
dt_node_provider_cmp_argv(pvp, pnp, "output",
old->pr_xargc, old->pr_xargs, new->pr_xargc, new->pr_xargs);

if (old->pr_nargs != old->pr_xargs && new->pr_nargs != new->pr_xargs) {
dt_node_provider_cmp_argv(pvp, pnp, "input",
old->pr_nargc, old->pr_nargs, new->pr_nargc, new->pr_nargs);
}

if (old->pr_nargs == old->pr_xargs && new->pr_nargs != new->pr_xargs) {
if (pvp->pv_flags & DT_PROVIDER_IMPL) {
dnerror(pnp, D_PROV_INCOMPAT,
"provider interface mismatch: %s\n"
"\t current: probe %s:%s has an output prototype\n"
"\tprevious: probe %s:%s has no output prototype\n",
pvp->pv_desc.dtvd_name, pvp->pv_desc.dtvd_name,
new->pr_ident->di_name, pvp->pv_desc.dtvd_name,
old->pr_ident->di_name);
}

if (old->pr_ident->di_gen == yypcb->pcb_hdl->dt_gen)
old->pr_ident->di_flags |= DT_IDFLG_ORPHAN;

dt_idhash_delete(pvp->pv_probes, old->pr_ident);
dt_probe_declare(pvp, new);
}
}

static void
dt_cook_probe(dt_node_t *dnp, dt_provider_t *pvp)
{
dtrace_hdl_t *dtp = yypcb->pcb_hdl;
dt_probe_t *prp = dnp->dn_ident->di_data;

dt_xlator_t *dxp;
uint_t i;

char n1[DT_TYPE_NAMELEN];
char n2[DT_TYPE_NAMELEN];

if (prp->pr_nargs == prp->pr_xargs)
return;

for (i = 0; i < prp->pr_xargc; i++) {
dt_node_t *xnp = prp->pr_xargv[i];
dt_node_t *nnp = prp->pr_nargv[prp->pr_mapping[i]];

if ((dxp = dt_xlator_lookup(dtp,
nnp, xnp, DT_XLATE_FUZZY)) != NULL) {
if (dt_provider_xref(dtp, pvp, dxp->dx_id) != 0)
longjmp(yypcb->pcb_jmpbuf, EDT_NOMEM);
continue;
}

if (dt_node_is_argcompat(nnp, xnp))
continue;

dnerror(dnp, D_PROV_PRXLATOR, "translator for %s:%s output "
"argument #%u from %s to %s is not defined\n",
pvp->pv_desc.dtvd_name, dnp->dn_ident->di_name, i + 1,
dt_node_type_name(nnp, n1, sizeof (n1)),
dt_node_type_name(xnp, n2, sizeof (n2)));
}
}


static dt_node_t *
dt_cook_provider(dt_node_t *dnp, uint_t idflags)
{
dt_provider_t *pvp = dnp->dn_provider;
dt_node_t *pnp;






for (pnp = dnp->dn_probes; pnp != NULL; pnp = pnp->dn_list) {
const char *probename = pnp->dn_ident->di_name;
dt_probe_t *prp = dt_probe_lookup(pvp, probename);

assert(pnp->dn_kind == DT_NODE_PROBE);

if (prp != NULL && dnp->dn_provred) {
dt_node_provider_cmp(pvp, pnp,
prp, pnp->dn_ident->di_data);
} else if (prp == NULL && dnp->dn_provred) {
dnerror(pnp, D_PROV_INCOMPAT,
"provider interface mismatch: %s\n"
"\t current: probe %s:%s defined\n"
"\tprevious: probe %s:%s not defined\n",
dnp->dn_provname, dnp->dn_provname,
probename, dnp->dn_provname, probename);
} else if (prp != NULL) {
dnerror(pnp, D_PROV_PRDUP, "probe redeclared: %s:%s\n",
dnp->dn_provname, probename);
} else
dt_probe_declare(pvp, pnp->dn_ident->di_data);

dt_cook_probe(pnp, pvp);
}

return (dnp);
}


static dt_node_t *
dt_cook_none(dt_node_t *dnp, uint_t idflags)
{
return (dnp);
}

static dt_node_t *(*dt_cook_funcs[])(dt_node_t *, uint_t) = {
dt_cook_none,
dt_cook_none,
dt_cook_none,
dt_cook_ident,
dt_cook_var,
dt_cook_none,
dt_cook_none,
dt_cook_func,
dt_cook_op1,
dt_cook_op2,
dt_cook_op3,
dt_cook_statement,
dt_cook_statement,
dt_cook_aggregation,
dt_cook_none,
dt_cook_clause,
dt_cook_inline,
dt_cook_member,
dt_cook_xlator,
dt_cook_none,
dt_cook_provider,
dt_cook_none,
dt_cook_none,
};






dt_node_t *
dt_node_cook(dt_node_t *dnp, uint_t idflags)
{
int oldlineno = yylineno;

yylineno = dnp->dn_line;

assert(dnp->dn_kind <
sizeof (dt_cook_funcs) / sizeof (dt_cook_funcs[0]));
dnp = dt_cook_funcs[dnp->dn_kind](dnp, idflags);
dnp->dn_flags |= DT_NF_COOKED;

if (dnp->dn_kind == DT_NODE_VAR || dnp->dn_kind == DT_NODE_AGG)
dnp->dn_ident->di_flags |= idflags;

yylineno = oldlineno;
return (dnp);
}

dtrace_attribute_t
dt_node_list_cook(dt_node_t **pnp, uint_t idflags)
{
dtrace_attribute_t attr = _dtrace_defattr;
dt_node_t *dnp, *nnp;

for (dnp = (pnp != NULL ? *pnp : NULL); dnp != NULL; dnp = nnp) {
nnp = dnp->dn_list;
dnp = *pnp = dt_node_cook(dnp, idflags);
attr = dt_attr_min(attr, dnp->dn_attr);
dnp->dn_list = nnp;
pnp = &dnp->dn_list;
}

return (attr);
}

void
dt_node_list_free(dt_node_t **pnp)
{
dt_node_t *dnp, *nnp;

for (dnp = (pnp != NULL ? *pnp : NULL); dnp != NULL; dnp = nnp) {
nnp = dnp->dn_list;
dt_node_free(dnp);
}

if (pnp != NULL)
*pnp = NULL;
}

void
dt_node_link_free(dt_node_t **pnp)
{
dt_node_t *dnp, *nnp;

for (dnp = (pnp != NULL ? *pnp : NULL); dnp != NULL; dnp = nnp) {
nnp = dnp->dn_link;
dt_node_free(dnp);
}

for (dnp = (pnp != NULL ? *pnp : NULL); dnp != NULL; dnp = nnp) {
nnp = dnp->dn_link;
free(dnp);
}

if (pnp != NULL)
*pnp = NULL;
}

dt_node_t *
dt_node_link(dt_node_t *lp, dt_node_t *rp)
{
dt_node_t *dnp;

if (lp == NULL)
return (rp);
else if (rp == NULL)
return (lp);

for (dnp = lp; dnp->dn_list != NULL; dnp = dnp->dn_list)
continue;

dnp->dn_list = rp;
return (lp);
}






void
dt_node_diftype(dtrace_hdl_t *dtp, const dt_node_t *dnp, dtrace_diftype_t *tp)
{
if (dnp->dn_ctfp == DT_STR_CTFP(dtp) &&
dnp->dn_type == DT_STR_TYPE(dtp)) {
tp->dtdt_kind = DIF_TYPE_STRING;
tp->dtdt_ckind = CTF_K_UNKNOWN;
} else {
tp->dtdt_kind = DIF_TYPE_CTF;
tp->dtdt_ckind = ctf_type_kind(dnp->dn_ctfp,
ctf_type_resolve(dnp->dn_ctfp, dnp->dn_type));
}

tp->dtdt_flags = (dnp->dn_flags & DT_NF_REF) ?
(dnp->dn_flags & DT_NF_USERLAND) ? DIF_TF_BYUREF :
DIF_TF_BYREF : 0;
tp->dtdt_pad = 0;
tp->dtdt_size = ctf_type_size(dnp->dn_ctfp, dnp->dn_type);
}
















void
dt_printd(dt_node_t *dnp, FILE *fp, int depth)
{
dt_node_t *arg;

switch (dnp->dn_kind) {
case DT_NODE_INT:
(void) fprintf(fp, "0x%llx", (u_longlong_t)dnp->dn_value);
if (!(dnp->dn_flags & DT_NF_SIGNED))
(void) fprintf(fp, "u");
break;

case DT_NODE_STRING: {
char *escd = strchr2esc(dnp->dn_string, strlen(dnp->dn_string));
(void) fprintf(fp, "\"%s\"", escd);
free(escd);
break;
}

case DT_NODE_IDENT:
(void) fprintf(fp, "%s", dnp->dn_string);
break;

case DT_NODE_VAR:
(void) fprintf(fp, "%s%s",
(dnp->dn_ident->di_flags & DT_IDFLG_LOCAL) ? "this->" :
(dnp->dn_ident->di_flags & DT_IDFLG_TLS) ? "self->" : "",
dnp->dn_ident->di_name);

if (dnp->dn_args != NULL) {
(void) fprintf(fp, "[");

for (arg = dnp->dn_args; arg != NULL;
arg = arg->dn_list) {
dt_printd(arg, fp, 0);
if (arg->dn_list != NULL)
(void) fprintf(fp, ", ");
}

(void) fprintf(fp, "]");
}
break;

case DT_NODE_SYM: {
const dtrace_syminfo_t *dts = dnp->dn_ident->di_data;
(void) fprintf(fp, "%s`%s", dts->dts_object, dts->dts_name);
break;
}
case DT_NODE_FUNC:
(void) fprintf(fp, "%s(", dnp->dn_ident->di_name);

for (arg = dnp->dn_args; arg != NULL; arg = arg->dn_list) {
dt_printd(arg, fp, 0);
if (arg->dn_list != NULL)
(void) fprintf(fp, ", ");
}
(void) fprintf(fp, ")");
break;

case DT_NODE_OP1:
(void) fprintf(fp, "%s(", opstr(dnp->dn_op));
dt_printd(dnp->dn_child, fp, 0);
(void) fprintf(fp, ")");
break;

case DT_NODE_OP2:
(void) fprintf(fp, "(");
dt_printd(dnp->dn_left, fp, 0);
if (dnp->dn_op == DT_TOK_LPAR) {
(void) fprintf(fp, ")");
dt_printd(dnp->dn_right, fp, 0);
break;
}
if (dnp->dn_op == DT_TOK_PTR || dnp->dn_op == DT_TOK_DOT ||
dnp->dn_op == DT_TOK_LBRAC)
(void) fprintf(fp, "%s", opstr(dnp->dn_op));
else
(void) fprintf(fp, " %s ", opstr(dnp->dn_op));
dt_printd(dnp->dn_right, fp, 0);
if (dnp->dn_op == DT_TOK_LBRAC) {
dt_node_t *ln = dnp->dn_right;
while (ln->dn_list != NULL) {
(void) fprintf(fp, ", ");
dt_printd(ln->dn_list, fp, depth);
ln = ln->dn_list;
}
(void) fprintf(fp, "]");
}
(void) fprintf(fp, ")");
break;

case DT_NODE_OP3:
(void) fprintf(fp, "(");
dt_printd(dnp->dn_expr, fp, 0);
(void) fprintf(fp, " ? ");
dt_printd(dnp->dn_left, fp, 0);
(void) fprintf(fp, " : ");
dt_printd(dnp->dn_right, fp, 0);
(void) fprintf(fp, ")");
break;

case DT_NODE_DEXPR:
case DT_NODE_DFUNC:
(void) fprintf(fp, "%*s", depth * 8, "");
dt_printd(dnp->dn_expr, fp, depth + 1);
(void) fprintf(fp, ";\n");
break;

case DT_NODE_PDESC:
(void) fprintf(fp, "%s:%s:%s:%s",
dnp->dn_desc->dtpd_provider, dnp->dn_desc->dtpd_mod,
dnp->dn_desc->dtpd_func, dnp->dn_desc->dtpd_name);
break;

case DT_NODE_CLAUSE:
for (arg = dnp->dn_pdescs; arg != NULL; arg = arg->dn_list) {
dt_printd(arg, fp, 0);
if (arg->dn_list != NULL)
(void) fprintf(fp, ",");
(void) fprintf(fp, "\n");
}

if (dnp->dn_pred != NULL) {
(void) fprintf(fp, "/");
dt_printd(dnp->dn_pred, fp, 0);
(void) fprintf(fp, "/\n");
}
(void) fprintf(fp, "{\n");

for (arg = dnp->dn_acts; arg != NULL; arg = arg->dn_list)
dt_printd(arg, fp, depth + 1);
(void) fprintf(fp, "}\n");
(void) fprintf(fp, "\n");
break;

case DT_NODE_IF:
(void) fprintf(fp, "%*sif (", depth * 8, "");
dt_printd(dnp->dn_conditional, fp, 0);
(void) fprintf(fp, ") {\n");

for (arg = dnp->dn_body; arg != NULL; arg = arg->dn_list)
dt_printd(arg, fp, depth + 1);
if (dnp->dn_alternate_body == NULL) {
(void) fprintf(fp, "%*s}\n", depth * 8, "");
} else {
(void) fprintf(fp, "%*s} else {\n", depth * 8, "");
for (arg = dnp->dn_alternate_body; arg != NULL;
arg = arg->dn_list)
dt_printd(arg, fp, depth + 1);
(void) fprintf(fp, "%*s}\n", depth * 8, "");
}

break;

default:
(void) fprintf(fp, "/* bad node %p, kind %d */\n",
(void *)dnp, dnp->dn_kind);
}
}

void
dt_node_printr(dt_node_t *dnp, FILE *fp, int depth)
{
char n[DT_TYPE_NAMELEN], buf[BUFSIZ], a[8];
const dtrace_syminfo_t *dts;
const dt_idnode_t *inp;
dt_node_t *arg;

(void) fprintf(fp, "%*s", depth * 2, "");
(void) dt_attr_str(dnp->dn_attr, a, sizeof (a));

if (dnp->dn_ctfp != NULL && dnp->dn_type != CTF_ERR &&
ctf_type_name(dnp->dn_ctfp, dnp->dn_type, n, sizeof (n)) != NULL) {
(void) snprintf(buf, BUFSIZ, "type=<%s> attr=%s flags=", n, a);
} else {
(void) snprintf(buf, BUFSIZ, "type=<%ld> attr=%s flags=",
dnp->dn_type, a);
}

if (dnp->dn_flags != 0) {
n[0] = '\0';
if (dnp->dn_flags & DT_NF_SIGNED)
(void) strcat(n, ",SIGN");
if (dnp->dn_flags & DT_NF_COOKED)
(void) strcat(n, ",COOK");
if (dnp->dn_flags & DT_NF_REF)
(void) strcat(n, ",REF");
if (dnp->dn_flags & DT_NF_LVALUE)
(void) strcat(n, ",LVAL");
if (dnp->dn_flags & DT_NF_WRITABLE)
(void) strcat(n, ",WRITE");
if (dnp->dn_flags & DT_NF_BITFIELD)
(void) strcat(n, ",BITF");
if (dnp->dn_flags & DT_NF_USERLAND)
(void) strcat(n, ",USER");
(void) strcat(buf, n + 1);
} else
(void) strcat(buf, "0");

switch (dnp->dn_kind) {
case DT_NODE_FREE:
(void) fprintf(fp, "FREE <node %p>\n", (void *)dnp);
break;

case DT_NODE_INT:
(void) fprintf(fp, "INT 0x%llx (%s)\n",
(u_longlong_t)dnp->dn_value, buf);
break;

case DT_NODE_STRING:
(void) fprintf(fp, "STRING \"%s\" (%s)\n", dnp->dn_string, buf);
break;

case DT_NODE_IDENT:
(void) fprintf(fp, "IDENT %s (%s)\n", dnp->dn_string, buf);
break;

case DT_NODE_VAR:
(void) fprintf(fp, "VARIABLE %s%s (%s)\n",
(dnp->dn_ident->di_flags & DT_IDFLG_LOCAL) ? "this->" :
(dnp->dn_ident->di_flags & DT_IDFLG_TLS) ? "self->" : "",
dnp->dn_ident->di_name, buf);

if (dnp->dn_args != NULL)
(void) fprintf(fp, "%*s[\n", depth * 2, "");

for (arg = dnp->dn_args; arg != NULL; arg = arg->dn_list) {
dt_node_printr(arg, fp, depth + 1);
if (arg->dn_list != NULL)
(void) fprintf(fp, "%*s,\n", depth * 2, "");
}

if (dnp->dn_args != NULL)
(void) fprintf(fp, "%*s]\n", depth * 2, "");
break;

case DT_NODE_SYM:
dts = dnp->dn_ident->di_data;
(void) fprintf(fp, "SYMBOL %s`%s (%s)\n",
dts->dts_object, dts->dts_name, buf);
break;

case DT_NODE_TYPE:
if (dnp->dn_string != NULL) {
(void) fprintf(fp, "TYPE (%s) %s\n",
buf, dnp->dn_string);
} else
(void) fprintf(fp, "TYPE (%s)\n", buf);
break;

case DT_NODE_FUNC:
(void) fprintf(fp, "FUNC %s (%s)\n",
dnp->dn_ident->di_name, buf);

for (arg = dnp->dn_args; arg != NULL; arg = arg->dn_list) {
dt_node_printr(arg, fp, depth + 1);
if (arg->dn_list != NULL)
(void) fprintf(fp, "%*s,\n", depth * 2, "");
}
break;

case DT_NODE_OP1:
(void) fprintf(fp, "OP1 %s (%s)\n", opstr(dnp->dn_op), buf);
dt_node_printr(dnp->dn_child, fp, depth + 1);
break;

case DT_NODE_OP2:
(void) fprintf(fp, "OP2 %s (%s)\n", opstr(dnp->dn_op), buf);
dt_node_printr(dnp->dn_left, fp, depth + 1);
dt_node_printr(dnp->dn_right, fp, depth + 1);
if (dnp->dn_op == DT_TOK_LBRAC) {
dt_node_t *ln = dnp->dn_right;
while (ln->dn_list != NULL) {
dt_node_printr(ln->dn_list, fp, depth + 1);
ln = ln->dn_list;
}
}
break;

case DT_NODE_OP3:
(void) fprintf(fp, "OP3 (%s)\n", buf);
dt_node_printr(dnp->dn_expr, fp, depth + 1);
(void) fprintf(fp, "%*s?\n", depth * 2, "");
dt_node_printr(dnp->dn_left, fp, depth + 1);
(void) fprintf(fp, "%*s:\n", depth * 2, "");
dt_node_printr(dnp->dn_right, fp, depth + 1);
break;

case DT_NODE_DEXPR:
case DT_NODE_DFUNC:
(void) fprintf(fp, "D EXPRESSION attr=%s\n", a);
dt_node_printr(dnp->dn_expr, fp, depth + 1);
break;

case DT_NODE_AGG:
(void) fprintf(fp, "AGGREGATE @%s attr=%s [\n",
dnp->dn_ident->di_name, a);

for (arg = dnp->dn_aggtup; arg != NULL; arg = arg->dn_list) {
dt_node_printr(arg, fp, depth + 1);
if (arg->dn_list != NULL)
(void) fprintf(fp, "%*s,\n", depth * 2, "");
}

if (dnp->dn_aggfun) {
(void) fprintf(fp, "%*s] = ", depth * 2, "");
dt_node_printr(dnp->dn_aggfun, fp, depth + 1);
} else
(void) fprintf(fp, "%*s]\n", depth * 2, "");

if (dnp->dn_aggfun)
(void) fprintf(fp, "%*s)\n", depth * 2, "");
break;

case DT_NODE_PDESC:
(void) fprintf(fp, "PDESC %s:%s:%s:%s [%u]\n",
dnp->dn_desc->dtpd_provider, dnp->dn_desc->dtpd_mod,
dnp->dn_desc->dtpd_func, dnp->dn_desc->dtpd_name,
dnp->dn_desc->dtpd_id);
break;

case DT_NODE_CLAUSE:
(void) fprintf(fp, "CLAUSE attr=%s\n", a);

for (arg = dnp->dn_pdescs; arg != NULL; arg = arg->dn_list)
dt_node_printr(arg, fp, depth + 1);

(void) fprintf(fp, "%*sCTXATTR %s\n", depth * 2, "",
dt_attr_str(dnp->dn_ctxattr, a, sizeof (a)));

if (dnp->dn_pred != NULL) {
(void) fprintf(fp, "%*sPREDICATE /\n", depth * 2, "");
dt_node_printr(dnp->dn_pred, fp, depth + 1);
(void) fprintf(fp, "%*s/\n", depth * 2, "");
}

for (arg = dnp->dn_acts; arg != NULL; arg = arg->dn_list)
dt_node_printr(arg, fp, depth + 1);
(void) fprintf(fp, "\n");
break;

case DT_NODE_INLINE:
inp = dnp->dn_ident->di_iarg;

(void) fprintf(fp, "INLINE %s (%s)\n",
dnp->dn_ident->di_name, buf);
dt_node_printr(inp->din_root, fp, depth + 1);
break;

case DT_NODE_MEMBER:
(void) fprintf(fp, "MEMBER %s (%s)\n", dnp->dn_membname, buf);
if (dnp->dn_membexpr)
dt_node_printr(dnp->dn_membexpr, fp, depth + 1);
break;

case DT_NODE_XLATOR:
(void) fprintf(fp, "XLATOR (%s)", buf);

if (ctf_type_name(dnp->dn_xlator->dx_src_ctfp,
dnp->dn_xlator->dx_src_type, n, sizeof (n)) != NULL)
(void) fprintf(fp, " from <%s>", n);

if (ctf_type_name(dnp->dn_xlator->dx_dst_ctfp,
dnp->dn_xlator->dx_dst_type, n, sizeof (n)) != NULL)
(void) fprintf(fp, " to <%s>", n);

(void) fprintf(fp, "\n");

for (arg = dnp->dn_members; arg != NULL; arg = arg->dn_list)
dt_node_printr(arg, fp, depth + 1);
break;

case DT_NODE_PROBE:
(void) fprintf(fp, "PROBE %s\n", dnp->dn_ident->di_name);
break;

case DT_NODE_PROVIDER:
(void) fprintf(fp, "PROVIDER %s (%s)\n",
dnp->dn_provname, dnp->dn_provred ? "redecl" : "decl");
for (arg = dnp->dn_probes; arg != NULL; arg = arg->dn_list)
dt_node_printr(arg, fp, depth + 1);
break;

case DT_NODE_PROG:
(void) fprintf(fp, "PROGRAM attr=%s\n", a);
for (arg = dnp->dn_list; arg != NULL; arg = arg->dn_list)
dt_node_printr(arg, fp, depth + 1);
break;

case DT_NODE_IF:
(void) fprintf(fp, "IF attr=%s CONDITION:\n", a);

dt_node_printr(dnp->dn_conditional, fp, depth + 1);

(void) fprintf(fp, "%*sIF BODY: \n", depth * 2, "");
for (arg = dnp->dn_body; arg != NULL; arg = arg->dn_list)
dt_node_printr(arg, fp, depth + 1);

if (dnp->dn_alternate_body != NULL) {
(void) fprintf(fp, "%*sIF ELSE: \n", depth * 2, "");
for (arg = dnp->dn_alternate_body; arg != NULL;
arg = arg->dn_list)
dt_node_printr(arg, fp, depth + 1);
}

break;

default:
(void) fprintf(fp, "<bad node %p, kind %d>\n",
(void *)dnp, dnp->dn_kind);
}
}

int
dt_node_root(dt_node_t *dnp)
{
yypcb->pcb_root = dnp;
return (0);
}


void
dnerror(const dt_node_t *dnp, dt_errtag_t tag, const char *format, ...)
{
int oldlineno = yylineno;
va_list ap;

yylineno = dnp->dn_line;

va_start(ap, format);
xyvwarn(tag, format, ap);
va_end(ap);

yylineno = oldlineno;
longjmp(yypcb->pcb_jmpbuf, EDT_COMPILER);
}


void
dnwarn(const dt_node_t *dnp, dt_errtag_t tag, const char *format, ...)
{
int oldlineno = yylineno;
va_list ap;

yylineno = dnp->dn_line;

va_start(ap, format);
xyvwarn(tag, format, ap);
va_end(ap);

yylineno = oldlineno;
}


void
xyerror(dt_errtag_t tag, const char *format, ...)
{
va_list ap;

va_start(ap, format);
xyvwarn(tag, format, ap);
va_end(ap);

longjmp(yypcb->pcb_jmpbuf, EDT_COMPILER);
}


void
xywarn(dt_errtag_t tag, const char *format, ...)
{
va_list ap;

va_start(ap, format);
xyvwarn(tag, format, ap);
va_end(ap);
}

void
xyvwarn(dt_errtag_t tag, const char *format, va_list ap)
{
if (yypcb == NULL)
return;

dt_set_errmsg(yypcb->pcb_hdl, dt_errtag(tag), yypcb->pcb_region,
yypcb->pcb_filetag, yypcb->pcb_fileptr ? yylineno : 0, format, ap);
}


void
yyerror(const char *format, ...)
{
va_list ap;

va_start(ap, format);
yyvwarn(format, ap);
va_end(ap);

longjmp(yypcb->pcb_jmpbuf, EDT_COMPILER);
}


void
yywarn(const char *format, ...)
{
va_list ap;

va_start(ap, format);
yyvwarn(format, ap);
va_end(ap);
}

void
yyvwarn(const char *format, va_list ap)
{
if (yypcb == NULL)
return;

dt_set_errmsg(yypcb->pcb_hdl, dt_errtag(D_SYNTAX), yypcb->pcb_region,
yypcb->pcb_filetag, yypcb->pcb_fileptr ? yylineno : 0, format, ap);

if (strchr(format, '\n') == NULL) {
dtrace_hdl_t *dtp = yypcb->pcb_hdl;
size_t len = strlen(dtp->dt_errmsg);
char *p, *s = dtp->dt_errmsg + len;
size_t n = sizeof (dtp->dt_errmsg) - len;

if (yytext[0] == '\0')
(void) snprintf(s, n, " near end of input");
else if (yytext[0] == '\n')
(void) snprintf(s, n, " near end of line");
else {
if ((p = strchr(yytext, '\n')) != NULL)
*p = '\0';
(void) snprintf(s, n, " near \"%s\"", yytext);
}
}
}

void
yylabel(const char *label)
{
dt_dprintf("set label to <%s>\n", label ? label : "NULL");
yypcb->pcb_region = label;
}

int
yywrap(void)
{
return (1);
}
