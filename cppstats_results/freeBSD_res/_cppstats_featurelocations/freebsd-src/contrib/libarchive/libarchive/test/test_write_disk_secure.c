























#include "test.h"
__FBSDID("$FreeBSD$");

#define UMASK 022






DEFINE_TEST(test_write_disk_secure)
{
#if defined(_WIN32) && !defined(__CYGWIN__)
skipping("archive_write_disk security checks not supported on Windows");
#else
struct archive *a;
struct archive_entry *ae;
struct stat st;
#if defined(HAVE_LCHMOD) && defined(HAVE_SYMLINK) && defined(S_IRUSR) && defined(S_IWUSR) && defined(S_IXUSR)

int working_lchmod;
#endif


assertUmask(UMASK);


assert((a = archive_write_disk_new()) != NULL);


assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "dir");
archive_entry_set_mode(ae, S_IFDIR | 0777);
assert(0 == archive_write_header(a, ae));
archive_entry_free(ae);
assert(0 == archive_write_finish_entry(a));


assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "link_to_dir");
archive_entry_set_mode(ae, S_IFLNK | 0777);
archive_entry_set_symlink(ae, "dir");
archive_write_disk_set_options(a, 0);
assert(0 == archive_write_header(a, ae));
assert(0 == archive_write_finish_entry(a));





assert(archive_entry_clear(ae) != NULL);
archive_entry_copy_pathname(ae, "link_to_dir/filea");
archive_entry_set_mode(ae, S_IFREG | 0777);
assert(0 == archive_write_header(a, ae));
assert(0 == archive_write_finish_entry(a));


assert(archive_entry_clear(ae) != NULL);
archive_entry_copy_pathname(ae, "link_to_dir/fileb");
archive_entry_set_mode(ae, S_IFREG | 0777);
archive_write_disk_set_options(a, ARCHIVE_EXTRACT_SECURE_SYMLINKS);
failure("Extracting a file through a symlink should fail here.");
assertEqualInt(ARCHIVE_FAILED, archive_write_header(a, ae));
archive_entry_free(ae);
assert(0 == archive_write_finish_entry(a));


assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "/tmp/libarchive_test-test_write_disk_secure-absolute_symlink");
archive_entry_set_mode(ae, S_IFLNK | 0777);
archive_entry_set_symlink(ae, "/tmp");
archive_write_disk_set_options(a, 0);
assert(0 == archive_write_header(a, ae));
assert(0 == archive_write_finish_entry(a));


assert(archive_entry_clear(ae) != NULL);
archive_entry_copy_pathname(ae, "/tmp/libarchive_test-test_write_disk_secure-absolute_symlink/libarchive_test-test_write_disk_secure-absolute_symlink_path.tmp");
archive_entry_set_mode(ae, S_IFREG | 0777);
archive_write_disk_set_options(a, ARCHIVE_EXTRACT_SECURE_SYMLINKS);
failure("Extracting a file through an absolute symlink should fail here.");
assertEqualInt(ARCHIVE_FAILED, archive_write_header(a, ae));
archive_entry_free(ae);
assertFileNotExists("/tmp/libarchive_test-test_write_disk_secure-absolute_symlink/libarchive_test-test_write_disk_secure-absolute_symlink_path.tmp");
assert(0 == unlink("/tmp/libarchive_test-test_write_disk_secure-absolute_symlink"));
unlink("/tmp/libarchive_test-test_write_disk_secure-absolute_symlink_path.tmp");


assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "link_to_dir2");
archive_entry_set_mode(ae, S_IFLNK | 0777);
archive_entry_set_symlink(ae, "dir");
archive_write_disk_set_options(a, 0);
assert(0 == archive_write_header(a, ae));
assert(0 == archive_write_finish_entry(a));





assert(archive_entry_clear(ae) != NULL);
archive_entry_copy_pathname(ae, "link_to_dir2/filec");
archive_entry_set_mode(ae, S_IFREG | 0777);
archive_write_disk_set_options(a, ARCHIVE_EXTRACT_SECURE_SYMLINKS | ARCHIVE_EXTRACT_UNLINK);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);
assert(0 == archive_write_finish_entry(a));


assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "dir/nested_link_to_dir");
archive_entry_set_mode(ae, S_IFLNK | 0777);
archive_entry_set_symlink(ae, "../dir");
archive_write_disk_set_options(a, 0);
assert(0 == archive_write_header(a, ae));
assert(0 == archive_write_finish_entry(a));


assert(archive_entry_clear(ae) != NULL);
archive_entry_copy_pathname(ae, "dir/nested_link_to_dir/filed");
archive_entry_set_mode(ae, S_IFREG | 0777);
archive_write_disk_set_options(a, ARCHIVE_EXTRACT_SECURE_SYMLINKS);
failure("Extracting a file through a symlink should fail here.");
assertEqualInt(ARCHIVE_FAILED, archive_write_header(a, ae));
archive_entry_free(ae);
assert(0 == archive_write_finish_entry(a));






assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "link_to_dir3");
archive_entry_set_mode(ae, S_IFLNK | 0777);
archive_entry_set_symlink(ae, "dir");
archive_write_disk_set_options(a, 0);
assert(0 == archive_write_header(a, ae));
assert(0 == archive_write_finish_entry(a));

assert(archive_entry_clear(ae) != NULL);
archive_entry_copy_pathname(ae, "link_to_dir3");
archive_entry_set_mode(ae, S_IFDIR | 0777);
assert(0 == archive_write_header(a, ae));
assert(0 == archive_write_finish_entry(a));

assertEqualInt(0, lstat("link_to_dir3", &st));
assert(S_ISLNK(st.st_mode));
archive_entry_free(ae);





assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "link_to_dir4");
archive_entry_set_mode(ae, S_IFLNK | 0777);
archive_entry_set_symlink(ae, "nonexistent_dir");
archive_write_disk_set_options(a, 0);
assert(0 == archive_write_header(a, ae));
assert(0 == archive_write_finish_entry(a));

assert(archive_entry_clear(ae) != NULL);
archive_entry_copy_pathname(ae, "link_to_dir4");
archive_entry_set_mode(ae, S_IFDIR | 0777);
assert(0 == archive_write_header(a, ae));
assert(0 == archive_write_finish_entry(a));

assertEqualInt(0, lstat("link_to_dir4", &st));
assert(S_ISDIR(st.st_mode));
archive_entry_free(ae);





assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "non_dir");
archive_entry_set_mode(ae, S_IFREG | 0777);
archive_write_disk_set_options(a, 0);
assert(0 == archive_write_header(a, ae));
assert(0 == archive_write_finish_entry(a));

archive_entry_copy_pathname(ae, "link_to_dir5");
archive_entry_set_mode(ae, S_IFLNK | 0777);
archive_entry_set_symlink(ae, "non_dir");
archive_write_disk_set_options(a, 0);
assert(0 == archive_write_header(a, ae));
assert(0 == archive_write_finish_entry(a));

assert(archive_entry_clear(ae) != NULL);
archive_entry_copy_pathname(ae, "link_to_dir5");
archive_entry_set_mode(ae, S_IFDIR | 0777);
assert(0 == archive_write_header(a, ae));
assert(0 == archive_write_finish_entry(a));

assertEqualInt(0, lstat("link_to_dir5", &st));
assert(S_ISDIR(st.st_mode));
archive_entry_free(ae);





assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "/tmp/libarchive_test-test_write_disk_secure-absolute_path.tmp");
archive_entry_set_mode(ae, S_IFREG | 0777);
assert(0 == archive_write_header(a, ae));
assert(0 == archive_write_finish_entry(a));
assertFileExists("/tmp/libarchive_test-test_write_disk_secure-absolute_path.tmp");
assert(0 == unlink("/tmp/libarchive_test-test_write_disk_secure-absolute_path.tmp"));


assert(archive_entry_clear(ae) != NULL);
archive_entry_copy_pathname(ae, "/tmp/libarchive_test-test_write_disk_secure-absolute_path.tmp");
archive_entry_set_mode(ae, S_IFREG | 0777);
archive_write_disk_set_options(a, ARCHIVE_EXTRACT_SECURE_NOABSOLUTEPATHS);
failure("Extracting an absolute path should fail here.");
assertEqualInt(ARCHIVE_FAILED, archive_write_header(a, ae));
archive_entry_free(ae);
assert(0 == archive_write_finish_entry(a));
assertFileNotExists("/tmp/libarchive_test-test_write_disk_secure-absolute_path.tmp");

assertEqualInt(ARCHIVE_OK, archive_write_free(a));


assert(0 == lstat("dir", &st));
failure("dir: st.st_mode=%o", st.st_mode);
assert((st.st_mode & 0777) == 0755);

assert(0 == lstat("link_to_dir", &st));
failure("link_to_dir: st.st_mode=%o", st.st_mode);
assert(S_ISLNK(st.st_mode));
#if defined(HAVE_SYMLINK) && defined(HAVE_LCHMOD) && defined(S_IRUSR) && defined(S_IWUSR) && defined(S_IXUSR)


if (symlink("dir", "testlink_to_dir") == 0) {
if (lchmod("testlink_to_dir",
S_IRUSR | S_IWUSR | S_IXUSR) != 0) {
switch (errno) {
case ENOTSUP:
case ENOSYS:
#if ENOTSUP != EOPNOTSUPP
case EOPNOTSUPP:
#endif
working_lchmod = 0;
break;
default:
working_lchmod = 1;
}
} else
working_lchmod = 1;
} else
working_lchmod = 0;

if (working_lchmod) {
failure("link_to_dir: st.st_mode=%o", st.st_mode);
assert((st.st_mode & 07777) == 0755);
}
#endif

assert(0 == lstat("dir/filea", &st));
failure("dir/filea: st.st_mode=%o", st.st_mode);
assert((st.st_mode & 07777) == 0755);

failure("dir/fileb: This file should not have been created");
assert(0 != lstat("dir/fileb", &st));

assert(0 == lstat("link_to_dir2", &st));
failure("link_to_dir2 should have been re-created as a true dir");
assert(S_ISDIR(st.st_mode));
failure("link_to_dir2: Implicit dir creation should obey umask, but st.st_mode=%o", st.st_mode);
assert((st.st_mode & 0777) == 0755);

assert(0 == lstat("link_to_dir2/filec", &st));
assert(S_ISREG(st.st_mode));
failure("link_to_dir2/filec: st.st_mode=%o", st.st_mode);
assert((st.st_mode & 07777) == 0755);

failure("dir/filed: This file should not have been created");
assert(0 != lstat("dir/filed", &st));
#endif
}
