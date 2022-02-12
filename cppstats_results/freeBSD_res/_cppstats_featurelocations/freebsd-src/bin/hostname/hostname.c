






























#if 0
#if !defined(lint)
static char const copyright[] =
"@(#) Copyright (c) 1988, 1993\n\
The Regents of the University of California. All rights reserved.\n";
#endif

#if !defined(lint)
static char sccsid[] = "@(#)hostname.c 8.1 (Berkeley) 5/31/93";
#endif
#endif
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/param.h>

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void usage(void) __dead2;

int
main(int argc, char *argv[])
{
int ch, sflag, dflag;
char hostname[MAXHOSTNAMELEN], *hostp, *p;

sflag = 0;
dflag = 0;
while ((ch = getopt(argc, argv, "fsd")) != -1)
switch (ch) {
case 'f':





break;
case 's':
sflag = 1;
break;
case 'd':
dflag = 1;
break;
case '?':
default:
usage();
}
argc -= optind;
argv += optind;

if (argc > 1 || (sflag && dflag))
usage();

if (*argv) {
if (sethostname(*argv, (int)strlen(*argv)))
err(1, "sethostname");
} else {
hostp = hostname;
if (gethostname(hostname, (int)sizeof(hostname)))
err(1, "gethostname");
if (sflag) {
p = strchr(hostname, '.');
if (p != NULL)
*p = '\0';
} else if (dflag) {
p = strchr(hostname, '.');
if (p != NULL)
hostp = p + 1;
}
(void)printf("%s\n", hostp);
}
exit(0);
}

static void
usage(void)
{

(void)fprintf(stderr, "usage: hostname [-f] [-s | -d] [name-of-host]\n");
exit(1);
}
