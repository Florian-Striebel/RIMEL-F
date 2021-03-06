























#include "test.h"
__FBSDID("$FreeBSD$");

static void
verify_padded_archive(const char *refname)
{
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
assertEqualString("file0", archive_entry_pathname(ae));
assertEqualInt(AE_IFREG | 0644, archive_entry_mode(ae));
assertEqualInt(6, archive_entry_size(ae));

assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("file1", archive_entry_pathname(ae));
assertEqualInt(AE_IFREG | 0644, archive_entry_mode(ae));
assertEqualInt(6, archive_entry_size(ae));

assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_free(a));

free(p);
}








DEFINE_TEST(test_read_format_zip_padded1)
{
const char *refname = "test_read_format_zip_padded1.zip";
verify_padded_archive(refname);
}






DEFINE_TEST(test_read_format_zip_padded2)
{
const char *refname = "test_read_format_zip_padded2.zip";
verify_padded_archive(refname);
}




DEFINE_TEST(test_read_format_zip_padded3)
{
const char *refname = "test_read_format_zip_padded3.zip";
verify_padded_archive(refname);
}

