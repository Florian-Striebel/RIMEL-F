
























#include "archive_platform.h"
__FBSDID("$FreeBSD$");

#if defined(HAVE_SYS_STAT_H)
#include <sys/stat.h>
#endif
#if defined(HAVE_ERRNO_H)
#include <errno.h>
#endif
#if defined(HAVE_FCNTL_H)
#include <fcntl.h>
#endif
#if defined(HAVE_IO_H)
#include <io.h>
#endif
#if defined(HAVE_STDLIB_H)
#include <stdlib.h>
#endif
#if defined(HAVE_STRING_H)
#include <string.h>
#endif
#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif

#include "archive.h"

struct write_fd_data {
int fd;
};

static int file_free(struct archive *, void *);
static int file_open(struct archive *, void *);
static ssize_t file_write(struct archive *, void *, const void *buff, size_t);

int
archive_write_open_fd(struct archive *a, int fd)
{
struct write_fd_data *mine;

mine = (struct write_fd_data *)malloc(sizeof(*mine));
if (mine == NULL) {
archive_set_error(a, ENOMEM, "No memory");
return (ARCHIVE_FATAL);
}
mine->fd = fd;
#if defined(__CYGWIN__) || defined(_WIN32)
setmode(mine->fd, O_BINARY);
#endif
return (archive_write_open2(a, mine,
file_open, file_write, NULL, file_free));
}

static int
file_open(struct archive *a, void *client_data)
{
struct write_fd_data *mine;
struct stat st;

mine = (struct write_fd_data *)client_data;

if (fstat(mine->fd, &st) != 0) {
archive_set_error(a, errno, "Couldn't stat fd %d", mine->fd);
return (ARCHIVE_FATAL);
}




if (S_ISREG(st.st_mode))
archive_write_set_skip_file(a, st.st_dev, st.st_ino);





if (archive_write_get_bytes_in_last_block(a) < 0) {



if (S_ISCHR(st.st_mode) || S_ISBLK(st.st_mode) ||
S_ISFIFO(st.st_mode) || (mine->fd == 1))

archive_write_set_bytes_in_last_block(a, 0);
else
archive_write_set_bytes_in_last_block(a, 1);
}

return (ARCHIVE_OK);
}

static ssize_t
file_write(struct archive *a, void *client_data, const void *buff, size_t length)
{
struct write_fd_data *mine;
ssize_t bytesWritten;

mine = (struct write_fd_data *)client_data;
for (;;) {
bytesWritten = write(mine->fd, buff, length);
if (bytesWritten <= 0) {
if (errno == EINTR)
continue;
archive_set_error(a, errno, "Write error");
return (-1);
}
return (bytesWritten);
}
}

static int
file_free(struct archive *a, void *client_data)
{
struct write_fd_data *mine = (struct write_fd_data *)client_data;

(void)a;
if (mine == NULL)
return (ARCHIVE_OK);
free(mine);
return (ARCHIVE_OK);
}
