























#include "test.h"
__FBSDID("$FreeBSD$");

#if !defined(_WIN32) || defined(__CYGWIN__)

#define UMASK 022

static long _default_gid = -1;
static long _invalid_gid = -1;
static long _alt_gid = -1;












static void
searchgid(void)
{
static int _searched = 0;
uid_t uid = getuid();
gid_t gid = 0;
unsigned int n;
struct stat st;
int fd;


if (_searched)
return;
_searched = 1;


fd = open("test_gid", O_CREAT | O_BINARY, 0664);
failure("Couldn't create a file for gid testing.");
assert(fd > 0);


assert(fstat(fd, &st) == 0);
_default_gid = st.st_gid;


_invalid_gid = -1;

for (gid = 1, n = 1; gid == n && n < 10000 ; n++, gid++) {
if (fchown(fd, uid, gid) != 0) {
_invalid_gid = gid;
break;
}
}





_alt_gid = -1;
for (gid = 0, n = 0; gid == n && n < 10000 ; n++, gid++) {

if (gid == (gid_t)_default_gid)
continue;
if (fchown(fd, uid, gid) == 0) {
_alt_gid = gid;
break;
}
}
close(fd);
}

static int
altgid(void)
{
searchgid();
return (_alt_gid);
}

static int
invalidgid(void)
{
searchgid();
return (_invalid_gid);
}

static int
defaultgid(void)
{
searchgid();
return (_default_gid);
}
#endif







DEFINE_TEST(test_write_disk_perms)
{
#if defined(_WIN32) && !defined(__CYGWIN__)
skipping("archive_write_disk interface");
#else
struct archive *a;
struct archive_entry *ae;
struct stat st;
uid_t original_uid;
uid_t try_to_change_uid;

assertUmask(UMASK);








assertEqualInt(0, chown(".", getuid(), getgid()));


assert((a = archive_write_disk_new()) != NULL);


assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "file_0755");
archive_entry_set_mode(ae, S_IFREG | 0777);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
assertEqualIntA(a, ARCHIVE_OK, archive_write_finish_entry(a));
archive_entry_free(ae);



assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "file_overwrite_0144");
archive_entry_set_mode(ae, S_IFREG | 0777);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);
assertEqualIntA(a, ARCHIVE_OK, archive_write_finish_entry(a));

assertEqualInt(0, stat("file_overwrite_0144", &st));
failure("file_overwrite_0144: st.st_mode=%o", st.st_mode);
assert((st.st_mode & 07777) != 0144);

assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "file_overwrite_0144");
archive_entry_set_mode(ae, S_IFREG | 0144);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);
assertEqualIntA(a, ARCHIVE_OK, archive_write_finish_entry(a));


assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "dir_0514");
archive_entry_set_mode(ae, S_IFDIR | 0514);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);
assertEqualIntA(a, ARCHIVE_OK, archive_write_finish_entry(a));



assertMakeDir("dir_overwrite_0744", 0744);

assertEqualInt(0, stat("dir_overwrite_0744", &st));
failure("dir_overwrite_0744: st.st_mode=%o", st.st_mode);
assertEqualInt(st.st_mode & 0777, 0744);

assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "dir_overwrite_0744");
archive_entry_set_mode(ae, S_IFDIR | 0777);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);
assertEqualIntA(a, ARCHIVE_OK, archive_write_finish_entry(a));

assertEqualInt(0, stat("dir_overwrite_0744", &st));
failure("dir_overwrite_0744: st.st_mode=%o", st.st_mode);
assertEqualInt(st.st_mode & 0777, 0744);


assertMakeDir("dir_owner", 0744);

if (getuid() == 0) {
original_uid = getuid() + 1;
try_to_change_uid = getuid();
assertEqualInt(0, chown("dir_owner", original_uid, getgid()));
} else {
original_uid = getuid();
try_to_change_uid = getuid() + 1;
}


assertEqualInt(0, stat("dir_owner", &st));
failure("dir_owner: st.st_uid=%d", st.st_uid);
assertEqualInt(st.st_uid, original_uid);

assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "dir_owner");
archive_entry_set_mode(ae, S_IFDIR | 0744);
archive_entry_set_uid(ae, try_to_change_uid);
archive_write_disk_set_options(a,
ARCHIVE_EXTRACT_OWNER | ARCHIVE_EXTRACT_NO_OVERWRITE);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);
assertEqualIntA(a, ARCHIVE_OK, archive_write_finish_entry(a));

assertEqualInt(0, stat("dir_owner", &st));
failure("dir_owner: st.st_uid=%d", st.st_uid);
assertEqualInt(st.st_uid, original_uid);


assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "file_no_suid");
archive_entry_set_mode(ae, S_IFREG | S_ISUID | 0777);
archive_write_disk_set_options(a, 0);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
assertEqualIntA(a, ARCHIVE_OK, archive_write_finish_entry(a));


assert(archive_entry_clear(ae) != NULL);
archive_entry_copy_pathname(ae, "file_0777");
archive_entry_set_mode(ae, S_IFREG | 0777);
archive_write_disk_set_options(a, ARCHIVE_EXTRACT_PERM);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
assertEqualIntA(a, ARCHIVE_OK, archive_write_finish_entry(a));


assert(archive_entry_clear(ae) != NULL);
archive_entry_copy_pathname(ae, "file_4742");
archive_entry_set_mode(ae, S_IFREG | S_ISUID | 0742);
archive_entry_set_uid(ae, getuid());
archive_write_disk_set_options(a, ARCHIVE_EXTRACT_PERM);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
assertEqualIntA(a, ARCHIVE_OK, archive_write_finish_entry(a));






assert(archive_entry_clear(ae) != NULL);
archive_entry_copy_pathname(ae, "file_bad_suid");
archive_entry_set_mode(ae, S_IFREG | S_ISUID | 0742);
archive_entry_set_uid(ae, getuid() + 1);
archive_write_disk_set_options(a, ARCHIVE_EXTRACT_PERM);
assertA(0 == archive_write_header(a, ae));






failure("Opportunistic SUID failure shouldn't return error.");
assertEqualInt(0, archive_write_finish_entry(a));

if (getuid() != 0) {
assert(archive_entry_clear(ae) != NULL);
archive_entry_copy_pathname(ae, "file_bad_suid2");
archive_entry_set_mode(ae, S_IFREG | S_ISUID | 0742);
archive_entry_set_uid(ae, getuid() + 1);
archive_write_disk_set_options(a,
ARCHIVE_EXTRACT_PERM | ARCHIVE_EXTRACT_OWNER);
assertA(0 == archive_write_header(a, ae));

failure("Non-opportunistic SUID failure should return error.");
assertEqualInt(ARCHIVE_WARN, archive_write_finish_entry(a));
}


assert(archive_entry_clear(ae) != NULL);
archive_entry_copy_pathname(ae, "file_perm_sgid");
archive_entry_set_mode(ae, S_IFREG | S_ISGID | 0742);
archive_entry_set_gid(ae, defaultgid());
archive_write_disk_set_options(a, ARCHIVE_EXTRACT_PERM);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
failure("Setting SGID bit should succeed here.");
assertEqualIntA(a, 0, archive_write_finish_entry(a));

if (altgid() == -1) {




skipping("Current user can't test gid restore: must belong to more than one group.");
} else {





















assert(archive_entry_clear(ae) != NULL);
archive_entry_copy_pathname(ae, "file_alt_sgid");
archive_entry_set_mode(ae, S_IFREG | S_ISGID | 0742);
archive_entry_set_uid(ae, getuid());
archive_entry_set_gid(ae, altgid());
archive_write_disk_set_options(a, ARCHIVE_EXTRACT_PERM);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
failure("Setting SGID bit should fail because of group mismatch but the failure should be silent because we didn't ask for the group to be set.");
assertEqualIntA(a, 0, archive_write_finish_entry(a));





assert(archive_entry_clear(ae) != NULL);
archive_entry_copy_pathname(ae, "file_alt_sgid_owner");
archive_entry_set_mode(ae, S_IFREG | S_ISGID | 0742);
archive_entry_set_uid(ae, getuid());
archive_entry_set_gid(ae, altgid());
archive_write_disk_set_options(a,
ARCHIVE_EXTRACT_PERM | ARCHIVE_EXTRACT_OWNER);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
failure("Setting SGID bit should succeed here.");
assertEqualIntA(a, ARCHIVE_OK, archive_write_finish_entry(a));
}






if (invalidgid() == -1) {

printf("Running as root: Can't test SGID failures.\n");
} else {
assert(archive_entry_clear(ae) != NULL);
archive_entry_copy_pathname(ae, "file_bad_sgid");
archive_entry_set_mode(ae, S_IFREG | S_ISGID | 0742);
archive_entry_set_gid(ae, invalidgid());
archive_write_disk_set_options(a, ARCHIVE_EXTRACT_PERM);
assertA(0 == archive_write_header(a, ae));
failure("This SGID restore should fail without an error.");
assertEqualIntA(a, 0, archive_write_finish_entry(a));

assert(archive_entry_clear(ae) != NULL);
archive_entry_copy_pathname(ae, "file_bad_sgid2");
archive_entry_set_mode(ae, S_IFREG | S_ISGID | 0742);
archive_entry_set_gid(ae, invalidgid());
archive_write_disk_set_options(a,
ARCHIVE_EXTRACT_PERM | ARCHIVE_EXTRACT_OWNER);
assertA(0 == archive_write_header(a, ae));
failure("This SGID restore should fail with an error.");
assertEqualIntA(a, ARCHIVE_WARN, archive_write_finish_entry(a));
}


if (getuid() == 0) {
printf("Running as root: Can't test setuid failures.\n");
} else {
assert(archive_entry_clear(ae) != NULL);
archive_entry_copy_pathname(ae, "file_bad_owner");
archive_entry_set_mode(ae, S_IFREG | 0744);
archive_entry_set_uid(ae, getuid() + 1);
archive_write_disk_set_options(a, ARCHIVE_EXTRACT_OWNER);
assertA(0 == archive_write_header(a, ae));
assertEqualIntA(a,ARCHIVE_WARN,archive_write_finish_entry(a));
}

assertEqualInt(ARCHIVE_OK, archive_write_free(a));
archive_entry_free(ae);


assertEqualInt(0, stat("file_0755", &st));
failure("file_0755: st.st_mode=%o", st.st_mode);
assertEqualInt(st.st_mode & 07777, 0755);

assertEqualInt(0, stat("file_overwrite_0144", &st));
failure("file_overwrite_0144: st.st_mode=%o", st.st_mode);
assertEqualInt(st.st_mode & 07777, 0144);

assertEqualInt(0, stat("dir_0514", &st));
failure("dir_0514: st.st_mode=%o", st.st_mode);
assertEqualInt(st.st_mode & 07777, 0514);

assertEqualInt(0, stat("dir_overwrite_0744", &st));
failure("dir_overwrite_0744: st.st_mode=%o", st.st_mode);
assertEqualInt(st.st_mode & 0777, 0744);

assertEqualInt(0, stat("file_no_suid", &st));
failure("file_0755: st.st_mode=%o", st.st_mode);
assertEqualInt(st.st_mode & 07777, 0755);

assertEqualInt(0, stat("file_0777", &st));
failure("file_0777: st.st_mode=%o", st.st_mode);
assertEqualInt(st.st_mode & 07777, 0777);


assertEqualInt(0, stat("file_4742", &st));
failure("file_4742: st.st_mode=%o", st.st_mode);
assertEqualInt(st.st_mode & 07777, S_ISUID | 0742);


assertEqualInt(0, stat("file_bad_suid", &st));
failure("file_bad_suid: st.st_mode=%o", st.st_mode);
assertEqualInt(st.st_mode & 07777, 0742);


if (getuid() != 0) {

assertEqualInt(0, stat("file_bad_suid2", &st));
failure("file_bad_suid2: st.st_mode=%o", st.st_mode);
assertEqualInt(st.st_mode & 07777, 0742);
}


assertEqualInt(0, stat("file_perm_sgid", &st));
failure("file_perm_sgid: st.st_mode=%o", st.st_mode);
assertEqualInt(st.st_mode & 07777, S_ISGID | 0742);

if (altgid() != -1) {

assertEqualInt(0, stat("file_alt_sgid", &st));
failure("file_alt_sgid: st.st_mode=%o", st.st_mode);
assertEqualInt(st.st_mode & 07777, 0742);


assertEqualInt(0, stat("file_alt_sgid_owner", &st));
failure("file_alt_sgid: st.st_mode=%o", st.st_mode);
assertEqualInt(st.st_mode & 07777, S_ISGID | 0742);
}

if (invalidgid() != -1) {

assertEqualInt(0, stat("file_bad_sgid", &st));
failure("file_bad_sgid: st.st_mode=%o", st.st_mode);
assertEqualInt(st.st_mode & 07777, 0742);

assertEqualInt(0, stat("file_bad_sgid2", &st));
failure("file_bad_sgid2: st.st_mode=%o", st.st_mode);
assertEqualInt(st.st_mode & 07777, 0742);
}

if (getuid() != 0) {
assertEqualInt(0, stat("file_bad_owner", &st));
failure("file_bad_owner: st.st_mode=%o", st.st_mode);
assertEqualInt(st.st_mode & 07777, 0744);
failure("file_bad_owner: st.st_uid=%d getuid()=%d",
st.st_uid, getuid());


assertEqualInt(st.st_uid, getuid());
}
#endif
}
