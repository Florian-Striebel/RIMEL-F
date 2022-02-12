























#include "test.h"
__FBSDID("$FreeBSD");

DEFINE_TEST(test_read_format_7zip_encryption_partially)
{



const char *refname = "test_read_format_7zip_encryption_partially.7z";
struct archive_entry *ae;
struct archive *a;
char buff[128];

extract_reference_file(refname);
assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, refname, 10240));

assertEqualIntA(a, ARCHIVE_READ_FORMAT_ENCRYPTION_DONT_KNOW, archive_read_has_encrypted_entries(a));


if (archive_read_next_header(a, &ae) == ARCHIVE_OK) {
assertEqualInt((AE_IFREG | 0664), archive_entry_mode(ae));
assertEqualString("bar_unencrypted.txt", archive_entry_pathname(ae));
assertEqualInt(1379079541, archive_entry_mtime(ae));
assertEqualInt(4, archive_entry_size(ae));
assertEqualInt(0, archive_entry_is_data_encrypted(ae));
assertEqualInt(0, archive_entry_is_metadata_encrypted(ae));
assertEqualIntA(a, 0, archive_read_has_encrypted_entries(a));
assertEqualInt(4, archive_read_data(a, buff, sizeof(buff)));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt((AE_IFREG | 0664), archive_entry_mode(ae));
assertEqualString("bar_encrypted.txt", archive_entry_pathname(ae));
assertEqualInt(1379079565, archive_entry_mtime(ae));
assertEqualInt(4, archive_entry_size(ae));
assertEqualInt(1, archive_entry_is_data_encrypted(ae));
assertEqualInt(0, archive_entry_is_metadata_encrypted(ae));
assertEqualIntA(a, 1, archive_read_has_encrypted_entries(a));
assertEqualInt(ARCHIVE_FATAL, archive_read_data(a, buff, sizeof(buff)));

assertEqualInt(2, archive_file_count(a));


assertEqualIntA(a, ARCHIVE_FATAL, archive_read_next_header(a, &ae));
}


assertEqualIntA(a, ARCHIVE_FILTER_NONE, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_7ZIP, archive_format(a));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}