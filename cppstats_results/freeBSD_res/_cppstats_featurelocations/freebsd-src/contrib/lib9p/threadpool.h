


























#if !defined(LIB9P_THREADPOOL_H)
#define LIB9P_THREADPOOL_H

#include <stdbool.h>
#include <pthread.h>
#include <sys/queue.h>
#include "lib9p.h"

STAILQ_HEAD(l9p_request_queue, l9p_request);









struct l9p_threadpool {
struct l9p_connection * ltp_conn;
struct l9p_request_queue ltp_workq;
struct l9p_request_queue ltp_replyq;
pthread_mutex_t ltp_mtx;
pthread_cond_t ltp_work_cv;
pthread_cond_t ltp_reply_cv;
LIST_HEAD(, l9p_worker) ltp_workers;
};







struct l9p_worker {
struct l9p_threadpool * ltw_tp;
pthread_t ltw_thread;
bool ltw_exiting;
bool ltw_responder;
LIST_ENTRY(l9p_worker) ltw_link;
};







enum l9p_workstate {
L9P_WS_NOTSTARTED,
L9P_WS_IMMEDIATE,
L9P_WS_INPROGRESS,
L9P_WS_RESPQUEUED,
L9P_WS_REPLYING,
};






















enum l9p_flushstate {
L9P_FLUSH_NONE = 0,
L9P_FLUSH_REQUESTED_PRE_START,
L9P_FLUSH_REQUESTED_POST_START,
L9P_FLUSH_TOOLATE
};

void l9p_threadpool_flushee_done(struct l9p_request *);
int l9p_threadpool_init(struct l9p_threadpool *, int);
void l9p_threadpool_run(struct l9p_threadpool *, struct l9p_request *);
int l9p_threadpool_shutdown(struct l9p_threadpool *);
int l9p_threadpool_tflush(struct l9p_request *);

#endif
