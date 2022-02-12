
























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_archive_read_close_twice_open_filename)
{
const char *filename = "empty.file";
struct archive* a = archive_read_new();

assertMakeFile(filename, 0644, "");
assertEqualInt(ARCHIVE_OK, archive_read_support_format_empty(a));
assertEqualInt(ARCHIVE_OK, archive_read_open_filename(a, filename, 0));
assertEqualInt(0, archive_errno(a));
assertEqualString(NULL, archive_error_string(a));

assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(0, archive_errno(a));
assertEqualString(NULL, archive_error_string(a));

assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(0, archive_errno(a));
assertEqualString(NULL, archive_error_string(a));

assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}
