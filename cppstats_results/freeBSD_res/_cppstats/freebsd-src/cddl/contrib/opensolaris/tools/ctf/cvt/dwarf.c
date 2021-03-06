#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <libelf.h>
#include <libdwarf.h>
#include <libgen.h>
#include <dwarf.h>
#include "ctf_headers.h"
#include "ctftools.h"
#include "memory.h"
#include "list.h"
#include "traverse.h"
#define TID_FILEMAX 0x3fffffff
#define TID_VOID 0x40000001
#define TID_LONG 0x40000002
#define TID_MFGTID_BASE 0x40000003
#define DW_ATTR_REQ 1
#define TDESC_HASH_BUCKETS 511
typedef struct dwarf {
Dwarf_Debug dw_dw;
Dwarf_Error dw_err;
Dwarf_Off dw_maxoff;
tdata_t *dw_td;
hash_t *dw_tidhash;
hash_t *dw_fwdhash;
hash_t *dw_enumhash;
tdesc_t *dw_void;
tdesc_t *dw_long;
size_t dw_ptrsz;
tid_t dw_mfgtid_last;
uint_t dw_nunres;
char *dw_cuname;
} dwarf_t;
static void die_create_one(dwarf_t *, Dwarf_Die);
static void die_create(dwarf_t *, Dwarf_Die);
static tid_t
mfgtid_next(dwarf_t *dw)
{
return (++dw->dw_mfgtid_last);
}
static void
tdesc_add(dwarf_t *dw, tdesc_t *tdp)
{
hash_add(dw->dw_tidhash, tdp);
}
static tdesc_t *
tdesc_lookup(dwarf_t *dw, int tid)
{
tdesc_t tmpl;
void *tdp;
tmpl.t_id = tid;
if (hash_find(dw->dw_tidhash, &tmpl, &tdp))
return (tdp);
else
return (NULL);
}
static size_t
tdesc_size(tdesc_t *tdp)
{
for (;;) {
switch (tdp->t_type) {
case INTRINSIC:
case POINTER:
case ARRAY:
case FUNCTION:
case STRUCT:
case UNION:
case ENUM:
return (tdp->t_size);
case FORWARD:
return (0);
case TYPEDEF:
case VOLATILE:
case CONST:
case RESTRICT:
tdp = tdp->t_tdesc;
continue;
case 0:
return (0);
default:
terminate("tdp %u: tdesc_size on unknown type %d\n",
tdp->t_id, tdp->t_type);
}
}
}
static size_t
tdesc_bitsize(tdesc_t *tdp)
{
for (;;) {
switch (tdp->t_type) {
case INTRINSIC:
return (tdp->t_intr->intr_nbits);
case ARRAY:
case FUNCTION:
case STRUCT:
case UNION:
case ENUM:
case POINTER:
return (tdp->t_size * NBBY);
case FORWARD:
return (0);
case TYPEDEF:
case VOLATILE:
case RESTRICT:
case CONST:
tdp = tdp->t_tdesc;
continue;
case 0:
return (0);
default:
terminate("tdp %u: tdesc_bitsize on unknown type %d\n",
tdp->t_id, tdp->t_type);
}
}
}
static tdesc_t *
tdesc_basetype(tdesc_t *tdp)
{
for (;;) {
switch (tdp->t_type) {
case TYPEDEF:
case VOLATILE:
case RESTRICT:
case CONST:
tdp = tdp->t_tdesc;
break;
case 0:
return (NULL);
default:
return (tdp);
}
}
}
static Dwarf_Off
die_off(dwarf_t *dw, Dwarf_Die die)
{
Dwarf_Off off;
if (dwarf_dieoffset(die, &off, &dw->dw_err) == DW_DLV_OK)
return (off);
terminate("failed to get offset for die: %s\n",
dwarf_errmsg(dw->dw_err));
return (0);
}
static Dwarf_Die
die_sibling(dwarf_t *dw, Dwarf_Die die)
{
Dwarf_Die sib;
int rc;
if ((rc = dwarf_siblingof(dw->dw_dw, die, &sib, &dw->dw_err)) ==
DW_DLV_OK)
return (sib);
else if (rc == DW_DLV_NO_ENTRY)
return (NULL);
terminate("die %llu: failed to find type sibling: %s\n",
die_off(dw, die), dwarf_errmsg(dw->dw_err));
return (NULL);
}
static Dwarf_Die
die_child(dwarf_t *dw, Dwarf_Die die)
{
Dwarf_Die child;
int rc;
if ((rc = dwarf_child(die, &child, &dw->dw_err)) == DW_DLV_OK)
return (child);
else if (rc == DW_DLV_NO_ENTRY)
return (NULL);
terminate("die %llu: failed to find type child: %s\n",
die_off(dw, die), dwarf_errmsg(dw->dw_err));
return (NULL);
}
static Dwarf_Half
die_tag(dwarf_t *dw, Dwarf_Die die)
{
Dwarf_Half tag;
if (dwarf_tag(die, &tag, &dw->dw_err) == DW_DLV_OK)
return (tag);
terminate("die %llu: failed to get tag for type: %s\n",
die_off(dw, die), dwarf_errmsg(dw->dw_err));
return (0);
}
static Dwarf_Attribute
die_attr(dwarf_t *dw, Dwarf_Die die, Dwarf_Half name, int req)
{
Dwarf_Attribute attr;
int rc;
if ((rc = dwarf_attr(die, name, &attr, &dw->dw_err)) == DW_DLV_OK) {
return (attr);
} else if (rc == DW_DLV_NO_ENTRY) {
if (req) {
terminate("die %llu: no attr 0x%x\n", die_off(dw, die),
name);
} else {
return (NULL);
}
}
terminate("die %llu: failed to get attribute for type: %s\n",
die_off(dw, die), dwarf_errmsg(dw->dw_err));
return (NULL);
}
static int
die_signed(dwarf_t *dw, Dwarf_Die die, Dwarf_Half name, Dwarf_Signed *valp,
int req)
{
*valp = 0;
if (dwarf_attrval_signed(die, name, valp, &dw->dw_err) != DW_DLV_OK) {
if (req)
terminate("die %llu: failed to get signed: %s\n",
die_off(dw, die), dwarf_errmsg(dw->dw_err));
return (0);
}
return (1);
}
static int
die_unsigned(dwarf_t *dw, Dwarf_Die die, Dwarf_Half name, Dwarf_Unsigned *valp,
int req)
{
*valp = 0;
if (dwarf_attrval_unsigned(die, name, valp, &dw->dw_err) != DW_DLV_OK) {
if (req)
terminate("die %llu: failed to get unsigned: %s\n",
die_off(dw, die), dwarf_errmsg(dw->dw_err));
return (0);
}
return (1);
}
static int
die_bool(dwarf_t *dw, Dwarf_Die die, Dwarf_Half name, Dwarf_Bool *valp, int req)
{
*valp = 0;
if (dwarf_attrval_flag(die, name, valp, &dw->dw_err) != DW_DLV_OK) {
if (req)
terminate("die %llu: failed to get flag: %s\n",
die_off(dw, die), dwarf_errmsg(dw->dw_err));
return (0);
}
return (1);
}
static int
die_string(dwarf_t *dw, Dwarf_Die die, Dwarf_Half name, char **strp, int req)
{
const char *str = NULL;
if (dwarf_attrval_string(die, name, &str, &dw->dw_err) != DW_DLV_OK ||
str == NULL) {
if (req)
terminate("die %llu: failed to get string: %s\n",
die_off(dw, die), dwarf_errmsg(dw->dw_err));
else
*strp = NULL;
return (0);
} else
*strp = xstrdup(str);
return (1);
}
static Dwarf_Off
die_attr_ref(dwarf_t *dw, Dwarf_Die die, Dwarf_Half name)
{
Dwarf_Off off;
if (dwarf_attrval_unsigned(die, name, &off, &dw->dw_err) != DW_DLV_OK) {
terminate("die %llu: failed to get ref: %s\n",
die_off(dw, die), dwarf_errmsg(dw->dw_err));
}
return (off);
}
static char *
die_name(dwarf_t *dw, Dwarf_Die die)
{
char *str = NULL;
(void) die_string(dw, die, DW_AT_name, &str, 0);
if (str == NULL)
str = xstrdup("");
return (str);
}
static int
die_isdecl(dwarf_t *dw, Dwarf_Die die)
{
Dwarf_Bool val;
return (die_bool(dw, die, DW_AT_declaration, &val, 0) && val);
}
static int
die_isglobal(dwarf_t *dw, Dwarf_Die die)
{
Dwarf_Signed vis;
Dwarf_Bool ext;
if (die_signed(dw, die, DW_AT_visibility, &vis, 0))
return (vis == DW_VIS_exported);
else
return (die_bool(dw, die, DW_AT_external, &ext, 0) && ext);
}
static tdesc_t *
die_add(dwarf_t *dw, Dwarf_Off off)
{
tdesc_t *tdp = xcalloc(sizeof (tdesc_t));
tdp->t_id = off;
tdesc_add(dw, tdp);
return (tdp);
}
static tdesc_t *
die_lookup_pass1(dwarf_t *dw, Dwarf_Die die, Dwarf_Half name)
{
Dwarf_Off ref = die_attr_ref(dw, die, name);
tdesc_t *tdp;
if ((tdp = tdesc_lookup(dw, ref)) != NULL)
return (tdp);
return (die_add(dw, ref));
}
static int
die_mem_offset(dwarf_t *dw, Dwarf_Die die, Dwarf_Half name,
Dwarf_Unsigned *valp, int req __unused)
{
Dwarf_Locdesc *loc = NULL;
Dwarf_Signed locnum = 0;
Dwarf_Attribute at;
Dwarf_Half form;
if (name != DW_AT_data_member_location)
terminate("die %llu: can only process attribute "
"DW_AT_data_member_location\n", die_off(dw, die));
if ((at = die_attr(dw, die, name, 0)) == NULL)
return (0);
if (dwarf_whatform(at, &form, &dw->dw_err) != DW_DLV_OK)
return (0);
switch (form) {
case DW_FORM_sec_offset:
case DW_FORM_block:
case DW_FORM_block1:
case DW_FORM_block2:
case DW_FORM_block4:
if (dwarf_loclist(at, &loc, &locnum, &dw->dw_err) != DW_DLV_OK)
return (0);
if (locnum != 1 || loc->ld_s->lr_atom != DW_OP_plus_uconst) {
terminate("die %llu: cannot parse member offset with "
"operator other than DW_OP_plus_uconst\n",
die_off(dw, die));
}
*valp = loc->ld_s->lr_number;
if (loc != NULL) {
dwarf_dealloc(dw->dw_dw, loc->ld_s, DW_DLA_LOC_BLOCK);
dwarf_dealloc(dw->dw_dw, loc, DW_DLA_LOCDESC);
}
break;
case DW_FORM_data1:
case DW_FORM_data2:
case DW_FORM_data4:
case DW_FORM_data8:
case DW_FORM_udata:
if (dwarf_attrval_unsigned(die, name, valp, &dw->dw_err) !=
DW_DLV_OK)
return (0);
break;
default:
terminate("die %llu: cannot parse member offset with form "
"%u\n", die_off(dw, die), form);
}
return (1);
}
static tdesc_t *
tdesc_intr_common(dwarf_t *dw, int tid, const char *name, size_t sz)
{
tdesc_t *tdp;
intr_t *intr;
intr = xcalloc(sizeof (intr_t));
intr->intr_type = INTR_INT;
intr->intr_signed = 1;
intr->intr_nbits = sz * NBBY;
tdp = xcalloc(sizeof (tdesc_t));
tdp->t_name = xstrdup(name);
tdp->t_size = sz;
tdp->t_id = tid;
tdp->t_type = INTRINSIC;
tdp->t_intr = intr;
tdp->t_flags = TDESC_F_RESOLVED;
tdesc_add(dw, tdp);
return (tdp);
}
static tdesc_t *
tdesc_intr_void(dwarf_t *dw)
{
if (dw->dw_void == NULL)
dw->dw_void = tdesc_intr_common(dw, TID_VOID, "void", 0);
return (dw->dw_void);
}
static tdesc_t *
tdesc_intr_long(dwarf_t *dw)
{
if (dw->dw_long == NULL) {
dw->dw_long = tdesc_intr_common(dw, TID_LONG, "long",
dw->dw_ptrsz);
}
return (dw->dw_long);
}
static tdesc_t *
tdesc_intr_clone(dwarf_t *dw, tdesc_t *old, size_t bitsz)
{
tdesc_t *new = xcalloc(sizeof (tdesc_t));
if (!(old->t_flags & TDESC_F_RESOLVED)) {
terminate("tdp %u: attempt to make a bit field from an "
"unresolved type\n", old->t_id);
}
new->t_name = xstrdup(old->t_name);
new->t_size = old->t_size;
new->t_id = mfgtid_next(dw);
new->t_type = INTRINSIC;
new->t_flags = TDESC_F_RESOLVED;
new->t_intr = xcalloc(sizeof (intr_t));
bcopy(old->t_intr, new->t_intr, sizeof (intr_t));
new->t_intr->intr_nbits = bitsz;
tdesc_add(dw, new);
return (new);
}
static void
tdesc_array_create(dwarf_t *dw, Dwarf_Die dim, tdesc_t *arrtdp,
tdesc_t *dimtdp)
{
Dwarf_Unsigned uval;
Dwarf_Signed sval;
tdesc_t *ctdp = NULL;
Dwarf_Die dim2;
ardef_t *ar;
if ((dim2 = die_sibling(dw, dim)) == NULL) {
ctdp = arrtdp;
} else if (die_tag(dw, dim2) == DW_TAG_subrange_type) {
ctdp = xcalloc(sizeof (tdesc_t));
ctdp->t_id = mfgtid_next(dw);
debug(3, "die %llu: creating new type %u for sub-dimension\n",
die_off(dw, dim2), ctdp->t_id);
tdesc_array_create(dw, dim2, arrtdp, ctdp);
} else {
terminate("die %llu: unexpected non-subrange node in array\n",
die_off(dw, dim2));
}
dimtdp->t_type = ARRAY;
dimtdp->t_ardef = ar = xcalloc(sizeof (ardef_t));
if (die_unsigned(dw, dim, DW_AT_upper_bound, &uval, 0))
ar->ad_nelems = uval + 1;
else if (die_signed(dw, dim, DW_AT_upper_bound, &sval, 0))
ar->ad_nelems = sval + 1;
else if (die_unsigned(dw, dim, DW_AT_count, &uval, 0))
ar->ad_nelems = uval;
else if (die_signed(dw, dim, DW_AT_count, &sval, 0))
ar->ad_nelems = sval;
else
ar->ad_nelems = 0;
ar->ad_idxtype = tdesc_intr_long(dw);
ar->ad_contents = ctdp;
if (ar->ad_contents->t_size != 0) {
dimtdp->t_size = ar->ad_contents->t_size * ar->ad_nelems;
dimtdp->t_flags |= TDESC_F_RESOLVED;
}
}
static void
die_array_create(dwarf_t *dw, Dwarf_Die arr, Dwarf_Off off, tdesc_t *tdp)
{
tdesc_t *arrtdp = die_lookup_pass1(dw, arr, DW_AT_type);
Dwarf_Unsigned uval;
Dwarf_Die dim;
debug(3, "die %llu <%llx>: creating array\n", off, off);
if ((dim = die_child(dw, arr)) == NULL ||
die_tag(dw, dim) != DW_TAG_subrange_type)
terminate("die %llu: failed to retrieve array bounds\n", off);
tdesc_array_create(dw, dim, arrtdp, tdp);
if (die_unsigned(dw, arr, DW_AT_byte_size, &uval, 0)) {
tdesc_t *dimtdp;
int flags;
tdp->t_size = uval;
flags = TDESC_F_RESOLVED;
for (dimtdp = tdp->t_ardef->ad_contents;
dimtdp->t_type == ARRAY;
dimtdp = dimtdp->t_ardef->ad_contents) {
if (!(dimtdp->t_flags & TDESC_F_RESOLVED)) {
flags = 0;
break;
}
}
tdp->t_flags |= flags;
}
debug(3, "die %llu <%llx>: array nelems %u size %u\n", off, off,
tdp->t_ardef->ad_nelems, tdp->t_size);
}
static int
die_array_resolve(tdesc_t *tdp, tdesc_t **tdpp __unused, void *private)
{
dwarf_t *dw = private;
size_t sz;
if (tdp->t_flags & TDESC_F_RESOLVED)
return (1);
debug(3, "trying to resolve array %d (cont %d)\n", tdp->t_id,
tdp->t_ardef->ad_contents->t_id);
if ((sz = tdesc_size(tdp->t_ardef->ad_contents)) == 0 &&
(tdp->t_ardef->ad_contents->t_flags & TDESC_F_RESOLVED) == 0) {
debug(3, "unable to resolve array %s (%d) contents %d\n",
tdesc_name(tdp), tdp->t_id,
tdp->t_ardef->ad_contents->t_id);
dw->dw_nunres++;
return (1);
}
tdp->t_size = sz * tdp->t_ardef->ad_nelems;
tdp->t_flags |= TDESC_F_RESOLVED;
debug(3, "resolved array %d: %u bytes\n", tdp->t_id, tdp->t_size);
return (1);
}
static int
die_array_failed(tdesc_t *tdp, tdesc_t **tdpp __unused, void *private __unused)
{
tdesc_t *cont = tdp->t_ardef->ad_contents;
if (tdp->t_flags & TDESC_F_RESOLVED)
return (1);
fprintf(stderr, "Array %d: failed to size contents type %s (%d)\n",
tdp->t_id, tdesc_name(cont), cont->t_id);
return (1);
}
static void
die_enum_create(dwarf_t *dw, Dwarf_Die die, Dwarf_Off off, tdesc_t *tdp)
{
Dwarf_Die mem;
Dwarf_Unsigned uval;
Dwarf_Signed sval;
if (die_isdecl(dw, die)) {
tdp->t_type = FORWARD;
return;
}
debug(3, "die %llu: creating enum\n", off);
tdp->t_type = ENUM;
(void) die_unsigned(dw, die, DW_AT_byte_size, &uval, DW_ATTR_REQ);
tdp->t_size = uval;
if ((mem = die_child(dw, die)) != NULL) {
elist_t **elastp = &tdp->t_emem;
do {
elist_t *el;
if (die_tag(dw, mem) != DW_TAG_enumerator) {
die_create_one(dw, mem);
continue;
}
el = xcalloc(sizeof (elist_t));
el->el_name = die_name(dw, mem);
if (die_signed(dw, mem, DW_AT_const_value, &sval, 0)) {
el->el_number = sval;
} else if (die_unsigned(dw, mem, DW_AT_const_value,
&uval, 0)) {
el->el_number = uval;
} else {
terminate("die %llu: enum %llu: member without "
"value\n", off, die_off(dw, mem));
}
debug(3, "die %llu: enum %llu: created %s = %d\n", off,
die_off(dw, mem), el->el_name, el->el_number);
*elastp = el;
elastp = &el->el_next;
} while ((mem = die_sibling(dw, mem)) != NULL);
hash_add(dw->dw_enumhash, tdp);
tdp->t_flags |= TDESC_F_RESOLVED;
if (tdp->t_name != NULL) {
iidesc_t *ii = xcalloc(sizeof (iidesc_t));
ii->ii_type = II_SOU;
ii->ii_name = xstrdup(tdp->t_name);
ii->ii_dtype = tdp;
iidesc_add(dw->dw_td->td_iihash, ii);
}
}
}
static int
die_enum_match(void *arg1, void *arg2)
{
tdesc_t *tdp = arg1, **fullp = arg2;
if (tdp->t_emem != NULL) {
*fullp = tdp;
return (-1);
}
return (0);
}
static int
die_enum_resolve(tdesc_t *tdp, tdesc_t **tdpp __unused, void *private)
{
dwarf_t *dw = private;
tdesc_t *full = NULL;
if (tdp->t_flags & TDESC_F_RESOLVED)
return (1);
(void) hash_find_iter(dw->dw_enumhash, tdp, die_enum_match, &full);
if (full == NULL) {
terminate("tdp %u: enum %s has no members\n", tdp->t_id,
tdesc_name(tdp));
}
debug(3, "tdp %u: enum %s redirected to %u\n", tdp->t_id,
tdesc_name(tdp), full->t_id);
tdp->t_flags |= TDESC_F_RESOLVED;
return (1);
}
static int
die_fwd_map(void *arg1, void *arg2)
{
tdesc_t *fwd = arg1, *sou = arg2;
debug(3, "tdp %u: mapped forward %s to sou %u\n", fwd->t_id,
tdesc_name(fwd), sou->t_id);
fwd->t_tdesc = sou;
return (0);
}
static void
die_sou_create(dwarf_t *dw, Dwarf_Die str, Dwarf_Off off, tdesc_t *tdp,
int type, const char *typename)
{
Dwarf_Unsigned sz, bitsz, bitoff;
#if BYTE_ORDER == _LITTLE_ENDIAN
Dwarf_Unsigned bysz;
#endif
Dwarf_Die mem;
mlist_t *ml, **mlastp;
iidesc_t *ii;
tdp->t_type = (die_isdecl(dw, str) ? FORWARD : type);
debug(3, "die %llu: creating %s %s\n", off,
(tdp->t_type == FORWARD ? "forward decl" : typename),
tdesc_name(tdp));
if (tdp->t_type == FORWARD) {
hash_add(dw->dw_fwdhash, tdp);
return;
}
(void) hash_find_iter(dw->dw_fwdhash, tdp, die_fwd_map, tdp);
(void) die_unsigned(dw, str, DW_AT_byte_size, &sz, DW_ATTR_REQ);
tdp->t_size = sz;
if ((mem = die_child(dw, str)) == NULL) {
goto out;
}
mlastp = &tdp->t_members;
do {
Dwarf_Off memoff = die_off(dw, mem);
Dwarf_Half tag = die_tag(dw, mem);
Dwarf_Unsigned mloff;
if (tag != DW_TAG_member) {
die_create_one(dw, mem);
continue;
}
debug(3, "die %llu: mem %llu: creating member\n", off, memoff);
ml = xcalloc(sizeof (mlist_t));
if ((ml->ml_name = die_name(dw, mem)) == NULL)
ml->ml_name = NULL;
ml->ml_type = die_lookup_pass1(dw, mem, DW_AT_type);
if (die_mem_offset(dw, mem, DW_AT_data_member_location,
&mloff, 0)) {
debug(3, "die %llu: got mloff %llx\n", off,
(u_longlong_t)mloff);
ml->ml_offset = mloff * 8;
}
if (die_unsigned(dw, mem, DW_AT_bit_size, &bitsz, 0))
ml->ml_size = bitsz;
else
ml->ml_size = tdesc_bitsize(ml->ml_type);
if (die_unsigned(dw, mem, DW_AT_bit_offset, &bitoff, 0)) {
#if BYTE_ORDER == _BIG_ENDIAN
ml->ml_offset += bitoff;
#else
if (die_unsigned(dw, mem, DW_AT_byte_size, &bysz, 0) &&
bysz > 0)
ml->ml_offset += bysz * NBBY - bitoff -
ml->ml_size;
else
ml->ml_offset += tdesc_bitsize(ml->ml_type) -
bitoff - ml->ml_size;
#endif
}
debug(3, "die %llu: mem %llu: created \"%s\" (off %u sz %u)\n",
off, memoff, ml->ml_name, ml->ml_offset, ml->ml_size);
*mlastp = ml;
mlastp = &ml->ml_next;
} while ((mem = die_sibling(dw, mem)) != NULL);
if (tdp->t_members == NULL) {
const char *old = tdesc_name(tdp);
size_t newsz = 7 + strlen(old) + 1;
char *new = xmalloc(newsz);
(void) snprintf(new, newsz, "orphan %s", old);
debug(3, "die %llu: worked around %s %s\n", off, typename, old);
if (tdp->t_name != NULL)
free(tdp->t_name);
tdp->t_name = new;
return;
}
out:
if (tdp->t_name != NULL) {
ii = xcalloc(sizeof (iidesc_t));
ii->ii_type = II_SOU;
ii->ii_name = xstrdup(tdp->t_name);
ii->ii_dtype = tdp;
iidesc_add(dw->dw_td->td_iihash, ii);
}
}
static void
die_struct_create(dwarf_t *dw, Dwarf_Die die, Dwarf_Off off, tdesc_t *tdp)
{
die_sou_create(dw, die, off, tdp, STRUCT, "struct");
}
static void
die_union_create(dwarf_t *dw, Dwarf_Die die, Dwarf_Off off, tdesc_t *tdp)
{
die_sou_create(dw, die, off, tdp, UNION, "union");
}
static int
die_sou_resolve(tdesc_t *tdp, tdesc_t **tdpp __unused, void *private)
{
dwarf_t *dw = private;
mlist_t *ml;
tdesc_t *mt;
if (tdp->t_flags & TDESC_F_RESOLVED)
return (1);
debug(3, "resolving sou %s\n", tdesc_name(tdp));
for (ml = tdp->t_members; ml != NULL; ml = ml->ml_next) {
if (ml->ml_size == 0) {
mt = tdesc_basetype(ml->ml_type);
if ((ml->ml_size = tdesc_bitsize(mt)) != 0)
continue;
if (mt->t_members == NULL)
continue;
if (mt->t_type == ARRAY) {
if (mt->t_ardef->ad_nelems == 0)
continue;
mt = tdesc_basetype(mt->t_ardef->ad_contents);
if ((mt->t_flags & TDESC_F_RESOLVED) != 0 &&
(mt->t_type == STRUCT ||
mt->t_type == UNION) &&
mt->t_members == NULL)
continue;
}
if ((mt->t_flags & TDESC_F_RESOLVED) != 0 &&
(mt->t_type == STRUCT || mt->t_type == UNION))
continue;
dw->dw_nunres++;
return (1);
}
if ((mt = tdesc_basetype(ml->ml_type)) == NULL) {
dw->dw_nunres++;
return (1);
}
if (ml->ml_size != 0 && mt->t_type == INTRINSIC &&
mt->t_intr->intr_nbits != ml->ml_size) {
debug(3, "tdp %u: creating bitfield for %d bits\n",
tdp->t_id, ml->ml_size);
ml->ml_type = tdesc_intr_clone(dw, mt, ml->ml_size);
}
}
tdp->t_flags |= TDESC_F_RESOLVED;
return (1);
}
static int
die_sou_failed(tdesc_t *tdp, tdesc_t **tdpp __unused, void *private __unused)
{
const char *typename = (tdp->t_type == STRUCT ? "struct" : "union");
mlist_t *ml;
if (tdp->t_flags & TDESC_F_RESOLVED)
return (1);
for (ml = tdp->t_members; ml != NULL; ml = ml->ml_next) {
if (ml->ml_size == 0) {
fprintf(stderr, "%s %d <%x>: failed to size member \"%s\" "
"of type %s (%d <%x>)\n", typename, tdp->t_id,
tdp->t_id,
ml->ml_name, tdesc_name(ml->ml_type),
ml->ml_type->t_id, ml->ml_type->t_id);
}
}
return (1);
}
static void
die_funcptr_create(dwarf_t *dw, Dwarf_Die die, Dwarf_Off off, tdesc_t *tdp)
{
Dwarf_Attribute attr;
Dwarf_Half tag;
Dwarf_Die arg;
fndef_t *fn;
int i;
debug(3, "die %llu <%llx>: creating function pointer\n", off, off);
for (arg = die_child(dw, die); arg != NULL;
arg = die_sibling(dw, arg)) {
if ((tag = die_tag(dw, arg)) != DW_TAG_formal_parameter &&
tag != DW_TAG_unspecified_parameters) {
die_create_one(dw, arg);
}
}
if (die_isdecl(dw, die)) {
return;
}
fn = xcalloc(sizeof (fndef_t));
tdp->t_type = FUNCTION;
if ((attr = die_attr(dw, die, DW_AT_type, 0)) != NULL) {
fn->fn_ret = die_lookup_pass1(dw, die, DW_AT_type);
} else {
fn->fn_ret = tdesc_intr_void(dw);
}
for (fn->fn_nargs = 0, arg = die_child(dw, die); arg != NULL;
arg = die_sibling(dw, arg)) {
if ((tag = die_tag(dw, arg)) == DW_TAG_formal_parameter)
fn->fn_nargs++;
else if (tag == DW_TAG_unspecified_parameters &&
fn->fn_nargs > 0)
fn->fn_vargs = 1;
}
if (fn->fn_nargs != 0) {
debug(3, "die %llu: adding %d argument%s\n", off, fn->fn_nargs,
(fn->fn_nargs > 1 ? "s" : ""));
fn->fn_args = xcalloc(sizeof (tdesc_t *) * fn->fn_nargs);
for (i = 0, arg = die_child(dw, die);
arg != NULL && i < (int) fn->fn_nargs;
arg = die_sibling(dw, arg)) {
if (die_tag(dw, arg) != DW_TAG_formal_parameter)
continue;
fn->fn_args[i++] = die_lookup_pass1(dw, arg,
DW_AT_type);
}
}
tdp->t_fndef = fn;
tdp->t_flags |= TDESC_F_RESOLVED;
}
static intr_t *
die_base_name_parse(const char *name, char **newp)
{
char buf[256];
char const *base;
char *c;
int nlong = 0, nshort = 0, nchar = 0, nint = 0;
int sign = 1;
char fmt = '\0';
intr_t *intr;
if (strlen(name) > sizeof (buf) - 1)
terminate("base type name \"%s\" is too long\n", name);
strncpy(buf, name, sizeof (buf));
for (c = strtok(buf, " "); c != NULL; c = strtok(NULL, " ")) {
if (strcmp(c, "signed") == 0)
sign = 1;
else if (strcmp(c, "unsigned") == 0)
sign = 0;
else if (strcmp(c, "long") == 0)
nlong++;
else if (strcmp(c, "char") == 0) {
nchar++;
fmt = 'c';
} else if (strcmp(c, "short") == 0)
nshort++;
else if (strcmp(c, "int") == 0)
nint++;
else {
return (NULL);
}
}
if (nchar > 1 || nshort > 1 || nint > 1 || nlong > 2)
return (NULL);
if (nchar > 0) {
if (nlong > 0 || nshort > 0 || nint > 0)
return (NULL);
base = "char";
} else if (nshort > 0) {
if (nlong > 0)
return (NULL);
base = "short";
} else if (nlong > 0) {
base = "long";
} else {
base = "int";
}
intr = xcalloc(sizeof (intr_t));
intr->intr_type = INTR_INT;
intr->intr_signed = sign;
intr->intr_iformat = fmt;
snprintf(buf, sizeof (buf), "%s%s%s",
(sign ? "" : "unsigned "),
(nlong > 1 ? "long " : ""),
base);
*newp = xstrdup(buf);
return (intr);
}
typedef struct fp_size_map {
size_t fsm_typesz[2];
uint_t fsm_enc[3];
} fp_size_map_t;
static const fp_size_map_t fp_encodings[] = {
{ { 4, 4 }, { CTF_FP_SINGLE, CTF_FP_CPLX, CTF_FP_IMAGRY } },
{ { 8, 8 }, { CTF_FP_DOUBLE, CTF_FP_DCPLX, CTF_FP_DIMAGRY } },
#if defined(__sparc)
{ { 16, 16 }, { CTF_FP_LDOUBLE, CTF_FP_LDCPLX, CTF_FP_LDIMAGRY } },
#else
{ { 12, 16 }, { CTF_FP_LDOUBLE, CTF_FP_LDCPLX, CTF_FP_LDIMAGRY } },
#endif
{ { 0, 0 }, { 0, 0, 0 } }
};
static uint_t
die_base_type2enc(dwarf_t *dw, Dwarf_Off off, Dwarf_Signed enc, size_t sz)
{
const fp_size_map_t *map = fp_encodings;
uint_t szidx = dw->dw_ptrsz == sizeof (uint64_t);
uint_t mult = 1, col = 0;
if (enc == DW_ATE_complex_float) {
mult = 2;
col = 1;
} else if (enc == DW_ATE_imaginary_float
#if defined(illumos)
|| enc == DW_ATE_SUN_imaginary_float
#endif
)
col = 2;
while (map->fsm_typesz[szidx] != 0) {
if (map->fsm_typesz[szidx] * mult == sz)
return (map->fsm_enc[col]);
map++;
}
terminate("die %llu: unrecognized real type size %u\n", off, sz);
return (0);
}
static intr_t *
die_base_from_dwarf(dwarf_t *dw, Dwarf_Die base, Dwarf_Off off, size_t sz)
{
intr_t *intr = xcalloc(sizeof (intr_t));
Dwarf_Signed enc;
(void) die_signed(dw, base, DW_AT_encoding, &enc, DW_ATTR_REQ);
switch (enc) {
case DW_ATE_unsigned:
case DW_ATE_address:
intr->intr_type = INTR_INT;
break;
case DW_ATE_unsigned_char:
intr->intr_type = INTR_INT;
intr->intr_iformat = 'c';
break;
case DW_ATE_signed:
intr->intr_type = INTR_INT;
intr->intr_signed = 1;
break;
case DW_ATE_signed_char:
intr->intr_type = INTR_INT;
intr->intr_signed = 1;
intr->intr_iformat = 'c';
break;
case DW_ATE_boolean:
intr->intr_type = INTR_INT;
intr->intr_signed = 1;
intr->intr_iformat = 'b';
break;
case DW_ATE_float:
case DW_ATE_complex_float:
case DW_ATE_imaginary_float:
#if defined(illumos)
case DW_ATE_SUN_imaginary_float:
case DW_ATE_SUN_interval_float:
#endif
intr->intr_type = INTR_REAL;
intr->intr_signed = 1;
intr->intr_fformat = die_base_type2enc(dw, off, enc, sz);
break;
default:
terminate("die %llu: unknown base type encoding 0x%llx\n",
off, enc);
}
return (intr);
}
static void
die_base_create(dwarf_t *dw, Dwarf_Die base, Dwarf_Off off, tdesc_t *tdp)
{
Dwarf_Unsigned sz;
intr_t *intr;
char *new;
debug(3, "die %llu: creating base type\n", off);
(void) die_unsigned(dw, base, DW_AT_byte_size, &sz, DW_ATTR_REQ);
if (tdp->t_name == NULL)
terminate("die %llu: base type without name\n", off);
if ((intr = die_base_name_parse(tdp->t_name, &new)) != NULL) {
debug(3, "die %llu: name \"%s\" remapped to \"%s\"\n", off,
tdesc_name(tdp), new);
free(tdp->t_name);
tdp->t_name = new;
} else {
debug(3, "die %llu: using dwarf data for base \"%s\"\n", off,
tdesc_name(tdp));
intr = die_base_from_dwarf(dw, base, off, sz);
}
intr->intr_nbits = sz * 8;
tdp->t_type = INTRINSIC;
tdp->t_intr = intr;
tdp->t_size = sz;
tdp->t_flags |= TDESC_F_RESOLVED;
}
static void
die_through_create(dwarf_t *dw, Dwarf_Die die, Dwarf_Off off, tdesc_t *tdp,
int type, const char *typename)
{
Dwarf_Attribute attr;
debug(3, "die %llu <%llx>: creating %s type %d\n", off, off, typename, type);
tdp->t_type = type;
if ((attr = die_attr(dw, die, DW_AT_type, 0)) != NULL) {
tdp->t_tdesc = die_lookup_pass1(dw, die, DW_AT_type);
} else {
tdp->t_tdesc = tdesc_intr_void(dw);
}
if (type == POINTER)
tdp->t_size = dw->dw_ptrsz;
tdp->t_flags |= TDESC_F_RESOLVED;
if (type == TYPEDEF) {
iidesc_t *ii = xcalloc(sizeof (iidesc_t));
ii->ii_type = II_TYPE;
ii->ii_name = xstrdup(tdp->t_name);
ii->ii_dtype = tdp;
iidesc_add(dw->dw_td->td_iihash, ii);
}
}
static void
die_typedef_create(dwarf_t *dw, Dwarf_Die die, Dwarf_Off off, tdesc_t *tdp)
{
die_through_create(dw, die, off, tdp, TYPEDEF, "typedef");
}
static void
die_const_create(dwarf_t *dw, Dwarf_Die die, Dwarf_Off off, tdesc_t *tdp)
{
die_through_create(dw, die, off, tdp, CONST, "const");
}
static void
die_pointer_create(dwarf_t *dw, Dwarf_Die die, Dwarf_Off off, tdesc_t *tdp)
{
die_through_create(dw, die, off, tdp, POINTER, "pointer");
}
static void
die_restrict_create(dwarf_t *dw, Dwarf_Die die, Dwarf_Off off, tdesc_t *tdp)
{
die_through_create(dw, die, off, tdp, RESTRICT, "restrict");
}
static void
die_volatile_create(dwarf_t *dw, Dwarf_Die die, Dwarf_Off off, tdesc_t *tdp)
{
die_through_create(dw, die, off, tdp, VOLATILE, "volatile");
}
static void
die_function_create(dwarf_t *dw, Dwarf_Die die, Dwarf_Off off, tdesc_t *tdp __unused)
{
Dwarf_Die arg;
Dwarf_Half tag;
iidesc_t *ii;
char *name;
debug(3, "die %llu <%llx>: creating function definition\n", off, off);
for (arg = die_child(dw, die); arg != NULL;
arg = die_sibling(dw, arg)) {
if ((tag = die_tag(dw, arg)) != DW_TAG_formal_parameter &&
tag != DW_TAG_variable) {
die_create_one(dw, arg);
}
}
if (die_isdecl(dw, die) || (name = die_name(dw, die)) == NULL) {
return;
}
ii = xcalloc(sizeof (iidesc_t));
ii->ii_type = die_isglobal(dw, die) ? II_GFUN : II_SFUN;
ii->ii_name = name;
if (ii->ii_type == II_SFUN)
ii->ii_owner = xstrdup(dw->dw_cuname);
debug(3, "die %llu: function %s is %s\n", off, ii->ii_name,
(ii->ii_type == II_GFUN ? "global" : "static"));
if (die_attr(dw, die, DW_AT_type, 0) != NULL)
ii->ii_dtype = die_lookup_pass1(dw, die, DW_AT_type);
else
ii->ii_dtype = tdesc_intr_void(dw);
for (arg = die_child(dw, die); arg != NULL;
arg = die_sibling(dw, arg)) {
char *name1;
debug(3, "die %llu: looking at sub member at %llu\n",
off, die_off(dw, die));
if (die_tag(dw, arg) != DW_TAG_formal_parameter)
continue;
if ((name1 = die_name(dw, arg)) == NULL) {
terminate("die %llu: func arg %d has no name\n",
off, ii->ii_nargs + 1);
}
if (strcmp(name1, "...") == 0) {
free(name1);
ii->ii_vargs = 1;
continue;
}
free(name1);
ii->ii_nargs++;
}
if (ii->ii_nargs > 0) {
int i;
debug(3, "die %llu: function has %d argument%s\n", off,
ii->ii_nargs, (ii->ii_nargs == 1 ? "" : "s"));
ii->ii_args = xcalloc(sizeof (tdesc_t) * ii->ii_nargs);
for (arg = die_child(dw, die), i = 0;
arg != NULL && i < ii->ii_nargs;
arg = die_sibling(dw, arg)) {
if (die_tag(dw, arg) != DW_TAG_formal_parameter)
continue;
ii->ii_args[i++] = die_lookup_pass1(dw, arg,
DW_AT_type);
}
}
iidesc_add(dw->dw_td->td_iihash, ii);
}
static void
die_variable_create(dwarf_t *dw, Dwarf_Die die, Dwarf_Off off, tdesc_t *tdp __unused)
{
iidesc_t *ii;
char *name;
debug(3, "die %llu: creating object definition\n", off);
if (die_isdecl(dw, die) || (name = die_name(dw, die)) == NULL)
return;
ii = xcalloc(sizeof (iidesc_t));
ii->ii_type = die_isglobal(dw, die) ? II_GVAR : II_SVAR;
ii->ii_name = name;
ii->ii_dtype = die_lookup_pass1(dw, die, DW_AT_type);
if (ii->ii_type == II_SVAR)
ii->ii_owner = xstrdup(dw->dw_cuname);
iidesc_add(dw->dw_td->td_iihash, ii);
}
static int
die_fwd_resolve(tdesc_t *fwd, tdesc_t **fwdp, void *private __unused)
{
if (fwd->t_flags & TDESC_F_RESOLVED)
return (1);
if (fwd->t_tdesc != NULL) {
debug(3, "tdp %u: unforwarded %s\n", fwd->t_id,
tdesc_name(fwd));
*fwdp = fwd->t_tdesc;
}
fwd->t_flags |= TDESC_F_RESOLVED;
return (1);
}
static void
die_lexblk_descend(dwarf_t *dw, Dwarf_Die die, Dwarf_Off off __unused, tdesc_t *tdp __unused)
{
Dwarf_Die child = die_child(dw, die);
if (child != NULL)
die_create(dw, child);
}
#define DW_F_NOTDP 0x1
typedef struct die_creator {
Dwarf_Half dc_tag;
uint16_t dc_flags;
void (*dc_create)(dwarf_t *, Dwarf_Die, Dwarf_Off, tdesc_t *);
} die_creator_t;
static const die_creator_t die_creators[] = {
{ DW_TAG_array_type, 0, die_array_create },
{ DW_TAG_enumeration_type, 0, die_enum_create },
{ DW_TAG_lexical_block, DW_F_NOTDP, die_lexblk_descend },
{ DW_TAG_pointer_type, 0, die_pointer_create },
{ DW_TAG_structure_type, 0, die_struct_create },
{ DW_TAG_subroutine_type, 0, die_funcptr_create },
{ DW_TAG_typedef, 0, die_typedef_create },
{ DW_TAG_union_type, 0, die_union_create },
{ DW_TAG_base_type, 0, die_base_create },
{ DW_TAG_const_type, 0, die_const_create },
{ DW_TAG_subprogram, DW_F_NOTDP, die_function_create },
{ DW_TAG_variable, DW_F_NOTDP, die_variable_create },
{ DW_TAG_volatile_type, 0, die_volatile_create },
{ DW_TAG_restrict_type, 0, die_restrict_create },
{ 0, 0, NULL }
};
static const die_creator_t *
die_tag2ctor(Dwarf_Half tag)
{
const die_creator_t *dc;
for (dc = die_creators; dc->dc_create != NULL; dc++) {
if (dc->dc_tag == tag)
return (dc);
}
return (NULL);
}
static void
die_create_one(dwarf_t *dw, Dwarf_Die die)
{
Dwarf_Off off = die_off(dw, die);
const die_creator_t *dc;
Dwarf_Half tag;
tdesc_t *tdp;
debug(3, "die %llu <%llx>: create_one\n", off, off);
if (off > dw->dw_maxoff) {
terminate("illegal die offset %llu (max %llu)\n", off,
dw->dw_maxoff);
}
tag = die_tag(dw, die);
if ((dc = die_tag2ctor(tag)) == NULL) {
debug(2, "die %llu: ignoring tag type %x\n", off, tag);
return;
}
if ((tdp = tdesc_lookup(dw, off)) == NULL &&
!(dc->dc_flags & DW_F_NOTDP)) {
tdp = xcalloc(sizeof (tdesc_t));
tdp->t_id = off;
tdesc_add(dw, tdp);
}
if (tdp != NULL)
tdp->t_name = die_name(dw, die);
dc->dc_create(dw, die, off, tdp);
}
static void
die_create(dwarf_t *dw, Dwarf_Die die)
{
do {
die_create_one(dw, die);
} while ((die = die_sibling(dw, die)) != NULL);
}
static tdtrav_cb_f die_resolvers[] = {
NULL,
NULL,
NULL,
die_array_resolve,
NULL,
die_sou_resolve,
die_sou_resolve,
die_enum_resolve,
die_fwd_resolve,
NULL,
NULL,
NULL,
NULL,
NULL,
};
static tdtrav_cb_f die_fail_reporters[] = {
NULL,
NULL,
NULL,
die_array_failed,
NULL,
die_sou_failed,
die_sou_failed,
NULL,
NULL,
NULL,
NULL,
NULL,
NULL,
NULL,
};
static void
die_resolve(dwarf_t *dw)
{
int last = -1;
int pass = 0;
do {
pass++;
dw->dw_nunres = 0;
(void) iitraverse_hash(dw->dw_td->td_iihash,
&dw->dw_td->td_curvgen, NULL, NULL, die_resolvers, dw);
debug(3, "resolve: pass %d, %u left\n", pass, dw->dw_nunres);
if ((int) dw->dw_nunres == last) {
fprintf(stderr, "%s: failed to resolve the following "
"types:\n", progname);
(void) iitraverse_hash(dw->dw_td->td_iihash,
&dw->dw_td->td_curvgen, NULL, NULL,
die_fail_reporters, dw);
terminate("failed to resolve types\n");
}
last = dw->dw_nunres;
} while (dw->dw_nunres != 0);
}
static boolean_t
should_have_dwarf(Elf *elf)
{
Elf_Scn *scn = NULL;
Elf_Data *data = NULL;
GElf_Shdr shdr;
GElf_Sym sym;
uint32_t symdx = 0;
size_t nsyms = 0;
boolean_t found = B_FALSE;
while ((scn = elf_nextscn(elf, scn)) != NULL) {
gelf_getshdr(scn, &shdr);
if (shdr.sh_type == SHT_SYMTAB) {
found = B_TRUE;
break;
}
}
if (!found)
terminate("cannot convert stripped objects\n");
data = elf_getdata(scn, NULL);
nsyms = shdr.sh_size / shdr.sh_entsize;
for (symdx = 0; symdx < nsyms; symdx++) {
gelf_getsym(data, symdx, &sym);
if ((GELF_ST_TYPE(sym.st_info) == STT_FUNC) ||
(GELF_ST_TYPE(sym.st_info) == STT_TLS) ||
(GELF_ST_TYPE(sym.st_info) == STT_OBJECT)) {
char *name;
name = elf_strptr(elf, shdr.sh_link, sym.st_name);
if ((strcmp(name, "Bbss.bss") != 0) &&
(strcmp(name, "Ttbss.bss") != 0) &&
(strcmp(name, "Ddata.data") != 0) &&
(strcmp(name, "Ttdata.data") != 0) &&
(strcmp(name, "Drodata.rodata") != 0))
return (B_TRUE);
}
}
return (B_FALSE);
}
int
dw_read(tdata_t *td, Elf *elf, char *filename __unused)
{
Dwarf_Unsigned abboff, hdrlen, lang, nxthdr;
Dwarf_Half vers, addrsz, offsz;
Dwarf_Die cu = 0;
Dwarf_Die child = 0;
dwarf_t dw;
char *prod = NULL;
int rc;
bzero(&dw, sizeof (dwarf_t));
dw.dw_td = td;
dw.dw_ptrsz = elf_ptrsz(elf);
dw.dw_mfgtid_last = TID_MFGTID_BASE;
dw.dw_tidhash = hash_new(TDESC_HASH_BUCKETS, tdesc_idhash, tdesc_idcmp);
dw.dw_fwdhash = hash_new(TDESC_HASH_BUCKETS, tdesc_namehash,
tdesc_namecmp);
dw.dw_enumhash = hash_new(TDESC_HASH_BUCKETS, tdesc_namehash,
tdesc_namecmp);
if ((rc = dwarf_elf_init(elf, DW_DLC_READ, NULL, NULL, &dw.dw_dw,
&dw.dw_err)) == DW_DLV_NO_ENTRY) {
if (should_have_dwarf(elf)) {
errno = ENOENT;
return (-1);
} else {
return (0);
}
} else if (rc != DW_DLV_OK) {
if (dwarf_errno(dw.dw_err) == DW_DLE_DEBUG_INFO_NULL) {
return (0);
}
terminate("failed to initialize DWARF: %s\n",
dwarf_errmsg(dw.dw_err));
}
if ((rc = dwarf_next_cu_header_b(dw.dw_dw, &hdrlen, &vers, &abboff,
&addrsz, &offsz, NULL, &nxthdr, &dw.dw_err)) != DW_DLV_OK) {
if (dw.dw_err.err_error == DW_DLE_NO_ENTRY)
exit(0);
else
terminate("rc = %d %s\n", rc, dwarf_errmsg(dw.dw_err));
}
if ((cu = die_sibling(&dw, NULL)) == NULL ||
(((child = die_child(&dw, cu)) == NULL) &&
should_have_dwarf(elf))) {
terminate("file does not contain dwarf type data "
"(try compiling with -g)\n");
} else if (child == NULL) {
return (0);
}
dw.dw_maxoff = nxthdr - 1;
if (dw.dw_maxoff > TID_FILEMAX)
terminate("file contains too many types\n");
debug(1, "DWARF version: %d\n", vers);
if (vers < 2 || vers > 4) {
terminate("file contains incompatible version %d DWARF code "
"(version 2, 3 or 4 required)\n", vers);
}
if (die_string(&dw, cu, DW_AT_producer, &prod, 0)) {
debug(1, "DWARF emitter: %s\n", prod);
free(prod);
}
if (dwarf_attrval_unsigned(cu, DW_AT_language, &lang, &dw.dw_err) == 0)
switch (lang) {
case DW_LANG_C:
case DW_LANG_C89:
case DW_LANG_C99:
case DW_LANG_C11:
case DW_LANG_C_plus_plus:
case DW_LANG_C_plus_plus_03:
case DW_LANG_C_plus_plus_11:
case DW_LANG_C_plus_plus_14:
case DW_LANG_Mips_Assembler:
break;
default:
terminate("file contains DWARF for unsupported "
"language %#x", lang);
}
else
warning("die %llu: failed to get language attribute: %s\n",
die_off(&dw, cu), dwarf_errmsg(dw.dw_err));
if ((dw.dw_cuname = die_name(&dw, cu)) != NULL) {
char *base = xstrdup(basename(dw.dw_cuname));
free(dw.dw_cuname);
dw.dw_cuname = base;
debug(1, "CU name: %s\n", dw.dw_cuname);
}
if ((child = die_child(&dw, cu)) != NULL)
die_create(&dw, child);
if ((rc = dwarf_next_cu_header_b(dw.dw_dw, &hdrlen, &vers, &abboff,
&addrsz, &offsz, NULL, &nxthdr, &dw.dw_err)) != DW_DLV_NO_ENTRY)
terminate("multiple compilation units not supported\n");
(void) dwarf_finish(dw.dw_dw, &dw.dw_err);
die_resolve(&dw);
cvt_fixups(td, dw.dw_ptrsz);
return (0);
}
