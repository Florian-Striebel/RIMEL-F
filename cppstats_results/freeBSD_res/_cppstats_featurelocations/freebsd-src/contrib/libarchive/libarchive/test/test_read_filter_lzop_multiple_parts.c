
























#include "test.h"

DEFINE_TEST(test_read_filter_lzop_multiple_parts)
{
const char *reference = "test_read_filter_lzop_multiple_parts.tar.lzo";
struct archive_entry *ae;
struct archive *a;
int r;

extract_reference_file(reference);
assert((a = archive_read_new()) != NULL);
r = archive_read_support_filter_lzop(a);
if (r != ARCHIVE_OK) {
if (!canLzop()) {
skipping("lzop compression is not supported "
"on this platform");
} else if (r == ARCHIVE_WARN) {
skipping("lzop multiple parts decoding is not "
"supported via external program");

} else
assertEqualIntA(a, ARCHIVE_OK, r);
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
return;
}
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, reference, 10240));

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


assertEqualInt(archive_filter_count(a), 2);
assertEqualInt(archive_filter_code(a, 0), ARCHIVE_FILTER_LZOP);
assertEqualString(archive_filter_name(a, 0), "lzop");
assertEqualInt(archive_format(a), ARCHIVE_FORMAT_TAR_USTAR);

assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}
