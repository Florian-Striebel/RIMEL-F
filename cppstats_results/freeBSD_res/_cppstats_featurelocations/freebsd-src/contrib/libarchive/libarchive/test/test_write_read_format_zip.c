





























#include "test.h"
__FBSDID("$FreeBSD: head/lib/libarchive/test/test_write_format_zip.c 201247 2009-12-30 05:59:21Z kientzle $");









static void
write_contents(struct archive *a)
{
struct archive_entry *ae;










assert((ae = archive_entry_new()) != NULL);
archive_entry_set_mtime(ae, 1, 10);
archive_entry_copy_pathname(ae, "file");
archive_entry_set_mode(ae, AE_IFREG | 0755);
archive_entry_set_size(ae, 8);
assertEqualInt(0, archive_write_header(a, ae));
archive_entry_free(ae);
assertEqualInt(8, archive_write_data(a, "12345678", 9));
assertEqualInt(0, archive_write_data(a, "1", 1));




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_mtime(ae, 1, 10);
archive_entry_copy_pathname(ae, "file2");
archive_entry_set_mode(ae, AE_IFREG | 0755);
archive_entry_set_size(ae, 4);
assertEqualInt(ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);
assertEqualInt(4, archive_write_data(a, "1234", 4));




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_mtime(ae, 2, 15);
archive_entry_copy_pathname(ae, "file3");
archive_entry_set_mode(ae, AE_IFREG | 0621);
archive_entry_unset_size(ae);
assertEqualInt(ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);
assertEqualInt(5, archive_write_data(a, "mnopq", 5));




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_mtime(ae, 1, 10);
assertEqualInt(1, archive_entry_mtime(ae));
assertEqualInt(10, archive_entry_mtime_nsec(ae));
archive_entry_copy_pathname(ae, "symlink");
assertEqualString("symlink", archive_entry_pathname(ae));
archive_entry_copy_symlink(ae, "file1");
assertEqualString("file1", archive_entry_symlink(ae));
archive_entry_set_mode(ae, AE_IFLNK | 0755);
assertEqualInt((AE_IFLNK | 0755), archive_entry_mode(ae));
archive_entry_set_size(ae, 4);

assertEqualInt(ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_mtime(ae, 11, 110);
archive_entry_copy_pathname(ae, "dir");
archive_entry_set_mode(ae, S_IFDIR | 0755);
archive_entry_set_size(ae, 512);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
failure("size should be zero so that applications know not to write");
assertEqualInt(0, archive_entry_size(ae));
archive_entry_free(ae);
assertEqualIntA(a, 0, archive_write_data(a, "12345678", 9));




#if defined(HAVE_ZLIB_H)
assertEqualIntA(a, ARCHIVE_OK, archive_write_zip_set_compression_deflate(a));




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_mtime(ae, 1, 10);
archive_entry_copy_pathname(ae, "file_deflate");
archive_entry_set_mode(ae, AE_IFREG | 0755);
archive_entry_set_size(ae, 8);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);
assertEqualInt(8, archive_write_data(a, "12345678", 9));
assertEqualInt(0, archive_write_data(a, "1", 1));




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_mtime(ae, 1, 10);
archive_entry_copy_pathname(ae, "file2_deflate");
archive_entry_set_mode(ae, AE_IFREG | 0755);
archive_entry_set_size(ae, 4);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);
assertEqualInt(4, archive_write_data(a, "1234", 4));




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_mtime(ae, 2, 15);
archive_entry_copy_pathname(ae, "file3_deflate");
archive_entry_set_mode(ae, AE_IFREG | 0621);
archive_entry_unset_size(ae);
assertEqualInt(ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);
assertEqualInt(5, archive_write_data(a, "ghijk", 5));




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_mtime(ae, 1, 10);
archive_entry_copy_pathname(ae, "symlink_deflate");
archive_entry_copy_symlink(ae, "file1");
archive_entry_set_mode(ae, AE_IFLNK | 0755);
archive_entry_set_size(ae, 4);
assertEqualInt(ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_mtime(ae, 11, 110);
archive_entry_copy_pathname(ae, "dir_deflate");
archive_entry_set_mode(ae, S_IFDIR | 0755);
archive_entry_set_size(ae, 512);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
failure("size should be zero so that applications know not to write");
assertEqualInt(0, archive_entry_size(ae));
archive_entry_free(ae);
assertEqualIntA(a, 0, archive_write_data(a, "12345678", 9));
assertEqualIntA(a, ARCHIVE_OK, archive_write_finish_entry(a));
#endif




assertEqualIntA(a, ARCHIVE_OK, archive_write_zip_set_compression_store(a));




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_mtime(ae, 1, 10);
archive_entry_copy_pathname(ae, "file_stored");
archive_entry_set_mode(ae, AE_IFREG | 0755);
archive_entry_set_size(ae, 8);
assertEqualInt(0, archive_write_header(a, ae));
archive_entry_free(ae);
assertEqualInt(8, archive_write_data(a, "12345678", 9));
assertEqualInt(0, archive_write_data(a, "1", 1));




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_mtime(ae, 1, 10);
archive_entry_copy_pathname(ae, "file2_stored");
archive_entry_set_mode(ae, AE_IFREG | 0755);
archive_entry_set_size(ae, 4);
assertEqualInt(ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);
assertEqualInt(4, archive_write_data(a, "ACEG", 4));




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_mtime(ae, 2, 15);
archive_entry_copy_pathname(ae, "file3_stored");
archive_entry_set_mode(ae, AE_IFREG | 0621);
archive_entry_unset_size(ae);
assertEqualInt(ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);
assertEqualInt(5, archive_write_data(a, "ijklm", 5));




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_mtime(ae, 1, 10);
archive_entry_copy_pathname(ae, "symlink_stored");
archive_entry_copy_symlink(ae, "file1");
archive_entry_set_mode(ae, AE_IFLNK | 0755);
archive_entry_set_size(ae, 4);
assertEqualInt(ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_mtime(ae, 11, 110);
archive_entry_copy_pathname(ae, "dir_stored");
archive_entry_set_mode(ae, S_IFDIR | 0755);
archive_entry_set_size(ae, 512);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
failure("size should be zero so that applications know not to write");
assertEqualInt(0, archive_entry_size(ae));
archive_entry_free(ae);
assertEqualIntA(a, 0, archive_write_data(a, "12345678", 9));



assertEqualInt(ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));
}




static void
verify_contents(struct archive *a, int seeking, int improved_streaming)
{
char filedata[64];
struct archive_entry *ae;






assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(1, archive_entry_mtime(ae));

assertEqualInt(0, archive_entry_mtime_nsec(ae));
assertEqualInt(0, archive_entry_atime(ae));
assertEqualInt(0, archive_entry_ctime(ae));
assertEqualString("file", archive_entry_pathname(ae));
if (seeking || improved_streaming) {
assertEqualInt(AE_IFREG | 0755, archive_entry_mode(ae));
}
assertEqualInt(8, archive_entry_size(ae));
assert(archive_entry_size_is_set(ae));
assertEqualIntA(a, 8,
archive_read_data(a, filedata, sizeof(filedata)));
assertEqualMem(filedata, "12345678", 8);



assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(1, archive_entry_mtime(ae));
assertEqualInt(0, archive_entry_mtime_nsec(ae));
assertEqualInt(0, archive_entry_atime(ae));
assertEqualInt(0, archive_entry_ctime(ae));
assertEqualString("file2", archive_entry_pathname(ae));
if (seeking || improved_streaming) {
assertEqualInt(AE_IFREG | 0755, archive_entry_mode(ae));
}
assertEqualInt(4, archive_entry_size(ae));
assert(archive_entry_size_is_set(ae));
assertEqualIntA(a, 4,
archive_read_data(a, filedata, sizeof(filedata)));
assertEqualMem(filedata, "1234", 4);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(2, archive_entry_mtime(ae));
assertEqualInt(0, archive_entry_mtime_nsec(ae));
assertEqualInt(0, archive_entry_atime(ae));
assertEqualInt(0, archive_entry_ctime(ae));
assertEqualString("file3", archive_entry_pathname(ae));
if (seeking || improved_streaming) {
assertEqualInt(AE_IFREG | 0621, archive_entry_mode(ae));
}
if (seeking) {
assertEqualInt(5, archive_entry_size(ae));
} else {
assertEqualInt(0, archive_entry_size_is_set(ae));
}
assertEqualIntA(a, 5,
archive_read_data(a, filedata, sizeof(filedata)));
assertEqualMem(filedata, "mnopq", 5);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(1, archive_entry_mtime(ae));
assertEqualInt(0, archive_entry_mtime_nsec(ae));
assertEqualInt(0, archive_entry_atime(ae));
assertEqualInt(0, archive_entry_ctime(ae));
assertEqualString("symlink", archive_entry_pathname(ae));
if (seeking || improved_streaming) {
assertEqualInt(AE_IFLNK | 0755, archive_entry_mode(ae));
assertEqualInt(0, archive_entry_size(ae));
assertEqualString("file1", archive_entry_symlink(ae));
} else {


assertEqualInt(AE_IFREG | 0664, archive_entry_mode(ae));
assertEqualInt(5, archive_entry_size(ae));
assert(archive_entry_size_is_set(ae));
}


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(11, archive_entry_mtime(ae));
assertEqualInt(0, archive_entry_mtime_nsec(ae));
assertEqualInt(0, archive_entry_atime(ae));
assertEqualInt(0, archive_entry_ctime(ae));
assertEqualString("dir/", archive_entry_pathname(ae));
if (seeking || improved_streaming)
assertEqualInt(AE_IFDIR | 0755, archive_entry_mode(ae));
assertEqualInt(0, archive_entry_size(ae));
assert(archive_entry_size_is_set(ae));
assertEqualIntA(a, 0, archive_read_data(a, filedata, 10));

#if defined(HAVE_ZLIB_H)





assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(1, archive_entry_mtime(ae));

assertEqualInt(0, archive_entry_mtime_nsec(ae));
assertEqualInt(0, archive_entry_atime(ae));
assertEqualInt(0, archive_entry_ctime(ae));
assertEqualString("file_deflate", archive_entry_pathname(ae));
if (seeking || improved_streaming) {
assertEqualInt(AE_IFREG | 0755, archive_entry_mode(ae));
}
assertEqualInt(8, archive_entry_size(ae));
assert(archive_entry_size_is_set(ae));
assertEqualIntA(a, 8,
archive_read_data(a, filedata, sizeof(filedata)));
assertEqualMem(filedata, "12345678", 8);



assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(1, archive_entry_mtime(ae));
assertEqualInt(0, archive_entry_mtime_nsec(ae));
assertEqualInt(0, archive_entry_atime(ae));
assertEqualInt(0, archive_entry_ctime(ae));
assertEqualString("file2_deflate", archive_entry_pathname(ae));
if (seeking || improved_streaming) {
assertEqualInt(AE_IFREG | 0755, archive_entry_mode(ae));
}
assertEqualInt(4, archive_entry_size(ae));
assert(archive_entry_size_is_set(ae));
assertEqualIntA(a, 4,
archive_read_data(a, filedata, sizeof(filedata)));
assertEqualMem(filedata, "1234", 4);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(2, archive_entry_mtime(ae));
assertEqualInt(0, archive_entry_mtime_nsec(ae));
assertEqualInt(0, archive_entry_atime(ae));
assertEqualInt(0, archive_entry_ctime(ae));
assertEqualString("file3_deflate", archive_entry_pathname(ae));
if (seeking || improved_streaming) {
assertEqualInt(AE_IFREG | 0621, archive_entry_mode(ae));
}
if (seeking) {
assertEqualInt(5, archive_entry_size(ae));
} else {
assertEqualInt(0, archive_entry_size_is_set(ae));
}
assertEqualIntA(a, 5,
archive_read_data(a, filedata, sizeof(filedata)));
assertEqualMem(filedata, "ghijk", 4);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(1, archive_entry_mtime(ae));
assertEqualInt(0, archive_entry_mtime_nsec(ae));
assertEqualInt(0, archive_entry_atime(ae));
assertEqualInt(0, archive_entry_ctime(ae));
assertEqualString("symlink_deflate", archive_entry_pathname(ae));
if (seeking || improved_streaming) {
assertEqualInt(AE_IFLNK | 0755, archive_entry_mode(ae));
assertEqualInt(0, archive_entry_size(ae));
assertEqualString("file1", archive_entry_symlink(ae));
} else {
assertEqualInt(AE_IFREG | 0664, archive_entry_mode(ae));
assertEqualInt(5, archive_entry_size(ae));
assertEqualIntA(a, 5, archive_read_data(a, filedata, 10));
assertEqualMem(filedata, "file1", 5);
}


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(11, archive_entry_mtime(ae));
assertEqualInt(0, archive_entry_mtime_nsec(ae));
assertEqualInt(0, archive_entry_atime(ae));
assertEqualInt(0, archive_entry_ctime(ae));
assertEqualString("dir_deflate/", archive_entry_pathname(ae));
if (seeking) {
assertEqualInt(AE_IFDIR | 0755, archive_entry_mode(ae));
}
assertEqualInt(0, archive_entry_size(ae));
assert(archive_entry_size_is_set(ae));
assertEqualIntA(a, 0, archive_read_data(a, filedata, 10));
#endif






assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(1, archive_entry_mtime(ae));

assertEqualInt(0, archive_entry_mtime_nsec(ae));
assertEqualInt(0, archive_entry_atime(ae));
assertEqualInt(0, archive_entry_ctime(ae));
assertEqualString("file_stored", archive_entry_pathname(ae));
if (seeking || improved_streaming) {
assertEqualInt(AE_IFREG | 0755, archive_entry_mode(ae));
}
assert(archive_entry_size_is_set(ae));
assert(archive_entry_size_is_set(ae));
assertEqualInt(8, archive_entry_size(ae));
assertEqualIntA(a, 8,
archive_read_data(a, filedata, sizeof(filedata)));
assertEqualMem(filedata, "12345678", 8);



assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(1, archive_entry_mtime(ae));
assertEqualInt(0, archive_entry_mtime_nsec(ae));
assertEqualInt(0, archive_entry_atime(ae));
assertEqualInt(0, archive_entry_ctime(ae));
assertEqualString("file2_stored", archive_entry_pathname(ae));
if (seeking || improved_streaming) {
assertEqualInt(AE_IFREG | 0755, archive_entry_mode(ae));
}
assertEqualInt(4, archive_entry_size(ae));
assert(archive_entry_size_is_set(ae));
assertEqualIntA(a, 4,
archive_read_data(a, filedata, sizeof(filedata)));
assertEqualMem(filedata, "ACEG", 4);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(2, archive_entry_mtime(ae));
assertEqualInt(0, archive_entry_mtime_nsec(ae));
assertEqualInt(0, archive_entry_atime(ae));
assertEqualInt(0, archive_entry_ctime(ae));
assertEqualString("file3_stored", archive_entry_pathname(ae));
if (seeking || improved_streaming)
assertEqualInt(AE_IFREG | 0621, archive_entry_mode(ae));
if (seeking) {
assertEqualInt(5, archive_entry_size(ae));
} else {
assertEqualInt(0, archive_entry_size_is_set(ae));
}
assertEqualIntA(a, 5,
archive_read_data(a, filedata, sizeof(filedata)));
assertEqualMem(filedata, "ijklm", 4);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(1, archive_entry_mtime(ae));
assertEqualInt(0, archive_entry_mtime_nsec(ae));
assertEqualInt(0, archive_entry_atime(ae));
assertEqualInt(0, archive_entry_ctime(ae));
assertEqualString("symlink_stored", archive_entry_pathname(ae));
if (seeking || improved_streaming) {
assertEqualInt(AE_IFLNK | 0755, archive_entry_mode(ae));
assertEqualInt(0, archive_entry_size(ae));
assertEqualString("file1", archive_entry_symlink(ae));
} else {
assertEqualInt(AE_IFREG | 0664, archive_entry_mode(ae));
assertEqualInt(5, archive_entry_size(ae));
assertEqualIntA(a, 5, archive_read_data(a, filedata, 10));
assertEqualMem(filedata, "file1", 5);
}


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(11, archive_entry_mtime(ae));
assertEqualInt(0, archive_entry_mtime_nsec(ae));
assertEqualInt(0, archive_entry_atime(ae));
assertEqualInt(0, archive_entry_ctime(ae));
assertEqualString("dir_stored/", archive_entry_pathname(ae));
if (seeking || improved_streaming)
assertEqualInt(AE_IFDIR | 0755, archive_entry_mode(ae));
assertEqualInt(0, archive_entry_size(ae));
assertEqualIntA(a, 0, archive_read_data(a, filedata, 10));


assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));
assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}




DEFINE_TEST(test_write_read_format_zip)
{
struct archive *a;
size_t used;
size_t buffsize = 1000000;
char *buff;

buff = malloc(buffsize);


assert((a = archive_write_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_format_zip(a));
assertEqualIntA(a, ARCHIVE_OK, archive_write_add_filter_none(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_open_memory(a, buff, buffsize, &used));
write_contents(a);
dumpfile("constructed.zip", buff, used);





assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_open_memory(a, buff, used));
verify_contents(a, 1, 0);


assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, read_open_memory(a, buff, used, 7));

verify_contents(a, 0, 0);


assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, read_open_memory_seek(a, buff, used, 7));
verify_contents(a, 1, 0);

free(buff);
}




DEFINE_TEST(test_write_read_format_zip_improved_streaming)
{
struct archive *a;
size_t used;
size_t buffsize = 1000000;
char *buff;

buff = malloc(buffsize);


assert((a = archive_write_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_format_zip(a));
assertEqualIntA(a, ARCHIVE_OK, archive_write_add_filter_none(a));
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_options(a, "zip:experimental"));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_open_memory(a, buff, buffsize, &used));
write_contents(a);
dumpfile("constructed.zip", buff, used);





assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_open_memory(a, buff, used));
verify_contents(a, 1, 1);


assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, read_open_memory(a, buff, used, 7));

verify_contents(a, 0, 1);


assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, read_open_memory_seek(a, buff, used, 7));
verify_contents(a, 1, 1);

free(buff);
}




DEFINE_TEST(test_write_read_format_zip64)
{
struct archive *a;
size_t used;
size_t buffsize = 1000000;
char *buff;

buff = malloc(buffsize);


assert((a = archive_write_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_format_zip(a));
assertEqualIntA(a, ARCHIVE_OK, archive_write_add_filter_none(a));
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_options(a, "zip:zip64"));
#if ZIP_IMPROVED_STREAMING
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_options(a, "zip:experimental"));
#endif
assertEqualIntA(a, ARCHIVE_OK,
archive_write_open_memory(a, buff, buffsize, &used));
write_contents(a);
dumpfile("constructed64.zip", buff, used);





assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_open_memory(a, buff, used));
verify_contents(a, 1, 0);


assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, read_open_memory(a, buff, used, 7));

verify_contents(a, 0, 0);


assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, read_open_memory_seek(a, buff, used, 7));
verify_contents(a, 1, 0);

free(buff);
}





DEFINE_TEST(test_write_read_format_zip64_improved_streaming)
{
struct archive *a;
size_t used;
size_t buffsize = 1000000;
char *buff;

buff = malloc(buffsize);


assert((a = archive_write_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_format_zip(a));
assertEqualIntA(a, ARCHIVE_OK, archive_write_add_filter_none(a));
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_options(a, "zip:zip64"));
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_options(a, "zip:experimental"));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_open_memory(a, buff, buffsize, &used));
write_contents(a);
dumpfile("constructed64.zip", buff, used);





assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_open_memory(a, buff, used));
verify_contents(a, 1, 1);


assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, read_open_memory(a, buff, used, 7));

verify_contents(a, 0, 1);


assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, read_open_memory_seek(a, buff, used, 7));
verify_contents(a, 1, 1);

free(buff);
}
