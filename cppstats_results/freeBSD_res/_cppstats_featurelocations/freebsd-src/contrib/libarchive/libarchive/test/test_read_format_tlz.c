
























#include "test.h"
__FBSDID("$FreeBSD$");

static unsigned char archive[] = {
93, 0, 0,128, 0,255,255,255,255,255,255,255,255, 0, 23, 0,
51, 80, 24,164,204,238, 45, 77, 28,191, 13,144, 8, 10, 70, 5,
173,215, 47,132,237,145,162, 96, 6,131,168,152, 8,135,161,189,
73,110,132, 27,195, 52,109,203, 22, 17,168,211, 18,181, 76, 93,
120, 88,154,155,244,141,193,206,170,224, 80,137,134, 67, 1, 9,
123,121,189, 74,137,197, 63,255,214, 55,119, 0
};

DEFINE_TEST(test_read_format_tlz)
{
struct archive_entry *ae;
struct archive *a;
int r;

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
r = archive_read_support_filter_lzma(a);
if (r == ARCHIVE_WARN) {
skipping("lzma reading not fully supported on this platform");
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
return;
}
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_memory(a, archive, sizeof(archive)));
assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(1, archive_file_count(a));
assertEqualInt(archive_filter_code(a, 0), ARCHIVE_FILTER_LZMA);
assertEqualInt(archive_format(a), ARCHIVE_FORMAT_TAR_USTAR);
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}
