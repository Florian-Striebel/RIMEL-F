#pragma ident "%Z%%M% %I% %E% SMI"
#include <sys/types.h>
#include <thread.h>
#include <pthread.h>
#if defined(__cplusplus)
extern "C" {
#endif
typedef struct tpool tpool_t;
#if defined(__STDC__)
extern tpool_t *tpool_create(uint_t min_threads, uint_t max_threads,
uint_t linger, pthread_attr_t *attr);
extern int tpool_dispatch(tpool_t *tpool,
void (*func)(void *), void *arg);
extern void tpool_destroy(tpool_t *tpool);
extern void tpool_abandon(tpool_t *tpool);
extern void tpool_wait(tpool_t *tpool);
extern void tpool_suspend(tpool_t *tpool);
extern int tpool_suspended(tpool_t *tpool);
extern void tpool_resume(tpool_t *tpool);
extern int tpool_member(tpool_t *tpool);
#else
extern tpool_t *tpool_create();
extern int tpool_dispatch();
extern void tpool_destroy();
extern void tpool_abandon();
extern void tpool_wait();
extern void tpool_suspend();
extern int tpool_suspended();
extern void tpool_resume();
extern int tpool_member();
#endif
#if defined(__cplusplus)
}
#endif
