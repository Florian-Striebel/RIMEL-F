
































#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)getopt.c 8.3 (Berkeley) 4/27/95";
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "getopt.h"

int opterr = 1,
optind = 1,
optopt,
optreset;
char *optarg;

#define BADCH (int)'?'
#define BADARG (int)':'
#define EMSG ""





int
getopt(int nargc, char * const *nargv, const char *ostr)
{
char *cp;
static char *__progname;
static char *place = EMSG;
char *oli;

if (__progname == NULL) {
if ((cp = strrchr(nargv[0], '/')) != NULL)
__progname = cp + 1;
else
__progname = nargv[0];
}
if (optreset || !*place) {
optreset = 0;
if (optind >= nargc || *(place = nargv[optind]) != '-') {
place = EMSG;
return (-1);
}
if (place[1] && *++place == '-') {
++optind;
place = EMSG;
return (-1);
}
}
if ((optopt = (int)*place++) == (int)':' ||
!(oli = strchr(ostr, optopt))) {




if (optopt == (int)'-')
return (-1);
if (!*place)
++optind;
if (opterr && *ostr != ':')
(void)fprintf(stderr,
"%s: illegal option -- %c\n", __progname, optopt);
return (BADCH);
}
if (*++oli != ':') {
optarg = NULL;
if (!*place)
++optind;
}
else {
if (*place)
optarg = place;
else if (nargc <= ++optind) {
place = EMSG;
if (*ostr == ':')
return (BADARG);
if (opterr)
(void)fprintf(stderr,
"%s: option requires an argument -- %c\n",
__progname, optopt);
return (BADCH);
}
else
optarg = nargv[optind];
place = EMSG;
++optind;
}
return (optopt);
}
