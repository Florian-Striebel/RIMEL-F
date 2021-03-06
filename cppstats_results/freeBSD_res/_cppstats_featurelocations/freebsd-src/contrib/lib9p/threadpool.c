


























#include <errno.h>
#include <stdlib.h>
#include <pthread.h>
#if defined(__FreeBSD__)
#include <pthread_np.h>
#endif
#include <sys/queue.h>
#include "lib9p.h"
#include "threadpool.h"

static void l9p_threadpool_rflush(struct l9p_threadpool *tp,
struct l9p_request *req);

static void *
l9p_responder(void *arg)
{
struct l9p_threadpool *tp;
struct l9p_worker *worker = arg;
struct l9p_request *req;

tp = worker->ltw_tp;
for (;;) {

pthread_mutex_lock(&tp->ltp_mtx);
while (STAILQ_EMPTY(&tp->ltp_replyq) && !worker->ltw_exiting)
pthread_cond_wait(&tp->ltp_reply_cv, &tp->ltp_mtx);
if (worker->ltw_exiting) {
pthread_mutex_unlock(&tp->ltp_mtx);
break;
}


req = STAILQ_FIRST(&tp->ltp_replyq);
STAILQ_REMOVE_HEAD(&tp->ltp_replyq, lr_worklink);


req->lr_workstate = L9P_WS_REPLYING;


if (req->lr_flushstate != L9P_FLUSH_NONE)
l9p_threadpool_rflush(tp, req);

pthread_mutex_unlock(&tp->ltp_mtx);


l9p_respond(req, false, true);
}
return (NULL);
}

static void *
l9p_worker(void *arg)
{
struct l9p_threadpool *tp;
struct l9p_worker *worker = arg;
struct l9p_request *req;

tp = worker->ltw_tp;
pthread_mutex_lock(&tp->ltp_mtx);
for (;;) {
while (STAILQ_EMPTY(&tp->ltp_workq) && !worker->ltw_exiting)
pthread_cond_wait(&tp->ltp_work_cv, &tp->ltp_mtx);
if (worker->ltw_exiting)
break;


req = STAILQ_FIRST(&tp->ltp_workq);
STAILQ_REMOVE_HEAD(&tp->ltp_workq, lr_worklink);
req->lr_workstate = L9P_WS_INPROGRESS;
req->lr_worker = worker;
pthread_mutex_unlock(&tp->ltp_mtx);


req->lr_error = l9p_dispatch_request(req);


pthread_mutex_lock(&tp->ltp_mtx);
req->lr_workstate = L9P_WS_RESPQUEUED;
req->lr_worker = NULL;
STAILQ_INSERT_TAIL(&tp->ltp_replyq, req, lr_worklink);


pthread_cond_signal(&tp->ltp_reply_cv);
}
pthread_mutex_unlock(&tp->ltp_mtx);
return (NULL);
}







static void
l9p_threadpool_rflush(struct l9p_threadpool *tp, struct l9p_request *req)
{
struct l9p_request *flusher;

















STAILQ_FOREACH(flusher, &req->lr_flushq, lr_flushlink) {
flusher->lr_workstate = L9P_WS_RESPQUEUED;
#if defined(notdef)
if (not the last) {
flusher->lr_flushstate = L9P_FLUSH_NOT_RUN;

}
#endif
STAILQ_INSERT_TAIL(&tp->ltp_replyq, flusher, lr_worklink);
}
}

int
l9p_threadpool_init(struct l9p_threadpool *tp, int size)
{
struct l9p_worker *worker;
#if defined(__FreeBSD__)
char threadname[16];
#endif
int error;
int i, nworkers, nresponders;

if (size <= 0)
return (EINVAL);
error = pthread_mutex_init(&tp->ltp_mtx, NULL);
if (error)
return (error);
error = pthread_cond_init(&tp->ltp_work_cv, NULL);
if (error)
goto fail_work_cv;
error = pthread_cond_init(&tp->ltp_reply_cv, NULL);
if (error)
goto fail_reply_cv;

STAILQ_INIT(&tp->ltp_workq);
STAILQ_INIT(&tp->ltp_replyq);
LIST_INIT(&tp->ltp_workers);

nresponders = 0;
nworkers = 0;
for (i = 0; i <= size; i++) {
worker = calloc(1, sizeof(struct l9p_worker));
worker->ltw_tp = tp;
worker->ltw_responder = i == 0;
error = pthread_create(&worker->ltw_thread, NULL,
worker->ltw_responder ? l9p_responder : l9p_worker,
(void *)worker);
if (error) {
free(worker);
break;
}
if (worker->ltw_responder)
nresponders++;
else
nworkers++;

#if defined(__FreeBSD__)
if (worker->ltw_responder) {
pthread_set_name_np(worker->ltw_thread, "9p-responder");
} else {
sprintf(threadname, "9p-worker:%d", i - 1);
pthread_set_name_np(worker->ltw_thread, threadname);
}
#endif

LIST_INSERT_HEAD(&tp->ltp_workers, worker, ltw_link);
}
if (nresponders == 0 || nworkers == 0) {

l9p_threadpool_shutdown(tp);
return (error);
}
return (0);







fail_reply_cv:
pthread_cond_destroy(&tp->ltp_work_cv);
fail_work_cv:
pthread_mutex_destroy(&tp->ltp_mtx);

return (error);
}




void
l9p_threadpool_run(struct l9p_threadpool *tp, struct l9p_request *req)
{






if (req->lr_req.hdr.type == L9P_TFLUSH) {

req->lr_workstate = L9P_WS_IMMEDIATE;
(void) l9p_dispatch_request(req);
} else {
pthread_mutex_lock(&tp->ltp_mtx);
req->lr_workstate = L9P_WS_NOTSTARTED;
STAILQ_INSERT_TAIL(&tp->ltp_workq, req, lr_worklink);
pthread_cond_signal(&tp->ltp_work_cv);
pthread_mutex_unlock(&tp->ltp_mtx);
}
}





int
l9p_threadpool_tflush(struct l9p_request *req)
{
struct l9p_connection *conn;
struct l9p_threadpool *tp;
struct l9p_request *flushee;
uint16_t oldtag;
enum l9p_flushstate nstate;




req->lr_error = 0;
conn = req->lr_conn;
tp = &conn->lc_tp;
oldtag = req->lr_req.tflush.oldtag;
ht_wrlock(&conn->lc_requests);
flushee = ht_find_locked(&conn->lc_requests, oldtag);
if (flushee == NULL) {





ht_unlock(&conn->lc_requests);
pthread_mutex_lock(&tp->ltp_mtx);
goto done;
}





pthread_mutex_lock(&tp->ltp_mtx);
ht_unlock(&conn->lc_requests);

switch (flushee->lr_workstate) {

case L9P_WS_NOTSTARTED:























nstate = L9P_FLUSH_REQUESTED_PRE_START;
break;

case L9P_WS_IMMEDIATE:







nstate = L9P_FLUSH_REQUESTED_POST_START;
break;

case L9P_WS_INPROGRESS:







#if defined(notyet)
pthread_kill(...);
#endif
nstate = L9P_FLUSH_REQUESTED_POST_START;
break;

case L9P_WS_RESPQUEUED:





nstate = L9P_FLUSH_TOOLATE;
break;

case L9P_WS_REPLYING:









goto done;
}








if (flushee->lr_flushstate == L9P_FLUSH_NONE)
STAILQ_INIT(&flushee->lr_flushq);
flushee->lr_flushstate = nstate;
STAILQ_INSERT_TAIL(&flushee->lr_flushq, req, lr_flushlink);

pthread_mutex_unlock(&tp->ltp_mtx);

return (0);

done:




req->lr_workstate = L9P_WS_RESPQUEUED;
STAILQ_INSERT_TAIL(&tp->ltp_replyq, req, lr_worklink);
pthread_mutex_unlock(&tp->ltp_mtx);
pthread_cond_signal(&tp->ltp_reply_cv);
return (0);
}

int
l9p_threadpool_shutdown(struct l9p_threadpool *tp)
{
struct l9p_worker *worker, *tmp;

LIST_FOREACH_SAFE(worker, &tp->ltp_workers, ltw_link, tmp) {
pthread_mutex_lock(&tp->ltp_mtx);
worker->ltw_exiting = true;
if (worker->ltw_responder)
pthread_cond_signal(&tp->ltp_reply_cv);
else
pthread_cond_broadcast(&tp->ltp_work_cv);
pthread_mutex_unlock(&tp->ltp_mtx);
pthread_join(worker->ltw_thread, NULL);
LIST_REMOVE(worker, ltw_link);
free(worker);
}
pthread_cond_destroy(&tp->ltp_reply_cv);
pthread_cond_destroy(&tp->ltp_work_cv);
pthread_mutex_destroy(&tp->ltp_mtx);

return (0);
}
