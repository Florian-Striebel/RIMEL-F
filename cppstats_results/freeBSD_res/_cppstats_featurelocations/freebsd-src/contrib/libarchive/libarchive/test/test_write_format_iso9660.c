
























#include "test.h"
__FBSDID("$FreeBSD$");

static char buff2[64];
DEFINE_TEST(test_write_format_iso9660)
{
size_t buffsize = 1000000;
char *buff;
struct archive_entry *ae;
struct archive *a;
char dirname[1024];
char dir[6];
char longname[] =
"longname00aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
"bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"
"cccccccccccccccccccccccccccccccccccccccccccccccccc"
"dddddddddddddddddddddddddddddddddddddddddddddddddd";

size_t used;
int i;

buff = malloc(buffsize);
assert(buff != NULL);


assert((a = archive_write_new()) != NULL);
assertA(0 == archive_write_set_format_iso9660(a));
assertA(0 == archive_write_add_filter_none(a));
assertA(0 == archive_write_open_memory(a, buff, buffsize, &used));




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_atime(ae, 2, 20);
archive_entry_set_birthtime(ae, 3, 30);
archive_entry_set_ctime(ae, 4, 40);
archive_entry_set_mtime(ae, 5, 50);
archive_entry_copy_pathname(ae, "file");
archive_entry_set_mode(ae, AE_IFREG | 0755);
archive_entry_set_size(ae, 8);
archive_entry_set_nlink(ae, 2);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);
assertEqualIntA(a, 8, archive_write_data(a, "12345678", 9));




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_atime(ae, 2, 20);
archive_entry_set_birthtime(ae, 3, 30);
archive_entry_set_ctime(ae, 4, 40);
archive_entry_set_mtime(ae, 5, 50);
archive_entry_copy_pathname(ae, "hardlnk");
archive_entry_set_mode(ae, AE_IFREG | 0755);
archive_entry_set_hardlink(ae, "file");
archive_entry_set_nlink(ae, 2);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_atime(ae, 2, 20);
archive_entry_set_birthtime(ae, 8, 80);
archive_entry_set_ctime(ae, 4, 40);
archive_entry_set_mtime(ae, 5, 50);
archive_entry_copy_pathname(ae, longname);
archive_entry_set_mode(ae, AE_IFREG | 0666);
archive_entry_set_size(ae, 8);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);
assertEqualIntA(a, 8, archive_write_data(a, "12345678", 9));




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_atime(ae, 2, 20);
archive_entry_set_ctime(ae, 4, 40);
archive_entry_set_mtime(ae, 5, 50);
archive_entry_copy_pathname(ae, "symlnk");
archive_entry_set_mode(ae, AE_IFLNK | 0555);
archive_entry_set_symlink(ae, longname);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);




dirname[0] = '\0';
strcpy(dir, "/dir0");
for (i = 0; i < 13; i++) {
dir[4] = "0123456789ABCDEF"[i];
if (i == 0)
strcat(dirname, dir+1);
else
strcat(dirname, dir);
assert((ae = archive_entry_new()) != NULL);
archive_entry_set_atime(ae, 2, 20);
archive_entry_set_birthtime(ae, 3, 30);
archive_entry_set_ctime(ae, 4, 40);
archive_entry_set_mtime(ae, 5, 50);
archive_entry_copy_pathname(ae, dirname);
archive_entry_set_mode(ae, S_IFDIR | 0755);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);
}

strcat(dirname, "/file");
assert((ae = archive_entry_new()) != NULL);
archive_entry_set_atime(ae, 2, 20);
archive_entry_set_birthtime(ae, 3, 30);
archive_entry_set_ctime(ae, 4, 40);
archive_entry_set_mtime(ae, 5, 50);
archive_entry_copy_pathname(ae, dirname);
archive_entry_set_mode(ae, S_IFREG | 0755);
archive_entry_set_size(ae, 8);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);
assertEqualIntA(a, 8, archive_write_data(a, "12345678", 9));




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_atime(ae, 2, 20);
archive_entry_set_birthtime(ae, 3, 30);
archive_entry_set_ctime(ae, 4, 40);
archive_entry_set_mtime(ae, 5, 50);
archive_entry_copy_pathname(ae, "dir0/dir1/file1");
archive_entry_set_mode(ae, AE_IFREG | 0755);
archive_entry_set_size(ae, 8);
archive_entry_set_nlink(ae, 1);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);
assertEqualIntA(a, 8, archive_write_data(a, "12345678", 9));




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_atime(ae, 2, 20);
archive_entry_set_birthtime(ae, 3, 30);
archive_entry_set_ctime(ae, 4, 40);
archive_entry_set_mtime(ae, 5, 50);
archive_entry_copy_pathname(ae, "dir0/dir1/file2");
archive_entry_set_mode(ae, AE_IFREG | 0755);
archive_entry_set_size(ae, 8);
archive_entry_set_nlink(ae, 1);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);
assertEqualIntA(a, 8, archive_write_data(a, "12345678", 9));




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_atime(ae, 2, 20);
archive_entry_set_birthtime(ae, 3, 30);
archive_entry_set_ctime(ae, 4, 40);
archive_entry_set_mtime(ae, 5, 50);
archive_entry_copy_pathname(ae, "dir0/dir1/file2/wrong");
archive_entry_set_mode(ae, AE_IFREG | 0755);
archive_entry_set_size(ae, 8);
archive_entry_set_nlink(ae, 1);
assertEqualIntA(a, ARCHIVE_FAILED, archive_write_header(a, ae));
archive_entry_free(ae);


assertEqualIntA(a, ARCHIVE_OK, archive_write_close(a));
assertEqualIntA(a, ARCHIVE_OK, archive_write_free(a));






assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, 0, archive_read_support_format_all(a));
assertEqualIntA(a, 0, archive_read_support_filter_all(a));
assertEqualIntA(a, 0, archive_read_open_memory(a, buff, used));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(archive_entry_atime(ae), archive_entry_ctime(ae));
assertEqualInt(archive_entry_atime(ae), archive_entry_mtime(ae));
assertEqualString(".", archive_entry_pathname(ae));
assert((S_IFDIR | 0555) == archive_entry_mode(ae));
assertEqualInt(2048, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(2, archive_entry_atime(ae));
assertEqualInt(3, archive_entry_birthtime(ae));
assertEqualInt(4, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("dir0", archive_entry_pathname(ae));
assert((S_IFDIR | 0555) == archive_entry_mode(ae));
assertEqualInt(2048, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(2, archive_entry_atime(ae));
assertEqualInt(3, archive_entry_birthtime(ae));
assertEqualInt(4, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("dir0/dir1", archive_entry_pathname(ae));
assert((S_IFDIR | 0555) == archive_entry_mode(ae));
assertEqualInt(2048, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(2, archive_entry_atime(ae));
assertEqualInt(3, archive_entry_birthtime(ae));
assertEqualInt(4, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("dir0/dir1/dir2", archive_entry_pathname(ae));
assert((S_IFDIR | 0555) == archive_entry_mode(ae));
assertEqualInt(2048, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(2, archive_entry_atime(ae));
assertEqualInt(3, archive_entry_birthtime(ae));
assertEqualInt(4, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("dir0/dir1/dir2/dir3",
archive_entry_pathname(ae));
assert((S_IFDIR | 0555) == archive_entry_mode(ae));
assertEqualInt(2048, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(2, archive_entry_atime(ae));
assertEqualInt(3, archive_entry_birthtime(ae));
assertEqualInt(4, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("dir0/dir1/dir2/dir3/dir4",
archive_entry_pathname(ae));
assert((S_IFDIR | 0555) == archive_entry_mode(ae));
assertEqualInt(2048, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(2, archive_entry_atime(ae));
assertEqualInt(3, archive_entry_birthtime(ae));
assertEqualInt(4, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("dir0/dir1/dir2/dir3/dir4/dir5",
archive_entry_pathname(ae));
assert((S_IFDIR | 0555) == archive_entry_mode(ae));
assertEqualInt(2048, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(2, archive_entry_atime(ae));
assertEqualInt(3, archive_entry_birthtime(ae));
assertEqualInt(4, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("dir0/dir1/dir2/dir3/dir4/dir5/dir6",
archive_entry_pathname(ae));
assert((S_IFDIR | 0555) == archive_entry_mode(ae));
assertEqualInt(2048, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(2, archive_entry_atime(ae));
assertEqualInt(3, archive_entry_birthtime(ae));
assertEqualInt(4, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("dir0/dir1/dir2/dir3/dir4/dir5/dir6/dir7",
archive_entry_pathname(ae));
assert((S_IFDIR | 0555) == archive_entry_mode(ae));
assertEqualInt(2048, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(2, archive_entry_atime(ae));
assertEqualInt(3, archive_entry_birthtime(ae));
assertEqualInt(4, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("dir0/dir1/dir2/dir3/dir4/dir5/dir6/dir7/dir8",
archive_entry_pathname(ae));
assert((S_IFDIR | 0555) == archive_entry_mode(ae));
assertEqualInt(2048, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(2, archive_entry_atime(ae));
assertEqualInt(3, archive_entry_birthtime(ae));
assertEqualInt(4, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("dir0/dir1/dir2/dir3/dir4/dir5/dir6/dir7/dir8/dir9",
archive_entry_pathname(ae));
assert((S_IFDIR | 0555) == archive_entry_mode(ae));
assertEqualInt(2048, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(2, archive_entry_atime(ae));
assertEqualInt(3, archive_entry_birthtime(ae));
assertEqualInt(4, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("dir0/dir1/dir2/dir3/dir4/dir5/dir6/dir7/dir8/dir9/dirA",
archive_entry_pathname(ae));
assert((S_IFDIR | 0555) == archive_entry_mode(ae));
assertEqualInt(2048, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(2, archive_entry_atime(ae));
assertEqualInt(3, archive_entry_birthtime(ae));
assertEqualInt(4, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("dir0/dir1/dir2/dir3/dir4/dir5/dir6/dir7/dir8/dir9/dirA/dirB",
archive_entry_pathname(ae));
assert((S_IFDIR | 0555) == archive_entry_mode(ae));
assertEqualInt(2048, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(2, archive_entry_atime(ae));
assertEqualInt(3, archive_entry_birthtime(ae));
assertEqualInt(4, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("dir0/dir1/dir2/dir3/dir4/dir5/dir6/dir7/dir8/dir9/dirA/dirB/dirC",
archive_entry_pathname(ae));
assert((S_IFDIR | 0555) == archive_entry_mode(ae));
assertEqualInt(2048, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(2, archive_entry_atime(ae));
assertEqualInt(3, archive_entry_birthtime(ae));
assertEqualInt(4, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("hardlnk", archive_entry_pathname(ae));
assert((AE_IFREG | 0555) == archive_entry_mode(ae));
assertEqualInt(2, archive_entry_nlink(ae));
assertEqualInt(8, archive_entry_size(ae));
assertEqualIntA(a, 8, archive_read_data(a, buff2, 10));
assertEqualMem(buff2, "12345678", 8);




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(2, archive_entry_atime(ae));
assertEqualInt(3, archive_entry_birthtime(ae));
assertEqualInt(4, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("file", archive_entry_pathname(ae));
assertEqualString("hardlnk", archive_entry_hardlink(ae));
assert((AE_IFREG | 0555) == archive_entry_mode(ae));
assertEqualInt(2, archive_entry_nlink(ae));
assertEqualInt(0, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assert(archive_entry_atime_is_set(ae));
assertEqualInt(2, archive_entry_atime(ae));

assert(!archive_entry_birthtime_is_set(ae));
assertEqualInt(0, archive_entry_birthtime(ae));
assert(archive_entry_ctime_is_set(ae));
assertEqualInt(4, archive_entry_ctime(ae));
assert(archive_entry_mtime_is_set(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString(longname, archive_entry_pathname(ae));
#if !defined(_WIN32) && !defined(__CYGWIN__)
assert((AE_IFREG | 0444) == archive_entry_mode(ae));
#else

assert((AE_IFREG | 0555) == archive_entry_mode(ae));
#endif
assertEqualInt(8, archive_entry_size(ae));
assertEqualIntA(a, 8, archive_read_data(a, buff2, 10));
assertEqualMem(buff2, "12345678", 8);




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(2, archive_entry_atime(ae));
assertEqualInt(3, archive_entry_birthtime(ae));
assertEqualInt(4, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("dir0/dir1/dir2/dir3/dir4/dir5/dir6/dir7/dir8/dir9/dirA/dirB/dirC/file", archive_entry_pathname(ae));
assert((AE_IFREG | 0555) == archive_entry_mode(ae));
assertEqualInt(1, archive_entry_nlink(ae));
assertEqualInt(8, archive_entry_size(ae));
assertEqualIntA(a, 8, archive_read_data(a, buff2, 10));
assertEqualMem(buff2, "12345678", 8);




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(2, archive_entry_atime(ae));
assertEqualInt(3, archive_entry_birthtime(ae));
assertEqualInt(4, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("dir0/dir1/file1", archive_entry_pathname(ae));
assert((AE_IFREG | 0555) == archive_entry_mode(ae));
assertEqualInt(1, archive_entry_nlink(ae));
assertEqualInt(8, archive_entry_size(ae));
assertEqualIntA(a, 8, archive_read_data(a, buff2, 10));
assertEqualMem(buff2, "12345678", 8);




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(2, archive_entry_atime(ae));
assertEqualInt(3, archive_entry_birthtime(ae));
assertEqualInt(4, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("dir0/dir1/file2", archive_entry_pathname(ae));
assert((AE_IFREG | 0555) == archive_entry_mode(ae));
assertEqualInt(1, archive_entry_nlink(ae));
assertEqualInt(8, archive_entry_size(ae));
assertEqualIntA(a, 8, archive_read_data(a, buff2, 10));
assertEqualMem(buff2, "12345678", 8);




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assert(archive_entry_atime_is_set(ae));
assertEqualInt(2, archive_entry_atime(ae));
assert(!archive_entry_birthtime_is_set(ae));
assertEqualInt(0, archive_entry_birthtime(ae));
assert(archive_entry_ctime_is_set(ae));
assertEqualInt(4, archive_entry_ctime(ae));
assert(archive_entry_mtime_is_set(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("symlnk", archive_entry_pathname(ae));
assert((AE_IFLNK | 0555) == archive_entry_mode(ae));
assertEqualString(longname, archive_entry_symlink(ae));




assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_free(a));







assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, 0, archive_read_support_format_all(a));
assertEqualIntA(a, 0, archive_read_support_filter_all(a));

assertEqualInt(ARCHIVE_OK,
archive_read_set_option(a, NULL, "rockridge", NULL));
assertEqualIntA(a, 0, archive_read_open_memory(a, buff, used));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(archive_entry_atime(ae), archive_entry_ctime(ae));
assertEqualInt(archive_entry_atime(ae), archive_entry_mtime(ae));
assertEqualString(".", archive_entry_pathname(ae));
assert((S_IFDIR | 0700) == archive_entry_mode(ae));
assertEqualInt(2048, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(5, archive_entry_atime(ae));
assertEqualInt(5, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("dir0", archive_entry_pathname(ae));
assert((S_IFDIR | 0700) == archive_entry_mode(ae));
assertEqualInt(2048, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(5, archive_entry_atime(ae));
assertEqualInt(5, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("dir0/dir1", archive_entry_pathname(ae));
assert((S_IFDIR | 0700) == archive_entry_mode(ae));
assertEqualInt(2048, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(5, archive_entry_atime(ae));
assertEqualInt(5, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("dir0/dir1/dir2", archive_entry_pathname(ae));
assert((S_IFDIR | 0700) == archive_entry_mode(ae));
assertEqualInt(2048, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(5, archive_entry_atime(ae));
assertEqualInt(5, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("dir0/dir1/dir2/dir3",
archive_entry_pathname(ae));
assert((S_IFDIR | 0700) == archive_entry_mode(ae));
assertEqualInt(2048, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(5, archive_entry_atime(ae));
assertEqualInt(5, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("dir0/dir1/dir2/dir3/dir4",
archive_entry_pathname(ae));
assert((S_IFDIR | 0700) == archive_entry_mode(ae));
assertEqualInt(2048, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(5, archive_entry_atime(ae));
assertEqualInt(5, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("dir0/dir1/dir2/dir3/dir4/dir5",
archive_entry_pathname(ae));
assert((S_IFDIR | 0700) == archive_entry_mode(ae));
assertEqualInt(2048, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(5, archive_entry_atime(ae));
assertEqualInt(5, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("dir0/dir1/dir2/dir3/dir4/dir5/dir6",
archive_entry_pathname(ae));
assert((S_IFDIR | 0700) == archive_entry_mode(ae));
assertEqualInt(2048, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(5, archive_entry_atime(ae));
assertEqualInt(5, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("dir0/dir1/dir2/dir3/dir4/dir5/dir6/dir7",
archive_entry_pathname(ae));
assert((S_IFDIR | 0700) == archive_entry_mode(ae));
assertEqualInt(2048, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(5, archive_entry_atime(ae));
assertEqualInt(5, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("dir0/dir1/dir2/dir3/dir4/dir5/dir6/dir7/dir8",
archive_entry_pathname(ae));
assert((S_IFDIR | 0700) == archive_entry_mode(ae));
assertEqualInt(2048, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(5, archive_entry_atime(ae));
assertEqualInt(5, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("dir0/dir1/dir2/dir3/dir4/dir5/dir6/dir7/dir8/dir9",
archive_entry_pathname(ae));
assert((S_IFDIR | 0700) == archive_entry_mode(ae));
assertEqualInt(2048, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(5, archive_entry_atime(ae));
assertEqualInt(5, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("dir0/dir1/dir2/dir3/dir4/dir5/dir6/dir7/dir8/dir9/dirA",
archive_entry_pathname(ae));
assert((S_IFDIR | 0700) == archive_entry_mode(ae));
assertEqualInt(2048, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(5, archive_entry_atime(ae));
assertEqualInt(5, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("dir0/dir1/dir2/dir3/dir4/dir5/dir6/dir7/dir8/dir9/dirA/dirB",
archive_entry_pathname(ae));
assert((S_IFDIR | 0700) == archive_entry_mode(ae));
assertEqualInt(2048, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(5, archive_entry_atime(ae));
assertEqualInt(5, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("dir0/dir1/dir2/dir3/dir4/dir5/dir6/dir7/dir8/dir9/dirA/dirB/dirC",
archive_entry_pathname(ae));
assert((S_IFDIR | 0700) == archive_entry_mode(ae));
assertEqualInt(2048, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(5, archive_entry_atime(ae));
assertEqualInt(5, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("file", archive_entry_pathname(ae));
assert((AE_IFREG | 0400) == archive_entry_mode(ae));
assertEqualInt(8, archive_entry_size(ae));
assertEqualIntA(a, 8, archive_read_data(a, buff2, 10));
assertEqualMem(buff2, "12345678", 8);




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(5, archive_entry_atime(ae));
assertEqualInt(5, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("hardlnk", archive_entry_pathname(ae));
assertEqualString("file", archive_entry_hardlink(ae));
assert((AE_IFREG | 0400) == archive_entry_mode(ae));
assertEqualInt(2, archive_entry_nlink(ae));
assertEqualInt(0, archive_entry_size(ae));
assertEqualIntA(a, 0, archive_read_data(a, buff2, 10));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assert(archive_entry_atime_is_set(ae));
assertEqualInt(5, archive_entry_atime(ae));
assert(archive_entry_ctime_is_set(ae));
assertEqualInt(5, archive_entry_ctime(ae));
assert(archive_entry_mtime_is_set(ae));
assertEqualInt(5, archive_entry_mtime(ae));

longname[64] = '\0';
assertEqualString(longname, archive_entry_pathname(ae));
assert((AE_IFREG | 0400) == archive_entry_mode(ae));
assertEqualInt(8, archive_entry_size(ae));
assertEqualIntA(a, 8, archive_read_data(a, buff2, 10));
assertEqualMem(buff2, "12345678", 8);




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(5, archive_entry_atime(ae));
assertEqualInt(5, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString(
"dir0/dir1/dir2/dir3/dir4/dir5/dir6/dir7/dir8/dir9/dirA/dirB/dirC/file",
archive_entry_pathname(ae));
assert((AE_IFREG | 0400) == archive_entry_mode(ae));
assertEqualInt(1, archive_entry_nlink(ae));
assertEqualInt(8, archive_entry_size(ae));
assertEqualIntA(a, 8, archive_read_data(a, buff2, 10));
assertEqualMem(buff2, "12345678", 8);




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(5, archive_entry_atime(ae));
assertEqualInt(5, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("dir0/dir1/file1", archive_entry_pathname(ae));
assert((AE_IFREG | 0400) == archive_entry_mode(ae));
assertEqualInt(1, archive_entry_nlink(ae));
assertEqualInt(8, archive_entry_size(ae));
assertEqualIntA(a, 8, archive_read_data(a, buff2, 10));
assertEqualMem(buff2, "12345678", 8);




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(5, archive_entry_atime(ae));
assertEqualInt(5, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("dir0/dir1/file2", archive_entry_pathname(ae));
assert((AE_IFREG | 0400) == archive_entry_mode(ae));
assertEqualInt(1, archive_entry_nlink(ae));
assertEqualInt(8, archive_entry_size(ae));
assertEqualIntA(a, 8, archive_read_data(a, buff2, 10));
assertEqualMem(buff2, "12345678", 8);




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assert(archive_entry_atime_is_set(ae));
assertEqualInt(5, archive_entry_atime(ae));
assert(!archive_entry_birthtime_is_set(ae));
assertEqualInt(0, archive_entry_birthtime(ae));
assert(archive_entry_ctime_is_set(ae));
assertEqualInt(5, archive_entry_ctime(ae));
assert(archive_entry_mtime_is_set(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assert((AE_IFREG | 0400) == archive_entry_mode(ae));
assertEqualInt(0, archive_entry_size(ae));




assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_free(a));








assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, 0, archive_read_support_format_all(a));
assertEqualIntA(a, 0, archive_read_support_filter_all(a));

assertEqualInt(ARCHIVE_OK,
archive_read_set_option(a, NULL, "rockridge", NULL));
assertEqualInt(ARCHIVE_OK,
archive_read_set_option(a, NULL, "joliet", NULL));
assertEqualIntA(a, 0, archive_read_open_memory(a, buff, used));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(archive_entry_atime(ae), archive_entry_ctime(ae));
assertEqualInt(archive_entry_atime(ae), archive_entry_mtime(ae));
assertEqualString(".", archive_entry_pathname(ae));
assert((S_IFDIR | 0700) == archive_entry_mode(ae));
assertEqualInt(2048, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(archive_entry_atime(ae), archive_entry_ctime(ae));
assertEqualInt(archive_entry_atime(ae), archive_entry_mtime(ae));
assertEqualString("RR_MOVED", archive_entry_pathname(ae));
assert((S_IFDIR | 0700) == archive_entry_mode(ae));
assertEqualInt(2048, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(5, archive_entry_atime(ae));
assertEqualInt(5, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("RR_MOVED/DIR7", archive_entry_pathname(ae));
assert((S_IFDIR | 0700) == archive_entry_mode(ae));
assertEqualInt(2048, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(5, archive_entry_atime(ae));
assertEqualInt(5, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("RR_MOVED/DIR7/DIR8", archive_entry_pathname(ae));
assert((S_IFDIR | 0700) == archive_entry_mode(ae));
assertEqualInt(2048, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(5, archive_entry_atime(ae));
assertEqualInt(5, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("RR_MOVED/DIR7/DIR8/DIR9",
archive_entry_pathname(ae));
assert((S_IFDIR | 0700) == archive_entry_mode(ae));
assertEqualInt(2048, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(5, archive_entry_atime(ae));
assertEqualInt(5, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("RR_MOVED/DIR7/DIR8/DIR9/DIRA",
archive_entry_pathname(ae));
assert((S_IFDIR | 0700) == archive_entry_mode(ae));
assertEqualInt(2048, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(5, archive_entry_atime(ae));
assertEqualInt(5, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("RR_MOVED/DIR7/DIR8/DIR9/DIRA/DIRB",
archive_entry_pathname(ae));
assert((S_IFDIR | 0700) == archive_entry_mode(ae));
assertEqualInt(2048, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(5, archive_entry_atime(ae));
assertEqualInt(5, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("RR_MOVED/DIR7/DIR8/DIR9/DIRA/DIRB/DIRC",
archive_entry_pathname(ae));
assert((S_IFDIR | 0700) == archive_entry_mode(ae));
assertEqualInt(2048, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(5, archive_entry_atime(ae));
assertEqualInt(5, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("DIR0", archive_entry_pathname(ae));
assert((S_IFDIR | 0700) == archive_entry_mode(ae));
assertEqualInt(2048, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(5, archive_entry_atime(ae));
assertEqualInt(5, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("DIR0/DIR1", archive_entry_pathname(ae));
assert((S_IFDIR | 0700) == archive_entry_mode(ae));
assertEqualInt(2048, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(5, archive_entry_atime(ae));
assertEqualInt(5, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("DIR0/DIR1/DIR2", archive_entry_pathname(ae));
assert((S_IFDIR | 0700) == archive_entry_mode(ae));
assertEqualInt(2048, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(5, archive_entry_atime(ae));
assertEqualInt(5, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("DIR0/DIR1/DIR2/DIR3",
archive_entry_pathname(ae));
assert((S_IFDIR | 0700) == archive_entry_mode(ae));
assertEqualInt(2048, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(5, archive_entry_atime(ae));
assertEqualInt(5, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("DIR0/DIR1/DIR2/DIR3/DIR4",
archive_entry_pathname(ae));
assert((S_IFDIR | 0700) == archive_entry_mode(ae));
assertEqualInt(2048, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(5, archive_entry_atime(ae));
assertEqualInt(5, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("DIR0/DIR1/DIR2/DIR3/DIR4/DIR5",
archive_entry_pathname(ae));
assert((S_IFDIR | 0700) == archive_entry_mode(ae));
assertEqualInt(2048, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(5, archive_entry_atime(ae));
assertEqualInt(5, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("DIR0/DIR1/DIR2/DIR3/DIR4/DIR5/DIR6",
archive_entry_pathname(ae));
assert((S_IFDIR | 0700) == archive_entry_mode(ae));
assertEqualInt(2048, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(5, archive_entry_atime(ae));
assertEqualInt(5, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("HARDLNK", archive_entry_pathname(ae));
assertEqualString(NULL, archive_entry_hardlink(ae));
assert((AE_IFREG | 0400) == archive_entry_mode(ae));
assertEqualInt(8, archive_entry_size(ae));
assertEqualIntA(a, 8, archive_read_data(a, buff2, 10));
assertEqualMem(buff2, "12345678", 8);




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(5, archive_entry_atime(ae));
assertEqualInt(0, archive_entry_birthtime(ae));
assertEqualInt(5, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("FILE", archive_entry_pathname(ae));
assertEqualString("HARDLNK", archive_entry_hardlink(ae));
assert((AE_IFREG | 0400) == archive_entry_mode(ae));
assertEqualInt(2, archive_entry_nlink(ae));
assertEqualInt(0, archive_entry_size(ae));
assertEqualIntA(a, 0, archive_read_data(a, buff2, 10));





assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assert(archive_entry_atime_is_set(ae));
assertEqualInt(5, archive_entry_atime(ae));
assert(archive_entry_ctime_is_set(ae));
assertEqualInt(5, archive_entry_ctime(ae));
assert(archive_entry_mtime_is_set(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("LONGNAME", archive_entry_pathname(ae));
assert((AE_IFREG | 0400) == archive_entry_mode(ae));
assertEqualInt(8, archive_entry_size(ae));
assertEqualIntA(a, 8, archive_read_data(a, buff2, 10));
assertEqualMem(buff2, "12345678", 8);




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(5, archive_entry_atime(ae));
assertEqualInt(5, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString(
"RR_MOVED/DIR7/DIR8/DIR9/DIRA/DIRB/DIRC/FILE",
archive_entry_pathname(ae));
assert((AE_IFREG | 0400) == archive_entry_mode(ae));
assertEqualInt(1, archive_entry_nlink(ae));
assertEqualInt(8, archive_entry_size(ae));
assertEqualIntA(a, 8, archive_read_data(a, buff2, 10));
assertEqualMem(buff2, "12345678", 8);




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(5, archive_entry_atime(ae));
assertEqualInt(5, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("DIR0/DIR1/FILE1", archive_entry_pathname(ae));
assert((AE_IFREG | 0400) == archive_entry_mode(ae));
assertEqualInt(1, archive_entry_nlink(ae));
assertEqualInt(8, archive_entry_size(ae));
assertEqualIntA(a, 8, archive_read_data(a, buff2, 10));
assertEqualMem(buff2, "12345678", 8);




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(5, archive_entry_atime(ae));
assertEqualInt(5, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("DIR0/DIR1/FILE2", archive_entry_pathname(ae));
assert((AE_IFREG | 0400) == archive_entry_mode(ae));
assertEqualInt(1, archive_entry_nlink(ae));
assertEqualInt(8, archive_entry_size(ae));
assertEqualIntA(a, 8, archive_read_data(a, buff2, 10));
assertEqualMem(buff2, "12345678", 8);




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(5, archive_entry_atime(ae));
assertEqualInt(5, archive_entry_ctime(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("DIR0/DIR1/DIR2/DIR3/DIR4/DIR5/DIR6/DIR7",
archive_entry_pathname(ae));
assert((AE_IFREG | 0400) == archive_entry_mode(ae));
assertEqualInt(0, archive_entry_size(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assert(archive_entry_atime_is_set(ae));
assertEqualInt(5, archive_entry_atime(ae));
assert(!archive_entry_birthtime_is_set(ae));
assertEqualInt(0, archive_entry_birthtime(ae));
assert(archive_entry_ctime_is_set(ae));
assertEqualInt(5, archive_entry_ctime(ae));
assert(archive_entry_mtime_is_set(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualString("SYMLNK", archive_entry_pathname(ae));
assert((AE_IFREG | 0400) == archive_entry_mode(ae));
assertEqualInt(0, archive_entry_size(ae));




assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_free(a));

free(buff);
}
