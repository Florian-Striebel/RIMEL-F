






























#include <sys/types.h>
#include <sys/sysmacros.h>
#include <sys/isa_defs.h>

#include <strings.h>
#include <stdlib.h>
#include <setjmp.h>
#include <assert.h>
#include <errno.h>

#include <dt_impl.h>
#include <dt_grammar.h>
#include <dt_parser.h>
#include <dt_provider.h>

static void dt_cg_node(dt_node_t *, dt_irlist_t *, dt_regset_t *);

static dt_irnode_t *
dt_cg_node_alloc(uint_t label, dif_instr_t instr)
{
dt_irnode_t *dip = malloc(sizeof (dt_irnode_t));

if (dip == NULL)
longjmp(yypcb->pcb_jmpbuf, EDT_NOMEM);

dip->di_label = label;
dip->di_instr = instr;
dip->di_extern = NULL;
dip->di_next = NULL;

return (dip);
}






static ctf_file_t *
dt_cg_membinfo(ctf_file_t *fp, ctf_id_t type, const char *s, ctf_membinfo_t *mp)
{
while (ctf_type_kind(fp, type) == CTF_K_FORWARD) {
char n[DT_TYPE_NAMELEN];
dtrace_typeinfo_t dtt;

if (ctf_type_name(fp, type, n, sizeof (n)) == NULL ||
dt_type_lookup(n, &dtt) == -1 || (
dtt.dtt_ctfp == fp && dtt.dtt_type == type))
break;

fp = dtt.dtt_ctfp;
type = ctf_type_resolve(fp, dtt.dtt_type);
}

if (ctf_member_info(fp, type, s, mp) == CTF_ERR)
return (NULL);

return (fp);
}

static void
dt_cg_xsetx(dt_irlist_t *dlp, dt_ident_t *idp, uint_t lbl, int reg, uint64_t x)
{
int flag = idp != NULL ? DT_INT_PRIVATE : DT_INT_SHARED;
int intoff = dt_inttab_insert(yypcb->pcb_inttab, x, flag);
dif_instr_t instr = DIF_INSTR_SETX((uint_t)intoff, reg);

if (intoff == -1)
longjmp(yypcb->pcb_jmpbuf, EDT_NOMEM);

if (intoff > DIF_INTOFF_MAX)
longjmp(yypcb->pcb_jmpbuf, EDT_INT2BIG);

dt_irlist_append(dlp, dt_cg_node_alloc(lbl, instr));

if (idp != NULL)
dlp->dl_last->di_extern = idp;
}

static void
dt_cg_setx(dt_irlist_t *dlp, int reg, uint64_t x)
{
dt_cg_xsetx(dlp, NULL, DT_LBL_NONE, reg, x);
}






static size_t
clp2(size_t x)
{
x--;

x |= (x >> 1);
x |= (x >> 2);
x |= (x >> 4);
x |= (x >> 8);
x |= (x >> 16);

return (x + 1);
}









static uint_t
dt_cg_load(dt_node_t *dnp, ctf_file_t *ctfp, ctf_id_t type)
{
static const uint_t ops[] = {
DIF_OP_LDUB, DIF_OP_LDUH, 0, DIF_OP_LDUW,
0, 0, 0, DIF_OP_LDX,
DIF_OP_LDSB, DIF_OP_LDSH, 0, DIF_OP_LDSW,
0, 0, 0, DIF_OP_LDX,
DIF_OP_ULDUB, DIF_OP_ULDUH, 0, DIF_OP_ULDUW,
0, 0, 0, DIF_OP_ULDX,
DIF_OP_ULDSB, DIF_OP_ULDSH, 0, DIF_OP_ULDSW,
0, 0, 0, DIF_OP_ULDX,
};

ctf_encoding_t e;
ssize_t size;






if ((dnp->dn_flags & DT_NF_BITFIELD) &&
ctf_type_encoding(ctfp, type, &e) != CTF_ERR)
size = clp2(P2ROUNDUP(e.cte_bits, NBBY) / NBBY);
else
size = ctf_type_size(ctfp, type);

if (size < 1 || size > 8 || (size & (size - 1)) != 0) {
xyerror(D_UNKNOWN, "internal error -- cg cannot load "
"size %ld when passed by value\n", (long)size);
}

size--;

if (dnp->dn_flags & DT_NF_SIGNED)
size |= 0x08;
if (dnp->dn_flags & DT_NF_USERLAND)
size |= 0x10;

return (ops[size]);
}

static void
dt_cg_ptrsize(dt_node_t *dnp, dt_irlist_t *dlp, dt_regset_t *drp,
uint_t op, int dreg)
{
ctf_file_t *ctfp = dnp->dn_ctfp;
ctf_arinfo_t r;
dif_instr_t instr;
ctf_id_t type;
uint_t kind;
ssize_t size;
int sreg;

type = ctf_type_resolve(ctfp, dnp->dn_type);
kind = ctf_type_kind(ctfp, type);
assert(kind == CTF_K_POINTER || kind == CTF_K_ARRAY);

if (kind == CTF_K_ARRAY) {
if (ctf_array_info(ctfp, type, &r) != 0) {
yypcb->pcb_hdl->dt_ctferr = ctf_errno(ctfp);
longjmp(yypcb->pcb_jmpbuf, EDT_CTF);
}
type = r.ctr_contents;
} else
type = ctf_type_reference(ctfp, type);

if ((size = ctf_type_size(ctfp, type)) == 1)
return;

sreg = dt_regset_alloc(drp);
dt_cg_setx(dlp, sreg, size);
instr = DIF_INSTR_FMT(op, dreg, sreg, dreg);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));
dt_regset_free(drp, sreg);
}


















static void
dt_cg_field_get(dt_node_t *dnp, dt_irlist_t *dlp, dt_regset_t *drp,
ctf_file_t *fp, const ctf_membinfo_t *mp)
{
ctf_encoding_t e;
dif_instr_t instr;
uint64_t shift;
int r1, r2;

if (ctf_type_encoding(fp, mp->ctm_type, &e) != 0 || e.cte_bits > 64) {
xyerror(D_UNKNOWN, "cg: bad field: off %lu type <%ld> "
"bits %u\n", mp->ctm_offset, mp->ctm_type, e.cte_bits);
}

assert(dnp->dn_op == DT_TOK_PTR || dnp->dn_op == DT_TOK_DOT);
r1 = dnp->dn_left->dn_reg;
r2 = dt_regset_alloc(drp);












if (dnp->dn_flags & DT_NF_SIGNED) {
#if BYTE_ORDER == _BIG_ENDIAN
shift = clp2(P2ROUNDUP(e.cte_bits, NBBY) / NBBY) * NBBY -
mp->ctm_offset % NBBY;
#else
shift = mp->ctm_offset % NBBY + e.cte_bits;
#endif
dt_cg_setx(dlp, r2, 64 - shift);
instr = DIF_INSTR_FMT(DIF_OP_SLL, r1, r2, r1);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));

dt_cg_setx(dlp, r2, 64 - e.cte_bits);
instr = DIF_INSTR_FMT(DIF_OP_SRA, r1, r2, r1);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));
} else {
#if BYTE_ORDER == _BIG_ENDIAN
shift = clp2(P2ROUNDUP(e.cte_bits, NBBY) / NBBY) * NBBY -
(mp->ctm_offset % NBBY + e.cte_bits);
#else
shift = mp->ctm_offset % NBBY;
#endif
dt_cg_setx(dlp, r2, shift);
instr = DIF_INSTR_FMT(DIF_OP_SRL, r1, r2, r1);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));

dt_cg_setx(dlp, r2, (1ULL << e.cte_bits) - 1);
instr = DIF_INSTR_FMT(DIF_OP_AND, r1, r2, r1);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));
}

dt_regset_free(drp, r2);
}























static int
dt_cg_field_set(dt_node_t *src, dt_irlist_t *dlp,
dt_regset_t *drp, dt_node_t *dst)
{
uint64_t cmask, fmask, shift;
dif_instr_t instr;
int r1, r2, r3;

ctf_membinfo_t m;
ctf_encoding_t e;
ctf_file_t *fp, *ofp;
ctf_id_t type;

assert(dst->dn_op == DT_TOK_PTR || dst->dn_op == DT_TOK_DOT);
assert(dst->dn_right->dn_kind == DT_NODE_IDENT);

fp = dst->dn_left->dn_ctfp;
type = ctf_type_resolve(fp, dst->dn_left->dn_type);

if (dst->dn_op == DT_TOK_PTR) {
type = ctf_type_reference(fp, type);
type = ctf_type_resolve(fp, type);
}

if ((fp = dt_cg_membinfo(ofp = fp, type,
dst->dn_right->dn_string, &m)) == NULL) {
yypcb->pcb_hdl->dt_ctferr = ctf_errno(ofp);
longjmp(yypcb->pcb_jmpbuf, EDT_CTF);
}

if (ctf_type_encoding(fp, m.ctm_type, &e) != 0 || e.cte_bits > 64) {
xyerror(D_UNKNOWN, "cg: bad field: off %lu type <%ld> "
"bits %u\n", m.ctm_offset, m.ctm_type, e.cte_bits);
}

r1 = dt_regset_alloc(drp);
r2 = dt_regset_alloc(drp);
r3 = dt_regset_alloc(drp);









#if BYTE_ORDER == _BIG_ENDIAN
shift = clp2(P2ROUNDUP(e.cte_bits, NBBY) / NBBY) * NBBY -
(m.ctm_offset % NBBY + e.cte_bits);
#else
shift = m.ctm_offset % NBBY;
#endif
fmask = (1ULL << e.cte_bits) - 1;
cmask = ~(fmask << shift);

instr = DIF_INSTR_LOAD(
dt_cg_load(dst, fp, m.ctm_type), dst->dn_reg, r1);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));

dt_cg_setx(dlp, r2, cmask);
instr = DIF_INSTR_FMT(DIF_OP_AND, r1, r2, r1);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));

dt_cg_setx(dlp, r2, fmask);
instr = DIF_INSTR_FMT(DIF_OP_AND, src->dn_reg, r2, r2);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));

dt_cg_setx(dlp, r3, shift);
instr = DIF_INSTR_FMT(DIF_OP_SLL, r2, r3, r2);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));

instr = DIF_INSTR_FMT(DIF_OP_OR, r1, r2, r1);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));

dt_regset_free(drp, r3);
dt_regset_free(drp, r2);

return (r1);
}

static void
dt_cg_store(dt_node_t *src, dt_irlist_t *dlp, dt_regset_t *drp, dt_node_t *dst)
{
ctf_encoding_t e;
dif_instr_t instr;
size_t size;
int reg;






if ((dst->dn_flags & DT_NF_BITFIELD) &&
ctf_type_encoding(dst->dn_ctfp, dst->dn_type, &e) != CTF_ERR)
size = clp2(P2ROUNDUP(e.cte_bits, NBBY) / NBBY);
else
size = dt_node_type_size(src);

if (src->dn_flags & DT_NF_REF) {
reg = dt_regset_alloc(drp);
dt_cg_setx(dlp, reg, size);
instr = DIF_INSTR_COPYS(src->dn_reg, reg, dst->dn_reg);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));
dt_regset_free(drp, reg);
} else {
if (dst->dn_flags & DT_NF_BITFIELD)
reg = dt_cg_field_set(src, dlp, drp, dst);
else
reg = src->dn_reg;

switch (size) {
case 1:
instr = DIF_INSTR_STORE(DIF_OP_STB, reg, dst->dn_reg);
break;
case 2:
instr = DIF_INSTR_STORE(DIF_OP_STH, reg, dst->dn_reg);
break;
case 4:
instr = DIF_INSTR_STORE(DIF_OP_STW, reg, dst->dn_reg);
break;
case 8:
instr = DIF_INSTR_STORE(DIF_OP_STX, reg, dst->dn_reg);
break;
default:
xyerror(D_UNKNOWN, "internal error -- cg cannot store "
"size %lu when passed by value\n", (ulong_t)size);
}
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));

if (dst->dn_flags & DT_NF_BITFIELD)
dt_regset_free(drp, reg);
}
}








static void
dt_cg_typecast(const dt_node_t *src, const dt_node_t *dst,
dt_irlist_t *dlp, dt_regset_t *drp)
{
size_t srcsize = dt_node_type_size(src);
size_t dstsize = dt_node_type_size(dst);

dif_instr_t instr;
int rg;

if (!dt_node_is_scalar(dst))
return;
if (dstsize == srcsize &&
((src->dn_flags ^ dst->dn_flags) & DT_NF_SIGNED) != 0)
return;
if (dstsize > srcsize && (src->dn_flags & DT_NF_SIGNED) == 0)
return;

rg = dt_regset_alloc(drp);

if (dstsize > srcsize) {
int n = sizeof (uint64_t) * NBBY - srcsize * NBBY;
int s = (dstsize - srcsize) * NBBY;

dt_cg_setx(dlp, rg, n);

instr = DIF_INSTR_FMT(DIF_OP_SLL, src->dn_reg, rg, dst->dn_reg);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));

if ((dst->dn_flags & DT_NF_SIGNED) || n == s) {
instr = DIF_INSTR_FMT(DIF_OP_SRA,
dst->dn_reg, rg, dst->dn_reg);
dt_irlist_append(dlp,
dt_cg_node_alloc(DT_LBL_NONE, instr));
} else {
dt_cg_setx(dlp, rg, s);
instr = DIF_INSTR_FMT(DIF_OP_SRA,
dst->dn_reg, rg, dst->dn_reg);
dt_irlist_append(dlp,
dt_cg_node_alloc(DT_LBL_NONE, instr));
dt_cg_setx(dlp, rg, n - s);
instr = DIF_INSTR_FMT(DIF_OP_SRL,
dst->dn_reg, rg, dst->dn_reg);
dt_irlist_append(dlp,
dt_cg_node_alloc(DT_LBL_NONE, instr));
}
} else if (dstsize != sizeof (uint64_t)) {
int n = sizeof (uint64_t) * NBBY - dstsize * NBBY;

dt_cg_setx(dlp, rg, n);

instr = DIF_INSTR_FMT(DIF_OP_SLL, src->dn_reg, rg, dst->dn_reg);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));

instr = DIF_INSTR_FMT((dst->dn_flags & DT_NF_SIGNED) ?
DIF_OP_SRA : DIF_OP_SRL, dst->dn_reg, rg, dst->dn_reg);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));
}

dt_regset_free(drp, rg);
}













static void
dt_cg_arglist(dt_ident_t *idp, dt_node_t *args,
dt_irlist_t *dlp, dt_regset_t *drp)
{
const dt_idsig_t *isp = idp->di_data;
dt_node_t *dnp;
int i = 0;

for (dnp = args; dnp != NULL; dnp = dnp->dn_list)
dt_cg_node(dnp, dlp, drp);

dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, DIF_INSTR_FLUSHTS));

for (dnp = args; dnp != NULL; dnp = dnp->dn_list, i++) {
dtrace_diftype_t t;
dif_instr_t instr;
uint_t op;
int reg;

dt_node_diftype(yypcb->pcb_hdl, dnp, &t);

isp->dis_args[i].dn_reg = dnp->dn_reg;
dt_cg_typecast(dnp, &isp->dis_args[i], dlp, drp);
isp->dis_args[i].dn_reg = -1;

if (t.dtdt_flags & DIF_TF_BYREF) {
op = DIF_OP_PUSHTR;
if (t.dtdt_size != 0) {
reg = dt_regset_alloc(drp);
dt_cg_setx(dlp, reg, t.dtdt_size);
} else {
reg = DIF_REG_R0;
}
} else {
op = DIF_OP_PUSHTV;
reg = DIF_REG_R0;
}

instr = DIF_INSTR_PUSHTS(op, t.dtdt_kind, reg, dnp->dn_reg);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));
dt_regset_free(drp, dnp->dn_reg);

if (reg != DIF_REG_R0)
dt_regset_free(drp, reg);
}

if (i > yypcb->pcb_hdl->dt_conf.dtc_diftupregs)
longjmp(yypcb->pcb_jmpbuf, EDT_NOTUPREG);
}

static void
dt_cg_arithmetic_op(dt_node_t *dnp, dt_irlist_t *dlp,
dt_regset_t *drp, uint_t op)
{
int is_ptr_op = (dnp->dn_op == DT_TOK_ADD || dnp->dn_op == DT_TOK_SUB ||
dnp->dn_op == DT_TOK_ADD_EQ || dnp->dn_op == DT_TOK_SUB_EQ);

int lp_is_ptr = dt_node_is_pointer(dnp->dn_left);
int rp_is_ptr = dt_node_is_pointer(dnp->dn_right);

dif_instr_t instr;

if (lp_is_ptr && rp_is_ptr) {
assert(dnp->dn_op == DT_TOK_SUB);
is_ptr_op = 0;
}

dt_cg_node(dnp->dn_left, dlp, drp);
if (is_ptr_op && rp_is_ptr)
dt_cg_ptrsize(dnp, dlp, drp, DIF_OP_MUL, dnp->dn_left->dn_reg);

dt_cg_node(dnp->dn_right, dlp, drp);
if (is_ptr_op && lp_is_ptr)
dt_cg_ptrsize(dnp, dlp, drp, DIF_OP_MUL, dnp->dn_right->dn_reg);

instr = DIF_INSTR_FMT(op, dnp->dn_left->dn_reg,
dnp->dn_right->dn_reg, dnp->dn_left->dn_reg);

dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));
dt_regset_free(drp, dnp->dn_right->dn_reg);
dnp->dn_reg = dnp->dn_left->dn_reg;

if (lp_is_ptr && rp_is_ptr)
dt_cg_ptrsize(dnp->dn_right,
dlp, drp, DIF_OP_UDIV, dnp->dn_reg);
}

static uint_t
dt_cg_stvar(const dt_ident_t *idp)
{
static const uint_t aops[] = { DIF_OP_STGAA, DIF_OP_STTAA, DIF_OP_NOP };
static const uint_t sops[] = { DIF_OP_STGS, DIF_OP_STTS, DIF_OP_STLS };

uint_t i = (((idp->di_flags & DT_IDFLG_LOCAL) != 0) << 1) |
((idp->di_flags & DT_IDFLG_TLS) != 0);

return (idp->di_kind == DT_IDENT_ARRAY ? aops[i] : sops[i]);
}

static void
dt_cg_prearith_op(dt_node_t *dnp, dt_irlist_t *dlp, dt_regset_t *drp, uint_t op)
{
ctf_file_t *ctfp = dnp->dn_ctfp;
dif_instr_t instr;
ctf_id_t type;
ssize_t size = 1;
int reg;

if (dt_node_is_pointer(dnp)) {
type = ctf_type_resolve(ctfp, dnp->dn_type);
assert(ctf_type_kind(ctfp, type) == CTF_K_POINTER);
size = ctf_type_size(ctfp, ctf_type_reference(ctfp, type));
}

dt_cg_node(dnp->dn_child, dlp, drp);
dnp->dn_reg = dnp->dn_child->dn_reg;

reg = dt_regset_alloc(drp);
dt_cg_setx(dlp, reg, size);

instr = DIF_INSTR_FMT(op, dnp->dn_reg, reg, dnp->dn_reg);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));
dt_regset_free(drp, reg);








if (dnp->dn_child->dn_kind == DT_NODE_VAR) {
dt_ident_t *idp = dt_ident_resolve(dnp->dn_child->dn_ident);

idp->di_flags |= DT_IDFLG_DIFW;
instr = DIF_INSTR_STV(dt_cg_stvar(idp),
idp->di_id, dnp->dn_reg);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));
} else {
uint_t rbit = dnp->dn_child->dn_flags & DT_NF_REF;

assert(dnp->dn_child->dn_flags & DT_NF_WRITABLE);
assert(dnp->dn_child->dn_flags & DT_NF_LVALUE);

dnp->dn_child->dn_flags |= DT_NF_REF;
dt_cg_node(dnp->dn_child, dlp, drp);

dt_cg_store(dnp, dlp, drp, dnp->dn_child);
dt_regset_free(drp, dnp->dn_child->dn_reg);

dnp->dn_left->dn_flags &= ~DT_NF_REF;
dnp->dn_left->dn_flags |= rbit;
}
}

static void
dt_cg_postarith_op(dt_node_t *dnp, dt_irlist_t *dlp,
dt_regset_t *drp, uint_t op)
{
ctf_file_t *ctfp = dnp->dn_ctfp;
dif_instr_t instr;
ctf_id_t type;
ssize_t size = 1;
int nreg;

if (dt_node_is_pointer(dnp)) {
type = ctf_type_resolve(ctfp, dnp->dn_type);
assert(ctf_type_kind(ctfp, type) == CTF_K_POINTER);
size = ctf_type_size(ctfp, ctf_type_reference(ctfp, type));
}

dt_cg_node(dnp->dn_child, dlp, drp);
dnp->dn_reg = dnp->dn_child->dn_reg;

nreg = dt_regset_alloc(drp);
dt_cg_setx(dlp, nreg, size);
instr = DIF_INSTR_FMT(op, dnp->dn_reg, nreg, nreg);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));








if (dnp->dn_child->dn_kind == DT_NODE_VAR) {
dt_ident_t *idp = dt_ident_resolve(dnp->dn_child->dn_ident);

idp->di_flags |= DT_IDFLG_DIFW;
instr = DIF_INSTR_STV(dt_cg_stvar(idp), idp->di_id, nreg);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));
} else {
uint_t rbit = dnp->dn_child->dn_flags & DT_NF_REF;
int oreg = dnp->dn_reg;

assert(dnp->dn_child->dn_flags & DT_NF_WRITABLE);
assert(dnp->dn_child->dn_flags & DT_NF_LVALUE);

dnp->dn_child->dn_flags |= DT_NF_REF;
dt_cg_node(dnp->dn_child, dlp, drp);

dnp->dn_reg = nreg;
dt_cg_store(dnp, dlp, drp, dnp->dn_child);
dnp->dn_reg = oreg;

dt_regset_free(drp, dnp->dn_child->dn_reg);
dnp->dn_left->dn_flags &= ~DT_NF_REF;
dnp->dn_left->dn_flags |= rbit;
}

dt_regset_free(drp, nreg);
}






static int
dt_cg_compare_signed(dt_node_t *dnp)
{
dt_node_t dn;

if (dt_node_is_string(dnp->dn_left) ||
dt_node_is_string(dnp->dn_right))
return (1);
else if (!dt_node_is_arith(dnp->dn_left) ||
!dt_node_is_arith(dnp->dn_right))
return (0);

bzero(&dn, sizeof (dn));
dt_node_promote(dnp->dn_left, dnp->dn_right, &dn);
return (dn.dn_flags & DT_NF_SIGNED);
}

static void
dt_cg_compare_op(dt_node_t *dnp, dt_irlist_t *dlp, dt_regset_t *drp, uint_t op)
{
uint_t lbl_true = dt_irlist_label(dlp);
uint_t lbl_post = dt_irlist_label(dlp);

dif_instr_t instr;
uint_t opc;

dt_cg_node(dnp->dn_left, dlp, drp);
dt_cg_node(dnp->dn_right, dlp, drp);

if (dt_node_is_string(dnp->dn_left) || dt_node_is_string(dnp->dn_right))
opc = DIF_OP_SCMP;
else
opc = DIF_OP_CMP;

instr = DIF_INSTR_CMP(opc, dnp->dn_left->dn_reg, dnp->dn_right->dn_reg);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));
dt_regset_free(drp, dnp->dn_right->dn_reg);
dnp->dn_reg = dnp->dn_left->dn_reg;

instr = DIF_INSTR_BRANCH(op, lbl_true);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));

instr = DIF_INSTR_MOV(DIF_REG_R0, dnp->dn_reg);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));

instr = DIF_INSTR_BRANCH(DIF_OP_BA, lbl_post);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));

dt_cg_xsetx(dlp, NULL, lbl_true, dnp->dn_reg, 1);
dt_irlist_append(dlp, dt_cg_node_alloc(lbl_post, DIF_INSTR_NOP));
}











static void
dt_cg_ternary_op(dt_node_t *dnp, dt_irlist_t *dlp, dt_regset_t *drp)
{
uint_t lbl_false = dt_irlist_label(dlp);
uint_t lbl_post = dt_irlist_label(dlp);

dif_instr_t instr;
dt_irnode_t *dip;

dt_cg_node(dnp->dn_expr, dlp, drp);
instr = DIF_INSTR_TST(dnp->dn_expr->dn_reg);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));
dt_regset_free(drp, dnp->dn_expr->dn_reg);

instr = DIF_INSTR_BRANCH(DIF_OP_BE, lbl_false);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));

dt_cg_node(dnp->dn_left, dlp, drp);
instr = DIF_INSTR_MOV(dnp->dn_left->dn_reg, DIF_REG_R0);
dip = dt_cg_node_alloc(DT_LBL_NONE, instr);
dt_irlist_append(dlp, dip);
dt_regset_free(drp, dnp->dn_left->dn_reg);

instr = DIF_INSTR_BRANCH(DIF_OP_BA, lbl_post);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));

dt_irlist_append(dlp, dt_cg_node_alloc(lbl_false, DIF_INSTR_NOP));
dt_cg_node(dnp->dn_right, dlp, drp);
dnp->dn_reg = dnp->dn_right->dn_reg;






dip->di_instr = DIF_INSTR_MOV(dnp->dn_left->dn_reg, dnp->dn_reg);
dt_irlist_append(dlp, dt_cg_node_alloc(lbl_post, DIF_INSTR_NOP));
}

static void
dt_cg_logical_and(dt_node_t *dnp, dt_irlist_t *dlp, dt_regset_t *drp)
{
uint_t lbl_false = dt_irlist_label(dlp);
uint_t lbl_post = dt_irlist_label(dlp);

dif_instr_t instr;

dt_cg_node(dnp->dn_left, dlp, drp);
instr = DIF_INSTR_TST(dnp->dn_left->dn_reg);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));
dt_regset_free(drp, dnp->dn_left->dn_reg);

instr = DIF_INSTR_BRANCH(DIF_OP_BE, lbl_false);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));

dt_cg_node(dnp->dn_right, dlp, drp);
instr = DIF_INSTR_TST(dnp->dn_right->dn_reg);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));
dnp->dn_reg = dnp->dn_right->dn_reg;

instr = DIF_INSTR_BRANCH(DIF_OP_BE, lbl_false);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));

dt_cg_setx(dlp, dnp->dn_reg, 1);

instr = DIF_INSTR_BRANCH(DIF_OP_BA, lbl_post);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));

instr = DIF_INSTR_MOV(DIF_REG_R0, dnp->dn_reg);
dt_irlist_append(dlp, dt_cg_node_alloc(lbl_false, instr));

dt_irlist_append(dlp, dt_cg_node_alloc(lbl_post, DIF_INSTR_NOP));
}

static void
dt_cg_logical_xor(dt_node_t *dnp, dt_irlist_t *dlp, dt_regset_t *drp)
{
uint_t lbl_next = dt_irlist_label(dlp);
uint_t lbl_tail = dt_irlist_label(dlp);

dif_instr_t instr;

dt_cg_node(dnp->dn_left, dlp, drp);
instr = DIF_INSTR_TST(dnp->dn_left->dn_reg);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));

instr = DIF_INSTR_BRANCH(DIF_OP_BE, lbl_next);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));
dt_cg_setx(dlp, dnp->dn_left->dn_reg, 1);

dt_irlist_append(dlp, dt_cg_node_alloc(lbl_next, DIF_INSTR_NOP));
dt_cg_node(dnp->dn_right, dlp, drp);

instr = DIF_INSTR_TST(dnp->dn_right->dn_reg);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));

instr = DIF_INSTR_BRANCH(DIF_OP_BE, lbl_tail);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));
dt_cg_setx(dlp, dnp->dn_right->dn_reg, 1);

instr = DIF_INSTR_FMT(DIF_OP_XOR, dnp->dn_left->dn_reg,
dnp->dn_right->dn_reg, dnp->dn_left->dn_reg);

dt_irlist_append(dlp, dt_cg_node_alloc(lbl_tail, instr));

dt_regset_free(drp, dnp->dn_right->dn_reg);
dnp->dn_reg = dnp->dn_left->dn_reg;
}

static void
dt_cg_logical_or(dt_node_t *dnp, dt_irlist_t *dlp, dt_regset_t *drp)
{
uint_t lbl_true = dt_irlist_label(dlp);
uint_t lbl_false = dt_irlist_label(dlp);
uint_t lbl_post = dt_irlist_label(dlp);

dif_instr_t instr;

dt_cg_node(dnp->dn_left, dlp, drp);
instr = DIF_INSTR_TST(dnp->dn_left->dn_reg);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));
dt_regset_free(drp, dnp->dn_left->dn_reg);

instr = DIF_INSTR_BRANCH(DIF_OP_BNE, lbl_true);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));

dt_cg_node(dnp->dn_right, dlp, drp);
instr = DIF_INSTR_TST(dnp->dn_right->dn_reg);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));
dnp->dn_reg = dnp->dn_right->dn_reg;

instr = DIF_INSTR_BRANCH(DIF_OP_BE, lbl_false);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));

dt_cg_xsetx(dlp, NULL, lbl_true, dnp->dn_reg, 1);

instr = DIF_INSTR_BRANCH(DIF_OP_BA, lbl_post);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));

instr = DIF_INSTR_MOV(DIF_REG_R0, dnp->dn_reg);
dt_irlist_append(dlp, dt_cg_node_alloc(lbl_false, instr));

dt_irlist_append(dlp, dt_cg_node_alloc(lbl_post, DIF_INSTR_NOP));
}

static void
dt_cg_logical_neg(dt_node_t *dnp, dt_irlist_t *dlp, dt_regset_t *drp)
{
uint_t lbl_zero = dt_irlist_label(dlp);
uint_t lbl_post = dt_irlist_label(dlp);

dif_instr_t instr;

dt_cg_node(dnp->dn_child, dlp, drp);
dnp->dn_reg = dnp->dn_child->dn_reg;

instr = DIF_INSTR_TST(dnp->dn_reg);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));

instr = DIF_INSTR_BRANCH(DIF_OP_BE, lbl_zero);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));

instr = DIF_INSTR_MOV(DIF_REG_R0, dnp->dn_reg);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));

instr = DIF_INSTR_BRANCH(DIF_OP_BA, lbl_post);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));

dt_cg_xsetx(dlp, NULL, lbl_zero, dnp->dn_reg, 1);
dt_irlist_append(dlp, dt_cg_node_alloc(lbl_post, DIF_INSTR_NOP));
}

static void
dt_cg_asgn_op(dt_node_t *dnp, dt_irlist_t *dlp, dt_regset_t *drp)
{
dif_instr_t instr;
dt_ident_t *idp;







if ((idp = dt_node_resolve(dnp->dn_right, DT_IDENT_XLSOU)) != NULL) {
ctf_membinfo_t ctm;
dt_xlator_t *dxp = idp->di_data;
dt_node_t *mnp, dn, mn;
int r1, r2;







bzero(&dn, sizeof (dt_node_t));
dn.dn_kind = DT_NODE_OP2;
dn.dn_op = DT_TOK_DOT;
dn.dn_left = dnp;
dn.dn_right = &mn;

bzero(&mn, sizeof (dt_node_t));
mn.dn_kind = DT_NODE_IDENT;
mn.dn_op = DT_TOK_IDENT;






r1 = dt_regset_alloc(drp);
dt_cg_setx(dlp, r1,
ctf_type_size(dxp->dx_dst_ctfp, dxp->dx_dst_base));

instr = DIF_INSTR_ALLOCS(r1, r1);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));







dxp->dx_ident->di_flags |= DT_IDFLG_CGREG;
dxp->dx_ident->di_id = dnp->dn_right->dn_reg;

for (mnp = dxp->dx_members; mnp != NULL; mnp = mnp->dn_list) {




dt_cg_node(mnp->dn_membexpr, dlp, drp);
mnp->dn_reg = mnp->dn_membexpr->dn_reg;
dt_cg_typecast(mnp->dn_membexpr, mnp, dlp, drp);






if (ctf_member_info(dxp->dx_dst_ctfp, dxp->dx_dst_base,
mnp->dn_membname, &ctm) == CTF_ERR) {
yypcb->pcb_hdl->dt_ctferr =
ctf_errno(dxp->dx_dst_ctfp);
longjmp(yypcb->pcb_jmpbuf, EDT_CTF);
}







if (ctm.ctm_offset != 0) {
r2 = dt_regset_alloc(drp);







dt_cg_setx(dlp, r2, ctm.ctm_offset / NBBY);
instr = DIF_INSTR_FMT(DIF_OP_ADD, r1, r2, r2);
dt_irlist_append(dlp,
dt_cg_node_alloc(DT_LBL_NONE, instr));

dt_node_type_propagate(mnp, &dn);
dn.dn_right->dn_string = mnp->dn_membname;
dn.dn_reg = r2;

dt_cg_store(mnp, dlp, drp, &dn);
dt_regset_free(drp, r2);

} else {
dt_node_type_propagate(mnp, &dn);
dn.dn_right->dn_string = mnp->dn_membname;
dn.dn_reg = r1;

dt_cg_store(mnp, dlp, drp, &dn);
}

dt_regset_free(drp, mnp->dn_reg);
}

dxp->dx_ident->di_flags &= ~DT_IDFLG_CGREG;
dxp->dx_ident->di_id = 0;

if (dnp->dn_right->dn_reg != -1)
dt_regset_free(drp, dnp->dn_right->dn_reg);

assert(dnp->dn_reg == dnp->dn_right->dn_reg);
dnp->dn_reg = r1;
}








if (dnp->dn_left->dn_kind == DT_NODE_VAR) {
idp = dt_ident_resolve(dnp->dn_left->dn_ident);

if (idp->di_kind == DT_IDENT_ARRAY)
dt_cg_arglist(idp, dnp->dn_left->dn_args, dlp, drp);

idp->di_flags |= DT_IDFLG_DIFW;
instr = DIF_INSTR_STV(dt_cg_stvar(idp),
idp->di_id, dnp->dn_reg);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));
} else {
uint_t rbit = dnp->dn_left->dn_flags & DT_NF_REF;

assert(dnp->dn_left->dn_flags & DT_NF_WRITABLE);
assert(dnp->dn_left->dn_flags & DT_NF_LVALUE);

dnp->dn_left->dn_flags |= DT_NF_REF;

dt_cg_node(dnp->dn_left, dlp, drp);
dt_cg_store(dnp, dlp, drp, dnp->dn_left);
dt_regset_free(drp, dnp->dn_left->dn_reg);

dnp->dn_left->dn_flags &= ~DT_NF_REF;
dnp->dn_left->dn_flags |= rbit;
}
}

static void
dt_cg_assoc_op(dt_node_t *dnp, dt_irlist_t *dlp, dt_regset_t *drp)
{
dif_instr_t instr;
uint_t op;

assert(dnp->dn_kind == DT_NODE_VAR);
assert(!(dnp->dn_ident->di_flags & DT_IDFLG_LOCAL));
assert(dnp->dn_args != NULL);

dt_cg_arglist(dnp->dn_ident, dnp->dn_args, dlp, drp);

dnp->dn_reg = dt_regset_alloc(drp);

if (dnp->dn_ident->di_flags & DT_IDFLG_TLS)
op = DIF_OP_LDTAA;
else
op = DIF_OP_LDGAA;

dnp->dn_ident->di_flags |= DT_IDFLG_DIFR;
instr = DIF_INSTR_LDV(op, dnp->dn_ident->di_id, dnp->dn_reg);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));


























if (dnp->dn_flags & DT_NF_REF) {
uint_t stvop = op == DIF_OP_LDTAA ? DIF_OP_STTAA : DIF_OP_STGAA;
uint_t label = dt_irlist_label(dlp);

instr = DIF_INSTR_TST(dnp->dn_reg);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));

instr = DIF_INSTR_BRANCH(DIF_OP_BNE, label);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));

dt_cg_setx(dlp, dnp->dn_reg, dt_node_type_size(dnp));
instr = DIF_INSTR_ALLOCS(dnp->dn_reg, dnp->dn_reg);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));

dnp->dn_ident->di_flags |= DT_IDFLG_DIFW;
instr = DIF_INSTR_STV(stvop, dnp->dn_ident->di_id, dnp->dn_reg);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));

instr = DIF_INSTR_LDV(op, dnp->dn_ident->di_id, dnp->dn_reg);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));

dt_irlist_append(dlp, dt_cg_node_alloc(label, DIF_INSTR_NOP));
}
}

static void
dt_cg_array_op(dt_node_t *dnp, dt_irlist_t *dlp, dt_regset_t *drp)
{
dt_probe_t *prp = yypcb->pcb_probe;
uintmax_t saved = dnp->dn_args->dn_value;
dt_ident_t *idp = dnp->dn_ident;

dif_instr_t instr;
uint_t op;
size_t size;
int reg, n;

assert(dnp->dn_kind == DT_NODE_VAR);
assert(!(idp->di_flags & DT_IDFLG_LOCAL));

assert(dnp->dn_args->dn_kind == DT_NODE_INT);
assert(dnp->dn_args->dn_list == NULL);








if (idp->di_id == DIF_VAR_ARGS) {
if ((idp->di_kind == DT_IDENT_XLPTR ||
idp->di_kind == DT_IDENT_XLSOU) &&
dt_xlator_dynamic(idp->di_data)) {
dnp->dn_reg = -1;
return;
}
dnp->dn_args->dn_value = prp->pr_mapping[saved];
}

dt_cg_node(dnp->dn_args, dlp, drp);
dnp->dn_args->dn_value = saved;

dnp->dn_reg = dnp->dn_args->dn_reg;

if (idp->di_flags & DT_IDFLG_TLS)
op = DIF_OP_LDTA;
else
op = DIF_OP_LDGA;

idp->di_flags |= DT_IDFLG_DIFR;

instr = DIF_INSTR_LDA(op, idp->di_id,
dnp->dn_args->dn_reg, dnp->dn_reg);

dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));












if (idp->di_id != DIF_VAR_ARGS || !dt_node_is_scalar(dnp))
return;

if ((size = dt_node_type_size(dnp)) == sizeof (uint64_t))
return;

reg = dt_regset_alloc(drp);
assert(size < sizeof (uint64_t));
n = sizeof (uint64_t) * NBBY - size * NBBY;

dt_cg_setx(dlp, reg, n);

instr = DIF_INSTR_FMT(DIF_OP_SLL, dnp->dn_reg, reg, dnp->dn_reg);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));

instr = DIF_INSTR_FMT((dnp->dn_flags & DT_NF_SIGNED) ?
DIF_OP_SRA : DIF_OP_SRL, dnp->dn_reg, reg, dnp->dn_reg);

dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));
dt_regset_free(drp, reg);
}













static void
dt_cg_inline(dt_node_t *dnp, dt_irlist_t *dlp, dt_regset_t *drp)
{
dt_ident_t *idp = dnp->dn_ident;
dt_idnode_t *inp = idp->di_iarg;

dt_idnode_t *pinp;
dt_node_t *pnp;
int i;

assert(idp->di_flags & DT_IDFLG_INLINE);
assert(idp->di_ops == &dt_idops_inline);

if (idp->di_kind == DT_IDENT_ARRAY) {
for (i = 0, pnp = dnp->dn_args;
pnp != NULL; pnp = pnp->dn_list, i++) {
if (inp->din_argv[i] != NULL) {
pinp = inp->din_argv[i]->di_iarg;
pinp->din_root = pnp;
}
}
}

dt_cg_node(inp->din_root, dlp, drp);
dnp->dn_reg = inp->din_root->dn_reg;
dt_cg_typecast(inp->din_root, dnp, dlp, drp);

if (idp->di_kind == DT_IDENT_ARRAY) {
for (i = 0; i < inp->din_argc; i++) {
pinp = inp->din_argv[i]->di_iarg;
pinp->din_root = NULL;
}
}
}

typedef struct dt_xlmemb {
dt_ident_t *dtxl_idp;
dt_irlist_t *dtxl_dlp;
dt_regset_t *dtxl_drp;
int dtxl_sreg;
int dtxl_dreg;
} dt_xlmemb_t;


static int
dt_cg_xlate_member(const char *name, ctf_id_t type, ulong_t off, void *arg)
{
dt_xlmemb_t *dx = arg;
dt_ident_t *idp = dx->dtxl_idp;
dt_irlist_t *dlp = dx->dtxl_dlp;
dt_regset_t *drp = dx->dtxl_drp;

dt_node_t *mnp;
dt_xlator_t *dxp;

int reg, treg;
uint32_t instr;
size_t size;


dxp = idp->di_data;
mnp = dt_xlator_member(dxp, name);


if (mnp == NULL)
return (0);

dxp->dx_ident->di_flags |= DT_IDFLG_CGREG;
dxp->dx_ident->di_id = dx->dtxl_sreg;

dt_cg_node(mnp->dn_membexpr, dlp, drp);

dxp->dx_ident->di_flags &= ~DT_IDFLG_CGREG;
dxp->dx_ident->di_id = 0;

treg = mnp->dn_membexpr->dn_reg;


reg = dt_regset_alloc(drp);

dt_cg_setx(dlp, reg, off / NBBY);
instr = DIF_INSTR_FMT(DIF_OP_ADD, dx->dtxl_dreg, reg, reg);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));

size = ctf_type_size(mnp->dn_membexpr->dn_ctfp,
mnp->dn_membexpr->dn_type);
if (dt_node_is_scalar(mnp->dn_membexpr)) {



switch (size) {
case 1:
instr = DIF_INSTR_STORE(DIF_OP_STB, treg, reg);
break;
case 2:
instr = DIF_INSTR_STORE(DIF_OP_STH, treg, reg);
break;
case 4:
instr = DIF_INSTR_STORE(DIF_OP_STW, treg, reg);
break;
case 8:
instr = DIF_INSTR_STORE(DIF_OP_STX, treg, reg);
break;
default:
xyerror(D_UNKNOWN, "internal error -- unexpected "
"size: %lu\n", (ulong_t)size);
}

dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));

} else if (dt_node_is_string(mnp->dn_membexpr)) {
int szreg;




szreg = dt_regset_alloc(drp);
dt_cg_setx(dlp, szreg, size);
instr = DIF_INSTR_COPYS(treg, szreg, reg);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));
dt_regset_free(drp, szreg);
} else {
int szreg;




szreg = dt_regset_alloc(drp);
dt_cg_setx(dlp, szreg, size);
dt_irlist_append(dlp,
dt_cg_node_alloc(DT_LBL_NONE, DIF_INSTR_FLUSHTS));
instr = DIF_INSTR_PUSHTS(DIF_OP_PUSHTV, DIF_TYPE_CTF,
DIF_REG_R0, treg);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));
instr = DIF_INSTR_PUSHTS(DIF_OP_PUSHTV, DIF_TYPE_CTF,
DIF_REG_R0, reg);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));
instr = DIF_INSTR_PUSHTS(DIF_OP_PUSHTV, DIF_TYPE_CTF,
DIF_REG_R0, szreg);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));
instr = DIF_INSTR_CALL(DIF_SUBR_BCOPY, szreg);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));
dt_regset_free(drp, szreg);
}

dt_regset_free(drp, reg);
dt_regset_free(drp, treg);

return (0);
}





static int
dt_cg_xlate_expand(dt_node_t *dnp, dt_ident_t *idp, dt_irlist_t *dlp,
dt_regset_t *drp)
{
dt_xlmemb_t dlm;
uint32_t instr;
int dreg;
size_t size;

dreg = dt_regset_alloc(drp);
size = ctf_type_size(dnp->dn_ident->di_ctfp, dnp->dn_ident->di_type);


dt_cg_setx(dlp, dreg, size);

dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, DIF_INSTR_FLUSHTS));

instr = DIF_INSTR_PUSHTS(DIF_OP_PUSHTV, DIF_TYPE_CTF, DIF_REG_R0, dreg);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));

instr = DIF_INSTR_CALL(DIF_SUBR_ALLOCA, dreg);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));


dlm.dtxl_idp = idp;
dlm.dtxl_dlp = dlp;
dlm.dtxl_drp = drp;
dlm.dtxl_sreg = dnp->dn_reg;
dlm.dtxl_dreg = dreg;
(void) ctf_member_iter(dnp->dn_ident->di_ctfp,
dnp->dn_ident->di_type, dt_cg_xlate_member,
&dlm);

return (dreg);
}

static void
dt_cg_node(dt_node_t *dnp, dt_irlist_t *dlp, dt_regset_t *drp)
{
ctf_file_t *ctfp = dnp->dn_ctfp;
ctf_file_t *octfp;
ctf_membinfo_t m;
ctf_id_t type;

dif_instr_t instr;
dt_ident_t *idp;
ssize_t stroff;
uint_t op;

switch (dnp->dn_op) {
case DT_TOK_COMMA:
dt_cg_node(dnp->dn_left, dlp, drp);
dt_regset_free(drp, dnp->dn_left->dn_reg);
dt_cg_node(dnp->dn_right, dlp, drp);
dnp->dn_reg = dnp->dn_right->dn_reg;
break;

case DT_TOK_ASGN:
dt_cg_node(dnp->dn_right, dlp, drp);
dnp->dn_reg = dnp->dn_right->dn_reg;
dt_cg_asgn_op(dnp, dlp, drp);
break;

case DT_TOK_ADD_EQ:
dt_cg_arithmetic_op(dnp, dlp, drp, DIF_OP_ADD);
dt_cg_asgn_op(dnp, dlp, drp);
break;

case DT_TOK_SUB_EQ:
dt_cg_arithmetic_op(dnp, dlp, drp, DIF_OP_SUB);
dt_cg_asgn_op(dnp, dlp, drp);
break;

case DT_TOK_MUL_EQ:
dt_cg_arithmetic_op(dnp, dlp, drp, DIF_OP_MUL);
dt_cg_asgn_op(dnp, dlp, drp);
break;

case DT_TOK_DIV_EQ:
dt_cg_arithmetic_op(dnp, dlp, drp,
(dnp->dn_flags & DT_NF_SIGNED) ? DIF_OP_SDIV : DIF_OP_UDIV);
dt_cg_asgn_op(dnp, dlp, drp);
break;

case DT_TOK_MOD_EQ:
dt_cg_arithmetic_op(dnp, dlp, drp,
(dnp->dn_flags & DT_NF_SIGNED) ? DIF_OP_SREM : DIF_OP_UREM);
dt_cg_asgn_op(dnp, dlp, drp);
break;

case DT_TOK_AND_EQ:
dt_cg_arithmetic_op(dnp, dlp, drp, DIF_OP_AND);
dt_cg_asgn_op(dnp, dlp, drp);
break;

case DT_TOK_XOR_EQ:
dt_cg_arithmetic_op(dnp, dlp, drp, DIF_OP_XOR);
dt_cg_asgn_op(dnp, dlp, drp);
break;

case DT_TOK_OR_EQ:
dt_cg_arithmetic_op(dnp, dlp, drp, DIF_OP_OR);
dt_cg_asgn_op(dnp, dlp, drp);
break;

case DT_TOK_LSH_EQ:
dt_cg_arithmetic_op(dnp, dlp, drp, DIF_OP_SLL);
dt_cg_asgn_op(dnp, dlp, drp);
break;

case DT_TOK_RSH_EQ:
dt_cg_arithmetic_op(dnp, dlp, drp,
(dnp->dn_flags & DT_NF_SIGNED) ? DIF_OP_SRA : DIF_OP_SRL);
dt_cg_asgn_op(dnp, dlp, drp);
break;

case DT_TOK_QUESTION:
dt_cg_ternary_op(dnp, dlp, drp);
break;

case DT_TOK_LOR:
dt_cg_logical_or(dnp, dlp, drp);
break;

case DT_TOK_LXOR:
dt_cg_logical_xor(dnp, dlp, drp);
break;

case DT_TOK_LAND:
dt_cg_logical_and(dnp, dlp, drp);
break;

case DT_TOK_BOR:
dt_cg_arithmetic_op(dnp, dlp, drp, DIF_OP_OR);
break;

case DT_TOK_XOR:
dt_cg_arithmetic_op(dnp, dlp, drp, DIF_OP_XOR);
break;

case DT_TOK_BAND:
dt_cg_arithmetic_op(dnp, dlp, drp, DIF_OP_AND);
break;

case DT_TOK_EQU:
dt_cg_compare_op(dnp, dlp, drp, DIF_OP_BE);
break;

case DT_TOK_NEQ:
dt_cg_compare_op(dnp, dlp, drp, DIF_OP_BNE);
break;

case DT_TOK_LT:
dt_cg_compare_op(dnp, dlp, drp,
dt_cg_compare_signed(dnp) ? DIF_OP_BL : DIF_OP_BLU);
break;

case DT_TOK_LE:
dt_cg_compare_op(dnp, dlp, drp,
dt_cg_compare_signed(dnp) ? DIF_OP_BLE : DIF_OP_BLEU);
break;

case DT_TOK_GT:
dt_cg_compare_op(dnp, dlp, drp,
dt_cg_compare_signed(dnp) ? DIF_OP_BG : DIF_OP_BGU);
break;

case DT_TOK_GE:
dt_cg_compare_op(dnp, dlp, drp,
dt_cg_compare_signed(dnp) ? DIF_OP_BGE : DIF_OP_BGEU);
break;

case DT_TOK_LSH:
dt_cg_arithmetic_op(dnp, dlp, drp, DIF_OP_SLL);
break;

case DT_TOK_RSH:
dt_cg_arithmetic_op(dnp, dlp, drp,
(dnp->dn_flags & DT_NF_SIGNED) ? DIF_OP_SRA : DIF_OP_SRL);
break;

case DT_TOK_ADD:
dt_cg_arithmetic_op(dnp, dlp, drp, DIF_OP_ADD);
break;

case DT_TOK_SUB:
dt_cg_arithmetic_op(dnp, dlp, drp, DIF_OP_SUB);
break;

case DT_TOK_MUL:
dt_cg_arithmetic_op(dnp, dlp, drp, DIF_OP_MUL);
break;

case DT_TOK_DIV:
dt_cg_arithmetic_op(dnp, dlp, drp,
(dnp->dn_flags & DT_NF_SIGNED) ? DIF_OP_SDIV : DIF_OP_UDIV);
break;

case DT_TOK_MOD:
dt_cg_arithmetic_op(dnp, dlp, drp,
(dnp->dn_flags & DT_NF_SIGNED) ? DIF_OP_SREM : DIF_OP_UREM);
break;

case DT_TOK_LNEG:
dt_cg_logical_neg(dnp, dlp, drp);
break;

case DT_TOK_BNEG:
dt_cg_node(dnp->dn_child, dlp, drp);
dnp->dn_reg = dnp->dn_child->dn_reg;
instr = DIF_INSTR_NOT(dnp->dn_reg, dnp->dn_reg);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));
break;

case DT_TOK_PREINC:
dt_cg_prearith_op(dnp, dlp, drp, DIF_OP_ADD);
break;

case DT_TOK_POSTINC:
dt_cg_postarith_op(dnp, dlp, drp, DIF_OP_ADD);
break;

case DT_TOK_PREDEC:
dt_cg_prearith_op(dnp, dlp, drp, DIF_OP_SUB);
break;

case DT_TOK_POSTDEC:
dt_cg_postarith_op(dnp, dlp, drp, DIF_OP_SUB);
break;

case DT_TOK_IPOS:
dt_cg_node(dnp->dn_child, dlp, drp);
dnp->dn_reg = dnp->dn_child->dn_reg;
break;

case DT_TOK_INEG:
dt_cg_node(dnp->dn_child, dlp, drp);
dnp->dn_reg = dnp->dn_child->dn_reg;

instr = DIF_INSTR_FMT(DIF_OP_SUB, DIF_REG_R0,
dnp->dn_reg, dnp->dn_reg);

dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));
break;

case DT_TOK_DEREF:
dt_cg_node(dnp->dn_child, dlp, drp);
dnp->dn_reg = dnp->dn_child->dn_reg;

if (dt_node_is_dynamic(dnp->dn_child)) {
int reg;
idp = dt_node_resolve(dnp->dn_child, DT_IDENT_XLPTR);
assert(idp != NULL);
reg = dt_cg_xlate_expand(dnp, idp, dlp, drp);

dt_regset_free(drp, dnp->dn_child->dn_reg);
dnp->dn_reg = reg;

} else if (!(dnp->dn_flags & DT_NF_REF)) {
uint_t ubit = dnp->dn_flags & DT_NF_USERLAND;






dnp->dn_flags |=
(dnp->dn_child->dn_flags & DT_NF_USERLAND);

instr = DIF_INSTR_LOAD(dt_cg_load(dnp, ctfp,
dnp->dn_type), dnp->dn_reg, dnp->dn_reg);

dnp->dn_flags &= ~DT_NF_USERLAND;
dnp->dn_flags |= ubit;

dt_irlist_append(dlp,
dt_cg_node_alloc(DT_LBL_NONE, instr));
}
break;

case DT_TOK_ADDROF: {
uint_t rbit = dnp->dn_child->dn_flags & DT_NF_REF;

dnp->dn_child->dn_flags |= DT_NF_REF;
dt_cg_node(dnp->dn_child, dlp, drp);
dnp->dn_reg = dnp->dn_child->dn_reg;

dnp->dn_child->dn_flags &= ~DT_NF_REF;
dnp->dn_child->dn_flags |= rbit;
break;
}

case DT_TOK_SIZEOF: {
size_t size = dt_node_sizeof(dnp->dn_child);
dnp->dn_reg = dt_regset_alloc(drp);
assert(size != 0);
dt_cg_setx(dlp, dnp->dn_reg, size);
break;
}

case DT_TOK_STRINGOF:
dt_cg_node(dnp->dn_child, dlp, drp);
dnp->dn_reg = dnp->dn_child->dn_reg;
break;

case DT_TOK_XLATE:







if (dnp->dn_kind == DT_NODE_XLATOR) {
dt_xlator_t *dxp = dnp->dn_xlator;

assert(dxp->dx_ident->di_flags & DT_IDFLG_CGREG);
assert(dxp->dx_ident->di_id != 0);

dnp->dn_reg = dt_regset_alloc(drp);

if (dxp->dx_arg == -1) {
instr = DIF_INSTR_MOV(
dxp->dx_ident->di_id, dnp->dn_reg);
dt_irlist_append(dlp,
dt_cg_node_alloc(DT_LBL_NONE, instr));
op = DIF_OP_XLATE;
} else
op = DIF_OP_XLARG;

instr = DIF_INSTR_XLATE(op, 0, dnp->dn_reg);
dt_irlist_append(dlp,
dt_cg_node_alloc(DT_LBL_NONE, instr));

dlp->dl_last->di_extern = dnp->dn_xmember;
break;
}

assert(dnp->dn_kind == DT_NODE_OP2);
dt_cg_node(dnp->dn_right, dlp, drp);
dnp->dn_reg = dnp->dn_right->dn_reg;
break;

case DT_TOK_LPAR:
dt_cg_node(dnp->dn_right, dlp, drp);
dnp->dn_reg = dnp->dn_right->dn_reg;
dt_cg_typecast(dnp->dn_right, dnp, dlp, drp);
break;

case DT_TOK_PTR:
case DT_TOK_DOT:
assert(dnp->dn_right->dn_kind == DT_NODE_IDENT);
dt_cg_node(dnp->dn_left, dlp, drp);








if ((idp = dt_node_resolve(
dnp->dn_left, DT_IDENT_XLSOU)) != NULL ||
(idp = dt_node_resolve(
dnp->dn_left, DT_IDENT_XLPTR)) != NULL) {

dt_xlator_t *dxp;
dt_node_t *mnp;

dxp = idp->di_data;
mnp = dt_xlator_member(dxp, dnp->dn_right->dn_string);
assert(mnp != NULL);

dxp->dx_ident->di_flags |= DT_IDFLG_CGREG;
dxp->dx_ident->di_id = dnp->dn_left->dn_reg;

dt_cg_node(mnp->dn_membexpr, dlp, drp);
dnp->dn_reg = mnp->dn_membexpr->dn_reg;
dt_cg_typecast(mnp->dn_membexpr, dnp, dlp, drp);

dxp->dx_ident->di_flags &= ~DT_IDFLG_CGREG;
dxp->dx_ident->di_id = 0;

if (dnp->dn_left->dn_reg != -1)
dt_regset_free(drp, dnp->dn_left->dn_reg);
break;
}

ctfp = dnp->dn_left->dn_ctfp;
type = ctf_type_resolve(ctfp, dnp->dn_left->dn_type);

if (dnp->dn_op == DT_TOK_PTR) {
type = ctf_type_reference(ctfp, type);
type = ctf_type_resolve(ctfp, type);
}

if ((ctfp = dt_cg_membinfo(octfp = ctfp, type,
dnp->dn_right->dn_string, &m)) == NULL) {
yypcb->pcb_hdl->dt_ctferr = ctf_errno(octfp);
longjmp(yypcb->pcb_jmpbuf, EDT_CTF);
}

if (m.ctm_offset != 0) {
int reg;

reg = dt_regset_alloc(drp);






dt_cg_setx(dlp, reg, m.ctm_offset / NBBY);

instr = DIF_INSTR_FMT(DIF_OP_ADD,
dnp->dn_left->dn_reg, reg, dnp->dn_left->dn_reg);

dt_irlist_append(dlp,
dt_cg_node_alloc(DT_LBL_NONE, instr));
dt_regset_free(drp, reg);
}

if (!(dnp->dn_flags & DT_NF_REF)) {
uint_t ubit = dnp->dn_flags & DT_NF_USERLAND;






dnp->dn_flags |=
(dnp->dn_left->dn_flags & DT_NF_USERLAND);

instr = DIF_INSTR_LOAD(dt_cg_load(dnp,
ctfp, m.ctm_type), dnp->dn_left->dn_reg,
dnp->dn_left->dn_reg);

dnp->dn_flags &= ~DT_NF_USERLAND;
dnp->dn_flags |= ubit;

dt_irlist_append(dlp,
dt_cg_node_alloc(DT_LBL_NONE, instr));

if (dnp->dn_flags & DT_NF_BITFIELD)
dt_cg_field_get(dnp, dlp, drp, ctfp, &m);
}

dnp->dn_reg = dnp->dn_left->dn_reg;
break;

case DT_TOK_STRING:
dnp->dn_reg = dt_regset_alloc(drp);

assert(dnp->dn_kind == DT_NODE_STRING);
stroff = dt_strtab_insert(yypcb->pcb_strtab, dnp->dn_string);

if (stroff == -1L)
longjmp(yypcb->pcb_jmpbuf, EDT_NOMEM);
if (stroff > DIF_STROFF_MAX)
longjmp(yypcb->pcb_jmpbuf, EDT_STR2BIG);

instr = DIF_INSTR_SETS((ulong_t)stroff, dnp->dn_reg);
dt_irlist_append(dlp, dt_cg_node_alloc(DT_LBL_NONE, instr));
break;

case DT_TOK_IDENT:






if (dnp->dn_kind == DT_NODE_VAR &&
(dnp->dn_ident->di_flags & DT_IDFLG_CGREG)) {
dnp->dn_reg = dt_regset_alloc(drp);
instr = DIF_INSTR_MOV(dnp->dn_ident->di_id,
dnp->dn_reg);
dt_irlist_append(dlp,
dt_cg_node_alloc(DT_LBL_NONE, instr));
break;
}






if (dnp->dn_kind == DT_NODE_VAR &&
(dnp->dn_ident->di_flags & DT_IDFLG_INLINE)) {
dt_cg_inline(dnp, dlp, drp);
break;
}

switch (dnp->dn_kind) {
case DT_NODE_FUNC: {
if ((idp = dnp->dn_ident)->di_kind != DT_IDENT_FUNC) {
dnerror(dnp, D_CG_EXPR, "%s %s( ) may not be "
"called from a D expression (D program "
"context required)\n",
dt_idkind_name(idp->di_kind), idp->di_name);
}

dt_cg_arglist(dnp->dn_ident, dnp->dn_args, dlp, drp);

dnp->dn_reg = dt_regset_alloc(drp);
instr = DIF_INSTR_CALL(dnp->dn_ident->di_id,
dnp->dn_reg);

dt_irlist_append(dlp,
dt_cg_node_alloc(DT_LBL_NONE, instr));

break;
}

case DT_NODE_VAR:
if (dnp->dn_ident->di_kind == DT_IDENT_XLSOU ||
dnp->dn_ident->di_kind == DT_IDENT_XLPTR) {




assert(dnp->dn_ident->di_id == DIF_VAR_ARGS);
dt_cg_array_op(dnp, dlp, drp);
break;
}

if (dnp->dn_ident->di_kind == DT_IDENT_ARRAY) {
if (dnp->dn_ident->di_id > DIF_VAR_ARRAY_MAX)
dt_cg_assoc_op(dnp, dlp, drp);
else
dt_cg_array_op(dnp, dlp, drp);
break;
}

dnp->dn_reg = dt_regset_alloc(drp);

if (dnp->dn_ident->di_flags & DT_IDFLG_LOCAL)
op = DIF_OP_LDLS;
else if (dnp->dn_ident->di_flags & DT_IDFLG_TLS)
op = DIF_OP_LDTS;
else
op = DIF_OP_LDGS;

dnp->dn_ident->di_flags |= DT_IDFLG_DIFR;

instr = DIF_INSTR_LDV(op,
dnp->dn_ident->di_id, dnp->dn_reg);

dt_irlist_append(dlp,
dt_cg_node_alloc(DT_LBL_NONE, instr));
break;

case DT_NODE_SYM: {
dtrace_hdl_t *dtp = yypcb->pcb_hdl;
dtrace_syminfo_t *sip = dnp->dn_ident->di_data;
GElf_Sym sym;

if (dtrace_lookup_by_name(dtp,
sip->dts_object, sip->dts_name, &sym, NULL) == -1) {
xyerror(D_UNKNOWN, "cg failed for symbol %s`%s:"
" %s\n", sip->dts_object, sip->dts_name,
dtrace_errmsg(dtp, dtrace_errno(dtp)));
}

dnp->dn_reg = dt_regset_alloc(drp);
dt_cg_xsetx(dlp, dnp->dn_ident,
DT_LBL_NONE, dnp->dn_reg, sym.st_value);

if (!(dnp->dn_flags & DT_NF_REF)) {
instr = DIF_INSTR_LOAD(dt_cg_load(dnp, ctfp,
dnp->dn_type), dnp->dn_reg, dnp->dn_reg);
dt_irlist_append(dlp,
dt_cg_node_alloc(DT_LBL_NONE, instr));
}
break;
}

default:
xyerror(D_UNKNOWN, "internal error -- node type %u is "
"not valid for an identifier\n", dnp->dn_kind);
}
break;

case DT_TOK_INT:
dnp->dn_reg = dt_regset_alloc(drp);
dt_cg_setx(dlp, dnp->dn_reg, dnp->dn_value);
break;

default:
xyerror(D_UNKNOWN, "internal error -- token type %u is not a "
"valid D compilation token\n", dnp->dn_op);
}
}

void
dt_cg(dt_pcb_t *pcb, dt_node_t *dnp)
{
dif_instr_t instr;
dt_xlator_t *dxp;
dt_ident_t *idp;

if (pcb->pcb_regs == NULL && (pcb->pcb_regs =
dt_regset_create(pcb->pcb_hdl->dt_conf.dtc_difintregs)) == NULL)
longjmp(pcb->pcb_jmpbuf, EDT_NOMEM);

dt_regset_reset(pcb->pcb_regs);
(void) dt_regset_alloc(pcb->pcb_regs);

if (pcb->pcb_inttab != NULL)
dt_inttab_destroy(pcb->pcb_inttab);

if ((pcb->pcb_inttab = dt_inttab_create(yypcb->pcb_hdl)) == NULL)
longjmp(pcb->pcb_jmpbuf, EDT_NOMEM);

if (pcb->pcb_strtab != NULL)
dt_strtab_destroy(pcb->pcb_strtab);

if ((pcb->pcb_strtab = dt_strtab_create(BUFSIZ)) == NULL)
longjmp(pcb->pcb_jmpbuf, EDT_NOMEM);

dt_irlist_destroy(&pcb->pcb_ir);
dt_irlist_create(&pcb->pcb_ir);

assert(pcb->pcb_dret == NULL);
pcb->pcb_dret = dnp;

if (dt_node_resolve(dnp, DT_IDENT_XLPTR) != NULL) {
dnerror(dnp, D_CG_DYN, "expression cannot evaluate to result "
"of a translated pointer\n");
}





if (dnp->dn_kind == DT_NODE_MEMBER) {
dxp = dnp->dn_membxlator;
dnp = dnp->dn_membexpr;

dxp->dx_ident->di_flags |= DT_IDFLG_CGREG;
dxp->dx_ident->di_id = dt_regset_alloc(pcb->pcb_regs);
}

dt_cg_node(dnp, &pcb->pcb_ir, pcb->pcb_regs);

if ((idp = dt_node_resolve(dnp, DT_IDENT_XLSOU)) != NULL) {
int reg = dt_cg_xlate_expand(dnp, idp,
&pcb->pcb_ir, pcb->pcb_regs);
dt_regset_free(pcb->pcb_regs, dnp->dn_reg);
dnp->dn_reg = reg;
}

instr = DIF_INSTR_RET(dnp->dn_reg);
dt_regset_free(pcb->pcb_regs, dnp->dn_reg);
dt_irlist_append(&pcb->pcb_ir, dt_cg_node_alloc(DT_LBL_NONE, instr));

if (dnp->dn_kind == DT_NODE_MEMBER) {
dt_regset_free(pcb->pcb_regs, dxp->dx_ident->di_id);
dxp->dx_ident->di_id = 0;
dxp->dx_ident->di_flags &= ~DT_IDFLG_CGREG;
}

dt_regset_free(pcb->pcb_regs, 0);
dt_regset_assert_free(pcb->pcb_regs);
}
