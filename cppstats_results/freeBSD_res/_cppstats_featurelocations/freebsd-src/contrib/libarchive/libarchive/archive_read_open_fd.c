
























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

struct read_fd_data {
int fd;
size_t block_size;
char use_lseek;
void *buffer;
};

static int file_close(struct archive *, void *);
static ssize_t file_read(struct archive *, void *, const void **buff);
static int64_t file_seek(struct archive *, void *, int64_t request, int);
static int64_t file_skip(struct archive *, void *, int64_t request);

int
archive_read_open_fd(struct archive *a, int fd, size_t block_size)
{
struct stat st;
struct read_fd_data *mine;
void *b;

archive_clear_error(a);
if (fstat(fd, &st) != 0) {
archive_set_error(a, errno, "Can't stat fd %d", fd);
return (ARCHIVE_FATAL);
}

mine = (struct read_fd_data *)calloc(1, sizeof(*mine));
b = malloc(block_size);
if (mine == NULL || b == NULL) {
archive_set_error(a, ENOMEM, "No memory");
free(mine);
free(b);
return (ARCHIVE_FATAL);
}
mine->block_size = block_size;
mine->buffer = b;
mine->fd = fd;







if (S_ISREG(st.st_mode)) {
archive_read_extract_set_skip_file(a, st.st_dev, st.st_ino);
mine->use_lseek = 1;
}
#if defined(__CYGWIN__) || defined(_WIN32)
setmode(mine->fd, O_BINARY);
#endif

archive_read_set_read_callback(a, file_read);
archive_read_set_skip_callback(a, file_skip);
archive_read_set_seek_callback(a, file_seek);
archive_read_set_close_callback(a, file_close);
archive_read_set_callback_data(a, mine);
return (archive_read_open1(a));
}

static ssize_t
file_read(struct archive *a, void *client_data, const void **buff)
{
struct read_fd_data *mine = (struct read_fd_data *)client_data;
ssize_t bytes_read;

*buff = mine->buffer;
for (;;) {
bytes_read = read(mine->fd, mine->buffer, mine->block_size);
if (bytes_read < 0) {
if (errno == EINTR)
continue;
archive_set_error(a, errno, "Error reading fd %d",
mine->fd);
}
return (bytes_read);
}
}

static int64_t
file_skip(struct archive *a, void *client_data, int64_t request)
{
struct read_fd_data *mine = (struct read_fd_data *)client_data;
int64_t skip = request;
int64_t old_offset, new_offset;
int skip_bits = sizeof(skip) * 8 - 1;

if (!mine->use_lseek)
return (0);


if (sizeof(request) > sizeof(skip)) {
int64_t max_skip =
(((int64_t)1 << (skip_bits - 1)) - 1) * 2 + 1;
if (request > max_skip)
skip = max_skip;
}


request = (request / mine->block_size) * mine->block_size;
if (request == 0)
return (0);

if (((old_offset = lseek(mine->fd, 0, SEEK_CUR)) >= 0) &&
((new_offset = lseek(mine->fd, skip, SEEK_CUR)) >= 0))
return (new_offset - old_offset);


mine->use_lseek = 0;


if (errno == ESPIPE)
return (0);






archive_set_error(a, errno, "Error seeking");
return (-1);
}




static int64_t
file_seek(struct archive *a, void *client_data, int64_t request, int whence)
{
struct read_fd_data *mine = (struct read_fd_data *)client_data;
int64_t r;



r = lseek(mine->fd, request, whence);
if (r >= 0)
return r;

if (errno == ESPIPE) {
archive_set_error(a, errno,
"A file descriptor(%d) is not seekable(PIPE)", mine->fd);
return (ARCHIVE_FAILED);
} else {

archive_set_error(a, errno,
"Error seeking in a file descriptor(%d)", mine->fd);
return (ARCHIVE_FATAL);
}
}

static int
file_close(struct archive *a, void *client_data)
{
struct read_fd_data *mine = (struct read_fd_data *)client_data;

(void)a;
free(mine->buffer);
free(mine);
return (ARCHIVE_OK);
}
