























#include "test.h"
__FBSDID("$FreeBSD$");


static int
is_octal(const char *p, size_t l)
{
while (l > 0) {
if (*p < '0' || *p > '7')
return (0);
--l;
++p;
}
return (1);
}





DEFINE_TEST(test_write_format_cpio_odc)
{
struct archive *a;
struct archive_entry *entry;
char *buff, *e, *file;
size_t buffsize = 100000;
size_t used;

buff = malloc(buffsize);


assert((a = archive_write_new()) != NULL);
assertEqualIntA(a, 0, archive_write_set_format_cpio_odc(a));
assertEqualIntA(a, 0, archive_write_add_filter_none(a));
assertEqualIntA(a, 0, archive_write_open_memory(a, buff, buffsize, &used));







assert((entry = archive_entry_new()) != NULL);
archive_entry_set_mtime(entry, 1, 10);
archive_entry_set_pathname(entry, "file");
archive_entry_set_mode(entry, S_IFREG | 0664);
archive_entry_set_size(entry, 10);
archive_entry_set_uid(entry, 80);
archive_entry_set_gid(entry, 90);
archive_entry_set_dev(entry, 12);
archive_entry_set_ino(entry, 89);
archive_entry_set_nlink(entry, 2);
assertEqualIntA(a, 0, archive_write_header(a, entry));
archive_entry_free(entry);
assertEqualIntA(a, 10, archive_write_data(a, "1234567890", 10));


assert((entry = archive_entry_new()) != NULL);
archive_entry_set_mtime(entry, 1, 10);
archive_entry_set_pathname(entry, "linkfile");
archive_entry_set_mode(entry, S_IFREG | 0664);
archive_entry_set_size(entry, 10);
archive_entry_set_uid(entry, 80);
archive_entry_set_gid(entry, 90);
archive_entry_set_dev(entry, 12);
archive_entry_set_ino(entry, 89);
archive_entry_set_nlink(entry, 2);
assertEqualIntA(a, 0, archive_write_header(a, entry));
archive_entry_free(entry);
assertEqualIntA(a, 10, archive_write_data(a, "1234567890", 10));


assert((entry = archive_entry_new()) != NULL);
archive_entry_set_mtime(entry, 2, 20);
archive_entry_set_pathname(entry, "dir");
archive_entry_set_mode(entry, S_IFDIR | 0775);
archive_entry_set_size(entry, 10);
archive_entry_set_nlink(entry, 2);
assertEqualIntA(a, 0, archive_write_header(a, entry));
archive_entry_free(entry);

assertEqualIntA(a, 0, archive_write_data(a, "1234567890", 10));


assert((entry = archive_entry_new()) != NULL);
archive_entry_set_mtime(entry, 3, 30);
archive_entry_set_pathname(entry, "symlink");
archive_entry_set_mode(entry, 0664);
archive_entry_set_filetype(entry, AE_IFLNK);
archive_entry_set_symlink(entry,"file");
archive_entry_set_size(entry, 0);
archive_entry_set_uid(entry, 88);
archive_entry_set_gid(entry, 98);
archive_entry_set_dev(entry, 12);
archive_entry_set_ino(entry, 90);
archive_entry_set_nlink(entry, 1);
assertEqualIntA(a, 0, archive_write_header(a, entry));
archive_entry_free(entry);

assertEqualIntA(a, 0, archive_write_data(a, "1234567890", 10));

assertEqualInt(ARCHIVE_OK, archive_write_free(a));


















e = buff;


file = e;
assert(is_octal(e, 76));
assertEqualMem(e + 0, "070707", 6);
assertEqualMem(e + 6, "000014", 6);
assert(memcmp(e + 12, "000000", 6) != 0);
assertEqualMem(e + 18, "100664", 6);
assertEqualMem(e + 24, "000120", 6);
assertEqualMem(e + 30, "000132", 6);
assertEqualMem(e + 36, "000002", 6);
assertEqualMem(e + 42, "000000", 6);
assertEqualMem(e + 48, "00000000001", 11);
assertEqualMem(e + 59, "000005", 6);
assertEqualMem(e + 65, "00000000012", 11);
assertEqualMem(e + 76, "file\0", 5);
assertEqualMem(e + 81, "1234567890", 10);
e += 91;


assert(is_octal(e, 76));
assertEqualMem(e + 0, "070707", 6);
assertEqualMem(e + 6, "000014", 6);
assertEqualMem(e + 12, file + 12, 6);
assertEqualMem(e + 18, "100664", 6);
assertEqualMem(e + 24, "000120", 6);
assertEqualMem(e + 30, "000132", 6);
assertEqualMem(e + 36, "000002", 6);
assertEqualMem(e + 42, "000000", 6);
assertEqualMem(e + 48, "00000000001", 11);
assertEqualMem(e + 59, "000011", 6);
assertEqualMem(e + 65, "00000000012", 11);
assertEqualMem(e + 76, "linkfile\0", 9);
assertEqualMem(e + 85, "1234567890", 10);
e += 95;


assert(is_octal(e, 76));
assertEqualMem(e + 0, "070707", 6);
assertEqualMem(e + 6, "000000", 6);
assertEqualMem(e + 12, "000000", 6);
assertEqualMem(e + 18, "040775", 6);
assertEqualMem(e + 24, "000000", 6);
assertEqualMem(e + 30, "000000", 6);
assertEqualMem(e + 36, "000002", 6);
assertEqualMem(e + 42, "000000", 6);
assertEqualMem(e + 48, "00000000002", 11);
assertEqualMem(e + 59, "000004", 6);
assertEqualMem(e + 65, "00000000000", 11);
assertEqualMem(e + 76, "dir\0", 4);
e += 80;


assert(is_octal(e, 76));
assertEqualMem(e + 0, "070707", 6);
assertEqualMem(e + 6, "000014", 6);
assert(memcmp(e + 12, file + 12, 6) != 0);
assert(memcmp(e + 12, "000000", 6) != 0);
assertEqualMem(e + 18, "120664", 6);
assertEqualMem(e + 24, "000130", 6);
assertEqualMem(e + 30, "000142", 6);
assertEqualMem(e + 36, "000001", 6);
assertEqualMem(e + 42, "000000", 6);
assertEqualMem(e + 48, "00000000003", 11);
assertEqualMem(e + 59, "000010", 6);
assertEqualMem(e + 65, "00000000004", 11);
assertEqualMem(e + 76, "symlink\0", 8);
assertEqualMem(e + 84, "file", 4);
e += 88;




assert(is_octal(e, 76));
assertEqualMem(e + 0, "070707", 6);
assertEqualMem(e + 6, "000000", 6);
assertEqualMem(e + 12, "000000", 6);
assertEqualMem(e + 18, "000000", 6);
assertEqualMem(e + 24, "000000", 6);
assertEqualMem(e + 30, "000000", 6);
assertEqualMem(e + 36, "000001", 6);
assertEqualMem(e + 42, "000000", 6);
assertEqualMem(e + 48, "00000000000", 11);
assertEqualMem(e + 59, "000013", 6);
assertEqualMem(e + 65, "00000000000", 11);
assertEqualMem(e + 76, "TRAILER!!!\0", 11);
e += 87;

assertEqualInt((int)used, e - buff);

free(buff);
}
