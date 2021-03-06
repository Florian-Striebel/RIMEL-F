























#include "test.h"
__FBSDID("$FreeBSD$");


DEFINE_TEST(test_bad_fd)
{
struct archive *a;
assert((a = archive_read_new()) != NULL);
assertA(0 == archive_read_support_filter_all(a));
assertA(ARCHIVE_FATAL == archive_read_open_fd(a, -1, 1024));
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}
