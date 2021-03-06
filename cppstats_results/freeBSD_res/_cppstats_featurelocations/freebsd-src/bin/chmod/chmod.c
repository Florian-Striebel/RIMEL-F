






























#if 0
#if !defined(lint)
static char const copyright[] =
"@(#) Copyright (c) 1989, 1993, 1994\n\
The Regents of the University of California. All rights reserved.\n";
#endif

#if !defined(lint)
static char sccsid[] = "@(#)chmod.c 8.8 (Berkeley) 4/1/94";
#endif
#endif
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/param.h>
#include <sys/stat.h>

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <fts.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static volatile sig_atomic_t siginfo;

static void usage(void);
static int may_have_nfs4acl(const FTSENT *ent, int hflag);

static void
siginfo_handler(int sig __unused)
{

siginfo = 1;
}

int
main(int argc, char *argv[])
{
FTS *ftsp;
FTSENT *p;
mode_t *set;
int Hflag, Lflag, Rflag, ch, fflag, fts_options, hflag, rval;
int vflag;
char *mode;
mode_t newmode;

set = NULL;
Hflag = Lflag = Rflag = fflag = hflag = vflag = 0;
while ((ch = getopt(argc, argv, "HLPRXfghorstuvwx")) != -1)
switch (ch) {
case 'H':
Hflag = 1;
Lflag = 0;
break;
case 'L':
Lflag = 1;
Hflag = 0;
break;
case 'P':
Hflag = Lflag = 0;
break;
case 'R':
Rflag = 1;
break;
case 'f':
fflag = 1;
break;
case 'h':







hflag = 1;
break;






case 'g': case 'o': case 'r': case 's':
case 't': case 'u': case 'w': case 'X': case 'x':
if (argv[optind - 1][0] == '-' &&
argv[optind - 1][1] == ch &&
argv[optind - 1][2] == '\0')
--optind;
goto done;
case 'v':
vflag++;
break;
case '?':
default:
usage();
}
done: argv += optind;
argc -= optind;

if (argc < 2)
usage();

(void)signal(SIGINFO, siginfo_handler);

if (Rflag) {
if (hflag)
errx(1, "the -R and -h options may not be "
"specified together.");
if (Lflag) {
fts_options = FTS_LOGICAL;
} else {
fts_options = FTS_PHYSICAL;

if (Hflag) {
fts_options |= FTS_COMFOLLOW;
}
}
} else if (hflag) {
fts_options = FTS_PHYSICAL;
} else {
fts_options = FTS_LOGICAL;
}

mode = *argv;
if ((set = setmode(mode)) == NULL)
errx(1, "invalid file mode: %s", mode);

if ((ftsp = fts_open(++argv, fts_options, 0)) == NULL)
err(1, "fts_open");
for (rval = 0; errno = 0, (p = fts_read(ftsp)) != NULL;) {
int atflag;

if ((fts_options & FTS_LOGICAL) ||
((fts_options & FTS_COMFOLLOW) &&
p->fts_level == FTS_ROOTLEVEL))
atflag = 0;
else
atflag = AT_SYMLINK_NOFOLLOW;

switch (p->fts_info) {
case FTS_D:
if (!Rflag)
fts_set(ftsp, p, FTS_SKIP);
break;
case FTS_DNR:
warnx("%s: %s", p->fts_path, strerror(p->fts_errno));
rval = 1;
break;
case FTS_DP:
continue;
case FTS_ERR:
case FTS_NS:
warnx("%s: %s", p->fts_path, strerror(p->fts_errno));
rval = 1;
continue;
default:
break;
}
newmode = getmode(set, p->fts_statp->st_mode);





if (may_have_nfs4acl(p, hflag) == 0 &&
(newmode & ALLPERMS) == (p->fts_statp->st_mode & ALLPERMS))
continue;
if (fchmodat(AT_FDCWD, p->fts_accpath, newmode, atflag) == -1
&& !fflag) {
warn("%s", p->fts_path);
rval = 1;
} else if (vflag || siginfo) {
(void)printf("%s", p->fts_path);

if (vflag > 1 || siginfo) {
char m1[12], m2[12];

strmode(p->fts_statp->st_mode, m1);
strmode((p->fts_statp->st_mode &
S_IFMT) | newmode, m2);
(void)printf(": 0%o [%s] -> 0%o [%s]",
p->fts_statp->st_mode, m1,
(p->fts_statp->st_mode & S_IFMT) |
newmode, m2);
}
(void)printf("\n");
siginfo = 0;
}
}
if (errno)
err(1, "fts_read");
exit(rval);
}

static void
usage(void)
{
(void)fprintf(stderr,
"usage: chmod [-fhv] [-R [-H | -L | -P]] mode file ...\n");
exit(1);
}

static int
may_have_nfs4acl(const FTSENT *ent, int hflag)
{
int ret;
static dev_t previous_dev = NODEV;
static int supports_acls = -1;

if (previous_dev != ent->fts_statp->st_dev) {
previous_dev = ent->fts_statp->st_dev;
supports_acls = 0;

if (hflag)
ret = lpathconf(ent->fts_accpath, _PC_ACL_NFS4);
else
ret = pathconf(ent->fts_accpath, _PC_ACL_NFS4);
if (ret > 0)
supports_acls = 1;
else if (ret < 0 && errno != EINVAL)
warn("%s", ent->fts_path);
}

return (supports_acls);
}
