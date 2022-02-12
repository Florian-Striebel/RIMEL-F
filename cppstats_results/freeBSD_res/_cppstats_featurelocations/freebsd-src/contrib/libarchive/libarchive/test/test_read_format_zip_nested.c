























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_read_format_zip_nested)
{
const char *refname = "test_read_format_zip_nested.zip";
char *p, *inner;
size_t s, innerLength;
struct archive *a;
struct archive_entry *ae;

extract_reference_file(refname);
p = slurpfile(&s, "%s", refname);


assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_zip(a));
assertEqualIntA(a, ARCHIVE_OK, read_open_memory_seek(a, p, s, 1));

assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("small.zip", archive_entry_pathname(ae));
assertEqualInt(211, archive_entry_size(ae));
assertEqualInt(AE_IFREG, archive_entry_filetype(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);


innerLength = (size_t)archive_entry_size(ae);
inner = calloc(innerLength, 1);
assertEqualInt(innerLength, archive_read_data(a, inner, innerLength));

assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("file.txt", archive_entry_pathname(ae));
assertEqualInt(53, archive_entry_size(ae));
assertEqualInt(AE_IFREG, archive_entry_filetype(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);


assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_free(a));

free(p);


assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_zip(a));
assertEqualIntA(a, ARCHIVE_OK, read_open_memory_seek(a, inner, innerLength, 1));

assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("another_file.txt", archive_entry_pathname(ae));
assertEqualInt(29, archive_entry_size(ae));
assertEqualInt(AE_IFREG, archive_entry_filetype(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);

assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_free(a));

free(inner);
}
