























#include "test.h"

static void
test_format(int (*set_format)(struct archive *))
{
char filedata[64];
struct archive_entry *ae;
struct archive *a;
size_t used;
size_t buffsize = 1000000;
char *buff;

buff = malloc(buffsize);


assert((a = archive_write_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, (*set_format)(a));
assertEqualIntA(a, ARCHIVE_OK, archive_write_add_filter_b64encode(a));
assertEqualIntA(a, ARCHIVE_OK, archive_write_open_memory(a, buff, buffsize, &used));




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_pathname(ae, "test");
archive_entry_set_filetype(ae, AE_IFREG);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);
assertEqualIntA(a, 9, archive_write_data(a, "12345678", 9));
assertEqualIntA(a, ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));




assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_raw(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_none(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_open_memory(a, buff, used));

assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualIntA(a, 37, archive_read_data(a, filedata, 64));
assertEqualMem(filedata, "begin-base64 644 -\nMTIzNDU2NzgA\n====\n", 37);
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));

free(buff);
}

DEFINE_TEST(test_write_format_raw_b64)
{
test_format(archive_write_set_format_raw);
}
