




























#include "bsdtar_platform.h"
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

#include "bsdtar.h"
#include "err.h"




static const char *short_options
= "aBb:C:cf:HhI:JjkLlmnOoPpqrSs:T:tUuvW:wX:xyZz";










static const struct bsdtar_option {
const char *name;
int required;
int equivalent;
} tar_longopts[] = {
{ "absolute-paths", 0, 'P' },
{ "append", 0, 'r' },
{ "acls", 0, OPTION_ACLS },
{ "auto-compress", 0, 'a' },
{ "b64encode", 0, OPTION_B64ENCODE },
{ "block-size", 1, 'b' },
{ "blocking-factor", 1, 'b' },
{ "bunzip2", 0, 'j' },
{ "bzip", 0, 'j' },
{ "bzip2", 0, 'j' },
{ "cd", 1, 'C' },
{ "check-links", 0, OPTION_CHECK_LINKS },
{ "chroot", 0, OPTION_CHROOT },
{ "clear-nochange-fflags", 0, OPTION_CLEAR_NOCHANGE_FFLAGS },
{ "compress", 0, 'Z' },
{ "confirmation", 0, 'w' },
{ "create", 0, 'c' },
{ "dereference", 0, 'L' },
{ "directory", 1, 'C' },
{ "disable-copyfile", 0, OPTION_NO_MAC_METADATA },
{ "exclude", 1, OPTION_EXCLUDE },
{ "exclude-from", 1, 'X' },
{ "exclude-vcs", 0, OPTION_EXCLUDE_VCS },
{ "extract", 0, 'x' },
{ "fast-read", 0, 'q' },
{ "fflags", 0, OPTION_FFLAGS },
{ "file", 1, 'f' },
{ "files-from", 1, 'T' },
{ "format", 1, OPTION_FORMAT },
{ "gid", 1, OPTION_GID },
{ "gname", 1, OPTION_GNAME },
{ "grzip", 0, OPTION_GRZIP },
{ "gunzip", 0, 'z' },
{ "gzip", 0, 'z' },
{ "help", 0, OPTION_HELP },
{ "hfsCompression", 0, OPTION_HFS_COMPRESSION },
{ "ignore-zeros", 0, OPTION_IGNORE_ZEROS },
{ "include", 1, OPTION_INCLUDE },
{ "insecure", 0, 'P' },
{ "interactive", 0, 'w' },
{ "keep-newer-files", 0, OPTION_KEEP_NEWER_FILES },
{ "keep-old-files", 0, 'k' },
{ "list", 0, 't' },
{ "lrzip", 0, OPTION_LRZIP },
{ "lz4", 0, OPTION_LZ4 },
{ "lzip", 0, OPTION_LZIP },
{ "lzma", 0, OPTION_LZMA },
{ "lzop", 0, OPTION_LZOP },
{ "mac-metadata", 0, OPTION_MAC_METADATA },
{ "modification-time", 0, 'm' },
{ "newer", 1, OPTION_NEWER_CTIME },
{ "newer-ctime", 1, OPTION_NEWER_CTIME },
{ "newer-ctime-than", 1, OPTION_NEWER_CTIME_THAN },
{ "newer-mtime", 1, OPTION_NEWER_MTIME },
{ "newer-mtime-than", 1, OPTION_NEWER_MTIME_THAN },
{ "newer-than", 1, OPTION_NEWER_CTIME_THAN },
{ "no-acls", 0, OPTION_NO_ACLS },
{ "no-fflags", 0, OPTION_NO_FFLAGS },
{ "no-mac-metadata", 0, OPTION_NO_MAC_METADATA },
{ "no-recursion", 0, 'n' },
{ "no-safe-writes", 0, OPTION_NO_SAFE_WRITES },
{ "no-same-owner", 0, OPTION_NO_SAME_OWNER },
{ "no-same-permissions", 0, OPTION_NO_SAME_PERMISSIONS },
{ "no-xattr", 0, OPTION_NO_XATTRS },
{ "no-xattrs", 0, OPTION_NO_XATTRS },
{ "nodump", 0, OPTION_NODUMP },
{ "nopreserveHFSCompression",0, OPTION_NOPRESERVE_HFS_COMPRESSION },
{ "norecurse", 0, 'n' },
{ "null", 0, OPTION_NULL },
{ "numeric-owner", 0, OPTION_NUMERIC_OWNER },
{ "older", 1, OPTION_OLDER_CTIME },
{ "older-ctime", 1, OPTION_OLDER_CTIME },
{ "older-ctime-than", 1, OPTION_OLDER_CTIME_THAN },
{ "older-mtime", 1, OPTION_OLDER_MTIME },
{ "older-mtime-than", 1, OPTION_OLDER_MTIME_THAN },
{ "older-than", 1, OPTION_OLDER_CTIME_THAN },
{ "one-file-system", 0, OPTION_ONE_FILE_SYSTEM },
{ "options", 1, OPTION_OPTIONS },
{ "passphrase", 1, OPTION_PASSPHRASE },
{ "posix", 0, OPTION_POSIX },
{ "preserve-permissions", 0, 'p' },
{ "read-full-blocks", 0, 'B' },
{ "safe-writes", 0, OPTION_SAFE_WRITES },
{ "same-owner", 0, OPTION_SAME_OWNER },
{ "same-permissions", 0, 'p' },
{ "strip-components", 1, OPTION_STRIP_COMPONENTS },
{ "to-stdout", 0, 'O' },
{ "totals", 0, OPTION_TOTALS },
{ "uid", 1, OPTION_UID },
{ "uname", 1, OPTION_UNAME },
{ "uncompress", 0, 'Z' },
{ "unlink", 0, 'U' },
{ "unlink-first", 0, 'U' },
{ "update", 0, 'u' },
{ "use-compress-program", 1, OPTION_USE_COMPRESS_PROGRAM },
{ "uuencode", 0, OPTION_UUENCODE },
{ "verbose", 0, 'v' },
{ "version", 0, OPTION_VERSION },
{ "xattrs", 0, OPTION_XATTRS },
{ "xz", 0, 'J' },
{ "zstd", 0, OPTION_ZSTD },
{ NULL, 0, 0 }
};











































int
bsdtar_getopt(struct bsdtar *bsdtar)
{
enum { state_start = 0, state_old_tar, state_next_word,
state_short, state_long };

const struct bsdtar_option *popt, *match = NULL, *match2 = NULL;
const char *p, *long_prefix = "--";
size_t optlength;
int opt = '?';
int required = 0;

bsdtar->argument = NULL;


if (bsdtar->getopt_state == state_start) {

++bsdtar->argv;
--bsdtar->argc;
if (*bsdtar->argv == NULL)
return (-1);

if (bsdtar->argv[0][0] == '-') {
bsdtar->getopt_state = state_next_word;
} else {
bsdtar->getopt_state = state_old_tar;
bsdtar->getopt_word = *bsdtar->argv++;
--bsdtar->argc;
}
}




if (bsdtar->getopt_state == state_old_tar) {

opt = *bsdtar->getopt_word++;
if (opt == '\0') {

bsdtar->getopt_state = state_next_word;
} else {

p = strchr(short_options, opt);
if (p == NULL)
return ('?');
if (p[1] == ':') {
bsdtar->argument = *bsdtar->argv;
if (bsdtar->argument == NULL) {
lafe_warnc(0,
"Option %c requires an argument",
opt);
return ('?');
}
++bsdtar->argv;
--bsdtar->argc;
}
}
}




if (bsdtar->getopt_state == state_next_word) {

if (bsdtar->argv[0] == NULL)
return (-1);

if (bsdtar->argv[0][0] != '-')
return (-1);

if (strcmp(bsdtar->argv[0], "--") == 0) {
++bsdtar->argv;
--bsdtar->argc;
return (-1);
}

bsdtar->getopt_word = *bsdtar->argv++;
--bsdtar->argc;
if (bsdtar->getopt_word[1] == '-') {

bsdtar->getopt_state = state_long;
bsdtar->getopt_word += 2;
} else {

bsdtar->getopt_state = state_short;
++bsdtar->getopt_word;
}
}




if (bsdtar->getopt_state == state_short) {

opt = *bsdtar->getopt_word++;
if (opt == '\0') {

bsdtar->getopt_state = state_next_word;
return bsdtar_getopt(bsdtar);
}


p = strchr(short_options, opt);
if (p == NULL)
return ('?');
if (p[1] == ':')
required = 1;


if (required) {

if (bsdtar->getopt_word[0] == '\0') {

bsdtar->getopt_word = *bsdtar->argv;
if (bsdtar->getopt_word == NULL) {
lafe_warnc(0,
"Option -%c requires an argument",
opt);
return ('?');
}
++bsdtar->argv;
--bsdtar->argc;
}
if (opt == 'W') {
bsdtar->getopt_state = state_long;
long_prefix = "-W ";
} else {
bsdtar->getopt_state = state_next_word;
bsdtar->argument = bsdtar->getopt_word;
}
}
}


if (bsdtar->getopt_state == state_long) {

bsdtar->getopt_state = state_next_word;


p = strchr(bsdtar->getopt_word, '=');
if (p != NULL) {
optlength = (size_t)(p - bsdtar->getopt_word);
bsdtar->argument = (char *)(uintptr_t)(p + 1);
} else {
optlength = strlen(bsdtar->getopt_word);
}


for (popt = tar_longopts; popt->name != NULL; popt++) {

if (popt->name[0] != bsdtar->getopt_word[0])
continue;

if (strncmp(bsdtar->getopt_word, popt->name, optlength) == 0) {
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
long_prefix, bsdtar->getopt_word);
return ('?');
}
if (match2 != NULL) {
lafe_warnc(0,
"Ambiguous option %s%s (matches --%s and --%s)",
long_prefix, bsdtar->getopt_word, match->name, match2->name);
return ('?');
}


if (match->required) {

if (bsdtar->argument == NULL) {
bsdtar->argument = *bsdtar->argv;
if (bsdtar->argument == NULL) {
lafe_warnc(0,
"Option %s%s requires an argument",
long_prefix, match->name);
return ('?');
}
++bsdtar->argv;
--bsdtar->argc;
}
} else {

if (bsdtar->argument != NULL) {
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
