























#include "test.h"
__FBSDID("$FreeBSD$");

static char buff[1000000];
static char buff2[64];

DEFINE_TEST(test_write_format_tar)
{
struct archive_entry *ae;
struct archive *a;
char *p;
size_t used;
size_t blocksize;


for (blocksize = 1; blocksize < 100000; blocksize += blocksize + 3) {

assert((a = archive_write_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK,
archive_write_set_format_ustar(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_add_filter_none(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_set_bytes_per_block(a, (int)blocksize));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_set_bytes_in_last_block(a, (int)blocksize));
assertEqualInt(blocksize,
archive_write_get_bytes_in_last_block(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_open_memory(a, buff, sizeof(buff), &used));
assertEqualInt(blocksize,
archive_write_get_bytes_in_last_block(a));




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_mtime(ae, 1, 10);
assertEqualInt(1, archive_entry_mtime(ae));
assertEqualInt(10, archive_entry_mtime_nsec(ae));
p = strdup("file");
archive_entry_copy_pathname(ae, p);
strcpy(p, "XXXX");
free(p);
assertEqualString("file", archive_entry_pathname(ae));
archive_entry_set_mode(ae, S_IFREG | 0755);
assertEqualInt(S_IFREG | 0755, archive_entry_mode(ae));
archive_entry_set_size(ae, 8);

assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);
assertEqualInt(8, archive_write_data(a, "12345678", 9));


assertEqualIntA(a, ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));



failure("blocksize=%zu", blocksize);
assertEqualInt(((2048 - 1)/blocksize+1)*blocksize, used);




assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_memory(a, buff, used));

assertEqualIntA(a, ARCHIVE_OK,
archive_read_next_header(a, &ae));

assertEqualInt(1, archive_entry_mtime(ae));

assertEqualInt(0, archive_entry_mtime_nsec(ae));
assertEqualInt(0, archive_entry_atime(ae));
assertEqualInt(0, archive_entry_ctime(ae));
assertEqualString("file", archive_entry_pathname(ae));
assertEqualInt(AE_IFREG, archive_entry_filetype(ae));
assertEqualInt(AE_IFREG | 0755, archive_entry_mode(ae));
assertEqualInt(8, archive_entry_size(ae));
assertEqualInt(8, archive_read_data(a, buff2, 10));
assertEqualMem(buff2, "12345678", 8);


assertEqualIntA(a, ARCHIVE_EOF,
archive_read_next_header(a, &ae));
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}
}
