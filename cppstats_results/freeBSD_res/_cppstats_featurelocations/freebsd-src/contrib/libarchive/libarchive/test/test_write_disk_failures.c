























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_write_disk_failures)
{
#if defined(_WIN32) && !defined(__CYGWIN__)
skipping("archive_write_disk interface");
#else
struct archive_entry *ae;
struct archive *a;
int fd;


assertUmask(022);


assertMakeDir("dir", 0555);


fd = open("dir/testfile", O_WRONLY | O_CREAT | O_BINARY, 0777);
if (fd >= 0) {

close(fd);
skipping("Can't test writing to non-writable directory");
return;
}


assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "dir/file");
archive_entry_set_mode(ae, S_IFREG | 0755);
archive_entry_set_size(ae, 8);
assert((a = archive_write_disk_new()) != NULL);
archive_write_disk_set_options(a, ARCHIVE_EXTRACT_TIME);
archive_entry_set_mtime(ae, 123456789, 0);
assertEqualIntA(a, ARCHIVE_FAILED, archive_write_header(a, ae));
assertEqualIntA(a, 0, archive_write_finish_entry(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));
archive_entry_free(ae);
#endif
}
