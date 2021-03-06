
























#include "archive_platform.h"
__FBSDID("$FreeBSD$");

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "archive.h"

struct write_memory_data {
size_t used;
size_t size;
size_t * client_size;
unsigned char * buff;
};

static int memory_write_free(struct archive *, void *);
static int memory_write_open(struct archive *, void *);
static ssize_t memory_write(struct archive *, void *, const void *buff, size_t);






int
archive_write_open_memory(struct archive *a, void *buff, size_t buffSize, size_t *used)
{
struct write_memory_data *mine;

mine = (struct write_memory_data *)calloc(1, sizeof(*mine));
if (mine == NULL) {
archive_set_error(a, ENOMEM, "No memory");
return (ARCHIVE_FATAL);
}
mine->buff = buff;
mine->size = buffSize;
mine->client_size = used;
return (archive_write_open2(a, mine,
memory_write_open, memory_write, NULL, memory_write_free));
}

static int
memory_write_open(struct archive *a, void *client_data)
{
struct write_memory_data *mine;
mine = client_data;
mine->used = 0;
if (mine->client_size != NULL)
*mine->client_size = mine->used;

if (-1 == archive_write_get_bytes_in_last_block(a))
archive_write_set_bytes_in_last_block(a, 1);
return (ARCHIVE_OK);
}







static ssize_t
memory_write(struct archive *a, void *client_data, const void *buff, size_t length)
{
struct write_memory_data *mine;
mine = client_data;

if (mine->used + length > mine->size) {
archive_set_error(a, ENOMEM, "Buffer exhausted");
return (ARCHIVE_FATAL);
}
memcpy(mine->buff + mine->used, buff, length);
mine->used += length;
if (mine->client_size != NULL)
*mine->client_size = mine->used;
return (length);
}

static int
memory_write_free(struct archive *a, void *client_data)
{
struct write_memory_data *mine;
(void)a;
mine = client_data;
if (mine == NULL)
return (ARCHIVE_OK);
free(mine);
return (ARCHIVE_OK);
}
