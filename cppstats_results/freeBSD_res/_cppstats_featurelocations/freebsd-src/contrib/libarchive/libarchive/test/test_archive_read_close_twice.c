
























#include "test.h"
__FBSDID("$FreeBSD$");


DEFINE_TEST(test_archive_read_close_twice)
{
struct archive* a = archive_read_new();

assertEqualInt(0, archive_read_close(a));
assertEqualInt(0, archive_errno(a));
assertEqualString(NULL, archive_error_string(a));

assertEqualInt(0, archive_read_close(a));
assertEqualInt(0, archive_errno(a));
assertEqualString(NULL, archive_error_string(a));

archive_read_free(a);
}
