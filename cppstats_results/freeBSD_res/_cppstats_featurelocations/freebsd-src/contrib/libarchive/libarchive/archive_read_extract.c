
























#include "archive_platform.h"
__FBSDID("$FreeBSD$");

#if defined(HAVE_ERRNO_H)
#include <errno.h>
#endif

#include "archive.h"
#include "archive_entry.h"
#include "archive_private.h"
#include "archive_read_private.h"

int
archive_read_extract(struct archive *_a, struct archive_entry *entry, int flags)
{
struct archive_read_extract *extract;
struct archive_read * a = (struct archive_read *)_a;

extract = __archive_read_get_extract(a);
if (extract == NULL)
return (ARCHIVE_FATAL);


if (extract->ad == NULL) {
extract->ad = archive_write_disk_new();
if (extract->ad == NULL) {
archive_set_error(&a->archive, ENOMEM, "Can't extract");
return (ARCHIVE_FATAL);
}
archive_write_disk_set_standard_lookup(extract->ad);
}

archive_write_disk_set_options(extract->ad, flags);
return (archive_read_extract2(&a->archive, entry, extract->ad));
}
