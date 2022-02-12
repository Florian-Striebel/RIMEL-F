
























#include "test.h"
__FBSDID("$FreeBSD$");

static void
test_malformed1(void)
{
const char *refname = "test_read_format_7zip_malformed.7z";
struct archive *a;
struct archive_entry *ae;

extract_reference_file(refname);

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_open_filename(a, refname, 10240));
assertEqualIntA(a, ARCHIVE_FATAL, archive_read_next_header(a, &ae));
assertEqualIntA(a, ARCHIVE_OK, archive_read_free(a));
}

static void
test_malformed2(void)
{
const char *refname = "test_read_format_7zip_malformed2.7z";
struct archive *a;
struct archive_entry *ae;

extract_reference_file(refname);

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_open_filename(a, refname, 10240));
assertEqualIntA(a, ARCHIVE_FATAL, archive_read_next_header(a, &ae));
assertEqualIntA(a, ARCHIVE_OK, archive_read_free(a));
}

DEFINE_TEST(test_read_format_7zip_malformed)
{
test_malformed1();
test_malformed2();
}
