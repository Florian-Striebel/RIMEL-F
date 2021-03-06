

























#include "test.h"
__FBSDID("$FreeBSD$");

static void
test_basic(const char *compression_type)
{
char filedata[64];
struct archive_entry *ae;
struct archive *a;
size_t used;
size_t buffsize = 1000;
char *buff;

buff = malloc(buffsize);


assert((a = archive_write_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_format_7zip(a));
if (compression_type != NULL &&
ARCHIVE_OK != archive_write_set_format_option(a, "7zip",
"compression", compression_type)) {
skipping("%s writing not fully supported on this platform",
compression_type);
assertEqualInt(ARCHIVE_OK, archive_write_free(a));
free(buff);
return;
}
assertEqualIntA(a, ARCHIVE_OK, archive_write_add_filter_none(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_open_memory(a, buff, buffsize, &used));




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_mtime(ae, 1, 10);
assertEqualInt(1, archive_entry_mtime(ae));
assertEqualInt(10, archive_entry_mtime_nsec(ae));
archive_entry_copy_pathname(ae, "empty");
assertEqualString("empty", archive_entry_pathname(ae));
archive_entry_set_mode(ae, AE_IFREG | 0755);
assertEqualInt((AE_IFREG | 0755), archive_entry_mode(ae));

assertEqualInt(ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_mtime(ae, 1, 10);
assertEqualInt(1, archive_entry_mtime(ae));
assertEqualInt(10, archive_entry_mtime_nsec(ae));
archive_entry_copy_pathname(ae, "empty2");
assertEqualString("empty2", archive_entry_pathname(ae));
archive_entry_set_mode(ae, AE_IFREG | 0444);
assertEqualInt((AE_IFREG | 0444), archive_entry_mode(ae));

assertEqualInt(ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_mtime(ae, 1, 100);
assertEqualInt(1, archive_entry_mtime(ae));
assertEqualInt(100, archive_entry_mtime_nsec(ae));
archive_entry_copy_pathname(ae, "file");
assertEqualString("file", archive_entry_pathname(ae));
archive_entry_set_mode(ae, AE_IFREG | 0755);
assertEqualInt((AE_IFREG | 0755), archive_entry_mode(ae));
archive_entry_set_size(ae, 8);

assertEqualInt(0, archive_write_header(a, ae));
archive_entry_free(ae);
assertEqualInt(8, archive_write_data(a, "12345678", 9));
assertEqualInt(0, archive_write_data(a, "1", 1));




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_mtime(ae, 1, 10);
assertEqualInt(1, archive_entry_mtime(ae));
assertEqualInt(10, archive_entry_mtime_nsec(ae));
archive_entry_copy_pathname(ae, "file2");
assertEqualString("file2", archive_entry_pathname(ae));
archive_entry_set_mode(ae, AE_IFREG | 0755);
assertEqualInt((AE_IFREG | 0755), archive_entry_mode(ae));
archive_entry_set_size(ae, 4);

assertEqualInt(ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);
assertEqualInt(4, archive_write_data(a, "1234", 5));




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_mtime(ae, 1, 10);
assertEqualInt(1, archive_entry_mtime(ae));
assertEqualInt(10, archive_entry_mtime_nsec(ae));
archive_entry_copy_pathname(ae, "symbolic");
archive_entry_copy_symlink(ae, "file1");
assertEqualString("symbolic", archive_entry_pathname(ae));
archive_entry_set_mode(ae, AE_IFLNK | 0755);
assertEqualInt((AE_IFLNK | 0755), archive_entry_mode(ae));

assertEqualInt(ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_mtime(ae, 11, 100);
archive_entry_copy_pathname(ae, "dir");
archive_entry_set_mode(ae, AE_IFDIR | 0755);
archive_entry_set_size(ae, 512);

assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
failure("size should be zero so that applications know not to write");
assertEqualInt(0, archive_entry_size(ae));
archive_entry_free(ae);
assertEqualIntA(a, 0, archive_write_data(a, "12345678", 9));




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_mtime(ae, 11, 200);
archive_entry_copy_pathname(ae, "dir/subdir");
archive_entry_set_mode(ae, AE_IFDIR | 0755);
archive_entry_set_size(ae, 512);

assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
failure("size should be zero so that applications know not to write");
assertEqualInt(0, archive_entry_size(ae));
archive_entry_free(ae);
assertEqualIntA(a, 0, archive_write_data(a, "12345678", 9));




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_mtime(ae, 11, 300);
archive_entry_copy_pathname(ae, "dir/subdir/subdir");
archive_entry_set_mode(ae, AE_IFDIR | 0755);
archive_entry_set_size(ae, 512);

assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
failure("size should be zero so that applications know not to write");
assertEqualInt(0, archive_entry_size(ae));
archive_entry_free(ae);
assertEqualIntA(a, 0, archive_write_data(a, "12345678", 9));


assertEqualInt(ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));


assertEqualMem(buff, "\x37\x7a\xbc\xaf\x27\x1c\x00\x03", 8);





assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, read_open_memory_seek(a, buff, used, 7));




assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(1, archive_entry_mtime(ae));
assertEqualInt(100, archive_entry_mtime_nsec(ae));
assertEqualInt(0, archive_entry_atime(ae));
assertEqualInt(0, archive_entry_ctime(ae));
assertEqualString("file", archive_entry_pathname(ae));
assertEqualInt(AE_IFREG | 0755, archive_entry_mode(ae));
assertEqualInt(8, archive_entry_size(ae));
assertEqualIntA(a, 8,
archive_read_data(a, filedata, sizeof(filedata)));
assertEqualMem(filedata, "12345678", 8);





assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(1, archive_entry_mtime(ae));
assertEqualInt(0, archive_entry_mtime_nsec(ae));
assertEqualInt(0, archive_entry_atime(ae));
assertEqualInt(0, archive_entry_ctime(ae));
assertEqualString("file2", archive_entry_pathname(ae));
assertEqualInt(AE_IFREG | 0755, archive_entry_mode(ae));
assertEqualInt(4, archive_entry_size(ae));
assertEqualIntA(a, 4,
archive_read_data(a, filedata, sizeof(filedata)));
assertEqualMem(filedata, "1234", 4);




assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(1, archive_entry_mtime(ae));
assertEqualInt(0, archive_entry_mtime_nsec(ae));
assertEqualInt(0, archive_entry_atime(ae));
assertEqualInt(0, archive_entry_ctime(ae));
assertEqualString("symbolic", archive_entry_pathname(ae));
assertEqualString("file1", archive_entry_symlink(ae));
assertEqualInt(AE_IFLNK | 0755, archive_entry_mode(ae));
assertEqualInt(0, archive_entry_size(ae));




assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(1, archive_entry_mtime(ae));
assertEqualInt(0, archive_entry_mtime_nsec(ae));
assertEqualInt(0, archive_entry_atime(ae));
assertEqualInt(0, archive_entry_ctime(ae));
assertEqualString("empty", archive_entry_pathname(ae));
assertEqualInt(AE_IFREG | 0755, archive_entry_mode(ae));
assertEqualInt(0, archive_entry_size(ae));




assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(1, archive_entry_mtime(ae));
assertEqualInt(0, archive_entry_mtime_nsec(ae));
assertEqualInt(0, archive_entry_atime(ae));
assertEqualInt(0, archive_entry_ctime(ae));
assertEqualString("empty2", archive_entry_pathname(ae));
assertEqualInt(AE_IFREG | 0444, archive_entry_mode(ae));
assertEqualInt(0, archive_entry_size(ae));




assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(11, archive_entry_mtime(ae));
assertEqualInt(300, archive_entry_mtime_nsec(ae));
assertEqualInt(0, archive_entry_atime(ae));
assertEqualInt(0, archive_entry_ctime(ae));
assertEqualString("dir/subdir/subdir/", archive_entry_pathname(ae));
assertEqualInt(AE_IFDIR | 0755, archive_entry_mode(ae));
assertEqualInt(0, archive_entry_size(ae));
assertEqualIntA(a, 0, archive_read_data(a, filedata, 10));




assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(11, archive_entry_mtime(ae));
assertEqualInt(200, archive_entry_mtime_nsec(ae));
assertEqualInt(0, archive_entry_atime(ae));
assertEqualInt(0, archive_entry_ctime(ae));
assertEqualString("dir/subdir/", archive_entry_pathname(ae));
assertEqualInt(AE_IFDIR | 0755, archive_entry_mode(ae));
assertEqualInt(0, archive_entry_size(ae));
assertEqualIntA(a, 0, archive_read_data(a, filedata, 10));




assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(11, archive_entry_mtime(ae));
assertEqualInt(100, archive_entry_mtime_nsec(ae));
assertEqualInt(0, archive_entry_atime(ae));
assertEqualInt(0, archive_entry_ctime(ae));
assertEqualString("dir/", archive_entry_pathname(ae));
assertEqualInt(AE_IFDIR | 0755, archive_entry_mode(ae));
assertEqualInt(0, archive_entry_size(ae));
assertEqualIntA(a, 0, archive_read_data(a, filedata, 10));


assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualIntA(a, ARCHIVE_FILTER_NONE, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_7ZIP, archive_format(a));

assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));

free(buff);
}

static void
test_basic2(const char *compression_type)
{
char filedata[64];
struct archive_entry *ae;
struct archive *a;
size_t used;
size_t buffsize = 1000;
char *buff;

buff = malloc(buffsize);


assert((a = archive_write_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_format_7zip(a));
if (compression_type != NULL &&
ARCHIVE_OK != archive_write_set_format_option(a, "7zip",
"compression", compression_type)) {
skipping("%s writing not fully supported on this platform",
compression_type);
assertEqualInt(ARCHIVE_OK, archive_write_free(a));
free(buff);
return;
}
assertEqualIntA(a, ARCHIVE_OK, archive_write_add_filter_none(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_open_memory(a, buff, buffsize, &used));




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_mtime(ae, 1, 100);
assertEqualInt(1, archive_entry_mtime(ae));
assertEqualInt(100, archive_entry_mtime_nsec(ae));
archive_entry_copy_pathname(ae, "file");
assertEqualString("file", archive_entry_pathname(ae));
archive_entry_set_mode(ae, AE_IFREG | 0755);
assertEqualInt((AE_IFREG | 0755), archive_entry_mode(ae));
archive_entry_set_size(ae, 8);

assertEqualInt(0, archive_write_header(a, ae));
archive_entry_free(ae);
assertEqualInt(8, archive_write_data(a, "12345678", 9));
assertEqualInt(0, archive_write_data(a, "1", 1));




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_mtime(ae, 1, 10);
assertEqualInt(1, archive_entry_mtime(ae));
assertEqualInt(10, archive_entry_mtime_nsec(ae));
archive_entry_copy_pathname(ae, "file2");
assertEqualString("file2", archive_entry_pathname(ae));
archive_entry_set_mode(ae, AE_IFREG | 0755);
assertEqualInt((AE_IFREG | 0755), archive_entry_mode(ae));
archive_entry_set_size(ae, 4);

assertEqualInt(ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);
assertEqualInt(4, archive_write_data(a, "1234", 5));




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_mtime(ae, 11, 100);
archive_entry_copy_pathname(ae, "dir");
archive_entry_set_mode(ae, AE_IFDIR | 0755);
archive_entry_set_size(ae, 512);

assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
failure("size should be zero so that applications know not to write");
assertEqualInt(0, archive_entry_size(ae));
archive_entry_free(ae);
assertEqualIntA(a, 0, archive_write_data(a, "12345678", 9));




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_mtime(ae, 11, 200);
archive_entry_copy_pathname(ae, "dir/subdir");
archive_entry_set_mode(ae, AE_IFDIR | 0755);
archive_entry_set_size(ae, 512);

assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
failure("size should be zero so that applications know not to write");
assertEqualInt(0, archive_entry_size(ae));
archive_entry_free(ae);
assertEqualIntA(a, 0, archive_write_data(a, "12345678", 9));




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_mtime(ae, 11, 300);
archive_entry_copy_pathname(ae, "dir/subdir/subdir");
archive_entry_set_mode(ae, AE_IFDIR | 0755);
archive_entry_set_size(ae, 512);

assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
failure("size should be zero so that applications know not to write");
assertEqualInt(0, archive_entry_size(ae));
archive_entry_free(ae);
assertEqualIntA(a, 0, archive_write_data(a, "12345678", 9));


assertEqualInt(ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));


assertEqualMem(buff, "\x37\x7a\xbc\xaf\x27\x1c\x00\x03", 8);





assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, read_open_memory_seek(a, buff, used, 7));




assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(1, archive_entry_mtime(ae));
assertEqualInt(100, archive_entry_mtime_nsec(ae));
assertEqualInt(0, archive_entry_atime(ae));
assertEqualInt(0, archive_entry_ctime(ae));
assertEqualString("file", archive_entry_pathname(ae));
assertEqualInt(AE_IFREG | 0755, archive_entry_mode(ae));
assertEqualInt(8, archive_entry_size(ae));
assertEqualIntA(a, 8,
archive_read_data(a, filedata, sizeof(filedata)));
assertEqualMem(filedata, "12345678", 8);





assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(1, archive_entry_mtime(ae));
assertEqualInt(0, archive_entry_mtime_nsec(ae));
assertEqualInt(0, archive_entry_atime(ae));
assertEqualInt(0, archive_entry_ctime(ae));
assertEqualString("file2", archive_entry_pathname(ae));
assertEqualInt(AE_IFREG | 0755, archive_entry_mode(ae));
assertEqualInt(4, archive_entry_size(ae));
assertEqualIntA(a, 4,
archive_read_data(a, filedata, sizeof(filedata)));
assertEqualMem(filedata, "1234", 4);




assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(11, archive_entry_mtime(ae));
assertEqualInt(300, archive_entry_mtime_nsec(ae));
assertEqualInt(0, archive_entry_atime(ae));
assertEqualInt(0, archive_entry_ctime(ae));
assertEqualString("dir/subdir/subdir/", archive_entry_pathname(ae));
assertEqualInt(AE_IFDIR | 0755, archive_entry_mode(ae));
assertEqualInt(0, archive_entry_size(ae));
assertEqualIntA(a, 0, archive_read_data(a, filedata, 10));




assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(11, archive_entry_mtime(ae));
assertEqualInt(200, archive_entry_mtime_nsec(ae));
assertEqualInt(0, archive_entry_atime(ae));
assertEqualInt(0, archive_entry_ctime(ae));
assertEqualString("dir/subdir/", archive_entry_pathname(ae));
assertEqualInt(AE_IFDIR | 0755, archive_entry_mode(ae));
assertEqualInt(0, archive_entry_size(ae));
assertEqualIntA(a, 0, archive_read_data(a, filedata, 10));




assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(11, archive_entry_mtime(ae));
assertEqualInt(100, archive_entry_mtime_nsec(ae));
assertEqualInt(0, archive_entry_atime(ae));
assertEqualInt(0, archive_entry_ctime(ae));
assertEqualString("dir/", archive_entry_pathname(ae));
assertEqualInt(AE_IFDIR | 0755, archive_entry_mode(ae));
assertEqualInt(0, archive_entry_size(ae));
assertEqualIntA(a, 0, archive_read_data(a, filedata, 10));


assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualIntA(a, ARCHIVE_FILTER_NONE, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_7ZIP, archive_format(a));

assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));

free(buff);
}

DEFINE_TEST(test_write_format_7zip)
{


test_basic(NULL);

test_basic2(NULL);
}

DEFINE_TEST(test_write_format_7zip_basic_bzip2)
{

test_basic("bzip2");
}

DEFINE_TEST(test_write_format_7zip_basic_copy)
{

test_basic("copy");
}

DEFINE_TEST(test_write_format_7zip_basic_deflate)
{

test_basic("deflate");
}

DEFINE_TEST(test_write_format_7zip_basic_lzma1)
{

test_basic("lzma1");
}

DEFINE_TEST(test_write_format_7zip_basic_lzma2)
{

test_basic("lzma2");
}

DEFINE_TEST(test_write_format_7zip_basic_ppmd)
{

test_basic("ppmd");
}
