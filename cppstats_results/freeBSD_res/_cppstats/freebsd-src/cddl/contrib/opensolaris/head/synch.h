#if !defined(_ASM)
#include <sys/machlock.h>
#include <sys/time_impl.h>
#include <sys/synch.h>
#endif
#if defined(__cplusplus)
extern "C" {
#endif
#if !defined(_ASM)
typedef struct _sema {
uint32_t count;
uint16_t type;
uint16_t magic;
upad64_t pad1[3];
upad64_t pad2[2];
} sema_t;
typedef lwp_mutex_t mutex_t;
typedef lwp_cond_t cond_t;
typedef struct _rwlock {
int32_t readers;
uint16_t type;
uint16_t magic;
mutex_t mutex;
cond_t readercv;
cond_t writercv;
} rwlock_t;
int _lwp_mutex_lock(lwp_mutex_t *);
int _lwp_mutex_unlock(lwp_mutex_t *);
int _lwp_mutex_trylock(lwp_mutex_t *);
int _lwp_cond_wait(lwp_cond_t *, lwp_mutex_t *);
int _lwp_cond_timedwait(lwp_cond_t *, lwp_mutex_t *, timespec_t *);
int _lwp_cond_reltimedwait(lwp_cond_t *, lwp_mutex_t *, timespec_t *);
int _lwp_cond_signal(lwp_cond_t *);
int _lwp_cond_broadcast(lwp_cond_t *);
int _lwp_sema_init(lwp_sema_t *, int);
int _lwp_sema_wait(lwp_sema_t *);
int _lwp_sema_trywait(lwp_sema_t *);
int _lwp_sema_post(lwp_sema_t *);
int cond_init(cond_t *, int, void *);
int cond_destroy(cond_t *);
int cond_wait(cond_t *, mutex_t *);
int cond_timedwait(cond_t *, mutex_t *, const timespec_t *);
int cond_reltimedwait(cond_t *, mutex_t *, const timespec_t *);
int cond_signal(cond_t *);
int cond_broadcast(cond_t *);
int mutex_init(mutex_t *, int, void *);
int mutex_destroy(mutex_t *);
int mutex_consistent(mutex_t *);
int mutex_lock(mutex_t *);
int mutex_trylock(mutex_t *);
int mutex_unlock(mutex_t *);
int rwlock_init(rwlock_t *, int, void *);
int rwlock_destroy(rwlock_t *);
int rw_rdlock(rwlock_t *);
int rw_wrlock(rwlock_t *);
int rw_unlock(rwlock_t *);
int rw_tryrdlock(rwlock_t *);
int rw_trywrlock(rwlock_t *);
int sema_init(sema_t *, unsigned int, int, void *);
int sema_destroy(sema_t *);
int sema_wait(sema_t *);
int sema_timedwait(sema_t *, const timespec_t *);
int sema_reltimedwait(sema_t *, const timespec_t *);
int sema_post(sema_t *);
int sema_trywait(sema_t *);
#endif
#define MUTEX_MAGIC _MUTEX_MAGIC
#define SEMA_MAGIC _SEMA_MAGIC
#define COND_MAGIC _COND_MAGIC
#define RWL_MAGIC _RWL_MAGIC
#define DEFAULTMUTEX {{0, 0, 0, {USYNC_THREAD}, MUTEX_MAGIC}, {{{0, 0, 0, 0, 0, 0, 0, 0}}}, 0}
#define SHAREDMUTEX {{0, 0, 0, {USYNC_PROCESS}, MUTEX_MAGIC}, {{{0, 0, 0, 0, 0, 0, 0, 0}}}, 0}
#define RECURSIVEMUTEX {{0, 0, 0, {USYNC_THREAD|LOCK_RECURSIVE}, MUTEX_MAGIC}, {{{0, 0, 0, 0, 0, 0, 0, 0}}}, 0}
#define ERRORCHECKMUTEX {{0, 0, 0, {USYNC_THREAD|LOCK_ERRORCHECK}, MUTEX_MAGIC}, {{{0, 0, 0, 0, 0, 0, 0, 0}}}, 0}
#define RECURSIVE_ERRORCHECKMUTEX {{0, 0, 0, {USYNC_THREAD|LOCK_RECURSIVE|LOCK_ERRORCHECK}, MUTEX_MAGIC}, {{{0, 0, 0, 0, 0, 0, 0, 0}}}, 0}
#define DEFAULTCV {{{0, 0, 0, 0}, USYNC_THREAD, COND_MAGIC}, 0}
#define SHAREDCV {{{0, 0, 0, 0}, USYNC_PROCESS, COND_MAGIC}, 0}
#define DEFAULTSEMA {0, USYNC_THREAD, SEMA_MAGIC, {0, 0, 0}, {0, 0}}
#define SHAREDSEMA {0, USYNC_PROCESS, SEMA_MAGIC, {0, 0, 0}, {0, 0}}
#define DEFAULTRWLOCK {0, USYNC_THREAD, RWL_MAGIC, DEFAULTMUTEX, DEFAULTCV, DEFAULTCV}
#define SHAREDRWLOCK {0, USYNC_PROCESS, RWL_MAGIC, SHAREDMUTEX, SHAREDCV, SHAREDCV}
#define SEMA_HELD(x) _sema_held(x)
#define RW_READ_HELD(x) _rw_read_held(x)
#define RW_WRITE_HELD(x) _rw_write_held(x)
#define RW_LOCK_HELD(x) (RW_READ_HELD(x) || RW_WRITE_HELD(x))
#define MUTEX_HELD(x) _mutex_held(x)
#if !defined(NO_LOCKS_HELD)
#define NO_LOCKS_HELD 1
#endif
#if !defined(NO_COMPETING_THREADS)
#define NO_COMPETING_THREADS 1
#endif
#if !defined(_ASM)
int _sema_held(void *);
int _rw_read_held(void *);
int _rw_write_held(void *);
int _mutex_held(void *);
void smt_pause(void);
#endif
#if defined(__cplusplus)
}
#endif
