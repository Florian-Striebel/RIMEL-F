
























#include "test.h"
__FBSDID("$FreeBSD$");

static void
test_malformed1(void)
{
const char *refname = "test_read_format_zip_malformed1.zip";
struct archive *a;
struct archive_entry *ae;
char *p;
size_t s;

extract_reference_file(refname);


assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_open_filename(a, refname, 10240));
assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualIntA(a, ARCHIVE_OK, archive_read_free(a));


p = slurpfile(&s, "%s", refname);
assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, read_open_memory(a, p, s, 31));
assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualIntA(a, ARCHIVE_OK, archive_read_free(a));
free(p);
}

DEFINE_TEST(test_read_format_zip_malformed)
{
test_malformed1();
}
