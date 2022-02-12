#pragma ident "%Z%%M% %I% %E% SMI"
#include <stdio.h>
#include <strings.h>
#include <assert.h>
#include <pthread.h>
#include "ctf_headers.h"
#include "ctftools.h"
#include "list.h"
#include "alist.h"
#include "memory.h"
#include "traverse.h"
typedef struct equiv_data equiv_data_t;
typedef struct merge_cb_data merge_cb_data_t;
typedef struct tdesc_ops {
const char *name;
int (*equiv)(tdesc_t *, tdesc_t *, equiv_data_t *);
tdesc_t *(*conjure)(tdesc_t *, int, merge_cb_data_t *);
} tdesc_ops_t;
extern tdesc_ops_t tdesc_ops[];
struct merge_cb_data {
tdata_t *md_parent;
tdata_t *md_tgt;
alist_t *md_ta;
alist_t *md_fdida;
list_t **md_iitba;
hash_t *md_tdtba;
list_t **md_tdtbr;
int md_flags;
};
#define MCD_F_SELFUNIQUIFY 0x1
#define MCD_F_REFMERGE 0x2
static void
add_mapping(alist_t *ta, tid_t srcid, tid_t tgtid)
{
debug(3, "Adding mapping %u <%x> => %u <%x>\n", srcid, srcid, tgtid, tgtid);
assert(!alist_find(ta, (void *)(uintptr_t)srcid, NULL));
assert(srcid != 0 && tgtid != 0);
alist_add(ta, (void *)(uintptr_t)srcid, (void *)(uintptr_t)tgtid);
}
static tid_t
get_mapping(alist_t *ta, int srcid)
{
void *ltgtid;
if (alist_find(ta, (void *)(uintptr_t)srcid, (void **)&ltgtid))
return ((uintptr_t)ltgtid);
else
return (0);
}
struct equiv_data {
alist_t *ed_ta;
tdesc_t *ed_node;
tdesc_t *ed_tgt;
int ed_clear_mark;
int ed_cur_mark;
int ed_selfuniquify;
};
static int equiv_node(tdesc_t *, tdesc_t *, equiv_data_t *);
static int
equiv_intrinsic(tdesc_t *stdp, tdesc_t *ttdp, equiv_data_t *ed __unused)
{
intr_t *si = stdp->t_intr;
intr_t *ti = ttdp->t_intr;
if (si->intr_type != ti->intr_type ||
si->intr_signed != ti->intr_signed ||
si->intr_offset != ti->intr_offset ||
si->intr_nbits != ti->intr_nbits)
return (0);
if (si->intr_type == INTR_INT &&
si->intr_iformat != ti->intr_iformat)
return (0);
else if (si->intr_type == INTR_REAL &&
si->intr_fformat != ti->intr_fformat)
return (0);
return (1);
}
static int
equiv_plain(tdesc_t *stdp, tdesc_t *ttdp, equiv_data_t *ed)
{
return (equiv_node(stdp->t_tdesc, ttdp->t_tdesc, ed));
}
static int
equiv_function(tdesc_t *stdp, tdesc_t *ttdp, equiv_data_t *ed)
{
fndef_t *fn1 = stdp->t_fndef, *fn2 = ttdp->t_fndef;
int i;
if (fn1->fn_nargs != fn2->fn_nargs ||
fn1->fn_vargs != fn2->fn_vargs)
return (0);
if (!equiv_node(fn1->fn_ret, fn2->fn_ret, ed))
return (0);
for (i = 0; i < (int) fn1->fn_nargs; i++) {
if (!equiv_node(fn1->fn_args[i], fn2->fn_args[i], ed))
return (0);
}
return (1);
}
static int
equiv_array(tdesc_t *stdp, tdesc_t *ttdp, equiv_data_t *ed)
{
ardef_t *ar1 = stdp->t_ardef, *ar2 = ttdp->t_ardef;
if (!equiv_node(ar1->ad_contents, ar2->ad_contents, ed) ||
!equiv_node(ar1->ad_idxtype, ar2->ad_idxtype, ed))
return (0);
if (ar1->ad_nelems != ar2->ad_nelems)
return (0);
return (1);
}
static int
equiv_su(tdesc_t *stdp, tdesc_t *ttdp, equiv_data_t *ed)
{
mlist_t *ml1 = stdp->t_members, *ml2 = ttdp->t_members;
while (ml1 && ml2) {
if (ml1->ml_offset != ml2->ml_offset ||
strcmp(ml1->ml_name, ml2->ml_name) != 0 ||
ml1->ml_size != ml2->ml_size ||
!equiv_node(ml1->ml_type, ml2->ml_type, ed))
return (0);
ml1 = ml1->ml_next;
ml2 = ml2->ml_next;
}
if (ml1 || ml2)
return (0);
return (1);
}
static int
equiv_enum(tdesc_t *stdp, tdesc_t *ttdp, equiv_data_t *ed __unused)
{
elist_t *el1 = stdp->t_emem;
elist_t *el2 = ttdp->t_emem;
while (el1 && el2) {
if (el1->el_number != el2->el_number ||
strcmp(el1->el_name, el2->el_name) != 0)
return (0);
el1 = el1->el_next;
el2 = el2->el_next;
}
if (el1 || el2)
return (0);
return (1);
}
static int
equiv_assert(tdesc_t *stdp __unused, tdesc_t *ttdp __unused, equiv_data_t *ed __unused)
{
assert(1 == 0);
return (0);
}
static int
fwd_equiv(tdesc_t *ctdp, tdesc_t *mtdp)
{
tdesc_t *defn = (ctdp->t_type == FORWARD ? mtdp : ctdp);
return (defn->t_type == STRUCT || defn->t_type == UNION ||
defn->t_type == ENUM);
}
static int
equiv_node(tdesc_t *ctdp, tdesc_t *mtdp, equiv_data_t *ed)
{
int (*equiv)(tdesc_t *, tdesc_t *, equiv_data_t *);
int mapping;
if (ctdp->t_emark > ed->ed_clear_mark &&
mtdp->t_emark > ed->ed_clear_mark)
return (ctdp->t_emark == mtdp->t_emark);
if ((mapping = get_mapping(ed->ed_ta, ctdp->t_id)) > 0 &&
mapping == mtdp->t_id && !ed->ed_selfuniquify)
return (1);
if (!streq(ctdp->t_name, mtdp->t_name))
return (0);
if (ctdp->t_type != mtdp->t_type) {
if (ctdp->t_type == FORWARD || mtdp->t_type == FORWARD)
return (fwd_equiv(ctdp, mtdp));
else
return (0);
}
ctdp->t_emark = ed->ed_cur_mark;
mtdp->t_emark = ed->ed_cur_mark;
ed->ed_cur_mark++;
if ((equiv = tdesc_ops[ctdp->t_type].equiv) != NULL)
return (equiv(ctdp, mtdp, ed));
return (1);
}
static int
equiv_cb(void *bucket, void *arg)
{
equiv_data_t *ed = arg;
tdesc_t *mtdp = bucket;
tdesc_t *ctdp = ed->ed_node;
ed->ed_clear_mark = ed->ed_cur_mark + 1;
ed->ed_cur_mark = ed->ed_clear_mark + 1;
if (equiv_node(ctdp, mtdp, ed)) {
debug(3, "equiv_node matched %d <%x> %d <%x>\n",
ctdp->t_id, ctdp->t_id, mtdp->t_id, mtdp->t_id);
ed->ed_tgt = mtdp;
return (-1);
}
return (0);
}
static int
map_td_tree_pre(tdesc_t *ctdp, tdesc_t **ctdpp __unused, void *private)
{
merge_cb_data_t *mcd = private;
if (get_mapping(mcd->md_ta, ctdp->t_id) > 0)
return (0);
return (1);
}
static int
map_td_tree_post(tdesc_t *ctdp, tdesc_t **ctdpp __unused, void *private)
{
merge_cb_data_t *mcd = private;
equiv_data_t ed;
ed.ed_ta = mcd->md_ta;
ed.ed_clear_mark = mcd->md_parent->td_curemark;
ed.ed_cur_mark = mcd->md_parent->td_curemark + 1;
ed.ed_node = ctdp;
ed.ed_selfuniquify = 0;
debug(3, "map_td_tree_post on %d <%x> %s\n", ctdp->t_id, ctdp->t_id,tdesc_name(ctdp));
if (hash_find_iter(mcd->md_parent->td_layouthash, ctdp,
equiv_cb, &ed) < 0) {
if (ed.ed_tgt->t_type == FORWARD && ctdp->t_type != FORWARD) {
int id = mcd->md_tgt->td_nextid++;
debug(3, "Creating new defn type %d <%x>\n", id, id);
add_mapping(mcd->md_ta, ctdp->t_id, id);
alist_add(mcd->md_fdida, (void *)(ulong_t)ed.ed_tgt,
(void *)(ulong_t)id);
hash_add(mcd->md_tdtba, ctdp);
} else
add_mapping(mcd->md_ta, ctdp->t_id, ed.ed_tgt->t_id);
} else if (debug_level > 1 && hash_iter(mcd->md_parent->td_idhash,
equiv_cb, &ed) < 0) {
aborterr("Second pass for %d (%s) == %d\n", ctdp->t_id,
tdesc_name(ctdp), ed.ed_tgt->t_id);
} else {
int id = mcd->md_tgt->td_nextid++;
debug(3, "Creating new type %d <%x>\n", id, id);
add_mapping(mcd->md_ta, ctdp->t_id, id);
hash_add(mcd->md_tdtba, ctdp);
}
mcd->md_parent->td_curemark = ed.ed_cur_mark + 1;
return (1);
}
static int
map_td_tree_self_post(tdesc_t *ctdp, tdesc_t **ctdpp __unused, void *private)
{
merge_cb_data_t *mcd = private;
equiv_data_t ed;
ed.ed_ta = mcd->md_ta;
ed.ed_clear_mark = mcd->md_parent->td_curemark;
ed.ed_cur_mark = mcd->md_parent->td_curemark + 1;
ed.ed_node = ctdp;
ed.ed_selfuniquify = 1;
ed.ed_tgt = NULL;
if (hash_find_iter(mcd->md_tdtba, ctdp, equiv_cb, &ed) < 0) {
debug(3, "Self check found %d <%x> in %d <%x>\n", ctdp->t_id,
ctdp->t_id, ed.ed_tgt->t_id, ed.ed_tgt->t_id);
add_mapping(mcd->md_ta, ctdp->t_id,
get_mapping(mcd->md_ta, ed.ed_tgt->t_id));
} else if (debug_level > 1 && hash_iter(mcd->md_tdtba,
equiv_cb, &ed) < 0) {
aborterr("Self-unique second pass for %d <%x> (%s) == %d <%x>\n",
ctdp->t_id, ctdp->t_id, tdesc_name(ctdp), ed.ed_tgt->t_id,
ed.ed_tgt->t_id);
} else {
int id = mcd->md_tgt->td_nextid++;
debug(3, "Creating new type %d <%x>\n", id, id);
add_mapping(mcd->md_ta, ctdp->t_id, id);
hash_add(mcd->md_tdtba, ctdp);
}
mcd->md_parent->td_curemark = ed.ed_cur_mark + 1;
return (1);
}
static tdtrav_cb_f map_pre[] = {
NULL,
map_td_tree_pre,
map_td_tree_pre,
map_td_tree_pre,
map_td_tree_pre,
map_td_tree_pre,
map_td_tree_pre,
map_td_tree_pre,
map_td_tree_pre,
map_td_tree_pre,
tdtrav_assert,
map_td_tree_pre,
map_td_tree_pre,
map_td_tree_pre
};
static tdtrav_cb_f map_post[] = {
NULL,
map_td_tree_post,
map_td_tree_post,
map_td_tree_post,
map_td_tree_post,
map_td_tree_post,
map_td_tree_post,
map_td_tree_post,
map_td_tree_post,
map_td_tree_post,
tdtrav_assert,
map_td_tree_post,
map_td_tree_post,
map_td_tree_post
};
static tdtrav_cb_f map_self_post[] = {
NULL,
map_td_tree_self_post,
map_td_tree_self_post,
map_td_tree_self_post,
map_td_tree_self_post,
map_td_tree_self_post,
map_td_tree_self_post,
map_td_tree_self_post,
map_td_tree_self_post,
map_td_tree_self_post,
tdtrav_assert,
map_td_tree_self_post,
map_td_tree_self_post,
map_td_tree_self_post
};
typedef struct iifind_data {
iidesc_t *iif_template;
alist_t *iif_ta;
int iif_newidx;
int iif_refmerge;
} iifind_data_t;
static int
iidesc_match(void *data, void *arg)
{
iidesc_t *node = data;
iifind_data_t *iif = arg;
int i;
if (node->ii_type != iif->iif_template->ii_type ||
!streq(node->ii_name, iif->iif_template->ii_name) ||
node->ii_dtype->t_id != iif->iif_newidx)
return (0);
if ((node->ii_type == II_SVAR || node->ii_type == II_SFUN) &&
!streq(node->ii_owner, iif->iif_template->ii_owner))
return (0);
if (node->ii_nargs != iif->iif_template->ii_nargs)
return (0);
for (i = 0; i < node->ii_nargs; i++) {
if (get_mapping(iif->iif_ta,
iif->iif_template->ii_args[i]->t_id) !=
node->ii_args[i]->t_id)
return (0);
}
if (iif->iif_refmerge) {
switch (iif->iif_template->ii_type) {
case II_GFUN:
case II_SFUN:
case II_GVAR:
case II_SVAR:
debug(3, "suppressing duping of %d %s from %s\n",
iif->iif_template->ii_type,
iif->iif_template->ii_name,
(iif->iif_template->ii_owner ?
iif->iif_template->ii_owner : "NULL"));
return (0);
case II_NOT:
case II_PSYM:
case II_SOU:
case II_TYPE:
break;
}
}
return (-1);
}
static int
merge_type_cb(void *data, void *arg)
{
iidesc_t *sii = data;
merge_cb_data_t *mcd = arg;
iifind_data_t iif;
tdtrav_cb_f *post;
post = (mcd->md_flags & MCD_F_SELFUNIQUIFY ? map_self_post : map_post);
(void) iitraverse(sii, &mcd->md_parent->td_curvgen, NULL, map_pre, post,
mcd);
iif.iif_template = sii;
iif.iif_ta = mcd->md_ta;
iif.iif_newidx = get_mapping(mcd->md_ta, sii->ii_dtype->t_id);
iif.iif_refmerge = (mcd->md_flags & MCD_F_REFMERGE);
if (hash_match(mcd->md_parent->td_iihash, sii, iidesc_match,
&iif) == 1)
return (1);
debug(3, "tba %s (%d)\n", (sii->ii_name ? sii->ii_name : "(anon)"),
sii->ii_type);
list_add(mcd->md_iitba, sii);
return (0);
}
static int
remap_node(tdesc_t **tgtp, tdesc_t *oldtgt, int selftid, tdesc_t *newself,
merge_cb_data_t *mcd)
{
tdesc_t *tgt = NULL;
tdesc_t template;
int oldid = oldtgt->t_id;
if (oldid == selftid) {
*tgtp = newself;
return (1);
}
if ((template.t_id = get_mapping(mcd->md_ta, oldid)) == 0)
aborterr("failed to get mapping for tid %d <%x>\n", oldid, oldid);
if (!hash_find(mcd->md_parent->td_idhash, (void *)&template,
(void *)&tgt) && (!(mcd->md_flags & MCD_F_REFMERGE) ||
!hash_find(mcd->md_tgt->td_idhash, (void *)&template,
(void *)&tgt))) {
debug(3, "Remap couldn't find %d <%x> (from %d <%x>)\n", template.t_id,
template.t_id, oldid, oldid);
*tgtp = oldtgt;
list_add(mcd->md_tdtbr, tgtp);
return (0);
}
*tgtp = tgt;
return (1);
}
static tdesc_t *
conjure_template(tdesc_t *old, int newselfid)
{
tdesc_t *new = xcalloc(sizeof (tdesc_t));
new->t_name = old->t_name ? xstrdup(old->t_name) : NULL;
new->t_type = old->t_type;
new->t_size = old->t_size;
new->t_id = newselfid;
new->t_flags = old->t_flags;
return (new);
}
static tdesc_t *
conjure_intrinsic(tdesc_t *old, int newselfid, merge_cb_data_t *mcd __unused)
{
tdesc_t *new = conjure_template(old, newselfid);
new->t_intr = xmalloc(sizeof (intr_t));
bcopy(old->t_intr, new->t_intr, sizeof (intr_t));
return (new);
}
static tdesc_t *
conjure_plain(tdesc_t *old, int newselfid, merge_cb_data_t *mcd)
{
tdesc_t *new = conjure_template(old, newselfid);
(void) remap_node(&new->t_tdesc, old->t_tdesc, old->t_id, new, mcd);
return (new);
}
static tdesc_t *
conjure_function(tdesc_t *old, int newselfid, merge_cb_data_t *mcd)
{
tdesc_t *new = conjure_template(old, newselfid);
fndef_t *nfn = xmalloc(sizeof (fndef_t));
fndef_t *ofn = old->t_fndef;
int i;
(void) remap_node(&nfn->fn_ret, ofn->fn_ret, old->t_id, new, mcd);
nfn->fn_nargs = ofn->fn_nargs;
nfn->fn_vargs = ofn->fn_vargs;
if (nfn->fn_nargs > 0)
nfn->fn_args = xcalloc(sizeof (tdesc_t *) * ofn->fn_nargs);
for (i = 0; i < (int) ofn->fn_nargs; i++) {
(void) remap_node(&nfn->fn_args[i], ofn->fn_args[i], old->t_id,
new, mcd);
}
new->t_fndef = nfn;
return (new);
}
static tdesc_t *
conjure_array(tdesc_t *old, int newselfid, merge_cb_data_t *mcd)
{
tdesc_t *new = conjure_template(old, newselfid);
ardef_t *nar = xmalloc(sizeof (ardef_t));
ardef_t *oar = old->t_ardef;
(void) remap_node(&nar->ad_contents, oar->ad_contents, old->t_id, new,
mcd);
(void) remap_node(&nar->ad_idxtype, oar->ad_idxtype, old->t_id, new,
mcd);
nar->ad_nelems = oar->ad_nelems;
new->t_ardef = nar;
return (new);
}
static tdesc_t *
conjure_su(tdesc_t *old, int newselfid, merge_cb_data_t *mcd)
{
tdesc_t *new = conjure_template(old, newselfid);
mlist_t *omem, **nmemp;
for (omem = old->t_members, nmemp = &new->t_members;
omem; omem = omem->ml_next, nmemp = &((*nmemp)->ml_next)) {
*nmemp = xmalloc(sizeof (mlist_t));
(*nmemp)->ml_offset = omem->ml_offset;
(*nmemp)->ml_size = omem->ml_size;
(*nmemp)->ml_name = xstrdup(omem->ml_name ? omem->ml_name : "empty omem->ml_name");
(void) remap_node(&((*nmemp)->ml_type), omem->ml_type,
old->t_id, new, mcd);
}
*nmemp = NULL;
return (new);
}
static tdesc_t *
conjure_enum(tdesc_t *old, int newselfid, merge_cb_data_t *mcd __unused)
{
tdesc_t *new = conjure_template(old, newselfid);
elist_t *oel, **nelp;
for (oel = old->t_emem, nelp = &new->t_emem;
oel; oel = oel->el_next, nelp = &((*nelp)->el_next)) {
*nelp = xmalloc(sizeof (elist_t));
(*nelp)->el_name = xstrdup(oel->el_name);
(*nelp)->el_number = oel->el_number;
}
*nelp = NULL;
return (new);
}
static tdesc_t *
conjure_forward(tdesc_t *old, int newselfid, merge_cb_data_t *mcd)
{
tdesc_t *new = conjure_template(old, newselfid);
list_add(&mcd->md_tgt->td_fwdlist, new);
return (new);
}
static tdesc_t *
conjure_assert(tdesc_t *old __unused, int newselfid __unused, merge_cb_data_t *mcd __unused)
{
assert(1 == 0);
return (NULL);
}
static iidesc_t *
conjure_iidesc(iidesc_t *old, merge_cb_data_t *mcd)
{
iidesc_t *new = iidesc_dup(old);
int i;
(void) remap_node(&new->ii_dtype, old->ii_dtype, -1, NULL, mcd);
for (i = 0; i < new->ii_nargs; i++) {
(void) remap_node(&new->ii_args[i], old->ii_args[i], -1, NULL,
mcd);
}
return (new);
}
static int
fwd_redir(tdesc_t *fwd, tdesc_t **fwdp, void *private)
{
alist_t *map = private;
void *defn;
if (!alist_find(map, (void *)fwd, (void **)&defn))
return (0);
debug(3, "Redirecting an edge to %s\n", tdesc_name(defn));
*fwdp = defn;
return (1);
}
static tdtrav_cb_f fwd_redir_cbs[] = {
NULL,
NULL,
NULL,
NULL,
NULL,
NULL,
NULL,
NULL,
fwd_redir,
NULL,
tdtrav_assert,
NULL,
NULL,
NULL
};
typedef struct redir_mstr_data {
tdata_t *rmd_tgt;
alist_t *rmd_map;
} redir_mstr_data_t;
static int
redir_mstr_fwd_cb(void *name, void *value, void *arg)
{
tdesc_t *fwd = name;
int defnid = (uintptr_t)value;
redir_mstr_data_t *rmd = arg;
tdesc_t template;
tdesc_t *defn;
template.t_id = defnid;
if (!hash_find(rmd->rmd_tgt->td_idhash, (void *)&template,
(void *)&defn)) {
aborterr("Couldn't unforward %d (%s)\n", defnid,
tdesc_name(defn));
}
debug(3, "Forward map: resolved %d to %s\n", defnid, tdesc_name(defn));
alist_add(rmd->rmd_map, (void *)fwd, (void *)defn);
return (1);
}
static void
redir_mstr_fwds(merge_cb_data_t *mcd)
{
redir_mstr_data_t rmd;
alist_t *map = alist_new(NULL, NULL);
rmd.rmd_tgt = mcd->md_tgt;
rmd.rmd_map = map;
if (alist_iter(mcd->md_fdida, redir_mstr_fwd_cb, &rmd)) {
(void) iitraverse_hash(mcd->md_tgt->td_iihash,
&mcd->md_tgt->td_curvgen, fwd_redir_cbs, NULL, NULL, map);
}
alist_free(map);
}
static int
add_iitba_cb(void *data, void *private)
{
merge_cb_data_t *mcd = private;
iidesc_t *tba = data;
iidesc_t *new;
iifind_data_t iif;
int newidx;
newidx = get_mapping(mcd->md_ta, tba->ii_dtype->t_id);
assert(newidx != -1);
(void) list_remove(mcd->md_iitba, data, NULL, NULL);
iif.iif_template = tba;
iif.iif_ta = mcd->md_ta;
iif.iif_newidx = newidx;
iif.iif_refmerge = (mcd->md_flags & MCD_F_REFMERGE);
if (hash_match(mcd->md_parent->td_iihash, tba, iidesc_match,
&iif) == 1) {
debug(3, "iidesc_t %s already exists\n",
(tba->ii_name ? tba->ii_name : "(anon)"));
return (1);
}
new = conjure_iidesc(tba, mcd);
hash_add(mcd->md_tgt->td_iihash, new);
return (1);
}
static int
add_tdesc(tdesc_t *oldtdp, int newid, merge_cb_data_t *mcd)
{
tdesc_t *newtdp;
tdesc_t template;
template.t_id = newid;
assert(hash_find(mcd->md_parent->td_idhash,
(void *)&template, NULL) == 0);
debug(3, "trying to conjure %d %s (%d, <%x>) as %d, <%x>\n",
oldtdp->t_type, tdesc_name(oldtdp), oldtdp->t_id,
oldtdp->t_id, newid, newid);
if ((newtdp = tdesc_ops[oldtdp->t_type].conjure(oldtdp, newid,
mcd)) == NULL)
return (0);
debug(3, "succeeded\n");
hash_add(mcd->md_tgt->td_idhash, newtdp);
hash_add(mcd->md_tgt->td_layouthash, newtdp);
return (1);
}
static int
add_tdtba_cb(void *data, void *arg)
{
tdesc_t *tdp = data;
merge_cb_data_t *mcd = arg;
int newid;
int rc;
newid = get_mapping(mcd->md_ta, tdp->t_id);
assert(newid != -1);
if ((rc = add_tdesc(tdp, newid, mcd)))
hash_remove(mcd->md_tdtba, (void *)tdp);
return (rc);
}
static int
add_tdtbr_cb(void *data, void *arg)
{
tdesc_t **tdpp = data;
merge_cb_data_t *mcd = arg;
debug(3, "Remapping %s (%d)\n", tdesc_name(*tdpp), (*tdpp)->t_id);
if (!remap_node(tdpp, *tdpp, -1, NULL, mcd))
return (0);
(void) list_remove(mcd->md_tdtbr, (void *)tdpp, NULL, NULL);
return (1);
}
static void
merge_types(hash_t *src, merge_cb_data_t *mcd)
{
list_t *iitba = NULL;
list_t *tdtbr = NULL;
int iirc, tdrc;
mcd->md_iitba = &iitba;
mcd->md_tdtba = hash_new(TDATA_LAYOUT_HASH_SIZE, tdesc_layouthash,
tdesc_layoutcmp);
mcd->md_tdtbr = &tdtbr;
(void) hash_iter(src, merge_type_cb, mcd);
tdrc = hash_iter(mcd->md_tdtba, add_tdtba_cb, mcd);
debug(3, "add_tdtba_cb added %d items\n", tdrc);
iirc = list_iter(*mcd->md_iitba, add_iitba_cb, mcd);
debug(3, "add_iitba_cb added %d items\n", iirc);
assert(list_count(*mcd->md_iitba) == 0 &&
hash_count(mcd->md_tdtba) == 0);
tdrc = list_iter(*mcd->md_tdtbr, add_tdtbr_cb, mcd);
debug(3, "add_tdtbr_cb added %d items\n", tdrc);
if (list_count(*mcd->md_tdtbr) != 0)
aborterr("Couldn't remap all nodes\n");
if (mcd->md_parent == mcd->md_tgt) {
redir_mstr_fwds(mcd);
}
}
void
merge_into_master(tdata_t *cur, tdata_t *mstr, tdata_t *tgt, int selfuniquify)
{
merge_cb_data_t mcd;
cur->td_ref++;
mstr->td_ref++;
if (tgt)
tgt->td_ref++;
assert(cur->td_ref == 1 && mstr->td_ref == 1 &&
(tgt == NULL || tgt->td_ref == 1));
mcd.md_parent = mstr;
mcd.md_tgt = (tgt ? tgt : mstr);
mcd.md_ta = alist_new(NULL, NULL);
mcd.md_fdida = alist_new(NULL, NULL);
mcd.md_flags = 0;
if (selfuniquify)
mcd.md_flags |= MCD_F_SELFUNIQUIFY;
if (tgt)
mcd.md_flags |= MCD_F_REFMERGE;
mstr->td_curvgen = MAX(mstr->td_curvgen, cur->td_curvgen);
mstr->td_curemark = MAX(mstr->td_curemark, cur->td_curemark);
merge_types(cur->td_iihash, &mcd);
if (debug_level >= 3) {
debug(3, "Type association stats\n");
alist_stats(mcd.md_ta, 0);
debug(3, "Layout hash stats\n");
hash_stats(mcd.md_tgt->td_layouthash, 1);
}
alist_free(mcd.md_fdida);
alist_free(mcd.md_ta);
cur->td_ref--;
mstr->td_ref--;
if (tgt)
tgt->td_ref--;
}
tdesc_ops_t tdesc_ops[] = {
{ "ERROR! BAD tdesc TYPE", NULL, NULL },
{ "intrinsic", equiv_intrinsic, conjure_intrinsic },
{ "pointer", equiv_plain, conjure_plain },
{ "array", equiv_array, conjure_array },
{ "function", equiv_function, conjure_function },
{ "struct", equiv_su, conjure_su },
{ "union", equiv_su, conjure_su },
{ "enum", equiv_enum, conjure_enum },
{ "forward", NULL, conjure_forward },
{ "typedef", equiv_plain, conjure_plain },
{ "typedef_unres", equiv_assert, conjure_assert },
{ "volatile", equiv_plain, conjure_plain },
{ "const", equiv_plain, conjure_plain },
{ "restrict", equiv_plain, conjure_plain }
};
