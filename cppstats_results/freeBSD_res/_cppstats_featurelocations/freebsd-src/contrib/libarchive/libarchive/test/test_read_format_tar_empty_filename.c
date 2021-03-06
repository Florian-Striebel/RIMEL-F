























#include "test.h"
__FBSDID("$FreeBSD$");




DEFINE_TEST(test_read_format_tar_empty_filename)
{
char name[] = "test_read_format_tar_empty_filename.tar";
struct archive_entry *ae;
struct archive *a;

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
extract_reference_file(name);
assertEqualIntA(a, ARCHIVE_OK, archive_read_open_filename(a, name, 10240));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("", archive_entry_pathname(ae));
assertEqualInt(1208628157, archive_entry_mtime(ae));
assertEqualInt(1000, archive_entry_uid(ae));
assertEqualString("tim", archive_entry_uname(ae));
assertEqualInt(0, archive_entry_gid(ae));
assertEqualString("wheel", archive_entry_gname(ae));
assertEqualInt(040775, archive_entry_mode(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);


assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualInt(archive_filter_code(a, 0), ARCHIVE_FILTER_NONE);
assertEqualInt(archive_format(a), ARCHIVE_FORMAT_TAR_USTAR);

assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}
