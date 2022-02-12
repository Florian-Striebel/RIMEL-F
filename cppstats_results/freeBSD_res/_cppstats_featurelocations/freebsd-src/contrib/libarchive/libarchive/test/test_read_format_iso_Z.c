























#include "test.h"
__FBSDID("$FreeBSD$");

static void
test1(void)
{
struct archive_entry *ae;
struct archive *a;
const char *name = "test_read_format_iso.iso.Z";

extract_reference_file(name);

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, name, 512));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_next_header(a, &ae));
assertEqualInt(1, archive_file_count(a));
assertEqualInt(archive_filter_code(a, 0),
ARCHIVE_FILTER_COMPRESS);
assertEqualInt(archive_format(a), ARCHIVE_FORMAT_ISO9660);
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

static void
test2(void)
{
struct archive_entry *ae;
struct archive *a;
const char *name = "test_read_format_iso_2.iso.Z";

extract_reference_file(name);

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, name, 512));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_next_header(a, &ae));
assertEqualString(".", archive_entry_pathname(ae));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_next_header(a, &ae));
assertEqualString("A", archive_entry_pathname(ae));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_next_header(a, &ae));
assertEqualString("A/B", archive_entry_pathname(ae));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_next_header(a, &ae));
assertEqualString("C", archive_entry_pathname(ae));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_next_header(a, &ae));
assertEqualString("C/D", archive_entry_pathname(ae));
assertEqualIntA(a, ARCHIVE_EOF,
archive_read_next_header(a, &ae));
assertEqualInt(5, archive_file_count(a));
assertEqualInt(archive_filter_code(a, 0),
ARCHIVE_FILTER_COMPRESS);
assertEqualInt(archive_format(a), ARCHIVE_FORMAT_ISO9660);
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

DEFINE_TEST(test_read_format_iso_Z)
{
test1();
test2();
}
