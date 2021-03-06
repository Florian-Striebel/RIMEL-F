























#include "test.h"
__FBSDID("$FreeBSD");





DEFINE_TEST(test_compat_perl_archive_tar)
{
char name[] = "test_compat_perl_archive_tar.tar";
struct archive_entry *ae;
struct archive *a;
int r;

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
extract_reference_file(name);
assertEqualIntA(a, ARCHIVE_OK, archive_read_open_filename(a, name,
10240));


assertEqualIntA(a, ARCHIVE_OK, r = archive_read_next_header(a, &ae));
if (r != ARCHIVE_OK) {
archive_read_free(a);
return;
}
assertEqualString("file1", archive_entry_pathname(ae));
assertEqualInt(1480603099, archive_entry_mtime(ae));
assertEqualInt(1000, archive_entry_uid(ae));
assertEqualString("john", archive_entry_uname(ae));
assertEqualInt(1000, archive_entry_gid(ae));
assertEqualString("john", archive_entry_gname(ae));
assertEqualInt(0100644, archive_entry_mode(ae));


assertEqualInt(archive_filter_code(a, 0), ARCHIVE_FILTER_NONE);
assertEqualInt(archive_format(a), ARCHIVE_FORMAT_TAR_USTAR);

assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}
