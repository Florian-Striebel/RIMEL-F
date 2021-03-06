
























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_compat_uudecode_large)
{
const char *refname = "test_compat_uudecode_large.tar.Z.uu";
struct archive_entry *ae;
struct archive *a;

copy_reference_file(refname);
assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, refname, 2));

assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("file1", archive_entry_pathname(ae));
assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("file2", archive_entry_pathname(ae));
assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("file3", archive_entry_pathname(ae));
assertEqualInt(archive_filter_code(a, 0), ARCHIVE_FILTER_COMPRESS);
assertEqualInt(archive_filter_code(a, 1), ARCHIVE_FILTER_UU);
assertEqualInt(archive_format(a), ARCHIVE_FORMAT_TAR_USTAR);
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

