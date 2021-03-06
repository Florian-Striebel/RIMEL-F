
























#include "test.h"
__FBSDID("$FreeBSD$");

static unsigned char archive[] = {
76, 90, 73, 80, 1, 12, 0, 99,156, 62,160, 67,124,230, 93,220,
235,118, 29, 75, 27,226,158, 67,149,151, 96, 22, 54,198,209, 63,
104,209,148,249,238, 71,187,201,243,162, 1, 42, 47, 43,178, 35,
90, 6,156,208, 74,107, 91,229,126, 5, 85,255,136,255, 64, 0,
170,199,228,195, 0, 2, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0,
0, 0, 0, 0
};

DEFINE_TEST(test_read_format_cpio_bin_lzip)
{
struct archive_entry *ae;
struct archive *a;
int r;

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
r = archive_read_support_filter_lzip(a);
if (r == ARCHIVE_WARN) {
skipping("lzip reading not fully supported on this platform");
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
return;
}
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_memory(a, archive, sizeof(archive)));
assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);
assertEqualInt(archive_filter_code(a, 0), ARCHIVE_FILTER_LZIP);
assertEqualInt(archive_format(a), ARCHIVE_FORMAT_CPIO_BIN_LE);
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

