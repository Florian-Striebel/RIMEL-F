
























#include "test.h"
__FBSDID("$FreeBSD$");














static void
verify(const char *name, const char *n[])
{
struct archive_entry *ae;
struct archive *a;
int i,r;

assert((a = archive_read_new()) != NULL);
r = archive_read_support_filter_lz4(a);
if (r == ARCHIVE_WARN) {
skipping("lz4 reading not fully supported on this platform");
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
return;
}
assertEqualIntA(a, ARCHIVE_OK, r);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
copy_reference_file(name);
assertEqualIntA(a, ARCHIVE_OK, archive_read_open_filename(a, name, 200));


for (i = 0; n[i] != NULL; ++i) {
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


assertEqualInt(archive_filter_code(a, 0), ARCHIVE_FILTER_LZ4);
assertEqualString(archive_filter_name(a, 0), "lz4");
assertEqualInt(archive_format(a), ARCHIVE_FORMAT_TAR_USTAR);

assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}


DEFINE_TEST(test_compat_lz4)
{
const char *n[7] = { "f1", "f2", "f3", "d1/f1", "d1/f2", "d1/f3", NULL };
const char *n2[7] = { "xfile", "README", "NEWS", NULL };



verify("test_compat_lz4_1.tar.lz4.uu", n);


verify("test_compat_lz4_2.tar.lz4.uu", n);

verify("test_compat_lz4_3.tar.lz4.uu", n);

verify("test_compat_lz4_B4.tar.lz4.uu", n2);

verify("test_compat_lz4_B5.tar.lz4.uu", n2);

verify("test_compat_lz4_B6.tar.lz4.uu", n2);

verify("test_compat_lz4_B7.tar.lz4.uu", n2);

verify("test_compat_lz4_B4BD.tar.lz4.uu", n2);

verify("test_compat_lz4_B5BD.tar.lz4.uu", n2);

verify("test_compat_lz4_B6BD.tar.lz4.uu", n2);

verify("test_compat_lz4_B7BD.tar.lz4.uu", n2);

verify("test_compat_lz4_B4BDBX.tar.lz4.uu", n2);
}


