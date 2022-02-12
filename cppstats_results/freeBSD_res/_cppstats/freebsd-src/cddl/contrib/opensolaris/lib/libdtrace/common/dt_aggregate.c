#include <stdlib.h>
#include <strings.h>
#include <errno.h>
#include <unistd.h>
#include <dt_impl.h>
#include <assert.h>
#if defined(illumos)
#include <alloca.h>
#else
#include <sys/sysctl.h>
#include <libproc_compat.h>
#endif
#include <limits.h>
#define DTRACE_AHASHSIZE 32779
static pthread_mutex_t dt_qsort_lock = PTHREAD_MUTEX_INITIALIZER;
static int dt_revsort;
static int dt_keysort;
static int dt_keypos;
#define DT_LESSTHAN (dt_revsort == 0 ? -1 : 1)
#define DT_GREATERTHAN (dt_revsort == 0 ? 1 : -1)
static void
dt_aggregate_count(int64_t *existing, int64_t *new, size_t size)
{
uint_t i;
for (i = 0; i < size / sizeof (int64_t); i++)
existing[i] = existing[i] + new[i];
}
static int
dt_aggregate_countcmp(int64_t *lhs, int64_t *rhs)
{
int64_t lvar = *lhs;
int64_t rvar = *rhs;
if (lvar < rvar)
return (DT_LESSTHAN);
if (lvar > rvar)
return (DT_GREATERTHAN);
return (0);
}
static void
dt_aggregate_min(int64_t *existing, int64_t *new, size_t size)
{
if (*new < *existing)
*existing = *new;
}
static void
dt_aggregate_max(int64_t *existing, int64_t *new, size_t size)
{
if (*new > *existing)
*existing = *new;
}
static int
dt_aggregate_averagecmp(int64_t *lhs, int64_t *rhs)
{
int64_t lavg = lhs[0] ? (lhs[1] / lhs[0]) : 0;
int64_t ravg = rhs[0] ? (rhs[1] / rhs[0]) : 0;
if (lavg < ravg)
return (DT_LESSTHAN);
if (lavg > ravg)
return (DT_GREATERTHAN);
return (0);
}
static int
dt_aggregate_stddevcmp(int64_t *lhs, int64_t *rhs)
{
uint64_t lsd = dt_stddev((uint64_t *)lhs, 1);
uint64_t rsd = dt_stddev((uint64_t *)rhs, 1);
if (lsd < rsd)
return (DT_LESSTHAN);
if (lsd > rsd)
return (DT_GREATERTHAN);
return (0);
}
static void
dt_aggregate_lquantize(int64_t *existing, int64_t *new, size_t size)
{
int64_t arg = *existing++;
uint16_t levels = DTRACE_LQUANTIZE_LEVELS(arg);
int i;
for (i = 0; i <= levels + 1; i++)
existing[i] = existing[i] + new[i + 1];
}
static long double
dt_aggregate_lquantizedsum(int64_t *lquanta)
{
int64_t arg = *lquanta++;
int32_t base = DTRACE_LQUANTIZE_BASE(arg);
uint16_t step = DTRACE_LQUANTIZE_STEP(arg);
uint16_t levels = DTRACE_LQUANTIZE_LEVELS(arg), i;
long double total = (long double)lquanta[0] * (long double)(base - 1);
for (i = 0; i < levels; base += step, i++)
total += (long double)lquanta[i + 1] * (long double)base;
return (total + (long double)lquanta[levels + 1] *
(long double)(base + 1));
}
static int64_t
dt_aggregate_lquantizedzero(int64_t *lquanta)
{
int64_t arg = *lquanta++;
int32_t base = DTRACE_LQUANTIZE_BASE(arg);
uint16_t step = DTRACE_LQUANTIZE_STEP(arg);
uint16_t levels = DTRACE_LQUANTIZE_LEVELS(arg), i;
if (base - 1 == 0)
return (lquanta[0]);
for (i = 0; i < levels; base += step, i++) {
if (base != 0)
continue;
return (lquanta[i + 1]);
}
if (base + 1 == 0)
return (lquanta[levels + 1]);
return (0);
}
static int
dt_aggregate_lquantizedcmp(int64_t *lhs, int64_t *rhs)
{
long double lsum = dt_aggregate_lquantizedsum(lhs);
long double rsum = dt_aggregate_lquantizedsum(rhs);
int64_t lzero, rzero;
if (lsum < rsum)
return (DT_LESSTHAN);
if (lsum > rsum)
return (DT_GREATERTHAN);
lzero = dt_aggregate_lquantizedzero(lhs);
rzero = dt_aggregate_lquantizedzero(rhs);
if (lzero < rzero)
return (DT_LESSTHAN);
if (lzero > rzero)
return (DT_GREATERTHAN);
return (0);
}
static void
dt_aggregate_llquantize(int64_t *existing, int64_t *new, size_t size)
{
int i;
for (i = 1; i < size / sizeof (int64_t); i++)
existing[i] = existing[i] + new[i];
}
static long double
dt_aggregate_llquantizedsum(int64_t *llquanta)
{
int64_t arg = *llquanta++;
uint16_t factor = DTRACE_LLQUANTIZE_FACTOR(arg);
uint16_t low = DTRACE_LLQUANTIZE_LOW(arg);
uint16_t high = DTRACE_LLQUANTIZE_HIGH(arg);
uint16_t nsteps = DTRACE_LLQUANTIZE_NSTEP(arg);
int bin = 0, order;
int64_t value = 1, next, step;
long double total;
assert(nsteps >= factor);
assert(nsteps % factor == 0);
for (order = 0; order < low; order++)
value *= factor;
total = (long double)llquanta[bin++] * (long double)(value - 1);
next = value * factor;
step = next > nsteps ? next / nsteps : 1;
while (order <= high) {
assert(value < next);
total += (long double)llquanta[bin++] * (long double)(value);
if ((value += step) != next)
continue;
next = value * factor;
step = next > nsteps ? next / nsteps : 1;
order++;
}
return (total + (long double)llquanta[bin] * (long double)value);
}
static int
dt_aggregate_llquantizedcmp(int64_t *lhs, int64_t *rhs)
{
long double lsum = dt_aggregate_llquantizedsum(lhs);
long double rsum = dt_aggregate_llquantizedsum(rhs);
int64_t lzero, rzero;
if (lsum < rsum)
return (DT_LESSTHAN);
if (lsum > rsum)
return (DT_GREATERTHAN);
lzero = lhs[1];
rzero = rhs[1];
if (lzero < rzero)
return (DT_LESSTHAN);
if (lzero > rzero)
return (DT_GREATERTHAN);
return (0);
}
static int
dt_aggregate_quantizedcmp(int64_t *lhs, int64_t *rhs)
{
int nbuckets = DTRACE_QUANTIZE_NBUCKETS;
long double ltotal = 0, rtotal = 0;
int64_t lzero, rzero;
uint_t i;
for (i = 0; i < nbuckets; i++) {
int64_t bucketval = DTRACE_QUANTIZE_BUCKETVAL(i);
if (bucketval == 0) {
lzero = lhs[i];
rzero = rhs[i];
}
ltotal += (long double)bucketval * (long double)lhs[i];
rtotal += (long double)bucketval * (long double)rhs[i];
}
if (ltotal < rtotal)
return (DT_LESSTHAN);
if (ltotal > rtotal)
return (DT_GREATERTHAN);
if (lzero < rzero)
return (DT_LESSTHAN);
if (lzero > rzero)
return (DT_GREATERTHAN);
return (0);
}
static void
dt_aggregate_usym(dtrace_hdl_t *dtp, uint64_t *data)
{
uint64_t pid = data[0];
uint64_t *pc = &data[1];
struct ps_prochandle *P;
GElf_Sym sym;
if (dtp->dt_vector != NULL)
return;
if ((P = dt_proc_grab(dtp, pid, PGRAB_RDONLY | PGRAB_FORCE, 0)) == NULL)
return;
dt_proc_lock(dtp, P);
if (Plookup_by_addr(P, *pc, NULL, 0, &sym) == 0)
*pc = sym.st_value;
dt_proc_unlock(dtp, P);
dt_proc_release(dtp, P);
}
static void
dt_aggregate_umod(dtrace_hdl_t *dtp, uint64_t *data)
{
uint64_t pid = data[0];
uint64_t *pc = &data[1];
struct ps_prochandle *P;
const prmap_t *map;
if (dtp->dt_vector != NULL)
return;
if ((P = dt_proc_grab(dtp, pid, PGRAB_RDONLY | PGRAB_FORCE, 0)) == NULL)
return;
dt_proc_lock(dtp, P);
if ((map = Paddr_to_map(P, *pc)) != NULL)
*pc = map->pr_vaddr;
dt_proc_unlock(dtp, P);
dt_proc_release(dtp, P);
}
static void
dt_aggregate_sym(dtrace_hdl_t *dtp, uint64_t *data)
{
GElf_Sym sym;
uint64_t *pc = data;
if (dtrace_lookup_by_addr(dtp, *pc, &sym, NULL) == 0)
*pc = sym.st_value;
}
static void
dt_aggregate_mod(dtrace_hdl_t *dtp, uint64_t *data)
{
uint64_t *pc = data;
dt_module_t *dmp;
if (dtp->dt_vector != NULL) {
return;
}
for (dmp = dt_list_next(&dtp->dt_modlist); dmp != NULL;
dmp = dt_list_next(dmp)) {
if (*pc - dmp->dm_text_va < dmp->dm_text_size) {
*pc = dmp->dm_text_va;
return;
}
}
}
static dtrace_aggvarid_t
dt_aggregate_aggvarid(dt_ahashent_t *ent)
{
dtrace_aggdesc_t *agg = ent->dtahe_data.dtada_desc;
caddr_t data = ent->dtahe_data.dtada_data;
dtrace_recdesc_t *rec = agg->dtagd_rec;
if (agg->dtagd_varid != DTRACE_AGGVARIDNONE)
return (agg->dtagd_varid);
agg->dtagd_varid = *((dtrace_aggvarid_t *)(uintptr_t)(data +
rec->dtrd_offset));
return (agg->dtagd_varid);
}
static int
dt_aggregate_snap_cpu(dtrace_hdl_t *dtp, processorid_t cpu)
{
dtrace_epid_t id;
uint64_t hashval;
size_t offs, roffs, size, ndx;
int i, j, rval;
caddr_t addr, data;
dtrace_recdesc_t *rec;
dt_aggregate_t *agp = &dtp->dt_aggregate;
dtrace_aggdesc_t *agg;
dt_ahash_t *hash = &agp->dtat_hash;
dt_ahashent_t *h;
dtrace_bufdesc_t b = agp->dtat_buf, *buf = &b;
dtrace_aggdata_t *aggdata;
int flags = agp->dtat_flags;
buf->dtbd_cpu = cpu;
#if defined(illumos)
if (dt_ioctl(dtp, DTRACEIOC_AGGSNAP, buf) == -1) {
#else
if (dt_ioctl(dtp, DTRACEIOC_AGGSNAP, &buf) == -1) {
#endif
if (errno == ENOENT) {
return (0);
}
return (dt_set_errno(dtp, errno));
}
if (buf->dtbd_drops != 0) {
if (dt_handle_cpudrop(dtp, cpu,
DTRACEDROP_AGGREGATION, buf->dtbd_drops) == -1)
return (-1);
}
if (buf->dtbd_size == 0)
return (0);
if (hash->dtah_hash == NULL) {
size_t size;
hash->dtah_size = DTRACE_AHASHSIZE;
size = hash->dtah_size * sizeof (dt_ahashent_t *);
if ((hash->dtah_hash = malloc(size)) == NULL)
return (dt_set_errno(dtp, EDT_NOMEM));
bzero(hash->dtah_hash, size);
}
for (offs = 0; offs < buf->dtbd_size; ) {
id = *((dtrace_epid_t *)((uintptr_t)buf->dtbd_data +
(uintptr_t)offs));
if (id == DTRACE_AGGIDNONE) {
offs += sizeof (id);
continue;
}
if ((rval = dt_aggid_lookup(dtp, id, &agg)) != 0)
return (rval);
addr = buf->dtbd_data + offs;
size = agg->dtagd_size;
hashval = 0;
for (j = 0; j < agg->dtagd_nrecs - 1; j++) {
rec = &agg->dtagd_rec[j];
roffs = rec->dtrd_offset;
switch (rec->dtrd_action) {
case DTRACEACT_USYM:
dt_aggregate_usym(dtp,
(uint64_t *)&addr[roffs]);
break;
case DTRACEACT_UMOD:
dt_aggregate_umod(dtp,
(uint64_t *)&addr[roffs]);
break;
case DTRACEACT_SYM:
dt_aggregate_sym(dtp, (uint64_t *)&addr[roffs]);
break;
case DTRACEACT_MOD:
dt_aggregate_mod(dtp, (uint64_t *)&addr[roffs]);
break;
default:
break;
}
for (i = 0; i < rec->dtrd_size; i++)
hashval += addr[roffs + i];
}
ndx = hashval % hash->dtah_size;
for (h = hash->dtah_hash[ndx]; h != NULL; h = h->dtahe_next) {
if (h->dtahe_hashval != hashval)
continue;
if (h->dtahe_size != size)
continue;
aggdata = &h->dtahe_data;
data = aggdata->dtada_data;
for (j = 0; j < agg->dtagd_nrecs - 1; j++) {
rec = &agg->dtagd_rec[j];
roffs = rec->dtrd_offset;
for (i = 0; i < rec->dtrd_size; i++)
if (addr[roffs + i] != data[roffs + i])
goto hashnext;
}
rec = &agg->dtagd_rec[agg->dtagd_nrecs - 1];
roffs = rec->dtrd_offset;
h->dtahe_aggregate((int64_t *)&data[roffs],
(int64_t *)&addr[roffs], rec->dtrd_size);
if (aggdata->dtada_percpu != NULL) {
data = aggdata->dtada_percpu[cpu];
h->dtahe_aggregate((int64_t *)data,
(int64_t *)&addr[roffs], rec->dtrd_size);
}
goto bufnext;
hashnext:
continue;
}
if ((h = malloc(sizeof (dt_ahashent_t))) == NULL)
return (dt_set_errno(dtp, EDT_NOMEM));
bzero(h, sizeof (dt_ahashent_t));
aggdata = &h->dtahe_data;
if ((aggdata->dtada_data = malloc(size)) == NULL) {
free(h);
return (dt_set_errno(dtp, EDT_NOMEM));
}
bcopy(addr, aggdata->dtada_data, size);
aggdata->dtada_size = size;
aggdata->dtada_desc = agg;
aggdata->dtada_handle = dtp;
(void) dt_epid_lookup(dtp, agg->dtagd_epid,
&aggdata->dtada_edesc, &aggdata->dtada_pdesc);
aggdata->dtada_normal = 1;
h->dtahe_hashval = hashval;
h->dtahe_size = size;
(void) dt_aggregate_aggvarid(h);
rec = &agg->dtagd_rec[agg->dtagd_nrecs - 1];
if (flags & DTRACE_A_PERCPU) {
int max_cpus = agp->dtat_maxcpu;
caddr_t *percpu = malloc(max_cpus * sizeof (caddr_t));
if (percpu == NULL) {
free(aggdata->dtada_data);
free(h);
return (dt_set_errno(dtp, EDT_NOMEM));
}
for (j = 0; j < max_cpus; j++) {
percpu[j] = malloc(rec->dtrd_size);
if (percpu[j] == NULL) {
while (--j >= 0)
free(percpu[j]);
free(aggdata->dtada_data);
free(h);
return (dt_set_errno(dtp, EDT_NOMEM));
}
if (j == cpu) {
bcopy(&addr[rec->dtrd_offset],
percpu[j], rec->dtrd_size);
} else {
bzero(percpu[j], rec->dtrd_size);
}
}
aggdata->dtada_percpu = percpu;
}
switch (rec->dtrd_action) {
case DTRACEAGG_MIN:
h->dtahe_aggregate = dt_aggregate_min;
break;
case DTRACEAGG_MAX:
h->dtahe_aggregate = dt_aggregate_max;
break;
case DTRACEAGG_LQUANTIZE:
h->dtahe_aggregate = dt_aggregate_lquantize;
break;
case DTRACEAGG_LLQUANTIZE:
h->dtahe_aggregate = dt_aggregate_llquantize;
break;
case DTRACEAGG_COUNT:
case DTRACEAGG_SUM:
case DTRACEAGG_AVG:
case DTRACEAGG_STDDEV:
case DTRACEAGG_QUANTIZE:
h->dtahe_aggregate = dt_aggregate_count;
break;
default:
return (dt_set_errno(dtp, EDT_BADAGG));
}
if (hash->dtah_hash[ndx] != NULL)
hash->dtah_hash[ndx]->dtahe_prev = h;
h->dtahe_next = hash->dtah_hash[ndx];
hash->dtah_hash[ndx] = h;
if (hash->dtah_all != NULL)
hash->dtah_all->dtahe_prevall = h;
h->dtahe_nextall = hash->dtah_all;
hash->dtah_all = h;
bufnext:
offs += agg->dtagd_size;
}
return (0);
}
int
dtrace_aggregate_snap(dtrace_hdl_t *dtp)
{
int i, rval;
dt_aggregate_t *agp = &dtp->dt_aggregate;
hrtime_t now = gethrtime();
dtrace_optval_t interval = dtp->dt_options[DTRACEOPT_AGGRATE];
if (dtp->dt_lastagg != 0) {
if (now - dtp->dt_lastagg < interval)
return (0);
dtp->dt_lastagg += interval;
} else {
dtp->dt_lastagg = now;
}
if (!dtp->dt_active)
return (dt_set_errno(dtp, EINVAL));
if (agp->dtat_buf.dtbd_size == 0)
return (0);
for (i = 0; i < agp->dtat_ncpus; i++) {
if ((rval = dt_aggregate_snap_cpu(dtp, agp->dtat_cpus[i])))
return (rval);
}
return (0);
}
static int
dt_aggregate_hashcmp(const void *lhs, const void *rhs)
{
dt_ahashent_t *lh = *((dt_ahashent_t **)lhs);
dt_ahashent_t *rh = *((dt_ahashent_t **)rhs);
dtrace_aggdesc_t *lagg = lh->dtahe_data.dtada_desc;
dtrace_aggdesc_t *ragg = rh->dtahe_data.dtada_desc;
if (lagg->dtagd_nrecs < ragg->dtagd_nrecs)
return (DT_LESSTHAN);
if (lagg->dtagd_nrecs > ragg->dtagd_nrecs)
return (DT_GREATERTHAN);
return (0);
}
static int
dt_aggregate_varcmp(const void *lhs, const void *rhs)
{
dt_ahashent_t *lh = *((dt_ahashent_t **)lhs);
dt_ahashent_t *rh = *((dt_ahashent_t **)rhs);
dtrace_aggvarid_t lid, rid;
lid = dt_aggregate_aggvarid(lh);
rid = dt_aggregate_aggvarid(rh);
if (lid < rid)
return (DT_LESSTHAN);
if (lid > rid)
return (DT_GREATERTHAN);
return (0);
}
static int
dt_aggregate_keycmp(const void *lhs, const void *rhs)
{
dt_ahashent_t *lh = *((dt_ahashent_t **)lhs);
dt_ahashent_t *rh = *((dt_ahashent_t **)rhs);
dtrace_aggdesc_t *lagg = lh->dtahe_data.dtada_desc;
dtrace_aggdesc_t *ragg = rh->dtahe_data.dtada_desc;
dtrace_recdesc_t *lrec, *rrec;
char *ldata, *rdata;
int rval, i, j, keypos, nrecs;
if ((rval = dt_aggregate_hashcmp(lhs, rhs)) != 0)
return (rval);
nrecs = lagg->dtagd_nrecs - 1;
assert(nrecs == ragg->dtagd_nrecs - 1);
keypos = dt_keypos + 1 >= nrecs ? 0 : dt_keypos;
for (i = 1; i < nrecs; i++) {
uint64_t lval, rval;
int ndx = i + keypos;
if (ndx >= nrecs)
ndx = ndx - nrecs + 1;
lrec = &lagg->dtagd_rec[ndx];
rrec = &ragg->dtagd_rec[ndx];
ldata = lh->dtahe_data.dtada_data + lrec->dtrd_offset;
rdata = rh->dtahe_data.dtada_data + rrec->dtrd_offset;
if (lrec->dtrd_size < rrec->dtrd_size)
return (DT_LESSTHAN);
if (lrec->dtrd_size > rrec->dtrd_size)
return (DT_GREATERTHAN);
switch (lrec->dtrd_size) {
case sizeof (uint64_t):
lval = *((uint64_t *)ldata);
rval = *((uint64_t *)rdata);
break;
case sizeof (uint32_t):
lval = *((uint32_t *)ldata);
rval = *((uint32_t *)rdata);
break;
case sizeof (uint16_t):
lval = *((uint16_t *)ldata);
rval = *((uint16_t *)rdata);
break;
case sizeof (uint8_t):
lval = *((uint8_t *)ldata);
rval = *((uint8_t *)rdata);
break;
default:
switch (lrec->dtrd_action) {
case DTRACEACT_UMOD:
case DTRACEACT_UADDR:
case DTRACEACT_USYM:
for (j = 0; j < 2; j++) {
lval = ((uint64_t *)ldata)[j];
rval = ((uint64_t *)rdata)[j];
if (lval < rval)
return (DT_LESSTHAN);
if (lval > rval)
return (DT_GREATERTHAN);
}
break;
default:
for (j = 0; j < lrec->dtrd_size; j++) {
lval = ((uint8_t *)ldata)[j];
rval = ((uint8_t *)rdata)[j];
if (lval < rval)
return (DT_LESSTHAN);
if (lval > rval)
return (DT_GREATERTHAN);
}
}
continue;
}
if (lval < rval)
return (DT_LESSTHAN);
if (lval > rval)
return (DT_GREATERTHAN);
}
return (0);
}
static int
dt_aggregate_valcmp(const void *lhs, const void *rhs)
{
dt_ahashent_t *lh = *((dt_ahashent_t **)lhs);
dt_ahashent_t *rh = *((dt_ahashent_t **)rhs);
dtrace_aggdesc_t *lagg = lh->dtahe_data.dtada_desc;
dtrace_aggdesc_t *ragg = rh->dtahe_data.dtada_desc;
caddr_t ldata = lh->dtahe_data.dtada_data;
caddr_t rdata = rh->dtahe_data.dtada_data;
dtrace_recdesc_t *lrec, *rrec;
int64_t *laddr, *raddr;
int rval;
assert(lagg->dtagd_nrecs == ragg->dtagd_nrecs);
lrec = &lagg->dtagd_rec[lagg->dtagd_nrecs - 1];
rrec = &ragg->dtagd_rec[ragg->dtagd_nrecs - 1];
assert(lrec->dtrd_action == rrec->dtrd_action);
laddr = (int64_t *)(uintptr_t)(ldata + lrec->dtrd_offset);
raddr = (int64_t *)(uintptr_t)(rdata + rrec->dtrd_offset);
switch (lrec->dtrd_action) {
case DTRACEAGG_AVG:
rval = dt_aggregate_averagecmp(laddr, raddr);
break;
case DTRACEAGG_STDDEV:
rval = dt_aggregate_stddevcmp(laddr, raddr);
break;
case DTRACEAGG_QUANTIZE:
rval = dt_aggregate_quantizedcmp(laddr, raddr);
break;
case DTRACEAGG_LQUANTIZE:
rval = dt_aggregate_lquantizedcmp(laddr, raddr);
break;
case DTRACEAGG_LLQUANTIZE:
rval = dt_aggregate_llquantizedcmp(laddr, raddr);
break;
case DTRACEAGG_COUNT:
case DTRACEAGG_SUM:
case DTRACEAGG_MIN:
case DTRACEAGG_MAX:
rval = dt_aggregate_countcmp(laddr, raddr);
break;
default:
assert(0);
}
return (rval);
}
static int
dt_aggregate_valkeycmp(const void *lhs, const void *rhs)
{
int rval;
if ((rval = dt_aggregate_valcmp(lhs, rhs)) != 0)
return (rval);
return (dt_aggregate_keycmp(lhs, rhs));
}
static int
dt_aggregate_keyvarcmp(const void *lhs, const void *rhs)
{
int rval;
if ((rval = dt_aggregate_keycmp(lhs, rhs)) != 0)
return (rval);
return (dt_aggregate_varcmp(lhs, rhs));
}
static int
dt_aggregate_varkeycmp(const void *lhs, const void *rhs)
{
int rval;
if ((rval = dt_aggregate_varcmp(lhs, rhs)) != 0)
return (rval);
return (dt_aggregate_keycmp(lhs, rhs));
}
static int
dt_aggregate_valvarcmp(const void *lhs, const void *rhs)
{
int rval;
if ((rval = dt_aggregate_valkeycmp(lhs, rhs)) != 0)
return (rval);
return (dt_aggregate_varcmp(lhs, rhs));
}
static int
dt_aggregate_varvalcmp(const void *lhs, const void *rhs)
{
int rval;
if ((rval = dt_aggregate_varcmp(lhs, rhs)) != 0)
return (rval);
return (dt_aggregate_valkeycmp(lhs, rhs));
}
static int
dt_aggregate_keyvarrevcmp(const void *lhs, const void *rhs)
{
return (dt_aggregate_keyvarcmp(rhs, lhs));
}
static int
dt_aggregate_varkeyrevcmp(const void *lhs, const void *rhs)
{
return (dt_aggregate_varkeycmp(rhs, lhs));
}
static int
dt_aggregate_valvarrevcmp(const void *lhs, const void *rhs)
{
return (dt_aggregate_valvarcmp(rhs, lhs));
}
static int
dt_aggregate_varvalrevcmp(const void *lhs, const void *rhs)
{
return (dt_aggregate_varvalcmp(rhs, lhs));
}
static int
dt_aggregate_bundlecmp(const void *lhs, const void *rhs)
{
dt_ahashent_t **lh = *((dt_ahashent_t ***)lhs);
dt_ahashent_t **rh = *((dt_ahashent_t ***)rhs);
int i, rval;
if (dt_keysort) {
for (i = 0; lh[i + 1] != NULL; i++)
continue;
assert(i != 0);
assert(rh[i + 1] == NULL);
if ((rval = dt_aggregate_keycmp(&lh[i], &rh[i])) != 0)
return (rval);
}
for (i = 0; ; i++) {
if (lh[i + 1] == NULL) {
if (dt_keysort)
return (0);
assert(i != 0);
assert(rh[i + 1] == NULL);
return (dt_aggregate_keycmp(&lh[i], &rh[i]));
} else {
if ((rval = dt_aggregate_valcmp(&lh[i], &rh[i])) != 0)
return (rval);
}
}
}
int
dt_aggregate_go(dtrace_hdl_t *dtp)
{
dt_aggregate_t *agp = &dtp->dt_aggregate;
dtrace_optval_t size, cpu;
dtrace_bufdesc_t *buf = &agp->dtat_buf;
int rval, i;
assert(agp->dtat_maxcpu == 0);
assert(agp->dtat_ncpu == 0);
assert(agp->dtat_cpus == NULL);
agp->dtat_maxcpu = dt_sysconf(dtp, _SC_CPUID_MAX) + 1;
agp->dtat_ncpu = dt_sysconf(dtp, _SC_NPROCESSORS_MAX);
agp->dtat_cpus = malloc(agp->dtat_ncpu * sizeof (processorid_t));
if (agp->dtat_cpus == NULL)
return (dt_set_errno(dtp, EDT_NOMEM));
size = dtp->dt_options[DTRACEOPT_AGGSIZE];
rval = dtrace_getopt(dtp, "aggsize", &size);
assert(rval == 0);
if (size == 0 || size == DTRACEOPT_UNSET)
return (0);
buf = &agp->dtat_buf;
buf->dtbd_size = size;
if ((buf->dtbd_data = malloc(buf->dtbd_size)) == NULL)
return (dt_set_errno(dtp, EDT_NOMEM));
rval = dtrace_getopt(dtp, "cpu", &cpu);
assert(rval == 0 && cpu != DTRACEOPT_UNSET);
if (cpu != DTRACE_CPUALL) {
assert(cpu < agp->dtat_ncpu);
agp->dtat_cpus[agp->dtat_ncpus++] = (processorid_t)cpu;
return (0);
}
agp->dtat_ncpus = 0;
for (i = 0; i < agp->dtat_maxcpu; i++) {
if (dt_status(dtp, i) == -1)
continue;
agp->dtat_cpus[agp->dtat_ncpus++] = i;
}
return (0);
}
static int
dt_aggwalk_rval(dtrace_hdl_t *dtp, dt_ahashent_t *h, int rval)
{
dt_aggregate_t *agp = &dtp->dt_aggregate;
dtrace_aggdata_t *data;
dtrace_aggdesc_t *aggdesc;
dtrace_recdesc_t *rec;
int i;
switch (rval) {
case DTRACE_AGGWALK_NEXT:
break;
case DTRACE_AGGWALK_CLEAR: {
uint32_t size, offs = 0;
aggdesc = h->dtahe_data.dtada_desc;
rec = &aggdesc->dtagd_rec[aggdesc->dtagd_nrecs - 1];
size = rec->dtrd_size;
data = &h->dtahe_data;
if (rec->dtrd_action == DTRACEAGG_LQUANTIZE) {
offs = sizeof (uint64_t);
size -= sizeof (uint64_t);
}
bzero(&data->dtada_data[rec->dtrd_offset] + offs, size);
if (data->dtada_percpu == NULL)
break;
for (i = 0; i < dtp->dt_aggregate.dtat_maxcpu; i++)
bzero(data->dtada_percpu[i] + offs, size);
break;
}
case DTRACE_AGGWALK_ERROR:
return (dt_set_errno(dtp, errno));
case DTRACE_AGGWALK_ABORT:
return (dt_set_errno(dtp, EDT_DIRABORT));
case DTRACE_AGGWALK_DENORMALIZE:
h->dtahe_data.dtada_normal = 1;
return (0);
case DTRACE_AGGWALK_NORMALIZE:
if (h->dtahe_data.dtada_normal == 0) {
h->dtahe_data.dtada_normal = 1;
return (dt_set_errno(dtp, EDT_BADRVAL));
}
return (0);
case DTRACE_AGGWALK_REMOVE: {
dtrace_aggdata_t *aggdata = &h->dtahe_data;
int max_cpus = agp->dtat_maxcpu;
if (h->dtahe_prev != NULL) {
h->dtahe_prev->dtahe_next = h->dtahe_next;
} else {
dt_ahash_t *hash = &agp->dtat_hash;
size_t ndx = h->dtahe_hashval % hash->dtah_size;
assert(hash->dtah_hash[ndx] == h);
hash->dtah_hash[ndx] = h->dtahe_next;
}
if (h->dtahe_next != NULL)
h->dtahe_next->dtahe_prev = h->dtahe_prev;
if (h->dtahe_prevall != NULL) {
h->dtahe_prevall->dtahe_nextall = h->dtahe_nextall;
} else {
dt_ahash_t *hash = &agp->dtat_hash;
assert(hash->dtah_all == h);
hash->dtah_all = h->dtahe_nextall;
}
if (h->dtahe_nextall != NULL)
h->dtahe_nextall->dtahe_prevall = h->dtahe_prevall;
if (aggdata->dtada_percpu != NULL) {
for (i = 0; i < max_cpus; i++)
free(aggdata->dtada_percpu[i]);
free(aggdata->dtada_percpu);
}
free(aggdata->dtada_data);
free(h);
return (0);
}
default:
return (dt_set_errno(dtp, EDT_BADRVAL));
}
return (0);
}
void
dt_aggregate_qsort(dtrace_hdl_t *dtp, void *base, size_t nel, size_t width,
int (*compar)(const void *, const void *))
{
int rev = dt_revsort, key = dt_keysort, keypos = dt_keypos;
dtrace_optval_t keyposopt = dtp->dt_options[DTRACEOPT_AGGSORTKEYPOS];
dt_revsort = (dtp->dt_options[DTRACEOPT_AGGSORTREV] != DTRACEOPT_UNSET);
dt_keysort = (dtp->dt_options[DTRACEOPT_AGGSORTKEY] != DTRACEOPT_UNSET);
if (keyposopt != DTRACEOPT_UNSET && keyposopt <= INT_MAX) {
dt_keypos = (int)keyposopt;
} else {
dt_keypos = 0;
}
if (compar == NULL) {
if (!dt_keysort) {
compar = dt_aggregate_varvalcmp;
} else {
compar = dt_aggregate_varkeycmp;
}
}
qsort(base, nel, width, compar);
dt_revsort = rev;
dt_keysort = key;
dt_keypos = keypos;
}
int
dtrace_aggregate_walk(dtrace_hdl_t *dtp, dtrace_aggregate_f *func, void *arg)
{
dt_ahashent_t *h, *next;
dt_ahash_t *hash = &dtp->dt_aggregate.dtat_hash;
for (h = hash->dtah_all; h != NULL; h = next) {
next = h->dtahe_nextall;
if (dt_aggwalk_rval(dtp, h, func(&h->dtahe_data, arg)) == -1)
return (-1);
}
return (0);
}
static int
dt_aggregate_total(dtrace_hdl_t *dtp, boolean_t clear)
{
dt_ahashent_t *h;
dtrace_aggdata_t **total;
dtrace_aggid_t max = DTRACE_AGGVARIDNONE, id;
dt_aggregate_t *agp = &dtp->dt_aggregate;
dt_ahash_t *hash = &agp->dtat_hash;
uint32_t tflags;
tflags = DTRACE_A_TOTAL | DTRACE_A_HASNEGATIVES | DTRACE_A_HASPOSITIVES;
for (h = hash->dtah_all; h != NULL; h = h->dtahe_nextall) {
dtrace_aggdata_t *aggdata = &h->dtahe_data;
if ((id = dt_aggregate_aggvarid(h)) > max)
max = id;
aggdata->dtada_total = 0;
aggdata->dtada_flags &= ~tflags;
}
if (clear || max == DTRACE_AGGVARIDNONE)
return (0);
total = dt_zalloc(dtp, (max + 1) * sizeof (dtrace_aggdata_t *));
if (total == NULL)
return (-1);
for (h = hash->dtah_all; h != NULL; h = h->dtahe_nextall) {
dtrace_aggdata_t *aggdata = &h->dtahe_data;
dtrace_aggdesc_t *agg = aggdata->dtada_desc;
dtrace_recdesc_t *rec;
caddr_t data;
int64_t val, *addr;
rec = &agg->dtagd_rec[agg->dtagd_nrecs - 1];
data = aggdata->dtada_data;
addr = (int64_t *)(uintptr_t)(data + rec->dtrd_offset);
switch (rec->dtrd_action) {
case DTRACEAGG_STDDEV:
val = dt_stddev((uint64_t *)addr, 1);
break;
case DTRACEAGG_SUM:
case DTRACEAGG_COUNT:
val = *addr;
break;
case DTRACEAGG_AVG:
val = addr[0] ? (addr[1] / addr[0]) : 0;
break;
default:
continue;
}
if (total[agg->dtagd_varid] == NULL) {
total[agg->dtagd_varid] = aggdata;
aggdata->dtada_flags |= DTRACE_A_TOTAL;
} else {
aggdata = total[agg->dtagd_varid];
}
if (val > 0)
aggdata->dtada_flags |= DTRACE_A_HASPOSITIVES;
if (val < 0) {
aggdata->dtada_flags |= DTRACE_A_HASNEGATIVES;
val = -val;
}
if (dtp->dt_options[DTRACEOPT_AGGZOOM] != DTRACEOPT_UNSET) {
val = (int64_t)((long double)val *
(1 / DTRACE_AGGZOOM_MAX));
if (val > aggdata->dtada_total)
aggdata->dtada_total = val;
} else {
aggdata->dtada_total += val;
}
}
for (h = hash->dtah_all; h != NULL; h = h->dtahe_nextall) {
dtrace_aggdata_t *aggdata = &h->dtahe_data, *t;
dtrace_aggdesc_t *agg = aggdata->dtada_desc;
if ((t = total[agg->dtagd_varid]) == NULL || aggdata == t)
continue;
aggdata->dtada_total = t->dtada_total;
aggdata->dtada_flags |= (t->dtada_flags & tflags);
}
dt_free(dtp, total);
return (0);
}
static int
dt_aggregate_minmaxbin(dtrace_hdl_t *dtp, boolean_t clear)
{
dt_ahashent_t *h;
dtrace_aggdata_t **minmax;
dtrace_aggid_t max = DTRACE_AGGVARIDNONE, id;
dt_aggregate_t *agp = &dtp->dt_aggregate;
dt_ahash_t *hash = &agp->dtat_hash;
for (h = hash->dtah_all; h != NULL; h = h->dtahe_nextall) {
dtrace_aggdata_t *aggdata = &h->dtahe_data;
if ((id = dt_aggregate_aggvarid(h)) > max)
max = id;
aggdata->dtada_minbin = 0;
aggdata->dtada_maxbin = 0;
aggdata->dtada_flags &= ~DTRACE_A_MINMAXBIN;
}
if (clear || max == DTRACE_AGGVARIDNONE)
return (0);
minmax = dt_zalloc(dtp, (max + 1) * sizeof (dtrace_aggdata_t *));
if (minmax == NULL)
return (-1);
for (h = hash->dtah_all; h != NULL; h = h->dtahe_nextall) {
dtrace_aggdata_t *aggdata = &h->dtahe_data;
dtrace_aggdesc_t *agg = aggdata->dtada_desc;
dtrace_recdesc_t *rec;
caddr_t data;
int64_t *addr;
int minbin = -1, maxbin = -1, i;
int start = 0, size;
rec = &agg->dtagd_rec[agg->dtagd_nrecs - 1];
size = rec->dtrd_size / sizeof (int64_t);
data = aggdata->dtada_data;
addr = (int64_t *)(uintptr_t)(data + rec->dtrd_offset);
switch (rec->dtrd_action) {
case DTRACEAGG_LQUANTIZE:
start = 1;
minbin = start;
maxbin = size - 1 - start;
break;
case DTRACEAGG_QUANTIZE:
for (i = start; i < size; i++) {
if (!addr[i])
continue;
if (minbin == -1)
minbin = i - start;
maxbin = i - start;
}
if (minbin == -1) {
minbin = maxbin = DTRACE_QUANTIZE_ZEROBUCKET;
}
break;
default:
continue;
}
if (minmax[agg->dtagd_varid] == NULL) {
minmax[agg->dtagd_varid] = aggdata;
aggdata->dtada_flags |= DTRACE_A_MINMAXBIN;
aggdata->dtada_minbin = minbin;
aggdata->dtada_maxbin = maxbin;
continue;
}
if (minbin < minmax[agg->dtagd_varid]->dtada_minbin)
minmax[agg->dtagd_varid]->dtada_minbin = minbin;
if (maxbin > minmax[agg->dtagd_varid]->dtada_maxbin)
minmax[agg->dtagd_varid]->dtada_maxbin = maxbin;
}
for (h = hash->dtah_all; h != NULL; h = h->dtahe_nextall) {
dtrace_aggdata_t *aggdata = &h->dtahe_data, *mm;
dtrace_aggdesc_t *agg = aggdata->dtada_desc;
if ((mm = minmax[agg->dtagd_varid]) == NULL || aggdata == mm)
continue;
aggdata->dtada_minbin = mm->dtada_minbin;
aggdata->dtada_maxbin = mm->dtada_maxbin;
aggdata->dtada_flags |= DTRACE_A_MINMAXBIN;
}
dt_free(dtp, minmax);
return (0);
}
static int
dt_aggregate_walk_sorted(dtrace_hdl_t *dtp,
dtrace_aggregate_f *func, void *arg,
int (*sfunc)(const void *, const void *))
{
dt_aggregate_t *agp = &dtp->dt_aggregate;
dt_ahashent_t *h, **sorted;
dt_ahash_t *hash = &agp->dtat_hash;
size_t i, nentries = 0;
int rval = -1;
agp->dtat_flags &= ~(DTRACE_A_TOTAL | DTRACE_A_MINMAXBIN);
if (dtp->dt_options[DTRACEOPT_AGGHIST] != DTRACEOPT_UNSET) {
agp->dtat_flags |= DTRACE_A_TOTAL;
if (dt_aggregate_total(dtp, B_FALSE) != 0)
return (-1);
}
if (dtp->dt_options[DTRACEOPT_AGGPACK] != DTRACEOPT_UNSET) {
agp->dtat_flags |= DTRACE_A_MINMAXBIN;
if (dt_aggregate_minmaxbin(dtp, B_FALSE) != 0)
return (-1);
}
for (h = hash->dtah_all; h != NULL; h = h->dtahe_nextall)
nentries++;
sorted = dt_alloc(dtp, nentries * sizeof (dt_ahashent_t *));
if (sorted == NULL)
goto out;
for (h = hash->dtah_all, i = 0; h != NULL; h = h->dtahe_nextall)
sorted[i++] = h;
(void) pthread_mutex_lock(&dt_qsort_lock);
if (sfunc == NULL) {
dt_aggregate_qsort(dtp, sorted, nentries,
sizeof (dt_ahashent_t *), NULL);
} else {
qsort(sorted, nentries, sizeof (dt_ahashent_t *), sfunc);
}
(void) pthread_mutex_unlock(&dt_qsort_lock);
for (i = 0; i < nentries; i++) {
h = sorted[i];
if (dt_aggwalk_rval(dtp, h, func(&h->dtahe_data, arg)) == -1)
goto out;
}
rval = 0;
out:
if (agp->dtat_flags & DTRACE_A_TOTAL)
(void) dt_aggregate_total(dtp, B_TRUE);
if (agp->dtat_flags & DTRACE_A_MINMAXBIN)
(void) dt_aggregate_minmaxbin(dtp, B_TRUE);
dt_free(dtp, sorted);
return (rval);
}
int
dtrace_aggregate_walk_sorted(dtrace_hdl_t *dtp,
dtrace_aggregate_f *func, void *arg)
{
return (dt_aggregate_walk_sorted(dtp, func, arg, NULL));
}
int
dtrace_aggregate_walk_keysorted(dtrace_hdl_t *dtp,
dtrace_aggregate_f *func, void *arg)
{
return (dt_aggregate_walk_sorted(dtp, func,
arg, dt_aggregate_varkeycmp));
}
int
dtrace_aggregate_walk_valsorted(dtrace_hdl_t *dtp,
dtrace_aggregate_f *func, void *arg)
{
return (dt_aggregate_walk_sorted(dtp, func,
arg, dt_aggregate_varvalcmp));
}
int
dtrace_aggregate_walk_keyvarsorted(dtrace_hdl_t *dtp,
dtrace_aggregate_f *func, void *arg)
{
return (dt_aggregate_walk_sorted(dtp, func,
arg, dt_aggregate_keyvarcmp));
}
int
dtrace_aggregate_walk_valvarsorted(dtrace_hdl_t *dtp,
dtrace_aggregate_f *func, void *arg)
{
return (dt_aggregate_walk_sorted(dtp, func,
arg, dt_aggregate_valvarcmp));
}
int
dtrace_aggregate_walk_keyrevsorted(dtrace_hdl_t *dtp,
dtrace_aggregate_f *func, void *arg)
{
return (dt_aggregate_walk_sorted(dtp, func,
arg, dt_aggregate_varkeyrevcmp));
}
int
dtrace_aggregate_walk_valrevsorted(dtrace_hdl_t *dtp,
dtrace_aggregate_f *func, void *arg)
{
return (dt_aggregate_walk_sorted(dtp, func,
arg, dt_aggregate_varvalrevcmp));
}
int
dtrace_aggregate_walk_keyvarrevsorted(dtrace_hdl_t *dtp,
dtrace_aggregate_f *func, void *arg)
{
return (dt_aggregate_walk_sorted(dtp, func,
arg, dt_aggregate_keyvarrevcmp));
}
int
dtrace_aggregate_walk_valvarrevsorted(dtrace_hdl_t *dtp,
dtrace_aggregate_f *func, void *arg)
{
return (dt_aggregate_walk_sorted(dtp, func,
arg, dt_aggregate_valvarrevcmp));
}
int
dtrace_aggregate_walk_joined(dtrace_hdl_t *dtp, dtrace_aggvarid_t *aggvars,
int naggvars, dtrace_aggregate_walk_joined_f *func, void *arg)
{
dt_aggregate_t *agp = &dtp->dt_aggregate;
dt_ahashent_t *h, **sorted = NULL, ***bundle, **nbundle;
const dtrace_aggdata_t **data;
dt_ahashent_t *zaggdata = NULL;
dt_ahash_t *hash = &agp->dtat_hash;
size_t nentries = 0, nbundles = 0, start, zsize = 0, bundlesize;
dtrace_aggvarid_t max = 0, aggvar;
int rval = -1, *map, *remap = NULL;
int i, j;
dtrace_optval_t sortpos = dtp->dt_options[DTRACEOPT_AGGSORTPOS];
if (sortpos == DTRACEOPT_UNSET || sortpos >= naggvars)
sortpos = 0;
for (i = 0; i < naggvars; i++) {
if (aggvars[i] == DTRACE_AGGVARIDNONE || aggvars[i] < 0)
return (dt_set_errno(dtp, EDT_BADAGGVAR));
if (aggvars[i] > max)
max = aggvars[i];
}
if ((map = dt_zalloc(dtp, (max + 1) * sizeof (int))) == NULL)
return (-1);
zaggdata = dt_zalloc(dtp, naggvars * sizeof (dt_ahashent_t));
if (zaggdata == NULL)
goto out;
for (i = 0; i < naggvars; i++) {
int ndx = i + sortpos;
if (ndx >= naggvars)
ndx -= naggvars;
aggvar = aggvars[ndx];
assert(aggvar <= max);
if (map[aggvar]) {
if (remap == NULL) {
remap = dt_zalloc(dtp, naggvars * sizeof (int));
if (remap == NULL)
goto out;
}
assert(aggvars[(map[aggvar] - 1 + sortpos) %
naggvars] == aggvars[ndx]);
remap[i] = map[aggvar];
continue;
}
map[aggvar] = i + 1;
}
for (h = hash->dtah_all; h != NULL; h = h->dtahe_nextall) {
dtrace_aggvarid_t id;
int ndx;
if ((id = dt_aggregate_aggvarid(h)) > max || !(ndx = map[id]))
continue;
if (zaggdata[ndx - 1].dtahe_size == 0) {
zaggdata[ndx - 1].dtahe_size = h->dtahe_size;
zaggdata[ndx - 1].dtahe_data = h->dtahe_data;
}
nentries++;
}
if (nentries == 0) {
rval = 0;
goto out;
}
for (i = 0; i < naggvars; i++) {
if (zaggdata[i].dtahe_size == 0) {
dtrace_aggvarid_t aggvar;
aggvar = aggvars[(i - sortpos + naggvars) % naggvars];
assert(zaggdata[i].dtahe_data.dtada_data == NULL);
for (j = DTRACE_AGGIDNONE + 1; ; j++) {
dtrace_aggdesc_t *agg;
dtrace_aggdata_t *aggdata;
if (dt_aggid_lookup(dtp, j, &agg) != 0)
break;
if (agg->dtagd_varid != aggvar)
continue;
aggdata = &zaggdata[i].dtahe_data;
aggdata->dtada_size = agg->dtagd_size;
aggdata->dtada_desc = agg;
aggdata->dtada_handle = dtp;
(void) dt_epid_lookup(dtp, agg->dtagd_epid,
&aggdata->dtada_edesc,
&aggdata->dtada_pdesc);
aggdata->dtada_normal = 1;
zaggdata[i].dtahe_hashval = 0;
zaggdata[i].dtahe_size = agg->dtagd_size;
break;
}
if (zaggdata[i].dtahe_size == 0) {
caddr_t data;
for (j = 0; j < naggvars; j++) {
if (zaggdata[j].dtahe_size != 0)
break;
}
assert(j < naggvars);
zaggdata[i] = zaggdata[j];
data = zaggdata[i].dtahe_data.dtada_data;
assert(data != NULL);
}
}
}
for (i = 0; i < naggvars; i++) {
dtrace_aggdata_t *aggdata = &zaggdata[i].dtahe_data;
dtrace_aggdesc_t *aggdesc = aggdata->dtada_desc;
dtrace_recdesc_t *rec;
uint64_t larg;
caddr_t zdata;
zsize = zaggdata[i].dtahe_size;
assert(zsize != 0);
if ((zdata = dt_zalloc(dtp, zsize)) == NULL) {
for (j = i; j < naggvars; j++)
zaggdata[j].dtahe_data.dtada_data = NULL;
goto out;
}
aggvar = aggvars[(i - sortpos + naggvars) % naggvars];
rec = &aggdesc->dtagd_rec[0];
*((dtrace_aggvarid_t *)(zdata + rec->dtrd_offset)) = aggvar;
rec = &aggdesc->dtagd_rec[aggdesc->dtagd_nrecs - 1];
if (rec->dtrd_action == DTRACEAGG_LQUANTIZE) {
if (aggdata->dtada_data != NULL) {
larg = *((uint64_t *)(aggdata->dtada_data +
rec->dtrd_offset));
} else {
dtrace_stmtdesc_t *sdp;
dt_ident_t *aid;
dt_idsig_t *isp;
sdp = (dtrace_stmtdesc_t *)(uintptr_t)
aggdesc->dtagd_rec[0].dtrd_uarg;
aid = sdp->dtsd_aggdata;
isp = (dt_idsig_t *)aid->di_data;
assert(isp->dis_auxinfo != 0);
larg = isp->dis_auxinfo;
}
*((uint64_t *)(zdata + rec->dtrd_offset)) = larg;
}
aggdata->dtada_data = zdata;
}
sorted = dt_alloc(dtp, nentries * sizeof (dt_ahashent_t *));
if (sorted == NULL)
goto out;
for (h = hash->dtah_all, i = 0; h != NULL; h = h->dtahe_nextall) {
dtrace_aggvarid_t id;
if ((id = dt_aggregate_aggvarid(h)) > max || !map[id])
continue;
sorted[i++] = h;
}
assert(i == nentries);
(void) pthread_mutex_lock(&dt_qsort_lock);
qsort(sorted, nentries, sizeof (dt_ahashent_t *),
dt_aggregate_keyvarcmp);
bundle = (dt_ahashent_t ***)sorted;
for (i = 1, start = 0; i <= nentries; i++) {
if (i < nentries &&
dt_aggregate_keycmp(&sorted[i], &sorted[i - 1]) == 0)
continue;
assert(i - start <= naggvars);
bundlesize = (naggvars + 2) * sizeof (dt_ahashent_t *);
if ((nbundle = dt_zalloc(dtp, bundlesize)) == NULL) {
(void) pthread_mutex_unlock(&dt_qsort_lock);
goto out;
}
for (j = start; j < i; j++) {
dtrace_aggvarid_t id = dt_aggregate_aggvarid(sorted[j]);
assert(id <= max);
assert(map[id] != 0);
assert(map[id] - 1 < naggvars);
assert(nbundle[map[id] - 1] == NULL);
nbundle[map[id] - 1] = sorted[j];
if (nbundle[naggvars] == NULL)
nbundle[naggvars] = sorted[j];
}
for (j = 0; j < naggvars; j++) {
if (nbundle[j] != NULL)
continue;
if (remap != NULL && remap[j]) {
assert(remap[j] - 1 < j);
assert(nbundle[remap[j] - 1] != NULL);
nbundle[j] = nbundle[remap[j] - 1];
} else {
nbundle[j] = &zaggdata[j];
}
}
bundle[nbundles++] = nbundle;
start = i;
}
dt_aggregate_qsort(dtp, bundle, nbundles, sizeof (dt_ahashent_t **),
dt_aggregate_bundlecmp);
(void) pthread_mutex_unlock(&dt_qsort_lock);
data = alloca((naggvars + 1) * sizeof (dtrace_aggdata_t *));
for (i = 0; i < nbundles; i++) {
for (j = 0; j < naggvars; j++)
data[j + 1] = NULL;
for (j = 0; j < naggvars; j++) {
int ndx = j - sortpos;
if (ndx < 0)
ndx += naggvars;
assert(bundle[i][ndx] != NULL);
data[j + 1] = &bundle[i][ndx]->dtahe_data;
}
for (j = 0; j < naggvars; j++)
assert(data[j + 1] != NULL);
assert(bundle[i][j] != NULL);
data[0] = &bundle[i][j]->dtahe_data;
if ((rval = func(data, naggvars + 1, arg)) == -1)
goto out;
}
rval = 0;
out:
for (i = 0; i < nbundles; i++)
dt_free(dtp, bundle[i]);
if (zaggdata != NULL) {
for (i = 0; i < naggvars; i++)
dt_free(dtp, zaggdata[i].dtahe_data.dtada_data);
}
dt_free(dtp, zaggdata);
dt_free(dtp, sorted);
dt_free(dtp, remap);
dt_free(dtp, map);
return (rval);
}
int
dtrace_aggregate_print(dtrace_hdl_t *dtp, FILE *fp,
dtrace_aggregate_walk_f *func)
{
dt_print_aggdata_t pd;
bzero(&pd, sizeof (pd));
pd.dtpa_dtp = dtp;
pd.dtpa_fp = fp;
pd.dtpa_allunprint = 1;
if (func == NULL)
func = dtrace_aggregate_walk_sorted;
if ((*func)(dtp, dt_print_agg, &pd) == -1)
return (dt_set_errno(dtp, dtp->dt_errno));
return (0);
}
void
dtrace_aggregate_clear(dtrace_hdl_t *dtp)
{
dt_aggregate_t *agp = &dtp->dt_aggregate;
dt_ahash_t *hash = &agp->dtat_hash;
dt_ahashent_t *h;
dtrace_aggdata_t *data;
dtrace_aggdesc_t *aggdesc;
dtrace_recdesc_t *rec;
int i, max_cpus = agp->dtat_maxcpu;
for (h = hash->dtah_all; h != NULL; h = h->dtahe_nextall) {
aggdesc = h->dtahe_data.dtada_desc;
rec = &aggdesc->dtagd_rec[aggdesc->dtagd_nrecs - 1];
data = &h->dtahe_data;
bzero(&data->dtada_data[rec->dtrd_offset], rec->dtrd_size);
if (data->dtada_percpu == NULL)
continue;
for (i = 0; i < max_cpus; i++)
bzero(data->dtada_percpu[i], rec->dtrd_size);
}
}
void
dt_aggregate_destroy(dtrace_hdl_t *dtp)
{
dt_aggregate_t *agp = &dtp->dt_aggregate;
dt_ahash_t *hash = &agp->dtat_hash;
dt_ahashent_t *h, *next;
dtrace_aggdata_t *aggdata;
int i, max_cpus = agp->dtat_maxcpu;
if (hash->dtah_hash == NULL) {
assert(hash->dtah_all == NULL);
} else {
free(hash->dtah_hash);
for (h = hash->dtah_all; h != NULL; h = next) {
next = h->dtahe_nextall;
aggdata = &h->dtahe_data;
if (aggdata->dtada_percpu != NULL) {
for (i = 0; i < max_cpus; i++)
free(aggdata->dtada_percpu[i]);
free(aggdata->dtada_percpu);
}
free(aggdata->dtada_data);
free(h);
}
hash->dtah_hash = NULL;
hash->dtah_all = NULL;
hash->dtah_size = 0;
}
free(agp->dtat_buf.dtbd_data);
free(agp->dtat_cpus);
}
