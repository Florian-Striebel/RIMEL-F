























#include "test.h"
__FBSDID("$FreeBSD");









DEFINE_TEST(test_compat_plexus_archiver_tar)
{
char name[] = "test_compat_plexus_archiver_tar.tar";
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
assertEqualString("commons-logging-1.2/NOTICE.txt",
archive_entry_pathname(ae));
assertEqualInt(1404583896, archive_entry_mtime(ae));
assertEqualInt(0100664, archive_entry_mode(ae));
assertEqualInt(0, archive_entry_uid(ae));
assertEqualInt(0, archive_entry_gid(ae));


assertEqualInt(archive_filter_code(a, 0), ARCHIVE_FILTER_NONE);
assertEqualInt(archive_format(a), ARCHIVE_FORMAT_TAR_USTAR);

assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}
