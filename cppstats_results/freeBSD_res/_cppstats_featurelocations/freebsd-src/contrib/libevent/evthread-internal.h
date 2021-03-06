
























#if !defined(EVTHREAD_INTERNAL_H_INCLUDED_)
#define EVTHREAD_INTERNAL_H_INCLUDED_

#if defined(__cplusplus)
extern "C" {
#endif

#include "event2/event-config.h"
#include "evconfig-private.h"

#include "event2/thread.h"
#include "util-internal.h"

struct event_base;

#if !defined(_WIN32)




#define EVTHREAD_EXPOSE_STRUCTS
#endif

#if ! defined(EVENT__DISABLE_THREAD_SUPPORT) && defined(EVTHREAD_EXPOSE_STRUCTS)


extern struct evthread_lock_callbacks evthread_lock_fns_;
extern struct evthread_condition_callbacks evthread_cond_fns_;
extern unsigned long (*evthread_id_fn_)(void);
extern int evthread_lock_debugging_enabled_;


#define EVTHREAD_GET_ID() (evthread_id_fn_ ? evthread_id_fn_() : 1)




#define EVBASE_IN_THREAD(base) (evthread_id_fn_ == NULL || (base)->th_owner_id == evthread_id_fn_())






#define EVBASE_NEED_NOTIFY(base) (evthread_id_fn_ != NULL && (base)->running_loop && (base)->th_owner_id != evthread_id_fn_())






#define EVTHREAD_ALLOC_LOCK(lockvar, locktype) ((lockvar) = evthread_lock_fns_.alloc ? evthread_lock_fns_.alloc(locktype) : NULL)




#define EVTHREAD_FREE_LOCK(lockvar, locktype) do { void *lock_tmp_ = (lockvar); if (lock_tmp_ && evthread_lock_fns_.free) evthread_lock_fns_.free(lock_tmp_, (locktype)); } while (0)







#define EVLOCK_LOCK(lockvar,mode) do { if (lockvar) evthread_lock_fns_.lock(mode, lockvar); } while (0)






#define EVLOCK_UNLOCK(lockvar,mode) do { if (lockvar) evthread_lock_fns_.unlock(mode, lockvar); } while (0)






#define EVLOCK_SORTLOCKS_(lockvar1, lockvar2) do { if (lockvar1 && lockvar2 && lockvar1 > lockvar2) { void *tmp = lockvar1; lockvar1 = lockvar2; lockvar2 = tmp; } } while (0)










#define EVBASE_ACQUIRE_LOCK(base, lockvar) do { EVLOCK_LOCK((base)->lockvar, 0); } while (0)




#define EVBASE_RELEASE_LOCK(base, lockvar) do { EVLOCK_UNLOCK((base)->lockvar, 0); } while (0)





#define EVLOCK_ASSERT_LOCKED(lock) do { if ((lock) && evthread_lock_debugging_enabled_) { EVUTIL_ASSERT(evthread_is_debug_lock_held_(lock)); } } while (0)








static inline int EVLOCK_TRY_LOCK_(void *lock);
static inline int
EVLOCK_TRY_LOCK_(void *lock)
{
if (lock && evthread_lock_fns_.lock) {
int r = evthread_lock_fns_.lock(EVTHREAD_TRY, lock);
return !r;
} else {


return 1;
}
}


#define EVTHREAD_ALLOC_COND(condvar) do { (condvar) = evthread_cond_fns_.alloc_condition ? evthread_cond_fns_.alloc_condition(0) : NULL; } while (0)





#define EVTHREAD_FREE_COND(cond) do { if (cond) evthread_cond_fns_.free_condition((cond)); } while (0)





#define EVTHREAD_COND_SIGNAL(cond) ( (cond) ? evthread_cond_fns_.signal_condition((cond), 0) : 0 )


#define EVTHREAD_COND_BROADCAST(cond) ( (cond) ? evthread_cond_fns_.signal_condition((cond), 1) : 0 )





#define EVTHREAD_COND_WAIT(cond, lock) ( (cond) ? evthread_cond_fns_.wait_condition((cond), (lock), NULL) : 0 )



#define EVTHREAD_COND_WAIT_TIMED(cond, lock, tv) ( (cond) ? evthread_cond_fns_.wait_condition((cond), (lock), (tv)) : 0 )



#define EVTHREAD_LOCKING_ENABLED() (evthread_lock_fns_.lock != NULL)


#elif ! defined(EVENT__DISABLE_THREAD_SUPPORT)

unsigned long evthreadimpl_get_id_(void);
int evthreadimpl_is_lock_debugging_enabled_(void);
void *evthreadimpl_lock_alloc_(unsigned locktype);
void evthreadimpl_lock_free_(void *lock, unsigned locktype);
int evthreadimpl_lock_lock_(unsigned mode, void *lock);
int evthreadimpl_lock_unlock_(unsigned mode, void *lock);
void *evthreadimpl_cond_alloc_(unsigned condtype);
void evthreadimpl_cond_free_(void *cond);
int evthreadimpl_cond_signal_(void *cond, int broadcast);
int evthreadimpl_cond_wait_(void *cond, void *lock, const struct timeval *tv);
int evthreadimpl_locking_enabled_(void);

#define EVTHREAD_GET_ID() evthreadimpl_get_id_()
#define EVBASE_IN_THREAD(base) ((base)->th_owner_id == evthreadimpl_get_id_())

#define EVBASE_NEED_NOTIFY(base) ((base)->running_loop && ((base)->th_owner_id != evthreadimpl_get_id_()))



#define EVTHREAD_ALLOC_LOCK(lockvar, locktype) ((lockvar) = evthreadimpl_lock_alloc_(locktype))


#define EVTHREAD_FREE_LOCK(lockvar, locktype) do { void *lock_tmp_ = (lockvar); if (lock_tmp_) evthreadimpl_lock_free_(lock_tmp_, (locktype)); } while (0)







#define EVLOCK_LOCK(lockvar,mode) do { if (lockvar) evthreadimpl_lock_lock_(mode, lockvar); } while (0)






#define EVLOCK_UNLOCK(lockvar,mode) do { if (lockvar) evthreadimpl_lock_unlock_(mode, lockvar); } while (0)







#define EVBASE_ACQUIRE_LOCK(base, lockvar) do { EVLOCK_LOCK((base)->lockvar, 0); } while (0)




#define EVBASE_RELEASE_LOCK(base, lockvar) do { EVLOCK_UNLOCK((base)->lockvar, 0); } while (0)





#define EVLOCK_ASSERT_LOCKED(lock) do { if ((lock) && evthreadimpl_is_lock_debugging_enabled_()) { EVUTIL_ASSERT(evthread_is_debug_lock_held_(lock)); } } while (0)








static inline int EVLOCK_TRY_LOCK_(void *lock);
static inline int
EVLOCK_TRY_LOCK_(void *lock)
{
if (lock) {
int r = evthreadimpl_lock_lock_(EVTHREAD_TRY, lock);
return !r;
} else {


return 1;
}
}


#define EVTHREAD_ALLOC_COND(condvar) do { (condvar) = evthreadimpl_cond_alloc_(0); } while (0)




#define EVTHREAD_FREE_COND(cond) do { if (cond) evthreadimpl_cond_free_((cond)); } while (0)





#define EVTHREAD_COND_SIGNAL(cond) ( (cond) ? evthreadimpl_cond_signal_((cond), 0) : 0 )


#define EVTHREAD_COND_BROADCAST(cond) ( (cond) ? evthreadimpl_cond_signal_((cond), 1) : 0 )





#define EVTHREAD_COND_WAIT(cond, lock) ( (cond) ? evthreadimpl_cond_wait_((cond), (lock), NULL) : 0 )



#define EVTHREAD_COND_WAIT_TIMED(cond, lock, tv) ( (cond) ? evthreadimpl_cond_wait_((cond), (lock), (tv)) : 0 )


#define EVTHREAD_LOCKING_ENABLED() (evthreadimpl_locking_enabled_())


#else

#define EVTHREAD_GET_ID() 1
#define EVTHREAD_ALLOC_LOCK(lockvar, locktype) EVUTIL_NIL_STMT_
#define EVTHREAD_FREE_LOCK(lockvar, locktype) EVUTIL_NIL_STMT_

#define EVLOCK_LOCK(lockvar, mode) EVUTIL_NIL_STMT_
#define EVLOCK_UNLOCK(lockvar, mode) EVUTIL_NIL_STMT_
#define EVLOCK_LOCK2(lock1,lock2,mode1,mode2) EVUTIL_NIL_STMT_
#define EVLOCK_UNLOCK2(lock1,lock2,mode1,mode2) EVUTIL_NIL_STMT_

#define EVBASE_IN_THREAD(base) 1
#define EVBASE_NEED_NOTIFY(base) 0
#define EVBASE_ACQUIRE_LOCK(base, lock) EVUTIL_NIL_STMT_
#define EVBASE_RELEASE_LOCK(base, lock) EVUTIL_NIL_STMT_
#define EVLOCK_ASSERT_LOCKED(lock) EVUTIL_NIL_STMT_

#define EVLOCK_TRY_LOCK_(lock) 1

#define EVTHREAD_ALLOC_COND(condvar) EVUTIL_NIL_STMT_
#define EVTHREAD_FREE_COND(cond) EVUTIL_NIL_STMT_
#define EVTHREAD_COND_SIGNAL(cond) EVUTIL_NIL_STMT_
#define EVTHREAD_COND_BROADCAST(cond) EVUTIL_NIL_STMT_
#define EVTHREAD_COND_WAIT(cond, lock) EVUTIL_NIL_STMT_
#define EVTHREAD_COND_WAIT_TIMED(cond, lock, howlong) EVUTIL_NIL_STMT_

#define EVTHREAD_LOCKING_ENABLED() 0

#endif


#if ! defined(EVENT__DISABLE_THREAD_SUPPORT)

#define EVLOCK_SORTLOCKS_(lockvar1, lockvar2) do { if (lockvar1 && lockvar2 && lockvar1 > lockvar2) { void *tmp = lockvar1; lockvar1 = lockvar2; lockvar2 = tmp; } } while (0)










#define EVLOCK_LOCK2(lock1,lock2,mode1,mode2) do { void *lock1_tmplock_ = (lock1); void *lock2_tmplock_ = (lock2); EVLOCK_SORTLOCKS_(lock1_tmplock_,lock2_tmplock_); EVLOCK_LOCK(lock1_tmplock_,mode1); if (lock2_tmplock_ != lock1_tmplock_) EVLOCK_LOCK(lock2_tmplock_,mode2); } while (0)









#define EVLOCK_UNLOCK2(lock1,lock2,mode1,mode2) do { void *lock1_tmplock_ = (lock1); void *lock2_tmplock_ = (lock2); EVLOCK_SORTLOCKS_(lock1_tmplock_,lock2_tmplock_); if (lock2_tmplock_ != lock1_tmplock_) EVLOCK_UNLOCK(lock2_tmplock_,mode2); EVLOCK_UNLOCK(lock1_tmplock_,mode1); } while (0)









int evthread_is_debug_lock_held_(void *lock);
void *evthread_debug_get_real_lock_(void *lock);

void *evthread_setup_global_lock_(void *lock_, unsigned locktype,
int enable_locks);

#define EVTHREAD_SETUP_GLOBAL_LOCK(lockvar, locktype) do { lockvar = evthread_setup_global_lock_(lockvar, (locktype), enable_locks); if (!lockvar) { event_warn("Couldn't allocate %s", #lockvar); return -1; } } while (0);









int event_global_setup_locks_(const int enable_locks);
int evsig_global_setup_locks_(const int enable_locks);
int evutil_global_setup_locks_(const int enable_locks);
int evutil_secure_rng_global_setup_locks_(const int enable_locks);


struct evthread_lock_callbacks *evthread_get_lock_callbacks(void);

struct evthread_condition_callbacks *evthread_get_condition_callbacks(void);

void evthreadimpl_disable_lock_debugging_(void);

#endif

#if defined(__cplusplus)
}
#endif

#endif
