























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_empty_write)
{
char buff[32768];
struct archive_entry *ae;
struct archive *a;
size_t used;
int r;






assert((a = archive_write_new()) != NULL);
assertA(0 == archive_write_set_format_ustar(a));
r = archive_write_add_filter_gzip(a);
if (r != ARCHIVE_OK && !canGzip()) {
skipping("Empty write to gzip-compressed archive");
} else {
if (r != ARCHIVE_OK && canGzip())
assertEqualIntA(a, ARCHIVE_WARN, r);
else
assertEqualIntA(a, ARCHIVE_OK, r);
assertEqualIntA(a, ARCHIVE_OK,
archive_write_open_memory(a, buff, sizeof(buff), &used));

assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "file");
archive_entry_set_mode(ae, S_IFREG | 0755);
archive_entry_set_size(ae, 0);
assertA(0 == archive_write_header(a, ae));
archive_entry_free(ae);



assertEqualIntA(a, 0, archive_write_data(a, "", 0));


assertEqualIntA(a, ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));
}






assert((a = archive_write_new()) != NULL);
assertA(0 == archive_write_set_format_ustar(a));
r = archive_write_add_filter_bzip2(a);
if (r != ARCHIVE_OK && !canBzip2()) {
skipping("Empty write to bzip2-compressed archive");
} else {
if (r != ARCHIVE_OK && canBzip2())
assertEqualIntA(a, ARCHIVE_WARN, r);
else
assertEqualIntA(a, ARCHIVE_OK, r);
assertEqualIntA(a, ARCHIVE_OK,
archive_write_open_memory(a, buff, sizeof(buff), &used));

assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "file");
archive_entry_set_mode(ae, S_IFREG | 0755);
archive_entry_set_size(ae, 0);
assertA(0 == archive_write_header(a, ae));
archive_entry_free(ae);


assertEqualIntA(a, 0, archive_write_data(a, "", 0));


assertEqualIntA(a, ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));
}






assert((a = archive_write_new()) != NULL);
assertA(0 == archive_write_set_format_ustar(a));
assertA(0 == archive_write_add_filter_none(a));
assertA(0 == archive_write_open_memory(a, buff, sizeof(buff), &used));

assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "file");
archive_entry_set_mode(ae, S_IFREG | 0755);
archive_entry_set_size(ae, 0);
assertA(0 == archive_write_header(a, ae));
archive_entry_free(ae);


assertEqualIntA(a, 0, archive_write_data(a, "", 0));


assertEqualIntA(a, ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));
}
