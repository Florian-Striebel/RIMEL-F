

































#if !defined(lint)
static const char copyright[] =
"@(#) Copyright (c) 1989, 1993, 1994\n\
The Regents of the University of California. All rights reserved.\n";
#endif

#if 0
#if !defined(lint)
static char sccsid[] = "@(#)ls.c 8.5 (Berkeley) 4/2/94";
#endif
#endif
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/param.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mac.h>

#include <ctype.h>
#include <dirent.h>
#include <err.h>
#include <errno.h>
#include <fts.h>
#include <getopt.h>
#include <grp.h>
#include <inttypes.h>
#include <limits.h>
#include <locale.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#if defined(COLORLS)
#include <termcap.h>
#include <signal.h>
#endif

#include "ls.h"
#include "extern.h"






#define STRBUF_SIZEOF(t) (1 + CHAR_BIT * sizeof(t) / 3 + 1)






#define MAKENINES(n) do { intmax_t __i; for (__i = 1; n > 0; __i *= 10) n--; n = __i - 1; } while(0)









static void display(const FTSENT *, FTSENT *, int);
static int mastercmp(const FTSENT * const *, const FTSENT * const *);
static void traverse(int, char **, int);

#define COLOR_OPT (CHAR_MAX + 1)

static const struct option long_opts[] =
{
{"color", optional_argument, NULL, COLOR_OPT},
{NULL, no_argument, NULL, 0}
};

static void (*printfcn)(const DISPLAY *);
static int (*sortfcn)(const FTSENT *, const FTSENT *);

long blocksize;
int termwidth = 80;


int f_accesstime;
int f_birthtime;
int f_flags;
int f_humanval;
int f_inode;
static int f_kblocks;
int f_label;
static int f_listdir;
static int f_listdot;
int f_longform;
static int f_noautodot;
static int f_nofollow;
int f_nonprint;
static int f_nosort;
int f_notabs;
static int f_numericonly;
int f_octal;
int f_octal_escape;
static int f_recursive;
static int f_reversesort;
int f_samesort;
int f_sectime;
static int f_singlecol;
int f_size;
static int f_sizesort;
int f_slash;
int f_sortacross;
int f_statustime;
static int f_stream;
int f_thousands;
char *f_timeformat;
static int f_timesort;
int f_type;
static int f_whiteout;
#if defined(COLORLS)
int colorflag = COLORFLAG_NEVER;
int f_color;
bool explicitansi;
char *ansi_bgcol;
char *ansi_fgcol;
char *ansi_coloff;
char *attrs_off;
char *enter_bold;
char *enter_underline;
#endif

static int rval;

static bool
do_color_from_env(void)
{
const char *p;
bool doit;

doit = false;
p = getenv("CLICOLOR");
if (p == NULL) {




p = getenv("COLORTERM");
if (p != NULL && *p != '\0')
doit = true;
} else
doit = true;

return (doit &&
(isatty(STDOUT_FILENO) || getenv("CLICOLOR_FORCE")));
}

static bool
do_color(void)
{

#if defined(COLORLS)
if (colorflag == COLORFLAG_NEVER)
return (false);
else if (colorflag == COLORFLAG_ALWAYS)
return (true);
#endif
return (do_color_from_env());
}

#if defined(COLORLS)
static bool
do_color_always(const char *term)
{

return (strcmp(term, "always") == 0 || strcmp(term, "yes") == 0 ||
strcmp(term, "force") == 0);
}

static bool
do_color_never(const char *term)
{

return (strcmp(term, "never") == 0 || strcmp(term, "no") == 0 ||
strcmp(term, "none") == 0);
}

static bool
do_color_auto(const char *term)
{

return (strcmp(term, "auto") == 0 || strcmp(term, "tty") == 0 ||
strcmp(term, "if-tty") == 0);
}
#endif

int
main(int argc, char *argv[])
{
static char dot[] = ".", *dotav[] = {dot, NULL};
struct winsize win;
int ch, fts_options, notused;
char *p;
const char *errstr = NULL;
#if defined(COLORLS)
char termcapbuf[1024];
char tcapbuf[512];
char *bp = tcapbuf, *term;
#endif

(void)setlocale(LC_ALL, "");


if (isatty(STDOUT_FILENO)) {
termwidth = 80;
if ((p = getenv("COLUMNS")) != NULL && *p != '\0')
termwidth = strtonum(p, 0, INT_MAX, &errstr);
else if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &win) != -1 &&
win.ws_col > 0)
termwidth = win.ws_col;
f_nonprint = 1;
} else {
f_singlecol = 1;

p = getenv("COLUMNS");
if (p)
termwidth = strtonum(p, 0, INT_MAX, &errstr);
}

if (errstr)
termwidth = 80;

fts_options = FTS_PHYSICAL;
if (getenv("LS_SAMESORT"))
f_samesort = 1;





#if defined(COLORLS)
if (getenv("CLICOLOR"))
colorflag = COLORFLAG_AUTO;
#endif
while ((ch = getopt_long(argc, argv,
"+1ABCD:FGHILPRSTUWXZabcdfghiklmnopqrstuwxy,", long_opts,
NULL)) != -1) {
switch (ch) {




case '1':
f_singlecol = 1;
f_longform = 0;
f_stream = 0;
break;
case 'C':
f_sortacross = f_longform = f_singlecol = 0;
break;
case 'l':
f_longform = 1;
f_singlecol = 0;
f_stream = 0;
break;
case 'x':
f_sortacross = 1;
f_longform = 0;
f_singlecol = 0;
break;

case 'c':
f_statustime = 1;
f_accesstime = 0;
f_birthtime = 0;
break;
case 'u':
f_accesstime = 1;
f_statustime = 0;
f_birthtime = 0;
break;
case 'U':
f_birthtime = 1;
f_accesstime = 0;
f_statustime = 0;
break;
case 'f':
f_nosort = 1;

case 'a':
fts_options |= FTS_SEEDOT;

case 'A':
f_listdot = 1;
break;

case 'S':
f_sizesort = 1;
f_timesort = 0;
break;
case 't':
f_timesort = 1;
f_sizesort = 0;
break;

case ',':
f_thousands = 1;
break;
case 'B':
f_nonprint = 0;
f_octal = 1;
f_octal_escape = 0;
break;
case 'D':
f_timeformat = optarg;
break;
case 'F':
f_type = 1;
f_slash = 0;
break;
case 'G':





setenv("CLICOLOR", "", 1);
#if defined(COLORLS)
colorflag = COLORFLAG_AUTO;
#endif
break;
case 'H':
fts_options |= FTS_COMFOLLOW;
f_nofollow = 0;
break;
case 'I':
f_noautodot = 1;
break;
case 'L':
fts_options &= ~FTS_PHYSICAL;
fts_options |= FTS_LOGICAL;
f_nofollow = 0;
break;
case 'P':
fts_options &= ~FTS_COMFOLLOW;
fts_options &= ~FTS_LOGICAL;
fts_options |= FTS_PHYSICAL;
f_nofollow = 1;
break;
case 'R':
f_recursive = 1;
break;
case 'T':
f_sectime = 1;
break;
case 'W':
f_whiteout = 1;
break;
case 'Z':
f_label = 1;
break;
case 'b':
f_nonprint = 0;
f_octal = 0;
f_octal_escape = 1;
break;

case 'd':
f_listdir = 1;
f_recursive = 0;
break;
case 'g':
break;
case 'h':
f_humanval = 1;
break;
case 'i':
f_inode = 1;
break;
case 'k':
f_humanval = 0;
f_kblocks = 1;
break;
case 'm':
f_stream = 1;
f_singlecol = 0;
f_longform = 0;
break;
case 'n':
f_numericonly = 1;
break;
case 'o':
f_flags = 1;
break;
case 'p':
f_slash = 1;
f_type = 1;
break;
case 'q':
f_nonprint = 1;
f_octal = 0;
f_octal_escape = 0;
break;
case 'r':
f_reversesort = 1;
break;
case 's':
f_size = 1;
break;
case 'w':
f_nonprint = 0;
f_octal = 0;
f_octal_escape = 0;
break;
case 'y':
f_samesort = 1;
break;
case COLOR_OPT:
#if defined(COLORLS)
if (optarg == NULL || do_color_always(optarg))
colorflag = COLORFLAG_ALWAYS;
else if (do_color_auto(optarg))
colorflag = COLORFLAG_AUTO;
else if (do_color_never(optarg))
colorflag = COLORFLAG_NEVER;
else
errx(2, "unsupported --color value '%s' (must be always, auto, or never)",
optarg);
break;
#else
warnx("color support not compiled in");
#endif
default:
case '?':
usage();
}
}
argc -= optind;
argv += optind;


if (!f_listdot && getuid() == (uid_t)0 && !f_noautodot)
f_listdot = 1;





if (do_color()) {
#if defined(COLORLS)
if ((term = getenv("TERM")) != NULL &&
tgetent(termcapbuf, term) == 1) {
ansi_fgcol = tgetstr("AF", &bp);
ansi_bgcol = tgetstr("AB", &bp);
attrs_off = tgetstr("me", &bp);
enter_bold = tgetstr("md", &bp);
enter_underline = tgetstr("us", &bp);




ansi_coloff = tgetstr("op", &bp);
if (!ansi_coloff)
ansi_coloff = tgetstr("oc", &bp);
if (ansi_fgcol && ansi_bgcol && ansi_coloff)
f_color = 1;
} else if (colorflag == COLORFLAG_ALWAYS) {





f_color = 1;
explicitansi = true;
}
#endif
}

#if defined(COLORLS)
if (f_color) {





f_notabs = 1;
(void)signal(SIGINT, colorquit);
(void)signal(SIGQUIT, colorquit);
parsecolors(getenv("LSCOLORS"));
}
#endif






if (!f_inode && !f_longform && !f_size && !f_timesort &&
!f_sizesort && !f_type
#if defined(COLORLS)
&& !f_color
#endif
)
fts_options |= FTS_NOSTAT;






if (!f_nofollow && !f_longform && !f_listdir && (!f_type || f_slash)
#if defined(COLORLS)
&& !f_color
#endif
)
fts_options |= FTS_COMFOLLOW;




#if defined(FTS_WHITEOUT)
if (f_whiteout)
fts_options |= FTS_WHITEOUT;
#endif


if (f_inode || f_longform || f_size) {
if (f_kblocks)
blocksize = 2;
else {
(void)getbsize(&notused, &blocksize);
blocksize /= 512;
}
}

if (f_reversesort) {
if (!f_timesort && !f_sizesort)
sortfcn = revnamecmp;
else if (f_sizesort)
sortfcn = revsizecmp;
else if (f_accesstime)
sortfcn = revacccmp;
else if (f_birthtime)
sortfcn = revbirthcmp;
else if (f_statustime)
sortfcn = revstatcmp;
else
sortfcn = revmodcmp;
} else {
if (!f_timesort && !f_sizesort)
sortfcn = namecmp;
else if (f_sizesort)
sortfcn = sizecmp;
else if (f_accesstime)
sortfcn = acccmp;
else if (f_birthtime)
sortfcn = birthcmp;
else if (f_statustime)
sortfcn = statcmp;
else
sortfcn = modcmp;
}


if (f_singlecol)
printfcn = printscol;
else if (f_longform)
printfcn = printlong;
else if (f_stream)
printfcn = printstream;
else
printfcn = printcol;

if (argc)
traverse(argc, argv, fts_options);
else
traverse(1, dotav, fts_options);
exit(rval);
}

static int output;







static void
traverse(int argc, char *argv[], int options)
{
FTS *ftsp;
FTSENT *p, *chp;
int ch_options;

if ((ftsp =
fts_open(argv, options, f_nosort ? NULL : mastercmp)) == NULL)
err(1, "fts_open");





chp = fts_children(ftsp, 0);
if (chp != NULL)
display(NULL, chp, options);
if (f_listdir)
return;





ch_options = !f_recursive && !f_label &&
options & FTS_NOSTAT ? FTS_NAMEONLY : 0;

while (errno = 0, (p = fts_read(ftsp)) != NULL)
switch (p->fts_info) {
case FTS_DC:
warnx("%s: directory causes a cycle", p->fts_name);
break;
case FTS_DNR:
case FTS_ERR:
warnx("%s: %s", p->fts_path, strerror(p->fts_errno));
rval = 1;
break;
case FTS_D:
if (p->fts_level != FTS_ROOTLEVEL &&
p->fts_name[0] == '.' && !f_listdot)
break;






if (output) {
putchar('\n');
(void)printname(p->fts_path);
puts(":");
} else if (argc > 1) {
(void)printname(p->fts_path);
puts(":");
output = 1;
}
chp = fts_children(ftsp, ch_options);
display(p, chp, options);

if (!f_recursive && chp != NULL)
(void)fts_set(ftsp, p, FTS_SKIP);
break;
default:
break;
}
if (errno)
err(1, "fts_read");
}






static void
display(const FTSENT *p, FTSENT *list, int options)
{
struct stat *sp;
DISPLAY d;
FTSENT *cur;
NAMES *np;
off_t maxsize;
long maxblock;
uintmax_t maxinode;
u_long btotal, labelstrlen, maxlen, maxnlink;
u_long maxlabelstr;
u_int sizelen;
int maxflags;
gid_t maxgroup;
uid_t maxuser;
size_t flen, ulen, glen;
char *initmax;
int entries, needstats;
const char *user, *group;
char *flags, *labelstr = NULL;
char ngroup[STRBUF_SIZEOF(uid_t) + 1];
char nuser[STRBUF_SIZEOF(gid_t) + 1];
u_long width[9];
int i;

needstats = f_inode || f_longform || f_size;
flen = 0;
btotal = 0;

#define LS_COLWIDTHS_FIELDS 9
initmax = getenv("LS_COLWIDTHS");

for (i = 0 ; i < LS_COLWIDTHS_FIELDS; i++)
width[i] = 0;

if (initmax != NULL) {
char *endp;

for (i = 0; i < LS_COLWIDTHS_FIELDS && *initmax != '\0'; i++) {
if (*initmax == ':') {
width[i] = 0;
} else {
width[i] = strtoul(initmax, &endp, 10);
initmax = endp;
while (isspace(*initmax))
initmax++;
if (*initmax != ':')
break;
initmax++;
}
}
if (i < LS_COLWIDTHS_FIELDS)
#if defined(COLORLS)
if (!f_color)
#endif
f_notabs = 0;
}


maxinode = width[0];
maxblock = width[1];
maxnlink = width[2];
maxuser = width[3];
maxgroup = width[4];
maxflags = width[5];
maxsize = width[6];
maxlen = width[7];
maxlabelstr = width[8];

MAKENINES(maxinode);
MAKENINES(maxblock);
MAKENINES(maxnlink);
MAKENINES(maxsize);

d.s_size = 0;
sizelen = 0;
flags = NULL;
for (cur = list, entries = 0; cur; cur = cur->fts_link) {
if (cur->fts_info == FTS_ERR || cur->fts_info == FTS_NS) {
warnx("%s: %s",
cur->fts_name, strerror(cur->fts_errno));
cur->fts_number = NO_PRINT;
rval = 1;
continue;
}




if (p == NULL) {

if (cur->fts_info == FTS_D && !f_listdir) {
cur->fts_number = NO_PRINT;
continue;
}
} else {

if (cur->fts_name[0] == '.' && !f_listdot) {
cur->fts_number = NO_PRINT;
continue;
}
}
if (cur->fts_namelen > maxlen)
maxlen = cur->fts_namelen;
if (f_octal || f_octal_escape) {
u_long t = len_octal(cur->fts_name, cur->fts_namelen);

if (t > maxlen)
maxlen = t;
}
if (needstats) {
sp = cur->fts_statp;
if (sp->st_blocks > maxblock)
maxblock = sp->st_blocks;
if (sp->st_ino > maxinode)
maxinode = sp->st_ino;
if (sp->st_nlink > maxnlink)
maxnlink = sp->st_nlink;
if (sp->st_size > maxsize)
maxsize = sp->st_size;

btotal += sp->st_blocks;
if (f_longform) {
if (f_numericonly) {
(void)snprintf(nuser, sizeof(nuser),
"%u", sp->st_uid);
(void)snprintf(ngroup, sizeof(ngroup),
"%u", sp->st_gid);
user = nuser;
group = ngroup;
} else {
user = user_from_uid(sp->st_uid, 0);









if (user == NULL)
err(1, "user_from_uid");
group = group_from_gid(sp->st_gid, 0);

if (group == NULL)
err(1, "group_from_gid");
}
if ((ulen = strlen(user)) > maxuser)
maxuser = ulen;
if ((glen = strlen(group)) > maxgroup)
maxgroup = glen;
if (f_flags) {
flags = fflagstostr(sp->st_flags);
if (flags != NULL && *flags == '\0') {
free(flags);
flags = strdup("-");
}
if (flags == NULL)
err(1, "fflagstostr");
flen = strlen(flags);
if (flen > (size_t)maxflags)
maxflags = flen;
} else
flen = 0;
labelstr = NULL;
if (f_label) {
char name[PATH_MAX + 1];
mac_t label;
int error;

error = mac_prepare_file_label(&label);
if (error == -1) {
warn("MAC label for %s/%s",
cur->fts_parent->fts_path,
cur->fts_name);
goto label_out;
}

if (cur->fts_level == FTS_ROOTLEVEL)
snprintf(name, sizeof(name),
"%s", cur->fts_name);
else
snprintf(name, sizeof(name),
"%s/%s", cur->fts_parent->
fts_accpath, cur->fts_name);

if (options & FTS_LOGICAL)
error = mac_get_file(name,
label);
else
error = mac_get_link(name,
label);
if (error == -1) {
warn("MAC label for %s/%s",
cur->fts_parent->fts_path,
cur->fts_name);
mac_free(label);
goto label_out;
}

error = mac_to_text(label,
&labelstr);
if (error == -1) {
warn("MAC label for %s/%s",
cur->fts_parent->fts_path,
cur->fts_name);
mac_free(label);
goto label_out;
}
mac_free(label);
label_out:
if (labelstr == NULL)
labelstr = strdup("-");
labelstrlen = strlen(labelstr);
if (labelstrlen > maxlabelstr)
maxlabelstr = labelstrlen;
} else
labelstrlen = 0;

if ((np = malloc(sizeof(NAMES) + labelstrlen +
ulen + glen + flen + 4)) == NULL)
err(1, "malloc");

np->user = &np->data[0];
(void)strcpy(np->user, user);
np->group = &np->data[ulen + 1];
(void)strcpy(np->group, group);

if (S_ISCHR(sp->st_mode) ||
S_ISBLK(sp->st_mode)) {
sizelen = snprintf(NULL, 0,
"%#jx", (uintmax_t)sp->st_rdev);
if (d.s_size < sizelen)
d.s_size = sizelen;
}

if (f_flags) {
np->flags = &np->data[ulen + glen + 2];
(void)strcpy(np->flags, flags);
free(flags);
}
if (f_label) {
np->label = &np->data[ulen + glen + 2
+ (f_flags ? flen + 1 : 0)];
(void)strcpy(np->label, labelstr);
free(labelstr);
}
cur->fts_pointer = np;
}
}
++entries;
}







if (!entries && (!(f_longform || f_size) || p == NULL))
return;

d.list = list;
d.entries = entries;
d.maxlen = maxlen;
if (needstats) {
d.btotal = btotal;
d.s_block = snprintf(NULL, 0, "%lu", howmany(maxblock, blocksize));
d.s_flags = maxflags;
d.s_label = maxlabelstr;
d.s_group = maxgroup;
d.s_inode = snprintf(NULL, 0, "%ju", maxinode);
d.s_nlink = snprintf(NULL, 0, "%lu", maxnlink);
sizelen = f_humanval ? HUMANVALSTR_LEN :
snprintf(NULL, 0, "%ju", maxsize);
if (d.s_size < sizelen)
d.s_size = sizelen;
d.s_user = maxuser;
}
if (f_thousands)
d.s_size += (d.s_size - 1) / 3;
printfcn(&d);
output = 1;

if (f_longform)
for (cur = list; cur; cur = cur->fts_link)
free(cur->fts_pointer);
}







static int
mastercmp(const FTSENT * const *a, const FTSENT * const *b)
{
int a_info, b_info;

a_info = (*a)->fts_info;
if (a_info == FTS_ERR)
return (0);
b_info = (*b)->fts_info;
if (b_info == FTS_ERR)
return (0);

if (a_info == FTS_NS || b_info == FTS_NS)
return (namecmp(*a, *b));

if (a_info != b_info &&
(*a)->fts_level == FTS_ROOTLEVEL && !f_listdir) {
if (a_info == FTS_D)
return (1);
if (b_info == FTS_D)
return (-1);
}
return (sortfcn(*a, *b));
}
