























#include "test.h"
__FBSDID("$FreeBSD$");





static void
verify(const char *refname)
{
char *p;
size_t s;
struct archive *a;
struct archive_entry *ae;

extract_reference_file(refname);
p = slurpfile(&s, "%s", refname);


assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_zip(a));
assertEqualIntA(a, ARCHIVE_OK, read_open_memory_seek(a, p, s, 1));

assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("file0", archive_entry_pathname(ae));
assertEqualInt(AE_IFREG | 0644, archive_entry_mode(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);

assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("build.sh", archive_entry_pathname(ae));
assertEqualInt(AE_IFREG | 0755, archive_entry_mode(ae));
assertEqualInt(23, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);

assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_free(a));

free(p);
}

DEFINE_TEST(test_read_format_zip_comment_stored)
{
verify("test_read_format_zip_comment_stored_1.zip");
verify("test_read_format_zip_comment_stored_2.zip");
}