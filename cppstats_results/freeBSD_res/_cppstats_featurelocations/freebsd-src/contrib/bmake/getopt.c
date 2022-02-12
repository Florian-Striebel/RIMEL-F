






























#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif
#if !defined(HAVE_GETOPT) || defined(WANT_GETOPT_LONG) || defined(BROKEN_GETOPT)
#include <sys/cdefs.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define BADCH (int)'?'
#define BADARG (int)':'
#define EMSG ""

int opterr = 1,
optind = 1,
optopt = BADCH,
optreset;
char *optarg;





int
getopt(int nargc, char * const nargv[], const char *ostr)
{
extern char *__progname;
static const char *place = EMSG;
char *oli;

#if !defined(BSD4_4)
if (!__progname) {
if (__progname = strrchr(nargv[0], '/'))
++__progname;
else
__progname = nargv[0];
}
#endif

if (optreset || *place == 0) {
optreset = 0;
place = nargv[optind];
if (optind >= nargc || *place++ != '-') {

place = EMSG;
return (-1);
}
optopt = *place++;
if (optopt == '-' && *place == 0) {

++optind;
place = EMSG;
return (-1);
}
if (optopt == 0) {


place = EMSG;
if (strchr(ostr, '-') == NULL)
return -1;
optopt = '-';
}
} else
optopt = *place++;


if (optopt == ':' || (oli = strchr(ostr, optopt)) == NULL) {
if (*place == 0)
++optind;
if (opterr && *ostr != ':')
(void)fprintf(stderr,
"%s: unknown option -- %c\n", __progname, optopt);
return (BADCH);
}


if (oli[1] != ':') {

optarg = NULL;
if (*place == 0)
++optind;
} else {


if (*place)
optarg = __UNCONST(place);
else if (oli[2] == ':')




optarg = NULL;
else if (nargc > ++optind)
optarg = nargv[optind];
else {

place = EMSG;
if (*ostr == ':')
return (BADARG);
if (opterr)
(void)fprintf(stderr,
"%s: option requires an argument -- %c\n",
__progname, optopt);
return (BADCH);
}
place = EMSG;
++optind;
}
return (optopt);
}
#endif
#if defined(MAIN)
#if !defined(BSD4_4)
char *__progname;
#endif

int
main(argc, argv)
int argc;
char *argv[];
{
int c;
char *opts = argv[1];

--argc;
++argv;

while ((c = getopt(argc, argv, opts)) != EOF) {
switch (c) {
case '-':
if (optarg)
printf("--%s ", optarg);
break;
case '?':
exit(1);
break;
default:
if (optarg)
printf("-%c %s ", c, optarg);
else
printf("-%c ", c);
break;
}
}

if (optind < argc) {
printf("-- ");
for (; optind < argc; ++optind) {
printf("%s ", argv[optind]);
}
}
printf("\n");
exit(0);
}
#endif
