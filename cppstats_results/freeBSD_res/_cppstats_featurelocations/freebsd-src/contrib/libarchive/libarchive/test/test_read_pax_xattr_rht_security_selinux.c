



























#include "test.h"

DEFINE_TEST(test_read_pax_xattr_rht_security_selinux)
{
struct archive *a;
struct archive_entry *ae;
const char *refname = "test_read_pax_xattr_rht_security_selinux.tar";
const char *xname;
const void *xval;
size_t xsize;
const char *string;

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));

extract_reference_file(refname);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, refname, 10240));

assertEqualInt(ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(1, archive_entry_xattr_count(ae));
assertEqualInt(1, archive_entry_xattr_reset(ae));

assertEqualInt(0, archive_entry_xattr_next(ae, &xname, &xval, &xsize));
assertEqualString(xname, "security.selinux");
string = "system_u:object_r:admin_home_t:s0";
assertEqualMem(xval, string, xsize);
assertEqualInt((int)xsize, strlen(string));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}
