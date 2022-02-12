#if !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <inttypes.h>
#include <time.h>
#include <sys/stat.h>
#include <netdb.h>

#if !defined(__unused)
#define __unused __attribute__((__unused__))
#endif

#if !defined(__dead)
#define __dead __attribute__((__noreturn__))
#endif

#if !defined(__RCSID)
#define __RCSID(a)
#endif

#if !defined(__UNCONST)
#define __UNCONST(a) ((void *)(intptr_t)(a))
#endif

#if !defined(__arraycount)
#define __arraycount(a) (sizeof(a) / sizeof(a[0]))
#endif

#if !defined(HAVE_STRLCPY)
size_t strlcpy(char *, const char *, size_t);
#endif

#if !defined(HAVE_STRLCAT)
size_t strlcat(char *, const char *, size_t);
#endif

#if !defined(HAVE_POPENVE)
FILE *popenve(const char *, char *const *, char *const *, const char *);
int pcloseve(FILE *);
#define pclose(a) pcloseve(a);
#endif

#if !defined(HAVE_SOCKADDR_SNPRINTF)
struct sockaddr;
int sockaddr_snprintf(char *, size_t, const char *, const struct sockaddr *);
#endif

#if !defined(HAVE_STRTOI)
intmax_t strtoi(const char *, char **, int, intmax_t, intmax_t, int *);
#endif

#if !defined(HAVE_GETPROGNAME)
const char *getprogname(void);
void setprogname(char *);
#endif

#if !defined(HAVE_PIDFILE)
int pidfile(const char *);
#endif

#if !defined(HAVE_FPARSELN)
#define FPARSELN_UNESCALL 0xf
#define FPARSELN_UNESCCOMM 0x1
#define FPARSELN_UNESCCONT 0x2
#define FPARSELN_UNESCESC 0x4
#define FPARSELN_UNESCREST 0x8
char *fparseln(FILE *, size_t *, size_t *, const char delim[3], int);
#endif

#if !defined(HAVE_FGETLN)
char *fgetln(FILE *, size_t *);
#endif

#if !defined(HAVE_CLOCK_GETTIME)
struct timespec;
int clock_gettime(int, struct timespec *);
#define CLOCK_REALTIME 0
#endif

#if !defined(__FreeBSD__)
#define _PATH_BLCONF "conf"
#define _PATH_BLCONTROL "control"
#define _PATH_BLSOCK "blacklistd.sock"
#define _PATH_BLSTATE "blacklistd.db"
#endif
