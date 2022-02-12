
























#include "test.h"
__FBSDID("$FreeBSD$");

#include <errno.h>
#include <stdlib.h>
#include <string.h>









struct read_memory_data {
const unsigned char *start;
const unsigned char *p;
const unsigned char *end;
size_t read_size;
size_t copy_buff_size;
size_t copy_buff_offset;
char *copy_buff;
};

static int memory_read_close(struct archive *, void *);
static int memory_read_open(struct archive *, void *);
static int64_t memory_read_seek(struct archive *, void *, int64_t request, int whence);
static int64_t memory_read_skip(struct archive *, void *, int64_t request);
static ssize_t memory_read(struct archive *, void *, const void **buff);
static int read_open_memory_internal(struct archive *a, const void *buff,
size_t size, size_t read_size, int fullapi);


int
read_open_memory(struct archive *a, const void *buff, size_t size, size_t read_size)
{
return read_open_memory_internal(a, buff, size, read_size, 2);
}





int
read_open_memory_minimal(struct archive *a, const void *buff, size_t size, size_t read_size)
{
return read_open_memory_internal(a, buff, size, read_size, 1);
}




int
read_open_memory_seek(struct archive *a, const void *buff, size_t size, size_t read_size)
{
return read_open_memory_internal(a, buff, size, read_size, 3);
}

static int
read_open_memory_internal(struct archive *a, const void *buff,
size_t size, size_t read_size, int level)
{
struct read_memory_data *mine = NULL;

switch (level) {
case 3:
archive_read_set_seek_callback(a, memory_read_seek);
__LA_FALLTHROUGH;
case 2:
archive_read_set_open_callback(a, memory_read_open);
archive_read_set_skip_callback(a, memory_read_skip);
__LA_FALLTHROUGH;
case 1:
mine = malloc(sizeof(*mine));
if (mine == NULL) {
archive_set_error(a, ENOMEM, "No memory");
return (ARCHIVE_FATAL);
}
memset(mine, 0, sizeof(*mine));
mine->start = mine->p = (const unsigned char *)buff;
mine->end = mine->start + size;
mine->read_size = read_size;
mine->copy_buff_offset = 32;
mine->copy_buff_size = read_size + mine->copy_buff_offset * 2;
mine->copy_buff = malloc(mine->copy_buff_size);
memset(mine->copy_buff, 0xA5, mine->copy_buff_size);

archive_read_set_read_callback(a, memory_read);
archive_read_set_close_callback(a, memory_read_close);
archive_read_set_callback_data(a, mine);
}
return archive_read_open1(a);
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
size = mine->end - mine->p;
if (size < 0) {
buff = NULL;
return 0;
}
if ((size_t)size > mine->read_size)
size = mine->read_size;
else
memset(mine->copy_buff, 0xA5, mine->copy_buff_size);
memcpy(mine->copy_buff + mine->copy_buff_offset, mine->p, size);
*buff = mine->copy_buff + mine->copy_buff_offset;

mine->p += size;
return ((ssize_t)size);
}




static int64_t
memory_read_skip(struct archive *a, void *client_data, int64_t skip)
{
struct read_memory_data *mine = (struct read_memory_data *)client_data;

(void)a;

if ((off_t)skip > (off_t)(mine->end - mine->p))
skip = mine->end - mine->p;

if (skip > 71)
skip = 71;
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
case SEEK_END:
mine->p = mine->end + offset;
break;
case SEEK_CUR:
mine->p += offset;
break;
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
if (mine != NULL)
free(mine->copy_buff);
free(mine);
return (ARCHIVE_OK);
}
