
























#include "test.h"
__FBSDID("$FreeBSD$");

static void test_read(struct archive *a, char *buff, size_t used, char *filedata)
{
struct archive_entry *ae;
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_none(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_open_memory(a, buff, used));

assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualIntA(a, 9, archive_read_data(a, filedata, 10));
assertEqualMem(filedata, "12345678", 9);
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

DEFINE_TEST(test_write_format_warc)
{
char filedata[64];
struct archive *a;
struct archive_entry *ae;
const size_t bsiz = 2048U;
char *buff;
size_t used;

buff = malloc(bsiz);


assert((a = archive_write_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_format_warc(a));
assertEqualIntA(a, ARCHIVE_OK, archive_write_add_filter_none(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_open_memory(a, buff, bsiz, &used));




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_pathname(ae, "test");
archive_entry_set_filetype(ae, AE_IFREG);
archive_entry_set_size(ae, 9);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);
assertEqualIntA(a, 9, archive_write_data(a, "12345678", 9));
assertEqualIntA(a, ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));




assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_warc(a));
test_read(a, buff, used, filedata);

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_by_code(a, ARCHIVE_FORMAT_WARC));
test_read(a, buff, used, filedata);

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_set_format(a, ARCHIVE_FORMAT_WARC));
test_read(a, buff, used, filedata);


assert((a = archive_write_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_format_warc(a));
assertEqualIntA(a, ARCHIVE_OK, archive_write_add_filter_none(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_open_memory(a, buff, bsiz, &used));


assert((ae = archive_entry_new()) != NULL);
archive_entry_set_pathname(ae, "test");
archive_entry_set_filetype(ae, AE_IFREG);
archive_entry_set_size(ae, 9);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);
assertEqualIntA(a, 9, archive_write_data(a, "12345678", 9));


assert((ae = archive_entry_new()) != NULL);
archive_entry_set_pathname(ae, "test2");
archive_entry_set_filetype(ae, AE_IFREG);
archive_entry_set_size(ae, 9);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);
assertEqualIntA(a, 9, archive_write_data(a, "12345678", 9));

assertEqualIntA(a, ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));


assert((a = archive_write_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_format_warc(a));
assertEqualIntA(a, ARCHIVE_OK, archive_write_add_filter_none(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_open_memory(a, buff, bsiz, &used));


assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "dir");
archive_entry_set_filetype(ae, AE_IFDIR);
archive_entry_set_size(ae, 512);
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

free(buff);
}
