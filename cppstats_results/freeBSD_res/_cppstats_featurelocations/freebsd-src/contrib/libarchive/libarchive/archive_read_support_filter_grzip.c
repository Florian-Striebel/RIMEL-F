
























#include "archive_platform.h"

__FBSDID("$FreeBSD$");


#if defined(HAVE_ERRNO_H)
#include <errno.h>
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
#include "archive_private.h"
#include "archive_read_private.h"

static const unsigned char grzip_magic[] = {
0x47, 0x52, 0x5a, 0x69, 0x70, 0x49, 0x49, 0x00,
0x02, 0x04, 0x3a, 0x29 };

static int grzip_bidder_bid(struct archive_read_filter_bidder *,
struct archive_read_filter *);
static int grzip_bidder_init(struct archive_read_filter *);


static int
grzip_reader_free(struct archive_read_filter_bidder *self)
{
(void)self;
return (ARCHIVE_OK);
}

int
archive_read_support_filter_grzip(struct archive *_a)
{
struct archive_read *a = (struct archive_read *)_a;
struct archive_read_filter_bidder *reader;

archive_check_magic(_a, ARCHIVE_READ_MAGIC,
ARCHIVE_STATE_NEW, "archive_read_support_filter_grzip");

if (__archive_read_get_bidder(a, &reader) != ARCHIVE_OK)
return (ARCHIVE_FATAL);

reader->data = NULL;
reader->bid = grzip_bidder_bid;
reader->init = grzip_bidder_init;
reader->options = NULL;
reader->free = grzip_reader_free;

archive_set_error(_a, ARCHIVE_ERRNO_MISC,
"Using external grzip program for grzip decompression");
return (ARCHIVE_WARN);
}




static int
grzip_bidder_bid(struct archive_read_filter_bidder *self,
struct archive_read_filter *filter)
{
const unsigned char *p;
ssize_t avail;

(void)self;

p = __archive_read_filter_ahead(filter, sizeof(grzip_magic), &avail);
if (p == NULL || avail == 0)
return (0);

if (memcmp(p, grzip_magic, sizeof(grzip_magic)))
return (0);

return (sizeof(grzip_magic) * 8);
}

static int
grzip_bidder_init(struct archive_read_filter *self)
{
int r;

r = __archive_read_program(self, "grzip -d");



self->code = ARCHIVE_FILTER_GRZIP;
self->name = "grzip";
return (r);
}
