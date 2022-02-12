


























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_read_format_raw)
{
char buff[512];
struct archive_entry *ae;
struct archive *a;
const char *reffile1 = "test_read_format_raw.data";
const char *reffile2 = "test_read_format_raw.data.Z";
const char *reffile3 = "test_read_format_raw.bufr";
#if defined(HAVE_ZLIB_H)
const char *reffile4 = "test_read_format_raw.data.gz";
#endif


extract_reference_file(reffile1);
assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_raw(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, reffile1, 512));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("data", archive_entry_pathname(ae));

assert(!archive_entry_size_is_set(ae));
assert(!archive_entry_atime_is_set(ae));
assert(!archive_entry_ctime_is_set(ae));
assert(!archive_entry_mtime_is_set(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);
assertEqualInt(4, archive_read_data(a, buff, 32));
assertEqualMem(buff, "foo\n", 4);


assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));



extract_reference_file(reffile2);
assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_by_code(a, ARCHIVE_FORMAT_RAW));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, reffile2, 1));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("data", archive_entry_pathname(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);

assert(!archive_entry_size_is_set(ae));
assert(!archive_entry_atime_is_set(ae));
assert(!archive_entry_ctime_is_set(ae));
assert(!archive_entry_mtime_is_set(ae));
assertEqualInt(4, archive_read_data(a, buff, 32));
assertEqualMem(buff, "foo\n", 4);


assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));



extract_reference_file(reffile3);
assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_set_format(a, ARCHIVE_FORMAT_RAW));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, reffile3, 1));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("data", archive_entry_pathname(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);

assert(!archive_entry_size_is_set(ae));
assert(!archive_entry_atime_is_set(ae));
assert(!archive_entry_ctime_is_set(ae));
assert(!archive_entry_mtime_is_set(ae));


assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));

#if defined(HAVE_ZLIB_H)

extract_reference_file(reffile4);
assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_raw(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, reffile4, 1));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("test-file-name.data", archive_entry_pathname(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);
assert(archive_entry_mtime_is_set(ae));
assertEqualIntA(a, archive_entry_mtime(ae), 0x5cbafd25);

assert(!archive_entry_size_is_set(ae));
assert(!archive_entry_atime_is_set(ae));
assert(!archive_entry_ctime_is_set(ae));


assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
#endif
}
