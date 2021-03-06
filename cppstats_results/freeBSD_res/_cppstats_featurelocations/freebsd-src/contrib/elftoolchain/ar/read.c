


























#include <sys/queue.h>
#include <sys/stat.h>

#include <archive.h>
#include <archive_entry.h>
#include <assert.h>
#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ar.h"

ELFTC_VCSID("$Id: read.c 3629 2018-09-30 19:26:28Z jkoshy $");







int
ar_read_archive(struct bsdar *bsdar, int mode)
{
FILE *out;
struct archive *a;
struct archive_entry *entry;
struct stat sb;
struct tm *tp;
const char *bname;
const char *name;
mode_t md;
size_t size;
time_t mtime;
uid_t uid;
gid_t gid;
char **av;
char buf[25];
int found;
int exitcode, i, flags, r;

assert(mode == 'p' || mode == 't' || mode == 'x');

if ((a = archive_read_new()) == NULL)
bsdar_errc(bsdar, 0, "archive_read_new failed");
archive_read_support_format_ar(a);
AC(archive_read_open_filename(a, bsdar->filename, DEF_BLKSZ));

exitcode = EXIT_SUCCESS;
out = bsdar->output;

for (;;) {
r = archive_read_next_header(a, &entry);
if (r == ARCHIVE_WARN || r == ARCHIVE_RETRY ||
r == ARCHIVE_FATAL)
bsdar_warnc(bsdar, 0, "%s", archive_error_string(a));
if (r == ARCHIVE_EOF || r == ARCHIVE_FATAL)
break;
if (r == ARCHIVE_RETRY) {
bsdar_warnc(bsdar, 0, "Retrying...");
continue;
}

if (archive_format(a) == ARCHIVE_FORMAT_AR_BSD)
bsdar->options |= AR_BSD;
else
bsdar->options &= ~AR_BSD;

if ((name = archive_entry_pathname(entry)) == NULL)
break;


if (bsdar_is_pseudomember(bsdar, name))
continue;


if (strchr(name, '/')) {
bsdar_warnc(bsdar, 0, "ignoring entry: %s",
name);
continue;
}





if (bsdar->argc > 0) {
found = 0;
for(i = 0; i < bsdar->argc; i++) {
av = &bsdar->argv[i];
if (*av == NULL)
continue;




if ((bname = basename(*av)) == NULL)
bsdar_errc(bsdar, errno,
"basename failed");
if (strcmp(bname, name) != 0)
continue;

*av = NULL;
found = 1;
break;
}
if (!found)
continue;
}

if (mode == 't') {
if (bsdar->options & AR_V) {
md = archive_entry_mode(entry);
uid = archive_entry_uid(entry);
gid = archive_entry_gid(entry);
size = archive_entry_size(entry);
mtime = archive_entry_mtime(entry);
(void)fprintf(out, "%s %6d/%-6d %8ju ",
bsdar_strmode(md) + 1, uid, gid,
(uintmax_t)size);
tp = localtime(&mtime);
(void)strftime(buf, sizeof(buf),
"%b %e %H:%M %Y", tp);
(void)fprintf(out, "%s %s", buf, name);
} else
(void)fprintf(out, "%s", name);
r = archive_read_data_skip(a);
if (r == ARCHIVE_WARN || r == ARCHIVE_RETRY ||
r == ARCHIVE_FATAL) {
(void)fprintf(out, "\n");
bsdar_warnc(bsdar, 0, "%s",
archive_error_string(a));
}

if (r == ARCHIVE_FATAL)
break;

(void)fprintf(out, "\n");
} else {

if (mode == 'p') {
if (bsdar->options & AR_V) {
(void)fprintf(out, "\n<%s>\n\n",
name);
fflush(out);
}
r = archive_read_data_into_fd(a, fileno(out));
} else {

if (stat(name, &sb) != 0) {
if (errno != ENOENT) {
bsdar_warnc(bsdar, errno,
"stat %s failed",
bsdar->filename);
continue;
}
} else {

if (bsdar->options & AR_CC)
continue;
if (bsdar->options & AR_U &&
archive_entry_mtime(entry) <=
sb.st_mtime)
continue;
}

if (bsdar->options & AR_V)
(void)fprintf(out, "x - %s\n", name);

flags = ARCHIVE_EXTRACT_SECURE_SYMLINKS |
ARCHIVE_EXTRACT_SECURE_NODOTDOT;
if (bsdar->options & AR_O)
flags |= ARCHIVE_EXTRACT_TIME;

r = archive_read_extract(a, entry, flags);
}

if (r) {
bsdar_warnc(bsdar, 0, "%s",
archive_error_string(a));
exitcode = EXIT_FAILURE;
}
}
}

if (r == ARCHIVE_FATAL)
exitcode = EXIT_FAILURE;

AC(archive_read_close(a));
ACV(archive_read_free(a));

return (exitcode);
}
