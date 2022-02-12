























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_read_pax_truncated)
{
struct archive_entry *ae;
struct archive *a;
size_t used, i, buff_size = 1000000;
size_t filedata_size = 100000;
char *buff = malloc(buff_size);
char *buff2 = malloc(buff_size);
char *filedata = malloc(filedata_size);


assert((a = archive_write_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_format_pax(a));
assertEqualIntA(a, ARCHIVE_OK, archive_write_add_filter_none(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_open_memory(a, buff, buff_size, &used));




assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "file");
archive_entry_set_mode(ae, S_IFREG | 0755);
for (i = 0; i < filedata_size; i++)
filedata[i] = (unsigned char)rand();
archive_entry_set_atime(ae, 1, 2);
archive_entry_set_ctime(ae, 3, 4);
archive_entry_set_mtime(ae, 5, 6);
archive_entry_set_size(ae, filedata_size);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);
assertEqualIntA(a, (int)filedata_size,
(int)archive_write_data(a, filedata, filedata_size));


assertEqualIntA(a, ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));



for (i = 1; i < used + 100; i += 100) {
assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));

if (i < 512) {
assertEqualIntA(a, ARCHIVE_FATAL, read_open_memory_minimal(a, buff, i, 13));
goto wrap_up;
} else {
assertEqualIntA(a, ARCHIVE_OK, read_open_memory_minimal(a, buff, i, 13));
}


if (i < 1536) {
assertEqualIntA(a, ARCHIVE_FATAL, archive_read_next_header(a, &ae));
goto wrap_up;
} else {
failure("Archive truncated to %zu bytes", i);
assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
}


if (i < 1536 + filedata_size) {
assertEqualIntA(a, ARCHIVE_FATAL, archive_read_data(a, filedata, filedata_size));
goto wrap_up;
} else {
failure("Archive truncated to %zu bytes", i);
assertEqualIntA(a, filedata_size,
archive_read_data(a, filedata, filedata_size));
}







if (i < 1536 + 512*((filedata_size + 511)/512) + 512) {
failure("i=%zu minsize=%zu", i,
1536 + 512*((filedata_size + 511)/512) + 512);
assertEqualIntA(a, ARCHIVE_FATAL,
archive_read_next_header(a, &ae));
} else {
assertEqualIntA(a, ARCHIVE_EOF,
archive_read_next_header(a, &ae));
}
wrap_up:
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}




for (i = 1; i < used + 100; i += 100) {
assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));

if (i < 512) {
assertEqualIntA(a, ARCHIVE_FATAL, read_open_memory(a, buff, i, 7));
goto wrap_up2;
} else {
assertEqualIntA(a, ARCHIVE_OK, read_open_memory(a, buff, i, 7));
}

if (i < 1536) {
assertEqualIntA(a, ARCHIVE_FATAL, archive_read_next_header(a, &ae));
goto wrap_up2;
} else {
assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
}

if (i < 1536 + 512*((filedata_size+511)/512)) {
assertEqualIntA(a, ARCHIVE_FATAL, archive_read_data_skip(a));
goto wrap_up2;
} else {
assertEqualIntA(a, ARCHIVE_OK, archive_read_data_skip(a));
}







if (i < 1536 + 512*((filedata_size + 511)/512) + 512) {
assertEqualIntA(a, ARCHIVE_FATAL,
archive_read_next_header(a, &ae));
} else {
assertEqualIntA(a, ARCHIVE_EOF,
archive_read_next_header(a, &ae));
}
wrap_up2:
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}




memcpy(buff2, buff, buff_size);
buff2[512] = '9';
buff2[513] = '9';
buff2[514] = 'A';
assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_open_memory(a, buff2, used));
assertEqualIntA(a, ARCHIVE_WARN, archive_read_next_header(a, &ae));
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));


memcpy(buff2, buff, buff_size);
buff2[512] = 'A';
assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_open_memory(a, buff2, used));
assertEqualIntA(a, ARCHIVE_WARN, archive_read_next_header(a, &ae));
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));


memcpy(buff2, buff, buff_size);
for (i = 512; i < 520; i++)
buff2[i] = '9';
buff2[i] = ' ';
assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_open_memory(a, buff2, used));
assertEqualIntA(a, ARCHIVE_WARN, archive_read_next_header(a, &ae));
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));


memcpy(buff2, buff, buff_size);
buff2[512] = '9';
buff2[513] = '9';
buff2[514] = '9';
buff2[515] = ' ';
assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_open_memory(a, buff2, used));
assertEqualIntA(a, ARCHIVE_WARN, archive_read_next_header(a, &ae));
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));


memcpy(buff2, buff, buff_size);
buff2[512] = '1';
buff2[513] = ' ';
assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_open_memory(a, buff2, used));
assertEqualIntA(a, ARCHIVE_WARN, archive_read_next_header(a, &ae));
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));


memcpy(buff2, buff, buff_size);
buff2[512] = ' ';
assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_open_memory(a, buff2, used));
assertEqualIntA(a, ARCHIVE_WARN, archive_read_next_header(a, &ae));
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));


memcpy(buff2, buff, buff_size);
buff2[1024]++;
assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_open_memory(a, buff2, used));
assertEqualIntA(a, ARCHIVE_FATAL, archive_read_next_header(a, &ae));
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));







free(buff);
free(buff2);
free(filedata);
}
