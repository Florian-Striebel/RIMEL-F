























#include "test.h"
__FBSDID("$FreeBSD$");









static char filename[2048];

DEFINE_TEST(test_write_format_gnutar_filenames)
{
size_t buffsize = 1000000;
char *buff;
struct archive_entry *ae, *template;
struct archive *a;
size_t used;
int i;

buff = malloc(buffsize);
assert(buff != NULL);


assert((template = archive_entry_new()) != NULL);
archive_entry_set_atime(template, 2, 20);
archive_entry_set_birthtime(template, 3, 30);
archive_entry_set_ctime(template, 4, 40);
archive_entry_set_mtime(template, 5, 50);
archive_entry_set_mode(template, S_IFREG | 0755);
archive_entry_set_size(template, 8);

for (i = 0; i < 2000; ++i) {
filename[i] = 'a';
filename[i + 1] = '\0';
archive_entry_copy_pathname(template, filename);


assert((a = archive_write_new()) != NULL);
assertA(0 == archive_write_set_format_gnutar(a));
assertA(0 == archive_write_add_filter_none(a));
assertA(0 == archive_write_open_memory(a, buff, buffsize, &used));
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, template));
assertEqualIntA(a, 8, archive_write_data(a, "12345678", 9));
assertEqualIntA(a, ARCHIVE_OK, archive_write_close(a));
assertEqualIntA(a, ARCHIVE_OK, archive_write_free(a));



assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, 0, archive_read_support_format_all(a));
assertEqualIntA(a, 0, archive_read_support_filter_all(a));
assertEqualIntA(a, 0, archive_read_open_memory(a, buff, used));

assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualString(filename, archive_entry_pathname(ae));
assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_free(a));
}

archive_entry_free(template);

free(buff);
}


DEFINE_TEST(test_write_format_gnutar_linknames)
{
size_t buffsize = 1000000;
char *buff;
struct archive_entry *ae, *template;
struct archive *a;
size_t used;
int i;

#if defined(S_IFLNK)
assertEqualInt(S_IFLNK, AE_IFLNK);
#endif

buff = malloc(buffsize);
assert(buff != NULL);


assert((template = archive_entry_new()) != NULL);
archive_entry_set_atime(template, 2, 20);
archive_entry_set_birthtime(template, 3, 30);
archive_entry_set_ctime(template, 4, 40);
archive_entry_set_mtime(template, 5, 50);
archive_entry_set_mode(template, AE_IFLNK | 0755);
archive_entry_copy_pathname(template, "link");

for (i = 0; i < 2000; ++i) {
filename[i] = 'a';
filename[i + 1] = '\0';
archive_entry_copy_symlink(template, filename);


assert((a = archive_write_new()) != NULL);
assertA(0 == archive_write_set_format_gnutar(a));
assertA(0 == archive_write_add_filter_none(a));
assertA(0 == archive_write_open_memory(a, buff, buffsize, &used));
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, template));
assertEqualIntA(a, ARCHIVE_OK, archive_write_close(a));
assertEqualIntA(a, ARCHIVE_OK, archive_write_free(a));



assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, 0, archive_read_support_format_all(a));
assertEqualIntA(a, 0, archive_read_support_filter_all(a));
assertEqualIntA(a, 0, archive_read_open_memory(a, buff, used));

assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualString("link", archive_entry_pathname(ae));
assertEqualString(filename, archive_entry_symlink(ae));
assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_free(a));
}

archive_entry_free(template);

free(buff);
}
