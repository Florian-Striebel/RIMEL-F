























#include "test.h"
__FBSDID("$FreeBSD$");

DEFINE_TEST(test_entry_strmode)
{
struct archive_entry *entry;

assert((entry = archive_entry_new()) != NULL);

archive_entry_set_mode(entry, AE_IFREG | 0642);
assertEqualString(archive_entry_strmode(entry), "-rw-r---w- ");


archive_entry_set_mode(entry, AE_IFREG | 0644);
archive_entry_set_hardlink(entry, "link");
assertEqualString(archive_entry_strmode(entry), "-rw-r--r-- ");

archive_entry_set_mode(entry, 0640);
archive_entry_set_hardlink(entry, "link");
assertEqualString(archive_entry_strmode(entry), "hrw-r----- ");
archive_entry_set_hardlink(entry, NULL);

archive_entry_set_mode(entry, AE_IFDIR | 0777);
assertEqualString(archive_entry_strmode(entry), "drwxrwxrwx ");

archive_entry_set_mode(entry, AE_IFBLK | 03642);
assertEqualString(archive_entry_strmode(entry), "brw-r-S-wT ");

archive_entry_set_mode(entry, AE_IFCHR | 05777);
assertEqualString(archive_entry_strmode(entry), "crwsrwxrwt ");

archive_entry_set_mode(entry, AE_IFSOCK | 0222);
assertEqualString(archive_entry_strmode(entry), "s-w--w--w- ");

archive_entry_set_mode(entry, AE_IFIFO | 0444);
assertEqualString(archive_entry_strmode(entry), "pr--r--r-- ");

archive_entry_set_mode(entry, AE_IFLNK | 04000);
assertEqualString(archive_entry_strmode(entry), "l--S------ ");

archive_entry_acl_add_entry(entry, ARCHIVE_ENTRY_ACL_TYPE_ACCESS,
0007, ARCHIVE_ENTRY_ACL_GROUP, 78, "group78");
assertEqualString(archive_entry_strmode(entry), "l--S------+");


archive_entry_free(entry);
}
