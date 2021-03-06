
























#include "test.h"
__FBSDID("$FreeBSD$");

#define should(__a, __code, __m, __o, __v) assertEqualInt(__code, archive_read_set_format_option(__a, __m, __o, __v))


static void
test(int pristine)
{
struct archive* a = archive_read_new();
int known_option_rv = pristine ? ARCHIVE_FAILED : ARCHIVE_OK;

if (!pristine)
archive_read_support_format_all(a);



should(a, ARCHIVE_OK, NULL, NULL, NULL);
should(a, ARCHIVE_OK, "", "", "");


should(a, ARCHIVE_FAILED, "fubar", "snafu", NULL);
should(a, ARCHIVE_FAILED, "fubar", "snafu", "betcha");


should(a, ARCHIVE_FAILED, NULL, "snafu", NULL);
should(a, ARCHIVE_FAILED, NULL, "snafu", "betcha");


should(a, known_option_rv, "iso9660", "joliet", NULL);
should(a, known_option_rv, "iso9660", "joliet", NULL);
should(a, known_option_rv, NULL, "joliet", NULL);
should(a, known_option_rv, NULL, "joliet", NULL);

archive_read_free(a);
}

DEFINE_TEST(test_archive_read_set_format_option)
{
test(1);
test(0);
}
