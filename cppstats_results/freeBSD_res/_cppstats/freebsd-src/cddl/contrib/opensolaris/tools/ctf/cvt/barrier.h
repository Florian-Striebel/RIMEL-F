#pragma ident "%Z%%M% %I% %E% SMI"
#if defined(illumos)
#include <synch.h>
#else
#include <semaphore.h>
typedef sem_t sema_t;
#endif
#if defined(__cplusplus)
extern "C" {
#endif
typedef struct barrier {
pthread_mutex_t bar_lock;
int bar_numin;
sema_t bar_sem;
int bar_nthr;
} barrier_t;
extern void barrier_init(barrier_t *, int);
extern int barrier_wait(barrier_t *);
#if defined(__cplusplus)
}
#endif
