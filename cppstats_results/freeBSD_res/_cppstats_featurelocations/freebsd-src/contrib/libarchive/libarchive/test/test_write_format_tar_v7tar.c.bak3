
























#include "test.h"
__FBSDID("$FreeBSD$");

static int
is_null(const char *p, size_t l)
{
while (l > 0) {
if (*p != '\0')
return (0);
--l;
++p;
}
return (1);
}





#define myAssertEqualMem(a,b,s) assertEqualMem(a, b, s); memset(a, 0, s)





DEFINE_TEST(test_write_format_tar_v7tar)
{
struct archive *a;
struct archive_entry *entry;
char *buff, *e;
size_t buffsize = 100000;
size_t used;
int i;
char f99[100];
char f100[101];

for (i = 0; i < 99; ++i)
f99[i] = 'a' + i % 26;
f99[99] = '\0';

for (i = 0; i < 100; ++i)
f100[i] = 'A' + i % 26;
f100[100] = '\0';

buff = malloc(buffsize);


assert((a = archive_write_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK,
archive_write_set_format_v7tar(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_add_filter_none(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_open_memory(a, buff, buffsize, &used));







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
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, entry));
archive_entry_free(entry);
assertEqualIntA(a, 10, archive_write_data(a, "1234567890", 10));


assert((entry = archive_entry_new()) != NULL);
archive_entry_set_mtime(entry, 1, 10);
archive_entry_set_pathname(entry, "linkfile");
archive_entry_set_hardlink(entry, "file");
archive_entry_set_mode(entry, S_IFREG | 0664);
archive_entry_set_size(entry, 10);
archive_entry_set_uid(entry, 80);
archive_entry_set_gid(entry, 90);
archive_entry_set_dev(entry, 12);
archive_entry_set_ino(entry, 89);
archive_entry_set_nlink(entry, 2);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, entry));
archive_entry_free(entry);

assertEqualIntA(a, 0, archive_write_data(a, "1234567890", 10));


assert((entry = archive_entry_new()) != NULL);
archive_entry_set_mtime(entry, 2, 20);
archive_entry_set_pathname(entry, "dir");
archive_entry_set_mode(entry, S_IFDIR | 0775);
archive_entry_set_size(entry, 10);
archive_entry_set_nlink(entry, 2);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, entry));
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
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, entry));
archive_entry_free(entry);


assert((entry = archive_entry_new()) != NULL);
archive_entry_set_mtime(entry, 1, 10);
archive_entry_set_pathname(entry, f99);
archive_entry_set_mode(entry, S_IFREG | 0664);
archive_entry_set_size(entry, 0);
archive_entry_set_uid(entry, 82);
archive_entry_set_gid(entry, 93);
archive_entry_set_dev(entry, 102);
archive_entry_set_ino(entry, 7);
archive_entry_set_nlink(entry, 1);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, entry));
archive_entry_free(entry);


assert((entry = archive_entry_new()) != NULL);
archive_entry_set_mtime(entry, 1, 10);
archive_entry_set_pathname(entry, f100);
archive_entry_set_mode(entry, S_IFREG | 0664);
archive_entry_set_size(entry, 0);
archive_entry_set_uid(entry, 82);
archive_entry_set_gid(entry, 93);
archive_entry_set_dev(entry, 102);
archive_entry_set_ino(entry, 7);
archive_entry_set_nlink(entry, 1);
failure("100-char filename should be rejected");
assertEqualIntA(a, ARCHIVE_FAILED, archive_write_header(a, entry));
archive_entry_free(entry);


assertEqualInt(ARCHIVE_OK, archive_write_free(a));




e = buff;


myAssertEqualMem(e + 0, "file", 5);
myAssertEqualMem(e + 100, "000664 ", 8);
myAssertEqualMem(e + 108, "000120 ", 8);
myAssertEqualMem(e + 116, "000132 ", 8);
myAssertEqualMem(e + 124, "00000000012 ", 12);
myAssertEqualMem(e + 136, "00000000001 ", 12);
myAssertEqualMem(e + 148, "005335\0 ", 8);
myAssertEqualMem(e + 156, "", 1);
myAssertEqualMem(e + 157, "", 1);
assert(is_null(e + 0, 512));
myAssertEqualMem(e + 512, "1234567890", 10);
assert(is_null(e + 512, 512));
e += 1024;


myAssertEqualMem(e + 0, "linkfile", 9);
myAssertEqualMem(e + 100, "000664 ", 8);
myAssertEqualMem(e + 108, "000120 ", 8);
myAssertEqualMem(e + 116, "000132 ", 8);
myAssertEqualMem(e + 124, "00000000000 ", 12);
myAssertEqualMem(e + 136, "00000000001 ", 12);
myAssertEqualMem(e + 148, "007131\0 ", 8);
myAssertEqualMem(e + 156, "1", 1);
myAssertEqualMem(e + 157, "file", 5);
assert(is_null(e + 0, 512));
e += 512;


myAssertEqualMem(e + 0, "dir/", 4);
myAssertEqualMem(e + 100, "000775 ", 8);
myAssertEqualMem(e + 108, "000000 ", 8);
myAssertEqualMem(e + 116, "000000 ", 8);
myAssertEqualMem(e + 124, "00000000000 ", 12);
myAssertEqualMem(e + 136, "00000000002 ", 12);
myAssertEqualMem(e + 148, "005243\0 ", 8);
myAssertEqualMem(e + 156, "", 1);
myAssertEqualMem(e + 157, "", 1);
assert(is_null(e + 0, 512));
e += 512;


myAssertEqualMem(e + 0, "symlink", 8);
myAssertEqualMem(e + 100, "000664 ", 8);
myAssertEqualMem(e + 108, "000130 ", 8);
myAssertEqualMem(e + 116, "000142 ", 8);
myAssertEqualMem(e + 124, "00000000000 ", 12);
myAssertEqualMem(e + 136, "00000000003 ", 12);
myAssertEqualMem(e + 148, "007027\0 ", 8);
myAssertEqualMem(e + 156, "2", 1);
myAssertEqualMem(e + 157, "file", 5);
assert(is_null(e + 0, 512));
e += 512;


myAssertEqualMem(e + 0, f99, 100);
myAssertEqualMem(e + 100, "000664 ", 8);
myAssertEqualMem(e + 108, "000122 ", 8);
myAssertEqualMem(e + 116, "000135 ", 8);
myAssertEqualMem(e + 124, "00000000000 ", 12);
myAssertEqualMem(e + 136, "00000000001 ", 12);
myAssertEqualMem(e + 148, "031543\0 ", 8);
myAssertEqualMem(e + 156, "", 1);
myAssertEqualMem(e + 157, "", 1);
assert(is_null(e + 0, 512));
e += 512;




assert(is_null(e, 1024));
e += 1024;

assertEqualInt((int)used, e - buff);

free(buff);
}
