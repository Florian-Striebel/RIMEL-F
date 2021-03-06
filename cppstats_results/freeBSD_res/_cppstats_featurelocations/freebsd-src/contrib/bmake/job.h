














































































#if !defined(MAKE_JOB_H)
#define MAKE_JOB_H

#define TMPPAT "makeXXXXXX"

#if defined(USE_SELECT)





#define poll emul_poll
#define pollfd emul_pollfd

struct emul_pollfd {
int fd;
short events;
short revents;
};

#define POLLIN 0x0001
#define POLLOUT 0x0004

int
emul_poll(struct pollfd *fd, int nfd, int timeout);
#endif





#define POLL_MSEC 5000

struct pollfd;


#if defined(USE_META)
#include "meta.h"
#endif

typedef enum JobStatus {
JOB_ST_FREE = 0,
JOB_ST_SET_UP = 1,

JOB_ST_RUNNING = 3,
JOB_ST_FINISHED = 4
} JobStatus;














typedef struct Job {

int pid;


GNode *node;




StringListNode *tailCmds;


FILE *cmdFILE;

int exit_status;

JobStatus status;

bool suspended;


bool ignerr;

bool echo;

bool special;

int inPipe;
int outPipe;
struct pollfd *inPollfd;

#define JOB_BUFSIZE 1024

char outBuf[JOB_BUFSIZE + 1];
size_t curPos;

#if defined(USE_META)
struct BuildMon bm;
#endif
} Job;

extern const char *shellPath;
extern const char *shellName;
extern char *shellErrFlag;

extern int jobTokensRunning;

void Shell_Init(void);
const char *Shell_GetNewline(void);
void Job_Touch(GNode *, bool);
bool Job_CheckCommands(GNode *, void (*abortProc)(const char *, ...));
void Job_CatchChildren(void);
void Job_CatchOutput(void);
void Job_Make(GNode *);
void Job_Init(void);
bool Job_ParseShell(char *);
int Job_Finish(void);
void Job_End(void);
void Job_Wait(void);
void Job_AbortAll(void);
void Job_TokenReturn(void);
bool Job_TokenWithdraw(void);
void Job_ServerStart(int, int, int);
void Job_SetPrefix(void);
bool Job_RunTarget(const char *, const char *);
void Job_FlagsToString(const Job *, char *, size_t);
int Job_TempFile(const char *, char *, size_t);

#endif
