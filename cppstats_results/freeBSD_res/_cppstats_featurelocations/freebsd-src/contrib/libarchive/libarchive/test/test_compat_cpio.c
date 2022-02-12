























#include "test.h"
__FBSDID("$FreeBSD$");
















static void
test_compat_cpio_1(void)
{
char name[] = "test_compat_cpio_1.cpio";
struct archive_entry *ae;
struct archive *a;

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
extract_reference_file(name);
assertEqualIntA(a, ARCHIVE_OK, archive_read_open_filename(a, name, 17));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("foo1", archive_entry_pathname(ae));
assertEqualString(NULL, archive_entry_hardlink(ae));
assertEqualInt(1260250228, archive_entry_mtime(ae));
assertEqualInt(1000, archive_entry_uid(ae));
assertEqualInt(1000, archive_entry_gid(ae));
assertEqualInt(0100644, archive_entry_mode(ae));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("foo2", archive_entry_pathname(ae));
assertEqualString(NULL, archive_entry_hardlink(ae));
assertEqualInt(1260250228, archive_entry_mtime(ae));
assertEqualInt(1000, archive_entry_uid(ae));
assertEqualInt(1000, archive_entry_gid(ae));
assertEqualInt(0100644, archive_entry_mode(ae));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("bar1", archive_entry_pathname(ae));
assertEqualString(NULL, archive_entry_hardlink(ae));
assertEqualInt(1260250228, archive_entry_mtime(ae));
assertEqualInt(1000, archive_entry_uid(ae));
assertEqualInt(1000, archive_entry_gid(ae));
assertEqualInt(0100644, archive_entry_mode(ae));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("bar2", archive_entry_pathname(ae));
assertEqualString("bar1", archive_entry_hardlink(ae));
assertEqualInt(1260250228, archive_entry_mtime(ae));
assertEqualInt(1000, archive_entry_uid(ae));
assertEqualInt(1000, archive_entry_gid(ae));
assertEqualInt(0100644, archive_entry_mode(ae));


assertEqualInt(archive_filter_code(a, 0), ARCHIVE_FILTER_NONE);
assertEqualInt(archive_format(a), ARCHIVE_FORMAT_CPIO_SVR4_NOCRC);

assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}


DEFINE_TEST(test_compat_cpio)
{
test_compat_cpio_1();
}


