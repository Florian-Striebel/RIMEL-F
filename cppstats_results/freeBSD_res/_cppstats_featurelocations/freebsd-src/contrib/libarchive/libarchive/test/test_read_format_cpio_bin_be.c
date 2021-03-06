























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_read_format_cpio_bin_be)
{
struct archive_entry *ae;
struct archive *a;
const char *reference = "test_read_format_cpio_bin_be.cpio";

extract_reference_file(reference);
assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, reference, 10));
assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString(archive_entry_pathname(ae), "file1111222233334444");
assertEqualInt(archive_entry_size(ae), 5);
assertEqualInt(archive_entry_mtime(ae), 1240664175);
assertEqualInt(archive_entry_mode(ae), AE_IFREG | 0644);
assertEqualInt(archive_entry_uid(ae), 1000);
assertEqualInt(archive_entry_gid(ae), 0);
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);

assertEqualInt(archive_filter_code(a, 0), ARCHIVE_FILTER_NONE);
assertEqualInt(archive_format(a), ARCHIVE_FORMAT_CPIO_BIN_BE);
assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}


