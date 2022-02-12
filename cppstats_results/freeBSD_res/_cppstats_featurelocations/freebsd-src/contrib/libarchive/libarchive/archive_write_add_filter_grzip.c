
























#include "archive_platform.h"

__FBSDID("$FreeBSD$");

#if defined(HAVE_ERRNO_H)
#include <errno.h>
#endif
#if defined(HAVE_STDLIB_H)
#include <stdlib.h>
#endif

#include "archive.h"
#include "archive_write_private.h"

struct write_grzip {
struct archive_write_program_data *pdata;
};

static int archive_write_grzip_open(struct archive_write_filter *);
static int archive_write_grzip_options(struct archive_write_filter *,
const char *, const char *);
static int archive_write_grzip_write(struct archive_write_filter *,
const void *, size_t);
static int archive_write_grzip_close(struct archive_write_filter *);
static int archive_write_grzip_free(struct archive_write_filter *);

int
archive_write_add_filter_grzip(struct archive *_a)
{
struct archive_write_filter *f = __archive_write_allocate_filter(_a);
struct write_grzip *data;

archive_check_magic(_a, ARCHIVE_WRITE_MAGIC,
ARCHIVE_STATE_NEW, "archive_write_add_filter_grzip");

data = calloc(1, sizeof(*data));
if (data == NULL) {
archive_set_error(_a, ENOMEM, "Can't allocate memory");
return (ARCHIVE_FATAL);
}
data->pdata = __archive_write_program_allocate("grzip");
if (data->pdata == NULL) {
free(data);
archive_set_error(_a, ENOMEM, "Can't allocate memory");
return (ARCHIVE_FATAL);
}

f->name = "grzip";
f->code = ARCHIVE_FILTER_GRZIP;
f->data = data;
f->open = archive_write_grzip_open;
f->options = archive_write_grzip_options;
f->write = archive_write_grzip_write;
f->close = archive_write_grzip_close;
f->free = archive_write_grzip_free;



archive_set_error(_a, ARCHIVE_ERRNO_MISC,
"Using external grzip program for grzip compression");
return (ARCHIVE_WARN);
}

static int
archive_write_grzip_options(struct archive_write_filter *f, const char *key,
const char *value)
{
(void)f;
(void)key;
(void)value;



return (ARCHIVE_WARN);
}

static int
archive_write_grzip_open(struct archive_write_filter *f)
{
struct write_grzip *data = (struct write_grzip *)f->data;

return __archive_write_program_open(f, data->pdata, "grzip");
}

static int
archive_write_grzip_write(struct archive_write_filter *f,
const void *buff, size_t length)
{
struct write_grzip *data = (struct write_grzip *)f->data;

return __archive_write_program_write(f, data->pdata, buff, length);
}

static int
archive_write_grzip_close(struct archive_write_filter *f)
{
struct write_grzip *data = (struct write_grzip *)f->data;

return __archive_write_program_close(f, data->pdata);
}

static int
archive_write_grzip_free(struct archive_write_filter *f)
{
struct write_grzip *data = (struct write_grzip *)f->data;

__archive_write_program_free(data->pdata);
free(data);
return (ARCHIVE_OK);
}
