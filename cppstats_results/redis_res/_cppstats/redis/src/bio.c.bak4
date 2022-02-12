



























































#include "server.h"
#include "bio.h"

static pthread_t bio_threads[BIO_NUM_OPS];
static pthread_mutex_t bio_mutex[BIO_NUM_OPS];
static pthread_cond_t bio_newjob_cond[BIO_NUM_OPS];
static pthread_cond_t bio_step_cond[BIO_NUM_OPS];
static list *bio_jobs[BIO_NUM_OPS];






static unsigned long long bio_pending[BIO_NUM_OPS];



struct bio_job {

int fd;
lazy_free_fn *free_fn;
void *free_args[];
};

void *bioProcessBackgroundJobs(void *arg);



#define REDIS_THREAD_STACK_SIZE (1024*1024*4)


void bioInit(void) {
pthread_attr_t attr;
pthread_t thread;
size_t stacksize;
int j;


for (j = 0; j < BIO_NUM_OPS; j++) {
pthread_mutex_init(&bio_mutex[j],NULL);
pthread_cond_init(&bio_newjob_cond[j],NULL);
pthread_cond_init(&bio_step_cond[j],NULL);
bio_jobs[j] = listCreate();
bio_pending[j] = 0;
}


pthread_attr_init(&attr);
pthread_attr_getstacksize(&attr,&stacksize);
if (!stacksize) stacksize = 1;
while (stacksize < REDIS_THREAD_STACK_SIZE) stacksize *= 2;
pthread_attr_setstacksize(&attr, stacksize);




for (j = 0; j < BIO_NUM_OPS; j++) {
void *arg = (void*)(unsigned long) j;
if (pthread_create(&thread,&attr,bioProcessBackgroundJobs,arg) != 0) {
serverLog(LL_WARNING,"Fatal: Can't initialize Background Jobs.");
exit(1);
}
bio_threads[j] = thread;
}
}

void bioSubmitJob(int type, struct bio_job *job) {
pthread_mutex_lock(&bio_mutex[type]);
listAddNodeTail(bio_jobs[type],job);
bio_pending[type]++;
pthread_cond_signal(&bio_newjob_cond[type]);
pthread_mutex_unlock(&bio_mutex[type]);
}

void bioCreateLazyFreeJob(lazy_free_fn free_fn, int arg_count, ...) {
va_list valist;


struct bio_job *job = zmalloc(sizeof(*job) + sizeof(void *) * (arg_count));
job->free_fn = free_fn;

va_start(valist, arg_count);
for (int i = 0; i < arg_count; i++) {
job->free_args[i] = va_arg(valist, void *);
}
va_end(valist);
bioSubmitJob(BIO_LAZY_FREE, job);
}

void bioCreateCloseJob(int fd) {
struct bio_job *job = zmalloc(sizeof(*job));
job->fd = fd;

bioSubmitJob(BIO_CLOSE_FILE, job);
}

void bioCreateFsyncJob(int fd) {
struct bio_job *job = zmalloc(sizeof(*job));
job->fd = fd;

bioSubmitJob(BIO_AOF_FSYNC, job);
}

void *bioProcessBackgroundJobs(void *arg) {
struct bio_job *job;
unsigned long type = (unsigned long) arg;
sigset_t sigset;


if (type >= BIO_NUM_OPS) {
serverLog(LL_WARNING,
"Warning: bio thread started with wrong type %lu",type);
return NULL;
}

switch (type) {
case BIO_CLOSE_FILE:
redis_set_thread_title("bio_close_file");
break;
case BIO_AOF_FSYNC:
redis_set_thread_title("bio_aof_fsync");
break;
case BIO_LAZY_FREE:
redis_set_thread_title("bio_lazy_free");
break;
}

redisSetCpuAffinity(server.bio_cpulist);

makeThreadKillable();

pthread_mutex_lock(&bio_mutex[type]);


sigemptyset(&sigset);
sigaddset(&sigset, SIGALRM);
if (pthread_sigmask(SIG_BLOCK, &sigset, NULL))
serverLog(LL_WARNING,
"Warning: can't mask SIGALRM in bio.c thread: %s", strerror(errno));

while(1) {
listNode *ln;


if (listLength(bio_jobs[type]) == 0) {
pthread_cond_wait(&bio_newjob_cond[type],&bio_mutex[type]);
continue;
}

ln = listFirst(bio_jobs[type]);
job = ln->value;


pthread_mutex_unlock(&bio_mutex[type]);


if (type == BIO_CLOSE_FILE) {
close(job->fd);
} else if (type == BIO_AOF_FSYNC) {



if (redis_fsync(job->fd) == -1 &&
errno != EBADF && errno != EINVAL)
{
int last_status;
atomicGet(server.aof_bio_fsync_status,last_status);
atomicSet(server.aof_bio_fsync_status,C_ERR);
atomicSet(server.aof_bio_fsync_errno,errno);
if (last_status == C_OK) {
serverLog(LL_WARNING,
"Fail to fsync the AOF file: %s",strerror(errno));
}
} else {
atomicSet(server.aof_bio_fsync_status,C_OK);
}
} else if (type == BIO_LAZY_FREE) {
job->free_fn(job->free_args);
} else {
serverPanic("Wrong job type in bioProcessBackgroundJobs().");
}
zfree(job);



pthread_mutex_lock(&bio_mutex[type]);
listDelNode(bio_jobs[type],ln);
bio_pending[type]--;


pthread_cond_broadcast(&bio_step_cond[type]);
}
}


unsigned long long bioPendingJobsOfType(int type) {
unsigned long long val;
pthread_mutex_lock(&bio_mutex[type]);
val = bio_pending[type];
pthread_mutex_unlock(&bio_mutex[type]);
return val;
}











unsigned long long bioWaitStepOfType(int type) {
unsigned long long val;
pthread_mutex_lock(&bio_mutex[type]);
val = bio_pending[type];
if (val != 0) {
pthread_cond_wait(&bio_step_cond[type],&bio_mutex[type]);
val = bio_pending[type];
}
pthread_mutex_unlock(&bio_mutex[type]);
return val;
}





void bioKillThreads(void) {
int err, j;

for (j = 0; j < BIO_NUM_OPS; j++) {
if (bio_threads[j] == pthread_self()) continue;
if (bio_threads[j] && pthread_cancel(bio_threads[j]) == 0) {
if ((err = pthread_join(bio_threads[j],NULL)) != 0) {
serverLog(LL_WARNING,
"Bio thread for job type #%d can not be joined: %s",
j, strerror(err));
} else {
serverLog(LL_WARNING,
"Bio thread for job type #%d terminated",j);
}
}
}
}
