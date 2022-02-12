











































#include <sys/time.h>

#include "make.h"
#include "job.h"
#include "trace.h"

MAKE_RCSID("$NetBSD: trace.c,v 1.29 2021/09/21 23:06:18 rillig Exp $");

static FILE *trfile;
static pid_t trpid;
const char *trwd;

static const char evname[][4] = {
"BEG",
"END",
"ERR",
"JOB",
"DON",
"INT",
};

void
Trace_Init(const char *pathname)
{
if (pathname != NULL) {
FStr curDir;
trpid = getpid();


curDir = Var_Value(SCOPE_GLOBAL, ".CURDIR");
trwd = curDir.str;

trfile = fopen(pathname, "a");
}
}

void
Trace_Log(TrEvent event, Job *job)
{
struct timeval rightnow;

if (trfile == NULL)
return;

gettimeofday(&rightnow, NULL);

fprintf(trfile, "%lld.%06ld %d %s %d %s",
(long long)rightnow.tv_sec, (long)rightnow.tv_usec,
jobTokensRunning,
evname[event], trpid, trwd);
if (job != NULL) {
char flags[4];

Job_FlagsToString(job, flags, sizeof flags);
fprintf(trfile, " %s %d %s %x", job->node->name,
job->pid, flags, job->node->type);
}
fputc('\n', trfile);
fflush(trfile);
}

void
Trace_End(void)
{
if (trfile != NULL)
fclose(trfile);
}
