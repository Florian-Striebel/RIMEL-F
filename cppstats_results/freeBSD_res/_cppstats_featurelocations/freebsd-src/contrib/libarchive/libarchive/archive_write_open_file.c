
























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

struct write_FILE_data {
FILE *f;
};

static int file_free(struct archive *, void *);
static int file_open(struct archive *, void *);
static ssize_t file_write(struct archive *, void *, const void *buff, size_t);

int
archive_write_open_FILE(struct archive *a, FILE *f)
{
struct write_FILE_data *mine;

mine = (struct write_FILE_data *)malloc(sizeof(*mine));
if (mine == NULL) {
archive_set_error(a, ENOMEM, "No memory");
return (ARCHIVE_FATAL);
}
mine->f = f;
return (archive_write_open2(a, mine, file_open, file_write,
NULL, file_free));
}

static int
file_open(struct archive *a, void *client_data)
{
(void)a;
(void)client_data;

return (ARCHIVE_OK);
}

static ssize_t
file_write(struct archive *a, void *client_data, const void *buff, size_t length)
{
struct write_FILE_data *mine;
size_t bytesWritten;

mine = client_data;
for (;;) {
bytesWritten = fwrite(buff, 1, length, mine->f);
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
struct write_FILE_data *mine = client_data;

(void)a;
if (mine == NULL)
return (ARCHIVE_OK);
free(mine);
return (ARCHIVE_OK);
}
