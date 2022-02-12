























#include "test.h"
__FBSDID("$FreeBSD$");













static void
test_compat_gtar_1(void)
{
char name[] = "test_compat_gtar_1.tar";
struct archive_entry *ae;
struct archive *a;
int r;

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
extract_reference_file(name);
assertEqualIntA(a, ARCHIVE_OK, archive_read_open_filename(a, name, 10240));


assertEqualIntA(a, ARCHIVE_OK, r = archive_read_next_header(a, &ae));
if (r != ARCHIVE_OK) {
archive_read_free(a);
return;
}
assertEqualString(
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890",
archive_entry_pathname(ae));
assertEqualInt(1197179003, archive_entry_mtime(ae));
assertEqualInt(1000, archive_entry_uid(ae));
assertEqualString("tim", archive_entry_uname(ae));
assertEqualInt(1000, archive_entry_gid(ae));
assertEqualString("tim", archive_entry_gname(ae));
assertEqualInt(0100644, archive_entry_mode(ae));


assertEqualIntA(a, ARCHIVE_OK, r = archive_read_next_header(a, &ae));
if (r != ARCHIVE_OK) {
archive_read_free(a);
return;
}
assertEqualString(
"abcdefghijabcdefghijabcdefghijabcdefghijabcdefghij"
"abcdefghijabcdefghijabcdefghijabcdefghijabcdefghij"
"abcdefghijabcdefghijabcdefghijabcdefghijabcdefghij"
"abcdefghijabcdefghijabcdefghijabcdefghijabcdefghij",
archive_entry_pathname(ae));
assertEqualString(
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890"
"12345678901234567890123456789012345678901234567890",
archive_entry_symlink(ae));
assertEqualInt(1197179043, archive_entry_mtime(ae));
assertEqualInt(1000, archive_entry_uid(ae));
assertEqualString("tim", archive_entry_uname(ae));
assertEqualInt(1000, archive_entry_gid(ae));
assertEqualString("tim", archive_entry_gname(ae));
assertEqualInt(0120755, archive_entry_mode(ae));


assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualInt(archive_filter_code(a, 0), ARCHIVE_FILTER_NONE);
assertEqualInt(archive_format(a), ARCHIVE_FORMAT_TAR_GNUTAR);

assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}





static void
test_compat_gtar_2(void)
{
char name[] = "test_compat_gtar_2.tar";
struct archive_entry *ae;
struct archive *a;
int r;

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
extract_reference_file(name);
assertEqualIntA(a, ARCHIVE_OK, archive_read_open_filename(a, name, 10240));


assertEqualIntA(a, ARCHIVE_OK, r = archive_read_next_header(a, &ae));
if (r != ARCHIVE_OK) {
archive_read_free(a);
return;
}


assertEqualInt(2097152, archive_entry_uid(ae));
assertEqualInt(2097152, archive_entry_gid(ae));


assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualInt(archive_filter_code(a, 0), ARCHIVE_FILTER_NONE);
assertEqualInt(archive_format(a), ARCHIVE_FORMAT_TAR_GNUTAR);

assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

DEFINE_TEST(test_compat_gtar)
{
test_compat_gtar_1();
test_compat_gtar_2();
}


