
























#include "bsdtar_platform.h"
__FBSDID("$FreeBSD$");

#if defined(HAVE_SYS_PARAM_H)
#include <sys/param.h>
#endif
#if defined(HAVE_SYS_STAT_H)
#include <sys/stat.h>
#endif
#if defined(HAVE_COPYFILE_H)
#include <copyfile.h>
#endif
#if defined(HAVE_ERRNO_H)
#include <errno.h>
#endif
#if defined(HAVE_FCNTL_H)
#include <fcntl.h>
#endif
#if defined(HAVE_LANGINFO_H)
#include <langinfo.h>
#endif
#if defined(HAVE_LOCALE_H)
#include <locale.h>
#endif
#if defined(HAVE_PATHS_H)
#include <paths.h>
#endif
#if defined(HAVE_SIGNAL_H)
#include <signal.h>
#endif
#include <stdio.h>
#if defined(HAVE_STDLIB_H)
#include <stdlib.h>
#endif
#if defined(HAVE_STRING_H)
#include <string.h>
#endif
#if defined(HAVE_TIME_H)
#include <time.h>
#endif
#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif

#include "bsdtar.h"
#include "err.h"





#if defined(__linux)
#define _PATH_DEFTAPE "/dev/st0"
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
#define _PATH_DEFTAPE "\\\\.\\tape0"
#endif
#if defined(__APPLE__)
#undef _PATH_DEFTAPE
#define _PATH_DEFTAPE "-"
#endif

#if !defined(_PATH_DEFTAPE)
#define _PATH_DEFTAPE "/dev/tape"
#endif

#if defined(__MINGW32__)
int _CRT_glob = 0;
#endif

#if defined(HAVE_SIGACTION) && (defined(SIGINFO) || defined(SIGUSR1))
static volatile int siginfo_occurred;

static void
siginfo_handler(int sig)
{
(void)sig;
siginfo_occurred = 1;
}

int
need_report(void)
{
int r = siginfo_occurred;
siginfo_occurred = 0;
return (r);
}
#else
int
need_report(void)
{
return (0);
}
#endif

static void long_help(void) __LA_DEAD;
static void only_mode(struct bsdtar *, const char *opt,
const char *valid);
static void set_mode(struct bsdtar *, char opt);
static void version(void) __LA_DEAD;


#define SECURITY (ARCHIVE_EXTRACT_SECURE_SYMLINKS | ARCHIVE_EXTRACT_SECURE_NODOTDOT)



static char const * const vcs_files[] = {

"CVS", ".cvsignore",

"RCS",

"SCCS",

".svn",

".git", ".gitignore", ".gitattributes", ".gitmodules",

".arch-ids", "{arch}", "=RELEASE-ID", "=meta-update", "=update",

".bzr", ".bzrignore", ".bzrtags",

".hg", ".hgignore", ".hgtags",

"_darcs",
NULL
};

int
main(int argc, char **argv)
{
struct bsdtar *bsdtar, bsdtar_storage;
int opt, t;
char compression, compression2;
const char *compression_name, *compression2_name;
const char *compress_program;
char *tptr;
char possible_help_request;
char buff[16];





bsdtar = &bsdtar_storage;
memset(bsdtar, 0, sizeof(*bsdtar));
bsdtar->fd = -1;
bsdtar->gid = -1;
bsdtar->uid = -1;
bsdtar->flags = 0;
compression = compression2 = '\0';
compression_name = compression2_name = NULL;
compress_program = NULL;

#if defined(HAVE_SIGACTION)
{
struct sigaction sa;
sa.sa_handler = siginfo_handler;
sigemptyset(&sa.sa_mask);
sa.sa_flags = 0;
#if defined(SIGINFO)
if (sigaction(SIGINFO, &sa, NULL))
lafe_errc(1, errno, "sigaction(SIGINFO) failed");
#endif
#if defined(SIGUSR1)

if (sigaction(SIGUSR1, &sa, NULL))
lafe_errc(1, errno, "sigaction(SIGUSR1) failed");
#endif
#if defined(SIGPIPE)

sa.sa_handler = SIG_IGN;
sigaction(SIGPIPE, &sa, NULL);
#endif
}
#endif


lafe_setprogname(*argv, "bsdtar");

#if HAVE_SETLOCALE
if (setlocale(LC_ALL, "") == NULL)
lafe_warnc(0, "Failed to set default locale");
#endif
#if defined(HAVE_NL_LANGINFO) && defined(HAVE_D_MD_ORDER)
bsdtar->day_first = (*nl_langinfo(D_MD_ORDER) == 'd');
#endif
possible_help_request = 0;


bsdtar->user_uid = geteuid();


bsdtar->filename = getenv("TAPE");
if (bsdtar->filename == NULL)
bsdtar->filename = _PATH_DEFTAPE;


bsdtar->bytes_per_block = DEFAULT_BYTES_PER_BLOCK;

bsdtar->bytes_in_last_block = -1;


bsdtar->extract_flags = ARCHIVE_EXTRACT_TIME;


bsdtar->extract_flags |= SECURITY;

#if !defined(_WIN32)


if (bsdtar->user_uid == 0) {

bsdtar->extract_flags |= ARCHIVE_EXTRACT_OWNER;

bsdtar->extract_flags |= ARCHIVE_EXTRACT_PERM;
bsdtar->extract_flags |= ARCHIVE_EXTRACT_ACL;
bsdtar->extract_flags |= ARCHIVE_EXTRACT_XATTR;
bsdtar->extract_flags |= ARCHIVE_EXTRACT_FFLAGS;
bsdtar->extract_flags |= ARCHIVE_EXTRACT_MAC_METADATA;
}
#endif





bsdtar->readdisk_flags |= ARCHIVE_READDISK_MAC_COPYFILE;
#if defined(COPYFILE_DISABLE_VAR)
if (getenv(COPYFILE_DISABLE_VAR))
bsdtar->readdisk_flags &= ~ARCHIVE_READDISK_MAC_COPYFILE;
#endif
#if defined(__APPLE__)




bsdtar->readdisk_flags |= ARCHIVE_READDISK_NO_ACL;
#endif

bsdtar->matching = archive_match_new();
if (bsdtar->matching == NULL)
lafe_errc(1, errno, "Out of memory");
bsdtar->cset = cset_new();
if (bsdtar->cset == NULL)
lafe_errc(1, errno, "Out of memory");

bsdtar->argv = argv;
bsdtar->argc = argc;







while ((opt = bsdtar_getopt(bsdtar)) != -1) {
switch (opt) {
case 'a':
bsdtar->flags |= OPTFLAG_AUTO_COMPRESS;
break;
case OPTION_ACLS:
bsdtar->extract_flags |= ARCHIVE_EXTRACT_ACL;
bsdtar->readdisk_flags &= ~ARCHIVE_READDISK_NO_ACL;
bsdtar->flags |= OPTFLAG_ACLS;
break;
case 'B':

break;
case 'b':
errno = 0;
tptr = NULL;
t = (int)strtol(bsdtar->argument, &tptr, 10);
if (errno || t <= 0 || t > 8192 ||
*(bsdtar->argument) == '\0' || tptr == NULL ||
*tptr != '\0') {
lafe_errc(1, 0, "Invalid or out of range "
"(1..8192) argument to -b");
}
bsdtar->bytes_per_block = 512 * t;

bsdtar->bytes_in_last_block = bsdtar->bytes_per_block;
break;
case OPTION_B64ENCODE:
if (compression2 != '\0')
lafe_errc(1, 0,
"Can't specify both --uuencode and "
"--b64encode");
compression2 = opt;
compression2_name = "b64encode";
break;
case 'C':
if (strlen(bsdtar->argument) == 0)
lafe_errc(1, 0,
"Meaningless option: -C ''");

set_chdir(bsdtar, bsdtar->argument);
break;
case 'c':
set_mode(bsdtar, opt);
break;
case OPTION_CHECK_LINKS:
bsdtar->flags |= OPTFLAG_WARN_LINKS;
break;
case OPTION_CHROOT:
bsdtar->flags |= OPTFLAG_CHROOT;
break;
case OPTION_CLEAR_NOCHANGE_FFLAGS:
bsdtar->extract_flags |=
ARCHIVE_EXTRACT_CLEAR_NOCHANGE_FFLAGS;
break;
case OPTION_EXCLUDE:
if (archive_match_exclude_pattern(
bsdtar->matching, bsdtar->argument) != ARCHIVE_OK)
lafe_errc(1, 0,
"Couldn't exclude %s\n", bsdtar->argument);
break;
case OPTION_EXCLUDE_VCS:
for(t=0; vcs_files[t]; t++) {
if (archive_match_exclude_pattern(
bsdtar->matching,
vcs_files[t]) != ARCHIVE_OK)
lafe_errc(1, 0, "Couldn't "
"exclude %s\n", vcs_files[t]);
}
break;
case OPTION_FFLAGS:
bsdtar->extract_flags |= ARCHIVE_EXTRACT_FFLAGS;
bsdtar->readdisk_flags &= ~ARCHIVE_READDISK_NO_FFLAGS;
bsdtar->flags |= OPTFLAG_FFLAGS;
break;
case OPTION_FORMAT:
cset_set_format(bsdtar->cset, bsdtar->argument);
break;
case 'f':
bsdtar->filename = bsdtar->argument;
break;
case OPTION_GID:
errno = 0;
tptr = NULL;
t = (int)strtol(bsdtar->argument, &tptr, 10);
if (errno || t < 0 || *(bsdtar->argument) == '\0' ||
tptr == NULL || *tptr != '\0') {
lafe_errc(1, 0, "Invalid argument to --gid");
}
bsdtar->gid = t;
break;
case OPTION_GNAME:
bsdtar->gname = bsdtar->argument;
break;
case OPTION_GRZIP:
if (compression != '\0')
lafe_errc(1, 0,
"Can't specify both -%c and -%c", opt,
compression);
compression = opt;
compression_name = "grzip";
break;
case 'H':
bsdtar->symlink_mode = 'H';
break;
case 'h':
bsdtar->symlink_mode = 'L';

possible_help_request = 1;
break;
case OPTION_HELP:
long_help();
exit(0);
break;
case OPTION_HFS_COMPRESSION:
bsdtar->extract_flags |=
ARCHIVE_EXTRACT_HFS_COMPRESSION_FORCED;
break;
case OPTION_IGNORE_ZEROS:
bsdtar->flags |= OPTFLAG_IGNORE_ZEROS;
break;
case 'I':










bsdtar->names_from_file = bsdtar->argument;
break;
case OPTION_INCLUDE:





if (archive_match_include_pattern(bsdtar->matching,
bsdtar->argument) != ARCHIVE_OK)
lafe_errc(1, 0,
"Failed to add %s to inclusion list",
bsdtar->argument);
break;
case 'j':
if (compression != '\0')
lafe_errc(1, 0,
"Can't specify both -%c and -%c", opt,
compression);
compression = opt;
compression_name = "bzip2";
break;
case 'J':
if (compression != '\0')
lafe_errc(1, 0,
"Can't specify both -%c and -%c", opt,
compression);
compression = opt;
compression_name = "xz";
break;
case 'k':
bsdtar->extract_flags |= ARCHIVE_EXTRACT_NO_OVERWRITE;
break;
case OPTION_KEEP_NEWER_FILES:
bsdtar->extract_flags |= ARCHIVE_EXTRACT_NO_OVERWRITE_NEWER;
break;
case 'L':
bsdtar->symlink_mode = 'L';
break;
case 'l':

bsdtar->flags |= OPTFLAG_WARN_LINKS;
break;
case OPTION_LRZIP:
case OPTION_LZ4:
case OPTION_LZIP:
case OPTION_LZMA:
case OPTION_LZOP:
case OPTION_ZSTD:
if (compression != '\0')
lafe_errc(1, 0,
"Can't specify both -%c and -%c", opt,
compression);
compression = opt;
switch (opt) {
case OPTION_LRZIP: compression_name = "lrzip"; break;
case OPTION_LZ4: compression_name = "lz4"; break;
case OPTION_LZIP: compression_name = "lzip"; break;
case OPTION_LZMA: compression_name = "lzma"; break;
case OPTION_LZOP: compression_name = "lzop"; break;
case OPTION_ZSTD: compression_name = "zstd"; break;
}
break;
case 'm':
bsdtar->extract_flags &= ~ARCHIVE_EXTRACT_TIME;
break;
case OPTION_MAC_METADATA:
bsdtar->readdisk_flags |= ARCHIVE_READDISK_MAC_COPYFILE;
bsdtar->extract_flags |= ARCHIVE_EXTRACT_MAC_METADATA;
bsdtar->flags |= OPTFLAG_MAC_METADATA;
break;
case 'n':
bsdtar->flags |= OPTFLAG_NO_SUBDIRS;
break;






case OPTION_NEWER_CTIME:
if (archive_match_include_date(bsdtar->matching,
ARCHIVE_MATCH_CTIME | ARCHIVE_MATCH_NEWER,
bsdtar->argument) != ARCHIVE_OK)
lafe_errc(1, 0, "Error : %s",
archive_error_string(bsdtar->matching));
break;
case OPTION_NEWER_CTIME_THAN:
if (archive_match_include_file_time(bsdtar->matching,
ARCHIVE_MATCH_CTIME | ARCHIVE_MATCH_NEWER,
bsdtar->argument) != ARCHIVE_OK)
lafe_errc(1, 0, "Error : %s",
archive_error_string(bsdtar->matching));
break;
case OPTION_NEWER_MTIME:
if (archive_match_include_date(bsdtar->matching,
ARCHIVE_MATCH_MTIME | ARCHIVE_MATCH_NEWER,
bsdtar->argument) != ARCHIVE_OK)
lafe_errc(1, 0, "Error : %s",
archive_error_string(bsdtar->matching));
break;
case OPTION_NEWER_MTIME_THAN:
if (archive_match_include_file_time(bsdtar->matching,
ARCHIVE_MATCH_MTIME | ARCHIVE_MATCH_NEWER,
bsdtar->argument) != ARCHIVE_OK)
lafe_errc(1, 0, "Error : %s",
archive_error_string(bsdtar->matching));
break;
case OPTION_NODUMP:
bsdtar->readdisk_flags |= ARCHIVE_READDISK_HONOR_NODUMP;
break;
case OPTION_NOPRESERVE_HFS_COMPRESSION:

bsdtar->extract_flags |=
ARCHIVE_EXTRACT_NO_HFS_COMPRESSION;
break;
case OPTION_NO_ACLS:
bsdtar->extract_flags &= ~ARCHIVE_EXTRACT_ACL;
bsdtar->readdisk_flags |= ARCHIVE_READDISK_NO_ACL;
bsdtar->flags |= OPTFLAG_NO_ACLS;
break;
case OPTION_NO_FFLAGS:
bsdtar->extract_flags &= ~ARCHIVE_EXTRACT_FFLAGS;
bsdtar->readdisk_flags |= ARCHIVE_READDISK_NO_FFLAGS;
bsdtar->flags |= OPTFLAG_NO_FFLAGS;
break;
case OPTION_NO_MAC_METADATA:
bsdtar->readdisk_flags &= ~ARCHIVE_READDISK_MAC_COPYFILE;
bsdtar->extract_flags &= ~ARCHIVE_EXTRACT_MAC_METADATA;
bsdtar->flags |= OPTFLAG_NO_MAC_METADATA;
break;
case OPTION_NO_SAFE_WRITES:
bsdtar->extract_flags &= ~ARCHIVE_EXTRACT_SAFE_WRITES;
break;
case OPTION_NO_SAME_OWNER:
bsdtar->extract_flags &= ~ARCHIVE_EXTRACT_OWNER;
break;
case OPTION_NO_SAME_PERMISSIONS:
bsdtar->extract_flags &= ~ARCHIVE_EXTRACT_PERM;
bsdtar->extract_flags &= ~ARCHIVE_EXTRACT_ACL;
bsdtar->extract_flags &= ~ARCHIVE_EXTRACT_XATTR;
bsdtar->extract_flags &= ~ARCHIVE_EXTRACT_FFLAGS;
bsdtar->extract_flags &= ~ARCHIVE_EXTRACT_MAC_METADATA;
break;
case OPTION_NO_XATTRS:
bsdtar->extract_flags &= ~ARCHIVE_EXTRACT_XATTR;
bsdtar->readdisk_flags |= ARCHIVE_READDISK_NO_XATTR;
bsdtar->flags |= OPTFLAG_NO_XATTRS;
break;
case OPTION_NULL:
bsdtar->flags |= OPTFLAG_NULL;
break;
case OPTION_NUMERIC_OWNER:
bsdtar->uname = "";
bsdtar->gname = "";
bsdtar->flags |= OPTFLAG_NUMERIC_OWNER;
break;
case 'O':
bsdtar->flags |= OPTFLAG_STDOUT;
break;
case 'o':
bsdtar->flags |= OPTFLAG_O;
break;






case OPTION_OLDER_CTIME:
if (archive_match_include_date(bsdtar->matching,
ARCHIVE_MATCH_CTIME | ARCHIVE_MATCH_OLDER,
bsdtar->argument) != ARCHIVE_OK)
lafe_errc(1, 0, "Error : %s",
archive_error_string(bsdtar->matching));
break;
case OPTION_OLDER_CTIME_THAN:
if (archive_match_include_file_time(bsdtar->matching,
ARCHIVE_MATCH_CTIME | ARCHIVE_MATCH_OLDER,
bsdtar->argument) != ARCHIVE_OK)
lafe_errc(1, 0, "Error : %s",
archive_error_string(bsdtar->matching));
break;
case OPTION_OLDER_MTIME:
if (archive_match_include_date(bsdtar->matching,
ARCHIVE_MATCH_MTIME | ARCHIVE_MATCH_OLDER,
bsdtar->argument) != ARCHIVE_OK)
lafe_errc(1, 0, "Error : %s",
archive_error_string(bsdtar->matching));
break;
case OPTION_OLDER_MTIME_THAN:
if (archive_match_include_file_time(bsdtar->matching,
ARCHIVE_MATCH_MTIME | ARCHIVE_MATCH_OLDER,
bsdtar->argument) != ARCHIVE_OK)
lafe_errc(1, 0, "Error : %s",
archive_error_string(bsdtar->matching));
break;
case OPTION_ONE_FILE_SYSTEM:
bsdtar->readdisk_flags |=
ARCHIVE_READDISK_NO_TRAVERSE_MOUNTS;
break;
case OPTION_OPTIONS:
bsdtar->option_options = bsdtar->argument;
break;
#if 0






case 'P':

break;
#endif
case 'P':
bsdtar->extract_flags &= ~SECURITY;
bsdtar->flags |= OPTFLAG_ABSOLUTE_PATHS;
break;
case 'p':
bsdtar->extract_flags |= ARCHIVE_EXTRACT_PERM;
bsdtar->extract_flags |= ARCHIVE_EXTRACT_ACL;
bsdtar->extract_flags |= ARCHIVE_EXTRACT_XATTR;
bsdtar->extract_flags |= ARCHIVE_EXTRACT_FFLAGS;
bsdtar->extract_flags |= ARCHIVE_EXTRACT_MAC_METADATA;
break;
case OPTION_PASSPHRASE:
bsdtar->passphrase = bsdtar->argument;
break;
case OPTION_POSIX:
cset_set_format(bsdtar->cset, "pax");
break;
case 'q':
bsdtar->flags |= OPTFLAG_FAST_READ;
break;
case 'r':
set_mode(bsdtar, opt);
break;
case 'S':
bsdtar->extract_flags |= ARCHIVE_EXTRACT_SPARSE;
break;
case 's':
#if defined(HAVE_REGEX_H) || defined(HAVE_PCREPOSIX_H)
add_substitution(bsdtar, bsdtar->argument);
#else
lafe_warnc(0,
"-s is not supported by this version of bsdtar");
usage();
#endif
break;
case OPTION_SAFE_WRITES:
bsdtar->extract_flags |= ARCHIVE_EXTRACT_SAFE_WRITES;
break;
case OPTION_SAME_OWNER:
bsdtar->extract_flags |= ARCHIVE_EXTRACT_OWNER;
break;
case OPTION_STRIP_COMPONENTS:
errno = 0;
tptr = NULL;
t = (int)strtol(bsdtar->argument, &tptr, 10);
if (errno || t < 0 || *(bsdtar->argument) == '\0' ||
tptr == NULL || *tptr != '\0') {
lafe_errc(1, 0, "Invalid argument to "
"--strip-components");
}
bsdtar->strip_components = t;
break;
case 'T':
bsdtar->names_from_file = bsdtar->argument;
break;
case 't':
set_mode(bsdtar, opt);
bsdtar->verbose++;
break;
case OPTION_TOTALS:
bsdtar->flags |= OPTFLAG_TOTALS;
break;
case 'U':
bsdtar->extract_flags |= ARCHIVE_EXTRACT_UNLINK;
bsdtar->flags |= OPTFLAG_UNLINK_FIRST;
break;
case 'u':
set_mode(bsdtar, opt);
break;
case OPTION_UID:
errno = 0;
tptr = NULL;
t = (int)strtol(bsdtar->argument, &tptr, 10);
if (errno || t < 0 || *(bsdtar->argument) == '\0' ||
tptr == NULL || *tptr != '\0') {
lafe_errc(1, 0, "Invalid argument to --uid");
}
bsdtar->uid = t;
break;
case OPTION_UNAME:
bsdtar->uname = bsdtar->argument;
break;
case OPTION_UUENCODE:
if (compression2 != '\0')
lafe_errc(1, 0,
"Can't specify both --uuencode and "
"--b64encode");
compression2 = opt;
compression2_name = "uuencode";
break;
case 'v':
bsdtar->verbose++;
break;
case OPTION_VERSION:
version();
break;
#if 0




case 'W':
break;
#endif
case 'w':
bsdtar->flags |= OPTFLAG_INTERACTIVE;
break;
case 'X':
if (archive_match_exclude_pattern_from_file(
bsdtar->matching, bsdtar->argument, 0)
!= ARCHIVE_OK)
lafe_errc(1, 0, "Error : %s",
archive_error_string(bsdtar->matching));
break;
case 'x':
set_mode(bsdtar, opt);
break;
case OPTION_XATTRS:
bsdtar->extract_flags |= ARCHIVE_EXTRACT_XATTR;
bsdtar->readdisk_flags &= ~ARCHIVE_READDISK_NO_XATTR;
bsdtar->flags |= OPTFLAG_XATTRS;
break;
case 'y':
if (compression != '\0')
lafe_errc(1, 0,
"Can't specify both -%c and -%c", opt,
compression);
compression = opt;
compression_name = "bzip2";
break;
case 'Z':
if (compression != '\0')
lafe_errc(1, 0,
"Can't specify both -%c and -%c", opt,
compression);
compression = opt;
compression_name = "compress";
break;
case 'z':
if (compression != '\0')
lafe_errc(1, 0,
"Can't specify both -%c and -%c", opt,
compression);
compression = opt;
compression_name = "gzip";
break;
case OPTION_USE_COMPRESS_PROGRAM:
compress_program = bsdtar->argument;
break;
default:
usage();
}
}






if ((bsdtar->mode == '\0') && possible_help_request) {
long_help();
exit(0);
}


if (bsdtar->mode == '\0')
lafe_errc(1, 0,
"Must specify one of -c, -r, -t, -u, -x");


if (bsdtar->flags & OPTFLAG_AUTO_COMPRESS)
only_mode(bsdtar, "-a", "c");
if (bsdtar->readdisk_flags & ARCHIVE_READDISK_NO_TRAVERSE_MOUNTS)
only_mode(bsdtar, "--one-file-system", "cru");
if (bsdtar->flags & OPTFLAG_FAST_READ)
only_mode(bsdtar, "--fast-read", "xt");
if (bsdtar->extract_flags & ARCHIVE_EXTRACT_HFS_COMPRESSION_FORCED)
only_mode(bsdtar, "--hfsCompression", "x");
if (bsdtar->extract_flags & ARCHIVE_EXTRACT_NO_HFS_COMPRESSION)
only_mode(bsdtar, "--nopreserveHFSCompression", "x");
if (bsdtar->readdisk_flags & ARCHIVE_READDISK_HONOR_NODUMP)
only_mode(bsdtar, "--nodump", "cru");
if (bsdtar->flags & OPTFLAG_ACLS)
only_mode(bsdtar, "--acls", "crux");
if (bsdtar->flags & OPTFLAG_NO_ACLS)
only_mode(bsdtar, "--no-acls", "crux");
if (bsdtar->flags & OPTFLAG_XATTRS)
only_mode(bsdtar, "--xattrs", "crux");
if (bsdtar->flags & OPTFLAG_NO_XATTRS)
only_mode(bsdtar, "--no-xattrs", "crux");
if (bsdtar->flags & OPTFLAG_FFLAGS)
only_mode(bsdtar, "--fflags", "crux");
if (bsdtar->flags & OPTFLAG_NO_FFLAGS)
only_mode(bsdtar, "--no-fflags", "crux");
if (bsdtar->flags & OPTFLAG_MAC_METADATA)
only_mode(bsdtar, "--mac-metadata", "crux");
if (bsdtar->flags & OPTFLAG_NO_MAC_METADATA)
only_mode(bsdtar, "--no-mac-metadata", "crux");
if (bsdtar->flags & OPTFLAG_O) {
switch (bsdtar->mode) {
case 'c':





cset_set_format(bsdtar->cset, "ustar");

break;
case 'x':

bsdtar->flags |= OPTFLAG_NO_OWNER;
bsdtar->extract_flags &= ~ARCHIVE_EXTRACT_OWNER;
break;
default:
only_mode(bsdtar, "-o", "xc");
break;
}
}
if (bsdtar->flags & OPTFLAG_STDOUT)
only_mode(bsdtar, "-O", "xt");
if (bsdtar->flags & OPTFLAG_UNLINK_FIRST)
only_mode(bsdtar, "-U", "x");
if (bsdtar->flags & OPTFLAG_WARN_LINKS)
only_mode(bsdtar, "--check-links", "cr");

if ((bsdtar->flags & OPTFLAG_AUTO_COMPRESS) &&
cset_auto_compress(bsdtar->cset, bsdtar->filename)) {

compression = '\0';
compression2 = '\0';
}

if (compress_program != NULL) {
only_mode(bsdtar, "--use-compress-program", "cxt");
cset_add_filter_program(bsdtar->cset, compress_program);

compression = '\0';
compression2 = '\0';
}
if (compression != '\0') {
switch (compression) {
case 'J': case 'j': case 'y': case 'Z': case 'z':
strcpy(buff, "-?");
buff[1] = compression;
break;
default:
strcpy(buff, "--");
strcat(buff, compression_name);
break;
}
only_mode(bsdtar, buff, "cxt");
cset_add_filter(bsdtar->cset, compression_name);
}
if (compression2 != '\0') {
strcpy(buff, "--");
strcat(buff, compression2_name);
only_mode(bsdtar, buff, "cxt");
cset_add_filter(bsdtar->cset, compression2_name);
}
if (cset_get_format(bsdtar->cset) != NULL)
only_mode(bsdtar, "--format", "cru");
if (bsdtar->symlink_mode != '\0') {
strcpy(buff, "-?");
buff[1] = bsdtar->symlink_mode;
only_mode(bsdtar, buff, "cru");
}








archive_match_set_inclusion_recursion(bsdtar->matching,
!(bsdtar->flags & OPTFLAG_NO_SUBDIRS));


if (strcmp(bsdtar->filename, "-") == 0)
bsdtar->filename = NULL;

switch(bsdtar->mode) {
case 'c':
tar_mode_c(bsdtar);
break;
case 'r':
tar_mode_r(bsdtar);
break;
case 't':
tar_mode_t(bsdtar);
break;
case 'u':
tar_mode_u(bsdtar);
break;
case 'x':
tar_mode_x(bsdtar);
break;
}

archive_match_free(bsdtar->matching);
#if defined(HAVE_REGEX_H) || defined(HAVE_PCREPOSIX_H)
cleanup_substitution(bsdtar);
#endif
cset_free(bsdtar->cset);
passphrase_free(bsdtar->ppbuff);

if (bsdtar->return_value != 0)
lafe_warnc(0,
"Error exit delayed from previous errors.");
return (bsdtar->return_value);
}

static void
set_mode(struct bsdtar *bsdtar, char opt)
{
if (bsdtar->mode != '\0' && bsdtar->mode != opt)
lafe_errc(1, 0,
"Can't specify both -%c and -%c", opt, bsdtar->mode);
bsdtar->mode = opt;
}




static void
only_mode(struct bsdtar *bsdtar, const char *opt, const char *valid_modes)
{
if (strchr(valid_modes, bsdtar->mode) == NULL)
lafe_errc(1, 0,
"Option %s is not permitted in mode -%c",
opt, bsdtar->mode);
}


void
usage(void)
{
const char *p;

p = lafe_getprogname();

fprintf(stderr, "Usage:\n");
fprintf(stderr, " List: %s -tf <archive-filename>\n", p);
fprintf(stderr, " Extract: %s -xf <archive-filename>\n", p);
fprintf(stderr, " Create: %s -cf <archive-filename> [filenames...]\n", p);
fprintf(stderr, " Help: %s --help\n", p);
exit(1);
}

static void
version(void)
{
printf("bsdtar %s - %s \n",
BSDTAR_VERSION_STRING,
archive_version_details());
exit(0);
}

static const char *long_help_msg =
"First option must be a mode specifier:\n"
" -c Create -r Add/Replace -t List -u Update -x Extract\n"
"Common Options:\n"
" -b #Use #512-byte records per I/O block\n"
" -f <filename> Location of archive (default " _PATH_DEFTAPE ")\n"
" -v Verbose\n"
" -w Interactive\n"
"Create: %p -c [options] [<file> | <dir> | @<archive> | -C <dir> ]\n"
" <file>, <dir> add these items to archive\n"
" -z, -j, -J, --lzma Compress archive with gzip/bzip2/xz/lzma\n"
" --format {ustar|pax|cpio|shar} Select archive format\n"
" --exclude <pattern> Skip files that match pattern\n"
" -C <dir> Change to <dir> before processing remaining files\n"
" @<archive> Add entries from <archive> to output\n"
"List: %p -t [options] [<patterns>]\n"
" <patterns> If specified, list only entries that match\n"
"Extract: %p -x [options] [<patterns>]\n"
" <patterns> If specified, extract only entries that match\n"
" -k Keep (don't overwrite) existing files\n"
" -m Don't restore modification times\n"
" -O Write entries to stdout, don't restore to disk\n"
" -p Restore permissions (including ACLs, owner, file flags)\n";












static void
long_help(void)
{
const char *prog;
const char *p;

prog = lafe_getprogname();

fflush(stderr);

p = (strcmp(prog,"bsdtar") != 0) ? "(bsdtar)" : "";
printf("%s%s: manipulate archive files\n", prog, p);

for (p = long_help_msg; *p != '\0'; p++) {
if (*p == '%') {
if (p[1] == 'p') {
fputs(prog, stdout);
p++;
} else
putchar('%');
} else
putchar(*p);
}
version();
}
