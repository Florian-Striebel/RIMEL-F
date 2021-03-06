




























#include "bsdcat_platform.h"
__FBSDID("$FreeBSD$");

#if defined(HAVE_ERRNO_H)
#include <errno.h>
#endif
#if defined(HAVE_STDLIB_H)
#include <stdlib.h>
#endif
#if defined(HAVE_STRING_H)
#include <string.h>
#endif

#include "bsdcat.h"
#include "err.h"




static const char *short_options = "h";










static const struct bsdcat_option {
const char *name;
int required;
int equivalent;
} tar_longopts[] = {
{ "help", 0, 'h' },
{ "version", 0, OPTION_VERSION },
{ NULL, 0, 0 }
};











































int
bsdcat_getopt(struct bsdcat *bsdcat)
{
enum { state_start = 0, state_old_tar, state_next_word,
state_short, state_long };

const struct bsdcat_option *popt, *match = NULL, *match2 = NULL;
const char *p, *long_prefix = "--";
size_t optlength;
int opt = '?';
int required = 0;

bsdcat->argument = NULL;


if (bsdcat->getopt_state == state_start) {

++bsdcat->argv;
--bsdcat->argc;
if (*bsdcat->argv == NULL)
return (-1);

bsdcat->getopt_state = state_next_word;
}




if (bsdcat->getopt_state == state_next_word) {

if (bsdcat->argv[0] == NULL)
return (-1);

if (bsdcat->argv[0][0] != '-')
return (-1);

if (strcmp(bsdcat->argv[0], "--") == 0) {
++bsdcat->argv;
--bsdcat->argc;
return (-1);
}

bsdcat->getopt_word = *bsdcat->argv++;
--bsdcat->argc;
if (bsdcat->getopt_word[1] == '-') {

bsdcat->getopt_state = state_long;
bsdcat->getopt_word += 2;
} else {

bsdcat->getopt_state = state_short;
++bsdcat->getopt_word;
}
}




if (bsdcat->getopt_state == state_short) {

opt = *bsdcat->getopt_word++;
if (opt == '\0') {

bsdcat->getopt_state = state_next_word;
return bsdcat_getopt(bsdcat);
}


p = strchr(short_options, opt);
if (p == NULL)
return ('?');
if (p[1] == ':')
required = 1;


if (required) {

if (bsdcat->getopt_word[0] == '\0') {

bsdcat->getopt_word = *bsdcat->argv;
if (bsdcat->getopt_word == NULL) {
lafe_warnc(0,
"Option -%c requires an argument",
opt);
return ('?');
}
++bsdcat->argv;
--bsdcat->argc;
}
if (opt == 'W') {
bsdcat->getopt_state = state_long;
long_prefix = "-W ";
} else {
bsdcat->getopt_state = state_next_word;
bsdcat->argument = bsdcat->getopt_word;
}
}
}


if (bsdcat->getopt_state == state_long) {

bsdcat->getopt_state = state_next_word;


p = strchr(bsdcat->getopt_word, '=');
if (p != NULL) {
optlength = (size_t)(p - bsdcat->getopt_word);
bsdcat->argument = (char *)(uintptr_t)(p + 1);
} else {
optlength = strlen(bsdcat->getopt_word);
}


for (popt = tar_longopts; popt->name != NULL; popt++) {

if (popt->name[0] != bsdcat->getopt_word[0])
continue;

if (strncmp(bsdcat->getopt_word, popt->name, optlength) == 0) {
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
long_prefix, bsdcat->getopt_word);
return ('?');
}
if (match2 != NULL) {
lafe_warnc(0,
"Ambiguous option %s%s (matches --%s and --%s)",
long_prefix, bsdcat->getopt_word, match->name, match2->name);
return ('?');
}


if (match->required) {

if (bsdcat->argument == NULL) {
bsdcat->argument = *bsdcat->argv;
if (bsdcat->argument == NULL) {
lafe_warnc(0,
"Option %s%s requires an argument",
long_prefix, match->name);
return ('?');
}
++bsdcat->argv;
--bsdcat->argc;
}
} else {

if (bsdcat->argument != NULL) {
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
