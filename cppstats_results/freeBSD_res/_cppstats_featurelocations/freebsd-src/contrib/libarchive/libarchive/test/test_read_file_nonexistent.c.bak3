























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_read_file_nonexistent)
{
struct archive* a = archive_read_new();
assertEqualInt(ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualInt(ARCHIVE_FATAL,
archive_read_open_filename(a, "notexistent.tar", 512));
archive_read_free(a);
}


