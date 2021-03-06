
























#pragma ident "%Z%%M% %I% %E% SMI"



















































































































































#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>
#if defined(illumos)
#include <synch.h>
#endif
#include <signal.h>
#include <libgen.h>
#include <string.h>
#include <errno.h>
#if defined(illumos)
#include <alloca.h>
#endif
#include <sys/param.h>
#include <sys/types.h>
#include <sys/mman.h>
#if defined(illumos)
#include <sys/sysconf.h>
#endif

#include "ctf_headers.h"
#include "ctftools.h"
#include "ctfmerge.h"
#include "traverse.h"
#include "memory.h"
#include "fifo.h"
#include "barrier.h"

#pragma init(bigheap)

#define MERGE_PHASE1_BATCH_SIZE 8
#define MERGE_PHASE1_MAX_SLOTS 5
#define MERGE_INPUT_THROTTLE_LEN 10

const char *progname;
static char *outfile = NULL;
static char *tmpname = NULL;
static int dynsym;
int debug_level = DEBUG_LEVEL;
static size_t maxpgsize = 0x400000;


void
usage(void)
{
(void) fprintf(stderr,
"Usage: %s [-fgstv] -l label | -L labelenv -o outfile file ...\n"
" %s [-fgstv] -l label | -L labelenv -o outfile -d uniqfile\n"
" %*s [-g] [-D uniqlabel] file ...\n"
" %s [-fgstv] -l label | -L labelenv -o outfile -w withfile "
"file ...\n"
" %s [-g] -c srcfile destfile\n"
"\n"
" Note: if -L labelenv is specified and labelenv is not set in\n"
" the environment, a default value is used.\n",
progname, progname, (int)strlen(progname), " ",
progname, progname);
}

#if defined(illumos)
static void
bigheap(void)
{
size_t big, *size;
int sizes;
struct memcntl_mha mha;




if ((sizes = getpagesizes(NULL, 0)) == -1)
return;

if (sizes == 1 || (size = alloca(sizeof (size_t) * sizes)) == NULL)
return;

if (getpagesizes(size, sizes) == -1)
return;

while (size[sizes - 1] > maxpgsize)
sizes--;


big = size[sizes - 1];
if (big & (big - 1)) {




return;
}




if (brk((void *)((((uintptr_t)sbrk(0) - 1) & ~(big - 1)) + big)) != 0)
return;




mha.mha_cmd = MHA_MAPSIZE_BSSBRK;
mha.mha_flags = 0;
mha.mha_pagesize = big;

(void) memcntl(NULL, 0, MC_HAT_ADVISE, (caddr_t)&mha, 0, 0);
}
#endif

static void
finalize_phase_one(workqueue_t *wq)
{
int startslot, i;













for (startslot = -1, i = 0; i < wq->wq_nwipslots; i++) {
if (wq->wq_wip[i].wip_batchid == wq->wq_lastdonebatch + 1) {
startslot = i;
break;
}
}

assert(startslot != -1);

for (i = startslot; i < startslot + wq->wq_nwipslots; i++) {
int slotnum = i % wq->wq_nwipslots;
wip_t *wipslot = &wq->wq_wip[slotnum];

if (wipslot->wip_td != NULL) {
debug(2, "clearing slot %d (%d) (saving %d)\n",
slotnum, i, wipslot->wip_nmerged);
} else
debug(2, "clearing slot %d (%d)\n", slotnum, i);

if (wipslot->wip_td != NULL) {
fifo_add(wq->wq_donequeue, wipslot->wip_td);
wq->wq_wip[slotnum].wip_td = NULL;
}
}

wq->wq_lastdonebatch = wq->wq_next_batchid++;

debug(2, "phase one done: donequeue has %d items\n",
fifo_len(wq->wq_donequeue));
}

static void
init_phase_two(workqueue_t *wq)
{
int num;









wq->wq_ninqueue = num = fifo_len(wq->wq_donequeue);
while (num != 1) {
wq->wq_ninqueue += num / 2;
num = num / 2 + num % 2;
}





assert(fifo_len(wq->wq_queue) == 0);
fifo_free(wq->wq_queue, NULL);
wq->wq_queue = wq->wq_donequeue;
}

static void
wip_save_work(workqueue_t *wq, wip_t *slot, int slotnum)
{
pthread_mutex_lock(&wq->wq_donequeue_lock);

while (wq->wq_lastdonebatch + 1 < slot->wip_batchid)
pthread_cond_wait(&slot->wip_cv, &wq->wq_donequeue_lock);
assert(wq->wq_lastdonebatch + 1 == slot->wip_batchid);

fifo_add(wq->wq_donequeue, slot->wip_td);
wq->wq_lastdonebatch++;
pthread_cond_signal(&wq->wq_wip[(slotnum + 1) %
wq->wq_nwipslots].wip_cv);


slot->wip_td = NULL;
slot->wip_batchid = wq->wq_next_batchid++;

pthread_mutex_unlock(&wq->wq_donequeue_lock);
}

static void
wip_add_work(wip_t *slot, tdata_t *pow)
{
if (slot->wip_td == NULL) {
slot->wip_td = pow;
slot->wip_nmerged = 1;
} else {
debug(2, "%d: merging %p into %p\n", pthread_self(),
(void *)pow, (void *)slot->wip_td);

merge_into_master(pow, slot->wip_td, NULL, 0);
tdata_free(pow);

slot->wip_nmerged++;
}
}

static void
worker_runphase1(workqueue_t *wq)
{
wip_t *wipslot;
tdata_t *pow;
int wipslotnum, pownum;

for (;;) {
pthread_mutex_lock(&wq->wq_queue_lock);

while (fifo_empty(wq->wq_queue)) {
if (wq->wq_nomorefiles == 1) {
pthread_cond_broadcast(&wq->wq_work_avail);
pthread_mutex_unlock(&wq->wq_queue_lock);


return;
}

pthread_cond_wait(&wq->wq_work_avail,
&wq->wq_queue_lock);
}


pow = fifo_remove(wq->wq_queue);
pownum = wq->wq_nextpownum++;
pthread_cond_broadcast(&wq->wq_work_removed);

assert(pow != NULL);


wipslotnum = pownum % wq->wq_nwipslots;
wipslot = &wq->wq_wip[wipslotnum];

pthread_mutex_lock(&wipslot->wip_lock);

pthread_mutex_unlock(&wq->wq_queue_lock);

wip_add_work(wipslot, pow);

if (wipslot->wip_nmerged == wq->wq_maxbatchsz)
wip_save_work(wq, wipslot, wipslotnum);

pthread_mutex_unlock(&wipslot->wip_lock);
}
}

static void
worker_runphase2(workqueue_t *wq)
{
tdata_t *pow1, *pow2;
int batchid;

for (;;) {
pthread_mutex_lock(&wq->wq_queue_lock);

if (wq->wq_ninqueue == 1) {
pthread_cond_broadcast(&wq->wq_work_avail);
pthread_mutex_unlock(&wq->wq_queue_lock);

debug(2, "%d: entering p2 completion barrier\n",
pthread_self());
if (barrier_wait(&wq->wq_bar1)) {
pthread_mutex_lock(&wq->wq_queue_lock);
wq->wq_alldone = 1;
pthread_cond_signal(&wq->wq_alldone_cv);
pthread_mutex_unlock(&wq->wq_queue_lock);
}

return;
}

if (fifo_len(wq->wq_queue) < 2) {
pthread_cond_wait(&wq->wq_work_avail,
&wq->wq_queue_lock);
pthread_mutex_unlock(&wq->wq_queue_lock);
continue;
}


pow1 = fifo_remove(wq->wq_queue);
pow2 = fifo_remove(wq->wq_queue);
wq->wq_ninqueue -= 2;

batchid = wq->wq_next_batchid++;

pthread_mutex_unlock(&wq->wq_queue_lock);

debug(2, "%d: merging %p into %p\n", pthread_self(),
(void *)pow1, (void *)pow2);
merge_into_master(pow1, pow2, NULL, 0);
tdata_free(pow1);





pthread_mutex_lock(&wq->wq_queue_lock);
while (wq->wq_lastdonebatch + 1 != batchid) {
pthread_cond_wait(&wq->wq_done_cv,
&wq->wq_queue_lock);
}

wq->wq_lastdonebatch = batchid;

fifo_add(wq->wq_queue, pow2);
debug(2, "%d: added %p to queue, len now %d, ninqueue %d\n",
pthread_self(), (void *)pow2, fifo_len(wq->wq_queue),
wq->wq_ninqueue);
pthread_cond_broadcast(&wq->wq_done_cv);
pthread_cond_signal(&wq->wq_work_avail);
pthread_mutex_unlock(&wq->wq_queue_lock);
}
}




static void
worker_thread(workqueue_t *wq)
{
worker_runphase1(wq);

debug(2, "%d: entering first barrier\n", pthread_self());

if (barrier_wait(&wq->wq_bar1)) {

debug(2, "%d: doing work in first barrier\n", pthread_self());

finalize_phase_one(wq);

init_phase_two(wq);

debug(2, "%d: ninqueue is %d, %d on queue\n", pthread_self(),
wq->wq_ninqueue, fifo_len(wq->wq_queue));
}

debug(2, "%d: entering second barrier\n", pthread_self());

(void) barrier_wait(&wq->wq_bar2);

debug(2, "%d: phase 1 complete\n", pthread_self());

worker_runphase2(wq);
}





static int
merge_ctf_cb(tdata_t *td, char *name, void *arg)
{
workqueue_t *wq = arg;

debug(3, "Adding tdata %p for processing\n", (void *)td);

pthread_mutex_lock(&wq->wq_queue_lock);
while (fifo_len(wq->wq_queue) > wq->wq_ithrottle) {
debug(2, "Throttling input (len = %d, throttle = %d)\n",
fifo_len(wq->wq_queue), wq->wq_ithrottle);
pthread_cond_wait(&wq->wq_work_removed, &wq->wq_queue_lock);
}

fifo_add(wq->wq_queue, td);
debug(1, "Thread %d announcing %s\n", pthread_self(), name);
pthread_cond_broadcast(&wq->wq_work_avail);
pthread_mutex_unlock(&wq->wq_queue_lock);

return (1);
}



















static void
handle_sig(int sig)
{
terminate("Caught signal %d - exiting\n", sig);
}

static void
terminate_cleanup(void)
{
int dounlink = getenv("CTFMERGE_TERMINATE_NO_UNLINK") ? 0 : 1;

if (tmpname != NULL && dounlink)
unlink(tmpname);

if (outfile == NULL)
return;

#if !defined(__FreeBSD__)
if (dounlink) {
fprintf(stderr, "Removing %s\n", outfile);
unlink(outfile);
}
#endif
}

static void
copy_ctf_data(char *srcfile, char *destfile, int keep_stabs)
{
tdata_t *srctd;

if (read_ctf(&srcfile, 1, NULL, read_ctf_save_cb, &srctd, 1) == 0)
terminate("No CTF data found in source file %s\n", srcfile);

tmpname = mktmpname(destfile, ".ctf");
write_ctf(srctd, destfile, tmpname, CTF_COMPRESS | CTF_SWAP_BYTES | keep_stabs);
if (rename(tmpname, destfile) != 0) {
terminate("Couldn't rename temp file %s to %s", tmpname,
destfile);
}
free(tmpname);
tdata_free(srctd);
}

static void
wq_init(workqueue_t *wq, int nfiles)
{
int throttle, nslots, i;

if (getenv("CTFMERGE_MAX_SLOTS"))
nslots = atoi(getenv("CTFMERGE_MAX_SLOTS"));
else
nslots = MERGE_PHASE1_MAX_SLOTS;

if (getenv("CTFMERGE_PHASE1_BATCH_SIZE"))
wq->wq_maxbatchsz = atoi(getenv("CTFMERGE_PHASE1_BATCH_SIZE"));
else
wq->wq_maxbatchsz = MERGE_PHASE1_BATCH_SIZE;

nslots = MIN(nslots, (nfiles + wq->wq_maxbatchsz - 1) /
wq->wq_maxbatchsz);

wq->wq_wip = xcalloc(sizeof (wip_t) * nslots);
wq->wq_nwipslots = nslots;
wq->wq_nthreads = MIN(sysconf(_SC_NPROCESSORS_ONLN) * 3 / 2, nslots);
wq->wq_thread = xmalloc(sizeof (pthread_t) * wq->wq_nthreads);

if (getenv("CTFMERGE_INPUT_THROTTLE"))
throttle = atoi(getenv("CTFMERGE_INPUT_THROTTLE"));
else
throttle = MERGE_INPUT_THROTTLE_LEN;
wq->wq_ithrottle = throttle * wq->wq_nthreads;

debug(1, "Using %d slots, %d threads\n", wq->wq_nwipslots,
wq->wq_nthreads);

wq->wq_next_batchid = 0;

for (i = 0; i < nslots; i++) {
pthread_mutex_init(&wq->wq_wip[i].wip_lock, NULL);
pthread_cond_init(&wq->wq_wip[i].wip_cv, NULL);
wq->wq_wip[i].wip_batchid = wq->wq_next_batchid++;
}

pthread_mutex_init(&wq->wq_queue_lock, NULL);
wq->wq_queue = fifo_new();
pthread_cond_init(&wq->wq_work_avail, NULL);
pthread_cond_init(&wq->wq_work_removed, NULL);
wq->wq_ninqueue = nfiles;
wq->wq_nextpownum = 0;

pthread_mutex_init(&wq->wq_donequeue_lock, NULL);
wq->wq_donequeue = fifo_new();
wq->wq_lastdonebatch = -1;

pthread_cond_init(&wq->wq_done_cv, NULL);

pthread_cond_init(&wq->wq_alldone_cv, NULL);
wq->wq_alldone = 0;

barrier_init(&wq->wq_bar1, wq->wq_nthreads);
barrier_init(&wq->wq_bar2, wq->wq_nthreads);

wq->wq_nomorefiles = 0;
}

static void
start_threads(workqueue_t *wq)
{
sigset_t sets;
int i;

sigemptyset(&sets);
sigaddset(&sets, SIGINT);
sigaddset(&sets, SIGQUIT);
sigaddset(&sets, SIGTERM);
pthread_sigmask(SIG_BLOCK, &sets, NULL);

for (i = 0; i < wq->wq_nthreads; i++) {
pthread_create(&wq->wq_thread[i], NULL,
(void *(*)(void *))worker_thread, wq);
}

#if defined(illumos)
sigset(SIGINT, handle_sig);
sigset(SIGQUIT, handle_sig);
sigset(SIGTERM, handle_sig);
#else
signal(SIGINT, handle_sig);
signal(SIGQUIT, handle_sig);
signal(SIGTERM, handle_sig);
#endif
pthread_sigmask(SIG_UNBLOCK, &sets, NULL);
}

static void
join_threads(workqueue_t *wq)
{
int i;

for (i = 0; i < wq->wq_nthreads; i++) {
pthread_join(wq->wq_thread[i], NULL);
}
}

static int
strcompare(const void *p1, const void *p2)
{
char *s1 = *((char **)p1);
char *s2 = *((char **)p2);

return (strcmp(s1, s2));
}








static workqueue_t wq;

int
main(int argc, char **argv)
{
tdata_t *mstrtd, *savetd;
char *uniqfile = NULL, *uniqlabel = NULL;
char *withfile = NULL;
char *label = NULL;
char **ifiles, **tifiles;
int verbose = 0, docopy = 0;
int write_fuzzy_match = 0;
int keep_stabs = 0;
int require_ctf = 0;
int nifiles, nielems;
int c, i, idx, tidx, err;

progname = basename(argv[0]);

if (getenv("CTFMERGE_DEBUG_LEVEL"))
debug_level = atoi(getenv("CTFMERGE_DEBUG_LEVEL"));

err = 0;
while ((c = getopt(argc, argv, ":cd:D:fgl:L:o:tvw:s")) != EOF) {
switch (c) {
case 'c':
docopy = 1;
break;
case 'd':

uniqfile = optarg;
break;
case 'D':

uniqlabel = optarg;
break;
case 'f':
write_fuzzy_match = CTF_FUZZY_MATCH;
break;
case 'g':
keep_stabs = CTF_KEEP_STABS;
break;
case 'l':

label = optarg;
break;
case 'L':

if ((label = getenv(optarg)) == NULL)
label = CTF_DEFAULT_LABEL;
break;
case 'o':

outfile = optarg;
break;
case 't':

require_ctf = 1;
break;
case 'v':

verbose = 1;
break;
case 'w':

withfile = optarg;
break;
case 's':

dynsym = CTF_USE_DYNSYM;
break;
default:
usage();
exit(2);
}
}


if (docopy) {
if (uniqfile != NULL || uniqlabel != NULL || label != NULL ||
outfile != NULL || withfile != NULL || dynsym != 0)
err++;

if (argc - optind != 2)
err++;
} else {
if (uniqfile != NULL && withfile != NULL)
err++;

if (uniqlabel != NULL && uniqfile == NULL)
err++;

if (outfile == NULL || label == NULL)
err++;

if (argc - optind == 0)
err++;
}

if (err) {
usage();
exit(2);
}

if (getenv("STRIPSTABS_KEEP_STABS") != NULL)
keep_stabs = CTF_KEEP_STABS;

if (uniqfile && access(uniqfile, R_OK) != 0) {
warning("Uniquification file %s couldn't be opened and "
"will be ignored.\n", uniqfile);
uniqfile = NULL;
}
if (withfile && access(withfile, R_OK) != 0) {
warning("With file %s couldn't be opened and will be "
"ignored.\n", withfile);
withfile = NULL;
}
if (outfile && access(outfile, R_OK|W_OK) != 0)
terminate("Cannot open output file %s for r/w", outfile);






if (docopy) {
copy_ctf_data(argv[optind], argv[optind + 1], keep_stabs);

exit(0);
}

set_terminate_cleanup(terminate_cleanup);


nifiles = argc - optind;
ifiles = xmalloc(sizeof (char *) * nifiles);
tifiles = xmalloc(sizeof (char *) * nifiles);

for (i = 0; i < nifiles; i++)
tifiles[i] = argv[optind + i];
qsort(tifiles, nifiles, sizeof (char *), (int (*)())strcompare);

ifiles[0] = tifiles[0];
for (idx = 0, tidx = 1; tidx < nifiles; tidx++) {
if (strcmp(ifiles[idx], tifiles[tidx]) != 0)
ifiles[++idx] = tifiles[tidx];
}
nifiles = idx + 1;


if ((nielems = count_files(ifiles, nifiles)) < 0)
terminate("Some input files were inaccessible\n");


wq_init(&wq, nielems);

start_threads(&wq);







if (read_ctf(ifiles, nifiles, NULL, merge_ctf_cb,
&wq, require_ctf) == 0) {





if (require_ctf)
exit(0);
terminate("No ctf sections found to merge\n");
}

pthread_mutex_lock(&wq.wq_queue_lock);
wq.wq_nomorefiles = 1;
pthread_cond_broadcast(&wq.wq_work_avail);
pthread_mutex_unlock(&wq.wq_queue_lock);

pthread_mutex_lock(&wq.wq_queue_lock);
while (wq.wq_alldone == 0)
pthread_cond_wait(&wq.wq_alldone_cv, &wq.wq_queue_lock);
pthread_mutex_unlock(&wq.wq_queue_lock);

join_threads(&wq);












assert(fifo_len(wq.wq_queue) == 1);
mstrtd = fifo_remove(wq.wq_queue);

if (verbose || debug_level) {
debug(2, "Statistics for td %p\n", (void *)mstrtd);

iidesc_stats(mstrtd->td_iihash);
}

if (uniqfile != NULL || withfile != NULL) {
char *reffile, *reflabel = NULL;
tdata_t *reftd;

if (uniqfile != NULL) {
reffile = uniqfile;
reflabel = uniqlabel;
} else
reffile = withfile;

if (read_ctf(&reffile, 1, reflabel, read_ctf_save_cb,
&reftd, require_ctf) == 0) {
terminate("No CTF data found in reference file %s\n",
reffile);
}

savetd = tdata_new();

if (CTF_TYPE_ISCHILD(reftd->td_nextid))
terminate("No room for additional types in master\n");

savetd->td_nextid = withfile ? reftd->td_nextid :
CTF_INDEX_TO_TYPE(1, TRUE);
merge_into_master(mstrtd, reftd, savetd, 0);

tdata_label_add(savetd, label, CTF_LABEL_LASTIDX);

if (withfile) {



tdata_t *withtd = reftd;

tdata_merge(withtd, savetd);

savetd = withtd;
} else {
char uniqname[MAXPATHLEN];
labelent_t *parle;

parle = tdata_label_top(reftd);

savetd->td_parlabel = xstrdup(parle->le_name);

strncpy(uniqname, reffile, sizeof (uniqname));
uniqname[MAXPATHLEN - 1] = '\0';
savetd->td_parname = xstrdup(basename(uniqname));
}

} else {




tdata_label_free(mstrtd);
tdata_label_add(mstrtd, label, CTF_LABEL_LASTIDX);

savetd = mstrtd;
}

tmpname = mktmpname(outfile, ".ctf");
write_ctf(savetd, outfile, tmpname,
CTF_COMPRESS | CTF_SWAP_BYTES | write_fuzzy_match | dynsym | keep_stabs);
if (rename(tmpname, outfile) != 0)
terminate("Couldn't rename output temp file %s", tmpname);
free(tmpname);

return (0);
}
