

























#include "test.h"





DEFINE_TEST(test_write_filter_lrzip)
{
struct archive_entry *ae;
struct archive* a;
char *buff, *data;
size_t buffsize, datasize;
char path[16];
size_t used1, used2;
int i;

if (!canLrzip()) {
skipping("lrzip command-line program not found");
return;
}

buffsize = 2000000;
assert(NULL != (buff = (char *)malloc(buffsize)));

datasize = 10000;
assert(NULL != (data = (char *)malloc(datasize)));
memset(data, 0, datasize);




assert((a = archive_write_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_format_gnutar(a));
assertEqualIntA(a, ARCHIVE_WARN, archive_write_add_filter_lrzip(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_set_bytes_per_block(a, 10));
assertEqualInt(ARCHIVE_FILTER_LRZIP, archive_filter_code(a, 0));
assertEqualString("lrzip", archive_filter_name(a, 0));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_open_memory(a, buff, buffsize, &used1));
assert((ae = archive_entry_new()) != NULL);
archive_entry_set_filetype(ae, AE_IFREG);
archive_entry_set_size(ae, datasize);
for (i = 0; i < 100; i++) {
sprintf(path, "file%03d", i);
archive_entry_copy_pathname(ae, path);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
assertA(datasize
== (size_t)archive_write_data(a, data, datasize));
}
archive_entry_free(ae);
assertEqualIntA(a, ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_WARN, archive_read_support_filter_lrzip(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_memory(a, buff, used1));
for (i = 0; i < 100; i++) {
sprintf(path, "file%03d", i);
if (!assertEqualInt(ARCHIVE_OK,
archive_read_next_header(a, &ae)))
break;
assertEqualString(path, archive_entry_pathname(ae));
assertEqualInt((int)datasize, archive_entry_size(ae));
}
assertEqualInt(ARCHIVE_FILTER_LRZIP, archive_filter_code(a, 0));
assertEqualString("lrzip", archive_filter_name(a, 0));
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));





assert((a = archive_write_new()) != NULL);
assertEqualIntA(a, ARCHIVE_WARN, archive_write_add_filter_lrzip(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));

assert((a = archive_write_new()) != NULL);
assertEqualIntA(a, ARCHIVE_WARN, archive_write_add_filter_lrzip(a));
assertEqualInt(ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));

assert((a = archive_write_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_format_ustar(a));
assertEqualIntA(a, ARCHIVE_WARN, archive_write_add_filter_lrzip(a));
assertEqualInt(ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));

assert((a = archive_write_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_format_ustar(a));
assertEqualIntA(a, ARCHIVE_WARN, archive_write_add_filter_lrzip(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_open_memory(a, buff, buffsize, &used2));
assertEqualInt(ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));




free(data);
free(buff);
}
