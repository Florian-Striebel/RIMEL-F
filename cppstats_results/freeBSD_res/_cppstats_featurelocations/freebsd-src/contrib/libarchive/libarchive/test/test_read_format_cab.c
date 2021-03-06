























#include "test.h"
__FBSDID("$FreeBSD");































































































































static const char file1[] = {
" file 1 contents\n"
"hello\n"
"hello\n"
"hello\n"
};
#define file1_size (sizeof(file1)-1)
static const char file2[] = {
" file 2 contents\n"
"hello\n"
"hello\n"
"hello\n"
"hello\n"
"hello\n"
"hello\n"
};
#define file2_size (sizeof(file2)-1)

enum comp_type {
STORE = 0,
MSZIP,
LZX
};
static void
verify(const char *refname, enum comp_type comp)
{
struct archive_entry *ae;
struct archive *a;
char buff[128];
char zero[128];
size_t s;

memset(zero, 0, sizeof(zero));
extract_reference_file(refname);
assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, refname, 10240));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt((AE_IFREG | 0666), archive_entry_mode(ae));
assertEqualString("empty", archive_entry_pathname(ae));
assertEqualInt(0, archive_entry_uid(ae));
assertEqualInt(0, archive_entry_gid(ae));
assertEqualInt(0, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);

if (comp != STORE) {




assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt((AE_IFREG | 0666), archive_entry_mode(ae));
assertEqualString("zero", archive_entry_pathname(ae));
assertEqualInt(0, archive_entry_uid(ae));
assertEqualInt(0, archive_entry_gid(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);
assertEqualInt(33000, archive_entry_size(ae));
for (s = 0; s + sizeof(buff) < 33000; s+= sizeof(buff)) {
ssize_t rsize = archive_read_data(a, buff, sizeof(buff));
if (comp == MSZIP && rsize == ARCHIVE_FATAL && archive_zlib_version() == NULL) {
skipping("Skipping CAB format(MSZIP) check: %s",
archive_error_string(a));
goto finish;
}
assertEqualInt(sizeof(buff), rsize);
assertEqualMem(buff, zero, sizeof(buff));
}
assertEqualInt(33000 - s, archive_read_data(a, buff, 33000 - s));
assertEqualMem(buff, zero, 33000 - s);
}


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt((AE_IFREG | 0666), archive_entry_mode(ae));
assertEqualString("dir1/file1", archive_entry_pathname(ae));
assertEqualInt(0, archive_entry_uid(ae));
assertEqualInt(0, archive_entry_gid(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);
assertEqualInt(file1_size, archive_entry_size(ae));
assertEqualInt(file1_size, archive_read_data(a, buff, file1_size));
assertEqualMem(buff, file1, file1_size);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt((AE_IFREG | 0666), archive_entry_mode(ae));
assertEqualString("dir2/file2", archive_entry_pathname(ae));
assertEqualInt(0, archive_entry_uid(ae));
assertEqualInt(0, archive_entry_gid(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);
assertEqualInt(file2_size, archive_entry_size(ae));
assertEqualInt(file2_size, archive_read_data(a, buff, file2_size));
assertEqualMem(buff, file2, file2_size);


assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));

if (comp != STORE) {
assertEqualInt(4, archive_file_count(a));
} else {
assertEqualInt(3, archive_file_count(a));
}


assertEqualIntA(a, ARCHIVE_FILTER_NONE, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_CAB, archive_format(a));


finish:
assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}




static void
verify2(const char *refname, enum comp_type comp)
{
struct archive_entry *ae;
struct archive *a;
char buff[128];
char zero[128];

if (comp == MSZIP && archive_zlib_version() == NULL) {
skipping("Skipping CAB format(MSZIP) check for %s",
refname);
return;
}
memset(zero, 0, sizeof(zero));
extract_reference_file(refname);
assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, refname, 10240));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);
if (comp != STORE) {
assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);
}

assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt((AE_IFREG | 0666), archive_entry_mode(ae));
assertEqualString("dir2/file2", archive_entry_pathname(ae));
assertEqualInt(0, archive_entry_uid(ae));
assertEqualInt(0, archive_entry_gid(ae));
assertEqualInt(file2_size, archive_entry_size(ae));
assertEqualInt(file2_size, archive_read_data(a, buff, file2_size));
assertEqualMem(buff, file2, file2_size);


assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));

if (comp != STORE) {
assertEqualInt(4, archive_file_count(a));
} else {
assertEqualInt(3, archive_file_count(a));
}


assertEqualIntA(a, ARCHIVE_FILTER_NONE, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_CAB, archive_format(a));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}




static void
verify3(const char *refname, enum comp_type comp)
{
struct archive_entry *ae;
struct archive *a;
char zero[128];

memset(zero, 0, sizeof(zero));
extract_reference_file(refname);
assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, refname, 10240));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);
if (comp != STORE) {
assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);
}

assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);


assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));

if (comp != STORE) {
assertEqualInt(4, archive_file_count(a));
} else {
assertEqualInt(3, archive_file_count(a));
}


assertEqualIntA(a, ARCHIVE_FILTER_NONE, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_CAB, archive_format(a));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

DEFINE_TEST(test_read_format_cab)
{

verify("test_read_format_cab_1.cab", STORE);
verify2("test_read_format_cab_1.cab", STORE);
verify3("test_read_format_cab_1.cab", STORE);

verify("test_read_format_cab_2.cab", MSZIP);
verify2("test_read_format_cab_2.cab", MSZIP);
verify3("test_read_format_cab_2.cab", MSZIP);

verify("test_read_format_cab_3.cab", LZX);
verify2("test_read_format_cab_3.cab", LZX);
verify3("test_read_format_cab_3.cab", LZX);
}

