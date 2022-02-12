




























#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <pthread.h>

#include "ctftools.h"
#include "memory.h"
#include "traverse.h"
















int
tdesc_layouthash(int nbuckets, void *node)
{
tdesc_t *tdp = node;
char *name = NULL;
ulong_t h = 0;

if (tdp->t_name)
name = tdp->t_name;
else {
switch (tdp->t_type) {
case POINTER:
case TYPEDEF:
case VOLATILE:
case CONST:
case RESTRICT:
name = tdp->t_tdesc->t_name;
break;
case FUNCTION:
h = tdp->t_fndef->fn_nargs +
tdp->t_fndef->fn_vargs;
name = tdp->t_fndef->fn_ret->t_name;
break;
case ARRAY:
h = tdp->t_ardef->ad_nelems;
name = tdp->t_ardef->ad_contents->t_name;
break;
case STRUCT:
case UNION:






if (tdp->t_members != NULL)
name = tdp->t_members->ml_name;
break;
case ENUM:

name = tdp->t_emem->el_name;
break;
default:




warning("Unexpected unnamed %d tdesc (ID %d)\n",
tdp->t_type, tdp->t_id);
}
}

if (name)
return (hash_name(nbuckets, name));

return (h % nbuckets);
}

int
tdesc_layoutcmp(void *arg1, void *arg2)
{
tdesc_t *tdp1 = arg1, *tdp2 = arg2;

if (tdp1->t_name == NULL) {
if (tdp2->t_name == NULL)
return (0);
else
return (-1);
} else if (tdp2->t_name == NULL)
return (1);
else
return (strcmp(tdp1->t_name, tdp2->t_name));
}

int
tdesc_idhash(int nbuckets, void *data)
{
tdesc_t *tdp = data;

return (tdp->t_id % nbuckets);
}

int
tdesc_idcmp(void *arg1, void *arg2)
{
tdesc_t *tdp1 = arg1, *tdp2 = arg2;

if (tdp1->t_id == tdp2->t_id)
return (0);
else
return (tdp1->t_id > tdp2->t_id ? 1 : -1);
}

int
tdesc_namehash(int nbuckets, void *data)
{
tdesc_t *tdp = data;
ulong_t h, g;
char *c;

if (tdp->t_name == NULL)
return (0);

for (h = 0, c = tdp->t_name; *c; c++) {
h = (h << 4) + *c;
if ((g = (h & 0xf0000000)) != 0) {
h ^= (g >> 24);
h ^= g;
}
}

return (h % nbuckets);
}

int
tdesc_namecmp(void *arg1, void *arg2)
{
tdesc_t *tdp1 = arg1, *tdp2 = arg2;

return (!streq(tdp1->t_name, tdp2->t_name));
}

#if defined(illumos)

static int
tdesc_print(void *data, void *private __unused)
{
tdesc_t *tdp = data;

printf("%7d %s\n", tdp->t_id, tdesc_name(tdp));

return (1);
}
#endif

static void
free_intr(tdesc_t *tdp)
{
free(tdp->t_intr);
}

static void
free_ardef(tdesc_t *tdp)
{
free(tdp->t_ardef);
}

static void
free_mlist(tdesc_t *tdp)
{
mlist_t *ml = tdp->t_members;
mlist_t *oml;

while (ml) {
oml = ml;
ml = ml->ml_next;

if (oml->ml_name)
free(oml->ml_name);
free(oml);
}
}

static void
free_elist(tdesc_t *tdp)
{
elist_t *el = tdp->t_emem;
elist_t *oel;

while (el) {
oel = el;
el = el->el_next;

if (oel->el_name)
free(oel->el_name);
free(oel);
}
}

static void (*free_cbs[])(tdesc_t *) = {
NULL,
free_intr,
NULL,
free_ardef,
NULL,
free_mlist,
free_mlist,
free_elist,
NULL,
NULL,
NULL,
NULL,
NULL,
NULL
};


static void
tdesc_free_cb(void *arg, void *private __unused)
{
tdesc_t *tdp = arg;
if (tdp->t_name)
free(tdp->t_name);
if (free_cbs[tdp->t_type])
free_cbs[tdp->t_type](tdp);
free(tdp);

return;
}

void
tdesc_free(tdesc_t *tdp)
{
tdesc_free_cb(tdp, NULL);
}

static int
tdata_label_cmp(void *arg1, void *arg2)
{
labelent_t *le1 = arg1;
labelent_t *le2 = arg2;
return (le1->le_idx - le2->le_idx);
}

void
tdata_label_add(tdata_t *td, const char *label, int idx)
{
labelent_t *le = xmalloc(sizeof (*le));

le->le_name = xstrdup(label);
le->le_idx = (idx == -1 ? td->td_nextid - 1 : idx);

slist_add(&td->td_labels, le, tdata_label_cmp);
}

static int
tdata_label_top_cb(void *data, void *arg)
{
labelent_t *le = data;
labelent_t **topp = arg;

*topp = le;

return (1);
}

labelent_t *
tdata_label_top(tdata_t *td)
{
labelent_t *top = NULL;

(void) list_iter(td->td_labels, tdata_label_top_cb, &top);

return (top);
}

static int
tdata_label_find_cb(void *arg1, void *arg2)
{
labelent_t *le = arg1;
labelent_t *tmpl = arg2;
return (streq(le->le_name, tmpl->le_name));
}

int
tdata_label_find(tdata_t *td, char *label)
{
labelent_t let;
labelent_t *ret;

if (streq(label, "BASE")) {
ret = (labelent_t *)list_first(td->td_labels);
return (ret ? ret->le_idx : -1);
}

let.le_name = label;

if (!(ret = (labelent_t *)list_find(td->td_labels, &let,
tdata_label_find_cb)))
return (-1);

return (ret->le_idx);
}

static int
tdata_label_newmax_cb(void *data, void *arg)
{
labelent_t *le = data;
int *newmaxp = arg;

if (le->le_idx > *newmaxp) {
le->le_idx = *newmaxp;
return (1);
}

return (0);
}

void
tdata_label_newmax(tdata_t *td, int newmax)
{
(void) list_iter(td->td_labels, tdata_label_newmax_cb, &newmax);
}


static void
tdata_label_free_cb(void *arg, void *private __unused)
{
labelent_t *le = arg;
if (le->le_name)
free(le->le_name);
free(le);
}

void
tdata_label_free(tdata_t *td)
{
list_free(td->td_labels, tdata_label_free_cb, NULL);
td->td_labels = NULL;
}

tdata_t *
tdata_new(void)
{
tdata_t *new = xcalloc(sizeof (tdata_t));

new->td_layouthash = hash_new(TDATA_LAYOUT_HASH_SIZE, tdesc_layouthash,
tdesc_layoutcmp);
new->td_idhash = hash_new(TDATA_ID_HASH_SIZE, tdesc_idhash,
tdesc_idcmp);





new->td_iihash = hash_new(IIDESC_HASH_SIZE, iidesc_hash, NULL);
new->td_nextid = 1;
new->td_curvgen = 1;

pthread_mutex_init(&new->td_mergelock, NULL);

return (new);
}

void
tdata_free(tdata_t *td)
{
hash_free(td->td_iihash, iidesc_free, NULL);
hash_free(td->td_layouthash, tdesc_free_cb, NULL);
hash_free(td->td_idhash, NULL, NULL);
list_free(td->td_fwdlist, NULL, NULL);

tdata_label_free(td);

free(td->td_parlabel);
free(td->td_parname);

pthread_mutex_destroy(&td->td_mergelock);

free(td);
}


static int
build_hashes(tdesc_t *ctdp, tdesc_t **ctdpp __unused, void *private)
{
tdata_t *td = private;

hash_add(td->td_idhash, ctdp);
hash_add(td->td_layouthash, ctdp);

return (1);
}

static tdtrav_cb_f build_hashes_cbs[] = {
NULL,
build_hashes,
build_hashes,
build_hashes,
build_hashes,
build_hashes,
build_hashes,
build_hashes,
build_hashes,
build_hashes,
tdtrav_assert,
build_hashes,
build_hashes,
build_hashes
};

static void
tdata_build_hashes_common(tdata_t *td, hash_t *hash)
{
(void) iitraverse_hash(hash, &td->td_curvgen, NULL, NULL,
build_hashes_cbs, td);
}

void
tdata_build_hashes(tdata_t *td)
{
tdata_build_hashes_common(td, td->td_iihash);
}


void
tdata_merge(tdata_t *td1, tdata_t *td2)
{
td1->td_curemark = MAX(td1->td_curemark, td2->td_curemark);
td1->td_curvgen = MAX(td1->td_curvgen, td2->td_curvgen);
td1->td_nextid = MAX(td1->td_nextid, td2->td_nextid);

hash_merge(td1->td_iihash, td2->td_iihash);


tdata_build_hashes_common(td1, td2->td_iihash);

list_concat(&td1->td_fwdlist, td2->td_fwdlist);
td2->td_fwdlist = NULL;

slist_merge(&td1->td_labels, td2->td_labels,
tdata_label_cmp);
td2->td_labels = NULL;



hash_free(td2->td_layouthash, NULL, NULL);
td2->td_layouthash = NULL;

hash_free(td2->td_iihash, NULL, NULL);
td2->td_iihash = NULL;

tdata_free(td2);
}
