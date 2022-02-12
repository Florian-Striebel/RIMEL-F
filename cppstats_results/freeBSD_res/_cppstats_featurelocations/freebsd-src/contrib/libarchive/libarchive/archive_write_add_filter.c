
























#include "archive_platform.h"
__FBSDID("$FreeBSD$");

#if defined(HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif

#if defined(HAVE_ERRNO_H)
#include <errno.h>
#endif

#include "archive.h"
#include "archive_private.h"


static const
struct { int code; int (*setter)(struct archive *); } codes[] =
{
{ ARCHIVE_FILTER_NONE, archive_write_add_filter_none },
{ ARCHIVE_FILTER_GZIP, archive_write_add_filter_gzip },
{ ARCHIVE_FILTER_BZIP2, archive_write_add_filter_bzip2 },
{ ARCHIVE_FILTER_COMPRESS, archive_write_add_filter_compress },
{ ARCHIVE_FILTER_GRZIP, archive_write_add_filter_grzip },
{ ARCHIVE_FILTER_LRZIP, archive_write_add_filter_lrzip },
{ ARCHIVE_FILTER_LZ4, archive_write_add_filter_lz4 },
{ ARCHIVE_FILTER_LZIP, archive_write_add_filter_lzip },
{ ARCHIVE_FILTER_LZMA, archive_write_add_filter_lzma },
{ ARCHIVE_FILTER_LZOP, archive_write_add_filter_lzip },
{ ARCHIVE_FILTER_UU, archive_write_add_filter_uuencode },
{ ARCHIVE_FILTER_XZ, archive_write_add_filter_xz },
{ ARCHIVE_FILTER_ZSTD, archive_write_add_filter_zstd },
{ -1, NULL }
};

int
archive_write_add_filter(struct archive *a, int code)
{
int i;

for (i = 0; codes[i].code != -1; i++) {
if (code == codes[i].code)
return ((codes[i].setter)(a));
}

archive_set_error(a, EINVAL, "No such filter");
return (ARCHIVE_FATAL);
}
