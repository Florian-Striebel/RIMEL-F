
























#include "archive_platform.h"
__FBSDID("$FreeBSD: src/lib/libarchive/archive_read_extract.c,v 1.61 2008/05/26 17:00:22 kientzle Exp $");

#if defined(HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif
#if defined(HAVE_ERRNO_H)
#include <errno.h>
#endif
#if defined(HAVE_STRING_H)
#include <string.h>
#endif

#include "archive.h"
#include "archive_entry.h"
#include "archive_private.h"
#include "archive_read_private.h"

static int copy_data(struct archive *ar, struct archive *aw);
static int archive_read_extract_cleanup(struct archive_read *);





struct archive_read_extract *
__archive_read_get_extract(struct archive_read *a)
{
if (a->extract == NULL) {
a->extract = (struct archive_read_extract *)calloc(1, sizeof(*a->extract));
if (a->extract == NULL) {
archive_set_error(&a->archive, ENOMEM, "Can't extract");
return (NULL);
}
a->cleanup_archive_extract = archive_read_extract_cleanup;
}
return (a->extract);
}




static int
archive_read_extract_cleanup(struct archive_read *a)
{
int ret = ARCHIVE_OK;

if (a->extract->ad != NULL) {
ret = archive_write_free(a->extract->ad);
}
free(a->extract);
a->extract = NULL;
return (ret);
}

int
archive_read_extract2(struct archive *_a, struct archive_entry *entry,
struct archive *ad)
{
struct archive_read *a = (struct archive_read *)_a;
int r, r2;


if (a->skip_file_set)
archive_write_disk_set_skip_file(ad,
a->skip_file_dev, a->skip_file_ino);
r = archive_write_header(ad, entry);
if (r < ARCHIVE_WARN)
r = ARCHIVE_WARN;
if (r != ARCHIVE_OK)

archive_copy_error(&a->archive, ad);
else if (!archive_entry_size_is_set(entry) || archive_entry_size(entry) > 0)

r = copy_data(_a, ad);
r2 = archive_write_finish_entry(ad);
if (r2 < ARCHIVE_WARN)
r2 = ARCHIVE_WARN;

if (r2 != ARCHIVE_OK && r == ARCHIVE_OK)
archive_copy_error(&a->archive, ad);

if (r2 < r)
r = r2;
return (r);
}

void
archive_read_extract_set_progress_callback(struct archive *_a,
void (*progress_func)(void *), void *user_data)
{
struct archive_read *a = (struct archive_read *)_a;
struct archive_read_extract *extract = __archive_read_get_extract(a);
if (extract != NULL) {
extract->extract_progress = progress_func;
extract->extract_progress_user_data = user_data;
}
}

static int
copy_data(struct archive *ar, struct archive *aw)
{
int64_t offset;
const void *buff;
struct archive_read_extract *extract;
size_t size;
int r;

extract = __archive_read_get_extract((struct archive_read *)ar);
if (extract == NULL)
return (ARCHIVE_FATAL);
for (;;) {
r = archive_read_data_block(ar, &buff, &size, &offset);
if (r == ARCHIVE_EOF)
return (ARCHIVE_OK);
if (r != ARCHIVE_OK)
return (r);
r = (int)archive_write_data_block(aw, buff, size, offset);
if (r < ARCHIVE_WARN)
r = ARCHIVE_WARN;
if (r < ARCHIVE_OK) {
archive_set_error(ar, archive_errno(aw),
"%s", archive_error_string(aw));
return (r);
}
if (extract->extract_progress)
(extract->extract_progress)
(extract->extract_progress_user_data);
}
}
