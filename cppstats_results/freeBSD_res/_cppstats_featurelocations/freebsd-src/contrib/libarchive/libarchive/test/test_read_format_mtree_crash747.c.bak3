























#include "test.h"





DEFINE_TEST(test_read_format_mtree_crash747)
{
const char *reffile = "test_read_format_mtree_crash747.mtree.bz2";
struct archive *a;

if (archive_bzlib_version() == NULL) {
skipping("This test requires bzlib");
return;
}

extract_reference_file(reffile);

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_bzip2(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_mtree(a));
assertEqualIntA(a, ARCHIVE_FATAL, archive_read_open_filename(a, reffile, 10240));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

