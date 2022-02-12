

























#include "test.h"
__FBSDID("$FreeBSD: head/lib/libarchive/test/test_write_compress_gzip.c 191183 2009-04-17 01:06:31Z kientzle $");







DEFINE_TEST(test_write_filter_gzip)
{
struct archive_entry *ae;
struct archive* a;
char *buff, *data;
size_t buffsize, datasize;
unsigned char *rbuff;
char path[16];
size_t used1, used2;
int i, r, use_prog = 0;

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
r = archive_write_add_filter_gzip(a);
if (r != ARCHIVE_OK) {
if (canGzip() && r == ARCHIVE_WARN)
use_prog = 1;
else {
skipping("gzip writing not supported on this platform");
assertEqualInt(ARCHIVE_OK, archive_write_free(a));
free(buff);
free(data);
return;
}
}
assertEqualIntA(a, ARCHIVE_OK,
archive_write_set_bytes_per_block(a, 10));
assertEqualInt(ARCHIVE_FILTER_GZIP, archive_filter_code(a, 0));
assertEqualString("gzip", archive_filter_name(a, 0));
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


rbuff = (unsigned char *)buff;
assertEqualInt(rbuff[0], 0x1f);
assertEqualInt(rbuff[1], 0x8b);
assertEqualInt(rbuff[2], 0x08);
assertEqualInt(rbuff[3], 0x00);
assertEqualInt(rbuff[8], 0);

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
r = archive_read_support_filter_gzip(a);
if (r == ARCHIVE_WARN) {
skipping("Can't verify gzip writing by reading back;"
" gzip reading not fully supported on this platform");
} else {
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
assertEqualIntA(a, (use_prog)?ARCHIVE_WARN:ARCHIVE_OK,
archive_write_add_filter_gzip(a));
assertEqualIntA(a, ARCHIVE_FAILED,
archive_write_set_options(a, "gzip:nonexistent-option=0"));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_set_options(a, "gzip:compression-level=1"));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_set_filter_option(a, NULL, "compression-level", "9"));
assertEqualIntA(a, ARCHIVE_FAILED,
archive_write_set_filter_option(a, NULL, "compression-level", "abc"));
assertEqualIntA(a, ARCHIVE_FAILED,
archive_write_set_filter_option(a, NULL, "compression-level", "99"));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_set_options(a, "gzip:compression-level=9"));
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


rbuff = (unsigned char *)buff;
assertEqualInt(rbuff[0], 0x1f);
assertEqualInt(rbuff[1], 0x8b);
assertEqualInt(rbuff[2], 0x08);
assertEqualInt(rbuff[3], 0x00);
assertEqualInt(rbuff[8], 2);









assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
r = archive_read_support_filter_gzip(a);
if (r != ARCHIVE_OK && !use_prog) {
skipping("gzip reading not fully supported on this platform");
} else {
assertEqualIntA(a, ARCHIVE_OK,
archive_read_support_filter_all(a));
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
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_format_ustar(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_set_bytes_per_block(a, 10));
assertEqualIntA(a, (use_prog)?ARCHIVE_WARN:ARCHIVE_OK,
archive_write_add_filter_gzip(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_set_filter_option(a, NULL, "compression-level", "1"));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_open_memory(a, buff, buffsize, &used2));
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


failure("Compression-level=1 wrote %d bytes; default wrote %d bytes",
(int)used2, (int)used1);
assert(used2 > used1);


rbuff = (unsigned char *)buff;
assertEqualInt(rbuff[0], 0x1f);
assertEqualInt(rbuff[1], 0x8b);
assertEqualInt(rbuff[2], 0x08);
assertEqualInt(rbuff[3], 0x00);
assertEqualInt(rbuff[8], 4);

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
r = archive_read_support_filter_gzip(a);
if (r == ARCHIVE_WARN) {
skipping("gzip reading not fully supported on this platform");
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
assertEqualIntA(a, (use_prog)?ARCHIVE_WARN:ARCHIVE_OK,
archive_write_add_filter_gzip(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));

assert((a = archive_write_new()) != NULL);
assertEqualIntA(a, (use_prog)?ARCHIVE_WARN:ARCHIVE_OK,
archive_write_add_filter_gzip(a));
assertEqualInt(ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));

assert((a = archive_write_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_format_ustar(a));
assertEqualIntA(a, (use_prog)?ARCHIVE_WARN:ARCHIVE_OK,
archive_write_add_filter_gzip(a));
assertEqualInt(ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));

assert((a = archive_write_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_format_ustar(a));
assertEqualIntA(a, (use_prog)?ARCHIVE_WARN:ARCHIVE_OK,
archive_write_add_filter_gzip(a));
assertEqualIntA(a, ARCHIVE_OK, archive_write_open_memory(a, buff, buffsize, &used2));
assertEqualInt(ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));




free(data);
free(buff);
}
