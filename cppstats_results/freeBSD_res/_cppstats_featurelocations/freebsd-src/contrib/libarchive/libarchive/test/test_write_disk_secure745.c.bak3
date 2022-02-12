























#include "test.h"
__FBSDID("$FreeBSD$");

#define UMASK 022







DEFINE_TEST(test_write_disk_secure745)
{
#if defined(_WIN32) && !defined(__CYGWIN__)
skipping("archive_write_disk security checks not supported on Windows");
#else
struct archive *a;
struct archive_entry *ae;


assertUmask(UMASK);


assert((a = archive_write_disk_new()) != NULL);
archive_write_disk_set_options(a, ARCHIVE_EXTRACT_SECURE_SYMLINKS);


assertMakeDir("target", 0700);


assertMakeDir("sandbox", 0700);
assertChdir("sandbox");


assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "sym");
archive_entry_set_mode(ae, AE_IFLNK | 0777);
archive_entry_copy_symlink(ae, "../target");
assert(0 == archive_write_header(a, ae));
archive_entry_free(ae);


assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "sym");
archive_entry_set_mode(ae, S_IFDIR | 0777);
assert(0 == archive_write_header(a, ae));
archive_entry_free(ae);


assertFileMode("../target", 0700);

assert(0 == archive_write_close(a));
archive_write_free(a);
#endif
}
