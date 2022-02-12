

























#include "test.h"
__FBSDID("$FreeBSD$");


DEFINE_TEST(test_read_format_warc)
{
char buff[256U];
const char reffile[] = "test_read_format_warc.warc";
struct archive_entry *ae;
struct archive *a;

extract_reference_file(reffile);
assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, reffile, 10240));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("sometest.txt", archive_entry_pathname(ae));
assertEqualInt(1402399833, archive_entry_mtime(ae));
assertEqualInt(0, archive_entry_uid(ae));
assertEqualInt(0, archive_entry_gid(ae));
assertEqualInt(AE_IFREG, archive_entry_filetype(ae));
assertEqualInt(65, archive_entry_size(ae));
assertEqualInt(65, archive_read_data(a, buff, sizeof(buff)));
assertEqualMem(buff, "This is a sample text file for libarchive's WARC reader/writer.\n\n", 65);
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("moretest.txt", archive_entry_pathname(ae));
assertEqualInt(1402399884, archive_entry_mtime(ae));
assertEqualInt(0, archive_entry_uid(ae));
assertEqualInt(0, archive_entry_gid(ae));
assertEqualInt(AE_IFREG, archive_entry_filetype(ae));
assertEqualInt(76, archive_entry_size(ae));
assertA(76 == archive_read_data(a, buff, sizeof(buff)));
assertEqualMem(buff, "The beauty is that WARC remains ASCII only iff all contents are ASCII only.\n", 76);
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);


assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));
assertEqualInt(2, archive_file_count(a));


assertEqualIntA(a, ARCHIVE_FILTER_NONE, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_WARC, archive_format(a));


assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}
