























#include "test.h"
__FBSDID("$FreeBSD$");





DEFINE_TEST(test_write_format_shar_empty)
{
struct archive *a;
char buff[2048];
size_t used;


assert((a = archive_write_new()) != NULL);
assertA(0 == archive_write_set_format_shar(a));
assertA(0 == archive_write_add_filter_none(a));


assertA(0 == archive_write_set_bytes_per_block(a, 1));
assertA(0 == archive_write_set_bytes_in_last_block(a, 1));
assertA(0 == archive_write_open_memory(a, buff, sizeof(buff), &used));


assertEqualIntA(a, ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));

failure("Empty shar archive should be exactly 0 bytes, was %zu.", used);
assert(used == 0);
}
