























#include "test.h"
__FBSDID("$FreeBSD");


DEFINE_TEST(test_read_format_7zip_packinfo_digests)
{
struct archive_entry *ae;
struct archive *a;
char buff[4];
const char *refname = "test_read_format_7zip_packinfo_digests.7z";

extract_reference_file(refname);
assert((a = archive_read_new()) != NULL);
if (ARCHIVE_OK != archive_read_support_filter_xz(a)) {
skipping("7zip:lzma decoding is not supported on this "
"platform");
} else {
assertEqualIntA(a, ARCHIVE_OK,
archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, refname, 10240));


assertEqualIntA(a, ARCHIVE_OK,
archive_read_next_header(a, &ae));
assertEqualInt((AE_IFREG | 0666), archive_entry_mode(ae));
assertEqualString("a.txt", archive_entry_pathname(ae));
assertEqualInt(1576808819, archive_entry_mtime(ae));
assertEqualInt(4, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);
assertEqualInt(4, archive_read_data(a, buff, sizeof(buff)));
assertEqualMem(buff, "aaa\n", 4);


assertEqualIntA(a, ARCHIVE_OK,
archive_read_next_header(a, &ae));
assertEqualInt((AE_IFREG | 0666), archive_entry_mode(ae));
assertEqualString("b.txt", archive_entry_pathname(ae));
assertEqualInt(1576808819, archive_entry_mtime(ae));
assertEqualInt(4, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);
assertEqualInt(4, archive_read_data(a, buff, sizeof(buff)));
assertEqualMem(buff, "bbb\n", 4);

assertEqualInt(2, archive_file_count(a));


assertEqualIntA(a, ARCHIVE_EOF,
archive_read_next_header(a, &ae));


assertEqualIntA(a, ARCHIVE_FILTER_NONE,
archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_7ZIP,
archive_format(a));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
}
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}
