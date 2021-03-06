

























#pragma ident "%Z%%M% %I% %E% SMI"












#include <pthread.h>
#if defined(illumos)
#include <synch.h>
#endif
#include <stdio.h>

#include "barrier.h"

void
barrier_init(barrier_t *bar, int nthreads)
{
pthread_mutex_init(&bar->bar_lock, NULL);
#if defined(illumos)
sema_init(&bar->bar_sem, 0, USYNC_THREAD, NULL);
#else
sem_init(&bar->bar_sem, 0, 0);
#endif

bar->bar_numin = 0;
bar->bar_nthr = nthreads;
}

int
barrier_wait(barrier_t *bar)
{
pthread_mutex_lock(&bar->bar_lock);

if (++bar->bar_numin < bar->bar_nthr) {
pthread_mutex_unlock(&bar->bar_lock);
#if defined(illumos)
sema_wait(&bar->bar_sem);
#else
sem_wait(&bar->bar_sem);
#endif

return (0);

} else {
int i;


bar->bar_numin = 0;
for (i = 1; i < bar->bar_nthr; i++)
#if defined(illumos)
sema_post(&bar->bar_sem);
#else
sem_post(&bar->bar_sem);
#endif
pthread_mutex_unlock(&bar->bar_lock);

return (1);
}
}
