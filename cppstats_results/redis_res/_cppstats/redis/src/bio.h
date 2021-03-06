typedef void lazy_free_fn(void *args[]);
void bioInit(void);
unsigned long long bioPendingJobsOfType(int type);
unsigned long long bioWaitStepOfType(int type);
void bioKillThreads(void);
void bioCreateCloseJob(int fd);
void bioCreateFsyncJob(int fd);
void bioCreateLazyFreeJob(lazy_free_fn free_fn, int arg_count, ...);
#define BIO_CLOSE_FILE 0
#define BIO_AOF_FSYNC 1
#define BIO_LAZY_FREE 2
#define BIO_NUM_OPS 3
