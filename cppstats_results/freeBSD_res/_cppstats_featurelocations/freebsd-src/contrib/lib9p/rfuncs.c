


























#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#if defined(WITH_CASPER)
#include <libcasper.h>
#include <casper/cap_pwd.h>
#include <casper/cap_grp.h>
#endif

#include "rfuncs.h"























char *
r_basename(const char *path, char *buf, size_t bufsize)
{
const char *endp, *comp;
size_t len;






if (path == NULL || *path == '\0') {
comp = ".";
len = 1;
} else {






endp = path + strlen(path) - 1;
while (*endp == '/' && endp > path)
endp--;

if (*endp == '/') {

comp = "/";
len = 1;
} else {
























comp = endp;
while (comp > path && comp[-1] != '/')
comp--;
len = (size_t)(endp - comp + 1);
}
}
if (buf == NULL) {
buf = malloc(len + 1);
if (buf == NULL)
return (NULL);
} else {
if (len >= bufsize) {
errno = ENAMETOOLONG;
return (NULL);
}
}
memcpy(buf, comp, len);
buf[len] = '\0';
return (buf);
}





















char *
r_dirname(const char *path, char *buf, size_t bufsize)
{
const char *endp, *dirpart;
size_t len;






if (path == NULL || *path == '\0') {
dirpart = ".";
len = 1;
} else {






















endp = path + strlen(path) - 1;
while (endp > path && *endp == '/')
endp--;
while (endp > path && *endp != '/')
endp--;
while (endp > path && *endp == '/')
endp--;

len = (size_t)(endp - path + 1);
if (endp == path && *endp != '/')
dirpart = ".";
else
dirpart = path;
}
if (buf == NULL) {
buf = malloc(len + 1);
if (buf == NULL)
return (NULL);
} else {
if (len >= bufsize) {
errno = ENAMETOOLONG;
return (NULL);
}
}
memcpy(buf, dirpart, len);
buf[len] = '\0';
return (buf);
}

static void
r_pginit(struct r_pgdata *pg)
{


pg->r_pgbufsize = 1 << 9;
pg->r_pgbuf = NULL;
}

static int
r_pgexpand(struct r_pgdata *pg)
{
size_t nsize;

nsize = pg->r_pgbufsize << 1;
if (nsize >= (1 << 20) ||
(pg->r_pgbuf = realloc(pg->r_pgbuf, nsize)) == NULL)
return (ENOMEM);
return (0);
}

void
r_pgfree(struct r_pgdata *pg)
{

free(pg->r_pgbuf);
}

struct passwd *
r_getpwuid(uid_t uid, struct r_pgdata *pg)
{
struct passwd *result = NULL;
int error;

r_pginit(pg);
do {
error = r_pgexpand(pg);
if (error == 0)
error = getpwuid_r(uid, &pg->r_pgun.un_pw,
pg->r_pgbuf, pg->r_pgbufsize, &result);
} while (error == ERANGE);

return (error ? NULL : result);
}

struct group *
r_getgrgid(gid_t gid, struct r_pgdata *pg)
{
struct group *result = NULL;
int error;

r_pginit(pg);
do {
error = r_pgexpand(pg);
if (error == 0)
error = getgrgid_r(gid, &pg->r_pgun.un_gr,
pg->r_pgbuf, pg->r_pgbufsize, &result);
} while (error == ERANGE);

return (error ? NULL : result);
}

#if defined(WITH_CASPER)
struct passwd *
r_cap_getpwuid(cap_channel_t *cap, uid_t uid, struct r_pgdata *pg)
{
struct passwd *result = NULL;
int error;

r_pginit(pg);
do {
error = r_pgexpand(pg);
if (error == 0)
error = cap_getpwuid_r(cap, uid, &pg->r_pgun.un_pw,
pg->r_pgbuf, pg->r_pgbufsize, &result);
} while (error == ERANGE);

return (error ? NULL : result);
}

struct group *
r_cap_getgrgid(cap_channel_t *cap, gid_t gid, struct r_pgdata *pg)
{
struct group *result = NULL;
int error;

r_pginit(pg);
do {
error = r_pgexpand(pg);
if (error == 0)
error = cap_getgrgid_r(cap, gid, &pg->r_pgun.un_gr,
pg->r_pgbuf, pg->r_pgbufsize, &result);
} while (error == ERANGE);

return (error ? NULL : result);
}
#endif
