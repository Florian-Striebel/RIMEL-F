























#include "test.h"
__FBSDID("$FreeBSD$");


static int
is_hex(const char *p, size_t l)
{
while (l > 0) {
if (*p >= '0' && *p <= '9') {

} else if (*p >= 'a' && *p <= 'f') {

} else {

return (0);
}
--l;
++p;
}
return (1);
}





DEFINE_TEST(test_write_format_cpio_newc)
{
struct archive *a;
struct archive_entry *entry;
char *buff, *e, *file;
size_t buffsize = 100000;
size_t used;

buff = malloc(buffsize);


assert((a = archive_write_new()) != NULL);
assertEqualIntA(a, 0, archive_write_set_format_cpio_newc(a));
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
archive_entry_set_nlink(entry, 1);
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
archive_entry_set_pathname(entry, "lnk");
archive_entry_set_mode(entry, 0664);
archive_entry_set_filetype(entry, AE_IFLNK);
archive_entry_set_size(entry, 0);
archive_entry_set_uid(entry, 83);
archive_entry_set_gid(entry, 93);
archive_entry_set_dev(entry, 13);
archive_entry_set_ino(entry, 88);
archive_entry_set_nlink(entry, 1);
archive_entry_set_symlink(entry,"a");
assertEqualIntA(a, 0, archive_write_header(a, entry));
archive_entry_free(entry);

assertEqualInt(ARCHIVE_OK, archive_write_free(a));




e = buff;


file = e;
assert(is_hex(e, 110));
assertEqualMem(e + 0, "070701", 6);
assert(memcmp(e + 6, "00000000", 8) != 0);
assertEqualMem(e + 14, "000081b4", 8);
assertEqualMem(e + 22, "00000050", 8);
assertEqualMem(e + 30, "0000005a", 8);
assertEqualMem(e + 38, "00000001", 8);
assertEqualMem(e + 46, "00000001", 8);
assertEqualMem(e + 54, "0000000a", 8);
assertEqualMem(e + 62, "00000000", 8);
assertEqualMem(e + 70, "0000000c", 8);
assertEqualMem(e + 78, "00000000", 8);
assertEqualMem(e + 86, "00000000", 8);
assertEqualMem(e + 94, "00000005", 8);
assertEqualMem(e + 102, "00000000", 8);
assertEqualMem(e + 110, "file\0\0", 6);
assertEqualMem(e + 116, "1234567890", 10);
assertEqualMem(e + 126, "\0\0", 2);
e += 128;


assert(is_hex(e, 110));
assertEqualMem(e + 0, "070701", 6);
assertEqualMem(e + 6, "00000000", 8);
assertEqualMem(e + 14, "000041fd", 8);
assertEqualMem(e + 22, "00000000", 8);
assertEqualMem(e + 30, "00000000", 8);
assertEqualMem(e + 38, "00000002", 8);
assertEqualMem(e + 46, "00000002", 8);
assertEqualMem(e + 54, "00000000", 8);
assertEqualMem(e + 62, "00000000", 8);
assertEqualMem(e + 70, "00000000", 8);
assertEqualMem(e + 78, "00000000", 8);
assertEqualMem(e + 86, "00000000", 8);
assertEqualMem(e + 94, "00000004", 8);
assertEqualMem(e + 102, "00000000", 8);
assertEqualMem(e + 110, "dir\0", 4);
assertEqualMem(e + 114, "\0\0", 2);
e += 116;


assert(is_hex(e, 110));
assertEqualMem(e + 0, "070701", 6);
assert(memcmp(e + 6, file + 6, 8) != 0);
assert(memcmp(e + 6, "00000000", 8) != 0);
assertEqualMem(e + 14, "0000a1b4", 8);
assertEqualMem(e + 22, "00000053", 8);
assertEqualMem(e + 30, "0000005d", 8);
assertEqualMem(e + 38, "00000001", 8);
assertEqualMem(e + 46, "00000003", 8);
assertEqualMem(e + 54, "00000001", 8);
assertEqualMem(e + 62, "00000000", 8);
assertEqualMem(e + 70, "0000000d", 8);
assertEqualMem(e + 78, "00000000", 8);
assertEqualMem(e + 86, "00000000", 8);
assertEqualMem(e + 94, "00000004", 8);
assertEqualMem(e + 102, "00000000", 8);
assertEqualMem(e + 110, "lnk\0\0\0", 6);
assertEqualMem(e + 116, "a\0\0\0", 4);
e += 120;




assert(is_hex(e, 76));
assertEqualMem(e + 0, "070701", 6);
assertEqualMem(e + 6, "00000000", 8);
assertEqualMem(e + 14, "00000000", 8);
assertEqualMem(e + 22, "00000000", 8);
assertEqualMem(e + 30, "00000000", 8);
assertEqualMem(e + 38, "00000001", 8);
assertEqualMem(e + 46, "00000000", 8);
assertEqualMem(e + 54, "00000000", 8);
assertEqualMem(e + 62, "00000000", 8);
assertEqualMem(e + 70, "00000000", 8);
assertEqualMem(e + 78, "00000000", 8);
assertEqualMem(e + 86, "00000000", 8);
assertEqualMem(e + 94, "0000000b", 8);
assertEqualMem(e + 102, "00000000", 8);
assertEqualMem(e + 110, "TRAILER!!!\0", 11);
assertEqualMem(e + 121, "\0\0\0", 3);
e += 124;

assertEqualInt((int)used, e - buff);

free(buff);
}
