























#include "test.h"
__FBSDID("$FreeBSD$");



















static void
test_compat_tar_hardlink_1(void)
{
char name[] = "test_compat_tar_hardlink_1.tar";
struct archive_entry *ae;
struct archive *a;

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
extract_reference_file(name);
assertEqualIntA(a, ARCHIVE_OK, archive_read_open_filename(a, name, 10240));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("xmcd-3.3.2/docs_d/READMf",
archive_entry_pathname(ae));
assertEqualString(NULL, archive_entry_hardlink(ae));
assertEqualInt(321, archive_entry_size(ae));
assertEqualInt(1082575645, archive_entry_mtime(ae));
assertEqualInt(1851, archive_entry_uid(ae));
assertEqualInt(3, archive_entry_gid(ae));
assertEqualInt(0100444, archive_entry_mode(ae));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("xmcd-3.3.2/README",
archive_entry_pathname(ae));
assertEqualString(
"xmcd-3.3.2/docs_d/READMf",
archive_entry_hardlink(ae));
assertEqualInt(0, archive_entry_size(ae));
assertEqualInt(1082575645, archive_entry_mtime(ae));
assertEqualInt(1851, archive_entry_uid(ae));
assertEqualInt(3, archive_entry_gid(ae));
assertEqualInt(0100444, archive_entry_mode(ae));







assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualInt(archive_filter_code(a, 0), ARCHIVE_FILTER_NONE);
assertEqualInt(archive_format(a), ARCHIVE_FORMAT_TAR);

assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

DEFINE_TEST(test_compat_tar_hardlink)
{
test_compat_tar_hardlink_1();
}


