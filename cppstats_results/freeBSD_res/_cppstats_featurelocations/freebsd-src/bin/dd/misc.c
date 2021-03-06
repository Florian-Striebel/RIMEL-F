


































#if !defined(lint)
#if 0
static char sccsid[] = "@(#)misc.c 8.3 (Berkeley) 4/2/94";
#endif
#endif
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/types.h>

#include <err.h>
#include <errno.h>
#include <inttypes.h>
#include <libutil.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "dd.h"
#include "extern.h"

double
secs_elapsed(void)
{
struct timespec end, ts_res;
double secs, res;

if (clock_gettime(CLOCK_MONOTONIC, &end))
err(1, "clock_gettime");
if (clock_getres(CLOCK_MONOTONIC, &ts_res))
err(1, "clock_getres");
secs = (end.tv_sec - st.start.tv_sec) + \
(end.tv_nsec - st.start.tv_nsec) * 1e-9;
res = ts_res.tv_sec + ts_res.tv_nsec * 1e-9;
if (secs < res)
secs = res;

return (secs);
}

void
summary(void)
{
double secs;

if (ddflags & C_NOINFO)
return;

if (ddflags & C_PROGRESS)
fprintf(stderr, "\n");

secs = secs_elapsed();

(void)fprintf(stderr,
"%ju+%ju records in\n%ju+%ju records out\n",
st.in_full, st.in_part, st.out_full, st.out_part);
if (st.swab)
(void)fprintf(stderr, "%ju odd length swab %s\n",
st.swab, (st.swab == 1) ? "block" : "blocks");
if (st.trunc)
(void)fprintf(stderr, "%ju truncated %s\n",
st.trunc, (st.trunc == 1) ? "block" : "blocks");
if (!(ddflags & C_NOXFER)) {
(void)fprintf(stderr,
"%ju bytes transferred in %.6f secs (%.0f bytes/sec)\n",
st.bytes, secs, st.bytes / secs);
}
need_summary = 0;
}

void
progress(void)
{
static int outlen;
char si[4 + 1 + 2 + 1];
char iec[4 + 1 + 3 + 1];
char persec[4 + 1 + 2 + 1];
char *buf;
double secs;

secs = secs_elapsed();
humanize_number(si, sizeof(si), (int64_t)st.bytes, "B", HN_AUTOSCALE,
HN_DECIMAL | HN_DIVISOR_1000);
humanize_number(iec, sizeof(iec), (int64_t)st.bytes, "B", HN_AUTOSCALE,
HN_DECIMAL | HN_IEC_PREFIXES);
humanize_number(persec, sizeof(persec), (int64_t)(st.bytes / secs), "B",
HN_AUTOSCALE, HN_DECIMAL | HN_DIVISOR_1000);
asprintf(&buf, " %'ju bytes (%s, %s) transferred %.3fs, %s/s",
(uintmax_t)st.bytes, si, iec, secs, persec);
outlen = fprintf(stderr, "%-*s\r", outlen, buf) - 1;
fflush(stderr);
free(buf);
need_progress = 0;
}


void
siginfo_handler(int signo __unused)
{

need_summary = 1;
}


void
sigalarm_handler(int signo __unused)
{

need_progress = 1;
}


void
terminate(int sig)
{

summary();
_exit(sig == 0 ? 0 : 1);
}
