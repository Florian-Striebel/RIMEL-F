























#include "test.h"
__FBSDID("$FreeBSD");

DEFINE_TEST(test_read_format_lha_bugfix_0)
{
const char *refname = "test_read_format_lha_bugfix_0.lzh";
struct archive_entry *ae;
struct archive *a;
const void *pv;
size_t s;
int64_t o;

extract_reference_file(refname);
assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, refname, 10240));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("f", archive_entry_pathname(ae));
assertEqualInt(776, archive_entry_size(ae));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_data_block(a, &pv, &s, &o));
assertEqualInt(s, 776);
assertEqualIntA(a, ARCHIVE_EOF,
archive_read_data_block(a, &pv, &s, &o));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a),
ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);


assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualInt(1, archive_file_count(a));


assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a),
ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);


assertEqualIntA(a, ARCHIVE_FILTER_NONE, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_LHA, archive_format(a));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

