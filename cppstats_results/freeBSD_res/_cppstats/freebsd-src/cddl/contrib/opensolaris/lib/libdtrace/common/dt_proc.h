#include <libproc.h>
#include <dtrace.h>
#include <pthread.h>
#include <dt_list.h>
#if defined(__cplusplus)
extern "C" {
#endif
typedef struct dt_proc {
dt_list_t dpr_list;
struct dt_proc *dpr_hash;
dtrace_hdl_t *dpr_hdl;
struct ps_prochandle *dpr_proc;
char dpr_errmsg[BUFSIZ];
rd_agent_t *dpr_rtld;
pthread_mutex_t dpr_lock;
pthread_cond_t dpr_cv;
pid_t dpr_pid;
uint_t dpr_refs;
uint8_t dpr_cacheable;
uint8_t dpr_stop;
uint8_t dpr_quit;
uint8_t dpr_done;
uint8_t dpr_usdt;
uint8_t dpr_stale;
uint8_t dpr_rdonly;
pthread_t dpr_tid;
dt_list_t dpr_bps;
} dt_proc_t;
typedef struct dt_proc_notify {
dt_proc_t *dprn_dpr;
char dprn_errmsg[BUFSIZ];
struct dt_proc_notify *dprn_next;
} dt_proc_notify_t;
#define DT_PROC_STOP_IDLE 0x01
#define DT_PROC_STOP_CREATE 0x02
#define DT_PROC_STOP_GRAB 0x04
#define DT_PROC_STOP_PREINIT 0x08
#define DT_PROC_STOP_POSTINIT 0x10
#define DT_PROC_STOP_MAIN 0x20
typedef void dt_bkpt_f(dtrace_hdl_t *, dt_proc_t *, void *);
typedef struct dt_bkpt {
dt_list_t dbp_list;
dt_bkpt_f *dbp_func;
void *dbp_data;
uintptr_t dbp_addr;
ulong_t dbp_instr;
ulong_t dbp_hits;
int dbp_active;
} dt_bkpt_t;
typedef struct dt_proc_hash {
pthread_mutex_t dph_lock;
pthread_cond_t dph_cv;
dt_proc_notify_t *dph_notify;
dt_list_t dph_lrulist;
uint_t dph_lrulim;
uint_t dph_lrucnt;
uint_t dph_hashlen;
dt_proc_t *dph_hash[1];
} dt_proc_hash_t;
extern struct ps_prochandle *dt_proc_create(dtrace_hdl_t *,
const char *, char *const *, proc_child_func *, void *);
extern struct ps_prochandle *dt_proc_grab(dtrace_hdl_t *, pid_t, int, int);
extern void dt_proc_release(dtrace_hdl_t *, struct ps_prochandle *);
extern void dt_proc_continue(dtrace_hdl_t *, struct ps_prochandle *);
extern void dt_proc_lock(dtrace_hdl_t *, struct ps_prochandle *);
extern void dt_proc_unlock(dtrace_hdl_t *, struct ps_prochandle *);
extern dt_proc_t *dt_proc_lookup(dtrace_hdl_t *, struct ps_prochandle *, int);
extern void dt_proc_init(dtrace_hdl_t *);
extern void dt_proc_fini(dtrace_hdl_t *);
#if defined(__cplusplus)
}
#endif
