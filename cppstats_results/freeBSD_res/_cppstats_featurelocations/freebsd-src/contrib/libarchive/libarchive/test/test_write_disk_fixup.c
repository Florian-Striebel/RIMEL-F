























#include "test.h"




DEFINE_TEST(test_write_disk_fixup)
{
#if defined(_WIN32) && !defined(__CYGWIN__)
skipping("Skipping test on Windows");
#else
struct archive *ad;
struct archive_entry *ae;
int r;

if (!canSymlink()) {
skipping("Symlinks not supported");
return;
}


assert((ad = archive_write_disk_new()) != NULL);




assertMakeFile("file", 0600, "a");




assertMakeDir("dir", 0700);






assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "dir1/");
archive_entry_set_mode(ae, AE_IFDIR | 0555);
assertEqualIntA(ad, 0, archive_write_header(ad, ae));
assertEqualIntA(ad, 0, archive_write_finish_entry(ad));
archive_entry_free(ae);


assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "dir2/");
archive_entry_set_mode(ae, AE_IFDIR | 0555);
assertEqualIntA(ad, 0, archive_write_header(ad, ae));
assertEqualIntA(ad, 0, archive_write_finish_entry(ad));
archive_entry_free(ae);


assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "dir1");
archive_entry_set_mode(ae, AE_IFLNK | 0777);
archive_entry_set_size(ae, 0);
archive_entry_copy_symlink(ae, "dir");
assertEqualIntA(ad, 0, r = archive_write_header(ad, ae));
if (r >= ARCHIVE_WARN)
assertEqualIntA(ad, 0, archive_write_finish_entry(ad));
archive_entry_free(ae);


assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "dir2");
archive_entry_set_mode(ae, AE_IFLNK | 0777);
archive_entry_set_size(ae, 0);
archive_entry_copy_symlink(ae, "file");
assertEqualIntA(ad, 0, r = archive_write_header(ad, ae));
if (r >= ARCHIVE_WARN)
assertEqualIntA(ad, 0, archive_write_finish_entry(ad));
archive_entry_free(ae);

assertEqualInt(ARCHIVE_OK, archive_write_free(ad));


assertIsSymlink("dir1", "dir", 0);
assertIsSymlink("dir2", "file", 0);
assertFileMode("dir", 0700);
assertFileMode("file", 0600);
#endif
}
