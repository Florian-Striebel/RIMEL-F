























#include "test.h"
__FBSDID("$FreeBSD$");





DEFINE_TEST(test_write_format_tar_empty)
{
struct archive *a;
char buff[2048];
size_t used;
unsigned int i;


assert((a = archive_write_new()) != NULL);
assertA(0 == archive_write_set_format_ustar(a));
assertA(0 == archive_write_add_filter_none(a));
assertA(0 == archive_write_set_bytes_per_block(a, 512));
assertA(0 == archive_write_set_bytes_in_last_block(a, 512));
assertA(0 == archive_write_open_memory(a, buff, sizeof(buff), &used));


assertEqualIntA(a, ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));

assert(used == 1024);
for (i = 0; i < used; i++) {
failure("Empty tar archive should be all nulls.");
assert(buff[i] == 0);
}


assert((a = archive_write_new()) != NULL);
assertA(0 == archive_write_set_format_pax(a));
assertA(0 == archive_write_add_filter_none(a));
assertA(0 == archive_write_set_bytes_per_block(a, 512));
assertA(0 == archive_write_set_bytes_in_last_block(a, 512));
assertA(0 == archive_write_open_memory(a, buff, sizeof(buff), &used));


assertEqualIntA(a, ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));

assertEqualInt((int)used, 1024);
for (i = 0; i < used; i++) {
failure("Empty tar archive should be all nulls.");
assert(buff[i] == 0);
}
}
