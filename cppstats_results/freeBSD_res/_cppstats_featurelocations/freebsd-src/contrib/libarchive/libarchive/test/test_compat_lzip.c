
























#include "test.h"
__FBSDID("$FreeBSD$");
































































static void
compat_lzip(const char *name)
{
const char *n[7] = { "f1", "f2", "f3", "d1/f1", "d1/f2", "d1/f3", NULL };
struct archive_entry *ae;
struct archive *a;
int i, r;

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
r = archive_read_support_filter_lzip(a);
if (r == ARCHIVE_WARN) {
skipping("lzip reading not fully supported on this platform");
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
return;
}
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
extract_reference_file(name);
assertEqualIntA(a, ARCHIVE_OK, archive_read_open_filename(a, name, 2));


for (i = 0; i < 6; ++i) {
failure("Could not read file %d (%s) from %s", i, n[i], name);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_next_header(a, &ae));
assertEqualString(n[i], archive_entry_pathname(ae));
}


assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualInt(archive_filter_code(a, 0), ARCHIVE_FILTER_LZIP);
assertEqualString(archive_filter_name(a, 0), "lzip");
assertEqualInt(archive_format(a), ARCHIVE_FORMAT_TAR_USTAR);

assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}


DEFINE_TEST(test_compat_lzip)
{


compat_lzip("test_compat_lzip_1.tlz");


compat_lzip("test_compat_lzip_2.tlz");
}
