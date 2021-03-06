
























#include "test.h"

















static void verify(struct archive *a, int streaming) {
struct archive_entry *ae;

assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("abc", archive_entry_pathname(ae));
assertEqualInt(AE_IFREG | 0664, archive_entry_mode(ae));

assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
if (streaming) {

assertEqualString("def", archive_entry_pathname(ae));
assertEqualInt(AE_IFREG | 0664, archive_entry_mode(ae));
} else {

assertEqualString("def/", archive_entry_pathname(ae));
assertEqualInt(AE_IFDIR | 0775, archive_entry_mode(ae));
}

assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("def/foo", archive_entry_pathname(ae));
assertEqualInt(AE_IFREG | 0664, archive_entry_mode(ae));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("ghi/", archive_entry_pathname(ae));
assertEqualInt(AE_IFDIR | 0775, archive_entry_mode(ae));



assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));

assertEqualString("jkl/", archive_entry_pathname(ae));
assertEqualInt(AE_IFDIR | 0775, archive_entry_mode(ae));



assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("mno/", archive_entry_pathname(ae));
assertEqualInt(AE_IFDIR | 0775, archive_entry_mode(ae));

assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));
}

DEFINE_TEST(test_read_format_zip_msdos)
{
const char *refname = "test_read_format_zip_msdos.zip";
struct archive *a;
char *p;
size_t s;

extract_reference_file(refname);


assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_open_filename(a, refname, 17));
verify(a, 0);
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));


p = slurpfile(&s, "%s", refname);
assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, read_open_memory(a, p, s, 31));
verify(a, 1);
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));

free(p);
}
