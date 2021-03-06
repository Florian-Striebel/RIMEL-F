























#include "test.h"

DEFINE_TEST(test_read_filter_compress_truncated)
{
const char data[] = {0x1f, 0x9d};
struct archive *a;

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_compress(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_FATAL,
archive_read_open_memory(a, data, sizeof(data)));

assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}


DEFINE_TEST(test_read_filter_compress_empty2)
{
const char data[] = {0x1f, 0x9d, 0x10};
struct archive *a;
struct archive_entry *ae;

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_compress(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_memory(a, data, sizeof(data)));

assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualInt(archive_filter_code(a, 0), ARCHIVE_FILTER_COMPRESS);
assertEqualString(archive_filter_name(a, 0), "compress (.Z)");
assertEqualInt(archive_format(a), ARCHIVE_FORMAT_EMPTY);

assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}


DEFINE_TEST(test_read_filter_compress_invalid)
{
const char data[] = {0x1f, 0x9d, 0x11};
struct archive *a;

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_compress(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_FATAL,
archive_read_open_memory(a, data, sizeof(data)));

assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}
