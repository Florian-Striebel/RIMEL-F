





























#include <sys/types.h>
#include <strings.h>
#include <stdlib.h>
#include <assert.h>

#include <dt_impl.h>
#include <dt_parser.h>
#include <dt_as.h>

void
dt_irlist_create(dt_irlist_t *dlp)
{
bzero(dlp, sizeof (dt_irlist_t));
dlp->dl_label = 1;
}

void
dt_irlist_destroy(dt_irlist_t *dlp)
{
dt_irnode_t *dip, *nip;

for (dip = dlp->dl_list; dip != NULL; dip = nip) {
nip = dip->di_next;
free(dip);
}
}

void
dt_irlist_append(dt_irlist_t *dlp, dt_irnode_t *dip)
{
if (dlp->dl_last != NULL)
dlp->dl_last->di_next = dip;
else
dlp->dl_list = dip;

dlp->dl_last = dip;

if (dip->di_label == DT_LBL_NONE || dip->di_instr != DIF_INSTR_NOP)
dlp->dl_len++;
}

uint_t
dt_irlist_label(dt_irlist_t *dlp)
{
return (dlp->dl_label++);
}


static int
dt_countvar(dt_idhash_t *dhp, dt_ident_t *idp, void *data)
{
size_t *np = data;

if (idp->di_flags & (DT_IDFLG_DIFR | DT_IDFLG_DIFW))
(*np)++;

return (0);
}


static int
dt_copyvar(dt_idhash_t *dhp, dt_ident_t *idp, void *data)
{
dt_pcb_t *pcb = data;
dtrace_difv_t *dvp;
ssize_t stroff;
dt_node_t dn;

if (!(idp->di_flags & (DT_IDFLG_DIFR | DT_IDFLG_DIFW)))
return (0);

dvp = &pcb->pcb_difo->dtdo_vartab[pcb->pcb_asvidx++];
stroff = dt_strtab_insert(pcb->pcb_strtab, idp->di_name);

if (stroff == -1L)
longjmp(pcb->pcb_jmpbuf, EDT_NOMEM);
if (stroff > DIF_STROFF_MAX)
longjmp(pcb->pcb_jmpbuf, EDT_STR2BIG);

dvp->dtdv_name = (uint_t)stroff;
dvp->dtdv_id = idp->di_id;
dvp->dtdv_flags = 0;

dvp->dtdv_kind = (idp->di_kind == DT_IDENT_ARRAY) ?
DIFV_KIND_ARRAY : DIFV_KIND_SCALAR;

if (idp->di_flags & DT_IDFLG_LOCAL)
dvp->dtdv_scope = DIFV_SCOPE_LOCAL;
else if (idp->di_flags & DT_IDFLG_TLS)
dvp->dtdv_scope = DIFV_SCOPE_THREAD;
else
dvp->dtdv_scope = DIFV_SCOPE_GLOBAL;

if (idp->di_flags & DT_IDFLG_DIFR)
dvp->dtdv_flags |= DIFV_F_REF;
if (idp->di_flags & DT_IDFLG_DIFW)
dvp->dtdv_flags |= DIFV_F_MOD;

bzero(&dn, sizeof (dn));
dt_node_type_assign(&dn, idp->di_ctfp, idp->di_type, B_FALSE);
dt_node_diftype(pcb->pcb_hdl, &dn, &dvp->dtdv_type);

idp->di_flags &= ~(DT_IDFLG_DIFR | DT_IDFLG_DIFW);
return (0);
}

static ssize_t
dt_copystr(const char *s, size_t n, size_t off, dt_pcb_t *pcb)
{
bcopy(s, pcb->pcb_difo->dtdo_strtab + off, n);
return (n);
}








static void
dt_as_xlate(dt_pcb_t *pcb, dtrace_difo_t *dp,
uint_t i, uint_t xi, dt_node_t *dnp)
{
dtrace_hdl_t *dtp = pcb->pcb_hdl;
dt_xlator_t *dxp = dnp->dn_membexpr->dn_xlator;

assert(i < dp->dtdo_len);
assert(xi < dp->dtdo_xlmlen);

assert(dnp->dn_kind == DT_NODE_MEMBER);
assert(dnp->dn_membexpr->dn_kind == DT_NODE_XLATOR);

assert(dxp->dx_id < dtp->dt_xlatorid);
assert(dnp->dn_membid < dxp->dx_nmembers);

if (pcb->pcb_asxrefs == NULL) {
pcb->pcb_asxreflen = dtp->dt_xlatorid;
pcb->pcb_asxrefs =
dt_zalloc(dtp, sizeof (ulong_t *) * pcb->pcb_asxreflen);
if (pcb->pcb_asxrefs == NULL)
longjmp(pcb->pcb_jmpbuf, EDT_NOMEM);
}

if (pcb->pcb_asxrefs[dxp->dx_id] == NULL) {
pcb->pcb_asxrefs[dxp->dx_id] =
dt_zalloc(dtp, BT_SIZEOFMAP(dxp->dx_nmembers));
if (pcb->pcb_asxrefs[dxp->dx_id] == NULL)
longjmp(pcb->pcb_jmpbuf, EDT_NOMEM);
}

dp->dtdo_buf[i] = DIF_INSTR_XLATE(
DIF_INSTR_OP(dp->dtdo_buf[i]), xi, DIF_INSTR_RD(dp->dtdo_buf[i]));

BT_SET(pcb->pcb_asxrefs[dxp->dx_id], dnp->dn_membid);
dp->dtdo_xlmtab[xi] = dnp;
}

static void
dt_as_undef(const dt_ident_t *idp, uint_t offset)
{
const char *kind, *mark = (idp->di_flags & DT_IDFLG_USER) ? "``" : "`";
const dtrace_syminfo_t *dts = idp->di_data;

if (idp->di_flags & DT_IDFLG_USER)
kind = "user";
else if (idp->di_flags & DT_IDFLG_PRIM)
kind = "primary kernel";
else
kind = "loadable kernel";

yylineno = idp->di_lineno;

xyerror(D_ASRELO, "relocation remains against %s symbol %s%s%s (offset "
"0x%x)\n", kind, dts->dts_object, mark, dts->dts_name, offset);
}

dtrace_difo_t *
dt_as(dt_pcb_t *pcb)
{
dtrace_hdl_t *dtp = pcb->pcb_hdl;
dt_irlist_t *dlp = &pcb->pcb_ir;
uint_t *labels = NULL;
dt_irnode_t *dip;
dtrace_difo_t *dp;
dt_ident_t *idp;

size_t n = 0;
uint_t i;

uint_t kmask, kbits, umask, ubits;
uint_t krel = 0, urel = 0, xlrefs = 0;















switch (dtp->dt_linkmode) {
case DT_LINK_KERNEL:
kmask = 0;
kbits = -1u;
umask = DT_IDFLG_USER;
ubits = DT_IDFLG_USER;
break;
case DT_LINK_PRIMARY:
kmask = DT_IDFLG_USER | DT_IDFLG_PRIM;
kbits = 0;
umask = DT_IDFLG_USER;
ubits = DT_IDFLG_USER;
break;
case DT_LINK_DYNAMIC:
kmask = DT_IDFLG_USER;
kbits = 0;
umask = DT_IDFLG_USER;
ubits = DT_IDFLG_USER;
break;
case DT_LINK_STATIC:
kmask = umask = 0;
kbits = ubits = -1u;
break;
default:
xyerror(D_UNKNOWN, "internal error -- invalid link mode %u\n",
dtp->dt_linkmode);
}

assert(pcb->pcb_difo == NULL);
pcb->pcb_difo = dt_zalloc(dtp, sizeof (dtrace_difo_t));

if ((dp = pcb->pcb_difo) == NULL)
longjmp(pcb->pcb_jmpbuf, EDT_NOMEM);

dp->dtdo_buf = dt_alloc(dtp, sizeof (dif_instr_t) * dlp->dl_len);

if (dp->dtdo_buf == NULL)
longjmp(pcb->pcb_jmpbuf, EDT_NOMEM);

if ((labels = dt_alloc(dtp, sizeof (uint_t) * dlp->dl_label)) == NULL)
longjmp(pcb->pcb_jmpbuf, EDT_NOMEM);







for (i = 0, dip = dlp->dl_list; dip != NULL; dip = dip->di_next) {
if (dip->di_label != DT_LBL_NONE)
labels[dip->di_label] = i;

if (dip->di_label == DT_LBL_NONE ||
dip->di_instr != DIF_INSTR_NOP)
dp->dtdo_buf[i++] = dip->di_instr;

if (dip->di_extern == NULL)
continue;

switch (DIF_INSTR_OP(dip->di_instr)) {
case DIF_OP_SETX:
idp = dip->di_extern;
if ((idp->di_flags & kmask) == kbits)
krel++;
else if ((idp->di_flags & umask) == ubits)
urel++;
break;
case DIF_OP_XLATE:
case DIF_OP_XLARG:
xlrefs++;
break;
default:
xyerror(D_UNKNOWN, "unexpected assembler relocation "
"for opcode 0x%x\n", DIF_INSTR_OP(dip->di_instr));
}
}

assert(i == dlp->dl_len);
dp->dtdo_len = dlp->dl_len;






for (i = 0; i < dp->dtdo_len; i++) {
dif_instr_t instr = dp->dtdo_buf[i];
uint_t op = DIF_INSTR_OP(instr);

if (op == DIF_OP_CALL) {
if (DIF_INSTR_SUBR(instr) == DIF_SUBR_COPYOUT ||
DIF_INSTR_SUBR(instr) == DIF_SUBR_COPYOUTSTR)
dp->dtdo_destructive = 1;
continue;
}

if (op >= DIF_OP_BA && op <= DIF_OP_BLEU) {
assert(DIF_INSTR_LABEL(instr) < dlp->dl_label);
dp->dtdo_buf[i] = DIF_INSTR_BRANCH(op,
labels[DIF_INSTR_LABEL(instr)]);
}
}

dt_free(dtp, labels);
pcb->pcb_asvidx = 0;






(void) dt_idhash_iter(dtp->dt_tls, dt_countvar, &n);
(void) dt_idhash_iter(dtp->dt_globals, dt_countvar, &n);
(void) dt_idhash_iter(pcb->pcb_locals, dt_countvar, &n);

if (n != 0) {
dp->dtdo_vartab = dt_alloc(dtp, n * sizeof (dtrace_difv_t));
dp->dtdo_varlen = (uint32_t)n;

if (dp->dtdo_vartab == NULL)
longjmp(pcb->pcb_jmpbuf, EDT_NOMEM);

(void) dt_idhash_iter(dtp->dt_tls, dt_copyvar, pcb);
(void) dt_idhash_iter(dtp->dt_globals, dt_copyvar, pcb);
(void) dt_idhash_iter(pcb->pcb_locals, dt_copyvar, pcb);
}





if (krel != 0) {
dp->dtdo_kreltab = dt_alloc(dtp,
krel * sizeof (dof_relodesc_t));
dp->dtdo_krelen = krel;

if (dp->dtdo_kreltab == NULL)
longjmp(pcb->pcb_jmpbuf, EDT_NOMEM);
}

if (urel != 0) {
dp->dtdo_ureltab = dt_alloc(dtp,
urel * sizeof (dof_relodesc_t));
dp->dtdo_urelen = urel;

if (dp->dtdo_ureltab == NULL)
longjmp(pcb->pcb_jmpbuf, EDT_NOMEM);
}

if (xlrefs != 0) {
dp->dtdo_xlmtab = dt_zalloc(dtp, sizeof (dt_node_t *) * xlrefs);
dp->dtdo_xlmlen = xlrefs;

if (dp->dtdo_xlmtab == NULL)
longjmp(pcb->pcb_jmpbuf, EDT_NOMEM);
}





if (krel + urel + xlrefs != 0) {
uint_t knodef = pcb->pcb_cflags & DTRACE_C_KNODEF;
uint_t unodef = pcb->pcb_cflags & DTRACE_C_UNODEF;

dof_relodesc_t *krp = dp->dtdo_kreltab;
dof_relodesc_t *urp = dp->dtdo_ureltab;
dt_node_t **xlp = dp->dtdo_xlmtab;

i = 0;

for (dip = dlp->dl_list; dip != NULL; dip = dip->di_next) {
dof_relodesc_t *rp;
ssize_t soff;
uint_t nodef;

if (dip->di_label != DT_LBL_NONE &&
dip->di_instr == DIF_INSTR_NOP)
continue;

i++;

if (DIF_INSTR_OP(dip->di_instr) == DIF_OP_XLATE ||
DIF_INSTR_OP(dip->di_instr) == DIF_OP_XLARG) {
assert(dp->dtdo_buf[i - 1] == dip->di_instr);
dt_as_xlate(pcb, dp, i - 1, (uint_t)
(xlp++ - dp->dtdo_xlmtab), dip->di_extern);
continue;
}

if ((idp = dip->di_extern) == NULL)
continue;

if ((idp->di_flags & kmask) == kbits) {
nodef = knodef;
rp = krp++;
} else if ((idp->di_flags & umask) == ubits) {
nodef = unodef;
rp = urp++;
} else
continue;

if (!nodef)
dt_as_undef(idp, i);

assert(DIF_INSTR_OP(dip->di_instr) == DIF_OP_SETX);
soff = dt_strtab_insert(pcb->pcb_strtab, idp->di_name);

if (soff == -1L)
longjmp(pcb->pcb_jmpbuf, EDT_NOMEM);
if (soff > DIF_STROFF_MAX)
longjmp(pcb->pcb_jmpbuf, EDT_STR2BIG);

rp->dofr_name = (dof_stridx_t)soff;
rp->dofr_type = DOF_RELO_SETX;
rp->dofr_offset = DIF_INSTR_INTEGER(dip->di_instr) *
sizeof (uint64_t);
rp->dofr_data = 0;
}

assert(krp == dp->dtdo_kreltab + dp->dtdo_krelen);
assert(urp == dp->dtdo_ureltab + dp->dtdo_urelen);
assert(xlp == dp->dtdo_xlmtab + dp->dtdo_xlmlen);
assert(i == dp->dtdo_len);
}





if ((n = dt_strtab_size(pcb->pcb_strtab)) != 0) {
if ((dp->dtdo_strtab = dt_alloc(dtp, n)) == NULL)
longjmp(pcb->pcb_jmpbuf, EDT_NOMEM);

(void) dt_strtab_write(pcb->pcb_strtab,
(dt_strtab_write_f *)dt_copystr, pcb);
dp->dtdo_strlen = (uint32_t)n;
}





if ((n = dt_inttab_size(pcb->pcb_inttab)) != 0) {
if ((dp->dtdo_inttab = dt_alloc(dtp,
n * sizeof (uint64_t))) == NULL)
longjmp(pcb->pcb_jmpbuf, EDT_NOMEM);

dt_inttab_write(pcb->pcb_inttab, dp->dtdo_inttab);
dp->dtdo_intlen = (uint32_t)n;
}






dt_node_diftype(dtp, pcb->pcb_dret, &dp->dtdo_rtype);
pcb->pcb_difo = NULL;
pcb->pcb_dret = NULL;

if (pcb->pcb_cflags & DTRACE_C_DIFV)
dt_dis(dp, stderr);

return (dp);
}
