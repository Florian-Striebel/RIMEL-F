























#include "test.h"
__FBSDID("$FreeBSD$");














static void
verify(const char *name)
{
const char *n[7] = { "f1", "f2", "f3", "d1/f1", "d1/f2", "d1/f3", NULL };
struct archive_entry *ae;
struct archive *a;
int i,r;

assert((a = archive_read_new()) != NULL);
r = archive_read_support_filter_gzip(a);
if (r == ARCHIVE_WARN) {
skipping("gzip reading not fully supported on this platform");
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
return;
}
assertEqualIntA(a, ARCHIVE_OK, r);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
extract_reference_file(name);
assertEqualIntA(a, ARCHIVE_OK, archive_read_open_filename(a, name, 200));


for (i = 0; i < 6; ++i) {
failure("Could not read file %d (%s) from %s", i, n[i], name);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_next_header(a, &ae));
if (r != ARCHIVE_OK) {
archive_read_free(a);
return;
}
assertEqualString(n[i], archive_entry_pathname(ae));
}


assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualInt(archive_filter_code(a, 0), ARCHIVE_FILTER_GZIP);
assertEqualString(archive_filter_name(a, 0), "gzip");
assertEqualInt(archive_format(a), ARCHIVE_FORMAT_TAR_USTAR);

assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}


DEFINE_TEST(test_compat_gzip)
{



verify("test_compat_gzip_1.tgz");


verify("test_compat_gzip_2.tgz");
}


