











#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int getoldopt(int, char **, const char *);

int
getoldopt(int argc, char **argv, const char *optstring)
{
static char *key;
static char use_getopt;
char c;
char *place;

optarg = NULL;

if (key == NULL) {
if (argc < 2)
return (-1);
key = argv[1];
if (*key == '-')
use_getopt++;
else
optind = 2;
}

if (use_getopt)
return (getopt(argc, argv, optstring));

c = *key++;
if (c == '\0') {
key--;
return (-1);
}
place = strchr(optstring, c);

if (place == NULL || c == ':') {
fprintf(stderr, "%s: unknown option %c\n", argv[0], c);
return ('?');
}

place++;
if (*place == ':') {
if (optind < argc) {
optarg = argv[optind];
optind++;
} else {
fprintf(stderr, "%s: %c argument missing\n",
argv[0], c);
return ('?');
}
}

return (c);
}
