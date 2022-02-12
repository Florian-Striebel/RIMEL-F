

























#include "test.h"
__FBSDID("$FreeBSD$");




DEFINE_TEST(test_write_format_7zip_empty_archive)
{
struct archive *a;
size_t buffsize = 1000;
char *buff;
size_t used;

buff = malloc(buffsize);


assert((a = archive_write_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_format_7zip(a));
assertEqualIntA(a, ARCHIVE_OK, archive_write_add_filter_none(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_open_memory(a, buff, buffsize, &used));


assertEqualInt(ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));


assertEqualInt(32, used);


assertEqualMem(buff,
"\x37\x7a\xbc\xaf\x27\x1c\x00\x03"
"\x8d\x9b\xd5\x0f\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00", 32);

free(buff);
}




static void
test_only_empty_file(void)
{
struct archive *a;
struct archive_entry *ae;
size_t buffsize = 1000;
char *buff;
size_t used;

buff = malloc(buffsize);


assert((a = archive_write_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_format_7zip(a));
assertEqualIntA(a, ARCHIVE_OK, archive_write_add_filter_none(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_open_memory(a, buff, buffsize, &used));




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_mtime(ae, 1, 10);
assertEqualInt(1, archive_entry_mtime(ae));
assertEqualInt(10, archive_entry_mtime_nsec(ae));
archive_entry_set_atime(ae, 2, 20);
assertEqualInt(2, archive_entry_atime(ae));
assertEqualInt(20, archive_entry_atime_nsec(ae));
archive_entry_set_ctime(ae, 0, 100);
assertEqualInt(0, archive_entry_ctime(ae));
assertEqualInt(100, archive_entry_ctime_nsec(ae));
archive_entry_copy_pathname(ae, "empty");
assertEqualString("empty", archive_entry_pathname(ae));
archive_entry_set_mode(ae, AE_IFREG | 0755);
assertEqualInt((S_IFREG | 0755), archive_entry_mode(ae));

assertEqualInt(ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);


assertEqualInt(ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));


assertEqualInt(102, used);


assertEqualMem(buff,
"\x37\x7a\xbc\xaf\x27\x1c\x00\x03"
"\x00\x5b\x58\x25\x00\x00\x00\x00"
"\x00\x00\x00\x00\x46\x00\x00\x00"
"\x00\x00\x00\x00\x8f\xce\x1d\xf3", 32);





assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, read_open_memory_seek(a, buff, used, 7));




assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(1, archive_entry_mtime(ae));
assertEqualInt(0, archive_entry_mtime_nsec(ae));
assertEqualInt(2, archive_entry_atime(ae));
assertEqualInt(0, archive_entry_atime_nsec(ae));
assertEqualInt(0, archive_entry_ctime(ae));
assertEqualInt(100, archive_entry_ctime_nsec(ae));
assertEqualString("empty", archive_entry_pathname(ae));
assertEqualInt(AE_IFREG | 0755, archive_entry_mode(ae));
assertEqualInt(0, archive_entry_size(ae));


assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualIntA(a, ARCHIVE_FILTER_NONE, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_7ZIP, archive_format(a));

assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));

free(buff);
}

static void
test_only_empty_files(void)
{
struct archive *a;
struct archive_entry *ae;
size_t buffsize = 1000;
char *buff;
size_t used;

buff = malloc(buffsize);


assert((a = archive_write_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_format_7zip(a));
assertEqualIntA(a, ARCHIVE_OK, archive_write_add_filter_none(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_open_memory(a, buff, buffsize, &used));




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_mtime(ae, 1, 10);
assertEqualInt(1, archive_entry_mtime(ae));
assertEqualInt(10, archive_entry_mtime_nsec(ae));
archive_entry_set_atime(ae, 2, 20);
assertEqualInt(2, archive_entry_atime(ae));
assertEqualInt(20, archive_entry_atime_nsec(ae));
archive_entry_copy_pathname(ae, "empty");
assertEqualString("empty", archive_entry_pathname(ae));
archive_entry_set_mode(ae, AE_IFREG | 0755);
assertEqualInt((AE_IFREG | 0755), archive_entry_mode(ae));

assertEqualInt(ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_mtime(ae, 2, 10);
assertEqualInt(2, archive_entry_mtime(ae));
assertEqualInt(10, archive_entry_mtime_nsec(ae));
archive_entry_set_ctime(ae, 2, 10);
assertEqualInt(2, archive_entry_ctime(ae));
assertEqualInt(10, archive_entry_ctime_nsec(ae));
archive_entry_copy_pathname(ae, "empty2");
assertEqualString("empty2", archive_entry_pathname(ae));
archive_entry_set_mode(ae, AE_IFREG | 0644);
assertEqualInt((AE_IFREG | 0644), archive_entry_mode(ae));

assertEqualInt(ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_mtime(ae, 3, 10);
assertEqualInt(3, archive_entry_mtime(ae));
assertEqualInt(10, archive_entry_mtime_nsec(ae));
archive_entry_copy_pathname(ae, "empty3");
assertEqualString("empty3", archive_entry_pathname(ae));
archive_entry_set_mode(ae, AE_IFREG | 0644);
assertEqualInt((AE_IFREG | 0644), archive_entry_mode(ae));

assertEqualInt(ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);


assertEqualInt(ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));


assertEqualMem(buff, "\x37\x7a\xbc\xaf\x27\x1c\x00\x03", 8);





assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, read_open_memory_seek(a, buff, used, 7));




assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(1, archive_entry_mtime(ae));
assertEqualInt(0, archive_entry_mtime_nsec(ae));
assertEqualInt(2, archive_entry_atime(ae));
assertEqualInt(0, archive_entry_atime_nsec(ae));
assertEqualInt(0, archive_entry_ctime(ae));
assertEqualString("empty", archive_entry_pathname(ae));
assertEqualInt(AE_IFREG | 0755, archive_entry_mode(ae));
assertEqualInt(0, archive_entry_size(ae));




assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(2, archive_entry_mtime(ae));
assertEqualInt(0, archive_entry_mtime_nsec(ae));
assertEqualInt(0, archive_entry_atime(ae));
assertEqualInt(2, archive_entry_ctime(ae));
assertEqualInt(0, archive_entry_ctime_nsec(ae));
assertEqualString("empty2", archive_entry_pathname(ae));
assertEqualInt(AE_IFREG | 0644, archive_entry_mode(ae));
assertEqualInt(0, archive_entry_size(ae));




assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(3, archive_entry_mtime(ae));
assertEqualInt(0, archive_entry_mtime_nsec(ae));
assertEqualInt(0, archive_entry_atime(ae));
assertEqualInt(0, archive_entry_ctime(ae));
assertEqualString("empty3", archive_entry_pathname(ae));
assertEqualInt(AE_IFREG | 0644, archive_entry_mode(ae));
assertEqualInt(0, archive_entry_size(ae));


assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualIntA(a, ARCHIVE_FILTER_NONE, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_7ZIP, archive_format(a));

assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));

free(buff);
}

DEFINE_TEST(test_write_format_7zip_empty_files)
{

test_only_empty_file();
test_only_empty_files();
}
