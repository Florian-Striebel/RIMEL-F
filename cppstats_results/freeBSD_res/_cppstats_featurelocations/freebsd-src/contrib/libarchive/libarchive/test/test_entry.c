























#include "test.h"
__FBSDID("$FreeBSD$");

#include <locale.h>

#if defined(HAVE_LINUX_FS_H)
#include <linux/fs.h>
#endif

#if !defined(HAVE_WCSCPY)
static wchar_t * wcscpy(wchar_t *s1, const wchar_t *s2)
{
wchar_t *dest = s1;
while ((*s1 = *s2) != L'\0')
++s1, ++s2;
return dest;
}
#endif












DEFINE_TEST(test_entry)
{
char buff[128];
wchar_t wbuff[128];
struct stat st;
struct archive_entry *e, *e2;
const struct stat *pst;
unsigned long set, clear;
int type, permset, tag, qual;
const char *name;
const char *xname;
const void *xval;
size_t xsize;
wchar_t wc;
long l;
int i;

assert((e = archive_entry_new()) != NULL);





#if defined(S_IFREG)
assertEqualInt(S_IFREG, AE_IFREG);
#endif
#if defined(S_IFLNK)
assertEqualInt(S_IFLNK, AE_IFLNK);
#endif
#if defined(S_IFSOCK)
assertEqualInt(S_IFSOCK, AE_IFSOCK);
#endif
#if defined(S_IFCHR)
assertEqualInt(S_IFCHR, AE_IFCHR);
#endif


#if defined(S_IFBLK) && !defined(_WIN32)
assertEqualInt(S_IFBLK, AE_IFBLK);
#endif
#if defined(S_IFDIR)
assertEqualInt(S_IFDIR, AE_IFDIR);
#endif
#if defined(S_IFIFO)
assertEqualInt(S_IFIFO, AE_IFIFO);
#endif















archive_entry_set_atime(e, 13579, 24680);
assertEqualInt(archive_entry_atime(e), 13579);
assertEqualInt(archive_entry_atime_nsec(e), 24680);
archive_entry_set_atime(e, 13580, 1000000001L);
assertEqualInt(archive_entry_atime(e), 13581);
assertEqualInt(archive_entry_atime_nsec(e), 1);
archive_entry_set_atime(e, 13580, -7);
assertEqualInt(archive_entry_atime(e), 13579);
assertEqualInt(archive_entry_atime_nsec(e), 999999993);
archive_entry_unset_atime(e);
assertEqualInt(archive_entry_atime(e), 0);
assertEqualInt(archive_entry_atime_nsec(e), 0);
assert(!archive_entry_atime_is_set(e));


archive_entry_set_birthtime(e, 17579, 24990);
assertEqualInt(archive_entry_birthtime(e), 17579);
assertEqualInt(archive_entry_birthtime_nsec(e), 24990);
archive_entry_set_birthtime(e, 17580, 1234567890L);
assertEqualInt(archive_entry_birthtime(e), 17581);
assertEqualInt(archive_entry_birthtime_nsec(e), 234567890);
archive_entry_set_birthtime(e, 17581, -24990);
assertEqualInt(archive_entry_birthtime(e), 17580);
assertEqualInt(archive_entry_birthtime_nsec(e), 999975010);
archive_entry_unset_birthtime(e);
assertEqualInt(archive_entry_birthtime(e), 0);
assertEqualInt(archive_entry_birthtime_nsec(e), 0);
assert(!archive_entry_birthtime_is_set(e));


archive_entry_set_ctime(e, 13580, 24681);
assertEqualInt(archive_entry_ctime(e), 13580);
assertEqualInt(archive_entry_ctime_nsec(e), 24681);
archive_entry_set_ctime(e, 13581, 2008182348L);
assertEqualInt(archive_entry_ctime(e), 13583);
assertEqualInt(archive_entry_ctime_nsec(e), 8182348);
archive_entry_set_ctime(e, 13582, -24681);
assertEqualInt(archive_entry_ctime(e), 13581);
assertEqualInt(archive_entry_ctime_nsec(e), 999975319);
archive_entry_unset_ctime(e);
assertEqualInt(archive_entry_ctime(e), 0);
assertEqualInt(archive_entry_ctime_nsec(e), 0);
assert(!archive_entry_ctime_is_set(e));


assert(!archive_entry_dev_is_set(e));
archive_entry_set_dev(e, 235);
assert(archive_entry_dev_is_set(e));
assertEqualInt(archive_entry_dev(e), 235);



archive_entry_set_filetype(e, AE_IFREG);
assertEqualInt(archive_entry_filetype(e), AE_IFREG);




archive_entry_set_gid(e, 204);
assertEqualInt(archive_entry_gid(e), 204);


archive_entry_set_gname(e, "group");
assertEqualString(archive_entry_gname(e), "group");
assertEqualString(archive_entry_gname_utf8(e), "group");
assertEqualWString(archive_entry_gname_w(e), L"group");
wcscpy(wbuff, L"wgroup");
archive_entry_copy_gname_w(e, wbuff);
assertEqualWString(archive_entry_gname_w(e), L"wgroup");
memset(wbuff, 0, sizeof(wbuff));
assertEqualWString(archive_entry_gname_w(e), L"wgroup");
assertEqualString(archive_entry_gname_utf8(e), "wgroup");
assertEqualString(archive_entry_gname(e), "wgroup");
archive_entry_set_gname_utf8(e, "group");
assertEqualString(archive_entry_gname_utf8(e), "group");
assertEqualWString(archive_entry_gname_w(e), L"group");
assertEqualString(archive_entry_gname(e), "group");
archive_entry_update_gname_utf8(e, "group2");
assertEqualString(archive_entry_gname_utf8(e), "group2");
assertEqualWString(archive_entry_gname_w(e), L"group2");
assertEqualString(archive_entry_gname(e), "group2");


archive_entry_set_hardlink(e, "hardlinkname");
assertEqualString(archive_entry_hardlink(e), "hardlinkname");
assertEqualString(archive_entry_hardlink_utf8(e), "hardlinkname");
assertEqualWString(archive_entry_hardlink_w(e), L"hardlinkname");
strcpy(buff, "hardlinkname2");
archive_entry_copy_hardlink(e, buff);
assertEqualString(archive_entry_hardlink(e), "hardlinkname2");
assertEqualWString(archive_entry_hardlink_w(e), L"hardlinkname2");
assertEqualString(archive_entry_hardlink_utf8(e), "hardlinkname2");
memset(buff, 0, sizeof(buff));
assertEqualString(archive_entry_hardlink(e), "hardlinkname2");
assertEqualString(archive_entry_hardlink_utf8(e), "hardlinkname2");
assertEqualWString(archive_entry_hardlink_w(e), L"hardlinkname2");
archive_entry_copy_hardlink(e, NULL);
assertEqualString(archive_entry_hardlink(e), NULL);
assertEqualWString(archive_entry_hardlink_w(e), NULL);
assertEqualString(archive_entry_hardlink_utf8(e), NULL);
wcscpy(wbuff, L"whardlink");
archive_entry_copy_hardlink_w(e, wbuff);
assertEqualWString(archive_entry_hardlink_w(e), L"whardlink");
assertEqualString(archive_entry_hardlink_utf8(e), "whardlink");
assertEqualString(archive_entry_hardlink(e), "whardlink");
memset(wbuff, 0, sizeof(wbuff));
assertEqualWString(archive_entry_hardlink_w(e), L"whardlink");
archive_entry_copy_hardlink_w(e, NULL);
assertEqualString(archive_entry_hardlink(e), NULL);
assertEqualWString(archive_entry_hardlink_w(e), NULL);
archive_entry_set_hardlink_utf8(e, "hardlinkname");
assertEqualString(archive_entry_hardlink_utf8(e), "hardlinkname");
assertEqualWString(archive_entry_hardlink_w(e), L"hardlinkname");
assertEqualString(archive_entry_hardlink(e), "hardlinkname");
archive_entry_update_hardlink_utf8(e, "hardlinkname2");
assertEqualString(archive_entry_hardlink_utf8(e), "hardlinkname2");
assertEqualWString(archive_entry_hardlink_w(e), L"hardlinkname2");
assertEqualString(archive_entry_hardlink(e), "hardlinkname2");


assert(!archive_entry_ino_is_set(e));
archive_entry_set_ino(e, 8593);
assert(archive_entry_ino_is_set(e));
assertEqualInt(archive_entry_ino(e), 8593);
assertEqualInt(archive_entry_ino64(e), 8593);
archive_entry_set_ino64(e, 8594);
assert(archive_entry_ino_is_set(e));
assertEqualInt(archive_entry_ino(e), 8594);
assertEqualInt(archive_entry_ino64(e), 8594);


archive_entry_set_hardlink(e, "hardlinkname");
archive_entry_set_symlink(e, NULL);
archive_entry_set_link(e, "link");
assertEqualString(archive_entry_hardlink(e), "link");
assertEqualString(archive_entry_symlink(e), NULL);
archive_entry_copy_link(e, "link2");
assertEqualString(archive_entry_hardlink(e), "link2");
assertEqualString(archive_entry_symlink(e), NULL);
archive_entry_copy_link_w(e, L"link3");
assertEqualString(archive_entry_hardlink(e), "link3");
assertEqualString(archive_entry_symlink(e), NULL);
archive_entry_set_hardlink(e, NULL);
archive_entry_set_symlink(e, "symlink");
archive_entry_set_link(e, "link");
assertEqualString(archive_entry_hardlink(e), NULL);
assertEqualString(archive_entry_symlink(e), "link");
archive_entry_copy_link(e, "link2");
assertEqualString(archive_entry_hardlink(e), NULL);
assertEqualString(archive_entry_symlink(e), "link2");
archive_entry_copy_link_w(e, L"link3");
assertEqualString(archive_entry_hardlink(e), NULL);
assertEqualString(archive_entry_symlink(e), "link3");

archive_entry_set_hardlink(e, "hardlink");
archive_entry_set_symlink(e, "symlink");
archive_entry_set_link(e, "link");
assertEqualString(archive_entry_hardlink(e), "hardlink");
assertEqualString(archive_entry_symlink(e), "link");


archive_entry_set_mode(e, 0123456);
assertEqualInt(archive_entry_mode(e), 0123456);


archive_entry_set_mtime(e, 13581, 24682);
assertEqualInt(archive_entry_mtime(e), 13581);
assertEqualInt(archive_entry_mtime_nsec(e), 24682);
archive_entry_set_mtime(e, 13582, 1358297468);
assertEqualInt(archive_entry_mtime(e), 13583);
assertEqualInt(archive_entry_mtime_nsec(e), 358297468);
archive_entry_set_mtime(e, 13583, -24682);
assertEqualInt(archive_entry_mtime(e), 13582);
assertEqualInt(archive_entry_mtime_nsec(e), 999975318);
archive_entry_unset_mtime(e);
assertEqualInt(archive_entry_mtime(e), 0);
assertEqualInt(archive_entry_mtime_nsec(e), 0);
assert(!archive_entry_mtime_is_set(e));


archive_entry_set_nlink(e, 736);
assertEqualInt(archive_entry_nlink(e), 736);


archive_entry_set_pathname(e, "path");
assertEqualString(archive_entry_pathname(e), "path");
assertEqualString(archive_entry_pathname_utf8(e), "path");
assertEqualWString(archive_entry_pathname_w(e), L"path");
archive_entry_set_pathname(e, "path");
assertEqualString(archive_entry_pathname(e), "path");
assertEqualWString(archive_entry_pathname_w(e), L"path");
assertEqualString(archive_entry_pathname_utf8(e), "path");
strcpy(buff, "path2");
archive_entry_copy_pathname(e, buff);
assertEqualString(archive_entry_pathname(e), "path2");
assertEqualWString(archive_entry_pathname_w(e), L"path2");
assertEqualString(archive_entry_pathname_utf8(e), "path2");
memset(buff, 0, sizeof(buff));
assertEqualString(archive_entry_pathname(e), "path2");
assertEqualString(archive_entry_pathname_utf8(e), "path2");
assertEqualWString(archive_entry_pathname_w(e), L"path2");
wcscpy(wbuff, L"wpath");
archive_entry_copy_pathname_w(e, wbuff);
assertEqualWString(archive_entry_pathname_w(e), L"wpath");
assertEqualString(archive_entry_pathname_utf8(e), "wpath");
assertEqualString(archive_entry_pathname(e), "wpath");
memset(wbuff, 0, sizeof(wbuff));
assertEqualWString(archive_entry_pathname_w(e), L"wpath");
assertEqualString(archive_entry_pathname(e), "wpath");
assertEqualString(archive_entry_pathname_utf8(e), "wpath");
archive_entry_set_pathname_utf8(e, "path");
assertEqualWString(archive_entry_pathname_w(e), L"path");
assertEqualString(archive_entry_pathname(e), "path");
assertEqualString(archive_entry_pathname_utf8(e), "path");
archive_entry_update_pathname_utf8(e, "path2");
assertEqualWString(archive_entry_pathname_w(e), L"path2");
assertEqualString(archive_entry_pathname(e), "path2");
assertEqualString(archive_entry_pathname_utf8(e), "path2");


archive_entry_set_rdev(e, 532);
assertEqualInt(archive_entry_rdev(e), 532);



archive_entry_set_size(e, 987654321);
assertEqualInt(archive_entry_size(e), 987654321);
archive_entry_unset_size(e);
assertEqualInt(archive_entry_size(e), 0);
assert(!archive_entry_size_is_set(e));


archive_entry_copy_sourcepath(e, "path1");
assertEqualString(archive_entry_sourcepath(e), "path1");


archive_entry_set_symlink(e, "symlinkname");
assertEqualString(archive_entry_symlink(e), "symlinkname");
assertEqualString(archive_entry_symlink_utf8(e), "symlinkname");
assertEqualWString(archive_entry_symlink_w(e), L"symlinkname");
strcpy(buff, "symlinkname2");
archive_entry_copy_symlink(e, buff);
assertEqualString(archive_entry_symlink(e), "symlinkname2");
assertEqualWString(archive_entry_symlink_w(e), L"symlinkname2");
assertEqualString(archive_entry_symlink_utf8(e), "symlinkname2");
memset(buff, 0, sizeof(buff));
assertEqualString(archive_entry_symlink(e), "symlinkname2");
assertEqualString(archive_entry_symlink_utf8(e), "symlinkname2");
assertEqualWString(archive_entry_symlink_w(e), L"symlinkname2");
archive_entry_copy_symlink_w(e, NULL);
assertEqualWString(archive_entry_symlink_w(e), NULL);
assertEqualString(archive_entry_symlink(e), NULL);
assertEqualString(archive_entry_symlink_utf8(e), NULL);
archive_entry_copy_symlink_w(e, L"wsymlink");
assertEqualWString(archive_entry_symlink_w(e), L"wsymlink");
assertEqualString(archive_entry_symlink_utf8(e), "wsymlink");
assertEqualString(archive_entry_symlink(e), "wsymlink");
archive_entry_copy_symlink(e, NULL);
assertEqualWString(archive_entry_symlink_w(e), NULL);
assertEqualString(archive_entry_symlink(e), NULL);
assertEqualString(archive_entry_symlink_utf8(e), NULL);
archive_entry_set_symlink_utf8(e, "symlinkname");
assertEqualWString(archive_entry_symlink_w(e), L"symlinkname");
assertEqualString(archive_entry_symlink(e), "symlinkname");
assertEqualString(archive_entry_symlink_utf8(e), "symlinkname");
archive_entry_update_symlink_utf8(e, "symlinkname2");
assertEqualWString(archive_entry_symlink_w(e), L"symlinkname2");
assertEqualString(archive_entry_symlink(e), "symlinkname2");
assertEqualString(archive_entry_symlink_utf8(e), "symlinkname2");


archive_entry_set_uid(e, 83);
assertEqualInt(archive_entry_uid(e), 83);


archive_entry_set_uname(e, "user");
assertEqualString(archive_entry_uname(e), "user");
assertEqualString(archive_entry_uname_utf8(e), "user");
assertEqualWString(archive_entry_uname_w(e), L"user");
wcscpy(wbuff, L"wuser");
archive_entry_copy_uname_w(e, wbuff);
assertEqualWString(archive_entry_uname_w(e), L"wuser");
memset(wbuff, 0, sizeof(wbuff));
assertEqualWString(archive_entry_uname_w(e), L"wuser");
assertEqualString(archive_entry_uname_utf8(e), "wuser");
assertEqualString(archive_entry_uname(e), "wuser");
archive_entry_set_uname_utf8(e, "user");
assertEqualString(archive_entry_uname_utf8(e), "user");
assertEqualWString(archive_entry_uname_w(e), L"user");
assertEqualString(archive_entry_uname(e), "user");
archive_entry_set_uname_utf8(e, "user");
assertEqualWString(archive_entry_uname_w(e), L"user");
assertEqualString(archive_entry_uname(e), "user");
assertEqualString(archive_entry_uname_utf8(e), "user");
archive_entry_update_uname_utf8(e, "user2");
assertEqualWString(archive_entry_uname_w(e), L"user2");
assertEqualString(archive_entry_uname(e), "user2");
assertEqualString(archive_entry_uname_utf8(e), "user2");


archive_entry_set_fflags(e, 0x55, 0xAA);
archive_entry_fflags(e, &set, &clear);
failure("Testing set/get of fflags data.");
assertEqualInt(set, 0x55);
failure("Testing set/get of fflags data.");
assertEqualInt(clear, 0xAA);
#if defined(__FreeBSD__)


assertEqualString(archive_entry_fflags_text(e),
"uappnd,nouchg,nodump,noopaque,uunlnk,nosystem");
#endif

#if defined(__FreeBSD__) || defined(__APPLE__)

archive_entry_copy_fflags_text_w(e, L" ,nouappnd, nouchg, dump,hidden");
archive_entry_fflags(e, &set, &clear);
assertEqualInt(UF_HIDDEN, set);
assertEqualInt(UF_NODUMP | UF_IMMUTABLE | UF_APPEND, clear);

archive_entry_copy_fflags_text(e, " ,nouappnd, nouchg, dump,hidden");
archive_entry_fflags(e, &set, &clear);
assertEqualInt(UF_HIDDEN, set);
assertEqualInt(UF_NODUMP | UF_IMMUTABLE | UF_APPEND, clear);
#elif defined(_WIN32) && !defined(CYGWIN)
archive_entry_copy_fflags_text_w(e, L"rdonly,hidden,nosystem");
archive_entry_fflags(e, &set, &clear);
assertEqualInt(FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN, set);
assertEqualInt(FILE_ATTRIBUTE_SYSTEM, clear);
archive_entry_copy_fflags_text(e, "rdonly,hidden,nosystem");
archive_entry_fflags(e, &set, &clear);
assertEqualInt(FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN, set);
assertEqualInt(FILE_ATTRIBUTE_SYSTEM, clear);
#elif defined FS_IOC_GETFLAGS
archive_entry_copy_fflags_text_w(e, L"sappnd,schg,dump,noundel");
archive_entry_fflags(e, &set, &clear);
assertEqualInt(FS_APPEND_FL | FS_IMMUTABLE_FL, set);
assertEqualInt(FS_NODUMP_FL | FS_UNRM_FL, clear);
archive_entry_copy_fflags_text(e, "sappnd,schg,dump,noundel");
archive_entry_fflags(e, &set, &clear);
assertEqualInt(FS_APPEND_FL | FS_IMMUTABLE_FL, set);
assertEqualInt(FS_NODUMP_FL | FS_UNRM_FL, clear);
#endif




archive_entry_xattr_add_entry(e, "xattr1", "xattrvalue1", 12);
assertEqualInt(1, archive_entry_xattr_reset(e));
assertEqualInt(0, archive_entry_xattr_next(e, &xname, &xval, &xsize));
assertEqualString(xname, "xattr1");
assertEqualString(xval, "xattrvalue1");
assertEqualInt((int)xsize, 12);
assertEqualInt(1, archive_entry_xattr_count(e));
assertEqualInt(ARCHIVE_WARN,
archive_entry_xattr_next(e, &xname, &xval, &xsize));
assertEqualString(xname, NULL);
assertEqualString(xval, NULL);
assertEqualInt((int)xsize, 0);
archive_entry_xattr_clear(e);
assertEqualInt(0, archive_entry_xattr_reset(e));
assertEqualInt(ARCHIVE_WARN,
archive_entry_xattr_next(e, &xname, &xval, &xsize));
assertEqualString(xname, NULL);
assertEqualString(xval, NULL);
assertEqualInt((int)xsize, 0);
archive_entry_xattr_add_entry(e, "xattr1", "xattrvalue1", 12);
assertEqualInt(1, archive_entry_xattr_reset(e));
archive_entry_xattr_add_entry(e, "xattr2", "xattrvalue2", 12);
assertEqualInt(2, archive_entry_xattr_reset(e));
assertEqualInt(0, archive_entry_xattr_next(e, &xname, &xval, &xsize));
assertEqualInt(0, archive_entry_xattr_next(e, &xname, &xval, &xsize));
assertEqualInt(ARCHIVE_WARN,
archive_entry_xattr_next(e, &xname, &xval, &xsize));
assertEqualString(xname, NULL);
assertEqualString(xval, NULL);
assertEqualInt((int)xsize, 0);







archive_entry_clear(e);
archive_entry_set_atime(e, 13579, 24680);
archive_entry_set_birthtime(e, 13779, 24990);
archive_entry_set_ctime(e, 13580, 24681);
archive_entry_set_dev(e, 235);
archive_entry_set_fflags(e, 0x55, 0xAA);
archive_entry_set_gid(e, 204);
archive_entry_set_gname(e, "group");
archive_entry_set_hardlink(e, "hardlinkname");
archive_entry_set_ino(e, 8593);
archive_entry_set_mode(e, 0123456);
archive_entry_set_mtime(e, 13581, 24682);
archive_entry_set_nlink(e, 736);
archive_entry_set_pathname(e, "path");
archive_entry_set_rdev(e, 532);
archive_entry_set_size(e, 987654321);
archive_entry_copy_sourcepath(e, "source");
archive_entry_set_symlink(e, "symlinkname");
archive_entry_set_uid(e, 83);
archive_entry_set_uname(e, "user");

archive_entry_acl_add_entry(e, ARCHIVE_ENTRY_ACL_TYPE_ACCESS,
ARCHIVE_ENTRY_ACL_READ, ARCHIVE_ENTRY_ACL_USER, 77, "user77");

archive_entry_xattr_add_entry(e, "xattr1", "xattrvalue", 11);


e2 = archive_entry_clone(e);


assertEqualInt(archive_entry_atime(e2), 13579);
assertEqualInt(archive_entry_atime_nsec(e2), 24680);
assertEqualInt(archive_entry_birthtime(e2), 13779);
assertEqualInt(archive_entry_birthtime_nsec(e2), 24990);
assertEqualInt(archive_entry_ctime(e2), 13580);
assertEqualInt(archive_entry_ctime_nsec(e2), 24681);
assertEqualInt(archive_entry_dev(e2), 235);
archive_entry_fflags(e, &set, &clear);
assertEqualInt(clear, 0xAA);
assertEqualInt(set, 0x55);
assertEqualInt(archive_entry_gid(e2), 204);
assertEqualString(archive_entry_gname(e2), "group");
assertEqualString(archive_entry_hardlink(e2), "hardlinkname");
assertEqualInt(archive_entry_ino(e2), 8593);
assertEqualInt(archive_entry_mode(e2), 0123456);
assertEqualInt(archive_entry_mtime(e2), 13581);
assertEqualInt(archive_entry_mtime_nsec(e2), 24682);
assertEqualInt(archive_entry_nlink(e2), 736);
assertEqualString(archive_entry_pathname(e2), "path");
assertEqualInt(archive_entry_rdev(e2), 532);
assertEqualInt(archive_entry_size(e2), 987654321);
assertEqualString(archive_entry_sourcepath(e2), "source");
assertEqualString(archive_entry_symlink(e2), "symlinkname");
assertEqualInt(archive_entry_uid(e2), 83);
assertEqualString(archive_entry_uname(e2), "user");


assertEqualInt(4, archive_entry_acl_reset(e2,
ARCHIVE_ENTRY_ACL_TYPE_ACCESS));

assertEqualInt(0, archive_entry_acl_next(e2,
ARCHIVE_ENTRY_ACL_TYPE_ACCESS,
&type, &permset, &tag, &qual, &name));
assertEqualInt(type, ARCHIVE_ENTRY_ACL_TYPE_ACCESS);
assertEqualInt(permset, 4);
assertEqualInt(tag, ARCHIVE_ENTRY_ACL_USER_OBJ);
assertEqualInt(qual, -1);
assertEqualString(name, NULL);
assertEqualInt(0, archive_entry_acl_next(e2,
ARCHIVE_ENTRY_ACL_TYPE_ACCESS,
&type, &permset, &tag, &qual, &name));
assertEqualInt(type, ARCHIVE_ENTRY_ACL_TYPE_ACCESS);
assertEqualInt(permset, 5);
assertEqualInt(tag, ARCHIVE_ENTRY_ACL_GROUP_OBJ);
assertEqualInt(qual, -1);
assertEqualString(name, NULL);
assertEqualInt(0, archive_entry_acl_next(e2,
ARCHIVE_ENTRY_ACL_TYPE_ACCESS,
&type, &permset, &tag, &qual, &name));
assertEqualInt(type, ARCHIVE_ENTRY_ACL_TYPE_ACCESS);
assertEqualInt(permset, 6);
assertEqualInt(tag, ARCHIVE_ENTRY_ACL_OTHER);
assertEqualInt(qual, -1);
assertEqualString(name, NULL);

assertEqualInt(0, archive_entry_acl_next(e2,
ARCHIVE_ENTRY_ACL_TYPE_ACCESS,
&type, &permset, &tag, &qual, &name));
assertEqualInt(type, ARCHIVE_ENTRY_ACL_TYPE_ACCESS);
assertEqualInt(permset, ARCHIVE_ENTRY_ACL_READ);
assertEqualInt(tag, ARCHIVE_ENTRY_ACL_USER);
assertEqualInt(qual, 77);
assertEqualString(name, "user77");


assertEqualInt(1, archive_entry_xattr_reset(e2));
assertEqualInt(0, archive_entry_xattr_next(e2, &xname, &xval, &xsize));
assertEqualString(xname, "xattr1");
assertEqualString(xval, "xattrvalue");
assertEqualInt((int)xsize, 11);
assertEqualInt(ARCHIVE_WARN,
archive_entry_xattr_next(e2, &xname, &xval, &xsize));
assertEqualString(xname, NULL);
assertEqualString(xval, NULL);
assertEqualInt((int)xsize, 0);


archive_entry_set_atime(e, 13580, 24690);
archive_entry_set_birthtime(e, 13980, 24999);
archive_entry_set_ctime(e, 13590, 24691);
archive_entry_set_dev(e, 245);
archive_entry_set_fflags(e, 0x85, 0xDA);
archive_entry_set_filetype(e, AE_IFLNK);
archive_entry_set_gid(e, 214);
archive_entry_set_gname(e, "grouper");
archive_entry_set_hardlink(e, "hardlinkpath");
archive_entry_set_ino(e, 8763);
archive_entry_set_mode(e, 0123654);
archive_entry_set_mtime(e, 18351, 28642);
archive_entry_set_nlink(e, 73);
archive_entry_set_pathname(e, "pathest");
archive_entry_set_rdev(e, 132);
archive_entry_set_size(e, 987456321);
archive_entry_copy_sourcepath(e, "source2");
archive_entry_set_symlink(e, "symlinkpath");
archive_entry_set_uid(e, 93);
archive_entry_set_uname(e, "username");
archive_entry_acl_clear(e);
archive_entry_xattr_clear(e);


assertEqualInt(archive_entry_atime(e2), 13579);
assertEqualInt(archive_entry_atime_nsec(e2), 24680);
assertEqualInt(archive_entry_birthtime(e2), 13779);
assertEqualInt(archive_entry_birthtime_nsec(e2), 24990);
assertEqualInt(archive_entry_ctime(e2), 13580);
assertEqualInt(archive_entry_ctime_nsec(e2), 24681);
assertEqualInt(archive_entry_dev(e2), 235);
archive_entry_fflags(e2, &set, &clear);
assertEqualInt(clear, 0xAA);
assertEqualInt(set, 0x55);
assertEqualInt(archive_entry_gid(e2), 204);
assertEqualString(archive_entry_gname(e2), "group");
assertEqualString(archive_entry_hardlink(e2), "hardlinkname");
assertEqualInt(archive_entry_ino(e2), 8593);
assertEqualInt(archive_entry_mode(e2), 0123456);
assertEqualInt(archive_entry_mtime(e2), 13581);
assertEqualInt(archive_entry_mtime_nsec(e2), 24682);
assertEqualInt(archive_entry_nlink(e2), 736);
assertEqualString(archive_entry_pathname(e2), "path");
assertEqualInt(archive_entry_rdev(e2), 532);
assertEqualInt(archive_entry_size(e2), 987654321);
assertEqualString(archive_entry_sourcepath(e2), "source");
assertEqualString(archive_entry_symlink(e2), "symlinkname");
assertEqualInt(archive_entry_uid(e2), 83);
assertEqualString(archive_entry_uname(e2), "user");


assertEqualInt(4, archive_entry_acl_reset(e2,
ARCHIVE_ENTRY_ACL_TYPE_ACCESS));

assertEqualInt(0, archive_entry_acl_next(e2,
ARCHIVE_ENTRY_ACL_TYPE_ACCESS,
&type, &permset, &tag, &qual, &name));
assertEqualInt(type, ARCHIVE_ENTRY_ACL_TYPE_ACCESS);
assertEqualInt(permset, 4);
assertEqualInt(tag, ARCHIVE_ENTRY_ACL_USER_OBJ);
assertEqualInt(qual, -1);
assertEqualString(name, NULL);
assertEqualInt(0, archive_entry_acl_next(e2,
ARCHIVE_ENTRY_ACL_TYPE_ACCESS,
&type, &permset, &tag, &qual, &name));
assertEqualInt(type, ARCHIVE_ENTRY_ACL_TYPE_ACCESS);
assertEqualInt(permset, 5);
assertEqualInt(tag, ARCHIVE_ENTRY_ACL_GROUP_OBJ);
assertEqualInt(qual, -1);
assertEqualString(name, NULL);
assertEqualInt(0, archive_entry_acl_next(e2,
ARCHIVE_ENTRY_ACL_TYPE_ACCESS,
&type, &permset, &tag, &qual, &name));
assertEqualInt(type, ARCHIVE_ENTRY_ACL_TYPE_ACCESS);
assertEqualInt(permset, 6);
assertEqualInt(tag, ARCHIVE_ENTRY_ACL_OTHER);
assertEqualInt(qual, -1);
assertEqualString(name, NULL);

assertEqualInt(0, archive_entry_acl_next(e2,
ARCHIVE_ENTRY_ACL_TYPE_ACCESS,
&type, &permset, &tag, &qual, &name));
assertEqualInt(type, ARCHIVE_ENTRY_ACL_TYPE_ACCESS);
assertEqualInt(permset, ARCHIVE_ENTRY_ACL_READ);
assertEqualInt(tag, ARCHIVE_ENTRY_ACL_USER);
assertEqualInt(qual, 77);
assertEqualString(name, "user77");
assertEqualInt(1, archive_entry_acl_next(e2,
ARCHIVE_ENTRY_ACL_TYPE_ACCESS,
&type, &permset, &tag, &qual, &name));
assertEqualInt(type, 0);
assertEqualInt(permset, 0);
assertEqualInt(tag, 0);
assertEqualInt(qual, -1);
assertEqualString(name, NULL);


assertEqualInt(1, archive_entry_xattr_reset(e2));


archive_entry_free(e2);




archive_entry_clear(e);
assertEqualInt(archive_entry_atime(e), 0);
assertEqualInt(archive_entry_atime_nsec(e), 0);
assertEqualInt(archive_entry_birthtime(e), 0);
assertEqualInt(archive_entry_birthtime_nsec(e), 0);
assertEqualInt(archive_entry_ctime(e), 0);
assertEqualInt(archive_entry_ctime_nsec(e), 0);
assertEqualInt(archive_entry_dev(e), 0);
archive_entry_fflags(e, &set, &clear);
assertEqualInt(clear, 0);
assertEqualInt(set, 0);
assertEqualInt(archive_entry_filetype(e), 0);
assertEqualInt(archive_entry_gid(e), 0);
assertEqualString(archive_entry_gname(e), NULL);
assertEqualString(archive_entry_hardlink(e), NULL);
assertEqualInt(archive_entry_ino(e), 0);
assertEqualInt(archive_entry_mode(e), 0);
assertEqualInt(archive_entry_mtime(e), 0);
assertEqualInt(archive_entry_mtime_nsec(e), 0);
assertEqualInt(archive_entry_nlink(e), 0);
assertEqualString(archive_entry_pathname(e), NULL);
assertEqualInt(archive_entry_rdev(e), 0);
assertEqualInt(archive_entry_size(e), 0);
assertEqualString(archive_entry_symlink(e), NULL);
assertEqualInt(archive_entry_uid(e), 0);
assertEqualString(archive_entry_uname(e), NULL);

assertEqualInt(archive_entry_acl_count(e, ARCHIVE_ENTRY_ACL_TYPE_ACCESS), 0);
assertEqualInt(archive_entry_acl_count(e, ARCHIVE_ENTRY_ACL_TYPE_DEFAULT), 0);

assertEqualInt(archive_entry_xattr_count(e), 0);




memset(&st, 0, sizeof(st));

st.st_atime = 456789;
st.st_ctime = 345678;
st.st_dev = 123;
st.st_gid = 34;
st.st_ino = 234;
st.st_mode = 077777;
st.st_mtime = 234567;
st.st_nlink = 345;
st.st_size = 123456789;
st.st_uid = 23;
#if defined(__FreeBSD__)

st.st_atimespec.tv_nsec = 6543210;
st.st_ctimespec.tv_nsec = 5432109;
st.st_mtimespec.tv_nsec = 3210987;
st.st_birthtimespec.tv_nsec = 7459386;
#endif

archive_entry_copy_stat(e, &st);

assertEqualInt(archive_entry_atime(e), 456789);
assertEqualInt(archive_entry_ctime(e), 345678);
assertEqualInt(archive_entry_dev(e), 123);
assertEqualInt(archive_entry_gid(e), 34);
assertEqualInt(archive_entry_ino(e), 234);
assertEqualInt(archive_entry_mode(e), 077777);
assertEqualInt(archive_entry_mtime(e), 234567);
assertEqualInt(archive_entry_nlink(e), 345);
assertEqualInt(archive_entry_size(e), 123456789);
assertEqualInt(archive_entry_uid(e), 23);
#if __FreeBSD__

assertEqualInt(archive_entry_atime_nsec(e), 6543210);
assertEqualInt(archive_entry_ctime_nsec(e), 5432109);
assertEqualInt(archive_entry_mtime_nsec(e), 3210987);
assertEqualInt(archive_entry_birthtime_nsec(e), 7459386);
#endif





memset(&st, 0, sizeof(st));
archive_entry_copy_stat(e, &st);

archive_entry_set_atime(e, 456789, 321);
archive_entry_set_ctime(e, 345678, 432);
archive_entry_set_dev(e, 123);
archive_entry_set_gid(e, 34);
archive_entry_set_ino(e, 234);
archive_entry_set_mode(e, 012345);
archive_entry_set_mode(e, 012345);
archive_entry_set_mtime(e, 234567, 543);
archive_entry_set_nlink(e, 345);
archive_entry_set_size(e, 123456789);
archive_entry_set_uid(e, 23);

assert((pst = archive_entry_stat(e)) != NULL);
if (pst == NULL)
return;

assertEqualInt(pst->st_atime, 456789);
assertEqualInt(pst->st_ctime, 345678);
assertEqualInt(pst->st_dev, 123);
assertEqualInt(pst->st_gid, 34);
assertEqualInt(pst->st_ino, 234);
assertEqualInt(pst->st_mode, 012345);
assertEqualInt(pst->st_mtime, 234567);
assertEqualInt(pst->st_nlink, 345);
assertEqualInt(pst->st_size, 123456789);
assertEqualInt(pst->st_uid, 23);
#if defined(__FreeBSD__)

assertEqualInt(pst->st_atimespec.tv_nsec, 321);
assertEqualInt(pst->st_ctimespec.tv_nsec, 432);
assertEqualInt(pst->st_mtimespec.tv_nsec, 543);
#endif


archive_entry_set_atime(e, 456788, 0);
assert((pst = archive_entry_stat(e)) != NULL);
if (pst == NULL)
return;
assertEqualInt(pst->st_atime, 456788);
archive_entry_set_ctime(e, 345677, 431);
assert((pst = archive_entry_stat(e)) != NULL);
if (pst == NULL)
return;
assertEqualInt(pst->st_ctime, 345677);
archive_entry_set_dev(e, 122);
assert((pst = archive_entry_stat(e)) != NULL);
if (pst == NULL)
return;
assertEqualInt(pst->st_dev, 122);
archive_entry_set_gid(e, 33);
assert((pst = archive_entry_stat(e)) != NULL);
if (pst == NULL)
return;
assertEqualInt(pst->st_gid, 33);
archive_entry_set_ino(e, 233);
assert((pst = archive_entry_stat(e)) != NULL);
if (pst == NULL)
return;
assertEqualInt(pst->st_ino, 233);
archive_entry_set_mode(e, 012344);
assert((pst = archive_entry_stat(e)) != NULL);
if (pst == NULL)
return;
assertEqualInt(pst->st_mode, 012344);
archive_entry_set_mtime(e, 234566, 542);
assert((pst = archive_entry_stat(e)) != NULL);
if (pst == NULL)
return;
assertEqualInt(pst->st_mtime, 234566);
archive_entry_set_nlink(e, 344);
assert((pst = archive_entry_stat(e)) != NULL);
if (pst == NULL)
return;
assertEqualInt(pst->st_nlink, 344);
archive_entry_set_size(e, 123456788);
assert((pst = archive_entry_stat(e)) != NULL);
if (pst == NULL)
return;
assertEqualInt(pst->st_size, 123456788);
archive_entry_set_uid(e, 22);
assert((pst = archive_entry_stat(e)) != NULL);
if (pst == NULL)
return;
assertEqualInt(pst->st_uid, 22);
















#if __FreeBSD__
archive_entry_set_dev(e, 0x12345678);
assertEqualInt(archive_entry_devmajor(e), major(0x12345678));
assertEqualInt(archive_entry_devminor(e), minor(0x12345678));
assertEqualInt(archive_entry_dev(e), 0x12345678);
archive_entry_set_devmajor(e, 0xfe);
archive_entry_set_devminor(e, 0xdcba98);
assertEqualInt(archive_entry_devmajor(e), 0xfe);
assertEqualInt(archive_entry_devminor(e), 0xdcba98);
assertEqualInt(archive_entry_dev(e), makedev(0xfe, 0xdcba98));
archive_entry_set_rdev(e, 0x12345678);
assertEqualInt(archive_entry_rdevmajor(e), major(0x12345678));
assertEqualInt(archive_entry_rdevminor(e), minor(0x12345678));
assertEqualInt(archive_entry_rdev(e), 0x12345678);
archive_entry_set_rdevmajor(e, 0xfe);
archive_entry_set_rdevminor(e, 0xdcba98);
assertEqualInt(archive_entry_rdevmajor(e), 0xfe);
assertEqualInt(archive_entry_rdevminor(e), 0xdcba98);
assertEqualInt(archive_entry_rdev(e), makedev(0xfe, 0xdcba98));
#endif




if (NULL == setlocale(LC_ALL, "en_US.UTF-8")) {
skipping("Can't exercise charset-conversion logic without"
" a suitable locale.");
} else {

archive_entry_copy_pathname(e, "abc\314\214mno\374xyz");
failure("Converting invalid chars to Unicode should fail.");
assert(NULL == archive_entry_pathname_w(e));






archive_entry_copy_gname(e, "abc\314\214mno\374xyz");
failure("Converting invalid chars to Unicode should fail.");
assert(NULL == archive_entry_gname_w(e));


archive_entry_copy_uname(e, "abc\314\214mno\374xyz");
failure("Converting invalid chars to Unicode should fail.");
assert(NULL == archive_entry_uname_w(e));


archive_entry_copy_hardlink(e, "abc\314\214mno\374xyz");
failure("Converting invalid chars to Unicode should fail.");
assert(NULL == archive_entry_hardlink_w(e));


archive_entry_copy_symlink(e, "abc\314\214mno\374xyz");
failure("Converting invalid chars to Unicode should fail.");
assert(NULL == archive_entry_symlink_w(e));
}

l = 0x12345678L;
wc = (wchar_t)l;
if (NULL == setlocale(LC_ALL, "C") || (long)wc != l) {
skipping("Testing charset conversion failure requires 32-bit wchar_t and support for \"C\" locale.");
} else {




name = "xxxAyyyBzzz";
for (i = 0; i < (int)strlen(name); ++i)
wbuff[i] = name[i];
wbuff[3] = (wchar_t)0x12345678;
wbuff[7] = (wchar_t)0x5678;

archive_entry_copy_pathname_w(e, wbuff);
failure("Converting wide characters from Unicode should fail.");
assertEqualString(NULL, archive_entry_pathname(e));
}


archive_entry_free(e);
}
