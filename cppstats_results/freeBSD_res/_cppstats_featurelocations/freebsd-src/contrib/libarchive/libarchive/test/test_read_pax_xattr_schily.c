



























#include "test.h"

DEFINE_TEST(test_read_pax_xattr_schily)
{
struct archive *a;
struct archive_entry *ae;
const char *refname = "test_read_pax_xattr_schily.tar";
const char *xname;
const void *xval;
size_t xsize;
const char *string, *array;

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));

extract_reference_file(refname);
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, refname, 10240));

assertEqualInt(ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(2, archive_entry_xattr_count(ae));
assertEqualInt(2, archive_entry_xattr_reset(ae));

assertEqualInt(0, archive_entry_xattr_next(ae, &xname, &xval, &xsize));
assertEqualString(xname, "security.selinux");
string = "system_u:object_r:unlabeled_t:s0";
assertEqualString(xval, string);

assertEqualInt((int)xsize, strlen(string) + 1);

assertEqualInt(0, archive_entry_xattr_next(ae, &xname, &xval, &xsize));
assertEqualString(xname, "security.ima");
assertEqualInt((int)xsize, 265);

array = "\x03\x02\x04\xb0\xe9\xd6\x79\x01\x00\x2b\xad\x1e";
assertEqualMem(xval, array, 12);


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}
