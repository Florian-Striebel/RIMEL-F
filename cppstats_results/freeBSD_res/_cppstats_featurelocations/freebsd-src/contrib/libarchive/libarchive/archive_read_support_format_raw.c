























#include "archive_platform.h"
__FBSDID("$FreeBSD$");

#if defined(HAVE_ERRNO_H)
#include <errno.h>
#endif
#include <stdio.h>
#if defined(HAVE_STDLIB_H)
#include <stdlib.h>
#endif

#include "archive.h"
#include "archive_entry.h"
#include "archive_private.h"
#include "archive_read_private.h"

struct raw_info {
int64_t offset;
int64_t unconsumed;
int end_of_file;
};

static int archive_read_format_raw_bid(struct archive_read *, int);
static int archive_read_format_raw_cleanup(struct archive_read *);
static int archive_read_format_raw_read_data(struct archive_read *,
const void **, size_t *, int64_t *);
static int archive_read_format_raw_read_data_skip(struct archive_read *);
static int archive_read_format_raw_read_header(struct archive_read *,
struct archive_entry *);

int
archive_read_support_format_raw(struct archive *_a)
{
struct raw_info *info;
struct archive_read *a = (struct archive_read *)_a;
int r;

archive_check_magic(_a, ARCHIVE_READ_MAGIC,
ARCHIVE_STATE_NEW, "archive_read_support_format_raw");

info = (struct raw_info *)calloc(1, sizeof(*info));
if (info == NULL) {
archive_set_error(&a->archive, ENOMEM,
"Can't allocate raw_info data");
return (ARCHIVE_FATAL);
}

r = __archive_read_register_format(a,
info,
"raw",
archive_read_format_raw_bid,
NULL,
archive_read_format_raw_read_header,
archive_read_format_raw_read_data,
archive_read_format_raw_read_data_skip,
NULL,
archive_read_format_raw_cleanup,
NULL,
NULL);
if (r != ARCHIVE_OK)
free(info);
return (r);
}








static int
archive_read_format_raw_bid(struct archive_read *a, int best_bid)
{
if (best_bid < 1 && __archive_read_ahead(a, 1, NULL) != NULL)
return (1);
return (-1);
}




static int
archive_read_format_raw_read_header(struct archive_read *a,
struct archive_entry *entry)
{
struct raw_info *info;

info = (struct raw_info *)(a->format->data);
if (info->end_of_file)
return (ARCHIVE_EOF);

a->archive.archive_format = ARCHIVE_FORMAT_RAW;
a->archive.archive_format_name = "raw";
archive_entry_set_pathname(entry, "data");
archive_entry_set_filetype(entry, AE_IFREG);
archive_entry_set_perm(entry, 0644);



return __archive_read_header(a, entry);
}

static int
archive_read_format_raw_read_data(struct archive_read *a,
const void **buff, size_t *size, int64_t *offset)
{
struct raw_info *info;
ssize_t avail;

info = (struct raw_info *)(a->format->data);


if (info->unconsumed) {
__archive_read_consume(a, info->unconsumed);
info->unconsumed = 0;
}

if (info->end_of_file)
return (ARCHIVE_EOF);


*buff = __archive_read_ahead(a, 1, &avail);
if (avail > 0) {

*size = avail;
*offset = info->offset;
info->offset += *size;
info->unconsumed = avail;
return (ARCHIVE_OK);
} else if (0 == avail) {

info->end_of_file = 1;
*size = 0;
*offset = info->offset;
return (ARCHIVE_EOF);
} else {

*size = 0;
*offset = info->offset;
return ((int)avail);
}
}

static int
archive_read_format_raw_read_data_skip(struct archive_read *a)
{
struct raw_info *info = (struct raw_info *)(a->format->data);


if (info->unconsumed) {
__archive_read_consume(a, info->unconsumed);
info->unconsumed = 0;
}
info->end_of_file = 1;
return (ARCHIVE_OK);
}

static int
archive_read_format_raw_cleanup(struct archive_read *a)
{
struct raw_info *info;

info = (struct raw_info *)(a->format->data);
free(info);
a->format->data = NULL;
return (ARCHIVE_OK);
}
