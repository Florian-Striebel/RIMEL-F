
























#include "archive_platform.h"
__FBSDID("$FreeBSD$");

#include "archive.h"
#include "archive_private.h"

#if ARCHIVE_VERSION_NUMBER < 4000000

int
archive_read_support_compression_none(struct archive *a)
{
return archive_read_support_filter_none(a);
}
#endif





int
archive_read_support_filter_none(struct archive *a)
{
archive_check_magic(a, ARCHIVE_READ_MAGIC,
ARCHIVE_STATE_NEW, "archive_read_support_filter_none");

return (ARCHIVE_OK);
}
