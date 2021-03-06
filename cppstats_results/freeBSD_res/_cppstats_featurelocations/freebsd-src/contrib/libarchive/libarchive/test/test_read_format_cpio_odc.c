























#include "test.h"
__FBSDID("$FreeBSD$");

static unsigned char archive[] = {
'0','7','0','7','0','7','0','0','2','0','2','5','0','7','4','6','6','1','0',
'4','0','7','5','5','0','0','1','7','5','0','0','0','1','7','5','0','0','0',
'0','0','0','2','0','0','0','0','0','0','1','0','3','3','4','0','5','0','0',
'5','3','0','0','0','0','0','2','0','0','0','0','0','0','0','0','0','0','0',
'.',0,'0','7','0','7','0','7','0','0','0','0','0','0','0','0','0','0','0',
'0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0',
'0','0','0','0','0','1','0','0','0','0','0','0','0','0','0','0','0','0','0',
'0','0','0','0','0','0','0','0','1','3','0','0','0','0','0','0','0','0','0',
'0','0','T','R','A','I','L','E','R','!','!','!',0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0};

DEFINE_TEST(test_read_format_cpio_odc)
{
struct archive_entry *ae;
struct archive *a;
assert((a = archive_read_new()) != NULL);
assertA(0 == archive_read_support_filter_all(a));
assertA(0 == archive_read_support_format_all(a));
assertA(0 == archive_read_open_memory(a, archive, sizeof(archive)));
assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertA(archive_filter_code(a, 0) == ARCHIVE_FILTER_NONE);
assertA(archive_format(a) == ARCHIVE_FORMAT_CPIO_POSIX);
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}


