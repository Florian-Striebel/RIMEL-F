























#include "test.h"

DEFINE_TEST(test_read_too_many_filters)
{
const char *name = "test_read_too_many_filters.gz";
struct archive *a;
int r;

assert((a = archive_read_new()) != NULL);
r = archive_read_support_filter_gzip(a);
if (r == ARCHIVE_WARN) {
skipping("gzip reading not fully supported on this platform");
}
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
extract_reference_file(name);
assertEqualIntA(a, ARCHIVE_FATAL,
archive_read_open_filename(a, name, 200));





archive_read_close(a);
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}
