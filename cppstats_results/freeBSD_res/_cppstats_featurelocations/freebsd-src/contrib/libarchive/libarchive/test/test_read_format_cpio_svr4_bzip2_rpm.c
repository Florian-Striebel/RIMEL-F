
























#include "test.h"


























































DEFINE_TEST(test_read_format_cpio_svr4_bzip2_rpm)
{
struct archive_entry *ae;
struct archive *a;
const char *name = "test_read_format_cpio_svr4_bzip2_rpm.rpm";
int r;

assert((a = archive_read_new()) != NULL);
r = archive_read_support_filter_bzip2(a);
if (r == ARCHIVE_WARN) {
skipping("bzip2 reading not fully supported on this platform");
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
return;
}
assertEqualIntA(a, ARCHIVE_OK, r);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_support_filter_rpm(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
extract_reference_file(name);
assertEqualIntA(a, ARCHIVE_OK, archive_read_open_filename(a, name, 2));

assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("./etc/file1", archive_entry_pathname(ae));
assertEqualInt(86401, archive_entry_mtime(ae));
assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("./etc/file2", archive_entry_pathname(ae));
assertEqualInt(86401, archive_entry_mtime(ae));
assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("./etc/file3", archive_entry_pathname(ae));
assertEqualInt(86401, archive_entry_mtime(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);


assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualInt(archive_filter_code(a, 0), ARCHIVE_FILTER_BZIP2);
assertEqualString(archive_filter_name(a, 0), "bzip2");
assertEqualInt(archive_format(a), ARCHIVE_FORMAT_CPIO_SVR4_NOCRC);

assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

