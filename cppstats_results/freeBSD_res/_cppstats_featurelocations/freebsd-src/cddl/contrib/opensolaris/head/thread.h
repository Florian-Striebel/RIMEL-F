



























#if !defined(_THREAD_H)
#define _THREAD_H

#include <pthread.h>
#include <pthread_np.h>
#include <assert.h>





typedef pthread_t thread_t;
typedef pthread_mutex_t mutex_t;
typedef pthread_cond_t cond_t;
typedef pthread_rwlock_t rwlock_t;

#define USYNC_THREAD 0

#define thr_self() (unsigned long)pthread_self()
#define thr_equal(a,b) pthread_equal(a,b)
#define thr_join(t,d,s) pthread_join(t,s)
#define thr_exit(r) pthread_exit(r)
#define _mutex_init(l,f,a) pthread_mutex_init(l,NULL)
#define _mutex_destroy(l) pthread_mutex_destroy(l)
#define mutex_lock(l) pthread_mutex_lock(l)
#define mutex_trylock(l) pthread_mutex_trylock(l)
#define mutex_unlock(l) pthread_mutex_unlock(l)
#define rwlock_init(l,f,a) pthread_rwlock_init(l,NULL)
#define rwlock_destroy(l) pthread_rwlock_destroy(l)
#define rw_rdlock(l) pthread_rwlock_rdlock(l)
#define rw_wrlock(l) pthread_rwlock_wrlock(l)
#define rw_tryrdlock(l) pthread_rwlock_tryrdlock(l)
#define rw_trywrlock(l) pthread_rwlock_trywrlock(l)
#define rw_unlock(l) pthread_rwlock_unlock(l)
#define cond_init(l,f,a) pthread_cond_init(l,NULL)
#define cond_destroy(l) pthread_cond_destroy(l)
#define cond_wait(l,m) pthread_cond_wait(l,m)
#define cond_signal(l) pthread_cond_signal(l)
#define cond_broadcast(l) pthread_cond_broadcast(l)

#define THR_BOUND 0x00000001
#define THR_NEW_LWP 0x00000002
#define THR_DETACHED 0x00000040
#define THR_SUSPENDED 0x00000080
#define THR_DAEMON 0x00000100

static __inline int
thr_create(void *stack_base, size_t stack_size, void *(*start_func) (void*),
void *arg, long flags, thread_t *new_thread_ID)
{
pthread_t dummy;
int ret;

assert(stack_base == NULL);
assert(stack_size == 0);
assert((flags & ~THR_BOUND & ~THR_DETACHED) == 0);

pthread_attr_t attr;
pthread_attr_init(&attr);

if (flags & THR_DETACHED)
pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

if (new_thread_ID == NULL)
new_thread_ID = &dummy;



ret = pthread_create(new_thread_ID, &attr, start_func, arg);

pthread_attr_destroy(&attr);

return (ret);
}

#endif
