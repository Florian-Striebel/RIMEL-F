























#include "test.h"
__FBSDID("$FreeBSD$");

#define UMASK 022











DEFINE_TEST(test_write_disk_secure746a)
{
#if defined(_WIN32) && !defined(__CYGWIN__)
skipping("archive_write_disk security checks not supported on Windows");
#else
struct archive *a;
struct archive_entry *ae;


assertUmask(UMASK);


assertMakeDir("target", 0700);
assertMakeFile("target/foo", 0700, "unmodified");


assertMakeDir("sandbox", 0700);
assertChdir("sandbox");


assert((a = archive_write_disk_new()) != NULL);
archive_write_disk_set_options(a, ARCHIVE_EXTRACT_SECURE_NODOTDOT);


assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "bar");
archive_entry_set_mode(ae, AE_IFREG | 0777);
archive_entry_set_size(ae, 8);
archive_entry_copy_hardlink(ae, "../target/foo");
assertEqualInt(ARCHIVE_FAILED, archive_write_header(a, ae));
assertEqualInt(ARCHIVE_FATAL, archive_write_data(a, "modified", 8));
archive_entry_free(ae);


assertTextFileContents("unmodified", "../target/foo");

assertEqualIntA(a, ARCHIVE_FATAL, archive_write_close(a));
archive_write_free(a);
#endif
}





DEFINE_TEST(test_write_disk_secure746b)
{
#if defined(_WIN32) && !defined(__CYGWIN__)
skipping("archive_write_disk security checks not supported on Windows");
#else
struct archive *a;
struct archive_entry *ae;


assertUmask(UMASK);


assertMakeDir("target", 0700);
assertMakeFile("target/foo", 0700, "unmodified");


assertMakeDir("sandbox", 0700);
assertChdir("sandbox");


assert((a = archive_write_disk_new()) != NULL);
archive_write_disk_set_options(a, ARCHIVE_EXTRACT_SECURE_SYMLINKS);


assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "symlink");
archive_entry_set_mode(ae, AE_IFLNK | 0777);
archive_entry_copy_symlink(ae, "../target");
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);


assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "bar");
archive_entry_set_mode(ae, AE_IFREG | 0777);
archive_entry_set_size(ae, 8);
archive_entry_copy_hardlink(ae, "symlink/foo");
assertEqualIntA(a, ARCHIVE_FAILED, archive_write_header(a, ae));
assertEqualIntA(a, ARCHIVE_FATAL, archive_write_data(a, "modified", 8));
archive_entry_free(ae);


assertTextFileContents("unmodified", "../target/foo");

assertEqualIntA(a, ARCHIVE_FATAL, archive_write_close(a));
archive_write_free(a);
#endif
}
