
























#include "test.h"
__FBSDID("$FreeBSD$");

static void
verify_zip_filesize(uint64_t size, int expected)
{
struct archive *a;
struct archive_entry *ae;
char buff[256];
size_t used;


assert((a = archive_write_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_format_zip(a));

assertEqualIntA(a, ARCHIVE_OK,
archive_write_set_format_option(a, "zip", "zip64", NULL));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_open_memory(a, buff, sizeof(buff), &used));

assert((ae = archive_entry_new()) != NULL);
archive_entry_set_pathname(ae, "test");
archive_entry_set_mode(ae, AE_IFREG | 0644);
archive_entry_set_size(ae, size);
assertEqualInt(expected, archive_write_header(a, ae));

archive_entry_free(ae);


assertEqualIntA(a, ARCHIVE_OK, archive_write_free(a));
}

DEFINE_TEST(test_write_format_zip_zip64_oversize)
{






verify_zip_filesize(0xffffffffLL, ARCHIVE_OK);

verify_zip_filesize(0x100000000LL, ARCHIVE_FAILED);
}
