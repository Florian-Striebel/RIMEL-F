























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_read_format_rar_invalid1)
{
const char *refname = "test_read_format_rar_invalid1.rar";
struct archive *a;
struct archive_entry *ae;
char *buff[100];

extract_reference_file(refname);
assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_open_filename(a, refname, 10240));
assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualIntA(a, ARCHIVE_FATAL, archive_read_data(a, buff, 99));
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}
