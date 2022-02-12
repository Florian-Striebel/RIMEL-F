























#include "test.h"
__FBSDID("$FreeBSD$");

static char buff2[64];


static const char *longfilename = "abcdefghijklmnopqrstuvwxyz"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890";

static const char *longlinkname = "Xabcdefghijklmnopqrstuvwxyz"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890";

static const char *longhardlinkname = "Yabcdefghijklmnopqrstuvwxyz"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890";


DEFINE_TEST(test_write_format_gnutar)
{
size_t buffsize = 1000000;
char *buff;
struct archive_entry *ae;
struct archive *a;
size_t used;

buff = malloc(buffsize);
assert(buff != NULL);


assert((a = archive_write_new()) != NULL);
assertA(0 == archive_write_set_format_gnutar(a));
assertA(0 == archive_write_add_filter_none(a));
assertA(0 == archive_write_open_memory(a, buff, buffsize, &used));




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_atime(ae, 2, 20);
archive_entry_set_birthtime(ae, 3, 30);
archive_entry_set_ctime(ae, 4, 40);
archive_entry_set_mtime(ae, 5, 50);
archive_entry_copy_pathname(ae, "file");
archive_entry_set_mode(ae, S_IFREG | 0755);
archive_entry_set_size(ae, 8);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);
assertEqualIntA(a, 8, archive_write_data(a, "12345678", 9));




assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, longfilename);
archive_entry_set_mode(ae, S_IFREG | 0755);
archive_entry_set_size(ae, 8);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);
assertEqualIntA(a, 8, archive_write_data(a, "abcdefgh", 9));




assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, longhardlinkname);
archive_entry_copy_hardlink(ae, longfilename);
archive_entry_set_mode(ae, S_IFREG | 0755);
archive_entry_set_size(ae, 8);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);




assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, longlinkname);
archive_entry_copy_symlink(ae, longfilename);
archive_entry_set_mode(ae, AE_IFLNK | 0755);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);




assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "large_uid_gid");
archive_entry_set_mode(ae, S_IFREG | 0755);
archive_entry_set_size(ae, 8);
archive_entry_set_uid(ae, 123456789);
archive_entry_set_gid(ae, 987654321);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);
assertEqualIntA(a, 8, archive_write_data(a, "abcdefgh", 9));





assertEqualIntA(a, ARCHIVE_OK, archive_write_close(a));
assertEqualIntA(a, ARCHIVE_OK, archive_write_free(a));






assertEqualMem(buff + 257, "ustar \0", 8);

assertEqualInt(15360, used);






assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, 0, archive_read_support_format_all(a));
assertEqualIntA(a, 0, archive_read_support_filter_all(a));
assertEqualIntA(a, 0, archive_read_open_memory(a, buff, used));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assert(!archive_entry_atime_is_set(ae));
assert(!archive_entry_birthtime_is_set(ae));
assert(!archive_entry_ctime_is_set(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualInt(0, archive_entry_mtime_nsec(ae));
assertEqualString("file", archive_entry_pathname(ae));
assertEqualInt(S_IFREG | 0755, archive_entry_mode(ae));
assertEqualInt(8, archive_entry_size(ae));
assertEqualIntA(a, 8, archive_read_data(a, buff2, 10));
assertEqualMem(buff2, "12345678", 8);




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualString(longfilename, archive_entry_pathname(ae));
assertEqualInt(S_IFREG | 0755, archive_entry_mode(ae));
assertEqualInt(8, archive_entry_size(ae));
assertEqualIntA(a, 8, archive_read_data(a, buff2, 10));
assertEqualMem(buff2, "abcdefgh", 8);




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualString(longhardlinkname, archive_entry_pathname(ae));
assertEqualString(longfilename, archive_entry_hardlink(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualString(longlinkname, archive_entry_pathname(ae));
assertEqualString(longfilename, archive_entry_symlink(ae));
assertEqualInt(AE_IFLNK | 0755, archive_entry_mode(ae));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(123456789, archive_entry_uid(ae));
assertEqualInt(987654321, archive_entry_gid(ae));
assertEqualString("large_uid_gid", archive_entry_pathname(ae));
assertEqualInt(S_IFREG | 0755, archive_entry_mode(ae));




assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_free(a));

free(buff);
}
