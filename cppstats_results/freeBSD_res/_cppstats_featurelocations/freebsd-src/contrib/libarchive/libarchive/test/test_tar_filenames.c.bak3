























#include "test.h"
__FBSDID("$FreeBSD$");







static void
test_filename(const char *prefix, int dlen, int flen)
{
char buff[8192];
char filename[400];
char dirname[400];
struct archive_entry *ae;
struct archive *a;
size_t used;
char *p;
int i;

p = filename;
if (prefix) {
strcpy(filename, prefix);
p += strlen(p);
}
if (dlen > 0) {
for (i = 0; i < dlen; i++)
*p++ = 'a';
*p++ = '/';
}
for (i = 0; i < flen; i++)
*p++ = 'b';
*p = '\0';

strcpy(dirname, filename);


assert((a = archive_write_new()) != NULL);
assertA(0 == archive_write_set_format_pax_restricted(a));
assertA(0 == archive_write_add_filter_none(a));
assertA(0 == archive_write_set_bytes_per_block(a,0));
assertA(0 == archive_write_open_memory(a, buff, sizeof(buff), &used));




assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, filename);
archive_entry_set_mode(ae, S_IFREG | 0755);
failure("Pathname %d/%d", dlen, flen);
assertA(0 == archive_write_header(a, ae));
archive_entry_free(ae);




assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, dirname);
archive_entry_set_mode(ae, S_IFDIR | 0755);
failure("Dirname %d/%d", dlen, flen);
assertA(0 == archive_write_header(a, ae));
archive_entry_free(ae);


strcat(dirname, "/");




assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, dirname);
archive_entry_set_mode(ae, S_IFDIR | 0755);
failure("Dirname %d/%d", dlen, flen);
assertA(0 == archive_write_header(a, ae));
archive_entry_free(ae);


assertEqualIntA(a, ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));




assert((a = archive_read_new()) != NULL);
assertA(0 == archive_read_support_format_all(a));
assertA(0 == archive_read_support_filter_all(a));
assertA(0 == archive_read_open_memory(a, buff, used));


assertA(0 == archive_read_next_header(a, &ae));
assertEqualString(filename, archive_entry_pathname(ae));
assertEqualInt((S_IFREG | 0755), archive_entry_mode(ae));









assertA(0 == archive_read_next_header(a, &ae));
assertEqualString(dirname, archive_entry_pathname(ae));
assert((S_IFDIR | 0755) == archive_entry_mode(ae));

assertA(0 == archive_read_next_header(a, &ae));
assertEqualString(dirname, archive_entry_pathname(ae));
assert((S_IFDIR | 0755) == archive_entry_mode(ae));


assert(1 == archive_read_next_header(a, &ae));
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

DEFINE_TEST(test_tar_filenames)
{
int dlen, flen;


for (dlen = 45; dlen < 55; dlen++) {
for (flen = 45; flen < 55; flen++) {
test_filename(NULL, dlen, flen);
test_filename("/", dlen, flen);
}
}

for (dlen = 0; dlen < 140; dlen += 10) {
for (flen = 98; flen < 102; flen++) {
test_filename(NULL, dlen, flen);
test_filename("/", dlen, flen);
}
}

for (dlen = 140; dlen < 160; dlen++) {
for (flen = 95; flen < 105; flen++) {
test_filename(NULL, dlen, flen);
test_filename("/", dlen, flen);
}
}
}
