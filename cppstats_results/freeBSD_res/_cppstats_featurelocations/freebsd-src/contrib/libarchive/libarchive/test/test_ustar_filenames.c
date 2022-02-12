























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
int separator = 0;
int i = 0;

if (prefix != NULL) {
strcpy(filename, prefix);
i = (int)strlen(prefix);
}
if (dlen > 0) {
for (; i < dlen; i++)
filename[i] = 'a';
filename[i++] = '/';
separator = 1;
}
for (; i < dlen + flen + separator; i++)
filename[i] = 'b';
filename[i] = '\0';

strcpy(dirname, filename);


assert((a = archive_write_new()) != NULL);
assertA(0 == archive_write_set_format_ustar(a));
assertA(0 == archive_write_add_filter_none(a));
assertA(0 == archive_write_set_bytes_per_block(a,0));
assertA(0 == archive_write_open_memory(a, buff, sizeof(buff), &used));




assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, filename);
archive_entry_set_mode(ae, S_IFREG | 0755);
failure("dlen=%d, flen=%d", dlen, flen);
if (flen > 100) {
assertEqualIntA(a, ARCHIVE_FAILED, archive_write_header(a, ae));
} else {
assertEqualIntA(a, 0, archive_write_header(a, ae));
}
archive_entry_free(ae);




assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, dirname);
archive_entry_set_mode(ae, S_IFDIR | 0755);
failure("dlen=%d, flen=%d", dlen, flen);
if (flen >= 100) {
assertEqualIntA(a, ARCHIVE_FAILED, archive_write_header(a, ae));
} else {
assertEqualIntA(a, 0, archive_write_header(a, ae));
}
archive_entry_free(ae);


strcat(dirname, "/");




assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, dirname);
archive_entry_set_mode(ae, S_IFDIR | 0755);
failure("dlen=%d, flen=%d", dlen, flen);
if (flen >= 100) {
assertEqualIntA(a, ARCHIVE_FAILED, archive_write_header(a, ae));
} else {
assertEqualIntA(a, 0, archive_write_header(a, ae));
}
archive_entry_free(ae);


assertEqualIntA(a, ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));




assert((a = archive_read_new()) != NULL);
assertA(0 == archive_read_support_format_all(a));
assertA(0 == archive_read_support_filter_all(a));
assertA(0 == archive_read_open_memory(a, buff, used));

if (flen <= 100) {

assertA(0 == archive_read_next_header(a, &ae));
failure("dlen=%d, flen=%d", dlen, flen);
assertEqualString(filename, archive_entry_pathname(ae));
assertEqualInt((S_IFREG | 0755), archive_entry_mode(ae));
}








if (flen <= 99) {
assertA(0 == archive_read_next_header(a, &ae));
assert((S_IFDIR | 0755) == archive_entry_mode(ae));
failure("dlen=%d, flen=%d", dlen, flen);
assertEqualString(dirname, archive_entry_pathname(ae));
}

if (flen <= 99) {
assertA(0 == archive_read_next_header(a, &ae));
assert((S_IFDIR | 0755) == archive_entry_mode(ae));
assertEqualString(dirname, archive_entry_pathname(ae));
}


failure("This fails if entries were written that should not have been written. dlen=%d, flen=%d", dlen, flen);
assertEqualInt(1, archive_read_next_header(a, &ae));
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

DEFINE_TEST(test_ustar_filenames)
{
int dlen, flen;






for (dlen = 5; dlen < 70; dlen += 5) {
for (flen = 100 - dlen - 5; flen < 100 - dlen + 5; flen++) {
test_filename(NULL, dlen, flen);
test_filename("/", dlen, flen);
}
}


for (flen = 90; flen < 110; flen++) {
test_filename(NULL, 0, flen);
test_filename("/", dlen, flen);
}



}
