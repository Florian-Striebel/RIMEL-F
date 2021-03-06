

























#include <sys/param.h>

#include <capsicum_helpers.h>
#include <ctype.h>
#include <err.h>
#include <getopt.h>
#include <libelftc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "_elftc.h"

ELFTC_VCSID("$Id: cxxfilt.c 3499 2016-11-25 16:06:29Z emaste $");

#define STRBUFSZ 8192

static int stripus = 0;
static int noparam = 0;
static int format = 0;

enum options
{
OPTION_HELP,
OPTION_VERSION
};

static struct option longopts[] =
{
{"format", required_argument, NULL, 's'},
{"help", no_argument, NULL, OPTION_HELP},
{"no-params", no_argument, NULL, 'p'},
{"no-strip-underscores", no_argument, NULL, 'n'},
{"strip-underscores", no_argument, NULL, '_'},
{"version", no_argument, NULL, 'V'},
{NULL, 0, NULL, 0}
};

static struct {
const char *fname;
int fvalue;
} flist[] = {
{"auto", 0},
{"arm", ELFTC_DEM_ARM},
{"gnu", ELFTC_DEM_GNU2},
{"gnu-v3", ELFTC_DEM_GNU3}
};

#define USAGE_MESSAGE "Usage: %s [options] [encoded-names...]\nTranslate C++ symbol names to human-readable form.\n\nOptions:\n-_ | --strip-underscores Remove leading underscores prior to decoding.\n-n | --no-strip-underscores Do not remove leading underscores.\n-p | --no-params (Accepted but ignored).\n-s SCHEME | --format=SCHEME Select the encoding scheme to use.\nValid schemes are: 'arm', 'auto', 'gnu' and\n'gnu-v3'.\n--help Print a help message.\n--version Print a version identifier and exit.\n"












static void
usage(void)
{

(void) fprintf(stderr, USAGE_MESSAGE, ELFTC_GETPROGNAME());
exit(1);
}

static void
version(void)
{
fprintf(stderr, "%s (%s)\n", ELFTC_GETPROGNAME(), elftc_version());
exit(0);
}

static int
find_format(const char *fstr)
{
int i;

for (i = 0; (size_t) i < sizeof(flist) / sizeof(flist[0]); i++) {
if (!strcmp(fstr, flist[i].fname))
return (flist[i].fvalue);
}

return (-1);
}

static char *
demangle(char *name)
{
static char dem[STRBUFSZ];

if (stripus && *name == '_')
name++;

if (strlen(name) == 0)
return (NULL);

if (elftc_demangle(name, dem, sizeof(dem), (unsigned) format) < 0)
return (NULL);

return (dem);
}

int
main(int argc, char **argv)
{
char *dem, buf[STRBUFSZ];
size_t p;
int c, n, opt;

while ((opt = getopt_long(argc, argv, "_nps:V", longopts, NULL)) !=
-1) {
switch (opt) {
case '_':
stripus = 1;
break;
case 'n':
stripus = 0;
break;
case 'p':
noparam = 1;
break;
case 's':
if ((format = find_format(optarg)) < 0)
errx(EXIT_FAILURE, "unsupported format: %s",
optarg);
break;
case 'V':
version();

case OPTION_HELP:
default:
usage();

}
}

argv += optind;
argc -= optind;

if (caph_limit_stdio() < 0)
err(EXIT_FAILURE, "failed to limit stdio rights");
if (caph_enter() < 0)
err(EXIT_FAILURE, "failed to enter capability mode");

if (*argv != NULL) {
for (n = 0; n < argc; n++) {
if ((dem = demangle(argv[n])) == NULL)
printf("%s\n", argv[n]);
else
printf("%s\n", dem);
}
} else {
p = 0;
for (;;) {
setvbuf(stdout, NULL, _IOLBF, 0);
c = fgetc(stdin);
if (c == EOF || !(isalnum(c) || strchr(".$_", c))) {
if (p > 0) {
buf[p] = '\0';
if ((dem = demangle(buf)) == NULL)
printf("%s", buf);
else
printf("%s", dem);
p = 0;
}
if (c == EOF)
break;
putchar(c);
} else {
if ((size_t) p >= sizeof(buf) - 1)
warnx("buffer overflowed");
else
buf[p++] = (char) c;
}

}
}

exit(0);
}
