
























#include "test.h"
__FBSDID("$FreeBSD: head/lib/libarchive/test/test_write_compress_program.c 201247 2009-12-30 05:59:21Z kientzle $");

static char buff[1000000];
static char buff2[64];

DEFINE_TEST(test_write_filter_program)
{
struct archive_entry *ae;
struct archive *a;
size_t used;
int blocksize = 1024;
int r;

if (!canGzip()) {
skipping("Cannot run 'gzip'");
return;
}




assert((a = archive_read_new()) != NULL);
r = archive_read_support_filter_gzip(a);
if (r != ARCHIVE_OK && canGzip())
blocksize = 1;
assertEqualInt(ARCHIVE_OK, archive_read_free(a));



assert((a = archive_write_new()) != NULL);
assertA(0 == archive_write_set_format_ustar(a));
r = archive_write_add_filter_program(a, "gzip -6");
if (r == ARCHIVE_FATAL) {
skipping("Write compression via external "
"program unsupported on this platform");
archive_write_free(a);
return;
}
assertA(0 == archive_write_set_bytes_per_block(a, blocksize));
assertA(0 == archive_write_set_bytes_in_last_block(a, blocksize));
assertA(blocksize == archive_write_get_bytes_in_last_block(a));
assertA(0 == archive_write_open_memory(a, buff, sizeof(buff), &used));
assertA(blocksize == archive_write_get_bytes_in_last_block(a));




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_mtime(ae, 1, 10);
archive_entry_copy_pathname(ae, "file");
archive_entry_set_mode(ae, S_IFREG | 0755);
archive_entry_set_size(ae, 8);

assertA(0 == archive_write_header(a, ae));
archive_entry_free(ae);
assertA(8 == archive_write_data(a, "12345678", 9));


assertEqualIntA(a, ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));




assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
r = archive_read_support_filter_gzip(a);



if (r != ARCHIVE_OK && !canGzip()) {
skipping("No libz and no gunzip program, "
"unable to verify gzip compression");
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
return;
}
assertEqualIntA(a, ARCHIVE_OK, archive_read_open_memory(a, buff, used));

if (!assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae))) {
archive_read_free(a);
return;
}

assertEqualInt(1, archive_entry_mtime(ae));
assertEqualInt(0, archive_entry_atime(ae));
assertEqualInt(0, archive_entry_ctime(ae));
assertEqualString("file", archive_entry_pathname(ae));
assertEqualInt((S_IFREG | 0755), archive_entry_mode(ae));
assertEqualInt(8, archive_entry_size(ae));
assertEqualIntA(a, 8, archive_read_data(a, buff2, 10));
assertEqualMem(buff2, "12345678", 8);


assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}
