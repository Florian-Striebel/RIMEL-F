























#include "test.h"
__FBSDID("$FreeBSD$");

static char buff2[64];

DEFINE_TEST(test_write_format_pax)
{
size_t buffsize = 1000000;
char *buff;
struct archive_entry *ae;
struct archive *a;
size_t used;
int i;
char nulls[1024];
int64_t offset, length;

buff = malloc(buffsize);
assert(buff != NULL);


assert((a = archive_write_new()) != NULL);
assertA(0 == archive_write_set_format_pax(a));
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
archive_entry_set_atime(ae, 2, 20);
archive_entry_set_birthtime(ae, 8, 80);
archive_entry_set_ctime(ae, 4, 40);
archive_entry_set_mtime(ae, 5, 50);
archive_entry_copy_pathname(ae, "file2");
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
archive_entry_copy_pathname(ae, "file3"
"_123456789_123456789_123456789_123456789_123456789"
"_123456789_123456789_123456789_123456789_123456789"
"_123456789_123456789_123456789_123456789_123456789");
archive_entry_set_mode(ae, S_IFREG | 0755);
archive_entry_set_size(ae, 1024008);
archive_entry_sparse_add_entry(ae, 1024000, 8);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);
memset(nulls, 0, sizeof(nulls));
for (i = 0; i < 1024000; i += 1024)

assertEqualIntA(a, 1024, archive_write_data(a, nulls, 1024));
assertEqualIntA(a, 8, archive_write_data(a, "12345678", 9));





assert((ae = archive_entry_new()) != NULL);
archive_entry_set_atime(ae, 2, 20);
archive_entry_set_birthtime(ae, 3, 30);
archive_entry_set_ctime(ae, 4, 40);
archive_entry_set_mtime(ae, 5, 50);
archive_entry_copy_pathname(ae, "file4");
archive_entry_set_mode(ae, S_IFREG | 0755);
archive_entry_set_size(ae, 8);
archive_entry_copy_uname(ae,
"long-uname123456789012345678901234567890");
archive_entry_copy_gname(ae,
"long-gname123456789012345678901234567890");
archive_entry_set_uid(ae, 536870912);
archive_entry_set_gid(ae, 536870913);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);
assertEqualIntA(a, 8, archive_write_data(a, "12345678", 9));








assertEqualIntA(a, ARCHIVE_OK, archive_write_close(a));
assertEqualIntA(a, ARCHIVE_OK, archive_write_free(a));






assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, 0, archive_read_support_format_all(a));
assertEqualIntA(a, 0, archive_read_support_filter_all(a));
assertEqualIntA(a, 0, archive_read_open_memory(a, buff, used));




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(2, archive_entry_atime(ae));
assertEqualInt(20, archive_entry_atime_nsec(ae));
assertEqualInt(3, archive_entry_birthtime(ae));
assertEqualInt(30, archive_entry_birthtime_nsec(ae));
assertEqualInt(4, archive_entry_ctime(ae));
assertEqualInt(40, archive_entry_ctime_nsec(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualInt(50, archive_entry_mtime_nsec(ae));
assertEqualString("file", archive_entry_pathname(ae));
assert((S_IFREG | 0755) == archive_entry_mode(ae));
assertEqualInt(8, archive_entry_size(ae));
assertEqualIntA(a, 8, archive_read_data(a, buff2, 10));
assertEqualMem(buff2, "12345678", 8);




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assert(archive_entry_atime_is_set(ae));
assertEqualInt(2, archive_entry_atime(ae));
assertEqualInt(20, archive_entry_atime_nsec(ae));

assert(!archive_entry_birthtime_is_set(ae));
assertEqualInt(0, archive_entry_birthtime(ae));
assertEqualInt(0, archive_entry_birthtime_nsec(ae));
assert(archive_entry_ctime_is_set(ae));
assertEqualInt(4, archive_entry_ctime(ae));
assertEqualInt(40, archive_entry_ctime_nsec(ae));
assert(archive_entry_mtime_is_set(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualInt(50, archive_entry_mtime_nsec(ae));
assertEqualString("file2", archive_entry_pathname(ae));
assert((S_IFREG | 0755) == archive_entry_mode(ae));
assertEqualInt(8, archive_entry_size(ae));
assertEqualIntA(a, 8, archive_read_data(a, buff2, 10));
assertEqualMem(buff2, "12345678", 8);




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(2, archive_entry_atime(ae));
assertEqualInt(20, archive_entry_atime_nsec(ae));
assertEqualInt(3, archive_entry_birthtime(ae));
assertEqualInt(30, archive_entry_birthtime_nsec(ae));
assertEqualInt(4, archive_entry_ctime(ae));
assertEqualInt(40, archive_entry_ctime_nsec(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualInt(50, archive_entry_mtime_nsec(ae));
assertEqualString("file3"
"_123456789_123456789_123456789_123456789_123456789"
"_123456789_123456789_123456789_123456789_123456789"
"_123456789_123456789_123456789_123456789_123456789",
archive_entry_pathname(ae));
assert((S_IFREG | 0755) == archive_entry_mode(ae));
assertEqualInt(1024008, archive_entry_size(ae));
assertEqualInt(1, archive_entry_sparse_reset(ae));
assertEqualInt(ARCHIVE_OK,
archive_entry_sparse_next(ae, &offset, &length));
assertEqualInt(1024000, offset);
assertEqualInt(8, length);
for (i = 0; i < 1024000; i += 1024) {
int j;
assertEqualIntA(a, 1024, archive_read_data(a, nulls, 1024));
for (j = 0; j < 1024; j++)
assertEqualInt(0, nulls[j]);
}
assertEqualIntA(a, 8, archive_read_data(a, buff2, 10));
assertEqualMem(buff2, "12345678", 8);




assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualInt(2, archive_entry_atime(ae));
assertEqualInt(20, archive_entry_atime_nsec(ae));
assertEqualInt(3, archive_entry_birthtime(ae));
assertEqualInt(30, archive_entry_birthtime_nsec(ae));
assertEqualInt(4, archive_entry_ctime(ae));
assertEqualInt(40, archive_entry_ctime_nsec(ae));
assertEqualInt(5, archive_entry_mtime(ae));
assertEqualInt(50, archive_entry_mtime_nsec(ae));
assertEqualString("file4", archive_entry_pathname(ae));
assertEqualString("long-uname123456789012345678901234567890",
archive_entry_uname(ae));
assertEqualString("long-gname123456789012345678901234567890",
archive_entry_gname(ae));
assertEqualInt(536870912, archive_entry_uid(ae));
assertEqualInt(536870913, archive_entry_gid(ae));
assert((S_IFREG | 0755) == archive_entry_mode(ae));
assertEqualInt(8, archive_entry_size(ae));
assertEqualIntA(a, 8, archive_read_data(a, buff2, 10));
assertEqualMem(buff2, "12345678", 8);




assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_free(a));

free(buff);
}
