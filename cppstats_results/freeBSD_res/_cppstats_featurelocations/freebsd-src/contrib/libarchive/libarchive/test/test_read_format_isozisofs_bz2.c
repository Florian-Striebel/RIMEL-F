























#include "test.h"
__FBSDID("$FreeBSD$");



























DEFINE_TEST(test_read_format_isozisofs_bz2)
{
const char *refname = "test_read_format_iso_zisofs.iso.Z";
struct archive_entry *ae;
struct archive *a;
const void *p;
size_t size;
int64_t offset;
int i;

extract_reference_file(refname);
assert((a = archive_read_new()) != NULL);
assertEqualInt(0, archive_read_support_filter_all(a));
assertEqualInt(0, archive_read_support_format_all(a));
assertEqualInt(ARCHIVE_OK,
archive_read_open_filename(a, refname, 10240));



for (i = 0; i < 8; ++i) {
assertEqualInt(0, archive_read_next_header(a, &ae));

assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);

if (strcmp(".", archive_entry_pathname(ae)) == 0) {

assertEqualInt(AE_IFDIR, archive_entry_filetype(ae));
assertEqualInt(2048, archive_entry_size(ae));

assertEqualInt(86401, archive_entry_mtime(ae));
assertEqualInt(0, archive_entry_mtime_nsec(ae));

assertEqualInt(3, archive_entry_stat(ae)->st_nlink);
assertEqualInt(1, archive_entry_uid(ae));
assertEqualIntA(a, ARCHIVE_EOF,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 0);
} else if (strcmp("dir", archive_entry_pathname(ae)) == 0) {

assertEqualString("dir", archive_entry_pathname(ae));
assertEqualInt(AE_IFDIR, archive_entry_filetype(ae));
assertEqualInt(2048, archive_entry_size(ae));
assertEqualInt(86401, archive_entry_mtime(ae));
assertEqualInt(86401, archive_entry_atime(ae));
assertEqualInt(2, archive_entry_stat(ae)->st_nlink);
assertEqualInt(1, archive_entry_uid(ae));
assertEqualInt(2, archive_entry_gid(ae));
} else if (strcmp("file", archive_entry_pathname(ae)) == 0) {
int r;

assertEqualString("file", archive_entry_pathname(ae));
assertEqualInt(AE_IFREG, archive_entry_filetype(ae));
assertEqualInt(12345684, archive_entry_size(ae));
r = archive_read_data_block(a, &p, &size, &offset);
if (r == ARCHIVE_FAILED) {
skipping("Can't read body of ZISOFS entry.");
} else {
assertEqualInt(ARCHIVE_OK, r);
assertEqualInt(0, offset);
assertEqualMem(p, "hello\n", 6);
}
assertEqualInt(86401, archive_entry_mtime(ae));
assertEqualInt(86401, archive_entry_atime(ae));
assertEqualInt(2, archive_entry_stat(ae)->st_nlink);
assertEqualInt(1, archive_entry_uid(ae));
assertEqualInt(2, archive_entry_gid(ae));
} else if (strcmp("hardlink", archive_entry_pathname(ae)) == 0) {






assertEqualString("hardlink", archive_entry_pathname(ae));
assertEqualInt(AE_IFREG, archive_entry_filetype(ae));
assertEqualString("file", archive_entry_hardlink(ae));
assertEqualInt(0, archive_entry_size_is_set(ae));
assertEqualInt(0, archive_entry_size(ae));
assertEqualInt(86401, archive_entry_mtime(ae));
assertEqualInt(2, archive_entry_stat(ae)->st_nlink);
assertEqualInt(1, archive_entry_uid(ae));
assertEqualInt(2, archive_entry_gid(ae));
} else if (strcmp("symlink", archive_entry_pathname(ae)) == 0) {

assertEqualInt(AE_IFLNK, archive_entry_filetype(ae));
assertEqualString("file", archive_entry_symlink(ae));
assertEqualInt(0, archive_entry_size(ae));
assertEqualInt(172802, archive_entry_mtime(ae));
assertEqualInt(172802, archive_entry_atime(ae));
assertEqualInt(1, archive_entry_stat(ae)->st_nlink);
assertEqualInt(1, archive_entry_uid(ae));
assertEqualInt(2, archive_entry_gid(ae));
} else if (strcmp("symlink2", archive_entry_pathname(ae)) == 0) {

assertEqualInt(AE_IFLNK, archive_entry_filetype(ae));
assertEqualString("/tmp", archive_entry_symlink(ae));
assertEqualInt(0, archive_entry_size(ae));
assertEqualInt(1, archive_entry_stat(ae)->st_nlink);
assertEqualInt(1, archive_entry_uid(ae));
assertEqualInt(2, archive_entry_gid(ae));
} else if (strcmp("symlink3", archive_entry_pathname(ae)) == 0) {

assertEqualInt(AE_IFLNK, archive_entry_filetype(ae));
assertEqualString("/tmp/..", archive_entry_symlink(ae));
assertEqualInt(0, archive_entry_size(ae));
assertEqualInt(1, archive_entry_stat(ae)->st_nlink);
assertEqualInt(1, archive_entry_uid(ae));
assertEqualInt(2, archive_entry_gid(ae));
} else if (strcmp("symlink4", archive_entry_pathname(ae)) == 0) {

assertEqualInt(AE_IFLNK, archive_entry_filetype(ae));
assertEqualString(".././../tmp",
archive_entry_symlink(ae));
assertEqualInt(0, archive_entry_size(ae));
assertEqualInt(1, archive_entry_stat(ae)->st_nlink);
assertEqualInt(1, archive_entry_uid(ae));
assertEqualInt(2, archive_entry_gid(ae));
} else {
failure("Saw a file that shouldn't have been there");
assertEqualString(archive_entry_pathname(ae), "");
}
}


assertEqualInt(ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualInt(archive_filter_code(a, 0), ARCHIVE_FILTER_COMPRESS);
assertEqualInt(archive_format(a), ARCHIVE_FORMAT_ISO9660_ROCKRIDGE);


assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}


