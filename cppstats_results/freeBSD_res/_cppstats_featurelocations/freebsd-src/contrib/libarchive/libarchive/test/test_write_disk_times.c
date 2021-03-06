























#include "test.h"
__FBSDID("$FreeBSD$");







DEFINE_TEST(test_write_disk_times)
{
struct archive *a;
struct archive_entry *ae;


assert((a = archive_write_disk_new()) != NULL);
assertEqualInt(ARCHIVE_OK,
archive_write_disk_set_options(a, ARCHIVE_EXTRACT_TIME));




assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "file1");
archive_entry_set_mode(ae, S_IFREG | 0777);
archive_entry_set_atime(ae, 123456, 0);
archive_entry_set_mtime(ae, 234567, 0);
assertEqualInt(ARCHIVE_OK, archive_write_header(a, ae));
assertEqualInt(ARCHIVE_OK, archive_write_finish_entry(a));
archive_entry_free(ae);

assertFileAtime("file1", 123456, 0);
assertFileMtime("file1", 234567, 0);




assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "file2");
archive_entry_set_mode(ae, S_IFREG | 0777);
archive_entry_set_mtime(ae, 234567, 0);
assertEqualInt(ARCHIVE_OK, archive_write_header(a, ae));
assertEqualInt(ARCHIVE_OK, archive_write_finish_entry(a));
archive_entry_free(ae);
assertFileMtime("file2", 234567, 0);
assertFileAtimeRecent("file2");




assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "file3");
archive_entry_set_mode(ae, S_IFREG | 0777);
archive_entry_set_atime(ae, 345678, 0);
assertEqualInt(ARCHIVE_OK, archive_write_header(a, ae));
assertEqualInt(ARCHIVE_OK, archive_write_finish_entry(a));
archive_entry_free(ae);

assertFileAtime("file3", 345678, 0);
assertFileMtimeRecent("file3");




assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "file4");
archive_entry_set_mode(ae, S_IFREG | 0777);
assertEqualInt(ARCHIVE_OK, archive_write_header(a, ae));
assertEqualInt(ARCHIVE_OK, archive_write_finish_entry(a));
archive_entry_free(ae);

assertFileAtimeRecent("file4");
assertFileMtimeRecent("file4");

#if defined(__FreeBSD__)



assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "file10");
archive_entry_set_mode(ae, S_IFREG | 0777);
archive_entry_set_atime(ae, 1234567, 23456);
archive_entry_set_mtime(ae, 2345678, 4567);
assertEqualInt(ARCHIVE_OK, archive_write_header(a, ae));
assertEqualInt(ARCHIVE_OK, archive_write_finish_entry(a));
archive_entry_free(ae);

assertFileMtime("file10", 2345678, 4567);
assertFileAtime("file10", 1234567, 23456);




assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "file11");
archive_entry_set_mode(ae, S_IFREG | 0777);
archive_entry_set_atime(ae, 1234567, 23456);
archive_entry_set_birthtime(ae, 3456789, 12345);

archive_entry_set_mtime(ae, 12345678, 4567);
assertEqualInt(ARCHIVE_OK, archive_write_header(a, ae));
assertEqualInt(ARCHIVE_OK, archive_write_finish_entry(a));
archive_entry_free(ae);

assertFileAtime("file11", 1234567, 23456);
assertFileBirthtime("file11", 3456789, 12345);
assertFileMtime("file11", 12345678, 4567);




assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "file12");
archive_entry_set_mode(ae, S_IFREG | 0777);
archive_entry_set_birthtime(ae, 3456789, 12345);
assertEqualInt(ARCHIVE_OK, archive_write_header(a, ae));
assertEqualInt(ARCHIVE_OK, archive_write_finish_entry(a));
archive_entry_free(ae);

assertFileAtimeRecent("file12");
assertFileBirthtime("file12", 3456789, 12345);
assertFileMtimeRecent("file12");




assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "file13");
archive_entry_set_mode(ae, S_IFREG | 0777);
archive_entry_set_mtime(ae, 4567890, 23456);
assertEqualInt(ARCHIVE_OK, archive_write_header(a, ae));
assertEqualInt(ARCHIVE_OK, archive_write_finish_entry(a));
archive_entry_free(ae);

assertFileAtimeRecent("file13");
assertFileBirthtime("file13", 4567890, 23456);
assertFileMtime("file13", 4567890, 23456);
#else
skipping("Platform-specific time restore tests");
#endif

assertEqualInt(ARCHIVE_OK, archive_write_free(a));
}
