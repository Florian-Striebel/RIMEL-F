



























#pragma ident "%Z%%M% %I% %E% SMI"

#include <strings.h>
#include <stdlib.h>
#include <limits.h>
#include <alloca.h>
#include <assert.h>

#include <dt_decl.h>
#include <dt_parser.h>
#include <dt_module.h>
#include <dt_impl.h>

static dt_decl_t *
dt_decl_check(dt_decl_t *ddp)
{
if (ddp->dd_kind == CTF_K_UNKNOWN)
return (ddp);

if (ddp->dd_name != NULL && strcmp(ddp->dd_name, "char") == 0 &&
(ddp->dd_attr & (DT_DA_SHORT | DT_DA_LONG | DT_DA_LONGLONG))) {
xyerror(D_DECL_CHARATTR, "invalid type declaration: short and "
"long may not be used with char type\n");
}

if (ddp->dd_name != NULL && strcmp(ddp->dd_name, "void") == 0 &&
(ddp->dd_attr & (DT_DA_SHORT | DT_DA_LONG | DT_DA_LONGLONG |
(DT_DA_SIGNED | DT_DA_UNSIGNED)))) {
xyerror(D_DECL_VOIDATTR, "invalid type declaration: attributes "
"may not be used with void type\n");
}

if (ddp->dd_kind != CTF_K_INTEGER &&
(ddp->dd_attr & (DT_DA_SIGNED | DT_DA_UNSIGNED))) {
xyerror(D_DECL_SIGNINT, "invalid type declaration: signed and "
"unsigned may only be used with integer type\n");
}

if (ddp->dd_kind != CTF_K_INTEGER && ddp->dd_kind != CTF_K_FLOAT &&
(ddp->dd_attr & (DT_DA_LONG | DT_DA_LONGLONG))) {
xyerror(D_DECL_LONGINT, "invalid type declaration: long and "
"long long may only be used with integer or "
"floating-point type\n");
}

return (ddp);
}

dt_decl_t *
dt_decl_alloc(ushort_t kind, char *name)
{
dt_decl_t *ddp = malloc(sizeof (dt_decl_t));

if (ddp == NULL)
longjmp(yypcb->pcb_jmpbuf, EDT_NOMEM);

ddp->dd_kind = kind;
ddp->dd_attr = 0;
ddp->dd_ctfp = NULL;
ddp->dd_type = CTF_ERR;
ddp->dd_name = name;
ddp->dd_node = NULL;
ddp->dd_next = NULL;

return (ddp);
}

void
dt_decl_free(dt_decl_t *ddp)
{
dt_decl_t *ndp;

for (; ddp != NULL; ddp = ndp) {
ndp = ddp->dd_next;
free(ddp->dd_name);
dt_node_list_free(&ddp->dd_node);
free(ddp);
}
}

void
dt_decl_reset(void)
{
dt_scope_t *dsp = &yypcb->pcb_dstack;
dt_decl_t *ddp = dsp->ds_decl;

while (ddp->dd_next != NULL) {
dsp->ds_decl = ddp->dd_next;
ddp->dd_next = NULL;
dt_decl_free(ddp);
ddp = dsp->ds_decl;
}
}

dt_decl_t *
dt_decl_push(dt_decl_t *ddp)
{
dt_scope_t *dsp = &yypcb->pcb_dstack;
dt_decl_t *top = dsp->ds_decl;

if (top != NULL &&
top->dd_kind == CTF_K_UNKNOWN && top->dd_name == NULL) {
top->dd_kind = CTF_K_INTEGER;
(void) dt_decl_check(top);
}

assert(ddp->dd_next == NULL);
ddp->dd_next = top;
dsp->ds_decl = ddp;

return (ddp);
}

dt_decl_t *
dt_decl_pop(void)
{
dt_scope_t *dsp = &yypcb->pcb_dstack;
dt_decl_t *ddp = dt_decl_top();

dsp->ds_decl = NULL;
free(dsp->ds_ident);
dsp->ds_ident = NULL;
dsp->ds_ctfp = NULL;
dsp->ds_type = CTF_ERR;
dsp->ds_class = DT_DC_DEFAULT;
dsp->ds_enumval = -1;

return (ddp);
}

dt_decl_t *
dt_decl_pop_param(char **idp)
{
dt_scope_t *dsp = &yypcb->pcb_dstack;

if (dsp->ds_class != DT_DC_DEFAULT && dsp->ds_class != DT_DC_REGISTER) {
xyerror(D_DECL_PARMCLASS, "inappropriate storage class "
"for function or associative array parameter\n");
}

if (idp != NULL && dt_decl_top() != NULL) {
*idp = dsp->ds_ident;
dsp->ds_ident = NULL;
}

return (dt_decl_pop());
}

dt_decl_t *
dt_decl_top(void)
{
dt_decl_t *ddp = yypcb->pcb_dstack.ds_decl;

if (ddp == NULL)
longjmp(yypcb->pcb_jmpbuf, EDT_NODECL);

if (ddp->dd_kind == CTF_K_UNKNOWN && ddp->dd_name == NULL) {
ddp->dd_kind = CTF_K_INTEGER;
(void) dt_decl_check(ddp);
}

return (ddp);
}

dt_decl_t *
dt_decl_ident(char *name)
{
dt_scope_t *dsp = &yypcb->pcb_dstack;
dt_decl_t *ddp = dsp->ds_decl;

if (dsp->ds_ident != NULL) {
free(name);
xyerror(D_DECL_IDENT, "old-style declaration or "
"incorrect type specified\n");
}

dsp->ds_ident = name;

if (ddp == NULL)
ddp = dt_decl_push(dt_decl_alloc(CTF_K_UNKNOWN, NULL));

return (ddp);
}

void
dt_decl_class(dt_dclass_t class)
{
dt_scope_t *dsp = &yypcb->pcb_dstack;

if (dsp->ds_class != DT_DC_DEFAULT) {
xyerror(D_DECL_CLASS, "only one storage class allowed "
"in a declaration\n");
}

dsp->ds_class = class;
}







dt_decl_t *
dt_decl_spec(ushort_t kind, char *name)
{
dt_decl_t *ddp = yypcb->pcb_dstack.ds_decl;

if (ddp == NULL)
return (dt_decl_push(dt_decl_alloc(kind, name)));








if (ddp->dd_name != NULL && kind == CTF_K_TYPEDEF) {
if (yypcb->pcb_dstack.ds_class != DT_DC_TYPEDEF)
return (dt_decl_ident(name));
xyerror(D_DECL_IDRED, "identifier redeclared: %s\n", name);
}

if (ddp->dd_name != NULL || ddp->dd_kind != CTF_K_UNKNOWN)
xyerror(D_DECL_COMBO, "invalid type combination\n");

ddp->dd_kind = kind;
ddp->dd_name = name;

return (dt_decl_check(ddp));
}

dt_decl_t *
dt_decl_attr(ushort_t attr)
{
dt_decl_t *ddp = yypcb->pcb_dstack.ds_decl;

if (ddp == NULL) {
ddp = dt_decl_push(dt_decl_alloc(CTF_K_UNKNOWN, NULL));
ddp->dd_attr = attr;
return (ddp);
}

if (attr == DT_DA_LONG && (ddp->dd_attr & DT_DA_LONG)) {
ddp->dd_attr &= ~DT_DA_LONG;
attr = DT_DA_LONGLONG;
}

ddp->dd_attr |= attr;
return (dt_decl_check(ddp));
}






static int
dt_decl_protoform(dt_node_t *fnp, dt_node_t *flist)
{
dt_node_t *dnp;

for (dnp = flist; dnp != fnp && dnp != NULL; dnp = dnp->dn_list) {
if (dnp->dn_string != NULL &&
strcmp(dnp->dn_string, fnp->dn_string) == 0)
return (B_TRUE);
}

return (B_FALSE);
}









int
dt_decl_prototype(dt_node_t *plist,
dt_node_t *flist, const char *kind, uint_t flags)
{
char n[DT_TYPE_NAMELEN];
int is_void, v = 0, i = 1;
int form = plist != flist;
dt_node_t *dnp;

for (dnp = plist; dnp != NULL; dnp = dnp->dn_list, i++) {

if (dnp->dn_type == CTF_ERR && !(flags & DT_DP_VARARGS)) {
dnerror(dnp, D_DECL_PROTO_VARARGS, "%s prototype may "
"not use a variable-length argument list\n", kind);
}

if (dt_node_is_dynamic(dnp) && !(flags & DT_DP_DYNAMIC)) {
dnerror(dnp, D_DECL_PROTO_TYPE, "%s prototype may not "
"use parameter of type %s: %s, parameter #%d\n",
kind, dt_node_type_name(dnp, n, sizeof (n)),
dnp->dn_string ? dnp->dn_string : "(anonymous)", i);
}

is_void = dt_node_is_void(dnp);
v += is_void;

if (is_void && !(flags & DT_DP_VOID)) {
dnerror(dnp, D_DECL_PROTO_TYPE, "%s prototype may not "
"use parameter of type %s: %s, parameter #%d\n",
kind, dt_node_type_name(dnp, n, sizeof (n)),
dnp->dn_string ? dnp->dn_string : "(anonymous)", i);
}

if (is_void && dnp->dn_string != NULL) {
dnerror(dnp, D_DECL_PROTO_NAME, "void parameter may "
"not have a name: %s\n", dnp->dn_string);
}

if (dnp->dn_string != NULL &&
dt_decl_protoform(dnp, flist) != form) {
dnerror(dnp, D_DECL_PROTO_FORM, "parameter is "
"%s declared in %s prototype: %s, parameter #%d\n",
form ? "not" : "already", kind, dnp->dn_string, i);
}

if (dnp->dn_string == NULL &&
!is_void && !(flags & DT_DP_ANON)) {
dnerror(dnp, D_DECL_PROTO_NAME, "parameter declaration "
"requires a name: parameter #%d\n", i);
}
}

if (v != 0 && plist->dn_list != NULL)
xyerror(D_DECL_PROTO_VOID, "void must be sole parameter\n");

return (v ? 0 : i - 1);
}

dt_decl_t *
dt_decl_array(dt_node_t *dnp)
{
dt_decl_t *ddp = dt_decl_push(dt_decl_alloc(CTF_K_ARRAY, NULL));
dt_scope_t *dsp = &yypcb->pcb_dstack;
dt_decl_t *ndp = ddp;







while (ndp->dd_next != NULL && ndp->dd_next->dd_kind == CTF_K_ARRAY)
ndp = ndp->dd_next;

if (ndp != ddp) {
if (dnp != NULL && dnp->dn_kind == DT_NODE_TYPE) {
xyerror(D_DECL_DYNOBJ,
"cannot declare array of associative arrays\n");
}
dsp->ds_decl = ddp->dd_next;
ddp->dd_next = ndp->dd_next;
ndp->dd_next = ddp;
}

if (ddp->dd_next->dd_name != NULL &&
strcmp(ddp->dd_next->dd_name, "void") == 0)
xyerror(D_DECL_VOIDOBJ, "cannot declare array of void\n");

if (dnp != NULL && dnp->dn_kind != DT_NODE_TYPE) {
dnp = ddp->dd_node = dt_node_cook(dnp, DT_IDFLG_REF);

if (dt_node_is_posconst(dnp) == 0) {
xyerror(D_DECL_ARRSUB, "positive integral constant "
"expression or tuple signature expected as "
"array declaration subscript\n");
}

if (dnp->dn_value > UINT_MAX)
xyerror(D_DECL_ARRBIG, "array dimension too big\n");

} else if (dnp != NULL) {
ddp->dd_node = dnp;
(void) dt_decl_prototype(dnp, dnp, "array", DT_DP_ANON);
}

return (ddp);
}












dt_decl_t *
dt_decl_func(dt_decl_t *pdp, dt_node_t *dnp)
{
dt_decl_t *ddp = dt_decl_alloc(CTF_K_FUNCTION, NULL);

ddp->dd_node = dnp;

(void) dt_decl_prototype(dnp, dnp, "function",
DT_DP_VARARGS | DT_DP_VOID | DT_DP_ANON);

if (pdp == NULL || pdp->dd_kind != CTF_K_POINTER)
return (dt_decl_push(ddp));

while (pdp->dd_next != NULL && !(pdp->dd_next->dd_attr & DT_DA_PAREN))
pdp = pdp->dd_next;

if (pdp->dd_next == NULL)
return (dt_decl_push(ddp));

ddp->dd_next = pdp->dd_next;
pdp->dd_next = ddp;

return (pdp);
}

dt_decl_t *
dt_decl_ptr(void)
{
return (dt_decl_push(dt_decl_alloc(CTF_K_POINTER, NULL)));
}

dt_decl_t *
dt_decl_sou(uint_t kind, char *name)
{
dt_decl_t *ddp = dt_decl_spec(kind, name);
char n[DT_TYPE_NAMELEN];
ctf_file_t *ctfp;
ctf_id_t type;
uint_t flag;

if (yypcb->pcb_idepth != 0)
ctfp = yypcb->pcb_hdl->dt_cdefs->dm_ctfp;
else
ctfp = yypcb->pcb_hdl->dt_ddefs->dm_ctfp;

if (yypcb->pcb_dstack.ds_next != NULL)
flag = CTF_ADD_NONROOT;
else
flag = CTF_ADD_ROOT;

(void) snprintf(n, sizeof (n), "%s %s",
kind == CTF_K_STRUCT ? "struct" : "union",
name == NULL ? "(anon)" : name);

if (name != NULL && (type = ctf_lookup_by_name(ctfp, n)) != CTF_ERR &&
ctf_type_kind(ctfp, type) != CTF_K_FORWARD)
xyerror(D_DECL_TYPERED, "type redeclared: %s\n", n);

if (kind == CTF_K_STRUCT)
type = ctf_add_struct(ctfp, flag, name);
else
type = ctf_add_union(ctfp, flag, name);

if (type == CTF_ERR || ctf_update(ctfp) == CTF_ERR) {
xyerror(D_UNKNOWN, "failed to define %s: %s\n",
n, ctf_errmsg(ctf_errno(ctfp)));
}

ddp->dd_ctfp = ctfp;
ddp->dd_type = type;

dt_scope_push(ctfp, type);
return (ddp);
}

void
dt_decl_member(dt_node_t *dnp)
{
dt_scope_t *dsp = yypcb->pcb_dstack.ds_next;
dt_decl_t *ddp = yypcb->pcb_dstack.ds_decl;
char *ident = yypcb->pcb_dstack.ds_ident;

const char *idname = ident ? ident : "(anon)";
char n[DT_TYPE_NAMELEN];

dtrace_typeinfo_t dtt;
ctf_encoding_t cte;
ctf_id_t base;
uint_t kind;
ssize_t size;

if (dsp == NULL)
longjmp(yypcb->pcb_jmpbuf, EDT_NOSCOPE);

if (ddp == NULL)
longjmp(yypcb->pcb_jmpbuf, EDT_NODECL);

if (dnp == NULL && ident == NULL)
xyerror(D_DECL_MNAME, "member declaration requires a name\n");

if (ddp->dd_kind == CTF_K_UNKNOWN && ddp->dd_name == NULL) {
ddp->dd_kind = CTF_K_INTEGER;
(void) dt_decl_check(ddp);
}

if (dt_decl_type(ddp, &dtt) != 0)
longjmp(yypcb->pcb_jmpbuf, EDT_COMPILER);

if (ident != NULL && strchr(ident, '`') != NULL) {
xyerror(D_DECL_SCOPE, "D scoping operator may not be used "
"in a member name (%s)\n", ident);
}

if (dtt.dtt_ctfp == DT_DYN_CTFP(yypcb->pcb_hdl) &&
dtt.dtt_type == DT_DYN_TYPE(yypcb->pcb_hdl)) {
xyerror(D_DECL_DYNOBJ,
"cannot have dynamic member: %s\n", ident);
}

base = ctf_type_resolve(dtt.dtt_ctfp, dtt.dtt_type);
kind = ctf_type_kind(dtt.dtt_ctfp, base);
size = ctf_type_size(dtt.dtt_ctfp, base);

if (kind == CTF_K_FORWARD || ((kind == CTF_K_STRUCT ||
kind == CTF_K_UNION) && size == 0)) {
xyerror(D_DECL_INCOMPLETE, "incomplete struct/union/enum %s: "
"%s\n", dt_type_name(dtt.dtt_ctfp, dtt.dtt_type,
n, sizeof (n)), ident);
}

if (size == 0)
xyerror(D_DECL_VOIDOBJ, "cannot have void member: %s\n", ident);







if (dnp != NULL) {
dnp = dt_node_cook(dnp, DT_IDFLG_REF);







if (ident == NULL && dnp->dn_kind == DT_NODE_INT &&
dnp->dn_value == 0) {
dt_node_free(dnp);
goto done;
}

if (dt_node_is_posconst(dnp) == 0) {
xyerror(D_DECL_BFCONST, "positive integral constant "
"expression expected as bit-field size\n");
}

if (ctf_type_kind(dtt.dtt_ctfp, base) != CTF_K_INTEGER ||
ctf_type_encoding(dtt.dtt_ctfp, base, &cte) == CTF_ERR ||
IS_VOID(cte)) {
xyerror(D_DECL_BFTYPE, "invalid type for "
"bit-field: %s\n", idname);
}

if (dnp->dn_value > cte.cte_bits) {
xyerror(D_DECL_BFSIZE, "bit-field too big "
"for type: %s\n", idname);
}

cte.cte_offset = 0;
cte.cte_bits = (uint_t)dnp->dn_value;

dtt.dtt_type = ctf_add_integer(dsp->ds_ctfp,
CTF_ADD_NONROOT, ctf_type_name(dtt.dtt_ctfp,
dtt.dtt_type, n, sizeof (n)), &cte);

if (dtt.dtt_type == CTF_ERR ||
ctf_update(dsp->ds_ctfp) == CTF_ERR) {
xyerror(D_UNKNOWN, "failed to create type for "
"member '%s': %s\n", idname,
ctf_errmsg(ctf_errno(dsp->ds_ctfp)));
}

dtt.dtt_ctfp = dsp->ds_ctfp;
dt_node_free(dnp);
}







if (dtt.dtt_ctfp != dsp->ds_ctfp &&
dtt.dtt_ctfp != ctf_parent_file(dsp->ds_ctfp)) {

dtt.dtt_type = ctf_add_type(dsp->ds_ctfp,
dtt.dtt_ctfp, dtt.dtt_type);
dtt.dtt_ctfp = dsp->ds_ctfp;

if (dtt.dtt_type == CTF_ERR ||
ctf_update(dtt.dtt_ctfp) == CTF_ERR) {
xyerror(D_UNKNOWN, "failed to copy type of '%s': %s\n",
idname, ctf_errmsg(ctf_errno(dtt.dtt_ctfp)));
}
}

if (ctf_add_member(dsp->ds_ctfp, dsp->ds_type,
ident, dtt.dtt_type) == CTF_ERR) {
xyerror(D_UNKNOWN, "failed to define member '%s': %s\n",
idname, ctf_errmsg(ctf_errno(dsp->ds_ctfp)));
}

done:
free(ident);
yypcb->pcb_dstack.ds_ident = NULL;
dt_decl_reset();
}


static int
dt_decl_hasmembers(const char *name, int value, void *private)
{
return (1);
}

dt_decl_t *
dt_decl_enum(char *name)
{
dt_decl_t *ddp = dt_decl_spec(CTF_K_ENUM, name);
char n[DT_TYPE_NAMELEN];
ctf_file_t *ctfp;
ctf_id_t type;
uint_t flag;

if (yypcb->pcb_idepth != 0)
ctfp = yypcb->pcb_hdl->dt_cdefs->dm_ctfp;
else
ctfp = yypcb->pcb_hdl->dt_ddefs->dm_ctfp;

if (yypcb->pcb_dstack.ds_next != NULL)
flag = CTF_ADD_NONROOT;
else
flag = CTF_ADD_ROOT;

(void) snprintf(n, sizeof (n), "enum %s", name ? name : "(anon)");

if (name != NULL && (type = ctf_lookup_by_name(ctfp, n)) != CTF_ERR) {
if (ctf_enum_iter(ctfp, type, dt_decl_hasmembers, NULL))
xyerror(D_DECL_TYPERED, "type redeclared: %s\n", n);
} else if ((type = ctf_add_enum(ctfp, flag, name)) == CTF_ERR) {
xyerror(D_UNKNOWN, "failed to define %s: %s\n",
n, ctf_errmsg(ctf_errno(ctfp)));
}

ddp->dd_ctfp = ctfp;
ddp->dd_type = type;

dt_scope_push(ctfp, type);
return (ddp);
}

void
dt_decl_enumerator(char *s, dt_node_t *dnp)
{
dt_scope_t *dsp = yypcb->pcb_dstack.ds_next;
dtrace_hdl_t *dtp = yypcb->pcb_hdl;

dt_idnode_t *inp;
dt_ident_t *idp;
char *name;
int value;

name = alloca(strlen(s) + 1);
(void) strcpy(name, s);
free(s);

if (dsp == NULL)
longjmp(yypcb->pcb_jmpbuf, EDT_NOSCOPE);

assert(dsp->ds_decl->dd_kind == CTF_K_ENUM);
value = dsp->ds_enumval + 1;

if (strchr(name, '`') != NULL) {
xyerror(D_DECL_SCOPE, "D scoping operator may not be used in "
"an enumerator name (%s)\n", name);
}






if (dnp != NULL) {
if (dnp->dn_kind != DT_NODE_IDENT || ctf_enum_value(
dsp->ds_ctfp, dsp->ds_type, dnp->dn_string, &value) != 0) {
dnp = dt_node_cook(dnp, DT_IDFLG_REF);

if (dnp->dn_kind != DT_NODE_INT) {
xyerror(D_DECL_ENCONST, "enumerator '%s' must "
"be assigned to an integral constant "
"expression\n", name);
}

if ((intmax_t)dnp->dn_value > INT_MAX ||
(intmax_t)dnp->dn_value < INT_MIN) {
xyerror(D_DECL_ENOFLOW, "enumerator '%s' value "
"overflows INT_MAX (%d)\n", name, INT_MAX);
}

value = (int)dnp->dn_value;
}
dt_node_free(dnp);
}

if (ctf_add_enumerator(dsp->ds_ctfp, dsp->ds_type,
name, value) == CTF_ERR || ctf_update(dsp->ds_ctfp) == CTF_ERR) {
xyerror(D_UNKNOWN, "failed to define enumerator '%s': %s\n",
name, ctf_errmsg(ctf_errno(dsp->ds_ctfp)));
}

dsp->ds_enumval = value;









if (dt_idstack_lookup(&yypcb->pcb_globals, name) != NULL) {
if (dsp->ds_ctfp == dtp->dt_ddefs->dm_ctfp) {
xyerror(D_DECL_IDRED,
"identifier redeclared: %s\n", name);
} else
return;
}

dt_dprintf("add global enumerator %s = %d\n", name, value);

idp = dt_idhash_insert(dtp->dt_globals, name, DT_IDENT_ENUM,
DT_IDFLG_INLINE | DT_IDFLG_REF, 0, _dtrace_defattr, 0,
&dt_idops_inline, NULL, dtp->dt_gen);

if (idp == NULL)
longjmp(yypcb->pcb_jmpbuf, EDT_NOMEM);

yyintprefix = 0;
yyintsuffix[0] = '\0';
yyintdecimal = 0;

dnp = dt_node_int(value);
dt_node_type_assign(dnp, dsp->ds_ctfp, dsp->ds_type, B_FALSE);

if ((inp = malloc(sizeof (dt_idnode_t))) == NULL)
longjmp(yypcb->pcb_jmpbuf, EDT_NOMEM);





assert(yypcb->pcb_list == dnp);
yypcb->pcb_list = dnp->dn_link;
dnp->dn_link = NULL;

bzero(inp, sizeof (dt_idnode_t));
inp->din_list = dnp;
inp->din_root = dnp;

idp->di_iarg = inp;
idp->di_ctfp = dsp->ds_ctfp;
idp->di_type = dsp->ds_type;
}







int
dt_decl_type(dt_decl_t *ddp, dtrace_typeinfo_t *tip)
{
dtrace_hdl_t *dtp = yypcb->pcb_hdl;

dt_module_t *dmp;
ctf_arinfo_t r;
ctf_id_t type;

char n[DT_TYPE_NAMELEN];
uint_t flag;
char *name;
int rv;

tip->dtt_flags = 0;





dmp = yypcb->pcb_idepth ? dtp->dt_cdefs : dtp->dt_ddefs;
flag = yypcb->pcb_dstack.ds_next ? CTF_ADD_NONROOT : CTF_ADD_ROOT;

if (ddp->dd_attr & DT_DA_USER)
tip->dtt_flags = DTT_FL_USER;





if (ddp->dd_ctfp != NULL &&
(dmp = dt_module_lookup_by_ctf(dtp, ddp->dd_ctfp)) != NULL) {
tip->dtt_object = dmp->dm_name;
tip->dtt_ctfp = ddp->dd_ctfp;
tip->dtt_type = ddp->dd_type;
return (0);
}








if (ddp->dd_kind == CTF_K_FUNCTION) {
tip->dtt_object = dtp->dt_ddefs->dm_name;
tip->dtt_ctfp = DT_FUNC_CTFP(dtp);
tip->dtt_type = DT_FUNC_TYPE(dtp);
return (0);
}






if (ddp->dd_kind == CTF_K_POINTER) {
if (ddp->dd_next->dd_kind == CTF_K_FUNCTION) {
tip->dtt_object = dtp->dt_ddefs->dm_name;
tip->dtt_ctfp = DT_FPTR_CTFP(dtp);
tip->dtt_type = DT_FPTR_TYPE(dtp);
return (0);
}

if ((rv = dt_decl_type(ddp->dd_next, tip)) == 0 &&
(rv = dt_type_pointer(tip)) != 0) {
xywarn(D_UNKNOWN, "cannot find type: %s*: %s\n",
dt_type_name(tip->dtt_ctfp, tip->dtt_type,
n, sizeof (n)), ctf_errmsg(dtp->dt_ctferr));
}

return (rv);
}


























if (ddp->dd_kind == CTF_K_ARRAY) {




if (ddp->dd_node != NULL &&
ddp->dd_node->dn_kind == DT_NODE_TYPE) {
tip->dtt_object = dtp->dt_ddefs->dm_name;
tip->dtt_ctfp = DT_DYN_CTFP(dtp);
tip->dtt_type = DT_DYN_TYPE(dtp);
return (0);
}

if ((rv = dt_decl_type(ddp->dd_next, tip)) != 0)
return (rv);






if (tip->dtt_ctfp != dmp->dm_ctfp &&
tip->dtt_ctfp != ctf_parent_file(dmp->dm_ctfp)) {

tip->dtt_type = ctf_add_type(dmp->dm_ctfp,
tip->dtt_ctfp, tip->dtt_type);
tip->dtt_ctfp = dmp->dm_ctfp;

if (tip->dtt_type == CTF_ERR ||
ctf_update(tip->dtt_ctfp) == CTF_ERR) {
xywarn(D_UNKNOWN, "failed to copy type: %s\n",
ctf_errmsg(ctf_errno(tip->dtt_ctfp)));
return (-1);
}
}





r.ctr_contents = tip->dtt_type;
r.ctr_index = ctf_lookup_by_name(tip->dtt_ctfp, "long");
r.ctr_nelems = ddp->dd_node ?
(uint_t)ddp->dd_node->dn_value : 0;

tip->dtt_object = dmp->dm_name;
tip->dtt_ctfp = dmp->dm_ctfp;
tip->dtt_type = ctf_add_array(dmp->dm_ctfp, CTF_ADD_ROOT, &r);

if (tip->dtt_type == CTF_ERR ||
ctf_update(tip->dtt_ctfp) == CTF_ERR) {
xywarn(D_UNKNOWN, "failed to create array type: %s\n",
ctf_errmsg(ctf_errno(tip->dtt_ctfp)));
return (-1);
}

return (0);
}





name = alloca(ddp->dd_name ? strlen(ddp->dd_name) + 20 : 20);
name[0] = '\0';

switch (ddp->dd_kind) {
case CTF_K_INTEGER:
case CTF_K_FLOAT:
if (ddp->dd_attr & DT_DA_SIGNED)
(void) strcat(name, "signed ");
if (ddp->dd_attr & DT_DA_UNSIGNED)
(void) strcat(name, "unsigned ");
if (ddp->dd_attr & DT_DA_SHORT)
(void) strcat(name, "short ");
if (ddp->dd_attr & DT_DA_LONG)
(void) strcat(name, "long ");
if (ddp->dd_attr & DT_DA_LONGLONG)
(void) strcat(name, "long long ");
if (ddp->dd_attr == 0 && ddp->dd_name == NULL)
(void) strcat(name, "int");
break;
case CTF_K_STRUCT:
(void) strcpy(name, "struct ");
break;
case CTF_K_UNION:
(void) strcpy(name, "union ");
break;
case CTF_K_ENUM:
(void) strcpy(name, "enum ");
break;
case CTF_K_TYPEDEF:
break;
default:
xywarn(D_UNKNOWN, "internal error -- "
"bad decl kind %u\n", ddp->dd_kind);
return (-1);
}





if (ddp->dd_name != NULL && (ddp->dd_kind != CTF_K_INTEGER ||
(ddp->dd_attr & (DT_DA_SHORT | DT_DA_LONG | DT_DA_LONGLONG)) == 0))
(void) strcat(name, ddp->dd_name);






if ((rv = dt_type_lookup(name, tip)) == 0)
return (rv);

switch (ddp->dd_kind) {
case CTF_K_STRUCT:
case CTF_K_UNION:
case CTF_K_ENUM:
type = ctf_add_forward(dmp->dm_ctfp, flag,
ddp->dd_name, ddp->dd_kind);
break;
default:
xywarn(D_UNKNOWN, "failed to resolve type %s: %s\n", name,
dtrace_errmsg(dtp, dtrace_errno(dtp)));
return (rv);
}

if (type == CTF_ERR || ctf_update(dmp->dm_ctfp) == CTF_ERR) {
xywarn(D_UNKNOWN, "failed to add forward tag for %s: %s\n",
name, ctf_errmsg(ctf_errno(dmp->dm_ctfp)));
return (-1);
}

ddp->dd_ctfp = dmp->dm_ctfp;
ddp->dd_type = type;

tip->dtt_object = dmp->dm_name;
tip->dtt_ctfp = dmp->dm_ctfp;
tip->dtt_type = type;

return (0);
}

void
dt_scope_create(dt_scope_t *dsp)
{
dsp->ds_decl = NULL;
dsp->ds_next = NULL;
dsp->ds_ident = NULL;
dsp->ds_ctfp = NULL;
dsp->ds_type = CTF_ERR;
dsp->ds_class = DT_DC_DEFAULT;
dsp->ds_enumval = -1;
}

void
dt_scope_destroy(dt_scope_t *dsp)
{
dt_scope_t *nsp;

for (; dsp != NULL; dsp = nsp) {
dt_decl_free(dsp->ds_decl);
free(dsp->ds_ident);
nsp = dsp->ds_next;
if (dsp != &yypcb->pcb_dstack)
free(dsp);
}
}

void
dt_scope_push(ctf_file_t *ctfp, ctf_id_t type)
{
dt_scope_t *rsp = &yypcb->pcb_dstack;
dt_scope_t *dsp = malloc(sizeof (dt_scope_t));

if (dsp == NULL)
longjmp(yypcb->pcb_jmpbuf, EDT_NOMEM);

dsp->ds_decl = rsp->ds_decl;
dsp->ds_next = rsp->ds_next;
dsp->ds_ident = rsp->ds_ident;
dsp->ds_ctfp = ctfp;
dsp->ds_type = type;
dsp->ds_class = rsp->ds_class;
dsp->ds_enumval = rsp->ds_enumval;

dt_scope_create(rsp);
rsp->ds_next = dsp;
}

dt_decl_t *
dt_scope_pop(void)
{
dt_scope_t *rsp = &yypcb->pcb_dstack;
dt_scope_t *dsp = rsp->ds_next;

if (dsp == NULL)
longjmp(yypcb->pcb_jmpbuf, EDT_NOSCOPE);

if (dsp->ds_ctfp != NULL && ctf_update(dsp->ds_ctfp) == CTF_ERR) {
xyerror(D_UNKNOWN, "failed to update type definitions: %s\n",
ctf_errmsg(ctf_errno(dsp->ds_ctfp)));
}

dt_decl_free(rsp->ds_decl);
free(rsp->ds_ident);

rsp->ds_decl = dsp->ds_decl;
rsp->ds_next = dsp->ds_next;
rsp->ds_ident = dsp->ds_ident;
rsp->ds_ctfp = dsp->ds_ctfp;
rsp->ds_type = dsp->ds_type;
rsp->ds_class = dsp->ds_class;
rsp->ds_enumval = dsp->ds_enumval;

free(dsp);
return (rsp->ds_decl);
}
