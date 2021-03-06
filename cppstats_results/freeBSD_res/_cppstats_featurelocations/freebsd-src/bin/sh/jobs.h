



































#define FORK_FG 0
#define FORK_BG 1
#define FORK_NOJOB 2

#include <signal.h>

struct job;

enum {
SHOWJOBS_DEFAULT,
SHOWJOBS_VERBOSE,
SHOWJOBS_PIDS,
SHOWJOBS_PGIDS
};

extern int job_warning;

void setjobctl(int);
void showjobs(int, int);
struct job *makejob(union node *, int);
pid_t forkshell(struct job *, union node *, int);
pid_t vforkexecshell(struct job *, char **, char **, const char *, int, int []);
int waitforjob(struct job *, int *);
int stoppedjobs(void);
int backgndpidset(void);
pid_t backgndpidval(void);
char *commandtext(union node *);

#if ! JOBS
#define setjobctl(on)
#endif
