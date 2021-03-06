
























#include "test.h"
__FBSDID("$FreeBSD$");

static void
test(struct archive *a, int code, const char *msg)
{
archive_set_error(a, code, "%s", msg);

assertEqualInt(code, archive_errno(a));
assertEqualString(msg, archive_error_string(a));
}

DEFINE_TEST(test_archive_set_error)
{
struct archive* a = archive_read_new();




test(a, 12, "abcdefgh");
test(a, 0, "123456");
test(a, -1, "tuvw");
test(a, 34, "XYZ");

archive_read_free(a);
}
