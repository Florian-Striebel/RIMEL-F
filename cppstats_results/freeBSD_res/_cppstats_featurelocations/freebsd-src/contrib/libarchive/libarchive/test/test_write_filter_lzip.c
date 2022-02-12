


























#include "test.h"
__FBSDID("$FreeBSD$");






DEFINE_TEST(test_write_filter_lzip)
{
struct archive_entry *ae;
struct archive* a;
char *buff, *data;
size_t buffsize, datasize;
char path[16];
size_t used1, used2;
int i, r;

buffsize = 2000000;
assert(NULL != (buff = (char *)malloc(buffsize)));
if (buff == NULL)
return;

datasize = 10000;
assert(NULL != (data = (char *)malloc(datasize)));
if (data == NULL) {
free(buff);
return;
}
memset(data, 0, datasize);




assert((a = archive_write_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_format_ustar(a));
r = archive_write_add_filter_lzip(a);
if (r == ARCHIVE_FATAL) {
skipping("lzip writing not supported on this platform");
assertEqualInt(ARCHIVE_OK, archive_write_free(a));
free(buff);
free(data);
return;
}
assertEqualIntA(a, ARCHIVE_OK,
archive_write_set_bytes_per_block(a, 10));
assertEqualInt(ARCHIVE_FILTER_LZIP, archive_filter_code(a, 0));
assertEqualString("lzip", archive_filter_name(a, 0));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_open_memory(a, buff, buffsize, &used1));
assertEqualInt(ARCHIVE_FILTER_LZIP, archive_filter_code(a, 0));
assertEqualString("lzip", archive_filter_name(a, 0));
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
r = archive_read_support_filter_lzip(a);
if (r == ARCHIVE_WARN) {
skipping("Can't verify lzip writing by reading back;"
" lzip reading not fully supported on this platform");
} else {
assertEqualIntA(a, ARCHIVE_OK,
archive_read_support_filter_all(a));
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
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
}
assertEqualInt(ARCHIVE_OK, archive_read_free(a));





assert((a = archive_write_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_format_ustar(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_set_bytes_per_block(a, 10));
assertEqualIntA(a, ARCHIVE_OK, archive_write_add_filter_lzip(a));
assertEqualIntA(a, ARCHIVE_FAILED,
archive_write_set_filter_option(a, NULL, "nonexistent-option", "0"));
assertEqualIntA(a, ARCHIVE_FAILED,
archive_write_set_filter_option(a, NULL, "compression-level", "abc"));
assertEqualIntA(a, ARCHIVE_FAILED,
archive_write_set_filter_option(a, NULL, "compression-level", "99"));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_set_filter_option(a, NULL, "compression-level", "9"));
assertEqualIntA(a, ARCHIVE_OK, archive_write_open_memory(a, buff, buffsize, &used2));
for (i = 0; i < 100; i++) {
sprintf(path, "file%03d", i);
assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, path);
archive_entry_set_size(ae, datasize);
archive_entry_set_filetype(ae, AE_IFREG);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
assertA(datasize == (size_t)archive_write_data(a, data, datasize));
archive_entry_free(ae);
}
assertEqualIntA(a, ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));


assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
r = archive_read_support_filter_lzip(a);
if (r == ARCHIVE_WARN) {
skipping("lzip reading not fully supported on this platform");
} else {
assertEqualIntA(a, ARCHIVE_OK,
archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_memory(a, buff, used2));
for (i = 0; i < 100; i++) {
sprintf(path, "file%03d", i);
failure("Trying to read %s", path);
if (!assertEqualIntA(a, ARCHIVE_OK,
archive_read_next_header(a, &ae)))
break;
assertEqualString(path, archive_entry_pathname(ae));
assertEqualInt((int)datasize, archive_entry_size(ae));
}
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
}
assertEqualInt(ARCHIVE_OK, archive_read_free(a));




assert((a = archive_write_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_format_ustar(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_set_bytes_per_block(a, 10));
assertEqualIntA(a, ARCHIVE_OK, archive_write_add_filter_lzip(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_set_filter_option(a, NULL, "compression-level", "0"));
assertEqualIntA(a, ARCHIVE_OK, archive_write_open_memory(a, buff, buffsize, &used2));
for (i = 0; i < 100; i++) {
sprintf(path, "file%03d", i);
assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, path);
archive_entry_set_size(ae, datasize);
archive_entry_set_filetype(ae, AE_IFREG);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
failure("Writing file %s", path);
assertEqualIntA(a, datasize,
(size_t)archive_write_data(a, data, datasize));
archive_entry_free(ae);
}
assertEqualIntA(a, ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));


failure("Compression-level=0 wrote %d bytes; default wrote %d bytes",
(int)used2, (int)used1);
assert(used2 > used1);

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
r = archive_read_support_filter_lzip(a);
if (r == ARCHIVE_WARN) {
skipping("lzip reading not fully supported on this platform");
} else {
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_memory(a, buff, used2));
for (i = 0; i < 100; i++) {
sprintf(path, "file%03d", i);
if (!assertEqualInt(ARCHIVE_OK,
archive_read_next_header(a, &ae)))
break;
assertEqualString(path, archive_entry_pathname(ae));
assertEqualInt((int)datasize, archive_entry_size(ae));
}
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
}
assertEqualInt(ARCHIVE_OK, archive_read_free(a));





assert((a = archive_write_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_write_add_filter_lzip(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));

assert((a = archive_write_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_write_add_filter_lzip(a));
assertEqualInt(ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));

assert((a = archive_write_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_format_ustar(a));
assertEqualIntA(a, ARCHIVE_OK, archive_write_add_filter_lzip(a));
assertEqualInt(ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));

assert((a = archive_write_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_format_ustar(a));
assertEqualIntA(a, ARCHIVE_OK, archive_write_add_filter_lzip(a));
assertEqualIntA(a, ARCHIVE_OK, archive_write_open_memory(a, buff, buffsize, &used2));
assertEqualInt(ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));




free(data);
free(buff);
}
