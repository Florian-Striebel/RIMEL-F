#pragma ident "%Z%%M% %I% %E% SMI"
#include <pthread.h>
#if defined(__cplusplus)
extern "C" {
#endif
#include "ctftools.h"
#include "barrier.h"
#include "fifo.h"
typedef struct wip {
pthread_mutex_t wip_lock;
pthread_cond_t wip_cv;
tdata_t *wip_td;
int wip_nmerged;
int wip_batchid;
} wip_t;
typedef struct workqueue {
int wq_next_batchid;
int wq_maxbatchsz;
wip_t *wq_wip;
int wq_nwipslots;
int wq_nthreads;
int wq_ithrottle;
pthread_mutex_t wq_queue_lock;
fifo_t *wq_queue;
pthread_cond_t wq_work_avail;
pthread_cond_t wq_work_removed;
int wq_ninqueue;
int wq_nextpownum;
pthread_mutex_t wq_donequeue_lock;
fifo_t *wq_donequeue;
int wq_lastdonebatch;
pthread_cond_t wq_done_cv;
pthread_cond_t wq_alldone_cv;
int wq_alldone;
int wq_nomorefiles;
pthread_t *wq_thread;
barrier_t wq_bar1;
barrier_t wq_bar2;
} workqueue_t;
#if defined(__cplusplus)
}
#endif
