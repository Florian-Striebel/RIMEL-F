


























#include "cpio_platform.h"
__FBSDID("$FreeBSD$");

#if defined(HAVE_ERRNO_H)
#include <errno.h>
#endif
#if defined(HAVE_GRP_H)
#include <grp.h>
#endif
#if defined(HAVE_PWD_H)
#include <pwd.h>
#endif
#include <stdio.h>
#if defined(HAVE_STDLIB_H)
#include <stdlib.h>
#endif
#if defined(HAVE_STRING_H)
#include <string.h>
#endif

#include "cpio.h"
#include "err.h"




static const char *short_options = "067AaBC:cdE:F:f:H:hI:iJjLlmnO:opR:rtuVvW:yZz";




static const struct option {
const char *name;
int required;
int equivalent;
} cpio_longopts[] = {
{ "b64encode", 0, OPTION_B64ENCODE },
{ "binary", 0, '7' },
{ "create", 0, 'o' },
{ "dereference", 0, 'L' },
{ "dot", 0, 'V' },
{ "extract", 0, 'i' },
{ "file", 1, 'F' },
{ "format", 1, 'H' },
{ "grzip", 0, OPTION_GRZIP },
{ "help", 0, 'h' },
{ "insecure", 0, OPTION_INSECURE },
{ "link", 0, 'l' },
{ "list", 0, 't' },
{ "lrzip", 0, OPTION_LRZIP },
{ "lz4", 0, OPTION_LZ4 },
{ "lzma", 0, OPTION_LZMA },
{ "lzop", 0, OPTION_LZOP },
{ "make-directories", 0, 'd' },
{ "no-preserve-owner", 0, OPTION_NO_PRESERVE_OWNER },
{ "null", 0, '0' },
{ "numeric-uid-gid", 0, 'n' },
{ "owner", 1, 'R' },
{ "passphrase", 1, OPTION_PASSPHRASE },
{ "pass-through", 0, 'p' },
{ "preserve-modification-time", 0, 'm' },
{ "preserve-owner", 0, OPTION_PRESERVE_OWNER },
{ "pwb", 0, '6' },
{ "quiet", 0, OPTION_QUIET },
{ "unconditional", 0, 'u' },
{ "uuencode", 0, OPTION_UUENCODE },
{ "verbose", 0, 'v' },
{ "version", 0, OPTION_VERSION },
{ "xz", 0, 'J' },
{ "zstd", 0, OPTION_ZSTD },
{ NULL, 0, 0 }
};











int
cpio_getopt(struct cpio *cpio)
{
enum { state_start = 0, state_next_word, state_short, state_long };
static int state = state_start;
static char *opt_word;

const struct option *popt, *match = NULL, *match2 = NULL;
const char *p, *long_prefix = "--";
size_t optlength;
int opt = '?';
int required = 0;

cpio->argument = NULL;


if (state == state_start) {

++cpio->argv;
--cpio->argc;
state = state_next_word;
}




if (state == state_next_word) {

if (cpio->argv[0] == NULL)
return (-1);

if (cpio->argv[0][0] != '-')
return (-1);

if (strcmp(cpio->argv[0], "--") == 0) {
++cpio->argv;
--cpio->argc;
return (-1);
}

opt_word = *cpio->argv++;
--cpio->argc;
if (opt_word[1] == '-') {

state = state_long;
opt_word += 2;
} else {

state = state_short;
++opt_word;
}
}




if (state == state_short) {

opt = *opt_word++;
if (opt == '\0') {

state = state_next_word;
return cpio_getopt(cpio);
}


p = strchr(short_options, opt);
if (p == NULL)
return ('?');
if (p[1] == ':')
required = 1;


if (required) {

if (opt_word[0] == '\0') {

opt_word = *cpio->argv;
if (opt_word == NULL) {
lafe_warnc(0,
"Option -%c requires an argument",
opt);
return ('?');
}
++cpio->argv;
--cpio->argc;
}
if (opt == 'W') {
state = state_long;
long_prefix = "-W ";
} else {
state = state_next_word;
cpio->argument = opt_word;
}
}
}


if (state == state_long) {

state = state_next_word;


p = strchr(opt_word, '=');
if (p != NULL) {
optlength = (size_t)(p - opt_word);
cpio->argument = (char *)(uintptr_t)(p + 1);
} else {
optlength = strlen(opt_word);
}


for (popt = cpio_longopts; popt->name != NULL; popt++) {

if (popt->name[0] != opt_word[0])
continue;

if (strncmp(opt_word, popt->name, optlength) == 0) {
match2 = match;
match = popt;

if (strlen(popt->name) == optlength) {
match2 = NULL;
break;
}
}
}


if (match == NULL) {
lafe_warnc(0,
"Option %s%s is not supported",
long_prefix, opt_word);
return ('?');
}
if (match2 != NULL) {
lafe_warnc(0,
"Ambiguous option %s%s (matches --%s and --%s)",
long_prefix, opt_word, match->name, match2->name);
return ('?');
}


if (match->required) {

if (cpio->argument == NULL) {
cpio->argument = *cpio->argv;
if (cpio->argument == NULL) {
lafe_warnc(0,
"Option %s%s requires an argument",
long_prefix, match->name);
return ('?');
}
++cpio->argv;
--cpio->argc;
}
} else {

if (cpio->argument != NULL) {
lafe_warnc(0,
"Option %s%s does not allow an argument",
long_prefix, match->name);
return ('?');
}
}
return (match->equivalent);
}

return (opt);
}

























const char *
owner_parse(const char *spec, int *uid, int *gid)
{
static char errbuff[128];
const char *u, *ue, *g;

*uid = -1;
*gid = -1;

if (spec[0] == '\0')
return ("Invalid empty user/group spec");







if (*spec == ':' || *spec == '.') {

ue = u = NULL;
g = spec + 1;
} else {

ue = u = spec;
while (*ue != ':' && *ue != '.' && *ue != '\0')
++ue;
g = ue;
if (*g != '\0')
++g;
}

if (u != NULL) {

char *user;
struct passwd *pwent;

user = (char *)malloc(ue - u + 1);
if (user == NULL)
return ("Couldn't allocate memory");
memcpy(user, u, ue - u);
user[ue - u] = '\0';
if ((pwent = getpwnam(user)) != NULL) {
*uid = pwent->pw_uid;
if (*ue != '\0')
*gid = pwent->pw_gid;
} else {
char *end;
errno = 0;
*uid = (int)strtoul(user, &end, 10);
if (errno || *end != '\0') {
snprintf(errbuff, sizeof(errbuff),
"Couldn't lookup user ``%s''", user);
errbuff[sizeof(errbuff) - 1] = '\0';
free(user);
return (errbuff);
}
}
free(user);
}

if (*g != '\0') {
struct group *grp;
if ((grp = getgrnam(g)) != NULL) {
*gid = grp->gr_gid;
} else {
char *end;
errno = 0;
*gid = (int)strtoul(g, &end, 10);
if (errno || *end != '\0') {
snprintf(errbuff, sizeof(errbuff),
"Couldn't lookup group ``%s''", g);
errbuff[sizeof(errbuff) - 1] = '\0';
return (errbuff);
}
}
}
return (NULL);
}
