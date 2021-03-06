
























#include "archive_platform.h"

#if defined(HAVE_ERRNO_H)
#include <errno.h>
#endif

#include "archive_entry.h"
#include "archive_write_private.h"

static ssize_t archive_write_raw_data(struct archive_write *,
const void *buff, size_t s);
static int archive_write_raw_free(struct archive_write *);
static int archive_write_raw_header(struct archive_write *,
struct archive_entry *);

struct raw {
int entries_written;
};




int
archive_write_set_format_raw(struct archive *_a)
{
struct archive_write *a = (struct archive_write *)_a;
struct raw *raw;

archive_check_magic(_a, ARCHIVE_WRITE_MAGIC,
ARCHIVE_STATE_NEW, "archive_write_set_format_raw");


if (a->format_free != NULL)
(a->format_free)(a);

raw = (struct raw *)calloc(1, sizeof(*raw));
if (raw == NULL) {
archive_set_error(&a->archive, ENOMEM, "Can't allocate raw data");
return (ARCHIVE_FATAL);
}
raw->entries_written = 0;
a->format_data = raw;
a->format_name = "raw";

a->format_options = NULL;
a->format_write_header = archive_write_raw_header;
a->format_write_data = archive_write_raw_data;
a->format_finish_entry = NULL;

a->format_close = NULL;
a->format_free = archive_write_raw_free;
a->archive.archive_format = ARCHIVE_FORMAT_RAW;
a->archive.archive_format_name = "RAW";
return (ARCHIVE_OK);
}

static int
archive_write_raw_header(struct archive_write *a, struct archive_entry *entry)
{
struct raw *raw = (struct raw *)a->format_data;

if (archive_entry_filetype(entry) != AE_IFREG) {
archive_set_error(&a->archive, ERANGE,
"Raw format only supports filetype AE_IFREG");
return (ARCHIVE_FATAL);
}


if (raw->entries_written > 0) {
archive_set_error(&a->archive, ERANGE,
"Raw format only supports one entry per archive");
return (ARCHIVE_FATAL);
}
raw->entries_written++;

return (ARCHIVE_OK);
}

static ssize_t
archive_write_raw_data(struct archive_write *a, const void *buff, size_t s)
{
int ret;

ret = __archive_write_output(a, buff, s);
if (ret >= 0)
return (s);
else
return (ret);
}

static int
archive_write_raw_free(struct archive_write *a)
{
struct raw *raw;

raw = (struct raw *)a->format_data;
free(raw);
a->format_data = NULL;
return (ARCHIVE_OK);
}
