



























#include "test.h"
__FBSDID("$FreeBSD$");
























DEFINE_TEST(test_read_format_isojoliet_bz2)
{
const char *refname = "test_read_format_iso_joliet.iso.Z";
struct archive_entry *ae;
struct archive *a;
const void *p;
size_t size;
int64_t offset;

extract_reference_file(refname);
assert((a = archive_read_new()) != NULL);
assertEqualInt(0, archive_read_support_filter_all(a));
assertEqualInt(0, archive_read_support_format_all(a));
assertEqualInt(ARCHIVE_OK,
archive_read_set_option(a, "iso9660", "rockridge", NULL));
assertEqualInt(ARCHIVE_OK,
archive_read_open_filename(a, refname, 10240));


assertEqualInt(0, archive_read_next_header(a, &ae));
assertEqualString(".", archive_entry_pathname(ae));
assertEqualInt(AE_IFDIR, archive_entry_filetype(ae));
assertEqualInt(2048, archive_entry_size(ae));
assertEqualInt(86401, archive_entry_mtime(ae));
assertEqualInt(0, archive_entry_mtime_nsec(ae));
assertEqualInt(86401, archive_entry_ctime(ae));
assertEqualInt(3, archive_entry_stat(ae)->st_nlink);
assertEqualInt(0, archive_entry_uid(ae));
assertEqualIntA(a, ARCHIVE_EOF,
archive_read_data_block(a, &p, &size, &offset));
assertEqualInt((int)size, 0);
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);


assertEqualInt(0, archive_read_next_header(a, &ae));
assertEqualString("dir", archive_entry_pathname(ae));
assertEqualInt(AE_IFDIR, archive_entry_filetype(ae));
assertEqualInt(2048, archive_entry_size(ae));
assertEqualInt(86401, archive_entry_mtime(ae));
assertEqualInt(86401, archive_entry_atime(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);



assertEqualInt(0, archive_read_next_header(a, &ae));
assertEqualString("hardlink",
archive_entry_pathname(ae));
assertEqualInt(AE_IFREG, archive_entry_filetype(ae));
assert(archive_entry_hardlink(ae) == NULL);
assertEqualInt(6, archive_entry_size(ae));
assertEqualInt(0, archive_read_data_block(a, &p, &size, &offset));
assertEqualInt(6, (int)size);
assertEqualInt(0, offset);
assertEqualMem(p, "hello\n", 6);



assertEqualInt(0, archive_read_next_header(a, &ae));
assertEqualString("long-joliet-file-name.textfile",
archive_entry_pathname(ae));
assertEqualInt(AE_IFREG, archive_entry_filetype(ae));
assertEqualString("hardlink",
archive_entry_hardlink(ae));
assert(!archive_entry_size_is_set(ae));


assertEqualInt(0, archive_read_next_header(a, &ae));
assertEqualString("symlink", archive_entry_pathname(ae));
assertEqualInt(0, archive_entry_size(ae));
assertEqualInt(172802, archive_entry_mtime(ae));
assertEqualInt(172802, archive_entry_atime(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);


assertEqualInt(ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualInt(archive_filter_code(a, 0), ARCHIVE_FILTER_COMPRESS);


assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

