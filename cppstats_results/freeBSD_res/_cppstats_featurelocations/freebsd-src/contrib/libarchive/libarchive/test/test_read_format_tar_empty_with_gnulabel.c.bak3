























#include "test.h"
__FBSDID("$FreeBSD$");





DEFINE_TEST(test_read_format_tar_empty_with_gnulabel)
{
char name[] = "test_read_format_tar_empty_with_gnulabel.tar";
struct archive_entry *ae;
struct archive *a;

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
extract_reference_file(name);
assertEqualIntA(a, ARCHIVE_OK, archive_read_open_filename(a, name, 10240));


assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualInt(archive_filter_code(a, 0), ARCHIVE_FILTER_NONE);
assertEqualInt(archive_format(a), ARCHIVE_FORMAT_TAR);

assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}
