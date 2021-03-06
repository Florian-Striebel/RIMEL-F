







































#if defined(__FreeBSD__)
#define USE_CAPSICUM 1
#endif

#include <sys/param.h>
#if USE_CAPSICUM
#include <sys/capsicum.h>
#endif
#include <sys/stat.h>

#include <capsicum_helpers.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <paths.h>
#include <pwd.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

#include "dma.h"


static void
logfail(int exitcode, const char *fmt, ...)
{
int oerrno = errno;
va_list ap;
char outs[1024];

outs[0] = 0;
if (fmt != NULL) {
va_start(ap, fmt);
vsnprintf(outs, sizeof(outs), fmt, ap);
va_end(ap);
}

errno = oerrno;
if (*outs != 0)
syslog(LOG_ERR, errno ? "%s: %m" : "%s", outs);
else
syslog(LOG_ERR, errno ? "%m" : "unknown error");

exit(exitcode);
}






int
main(int argc, char **argv)
{
#if USE_CAPSICUM
cap_rights_t rights;
#endif
const char *user;
struct passwd *pw;
struct group *gr;
uid_t user_uid;
gid_t mail_gid;
int f, maildirfd;




openlog("dma-mbox-create", LOG_NDELAY, LOG_MAIL);

errno = 0;
gr = getgrnam(DMA_GROUP);
if (!gr)
logfail(EX_CONFIG, "cannot find dma group `%s'", DMA_GROUP);

mail_gid = gr->gr_gid;

if (setgid(mail_gid) != 0)
logfail(EX_NOPERM, "cannot set gid to %d (%s)", mail_gid, DMA_GROUP);
if (getegid() != mail_gid)
logfail(EX_NOPERM, "cannot set gid to %d (%s), still at %d", mail_gid, DMA_GROUP, getegid());




if (argc != 2) {
errno = 0;
logfail(EX_USAGE, "no arguments");
}
user = argv[1];

syslog(LOG_NOTICE, "creating mbox for `%s'", user);


if (strchr(user, '/')) {
errno = 0;
logfail(EX_DATAERR, "path separator in username `%s'", user);
exit(1);
}


errno = 0;
pw = getpwnam(user);
if (!pw)
logfail(EX_NOUSER, "cannot find user `%s'", user);

maildirfd = open(_PATH_MAILDIR, O_RDONLY);
if (maildirfd < 0)
logfail(EX_NOINPUT, "cannot open maildir %s", _PATH_MAILDIR);





caph_cache_catpages();




caph_cache_tzdata();

#if USE_CAPSICUM
cap_rights_init(&rights, CAP_CREATE, CAP_FCHMOD, CAP_FCHOWN,
CAP_LOOKUP, CAP_READ);
if (cap_rights_limit(maildirfd, &rights) < 0 && errno != ENOSYS)
err(EX_OSERR, "can't limit maildirfd rights");


if (caph_enter() < 0)
err(EX_OSERR, "cap_enter");
#endif

user_uid = pw->pw_uid;

f = openat(maildirfd, user, O_RDONLY|O_CREAT|O_NOFOLLOW, 0600);
if (f < 0)
logfail(EX_NOINPUT, "cannot open mbox `%s'", user);

if (fchown(f, user_uid, mail_gid))
logfail(EX_OSERR, "cannot change owner of mbox `%s'", user);

if (fchmod(f, 0620))
logfail(EX_OSERR, "cannot change permissions of mbox `%s'",
user);



syslog(LOG_NOTICE, "successfully created mbox for `%s'", user);

return (0);
}
