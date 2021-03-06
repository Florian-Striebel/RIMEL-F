

























#include "test.h"
__FBSDID("$FreeBSD$");

static char buff[4096];

DEFINE_TEST(test_write_format_mtree_absolute_path)
{
struct archive_entry *ae;
struct archive* a;
size_t used;


assert((a = archive_write_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_format_mtree(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_open_memory(a, buff, sizeof(buff)-1, &used));


assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, ".");
archive_entry_set_mode(ae, AE_IFDIR | 0755);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);


assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "/file");
archive_entry_set_size(ae, 0);
archive_entry_set_mode(ae, AE_IFREG | 0644);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);


assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "/dir");
archive_entry_set_mode(ae, AE_IFDIR | 0755);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);

assertEqualIntA(a, ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));




assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_open_memory(a, buff, used));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
failure("The path should be just \".\"");
assertEqualString(archive_entry_pathname(ae), ".");
assertEqualInt(archive_entry_mode(ae), AE_IFDIR | 0755);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
failure("The path should have \"./\" prefix");
assertEqualString(archive_entry_pathname(ae), "./file");
assertEqualInt(archive_entry_size(ae), 0);
assertEqualInt(archive_entry_mode(ae), AE_IFREG | 0644);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
failure("The path should have \"./\" prefix");
assertEqualString(archive_entry_pathname(ae), "./dir");
assertEqualInt(archive_entry_mode(ae), AE_IFDIR | 0755);

assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

