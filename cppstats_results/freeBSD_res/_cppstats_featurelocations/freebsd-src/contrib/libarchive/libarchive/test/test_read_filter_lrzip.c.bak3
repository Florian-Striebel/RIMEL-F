























#include "test.h"

DEFINE_TEST(test_read_filter_lrzip)
{
const char *name = "test_read_filter_lrzip.tar.lrz";

const char *n[] = {
"d1/", "d1/f1", "d1/f2", "d1/f3", "f1", "f2", "f3", NULL };
struct archive_entry *ae;
struct archive *a;
int i;

if (!canLrzip()) {
skipping("lrzip command-line program not found");
return;
}

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_WARN, archive_read_support_filter_lrzip(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
extract_reference_file(name);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, name, 200));


for (i = 0; i < 7; ++i) {
failure("Could not read file %d (%s) from %s", i, n[i], name);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_next_header(a, &ae));
assertEqualString(n[i], archive_entry_pathname(ae));
}


assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualInt(archive_filter_code(a, 0), ARCHIVE_FILTER_LRZIP);
assertEqualString(archive_filter_name(a, 0), "lrzip");
assertEqualInt(archive_format(a), ARCHIVE_FORMAT_TAR_GNUTAR);

assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}
