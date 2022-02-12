























#include "test.h"
__FBSDID("$FreeBSD$");






DEFINE_TEST(test_read_format_zip_with_invalid_traditional_eocd)
{
const char *refname = "test_read_format_zip_with_invalid_traditional_eocd.zip";
char *p;
size_t s;
struct archive *a;
struct archive_entry *ae;

extract_reference_file(refname);
p = slurpfile(&s, "%s", refname);

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_zip_seekable(a));
assertEqualIntA(a, ARCHIVE_OK, read_open_memory_seek(a, p, s, 1));

assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("test1.txt", archive_entry_pathname(ae));
assertEqualInt(0, archive_entry_size(ae));

assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("test2.txt", archive_entry_pathname(ae));
assertEqualInt(0, archive_entry_size(ae));

assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_free(a));
free(p);
}
