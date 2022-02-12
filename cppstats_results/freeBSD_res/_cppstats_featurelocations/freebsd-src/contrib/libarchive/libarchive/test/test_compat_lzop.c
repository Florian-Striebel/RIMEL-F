
























#include "test.h"
__FBSDID("$FreeBSD$");










DEFINE_TEST(test_compat_lzop)
{
const char *sample1 = "test_compat_lzop_1.tar.lzo";
const char *sample2 = "test_compat_lzop_2.tar.lzo";
const char *sample3 = "test_compat_lzop_3.tar.lzo";
struct archive_entry *ae;
struct archive *a;
int r;




assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
r = archive_read_support_filter_lzop(a);
if (r == ARCHIVE_WARN) {
skipping("lzop reading not fully supported on this platform");
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
return;
}
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
extract_reference_file(sample1);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, sample1, 2));

assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("INSTALL", archive_entry_pathname(ae));


assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualInt(archive_filter_code(a, 0), ARCHIVE_FILTER_LZOP);
assertEqualString(archive_filter_name(a, 0), "lzop");
assertEqualInt(archive_format(a), ARCHIVE_FORMAT_TAR_USTAR);

assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));




assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
extract_reference_file(sample2);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, sample2, 2));

assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("file1", archive_entry_pathname(ae));
assertEqualInt(19, archive_entry_size(ae));
assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("file2", archive_entry_pathname(ae));
assertEqualInt(262144, archive_entry_size(ae));
assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("file3", archive_entry_pathname(ae));
assertEqualInt(19, archive_entry_size(ae));


assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualInt(archive_filter_code(a, 0), ARCHIVE_FILTER_LZOP);
assertEqualString(archive_filter_name(a, 0), "lzop");
assertEqualInt(archive_format(a), ARCHIVE_FORMAT_TAR_USTAR);

assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));




assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
extract_reference_file(sample3);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, sample3, 2));

assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("INSTALL", archive_entry_pathname(ae));


assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualInt(archive_filter_code(a, 0), ARCHIVE_FILTER_LZOP);
assertEqualString(archive_filter_name(a, 0), "lzop");
assertEqualInt(archive_format(a), ARCHIVE_FORMAT_TAR_USTAR);

assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}
