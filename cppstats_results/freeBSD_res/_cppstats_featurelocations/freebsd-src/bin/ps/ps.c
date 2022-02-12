





































#if !defined(lint)
static const char copyright[] =
"@(#) Copyright (c) 1990, 1993, 1994\n\
The Regents of the University of California. All rights reserved.\n";
#endif

#if 0
#if !defined(lint)
static char sccsid[] = "@(#)ps.c 8.4 (Berkeley) 4/2/94";
#endif
#endif

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/param.h>
#include <sys/jail.h>
#include <sys/proc.h>
#include <sys/user.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/sysctl.h>
#include <sys/mount.h>

#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <jail.h>
#include <kvm.h>
#include <limits.h>
#include <locale.h>
#include <paths.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libxo/xo.h>

#include "ps.h"

#define _PATH_PTS "/dev/pts/"

#define W_SEP " \t"
#define T_SEP ","

#if defined(LAZY_PS)
#define DEF_UREAD 0
#define OPT_LAZY_f "f"
#else
#define DEF_UREAD 1
#define OPT_LAZY_f
#endif





#define isdigitch(Anychar) isdigit((u_char)(Anychar))

int cflag;
int eval;
time_t now;
int rawcpu;
int sumrusage;
int termwidth;
int showthreads;

struct velisthead varlist = STAILQ_HEAD_INITIALIZER(varlist);

static int forceuread = DEF_UREAD;
static kvm_t *kd;
static int needcomm;
static int needenv;
static int needuser;
static int optfatal;
static int pid_max;

static enum sort { DEFAULT, SORTMEM, SORTCPU } sortby = DEFAULT;

struct listinfo;
typedef int addelem_rtn(struct listinfo *_inf, const char *_elem);

struct listinfo {
int count;
int maxcount;
int elemsize;
addelem_rtn *addelem;
const char *lname;
union {
gid_t *gids;
int *jids;
pid_t *pids;
dev_t *ttys;
uid_t *uids;
void *ptr;
} l;
};

static int addelem_gid(struct listinfo *, const char *);
static int addelem_jid(struct listinfo *, const char *);
static int addelem_pid(struct listinfo *, const char *);
static int addelem_tty(struct listinfo *, const char *);
static int addelem_uid(struct listinfo *, const char *);
static void add_list(struct listinfo *, const char *);
static void descendant_sort(KINFO *, int);
static void format_output(KINFO *);
static void *expand_list(struct listinfo *);
static const char *
fmt(char **(*)(kvm_t *, const struct kinfo_proc *, int),
KINFO *, char *, char *, int);
static void free_list(struct listinfo *);
static void init_list(struct listinfo *, addelem_rtn, int, const char *);
static char *kludge_oldps_options(const char *, char *, const char *);
static int pscomp(const void *, const void *);
static void saveuser(KINFO *);
static void scanvars(void);
static void sizevars(void);
static void pidmax_init(void);
static void usage(void);

static char dfmt[] = "pid,tt,state,time,command";
static char jfmt[] = "user,pid,ppid,pgid,sid,jobc,state,tt,time,command";
static char lfmt[] = "uid,pid,ppid,cpu,pri,nice,vsz,rss,mwchan,state,"
"tt,time,command";
static char o1[] = "pid";
static char o2[] = "tt,state,time,command";
static char ufmt[] = "user,pid,%cpu,%mem,vsz,rss,tt,state,start,time,command";
static char vfmt[] = "pid,state,time,sl,re,pagein,vsz,rss,lim,tsiz,"
"%cpu,%mem,command";
static char Zfmt[] = "label";

#define PS_ARGS "AaCcde" OPT_LAZY_f "G:gHhjJ:LlM:mN:O:o:p:rSTt:U:uvwXxZ"

int
main(int argc, char *argv[])
{
struct listinfo gidlist, jidlist, pgrplist, pidlist;
struct listinfo ruidlist, sesslist, ttylist, uidlist;
struct kinfo_proc *kp;
KINFO *kinfo = NULL, *next_KINFO;
KINFO_STR *ks;
struct varent *vent;
struct winsize ws = { .ws_row = 0 };
const char *nlistf, *memf, *str;
char *cols;
int all, ch, elem, flag, _fmt, i, lineno, linelen, left;
int descendancy, nentries, nkept, nselectors;
int prtheader, wflag, what, xkeep, xkeep_implied;
int fwidthmin, fwidthmax;
char errbuf[_POSIX2_LINE_MAX];
char fmtbuf[_POSIX2_LINE_MAX];

(void) setlocale(LC_ALL, "");
time(&now);















if ((cols = getenv("COLUMNS")) != NULL && *cols != '\0')
termwidth = atoi(cols);
else if ((ioctl(STDOUT_FILENO, TIOCGWINSZ, (char *)&ws) == -1 &&
ioctl(STDERR_FILENO, TIOCGWINSZ, (char *)&ws) == -1 &&
ioctl(STDIN_FILENO, TIOCGWINSZ, (char *)&ws) == -1) ||
ws.ws_col == 0)
termwidth = UNLIMITED;
else
termwidth = ws.ws_col - 1;





if (argc > 1)
argv[1] = kludge_oldps_options(PS_ARGS, argv[1], argv[2]);

pidmax_init();

all = descendancy = _fmt = nselectors = optfatal = 0;
prtheader = showthreads = wflag = xkeep_implied = 0;
xkeep = -1;
init_list(&gidlist, addelem_gid, sizeof(gid_t), "group");
init_list(&jidlist, addelem_jid, sizeof(int), "jail id");
init_list(&pgrplist, addelem_pid, sizeof(pid_t), "process group");
init_list(&pidlist, addelem_pid, sizeof(pid_t), "process id");
init_list(&ruidlist, addelem_uid, sizeof(uid_t), "ruser");
init_list(&sesslist, addelem_pid, sizeof(pid_t), "session id");
init_list(&ttylist, addelem_tty, sizeof(dev_t), "tty");
init_list(&uidlist, addelem_uid, sizeof(uid_t), "user");
memf = _PATH_DEVNULL;
nlistf = NULL;

argc = xo_parse_args(argc, argv);
if (argc < 0)
exit(1);

while ((ch = getopt(argc, argv, PS_ARGS)) != -1)
switch (ch) {
case 'A':





all = xkeep = 1;
break;
case 'a':
all = 1;
break;
case 'C':
rawcpu = 1;
break;
case 'c':
cflag = 1;
break;
case 'd':
descendancy = 1;
break;
case 'e':
needenv = 1;
break;
#if defined(LAZY_PS)
case 'f':
if (getuid() == 0 || getgid() == 0)
forceuread = 1;
break;
#endif
case 'G':
add_list(&gidlist, optarg);
xkeep_implied = 1;
nselectors++;
break;
case 'g':
#if 0





add_list(&pgrplist, optarg);
xkeep_implied = 1;
nselectors++;
break;
#else

break;
#endif
case 'H':
showthreads = KERN_PROC_INC_THREAD;
break;
case 'h':
prtheader = ws.ws_row > 5 ? ws.ws_row : 22;
break;
case 'J':
add_list(&jidlist, optarg);
xkeep_implied = 1;
nselectors++;
break;
case 'j':
parsefmt(jfmt, 0);
_fmt = 1;
jfmt[0] = '\0';
break;
case 'L':
showkey();
exit(0);
case 'l':
parsefmt(lfmt, 0);
_fmt = 1;
lfmt[0] = '\0';
break;
case 'M':
memf = optarg;
break;
case 'm':
sortby = SORTMEM;
break;
case 'N':
nlistf = optarg;
break;
case 'O':
parsefmt(o1, 1);
parsefmt(optarg, 1);
parsefmt(o2, 1);
o1[0] = o2[0] = '\0';
_fmt = 1;
break;
case 'o':
parsefmt(optarg, 1);
_fmt = 1;
break;
case 'p':
add_list(&pidlist, optarg);






nselectors++;
break;
#if 0
case 'R':







add_list(&ruidlist, optarg);
xkeep_implied = 1;
nselectors++;
break;
#endif
case 'r':
sortby = SORTCPU;
break;
case 'S':
sumrusage = 1;
break;
#if 0
case 's':






add_list(&sesslist, optarg);
xkeep_implied = 1;
nselectors++;
break;
#endif
case 'T':
if ((optarg = ttyname(STDIN_FILENO)) == NULL)
xo_errx(1, "stdin: not a terminal");

case 't':
add_list(&ttylist, optarg);
xkeep_implied = 1;
nselectors++;
break;
case 'U':

add_list(&uidlist, optarg);
xkeep_implied = 1;
nselectors++;
break;
case 'u':
parsefmt(ufmt, 0);
sortby = SORTCPU;
_fmt = 1;
ufmt[0] = '\0';
break;
case 'v':
parsefmt(vfmt, 0);
sortby = SORTMEM;
_fmt = 1;
vfmt[0] = '\0';
break;
case 'w':
if (wflag)
termwidth = UNLIMITED;
else if (termwidth < 131 && termwidth != UNLIMITED)
termwidth = 131;
wflag++;
break;
case 'X':











xkeep = 0;
break;
case 'x':
xkeep = 1;
break;
case 'Z':
parsefmt(Zfmt, 0);
Zfmt[0] = '\0';
break;
case '?':
default:
usage();
}
argc -= optind;
argv += optind;





while (*argv) {
if (!isdigitch(**argv))
break;
add_list(&pidlist, *argv);
argv++;
}
if (*argv) {
xo_warnx("illegal argument: %s\n", *argv);
usage();
}
if (optfatal)
exit(1);
if (xkeep < 0)
xkeep = xkeep_implied;

kd = kvm_openfiles(nlistf, memf, NULL, O_RDONLY, errbuf);
if (kd == NULL)
xo_errx(1, "%s", errbuf);

if (!_fmt)
parsefmt(dfmt, 0);

if (!all && nselectors == 0) {
uidlist.l.ptr = malloc(sizeof(uid_t));
if (uidlist.l.ptr == NULL)
xo_errx(1, "malloc failed");
nselectors = 1;
uidlist.count = uidlist.maxcount = 1;
*uidlist.l.uids = getuid();
}





scanvars();








what = showthreads != 0 ? KERN_PROC_ALL : KERN_PROC_PROC;
flag = 0;
if (nselectors == 1) {
if (gidlist.count == 1) {
what = KERN_PROC_RGID | showthreads;
flag = *gidlist.l.gids;
nselectors = 0;
} else if (pgrplist.count == 1) {
what = KERN_PROC_PGRP | showthreads;
flag = *pgrplist.l.pids;
nselectors = 0;
} else if (pidlist.count == 1 && !descendancy) {
what = KERN_PROC_PID | showthreads;
flag = *pidlist.l.pids;
nselectors = 0;
} else if (ruidlist.count == 1) {
what = KERN_PROC_RUID | showthreads;
flag = *ruidlist.l.uids;
nselectors = 0;
} else if (sesslist.count == 1) {
what = KERN_PROC_SESSION | showthreads;
flag = *sesslist.l.pids;
nselectors = 0;
} else if (ttylist.count == 1) {
what = KERN_PROC_TTY | showthreads;
flag = *ttylist.l.ttys;
nselectors = 0;
} else if (uidlist.count == 1) {
what = KERN_PROC_UID | showthreads;
flag = *uidlist.l.uids;
nselectors = 0;
}
}




nentries = -1;
kp = kvm_getprocs(kd, what, flag, &nentries);




if ((kp == NULL && errno != ESRCH) || (kp != NULL && nentries < 0))
xo_errx(1, "%s", kvm_geterr(kd));
nkept = 0;
if (descendancy)
for (elem = 0; elem < pidlist.count; elem++)
for (i = 0; i < nentries; i++)
if (kp[i].ki_ppid == pidlist.l.pids[elem]) {
if (pidlist.count >= pidlist.maxcount)
expand_list(&pidlist);
pidlist.l.pids[pidlist.count++] = kp[i].ki_pid;
}
if (nentries > 0) {
if ((kinfo = malloc(nentries * sizeof(*kinfo))) == NULL)
xo_errx(1, "malloc failed");
for (i = nentries; --i >= 0; ++kp) {





if (pidlist.count > 0) {
for (elem = 0; elem < pidlist.count; elem++)
if (kp->ki_pid == pidlist.l.pids[elem])
goto keepit;
}





if (xkeep == 0) {
if ((kp->ki_tdev == NODEV ||
(kp->ki_flag & P_CONTROLT) == 0))
continue;
}
if (nselectors == 0)
goto keepit;
if (gidlist.count > 0) {
for (elem = 0; elem < gidlist.count; elem++)
if (kp->ki_rgid == gidlist.l.gids[elem])
goto keepit;
}
if (jidlist.count > 0) {
for (elem = 0; elem < jidlist.count; elem++)
if (kp->ki_jid == jidlist.l.jids[elem])
goto keepit;
}
if (pgrplist.count > 0) {
for (elem = 0; elem < pgrplist.count; elem++)
if (kp->ki_pgid ==
pgrplist.l.pids[elem])
goto keepit;
}
if (ruidlist.count > 0) {
for (elem = 0; elem < ruidlist.count; elem++)
if (kp->ki_ruid ==
ruidlist.l.uids[elem])
goto keepit;
}
if (sesslist.count > 0) {
for (elem = 0; elem < sesslist.count; elem++)
if (kp->ki_sid == sesslist.l.pids[elem])
goto keepit;
}
if (ttylist.count > 0) {
for (elem = 0; elem < ttylist.count; elem++)
if (kp->ki_tdev == ttylist.l.ttys[elem])
goto keepit;
}
if (uidlist.count > 0) {
for (elem = 0; elem < uidlist.count; elem++)
if (kp->ki_uid == uidlist.l.uids[elem])
goto keepit;
}




continue;

keepit:
next_KINFO = &kinfo[nkept];
next_KINFO->ki_p = kp;
next_KINFO->ki_d.level = 0;
next_KINFO->ki_d.prefix = NULL;
next_KINFO->ki_pcpu = getpcpu(next_KINFO);
if (sortby == SORTMEM)
next_KINFO->ki_memsize = kp->ki_tsize +
kp->ki_dsize + kp->ki_ssize;
if (needuser)
saveuser(next_KINFO);
nkept++;
}
}

sizevars();

if (nkept == 0) {
printheader();
xo_finish();
exit(1);
}




qsort(kinfo, nkept, sizeof(KINFO), pscomp);




if (descendancy)
descendant_sort(kinfo, nkept);





for (i = 0; i < nkept; i++)
format_output(&kinfo[i]);




xo_open_container("process-information");
printheader();
if (xo_get_style(NULL) != XO_STYLE_TEXT)
termwidth = UNLIMITED;




xo_open_list("process");
for (i = lineno = 0; i < nkept; i++) {
linelen = 0;
xo_open_instance("process");
STAILQ_FOREACH(vent, &varlist, next_ve) {
ks = STAILQ_FIRST(&kinfo[i].ki_ks);
STAILQ_REMOVE_HEAD(&kinfo[i].ki_ks, ks_next);

fwidthmax = _POSIX2_LINE_MAX;
if (STAILQ_NEXT(vent, next_ve) == NULL &&
termwidth != UNLIMITED && ks->ks_str != NULL) {
left = termwidth - linelen;
if (left > 0 && left < (int)strlen(ks->ks_str))
fwidthmax = left;
}

str = ks->ks_str;
if (str == NULL)
str = "-";

fwidthmin = (xo_get_style(NULL) != XO_STYLE_TEXT ||
(STAILQ_NEXT(vent, next_ve) == NULL &&
(vent->var->flag & LJUST))) ? 0 : vent->var->width;
snprintf(fmtbuf, sizeof(fmtbuf), "{:%s/%%%s%d..%dhs}",
vent->var->field ? vent->var->field : vent->var->name,
(vent->var->flag & LJUST) ? "-" : "",
fwidthmin, fwidthmax);
xo_emit(fmtbuf, str);
linelen += fwidthmin;

if (ks->ks_str != NULL) {
free(ks->ks_str);
ks->ks_str = NULL;
}
free(ks);
ks = NULL;

if (STAILQ_NEXT(vent, next_ve) != NULL) {
xo_emit("{P: }");
linelen++;
}
}
xo_emit("\n");
xo_close_instance("process");
if (prtheader && lineno++ == prtheader - 4) {
xo_emit("\n");
printheader();
lineno = 0;
}
}
xo_close_list("process");
xo_close_container("process-information");
xo_finish();

free_list(&gidlist);
free_list(&jidlist);
free_list(&pidlist);
free_list(&pgrplist);
free_list(&ruidlist);
free_list(&sesslist);
free_list(&ttylist);
free_list(&uidlist);
for (i = 0; i < nkept; i++)
free(kinfo[i].ki_d.prefix);
free(kinfo);

exit(eval);
}

static int
addelem_gid(struct listinfo *inf, const char *elem)
{
struct group *grp;
const char *nameorID;
char *endp;
u_long bigtemp;

if (*elem == '\0' || strlen(elem) >= MAXLOGNAME) {
if (*elem == '\0')
xo_warnx("Invalid (zero-length) %s name", inf->lname);
else
xo_warnx("%s name too long: %s", inf->lname, elem);
optfatal = 1;
return (0);
}










grp = NULL;
nameorID = "named";
errno = 0;
bigtemp = strtoul(elem, &endp, 10);
if (errno == 0 && *endp == '\0' && bigtemp <= GID_MAX) {
nameorID = "name or ID matches";
grp = getgrgid((gid_t)bigtemp);
}
if (grp == NULL)
grp = getgrnam(elem);
if (grp == NULL) {
xo_warnx("No %s %s '%s'", inf->lname, nameorID, elem);
optfatal = 1;
return (0);
}
if (inf->count >= inf->maxcount)
expand_list(inf);
inf->l.gids[(inf->count)++] = grp->gr_gid;
return (1);
}

static int
addelem_jid(struct listinfo *inf, const char *elem)
{
int tempid;

if (*elem == '\0') {
warnx("Invalid (zero-length) jail id");
optfatal = 1;
return (0);
}

tempid = jail_getid(elem);
if (tempid < 0) {
warnx("Invalid %s: %s", inf->lname, elem);
optfatal = 1;
return (0);
}

if (inf->count >= inf->maxcount)
expand_list(inf);
inf->l.jids[(inf->count)++] = tempid;
return (1);
}

static int
addelem_pid(struct listinfo *inf, const char *elem)
{
char *endp;
long tempid;

if (*elem == '\0') {
xo_warnx("Invalid (zero-length) process id");
optfatal = 1;
return (0);
}

errno = 0;
tempid = strtol(elem, &endp, 10);
if (*endp != '\0' || tempid < 0 || elem == endp) {
xo_warnx("Invalid %s: %s", inf->lname, elem);
errno = ERANGE;
} else if (errno != 0 || tempid > pid_max) {
xo_warnx("%s too large: %s", inf->lname, elem);
errno = ERANGE;
}
if (errno == ERANGE) {
optfatal = 1;
return (0);
}
if (inf->count >= inf->maxcount)
expand_list(inf);
inf->l.pids[(inf->count)++] = tempid;
return (1);
}








static int
addelem_tty(struct listinfo *inf, const char *elem)
{
const char *ttypath;
struct stat sb;
char pathbuf[PATH_MAX], pathbuf2[PATH_MAX], pathbuf3[PATH_MAX];

ttypath = NULL;
pathbuf2[0] = '\0';
pathbuf3[0] = '\0';
switch (*elem) {
case '/':
ttypath = elem;
break;
case 'c':
if (strcmp(elem, "co") == 0) {
ttypath = _PATH_CONSOLE;
break;
}

default:
strlcpy(pathbuf, _PATH_DEV, sizeof(pathbuf));
strlcat(pathbuf, elem, sizeof(pathbuf));
ttypath = pathbuf;
if (strncmp(pathbuf, _PATH_TTY, strlen(_PATH_TTY)) == 0)
break;
if (strncmp(pathbuf, _PATH_PTS, strlen(_PATH_PTS)) == 0)
break;
if (strcmp(pathbuf, _PATH_CONSOLE) == 0)
break;

strlcpy(pathbuf2, _PATH_TTY, sizeof(pathbuf2));
strlcat(pathbuf2, elem, sizeof(pathbuf2));
if (stat(pathbuf2, &sb) == 0 && S_ISCHR(sb.st_mode)) {

ttypath = NULL;
break;
}

strlcpy(pathbuf3, _PATH_PTS, sizeof(pathbuf3));
strlcat(pathbuf3, elem, sizeof(pathbuf3));
if (stat(pathbuf3, &sb) == 0 && S_ISCHR(sb.st_mode)) {

ttypath = NULL;
break;
}
break;
}
if (ttypath) {
if (stat(ttypath, &sb) == -1) {
if (pathbuf3[0] != '\0')
xo_warn("%s, %s, and %s", pathbuf3, pathbuf2,
ttypath);
else
xo_warn("%s", ttypath);
optfatal = 1;
return (0);
}
if (!S_ISCHR(sb.st_mode)) {
if (pathbuf3[0] != '\0')
xo_warnx("%s, %s, and %s: Not a terminal",
pathbuf3, pathbuf2, ttypath);
else
xo_warnx("%s: Not a terminal", ttypath);
optfatal = 1;
return (0);
}
}
if (inf->count >= inf->maxcount)
expand_list(inf);
inf->l.ttys[(inf->count)++] = sb.st_rdev;
return (1);
}

static int
addelem_uid(struct listinfo *inf, const char *elem)
{
struct passwd *pwd;
char *endp;
u_long bigtemp;

if (*elem == '\0' || strlen(elem) >= MAXLOGNAME) {
if (*elem == '\0')
xo_warnx("Invalid (zero-length) %s name", inf->lname);
else
xo_warnx("%s name too long: %s", inf->lname, elem);
optfatal = 1;
return (0);
}

pwd = getpwnam(elem);
if (pwd == NULL) {
errno = 0;
bigtemp = strtoul(elem, &endp, 10);
if (errno != 0 || *endp != '\0' || bigtemp > UID_MAX)
xo_warnx("No %s named '%s'", inf->lname, elem);
else {

pwd = getpwuid((uid_t)bigtemp);
if (pwd == NULL)
xo_warnx("No %s name or ID matches '%s'",
inf->lname, elem);
}
}
if (pwd == NULL) {





optfatal = 1;
return (0);
}
if (inf->count >= inf->maxcount)
expand_list(inf);
inf->l.uids[(inf->count)++] = pwd->pw_uid;
return (1);
}

static void
add_list(struct listinfo *inf, const char *argp)
{
const char *savep;
char *cp, *endp;
int toolong;
char elemcopy[PATH_MAX];

if (*argp == '\0')
inf->addelem(inf, argp);
while (*argp != '\0') {
while (*argp != '\0' && strchr(W_SEP, *argp) != NULL)
argp++;
savep = argp;
toolong = 0;
cp = elemcopy;
if (strchr(T_SEP, *argp) == NULL) {
endp = elemcopy + sizeof(elemcopy) - 1;
while (*argp != '\0' && cp <= endp &&
strchr(W_SEP T_SEP, *argp) == NULL)
*cp++ = *argp++;
if (cp > endp)
toolong = 1;
}
if (!toolong) {
*cp = '\0';



inf->addelem(inf, elemcopy);
} else {




while (*argp != '\0' && strchr(W_SEP T_SEP,
*argp) == NULL)
argp++;
xo_warnx("Value too long: %.*s", (int)(argp - savep),
savep);
optfatal = 1;
}





while (*argp != '\0' && strchr(W_SEP, *argp) != NULL)
argp++;
if (*argp != '\0' && strchr(T_SEP, *argp) != NULL) {
argp++;

if (*argp == '\0')
inf->addelem(inf, argp);
}
}
}

static void
descendant_sort(KINFO *ki, int items)
{
int dst, lvl, maxlvl, n, ndst, nsrc, siblings, src;
unsigned char *path;
KINFO kn;





src = 0;
maxlvl = 0;
while (src < items) {
if (ki[src].ki_d.level) {
src++;
continue;
}
for (nsrc = 1; src + nsrc < items; nsrc++)
if (!ki[src + nsrc].ki_d.level)
break;

for (dst = 0; dst < items; dst++) {
if (ki[dst].ki_p->ki_pid == ki[src].ki_p->ki_pid)
continue;
if (ki[dst].ki_p->ki_pid == ki[src].ki_p->ki_ppid)
break;
}

if (dst == items) {
src += nsrc;
continue;
}

for (ndst = 1; dst + ndst < items; ndst++)
if (ki[dst + ndst].ki_d.level <= ki[dst].ki_d.level)
break;

for (n = src; n < src + nsrc; n++) {
ki[n].ki_d.level += ki[dst].ki_d.level + 1;
if (maxlvl < ki[n].ki_d.level)
maxlvl = ki[n].ki_d.level;
}

while (nsrc) {
if (src < dst) {
kn = ki[src];
memmove(ki + src, ki + src + 1,
(dst - src + ndst - 1) * sizeof *ki);
ki[dst + ndst - 1] = kn;
nsrc--;
dst--;
ndst++;
} else if (src != dst + ndst) {
kn = ki[src];
memmove(ki + dst + ndst + 1, ki + dst + ndst,
(src - dst - ndst) * sizeof *ki);
ki[dst + ndst] = kn;
ndst++;
nsrc--;
src++;
} else {
ndst += nsrc;
src += nsrc;
nsrc = 0;
}
}
}





path = malloc((maxlvl + 7) / 8);
memset(path, '\0', (maxlvl + 7) / 8);
for (src = 0; src < items; src++) {
if ((lvl = ki[src].ki_d.level) == 0) {
ki[src].ki_d.prefix = NULL;
continue;
}
if ((ki[src].ki_d.prefix = malloc(lvl * 2 + 1)) == NULL)
xo_errx(1, "malloc failed");
for (n = 0; n < lvl - 2; n++) {
ki[src].ki_d.prefix[n * 2] =
path[n / 8] & 1 << (n % 8) ? '|' : ' ';
ki[src].ki_d.prefix[n * 2 + 1] = ' ';
}
if (n == lvl - 2) {

for (siblings = 0, dst = src + 1; dst < items; dst++) {
if (ki[dst].ki_d.level > lvl)
continue;
if (ki[dst].ki_d.level == lvl)
siblings = 1;
break;
}
if (siblings)
path[n / 8] |= 1 << (n % 8);
else
path[n / 8] &= ~(1 << (n % 8));
ki[src].ki_d.prefix[n * 2] = siblings ? '|' : '`';
ki[src].ki_d.prefix[n * 2 + 1] = '-';
n++;
}
strcpy(ki[src].ki_d.prefix + n * 2, "- ");
}
free(path);
}

static void *
expand_list(struct listinfo *inf)
{
void *newlist;
int newmax;

newmax = (inf->maxcount + 1) << 1;
newlist = realloc(inf->l.ptr, newmax * inf->elemsize);
if (newlist == NULL) {
free(inf->l.ptr);
xo_errx(1, "realloc to %d %ss failed", newmax, inf->lname);
}
inf->maxcount = newmax;
inf->l.ptr = newlist;

return (newlist);
}

static void
free_list(struct listinfo *inf)
{

inf->count = inf->elemsize = inf->maxcount = 0;
if (inf->l.ptr != NULL)
free(inf->l.ptr);
inf->addelem = NULL;
inf->lname = NULL;
inf->l.ptr = NULL;
}

static void
init_list(struct listinfo *inf, addelem_rtn artn, int elemsize,
const char *lname)
{

inf->count = inf->maxcount = 0;
inf->elemsize = elemsize;
inf->addelem = artn;
inf->lname = lname;
inf->l.ptr = NULL;
}

VARENT *
find_varentry(VAR *v)
{
struct varent *vent;

STAILQ_FOREACH(vent, &varlist, next_ve) {
if (strcmp(vent->var->name, v->name) == 0)
return vent;
}
return NULL;
}

static void
scanvars(void)
{
struct varent *vent;
VAR *v;

STAILQ_FOREACH(vent, &varlist, next_ve) {
v = vent->var;
if (v->flag & USER)
needuser = 1;
if (v->flag & COMM)
needcomm = 1;
}
}

static void
format_output(KINFO *ki)
{
struct varent *vent;
VAR *v;
KINFO_STR *ks;
char *str;
int len;

STAILQ_INIT(&ki->ki_ks);
STAILQ_FOREACH(vent, &varlist, next_ve) {
v = vent->var;
str = (v->oproc)(ki, vent);
ks = malloc(sizeof(*ks));
if (ks == NULL)
xo_errx(1, "malloc failed");
ks->ks_str = str;
STAILQ_INSERT_TAIL(&ki->ki_ks, ks, ks_next);
if (str != NULL) {
len = strlen(str);
} else
len = 1;
if (v->width < len)
v->width = len;
}
}

static void
sizevars(void)
{
struct varent *vent;
VAR *v;
int i;

STAILQ_FOREACH(vent, &varlist, next_ve) {
v = vent->var;
i = strlen(vent->header);
if (v->width < i)
v->width = i;
}
}

static const char *
fmt(char **(*fn)(kvm_t *, const struct kinfo_proc *, int), KINFO *ki,
char *comm, char *thread, int maxlen)
{
const char *s;

s = fmt_argv((*fn)(kd, ki->ki_p, termwidth), comm,
showthreads && ki->ki_p->ki_numthreads > 1 ? thread : NULL, maxlen);
return (s);
}

#define UREADOK(ki) (forceuread || (ki->ki_p->ki_flag & P_INMEM))

static void
saveuser(KINFO *ki)
{
char tdname[COMMLEN + 1];
char *argsp;

if (ki->ki_p->ki_flag & P_INMEM) {






ki->ki_valid = 1;
} else
ki->ki_valid = 0;



if (needcomm) {
if (ki->ki_p->ki_stat == SZOMB) {
ki->ki_args = strdup("<defunct>");
} else if (UREADOK(ki) || (ki->ki_p->ki_args != NULL)) {
(void)snprintf(tdname, sizeof(tdname), "%s%s",
ki->ki_p->ki_tdname, ki->ki_p->ki_moretdname);
ki->ki_args = fmt(kvm_getargv, ki,
ki->ki_p->ki_comm, tdname, COMMLEN * 2 + 1);
} else {
asprintf(&argsp, "(%s)", ki->ki_p->ki_comm);
ki->ki_args = argsp;
}
if (ki->ki_args == NULL)
xo_errx(1, "malloc failed");
} else {
ki->ki_args = NULL;
}
if (needenv) {
if (UREADOK(ki))
ki->ki_env = fmt(kvm_getenvv, ki,
(char *)NULL, (char *)NULL, 0);
else
ki->ki_env = strdup("()");
if (ki->ki_env == NULL)
xo_errx(1, "malloc failed");
} else {
ki->ki_env = NULL;
}
}


#define DIFF_RETURN(a, b, field) do { if ((a)->field != (b)->field) return (((a)->field < (b)->field) ? -1 : 1); } while (0)




static int
pscomp(const void *a, const void *b)
{
const KINFO *ka, *kb;

ka = a;
kb = b;

if (sortby == SORTCPU)
DIFF_RETURN(kb, ka, ki_pcpu);
if (sortby == SORTMEM)
DIFF_RETURN(kb, ka, ki_memsize);




if (ka->ki_p->ki_tdev != kb->ki_p->ki_tdev) {
if (ka->ki_p->ki_tdev == NODEV)
return (-1);
if (kb->ki_p->ki_tdev == NODEV)
return (1);
DIFF_RETURN(ka, kb, ki_p->ki_tdev);
}


DIFF_RETURN(ka, kb, ki_p->ki_pid);
DIFF_RETURN(ka, kb, ki_p->ki_tid);
return (0);
}
#undef DIFF_RETURN












static char *
kludge_oldps_options(const char *optlist, char *origval, const char *nextarg)
{
size_t len;
char *argp, *cp, *newopts, *ns, *optp, *pidp;





argp = NULL;
if (optlist != NULL) {
for (cp = origval; *cp != '\0'; cp++) {
optp = strchr(optlist, *cp);
if ((optp != NULL) && *(optp + 1) == ':') {
argp = cp;
break;
}
}
}
if (argp != NULL && *origval == '-')
return (origval);













len = strlen(origval);
cp = origval + len - 1;
pidp = NULL;
if (*cp == 't' && *origval != '-' && cp == argp) {
if (nextarg == NULL || *nextarg == '-' || isdigitch(*nextarg))
*cp = 'T';
} else if (argp == NULL) {






if (isdigitch(*cp)) {
while (cp >= origval && (*cp == ',' || isdigitch(*cp)))
--cp;
pidp = cp + 1;
}
}





if (*origval == '-' && pidp == NULL)
return (origval);





if ((newopts = ns = malloc(len + 3)) == NULL)
xo_errx(1, "malloc failed");

if (*origval != '-')
*ns++ = '-';

if (pidp == NULL)
strcpy(ns, origval);
else {




len = pidp - origval;
memcpy(ns, origval, len);
ns += len;
*ns++ = 'p';
strcpy(ns, pidp);
}

return (newopts);
}

static void
pidmax_init(void)
{
size_t intsize;

intsize = sizeof(pid_max);
if (sysctlbyname("kern.pid_max", &pid_max, &intsize, NULL, 0) < 0) {
xo_warn("unable to read kern.pid_max");
pid_max = 99999;
}
}

static void __dead2
usage(void)
{
#define SINGLE_OPTS "[-aCcde" OPT_LAZY_f "HhjlmrSTuvwXxZ]"

(void)xo_error("%s\n%s\n%s\n%s\n",
"usage: ps " SINGLE_OPTS " [-O fmt | -o fmt] [-G gid[,gid...]]",
" [-J jid[,jid...]] [-M core] [-N system]",
" [-p pid[,pid...]] [-t tty[,tty...]] [-U user[,user...]]",
" ps [-L]");
exit(1);
}
