























#include "test.h"

DEFINE_TEST(test_warn_missing_hardlink_target)
{
struct archive *a;
struct archive_entry *ae;

assert(NULL != (a = archive_write_disk_new()));
assert(NULL != (ae = archive_entry_new()));

archive_entry_set_pathname(ae, "hardlink-name");
archive_entry_set_hardlink(ae, "hardlink-target");

assertEqualInt(ARCHIVE_FAILED, archive_write_header(a, ae));
assertEqualInt(ENOENT, archive_errno(a));
assertEqualString("Hard-link target 'hardlink-target' does not exist.",
archive_error_string(a));

archive_entry_free(ae);
archive_free(a);
}
