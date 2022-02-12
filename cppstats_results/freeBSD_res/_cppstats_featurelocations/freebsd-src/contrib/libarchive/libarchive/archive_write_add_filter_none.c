
























#include "archive_platform.h"
__FBSDID("$FreeBSD: head/lib/libarchive/archive_write_set_compression_none.c 201080 2009-12-28 02:03:54Z kientzle $");

#include "archive.h"

int
archive_write_set_compression_none(struct archive *a)
{
(void)a;
return (ARCHIVE_OK);
}

int
archive_write_add_filter_none(struct archive *a)
{
(void)a;
return (ARCHIVE_OK);
}
