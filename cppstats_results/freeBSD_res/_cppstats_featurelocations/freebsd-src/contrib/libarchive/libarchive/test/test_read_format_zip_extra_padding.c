























#include "test.h"

























static void verify(struct archive *a) {
struct archive_entry *ae;

assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("a", archive_entry_pathname(ae));
assertEqualInt(AE_IFREG | 0664, archive_entry_mode(ae));
assertEqualInt(0x5c1558d2, archive_entry_mtime(ae));
assertEqualInt(0, archive_entry_ctime(ae));
assertEqualInt(0x5c1558db, archive_entry_atime(ae));

assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));
}

DEFINE_TEST(test_read_format_zip_extra_padding)
{
const char *refname = "test_read_format_zip_extra_padding.zip";
struct archive *a;
char *p;
size_t s;

extract_reference_file(refname);


assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_open_filename(a, refname, 7));
verify(a);
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));


p = slurpfile(&s, "%s", refname);
assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, read_open_memory(a, p, s, 3));
verify(a);
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));

free(p);
}
