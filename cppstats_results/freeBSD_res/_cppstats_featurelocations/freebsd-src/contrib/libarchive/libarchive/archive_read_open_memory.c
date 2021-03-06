
























#include "archive_platform.h"
__FBSDID("$FreeBSD$");

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "archive.h"









struct read_memory_data {
const unsigned char *start;
const unsigned char *p;
const unsigned char *end;
ssize_t read_size;
};

static int memory_read_close(struct archive *, void *);
static int memory_read_open(struct archive *, void *);
static int64_t memory_read_seek(struct archive *, void *, int64_t offset, int whence);
static int64_t memory_read_skip(struct archive *, void *, int64_t request);
static ssize_t memory_read(struct archive *, void *, const void **buff);

int
archive_read_open_memory(struct archive *a, const void *buff, size_t size)
{
return archive_read_open_memory2(a, buff, size, size);
}






int
archive_read_open_memory2(struct archive *a, const void *buff,
size_t size, size_t read_size)
{
struct read_memory_data *mine;

mine = (struct read_memory_data *)calloc(1, sizeof(*mine));
if (mine == NULL) {
archive_set_error(a, ENOMEM, "No memory");
return (ARCHIVE_FATAL);
}
mine->start = mine->p = (const unsigned char *)buff;
mine->end = mine->start + size;
mine->read_size = read_size;
archive_read_set_open_callback(a, memory_read_open);
archive_read_set_read_callback(a, memory_read);
archive_read_set_seek_callback(a, memory_read_seek);
archive_read_set_skip_callback(a, memory_read_skip);
archive_read_set_close_callback(a, memory_read_close);
archive_read_set_callback_data(a, mine);
return (archive_read_open1(a));
}




static int
memory_read_open(struct archive *a, void *client_data)
{
(void)a;
(void)client_data;
return (ARCHIVE_OK);
}








static ssize_t
memory_read(struct archive *a, void *client_data, const void **buff)
{
struct read_memory_data *mine = (struct read_memory_data *)client_data;
ssize_t size;

(void)a;
*buff = mine->p;
size = mine->end - mine->p;
if (size > mine->read_size)
size = mine->read_size;
mine->p += size;
return (size);
}






static int64_t
memory_read_skip(struct archive *a, void *client_data, int64_t skip)
{
struct read_memory_data *mine = (struct read_memory_data *)client_data;

(void)a;
if ((int64_t)skip > (int64_t)(mine->end - mine->p))
skip = mine->end - mine->p;

skip /= mine->read_size;
skip *= mine->read_size;
mine->p += skip;
return (skip);
}




static int64_t
memory_read_seek(struct archive *a, void *client_data, int64_t offset, int whence)
{
struct read_memory_data *mine = (struct read_memory_data *)client_data;

(void)a;
switch (whence) {
case SEEK_SET:
mine->p = mine->start + offset;
break;
case SEEK_CUR:
mine->p += offset;
break;
case SEEK_END:
mine->p = mine->end + offset;
break;
default:
return ARCHIVE_FATAL;
}
if (mine->p < mine->start) {
mine->p = mine->start;
return ARCHIVE_FAILED;
}
if (mine->p > mine->end) {
mine->p = mine->end;
return ARCHIVE_FAILED;
}
return (mine->p - mine->start);
}




static int
memory_read_close(struct archive *a, void *client_data)
{
struct read_memory_data *mine = (struct read_memory_data *)client_data;
(void)a;
free(mine);
return (ARCHIVE_OK);
}
