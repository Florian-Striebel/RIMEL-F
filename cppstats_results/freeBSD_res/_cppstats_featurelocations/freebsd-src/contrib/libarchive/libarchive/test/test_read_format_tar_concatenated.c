























#include "test.h"
__FBSDID("$FreeBSD");








DEFINE_TEST(test_read_format_tar_concatenated)
{
char name[] = "test_read_format_tar_concatenated.tar";
struct archive_entry *ae;
struct archive *a;





assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
extract_reference_file(name);
assertEqualIntA(a, ARCHIVE_OK, archive_read_open_filename(a, name, 10240));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("file1", archive_entry_pathname(ae));


assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));

assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));






assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_set_options(a, "read_concatenated_archives"));
extract_reference_file(name);
assertEqualIntA(a, ARCHIVE_OK, archive_read_open_filename(a, name, 10240));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("file1", archive_entry_pathname(ae));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("file2", archive_entry_pathname(ae));


assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));

assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}
