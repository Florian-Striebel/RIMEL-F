
























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_write_format_warc_empty)
{
struct archive *a;
struct archive_entry *ae;
char buff[512U];
size_t used;


assert((a = archive_write_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_format_warc(a));
assertEqualIntA(a, ARCHIVE_OK, archive_write_add_filter_none(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_open_memory(a, buff, sizeof(buff), &used));


assert((ae = archive_entry_new()) != NULL);
archive_entry_set_atime(ae, 2, 0);
archive_entry_set_ctime(ae, 4, 0);
archive_entry_set_mtime(ae, 5, 0);
archive_entry_copy_pathname(ae, ".");
archive_entry_set_mode(ae, S_IFDIR | 0755);
assertEqualIntA(a, ARCHIVE_FAILED, archive_write_header(a, ae));
archive_entry_free(ae);


assert((ae = archive_entry_new()) != NULL);
archive_entry_set_atime(ae, 2, 0);
archive_entry_set_ctime(ae, 4, 0);
archive_entry_set_mtime(ae, 5, 0);
archive_entry_copy_pathname(ae, "..");
archive_entry_set_mode(ae, S_IFDIR | 0755);
assertEqualIntA(a, ARCHIVE_FAILED, archive_write_header(a, ae));
archive_entry_free(ae);


assert((ae = archive_entry_new()) != NULL);
archive_entry_set_atime(ae, 2, 0);
archive_entry_set_ctime(ae, 4, 0);
archive_entry_set_mtime(ae, 5, 0);
archive_entry_copy_pathname(ae, "/");
archive_entry_set_mode(ae, S_IFDIR | 0755);
assertEqualIntA(a, ARCHIVE_FAILED, archive_write_header(a, ae));
archive_entry_free(ae);


assert((ae = archive_entry_new()) != NULL);
archive_entry_set_atime(ae, 2, 0);
archive_entry_set_ctime(ae, 4, 0);
archive_entry_set_mtime(ae, 5, 0);
archive_entry_copy_pathname(ae, "../");
archive_entry_set_mode(ae, S_IFDIR | 0755);
assertEqualIntA(a, ARCHIVE_FAILED, archive_write_header(a, ae));
archive_entry_free(ae);


assert((ae = archive_entry_new()) != NULL);
archive_entry_set_atime(ae, 2, 0);
archive_entry_set_ctime(ae, 4, 0);
archive_entry_set_mtime(ae, 5, 0);
archive_entry_copy_pathname(ae, "../../.");
archive_entry_set_mode(ae, S_IFDIR | 0755);
assertEqualIntA(a, ARCHIVE_FAILED, archive_write_header(a, ae));
archive_entry_free(ae);


assert((ae = archive_entry_new()) != NULL);
archive_entry_set_atime(ae, 2, 0);
archive_entry_set_ctime(ae, 4, 0);
archive_entry_set_mtime(ae, 5, 0);
archive_entry_copy_pathname(ae, "..//.././");
archive_entry_set_mode(ae, S_IFDIR | 0755);
assertEqualIntA(a, ARCHIVE_FAILED, archive_write_header(a, ae));
archive_entry_free(ae);


assertEqualIntA(a, ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));


assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_open_memory(a, buff, used));


assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}
