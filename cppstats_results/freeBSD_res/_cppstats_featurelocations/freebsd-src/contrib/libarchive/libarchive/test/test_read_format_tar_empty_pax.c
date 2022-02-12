























#include "test.h"
__FBSDID("$FreeBSD$");







DEFINE_TEST(test_read_format_tar_empty_pax)
{





struct archive_entry *ae;
struct archive *a;
const char *refname = "test_read_format_tar_empty_pax.tar.Z";

extract_reference_file(refname);
assert((a = archive_read_new()) != NULL);
assertEqualInt(ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualInt(ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, refname, 10240));
assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);
assertEqualInt(ARCHIVE_FILTER_COMPRESS, archive_filter_code(a, 0));
assertEqualInt(ARCHIVE_FORMAT_TAR_PAX_INTERCHANGE, archive_format(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}
