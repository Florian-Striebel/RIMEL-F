
































typedef struct BuildMon {
char meta_fname[MAXPATHLEN];
struct filemon *filemon;
int mon_fd;
FILE *mfp;
} BuildMon;

struct Job;

void meta_init(void);
void meta_finish(void);
void meta_mode_init(const char *);
void meta_job_start(struct Job *, GNode *);
void meta_job_child(struct Job *);
void meta_job_parent(struct Job *, pid_t);
int meta_job_fd(struct Job *);
int meta_job_event(struct Job *);
void meta_job_error(struct Job *, GNode *, bool, int);
void meta_job_output(struct Job *, char *, const char *);
int meta_cmd_finish(void *);
int meta_job_finish(struct Job *);
bool meta_oodate(GNode *, bool);
void meta_compat_start(void);
void meta_compat_child(void);
void meta_compat_parent(pid_t);

extern bool useMeta;
