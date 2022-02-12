
























#include "archive_platform.h"
__FBSDID("$FreeBSD$");

#include "archive.h"
#include "archive_private.h"

int
archive_read_support_filter_by_code(struct archive *a, int filter_code)
{
archive_check_magic(a, ARCHIVE_READ_MAGIC,
ARCHIVE_STATE_NEW, "archive_read_support_filter_by_code");

switch (filter_code) {
case ARCHIVE_FILTER_NONE:
return archive_read_support_filter_none(a);
break;
case ARCHIVE_FILTER_GZIP:
return archive_read_support_filter_gzip(a);
break;
case ARCHIVE_FILTER_BZIP2:
return archive_read_support_filter_bzip2(a);
break;
case ARCHIVE_FILTER_COMPRESS:
return archive_read_support_filter_compress(a);
break;
case ARCHIVE_FILTER_LZMA:
return archive_read_support_filter_lzma(a);
break;
case ARCHIVE_FILTER_XZ:
return archive_read_support_filter_xz(a);
break;
case ARCHIVE_FILTER_UU:
return archive_read_support_filter_uu(a);
break;
case ARCHIVE_FILTER_RPM:
return archive_read_support_filter_rpm(a);
break;
case ARCHIVE_FILTER_LZIP:
return archive_read_support_filter_lzip(a);
break;
case ARCHIVE_FILTER_LRZIP:
return archive_read_support_filter_lrzip(a);
break;
case ARCHIVE_FILTER_LZOP:
return archive_read_support_filter_lzop(a);
break;
case ARCHIVE_FILTER_GRZIP:
return archive_read_support_filter_grzip(a);
break;
case ARCHIVE_FILTER_LZ4:
return archive_read_support_filter_lz4(a);
break;
case ARCHIVE_FILTER_ZSTD:
return archive_read_support_filter_zstd(a);
break;
}
return (ARCHIVE_FATAL);
}
