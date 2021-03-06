





























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_write_format_xar_empty)
{
struct archive *a;
struct archive_entry *ae;
char buff[256];
size_t used;


assert((a = archive_write_new()) != NULL);
if (archive_write_set_format_xar(a) != ARCHIVE_OK) {
skipping("xar is not supported on this platform");
assertEqualIntA(a, ARCHIVE_OK, archive_write_free(a));
return;
}
assertEqualIntA(a, ARCHIVE_OK, archive_write_add_filter_none(a));
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_bytes_per_block(a, 1));
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_bytes_in_last_block(a, 1));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_open_memory(a, buff, sizeof(buff), &used));


assert((ae = archive_entry_new()) != NULL);
archive_entry_set_atime(ae, 2, 0);
archive_entry_set_ctime(ae, 4, 0);
archive_entry_set_mtime(ae, 5, 0);
archive_entry_copy_pathname(ae, ".");
archive_entry_set_mode(ae, S_IFDIR | 0755);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);


assert((ae = archive_entry_new()) != NULL);
archive_entry_set_atime(ae, 2, 0);
archive_entry_set_ctime(ae, 4, 0);
archive_entry_set_mtime(ae, 5, 0);
archive_entry_copy_pathname(ae, "..");
archive_entry_set_mode(ae, S_IFDIR | 0755);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);


assert((ae = archive_entry_new()) != NULL);
archive_entry_set_atime(ae, 2, 0);
archive_entry_set_ctime(ae, 4, 0);
archive_entry_set_mtime(ae, 5, 0);
archive_entry_copy_pathname(ae, "/");
archive_entry_set_mode(ae, S_IFDIR | 0755);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);


assert((ae = archive_entry_new()) != NULL);
archive_entry_set_atime(ae, 2, 0);
archive_entry_set_ctime(ae, 4, 0);
archive_entry_set_mtime(ae, 5, 0);
archive_entry_copy_pathname(ae, "../");
archive_entry_set_mode(ae, S_IFDIR | 0755);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);


assert((ae = archive_entry_new()) != NULL);
archive_entry_set_atime(ae, 2, 0);
archive_entry_set_ctime(ae, 4, 0);
archive_entry_set_mtime(ae, 5, 0);
archive_entry_copy_pathname(ae, "../../.");
archive_entry_set_mode(ae, S_IFDIR | 0755);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);


assert((ae = archive_entry_new()) != NULL);
archive_entry_set_atime(ae, 2, 0);
archive_entry_set_ctime(ae, 4, 0);
archive_entry_set_mtime(ae, 5, 0);
archive_entry_copy_pathname(ae, "..//.././");
archive_entry_set_mode(ae, S_IFDIR | 0755);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);


assertEqualIntA(a, ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));


assertEqualInt(used, 0);
}
