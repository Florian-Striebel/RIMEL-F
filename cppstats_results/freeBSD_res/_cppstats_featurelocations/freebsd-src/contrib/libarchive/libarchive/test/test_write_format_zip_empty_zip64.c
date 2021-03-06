




























#include "test.h"
__FBSDID("$FreeBSD: head/lib/libarchive/test/test_write_format_zip_empty.c 201247 2009-12-30 05:59:21Z kientzle $");

DEFINE_TEST(test_write_format_zip_empty_zip64)
{
struct archive *a;
struct archive_entry *ae;
char buff[256];
size_t used;


assert((a = archive_write_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_format_zip(a));
assertEqualIntA(a, ARCHIVE_OK, archive_write_add_filter_none(a));
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_bytes_per_block(a, 1));
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_bytes_in_last_block(a, 1));

assertEqualIntA(a, ARCHIVE_OK, archive_write_set_format_option(a, "zip", "zip64", "1"));
assertEqualIntA(a, ARCHIVE_OK, archive_write_open_memory(a, buff, sizeof(buff), &used));


assertEqualIntA(a, ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));


assertEqualInt(used, 98);
assertEqualMem(buff,
"PK\006\006"
"\x2c\0\0\0\0\0\0\0"
"\x2d\0"
"\x2d\0"
"\0\0\0\0"
"\0\0\0\0"
"\0\0\0\0\0\0\0\0"
"\0\0\0\0\0\0\0\0"
"\0\0\0\0\0\0\0\0"
"\0\0\0\0\0\0\0\0"

"PK\006\007"
"\0\0\0\0"
"\0\0\0\0\0\0\0\0"
"\1\0\0\0"

"PK\005\006"
"\0\0"
"\0\0"
"\0\0"
"\0\0"
"\0\0\0\0"
"\0\0\0\0"
"\0\0" ,
98);




assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_zip(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_open_memory(a, buff, 98));
assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_zip(a));
assertEqualIntA(a, ARCHIVE_OK, read_open_memory(a, buff, 98, 1));
assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_zip(a));
assertEqualIntA(a, ARCHIVE_OK, read_open_memory_seek(a, buff, 98, 98));
assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));
}
