
























#include "test.h"
__FBSDID("$FreeBSD$");

#define should(__a, __code, __m, __o, __v) assertEqualInt(__code, archive_read_set_filter_option(__a, __m, __o, __v))


static void
test(int pristine)
{
struct archive* a = archive_read_new();

if (!pristine)
archive_read_support_filter_all(a);

should(a, ARCHIVE_OK, NULL, NULL, NULL);
should(a, ARCHIVE_OK, "", "", "");

should(a, ARCHIVE_FAILED, NULL, "fubar", NULL);
should(a, ARCHIVE_FAILED, NULL, "fubar", "snafu");
should(a, ARCHIVE_FAILED, "fubar", "snafu", NULL);
should(a, ARCHIVE_FAILED, "fubar", "snafu", "betcha");

archive_read_free(a);
}

DEFINE_TEST(test_archive_read_set_filter_option)
{
test(1);
test(0);
}
