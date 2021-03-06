


































#if 0
#if !defined(lint)
static char const copyright[] =
"@(#) Copyright (c) 1988, 1993, 1994\n\
The Regents of the University of California. All rights reserved.\n";
#endif

#if !defined(lint)
static char sccsid[] = "@(#)kill.c 8.4 (Berkeley) 4/28/95";
#endif
#endif
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(SHELL)
#define main killcmd
#include "bltin/bltin.h"
#endif

static void nosig(const char *);
static void printsignals(FILE *);
static int signame_to_signum(const char *);
static void usage(void);

int
main(int argc, char *argv[])
{
long pidl;
pid_t pid;
int errors, numsig, ret;
char *ep;

if (argc < 2)
usage();

numsig = SIGTERM;

argc--, argv++;
if (!strcmp(*argv, "-l")) {
argc--, argv++;
if (argc > 1)
usage();
if (argc == 1) {
if (!isdigit(**argv))
usage();
numsig = strtol(*argv, &ep, 10);
if (!**argv || *ep)
errx(2, "illegal signal number: %s", *argv);
if (numsig >= 128)
numsig -= 128;
if (numsig <= 0 || numsig >= sys_nsig)
nosig(*argv);
printf("%s\n", sys_signame[numsig]);
return (0);
}
printsignals(stdout);
return (0);
}

if (!strcmp(*argv, "-s")) {
argc--, argv++;
if (argc < 1) {
warnx("option requires an argument -- s");
usage();
}
if (strcmp(*argv, "0")) {
if ((numsig = signame_to_signum(*argv)) < 0)
nosig(*argv);
} else
numsig = 0;
argc--, argv++;
} else if (**argv == '-' && *(*argv + 1) != '-') {
++*argv;
if (isalpha(**argv)) {
if ((numsig = signame_to_signum(*argv)) < 0)
nosig(*argv);
} else if (isdigit(**argv)) {
numsig = strtol(*argv, &ep, 10);
if (!**argv || *ep)
errx(2, "illegal signal number: %s", *argv);
if (numsig < 0)
nosig(*argv);
} else
nosig(*argv);
argc--, argv++;
}

if (argc > 0 && strncmp(*argv, "--", 2) == 0)
argc--, argv++;

if (argc == 0)
usage();

for (errors = 0; argc; argc--, argv++) {
#if defined(SHELL)
if (**argv == '%')
ret = killjob(*argv, numsig);
else
#endif
{
pidl = strtol(*argv, &ep, 10);

pid = (pid_t)pidl;
if (!**argv || *ep || pid != pidl)
errx(2, "illegal process id: %s", *argv);
ret = kill(pid, numsig);
}
if (ret == -1) {
warn("%s", *argv);
errors = 1;
}
}

return (errors);
}

static int
signame_to_signum(const char *sig)
{
int n;

if (strncasecmp(sig, "SIG", 3) == 0)
sig += 3;
for (n = 1; n < sys_nsig; n++) {
if (!strcasecmp(sys_signame[n], sig))
return (n);
}
return (-1);
}

static void
nosig(const char *name)
{

warnx("unknown signal %s; valid signals:", name);
printsignals(stderr);
#if defined(SHELL)
error(NULL);
#else
exit(2);
#endif
}

static void
printsignals(FILE *fp)
{
int n;

for (n = 1; n < sys_nsig; n++) {
(void)fprintf(fp, "%s", sys_signame[n]);
if (n == (sys_nsig / 2) || n == (sys_nsig - 1))
(void)fprintf(fp, "\n");
else
(void)fprintf(fp, " ");
}
}

static void
usage(void)
{

(void)fprintf(stderr, "%s\n%s\n%s\n%s\n",
"usage: kill [-s signal_name] pid ...",
" kill -l [exit_status]",
" kill -signal_name pid ...",
" kill -signal_number pid ...");
#if defined(SHELL)
error(NULL);
#else
exit(2);
#endif
}
