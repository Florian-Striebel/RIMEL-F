
























#if !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#if defined(__linux__)
#include <sched.h>
#endif
#if defined(__FreeBSD__)
#include <sys/param.h>
#include <sys/cpuset.h>
#endif
#if defined(__DragonFly__)
#include <pthread.h>
#include <pthread_np.h>
#endif
#if defined(__NetBSD__)
#include <pthread.h>
#include <sched.h>
#endif
#include "config.h"

#if defined(USE_SETCPUAFFINITY)
static const char *next_token(const char *q, int sep) {
if (q)
q = strchr(q, sep);
if (q)
q++;

return q;
}

static int next_num(const char *str, char **end, int *result) {
if (!str || *str == '\0' || !isdigit(*str))
return -1;

*result = strtoul(str, end, 10);
if (str == *end)
return -1;

return 0;
}




void setcpuaffinity(const char *cpulist) {
const char *p, *q;
char *end = NULL;
#if defined(__linux__)
cpu_set_t cpuset;
#endif
#if defined (__FreeBSD__) || defined(__DragonFly__)
cpuset_t cpuset;
#endif
#if defined(__NetBSD__)
cpuset_t *cpuset;
#endif

if (!cpulist)
return;

#if !defined(__NetBSD__)
CPU_ZERO(&cpuset);
#else
cpuset = cpuset_create();
#endif

q = cpulist;
while (p = q, q = next_token(q, ','), p) {
int a, b, s;
const char *c1, *c2;

if (next_num(p, &end, &a) != 0)
return;

b = a;
s = 1;
p = end;

c1 = next_token(p, '-');
c2 = next_token(p, ',');

if (c1 != NULL && (c2 == NULL || c1 < c2)) {
if (next_num(c1, &end, &b) != 0)
return;

c1 = end && *end ? next_token(end, ':') : NULL;
if (c1 != NULL && (c2 == NULL || c1 < c2)) {
if (next_num(c1, &end, &s) != 0)
return;

if (s == 0)
return;
}
}

if ((a > b))
return;

while (a <= b) {
#if !defined(__NetBSD__)
CPU_SET(a, &cpuset);
#else
cpuset_set(a, cpuset);
#endif
a += s;
}
}

if (end && *end)
return;

#if defined(__linux__)
sched_setaffinity(0, sizeof(cpuset), &cpuset);
#endif
#if defined(__FreeBSD__)
cpuset_setaffinity(CPU_LEVEL_WHICH, CPU_WHICH_TID, -1, sizeof(cpuset), &cpuset);
#endif
#if defined(__DragonFly__)
pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);
#endif
#if defined(__NetBSD__)
pthread_setaffinity_np(pthread_self(), cpuset_size(cpuset), cpuset);
cpuset_destroy(cpuset);
#endif
}

#endif
