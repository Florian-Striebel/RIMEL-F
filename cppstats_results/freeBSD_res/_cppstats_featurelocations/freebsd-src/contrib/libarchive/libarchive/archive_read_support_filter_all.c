
























#include "archive_platform.h"
__FBSDID("$FreeBSD$");

#include "archive.h"
#include "archive_private.h"

#if ARCHIVE_VERSION_NUMBER < 4000000

int
archive_read_support_compression_all(struct archive *a)
{
return archive_read_support_filter_all(a);
}
#endif

int
archive_read_support_filter_all(struct archive *a)
{
archive_check_magic(a, ARCHIVE_READ_MAGIC,
ARCHIVE_STATE_NEW, "archive_read_support_filter_all");


archive_read_support_filter_bzip2(a);

archive_read_support_filter_compress(a);

archive_read_support_filter_gzip(a);

archive_read_support_filter_lzip(a);




archive_read_support_filter_lzma(a);

archive_read_support_filter_xz(a);

archive_read_support_filter_uu(a);

archive_read_support_filter_rpm(a);

archive_read_support_filter_lrzip(a);

archive_read_support_filter_lzop(a);

archive_read_support_filter_grzip(a);

archive_read_support_filter_lz4(a);

archive_read_support_filter_zstd(a);







archive_clear_error(a);
return (ARCHIVE_OK);
}
