
























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_archive_clear_error)
{
struct archive* a = archive_read_new();

archive_set_error(a, 12, "abcdefgh");
assertEqualInt(12, archive_errno(a));
assertEqualString("abcdefgh", archive_error_string(a));

archive_clear_error(a);
assertEqualInt(0, archive_errno(a));
assertEqualString(NULL, archive_error_string(a));

archive_read_free(a);
}
