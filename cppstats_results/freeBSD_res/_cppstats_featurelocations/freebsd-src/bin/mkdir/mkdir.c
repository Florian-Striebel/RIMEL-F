






























#if 0
#if !defined(lint)
static char const copyright[] =
"@(#) Copyright (c) 1983, 1992, 1993\n\
The Regents of the University of California. All rights reserved.\n";
#endif

#if !defined(lint)
static char sccsid[] = "@(#)mkdir.c 8.2 (Berkeley) 1/25/94";
#endif
#endif
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/types.h>
#include <sys/stat.h>

#include <err.h>
#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

static int build(char *, mode_t);
static void usage(void);

static int vflag;

int
main(int argc, char *argv[])
{
int ch, exitval, success, pflag;
mode_t omode;
void *set = NULL;
char *mode;

omode = pflag = 0;
mode = NULL;
while ((ch = getopt(argc, argv, "m:pv")) != -1)
switch(ch) {
case 'm':
mode = optarg;
break;
case 'p':
pflag = 1;
break;
case 'v':
vflag = 1;
break;
case '?':
default:
usage();
}

argc -= optind;
argv += optind;
if (argv[0] == NULL)
usage();

if (mode == NULL) {
omode = S_IRWXU | S_IRWXG | S_IRWXO;
} else {
if ((set = setmode(mode)) == NULL)
errx(1, "invalid file mode: %s", mode);
omode = getmode(set, S_IRWXU | S_IRWXG | S_IRWXO);
free(set);
}

for (exitval = 0; *argv != NULL; ++argv) {
if (pflag) {
success = build(*argv, omode);
} else if (mkdir(*argv, omode) < 0) {
if (errno == ENOTDIR || errno == ENOENT)
warn("%s", dirname(*argv));
else
warn("%s", *argv);
success = 0;
} else {
success = 1;
if (vflag)
(void)printf("%s\n", *argv);
}
if (!success)
exitval = 1;








if (success == 1 && mode != NULL && chmod(*argv, omode) == -1) {
warn("%s", *argv);
exitval = 1;
}
}
exit(exitval);
}






static int
build(char *path, mode_t omode)
{
struct stat sb;
mode_t numask, oumask;
int first, last, retval;
char *p;

p = path;
oumask = 0;
retval = 1;
if (p[0] == '/')
++p;
for (first = 1, last = 0; !last ; ++p) {
if (p[0] == '\0')
last = 1;
else if (p[0] != '/')
continue;
*p = '\0';
if (!last && p[1] == '\0')
last = 1;
if (first) {












oumask = umask(0);
numask = oumask & ~(S_IWUSR | S_IXUSR);
(void)umask(numask);
first = 0;
}
if (last)
(void)umask(oumask);
if (mkdir(path, last ? omode : S_IRWXU | S_IRWXG | S_IRWXO) < 0) {
if (errno == EEXIST || errno == EISDIR) {
if (stat(path, &sb) < 0) {
warn("%s", path);
retval = 0;
break;
} else if (!S_ISDIR(sb.st_mode)) {
if (last)
errno = EEXIST;
else
errno = ENOTDIR;
warn("%s", path);
retval = 0;
break;
}
if (last)
retval = 2;
} else {
warn("%s", path);
retval = 0;
break;
}
} else if (vflag)
printf("%s\n", path);
if (!last)
*p = '/';
}
if (!first && !last)
(void)umask(oumask);
return (retval);
}

static void
usage(void)
{

(void)fprintf(stderr,
"usage: mkdir [-pv] [-m mode] directory_name ...\n");
exit (EX_USAGE);
}
