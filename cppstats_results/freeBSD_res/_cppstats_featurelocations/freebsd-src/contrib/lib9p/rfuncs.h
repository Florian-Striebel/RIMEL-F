


























#if !defined(LIB9P_RFUNCS_H)
#define LIB9P_RFUNCS_H

#include <grp.h>
#include <pwd.h>
#include <string.h>

#if defined(WITH_CASPER)
#include <libcasper.h>
#endif





char *r_basename(const char *, char *, size_t);
char *r_dirname(const char *, char *, size_t);















struct r_pgdata {
char *r_pgbuf;
size_t r_pgbufsize;
union {
struct passwd un_pw;
struct group un_gr;
} r_pgun;
};


void r_pgfree(struct r_pgdata *);
struct passwd *r_getpwuid(uid_t, struct r_pgdata *);
struct group *r_getgrgid(gid_t, struct r_pgdata *);

#if defined(WITH_CASPER)
struct passwd *r_cap_getpwuid(cap_channel_t *, uid_t, struct r_pgdata *);
struct group *r_cap_getgrgid(cap_channel_t *, gid_t, struct r_pgdata *);
#endif

#endif
