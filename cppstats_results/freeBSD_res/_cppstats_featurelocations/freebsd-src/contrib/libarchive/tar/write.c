

























#include "bsdtar_platform.h"
__FBSDID("$FreeBSD$");

#if defined(HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif
#if defined(HAVE_SYS_STAT_H)
#include <sys/stat.h>
#endif
#if defined(HAVE_ATTR_XATTR_H)
#include <attr/xattr.h>
#endif
#if defined(HAVE_ERRNO_H)
#include <errno.h>
#endif
#if defined(HAVE_FCNTL_H)
#include <fcntl.h>
#endif
#if defined(HAVE_GRP_H)
#include <grp.h>
#endif
#if defined(HAVE_IO_H)
#include <io.h>
#endif
#if defined(HAVE_LIBGEN_H)
#include <libgen.h>
#endif
#if defined(HAVE_LIMITS_H)
#include <limits.h>
#endif
#if defined(HAVE_PATHS_H)
#include <paths.h>
#endif
#if defined(HAVE_PWD_H)
#include <pwd.h>
#endif
#if defined(HAVE_STDINT_H)
#include <stdint.h>
#endif
#include <stdio.h>
#if defined(HAVE_STDLIB_H)
#include <stdlib.h>
#endif
#if defined(HAVE_STRING_H)
#include <string.h>
#endif
#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif

#include "bsdtar.h"
#include "err.h"
#include "line_reader.h"

#if !defined(O_BINARY)
#define O_BINARY 0
#endif

struct archive_dir_entry {
struct archive_dir_entry *next;
time_t mtime_sec;
int mtime_nsec;
char *name;
};

struct archive_dir {
struct archive_dir_entry *head, *tail;
};

static int append_archive(struct bsdtar *, struct archive *,
struct archive *ina);
static int append_archive_filename(struct bsdtar *,
struct archive *, const char *fname);
static void archive_names_from_file(struct bsdtar *bsdtar,
struct archive *a);
static int copy_file_data_block(struct bsdtar *,
struct archive *a, struct archive *,
struct archive_entry *);
static void excluded_callback(struct archive *, void *,
struct archive_entry *);
static void report_write(struct bsdtar *, struct archive *,
struct archive_entry *, int64_t progress);
static void test_for_append(struct bsdtar *);
static int metadata_filter(struct archive *, void *,
struct archive_entry *);
static void write_archive(struct archive *, struct bsdtar *);
static void write_entry(struct bsdtar *, struct archive *,
struct archive_entry *);
static void write_file(struct bsdtar *, struct archive *,
struct archive_entry *);
static void write_hierarchy(struct bsdtar *, struct archive *,
const char *);

#if defined(_WIN32) && !defined(__CYGWIN__)

static int
seek_file(int fd, int64_t offset, int whence)
{
LARGE_INTEGER distance;
(void)whence;
distance.QuadPart = offset;
return (SetFilePointerEx((HANDLE)_get_osfhandle(fd),
distance, NULL, FILE_BEGIN) ? 1 : -1);
}
#define open _open
#define close _close
#define read _read
#if defined(lseek)
#undef lseek
#endif
#define lseek seek_file
#endif

static void
set_writer_options(struct bsdtar *bsdtar, struct archive *a)
{
const char *writer_options;
int r;

writer_options = getenv(ENV_WRITER_OPTIONS);
if (writer_options != NULL) {
size_t module_len = sizeof(IGNORE_WRONG_MODULE_NAME) - 1;
size_t opt_len = strlen(writer_options) + 1;
char *p;

if ((p = malloc(module_len + opt_len)) == NULL)
lafe_errc(1, errno, "Out of memory");



memcpy(p, IGNORE_WRONG_MODULE_NAME, module_len);
memcpy(p, writer_options, opt_len);
r = archive_write_set_options(a, p);
free(p);
if (r < ARCHIVE_WARN)
lafe_errc(1, 0, "%s", archive_error_string(a));
else
archive_clear_error(a);
}
if (ARCHIVE_OK != archive_write_set_options(a, bsdtar->option_options))
lafe_errc(1, 0, "%s", archive_error_string(a));
}

static void
set_reader_options(struct bsdtar *bsdtar, struct archive *a)
{
const char *reader_options;
int r;

(void)bsdtar;

reader_options = getenv(ENV_READER_OPTIONS);
if (reader_options != NULL) {
size_t module_len = sizeof(IGNORE_WRONG_MODULE_NAME) - 1;
size_t opt_len = strlen(reader_options) + 1;
char *p;

if ((p = malloc(module_len + opt_len)) == NULL)
if (p == NULL)
lafe_errc(1, errno, "Out of memory");



memcpy(p, IGNORE_WRONG_MODULE_NAME, module_len);
memcpy(p, reader_options, opt_len);
r = archive_read_set_options(a, p);
free(p);
if (r < ARCHIVE_WARN)
lafe_errc(1, 0, "%s", archive_error_string(a));
else
archive_clear_error(a);
}
}

void
tar_mode_c(struct bsdtar *bsdtar)
{
struct archive *a;
const void *filter_name;
int r;

if (*bsdtar->argv == NULL && bsdtar->names_from_file == NULL)
lafe_errc(1, 0, "no files or directories specified");

a = archive_write_new();


if (cset_get_format(bsdtar->cset) == NULL) {
r = archive_write_set_format_pax_restricted(a);
cset_set_format(bsdtar->cset, "pax restricted");
} else {
r = archive_write_set_format_by_name(a,
cset_get_format(bsdtar->cset));
}
if (r != ARCHIVE_OK) {
fprintf(stderr, "Can't use format %s: %s\n",
cset_get_format(bsdtar->cset),
archive_error_string(a));
usage();
}

archive_write_set_bytes_per_block(a, bsdtar->bytes_per_block);
archive_write_set_bytes_in_last_block(a, bsdtar->bytes_in_last_block);

r = cset_write_add_filters(bsdtar->cset, a, &filter_name);
if (r < ARCHIVE_WARN) {
lafe_errc(1, 0, "Unsupported compression option --%s",
(const char *)filter_name);
}

set_writer_options(bsdtar, a);
if (bsdtar->passphrase != NULL)
r = archive_write_set_passphrase(a, bsdtar->passphrase);
else
r = archive_write_set_passphrase_callback(a, bsdtar,
&passphrase_callback);
if (r != ARCHIVE_OK)
lafe_errc(1, 0, "%s", archive_error_string(a));
if (ARCHIVE_OK != archive_write_open_filename(a, bsdtar->filename))
lafe_errc(1, 0, "%s", archive_error_string(a));
write_archive(a, bsdtar);
}





void
tar_mode_r(struct bsdtar *bsdtar)
{
int64_t end_offset;
int format;
struct archive *a;
struct archive_entry *entry;
int r;


test_for_append(bsdtar);

format = ARCHIVE_FORMAT_TAR_PAX_RESTRICTED;

#if defined(__BORLANDC__)
bsdtar->fd = open(bsdtar->filename, O_RDWR | O_CREAT | O_BINARY);
#else
bsdtar->fd = open(bsdtar->filename, O_RDWR | O_CREAT | O_BINARY, 0666);
#endif
if (bsdtar->fd < 0)
lafe_errc(1, errno,
"Cannot open %s", bsdtar->filename);

a = archive_read_new();
archive_read_support_filter_all(a);
archive_read_support_format_empty(a);
archive_read_support_format_tar(a);
archive_read_support_format_gnutar(a);
set_reader_options(bsdtar, a);
r = archive_read_open_fd(a, bsdtar->fd, 10240);
if (r != ARCHIVE_OK)
lafe_errc(1, archive_errno(a),
"Can't read archive %s: %s", bsdtar->filename,
archive_error_string(a));
while (0 == archive_read_next_header(a, &entry)) {
if (archive_filter_code(a, 0) != ARCHIVE_FILTER_NONE) {
archive_read_free(a);
close(bsdtar->fd);
lafe_errc(1, 0,
"Cannot append to compressed archive.");
}

format = archive_format(a);
}

end_offset = archive_read_header_position(a);
archive_read_free(a);


a = archive_write_new();








if (cset_get_format(bsdtar->cset) != NULL) {

archive_write_set_format_by_name(a,
cset_get_format(bsdtar->cset));

format &= ARCHIVE_FORMAT_BASE_MASK;
if (format != (int)(archive_format(a) & ARCHIVE_FORMAT_BASE_MASK)
&& format != ARCHIVE_FORMAT_EMPTY) {
lafe_errc(1, 0,
"Format %s is incompatible with the archive %s.",
cset_get_format(bsdtar->cset), bsdtar->filename);
}
} else {




if (format == ARCHIVE_FORMAT_EMPTY)
format = ARCHIVE_FORMAT_TAR_PAX_RESTRICTED;
archive_write_set_format(a, format);
}
if (lseek(bsdtar->fd, end_offset, SEEK_SET) < 0)
lafe_errc(1, errno, "Could not seek to archive end");
set_writer_options(bsdtar, a);
if (ARCHIVE_OK != archive_write_open_fd(a, bsdtar->fd))
lafe_errc(1, 0, "%s", archive_error_string(a));

write_archive(a, bsdtar);

close(bsdtar->fd);
bsdtar->fd = -1;
}

void
tar_mode_u(struct bsdtar *bsdtar)
{
int64_t end_offset;
struct archive *a;
struct archive_entry *entry;
int format;
struct archive_dir_entry *p;
struct archive_dir archive_dir;

bsdtar->archive_dir = &archive_dir;
memset(&archive_dir, 0, sizeof(archive_dir));

format = ARCHIVE_FORMAT_TAR_PAX_RESTRICTED;


test_for_append(bsdtar);

bsdtar->fd = open(bsdtar->filename, O_RDWR | O_BINARY);
if (bsdtar->fd < 0)
lafe_errc(1, errno,
"Cannot open %s", bsdtar->filename);

a = archive_read_new();
archive_read_support_filter_all(a);
archive_read_support_format_tar(a);
archive_read_support_format_gnutar(a);
set_reader_options(bsdtar, a);
if (archive_read_open_fd(a, bsdtar->fd, bsdtar->bytes_per_block)
!= ARCHIVE_OK) {
lafe_errc(1, 0,
"Can't open %s: %s", bsdtar->filename,
archive_error_string(a));
}


while (0 == archive_read_next_header(a, &entry)) {
if (archive_filter_code(a, 0) != ARCHIVE_FILTER_NONE) {
archive_read_free(a);
close(bsdtar->fd);
lafe_errc(1, 0,
"Cannot append to compressed archive.");
}
if (archive_match_exclude_entry(bsdtar->matching,
ARCHIVE_MATCH_MTIME | ARCHIVE_MATCH_OLDER |
ARCHIVE_MATCH_EQUAL, entry) != ARCHIVE_OK)
lafe_errc(1, 0, "Error : %s",
archive_error_string(bsdtar->matching));

format = archive_format(a);

}

end_offset = archive_read_header_position(a);
archive_read_free(a);


a = archive_write_new();



archive_write_set_format(a, format);
archive_write_set_bytes_per_block(a, bsdtar->bytes_per_block);
archive_write_set_bytes_in_last_block(a, bsdtar->bytes_in_last_block);

if (lseek(bsdtar->fd, end_offset, SEEK_SET) < 0)
lafe_errc(1, errno, "Could not seek to archive end");
set_writer_options(bsdtar, a);
if (ARCHIVE_OK != archive_write_open_fd(a, bsdtar->fd))
lafe_errc(1, 0, "%s", archive_error_string(a));

write_archive(a, bsdtar);

close(bsdtar->fd);
bsdtar->fd = -1;

while (bsdtar->archive_dir->head != NULL) {
p = bsdtar->archive_dir->head->next;
free(bsdtar->archive_dir->head->name);
free(bsdtar->archive_dir->head);
bsdtar->archive_dir->head = p;
}
bsdtar->archive_dir->tail = NULL;
}





static void
write_archive(struct archive *a, struct bsdtar *bsdtar)
{
const char *arg;
struct archive_entry *entry, *sparse_entry;


bsdtar->buff_size = 64 * 1024;
while (bsdtar->buff_size < (size_t)bsdtar->bytes_per_block)
bsdtar->buff_size *= 2;

bsdtar->buff_size += 16 * 1024;


if ((bsdtar->buff = malloc(bsdtar->buff_size)) == NULL)
lafe_errc(1, 0, "cannot allocate memory");

if ((bsdtar->resolver = archive_entry_linkresolver_new()) == NULL)
lafe_errc(1, 0, "cannot create link resolver");
archive_entry_linkresolver_set_strategy(bsdtar->resolver,
archive_format(a));


if ((bsdtar->diskreader = archive_read_disk_new()) == NULL)
lafe_errc(1, 0, "Cannot create read_disk object");

switch (bsdtar->symlink_mode) {
case 'H':
archive_read_disk_set_symlink_hybrid(bsdtar->diskreader);
break;
case 'L':
archive_read_disk_set_symlink_logical(bsdtar->diskreader);
break;
default:
archive_read_disk_set_symlink_physical(bsdtar->diskreader);
break;
}

archive_read_disk_set_matching(bsdtar->diskreader,
bsdtar->matching, excluded_callback, bsdtar);
archive_read_disk_set_metadata_filter_callback(
bsdtar->diskreader, metadata_filter, bsdtar);

archive_read_disk_set_behavior(bsdtar->diskreader,
bsdtar->readdisk_flags);
archive_read_disk_set_standard_lookup(bsdtar->diskreader);

if (bsdtar->names_from_file != NULL)
archive_names_from_file(bsdtar, a);

while (*bsdtar->argv) {
arg = *bsdtar->argv;
if (arg[0] == '-' && arg[1] == 'C') {
arg += 2;
if (*arg == '\0') {
bsdtar->argv++;
arg = *bsdtar->argv;
if (arg == NULL) {
lafe_warnc(0, "%s",
"Missing argument for -C");
bsdtar->return_value = 1;
goto cleanup;
}
if (*arg == '\0') {
lafe_warnc(0,
"Meaningless argument for -C: ''");
bsdtar->return_value = 1;
goto cleanup;
}
}
set_chdir(bsdtar, arg);
} else {
if (*arg != '/')
do_chdir(bsdtar);
if (*arg == '@') {
if (append_archive_filename(bsdtar, a,
arg + 1) != 0)
break;
} else
write_hierarchy(bsdtar, a, arg);
}
bsdtar->argv++;
}

archive_read_disk_set_matching(bsdtar->diskreader, NULL, NULL, NULL);
archive_read_disk_set_metadata_filter_callback(
bsdtar->diskreader, NULL, NULL);
entry = NULL;
archive_entry_linkify(bsdtar->resolver, &entry, &sparse_entry);
while (entry != NULL) {
int r;
struct archive_entry *entry2;
struct archive *disk = bsdtar->diskreader;










r = archive_read_disk_open(disk,
archive_entry_sourcepath(entry));
if (r != ARCHIVE_OK) {
lafe_warnc(archive_errno(disk),
"%s", archive_error_string(disk));
bsdtar->return_value = 1;
goto next_entry;
}






entry2 = archive_entry_new();
r = archive_read_next_header2(disk, entry2);
archive_entry_free(entry2);
if (r != ARCHIVE_OK) {
lafe_warnc(archive_errno(disk),
"%s", archive_error_string(disk));
if (r == ARCHIVE_FATAL)
bsdtar->return_value = 1;
archive_read_close(disk);
goto next_entry;
}

write_file(bsdtar, a, entry);
archive_read_close(disk);
next_entry:
archive_entry_free(entry);
entry = NULL;
archive_entry_linkify(bsdtar->resolver, &entry, &sparse_entry);
}

if (archive_write_close(a)) {
lafe_warnc(0, "%s", archive_error_string(a));
bsdtar->return_value = 1;
}

cleanup:

free(bsdtar->buff);
archive_entry_linkresolver_free(bsdtar->resolver);
bsdtar->resolver = NULL;
archive_read_free(bsdtar->diskreader);
bsdtar->diskreader = NULL;

if (bsdtar->flags & OPTFLAG_TOTALS) {
fprintf(stderr, "Total bytes written: %s\n",
tar_i64toa(archive_filter_bytes(a, -1)));
}

archive_write_free(a);
}








static void
archive_names_from_file(struct bsdtar *bsdtar, struct archive *a)
{
struct lafe_line_reader *lr;
const char *line;

bsdtar->next_line_is_dir = 0;

lr = lafe_line_reader(bsdtar->names_from_file,
(bsdtar->flags & OPTFLAG_NULL));
while ((line = lafe_line_reader_next(lr)) != NULL) {
if (bsdtar->next_line_is_dir) {
if (*line != '\0')
set_chdir(bsdtar, line);
else {
lafe_warnc(0,
"Meaningless argument for -C: ''");
bsdtar->return_value = 1;
}
bsdtar->next_line_is_dir = 0;
} else if (((bsdtar->flags & OPTFLAG_NULL) == 0) &&
strcmp(line, "-C") == 0)
bsdtar->next_line_is_dir = 1;
else {
if (*line != '/')
do_chdir(bsdtar);
write_hierarchy(bsdtar, a, line);
}
}
lafe_line_reader_free(lr);
if (bsdtar->next_line_is_dir)
lafe_errc(1, errno,
"Unexpected end of filename list; "
"directory expected after -C");
}








static int
append_archive_filename(struct bsdtar *bsdtar, struct archive *a,
const char *raw_filename)
{
struct archive *ina;
const char *filename = raw_filename;
int rc;

if (strcmp(filename, "-") == 0)
filename = NULL;

ina = archive_read_new();
archive_read_support_format_all(ina);
archive_read_support_filter_all(ina);
set_reader_options(bsdtar, ina);
archive_read_set_options(ina, "mtree:checkfs");
if (bsdtar->passphrase != NULL)
rc = archive_read_add_passphrase(a, bsdtar->passphrase);
else
rc = archive_read_set_passphrase_callback(ina, bsdtar,
&passphrase_callback);
if (rc != ARCHIVE_OK)
lafe_errc(1, 0, "%s", archive_error_string(a));
if (archive_read_open_filename(ina, filename,
bsdtar->bytes_per_block)) {
lafe_warnc(0, "%s", archive_error_string(ina));
bsdtar->return_value = 1;
return (0);
}

rc = append_archive(bsdtar, a, ina);

if (rc != ARCHIVE_OK) {
lafe_warnc(0, "Error reading archive %s: %s",
raw_filename, archive_error_string(ina));
bsdtar->return_value = 1;
}
archive_read_free(ina);

return (rc);
}

static int
append_archive(struct bsdtar *bsdtar, struct archive *a, struct archive *ina)
{
struct archive_entry *in_entry;
int e;

while (ARCHIVE_OK == (e = archive_read_next_header(ina, &in_entry))) {
if (archive_match_excluded(bsdtar->matching, in_entry))
continue;
if ((bsdtar->flags & OPTFLAG_INTERACTIVE) &&
!yes("copy '%s'", archive_entry_pathname(in_entry)))
continue;
if (bsdtar->verbose > 1) {
safe_fprintf(stderr, "a ");
list_item_verbose(bsdtar, stderr, in_entry);
} else if (bsdtar->verbose > 0)
safe_fprintf(stderr, "a %s",
archive_entry_pathname(in_entry));
if (need_report())
report_write(bsdtar, a, in_entry, 0);

e = archive_write_header(a, in_entry);
if (e != ARCHIVE_OK) {
if (!bsdtar->verbose)
lafe_warnc(0, "%s: %s",
archive_entry_pathname(in_entry),
archive_error_string(a));
else
fprintf(stderr, ": %s", archive_error_string(a));
}
if (e == ARCHIVE_FATAL)
exit(1);

if (e >= ARCHIVE_WARN) {
if (archive_entry_size(in_entry) == 0)
archive_read_data_skip(ina);
else if (copy_file_data_block(bsdtar, a, ina, in_entry))
exit(1);
}

if (bsdtar->verbose)
fprintf(stderr, "\n");
}

return (e == ARCHIVE_EOF ? ARCHIVE_OK : e);
}


static int
copy_file_data_block(struct bsdtar *bsdtar, struct archive *a,
struct archive *in_a, struct archive_entry *entry)
{
size_t bytes_read;
ssize_t bytes_written;
int64_t offset, progress = 0;
char *null_buff = NULL;
const void *buff;
int r;

while ((r = archive_read_data_block(in_a, &buff,
&bytes_read, &offset)) == ARCHIVE_OK) {
if (need_report())
report_write(bsdtar, a, entry, progress);

if (offset > progress) {
int64_t sparse = offset - progress;
size_t ns;

if (null_buff == NULL) {
null_buff = bsdtar->buff;
memset(null_buff, 0, bsdtar->buff_size);
}

while (sparse > 0) {
if (sparse > (int64_t)bsdtar->buff_size)
ns = bsdtar->buff_size;
else
ns = (size_t)sparse;
bytes_written =
archive_write_data(a, null_buff, ns);
if (bytes_written < 0) {

lafe_warnc(0, "%s",
archive_error_string(a));
return (-1);
}
if ((size_t)bytes_written < ns) {


lafe_warnc(0,
"%s: Truncated write; file may "
"have grown while being archived.",
archive_entry_pathname(entry));
return (0);
}
progress += bytes_written;
sparse -= bytes_written;
}
}

bytes_written = archive_write_data(a, buff, bytes_read);
if (bytes_written < 0) {

lafe_warnc(0, "%s", archive_error_string(a));
return (-1);
}
if ((size_t)bytes_written < bytes_read) {

lafe_warnc(0,
"%s: Truncated write; file may have grown "
"while being archived.",
archive_entry_pathname(entry));
return (0);
}
progress += bytes_written;
}
if (r < ARCHIVE_WARN) {
lafe_warnc(archive_errno(a), "%s", archive_error_string(a));
return (-1);
}
return (0);
}

static void
excluded_callback(struct archive *a, void *_data, struct archive_entry *entry)
{
struct bsdtar *bsdtar = (struct bsdtar *)_data;

if (bsdtar->flags & OPTFLAG_NO_SUBDIRS)
return;
if (!archive_read_disk_can_descend(a))
return;
if ((bsdtar->flags & OPTFLAG_INTERACTIVE) &&
!yes("add '%s'", archive_entry_pathname(entry)))
return;
archive_read_disk_descend(a);
}

static int
metadata_filter(struct archive *a, void *_data, struct archive_entry *entry)
{
struct bsdtar *bsdtar = (struct bsdtar *)_data;





















if ((bsdtar->flags & OPTFLAG_INTERACTIVE) &&
!yes("add '%s'", archive_entry_pathname(entry)))
return (0);


if (((bsdtar->flags & OPTFLAG_NO_SUBDIRS) == 0) &&
archive_read_disk_can_descend(a))
archive_read_disk_descend(a);

return (1);
}




static void
write_hierarchy(struct bsdtar *bsdtar, struct archive *a, const char *path)
{
struct archive *disk = bsdtar->diskreader;
struct archive_entry *entry = NULL, *spare_entry = NULL;
int r;

r = archive_read_disk_open(disk, path);
if (r != ARCHIVE_OK) {
lafe_warnc(archive_errno(disk),
"%s", archive_error_string(disk));
bsdtar->return_value = 1;
return;
}
bsdtar->first_fs = -1;

for (;;) {
archive_entry_free(entry);
entry = archive_entry_new();
r = archive_read_next_header2(disk, entry);
if (r == ARCHIVE_EOF)
break;
else if (r != ARCHIVE_OK) {
lafe_warnc(archive_errno(disk),
"%s", archive_error_string(disk));
if (r == ARCHIVE_FATAL || r == ARCHIVE_FAILED) {
bsdtar->return_value = 1;
archive_entry_free(entry);
archive_read_close(disk);
return;
} else if (r < ARCHIVE_WARN)
continue;
}

if (bsdtar->uid >= 0) {
archive_entry_set_uid(entry, bsdtar->uid);
if (!bsdtar->uname)
archive_entry_set_uname(entry,
archive_read_disk_uname(bsdtar->diskreader,
bsdtar->uid));
}
if (bsdtar->gid >= 0) {
archive_entry_set_gid(entry, bsdtar->gid);
if (!bsdtar->gname)
archive_entry_set_gname(entry,
archive_read_disk_gname(bsdtar->diskreader,
bsdtar->gid));
}
if (bsdtar->uname)
archive_entry_set_uname(entry, bsdtar->uname);
if (bsdtar->gname)
archive_entry_set_gname(entry, bsdtar->gname);





if (edit_pathname(bsdtar, entry))
continue;


if (bsdtar->verbose > 1) {
safe_fprintf(stderr, "a ");
list_item_verbose(bsdtar, stderr, entry);
} else if (bsdtar->verbose > 0) {

safe_fprintf(stderr, "a %s",
archive_entry_pathname(entry));
}


if (archive_entry_filetype(entry) != AE_IFREG)
archive_entry_set_size(entry, 0);

archive_entry_linkify(bsdtar->resolver, &entry, &spare_entry);

while (entry != NULL) {
write_file(bsdtar, a, entry);
archive_entry_free(entry);
entry = spare_entry;
spare_entry = NULL;
}

if (bsdtar->verbose)
fprintf(stderr, "\n");
}
archive_entry_free(entry);
archive_read_close(disk);
}





static void
write_file(struct bsdtar *bsdtar, struct archive *a,
struct archive_entry *entry)
{
write_entry(bsdtar, a, entry);
}




static void
write_entry(struct bsdtar *bsdtar, struct archive *a,
struct archive_entry *entry)
{
int e;

e = archive_write_header(a, entry);
if (e != ARCHIVE_OK) {
if (bsdtar->verbose > 1) {
safe_fprintf(stderr, "a ");
list_item_verbose(bsdtar, stderr, entry);
lafe_warnc(0, ": %s", archive_error_string(a));
} else if (bsdtar->verbose > 0) {
lafe_warnc(0, "%s: %s",
archive_entry_pathname(entry),
archive_error_string(a));
} else
fprintf(stderr, ": %s", archive_error_string(a));
}

if (e == ARCHIVE_FATAL)
exit(1);







if (e >= ARCHIVE_WARN && archive_entry_size(entry) > 0) {
if (copy_file_data_block(bsdtar, a, bsdtar->diskreader, entry))
exit(1);
}
}

static void
report_write(struct bsdtar *bsdtar, struct archive *a,
struct archive_entry *entry, int64_t progress)
{
uint64_t comp, uncomp;
int compression;

if (bsdtar->verbose)
fprintf(stderr, "\n");
comp = archive_filter_bytes(a, -1);
uncomp = archive_filter_bytes(a, 0);
fprintf(stderr, "In: %d files, %s bytes;",
archive_file_count(a), tar_i64toa(uncomp));
if (comp >= uncomp)
compression = 0;
else
compression = (int)((uncomp - comp) * 100 / uncomp);
fprintf(stderr,
" Out: %s bytes, compression %d%%\n",
tar_i64toa(comp), compression);

safe_fprintf(stderr, "Current: %s (%s",
archive_entry_pathname(entry),
tar_i64toa(progress));
fprintf(stderr, "/%s bytes)\n",
tar_i64toa(archive_entry_size(entry)));
}

static void
test_for_append(struct bsdtar *bsdtar)
{
struct stat s;

if (*bsdtar->argv == NULL && bsdtar->names_from_file == NULL)
lafe_errc(1, 0, "no files or directories specified");
if (bsdtar->filename == NULL)
lafe_errc(1, 0, "Cannot append to stdout.");

if (stat(bsdtar->filename, &s) != 0)
return;

if (!S_ISREG(s.st_mode) && !S_ISBLK(s.st_mode))
lafe_errc(1, 0,
"Cannot append to %s: not a regular file.",
bsdtar->filename);







}
