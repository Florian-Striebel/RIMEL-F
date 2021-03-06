#include <sys/wait.h>
#if defined(illumos)
#include <sys/lwp.h>
#endif
#include <strings.h>
#include <signal.h>
#include <assert.h>
#include <errno.h>
#include <dt_proc.h>
#include <dt_pid.h>
#include <dt_impl.h>
#if !defined(illumos)
#include <sys/syscall.h>
#include <libproc_compat.h>
#define SYS_forksys SYS_fork
#endif
#define IS_SYS_EXEC(w) (w == SYS_execve)
#define IS_SYS_FORK(w) (w == SYS_vfork || w == SYS_forksys)
static dt_bkpt_t *
dt_proc_bpcreate(dt_proc_t *dpr, uintptr_t addr, dt_bkpt_f *func, void *data)
{
struct ps_prochandle *P = dpr->dpr_proc;
dt_bkpt_t *dbp;
assert(DT_MUTEX_HELD(&dpr->dpr_lock));
if ((dbp = dt_zalloc(dpr->dpr_hdl, sizeof (dt_bkpt_t))) != NULL) {
dbp->dbp_func = func;
dbp->dbp_data = data;
dbp->dbp_addr = addr;
if (Psetbkpt(P, dbp->dbp_addr, &dbp->dbp_instr) == 0)
dbp->dbp_active = B_TRUE;
dt_list_append(&dpr->dpr_bps, dbp);
}
return (dbp);
}
static void
dt_proc_bpdestroy(dt_proc_t *dpr, int delbkpts)
{
int state = Pstate(dpr->dpr_proc);
dt_bkpt_t *dbp, *nbp;
assert(DT_MUTEX_HELD(&dpr->dpr_lock));
for (dbp = dt_list_next(&dpr->dpr_bps); dbp != NULL; dbp = nbp) {
if (delbkpts && dbp->dbp_active &&
state != PS_LOST && state != PS_UNDEAD) {
(void) Pdelbkpt(dpr->dpr_proc,
dbp->dbp_addr, dbp->dbp_instr);
}
nbp = dt_list_next(dbp);
dt_list_delete(&dpr->dpr_bps, dbp);
dt_free(dpr->dpr_hdl, dbp);
}
}
static void
dt_proc_bpmatch(dtrace_hdl_t *dtp, dt_proc_t *dpr)
{
#if defined(illumos)
const lwpstatus_t *psp = &Pstatus(dpr->dpr_proc)->pr_lwp;
#else
unsigned long pc;
#endif
dt_bkpt_t *dbp;
assert(DT_MUTEX_HELD(&dpr->dpr_lock));
#if !defined(illumos)
proc_regget(dpr->dpr_proc, REG_PC, &pc);
proc_bkptregadj(&pc);
#endif
for (dbp = dt_list_next(&dpr->dpr_bps);
dbp != NULL; dbp = dt_list_next(dbp)) {
#if defined(illumos)
if (psp->pr_reg[R_PC] == dbp->dbp_addr)
break;
#else
if (pc == dbp->dbp_addr)
break;
#endif
}
if (dbp == NULL) {
dt_dprintf("pid %d: spurious breakpoint wakeup for %lx\n",
#if defined(illumos)
(int)dpr->dpr_pid, (ulong_t)psp->pr_reg[R_PC]);
#else
(int)dpr->dpr_pid, pc);
#endif
return;
}
dt_dprintf("pid %d: hit breakpoint at %lx (%lu)\n",
(int)dpr->dpr_pid, (ulong_t)dbp->dbp_addr, ++dbp->dbp_hits);
dbp->dbp_func(dtp, dpr, dbp->dbp_data);
(void) Pxecbkpt(dpr->dpr_proc, dbp->dbp_instr);
}
static void
dt_proc_bpenable(dt_proc_t *dpr)
{
dt_bkpt_t *dbp;
assert(DT_MUTEX_HELD(&dpr->dpr_lock));
for (dbp = dt_list_next(&dpr->dpr_bps);
dbp != NULL; dbp = dt_list_next(dbp)) {
if (!dbp->dbp_active && Psetbkpt(dpr->dpr_proc,
dbp->dbp_addr, &dbp->dbp_instr) == 0)
dbp->dbp_active = B_TRUE;
}
dt_dprintf("breakpoints enabled\n");
}
static void
dt_proc_bpdisable(dt_proc_t *dpr)
{
dt_bkpt_t *dbp;
assert(DT_MUTEX_HELD(&dpr->dpr_lock));
for (dbp = dt_list_next(&dpr->dpr_bps);
dbp != NULL; dbp = dt_list_next(dbp)) {
if (dbp->dbp_active && Pdelbkpt(dpr->dpr_proc,
dbp->dbp_addr, dbp->dbp_instr) == 0)
dbp->dbp_active = B_FALSE;
}
dt_dprintf("breakpoints disabled\n");
}
static void
dt_proc_notify(dtrace_hdl_t *dtp, dt_proc_hash_t *dph, dt_proc_t *dpr,
const char *msg)
{
dt_proc_notify_t *dprn = dt_alloc(dtp, sizeof (dt_proc_notify_t));
if (dprn == NULL) {
dt_dprintf("failed to allocate notification for %d %s\n",
(int)dpr->dpr_pid, msg);
} else {
dprn->dprn_dpr = dpr;
if (msg == NULL)
dprn->dprn_errmsg[0] = '\0';
else
(void) strlcpy(dprn->dprn_errmsg, msg,
sizeof (dprn->dprn_errmsg));
(void) pthread_mutex_lock(&dph->dph_lock);
dprn->dprn_next = dph->dph_notify;
dph->dph_notify = dprn;
(void) pthread_cond_broadcast(&dph->dph_cv);
(void) pthread_mutex_unlock(&dph->dph_lock);
}
}
static void
dt_proc_stop(dt_proc_t *dpr, uint8_t why)
{
assert(DT_MUTEX_HELD(&dpr->dpr_lock));
assert(why != DT_PROC_STOP_IDLE);
if (dpr->dpr_stop & why) {
dpr->dpr_stop |= DT_PROC_STOP_IDLE;
dpr->dpr_stop &= ~why;
(void) pthread_cond_broadcast(&dpr->dpr_cv);
dt_proc_bpdisable(dpr);
while (dpr->dpr_stop & DT_PROC_STOP_IDLE)
(void) pthread_cond_wait(&dpr->dpr_cv, &dpr->dpr_lock);
dt_proc_bpenable(dpr);
}
}
static void
dt_proc_bpmain(dtrace_hdl_t *dtp, dt_proc_t *dpr, const char *fname)
{
dt_dprintf("pid %d: breakpoint at %s()\n", (int)dpr->dpr_pid, fname);
dt_proc_stop(dpr, DT_PROC_STOP_MAIN);
}
static void
dt_proc_rdevent(dtrace_hdl_t *dtp, dt_proc_t *dpr, const char *evname)
{
rd_event_msg_t rdm;
rd_err_e err;
if ((err = rd_event_getmsg(dpr->dpr_rtld, &rdm)) != RD_OK) {
dt_dprintf("pid %d: failed to get %s event message: %s\n",
(int)dpr->dpr_pid, evname, rd_errstr(err));
return;
}
dt_dprintf("pid %d: rtld event %s type=%d state %d\n",
(int)dpr->dpr_pid, evname, rdm.type, rdm.u.state);
switch (rdm.type) {
case RD_DLACTIVITY:
if (rdm.u.state != RD_CONSISTENT)
break;
Pupdate_syms(dpr->dpr_proc);
if (dt_pid_create_probes_module(dtp, dpr) != 0)
dt_proc_notify(dtp, dtp->dt_procs, dpr,
dpr->dpr_errmsg);
break;
case RD_PREINIT:
Pupdate_syms(dpr->dpr_proc);
dt_proc_stop(dpr, DT_PROC_STOP_PREINIT);
break;
case RD_POSTINIT:
Pupdate_syms(dpr->dpr_proc);
dt_proc_stop(dpr, DT_PROC_STOP_POSTINIT);
break;
}
}
static void
dt_proc_rdwatch(dt_proc_t *dpr, rd_event_e event, const char *evname)
{
rd_notify_t rdn;
rd_err_e err;
if ((err = rd_event_addr(dpr->dpr_rtld, event, &rdn)) != RD_OK) {
dt_dprintf("pid %d: failed to get event address for %s: %s\n",
(int)dpr->dpr_pid, evname, rd_errstr(err));
return;
}
if (rdn.type != RD_NOTIFY_BPT) {
dt_dprintf("pid %d: event %s has unexpected type %d\n",
(int)dpr->dpr_pid, evname, rdn.type);
return;
}
(void) dt_proc_bpcreate(dpr, rdn.u.bptaddr,
#if defined(illumos)
(dt_bkpt_f *)dt_proc_rdevent, (void *)evname);
#else
(dt_bkpt_f *)dt_proc_rdevent, __DECONST(void *, evname));
#endif
}
static void
dt_proc_attach(dt_proc_t *dpr, int exec)
{
#if defined(illumos)
const pstatus_t *psp = Pstatus(dpr->dpr_proc);
#endif
rd_err_e err;
GElf_Sym sym;
assert(DT_MUTEX_HELD(&dpr->dpr_lock));
if (exec) {
#if defined(illumos)
if (psp->pr_lwp.pr_errno != 0)
return;
#endif
dt_proc_bpdestroy(dpr, B_FALSE);
#if defined(illumos)
Preset_maps(dpr->dpr_proc);
#endif
}
if ((dpr->dpr_rtld = Prd_agent(dpr->dpr_proc)) != NULL &&
(err = rd_event_enable(dpr->dpr_rtld, B_TRUE)) == RD_OK) {
#if defined(illumos)
dt_proc_rdwatch(dpr, RD_PREINIT, "RD_PREINIT");
#endif
dt_proc_rdwatch(dpr, RD_POSTINIT, "RD_POSTINIT");
#if defined(illumos)
dt_proc_rdwatch(dpr, RD_DLACTIVITY, "RD_DLACTIVITY");
#endif
} else {
dt_dprintf("pid %d: failed to enable rtld events: %s\n",
(int)dpr->dpr_pid, dpr->dpr_rtld ? rd_errstr(err) :
"rtld_db agent initialization failed");
}
Pupdate_maps(dpr->dpr_proc);
if (Pxlookup_by_name(dpr->dpr_proc, LM_ID_BASE,
"a.out", "main", &sym, NULL) == 0) {
(void) dt_proc_bpcreate(dpr, (uintptr_t)sym.st_value,
(dt_bkpt_f *)dt_proc_bpmain, "a.out`main");
} else {
dt_dprintf("pid %d: failed to find a.out`main: %s\n",
(int)dpr->dpr_pid, strerror(errno));
}
}
static void
dt_proc_waitrun(dt_proc_t *dpr)
{
printf("%s:%s(%d): not implemented\n", __FUNCTION__, __FILE__,
__LINE__);
#if defined(DOODAD)
struct ps_prochandle *P = dpr->dpr_proc;
const lwpstatus_t *psp = &Pstatus(P)->pr_lwp;
int krflag = psp->pr_flags & (PR_KLC | PR_RLC);
timestruc_t tstamp = psp->pr_tstamp;
lwpid_t lwpid = psp->pr_lwpid;
const long wstop = PCWSTOP;
int pfd = Pctlfd(P);
assert(DT_MUTEX_HELD(&dpr->dpr_lock));
assert(psp->pr_flags & PR_STOPPED);
assert(Pstate(P) == PS_STOP);
(void) Punsetflags(P, krflag);
Psync(P);
(void) pthread_mutex_unlock(&dpr->dpr_lock);
while (!dpr->dpr_quit) {
if (write(pfd, &wstop, sizeof (wstop)) == -1 && errno == EINTR)
continue;
(void) pthread_mutex_lock(&dpr->dpr_lock);
(void) Pstopstatus(P, PCNULL, 0);
psp = &Pstatus(P)->pr_lwp;
if (Pstate(P) != PS_STOP || psp->pr_lwpid != lwpid ||
bcmp(&psp->pr_tstamp, &tstamp, sizeof (tstamp)) != 0) {
(void) Psetflags(P, krflag);
Psync(P);
return;
}
(void) pthread_mutex_unlock(&dpr->dpr_lock);
(void) poll(NULL, 0, MILLISEC / 2);
}
(void) pthread_mutex_lock(&dpr->dpr_lock);
#endif
}
typedef struct dt_proc_control_data {
dtrace_hdl_t *dpcd_hdl;
dt_proc_t *dpcd_proc;
} dt_proc_control_data_t;
static void *
dt_proc_control(void *arg)
{
dt_proc_control_data_t *datap = arg;
dtrace_hdl_t *dtp = datap->dpcd_hdl;
dt_proc_t *dpr = datap->dpcd_proc;
dt_proc_hash_t *dph = dtp->dt_procs;
struct ps_prochandle *P = dpr->dpr_proc;
int pid = dpr->dpr_pid;
#if defined(illumos)
int pfd = Pctlfd(P);
const long wstop = PCWSTOP;
#endif
int notify = B_FALSE;
(void) pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
(void) pthread_mutex_lock(&dpr->dpr_lock);
#if defined(illumos)
(void) Punsetflags(P, PR_ASYNC);
(void) Psetflags(P, PR_BPTADJ);
(void) Punsetflags(P, PR_FORK);
(void) Pfault(P, FLTBPT, B_TRUE);
(void) Pfault(P, FLTTRACE, B_TRUE);
(void) Psysexit(P, SYS_execve, B_TRUE);
(void) Psysentry(P, SYS_vfork, B_TRUE);
(void) Psysexit(P, SYS_vfork, B_TRUE);
(void) Psysentry(P, SYS_forksys, B_TRUE);
(void) Psysexit(P, SYS_forksys, B_TRUE);
Psync(P);
#endif
dt_proc_attach(dpr, B_FALSE);
#if defined(illumos)
if (Pstatus(P)->pr_flags & PR_KLC)
#else
if (proc_getflags(P) & PR_KLC)
#endif
dt_proc_stop(dpr, DT_PROC_STOP_CREATE);
else
dt_proc_stop(dpr, DT_PROC_STOP_GRAB);
if (Psetrun(P, 0, 0) == -1) {
dt_dprintf("pid %d: failed to set running: %s\n",
(int)dpr->dpr_pid, strerror(errno));
}
(void) pthread_mutex_unlock(&dpr->dpr_lock);
while (!dpr->dpr_quit) {
const lwpstatus_t *psp;
#if defined(illumos)
if (write(pfd, &wstop, sizeof (wstop)) == -1 && errno == EINTR)
continue;
#else
proc_wstatus(P);
if (errno == EINTR)
continue;
#endif
(void) pthread_mutex_lock(&dpr->dpr_lock);
#if defined(illumos)
pwait_locked:
if (Pstopstatus(P, PCNULL, 0) == -1 && errno == EINTR) {
(void) pthread_mutex_unlock(&dpr->dpr_lock);
continue;
}
#endif
switch (Pstate(P)) {
case PS_STOP:
#if defined(illumos)
psp = &Pstatus(P)->pr_lwp;
#else
psp = proc_getlwpstatus(P);
#endif
dt_dprintf("pid %d: proc stopped showing %d/%d\n",
pid, psp->pr_why, psp->pr_what);
if (psp->pr_why == PR_REQUESTED) {
dt_proc_waitrun(dpr);
(void) pthread_mutex_unlock(&dpr->dpr_lock);
continue;
}
if (psp->pr_why == PR_FAULTED && psp->pr_what == FLTBPT)
dt_proc_bpmatch(dtp, dpr);
else if (psp->pr_why == PR_SYSENTRY &&
IS_SYS_FORK(psp->pr_what))
dt_proc_bpdisable(dpr);
else if (psp->pr_why == PR_SYSEXIT &&
IS_SYS_FORK(psp->pr_what))
dt_proc_bpenable(dpr);
else if (psp->pr_why == PR_SYSEXIT &&
IS_SYS_EXEC(psp->pr_what))
dt_proc_attach(dpr, B_TRUE);
break;
case PS_LOST:
#if defined(illumos)
if (Preopen(P) == 0)
goto pwait_locked;
#endif
dt_dprintf("pid %d: proc lost: %s\n",
pid, strerror(errno));
dpr->dpr_quit = B_TRUE;
notify = B_TRUE;
break;
case PS_UNDEAD:
dt_dprintf("pid %d: proc died\n", pid);
dpr->dpr_quit = B_TRUE;
notify = B_TRUE;
break;
}
if (Pstate(P) != PS_UNDEAD && Psetrun(P, 0, 0) == -1) {
dt_dprintf("pid %d: failed to set running: %s\n",
(int)dpr->dpr_pid, strerror(errno));
}
(void) pthread_mutex_unlock(&dpr->dpr_lock);
}
if (notify)
dt_proc_notify(dtp, dph, dpr, NULL);
(void) pthread_mutex_lock(&dpr->dpr_lock);
dt_proc_bpdestroy(dpr, B_TRUE);
dpr->dpr_done = B_TRUE;
dpr->dpr_tid = 0;
(void) pthread_cond_broadcast(&dpr->dpr_cv);
(void) pthread_mutex_unlock(&dpr->dpr_lock);
return (NULL);
}
static struct ps_prochandle *
dt_proc_error(dtrace_hdl_t *dtp, dt_proc_t *dpr, const char *format, ...)
{
va_list ap;
va_start(ap, format);
dt_set_errmsg(dtp, NULL, NULL, NULL, 0, format, ap);
va_end(ap);
if (dpr->dpr_proc != NULL)
Prelease(dpr->dpr_proc, 0);
dt_free(dtp, dpr);
(void) dt_set_errno(dtp, EDT_COMPILER);
return (NULL);
}
dt_proc_t *
dt_proc_lookup(dtrace_hdl_t *dtp, struct ps_prochandle *P, int remove)
{
dt_proc_hash_t *dph = dtp->dt_procs;
#if defined(illumos)
pid_t pid = Pstatus(P)->pr_pid;
#else
pid_t pid = proc_getpid(P);
#endif
dt_proc_t *dpr, **dpp = &dph->dph_hash[pid & (dph->dph_hashlen - 1)];
for (dpr = *dpp; dpr != NULL; dpr = dpr->dpr_hash) {
if (dpr->dpr_pid == pid)
break;
else
dpp = &dpr->dpr_hash;
}
assert(dpr != NULL);
assert(dpr->dpr_proc == P);
if (remove)
*dpp = dpr->dpr_hash;
return (dpr);
}
static void
dt_proc_destroy(dtrace_hdl_t *dtp, struct ps_prochandle *P)
{
dt_proc_t *dpr = dt_proc_lookup(dtp, P, B_FALSE);
dt_proc_hash_t *dph = dtp->dt_procs;
dt_proc_notify_t *npr, **npp;
int rflag;
assert(dpr != NULL);
#if defined(illumos)
if (!(Pstatus(dpr->dpr_proc)->pr_flags & (PR_KLC | PR_RLC))) {
#else
if (!(proc_getflags(dpr->dpr_proc) & (PR_KLC | PR_RLC))) {
#endif
dt_dprintf("abandoning pid %d\n", (int)dpr->dpr_pid);
rflag = PRELEASE_HANG;
#if defined(illumos)
} else if (Pstatus(dpr->dpr_proc)->pr_flags & PR_KLC) {
#else
} else if (proc_getflags(dpr->dpr_proc) & PR_KLC) {
#endif
dt_dprintf("killing pid %d\n", (int)dpr->dpr_pid);
rflag = PRELEASE_KILL;
} else {
dt_dprintf("releasing pid %d\n", (int)dpr->dpr_pid);
rflag = 0;
}
if (dpr->dpr_tid) {
(void) pthread_mutex_lock(&dpr->dpr_lock);
dpr->dpr_quit = B_TRUE;
#if defined(illumos)
(void) _lwp_kill(dpr->dpr_tid, SIGCANCEL);
#else
pthread_kill(dpr->dpr_tid, SIGTHR);
#endif
if (dpr->dpr_stop & DT_PROC_STOP_IDLE) {
dt_proc_bpenable(dpr);
dpr->dpr_stop &= ~DT_PROC_STOP_IDLE;
(void) pthread_cond_broadcast(&dpr->dpr_cv);
}
while (!dpr->dpr_done)
(void) pthread_cond_wait(&dpr->dpr_cv, &dpr->dpr_lock);
(void) pthread_mutex_unlock(&dpr->dpr_lock);
}
(void) pthread_mutex_lock(&dph->dph_lock);
(void) dt_proc_lookup(dtp, P, B_TRUE);
npp = &dph->dph_notify;
while ((npr = *npp) != NULL) {
if (npr->dprn_dpr == dpr) {
*npp = npr->dprn_next;
dt_free(dtp, npr);
} else {
npp = &npr->dprn_next;
}
}
(void) pthread_mutex_unlock(&dph->dph_lock);
if (dpr->dpr_cacheable) {
assert(dph->dph_lrucnt != 0);
dph->dph_lrucnt--;
}
dt_list_delete(&dph->dph_lrulist, dpr);
Prelease(dpr->dpr_proc, rflag);
dt_free(dtp, dpr);
}
static int
dt_proc_create_thread(dtrace_hdl_t *dtp, dt_proc_t *dpr, uint_t stop)
{
dt_proc_control_data_t data;
sigset_t nset, oset;
pthread_attr_t a;
int err;
(void) pthread_mutex_lock(&dpr->dpr_lock);
dpr->dpr_stop |= stop;
(void) pthread_attr_init(&a);
(void) pthread_attr_setdetachstate(&a, PTHREAD_CREATE_DETACHED);
(void) sigfillset(&nset);
(void) sigdelset(&nset, SIGABRT);
#if defined(illumos)
(void) sigdelset(&nset, SIGCANCEL);
#else
(void) sigdelset(&nset, SIGUSR1);
#endif
data.dpcd_hdl = dtp;
data.dpcd_proc = dpr;
(void) pthread_sigmask(SIG_SETMASK, &nset, &oset);
err = pthread_create(&dpr->dpr_tid, &a, dt_proc_control, &data);
(void) pthread_sigmask(SIG_SETMASK, &oset, NULL);
if (err == 0) {
while (!dpr->dpr_done && !(dpr->dpr_stop & DT_PROC_STOP_IDLE))
(void) pthread_cond_wait(&dpr->dpr_cv, &dpr->dpr_lock);
if (dpr->dpr_done) {
#if defined(illumos)
const psinfo_t *prp = Ppsinfo(dpr->dpr_proc);
int stat = prp ? prp->pr_wstat : 0;
int pid = dpr->dpr_pid;
#else
int stat = proc_getwstat(dpr->dpr_proc);
int pid = proc_getpid(dpr->dpr_proc);
#endif
if (proc_state(dpr->dpr_proc) == PS_LOST) {
(void) dt_proc_error(dpr->dpr_hdl, dpr,
"failed to control pid %d: process exec'd "
"set-id or unobservable program\n", pid);
} else if (WIFSIGNALED(stat)) {
(void) dt_proc_error(dpr->dpr_hdl, dpr,
"failed to control pid %d: process died "
"from signal %d\n", pid, WTERMSIG(stat));
} else {
(void) dt_proc_error(dpr->dpr_hdl, dpr,
"failed to control pid %d: process exited "
"with status %d\n", pid, WEXITSTATUS(stat));
}
err = ESRCH;
}
} else {
(void) dt_proc_error(dpr->dpr_hdl, dpr,
"failed to create control thread for process-id %d: %s\n",
(int)dpr->dpr_pid, strerror(err));
}
if (err == 0)
(void) pthread_mutex_unlock(&dpr->dpr_lock);
(void) pthread_attr_destroy(&a);
return (err);
}
struct ps_prochandle *
dt_proc_create(dtrace_hdl_t *dtp, const char *file, char *const *argv,
proc_child_func *pcf, void *child_arg)
{
dt_proc_hash_t *dph = dtp->dt_procs;
dt_proc_t *dpr;
int err;
if ((dpr = dt_zalloc(dtp, sizeof (dt_proc_t))) == NULL)
return (NULL);
(void) pthread_mutex_init(&dpr->dpr_lock, NULL);
(void) pthread_cond_init(&dpr->dpr_cv, NULL);
#if defined(illumos)
dpr->dpr_proc = Pxcreate(file, argv, dtp->dt_proc_env, &err, NULL, 0);
if (dpr->dpr_proc == NULL) {
return (dt_proc_error(dtp, dpr,
"failed to execute %s: %s\n", file, Pcreate_error(err)));
}
#else
if ((err = proc_create(file, argv, dtp->dt_proc_env, pcf, child_arg,
&dpr->dpr_proc)) != 0) {
return (dt_proc_error(dtp, dpr,
"failed to execute %s: %s\n", file, Pcreate_error(err)));
}
#endif
dpr->dpr_hdl = dtp;
#if defined(illumos)
dpr->dpr_pid = Pstatus(dpr->dpr_proc)->pr_pid;
#else
dpr->dpr_pid = proc_getpid(dpr->dpr_proc);
#endif
(void) Punsetflags(dpr->dpr_proc, PR_RLC);
(void) Psetflags(dpr->dpr_proc, PR_KLC);
if (dt_proc_create_thread(dtp, dpr, dtp->dt_prcmode) != 0)
return (NULL);
dpr->dpr_hash = dph->dph_hash[dpr->dpr_pid & (dph->dph_hashlen - 1)];
dph->dph_hash[dpr->dpr_pid & (dph->dph_hashlen - 1)] = dpr;
dt_list_prepend(&dph->dph_lrulist, dpr);
dt_dprintf("created pid %d\n", (int)dpr->dpr_pid);
dpr->dpr_refs++;
return (dpr->dpr_proc);
}
struct ps_prochandle *
dt_proc_grab(dtrace_hdl_t *dtp, pid_t pid, int flags, int nomonitor)
{
dt_proc_hash_t *dph = dtp->dt_procs;
uint_t h = pid & (dph->dph_hashlen - 1);
dt_proc_t *dpr, *opr;
int err;
for (dpr = dph->dph_hash[h]; dpr != NULL; dpr = dpr->dpr_hash) {
if (dpr->dpr_pid == pid && !dpr->dpr_stale) {
if (dpr->dpr_rdonly && !(flags & PGRAB_RDONLY)) {
dt_dprintf("upgrading pid %d\n", (int)pid);
dpr->dpr_stale = B_TRUE;
dpr->dpr_cacheable = B_FALSE;
dph->dph_lrucnt--;
break;
}
dt_dprintf("grabbed pid %d (cached)\n", (int)pid);
dt_list_delete(&dph->dph_lrulist, dpr);
dt_list_prepend(&dph->dph_lrulist, dpr);
dpr->dpr_refs++;
return (dpr->dpr_proc);
}
}
if ((dpr = dt_zalloc(dtp, sizeof (dt_proc_t))) == NULL)
return (NULL);
(void) pthread_mutex_init(&dpr->dpr_lock, NULL);
(void) pthread_cond_init(&dpr->dpr_cv, NULL);
#if defined(illumos)
if ((dpr->dpr_proc = Pgrab(pid, flags, &err)) == NULL) {
#else
if ((err = proc_attach(pid, flags, &dpr->dpr_proc)) != 0) {
#endif
return (dt_proc_error(dtp, dpr,
"failed to grab pid %d: %s\n", (int)pid, Pgrab_error(err)));
}
dpr->dpr_hdl = dtp;
dpr->dpr_pid = pid;
(void) Punsetflags(dpr->dpr_proc, PR_KLC);
(void) Psetflags(dpr->dpr_proc, PR_RLC);
if (nomonitor || (flags & PGRAB_RDONLY)) {
if (dph->dph_lrucnt >= dph->dph_lrulim) {
for (opr = dt_list_prev(&dph->dph_lrulist);
opr != NULL; opr = dt_list_prev(opr)) {
if (opr->dpr_cacheable && opr->dpr_refs == 0) {
dt_proc_destroy(dtp, opr->dpr_proc);
break;
}
}
}
if (flags & PGRAB_RDONLY) {
dpr->dpr_cacheable = B_TRUE;
dpr->dpr_rdonly = B_TRUE;
dph->dph_lrucnt++;
}
} else if (dt_proc_create_thread(dtp, dpr, DT_PROC_STOP_GRAB) != 0)
return (NULL);
dpr->dpr_hash = dph->dph_hash[h];
dph->dph_hash[h] = dpr;
dt_list_prepend(&dph->dph_lrulist, dpr);
dt_dprintf("grabbed pid %d\n", (int)pid);
dpr->dpr_refs++;
return (dpr->dpr_proc);
}
void
dt_proc_release(dtrace_hdl_t *dtp, struct ps_prochandle *P)
{
dt_proc_t *dpr = dt_proc_lookup(dtp, P, B_FALSE);
dt_proc_hash_t *dph = dtp->dt_procs;
assert(dpr != NULL);
assert(dpr->dpr_refs != 0);
if (--dpr->dpr_refs == 0 &&
(!dpr->dpr_cacheable || dph->dph_lrucnt > dph->dph_lrulim))
dt_proc_destroy(dtp, P);
}
void
dt_proc_continue(dtrace_hdl_t *dtp, struct ps_prochandle *P)
{
dt_proc_t *dpr = dt_proc_lookup(dtp, P, B_FALSE);
(void) pthread_mutex_lock(&dpr->dpr_lock);
if (dpr->dpr_stop & DT_PROC_STOP_IDLE) {
dpr->dpr_stop &= ~DT_PROC_STOP_IDLE;
(void) pthread_cond_broadcast(&dpr->dpr_cv);
}
(void) pthread_mutex_unlock(&dpr->dpr_lock);
}
void
dt_proc_lock(dtrace_hdl_t *dtp, struct ps_prochandle *P)
{
dt_proc_t *dpr = dt_proc_lookup(dtp, P, B_FALSE);
int err = pthread_mutex_lock(&dpr->dpr_lock);
assert(err == 0);
}
void
dt_proc_unlock(dtrace_hdl_t *dtp, struct ps_prochandle *P)
{
dt_proc_t *dpr = dt_proc_lookup(dtp, P, B_FALSE);
int err = pthread_mutex_unlock(&dpr->dpr_lock);
assert(err == 0);
}
void
dt_proc_init(dtrace_hdl_t *dtp)
{
extern char **environ;
static char *envdef[] = {
"LD_NOLAZYLOAD=1",
NULL
};
char **p;
int i;
if ((dtp->dt_procs = dt_zalloc(dtp, sizeof (dt_proc_hash_t) +
sizeof (dt_proc_t *) * _dtrace_pidbuckets - 1)) == NULL)
return;
(void) pthread_mutex_init(&dtp->dt_procs->dph_lock, NULL);
(void) pthread_cond_init(&dtp->dt_procs->dph_cv, NULL);
dtp->dt_procs->dph_hashlen = _dtrace_pidbuckets;
dtp->dt_procs->dph_lrulim = _dtrace_pidlrulim;
for (i = 1, p = environ; *p != NULL; i++, p++)
continue;
for (p = envdef; *p != NULL; i++, p++)
continue;
if ((dtp->dt_proc_env = dt_zalloc(dtp, sizeof (char *) * i)) == NULL)
return;
for (i = 0, p = environ; *p != NULL; i++, p++) {
if ((dtp->dt_proc_env[i] = strdup(*p)) == NULL)
goto err;
}
for (p = envdef; *p != NULL; i++, p++) {
if ((dtp->dt_proc_env[i] = strdup(*p)) == NULL)
goto err;
}
return;
err:
while (--i != 0) {
dt_free(dtp, dtp->dt_proc_env[i]);
}
dt_free(dtp, dtp->dt_proc_env);
dtp->dt_proc_env = NULL;
}
void
dt_proc_fini(dtrace_hdl_t *dtp)
{
dt_proc_hash_t *dph = dtp->dt_procs;
dt_proc_t *dpr;
char **p;
while ((dpr = dt_list_next(&dph->dph_lrulist)) != NULL)
dt_proc_destroy(dtp, dpr->dpr_proc);
dtp->dt_procs = NULL;
dt_free(dtp, dph);
for (p = dtp->dt_proc_env; *p != NULL; p++)
dt_free(dtp, *p);
dt_free(dtp, dtp->dt_proc_env);
dtp->dt_proc_env = NULL;
}
struct ps_prochandle *
dtrace_proc_create(dtrace_hdl_t *dtp, const char *file, char *const *argv,
proc_child_func *pcf, void *child_arg)
{
dt_ident_t *idp = dt_idhash_lookup(dtp->dt_macros, "target");
struct ps_prochandle *P = dt_proc_create(dtp, file, argv, pcf, child_arg);
if (P != NULL && idp != NULL && idp->di_id == 0) {
#if defined(illumos)
idp->di_id = Pstatus(P)->pr_pid;
#else
idp->di_id = proc_getpid(P);
#endif
}
return (P);
}
struct ps_prochandle *
dtrace_proc_grab(dtrace_hdl_t *dtp, pid_t pid, int flags)
{
dt_ident_t *idp = dt_idhash_lookup(dtp->dt_macros, "target");
struct ps_prochandle *P = dt_proc_grab(dtp, pid, flags, 0);
if (P != NULL && idp != NULL && idp->di_id == 0)
idp->di_id = pid;
return (P);
}
void
dtrace_proc_release(dtrace_hdl_t *dtp, struct ps_prochandle *P)
{
dt_proc_release(dtp, P);
}
void
dtrace_proc_continue(dtrace_hdl_t *dtp, struct ps_prochandle *P)
{
dt_proc_continue(dtp, P);
}
