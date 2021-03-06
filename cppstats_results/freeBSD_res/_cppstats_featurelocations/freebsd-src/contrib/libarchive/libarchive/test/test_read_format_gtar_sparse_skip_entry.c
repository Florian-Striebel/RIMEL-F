























#include "test.h"
__FBSDID("$FreeBSD");

#if defined(__BORLANDC__) || (defined(_MSC_VER) && _MSC_VER <= 1300)
#define LITERAL_LL(x) x##i64
#else
#define LITERAL_LL(x) x##ll
#endif



DEFINE_TEST(test_read_format_gtar_sparse_skip_entry)
{
#if !defined(__FreeBSD__)
const char *refname = "test_read_format_gtar_sparse_skip_entry.tar.Z.uu";
#else
const char *refname = "test_read_format_gtar_sparse_skip_entry.tar.Z";
#endif
struct archive *a;
struct archive_entry *ae;
const void *p;
size_t s;
int64_t o;

#if !defined(__FreeBSD__)
copy_reference_file(refname);
#else
extract_reference_file(refname);
#endif
assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, refname, 10240));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("a", archive_entry_pathname(ae));
assertEqualInt(LITERAL_LL(10737418244), archive_entry_size(ae));
#if !defined(__FreeBSD__)
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a),
ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);
#endif


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("b", archive_entry_pathname(ae));
assertEqualInt(4, archive_entry_size(ae));
#if !defined(__FreeBSD__)
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a),
ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);
#endif



assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualIntA(a, ARCHIVE_FILTER_COMPRESS, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_TAR_PAX_INTERCHANGE,
archive_format(a));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));





assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, refname, 10240));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("a", archive_entry_pathname(ae));
assertEqualInt(LITERAL_LL(10737418244), archive_entry_size(ae));
#if !defined(__FreeBSD__)
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a),
ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);
#endif
assertEqualInt(0, archive_read_data_block(a, &p, &s, &o));
assertEqualInt(4096, s);
assertEqualInt(0, o);



assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("b", archive_entry_pathname(ae));
assertEqualInt(4, archive_entry_size(ae));
#if !defined(__FreeBSD__)
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a),
ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);
#endif



assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualIntA(a, ARCHIVE_FILTER_COMPRESS, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_TAR_PAX_INTERCHANGE,
archive_format(a));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

