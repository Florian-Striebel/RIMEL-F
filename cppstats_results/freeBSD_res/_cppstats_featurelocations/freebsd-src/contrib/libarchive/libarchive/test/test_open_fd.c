























#include "test.h"
__FBSDID("$FreeBSD$");

#if defined(_WIN32) && !defined(__CYGWIN__)
#define open _open
#if !defined(__BORLANDC__)
#if defined(lseek)
#undef lseek
#endif
#define lseek _lseek
#endif
#define close _close
#endif

DEFINE_TEST(test_open_fd)
{
char buff[64];
struct archive_entry *ae;
struct archive *a;
int fd;
const char *skip_open_fd_err_test;

#if defined(__BORLANDC__)
fd = open("test.tar", O_RDWR | O_CREAT | O_BINARY);
#else
fd = open("test.tar", O_RDWR | O_CREAT | O_BINARY, 0777);
#endif
assert(fd >= 0);
if (fd < 0)
return;


assert((a = archive_write_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_write_set_format_ustar(a));
assertEqualIntA(a, ARCHIVE_OK, archive_write_add_filter_none(a));
assertEqualIntA(a, ARCHIVE_OK, archive_write_open_fd(a, fd));




assert((ae = archive_entry_new()) != NULL);
archive_entry_set_mtime(ae, 1, 0);
archive_entry_copy_pathname(ae, "file");
archive_entry_set_mode(ae, S_IFREG | 0755);
archive_entry_set_size(ae, 8);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);
assertEqualIntA(a, 8, archive_write_data(a, "12345678", 9));




assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "file2");
archive_entry_set_mode(ae, S_IFREG | 0755);
archive_entry_set_size(ae, 819200);
assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
archive_entry_free(ae);


assertEqualIntA(a, ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));




assert(lseek(fd, 0, SEEK_SET) == 0);
assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_open_fd(a, fd, 512));

assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(1, archive_entry_mtime(ae));
assertEqualInt(0, archive_entry_mtime_nsec(ae));
assertEqualInt(0, archive_entry_atime(ae));
assertEqualInt(0, archive_entry_ctime(ae));
assertEqualString("file", archive_entry_pathname(ae));
assert((S_IFREG | 0755) == archive_entry_mode(ae));
assertEqualInt(8, archive_entry_size(ae));
assertEqualIntA(a, 8, archive_read_data(a, buff, 10));
assertEqualMem(buff, "12345678", 8);

assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("file2", archive_entry_pathname(ae));
assert((S_IFREG | 0755) == archive_entry_mode(ae));
assertEqualInt(819200, archive_entry_size(ae));
assertEqualIntA(a, ARCHIVE_OK, archive_read_data_skip(a));


assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
close(fd);

skip_open_fd_err_test = getenv("SKIP_OPEN_FD_ERR_TEST");
if(skip_open_fd_err_test == NULL) {



assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));

assertEqualIntA(a, ARCHIVE_FATAL,
archive_read_open_fd(a, 100, 512));
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}
}
