























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

static void
verify(const char *refname, int posix)
{
struct archive_entry *ae;
struct archive *a;
char buff[128];
const void *pv;
size_t s;
int64_t o;
int uid, gid;

if (posix)
uid = gid = 1001;
else
uid = gid = 0;

extract_reference_file(refname);
assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, refname, 10240));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
if (posix)
assertEqualInt((AE_IFDIR | 0750), archive_entry_mode(ae));
else
assertEqualInt((AE_IFDIR | 0755), archive_entry_mode(ae));
assertEqualString("dir/", archive_entry_pathname(ae));
assertEqualInt(86401, archive_entry_mtime(ae));
assertEqualInt(uid, archive_entry_uid(ae));
assertEqualInt(gid, archive_entry_gid(ae));
assertEqualInt(0, archive_entry_size(ae));
assertEqualIntA(a, ARCHIVE_EOF,
archive_read_data_block(a, &pv, &s, &o));
assertEqualInt(s, 0);
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt((AE_IFDIR | 0755), archive_entry_mode(ae));
assertEqualString("dir2/", archive_entry_pathname(ae));
assertEqualInt(86401, archive_entry_mtime(ae));
assertEqualInt(uid, archive_entry_uid(ae));
assertEqualInt(gid, archive_entry_gid(ae));
assertEqualInt(0, archive_entry_size(ae));
assertEqualIntA(a, ARCHIVE_EOF,
archive_read_data_block(a, &pv, &s, &o));
assertEqualInt(s, 0);
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);

if (posix) {

assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt((AE_IFLNK | 0755), archive_entry_mode(ae));
assertEqualString("dir2/symlink1", archive_entry_pathname(ae));
assertEqualString("../file1", archive_entry_symlink(ae));
assertEqualInt(172802, archive_entry_mtime(ae));
assertEqualInt(uid, archive_entry_uid(ae));
assertEqualInt(gid, archive_entry_gid(ae));
assertEqualInt(0, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt((AE_IFLNK | 0755), archive_entry_mode(ae));
assertEqualString("dir2/symlink2", archive_entry_pathname(ae));
assertEqualString("../file2", archive_entry_symlink(ae));
assertEqualInt(172802, archive_entry_mtime(ae));
assertEqualInt(uid, archive_entry_uid(ae));
assertEqualInt(gid, archive_entry_gid(ae));
assertEqualInt(0, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);
}


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt((AE_IFREG | 0644), archive_entry_mode(ae));
assertEqualString("file1", archive_entry_pathname(ae));
assertEqualInt(86401, archive_entry_mtime(ae));
assertEqualInt(uid, archive_entry_uid(ae));
assertEqualInt(gid, archive_entry_gid(ae));
assertEqualInt(file1_size, archive_entry_size(ae));
assertEqualInt(file1_size, archive_read_data(a, buff, file1_size));
assertEqualMem(buff, file1, file1_size);
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
if (posix)
assertEqualInt((AE_IFREG | 0666), archive_entry_mode(ae));
else
assertEqualInt((AE_IFREG | 0644), archive_entry_mode(ae));
assertEqualString("file2", archive_entry_pathname(ae));
assertEqualInt(86401, archive_entry_mtime(ae));
assertEqualInt(uid, archive_entry_uid(ae));
assertEqualInt(gid, archive_entry_gid(ae));
assertEqualInt(file2_size, archive_entry_size(ae));
assertEqualInt(file2_size, archive_read_data(a, buff, file2_size));
assertEqualMem(buff, file2, file2_size);
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);


if (posix) {
assertEqualInt(6, archive_file_count(a));
} else {
assertEqualInt(4, archive_file_count(a));
}


assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


if (posix) {
assertEqualInt(6, archive_file_count(a));
} else {
assertEqualInt(4, archive_file_count(a));
}


assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);


assertEqualIntA(a, ARCHIVE_FILTER_NONE, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_LHA, archive_format(a));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

DEFINE_TEST(test_read_format_lha)
{

verify("test_read_format_lha_header0.lzh", 1);

verify("test_read_format_lha_header1.lzh", 1);

verify("test_read_format_lha_header2.lzh", 1);


verify("test_read_format_lha_header3.lzh", 0);

verify("test_read_format_lha_lh6.lzh", 1);

verify("test_read_format_lha_lh7.lzh", 1);

verify("test_read_format_lha_lh0.lzh", 1);

verify("test_read_format_lha_withjunk.lzh", 1);
}

