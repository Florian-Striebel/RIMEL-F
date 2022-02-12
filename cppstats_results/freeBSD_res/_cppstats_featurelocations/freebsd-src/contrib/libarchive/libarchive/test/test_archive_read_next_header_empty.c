
























#include "test.h"
__FBSDID("$FreeBSD$");

static void
test_empty_file1(void)
{
struct archive* a = archive_read_new();


assertEqualInt(ARCHIVE_OK, archive_read_support_format_raw(a));
assertEqualInt(0, archive_errno(a));
assertEqualString(NULL, archive_error_string(a));


assertEqualInt(ARCHIVE_FATAL, archive_read_open_filename(a, "emptyfile", 0));
assert(NULL != archive_error_string(a));

archive_read_free(a);
}

static void
test_empty_file2_check(struct archive* a)
{
struct archive_entry* e;
assertEqualInt(0, archive_errno(a));
assertEqualString(NULL, archive_error_string(a));

assertEqualInt(ARCHIVE_OK, archive_read_open_filename(a, "emptyfile", 0));
assertEqualInt(0, archive_errno(a));
assertEqualString(NULL, archive_error_string(a));

assertEqualInt(ARCHIVE_EOF, archive_read_next_header(a, &e));
assertEqualInt(0, archive_errno(a));
assertEqualString(NULL, archive_error_string(a));

archive_read_free(a);
}

static void
test_empty_file2(void)
{
struct archive* a = archive_read_new();


assertEqualInt(ARCHIVE_OK, archive_read_support_format_raw(a));
assertEqualInt(ARCHIVE_OK, archive_read_support_format_empty(a));
test_empty_file2_check(a);

a = archive_read_new();
assertEqualInt(ARCHIVE_OK, archive_read_support_format_by_code(a, ARCHIVE_FORMAT_EMPTY));
test_empty_file2_check(a);

a = archive_read_new();
assertEqualInt(ARCHIVE_OK, archive_read_set_format(a, ARCHIVE_FORMAT_EMPTY));
test_empty_file2_check(a);
}

static void
test_empty_tarfile(void)
{
struct archive* a = archive_read_new();
struct archive_entry* e;


assertEqualInt(ARCHIVE_OK, archive_read_support_format_tar(a));
assertEqualInt(0, archive_errno(a));
assertEqualString(NULL, archive_error_string(a));

assertEqualInt(ARCHIVE_OK, archive_read_open_filename(a, "empty.tar", 0));
assertEqualInt(0, archive_errno(a));
assertEqualString(NULL, archive_error_string(a));

assertEqualInt(ARCHIVE_EOF, archive_read_next_header(a, &e));
assertEqualInt(0, archive_errno(a));
assertEqualString(NULL, archive_error_string(a));

archive_read_free(a);
}


static char nulls[512];

DEFINE_TEST(test_archive_read_next_header_empty)
{
FILE *f;


f = fopen("emptyfile", "wb");
fclose(f);


f = fopen("empty.tar", "wb");
assertEqualInt(512, fwrite(nulls, 1, 512, f));
fclose(f);

test_empty_file1();
test_empty_file2();
test_empty_tarfile();

}
