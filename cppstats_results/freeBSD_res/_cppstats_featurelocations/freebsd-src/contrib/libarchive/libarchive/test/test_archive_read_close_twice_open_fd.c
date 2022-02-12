
























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_archive_read_close_twice_open_fd)
{
struct archive* a = archive_read_new();

assertEqualInt(ARCHIVE_OK, archive_read_support_format_empty(a));
assertEqualInt(ARCHIVE_OK, archive_read_open_fd(a, 0, 0));
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
