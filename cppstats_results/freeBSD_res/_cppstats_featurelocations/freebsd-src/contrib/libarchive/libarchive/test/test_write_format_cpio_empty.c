























#include "test.h"
__FBSDID("$FreeBSD$");






static char ref[] =
"070707"
"000000"
"000000"
"000000"
"000000"
"000000"
"000001"
"000000"
"00000000000"
"000013"
"00000000000"
"TRAILER!!!\0";

DEFINE_TEST(test_write_format_cpio_empty)
{
struct archive *a;
char buff[2048];
size_t used;


assert((a = archive_write_new()) != NULL);
assertA(0 == archive_write_set_format_cpio_odc(a));
assertA(0 == archive_write_add_filter_none(a));


assertA(0 == archive_write_set_bytes_per_block(a, 1));
assertA(0 == archive_write_set_bytes_in_last_block(a, 1));
assertA(0 == archive_write_open_memory(a, buff, sizeof(buff), &used));


assertEqualIntA(a, ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));

failure("Empty cpio archive should be exactly 87 bytes, was %zu.", used);
assert(used == 87);
failure("Empty cpio archive is incorrectly formatted.");
assertEqualMem(buff, ref, 87);
}
