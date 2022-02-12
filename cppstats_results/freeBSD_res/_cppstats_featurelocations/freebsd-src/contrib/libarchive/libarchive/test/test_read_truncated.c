























#include "test.h"
__FBSDID("$FreeBSD$");

static char buff[1000000];
static char buff2[100000];

DEFINE_TEST(test_read_truncated)
{
struct archive_entry *ae;
struct archive *a;
unsigned int i;
size_t used;


assert((a = archive_write_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_format_ustar(a));
assertEqualIntA(a, ARCHIVE_OK, archive_write_add_filter_none(a));
assertEqualIntA(a, ARCHIVE_OK, archive_write_open_memory(a, buff, sizeof(buff), &used));




assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "file");
archive_entry_set_mode(ae, S_IFREG | 0755);
for (i = 0; i < sizeof(buff2); i++)
buff2[i] = (unsigned char)rand();
archive_entry_set_size(ae, sizeof(buff2));
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);
assertEqualIntA(a, sizeof(buff2), archive_write_data(a, buff2, sizeof(buff2)));


assertEqualIntA(a, ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));



for (i = 1; i < used + 100; i += 100) {
assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
if (i < 512) {
assertEqualIntA(a, ARCHIVE_FATAL, archive_read_open_memory(a, buff, i));
goto wrap_up;
} else {
assertEqualIntA(a, ARCHIVE_OK, archive_read_open_memory(a, buff, i));
}
assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));

if (i < 512 + sizeof(buff2)) {
assertEqualIntA(a, ARCHIVE_FATAL, archive_read_data(a, buff2, sizeof(buff2)));
goto wrap_up;
} else {
assertEqualIntA(a, sizeof(buff2), archive_read_data(a, buff2, sizeof(buff2)));
}







if (i < 512 + 512*((sizeof(buff2) + 511)/512) + 512) {
assertEqualIntA(a, ARCHIVE_FATAL, archive_read_next_header(a, &ae));
} else {
assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));
}
wrap_up:
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}




for (i = 1; i < used + 100; i += 100) {
assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
if (i < 512) {
assertEqualIntA(a, ARCHIVE_FATAL, archive_read_open_memory(a, buff, i));
goto wrap_up2;
} else {
assertEqualIntA(a, ARCHIVE_OK, archive_read_open_memory(a, buff, i));
}
assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));

if (i < 512 + 512*((sizeof(buff2)+511)/512)) {
assertEqualIntA(a, ARCHIVE_FATAL, archive_read_data_skip(a));
goto wrap_up2;
} else {
assertEqualIntA(a, ARCHIVE_OK, archive_read_data_skip(a));
}







if (i < 512 + 512*((sizeof(buff2) + 511)/512) + 512) {
assertEqualIntA(a, ARCHIVE_FATAL, archive_read_next_header(a, &ae));
} else {
assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));
}
wrap_up2:
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}
}
