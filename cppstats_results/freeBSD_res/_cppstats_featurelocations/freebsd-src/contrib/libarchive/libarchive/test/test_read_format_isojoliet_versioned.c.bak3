



























#include "test.h"
__FBSDID("$FreeBSD: head/lib/libarchive/test/test_read_format_isojoliet_bz2.c 201247 2009-12-30 05:59:21Z kientzle $");






DEFINE_TEST(test_read_format_isojoliet_versioned)
{
const char *refname = "test_read_format_iso_joliet_by_nero.iso.Z";
struct archive_entry *ae;
struct archive *a;

extract_reference_file(refname);
assert((a = archive_read_new()) != NULL);
assertEqualInt(0, archive_read_support_filter_all(a));
assertEqualInt(0, archive_read_support_format_all(a));
assertEqualInt(ARCHIVE_OK,
archive_read_set_option(a, "iso9660", "rockridge", NULL));
assertEqualInt(ARCHIVE_OK,
archive_read_open_filename(a, refname, 10240));


assertEqualInt(0, archive_read_next_header(a, &ae));
assertEqualString(".", archive_entry_pathname(ae));
assertEqualInt(AE_IFDIR, archive_entry_filetype(ae));


assertEqualInt(0, archive_read_next_header(a, &ae));
assertEqualString("test", archive_entry_pathname(ae));
assertEqualInt(AE_IFDIR, archive_entry_filetype(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);







assertEqualInt(0, archive_read_next_header(a, &ae));
assertEqualString("test/test.txt",
archive_entry_pathname(ae));
assertEqualInt(AE_IFREG, archive_entry_filetype(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);


assertEqualInt(ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualInt(archive_filter_code(a, 0), ARCHIVE_FILTER_COMPRESS);


assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

