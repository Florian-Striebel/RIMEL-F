






























#if 0
#if !defined(lint)
static const char copyright[] =
"@(#) Copyright (c) 1990, 1993, 1994\n\
The Regents of the University of California. All rights reserved.\n";
#endif

#if !defined(lint)
static char sccsid[] = "@(#)rm.c 8.5 (Berkeley) 4/18/94";
#endif
#endif
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/stat.h>
#include <sys/param.h>
#include <sys/mount.h>

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <fts.h>
#include <grp.h>
#include <locale.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

static int dflag, eval, fflag, iflag, vflag, Wflag, stdin_ok;
static int rflag, Iflag, xflag;
static uid_t uid;
static volatile sig_atomic_t info;

static int check(const char *, const char *, struct stat *);
static int check2(char **);
static void checkdot(char **);
static void checkslash(char **);
static void rm_file(char **);
static void rm_tree(char **);
static void siginfo(int __unused);
static void usage(void);








int
main(int argc, char *argv[])
{
int ch;
char *p;

(void)setlocale(LC_ALL, "");






if ((p = strrchr(argv[0], '/')) == NULL)
p = argv[0];
else
++p;
if (strcmp(p, "unlink") == 0) {
if (argc == 2)
rm_file(&argv[1]);
else if (argc == 3 && strcmp(argv[1], "--") == 0)
rm_file(&argv[2]);
else
usage();
exit(eval);
}

rflag = xflag = 0;
while ((ch = getopt(argc, argv, "dfiIPRrvWx")) != -1)
switch(ch) {
case 'd':
dflag = 1;
break;
case 'f':
fflag = 1;
iflag = 0;
break;
case 'i':
fflag = 0;
iflag = 1;
break;
case 'I':
Iflag = 1;
break;
case 'P':

break;
case 'R':
case 'r':
rflag = 1;
break;
case 'v':
vflag = 1;
break;
case 'W':
Wflag = 1;
break;
case 'x':
xflag = 1;
break;
default:
usage();
}
argc -= optind;
argv += optind;

if (argc < 1) {
if (fflag)
return (0);
usage();
}

checkdot(argv);
checkslash(argv);
uid = geteuid();

(void)signal(SIGINFO, siginfo);
if (*argv) {
stdin_ok = isatty(STDIN_FILENO);

if (Iflag) {
if (check2(argv) == 0)
exit (1);
}
if (rflag)
rm_tree(argv);
else
rm_file(argv);
}

exit (eval);
}

static void
rm_tree(char **argv)
{
FTS *fts;
FTSENT *p;
int needstat;
int flags;
int rval;





needstat = !uid || (!fflag && !iflag && stdin_ok);





#define SKIPPED 1

flags = FTS_PHYSICAL;
if (!needstat)
flags |= FTS_NOSTAT;
if (Wflag)
flags |= FTS_WHITEOUT;
if (xflag)
flags |= FTS_XDEV;
if (!(fts = fts_open(argv, flags, NULL))) {
if (fflag && errno == ENOENT)
return;
err(1, "fts_open");
}
while (errno = 0, (p = fts_read(fts)) != NULL) {
switch (p->fts_info) {
case FTS_DNR:
if (!fflag || p->fts_errno != ENOENT) {
warnx("%s: %s",
p->fts_path, strerror(p->fts_errno));
eval = 1;
}
continue;
case FTS_ERR:
errx(1, "%s: %s", p->fts_path, strerror(p->fts_errno));
case FTS_NS:




if (!needstat)
break;
if (!fflag || p->fts_errno != ENOENT) {
warnx("%s: %s",
p->fts_path, strerror(p->fts_errno));
eval = 1;
}
continue;
case FTS_D:

if (!fflag && !check(p->fts_path, p->fts_accpath,
p->fts_statp)) {
(void)fts_set(fts, p, FTS_SKIP);
p->fts_number = SKIPPED;
}
else if (!uid &&
(p->fts_statp->st_flags & (UF_APPEND|UF_IMMUTABLE)) &&
!(p->fts_statp->st_flags & (SF_APPEND|SF_IMMUTABLE)) &&
lchflags(p->fts_accpath,
p->fts_statp->st_flags &= ~(UF_APPEND|UF_IMMUTABLE)) < 0)
goto err;
continue;
case FTS_DP:

if (p->fts_number == SKIPPED)
continue;
break;
default:
if (!fflag &&
!check(p->fts_path, p->fts_accpath, p->fts_statp))
continue;
}

rval = 0;
if (!uid &&
(p->fts_statp->st_flags & (UF_APPEND|UF_IMMUTABLE)) &&
!(p->fts_statp->st_flags & (SF_APPEND|SF_IMMUTABLE)))
rval = lchflags(p->fts_accpath,
p->fts_statp->st_flags &= ~(UF_APPEND|UF_IMMUTABLE));
if (rval == 0) {





switch (p->fts_info) {
case FTS_DP:
case FTS_DNR:
rval = rmdir(p->fts_accpath);
if (rval == 0 || (fflag && errno == ENOENT)) {
if (rval == 0 && vflag)
(void)printf("%s\n",
p->fts_path);
if (rval == 0 && info) {
info = 0;
(void)printf("%s\n",
p->fts_path);
}
continue;
}
break;

case FTS_W:
rval = undelete(p->fts_accpath);
if (rval == 0 && (fflag && errno == ENOENT)) {
if (vflag)
(void)printf("%s\n",
p->fts_path);
if (info) {
info = 0;
(void)printf("%s\n",
p->fts_path);
}
continue;
}
break;

case FTS_NS:




if (fflag)
continue;


case FTS_F:
case FTS_NSOK:
default:
rval = unlink(p->fts_accpath);
if (rval == 0 || (fflag && errno == ENOENT)) {
if (rval == 0 && vflag)
(void)printf("%s\n",
p->fts_path);
if (rval == 0 && info) {
info = 0;
(void)printf("%s\n",
p->fts_path);
}
continue;
}
}
}
err:
warn("%s", p->fts_path);
eval = 1;
}
if (!fflag && errno)
err(1, "fts_read");
fts_close(fts);
}

static void
rm_file(char **argv)
{
struct stat sb;
int rval;
char *f;





while ((f = *argv++) != NULL) {

if (lstat(f, &sb)) {
if (Wflag) {
sb.st_mode = S_IFWHT|S_IWUSR|S_IRUSR;
} else {
if (!fflag || errno != ENOENT) {
warn("%s", f);
eval = 1;
}
continue;
}
} else if (Wflag) {
warnx("%s: %s", f, strerror(EEXIST));
eval = 1;
continue;
}

if (S_ISDIR(sb.st_mode) && !dflag) {
warnx("%s: is a directory", f);
eval = 1;
continue;
}
if (!fflag && !S_ISWHT(sb.st_mode) && !check(f, f, &sb))
continue;
rval = 0;
if (!uid && !S_ISWHT(sb.st_mode) &&
(sb.st_flags & (UF_APPEND|UF_IMMUTABLE)) &&
!(sb.st_flags & (SF_APPEND|SF_IMMUTABLE)))
rval = lchflags(f, sb.st_flags & ~(UF_APPEND|UF_IMMUTABLE));
if (rval == 0) {
if (S_ISWHT(sb.st_mode))
rval = undelete(f);
else if (S_ISDIR(sb.st_mode))
rval = rmdir(f);
else
rval = unlink(f);
}
if (rval && (!fflag || errno != ENOENT)) {
warn("%s", f);
eval = 1;
}
if (vflag && rval == 0)
(void)printf("%s\n", f);
if (info && rval == 0) {
info = 0;
(void)printf("%s\n", f);
}
}
}

static int
check(const char *path, const char *name, struct stat *sp)
{
int ch, first;
char modep[15], *flagsp;


if (iflag)
(void)fprintf(stderr, "remove %s? ", path);
else {






if (!stdin_ok || S_ISLNK(sp->st_mode) ||
(!access(name, W_OK) &&
!(sp->st_flags & (SF_APPEND|SF_IMMUTABLE)) &&
(!(sp->st_flags & (UF_APPEND|UF_IMMUTABLE)) || !uid)))
return (1);
strmode(sp->st_mode, modep);
if ((flagsp = fflagstostr(sp->st_flags)) == NULL)
err(1, "fflagstostr");
(void)fprintf(stderr, "override %s%s%s/%s %s%sfor %s? ",
modep + 1, modep[10] == ' ' ? "" : " ",
user_from_uid(sp->st_uid, 0),
group_from_gid(sp->st_gid, 0),
*flagsp ? flagsp : "", *flagsp ? " " : "",
path);
free(flagsp);
}
(void)fflush(stderr);

first = ch = getchar();
while (ch != '\n' && ch != EOF)
ch = getchar();
return (first == 'y' || first == 'Y');
}

#define ISSLASH(a) ((a)[0] == '/' && (a)[1] == '\0')
static void
checkslash(char **argv)
{
char **t, **u;
int complained;

complained = 0;
for (t = argv; *t;) {
if (ISSLASH(*t)) {
if (!complained++)
warnx("\"/\" may not be removed");
eval = 1;
for (u = t; u[0] != NULL; ++u)
u[0] = u[1];
} else {
++t;
}
}
}

static int
check2(char **argv)
{
struct stat st;
int first;
int ch;
int fcount = 0;
int dcount = 0;
int i;
const char *dname = NULL;

for (i = 0; argv[i]; ++i) {
if (lstat(argv[i], &st) == 0) {
if (S_ISDIR(st.st_mode)) {
++dcount;
dname = argv[i];
} else {
++fcount;
}
}
}
first = 0;
while (first != 'n' && first != 'N' && first != 'y' && first != 'Y') {
if (dcount && rflag) {
fprintf(stderr, "recursively remove");
if (dcount == 1)
fprintf(stderr, " %s", dname);
else
fprintf(stderr, " %d dirs", dcount);
if (fcount == 1)
fprintf(stderr, " and 1 file");
else if (fcount > 1)
fprintf(stderr, " and %d files", fcount);
} else if (dcount + fcount > 3) {
fprintf(stderr, "remove %d files", dcount + fcount);
} else {
return(1);
}
fprintf(stderr, "? ");
fflush(stderr);

first = ch = getchar();
while (ch != '\n' && ch != EOF)
ch = getchar();
if (ch == EOF)
break;
}
return (first == 'y' || first == 'Y');
}

#define ISDOT(a) ((a)[0] == '.' && (!(a)[1] || ((a)[1] == '.' && !(a)[2])))
static void
checkdot(char **argv)
{
char *p, **save, **t;
int complained;

complained = 0;
for (t = argv; *t;) {
if ((p = strrchr(*t, '/')) != NULL)
++p;
else
p = *t;
if (ISDOT(p)) {
if (!complained++)
warnx("\".\" and \"..\" may not be removed");
eval = 1;
for (save = t; (t[0] = t[1]) != NULL; ++t)
continue;
t = save;
} else
++t;
}
}

static void
usage(void)
{

(void)fprintf(stderr, "%s\n%s\n",
"usage: rm [-f | -i] [-dIPRrvWx] file ...",
" unlink [--] file");
exit(EX_USAGE);
}

static void
siginfo(int sig __unused)
{

info = 1;
}
