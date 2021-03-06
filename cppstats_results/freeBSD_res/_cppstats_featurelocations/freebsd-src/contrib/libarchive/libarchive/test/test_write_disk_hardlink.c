























#include "test.h"
__FBSDID("$FreeBSD$");

#if defined(_WIN32) && !defined(__CYGWIN__)

#define UMASK 0177
#define E_MASK (~0177)
#else
#define UMASK 022
#define E_MASK (~0)
#endif








DEFINE_TEST(test_write_disk_hardlink)
{
#if defined(__HAIKU__)
skipping("archive_write_disk_hardlink; hardlinks are not supported on bfs");
#else
static const char data[]="abcdefghijklmnopqrstuvwxyz";
struct archive *ad;
struct archive_entry *ae;
#if defined(HAVE_LINKAT)
int can_symlink;
#endif
int r;


assertUmask(UMASK);


assert((ad = archive_write_disk_new()) != NULL);







assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "link1a");
archive_entry_set_mode(ae, S_IFREG | 0755);
archive_entry_set_size(ae, sizeof(data));
assertEqualIntA(ad, 0, archive_write_header(ad, ae));
assertEqualInt(sizeof(data),
archive_write_data(ad, data, sizeof(data)));
assertEqualIntA(ad, 0, archive_write_finish_entry(ad));
archive_entry_free(ae);


assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "link1b");
archive_entry_set_mode(ae, S_IFREG | 0642);
archive_entry_set_size(ae, 0);
archive_entry_copy_hardlink(ae, "link1a");
assertEqualIntA(ad, 0, r = archive_write_header(ad, ae));
if (r >= ARCHIVE_WARN) {
assertEqualInt(ARCHIVE_WARN,
archive_write_data(ad, data, sizeof(data)));
assertEqualIntA(ad, 0, archive_write_finish_entry(ad));
}
archive_entry_free(ae);







assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "link2a");
archive_entry_set_mode(ae, S_IFREG | 0755);
archive_entry_set_size(ae, sizeof(data));
assertEqualIntA(ad, 0, archive_write_header(ad, ae));
assertEqualInt(sizeof(data),
archive_write_data(ad, data, sizeof(data)));
assertEqualIntA(ad, 0, archive_write_finish_entry(ad));
archive_entry_free(ae);


assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "link2b");
archive_entry_set_mode(ae, S_IFREG | 0642);
archive_entry_unset_size(ae);
archive_entry_copy_hardlink(ae, "link2a");
assertEqualIntA(ad, 0, r = archive_write_header(ad, ae));
if (r >= ARCHIVE_WARN) {
assertEqualInt(ARCHIVE_WARN,
archive_write_data(ad, data, sizeof(data)));
assertEqualIntA(ad, 0, archive_write_finish_entry(ad));
}
archive_entry_free(ae);







assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "link3a");
archive_entry_set_mode(ae, S_IFREG | 0600);
archive_entry_set_size(ae, sizeof(data));
assertEqualIntA(ad, 0, archive_write_header(ad, ae));
assertEqualInt(sizeof(data), archive_write_data(ad, data, sizeof(data)));
assertEqualIntA(ad, 0, archive_write_finish_entry(ad));
archive_entry_free(ae);


assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "link3b");
archive_entry_set_mode(ae, S_IFREG | 0755);
archive_entry_set_size(ae, sizeof(data));
archive_entry_copy_hardlink(ae, "link3a");
assertEqualIntA(ad, 0, r = archive_write_header(ad, ae));
if (r > ARCHIVE_WARN) {
assertEqualInt(sizeof(data),
archive_write_data(ad, data, sizeof(data)));
assertEqualIntA(ad, 0, archive_write_finish_entry(ad));
}
archive_entry_free(ae);







assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "link4a");
archive_entry_set_mode(ae, S_IFREG | 0600);
archive_entry_set_size(ae, 0);
assertEqualIntA(ad, 0, archive_write_header(ad, ae));
assertEqualInt(ARCHIVE_WARN, archive_write_data(ad, data, 1));
assertEqualIntA(ad, 0, archive_write_finish_entry(ad));
archive_entry_free(ae);


assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "link4b");
archive_entry_set_mode(ae, S_IFREG | 0755);
archive_entry_set_size(ae, sizeof(data));
archive_entry_copy_hardlink(ae, "link4a");
assertEqualIntA(ad, 0, r = archive_write_header(ad, ae));
if (r > ARCHIVE_FAILED) {
assertEqualInt(sizeof(data),
archive_write_data(ad, data, sizeof(data)));
assertEqualIntA(ad, 0, archive_write_finish_entry(ad));
}
archive_entry_free(ae);

#if defined(HAVE_LINKAT)

can_symlink = canSymlink();
if (can_symlink) {

assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "link5a");
archive_entry_set_mode(ae, AE_IFLNK | 0642);
archive_entry_unset_size(ae);
archive_entry_copy_symlink(ae, "foo");
assertEqualIntA(ad, 0, r = archive_write_header(ad, ae));
if (r >= ARCHIVE_WARN) {
assertEqualInt(ARCHIVE_WARN,
archive_write_data(ad, data, sizeof(data)));
assertEqualIntA(ad, 0, archive_write_finish_entry(ad));
}
archive_entry_free(ae);



assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "link5b");
archive_entry_set_mode(ae, S_IFREG | 0642);
archive_entry_set_size(ae, 0);
archive_entry_copy_hardlink(ae, "link5a");
assertEqualIntA(ad, 0, r = archive_write_header(ad, ae));
if (r >= ARCHIVE_WARN) {
assertEqualInt(ARCHIVE_WARN,
archive_write_data(ad, data, sizeof(data)));
assertEqualIntA(ad, 0, archive_write_finish_entry(ad));
}
archive_entry_free(ae);
}
#endif
assertEqualInt(0, archive_write_free(ad));








assertIsReg("link1a", 0755 & ~UMASK);
assertFileSize("link1a", sizeof(data));
assertFileNLinks("link1a", 2);
assertIsHardlink("link1a", "link1b");








assertIsReg("link2a", 0755 & ~UMASK);
assertFileSize("link2a", sizeof(data));
assertFileNLinks("link2a", 2);
assertIsHardlink("link2a", "link2b");


assertIsReg("link3a", 0755 & ~UMASK);
assertFileSize("link3a", sizeof(data));
assertFileNLinks("link3a", 2);
assertIsHardlink("link3a", "link3b");


assertIsReg("link4a", 0755 & ~UMASK);
assertFileNLinks("link4a", 2);
assertFileSize("link4a", sizeof(data));
assertIsHardlink("link4a", "link4b");

#if defined(HAVE_LINKAT)
if (can_symlink) {

assertIsSymlink("link5a", "foo", 0);
assertFileNLinks("link5a", 2);
assertIsHardlink("link5a", "link5b");
}
#endif
#endif
}
